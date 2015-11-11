//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for 1187.
//
//=============================================================================//

#include "cbase.h"
#include "1187_player.h"
#include "globalstate.h"
#include "game.h"
#include "gamerules.h"
#include "trains.h"
#include "1187_baseviewmodel_shared.h"
#include "1187_basecombatweapon_shared.h"
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
#include "1187_gamerules.h"
#include "prop_combine_ball.h"
#include "datacache/imdlcache.h"
#include "eventqueue.h"
#include "gamestats.h"
#include "filters.h"
#include "tier0/icommandline.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define PLAYER_MODEL		"models/humans/group01/male_09.mdl"

extern int gEvilImpulse101;

LINK_ENTITY_TO_CLASS( player, C1187_Player );

// Global Savedata for 1187 player
BEGIN_DATADESC(C1187_Player)
	DEFINE_EMBEDDED( m_1187Local ),

	DEFINE_FIELD(m_angEyeAngles, FIELD_VECTOR),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(C1187_Player, DT_1187_Player)
	SendPropDataTable(SENDINFO_DT(m_1187Local), &REFERENCE_SEND_TABLE(DT_1187Local), SendProxy_SendLocalDataTable),

	SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 0), 11, SPROP_CHANGES_OFTEN),
	SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 1), 11, SPROP_CHANGES_OFTEN),

END_SEND_TABLE()

C1187_Player::C1187_Player() : m_PlayerAnimState(this)
{
	m_angEyeAngles.Init();
}

