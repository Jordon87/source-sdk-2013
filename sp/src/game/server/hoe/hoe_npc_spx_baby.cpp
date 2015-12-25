//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Implements the headcrab, a tiny, jumpy alien parasite.
//
// TODO: make poison headcrab hop in response to nearby bullet impacts?
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
#include "decals.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define NPC_SPX_BABY_MODEL				"models/spx_baby/spx_baby.mdl"
#define NPC_SPX_BURN_SOUND_FREQUENCY	10

extern ConVar	sk_headcrab_health;

//-----------------------------------------------------------------------------
// Animation events.
//-----------------------------------------------------------------------------
int AE_SPXBABY_JUMPATTACK;

class CHoe_NPC_SPX_Baby : public CBaseHeadcrab
{
	DECLARE_CLASS(CHoe_NPC_SPX_Baby, CBaseHeadcrab);

public:
	DEFINE_CUSTOM_AI;


	void	Precache(void);
	void	Spawn(void);
	void	HandleAnimEvent(animevent_t *pEvent);

	float	MaxYawSpeed(void);
	Activity NPC_TranslateActivity(Activity eNewActivity);

	void	BiteSound(void);
	void	PainSound(const CTakeDamageInfo &info);
	void	DeathSound(const CTakeDamageInfo &info);
	void	IdleSound(void);
	void	AlertSound(void);
	void	AttackSound(void);
	void	TelegraphSound(void);
};

LINK_ENTITY_TO_CLASS(npc_spx_baby, CHoe_NPC_SPX_Baby);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::Precache(void)
{
	PrecacheModel(NPC_SPX_BABY_MODEL);

	PrecacheScriptSound("NPC_SpxBaby.Idle");
	PrecacheScriptSound("NPC_SpxBaby.Alert");
	PrecacheScriptSound("NPC_SpxBaby.Pain");
	PrecacheScriptSound("NPC_SpxBaby.Die");
	PrecacheScriptSound("NPC_SpxBaby.Attack");
	PrecacheScriptSound("NPC_SpxBaby.Bite");
	PrecacheScriptSound("NPC_SpxBaby.Jump");
	PrecacheScriptSound("NPC_SpxBaby.Run");

	BaseClass::Precache();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::Spawn(void)
{
	Precache();
	SetModel(NPC_SPX_BABY_MODEL);

	BaseClass::Spawn();

	m_iHealth = sk_headcrab_health.GetFloat();
	m_flBurrowTime = 0.0f;
	m_bCrawlFromCanister = false;
	m_bMidJump = false;

	NPCInit();
	HeadcrabInit();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Activity CHoe_NPC_SPX_Baby::NPC_TranslateActivity(Activity eNewActivity)
{
	if (eNewActivity == ACT_WALK)
		return ACT_RUN;

	return BaseClass::NPC_TranslateActivity(eNewActivity);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::IdleSound(void)
{
	EmitSound("NPC_SpxBaby.Idle");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::AlertSound(void)
{
	EmitSound("NPC_SpxBaby.Alert");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::PainSound(const CTakeDamageInfo &info)
{
	if (IsOnFire() && random->RandomInt(0, NPC_SPX_BURN_SOUND_FREQUENCY) > 0)
	{
		// Don't squeak every think when burning.
		return;
	}

	EmitSound("NPC_SpxBaby.Pain");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::DeathSound(const CTakeDamageInfo &info)
{
	EmitSound("NPC_SpxBaby.Die");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::TelegraphSound(void)
{
	EmitSound("NPC_SpxBaby.Alert");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::AttackSound(void)
{
	EmitSound("NPC_SpxBaby.Attack");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::BiteSound(void)
{
	EmitSound("NPC_SpxBaby.Bite");
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  :
// Output : 
//-----------------------------------------------------------------------------
float CHoe_NPC_SPX_Baby::MaxYawSpeed(void)
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
		return 30;

	default:
		return 30;
	}

	return BaseClass::MaxYawSpeed();
}



//-----------------------------------------------------------------------------
// Purpose: Catches the monster-specific messages that occur when tagged
//			animation frames are played.
// Input  : *pEvent - 
//-----------------------------------------------------------------------------
void CHoe_NPC_SPX_Baby::HandleAnimEvent(animevent_t *pEvent)
{
	if (pEvent->event == AE_SPXBABY_JUMPATTACK)
	{
		// Ignore if we're in mid air
		if (m_bMidJump)
			return;

		CBaseEntity *pEnemy = GetEnemy();

		if (pEnemy)
		{
			if (m_bCommittedToJump)
			{
				JumpAttack(false, m_vecCommittedJumpPos);
			}
			else
			{
				// Jump at my enemy's eyes.
				JumpAttack(false, pEnemy->EyePosition());
			}

			m_bCommittedToJump = false;

		}
		else
		{
			// Jump hop, don't care where.
			JumpAttack(true);
		}

		return;
	}

	BaseClass::HandleAnimEvent(pEvent);
}




//-----------------------------------------------------------------------------
//
// Schedules
//
//-----------------------------------------------------------------------------

AI_BEGIN_CUSTOM_NPC(npc_spx_baby, CHoe_NPC_SPX_Baby)

	DECLARE_ANIMEVENT(AE_SPXBABY_JUMPATTACK);

AI_END_CUSTOM_NPC()