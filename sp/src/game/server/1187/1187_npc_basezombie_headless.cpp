//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Combine Zombie... Zombie Combine... its like a... Zombine... get it?
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "ai_basenpc.h"
#include "ai_default.h"
#include "ai_schedule.h"
#include "ai_hull.h"
#include "ai_motor.h"
#include "ai_memory.h"
#include "ai_route.h"
#include "ai_squad.h"
#include "soundent.h"
#include "game.h"
#include "npcevent.h"
#include "entitylist.h"
#include "ai_task.h"
#include "activitylist.h"
#include "engine/IEngineSound.h"
#include "npc_BaseZombie.h"
#include "movevars_shared.h"
#include "IEffects.h"
#include "props.h"
#include "physics_npc_solver.h"
#include "hl2_player.h"
#include "hl2_gamerules.h"

#include "basecombatweapon.h"
#include "basegrenade_shared.h"
#include "grenade_frag.h"

#include "ai_interactions.h"

#include "1187_npc_basezombie_headless.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

enum
{
	SQUAD_SLOT_BASEZOMBIE_HEADLESS_SPRINT1 = LAST_SHARED_SQUADSLOT,
	SQUAD_SLOT_BASEZOMBIE_HEADLESS_SPRINT2,
};

#define MIN_SPRINT_TIME 3.5f
#define MAX_SPRINT_TIME 5.5f

#define MIN_SPRINT_DISTANCE 64.0f
#define MAX_SPRINT_DISTANCE 1024.0f

#define SPRINT_CHANCE_VALUE 10
#define SPRINT_CHANCE_VALUE_DARKNESS 50

extern int ACT_ZOMBINE_ATTACK_FAST;

BEGIN_DATADESC(C1187_NPC_BaseZombie_Headless)
DEFINE_FIELD(m_flSprintTime, FIELD_TIME),
DEFINE_FIELD(m_flSprintRestTime, FIELD_TIME),
DEFINE_FIELD(m_flSuperFastAttackTime, FIELD_TIME),
DEFINE_INPUTFUNC(FIELD_VOID, "StartSprint", InputStartSprint),
END_DATADESC()

//---------------------------------------------------------
//---------------------------------------------------------
const char *C1187_NPC_BaseZombie_Headless::pMoanSounds[] =
{
	"ATV_engine_null",
};

void C1187_NPC_BaseZombie_Headless::Spawn(void)
{
	Precache();

	m_fIsTorso = false;
	m_fIsHeadless = true;

	SetBloodColor(BLOOD_COLOR_ZOMBIE);

	m_flFieldOfView = 0.2;

	CapabilitiesClear();

	BaseClass::Spawn();

	m_flSprintTime = 0.0f;
	m_flSprintRestTime = 0.0f;

	m_flNextMoanSound = gpGlobals->curtime + random->RandomFloat(1.0, 4.0);
}

void C1187_NPC_BaseZombie_Headless::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel( STRING( GetModelName() ) );

	PrecacheScriptSound("Zombie.FootstepRight");
	PrecacheScriptSound("Zombie.FootstepLeft");
	PrecacheScriptSound("Zombine.ScuffRight");
	PrecacheScriptSound("Zombine.ScuffLeft");
	PrecacheScriptSound("Zombie.AttackHit");
	PrecacheScriptSound("Zombie.AttackMiss");
	PrecacheScriptSound("Zombine.Pain");
	PrecacheScriptSound("Zombine.Die");
	PrecacheScriptSound("Zombine.Alert");
	PrecacheScriptSound("Zombine.Idle");

	PrecacheScriptSound("ATV_engine_null");
	PrecacheScriptSound("Zombine.Charge");
	PrecacheScriptSound("Zombie.Attack");
}

void C1187_NPC_BaseZombie_Headless::SetZombieModel(void)
{
	SetModel( STRING( GetModelName() ) );
	SetHullType(HULL_HUMAN);

	SetBodygroup(ZOMBIE_BODYGROUP_HEADCRAB, !m_fIsHeadless);

	SetHullSizeNormal(true);
	SetDefaultEyeOffset();
	SetActivity(ACT_IDLE);
}

void C1187_NPC_BaseZombie_Headless::PrescheduleThink(void)
{
	if (gpGlobals->curtime > m_flNextMoanSound)
	{
		if (CanPlayMoanSound())
		{
			// Classic guy idles instead of moans.
			IdleSound();

			m_flNextMoanSound = gpGlobals->curtime + random->RandomFloat(10.0, 15.0);
		}
		else
		{
			m_flNextMoanSound = gpGlobals->curtime + random->RandomFloat(2.5, 5.0);
		}
	}

	BaseClass::PrescheduleThink();
}

