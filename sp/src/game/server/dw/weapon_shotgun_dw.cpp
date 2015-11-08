//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "weapon_shotgun.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponShotgunDW : public CWeaponShotgun
{
	DECLARE_CLASS(CWeaponShotgunDW, CWeaponShotgun);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(CWeaponShotgunDW, DT_WeaponShotgunDW)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_shotgun_dw, CWeaponShotgunDW);
PRECACHE_WEAPON_REGISTER(weapon_shotgun_dw);