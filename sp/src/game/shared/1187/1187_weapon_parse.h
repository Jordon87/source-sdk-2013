//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Weapon data file parsing, shared by game & client dlls.
//
// $NoKeywords: $
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_WEAPON_PARSE_H
#define ELEVENEIGHTYSEVEN_WEAPON_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_parse.h"

class KeyValues;

//-----------------------------------------------------------------------------
// Purpose: Contains the data read from the weapon's script file. 
// It's cached so we only read each weapon's script file once.
// Each game provides a CreateWeaponInfo function so it can have game-specific
// data (like CS move speeds) in the weapon script.
//-----------------------------------------------------------------------------
class C1187FileWeaponInfo_t : public FileWeaponInfo_t
{
	typedef FileWeaponInfo_t BaseClass;
public:

	C1187FileWeaponInfo_t();
	
	// Each game can override this to get whatever values it wants from the script.
	virtual void Parse( KeyValues *pKeyValuesData, const char *szWeaponName );

	
public:	
	Vector					vecIronsightPosOffset;
	QAngle					angIronsightAngOffset;
	float					flIronsightFOVOffset;

	Vector					vecFragPosOffset;
};

// Each game implements this. It can return a derived class and override Parse() if it wants.
extern FileWeaponInfo_t* CreateWeaponInfo();


#endif // ELEVENEIGHTYSEVEN_WEAPON_PARSE_H
