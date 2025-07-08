//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "ctimer.h"

namespace HammerIdFix {
    CUtlLinkedList<CTimerBase*> g_timers;

    void RemoveTimers()
    {
        g_timers.PurgeAndDeleteElements();
    }
}