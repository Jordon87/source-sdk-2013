//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "triage_basecombatweapon.h"
#include "triage_weapon_smg1.h"
#include "npcevent.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "game.h"
#include "in_buttons.h"
#include "grenade_ar2.h"
#include "ai_memory.h"
#include "soundent.h"
#include "rumble_shared.h"
#include "gamestats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CTriageWeaponSMG1Silenced : public CTriageWeaponSMG1
{
	DECLARE_CLASS(CTriageWeaponSMG1Silenced, CTriageWeaponSMG1);
public:
	DECLARE_SERVERCLASS();
};

IMPLEMENT_SERVERCLASS_ST(CTriageWeaponSMG1Silenced, DT_TriageWeaponSMG1Silenced)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_smg1_s, CTriageWeaponSMG1Silenced);
PRECACHE_WEAPON_REGISTER(weapon_smg1_s);