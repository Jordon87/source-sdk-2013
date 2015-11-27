//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The Triage game rules, such as the relationship tables and ammo
//			damage cvars.
//
//=============================================================================

#include "cbase.h"
#include "triage_gamerules.h"
#include "ammodef.h"
#include "hl2_shareddefs.h"

#ifdef CLIENT_DLL

#else
	#include "player.h"
	#include "game.h"
	#include "gamerules.h"
	#include "teamplay_gamerules.h"
	#include "triage_player.h"
	#include "voice_gamemgr.h"
	#include "globalstate.h"
	#include "ai_basenpc.h"
	#include "weapon_physcannon.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


REGISTER_GAMERULES_CLASS( CTriage );

BEGIN_NETWORK_TABLE_NOBASE( CTriage, DT_TriageGameRules )
	#ifdef CLIENT_DLL
	#else
	#endif
END_NETWORK_TABLE()


LINK_ENTITY_TO_CLASS( triage_gamerules, CTriageProxy );
IMPLEMENT_NETWORKCLASS_ALIASED( TriageProxy, DT_TriageProxy )


#ifdef CLIENT_DLL
	void RecvProxy_TriageGameRules( const RecvProp *pProp, void **pOut, void *pData, int objectID )
	{
		CTriage *pRules = TriageGameRules();
		Assert( pRules );
		*pOut = pRules;
	}

	BEGIN_RECV_TABLE( CTriageProxy, DT_TriageProxy )
		RecvPropDataTable( "triage_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE( DT_TriageGameRules ), RecvProxy_TriageGameRules )
	END_RECV_TABLE()
#else
	void* SendProxy_TriageGameRules( const SendProp *pProp, const void *pStructBase, const void *pData, CSendProxyRecipients *pRecipients, int objectID )
	{
		CTriage *pRules = TriageGameRules();
		Assert( pRules );
		pRecipients->SetAllRecipients();
		return pRules;
	}

	BEGIN_SEND_TABLE( CTriageProxy, DT_TriageProxy )
		SendPropDataTable( "triage_gamerules_data", 0, &REFERENCE_SEND_TABLE( DT_TriageGameRules ), SendProxy_TriageGameRules )
	END_SEND_TABLE()
#endif

extern ConVar  physcannon_mega_enabled;

// Controls the application of the robus radius damage model.
extern ConVar	sv_robust_explosions;

// Damage scale for damage inflicted by the player on each skill level.
extern ConVar	sk_dmg_inflict_scale1;
extern ConVar	sk_dmg_inflict_scale2;
extern ConVar	sk_dmg_inflict_scale3;

// Damage scale for damage taken by the player on each skill level.
extern ConVar	sk_dmg_take_scale1;
extern ConVar	sk_dmg_take_scale2;
extern ConVar	sk_dmg_take_scale3;

extern ConVar	sk_allow_autoaim;

// Autoaim scale
extern ConVar	sk_autoaim_scale1;
extern ConVar	sk_autoaim_scale2;
//ConVar	sk_autoaim_scale3( "sk_autoaim_scale3", "0.0", FCVAR_REPLICATED ); NOT CURRENTLY OFFERED ON SKILL 3

// Quantity scale for ammo received by the player.
extern ConVar	sk_ammo_qty_scale1;
extern ConVar	sk_ammo_qty_scale2;
extern ConVar	sk_ammo_qty_scale3;

extern ConVar	sk_plr_health_drop_time;
extern ConVar	sk_plr_grenade_drop_time;

extern ConVar	sk_plr_dmg_ar2;
extern ConVar	sk_npc_dmg_ar2;
extern ConVar	sk_max_ar2;
extern ConVar	sk_max_ar2_altfire;

extern ConVar	sk_plr_dmg_alyxgun;
extern ConVar	sk_npc_dmg_alyxgun;
extern ConVar	sk_max_alyxgun;

extern ConVar	sk_plr_dmg_pistol;
extern ConVar	sk_npc_dmg_pistol;
extern ConVar	sk_max_pistol;

extern ConVar	sk_plr_dmg_smg1;
extern ConVar	sk_npc_dmg_smg1;
extern ConVar	sk_max_smg1;

extern ConVar	sk_plr_dmg_buckshot;
extern ConVar	sk_npc_dmg_buckshot;
extern ConVar	sk_max_buckshot;
extern ConVar	sk_plr_num_shotgun_pellets;

extern ConVar	sk_plr_dmg_rpg_round;
extern ConVar	sk_npc_dmg_rpg_round;
extern ConVar	sk_max_rpg_round;

extern ConVar	sk_plr_dmg_sniper_round;
extern ConVar	sk_npc_dmg_sniper_round;
extern ConVar	sk_max_sniper_round;

extern ConVar	sk_plr_dmg_grenade;
extern ConVar	sk_npc_dmg_grenade;
extern ConVar	sk_max_grenade;

extern ConVar	sk_max_hopwire;
extern ConVar	sk_max_striderbuster;

extern ConVar	sk_plr_dmg_smg1_grenade;
extern ConVar	sk_npc_dmg_smg1_grenade;
extern ConVar	sk_max_smg1_grenade;

extern ConVar	sk_plr_dmg_357;
extern ConVar	sk_npc_dmg_357;
extern ConVar	sk_max_357;

extern ConVar	sk_plr_dmg_crossbow;
extern ConVar	sk_npc_dmg_crossbow;
extern ConVar	sk_max_crossbow;

extern ConVar	sk_dmg_sniper_penetrate_plr;
extern ConVar	sk_dmg_sniper_penetrate_npc;

extern ConVar	sk_plr_dmg_airboat;
extern ConVar	sk_npc_dmg_airboat;

extern ConVar	sk_max_gauss_round;

// Gunship & Dropship cannons
extern ConVar	sk_npc_dmg_gunship;
extern ConVar	sk_npc_dmg_gunship_to_plr;

//==================================================================
//
// TRIAGE CVARS
//
//==================================================================

// ai

// higher values are less accurate
// ai accuracy spread scale
ConVar sk_ai_spread_scale1("sk_ai_spread_scale1", "0", FCVAR_REPLICATED);
ConVar sk_ai_spread_scale2("sk_ai_spread_scale2", "0", FCVAR_REPLICATED);
ConVar sk_ai_spread_scale3("sk_ai_spread_scale3", "0", FCVAR_REPLICATED);

ConVar sk_ai_close_burstlength("sk_ai_close_burstlength", "0", FCVAR_REPLICATED);
ConVar sk_ai_plrdying_burstlength("sk_ai_plrdying_burstlength", "0", FCVAR_REPLICATED);
ConVar sk_ai_plrdying_spread("sk_ai_plrdying_spread", "0", FCVAR_REPLICATED);
ConVar sk_ai_close_spread("sk_ai_close_spread", "0", FCVAR_REPLICATED);
ConVar sk_ai_close_dist("sk_ai_close_dist", "0", FCVAR_REPLICATED);


// npcs

ConVar  sk_eviladvisor_health("sk_eviladvisor_health", "0");

ConVar  sk_loyalist_health("sk_loyalist_health", "0");

ConVar	sk_npc_metropolice_armor_pistol("sk_npc_metropolice_armor_pistol", "0", FCVAR_REPLICATED);
ConVar	sk_npc_metropolice_armor_buckshot("sk_npc_metropolice_armor_buckshot", "0", FCVAR_REPLICATED);
ConVar	sk_npc_metropolice_armor_birdshot("sk_npc_metropolice_armor_birdshot", "0", FCVAR_REPLICATED);

// player regen
ConVar sk_plr_regen("sk_plr_regen", "0", FCVAR_REPLICATED); // how much i regen in 1/10 of a second
ConVar sk_plr_can_regen_timer("sk_plr_can_regen_timer", "0", FCVAR_REPLICATED); // how much time before my health begins to regen

// Quantity scale for ammo received by the player.

ConVar	sk_plr_dmg_AK47("sk_plr_dmg_AK47", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_AK47("sk_npc_dmg_AK47", "0", FCVAR_REPLICATED);
ConVar	sk_max_AK47("sk_max_AK47", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_birdshot("sk_plr_dmg_birdshot", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_birdshot("sk_npc_dmg_birdshot", "0", FCVAR_REPLICATED);
ConVar	sk_max_birdshot("sk_max_birdshot", "0", FCVAR_REPLICATED);

ConVar	sk_plr_dmg_birdshotBlank("sk_plr_dmg_birdshotBlank", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_birdshotBlank("sk_npc_dmg_birdshotBlank", "0", FCVAR_REPLICATED);


//==================================================================

#ifdef CLIENT_DLL //{


#else //}{
	

	//-----------------------------------------------------------------------------
	// Purpose: called each time a player uses a "cmd" command
	// Input  : *pEdict - the player who issued the command
	//			Use engine.Cmd_Argv,  engine.Cmd_Argv, and engine.Cmd_Argc to get 
	//			pointers the character string command.
	//-----------------------------------------------------------------------------
	bool CTriage::ClientCommand( CBaseEntity *pEdict, const CCommand &args )
	{
		if( BaseClass::ClientCommand( pEdict, args ) )
			return true;

		CTriage_Player *pPlayer = (CTriage_Player *) pEdict;

		if ( pPlayer->ClientCommand( args ) )
			return true;

		return false;
	}


#endif//CLIENT_DLL

// ------------------------------------------------------------------------------------ //
// Global functions.
// ------------------------------------------------------------------------------------ //

#if defined ( TRIAGE_DLL ) || defined ( TRIAGE_CLIENT_DLL )

// shared ammo definition
// JAY: Trying to make a more physical bullet response
#define BULLET_MASS_GRAINS_TO_LB(grains)	(0.002285*(grains)/16.0f)
#define BULLET_MASS_GRAINS_TO_KG(grains)	lbs2kg(BULLET_MASS_GRAINS_TO_LB(grains))

// exaggerate all of the forces, but use real numbers to keep them consistent
#define BULLET_IMPULSE_EXAGGERATION			3.5
// convert a velocity in ft/sec and a mass in grains to an impulse in kg in/s
#define BULLET_IMPULSE(grains, ftpersec)	((ftpersec)*12*BULLET_MASS_GRAINS_TO_KG(grains)*BULLET_IMPULSE_EXAGGERATION)

CAmmoDef *GetAmmoDef()
{
	static CAmmoDef def;
	static bool bInitted = false;
	
	if ( !bInitted )
	{
		bInitted = true;

		def.AddAmmoType("AR2",				DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_dmg_ar2",			"sk_npc_dmg_ar2",			"sk_max_ar2",			BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType("AlyxGun",			DMG_BULLET,					TRACER_LINE,			"sk_plr_dmg_alyxgun",		"sk_npc_dmg_alyxgun",		"sk_max_alyxgun",		BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType("Pistol",			DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_dmg_pistol",		"sk_npc_dmg_pistol",		"sk_max_pistol",		BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType("SMG1",				DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_dmg_smg1",			"sk_npc_dmg_smg1",			"sk_max_smg1",			BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType("357",				DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_dmg_357",			"sk_npc_dmg_357",			"sk_max_357",			BULLET_IMPULSE(800, 5000), 0 );
		def.AddAmmoType("XBowBolt",			DMG_BULLET,					TRACER_LINE,			"sk_plr_dmg_crossbow",		"sk_npc_dmg_crossbow",		"sk_max_crossbow",		BULLET_IMPULSE(800, 8000), 0 );

		def.AddAmmoType("Buckshot",			DMG_BULLET | DMG_BUCKSHOT,	TRACER_LINE,			"sk_plr_dmg_buckshot",		"sk_npc_dmg_buckshot",		"sk_max_buckshot",		BULLET_IMPULSE(400, 1200), 0 );
		def.AddAmmoType("RPG_Round",		DMG_BURN,					TRACER_NONE,			"sk_plr_dmg_rpg_round",		"sk_npc_dmg_rpg_round",		"sk_max_rpg_round",		0, 0 );
		def.AddAmmoType("SMG1_Grenade",		DMG_BURN,					TRACER_NONE,			"sk_plr_dmg_smg1_grenade",	"sk_npc_dmg_smg1_grenade",	"sk_max_smg1_grenade",	0, 0 );
		def.AddAmmoType("SniperRound",		DMG_BULLET | DMG_SNIPER,	TRACER_NONE,			"sk_plr_dmg_sniper_round",	"sk_npc_dmg_sniper_round",	"sk_max_sniper_round",	BULLET_IMPULSE(650, 6000), 0 );
		def.AddAmmoType("SniperPenetratedRound", DMG_BULLET | DMG_SNIPER, TRACER_NONE,			"sk_dmg_sniper_penetrate_plr", "sk_dmg_sniper_penetrate_npc", "sk_max_sniper_round", BULLET_IMPULSE(150, 6000), 0 );
		def.AddAmmoType("Grenade",			DMG_BURN,					TRACER_NONE,			"sk_plr_dmg_grenade",		"sk_npc_dmg_grenade",		"sk_max_grenade",		0, 0);
		def.AddAmmoType("Thumper",			DMG_SONIC,					TRACER_NONE,			10, 10, 2, 0, 0 );
		def.AddAmmoType("Gravity",			DMG_CLUB,					TRACER_NONE,			0,	0, 8, 0, 0 );
//		def.AddAmmoType("Extinguisher",		DMG_BURN,					TRACER_NONE,			0,	0, 100, 0, 0 );
		def.AddAmmoType("Battery",			DMG_CLUB,					TRACER_NONE,			NULL, NULL, NULL, 0, 0 );
		def.AddAmmoType("GaussEnergy",		DMG_SHOCK,					TRACER_NONE,			"sk_jeep_gauss_damage",		"sk_jeep_gauss_damage", "sk_max_gauss_round", BULLET_IMPULSE(650, 8000), 0 ); // hit like a 10kg weight at 400 in/s
		def.AddAmmoType("CombineCannon",	DMG_BULLET,					TRACER_LINE,			"sk_npc_dmg_gunship_to_plr", "sk_npc_dmg_gunship", NULL, 1.5 * 750 * 12, 0 ); // hit like a 1.5kg weight at 750 ft/s
		def.AddAmmoType("AirboatGun",		DMG_AIRBOAT,				TRACER_LINE,			"sk_plr_dmg_airboat",		"sk_npc_dmg_airboat",		NULL,					BULLET_IMPULSE(10, 600), 0 );

		//=====================================================================
		// STRIDER MINIGUN DAMAGE - Pull up a chair and I'll tell you a tale.
		//
		// When we shipped Half-Life 2 in 2004, we were unaware of a bug in
		// CAmmoDef::NPCDamage() which was returning the MaxCarry field of
		// an ammotype as the amount of damage that should be done to a NPC
		// by that type of ammo. Thankfully, the bug only affected Ammo Types 
		// that DO NOT use ConVars to specify their parameters. As you can see,
		// all of the important ammotypes use ConVars, so the effect of the bug
		// was limited. The Strider Minigun was affected, though.
		//
		// According to my perforce Archeology, we intended to ship the Strider
		// Minigun ammo type to do 15 points of damage per shot, and we did. 
		// To achieve this we, unaware of the bug, set the Strider Minigun ammo 
		// type to have a maxcarry of 15, since our observation was that the 
		// number that was there before (8) was indeed the amount of damage being
		// done to NPC's at the time. So we changed the field that was incorrectly
		// being used as the NPC Damage field.
		//
		// The bug was fixed during Episode 1's development. The result of the 
		// bug fix was that the Strider was reduced to doing 5 points of damage
		// to NPC's, since 5 is the value that was being assigned as NPC damage
		// even though the code was returning 15 up to that point.
		//
		// Now as we go to ship Orange Box, we discover that the Striders in 
		// Half-Life 2 are hugely ineffective against citizens, causing big
		// problems in maps 12 and 13. 
		//
		// In order to restore balance to HL2 without upsetting the delicate 
		// balance of ep2_outland_12, I have chosen to build Episodic binaries
		// with 5 as the Strider->NPC damage, since that's the value that has
		// been in place for all of Episode 2's development. Half-Life 2 will
		// build with 15 as the Strider->NPC damage, which is how HL2 shipped
		// originally, only this time the 15 is located in the correct field
		// now that the AmmoDef code is behaving correctly.
		//
		//=====================================================================
#ifdef HL2_EPISODIC
		def.AddAmmoType("StriderMinigun",	DMG_BULLET,					TRACER_LINE,			5, 5, 15, 1.0 * 750 * 12, AMMO_FORCE_DROP_IF_CARRIED ); // hit like a 1.0kg weight at 750 ft/s
#else
		def.AddAmmoType("StriderMinigun",	DMG_BULLET,					TRACER_LINE,			5, 15,15, 1.0 * 750 * 12, AMMO_FORCE_DROP_IF_CARRIED ); // hit like a 1.0kg weight at 750 ft/s
#endif//HL2_EPISODIC

		def.AddAmmoType("StriderMinigunDirect",	DMG_BULLET,				TRACER_LINE,			2, 2, 15, 1.0 * 750 * 12, AMMO_FORCE_DROP_IF_CARRIED ); // hit like a 1.0kg weight at 750 ft/s
		def.AddAmmoType("HelicopterGun",	DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_npc_dmg_helicopter_to_plr", "sk_npc_dmg_helicopter",	"sk_max_smg1",	BULLET_IMPULSE(400, 1225), AMMO_FORCE_DROP_IF_CARRIED | AMMO_INTERPRET_PLRDAMAGE_AS_DAMAGE_TO_PLAYER );
		def.AddAmmoType("AR2AltFire",		DMG_DISSOLVE,				TRACER_NONE,			0, 0, "sk_max_ar2_altfire", 0, 0 );
		def.AddAmmoType("Grenade",			DMG_BURN,					TRACER_NONE,			"sk_plr_dmg_grenade",		"sk_npc_dmg_grenade",		"sk_max_grenade",		0, 0);
#ifdef HL2_EPISODIC
		def.AddAmmoType("Hopwire",			DMG_BLAST,					TRACER_NONE,			"sk_plr_dmg_grenade",		"sk_npc_dmg_grenade",		"sk_max_hopwire",		0, 0);
		def.AddAmmoType("CombineHeavyCannon",	DMG_BULLET,				TRACER_LINE,			40,	40, NULL, 10 * 750 * 12, AMMO_FORCE_DROP_IF_CARRIED ); // hit like a 10 kg weight at 750 ft/s
		def.AddAmmoType("ammo_proto1",			DMG_BULLET,				TRACER_LINE,			0, 0, 10, 0, 0 );
#endif // HL2_EPISODIC

		def.AddAmmoType("AK47",				DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_dmg_AK47",			"sk_npc_dmg_AK47",			"sk_max_AK47",			BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType("Birdshot",			DMG_BULLET | DMG_BUCKSHOT,	TRACER_LINE,			"sk_plr_dmg_birdshot",		"sk_npc_dmg_birdshot",		"sk_max_birdshot",		BULLET_IMPULSE(400, 1200), 0 );
		def.AddAmmoType("BirdshotBlank",	DMG_BULLET | DMG_BUCKSHOT,	TRACER_LINE,			"sk_plr_dmg_birdshotBlank",		"sk_npc_dmg_birdshotBlank",		"sk_max_birdshot",		BULLET_IMPULSE(400, 1200), 0 );
	}

	return &def;
}

#endif // defined ( TRIAGE_DLL ) || defined ( TRIAGE_CLIENT_DLL )