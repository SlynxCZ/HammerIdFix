//
// Created by Michal Přikryl on 03.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "WSItemManager.h"
#include "InventoryManagerCore.h"
#include "json.hpp"
#include "utils/globals.h"
#include "utils/log.h"

namespace InventoryManager {
    WSItemManager::~WSItemManager() = default;

    void WSItemManager::Init() {
        m_socket = std::make_unique<ix::WebSocket>();
        m_socket->setUrl("wss://57.128.211.14:3002/");

        SetupHandlers();

        m_socket->start();
    }

    void WSItemManager::SetupHandlers() {
        m_socket->setOnMessageCallback([this](const ix::WebSocketMessagePtr& msg) {
            if (msg->type == ix::WebSocketMessageType::Message) {
                HandleMessage(msg->str);
            } else if (msg->type == ix::WebSocketMessageType::Open) {
                InventoryManager_INFO("Connected to Inventory WS.");
                m_connected = true;
            } else if (msg->type == ix::WebSocketMessageType::Close) {
                InventoryManager_INFO("Disconnected from Inventory WS.");
                m_connected = false;
            } else if (msg->type == ix::WebSocketMessageType::Error) {
                InventoryManager_INFO("WebSocket error: reason={}", msg->errorInfo.reason);
            }
        });
    }

    void WSItemManager::OnFrame() {
        if (!m_connected || !m_socket)
            return;

        static float lastHeartbeat = 0.0f;
        float now = globals::getGlobalVars()->curtime;

        if (now - lastHeartbeat >= 5.0f) {
            m_socket->send("{\"type\": \"ping\"}");
            lastHeartbeat = now;
        }
    }

    void WSItemManager::HandleMessage(const std::string& msg) {
        try {
            auto json = nlohmann::json::parse(msg);
            uint64_t steamid = json["account_id"];

            InventoryManagerCore::CreateInventory(steamid);
            auto* inv = InventoryManagerCore::GetInventory(steamid);
            if (!inv) return;

            if (json.contains("equipped_item")) {
                for (auto& e : json["equipped_item"]) {
                    inv->SetEquippedItem(e["item_id"], e["team"]);
                }
            }

            if (json.contains("default_equipped")) {
                for (auto& d : json["default_equipped"]) {
                    inv->SetDefaultEquipped(d["item"], d["team"]);
                }
            }

            if (json.contains("version")) {
                inv->SetInventoryVersion(json["version"]);
            }

            for (auto& item : json["items"]) {
                CEconItem econItem{};
                econItem.item_id = item["item_id"];
                econItem.def_index = item["def_index"];
                econItem.quality = item.value("quality", 0);
                econItem.custom_name = item.value("custom_name", "");

                if (item.contains("paint_kit")) {
                    econItem.paint_kit.index = item["paint_kit"]["index"];
                    econItem.paint_kit.wear = item["paint_kit"]["wear"];
                    econItem.paint_kit.seed = item["paint_kit"].value("seed", 0);
                }

                if (item.contains("kill_eater")) {
                    econItem.kill_eater.value = item["kill_eater"]["value"];
                    econItem.kill_eater.type = item["kill_eater"]["type"];
                }

                if (item.contains("stickers")) {
                    for (auto& s : item["stickers"]) {
                        CEconItem::Sticker sticker;
                        sticker.slot = s["slot"];
                        sticker.id = s["id"];
                        sticker.wear = s.value("wear", 0.0f);
                        econItem.stickers.push_back(sticker);
                    }
                }

                inv->AddOrUpdateItem(econItem);
            }
        } catch (const std::exception& e) {
            InventoryManager_ERROR("Failed to parse WS inventory json: {}", e.what());
        }
    }
}
