//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "extension.h"
#include "log.h"

#include <funchook.h>

#include <cstdio>
#include "entity2/entitysystem.h"
#include "igameeventsystem.h"
#include "iserver.h"
#include "tier0/vprof.h"
#include <entitysystem.h>
#include <entity2/entitysystem.h>

bool g_pluginRegistered = false;

CGameEntitySystem *GameEntitySystem() { return nullptr; }

PLUGIN_EXPOSE(CorePlugin, Core::gPlugin);

namespace Core {

    CorePlugin gPlugin;

    bool CorePlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) {
        PLUGIN_SAVEVARS();

        Log::Init();
        g_SMAPI->AddListener(this, this);
        g_pluginRegistered = true;

        ACC_CORE_INFO("- [ [CorePlugin] loaded. ] -");

        return true;
    }

    bool CorePlugin::Unload(char *error, size_t maxlen) {
        Log::Close();
        g_pluginRegistered = false;

        ACC_CORE_INFO("- [ [CorePlugin] unloaded. ] -");

        return true;
    }

    void CorePlugin::AllPluginsLoaded() {
        std::thread([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            if (g_pluginRegistered)
                ACC_CORE_INFO("- [ [Core] is active and linked. ] -");
            else
                ACC_CORE_ERROR("- [ [Core] plugin did not register itself. ] -");
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
