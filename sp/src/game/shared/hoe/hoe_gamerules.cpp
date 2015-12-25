//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The Heart of Evil game rules, such as the relationship tables and ammo
//			damage cvars.
//
//=============================================================================

#include "cbase.h"
#include "hoe_gamerules.h"
#include "ammodef.h"
#include "hoe_shareddefs.h"

#ifdef CLIENT_DLL

#else
	#include "player.h"
	#include "game.h"
	#include "gamerules.h"
	#include "teamplay_gamerules.h"
	#include "hoe_player.h"
	#include "voice_gamemgr.h"
	#include "globalstate.h"
	#include "ai_basenpc.h"
	#include "weapon_physcannon.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


REGISTER_GAMERULES_CLASS( CHeartofEvil );

BEGIN_NETWORK_TABLE_NOBASE(CHeartofEvil, DT_HoeGameRules)
	#ifdef CLIENT_DLL
		// RecvPropBool( RECVINFO( m_bMegaPhysgun ) ),
	#else
		// SendPropBool( SENDINFO( m_bMegaPhysgun ) ),
	#endif
END_NETWORK_TABLE()


LINK_ENTITY_TO_CLASS(hoe_gamerules, CHeartofEvilProxy);
IMPLEMENT_NETWORKCLASS_ALIASED(HeartofEvilProxy, DT_HeartofEvilProxy)


#ifdef CLIENT_DLL
	void RecvProxy_HoeGameRules( const RecvProp *pProp, void **pOut, void *pData, int objectID )
	{
		CHeartofEvil *pRules = HoeGameRules();
		Assert( pRules );
		*pOut = pRules;
	}

	BEGIN_RECV_TABLE( CHeartofEvilProxy, DT_HeartofEvilProxy )
		RecvPropDataTable( "hoe_gamerules_data", 0, 0, &REFERENCE_RECV_TABLE( DT_HoeGameRules ), RecvProxy_HoeGameRules )
	END_RECV_TABLE()
#else
	void* SendProxy_HoeGameRules( const SendProp *pProp, const void *pStructBase, const void *pData, CSendProxyRecipients *pRecipients, int objectID )
	{
		CHeartofEvil *pRules = HoeGameRules();
		Assert( pRules );
		pRecipients->SetAllRecipients();
		return pRules;
	}

	BEGIN_SEND_TABLE(CHeartofEvilProxy, DT_HeartofEvilProxy)
		SendPropDataTable( "hoe_gamerules_data", 0, &REFERENCE_SEND_TABLE( DT_HoeGameRules ), SendProxy_HoeGameRules )
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


#if defined ( HOE_DLL ) || defined ( HOE_CLIENT_DLL )

// Pistol Whip
ConVar	sk_plr_pistolwhip("sk_plr_pistolwhip", "0", FCVAR_REPLICATED);

// Chainsaw (every tenth of a second, doubled when you press primary fire)
ConVar	sk_chainsaw_dmg("sk_chainsaw_dmg", "0", FCVAR_REPLICATED);
ConVar	sk_max_chainsaw("sk_max_chainsaw", "100", FCVAR_REPLICATED);

// M203 grenade (m79)
ConVar	sk_plr_9mmAR_grenade("sk_plr_9mmAR_grenade", "0", FCVAR_REPLICATED);
ConVar	sk_npc_9mmAR_grenade("sk_npc_9mmAR_grenade", "0", FCVAR_REPLICATED);
ConVar	sk_max_9mmAR_grenade("sk_max_9mmAR_grenade", "0", FCVAR_REPLICATED);

// 12mm (huey)
ConVar	sk_npc_12mm_bullet("sk_npc_12mm_bullet", "0", FCVAR_REPLICATED);

