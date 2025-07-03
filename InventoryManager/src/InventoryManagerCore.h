//
// Created by Michal Přikryl on 03.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <unordered_map>
#include <cstdint>
#include <memory>
#include "PlayerInventory.h"
#include "utils/extends/CCSPlayerController.h"

namespace InventoryManager {
    class InventoryManagerCore {
    public:
        static void CreateInventory(uint64_t steamid);
        static PlayerInventory* GetInventory(uint64_t steamid);
        static void RemoveInventory(uint64_t steamid);
        static void ClearAll();
        void ApplyInventoryToPlayer(CCSPlayerController* player);

    private:
        static inline std::unordered_map<uint64_t, std::unique_ptr<PlayerInventory>> inventories;
    };
}
