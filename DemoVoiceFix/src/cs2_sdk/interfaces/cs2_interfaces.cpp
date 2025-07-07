//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "cs2_interfaces.h"
#include "DemoVoiceFix/src/utils/memory_module.h"
#include "DemoVoiceFix/src/utils/globals.h"
#include "interfaces/interfaces.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

namespace DemoVoiceFix {
void interfaces::Initialize()
{
    pGameResourceServiceServer = (CGameResourceService*)modules::engine->FindInterface(GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
    g_pCVar = (ICvar*)modules::tier0->FindInterface(CVAR_INTERFACE_VERSION);
    g_pSource2GameEntities = (ISource2GameEntities*)modules::server->FindInterface(SOURCE2GAMEENTITIES_INTERFACE_VERSION);
}
} // namespace DemoVoiceFix
