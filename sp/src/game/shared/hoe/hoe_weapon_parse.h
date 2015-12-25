//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Weapon data file parsing, shared by game & client dlls.
//
// $NoKeywords: $
//=============================================================================//

#ifndef HOE_WEAPON_PARSE_H
#define HOE_WEAPON_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_parse.h"

//-----------------------------------------------------------------------------
// Purpose: Contains the data read from the weapon's script file. 
// It's cached so we only read each weapon's script file once.
// Each game provides a CreateWeaponInfo function so it can have game-specific
// data (like CS move speeds) in the weapon script.
//-----------------------------------------------------------------------------
class HoeFileWeaponInfo_t : public FileWeaponInfo_t
{
	typedef FileWeaponInfo_t BaseClass;
public:

	HoeFileWeaponInfo_t();
	
	// Each game can override this to get whatever values it wants from the script.
	virtual void Parse( KeyValues *pKeyValuesData, const char *szWeaponName );

	
public:	

	Vector					vecIronsightPosOffset;
	QAngle					angIronsightAngOffset;
	float					flIronsightFOVOffset;

};


#endif // HOE_WEAPON_PARSE_H
