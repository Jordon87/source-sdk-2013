//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_revolver.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponMP5K
//-----------------------------------------------------------------------------
class C1187WeaponPPC : public C1187_BaseWeapon_Revolver
{
	DECLARE_CLASS(C1187WeaponPPC, C1187_BaseWeapon_Revolver);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponPPC, DT_1187WeaponPPC)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_357, C1187WeaponPPC);
PRECACHE_WEAPON_REGISTER(weapon_357);