C1187_Player::~C1187_Player(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel(PLAYER_MODEL);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::Spawn(void)
{
	BaseClass::Spawn();

	SetModel(PLAYER_MODEL);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::PostThink(void)
{
	BaseClass::PostThink();

	m_PlayerAnimState.Update();

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles(angles);
}


// Set the activity based on an event or current state
void C1187_Player::SetAnimation(PLAYER_ANIM playerAnim)
{
	int animDesired;

	float speed;

	speed = GetAbsVelocity().Length2D();


	// bool bRunning = true;

	//Revisit!
	/*	if ( ( m_nButtons & ( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT ) ) )
	{
	if ( speed > 1.0f && speed < hl2_normspeed.GetFloat() - 20.0f )
	{
	bRunning = false;
	}
	}*/

	if (GetFlags() & (FL_FROZEN | FL_ATCONTROLS))
	{
		speed = 0;
		playerAnim = PLAYER_IDLE;
	}

	Activity idealActivity = ACT_RUN;

	// This could stand to be redone. Why is playerAnim abstracted from activity? (sjb)
	if (playerAnim == PLAYER_JUMP)
	{
		idealActivity = ACT_JUMP;
	}
	else if (playerAnim == PLAYER_DIE)
	{
		if (m_lifeState == LIFE_ALIVE)
		{
			return;
		}
	}
	else if (playerAnim == PLAYER_ATTACK1)
	{
		if (GetActivity() == ACT_HOVER ||
			GetActivity() == ACT_SWIM ||
			GetActivity() == ACT_HOP ||
			GetActivity() == ACT_LEAP ||
			GetActivity() == ACT_DIESIMPLE)
		{
			idealActivity = GetActivity();
		}
		else
		{
			idealActivity = ACT_GESTURE_RANGE_ATTACK1;
		}
	}
	else if (playerAnim == PLAYER_RELOAD)
	{
		idealActivity = ACT_GESTURE_RELOAD;
	}
	else if (playerAnim == PLAYER_IDLE || playerAnim == PLAYER_WALK)
	{
		if (!(GetFlags() & FL_ONGROUND) && GetActivity() == ACT_JUMP)	// Still jumping
		{
			idealActivity = GetActivity();
		}
		/*
		else if ( GetWaterLevel() > 1 )
		{
		if ( speed == 0 )
		idealActivity = ACT_HOVER;
		else
		idealActivity = ACT_SWIM;
		}
		*/
		else
		{
			if (GetFlags() & FL_DUCKING)
			{
				if (speed > 0)
				{
					idealActivity = ACT_WALK_CROUCH;
				}
				else
				{
					idealActivity = ACT_COVER_LOW;
				}
			}
			else
			{
				if (speed > 0)
				{
					/*
					if ( bRunning == false )
					{
					idealActivity = ACT_WALK;
					}
					else
					*/
					{
						idealActivity = ACT_RUN;
					}
				}
				else
				{
					idealActivity = ACT_IDLE;
				}
			}
		}

		// idealActivity = TranslateTeamActivity(idealActivity);
	}

	if (idealActivity == ACT_GESTURE_RANGE_ATTACK1)
	{
		RestartGesture(Weapon_TranslateActivity(idealActivity));

		// FIXME: this seems a bit wacked
		Weapon_SetActivity(Weapon_TranslateActivity(ACT_RANGE_ATTACK1), 0);

		return;
	}
	else if (idealActivity == ACT_GESTURE_RELOAD)
	{
		RestartGesture(Weapon_TranslateActivity(idealActivity));
		return;
	}
	else
	{
		SetActivity(idealActivity);

		animDesired = SelectWeightedSequence(Weapon_TranslateActivity(idealActivity));

		if (animDesired == -1)
		{
			animDesired = SelectWeightedSequence(idealActivity);

			if (animDesired == -1)
			{
				animDesired = 0;
			}
		}

		// Already using the desired animation?
		if (GetSequence() == animDesired)
			return;

		m_flPlaybackRate = 1.0;
		ResetSequence(animDesired);
		SetCycle(0);
		return;
	}

	// Already using the desired animation?
	if (GetSequence() == animDesired)
		return;

	//Msg( "Set animation to %d\n", animDesired );
	// Reset to first frame of desired animation
	ResetSequence(animDesired);
	SetCycle(0);
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : iImpulse - 
//-----------------------------------------------------------------------------
void C1187_Player::CheatImpulseCommands(int iImpulse)
{
	switch (iImpulse)
	{
	case 101:
	{
		gEvilImpulse101 = true;

		EquipSuit();

		// Give the player everything!
		CBaseCombatCharacter::GiveAmmo(255, "Pistol");
		// CBaseCombatCharacter::GiveAmmo(255, "AR2");
		// CBaseCombatCharacter::GiveAmmo(5,	"AR2AltFire");
		CBaseCombatCharacter::GiveAmmo(255, "SMG1");
		CBaseCombatCharacter::GiveAmmo(255, "Buckshot");
		// CBaseCombatCharacter::GiveAmmo(3,	"smg1_grenade");
		CBaseCombatCharacter::GiveAmmo(3,	"rpg_round");
		CBaseCombatCharacter::GiveAmmo(5,	"grenade");
		CBaseCombatCharacter::GiveAmmo(32,	"357");
		// CBaseCombatCharacter::GiveAmmo(16,	"XBowBolt");

		CBaseCombatCharacter::GiveAmmo(255, "ColtPistol");
		CBaseCombatCharacter::GiveAmmo(255, "Health");
		CBaseCombatCharacter::GiveAmmo(255, "M4");
		CBaseCombatCharacter::GiveAmmo(255, "M16");

		GiveNamedItem("weapon_smg1");
		// GiveNamedItem("weapon_frag");
		GiveNamedItem("weapon_crowbar");
		GiveNamedItem("weapon_pistol");
		// GiveNamedItem("weapon_ar2");
		GiveNamedItem("weapon_shotgun");
		// GiveNamedItem("weapon_physcannon");
		// GiveNamedItem("weapon_bugbait");
		GiveNamedItem("weapon_rpg");
		GiveNamedItem("weapon_357");
		// GiveNamedItem("weapon_crossbow");
		GiveNamedItem("weapon_healthpack");
		GiveNamedItem("weapon_knife");
		GiveNamedItem("weapon_m4");
		GiveNamedItem("weapon_m16");

		if (GetHealth() < 100)
		{
			TakeHealth(25, DMG_GENERIC);
		}

		gEvilImpulse101 = false;

		break;
	}

	default:
		BaseClass::CheatImpulseCommands(iImpulse);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_Player::ClientCommand(const CCommand &args)
{
	if (stricmp(args[0], "toggle_ironsight") == 0)
	{
		CBase1187CombatWeapon *pWeapon = ToBase1187CombatWeapon(GetActiveWeapon());
		if (pWeapon != NULL)
			pWeapon->ToggleIronsights();

		return true;
	}

	return BaseClass::ClientCommand(args);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::OnJumping(float fImpulse)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::OnLanding(float fVelocity)
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::Event_OnJump(float fImpulse)
{
	CBase1187CombatWeapon *pWeapon = ToBase1187CombatWeapon(GetActiveWeapon());
	if (pWeapon)
	{
		pWeapon->Operator_HandleJumpEvent(fImpulse, this);
	}

	OnJumping(fImpulse);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::Event_OnLand(float fVelocity)
{
	CBase1187CombatWeapon *pWeapon = ToBase1187CombatWeapon(GetActiveWeapon());
	if (pWeapon)
	{
		pWeapon->Operator_HandleLandEvent(fVelocity, this);
	}

	OnLanding(fVelocity);
}