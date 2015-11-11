//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_rifle.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponM16
//-----------------------------------------------------------------------------
class C1187WeaponM16 : public C1187_BaseWeapon_Rifle
{
	DECLARE_CLASS(C1187WeaponM16, C1187_BaseWeapon_Rifle);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponM16, DT_1187WeaponM16)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_m16, C1187WeaponM16);
PRECACHE_WEAPON_REGISTER(weapon_m16);