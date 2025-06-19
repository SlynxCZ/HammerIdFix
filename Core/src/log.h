//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include <memory>
#include <spdlog/spdlog.h>

namespace Core {
    class Log {
    public:
        static void Init();

        static void Close();

        static std::shared_ptr<spdlog::logger> &GetLogger() { return m_core_logger; }

    private:
        static std::shared_ptr<spdlog::logger> m_core_logger;
    };

    // Shortcuts
#define CORE_TRACE(...)    ::Core::Log::GetLogger()->trace(__VA_ARGS__)
#define CORE_DEBUG(...)    ::Core::Log::GetLogger()->debug(__VA_ARGS__)
#define CORE_INFO(...)     ::Core::Log::GetLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)     ::Core::Log::GetLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)    ::Core::Log::GetLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...) ::Core::Log::GetLogger()->critical(__VA_ARGS__)
}
