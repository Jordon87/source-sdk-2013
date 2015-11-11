//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BASEWEAPON_RIFLE_H
#define ELEVENEIGHTYSEVEN_BASEWEAPON_RIFLE_H

#ifdef _WIN32
#pragma once
#endif

#include "1187_basecombatweapon.h"

class C1187_BaseWeapon_Rifle : public C1187SelectFireMachineGun
{
public:
	DECLARE_CLASS(C1187_BaseWeapon_Rifle, C1187SelectFireMachineGun);

	C1187_BaseWeapon_Rifle();

	void	Precache(void);
	void	AddViewKick(void);
	void	SecondaryAttack(void) { return; }

	int		GetMinBurst() { return 2; }
	int		GetMaxBurst() { return 5; }

	virtual void Equip(CBaseCombatCharacter *pOwner);
	bool	Reload(void);

	float	GetFireRate(void) { return 0.075f; }	// 13.3hz
	int		CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual const Vector& GetBulletSpread(void);

	const WeaponProficiencyInfo_t *GetProficiencyValues();

	void FireNPCPrimaryAttack(CBaseCombatCharacter *pOperator, Vector &vecShootOrigin, Vector &vecShootDir);
	void Operator_ForceNPCFire(CBaseCombatCharacter  *pOperator, bool bSecondary);
	void Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	DECLARE_ACTTABLE();

protected:
};

#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_RIFLE_H