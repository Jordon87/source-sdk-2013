//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef TRIAGE_WEAPON_CROWBAR_H
#define TRIAGE_WEAPON_CROWBAR_H

#include "triage_basebludgeonweapon.h"

#if defined( _WIN32 )
#pragma once
#endif

#define	CROWBAR_RANGE	75.0f
#define	CROWBAR_REFIRE	0.4f

//-----------------------------------------------------------------------------
// CWeaponCrowbar
//-----------------------------------------------------------------------------

class CTriageWeaponCrowbar : public CBaseTriageBludgeonWeapon
{
public:
	DECLARE_CLASS( CTriageWeaponCrowbar, CBaseTriageBludgeonWeapon );

	DECLARE_SERVERCLASS();
	DECLARE_ACTTABLE();

	CTriageWeaponCrowbar();

	float		GetRange( void )		{	return	CROWBAR_RANGE;	}
	float		GetFireRate( void )		{	return	CROWBAR_REFIRE;	}

	void		AddViewKick( void );
	float		GetDamageForActivity( Activity hitActivity );

	virtual int WeaponMeleeAttack1Condition( float flDot, float flDist );
	void		SecondaryAttack( void )	{	return;	}

	// Animation event
	virtual void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );

private:
	// Animation event handlers
	void HandleAnimEventMeleeHit( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
};

#endif // TRIAGE_WEAPON_CROWBAR_H
