//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "triage_basecombatweapon.h"
#include "triage_weapon_ak47.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_SERVERCLASS_ST(CTriageWeaponAK47, DT_TriageWeaponAK47)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_ak47, CTriageWeaponAK47);