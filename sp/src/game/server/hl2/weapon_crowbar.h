//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef WEAPON_CROWBAR_H
#define WEAPON_CROWBAR_H

#include "basehlcombatweapon.h"

#if defined( _WIN32 )
#pragma once
#endif

#ifdef HL2MP
#error weapon_crowbar.h must not be included in hl2mp. The windows compiler will use the wrong class elsewhere if it is.
#endif


//-----------------------------------------------------------------------------
// CWeaponCrowbar
//-----------------------------------------------------------------------------

class CWeaponCrowbar : public CBaseHLCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponCrowbar, CBaseHLCombatWeapon);

	DECLARE_SERVERCLASS();
	DECLARE_ACTTABLE();

	CWeaponCrowbar();

	bool		HasAnyAmmo() { return true; }
	bool		HasIronsights() { return false; }

	void		PrimaryAttack( void );
	void		ItemPostFrame( void );
};

#endif // WEAPON_CROWBAR_H
