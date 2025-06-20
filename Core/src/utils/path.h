//
// Created by Michal Přikryl on 19.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <public/eiface.h>
#include <string>

namespace Core {
    class Utils {
        static std::string gameDirectory;

        static std::string GameDirectory()
        {
            if (gameDirectory.empty())
            {
                CBufferStringGrowable<255> gamePath;
                g_pEngineServer->GetGameDir(gamePath);
                gameDirectory = std::string(gamePath.Get());
            }

            return gameDirectory;
        }

        static std::string GetRootDirectory() { return GameDirectory() + "/addons/Core"; }
        static std::string GamedataDirectory() { return GameDirectory() + "/addons/Core/gamedata"; }
    };
} // namespace core