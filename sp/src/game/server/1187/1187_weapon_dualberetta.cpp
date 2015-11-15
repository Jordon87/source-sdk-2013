//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_dualpistol.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponMP5K
//-----------------------------------------------------------------------------
class C1187WeaponDualBeretta : public C1187_BaseWeapon_DualPistol
{
	DECLARE_CLASS(C1187WeaponDualBeretta, C1187_BaseWeapon_DualPistol);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponDualBeretta, DT_1187WeaponDualBeretta)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dualpistol, C1187WeaponDualBeretta);
PRECACHE_WEAPON_REGISTER(weapon_dualpistol);