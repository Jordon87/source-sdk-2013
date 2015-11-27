//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef TRIAGE_WEAPON_SLOTS_H
#define TRIAGE_WEAPON_SLOTS_H

#ifdef _WIN32
#pragma once
#endif

enum WeaponSlot_t
{
	WEAPON_SLOT_INVALID = -1,
	WEAPON_SLOT_MELEE = 0,
	WEAPON_SLOT_PRIMARY,
	WEAPON_SLOT_SECONDARY,
	WEAPON_SLOT_EXPLOSIVE,

	// add new weapon slots here.

	WEAPON_SLOT_LAST,			// Must be the last.
};

#endif // TRIAGE_WEAPON_SLOTS_H