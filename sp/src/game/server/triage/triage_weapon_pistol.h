//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef TRIAGE_WEAPON_PISTOL_H
#define TRIAGE_WEAPON_PISTOL_H

#ifdef _WIN32
#pragma once
#endif

#include "triage_baseweapon_pistol.h"

//-----------------------------------------------------------------------------
// CTriageWeaponPistol
//-----------------------------------------------------------------------------

class CTriageWeaponPistol : public CTriage_BaseWeapon_Pistol
{
	DECLARE_CLASS(CTriageWeaponPistol, CTriage_BaseWeapon_Pistol);
public:
	DECLARE_SERVERCLASS();
};


#endif // TRIAGE_WEAPON_PISTOL_H