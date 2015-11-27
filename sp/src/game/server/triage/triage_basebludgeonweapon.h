//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		The class from which all bludgeon melee
//				weapons are derived. 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#include "triage_basecombatweapon.h"

#ifndef TRIAGE_BASEBLUDGEONWEAPON_H
#define TRIAGE_BASEBLUDGEONWEAPON_H

//=========================================================
// CBaseTriageBludgeonWeapon 
//=========================================================
class CBaseTriageBludgeonWeapon : public CBaseTriageCombatWeapon
{
	DECLARE_CLASS(CBaseTriageBludgeonWeapon, CBaseTriageCombatWeapon);
public:
	CBaseTriageBludgeonWeapon();

	DECLARE_SERVERCLASS();

	virtual	void	Spawn( void );
	virtual	void	Precache( void );
	
	//Attack functions
	virtual	void	PrimaryAttack( void );
	virtual	void	SecondaryAttack( void );
	
	virtual void	ItemPostFrame( void );

	//Functions to select animation sequences 
	virtual Activity	GetPrimaryAttackActivity( void )	{	return	ACT_VM_HITCENTER;	}
	virtual Activity	GetSecondaryAttackActivity( void )	{	return	ACT_VM_HITCENTER2;	}

	virtual	float	GetFireRate( void )								{	return	0.2f;	}
	virtual float	GetRange( void )								{	return	32.0f;	}
	virtual	float	GetDamageForActivity( Activity hitActivity )	{	return	1.0f;	}

	virtual int		CapabilitiesGet( void );
	virtual	int		WeaponMeleeAttack1Condition( float flDot, float flDist );

	// Ironsights
	virtual bool	HasIronsights() { return false; }

	// Weapon slots
	virtual const WeaponSlot_t	GetWeaponSlot() const { return WEAPON_SLOT_MELEE; }

protected:
	virtual	void	ImpactEffect( trace_t &trace );

private:
	bool			ImpactWater( const Vector &start, const Vector &end );
	void			Swing( int bIsSecondary );
	void			Hit( trace_t &traceHit, Activity nHitActivity, bool bIsSecondary );
	Activity		ChooseIntersectionPointAndActivity( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CBasePlayer *pOwner );
};

#endif // TRIAGE_BASEBLUDGEONWEAPON_H