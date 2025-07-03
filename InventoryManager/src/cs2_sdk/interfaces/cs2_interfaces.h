//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include "cgameresourceserviceserver.h"

namespace InventoryManager {
namespace interfaces {
void Initialize();

inline CGameResourceService* pGameResourceServiceServer = nullptr;
} // namespace interfaces
} // namespace InventoryManager
