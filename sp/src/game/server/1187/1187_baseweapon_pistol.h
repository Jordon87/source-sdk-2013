//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BASEWEAPON_PISTOL_H
#define ELEVENEIGHTYSEVEN_BASEWEAPON_PISTOL_H

#if defined ( _WIN32 )
#pragma once
#endif

#include "1187_basecombatweapon.h"

class C1187_BaseWeapon_Pistol : public CBase1187CombatWeapon
{
	DECLARE_DATADESC();

public:
	DECLARE_CLASS(C1187_BaseWeapon_Pistol, CBase1187CombatWeapon);

	C1187_BaseWeapon_Pistol(void);

	void	Precache(void);
	void	ItemPostFrame(void);
	void	ItemPreFrame(void);
	void	ItemBusyFrame(void);
	void	PrimaryAttack(void);
	void	AddViewKick(void);
	void	DryFire(void);
	void	Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	void	UpdatePenaltyTime(void);

	int		CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	virtual bool Reload(void);

	virtual const Vector& GetBulletSpread(void);

	virtual int	GetMinBurst() { return 1; }
	virtual int	GetMaxBurst() { return 3; }

	virtual float GetFireRate(void) { return 0.5f; }

	DECLARE_ACTTABLE();

private:
	float	m_flSoonestPrimaryAttack;
	float	m_flLastAttackTime;
	float	m_flAccuracyPenalty;
	int		m_nNumShotsFired;
};




#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_PISTOL_H