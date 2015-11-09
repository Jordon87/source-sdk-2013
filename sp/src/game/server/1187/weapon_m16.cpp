//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponM16 : public CHLSelectFireMachineGun
{
	DECLARE_CLASS(CWeaponM16, CHLSelectFireMachineGun);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(CWeaponM16, DT_WeaponM16)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_m16, CWeaponM16);
PRECACHE_WEAPON_REGISTER(weapon_m16);