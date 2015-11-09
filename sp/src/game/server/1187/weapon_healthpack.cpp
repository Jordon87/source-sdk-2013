//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponHealthPack : public CBaseHLCombatWeapon
{
	DECLARE_CLASS(CWeaponHealthPack, CBaseHLCombatWeapon);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(CWeaponHealthPack, DT_WeaponHealthPack)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_healthpack, CWeaponHealthPack);
PRECACHE_WEAPON_REGISTER(weapon_healthpack);
