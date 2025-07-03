//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include <memory>
#include <spdlog/spdlog.h>

namespace InventoryManager {
    class Log {
    public:
        static void Init();

        static void Close();

        static std::shared_ptr<spdlog::logger> &GetLogger() { return m_InventoryManager_logger; }

    private:
        static std::shared_ptr<spdlog::logger> m_InventoryManager_logger;
    };

    // Shortcuts
#define InventoryManager_TRACE(...)    ::InventoryManager::Log::GetLogger()->trace(__VA_ARGS__)
#define InventoryManager_DEBUG(...)    ::InventoryManager::Log::GetLogger()->debug(__VA_ARGS__)
#define InventoryManager_INFO(...)     ::InventoryManager::Log::GetLogger()->info(__VA_ARGS__)
#define InventoryManager_WARN(...)     ::InventoryManager::Log::GetLogger()->warn(__VA_ARGS__)
#define InventoryManager_ERROR(...)    ::InventoryManager::Log::GetLogger()->error(__VA_ARGS__)
#define InventoryManager_CRITICAL(...) ::InventoryManager::Log::GetLogger()->critical(__VA_ARGS__)
}
