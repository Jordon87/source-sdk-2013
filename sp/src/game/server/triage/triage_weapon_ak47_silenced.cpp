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

class CTriageWeaponAK47Silenced : public CTriageWeaponAK47
{
	DECLARE_CLASS(CTriageWeaponAK47Silenced, CTriageWeaponAK47);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(CTriageWeaponAK47Silenced, DT_TriageWeaponAK47Silenced)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_ak47_s, CTriageWeaponAK47Silenced);
PRECACHE_WEAPON_REGISTER(weapon_ak47_s);