//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef TRIAGE_BASEWEAPON_RIFLE_H
#define TRIAGE_BASEWEAPON_RIFLE_H

#ifdef _WIN32
#pragma once
#endif

#include "triage_basecombatweapon.h"

//=========================================================
// CTriage_BaseWeapon_Rifle
//=========================================================
class CTriage_BaseWeapon_Rifle : public CTriageSelectFireMachineGun
{
public:
	DECLARE_CLASS(CTriage_BaseWeapon_Rifle, CTriageSelectFireMachineGun);

	CTriage_BaseWeapon_Rifle();

	virtual void	AddViewKick(void);

	virtual int		GetMinBurst() { return 2; }
	virtual int		GetMaxBurst() { return 5; }

	virtual void Equip(CBaseCombatCharacter *pOwner);

	virtual float	GetFireRate(void) { return 0.075f; }	// 13.3hz
	virtual int		CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
	virtual Activity	GetPrimaryAttackActivity(void);

	virtual const Vector& GetBulletSpread(void);

	const WeaponProficiencyInfo_t *GetProficiencyValues();

	void FireNPCPrimaryAttack(CBaseCombatCharacter *pOperator, Vector &vecShootOrigin, Vector &vecShootDir);
	void Operator_ForceNPCFire(CBaseCombatCharacter  *pOperator, bool bSecondary);
	void Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	// Weapon slots
	virtual const WeaponSlot_t	GetWeaponSlot() const { return WEAPON_SLOT_SECONDARY; }

	DECLARE_ACTTABLE();
};


#endif // TRIAGE_BASEWEAPON_RIFLE_H