//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BASEWEAPON_MISSILELAUNCHER_H
#define ELEVENEIGHTYSEVEN_BASEWEAPON_MISSILELAUNCHER_H

#include "1187_basecombatweapon_shared.h"
#include "weapon_rpg.h"

#if defined( _WIN32 )
#pragma once
#endif

//-----------------------------------------------------------------------------
// C1187_BaseWeapon_MissileLauncher
//-----------------------------------------------------------------------------
class C1187_BaseWeapon_MissileLauncher : public CBase1187CombatWeapon
{
	DECLARE_CLASS(C1187_BaseWeapon_MissileLauncher, CBase1187CombatWeapon);
public:

	C1187_BaseWeapon_MissileLauncher();
	~C1187_BaseWeapon_MissileLauncher();

	void	Precache(void);

	void	PrimaryAttack(void);
	virtual float GetFireRate(void) { return 1; }

	void	DecrementAmmo(CBaseCombatCharacter *pOwner);

	bool	Holster(CBaseCombatWeapon *pSwitchingTo = NULL);
	bool	Reload(void);
	bool	WeaponShouldBeLowered(void);
	bool	Lower(void);

	int		GetMinBurst() { return 1; }
	int		GetMaxBurst() { return 1; }
	float	GetMinRestTime() { return 4.0; }
	float	GetMaxRestTime() { return 4.0; }

	bool	WeaponLOSCondition(const Vector &ownerPos, const Vector &targetPos, bool bSetConditions);
	int		WeaponRangeAttack1Condition(float flDot, float flDist);

	void	Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	bool	HasAnyAmmo(void);

	int		CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual const Vector& GetBulletSpread(void);

	CBaseEntity *GetMissile(void) { return m_hMissile; }

	DECLARE_ACTTABLE();
	DECLARE_DATADESC();

protected:

	CHandle<CMissile>	m_hMissile;
};


#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_MISSILELAUNCHER_H
