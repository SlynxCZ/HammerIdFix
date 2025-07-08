//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "HammerIdFix.h"

#include <thread>
#include <chrono>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <ctime>
#include "utils/log.h"
#include "utils/gameconfig.h"
#include "utils/path.h"
#include "entity2/entitysystem.h"
#include "igameeventsystem.h"
#include "cs2_sdk/interfaces/cs2_interfaces.h"
#include "iserver.h"
#include "tier0/vprof.h"
#include <sourcehook.h>
#include <funchook.h>
#include "utils/extends/CCSPlayerController.h"
#include <cstring>
#include <cstdint>
#include <stdio.h>
#include "metamod_oslink.h"
#include <utlstring.h>
#include <KeyValues.h>
#include <map>
#include <entity2/entitysystem.h>
#include "utils/vfunc.h"
#include "utils/extends/CBaseEntity.h"

#ifdef _WIN32
#include <windows.h>
#else
    #include <unistd.h>
    #include <sys/mman.h>
#endif

PLUGIN_EXPOSE(HammerIdFixPlugin, HammerIdFix::gPlugin);

CGameEntitySystem *GameEntitySystem() { return HammerIdFix::globals::entitySystem; }

class GameSessionConfiguration_t {
};

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&,
                   ISource2WorldSession*, const char*);

namespace HammerIdFix {
    HammerIdFixPlugin gPlugin;
    bool g_pluginRegistered = false;
    static std::unordered_set<void *> g_PatchedVTables;

    bool HammerIdFixPlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
        PLUGIN_SAVEVARS();

        globals::ismm = ismm;
        globals::gameThreadId = std::this_thread::get_id();

        Log::Init();

        GET_V_IFACE_CURRENT(GetEngineFactory, globals::engineServer2, IVEngineServer2,
                            SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
        GET_V_IFACE_CURRENT(GetEngineFactory, globals::engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
        GET_V_IFACE_CURRENT(GetEngineFactory, globals::cvars, ICvar, CVAR_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetServerFactory, globals::server, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
        GET_V_IFACE_ANY(GetServerFactory, globals::serverGameClients, IServerGameClients,
                        INTERFACEVERSION_SERVERGAMECLIENTS);
        GET_V_IFACE_ANY(GetEngineFactory, globals::networkServerService, INetworkServerService,
                        NETWORKSERVERSERVICE_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetEngineFactory, globals::schemaSystem, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetEngineFactory, globals::gameEventSystem, IGameEventSystem,
                        GAMEEVENTSYSTEM_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetEngineFactory, globals::engineServiceManager, IEngineServiceMgr,
                        ENGINESERVICEMGR_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetEngineFactory, globals::networkMessages, INetworkMessages,
                        NETWORKMESSAGES_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetServerFactory, globals::gameEntities, ISource2GameEntities,
                        SOURCE2GAMEENTITIES_INTERFACE_VERSION);

        globals::ismm->AddListener(this, this);

        auto gamedata_path = std::string(Utils::GamedataDirectory() + "/gamedata.json");
        globals::gameConfig = new CGameConfig(gamedata_path);
        char conf_error[255] = "";

        if (!globals::gameConfig->Init(conf_error, sizeof(conf_error))) {
            HMR_ERROR("Could not read '{}'. Error: {}", gamedata_path, conf_error);
            return false;
        }

        globals::Initialize();
        HMR_INFO("- [ Globals loaded. ] -");

        SH_ADD_HOOK(INetworkServerService, StartupServer, globals::networkServerService,
                    SH_MEMBER(this, &HammerIdFixPlugin::StartupServer), true);
        HMR_INFO("- [ Hooks declared . ] -");

        globals::mmPlugin = &gPlugin;

        g_pCVar = globals::cvars;
        ConVar_Register(FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

        g_pluginRegistered = true;
        HMR_INFO("- [ [HammerIdFixPlugin] loaded. ] -");
        return true;
    }

    bool HammerIdFixPlugin::Unload(char *error, size_t maxlen) {
        Log::Close();
        SH_REMOVE_HOOK(INetworkServerService, StartupServer, globals::networkServerService,
                       SH_MEMBER(this, &HammerIdFixPlugin::StartupServer), true);
        globals::entitySystem->RemoveListenerEntity(&globals::entityListener);
        g_pluginRegistered = false;
        HMR_INFO("- [ [HammerIdFixPlugin] unloaded. ] -");
        return true;
    }

    void HammerIdFixPlugin::AllPluginsLoaded() {
        std::thread([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            if (g_pluginRegistered)
                HMR_INFO("- [ [HammerIdFixPlugin] is active and linked. ] -");
            else
                HMR_ERROR("- [ [HammerIdFixPlugin] plugin did not register itself. ] -");
        }).detach();
    }

    void HammerIdFixPlugin::StartupServer(const GameSessionConfiguration_t &config, ISource2WorldSession *,
                                          const char *) {
        static bool bDone = false;
        if (!bDone) {
            globals::entitySystem = *reinterpret_cast<CGameEntitySystem **>(
                reinterpret_cast<uintptr_t>(interfaces::pGameResourceServiceServer) + globals::gameConfig->
                GetOffset("GameEntitySystem"));
            globals::entitySystem->AddListenerEntity(&globals::entityListener);
            HMR_INFO("- [ Entity system added . ] -");
            bDone = true;
        }
    }

    void EntityListener::OnEntitySpawned(CEntityInstance *pEntity) {
        if (!globals::entitySystem) {
            return;
        }

        for (int i = 1; i < 1024; ++i) {
            CEntityInstance *entity = globals::entitySystem->GetEntityInstance(CEntityIndex{i});
            if (!entity) {
                continue;
            }

            void **vtable = *(void ***) entity;
            int offset = globals::gameConfig->GetOffset("GetHammerUniqueId");
            if (offset == 108 || offset == 109) {
                void *funcPtr = vtable[offset];
                if (!funcPtr) {
                    continue;
                }

                if (g_PatchedVTables.contains(funcPtr)) {
                    continue;
                }

                unsigned char patch[] = {0xB0, 0x01};
                HammerIdFixPlugin::PatchBytesAtAddress(funcPtr, patch, sizeof(patch));
                g_PatchedVTables.insert(funcPtr);
            }
        }
    }

    void HammerIdFixPlugin::PatchBytesAtAddress(void *addr, const unsigned char *bytes, size_t len) {
#ifdef _WIN32
        DWORD oldProtect;
        if (VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
            memcpy(addr, bytes, len);
            VirtualProtect(addr, len, oldProtect, &oldProtect);
        }
#else
        uintptr_t pageStart = (uintptr_t)addr & ~(getpagesize() - 1);
        mprotect((void *)pageStart, len, PROT_READ | PROT_WRITE | PROT_EXEC);
        memcpy(addr, bytes, len);
#endif
    }

    moodycamel::ConcurrentQueue<std::function<void()> > m_nextTasks;

    const char *HammerIdFixPlugin::GetAuthor() { return "Slynx"; }
    const char *HammerIdFixPlugin::GetName() { return "HammerIdFix"; }
    const char *HammerIdFixPlugin::GetDescription() { return "HammerIdFix plugin for FUNPLAY.pro servers"; }
    const char *HammerIdFixPlugin::GetURL() { return "https://funplay.pro/"; }
    const char *HammerIdFixPlugin::GetLicense() { return "FUNPLAY"; }
    const char *HammerIdFixPlugin::GetVersion() { return "1.0.0"; }
    const char *HammerIdFixPlugin::GetDate() { return __DATE__; }
    const char *HammerIdFixPlugin::GetLogTag() { return "HammerIdFix"; }
}
