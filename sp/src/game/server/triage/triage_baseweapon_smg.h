//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef TRIAGE_BASEWEAPON_SMG_H
#define TRIAGE_BASEWEAPON_SMG_H

#ifdef _WIN32
#pragma once
#endif

#include "triage_baseweapon_rifle.h"

//=========================================================
// CTriage_BaseWeapon_SMG
//=========================================================
class CTriage_BaseWeapon_SMG : public CTriage_BaseWeapon_Rifle
{
	DECLARE_DATADESC();
public:
	DECLARE_CLASS(CTriage_BaseWeapon_SMG, CTriage_BaseWeapon_Rifle);

	virtual void	Precache(void);
	virtual void	SecondaryAttack(void);
	virtual bool	Reload(void);

	virtual int		WeaponRangeAttack2Condition(float flDot, float flDist);

	// Weapon slots
	virtual const WeaponSlot_t	GetWeaponSlot() const { return WEAPON_SLOT_SECONDARY; }

protected:

	Vector	m_vecTossVelocity;
	float	m_flNextGrenadeCheck;
};


#endif // TRIAGE_BASEWEAPON_SMG_H