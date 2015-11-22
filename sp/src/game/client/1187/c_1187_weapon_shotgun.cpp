//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_weapon__stubs.h"
#include "c_1187_basecombatweapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_1187WeaponShotgun : public C_Base1187CombatWeapon
{
	DECLARE_CLASS(C_1187WeaponShotgun, C_Base1187CombatWeapon);
public:
	C_1187WeaponShotgun(void);

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	virtual void OnDataChanged(DataUpdateType_t updateType);
	virtual void ClientThink(void);

	// Flashlight
	virtual bool			HasBuiltInFlashlight(void) { return true; }
};

STUB_WEAPON_CLASS_IMPLEMENT(weapon_shotgun, C_1187WeaponShotgun);

IMPLEMENT_CLIENTCLASS_DT(C_1187WeaponShotgun, DT_1187WeaponShotgun, C1187WeaponShotgun)
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
C_1187WeaponShotgun::C_1187WeaponShotgun(void)
{

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_1187WeaponShotgun::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);
	SetNextClientThink(CLIENT_THINK_ALWAYS);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_1187WeaponShotgun::ClientThink(void)
{
	BaseClass::ClientThink();
}

