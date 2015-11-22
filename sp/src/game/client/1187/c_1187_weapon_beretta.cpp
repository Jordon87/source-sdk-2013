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

class C_1187WeaponBeretta : public C_Base1187CombatWeapon
{
	DECLARE_CLASS(C_1187WeaponBeretta, C_Base1187CombatWeapon);
public:
	C_1187WeaponBeretta(void);

	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	virtual void OnDataChanged(DataUpdateType_t updateType);
	virtual void ClientThink(void);

	// Flashlight
	virtual bool			HasBuiltInFlashlight(void) { return true; }
};

STUB_WEAPON_CLASS_IMPLEMENT(weapon_pistol, C_1187WeaponBeretta);

IMPLEMENT_CLIENTCLASS_DT(C_1187WeaponBeretta, DT_1187WeaponBeretta, C1187WeaponBeretta)
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
C_1187WeaponBeretta::C_1187WeaponBeretta(void)
{

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_1187WeaponBeretta::OnDataChanged(DataUpdateType_t updateType)
{
	BaseClass::OnDataChanged(updateType);
	SetNextClientThink(CLIENT_THINK_ALWAYS);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_1187WeaponBeretta::ClientThink(void)
{
	BaseClass::ClientThink();
}

