//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BASEWEAPON_BOLTACTION_H
#define ELEVENEIGHTYSEVEN_BASEWEAPON_BOLTACTION_H

#include "1187_basecombatweapon_shared.h"

#if defined( _WIN32 )
#pragma once
#endif

class C1187_BaseWeapon_BoltAction : public CBase1187CombatWeapon
{
	DECLARE_DATADESC();
public:
	DECLARE_CLASS(C1187_BaseWeapon_BoltAction, CBase1187CombatWeapon);

private:
	bool	m_bNeedPullPin;		// When emptied completely
	bool	m_bDelayedFire1;	// Fire primary when finished reloading

public:
	void	Precache(void);

	int CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual const Vector& GetBulletSpread(void);

	virtual int				GetMinBurst() { return 1; }
	virtual int				GetMaxBurst() { return 3; }

	virtual float			GetMinRestTime();
	virtual float			GetMaxRestTime();

	virtual float			GetFireRate(void);

	void CheckHolsterReload(void);
	void PullPin(void);
	//	void WeaponIdle( void );
	void ItemHolsterFrame(void);
	void ItemPostFrame(void);
	void PrimaryAttack(void);
	void DryFire(void);

	void FireNPCPrimaryAttack(CBaseCombatCharacter *pOperator, bool bUseWeaponAngles);
	void Operator_ForceNPCFire(CBaseCombatCharacter  *pOperator, bool bSecondary);
	void Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	DECLARE_ACTTABLE();

	C1187_BaseWeapon_BoltAction(void);
};


#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_BOLTACTION_H