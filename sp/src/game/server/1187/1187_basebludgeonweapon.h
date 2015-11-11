//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		The class from which all bludgeon melee
//				weapons are derived. 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#include "1187_basecombatweapon.h"

#ifndef ELEVENEIGHTYSEVEN_BASEBLUDGEONWEAPON_H
#define ELEVENEIGHTYSEVEN_BASEBLUDGEONWEAPON_H

//=========================================================
// CBase1187BludgeonWeapon 
//=========================================================
class CBase1187BludgeonWeapon : public CBase1187CombatWeapon
{
	DECLARE_CLASS(CBase1187BludgeonWeapon, CBase1187CombatWeapon);
public:
	CBase1187BludgeonWeapon();

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

	virtual bool	HasIronsights(void) { return false; }

protected:
	virtual	void	ImpactEffect( trace_t &trace );

#if 0 // Allow derived classes.
private:
#endif
	bool			ImpactWater( const Vector &start, const Vector &end );
	void			Swing( int bIsSecondary );
	void			Hit( trace_t &traceHit, Activity nHitActivity, bool bIsSecondary );
	Activity		ChooseIntersectionPointAndActivity( trace_t &hitTrace, const Vector &mins, const Vector &maxs, CBasePlayer *pOwner );
};

#endif // ELEVENEIGHTYSEVEN_BASEBLUDGEONWEAPON_H