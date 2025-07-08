﻿//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBasePlayerController.h"
#include "services.h"

namespace DemoVoiceFix {
    class CCSPlayerController : public CBasePlayerController {
    public:
        DECLARE_SCHEMA_CLASS(CCSPlayerController);

        SCHEMA_FIELD(CCSPlayerController_InGameMoneyServices*, m_pInGameMoneyServices);

        SCHEMA_FIELD(CCSPlayerController_ActionTrackingServices*, m_pActionTrackingServices);

        SCHEMA_FIELD(CCSPlayerController_InventoryServices*, m_pInventoryServices);

        SCHEMA_FIELD(uint32_t, m_iPing);

        SCHEMA_FIELD(CUtlSymbolLarge, m_szClan);

        SCHEMA_FIELD_POINTER(char, m_szClanName) // char m_szClanName[32]
        SCHEMA_FIELD(bool, m_bEverFullyConnected);

        SCHEMA_FIELD(bool, m_bPawnIsAlive);

        SCHEMA_FIELD(int32_t, m_nDisconnectionTick);

        SCHEMA_FIELD(CHandle<CCSPlayerPawn>, m_hPlayerPawn);

        SCHEMA_FIELD(int32, m_DesiredObserverMode);

        SCHEMA_FIELD(CHandle<CCSPlayerPawnBase>, m_hObserverPawn);

        SCHEMA_FIELD(CHandle<CCSPlayerController>, m_hOriginalControllerOfCurrentPawn);

        SCHEMA_FIELD(uint32_t, m_iPawnHealth);

        SCHEMA_FIELD(int32_t, m_iPawnArmor);

        SCHEMA_FIELD(int32_t, m_iSDemoVoiceFix);

        SCHEMA_FIELD(int32_t, m_iRoundSDemoVoiceFix);

        SCHEMA_FIELD(int32_t, m_iRoundsWon);

        SCHEMA_FIELD(int32_t, m_iMVPs);

        SCHEMA_FIELD(float, m_flSmoothedPing);

        SCHEMA_FIELD(GameTime_t, m_flForceTeamTime);

        SCHEMA_FIELD(int32_t, m_iCompetitiveRanking);

        SCHEMA_FIELD(int8_t, m_iCompetitiveRankType);

        SCHEMA_FIELD(int32_t, m_iCompetitiveWins);

        std::vector<CCSWeaponBase*> GetWeapons() {
            std::vector<CCSWeaponBase*> weapons;
            auto pawn = GetPlayerPawn();
            if (!pawn || !pawn->m_pWeaponServices || !pawn->m_pWeaponServices->m_hMyWeapons)
                return weapons;

            const auto& myWeapons = *pawn->m_pWeaponServices->m_hMyWeapons;

            for (int i = 0; i < myWeapons.Count(); ++i) {
                const auto& handle = myWeapons[i];
                if (!handle.IsValid()) continue;

                auto ent = reinterpret_cast<CBaseEntity*>(
                    globals::entitySystem->GetEntityInstance(CEntityIndex(handle.GetEntryIndex()))
                );

                if (auto* weapon = dynamic_cast<CCSWeaponBase*>(ent)) {
                    weapons.push_back(weapon);
                }
            }

            return weapons;
        }

        static CCSPlayerController *FromUserId(int userid) {
            for (int i = 0; i < globals::globalVars->maxClients; ++i) {
                CCSPlayerController *controller = FromSlot(i);
                if (!controller)
                    continue;

                return controller;
            }
            return nullptr;
        }

        static CCSPlayerController *FromPawn(CCSPlayerPawn *pawn) {
            return (CCSPlayerController *) pawn->m_hController().Get();
        }

        static CCSPlayerController *FromSlot(int iSlot) {
            return (CCSPlayerController *) globals::entitySystem->GetEntityInstance(CEntityIndex(iSlot + 1));
        }

        CCSPlayerPawn *GetPlayerPawn() {
            return m_hPlayerPawn().Get();
        }

        bool IsBot() {
            return m_fFlags() & FL_CONTROLLER_FAKECLIENT;
        }

        void ChangeTeam(int iTeam) {
            CALL_VIRTUAL(void, 99, this, iTeam);
        }

        void Respawn() {
            CALL_VIRTUAL(void, 259, this);
        }

        CSPlayerState GetPawnState() {
            CCSPlayerPawnBase *pPawn = (CCSPlayerPawnBase *) GetPawn();
            if (!pPawn)
                return STATE_WELCOME;

            return pPawn->m_iPlayerState();
        }

        CSPlayerState GetPlayerPawnState() {
            CCSPlayerPawn *pPawn = GetPlayerPawn();
            if (!pPawn)
                return STATE_WELCOME;

            return pPawn->m_iPlayerState();
        }

        CBaseEntity *GetObserverTarget() {
            auto pPawn = GetPawn();

            if (!pPawn)
                return nullptr;

            return pPawn->m_pObserverServices->m_hObserverTarget().Get();
        }
    };
}
