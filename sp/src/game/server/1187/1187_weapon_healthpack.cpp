//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponHealthPack
//-----------------------------------------------------------------------------
class CWeaponHealthPack : public CBaseHLCombatWeapon
{
	DECLARE_CLASS(CWeaponHealthPack, CBaseHLCombatWeapon);
	DECLARE_SERVERCLASS();

public:
	CWeaponHealthPack();

	void Think(void);

	bool HasIronsights() { return false; }
	void PrimaryAttack(void) { return; }
};

IMPLEMENT_SERVERCLASS_ST(CWeaponHealthPack, DT_WeaponHealthPack)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_healthpack, CWeaponHealthPack);
PRECACHE_WEAPON_REGISTER(weapon_healthpack);


CWeaponHealthPack::CWeaponHealthPack()
{
	m_bReloadsSingly = true;
	m_bFiresUnderwater = true;
}

void CWeaponHealthPack::Think(void)
{
	BaseClass::Think();

	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner && pOwner->IsPlayer() && m_iClip1 <= 0)
	{
		pOwner->SelectLastItem();
		pOwner->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
	}
}
