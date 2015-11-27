//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef TRIAGE_WEAPON_PISTOL_TACTICAL_H
#define TRIAGE_WEAPON_PISTOL_TACTICAL_H

#ifdef _WIN32
#pragma once
#endif

#include "triage_weapon_pistol.h"

class CTriageWeaponPistolTactical : public CTriageWeaponPistol
{
	DECLARE_CLASS(CTriageWeaponPistolTactical, CTriageWeaponPistol);
public:
	DECLARE_SERVERCLASS();
};

#endif // TRIAGE_WEAPON_PISTOL_TACTICAL_H