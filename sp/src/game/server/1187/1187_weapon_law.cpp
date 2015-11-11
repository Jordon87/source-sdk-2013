//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Pistol - hand gun
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_missilelauncher.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponLAW
//-----------------------------------------------------------------------------
class C1187WeaponLAW : public C1187_BaseWeapon_MissileLauncher
{
	DECLARE_CLASS(C1187WeaponLAW, C1187_BaseWeapon_MissileLauncher);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponLAW, DT_1187WeaponLAW)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_rpg, C1187WeaponLAW);
PRECACHE_WEAPON_REGISTER(weapon_rpg);