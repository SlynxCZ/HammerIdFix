//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "InventoryManager.h"

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

#include "InventoryManagerCore.h"
#include "WSItemManager.h"

#include "WSItemManager.h"
#include "utils/extends/CCSPlayerController.h"

PLUGIN_EXPOSE(InventoryManagerPlugin, InventoryManager::gPlugin);

CGameEntitySystem *GameEntitySystem() { return nullptr; }

class GameSessionConfiguration_t {
};

namespace InventoryManager {
    InventoryManagerPlugin gPlugin;
    WSItemManager gWSItemManager;
    InventoryManagerCore gInventoryManager;

    struct ServerSettingsStruct {
        std::string ServerPrefix = "FP";
        bool MapVoteSystem = true;
        bool RtvSystem = true;
        bool TerroristAgentOverride = true;
        int RtvMinTimePassed = 300;
        int RecentMapsMinCount = 5;
        bool InactivitySolving = false;
        bool ShowRating = false;
        int TimeServerStarted = 0;
    };

    struct PluginDataStruct {
        std::optional<std::string> LastDailyStreakResetDate;
        std::optional<time_t> LastHourlyReward;
        time_t CurrentMapStartTime = time(nullptr);
        ServerSettingsStruct ServerSettings;
        int AdCooldown = 0;
        std::string AdLast;
        bool MvpPlayed = false;
        std::vector<std::string> RecentMaps;
        int KnifeTolerance = 3;
    };

    struct InventoryManagerPlayersDataStruct {
        int PlayedTime = -1;
        uint16_t MusicKit = 0;
        uint16_t Medal = 0;
        int KnifeCount = 0;
        bool HasBeenSpawned = false;
    };

    std::unordered_map<uint64_t, InventoryManagerPlayersDataStruct> InventoryManagerPlayersData;
    PluginDataStruct PluginData;
    bool g_pluginRegistered;

    SH_DECL_HOOK3_void(IServerGameDLL, GameFrame, SH_NOATTRIB, 0, bool, bool, bool);
    SH_DECL_HOOK4_void(IServerGameClients, ClientActive, SH_NOATTRIB, false, CPlayerSlot, bool, const char *, uint64);
    SH_DECL_HOOK5_void(IServerGameClients, ClientDisconnect, SH_NOATTRIB, false, CPlayerSlot,
                       ENetworkDisconnectionReason, const char *, uint64, const char *);

    bool InventoryManagerPlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
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

        auto gamedata_path = std::string(Utils::GamedataDirectory() + "/gamedata.json");
        globals::gameConfig = new CGameConfig(gamedata_path);
        char conf_error[255] = "";

        if (!globals::gameConfig->Init(conf_error, sizeof(conf_error))) {
            InventoryManager_ERROR("Could not read \'{}\'. Error: {}", gamedata_path, conf_error);
            g_pluginRegistered = false;
            return false;
        }

        SH_ADD_HOOK(IServerGameDLL, GameFrame, globals::server, SH_MEMBER(this, &InventoryManagerPlugin::GameFrame),
                    true);
        SH_ADD_HOOK(IServerGameClients, ClientActive, globals::serverGameClients,
                    SH_MEMBER(this, &InventoryManagerPlugin::OnClientActive), false);
        SH_ADD_HOOK(IServerGameClients, ClientDisconnect, globals::serverGameClients,
                    SH_MEMBER(this, &InventoryManagerPlugin::OnClientDisconnect), false);
        InventoryManager_INFO("- [ Hook added. ] -");

        globals::Initialize();
        InventoryManager_INFO("- [ Globals loaded. ] -");

        gWSItemManager.Init();
        InventoryManager_INFO("- [ WS loaded. ] -");

        globals::mmPlugin = &gPlugin;

        g_pCVar = globals::cvars;
        ConVar_Register(FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

        g_pluginRegistered = true;

        InventoryManager_INFO("- [ [InventoryManagerPlugin] loaded. ] -");

        return true;
    }

    bool InventoryManagerPlugin::Unload(char *error, size_t maxlen) {
        Log::Close();

        SH_REMOVE_HOOK(IServerGameDLL, GameFrame, globals::server, SH_MEMBER(this, &InventoryManagerPlugin::GameFrame),
                       true);
        SH_REMOVE_HOOK(IServerGameClients, ClientActive, globals::serverGameClients,
                       SH_MEMBER(this, &InventoryManagerPlugin::OnClientActive), false);
        SH_REMOVE_HOOK(IServerGameClients, ClientDisconnect, globals::serverGameClients,
                       SH_MEMBER(this, &InventoryManagerPlugin::OnClientDisconnect), false);

        g_pluginRegistered = false;

        InventoryManager_INFO("- [ [InventoryManagerPlugin] unloaded. ] -");

        return true;
    }

    void InventoryManagerPlugin::AllPluginsLoaded() {
        std::thread([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            PluginData.CurrentMapStartTime = time(nullptr);

            if (g_pluginRegistered)
                InventoryManager_INFO("- [ [InventoryManagerPlugin] is active and linked. ] -");
            else
                InventoryManager_ERROR("- [ [InventoryManagerPlugin] plugin did not register itself. ] -");
        }).detach();
    }

    void InventoryManagerPlugin::GameFrame(bool simulating, bool bFirstTick, bool bLastTick) {
        gWSItemManager.OnFrame();
    }

    void InventoryManagerPlugin::OnClientActive(CPlayerSlot slot, bool bLoadGame, const char *pszName, uint64 xuid) {
        auto controller = CCSPlayerController::FromSlot(slot.Get());
        if (!controller || !controller->IsConnected())
            return;

        InventoryManagerCore::CreateInventory(xuid);
        gInventoryManager.ApplyInventoryToPlayer(controller);

        InventoryManager_INFO("Applied inventory to player {}", xuid);
    }

    void InventoryManagerPlugin::OnClientDisconnect(CPlayerSlot slot, ENetworkDisconnectionReason reason, const char *pszReason, uint64 xuid, const char *pszNetworkID)
    {
        gInventoryManager.RemoveInventory(xuid);
        InventoryManager_INFO("Removed inventory for player {}", xuid);
    }

    const char *InventoryManagerPlugin::GetAuthor() { return "Slynx"; }
    const char *InventoryManagerPlugin::GetName() { return "InventoryManager"; }
    const char *InventoryManagerPlugin::GetDescription() { return "InventoryManager plugin for FUNPLAY.pro servers"; }
    const char *InventoryManagerPlugin::GetURL() { return "https://funplay.pro/"; }
    const char *InventoryManagerPlugin::GetLicense() { return "FUNPLAY"; }
    const char *InventoryManagerPlugin::GetVersion() { return "1.0.0"; }
    const char *InventoryManagerPlugin::GetDate() { return __DATE__; }
    const char *InventoryManagerPlugin::GetLogTag() { return "InventoryManager"; }
}
