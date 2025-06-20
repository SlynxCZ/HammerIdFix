// core.cpp
#include "core.h"
#include "utils/log.h"
#include <thread>
#include <chrono>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <ctime>

#include <funchook.h>
#include <entity2/entitysystem.h>
#include <iserver.h>
#include <igameeventsystem.h>
#include <tier0/vprof.h>

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

    bool CorePlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
        PLUGIN_SAVEVARS();

        GET_V_IFACE_CURRENT(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
        GET_V_IFACE_CURRENT(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
        GET_V_IFACE_CURRENT(GetEngineFactory, g_pEngineServer, IVEngineServer2, INTERFACEVERSION_VENGINESERVER);
        GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);

        Log::Init();

        g_SMAPI->AddListener(this, this);
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