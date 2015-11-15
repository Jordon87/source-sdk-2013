//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"

#include "doors.h"

#include "simtimer.h"
#include "npc_BaseZombie.h"
#include "ai_hull.h"
#include "ai_navigator.h"
#include "ai_memory.h"
#include "gib.h"
#include "soundenvelope.h"
#include "engine/IEngineSound.h"
#include "ammodef.h"

#include  "1187_npc_basezombie_headless_classic.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// ACT_FLINCH_PHYSICS

extern envelopePoint_t envZombieMoanVolumeFast[2];
extern envelopePoint_t envZombieMoanVolume[3];
extern envelopePoint_t envZombieMoanVolumeLong[3];
extern envelopePoint_t envZombieMoanIgnited[3];

extern int ACT_ZOMBIE_TANTRUM;
extern int ACT_ZOMBIE_WALLPOUND;

BEGIN_DATADESC(C1187_NPC_BaseZombie_Headless_Classic)

DEFINE_FIELD(m_hBlockingDoor, FIELD_EHANDLE),
DEFINE_FIELD(m_flDoorBashYaw, FIELD_FLOAT),
DEFINE_EMBEDDED(m_DurationDoorBash),
DEFINE_EMBEDDED(m_NextTimeToStartDoorBash),
DEFINE_FIELD(m_vPositionCharged, FIELD_POSITION_VECTOR),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_NPC_BaseZombie_Headless_Classic::PrescheduleThink(void)
{
	if (gpGlobals->curtime > m_flNextMoanSound)
	{
		if (CanPlayMoanSound())
		{
			// Classic guy idles instead of moans.
			IdleSound();

			m_flNextMoanSound = gpGlobals->curtime + random->RandomFloat(2.0, 5.0);
		}
		else
		{
			m_flNextMoanSound = gpGlobals->curtime + random->RandomFloat(1.0, 2.0);
		}
	}

	BaseClass::PrescheduleThink();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int C1187_NPC_BaseZombie_Headless_Classic::SelectSchedule(void)
{
	if (HasCondition(COND_PHYSICS_DAMAGE) && !m_ActBusyBehavior.IsActive())
	{
		return SCHED_FLINCH_PHYSICS;
	}

	return BaseClass::SelectSchedule();
}

//---------------------------------------------------------
//---------------------------------------------------------
bool C1187_NPC_BaseZombie_Headless_Classic::ShouldBecomeTorso(const CTakeDamageInfo &info, float flDamageThreshold)
{
	if (IsSlumped())
	{
		// Never break apart a slouched zombie. This is because the most fun
		// slouched zombies to kill are ones sleeping leaning against explosive
		// barrels. If you break them in half in the blast, the force of being
		// so close to the explosion makes the body pieces fly at ridiculous 
		// velocities because the pieces weigh less than the whole.
		return false;
	}

	return BaseClass::ShouldBecomeTorso(info, flDamageThreshold);
}

//---------------------------------------------------------
//---------------------------------------------------------
void C1187_NPC_BaseZombie_Headless_Classic::GatherConditions(void)
{
	BaseClass::GatherConditions();

	static int conditionsToClear[] =
	{
		COND_HEADLESS_CLASSIC_BLOCKED_BY_DOOR,
		COND_HEADLESS_CLASSIC_DOOR_OPENED,
		COND_HEADLESS_CLASSIC_CHARGE_TARGET_MOVED,
	};

	ClearConditions(conditionsToClear, ARRAYSIZE(conditionsToClear));

	if (m_hBlockingDoor == NULL ||
		(m_hBlockingDoor->m_toggle_state == TS_AT_TOP ||
		m_hBlockingDoor->m_toggle_state == TS_GOING_UP))
	{
		ClearCondition(COND_HEADLESS_CLASSIC_BLOCKED_BY_DOOR);
		if (m_hBlockingDoor != NULL)
		{
			SetCondition(COND_HEADLESS_CLASSIC_DOOR_OPENED);
			m_hBlockingDoor = NULL;
		}
	}
	else
		SetCondition(COND_HEADLESS_CLASSIC_BLOCKED_BY_DOOR);

	if (ConditionInterruptsCurSchedule(COND_HEADLESS_CLASSIC_CHARGE_TARGET_MOVED))
	{
		if (GetNavigator()->IsGoalActive())
		{
			const float CHARGE_RESET_TOLERANCE = 60.0;
			if (!GetEnemy() ||
				(m_vPositionCharged - GetEnemyLKP()).Length() > CHARGE_RESET_TOLERANCE)
			{
				SetCondition(COND_HEADLESS_CLASSIC_CHARGE_TARGET_MOVED);
			}

		}
	}
}

//---------------------------------------------------------
//---------------------------------------------------------

int C1187_NPC_BaseZombie_Headless_Classic::SelectFailSchedule(int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode)
{
	if (HasCondition(COND_HEADLESS_CLASSIC_BLOCKED_BY_DOOR) && m_hBlockingDoor != NULL)
	{
		ClearCondition(COND_HEADLESS_CLASSIC_BLOCKED_BY_DOOR);
		if (m_NextTimeToStartDoorBash.Expired() && failedSchedule != SCHED_HEADLESS_CLASSIC_BASH_DOOR)
			return SCHED_HEADLESS_CLASSIC_BASH_DOOR;
		m_hBlockingDoor = NULL;
	}

	if (failedSchedule != SCHED_HEADLESS_CLASSIC_CHARGE_ENEMY &&
		IsPathTaskFailure(taskFailCode) &&
		random->RandomInt(1, 100) < 50)
	{
		return SCHED_HEADLESS_CLASSIC_CHARGE_ENEMY;
	}

	if (failedSchedule != SCHED_HEADLESS_CLASSIC_WANDER_ANGRILY &&
		(failedSchedule == SCHED_TAKE_COVER_FROM_ENEMY ||
		failedSchedule == SCHED_CHASE_ENEMY_FAILED))
	{
		return SCHED_HEADLESS_CLASSIC_WANDER_ANGRILY;
	}

	return BaseClass::SelectFailSchedule(failedSchedule, failedTask, taskFailCode);
}

//---------------------------------------------------------
//---------------------------------------------------------

int C1187_NPC_BaseZombie_Headless_Classic::TranslateSchedule(int scheduleType)
{
	if (scheduleType == SCHED_COMBAT_FACE && IsUnreachable(GetEnemy()))
		return SCHED_TAKE_COVER_FROM_ENEMY;

	if (!m_fIsTorso && scheduleType == SCHED_FAIL)
		return SCHED_HEADLESS_CLASSIC_FAIL;

	return BaseClass::TranslateSchedule(scheduleType);
}

//---------------------------------------------------------

Activity C1187_NPC_BaseZombie_Headless_Classic::NPC_TranslateActivity(Activity newActivity)
{
	newActivity = BaseClass::NPC_TranslateActivity(newActivity);

	if (newActivity == ACT_RUN)
		return ACT_WALK;

	if (m_fIsTorso && (newActivity == ACT_ZOMBIE_TANTRUM))
		return ACT_IDLE;

	return newActivity;
}

//---------------------------------------------------------
//---------------------------------------------------------
void C1187_NPC_BaseZombie_Headless_Classic::OnStateChange(NPC_STATE OldState, NPC_STATE NewState)
{
	BaseClass::OnStateChange(OldState, NewState);
}

//---------------------------------------------------------
//---------------------------------------------------------

void C1187_NPC_BaseZombie_Headless_Classic::StartTask(const Task_t *pTask)
{
	switch (pTask->iTask)
	{
	case TASK_HEADLESS_CLASSIC_EXPRESS_ANGER:
	{
		if (random->RandomInt(1, 4) == 2)
		{
			SetIdealActivity((Activity)ACT_ZOMBIE_TANTRUM);
		}
		else
		{
			TaskComplete();
		}

		break;
	}

	case TASK_HEADLESS_CLASSIC_YAW_TO_DOOR:
	{
		AssertMsg(m_hBlockingDoor != NULL, "Expected condition handling to break schedule before landing here");
		if (m_hBlockingDoor != NULL)
		{
			GetMotor()->SetIdealYaw(m_flDoorBashYaw);
		}
		TaskComplete();
		break;
	}

	case TASK_HEADLESS_CLASSIC_ATTACK_DOOR:
	{
		m_DurationDoorBash.Reset();
		SetIdealActivity(SelectDoorBash());
		break;
	}

	case TASK_HEADLESS_CLASSIC_CHARGE_ENEMY:
	{
		if (!GetEnemy())
			TaskFail(FAIL_NO_ENEMY);
		else if (GetNavigator()->SetVectorGoalFromTarget(GetEnemy()->GetLocalOrigin()))
		{
			m_vPositionCharged = GetEnemy()->GetLocalOrigin();
			TaskComplete();
		}
		else
			TaskFail(FAIL_NO_ROUTE);
		break;
	}

	default:
		BaseClass::StartTask(pTask);
		break;
	}
}

//---------------------------------------------------------
//---------------------------------------------------------

void C1187_NPC_BaseZombie_Headless_Classic::RunTask(const Task_t *pTask)
{
	switch (pTask->iTask)
	{
	case TASK_HEADLESS_CLASSIC_ATTACK_DOOR:
	{
		if (IsActivityFinished())
		{
			if (m_DurationDoorBash.Expired())
			{
				TaskComplete();
				m_NextTimeToStartDoorBash.Reset();
			}
			else
				ResetIdealActivity(SelectDoorBash());
		}
		break;
	}

	case TASK_HEADLESS_CLASSIC_CHARGE_ENEMY:
	{
		break;
	}

	case TASK_HEADLESS_CLASSIC_EXPRESS_ANGER:
	{
		if (IsActivityFinished())
		{
			TaskComplete();
		}
		break;
	}

	default:
		BaseClass::RunTask(pTask);
		break;
	}
}

//---------------------------------------------------------
//---------------------------------------------------------

bool C1187_NPC_BaseZombie_Headless_Classic::OnObstructingDoor(AILocalMoveGoal_t *pMoveGoal, CBaseDoor *pDoor,
	float distClear, AIMoveResult_t *pResult)
{
	if (BaseClass::OnObstructingDoor(pMoveGoal, pDoor, distClear, pResult))
	{
		if (IsMoveBlocked(*pResult) && pMoveGoal->directTrace.vHitNormal != vec3_origin)
		{
			m_hBlockingDoor = pDoor;
			m_flDoorBashYaw = UTIL_VecToYaw(pMoveGoal->directTrace.vHitNormal * -1);
		}
		return true;
	}

	return false;
}

//---------------------------------------------------------
//---------------------------------------------------------

Activity C1187_NPC_BaseZombie_Headless_Classic::SelectDoorBash()
{
	if (random->RandomInt(1, 3) == 1)
		return ACT_MELEE_ATTACK1;
	return (Activity)ACT_ZOMBIE_WALLPOUND;
}

//---------------------------------------------------------
// Zombies should scream continuously while burning, so long
// as they are alive... but NOT IN GERMANY!
//---------------------------------------------------------
void C1187_NPC_BaseZombie_Headless_Classic::Ignite(float flFlameLifetime, bool bNPCOnly, float flSize, bool bCalledByLevelDesigner)
{
	if (!IsOnFire() && IsAlive())
	{
		BaseClass::Ignite(flFlameLifetime, bNPCOnly, flSize, bCalledByLevelDesigner);

		if (!UTIL_IsLowViolence())
		{
			RemoveSpawnFlags(SF_NPC_GAG);

			MoanSound(envZombieMoanIgnited, ARRAYSIZE(envZombieMoanIgnited));

			if (m_pMoanSound)
			{
				ENVELOPE_CONTROLLER.SoundChangePitch(m_pMoanSound, 120, 1.0);
				ENVELOPE_CONTROLLER.SoundChangeVolume(m_pMoanSound, 1, 1.0);
			}
		}
	}
}

//---------------------------------------------------------
// If a zombie stops burning and hasn't died, quiet him down
//---------------------------------------------------------
void C1187_NPC_BaseZombie_Headless_Classic::Extinguish()
{
	if (m_pMoanSound)
	{
		ENVELOPE_CONTROLLER.SoundChangeVolume(m_pMoanSound, 0, 2.0);
		ENVELOPE_CONTROLLER.SoundChangePitch(m_pMoanSound, 100, 2.0);
		m_flNextMoanSound = gpGlobals->curtime + random->RandomFloat(2.0, 4.0);
	}

	BaseClass::Extinguish();
}

//---------------------------------------------------------
//---------------------------------------------------------
int C1187_NPC_BaseZombie_Headless_Classic::OnTakeDamage_Alive(const CTakeDamageInfo &inputInfo)
{
#ifndef HL2_EPISODIC
	if (inputInfo.GetDamageType() & DMG_BUCKSHOT)
	{
		if (!m_fIsTorso && inputInfo.GetDamage() > (m_iMaxHealth / 3))
		{
			// Always flinch if damaged a lot by buckshot, even if not shot in the head.
			// The reason for making sure we did at least 1/3rd of the zombie's max health
			// is so the zombie doesn't flinch every time the odd shotgun pellet hits them,
			// and so the maximum number of times you'll see a zombie flinch like this is 2.(sjb)
			AddGesture(ACT_GESTURE_FLINCH_HEAD);
		}
	}
#endif // HL2_EPISODIC

	return BaseClass::OnTakeDamage_Alive(inputInfo);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool C1187_NPC_BaseZombie_Headless_Classic::IsHeavyDamage(const CTakeDamageInfo &info)
{
#ifdef HL2_EPISODIC
	if (info.GetDamageType() & DMG_BUCKSHOT)
	{
		if (!m_fIsTorso && info.GetDamage() > (m_iMaxHealth / 3))
			return true;
	}

	// Randomly treat all damage as heavy
	if (info.GetDamageType() & (DMG_BULLET | DMG_BUCKSHOT))
	{
		// Don't randomly flinch if I'm melee attacking
		if (!HasCondition(COND_CAN_MELEE_ATTACK1) && (RandomFloat() > 0.5))
		{
			// Randomly forget I've flinched, so that I'll be forced to play a big flinch
			// If this doesn't happen, it means I may not fully flinch if I recently flinched
			if (RandomFloat() > 0.75)
			{
				Forget(bits_MEMORY_FLINCHED);
			}

			return true;
		}
	}
#endif // HL2_EPISODIC

	return BaseClass::IsHeavyDamage(info);
}

//---------------------------------------------------------
//---------------------------------------------------------
#define ZOMBIE_SQUASH_MASS	300.0f  // Anything this heavy or heavier squashes a zombie good. (show special fx)
bool C1187_NPC_BaseZombie_Headless_Classic::IsSquashed(const CTakeDamageInfo &info)
{
	if (GetHealth() > 0)
	{
		return false;
	}

	if (info.GetDamageType() & DMG_CRUSH)
	{
		IPhysicsObject *pCrusher = info.GetInflictor()->VPhysicsGetObject();
		if (pCrusher && pCrusher->GetMass() >= ZOMBIE_SQUASH_MASS && info.GetInflictor()->WorldSpaceCenter().z > EyePosition().z)
		{
			// This heuristic detects when a zombie has been squashed from above by a heavy
			// item. Done specifically so we can add gore effects to Ravenholm cartraps.
			// The zombie must take physics damage from a 300+kg object that is centered above its eyes (comes from above)
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------
//---------------------------------------------------------
void C1187_NPC_BaseZombie_Headless_Classic::BuildScheduleTestBits(void)
{
	BaseClass::BuildScheduleTestBits();

	if (!m_fIsTorso && !IsCurSchedule(SCHED_FLINCH_PHYSICS) && !m_ActBusyBehavior.IsActive())
	{
		SetCustomInterruptCondition(COND_PHYSICS_DAMAGE);
	}
}


//=============================================================================

AI_BEGIN_CUSTOM_NPC(npc_zombie, C1187_NPC_BaseZombie_Headless_Classic)

DECLARE_CONDITION(COND_HEADLESS_CLASSIC_BLOCKED_BY_DOOR)
DECLARE_CONDITION(COND_HEADLESS_CLASSIC_DOOR_OPENED)
DECLARE_CONDITION(COND_HEADLESS_CLASSIC_CHARGE_TARGET_MOVED)

DECLARE_TASK(TASK_HEADLESS_CLASSIC_EXPRESS_ANGER)
DECLARE_TASK(TASK_HEADLESS_CLASSIC_YAW_TO_DOOR)
DECLARE_TASK(TASK_HEADLESS_CLASSIC_ATTACK_DOOR)
DECLARE_TASK(TASK_HEADLESS_CLASSIC_CHARGE_ENEMY)

DECLARE_ACTIVITY(ACT_ZOMBIE_TANTRUM);
DECLARE_ACTIVITY(ACT_ZOMBIE_WALLPOUND);

DEFINE_SCHEDULE
(
SCHED_HEADLESS_CLASSIC_BASH_DOOR,

"	Tasks"
"		TASK_SET_ACTIVITY				ACTIVITY:ACT_ZOMBIE_TANTRUM"
"		TASK_SET_FAIL_SCHEDULE			SCHEDULE:SCHED_TAKE_COVER_FROM_ENEMY"
"		TASK_HEADLESS_CLASSIC_YAW_TO_DOOR			0"
"		TASK_FACE_IDEAL					0"
"		TASK_HEADLESS_CLASSIC_ATTACK_DOOR			0"
""
"	Interrupts"
"		COND_ZOMBIE_RELEASECRAB"
"		COND_ENEMY_DEAD"
"		COND_NEW_ENEMY"
"		COND_HEADLESS_CLASSIC_DOOR_OPENED"
)

DEFINE_SCHEDULE
(
SCHED_HEADLESS_CLASSIC_WANDER_ANGRILY,

"	Tasks"
"		TASK_WANDER						480240" // 48 units to 240 units.
"		TASK_WALK_PATH					0"
"		TASK_WAIT_FOR_MOVEMENT			4"
""
"	Interrupts"
"		COND_ZOMBIE_RELEASECRAB"
"		COND_ENEMY_DEAD"
"		COND_NEW_ENEMY"
"		COND_HEADLESS_CLASSIC_DOOR_OPENED"
)

DEFINE_SCHEDULE
(
SCHED_HEADLESS_CLASSIC_CHARGE_ENEMY,


"	Tasks"
"		TASK_HEADLESS_CLASSIC_CHARGE_ENEMY		0"
"		TASK_WALK_PATH					0"
"		TASK_WAIT_FOR_MOVEMENT			0"
"		TASK_PLAY_SEQUENCE				ACTIVITY:ACT_ZOMBIE_TANTRUM" /* placeholder until frustration/rage/fence shake animation available */
""
"	Interrupts"
"		COND_ZOMBIE_RELEASECRAB"
"		COND_ENEMY_DEAD"
"		COND_NEW_ENEMY"
"		COND_HEADLESS_CLASSIC_DOOR_OPENED"
"		COND_HEADLESS_CLASSIC_CHARGE_TARGET_MOVED"
)

DEFINE_SCHEDULE
(
SCHED_HEADLESS_CLASSIC_FAIL,

"	Tasks"
"		TASK_STOP_MOVING		0"
"		TASK_SET_ACTIVITY		ACTIVITY:ACT_ZOMBIE_TANTRUM"
"		TASK_WAIT				1"
"		TASK_WAIT_PVS			0"
""
"	Interrupts"
"		COND_CAN_RANGE_ATTACK1 "
"		COND_CAN_RANGE_ATTACK2 "
"		COND_CAN_MELEE_ATTACK1 "
"		COND_CAN_MELEE_ATTACK2"
"		COND_GIVE_WAY"
"		COND_HEADLESS_CLASSIC_DOOR_OPENED"
)

AI_END_CUSTOM_NPC()

//=============================================================================
