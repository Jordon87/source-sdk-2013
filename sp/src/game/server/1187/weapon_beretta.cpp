//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Pistol - hand gun
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "weapon_pistol.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//-----------------------------------------------------------------------------
// CWeaponBeretta
//-----------------------------------------------------------------------------

class CWeaponBeretta : public CWeaponPistol
{
	DECLARE_CLASS(CWeaponBeretta, CWeaponPistol);
public:
	DECLARE_SERVERCLASS();

	Activity		GetPrimaryAttackActivity(void);
	Activity		GetIronsightsPrimaryAttackActivity(void);
};

IMPLEMENT_SERVERCLASS_ST(CWeaponBeretta, DT_WeaponBeretta)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_pistol, CWeaponBeretta);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Activity CWeaponBeretta::GetPrimaryAttackActivity(void)
{
	return ACT_VM_PRIMARYATTACK;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Activity CWeaponBeretta::GetIronsightsPrimaryAttackActivity(void)
{
	return ACT_VM_RECOIL1;
}