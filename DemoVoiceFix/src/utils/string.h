//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <string>
#include <sstream>
namespace DemoVoiceFix {
    std::wstring widen(const std::string& str)
    {
        std::wostringstream wstm;
        const auto& ctfacet = std::use_facet<std::ctype<wchar_t>>(wstm.getloc());
        for (size_t i = 0; i < str.size(); ++i)
        {
            wstm << ctfacet.widen(str[i]);
        }
        return wstm.str();
    }

    std::string narrow(const std::wstring& str)
    {
        std::ostringstream stm;

        // Incorrect code from the link
        // const ctype<char>& ctfacet = use_facet<ctype<char>>(stm.getloc());

        // Correct code.
        const auto& ctfacet = std::use_facet<std::ctype<wchar_t>>(stm.getloc());

        for (size_t i = 0; i < str.size(); ++i)
        {
            stm << ctfacet.narrow(str[i], 0);
        }
        return stm.str();
    }
} // namespace DemoVoiceFix