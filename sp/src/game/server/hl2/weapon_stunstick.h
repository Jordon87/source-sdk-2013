//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef WEAPON_STUNSTICK_H
#define WEAPON_STUNSTICK_H
#ifdef _WIN32
#pragma once
#endif

#include "basebludgeonweapon.h"
#if defined ( HUMANERROR_DLL )
#include "Human_Error/hlss_weapon_id.h"
#endif

#define	STUNSTICK_RANGE		75.0f
#define	STUNSTICK_REFIRE	0.6f

#if defined ( HUMANERROR_DLL )

#define STUNSTICK_POWER_NEEDED 5

class CWeaponStunStick : public CBaseHLBludgeonWeapon
{
	DECLARE_CLASS( CWeaponStunStick, CBaseHLBludgeonWeapon );
	DECLARE_DATADESC();

public:

	virtual const int		HLSS_GetWeaponId() { return HLSS_WEAPON_ID_STUNSTICK; }

	CWeaponStunStick();

	DECLARE_SERVERCLASS();
	DECLARE_ACTTABLE();

	virtual void Precache();

	void		Spawn();

	void		ItemPostFrame( void );

	float		GetRange( void )		{ return STUNSTICK_RANGE; }
	float		GetFireRate( void )		{ return STUNSTICK_REFIRE; }

	int			WeaponMeleeAttack1Condition( float flDot, float flDist );

	bool		Deploy( void );
	bool		Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	
	void		Drop( const Vector &vecVelocity );
	void		ImpactEffect( trace_t &traceHit );
	void		PrimaryAttack( void );
	void		SecondaryAttack( void );
	void		SetStunState( bool state );
	bool		GetStunState( void );
	void		Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
	
	float		GetDamageForActivity( Activity hitActivity );

	bool		CanBePickedUpByNPCs( void ) { return false;	}	

	virtual const char *GetShootSound( int iIndex ) const;
	virtual void		Hit( trace_t &traceHit, Activity nHitActivity, bool bIsSecondary );
	void				Swing_StunStick();

	bool				DecreamentEnergy( CBasePlayer *pOwner, bool bTakeAmmo );
	virtual bool		VisibleInWeaponSelection( void ) { return true; }
	virtual bool		CanBeSelected( void ) { return true; }
	virtual bool		HasAnyAmmo( void ) { return true; }
	virtual bool		HasAmmo( void ) {  return true; }

#if defined ( STUNSTICK_IMPACT_EFFECT )
	virtual void	AddViewKick(void);

	virtual void	MeleeHit(trace_t &trace);
	virtual void	MeleeHitWorld(trace_t &trace);
#endif

private:

	CNetworkVar( bool, m_bActive );
	CNetworkVar( bool, m_bInSwing );
	bool m_bHasPower;
	int m_iStunstickHints;
	int m_iStunstickDepletedHints;

public:
	bool	HasPower() { return m_bHasPower; }
};
#else
class CWeaponStunStick : public CBaseHLBludgeonWeapon
{
	DECLARE_CLASS( CWeaponStunStick, CBaseHLBludgeonWeapon );
	DECLARE_DATADESC();

public:

	CWeaponStunStick();

	DECLARE_SERVERCLASS();
	DECLARE_ACTTABLE();

	virtual void Precache();

	void		Spawn();

	float		GetRange( void )		{ return STUNSTICK_RANGE; }
	float		GetFireRate( void )		{ return STUNSTICK_REFIRE; }

	int			WeaponMeleeAttack1Condition( float flDot, float flDist );

	bool		Deploy( void );
	bool		Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	
	void		Drop( const Vector &vecVelocity );
	void		ImpactEffect( trace_t &traceHit );
	void		SecondaryAttack( void )	{}
	void		SetStunState( bool state );
	bool		GetStunState( void );
	void		Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
	
	float		GetDamageForActivity( Activity hitActivity );

	bool		CanBePickedUpByNPCs( void ) { return false;	}		

private:

	CNetworkVar( bool, m_bActive );
};
#endif // defined ( HUMANERROR_DLL )

#endif // WEAPON_STUNSTICK_H
