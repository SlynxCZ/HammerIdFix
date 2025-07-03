//
// Created by Michal Přikryl on 03.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <string>
#include <vector>

namespace InventoryManager {
    struct CEconItem {
        uint64_t item_id = 0;
        uint16_t def_index = 0;
        uint8_t quality = 0;
        std::string custom_name;

        struct PaintKit {
            uint16_t index = 0;
            float wear = 0.0001f;
            uint32_t seed = 0;
        } paint_kit;

        struct KillEater {
            int value = 0;
            int type = 0;
        } kill_eater;

        struct Sticker {
            int slot = 0;
            int id = 0;
            float wear = 0.0f;
        };
        std::vector<Sticker> stickers;
    };
}
