//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "core.h"

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

PLUGIN_EXPOSE(CorePlugin, Core::gPlugin);

CGameEntitySystem *GameEntitySystem() { return nullptr; }

class GameSessionConfiguration_t {
};

namespace Core {
    CorePlugin gPlugin;

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

    struct CorePlayersDataStruct {
        int PlayedTime = -1;
        uint16_t MusicKit = 0;
        uint16_t Medal = 0;
        int KnifeCount = 0;
        bool HasBeenSpawned = false;
    };

    std::unordered_map<uint64_t, CorePlayersDataStruct> CorePlayersData;
    PluginDataStruct PluginData;
    bool g_pluginRegistered;

    bool CorePlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
        PLUGIN_SAVEVARS();

        globals::ismm = ismm;
        globals::gameThreadId = std::this_thread::get_id();

        Log::Init();

        GET_V_IFACE_CURRENT(GetServerFactory, globals::server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
        GET_V_IFACE_CURRENT(GetEngineFactory, globals::engine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
        GET_V_IFACE_CURRENT(GetEngineFactory, globals::cvars, ICvar, CVAR_INTERFACE_VERSION);

        auto gamedata_path = std::string(Utils::GamedataDirectory() + "/gamedata.json");
        globals::gameConfig = new CGameConfig(gamedata_path);
        char conf_error[255] = "";

        if (!globals::gameConfig->Init(conf_error, sizeof(conf_error))) {
            CORE_ERROR("Could not read \'{}\'. Error: {}", gamedata_path, conf_error);
            g_pluginRegistered = false;
            return false;
        }

        globals::Initialize();
        CORE_INFO("- [ Globals loaded. ] -");

        globals::mmPlugin = &gPlugin;

        g_pCVar = globals::cvars;
        ConVar_Register(FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

        g_pluginRegistered = true;

        CORE_INFO("- [ [CorePlugin] loaded. ] -");

        return true;
    }

    bool CorePlugin::Unload(char *error, size_t maxlen) {
        Log::Close();
        g_pluginRegistered = false;

        CORE_INFO("- [ [CorePlugin] unloaded. ] -");

        return true;
    }

    void CorePlugin::AllPluginsLoaded() {
        std::thread([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            PluginData.CurrentMapStartTime = time(nullptr);

            if (g_pluginRegistered)
                CORE_INFO("- [ [CorePlugin] is active and linked. ] -");
            else
                CORE_ERROR("- [ [CorePlugin] plugin did not register itself. ] -");
        }).detach();
    }

    const char *CorePlugin::GetAuthor() { return "Slynx"; }
    const char *CorePlugin::GetName() { return "Core"; }
    const char *CorePlugin::GetDescription() { return "Core plugin for FUNPLAY.pro servers"; }
    const char *CorePlugin::GetURL() { return "https://funplay.pro/"; }
    const char *CorePlugin::GetLicense() { return "FUNPLAY"; }
    const char *CorePlugin::GetVersion() { return "1.0.0"; }
    const char *CorePlugin::GetDate() { return __DATE__; }
    const char *CorePlugin::GetLogTag() { return "CORE"; }
}
