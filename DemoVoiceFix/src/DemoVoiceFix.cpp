//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "DemoVoiceFix.h"

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

#include "utils/vfunc.h"

PLUGIN_EXPOSE(DemoVoiceFixPlugin, DemoVoiceFix::gPlugin);

CGameEntitySystem *GameEntitySystem() { return DemoVoiceFix::globals::entitySystem; }

class GameSessionConfiguration_t {
};

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t&,
                   ISource2WorldSession*, const char*);
SH_DECL_HOOK3_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);

namespace DemoVoiceFix {
    DemoVoiceFixPlugin gPlugin;
    bool g_pluginRegistered = false;

    bool DemoVoiceFixPlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
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
                    SH_MEMBER(this, &DemoVoiceFixPlugin::StartupServer), true);
        SH_ADD_HOOK(IServerGameDLL, GameFrame, globals::server, SH_MEMBER(this, &DemoVoiceFixPlugin::GameFrame), true);
        HMR_INFO("- [ Hooks declared . ] -");

        globals::mmPlugin = &gPlugin;

        g_pluginRegistered = true;
        HMR_INFO("- [ [DemoVoiceFixPlugin] loaded. ] -");
        return true;
    }

    bool DemoVoiceFixPlugin::Unload(char *error, size_t maxlen) {
        Log::Close();
        SH_REMOVE_HOOK(IServerGameDLL, GameFrame, globals::server, SH_MEMBER(this, &DemoVoiceFixPlugin::GameFrame),
                       true);
        SH_REMOVE_HOOK(INetworkServerService, StartupServer, globals::networkServerService,
                       SH_MEMBER(this, &DemoVoiceFixPlugin::StartupServer), true);
        globals::gameEventManager->RemoveListener(&globals::OnRoundStart);

        g_pluginRegistered = false;
        HMR_INFO("- [ [DemoVoiceFixPlugin] unloaded. ] -");
        return true;
    }

    void DemoVoiceFixPlugin::AllPluginsLoaded() {
        std::thread([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            if (g_pluginRegistered)
                HMR_INFO("- [ [DemoVoiceFixPlugin] is active and linked. ] -");
            else
                HMR_ERROR("- [ [DemoVoiceFixPlugin] plugin did not register itself. ] -");
        }).detach();
    }

    void DemoVoiceFixPlugin::StartupServer(const GameSessionConfiguration_t &config, ISource2WorldSession *,
                                           const char *) {
        static bool bDone = false;
        if (!bDone) {
            globals::gameEventManager->AddListener(&globals::OnRoundStart, "round_start", true);
            HMR_INFO("- [ Hooks added . ] -");
            bDone = true;
        }
    }

    void DemoVoiceFixPlugin::GameFrame(bool simulating, bool bFirstTick, bool bLastTick) {
        std::function<void()> task;
        while (m_nextTasks.try_dequeue(task)) {
            task();
        }
    }

    void FixHltvVoiceChat() {
        HMR_INFO("ROUNDSTART");

        if (!globals::entitySystem) {
            HMR_ERROR("entitySystem == nullptr!");
            return;
        }

        CGlobalVars* vars = globals::getGlobalVars();
        if (!vars) {
            HMR_ERROR("globalVars == nullptr!");
            return;
        }

        HMR_INFO("About to test FromSlot...");

        for (int i = 0; i < vars->maxClients; ++i) {
            HMR_INFO("Slot {}", i);

            // Test only the call, wrapped safely
            CCSPlayerController* player = nullptr;
            try {
                player = CCSPlayerController::FromSlot(i);
            } catch (...) {
                HMR_ERROR("Exception while calling FromSlot({})", i);
                continue;
            }

            if (player)
                HMR_INFO("Slot {} OK: {}", i, player->GetPlayerName());
        }
    }

    void EventRoundStart::FireGameEvent(IGameEvent *event) {
        m_nextTasks.enqueue([] {
            FixHltvVoiceChat();
        });
    }

    moodycamel::ConcurrentQueue<std::function<void()> > m_nextTasks;

    const char *DemoVoiceFixPlugin::GetAuthor() { return "Slynx"; }
    const char *DemoVoiceFixPlugin::GetName() { return "DemoVoiceFix"; }
    const char *DemoVoiceFixPlugin::GetDescription() { return "DemoVoiceFix plugin for FUNPLAY.pro servers"; }
    const char *DemoVoiceFixPlugin::GetURL() { return "https://funplay.pro/"; }
    const char *DemoVoiceFixPlugin::GetLicense() { return "FUNPLAY"; }
    const char *DemoVoiceFixPlugin::GetVersion() { return "1.0.0"; }
    const char *DemoVoiceFixPlugin::GetDate() { return __DATE__; }
    const char *DemoVoiceFixPlugin::GetLogTag() { return "DemoVoiceFix"; }
}
