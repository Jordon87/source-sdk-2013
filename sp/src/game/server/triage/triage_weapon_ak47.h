//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef TRIAGE_WEAPON_AK47_H
#define TRIAGE_WEAPON_AK47_H

#ifdef _WIN32
#pragma once
#endif

#include "triage_baseweapon_rifle.h"

class CTriageWeaponAK47 : public CTriage_BaseWeapon_Rifle
{
	DECLARE_CLASS(CTriageWeaponAK47, CTriage_BaseWeapon_Rifle);
public:
	DECLARE_SERVERCLASS();
};


#endif // TRIAGE_WEAPON_AK47_H