//
// Created by Michal Přikryl on 03.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <vector>
#include <optional>
#include "CEconItem.h"

namespace InventoryManager {
    struct EquippedItem {
        uint64_t item_id;
        int team;
    };

    struct DefaultEquipped {
        std::string item;
        int team;
    };

    class PlayerInventory {
    public:
        void AddOrUpdateItem(const CEconItem& item);
        void RemoveItem(uint64_t item_id);
        std::optional<CEconItem> GetItem(uint64_t item_id);
        const std::vector<CEconItem>& GetAllItems() const;
        void SetEquippedItem(uint64_t item_id, int team);
        std::optional<uint64_t> GetEquippedItem(int team) const;
        void SetDefaultEquipped(const std::string& item, int team);
        void SetInventoryVersion(int version);
        int GetInventoryVersion() const;
        const std::vector<EquippedItem>& GetEquippedItems() const;

    private:
        std::vector<CEconItem> items;
        std::vector<EquippedItem> equippedItems;
        std::vector<DefaultEquipped> defaultEquipped;
        int inventoryVersion = 0;
    };
}
