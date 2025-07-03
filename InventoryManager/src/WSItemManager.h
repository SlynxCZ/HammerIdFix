#pragma once

#include <IXWebSocket.h>
#include <string>
#include <memory>

namespace InventoryManager {

    class WSItemManager {
    public:
        WSItemManager();
        ~WSItemManager();

        void Init();
        void OnFrame();
        void HandleMessage(const std::string& msg);

        static const char* GetClientName() { return "InventoryManager"; }

    private:
        std::unique_ptr<ix::WebSocket> m_socket;
        bool m_connected = false;

        void SetupHandlers();
    };

    extern WSItemManager gWSItemManager;

} // namespace InventoryManager
