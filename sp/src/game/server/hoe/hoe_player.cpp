//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for Hoe.
//
//=============================================================================//

#include "cbase.h"
#include "hoe_player.h"
#include "globalstate.h"
#include "game.h"
#include "gamerules.h"
#include "trains.h"
#include "hoe_basecombatweapon_shared.h"
#include "vcollide_parse.h"
#include "in_buttons.h"
#include "ai_interactions.h"
#include "ai_squad.h"
#include "igamemovement.h"
#include "ai_hull.h"
#include "hoe_shareddefs.h"
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
#include "hoe_gamerules.h"
#include "prop_combine_ball.h"
#include "datacache/imdlcache.h"
#include "eventqueue.h"
#include "gamestats.h"
#include "filters.h"
#include "tier0/icommandline.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern int gEvilImpulse101;

LINK_ENTITY_TO_CLASS( player, CHoe_Player );

// Global Savedata for HL2 player
BEGIN_DATADESC( CHoe_Player )

	DEFINE_EMBEDDED( m_HoeLocal ),

END_DATADESC()

CHoe_Player::CHoe_Player() : CHL2_Player()
{
}

CHoe_Player::~CHoe_Player(void)
{
}


IMPLEMENT_SERVERCLASS_ST(CHoe_Player, DT_Hoe_Player)
	SendPropDataTable(SENDINFO_DT(m_HoeLocal), &REFERENCE_SEND_TABLE(DT_HoeLocal), SendProxy_SendLocalDataTable),
END_SEND_TABLE()

bool CHoe_Player::ClientCommand(const CCommand &args)
{
	if (stricmp(args[0], "toggle_ironsight") == 0)
	{
		CHoe_BaseCombatWeapon *pWeapon = ToHoeBaseCombatWeapon( GetActiveWeapon() );
		if (pWeapon != NULL)
			pWeapon->ToggleIronsights();

		return true;
	}

	return BaseClass::ClientCommand(args);
}


//=========================================================
//=========================================================
void CHoe_Player::CheatImpulseCommands(int iImpulse)
{
	switch (iImpulse)
	{
	case 101:
		gEvilImpulse101 = true;

		EquipSuit();

		// Give the player everything!
		CBaseCombatCharacter::GiveAmmo(255, "Buckshot");
		CBaseCombatCharacter::GiveAmmo(255, "Elephantshot");
		CBaseCombatCharacter::GiveAmmo(255, "7_62x39mm_M1943");
		CBaseCombatCharacter::GiveAmmo(255, "Gas");
		CBaseCombatCharacter::GiveAmmo(255, "11_43mm");
		CBaseCombatCharacter::GiveAmmo(255, "grenade");
		CBaseCombatCharacter::GiveAmmo(255, "Letter");
		CBaseCombatCharacter::GiveAmmo(255, "5_56mm");
		CBaseCombatCharacter::GiveAmmo(255, "m21");
		CBaseCombatCharacter::GiveAmmo(255, "7_62mm");
		CBaseCombatCharacter::GiveAmmo(255, "AR_Grenade");
		CBaseCombatCharacter::GiveAmmo(255, "RPG_Round");
	
		GiveNamedItem("weapon_ak47");
		GiveNamedItem("weapon_chainsaw");
		GiveNamedItem("weapon_colt1911A1");
		GiveNamedItem("weapon_handgrenade");
		GiveNamedItem("weapon_m16");
		GiveNamedItem("weapon_m21");
		GiveNamedItem("weapon_m60");
		GiveNamedItem("weapon_m79");
		GiveNamedItem("weapon_machete");
		GiveNamedItem("weapon_rpg7");
		GiveNamedItem("weapon_870");
		GiveNamedItem("weapon_snark");
		GiveNamedItem("weapon_tripmine");

		if (GetHealth() < 100)
		{
			TakeHealth(25, DMG_GENERIC);
		}

		gEvilImpulse101 = false;

		break;

	default:
		BaseClass::CheatImpulseCommands(iImpulse);
		break;
	}
}