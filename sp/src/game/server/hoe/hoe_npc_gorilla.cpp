//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "game.h"
#include "antlion_dust.h"
#include "ai_default.h"
#include "ai_schedule.h"
#include "ai_hint.h"
#include "ai_hull.h"
#include "ai_navigator.h"
#include "ai_moveprobe.h"
#include "ai_memory.h"
#include "bitstring.h"
#include "hl2_shareddefs.h"
#include "npcevent.h"
#include "soundent.h"
#include "npc_headcrab.h"
#include "gib.h"
#include "ai_interactions.h"
#include "ndebugoverlay.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "movevars_shared.h"
#include "world.h"
#include "npc_bullseye.h"
#include "physics_npc_solver.h"
#include "hl2_gamerules.h"
#include "decals.h"
#include "vehicle_base.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define GORILLA_BURN_SOUND_FREQUENCY	10
#define GORILLA_MELEE_REACH				55

extern int AE_HEADCRAB_JUMPATTACK;

extern ConVar	sk_zombie_dmg_one_slash;
extern ConVar	sk_zombie_dmg_both_slash;

//--------------------------------------
// Animation events.
//--------------------------------------
int AE_GORILLA_JUMPATTACK;
int AE_GORILLA_ATTACK_LEFT;
int AE_GORILLA_ATTACK_RIGHT;
int AE_GORILLA_ATTACK_BOTH;


//============================================================
// CHoe_NPC_Gorilla
//============================================================
class CHoe_NPC_Gorilla : public CBaseHeadcrab
{
	DECLARE_CLASS(CHoe_NPC_Gorilla, CBaseHeadcrab);

public:
	DEFINE_CUSTOM_AI;

	void Precache(void);
	void Spawn(void);

	Class_T Classify(void) { return CLASS_STALKER; }

	void	HandleAnimEvent(animevent_t *pEvent);
	float	MaxYawSpeed(void);

	virtual float GetClawAttackRange() const { return GORILLA_MELEE_REACH; }
	CBaseEntity *ClawAttack(float flDist, int iDamage, QAngle &qaViewPunch, Vector &vecVelocityPunch);

	void	BiteSound(void);
	void	PainSound(const CTakeDamageInfo &info);
	void	DeathSound(const CTakeDamageInfo &info);
	void	IdleSound(void);
	void	AlertSound(void);
	void	AttackSound(void);
	void	TelegraphSound(void);
	void	AttackHitSound(void);
	void	AttackMissSound(void);

	int		MeleeAttack1Conditions(float flDot, float flDist);

	int		SelectSchedule(void);
};

