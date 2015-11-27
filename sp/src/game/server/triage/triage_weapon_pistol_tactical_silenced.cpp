//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "basehlcombatweapon.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "gamerules.h"
#include "in_buttons.h"
#include "soundent.h"
#include "game.h"
#include "vstdlib/random.h"
#include "gamestats.h"
#include "triage_weapon_pistol_tactical.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// CTriageWeaponPistolTacticalSilenced
//-----------------------------------------------------------------------------

class CTriageWeaponPistolTacticalSilenced : public CTriageWeaponPistolTactical
{
	DECLARE_CLASS(CTriageWeaponPistolTacticalSilenced, CTriageWeaponPistolTactical);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(CTriageWeaponPistolTacticalSilenced, DT_TriageWeaponPistolTacticalSilenced)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_pistol_t_s, CTriageWeaponPistolTacticalSilenced);
PRECACHE_WEAPON_REGISTER(weapon_pistol_t_s);