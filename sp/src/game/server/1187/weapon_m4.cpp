//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponM4 : public CHLSelectFireMachineGun
{
	DECLARE_CLASS(CWeaponM4, CHLSelectFireMachineGun);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(CWeaponM4, DT_WeaponM4)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_m4, CWeaponM4);
PRECACHE_WEAPON_REGISTER(weapon_m4);
