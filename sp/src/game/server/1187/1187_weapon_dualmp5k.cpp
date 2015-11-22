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

	virtual bool			HasIronsights(void) { return false; }

	virtual void	AddMeleeViewKick(void);
	virtual void	AddMeleeViewMiss(void);

	virtual void	AddViewKickLeft(void);
	virtual void	AddViewKickRight(void);
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponDualMP5K, DT_1187WeaponDualMP5K)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dualmp5k, C1187WeaponDualMP5K);
PRECACHE_WEAPON_REGISTER(weapon_dualmp5k);

void C1187WeaponDualMP5K::AddMeleeViewKick(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		pPlayer->ViewPunch(QAngle(random->RandomFloat(4, 6), random->RandomFloat(1, 2), 0));
	}
}

void C1187WeaponDualMP5K::AddMeleeViewMiss(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		pPlayer->ViewPunch(QAngle(random->RandomFloat(4, 8), random->RandomFloat(2, 3), 0));
	}
}

void C1187WeaponDualMP5K::AddViewKickLeft(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		pPlayer->ViewPunch(QAngle(-random->RandomFloat(0, 1), -random->RandomFloat(0, 1), 0));
	}
}

void C1187WeaponDualMP5K::AddViewKickRight(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		pPlayer->ViewPunch(QAngle(-random->RandomFloat(0, 1), random->RandomFloat(0, 1), 0));
	}
}