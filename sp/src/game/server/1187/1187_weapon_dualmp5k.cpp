//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_dualsmg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponMP5K
//-----------------------------------------------------------------------------
class C1187WeaponDualMP5K : public C1187_BaseWeapon_DualSMG
{
	DECLARE_CLASS(C1187WeaponDualMP5K, C1187_BaseWeapon_DualSMG);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponDualMP5K, DT_1187WeaponDualMP5K)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dualmp5k, C1187WeaponDualMP5K);
PRECACHE_WEAPON_REGISTER(weapon_dualmp5k);