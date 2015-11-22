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
#include "ammodef.h"

#include "1187_grenade_frag.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar hl2_walkspeed;
extern ConVar hl2_normspeed;
extern ConVar hl2_sprintspeed;

#define PLAYER_MODEL_FIRSTPERSON	"models/weapons/w_1187fp.mdl"
#define PLAYER_MODEL_THIRDPERSON	"models/humans/group01/male_09.mdl"

extern int gEvilImpulse101;




//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
static void CreateMustang(CBasePlayer *pPlayer)
{
	// Cheat to create a jeep in front of the player
	Vector vecForward;
	AngleVectors(pPlayer->EyeAngles(), &vecForward);
	CBaseEntity *pMustang = (CBaseEntity *)CreateEntityByName("prop_vehicle_car");
	if (pMustang)
	{
		Vector vecOrigin = pPlayer->GetAbsOrigin() + vecForward * 256 + Vector(0, 0, 64);
		QAngle vecAngles(0, pPlayer->GetAbsAngles().y - 90, 0);
		pMustang->SetAbsOrigin(vecOrigin);
		pMustang->SetAbsAngles(vecAngles);
		pMustang->KeyValue("model", "models/mustang.mdl");
		pMustang->KeyValue("solid", "6");
		pMustang->KeyValue("targetname", "mustang");
		pMustang->KeyValue("vehiclescript", "scripts/vehicles/mustang.txt");
		DispatchSpawn(pMustang);
		pMustang->Activate();
		pMustang->Teleport(&vecOrigin, &vecAngles, NULL);
	}
}


void CC_CH_CreateMustang(void)
{
	CBasePlayer *pPlayer = UTIL_GetCommandClient();
	if (!pPlayer)
		return;
	CreateMustang(pPlayer);
}

static ConCommand ch_createmustang("ch_createmustang", CC_CH_CreateMustang, "Spawn mustang in front of the player.", FCVAR_CHEAT);



LINK_ENTITY_TO_CLASS( player, C1187_Player );

// Global Savedata for 1187 player
BEGIN_DATADESC(C1187_Player)
	DEFINE_EMBEDDED( m_1187Local ),
#if 0
	DEFINE_FIELD(m_angEyeAngles, FIELD_VECTOR),
#endif
	DEFINE_FIELD(m_bAdjacentToWall, FIELD_BOOLEAN),

	DEFINE_FIELD(m_flHaulBackAnimTime, FIELD_TIME),
	DEFINE_FIELD(m_flHaulBackAnimEventTime, FIELD_TIME),
	DEFINE_FIELD(m_bIsHaulBackActive, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bHaulBackEventHandled, FIELD_BOOLEAN),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(C1187_Player, DT_1187_Player)
	SendPropDataTable(SENDINFO_DT(m_1187Local), &REFERENCE_SEND_TABLE(DT_1187Local), SendProxy_SendLocalDataTable),

#if 0
	SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 0), 11, SPROP_CHANGES_OFTEN),
	SendPropAngle(SENDINFO_VECTORELEM(m_angEyeAngles, 1), 11, SPROP_CHANGES_OFTEN),
#endif

	SendPropBool(SENDINFO(m_bAdjacentToWall)),

END_SEND_TABLE()