void C1187_NPC_BaseZombie_Headless::OnScheduleChange(void)
{
	if (HasCondition(COND_CAN_MELEE_ATTACK1) && IsSprinting() == true)
	{
		m_flSuperFastAttackTime = gpGlobals->curtime + 1.0f;
	}

	BaseClass::OnScheduleChange();
}


Activity C1187_NPC_BaseZombie_Headless::NPC_TranslateActivity(Activity baseAct)
{
	if (baseAct == ACT_MELEE_ATTACK1)
	{
		if (m_flSuperFastAttackTime > gpGlobals->curtime)
		{
			return (Activity)ACT_ZOMBINE_ATTACK_FAST;
		}
	}

	return BaseClass::NPC_TranslateActivity(baseAct);
}

bool C1187_NPC_BaseZombie_Headless::AllowedToSprint(void)
{
	if (IsOnFire())
		return false;

	//If you're sprinting then there's no reason to sprint again.
	if (IsSprinting())
		return false;

	int iChance = SPRINT_CHANCE_VALUE;

	CHL2_Player *pPlayer = dynamic_cast <CHL2_Player*> (AI_GetSinglePlayer());

	if (pPlayer)
	{
		if (HL2GameRules()->IsAlyxInDarknessMode() && pPlayer->FlashlightIsOn() == false)
		{
			iChance = SPRINT_CHANCE_VALUE_DARKNESS;
		}

		//Bigger chance of this happening if the player is not looking at the zombie
		if (pPlayer->FInViewCone(this) == false)
		{
			iChance *= 2;
		}
	}

	//Below 25% health they'll always sprint
	if ((GetHealth() > GetMaxHealth() * 0.5f))
	{
		if (IsStrategySlotRangeOccupied(SQUAD_SLOT_BASEZOMBIE_HEADLESS_SPRINT1, SQUAD_SLOT_BASEZOMBIE_HEADLESS_SPRINT2) == true)
			return false;

		if (random->RandomInt(0, 100) > iChance)
			return false;

		if (m_flSprintRestTime > gpGlobals->curtime)
			return false;
	}

	float flLength = (GetEnemy()->WorldSpaceCenter() - WorldSpaceCenter()).Length();

	if (flLength > MAX_SPRINT_DISTANCE)
		return false;

	return true;
}

void C1187_NPC_BaseZombie_Headless::StopSprint(void)
{
	GetNavigator()->SetMovementActivity(ACT_WALK);

	m_flSprintTime = gpGlobals->curtime;
	m_flSprintRestTime = m_flSprintTime + random->RandomFloat(2.5f, 5.0f);
}

void C1187_NPC_BaseZombie_Headless::Sprint(bool bMadSprint)
{
	if (IsSprinting())
		return;

	OccupyStrategySlotRange(SQUAD_SLOT_BASEZOMBIE_HEADLESS_SPRINT1, SQUAD_SLOT_BASEZOMBIE_HEADLESS_SPRINT2);
	GetNavigator()->SetMovementActivity(ACT_RUN);

	float flSprintTime = random->RandomFloat(MIN_SPRINT_TIME, MAX_SPRINT_TIME);

	//If holding a grenade then sprint until it blows up.
	if (bMadSprint == true)
	{
		flSprintTime = 9999;
	}

	m_flSprintTime = gpGlobals->curtime + flSprintTime;

	//Don't sprint for this long after I'm done with this sprint run.
	m_flSprintRestTime = m_flSprintTime + random->RandomFloat(2.5f, 5.0f);

	EmitSound("Zombine.Charge");
}

void C1187_NPC_BaseZombie_Headless::RunTask(const Task_t *pTask)
{
	switch (pTask->iTask)
	{
	case TASK_WAIT_FOR_MOVEMENT_STEP:
	case TASK_WAIT_FOR_MOVEMENT:
	{
		BaseClass::RunTask(pTask);

		if (IsOnFire() && IsSprinting())
		{
			StopSprint();
		}

		//Only do this if I have an enemy
		if (GetEnemy())
		{
			if (AllowedToSprint() == true)
			{
				Sprint((GetHealth() <= GetMaxHealth() * 0.5f));
				return;
			}

			if (GetNavigator()->GetMovementActivity() != ACT_WALK)
			{
				if (IsSprinting() == false)
				{
					GetNavigator()->SetMovementActivity(ACT_WALK);
				}
			}
		}
		else
		{
			GetNavigator()->SetMovementActivity(ACT_WALK);
		}

		break;
	}
	default:
	{
		BaseClass::RunTask(pTask);
		break;
	}
	}
}

