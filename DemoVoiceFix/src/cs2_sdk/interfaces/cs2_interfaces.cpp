#include "cs2_interfaces.h"
#include "utils/memory_module.h"
#include "utils/globals.h"
#include "interfaces/interfaces.h"

#include "tier0/memdbgon.h"

namespace DemoVoiceFix {
    void interfaces::Initialize()
    {
        pGameResourceServiceServer = (CGameResourceService*)modules::engine->FindInterface(GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
        globals::cvars = (ICvar*)modules::tier0->FindInterface(CVAR_INTERFACE_VERSION);
        globals::gameEntities = (ISource2GameEntities*)modules::server->FindInterface(SOURCE2GAMEENTITIES_INTERFACE_VERSION);
    }
}
