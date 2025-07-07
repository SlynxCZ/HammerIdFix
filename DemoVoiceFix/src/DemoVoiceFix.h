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

namespace DemoVoiceFix {
    class DemoVoiceFixPlugin : public ISmmPlugin, public IMetamodListener {
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
        void StartupServer(const GameSessionConfiguration_t &config, ISource2WorldSession *,
                           const char *);

        void GameFrame(bool simulating, bool bFirstTick, bool bLastTick);
    };

    class EventRoundStart : public IGameEventListener2 {
        void FireGameEvent(IGameEvent *event) override;
    };

    extern moodycamel::ConcurrentQueue<std::function<void()> > m_nextTasks;

    extern DemoVoiceFixPlugin gPlugin;

    static void FixHltvVoiceChat();
} // namespace DemoVoiceFix

#define PLUGIN_GLOBALVARS() \
DemoVoiceFix::DemoVoiceFixPlugin gPlugin

#endif // _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
