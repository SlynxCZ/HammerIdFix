//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <functional>
#include "utllinkedlist.h"

namespace InventoryManager {
    class CTimerBase {
    public:
        CTimerBase(float flInitialInterval) :
            m_flInterval(flInitialInterval) {};

        virtual bool Execute() = 0;

        float m_flInterval;
        float m_flLastExecute = -1;
    };

    extern CUtlLinkedList<CTimerBase*> g_timers;

    // Timer functions should return the time until next execution, or a negative value like -1.0f to stop
    // Having an interval of 0 is fine, in this case it will run on every game frame
    class CTimer : public CTimerBase
    {
    public:
        CTimer(float flInitialInterval, std::function<float()> func) :
            CTimerBase(flInitialInterval), m_func(func)
        {
            g_timers.AddToTail(this);
        };

        inline bool Execute() override
        {
            m_flInterval = m_func();

            return m_flInterval >= 0;
        }

        inline void RemoveTimer()
        {
            for (int i = g_timers.Head(); i != g_timers.InvalidIndex(); i = g_timers.Next(i))
            {
                if (g_timers[i] == this)
                {
                    g_timers.Remove(i);
                    delete this;
                    return;
                }
            }
        }

        std::function<float()> m_func;
    };


    void RemoveTimers();
}