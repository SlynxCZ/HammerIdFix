//
// Created by Michal Přikryl on 19.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <eiface.h>
#include <string>

namespace InventoryManager::Utils {
    static std::string gameDirectory;

    inline std::string GameDirectory() {
        if (gameDirectory.empty()) {
            CBufferStringGrowable<255> gamePath;
            globals::engine->GetGameDir(gamePath);
            gameDirectory = std::string(gamePath.Get());
        }

        return gameDirectory;
    }

    inline std::string GetRootDirectory() { return GameDirectory() + "/addons/InventoryManager"; }
    inline std::string GamedataDirectory() { return GameDirectory() + "/addons/InventoryManager/gamedata"; }
}// namespace InventoryManager