// 5.56 mm round (m16)
ConVar	sk_plr_m16_bullet("sk_plr_m16_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_npc_m16_bullet("sk_npc_m16_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_max_m16_bullet("sk_max_m16_bullet", "0", FCVAR_REPLICATED);

// 7.62 x 51 mm NATO round (m60)
ConVar	sk_plr_762_bullet("sk_plr_762_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_npc_762_bullet("sk_npc_762_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_max_762_bullet("sk_max_762_bullet", "0", FCVAR_REPLICATED);

// 7.62 x 51 mm NATO round (m21) A headshot, or a chest shot on lower skill levels, will kill
ConVar	sk_plr_m21_bullet("sk_plr_m21_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_npc_m21_bullet("sk_npc_m21_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_max_m21_bullet("sk_max_m21_bullet", "0", FCVAR_REPLICATED);

// 7.62 x 39 mm M1943 round (AK47)
ConVar	sk_plr_762x39_bullet("sk_plr_762x39_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_npc_762x39_bullet("sk_npc_762x39_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_max_762x39_bullet("sk_max_762x39_bullet", "0", FCVAR_REPLICATED);

// 11.43 mm round (Colt 1911A1)
ConVar	sk_plr_1143_bullet("sk_plr_1143_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_npc_1143_bullet("sk_npc_1143_bullet", "0", FCVAR_REPLICATED);
ConVar	sk_max_1143_bullet("sk_max_1143_bullet", "0", FCVAR_REPLICATED);

// Shotgun pellet, buckshot fires 6, elephantshot fires 30
ConVar	sk_plr_dmg_elephantshot("sk_plr_dmg_elephantshot", "0", FCVAR_REPLICATED);
ConVar	sk_max_elephantshot("sk_max_elephantshot", "0", FCVAR_REPLICATED);


// Tripmine
ConVar	sk_plr_dmg_tripmine("sk_plr_dmg_tripmine", "0", FCVAR_REPLICATED);
ConVar	sk_npc_dmg_tripmine("sk_npc_dmg_tripmine", "0", FCVAR_REPLICATED);
ConVar	sk_max_tripmine("sk_max_tripmine", "0", FCVAR_REPLICATED);

#endif // defined ( HOE_DLL ) || defined ( HOE_CLIENT_DLL )


#ifdef CLIENT_DLL //{


#else //}{

	extern bool		g_fGameOver;

	
	//-----------------------------------------------------------------------------
	// Purpose:
	// Input  :
	// Output :
	//-----------------------------------------------------------------------------
	CHeartofEvil::CHeartofEvil() : CHalfLife2()
	{
	}

	//-----------------------------------------------------------------------------
	// Purpose: called each time a player uses a "cmd" command
	// Input  : *pEdict - the player who issued the command
	//			Use engine.Cmd_Argv,  engine.Cmd_Argv, and engine.Cmd_Argc to get 
	//			pointers the character string command.
	//-----------------------------------------------------------------------------
	bool CHeartofEvil::ClientCommand(CBaseEntity *pEdict, const CCommand &args)
	{
		if( BaseClass::ClientCommand( pEdict, args ) )
			return true;

		CHoe_Player *pPlayer = (CHoe_Player *)pEdict;

		if ( pPlayer->ClientCommand( args ) )
			return true;

		return false;
	}

	//------------------------------------------------------------------------------
	// Purpose : Initialize all default class relationships
	// Input   :
	// Output  :
	//------------------------------------------------------------------------------
	void CHeartofEvil::InitDefaultAIRelationships(void)
	{
		BaseClass::InitDefaultAIRelationships();

		
		// ------------------------------------------------------------
		//	> CLASS_BULLSQUID
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_ANTLION,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_BARNACLE,			D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_CITIZEN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_CITIZEN_REBEL,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_COMBINE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_COMBINE_HUNTER,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_CONSCRIPT,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_HEADCRAB,			D_HT, 1);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_HOUNDEYE,			D_HT, 1);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_MANHACK,			D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_METROPOLICE,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_MILITARY,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_STALKER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_VORTIGAUNT,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_ZOMBIE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_BULLSQUID,			CLASS_HACKED_ROLLERMINE,D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_HOUNDEYE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_ANTLION,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_BULLSQUID,		D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_CITIZEN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_CITIZEN_REBEL,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_COMBINE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_COMBINE_HUNTER,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_CONSCRIPT,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_METROPOLICE,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_MILITARY,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_VORTIGAUNT,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HOUNDEYE,			CLASS_HACKED_ROLLERMINE,D_HT, 0);


		// ------------------------------------------------------------
		//	> CLASS_MACHINE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_MACHINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_HUMAN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_HUMAN_MILITARY,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_ALIEN_MILITARY,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,		CLASS_ALIEN_MONSTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_ALIEN_PREY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_ALIEN_PREDATOR,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_PLAYER_BIOWEAPON,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_MACHINE,				CLASS_ALIEN_BIOWEAPON,	D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_HUMAN_PASSIVE
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_PLAYER,			D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_PLAYER_ALLY,		D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_PLAYER_ALLY_VITAL,D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_MACHINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_HUMAN_PASSIVE,	D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_HUMAN_MILITARY,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_ALIEN_MILITARY,	D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_ALIEN_MONSTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_ALIEN_PREY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_ALIEN_PREDATOR,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_PLAYER_BIOWEAPON, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_PASSIVE,		CLASS_ALIEN_BIOWEAPON,	D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_HUMAN_MILITARY
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_MACHINE,			D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_HUMAN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_HUMAN_MILITARY,	D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_ALIEN_MILITARY,	D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_ALIEN_MONSTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_ALIEN_PREY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_ALIEN_PREDATOR,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_PLAYER_BIOWEAPON, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_HUMAN_MILITARY,		CLASS_ALIEN_BIOWEAPON,	D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_ALIEN_MILITARY
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_MACHINE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_HUMAN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_HUMAN_MILITARY,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_ALIEN_MILITARY,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_ALIEN_MONSTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_ALIEN_PREY,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_ALIEN_PREDATOR,	D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_PLAYER_BIOWEAPON, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MILITARY,		CLASS_ALIEN_BIOWEAPON,	D_NU, 0);


		// ------------------------------------------------------------
		//	> CLASS_ALIEN_MONSTER
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_MACHINE,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_HUMAN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_HUMAN_MILITARY,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_ALIEN_MILITARY,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_ALIEN_MONSTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_ALIEN_PREY,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_ALIEN_PREDATOR,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_PLAYER_BIOWEAPON, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_MONSTER,		CLASS_ALIEN_BIOWEAPON,	D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_ALIEN_PREY
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_MACHINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_HUMAN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_HUMAN_MILITARY,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_ALIEN_MILITARY,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_ALIEN_MONSTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_ALIEN_PREY,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_ALIEN_PREDATOR,	D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_PLAYER_BIOWEAPON, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREY,		CLASS_ALIEN_BIOWEAPON,	D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_ALIEN_PREDATOR
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_MACHINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_HUMAN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_HUMAN_MILITARY,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_ALIEN_MILITARY,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_ALIEN_MONSTER,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_ALIEN_PREY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_ALIEN_PREDATOR,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_PLAYER_BIOWEAPON, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_PREDATOR,		CLASS_ALIEN_BIOWEAPON,	D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_INSECT
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_PLAYER,			D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_PLAYER_ALLY,		D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_PLAYER_ALLY_VITAL,D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_MACHINE,			D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_HUMAN_PASSIVE,	D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_HUMAN_MILITARY,	D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_ALIEN_MILITARY,	D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_ALIEN_MONSTER,	D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_ALIEN_PREY,		D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_ALIEN_PREDATOR,	D_FR, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_PLAYER_BIOWEAPON, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_INSECT,		CLASS_ALIEN_BIOWEAPON,	D_NU, 0);

		// ------------------------------------------------------------
		//	> CLASS_PLAYER_BIOWEAPON
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_MACHINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_HUMAN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_HUMAN_MILITARY,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_ALIEN_MILITARY,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_ALIEN_MONSTER,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_ALIEN_PREY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_ALIEN_PREDATOR,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_PLAYER_BIOWEAPON, D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_PLAYER_BIOWEAPON,		CLASS_ALIEN_BIOWEAPON,	D_HT, 0);

		// ------------------------------------------------------------
		//	> CLASS_ALIEN_BIOWEAPON
		// ------------------------------------------------------------
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_NONE,				D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_PLAYER,			D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_BARNACLE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_BULLSEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_BULLSQUID,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_CITIZEN_PASSIVE,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_CITIZEN_REBEL,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_COMBINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_COMBINE_GUNSHIP,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_COMBINE_HUNTER,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_CONSCRIPT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_FLARE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_HEADCRAB,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_HOUNDEYE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_MANHACK,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_METROPOLICE,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_MILITARY,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_MISSILE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_SCANNER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_STALKER,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_VORTIGAUNT,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_ZOMBIE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_PROTOSNIPER,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_ANTLION,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_EARTH_FAUNA,		D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_PLAYER_ALLY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_PLAYER_ALLY_VITAL,D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_HACKED_ROLLERMINE,D_NU, 0);

		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_MACHINE,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_HUMAN_PASSIVE,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_HUMAN_MILITARY,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_ALIEN_MILITARY,	D_LI, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_ALIEN_MONSTER,	D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_ALIEN_PREY,		D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_ALIEN_PREDATOR,	D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_INSECT,			D_NU, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_PLAYER_BIOWEAPON, D_HT, 0);
		CBaseCombatCharacter::SetDefaultRelationship(CLASS_ALIEN_BIOWEAPON,		CLASS_ALIEN_BIOWEAPON,	D_NU, 0);
	}

	//------------------------------------------------------------------------------
	// Purpose : Return classify text for classify type
	// Input   :
	// Output  :
	//------------------------------------------------------------------------------
	const char* CHeartofEvil::AIClassText(int classType)
	{
		switch (classType)
		{
		//
		// HL2 legacy classes.
		//
		case CLASS_BULLSQUID:			return "CLASS_BULLSQUID";	
		case CLASS_HOUNDEYE:			return "CLASS_HOUNDEYE";

		//
		// HL1 legacy classes.
		//
		case CLASS_MACHINE:				return "CLASS_MACHINE";
		case CLASS_HUMAN_PASSIVE:		return "CLASS_HUMAN_PASSIVE";
		case CLASS_HUMAN_MILITARY:		return "CLASS_HUMAN_MILITARY";
		case CLASS_ALIEN_MILITARY:		return "CLASS_ALIEN_MILITARY";
		case CLASS_ALIEN_MONSTER:		return "CLASS_ALIEN_MONSTER";
		case CLASS_ALIEN_PREY:			return "CLASS_ALIEN_PREY";
		case CLASS_ALIEN_PREDATOR:		return "CLASS_ALIEN_PREDATOR";
		case CLASS_INSECT:				return "CLASS_INSECT";
		case CLASS_PLAYER_BIOWEAPON:	return "CLASS_PLAYER_BIOWEAPON";
		case CLASS_ALIEN_BIOWEAPON:		return "CLASS_ALIEN_BIOWEAPON";
		default:
			return BaseClass::AIClassText(classType);
		}
	}

