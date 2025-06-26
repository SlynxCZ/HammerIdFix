//
// Created by Michal Přikryl on 12.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#define VPROF_LEVEL 1

#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

#include <ISmmPlugin.h>
#include <functional>
#include <iserver.h>
#include <igameevents.h>
#include <iplayerinfo.h>
#include <sh_vector.h>
#include <vector>
#include "entitysystem.h"
#include "concurrentqueue.h"

namespace Core {
    class CorePlugin : public ISmmPlugin, public IMetamodListener {
    public:
        bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late);

        bool Unload(char *error, size_t maxlen);

        void AllPluginsLoaded();

        const char *GetAuthor();

        const char *GetName();

        const char *GetDescription();

        const char *GetURL();

        const char *GetLicense();

        const char *GetVersion();

        const char *GetDate();

        const char *GetLogTag();

    private:
        moodycamel::ConcurrentQueue<std::function<void()> > m_nextTasks;
    };

    extern CorePlugin gPlugin;

    static void Hook_ProcessConnectionlessPacket(void* thisptr, void* netpacket);
    static void (*orig_ProcessConnectionlessPacket)(void* thisptr, void* netpacket);

#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
}

PLUGIN_GLOBALVARS();
