//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include <memory>
#include <spdlog/spdlog.h>

namespace DemoVoiceFix {
    class Log {
    public:
        static void Init();

        static void Close();

        static std::shared_ptr<spdlog::logger> &GetLogger() { return m_DEMO_logger; }

    private:
        static std::shared_ptr<spdlog::logger> m_DEMO_logger;
    };

    // Shortcuts
#define DEMO_TRACE(...)    ::DemoVoiceFix::Log::GetLogger()->trace(__VA_ARGS__)
#define DEMO_DEBUG(...)    ::DemoVoiceFix::Log::GetLogger()->debug(__VA_ARGS__)
#define DEMO_INFO(...)     ::DemoVoiceFix::Log::GetLogger()->info(__VA_ARGS__)
#define DEMO_WARN(...)     ::DemoVoiceFix::Log::GetLogger()->warn(__VA_ARGS__)
#define DEMO_ERROR(...)    ::DemoVoiceFix::Log::GetLogger()->error(__VA_ARGS__)
#define DEMO_CRITICAL(...) ::DemoVoiceFix::Log::GetLogger()->critical(__VA_ARGS__)
}