#endif //} !CLIENT_DLL

// ------------------------------------------------------------------------------------ //
// Global functions.
// ------------------------------------------------------------------------------------ //


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

		def.AddAmmoType("Buckshot",			DMG_BULLET | DMG_BUCKSHOT,	TRACER_LINE,			"sk_plr_dmg_buckshot",		"sk_npc_dmg_buckshot",		"sk_max_buckshot",			BULLET_IMPULSE(400, 1200), 0 );
		def.AddAmmoType("Elephantshot",		DMG_BULLET | DMG_BUCKSHOT,	TRACER_LINE,			"sk_plr_dmg_elephantshot",	"sk_npc_dmg_buckshot",		"sk_max_elephantshot",		BULLET_IMPULSE(400, 1200), 0 );
		def.AddAmmoType("7_62x39mm_M1943",	DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_762x39_bullet",		"sk_npc_762x39_bullet",		"sk_max_762x39_bullet",		BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType("Gas",				DMG_SLASH,					TRACER_NONE,			"sk_chainsaw_dmg",			"sk_chainsaw_dmg",			"sk_max_chainsaw",			0, 0);
		def.AddAmmoType("11_43mm",			DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_1143_bullet",		"sk_npc_1143_bullet",		"sk_max_1143_bullet",		BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType("grenade",			DMG_BURN,					TRACER_NONE,			"sk_plr_dmg_grenade",		"sk_npc_dmg_grenade",		"sk_max_grenade",			0, 0);
		def.AddAmmoType("Letter",			DMG_BULLET,					TRACER_LINE,			0, 0, 10, 0, 0 );
		def.AddAmmoType("5_56mm",			DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_m16_bullet",		"sk_npc_m16_bullet",		"sk_max_m16_bullet",		BULLET_IMPULSE(200, 1225), 0 );
		def.AddAmmoType("m21",				DMG_BULLET | DMG_SNIPER,	TRACER_NONE,			"sk_plr_m21_bullet",		"sk_npc_m21_bullet",		"sk_max_m21_bullet",		BULLET_IMPULSE(650, 6000), 0 );
		def.AddAmmoType("7_62mm",			DMG_BULLET,					TRACER_LINE_AND_WHIZ,	"sk_plr_762_bullet",		"sk_npc_762_bullet",		"sk_max_762_bullet",		BULLET_IMPULSE(200, 1225), 0);
		def.AddAmmoType("AR_Grenade",		DMG_BURN,					TRACER_NONE,			"sk_plr_9mmAR_grenade",		"sk_npc_9mmAR_grenade",		"sk_max_9mmAR_grenade",		0, 0 );
		def.AddAmmoType("RPG_Round",		DMG_BURN,					TRACER_NONE,			"sk_plr_dmg_rpg_round",		"sk_npc_dmg_rpg_round",		"sk_max_rpg_round",			0, 0 );
		def.AddAmmoType("Tripmine",			DMG_BURN,					TRACER_NONE,			"sk_plr_dmg_tripmine",		"sk_npc_dmg_tripmine",		"sk_max_tripmine",		0, 0);
	}

	return &def;
}
