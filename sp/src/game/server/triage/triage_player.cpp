//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for Triage.
//
//=============================================================================//

#include "cbase.h"
#include "triage_player.h"
#include "globalstate.h"
#include "game.h"
#include "gamerules.h"
#include "trains.h"
#include "triage_basecombatweapon_shared.h"
#include "vcollide_parse.h"
#include "in_buttons.h"
#include "ai_interactions.h"
#include "ai_squad.h"
#include "igamemovement.h"
#include "ai_hull.h"
#include "hl2_shareddefs.h"
#include "info_camera_link.h"
#include "point_camera.h"
#include "engine/IEngineSound.h"
#include "ndebugoverlay.h"
#include "iservervehicle.h"
#include "IVehicle.h"
#include "globals.h"
#include "collisionutils.h"
#include "coordsize.h"
#include "effect_color_tables.h"
#include "vphysics/player_controller.h"
#include "player_pickup.h"
#include "weapon_physcannon.h"
#include "script_intro.h"
#include "effect_dispatch_data.h"
#include "te_effect_dispatch.h" 
#include "ai_basenpc.h"
#include "AI_Criteria.h"
#include "npc_barnacle.h"
#include "entitylist.h"
#include "env_zoom.h"
#include "triage_gamerules.h"
#include "prop_combine_ball.h"
#include "datacache/imdlcache.h"
#include "eventqueue.h"
#include "gamestats.h"
#include "filters.h"
#include "tier0/icommandline.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sv_regeneration("sv_regeneration", "1", FCVAR_REPLICATED);
ConVar sv_regeneration_wait_time("sv_regeneration_wait_time", "2.0", FCVAR_REPLICATED);
ConVar sv_regeneration_rate("sv_regeneration_rate", "0.05", FCVAR_REPLICATED);
ConVar sv_regeneration_health("sv_regeneration_health", "1", FCVAR_REPLICATED);

ConVar sv_player_dmg_alpha_min("sv_player_dmg_alpha_min", "0", FCVAR_REPLICATED);
ConVar sv_player_dmg_alpha_max("sv_player_dmg_alpha_max", "255", FCVAR_REPLICATED);

LINK_ENTITY_TO_CLASS( player, CTriage_Player );

// Global Savedata for HL2 player
BEGIN_DATADESC( CTriage_Player )
	DEFINE_EMBEDDED( m_TriageLocal ),
	DEFINE_FIELD(m_fRegenRemander, FIELD_FLOAT),
	DEFINE_FIELD(m_fTimeLastHurt, FIELD_TIME),
	DEFINE_FIELD(m_fTimeNextRegen, FIELD_TIME),
	DEFINE_FIELD(m_bFadeDamageRemoved, FIELD_BOOLEAN),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CTriage_Player, DT_Triage_Player)
	SendPropDataTable(SENDINFO_DT(m_TriageLocal), &REFERENCE_SEND_TABLE(DT_TriageLocal), SendProxy_SendLocalDataTable),
END_SEND_TABLE()

CTriage_Player::CTriage_Player() : BaseClass()
{
	m_fRegenRemander = 0;
	m_fTimeLastHurt = 0.0f;
	m_fTimeNextRegen = 0.0f;
	m_bFadeDamageRemoved = true;
}

bool CTriage_Player::ClientCommand(const CCommand &args)
{
	if (stricmp(args[0], "toggle_ironsight") == 0)
	{
		CBaseTriageCombatWeapon *pWeapon = ToBaseTriageCombatWeapon( GetActiveWeapon() );
		if (pWeapon != NULL)
			pWeapon->ToggleIronsights();


		return true;
	}

	return BaseClass::ClientCommand(args);
}

int CTriage_Player::OnTakeDamage(const CTakeDamageInfo &info)
{
	int iRet = BaseClass::OnTakeDamage(info);

	if (iRet)
	{
		if (GetHealth() < GetMaxHealth())
		{
			m_bFadeDamageRemoved = false;
			m_fTimeLastHurt = gpGlobals->curtime;
		}
	}

	return iRet;
}

