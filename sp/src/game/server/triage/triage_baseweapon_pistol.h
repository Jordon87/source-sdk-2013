//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef TRIAGE_BASEWEAPON_PISTOL_H
#define TRIAGE_BASEWEAPON_PISTOL_H

#ifdef _WIN32
#pragma once
#endif

#include "triage_basecombatweapon.h"

//=========================================================
// CTriage_BaseWeapon_Pistol
//=========================================================

//-----------------------------------------------------------------------------
// CTriage_BaseWeapon_Pistol
//-----------------------------------------------------------------------------

class CTriage_BaseWeapon_Pistol : public CBaseTriageCombatWeapon
{
	DECLARE_DATADESC();

public:
	DECLARE_CLASS(CTriage_BaseWeapon_Pistol, CBaseTriageCombatWeapon);

	CTriage_BaseWeapon_Pistol(void);

	void	ItemPostFrame(void);
	void	ItemPreFrame(void);
	void	ItemBusyFrame(void);
	void	PrimaryAttack(void);
	void	AddViewKick(void);
	void	DryFire(void);
	void	Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	void	UpdatePenaltyTime(void);

	int		CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
	Activity	GetPrimaryAttackActivity(void);

	virtual bool Reload(void);

	virtual const Vector& GetBulletSpread(void);

	virtual int	GetMinBurst() { return 1; }
	virtual int	GetMaxBurst() { return 3; }
	virtual float GetFireRate(void) { return 0.5f; }

	// Weapon slots
	virtual const WeaponSlot_t	GetWeaponSlot() const { return WEAPON_SLOT_PRIMARY; }

	DECLARE_ACTTABLE();

private:
	float	m_flSoonestPrimaryAttack;
	float	m_flLastAttackTime;
	float	m_flAccuracyPenalty;
	int		m_nNumShotsFired;
};



#endif // TRIAGE_BASEWEAPON_PISTOL_H