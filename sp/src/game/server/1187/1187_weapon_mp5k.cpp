//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_smg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponMP5K
//-----------------------------------------------------------------------------
class C1187WeaponMP5K : public C1187_BaseWeapon_SMG
{
	DECLARE_CLASS(C1187WeaponMP5K, C1187_BaseWeapon_SMG);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponMP5K, DT_1187WeaponMP5K)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_smg1, C1187WeaponMP5K);
PRECACHE_WEAPON_REGISTER(weapon_smg1);