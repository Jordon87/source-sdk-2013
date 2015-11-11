//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BASEWEAPON_SHOTGUN_H
#define ELEVENEIGHTYSEVEN_BASEWEAPON_SHOTGUN_H

#if defined ( _WIN32 )
#pragma once
#endif

#include "1187_basecombatweapon_shared.h"

class C1187_BaseWeapon_Shotgun : public CBase1187CombatWeapon
{
	DECLARE_DATADESC();
public:
	DECLARE_CLASS(C1187_BaseWeapon_Shotgun, CBase1187CombatWeapon);

private:
	bool	m_bNeedPump;		// When emptied completely
	bool	m_bDelayedFire1;	// Fire primary when finished reloading
	bool	m_bDelayedFire2;	// Fire secondary when finished reloading

public:
	void	Precache(void);

	int CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual const Vector& GetBulletSpread(void);

	virtual int				GetMinBurst() { return 1; }
	virtual int				GetMaxBurst() { return 3; }

	virtual float			GetMinRestTime();
	virtual float			GetMaxRestTime();

	virtual float			GetFireRate(void);

	bool StartReload(void);
	bool Reload(void);
	void FillClip(void);
	void FinishReload(void);
	void CheckHolsterReload(void);
	void Pump(void);
	//	void WeaponIdle( void );
	void ItemHolsterFrame(void);
	void ItemPostFrame(void);
	void PrimaryAttack(void);
	void SecondaryAttack(void);
	void DryFire(void);

	void FireNPCPrimaryAttack(CBaseCombatCharacter *pOperator, bool bUseWeaponAngles);
	void Operator_ForceNPCFire(CBaseCombatCharacter  *pOperator, bool bSecondary);
	void Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	DECLARE_ACTTABLE();

	C1187_BaseWeapon_Shotgun(void);
};


#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_SHOTGUN_H