//-----------------------------------------------------------------------------
// Purpose: Player reacts to bumping a weapon. 
// Input  : pWeapon - the weapon that the player bumped into.
// Output : Returns true if player picked up the weapon
//-----------------------------------------------------------------------------
bool CTriage_Player::BumpWeapon(CBaseCombatWeapon *pWeapon)
{
	bool bRet = BaseClass::BumpWeapon(pWeapon);
	
	// Only send the message if we already have a weapon in the same slot.
	if (!bRet)
	{
		Assert( GetActiveWeapon() );

		if (GetActiveWeapon()->GetSlot() == pWeapon->GetSlot() && !FClassnameIs(pWeapon, GetActiveWeapon()->GetClassname()))
		{
			m_TriageLocal.m_bBumpWeapon = true;
			m_TriageLocal.m_hBumpWeapon = pWeapon;
		}
		else
		{
			bRet = false;
		}
	}

	return bRet;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTriage_Player::ApplyBattery(float powerMultiplier)
{
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Returns whether the weapon passed in would occupy a slot already occupied by the carrier
// Input  : *pWeapon - weapon to test for
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTriage_Player::Weapon_SlotOccupied(CBaseCombatWeapon *pWeapon)
{
	bool bRet = BaseClass::Weapon_SlotOccupied(pWeapon);

	if (!bRet)
	{
		CBaseTriageCombatWeapon* pTriageWeapon = ToBaseTriageCombatWeapon( pWeapon );
		if (!pTriageWeapon)
			return false;

		CBaseTriageCombatWeapon* pPlayerTriageWeapon = NULL;

		for (int i = 0; i < WeaponCount(); i++)
		{
			if (!GetWeapon(i))
				continue;

			pPlayerTriageWeapon = ToBaseTriageCombatWeapon(GetWeapon(i));

			if (pPlayerTriageWeapon && 
				pPlayerTriageWeapon->GetWeaponSlot() == pTriageWeapon->GetWeaponSlot())
			{
				return true;
			}
		}
	}

	return bRet;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTriage_Player::StartSprinting(void)
{
	BaseClass::StartSprinting();

	if (GetActiveWeapon())
	{
		CBaseTriageCombatWeapon* pWeapon = ToBaseTriageCombatWeapon(GetActiveWeapon());

		if (!pWeapon || !pWeapon->HasIronsights() || !pWeapon->IsIronsighted())
			return;

		pWeapon->DisableIronsights();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTriage_Player::StopSprinting(void)
{
	BaseClass::StopSprinting();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTriage_Player::PreThink(void)
{
	m_TriageLocal.m_bBumpWeapon = false;
	m_TriageLocal.m_hBumpWeapon = NULL;

	BaseClass::PreThink();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTriage_Player::PostThink(void)
{
	BaseClass::PostThink();

	// Regenerate heath
	if (IsAlive() && GetHealth() < GetMaxHealth() && (sv_regeneration.GetInt() == 1))
	{
		m_bFadeDamageRemoved = false;

		// Calculate opacity based on the player's health.
		int alpha = (int)RemapValClamped(GetHealth(), 0, GetMaxHealth(), sv_player_dmg_alpha_min.GetFloat(), sv_player_dmg_alpha_max.GetFloat());

		// Color to overlay on the screen while the player is taking damage
		color32 hurtScreenOverlay = { 255, 0, 0, 255 - alpha };

		if (gpGlobals->curtime > m_fTimeLastHurt + sv_regeneration_wait_time.GetFloat())
		{
			//Regenerate based on rate
			if (gpGlobals->curtime > m_fTimeNextRegen)
			{
				TakeHealth(sv_regeneration_health.GetFloat(), DMG_GENERIC);

				m_fTimeNextRegen = gpGlobals->curtime + sv_regeneration_rate.GetFloat();
			}
		}
		else
		{
			// UTIL_ScreenFade(this, hurtScreenOverlay, 1.0f, 0.1f, FFADE_IN | FFADE_PURGE | FFADE_STAYOUT);
		}

		// Fade the screen according to
		UTIL_ScreenFade(this, hurtScreenOverlay, 0.1f, 0.1f, FFADE_PURGE | FFADE_STAYOUT);
	}
	else if (!m_bFadeDamageRemoved && GetHealth() >= GetMaxHealth())
	{
		// Remove any screen fade. 
		color32 noFade = { 255, 255, 255, 0 };

		UTIL_ScreenFade(this, noFade, 0.1f, 0.1f, FFADE_PURGE);

		m_bFadeDamageRemoved = true;
	}

	// DevMsg("Player health: %d\n", m_iHealth);
}