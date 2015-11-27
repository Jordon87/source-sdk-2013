//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Weapon data file parsing, shared by game & client dlls.
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include <KeyValues.h>
#include <tier0/mem.h>
#include "filesystem.h"
#include "utldict.h"
#include "ammodef.h"
#include "triage_weapon_parse.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// CTriageFileWeaponInfo_t implementation.
//-----------------------------------------------------------------------------

CTriageFileWeaponInfo_t::CTriageFileWeaponInfo_t()	
	: BaseClass()
{

}


void CTriageFileWeaponInfo_t::Parse( KeyValues *pKeyValuesData, const char *szWeaponName )
{
	BaseClass::Parse(pKeyValuesData, szWeaponName);

	KeyValues *pSights = pKeyValuesData->FindKey("IronSight");
	if (pSights)
	{
		vecIronsightPosOffset.x = pSights->GetFloat("forward", 0.0f);
		vecIronsightPosOffset.y = pSights->GetFloat("right", 0.0f);
		vecIronsightPosOffset.z = pSights->GetFloat("up", 0.0f);

		angIronsightAngOffset[PITCH] = pSights->GetFloat("pitch", 0.0f);
		angIronsightAngOffset[YAW] = pSights->GetFloat("yaw", 0.0f);
		angIronsightAngOffset[ROLL] = pSights->GetFloat("roll", 0.0f);

		flIronsightFOVOffset = pSights->GetFloat("fov", 0.0f);
	}
	else
	{
		//note: you can set a bool here if you'd like to disable ironsights for weapons with no IronSight-key
		vecIronsightPosOffset = vec3_origin;
		angIronsightAngOffset.Init();
		flIronsightFOVOffset = 0.0f;
	}

	// Read the equip icon.
	cEquipIcon = *pKeyValuesData->GetString("EquipIcon", "");

	DevMsg("\n");
}

