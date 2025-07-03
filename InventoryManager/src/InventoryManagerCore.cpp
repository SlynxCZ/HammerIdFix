//
// Created by Michal Přikryl on 03.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "InventoryManagerCore.h"

#include "utils/log.h"
#include "schemasystem/schemasystem.h"

namespace InventoryManager {
    void InventoryManagerCore::CreateInventory(uint64_t steamid) {
        if (!GetInventory(steamid)) {
            inventories[steamid] = std::make_unique<PlayerInventory>();
        }
    }

    PlayerInventory *InventoryManagerCore::GetInventory(uint64_t steamid) {
        auto it = inventories.find(steamid);
        return (it != inventories.end()) ? it->second.get() : nullptr;
    }

    void InventoryManagerCore::RemoveInventory(uint64_t steamid) {
        inventories.erase(steamid);
    }

    void InventoryManagerCore::ClearAll() {
        inventories.clear();
    }

    void InventoryManagerCore::ApplyInventoryToPlayer(CCSPlayerController *player) {
        if (!player || !player->IsConnected() || !player->IsAlive())
            return;

        auto steamid = player->m_steamID;
        auto *inventory = GetInventory(steamid);
        if (!inventory)
            return;

        const auto &equippedItems = inventory->GetEquippedItems(); // Přesně jak máš napojené
        const auto weapons = player->GetWeapons();

        for (const auto &equipped: equippedItems) {
            auto itemOpt = inventory->GetItem(equipped.item_id);
            if (!itemOpt.has_value())
                continue;

            const auto &item = itemOpt.value();

            for (auto *weapon: weapons) {
                if (!weapon)
                    continue;

                const auto defIndex = weapon->m_AttributeManager().m_Item().m_iItemDefinitionIndex();
                if (defIndex != item.def_index)
                    continue;

                weapon->m_nFallbackPaintKit(item.paint_kit.index);
                weapon->m_flFallbackWear(item.paint_kit.wear);
                weapon->m_nFallbackSeed(item.paint_kit.seed);
                weapon->m_nFallbackStatTrak(item.kill_eater.value);

                if (!item.custom_name.empty()) {
                    char** pCustomNamePtr = reinterpret_cast<char**>(
                        reinterpret_cast<uintptr_t>(&weapon->m_AttributeManager().m_Item()) +
                        schema::GetOffset("CEconItemView", hash_32_fnv1a_const("CEconItemView"), "m_szCustomName", hash_32_fnv1a_const("m_szCustomName")).offset
                    );
                    *pCustomNamePtr = const_cast<char*>(item.custom_name.c_str());
                }

                // Samolepky
                for (const auto &sticker: item.stickers) {
                    if (sticker.slot < 0 || sticker.slot >= 5)
                        continue;

                    // Sticker slot = 0x2 + slot * 4
                    const int index = 0x2 + sticker.slot * 4;

                    // Sticker ID
                    CEconItemAttribute attrId;
                    attrId.m_iAttributeDefinitionIndex = index;
                    attrId.m_flValue = sticker.id;
                    weapon->m_AttributeManager().m_Item().m_AttributeList().m_Attributes()->AddToTail(attrId);

                    // Sticker Wear
                    CEconItemAttribute attrWear;
                    attrWear.m_iAttributeDefinitionIndex = index + 1;
                    attrWear.m_flValue = *reinterpret_cast<const int *>(&sticker.wear);
                    weapon->m_AttributeManager().m_Item().m_AttributeList().m_Attributes()->AddToTail(attrWear);
                }

                InventoryManager_INFO("Applied item {} to weapon {}", item.item_id, defIndex);
                break; // Nehledáme dál, protože jsme zbraň našli
            }
        }
    }
}
