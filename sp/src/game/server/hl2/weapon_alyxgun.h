//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef WEAPON_ALYXGUN_H
#define WEAPON_ALYXGUN_H

#include "basehlcombatweapon.h"
#if defined ( HUMANERROR_DLL )
#include "Human_Error/hlss_weapon_id.h"
#endif

#if defined( _WIN32 )
#pragma once
#endif

class CWeaponAlyxGun : public CHLSelectFireMachineGun
{
	DECLARE_DATADESC();
public:
	DECLARE_CLASS( CWeaponAlyxGun, CHLSelectFireMachineGun );

	CWeaponAlyxGun();
	~CWeaponAlyxGun();

	DECLARE_SERVERCLASS();

#if defined ( HUMANERROR_DLL )
	virtual const int		HLSS_GetWeaponId() { return HLSS_WEAPON_ID_ALYXGUN; }
#endif
	
	void	Precache( void );

	virtual int		GetMinBurst( void ) { return 4; }
	virtual int		GetMaxBurst( void ) { return 7; }
	virtual float	GetMinRestTime( void );
	virtual float	GetMaxRestTime( void );

	virtual void Equip( CBaseCombatCharacter *pOwner );

	float	GetFireRate( void ) { return 0.1f; }
	int		CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
	int		WeaponRangeAttack1Condition( float flDot, float flDist );
	int		WeaponRangeAttack2Condition( float flDot, float flDist );

	virtual const Vector& GetBulletSpread( void );

	void FireNPCPrimaryAttack( CBaseCombatCharacter *pOperator, bool bUseWeaponAngles );

	void Operator_ForceNPCFire( CBaseCombatCharacter  *pOperator, bool bSecondary );
	void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );

#if defined ( HUMANERROR_DLL )
	/*virtual void SetPickupTouch( void )
	{
		// Alyx gun cannot be picked up
		SetTouch(NULL);
	}*/
#else
	virtual void SetPickupTouch( void )
	{
		// Alyx gun cannot be picked up
		SetTouch(NULL);
	}
#endif

	float m_flTooCloseTimer;

	DECLARE_ACTTABLE();

};

#endif // WEAPON_ALYXGUN_H
