//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_smg1.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponMP5K : public CWeaponSMG1
{
	DECLARE_CLASS(CWeaponMP5K, CWeaponSMG1);
public:
	DECLARE_SERVERCLASS();

	Activity	GetPrimaryAttackActivity(void);
	Activity	GetIronsightsPrimaryAttackActivity(void);
};

IMPLEMENT_SERVERCLASS_ST(CWeaponMP5K, DT_WeaponMP5K)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_smg1, CWeaponMP5K);

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponMP5K::GetPrimaryAttackActivity(void)
{
	return ACT_VM_PRIMARYATTACK;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponMP5K::GetIronsightsPrimaryAttackActivity(void)
{
	return ACT_VM_RECOIL1;
}