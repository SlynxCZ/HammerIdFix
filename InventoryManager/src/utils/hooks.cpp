//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "hooks.h"
#include <igameevents.h>
#include <funchook.h>

static std::unordered_map<std::string, std::function<void(const CCommand&)>> g_CommandHooks;
static std::unordered_map<std::string, std::function<void(IGameEvent*)>> g_GameEventHooks;

void RegisterCommandHook(const char* name, std::function<void(const CCommand&)> callback) {
    g_CommandHooks[name] = std::move(callback);
}

void OnCommandReceived(const char* name, const CCommand& args) {
    auto it = g_CommandHooks.find(name);
    if (it != g_CommandHooks.end()) {
        it->second(args);
    }
}

void RegisterGameEventHook(const char* name, std::function<void(IGameEvent* ev)> callback) {
    g_GameEventHooks[name] = std::move(callback);
}

void OnGameEventFired(IGameEvent* ev) {
    auto it = g_GameEventHooks.find(ev->GetName());
    if (it != g_GameEventHooks.end()) {
        it->second(ev);
    }
}

template<typename T>
void* GetVirtualFunctionPtr(T* instance, size_t index) {
    void** vtable = *(void***)instance;
    return vtable[index];
}
template void* GetVirtualFunctionPtr<void>(void*, size_t);

template<typename T>
void RegisterVirtualHook(const char* name, void* target, void* detour, T** trampoline) {
    *trampoline = reinterpret_cast<T*>(target);
    static funchook_t* hook = funchook_create();
    funchook_prepare(hook, reinterpret_cast<void**>(trampoline), detour);
    funchook_install(hook, 0);
}
template void RegisterVirtualHook<void()>(const char*, void*, void*, void (**)(void));
