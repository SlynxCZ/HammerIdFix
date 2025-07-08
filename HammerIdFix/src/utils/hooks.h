//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <functional>
#include <string>
#include <unordered_map>

class CCommand;
class IGameEvent;

void RegisterCommandHook(const char* name, std::function<void(const CCommand&)> callback);
void OnCommandReceived(const char* name, const CCommand& args);

void RegisterGameEventHook(const char* name, std::function<void(IGameEvent* ev)> callback);
void OnGameEventFired(IGameEvent* ev);

template<typename T>
void* GetVirtualFunctionPtr(T* instance, size_t index);

template<typename T>
void RegisterVirtualHook(const char* name, void* target, void* detour, T** trampoline);

template<typename T>
T* GetDummyInstance();

#define HMR_HOOK_COMMAND(commandName, functionName)                      \
    static void functionName(const CCommand& args);                       \
    struct functionName##_HookRegistrar {                                 \
        functionName##_HookRegistrar() {                                  \
            RegisterCommandHook(commandName, functionName);              \
        }                                                                 \
    };                                                                    \
    static functionName##_HookRegistrar functionName##_registrar;         \
    static void functionName(const CCommand& args)

#define HMR_HOOK_GAMEEVENT(eventName, functionName)                      \
    static void functionName(IGameEvent* ev);                             \
    struct functionName##_GameEventRegistrar {                            \
        functionName##_GameEventRegistrar() {                             \
            RegisterGameEventHook(eventName, functionName);              \
        }                                                                 \
    };                                                                    \
    static functionName##_GameEventRegistrar functionName##_registrar;    \
    static void functionName(IGameEvent* ev)

#define HMR_HOOK_VIRTUAL(className, index, returnType, functionName, argsDecl, argsCall) \
    returnType functionName##_impl argsDecl;                                              \
    returnType (*functionName##_trampoline) argsDecl = nullptr;                           \
    returnType functionName argsDecl { return functionName##_impl argsCall; }             \
    struct functionName##_VirtualHookRegistrar {                                           \
        functionName##_VirtualHookRegistrar() {                                            \
            className* obj = GetDummyInstance<className>();                               \
            void* fn = GetVirtualFunctionPtr(obj, index);                                 \
            RegisterVirtualHook<returnType argsDecl>(#functionName, fn, (void*)&functionName, &functionName##_trampoline); \
        }                                                                                  \
    };                                                                                     \
    static functionName##_VirtualHookRegistrar functionName##_registrar;                  \
    returnType functionName##_impl argsDecl
