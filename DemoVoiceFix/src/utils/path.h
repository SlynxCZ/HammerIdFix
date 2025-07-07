//
// Created by Michal Přikryl on 19.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <eiface.h>
#include <string>

namespace DemoVoiceFix::Utils {
    static std::string gameDirectory;

    inline std::string GameDirectory() {
        if (gameDirectory.empty()) {
            CBufferStringGrowable<255> gamePath;
            globals::engine->GetGameDir(gamePath);
            gameDirectory = std::string(gamePath.Get());
        }

        return gameDirectory;
    }

    inline std::string GetRootDirectory() { return GameDirectory() + "/addons/DemoVoiceFix"; }
    inline std::string GamedataDirectory() { return GameDirectory() + "/addons/DemoVoiceFix/gamedata"; }
}// namespace DemoVoiceFix