//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A shotgun.
//
//			Primary attack: single barrel shot.
//			Secondary attack: double barrel shot.
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "triage_baseweapon_shotgun.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "gamerules.h"		// For g_pGameRules
#include "in_buttons.h"
#include "soundent.h"
#include "vstdlib/random.h"
#include "gamestats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CTriageWeaponShotgun : public CTriage_BaseWeapon_Shotgun
{
public:
	DECLARE_CLASS(CTriageWeaponShotgun, CTriage_BaseWeapon_Shotgun);
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(CTriageWeaponShotgun, DT_TriageWeaponShotgun)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( weapon_shotgun, CTriageWeaponShotgun );