LINK_ENTITY_TO_CLASS(npc_gorilla, CHoe_NPC_Gorilla);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::Precache(void)
{
	PrecacheModel("models/ape/gorilla/gorilla.mdl");

	PrecacheScriptSound("NPC_Gorilla.FootstepLeft");
	PrecacheScriptSound("NPC_Gorilla.FootstepRight");
	PrecacheScriptSound("NPC_Gorilla.AnnounceAttack");
	PrecacheScriptSound("NPC_Gorilla.AttackHit");
	PrecacheScriptSound("NPC_Gorilla.AttackMiss");
	PrecacheScriptSound("NPC_Gorilla.Idle");
	PrecacheScriptSound("NPC_Gorilla.Alert");
	PrecacheScriptSound("NPC_Gorilla.Pain");
	PrecacheScriptSound("NPC_Gorilla.Death");
	PrecacheScriptSound("NPC_Gorilla.Victory");
	PrecacheScriptSound("NPC_Gorilla.BeatChest");
	PrecacheScriptSound("NPC_Gorilla.Eat");

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::Spawn(void)
{
	Precache();
	SetModel("models/ape/gorilla/gorilla.mdl");

	SetHullType(HULL_MEDIUM);
	SetHullSizeNormal();

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_STANDABLE);
	SetMoveType(MOVETYPE_STEP);

	SetViewOffset(Vector(16, 0, 36));		// Position of the eyes relative to NPC's origin.

	SetBloodColor(BLOOD_COLOR_RED);
	m_flFieldOfView = 0.5;
	m_NPCState = NPC_STATE_NONE;

	CapabilitiesClear();
	CapabilitiesAdd(bits_CAP_MOVE_GROUND | bits_CAP_INNATE_RANGE_ATTACK1 | bits_CAP_INNATE_MELEE_ATTACK1);
	CapabilitiesAdd(bits_CAP_SQUAD);

	// headcrabs get to cheat for 5 seconds (sjb)
	GetEnemies()->SetFreeKnowledgeDuration(5.0);

	m_bHidden = false;
	m_bHangingFromCeiling = false;
	m_bCrawlFromCanister = false;
	m_bMidJump = false;

	m_nGibCount = 0;
	m_flIlluminatedTime = -1;

	m_iHealth = m_iMaxHealth = 150;
	m_flBurrowTime = 0.0f;

	NPCInit();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CHoe_NPC_Gorilla::MeleeAttack1Conditions(float flDot, float flDist)
{
	if (flDot < 0.5f)
		return COND_NOT_FACING_ATTACK;

	if (flDist > 72)
		return COND_TOO_FAR_TO_ATTACK;

	return COND_CAN_MELEE_ATTACK1;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  :
// Output : 
//-----------------------------------------------------------------------------
float CHoe_NPC_Gorilla::MaxYawSpeed(void)
{
	switch (GetActivity())
	{
	case ACT_IDLE:
		return 30;

	case ACT_RUN:
	case ACT_WALK:
		return 20;

	case ACT_TURN_LEFT:
	case ACT_TURN_RIGHT:
		return 15;

	case ACT_RANGE_ATTACK1:
	default:
		return 30;
	}

	return BaseClass::MaxYawSpeed();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::IdleSound(void)
{
	EmitSound("NPC_Gorilla.Idle");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::AlertSound(void)
{
	EmitSound("NPC_Gorilla.Alert");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::PainSound(const CTakeDamageInfo &info)
{
	if (IsOnFire() && random->RandomInt(0, GORILLA_BURN_SOUND_FREQUENCY) > 0)
	{
		// Don't squeak every think when burning.
		return;
	}

	EmitSound("NPC_Gorilla.Pain");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::DeathSound(const CTakeDamageInfo &info)
{
	EmitSound("NPC_Gorilla.Die");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::TelegraphSound(void)
{
	//FIXME: Need a real one
	EmitSound("NPC_Gorilla.Alert");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::AttackSound(void)
{
	EmitSound("NPC_Gorilla.Attack");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::BiteSound(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: Play a random attack hit sound
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::AttackHitSound(void)
{
	EmitSound("NPC_Gorilla.AttackHit");
}

//-----------------------------------------------------------------------------
// Purpose: Play a random attack miss sound
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::AttackMissSound(void)
{
	// Play a random attack miss sound
	EmitSound("NPC_Gorilla.AttackMiss");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CHoe_NPC_Gorilla::SelectSchedule(void)
{
	if (m_NPCState == NPC_STATE_COMBAT)
	{
		if (HasCondition(COND_CAN_MELEE_ATTACK1))
		{
			return SCHED_MELEE_ATTACK1;
		}
	}

	return BaseClass::SelectSchedule();
}



//-----------------------------------------------------------------------------
// Purpose: Catches the monster-specific messages that occur when tagged
//			animation frames are played.
// Input  : *pEvent - 
//-----------------------------------------------------------------------------
void CHoe_NPC_Gorilla::HandleAnimEvent(animevent_t *pEvent)
{
	if ( pEvent->event == AE_GORILLA_JUMPATTACK )
	{
		pEvent->event = AE_HEADCRAB_JUMPATTACK;

		BaseClass::HandleAnimEvent( pEvent );
	}

	if ( pEvent->event == AE_GORILLA_ATTACK_RIGHT )
	{
		Vector right, forward;
		AngleVectors( GetLocalAngles(), &forward, &right, NULL );

		right = right * 100;
		forward = forward * 200;

		QAngle qa( -15, -20, -10 );
		Vector vec = right + forward;
		ClawAttack( GetClawAttackRange(), sk_zombie_dmg_one_slash.GetFloat(), qa, vec );
		return;
	}

	if ( pEvent->event == AE_GORILLA_ATTACK_LEFT )
	{
		Vector right, forward;
		AngleVectors( GetLocalAngles(), &forward, &right, NULL );

		right = right * -100;
		forward = forward * 200;

		QAngle qa(-15, 20, -10);
		Vector vec = right + forward;
		ClawAttack(GetClawAttackRange(), sk_zombie_dmg_one_slash.GetFloat(), qa, vec );
		return;
	}

	if ( pEvent->event == AE_GORILLA_ATTACK_BOTH )
	{
		Vector forward;
		QAngle qaPunch( 45, random->RandomInt(-5,5), random->RandomInt(-5,5) );
		AngleVectors( GetLocalAngles(), &forward );
		forward = forward * 200;
		ClawAttack( GetClawAttackRange(), sk_zombie_dmg_one_slash.GetFloat(), qaPunch, forward );
		return;
	}

	BaseClass::HandleAnimEvent(pEvent);
}

//-----------------------------------------------------------------------------
// Purpose: Look in front and see if the claw hit anything.
//
// Input  :	flDist				distance to trace		
//			iDamage				damage to do if attack hits
//			vecViewPunch		camera punch (if attack hits player)
//			vecVelocityPunch	velocity punch (if attack hits player)
//
// Output : The entity hit by claws. NULL if nothing.
//-----------------------------------------------------------------------------
CBaseEntity *CHoe_NPC_Gorilla::ClawAttack(float flDist, int iDamage, QAngle &qaViewPunch, Vector &vecVelocityPunch)
{
	// Added test because claw attack anim sometimes used when for cases other than melee
	int iDriverInitialHealth = -1;
	CBaseEntity *pDriver = NULL;
	if (GetEnemy())
	{
		trace_t	tr;
		AI_TraceHull(WorldSpaceCenter(), GetEnemy()->WorldSpaceCenter(), -Vector(8, 8, 8), Vector(8, 8, 8), MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &tr);

		if (tr.fraction < 1.0f)
			return NULL;

		// CheckTraceHullAttack() can damage player in vehicle as side effect of melee attack damaging physics objects, which the car forwards to the player
		// need to detect this to get correct damage effects
		CBaseCombatCharacter *pCCEnemy = (GetEnemy() != NULL) ? GetEnemy()->MyCombatCharacterPointer() : NULL;
		CBaseEntity *pVehicleEntity;
		if (pCCEnemy != NULL && (pVehicleEntity = pCCEnemy->GetVehicleEntity()) != NULL)
		{
			if (pVehicleEntity->GetServerVehicle() && dynamic_cast<CPropVehicleDriveable *>(pVehicleEntity))
			{
				pDriver = static_cast<CPropVehicleDriveable *>(pVehicleEntity)->GetDriver();
				if (pDriver && pDriver->IsPlayer())
				{
					iDriverInitialHealth = pDriver->GetHealth();
				}
				else
				{
					pDriver = NULL;
				}
			}
		}
	}

	//
	// Trace out a cubic section of our hull and see what we hit.
	//
	Vector vecMins = GetHullMins();
	Vector vecMaxs = GetHullMaxs();
	vecMins.z = vecMins.x;
	vecMaxs.z = vecMaxs.x;

	CBaseEntity *pHurt = NULL;
	if (GetEnemy() && GetEnemy()->Classify() == CLASS_BULLSEYE)
	{
		// We always hit bullseyes we're targeting
		pHurt = GetEnemy();
		CTakeDamageInfo info(this, this, vec3_origin, GetAbsOrigin(), iDamage, DMG_SLASH);
		pHurt->TakeDamage(info);
	}
	else
	{
		// Try to hit them with a trace
		pHurt = CheckTraceHullAttack(flDist, vecMins, vecMaxs, iDamage, DMG_SLASH);
	}

	if (pDriver && iDriverInitialHealth != pDriver->GetHealth())
	{
		pHurt = pDriver;
	}

	if (pHurt)
	{
		AttackHitSound();

		CBasePlayer *pPlayer = ToBasePlayer(pHurt);

		if (pPlayer != NULL && !(pPlayer->GetFlags() & FL_GODMODE))
		{
			pPlayer->ViewPunch(qaViewPunch);

			pPlayer->VelocityPunch(vecVelocityPunch);
		}
	}
	else
	{
		AttackMissSound();
	}

	return pHurt;
}


AI_BEGIN_CUSTOM_NPC(npc_gorilla, CHoe_NPC_Gorilla)

	DECLARE_ANIMEVENT(AE_GORILLA_JUMPATTACK)
	DECLARE_ANIMEVENT(AE_GORILLA_ATTACK_LEFT)
	DECLARE_ANIMEVENT(AE_GORILLA_ATTACK_RIGHT)
	DECLARE_ANIMEVENT(AE_GORILLA_ATTACK_BOTH)

AI_END_CUSTOM_NPC()