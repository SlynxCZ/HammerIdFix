//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include <memory>
#include <spdlog/spdlog.h>

namespace HammerIdFix {
    class Log {
    public:
        static void Init();

        static void Close();

        static std::shared_ptr<spdlog::logger> &GetLogger() { return m_HMR_logger; }

    private:
        static std::shared_ptr<spdlog::logger> m_HMR_logger;
    };

    // Shortcuts
#define HMR_TRACE(...)    ::HammerIdFix::Log::GetLogger()->trace(__VA_ARGS__)
#define HMR_DEBUG(...)    ::HammerIdFix::Log::GetLogger()->debug(__VA_ARGS__)
#define HMR_INFO(...)     ::HammerIdFix::Log::GetLogger()->info(__VA_ARGS__)
#define HMR_WARN(...)     ::HammerIdFix::Log::GetLogger()->warn(__VA_ARGS__)
#define HMR_ERROR(...)    ::HammerIdFix::Log::GetLogger()->error(__VA_ARGS__)
#define HMR_CRITICAL(...) ::HammerIdFix::Log::GetLogger()->critical(__VA_ARGS__)
}
