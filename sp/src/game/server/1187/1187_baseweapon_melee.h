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

#define BASEWEAPON_MELEE_RANGE		75.0f

//-----------------------------------------------------------------------------
// C1187_BaseWeapon_Melee
//-----------------------------------------------------------------------------

class C1187_BaseWeapon_Melee : public CBase1187BludgeonWeapon
{
	DECLARE_DATADESC();
public:
	DECLARE_CLASS(C1187_BaseWeapon_Melee, CBase1187BludgeonWeapon);
	DECLARE_ACTTABLE();

	C1187_BaseWeapon_Melee();

	void		AddViewKick( void );

	virtual void	ItemBusyFrame(void);
	virtual void	ItemPostFrame(void);

	virtual float	GetRange(void) { return BASEWEAPON_MELEE_RANGE; }

	virtual Activity	GetPrimaryAttackActivity(void)	{ return ACT_VM_PRIMARYATTACK; }
	virtual Activity	GetSecondaryAttackActivity(void)	{ return ACT_VM_SECONDARYATTACK; }
	virtual Activity	GetHitActivity(void)	{ return GetPrimaryAttackActivity(); }

	virtual int WeaponMeleeAttack1Condition( float flDot, float flDist );
	void		PrimaryAttack(void);
	void		SecondaryAttack( void )	{ return; }

	virtual void		UpdateAttackHit(void);

	virtual float		GetPrimaryAttackHitDelay(void) const { return 0.0f; }
	virtual float		GetSecondaryAttackHitDelay(void) const { return 0.0f; }

	// Animation event
	virtual void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
	virtual void Operator_HandleHitEvent(bool bIsSecondary, CBaseCombatCharacter *pOperator);

protected:

	void			Swing(int bIsSecondary);

private:
	// Animation event handlers
	void HandleAnimEventMeleeHit( animevent_t *pEvent, CBaseCombatCharacter *pOperator );

	float		m_flAttackHitTime;
	bool		m_bInAttackHit;
	bool		m_bIsSecondaryAttack;
};

#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_MELEE_H
