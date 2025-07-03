//
// Created by Michal Přikryl on 03.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "PlayerInventory.h"

namespace InventoryManager {
    void PlayerInventory::AddOrUpdateItem(const CEconItem& item) {
        for (auto& it : items) {
            if (it.item_id == item.item_id) {
                it = item;
                return;
            }
        }
        items.push_back(item);
    }

    void PlayerInventory::RemoveItem(uint64_t item_id) {
        items.erase(std::remove_if(items.begin(), items.end(),
            [item_id](const CEconItem& i) { return i.item_id == item_id; }), items.end());
    }

    std::optional<CEconItem> PlayerInventory::GetItem(uint64_t item_id) {
        for (const auto& it : items)
            if (it.item_id == item_id)
                return it;
        return std::nullopt;
    }

    const std::vector<CEconItem>& PlayerInventory::GetAllItems() const {
        return items;
    }

    void PlayerInventory::SetEquippedItem(uint64_t item_id, int team) {
        for (auto& eq : equippedItems) {
            if (eq.team == team) {
                eq.item_id = item_id;
                return;
            }
        }
        equippedItems.push_back({ item_id, team });
    }

    std::optional<uint64_t> PlayerInventory::GetEquippedItem(int team) const {
        for (const auto& eq : equippedItems) {
            if (eq.team == team)
                return eq.item_id;
        }
        return std::nullopt;
    }

    void PlayerInventory::SetDefaultEquipped(const std::string& item, int team) {
        for (auto& eq : defaultEquipped) {
            if (eq.team == team) {
                eq.item = item;
                return;
            }
        }
        defaultEquipped.push_back({item, team});
    }

    void PlayerInventory::SetInventoryVersion(int version) {
        inventoryVersion = version;
    }

    int PlayerInventory::GetInventoryVersion() const {
        return inventoryVersion;
    }

    const std::vector<EquippedItem>& PlayerInventory::GetEquippedItems() const {
        return equippedItems;
    }
}
