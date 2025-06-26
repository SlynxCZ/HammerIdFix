// //
// // Created by Michal Přikryl on 20.06.2025.
// // Copyright (c) 2025 slynxcz. All rights reserved.
// //
// #pragma once
// #include <cstdint>
// #include <string>
//
// namespace Core{
//     enum class CsTeam : uint8_t;
//     enum class VoiceFlags : uint8_t;
//     enum class ListenOverride : uint8_t;
//     enum class PlayerButtons : uint32_t;
//     enum class NetworkDisconnectionReason : int;
//
//     class CCSPlayerController {
//     public:
//         void* instance;
//
//         explicit CCSPlayerController(void* inst) : instance(inst) {}
//
//         int GetUserId();
//         CsTeam GetTeam();
//         void* GiveNamedItem(const char* itemName);
//         void PrintToConsole(const char* message);
//         void PrintToChat(const char* message);
//         void PrintToCenter(const char* message);
//         void PrintToCenterAlert(const char* message);
//         void PrintToCenterHtml(const char* message);
//         void DropActiveWeapon();
//         void RemoveWeapons();
//         void CommitSuicide(bool explode, bool force);
//         void Respawn();
//         bool IsBot();
//         void SwitchTeam(CsTeam team);
//         void ChangeTeam(CsTeam team);
//         std::string GetConVarValue(const std::string& conVar);
//         void SetFakeClientConVar(const std::string& name, const std::string& value);
//         PlayerButtons Buttons();
//         void ExecuteClientCommand(const std::string& command);
//         void ExecuteClientCommandFromServer(const std::string& command);
//         void Kick(NetworkDisconnectionReason reason);
//         void SetListenOverride(CCSPlayerController& sender, ListenOverride override);
//         ListenOverride GetListenOverride(CCSPlayerController& sender);
//         VoiceFlags GetVoiceFlags();
//         void SetVoiceFlags(VoiceFlags flags);
//         void ReplicateConVar(const std::string& conVar, const std::string& value);
//     };
// }