#if 1
C1187_Player::C1187_Player()
#else
C1187_Player::C1187_Player() : m_PlayerAnimState(this)
#endif
{
#if 0
	m_angEyeAngles.Init();
#endif
	m_bAdjacentToWall = false;
	m_bIsHaulBackActive = false;
	m_bHaulBackEventHandled = false;
	m_flHaulBackAnimTime = 0.0f;
	m_flHaulBackAnimEventTime = 0.0f;
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

	PrecacheModel(PLAYER_MODEL_FIRSTPERSON);
	PrecacheModel(PLAYER_MODEL_THIRDPERSON);

	PrecacheModel("sprites/glow01.vmt");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::Spawn(void)
{
	BaseClass::Spawn();

	SetModel(PLAYER_MODEL_FIRSTPERSON);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::PreThink(void)
{

	VPROF_SCOPE_BEGIN("C1187_Player::PreThink-WeaponLoweredOnSprint_Update");
	WeaponLoweredOnSprint_Update();
	VPROF_SCOPE_END();

	VPROF_SCOPE_BEGIN("C1187_Player::PreThink-WallProximity_Update");
	WallProximity_Update();
	VPROF_SCOPE_END();

	VPROF_SCOPE_BEGIN("C1187_Player::PreThink-CheckMeleeInput");
	CheckMeleeInput();
	VPROF_SCOPE_END();

	VPROF_SCOPE_BEGIN("C1187_Player::PreThink-CheckHaulBackInput");
	CheckHaulBackInput();
	VPROF_SCOPE_END();

	BaseClass::PreThink();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::PostThink(void)
{
	VPROF_SCOPE_BEGIN("C1187_Player::PostThink-WallProximity_Update");
	WallProximity_Update();
	VPROF_SCOPE_END();

	BaseClass::PostThink();

	if (!g_fGameOver && !IsPlayerLockedInPlace() && IsAlive())
	{
		HandleHaulBackAnimation();
		HandleMeleeAnimation();
	}

#if 0
	m_PlayerAnimState.Update();

	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();
#endif

	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;
	SetLocalAngles(angles);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::EquipSuit(bool bPlayEffects)
{
	MDLCACHE_CRITICAL_SECTION();
	CBasePlayer::EquipSuit();
	
	m_HL2Local.m_bDisplayReticle = true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::StartSprinting(void)
{
	BaseClass::StartSprinting();

	WeaponLoweredOnSprint_Start();

	// Disable ironsight if enabled.
	if (GetActiveWeapon() && ToBase1187CombatWeapon(GetActiveWeapon()))
		((CBase1187CombatWeapon*)GetActiveWeapon())->DisableIronsights();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::StopSprinting(void)
{
	BaseClass::StopSprinting();
	
	WeaponLoweredOnSprint_Stop();
}

// Set the activity based on an event or current state
void C1187_Player::SetAnimation(PLAYER_ANIM playerAnim)
{
	int animDesired;

	float speed;

	speed = GetAbsVelocity().Length2D();


	bool left, right, forward, back;
	left = (m_nButtons & IN_MOVELEFT) ? true : false;
	right = (m_nButtons & IN_MOVERIGHT) ? true : false;
	forward = (m_nButtons & IN_FORWARD) ? true : false;
	back = (m_nButtons & IN_BACK) ? true : false;


	// bool bRunning = true;

	//Revisit!
	/*	if ( ( m_nButtons & ( IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT ) ) )
	{
	if ( speed > 1.0f && speed < hl2_normspeed.GetFloat() - 20.0f )
	{
	bRunning = false;
	}
	}*/

	bool bRunning = true;

	if (speed > 1.0f && speed < hl2_normspeed.GetFloat() - 20.0f)
	{
		bRunning = false;
	}

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
					if (forward)
					{
						idealActivity = ACT_WALK_CROUCH; // Forward crouch walk.
					}
					else
					{
						idealActivity = ACT_MP_CROUCH_SECONDARY; // Backward crouch walk.
					}
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
					if (forward && left)
					{
						if (left)
						{
							idealActivity = (bRunning) ? ACT_MP_SPRINT : ACT_MP_AIRWALK; // Running forward left.
						}
						else if (right)
						{
							idealActivity = (bRunning) ? ACT_MP_RUN_PDA : ACT_MP_WALK_PDA; // Running forward right.
						}
						else
						{
							idealActivity = (bRunning) ? ACT_RUN : ACT_WALK; // Running forward.
						}
					}
					else if (back)
					{
						if (left)
						{
							idealActivity = (bRunning) ? ACT_MP_SWIM_SECONDARY : ACT_MP_AIRWALK_SECONDARY; // Running backward left.
						}
						else if (right)
						{
							idealActivity = (bRunning) ? ACT_MP_SWIM_PRIMARY : ACT_MP_AIRWALK_PRIMARY; // Running backward right.
						}
						else
						{
							idealActivity = (bRunning) ? ACT_MP_RUN_SECONDARY : ACT_MP_WALK_SECONDARY; // Running backward.
						}
					}
					else if (left)
					{
						idealActivity = (bRunning) ? ACT_MP_RUN_MELEE : ACT_MP_WALK_MELEE; // Running left.
					}
					else if (right)
					{
						idealActivity = (bRunning) ? ACT_MP_RUN_PRIMARY : ACT_MP_WALK_PRIMARY; // Running right.
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
		// RestartGesture(Weapon_TranslateActivity(idealActivity));

		// FIXME: this seems a bit wacked
		// Weapon_SetActivity(Weapon_TranslateActivity(ACT_RANGE_ATTACK1), 0);

		return;
	}
	else if (idealActivity == ACT_GESTURE_RELOAD)
	{
		// RestartGesture(Weapon_TranslateActivity(idealActivity));
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

		CBaseCombatCharacter::GiveAmmo(255, "98");
		CBaseCombatCharacter::GiveAmmo(255, "ColtPistol");
		CBaseCombatCharacter::GiveAmmo(255, "Health");
		CBaseCombatCharacter::GiveAmmo(255, "M4");
		CBaseCombatCharacter::GiveAmmo(255, "M16");

		GiveNamedItem("weapon_dualmp5k");
		GiveNamedItem("weapon_dualpistol");
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
		GiveNamedItem("weapon_kar98");
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

extern ConVar sk_max_grenade;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool C1187_Player::Weapon_CanUse(CBaseCombatWeapon *pWeapon)
{
	if (pWeapon->ClassMatches("weapon_frag"))
	{
		int ammoIndex = GetAmmoDef()->Index("grenade");
		Assert(ammoIndex != -1);

		if (GetAmmoCount(ammoIndex) < sk_max_grenade.GetInt())
		{ 
			GiveAmmo(1, ammoIndex, false);
			UTIL_Remove(pWeapon);
		}
		return false;
	}

	return BaseClass::Weapon_CanUse(pWeapon);
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
//-----------------------------------------------------------------------------
void C1187_Player::OnHaulBack(void)
{
	CBase1187CombatWeapon* pWeapon = ToBase1187CombatWeapon( GetActiveWeapon() );
	if (!pWeapon)
		return;

	Vector	vecEye;
	Vector	vForward, vRight, vUp;

	EyePositionAndVectors(&vecEye, &vForward, &vRight, &vUp);

	Vector vFragOffset = pWeapon->GetFragPositionOffset();

	Assert(vFragOffset != vec3_invalid);

	Vector vecSrc = vecEye + vForward * vFragOffset.x + vRight * vFragOffset.y + vUp * vFragOffset.z;

	trace_t tr;
	UTIL_TraceHull(vecEye, vecSrc,
		-Vector(FRAG_GRENADE_RADIUS + 2, FRAG_GRENADE_RADIUS + 2, FRAG_GRENADE_RADIUS + 2),
		Vector(FRAG_GRENADE_RADIUS + 2, FRAG_GRENADE_RADIUS + 2, FRAG_GRENADE_RADIUS + 2),
		PhysicsSolidMaskForEntity(), this, GetCollisionGroup(), &tr);

	if (tr.DidHit())
	{
		vecSrc = tr.endpos;
	}

	vForward[2] += 0.1f;

	Vector vecThrow;
	GetVelocity(&vecThrow, NULL);
	vecThrow += vForward * 1200;

	if (Fraggrenade_Create_1187(vecSrc, vec3_angle, vecThrow, AngularImpulse(600, random->RandomInt(-1200, 1200), 0), this, FRAG_GRENADE_TIMER))
	{
		// Decrement grenade count.
		int grenadeIndex = GetAmmoDef()->Index("grenade");

		Assert(grenadeIndex != -1);

		RemoveAmmo(1, grenadeIndex);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::OnMelee(void)
{

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::WeaponLoweredOnSprint_SetState(bool state)
{
	m_1187Local.m_bWeaponLoweredOnSprint = state;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::WeaponLoweredOnSprint_Start()
{
	WeaponLoweredOnSprint_SetState(true);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::WeaponLoweredOnSprint_Stop()
{
	WeaponLoweredOnSprint_SetState(false);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_Player::WeaponLoweredOnSprint_Update()
{
	if (!GetActiveWeapon())
		return;

	float speed = GetAbsVelocity().Length2D();

	if (speed <= hl2_normspeed.GetFloat())
		return;

	CBase1187CombatWeapon* pWeapon = ToBase1187CombatWeapon( GetActiveWeapon() );
	if (pWeapon)
	{
		if (WeaponLoweredOnSprint_IsActive() && !pWeapon->Sprint_IsWeaponLowered())
		{
			pWeapon->Sprint_Lower();
		}
		else if (pWeapon->Sprint_IsWeaponLowered())
		{
			pWeapon->Sprint_Leave();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_Player::WallProximity_ShouldCheck(void) const
{
	if (!GetActiveWeapon())
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::WallProximity_Update(void)
{
	if (!WallProximity_ShouldCheck())
		return;

	CBaseHLCombatWeapon* pWeapon = dynamic_cast<CBaseHLCombatWeapon*>(GetActiveWeapon());
	if (!pWeapon)
		return;

	if (WallProximity_Check() && !m_bAdjacentToWall)
	{
		m_1187Local.m_bAdjacentToWall = true;
		m_bAdjacentToWall = true;

		pWeapon->Lower();
	}
	else if (m_bAdjacentToWall)
	{
		m_1187Local.m_bAdjacentToWall = false;
		m_bAdjacentToWall = false;

		pWeapon->Ready();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_Player::WallProximity_Check(void)
{
	Vector vecSrc, vecForward;
	vecSrc = Weapon_ShootPosition();

	AngleVectors(LocalEyeAngles(), &vecForward);

	trace_t tr;
	UTIL_TraceLine(vecSrc, vecSrc + vecForward * 24, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr);

	if (tr.fraction != 1.0f && tr.m_pEnt)
	{
		CBaseEntity* pHit = tr.m_pEnt;

		Assert(pHit);

		if (pHit->m_takedamage == DAMAGE_NO && !pHit->IsNPC())
		{
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_Player::CanHaulBack(void) const
{
	if (m_fIsSprinting)
		return false;

	int grenadeIndex = GetAmmoDef()->Index("grenade");

	// Check if we have grenades.
	if (GetAmmoCount(grenadeIndex) <= 0)
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_Player::IsHaulBackActive(void) const
{
	return m_bIsHaulBackActive;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::StartHaulBackAnimation(void)
{
	MDLCACHE_CRITICAL_SECTION();
	CBaseViewModel *vm = GetViewModel(0);

	if (vm && GetActiveWeapon())
	{
		// vm->SetWeaponModel("models/weapons/v_hands.mdl", NULL);
		// ShowViewModel(true);

		int	idealSequence = vm->SelectWeightedSequence(ACT_VM_HAULBACK);

		if (idealSequence >= 0)
		{
			vm->SendViewModelMatchingSequence(idealSequence);
			m_flHaulBackAnimTime = gpGlobals->curtime + vm->SequenceDuration(idealSequence);
			m_flHaulBackAnimEventTime = gpGlobals->curtime + 0.28f;
			m_bIsHaulBackActive = true;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::HandleHaulBackAnimation(void)
{
	CBaseViewModel *pVM = GetViewModel();

	if (pVM && pVM->GetOwningWeapon())
	{
		if (m_flHaulBackAnimTime != 0.0)
		{
			if (m_flHaulBackAnimTime > gpGlobals->curtime)
			{
				// Check if it is time to summon Haul back event.
				if (!m_bHaulBackEventHandled && m_flHaulBackAnimEventTime < gpGlobals->curtime)
				{
					Event_HaulBack();

					// Perform haul back only once.
					m_bHaulBackEventHandled = true;
				}
				
				pVM->m_flPlaybackRate = 1.0f;
				pVM->StudioFrameAdvance();
			}
			else if (m_flHaulBackAnimTime < gpGlobals->curtime)
			{
				m_flHaulBackAnimTime		= 0.0f;
				m_bIsHaulBackActive			= false;
				m_bHaulBackEventHandled		= false;
				// pVM->SetWeaponModel(NULL, NULL);
			}
		}
	}
	else
	{
		// Reset all Haul Back data.
		m_bHaulBackEventHandled		= false;
		m_bIsHaulBackActive			= false;
		m_flHaulBackAnimTime		= 0.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::CheckHaulBackInput(void)
{
	int buttonsChanged = m_afButtonPressed | m_afButtonReleased;

	bool bCanHaulBack = CanHaulBack();
	bool bIsHaulBackActive = IsHaulBackActive();
	bool bWantHaulBack = (bCanHaulBack && (m_nButtons & IN_FRAG));
	if (bIsHaulBackActive != bWantHaulBack && (buttonsChanged & IN_FRAG))
	{
		if (bWantHaulBack)
		{
			StartHaulBackAnimation();
		}
		else
		{
			// Reset key, so it will be activated post whatever is suppressing it.
			m_nButtons &= ~IN_FRAG;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_Player::CanMelee(void) const
{
	if (!GetActiveWeapon())
		return false;

	if (m_fIsSprinting)
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_Player::IsInMelee(void) const
{
	return m_bIsInMelee;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::StartMeleeAnimation(void)
{
	MDLCACHE_CRITICAL_SECTION();
	CBaseViewModel *vm = GetViewModel(0);

	if (vm && GetActiveWeapon())
	{
		// vm->SetWeaponModel("models/weapons/v_hands.mdl", NULL);
		// ShowViewModel(true);

		int	idealSequence = vm->SelectWeightedSequence(ACT_VM_SWINGHARD);

		if (idealSequence >= 0)
		{
			vm->SendViewModelMatchingSequence(idealSequence);
			m_flMeleeAnimTime = gpGlobals->curtime + vm->SequenceDuration(idealSequence);
			m_flMeleeAnimEventTime = gpGlobals->curtime + 0.18f;
			m_bIsInMelee = true;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::HandleMeleeAnimation(void)
{
	CBaseViewModel *pVM = GetViewModel();

	if (pVM && pVM->GetOwningWeapon())
	{
		if (m_flMeleeAnimTime != 0.0)
		{
			if (m_flMeleeAnimTime > gpGlobals->curtime)
			{
				// Check if it is time to summon melee event.
				if (!m_bMeleeEventHandled && m_flMeleeAnimEventTime < gpGlobals->curtime)
				{
					Event_Melee();

					// Perform haul back only once.
					m_bMeleeEventHandled = true;
				}

				pVM->m_flPlaybackRate = 1.0f;
				pVM->StudioFrameAdvance();
			}
			else if (m_flHaulBackAnimTime < gpGlobals->curtime)
			{
				m_flMeleeAnimTime = 0.0f;
				m_bIsInMelee = false;
				m_bMeleeEventHandled = false;
				// pVM->SetWeaponModel(NULL, NULL);
			}
		}
	}
	else
	{
		// Reset all melee data.
		m_bMeleeEventHandled = false;
		m_bIsInMelee = false;
		m_flMeleeAnimTime = 0.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::CheckMeleeInput(void)
{
	int buttonsChanged = m_afButtonPressed | m_afButtonReleased;

	bool bCanMelee = CanMelee();
	bool bIsInMelee = IsInMelee();
	bool bWantMelee = (bCanMelee && (m_nButtons & IN_MELEE));
	if (bIsInMelee != bWantMelee && (buttonsChanged & IN_MELEE))
	{
		if (bWantMelee)
		{
			StartMeleeAnimation();
		}
		else
		{
			// Reset key, so it will be activated post whatever is suppressing it.
			m_nButtons &= ~IN_MELEE;
		}
	}
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

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::Event_HaulBack(void)
{
	OnHaulBack();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_Player::Event_Melee(void)
{
	CBase1187CombatWeapon *pWeapon = ToBase1187CombatWeapon(GetActiveWeapon());
	if (pWeapon)
	{
		pWeapon->Operator_HandleMeleeEvent(this);
	}

	OnMelee();
}