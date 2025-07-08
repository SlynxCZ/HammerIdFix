#include "cs2_interfaces.h"
#include "utils/memory_module.h"
#include "utils/globals.h"
#include "interfaces/interfaces.h"

#include "tier0/memdbgon.h"
#include "utils/log.h"

namespace HammerIdFix {
    void interfaces::Initialize()
    {
        const auto offset = globals::gameConfig->GetOffset("GameEntitySystem");
        pGameResourceServiceServer = (CGameResourceService*)modules::engine->FindInterface(GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
        globals::cvars = (ICvar*)modules::tier0->FindInterface(CVAR_INTERFACE_VERSION);
        globals::gameEntities = (ISource2GameEntities*)modules::server->FindInterface(SOURCE2GAMEENTITIES_INTERFACE_VERSION);
    }
}
