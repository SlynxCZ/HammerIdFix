//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <cstdio>
#include <cstdint>
#include <unordered_map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#if __linux__
#include <link.h>
#endif

#include "interface.h"
#include "strtools.h"
#undef snprintf

namespace Core::modules {
    struct Segments {
        Segments() = default;

        Segments(const Segments &) = default;

        Segments(Segments &&) = default;

        Segments &operator=(const Segments &) = default;

        Segments &operator=(Segments &&) = default;

        std::uintptr_t address{};
        std::vector<std::uint8_t> bytes{};
    };

    class CModule {
    public:
#ifdef _WIN32
        CModule(std::string_view path, std::uint64_t base);
#else
        CModule(std::string_view path, struct dl_phdr_info *info);
#endif

        void *FindSignature(const char *signature);

        void *FindInterface(std::string_view name);

        void *FindSymbol(const std::string &name);

        [[nodiscard]] bool IsInitialized() const { return m_bInitialized; }

        std::string m_pszModule{};
        std::string m_pszPath{};
        void *m_base{};
        size_t m_size{};

    private:
        bool m_bInitialized{};
        std::vector<Segments> m_vecSegments{};
        std::uintptr_t m_baseAddress{};
        std::unordered_map<std::string, std::uintptr_t> _symbols{};
        std::unordered_map<std::string, std::uintptr_t> _interfaces{};
        using fnCreateInterface = void* (*)(const char *);
        fnCreateInterface m_fnCreateInterface{};

#ifdef _WIN32
        void DumpSymbols();
#else
        void DumpSymbols(ElfW(Dyn) *dyn);
#endif

        std::optional<std::vector<std::uint8_t> >
        GetOriginalBytes(const std::vector<std::uint8_t> &disk_data, std::uintptr_t rva, std::size_t size);

        void *FindSignature(const std::vector<int16_t> &sigBytes);
    };
} // namespace Core::modules
