//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "gameconfig.h"
#include "memory_module.h"

void* FindSignature(const char* moduleName, const char* bytesStr)
{
    auto module = Core::modules::GetModuleByName(moduleName);
    if (module == nullptr)
    {
        return nullptr;
    }

    return module->FindSignature(bytesStr);
}