void C1187_NPC_BaseZombie_Headless::InputStartSprint(inputdata_t &inputdata)
{
	Sprint();
}

//-----------------------------------------------------------------------------
// Purpose: Returns a moan sound for this class of zombie.
//-----------------------------------------------------------------------------
const char *C1187_NPC_BaseZombie_Headless::GetMoanSound(int nSound)
{
	return pMoanSounds[nSound % ARRAYSIZE(pMoanSounds)];
}

//-----------------------------------------------------------------------------
// Purpose: Sound of a footstep
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::FootstepSound(bool fRightFoot)
{
	if (fRightFoot)
	{
		EmitSound("Zombie.FootstepRight");
	}
	else
	{
		EmitSound("Zombie.FootstepLeft");
	}
}

//-----------------------------------------------------------------------------
// Purpose: Overloaded so that explosions don't split the zombine in twain.
//-----------------------------------------------------------------------------
bool C1187_NPC_BaseZombie_Headless::ShouldBecomeTorso(const CTakeDamageInfo &info, float flDamageThreshold)
{
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Sound of a foot sliding/scraping
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::FootscuffSound(bool fRightFoot)
{
	if (fRightFoot)
	{
		EmitSound("Zombine.ScuffRight");
	}
	else
	{
		EmitSound("Zombine.ScuffLeft");
	}
}

//-----------------------------------------------------------------------------
// Purpose: Play a random attack hit sound
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::AttackHitSound(void)
{
	EmitSound("Zombie.AttackHit");
}

//-----------------------------------------------------------------------------
// Purpose: Play a random attack miss sound
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::AttackMissSound(void)
{
	// Play a random attack miss sound
	EmitSound("Zombie.AttackMiss");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::PainSound(const CTakeDamageInfo &info)
{
	// We're constantly taking damage when we are on fire. Don't make all those noises!
	if (IsOnFire())
	{
		return;
	}

	EmitSound("Zombine.Pain");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::DeathSound(const CTakeDamageInfo &info)
{
	EmitSound("Zombine.Die");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::AlertSound(void)
{
	EmitSound("Zombine.Alert");

	// Don't let a moan sound cut off the alert sound.
	m_flNextMoanSound += random->RandomFloat(2.0, 4.0);
}

//-----------------------------------------------------------------------------
// Purpose: Play a random idle sound.
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::IdleSound(void)
{
	if (GetState() == NPC_STATE_IDLE && random->RandomFloat(0, 1) == 0)
	{
		// Moan infrequently in IDLE state.
		return;
	}

	if (IsSlumped())
	{
		// Sleeping zombies are quiet.
		return;
	}

	EmitSound("Zombine.Idle");
	MakeAISpookySound(360.0f);
}

//-----------------------------------------------------------------------------
// Purpose: Play a random attack sound.
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::AttackSound(void)
{

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char *C1187_NPC_BaseZombie_Headless::GetHeadcrabModel(void)
{
	return "models/headcrabclassic.mdl";
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *C1187_NPC_BaseZombie_Headless::GetLegsModel(void)
{
	return "models/zombie/zombie_soldier_legs.mdl";
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char *C1187_NPC_BaseZombie_Headless::GetTorsoModel(void)
{
	return "models/zombie/zombie_soldier_torso.mdl";
}

//---------------------------------------------------------
// Classic zombie only uses moan sound if on fire.
//---------------------------------------------------------
void C1187_NPC_BaseZombie_Headless::MoanSound(envelopePoint_t *pEnvelope, int iEnvelopeSize)
{
	if (IsOnFire())
	{
		BaseClass::MoanSound(pEnvelope, iEnvelopeSize);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the classname (ie "npc_headcrab") to spawn when our headcrab bails.
//-----------------------------------------------------------------------------
const char *C1187_NPC_BaseZombie_Headless::GetHeadcrabClassname(void)
{
	return "npc_headcrab";
}

//-----------------------------------------------------------------------------
//
// Schedules
//
//-----------------------------------------------------------------------------

AI_BEGIN_CUSTOM_NPC(npc_basezombie_headless, C1187_NPC_BaseZombie_Headless)

//Squad slots
DECLARE_SQUADSLOT(SQUAD_SLOT_BASEZOMBIE_HEADLESS_SPRINT1)
DECLARE_SQUADSLOT(SQUAD_SLOT_BASEZOMBIE_HEADLESS_SPRINT2)

DECLARE_ACTIVITY(ACT_ZOMBINE_ATTACK_FAST)

AI_END_CUSTOM_NPC()



