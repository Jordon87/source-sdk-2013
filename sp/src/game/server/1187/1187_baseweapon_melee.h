//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BASEWEAPON_MELEE_H
#define ELEVENEIGHTYSEVEN_BASEWEAPON_MELEE_H

#include "1187_basebludgeonweapon.h"

#if defined( _WIN32 )
#pragma once
#endif

//-----------------------------------------------------------------------------
// CWeaponCrowbar
//-----------------------------------------------------------------------------

class C1187_BaseWeapon_Melee : public CBase1187BludgeonWeapon
{
public:
	DECLARE_CLASS(C1187_BaseWeapon_Melee, CBase1187BludgeonWeapon);
	DECLARE_ACTTABLE();

	C1187_BaseWeapon_Melee();

	void		AddViewKick( void );

	virtual Activity	GetPrimaryAttackActivity(void)	{ return ACT_VM_PRIMARYATTACK; }
	virtual Activity	GetSecondaryAttackActivity(void)	{ return ACT_VM_SECONDARYATTACK; }
	virtual Activity	GetHitActivity(void)	{ return GetPrimaryAttackActivity(); }

	virtual int WeaponMeleeAttack1Condition( float flDot, float flDist );
	void		PrimaryAttack(void);
	void		SecondaryAttack( void )	{ return; }

	// Animation event
	virtual void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );

protected:

	void			Swing(int bIsSecondary);

private:
	// Animation event handlers
	void HandleAnimEventMeleeHit( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
};

#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_MELEE_H
