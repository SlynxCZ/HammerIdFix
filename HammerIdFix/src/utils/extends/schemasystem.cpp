//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "schemasystem.h"

#include "platform.h"
#include "schemasystem/schemasystem.h"
#include "tier1/utlmap.h"
#include "tier0/memdbgon.h"
#include <entity2/entityidentity.h>
#include <entity2/entityinstance.h>
#include <edict.h>
#include "CBaseEntity.h"
#include "utils/log.h"

#ifdef _WIN32
#define MODULE_PREFIX ""
#define MODULE_EXT ".dll"
#else
#define MODULE_PREFIX "lib"
#define MODULE_EXT ".so"
#endif

namespace HammerIdFix {
extern CGlobalVars *gpGlobals;

using SchemaKeyValueMap_t = CUtlMap<uint32_t, SchemaKey>;
using SchemaTableMap_t = CUtlMap<uint32_t, SchemaKeyValueMap_t*>;


static bool IsFieldNetworked(SchemaClassFieldData_t& field)
{
    for (int i = 0; i < field.m_nStaticMetadataCount; i++)
    {
        static auto networkEnabled = hash_32_fnv1a_const("MNetworkEnable");
        if (networkEnabled == hash_32_fnv1a_const(field.m_pStaticMetadata[i].m_pszName))
            return true;
    }

    return false;
}

static bool InitSchemaFieldsForClass(SchemaTableMap_t *tableMap, const char* className, uint32_t classKey)
{
    CSchemaSystemTypeScope* pType = globals::schemaSystem->FindTypeScopeForModule(MODULE_PREFIX "server" MODULE_EXT);

    if (!globals::schemaSystem)
    {
        HMR_ERROR("schemaSystem is nullptr! Called too early?");
        return false;
    }

    if (!pType)
        return false;

    SchemaClassInfoData_t *pClassInfo = pType->FindDeclaredClass(className).Get();

    if (!pClassInfo)
    {
        SchemaKeyValueMap_t *map = new SchemaKeyValueMap_t(0, 0, DefLessFunc(uint32_t));
        tableMap->Insert(classKey, map);

        Msg("InitSchemaFieldsForClass(): '%s' was not found!\n", className);
        return false;
    }

    short fieldsSize = pClassInfo->m_nFieldCount;
    SchemaClassFieldData_t* pFields = pClassInfo->m_pFields;

    SchemaKeyValueMap_t *keyValueMap = new SchemaKeyValueMap_t(0, 0, DefLessFunc(uint32_t));
    keyValueMap->EnsureCapacity(fieldsSize);
    tableMap->Insert(classKey, keyValueMap);

    for (int i = 0; i < fieldsSize; ++i)
    {
        SchemaClassFieldData_t& field = pFields[i];

// #ifdef _DEBUG
// 		Msg("%s::%s found at -> 0x%X - %llx\n", className, field.m_pszName, field.m_nSingleInheritanceOffset, &field);
// #endif

        keyValueMap->Insert(hash_32_fnv1a_const(field.m_pszName), {field.m_nSingleInheritanceOffset, IsFieldNetworked(field)});
    }

    return true;
}

int16_t schema::FindChainOffset(const char* className)
{
    CSchemaSystemTypeScope* pType = globals::schemaSystem->FindTypeScopeForModule(MODULE_PREFIX "server" MODULE_EXT);

    if (!globals::schemaSystem)
    {
        HMR_ERROR("schemaSystem is nullptr! Called too early?");
        return false;
    }

    if (!pType)
        return false;

    SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className).Get();

    do
    {
        SchemaClassFieldData_t* pFields = pClassInfo->m_pFields;
        short fieldsSize = pClassInfo->m_nFieldCount;
        for (int i = 0; i < fieldsSize; ++i)
        {
            SchemaClassFieldData_t& field = pFields[i];

            if (V_strcmp(field.m_pszName, "__m_pChainEntity") == 0)
            {
                return field.m_nSingleInheritanceOffset;
            }
        }
    } while ((pClassInfo = pClassInfo->m_pBaseClasses ? pClassInfo->m_pBaseClasses->m_pClass : nullptr) != nullptr);

    return 0;
}

SchemaKey schema::GetOffset(const char* className, uint32_t classKey, const char* memberName, uint32_t memberKey)
{
	static SchemaTableMap_t schemaTableMap(0, 0, DefLessFunc(uint32_t));
	int16_t tableMapIndex = schemaTableMap.Find(classKey);
    if (!schemaTableMap.IsValidIndex(tableMapIndex))
    {
        if (InitSchemaFieldsForClass(&schemaTableMap, className, classKey))
            return GetOffset(className, classKey, memberName, memberKey);

        return { 0, 0 };
    }

    SchemaKeyValueMap_t *tableMap = schemaTableMap[tableMapIndex];
	int16_t memberIndex = tableMap->Find(memberKey);
    if (!tableMap->IsValidIndex(memberIndex))
        throw std::runtime_error(std::string("schema::GetOffset(): ") + memberName + " was not found in " + className);

    return tableMap->Element(memberIndex);
}

int32_t schema::GetServerOffset(const char* pszClassName, const char* pszPropName)
{
    SchemaClassInfoData_t* pClassInfo = globals::schemaSystem->FindTypeScopeForModule(MODULE_PREFIX "server" MODULE_EXT)->FindDeclaredClass(pszClassName).Get();

    if (!globals::schemaSystem)
    {
        HMR_ERROR("schemaSystem is nullptr! Called too early?");
        return 0;
    }

    if (pClassInfo)
    {
        for (int i = 0; i < pClassInfo->m_nFieldCount; i++)
        {
            auto& pFieldData = pClassInfo->m_pFields[i];

            if (std::strcmp(pFieldData.m_pszName, pszPropName) == 0)
            {
                return pFieldData.m_nSingleInheritanceOffset;
            }
        }
    }

    return -1;
}

void EntityNetworkStateChanged(uintptr_t entityInstance, uint nOffset)
{
    reinterpret_cast<CEntityInstance*>(entityInstance)->NetworkStateChanged(nOffset);
}

void ChainNetworkStateChanged(uintptr_t networkVarChainer, uint nLocalOffset)
{
    CEntityInstance* pEntity = *reinterpret_cast<CEntityInstance**>(networkVarChainer);
    if (pEntity && (pEntity->m_pEntity->m_flags & EF_IS_CONSTRUCTION_IN_PROGRESS) == 0)
    {
        pEntity->NetworkStateChanged(nLocalOffset, -1, *reinterpret_cast<ChangeAccessorFieldPathIndex_t*>(networkVarChainer + 32));
    }
}
}