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

namespace HammerIdFix {
    class HammerIdFixPlugin : public ISmmPlugin, public IMetamodListener {
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

        static void PatchBytesAtAddress(void* addr, const unsigned char* bytes, size_t len);
    private:
        void StartupServer(const GameSessionConfiguration_t &config, ISource2WorldSession *,
                           const char *);
    };

    class EntityListener : public IEntityListener
    {
        void OnEntitySpawned(CEntityInstance* pEntity) override;
    };

    extern moodycamel::ConcurrentQueue<std::function<void()> > m_nextTasks;

    extern HammerIdFixPlugin gPlugin;

    static void FixHltvVoiceChat();
} // namespace HammerIdFix

#define PLUGIN_GLOBALVARS() \
HammerIdFix::HammerIdFixPlugin gPlugin

#endif // _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
