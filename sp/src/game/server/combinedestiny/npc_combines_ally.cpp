//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//


#include "cbase.h"
#include "npc_combines_ally.h"

#include "ai_hint.h"
#include "ai_localnavigator.h"
#include "ai_memory.h"
#include "ai_pathfinder.h"
#include "ai_route.h"
#include "ai_senses.h"
#include "ai_squad.h"
#include "ai_squadslot.h"
#include "ai_tacticalservices.h"
#include "ai_interactions.h"

#include "globalstate.h"
#include "hl2_player.h"

#include "eventqueue.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar player_squad_autosummon_time;
extern ConVar player_squad_autosummon_move_tolerance;
extern ConVar player_squad_autosummon_player_tolerance;
extern ConVar player_squad_autosummon_time_after_combat;
extern ConVar player_squad_autosummon_debug;

extern ConVar g_debug_transitions;

static const int MAX_PLAYER_SQUAD = 4;

ConVar	npc_combine_explosive_resist("npc_combine_explosive_resist", "0");
ConVar	npc_combines_auto_player_squad("npc_combine_auto_player_squad", "1");
ConVar	npc_combines_auto_player_squad_allow_use("npc_combine_auto_player_squad_allow_use", "0");

ConVar	ai_combine_debug_commander("ai_combine_debug_commander", "1");
#define DebuggingCommanderMode() (ai_combine_debug_commander.GetBool() && (m_debugOverlays & OVERLAY_NPC_SELECTED_BIT))


#define ShouldAutosquad() (npc_combines_auto_player_squad.GetBool())

#define COMBINE_TRANSITION_SEARCH_DISTANCE		(100*12)

//================================================================================
// CNPC_CombineS_Ally
//================================================================================

LINK_ENTITY_TO_CLASS(npc_combine_s, CNPC_CombineS_Ally);

//---------------------------------
// Save & Restore
//---------------------------------

BEGIN_DATADESC(CNPC_CombineS_Ally)

	DEFINE_FIELD(m_bMovingAwayFromPlayer,		FIELD_BOOLEAN),

	DEFINE_FIELD(m_iszOriginalSquad,			FIELD_STRING),
	DEFINE_FIELD(m_flTimeJoinedPlayerSquad,		FIELD_TIME),
	DEFINE_FIELD(m_bWasInPlayerSquad,			FIELD_BOOLEAN),
	DEFINE_FIELD(m_flTimeLastCloseToPlayer,		FIELD_TIME),
	DEFINE_EMBEDDED(m_AutoSummonTimer),
	DEFINE_FIELD(m_vAutoSummonAnchor,			FIELD_POSITION_VECTOR),

	DEFINE_FIELD(m_hSavedFollowGoalEnt,			FIELD_EHANDLE),

	//
	// Keyfields
	//

	DEFINE_KEYFIELD(m_bNeverLeavePlayerSquad,	FIELD_BOOLEAN,		"neverleaveplayersquad"),
	DEFINE_KEYFIELD(m_iszDenyCommandConcept,	FIELD_STRING,		"denycommandconcept"),

	DEFINE_KEYFIELD(m_bAlwaysTransition,		FIELD_BOOLEAN,		"AlwaysTransition"),

	//
	// Inputs
	//

	DEFINE_INPUTFUNC(FIELD_VOID,		"OutsideTransition",		InputOutsideTransition),
	DEFINE_INPUTFUNC(FIELD_VOID,		"RemoveFromPlayerSquad",	InputRemoveFromPlayerSquad),
	DEFINE_INPUTFUNC(FIELD_VOID,		"SetCommandable",			InputSetCommandable),

	DEFINE_INPUTFUNC(FIELD_VOID,		"EnableAlwaysTransition",	InputEnableAlwaysTransition),
	DEFINE_INPUTFUNC(FIELD_VOID,		"DisableAlwaysTransition",	InputDisableAlwaysTransition),

	//
	// Outputs
	//

	DEFINE_OUTPUT(m_OnJoinedPlayerSquad, "OnJoinedPlayerSquad"),
	DEFINE_OUTPUT(m_OnLeftPlayerSquad,	 "OnLeftPlayerSquad"),
	DEFINE_OUTPUT(m_OnFollowOrder,		 "OnFollowOrder"),
	DEFINE_OUTPUT(m_OnStationOrder,		 "OnStationOrder"),
	DEFINE_OUTPUT(m_OnPlayerUse,		 "OnPlayerUse"),

	//
	// Function pointers
	//

	DEFINE_USEFUNC(CommanderUse),
	DEFINE_USEFUNC(SimpleUse),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

CSimpleSimTimer CNPC_CombineS_Ally::gm_PlayerSquadEvaluateTimer;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define COMMAND_POINT_CLASSNAME "info_target_command_point"

void CNPC_CombineS_Ally::Spawn(void)
{
	AddSpawnFlags(SF_COMBINE_FOLLOW);

	CapabilitiesAdd(bits_CAP_NO_HIT_PLAYER | bits_CAP_NO_HIT_SQUADMATES | bits_CAP_FRIENDLY_DMG_IMMUNE);

	m_iszOriginalSquad = m_SquadName;

	BaseClass::Spawn();

	if (ShouldAutosquad())
	{
		if (m_SquadName == GetPlayerSquadName())
		{
			CAI_Squad *pPlayerSquad = g_AI_SquadManager.FindSquad(GetPlayerSquadName());
			if (pPlayerSquad && pPlayerSquad->NumMembers() >= MAX_PLAYER_SQUAD)
				m_SquadName = NULL_STRING;
		}
		gm_PlayerSquadEvaluateTimer.Force();
	}

	SetUse(&CNPC_CombineS_Ally::CommanderUse);
	Assert(!ShouldAutosquad() || !IsInPlayerSquad());

	m_bWasInPlayerSquad = IsInPlayerSquad();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::PostNPCInit()
{
	if (!gEntList.FindEntityByClassname(NULL, COMMAND_POINT_CLASSNAME))
	{
		CreateEntityByName(COMMAND_POINT_CLASSNAME);
	}

	if (IsInPlayerSquad())
	{
		if (m_pSquad->NumMembers() > MAX_PLAYER_SQUAD)
			DevMsg("Error: Spawning citizen in player squad but exceeds squad limit of %d members\n", MAX_PLAYER_SQUAD);

		FixupPlayerSquad();
	}
	else
	{
		if ((m_spawnflags & SF_COMBINE_FOLLOW) && AI_IsSinglePlayer())
		{
			m_FollowBehavior.SetFollowTarget(UTIL_GetLocalPlayer());
			m_FollowBehavior.SetParameters(AIF_SIMPLE);
		}
	}

	BaseClass::PostNPCInit();
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::OnRestore()
{
	gm_PlayerSquadEvaluateTimer.Force();

	BaseClass::OnRestore();

	if (!gEntList.FindEntityByClassname(NULL, COMMAND_POINT_CLASSNAME))
	{
		CreateEntityByName(COMMAND_POINT_CLASSNAME);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::ShouldAlwaysThink()
{
	return (BaseClass::ShouldAlwaysThink() || (m_FollowBehavior.GetFollowTarget() && m_FollowBehavior.GetFollowTarget()->IsPlayer()));
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define COMBINE_FOLLOWER_DESERT_FUNCTANK_DIST	45.0f*12.0f
bool CNPC_CombineS_Ally::ShouldBehaviorSelectSchedule(CAI_BehaviorBase *pBehavior)
{
	if (pBehavior == &m_FollowBehavior)
	{
		// Suppress follow behavior if I have a func_tank and the func tank is near
		// what I'm supposed to be following.
		if (m_FuncTankBehavior.CanSelectSchedule())
		{
			// Is the tank close to the follow target?
			Vector vecTank = m_FuncTankBehavior.GetFuncTank()->WorldSpaceCenter();
			Vector vecFollowGoal = m_FollowBehavior.GetFollowGoalInfo().position;

			float flTankDistSqr = (vecTank - vecFollowGoal).LengthSqr();
			float flAllowDist = m_FollowBehavior.GetFollowGoalInfo().followPointTolerance * 2.0f;
			float flAllowDistSqr = flAllowDist * flAllowDist;
			if (flTankDistSqr < flAllowDistSqr)
			{
				// Deny follow behavior so the tank can go.
				return false;
			}
		}
	}
	else if (IsInPlayerSquad() && pBehavior == &m_FuncTankBehavior && m_FuncTankBehavior.IsMounted())
	{
		if (m_FollowBehavior.GetFollowTarget())
		{
			Vector vecFollowGoal = m_FollowBehavior.GetFollowTarget()->GetAbsOrigin();
			if (vecFollowGoal.DistToSqr(GetAbsOrigin()) > Square(COMBINE_FOLLOWER_DESERT_FUNCTANK_DIST))
			{
				return false;
			}
		}
	}

	return BaseClass::ShouldBehaviorSelectSchedule(pBehavior);
}

void CNPC_CombineS_Ally::GatherConditions()
{
	BaseClass::GatherConditions();

	if (IsInPlayerSquad())
	{
		// Leave the player squad if someone has made me neutral to player.
		if (IRelationType(UTIL_GetLocalPlayer()) == D_NU)
		{
			RemoveFromPlayerSquad();
		}
	}

	if (AI_IsSinglePlayer())
	{
		CBasePlayer *pPlayer = UTIL_GetLocalPlayer();

		if (Classify() == CLASS_COMBINE)
		{
			bool bInPlayerSquad = (m_pSquad && MAKE_STRING(m_pSquad->GetName()) == GetPlayerSquadName());
			if (bInPlayerSquad)
			{
				if (GetState() == NPC_STATE_SCRIPT || (!HasCondition(COND_SEE_PLAYER) && (GetAbsOrigin() - pPlayer->GetAbsOrigin()).LengthSqr() > Square(50 * 12)))
				{
					RemoveFromSquad();
				}
			}
			else if (GetState() != NPC_STATE_SCRIPT)
			{
				if (HasCondition(COND_SEE_PLAYER) && (GetAbsOrigin() - pPlayer->GetAbsOrigin()).LengthSqr() < Square(25 * 12))
				{
					if (hl2_episodic.GetBool())
					{
						// Don't stomp our squad if we're in one
						if (GetSquad() == NULL)
						{
							AddToSquad(GetPlayerSquadName());
						}
					}
					else
					{
						AddToSquad(GetPlayerSquadName());
					}
				}
			}
		}

		m_flBoostSpeed = 0;

		if (m_FollowBehavior.GetFollowTarget() &&
			(m_FollowBehavior.GetFollowTarget()->IsPlayer() || GetCommandGoal() != vec3_invalid) &&
			m_FollowBehavior.IsMovingToFollowTarget() &&
			m_FollowBehavior.GetGoalRange() > 0.1 &&
			BaseClass::GetIdealSpeed() > 0.1)
		{
			Vector vPlayerToFollower = GetAbsOrigin() - pPlayer->GetAbsOrigin();
			float dist = vPlayerToFollower.NormalizeInPlace();

			bool bDoSpeedBoost = false;
			if (!HasCondition(COND_IN_PVS))
				bDoSpeedBoost = true;
			else if (m_FollowBehavior.GetFollowTarget()->IsPlayer())
			{
				if (dist > m_FollowBehavior.GetGoalRange() * 2)
				{
					float dot = vPlayerToFollower.Dot(pPlayer->EyeDirection3D());
					if (dot < 0)
					{
						bDoSpeedBoost = true;
					}
				}
			}

			if (bDoSpeedBoost)
			{
				float lag = dist / m_FollowBehavior.GetGoalRange();

				float mult;

				if (lag > 10.0)
					mult = 2.0;
				else if (lag > 5.0)
					mult = 1.5;
				else if (lag > 3.0)
					mult = 1.25;
				else
					mult = 1.1;

				m_flBoostSpeed = pPlayer->GetSmoothedVelocity().Length();

				if (m_flBoostSpeed < BaseClass::GetIdealSpeed())
					m_flBoostSpeed = BaseClass::GetIdealSpeed();

				m_flBoostSpeed *= mult;
			}
		}
	}


	PredictPlayerPush();

	if (GetMotor()->IsDeceleratingToGoal() && IsCurTaskContinuousMove() &&
		HasCondition(COND_PLAYER_PUSHING) && IsCurSchedule(SCHED_MOVE_AWAY))
	{
		ClearSchedule("Being pushed by player");
	}

}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::PrescheduleThink()
{
	BaseClass::PrescheduleThink();

	UpdatePlayerSquad();
	UpdateFollowCommandPoint();

	if (DebuggingCommanderMode())
	{
		if (HaveCommandGoal())
		{
			CBaseEntity *pCommandPoint = gEntList.FindEntityByClassname(NULL, COMMAND_POINT_CLASSNAME);

			if (pCommandPoint)
			{
				NDebugOverlay::Cross3D(pCommandPoint->GetAbsOrigin(), 16, 0, 255, 255, false, 0.1);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Allows for modification of the interrupt mask for the current schedule.
//			In the most cases the base implementation should be called first.
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::BuildScheduleTestBits()
{
	BaseClass::BuildScheduleTestBits();

	// Always interrupt to get into the car
	// SetCustomInterruptCondition(COND_PC_BECOMING_PASSENGER);

	if (IsCurSchedule(SCHED_RANGE_ATTACK1))
	{
		SetCustomInterruptCondition(COND_PLAYER_PUSHING);
	}

	if ((ConditionInterruptsCurSchedule(COND_GIVE_WAY) ||
		IsCurSchedule(SCHED_HIDE_AND_RELOAD) ||
		IsCurSchedule(SCHED_RELOAD) ||
		IsCurSchedule(SCHED_STANDOFF) ||
		IsCurSchedule(SCHED_TAKE_COVER_FROM_ENEMY) ||
		IsCurSchedule(SCHED_COMBAT_FACE) ||
		IsCurSchedule(SCHED_ALERT_FACE) ||
		IsCurSchedule(SCHED_COMBAT_STAND) ||
		IsCurSchedule(SCHED_ALERT_FACE_BESTSOUND) ||
		IsCurSchedule(SCHED_ALERT_STAND)))
	{
		SetCustomInterruptCondition(COND_HEAR_MOVE_AWAY);
		SetCustomInterruptCondition(COND_PLAYER_PUSHING);
		// SetCustomInterruptCondition(COND_PC_HURTBYFIRE);
	}
}

int CNPC_CombineS_Ally::SelectSchedule()
{
	m_bMovingAwayFromPlayer = false;

	int schedule;

	schedule = SelectSchedulePriorityAction();
	if (schedule != SCHED_NONE)
		return schedule;

	if (ShouldDeferToFollowBehavior())
	{
		DeferSchedulingToBehavior(&m_FollowBehavior);
	}

	return BaseClass::SelectSchedule();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CNPC_CombineS_Ally::SelectSchedulePriorityAction()
{
	int schedule = SelectSchedulePlayerPush();
	if (schedule != SCHED_NONE)
	{
		if (m_FollowBehavior.IsRunning())
			KeepRunningBehavior();
		return schedule;
	}

	return SCHED_NONE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CNPC_CombineS_Ally::SelectSchedulePlayerPush()
{
	if (HasCondition(COND_PLAYER_PUSHING) && !IsInAScript() && !IgnorePlayerPushing())
	{
		// Ignore move away before gordon becomes the man
		if (GlobalEntity_GetState("gordon_precriminal") != GLOBAL_ON)
		{
			m_bMovingAwayFromPlayer = true;
			return SCHED_MOVE_AWAY;
		}
	}

	return SCHED_NONE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::ShouldDeferToFollowBehavior()
{
	if (!m_FollowBehavior.CanSelectSchedule() || !m_FollowBehavior.FarFromFollowTarget())
		return false;

	if (m_StandoffBehavior.CanSelectSchedule() && !m_StandoffBehavior.IsBehindBattleLines(m_FollowBehavior.GetFollowTarget()->GetAbsOrigin()))
		return false;

	if (HasCondition(COND_BETTER_WEAPON_AVAILABLE) && !GetActiveWeapon())
	{
		// Unarmed allies should arm themselves as soon as the opportunity presents itself.
		return false;
	}

	// Even though assault and act busy are placed ahead of the follow behavior in precedence, the below
	// code is necessary because we call ShouldDeferToFollowBehavior BEFORE we call the generic
	// BehaviorSelectSchedule, which tries the behaviors in priority order.
	if (m_AssaultBehavior.CanSelectSchedule() && hl2_episodic.GetBool())
	{
		return false;
	}

	if (hl2_episodic.GetBool())
	{
		if (m_ActBusyBehavior.CanSelectSchedule() && m_ActBusyBehavior.IsCombatActBusy())
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CNPC_CombineS_Ally::Touch(CBaseEntity *pOther)
{
	BaseClass::Touch(pOther);

	// Did the player touch me?
	if (pOther->IsPlayer() || (pOther->VPhysicsGetObject() && (pOther->VPhysicsGetObject()->GetGameFlags() & FVPHYSICS_PLAYER_HELD)))
	{
		// Ignore if pissed at player
		if (m_afMemory & bits_MEMORY_PROVOKED)
			return;

		TestPlayerPushing((pOther->IsPlayer()) ? pOther : AI_GetSinglePlayer());
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::ShouldAcceptGoal(CAI_BehaviorBase *pBehavior, CAI_GoalEntity *pGoal)
{
	if (BaseClass::ShouldAcceptGoal(pBehavior, pGoal))
	{
		CAI_FollowBehavior *pFollowBehavior = dynamic_cast<CAI_FollowBehavior *>(pBehavior);
		if (pFollowBehavior)
		{
			if (IsInPlayerSquad())
			{
				m_hSavedFollowGoalEnt = (CAI_FollowGoal *)pGoal;
				return false;
			}
		}
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::OnClearGoal(CAI_BehaviorBase *pBehavior, CAI_GoalEntity *pGoal)
{
	if (m_hSavedFollowGoalEnt == pGoal)
		m_hSavedFollowGoalEnt = NULL;
}


//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::PredictPlayerPush()
{
	CBasePlayer *pPlayer = AI_GetSinglePlayer();
	if (pPlayer && pPlayer->GetSmoothedVelocity().LengthSqr() >= Square(140))
	{
		Vector predictedPosition = pPlayer->WorldSpaceCenter() + pPlayer->GetSmoothedVelocity() * .4;
		Vector delta = WorldSpaceCenter() - predictedPosition;
		if (delta.z < GetHullHeight() * .5 && delta.Length2DSqr() < Square(GetHullWidth() * 1.414))
			TestPlayerPushing(pPlayer);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::IgnorePlayerPushing(void)
{
	if (m_AssaultBehavior.IsRunning() && m_AssaultBehavior.OnStrictAssault())
		return true;

	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::SimpleUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	m_bDontUseSemaphore = false;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CNPC_CombineS_Ally::OnTakeDamage_Alive(const CTakeDamageInfo &info)
{
	if ((info.GetDamageType() & DMG_BURN) && (info.GetDamageType() & DMG_DIRECT))
	{
#define CITIZEN_SCORCH_RATE		6
#define CITIZEN_SCORCH_FLOOR	75

		Scorch(CITIZEN_SCORCH_RATE, CITIZEN_SCORCH_FLOOR);
	}

	CTakeDamageInfo newInfo = info;

	if (IsInSquad() && (info.GetDamageType() & DMG_BLAST) && info.GetInflictor())
	{
		if (npc_combine_explosive_resist.GetBool())
		{
			// Blast damage. If this kills a squad member, give the 
			// remaining citizens a resistance bonus to this inflictor
			// to try to avoid having the entire squad wiped out by a
			// single explosion.
			if (m_pSquad->IsSquadInflictor(info.GetInflictor()))
			{
				newInfo.ScaleDamage(0.5);
			}
			else
			{
				// If this blast is going to kill me, designate the inflictor
				// so that the rest of the squad can enjoy a damage resist.
				if (info.GetDamage() >= GetHealth())
				{
					m_pSquad->SetSquadInflictor(info.GetInflictor());
				}
			}
		}
	}

	return BaseClass::OnTakeDamage_Alive(newInfo);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::FValidateHintType(CAI_Hint *pHint)
{
	switch (pHint->HintType())
	{
	case HINT_PLAYER_SQUAD_TRANSITON_POINT:
	case HINT_WORLD_VISUALLY_INTERESTING_DONT_AIM:
	case HINT_PLAYER_ALLY_MOVE_AWAY_DEST:
	case HINT_PLAYER_ALLY_FEAR_DEST:
		return true;
		break;

	default:
		break;
	}

	return BaseClass::FValidateHintType(pHint);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float CNPC_CombineS_Ally::GetIdealSpeed() const
{
	float baseSpeed = BaseClass::GetIdealSpeed();

	if (baseSpeed < m_flBoostSpeed)
		return m_flBoostSpeed;

	return baseSpeed;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float CNPC_CombineS_Ally::GetIdealAccel() const
{
	float multiplier = 1.0;
	if (AI_IsSinglePlayer())
	{
		if (m_bMovingAwayFromPlayer && (UTIL_PlayerByIndex(1)->GetAbsOrigin() - GetAbsOrigin()).Length2DSqr() < Square(3.0*12.0))
			multiplier = 2.0;
	}
	return BaseClass::GetIdealAccel() * multiplier;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::OnObstructionPreSteer(AILocalMoveGoal_t *pMoveGoal, float distClear, AIMoveResult_t *pResult)
{
	if (pMoveGoal->directTrace.flTotalDist - pMoveGoal->directTrace.flDistObstructed < GetHullWidth() * 1.5)
	{
		CAI_BaseNPC *pBlocker = pMoveGoal->directTrace.pObstruction->MyNPCPointer();
		if (pBlocker && pBlocker->IsPlayerAlly() && !pBlocker->IsMoving() && !pBlocker->IsInAScript() &&
			(IsCurSchedule(SCHED_NEW_WEAPON) ||
			IsCurSchedule(SCHED_GET_HEALTHKIT) ||
			pBlocker->IsCurSchedule(SCHED_FAIL) ||
			(IsInPlayerSquad() && !pBlocker->IsInPlayerSquad()) ||
			Classify() == CLASS_COMBINE ||
			IsInAScript()))

		{
			if (pBlocker->ConditionInterruptsCurSchedule(COND_GIVE_WAY) ||
				pBlocker->ConditionInterruptsCurSchedule(COND_PLAYER_PUSHING))
			{
				// HACKHACK
				pBlocker->GetMotor()->SetIdealYawToTarget(WorldSpaceCenter());
				pBlocker->SetSchedule(SCHED_MOVE_AWAY);
			}

		}
	}

	if (pMoveGoal->directTrace.pObstruction)
	{
	}

	return BaseClass::OnObstructionPreSteer(pMoveGoal, distClear, pResult);
}

//-----------------------------------------------------------------------------
// Purpose: Whether or not we should always transition with the player
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::ShouldAlwaysTransition(void)
{
	// No matter what, come through
	if (m_bAlwaysTransition)
		return true;
#if 0
	// Squadmates always come with
	if (IsInPlayerSquad())
		return true;

	// If we're following the player, then come along
	if (m_FollowBehavior.GetFollowTarget() && m_FollowBehavior.GetFollowTarget()->IsPlayer())
		return true;
#endif

	return false;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::IsNavigationUrgent(void)
{
	bool bBase = BaseClass::IsNavigationUrgent();

	// Consider follow & assault behaviour urgent
	if (!bBase && (m_FollowBehavior.IsActive() || (m_AssaultBehavior.IsRunning() && m_AssaultBehavior.IsUrgent())) && Classify() == CLASS_COMBINE)
	{
		// But only if the blocker isn't the player, and isn't a physics object that's still moving
		CBaseEntity *pBlocker = GetNavigator()->GetBlockingEntity();
		if (pBlocker && !pBlocker->IsPlayer())
		{
			IPhysicsObject *pPhysObject = pBlocker->VPhysicsGetObject();
			if (pPhysObject && !pPhysObject->IsAsleep())
				return false;
			if (pBlocker->IsNPC())
				return false;
		}

		// If we're within the player's viewcone, then don't teleport.

		// This test was made more general because previous iterations had cases where characters
		// could not see the player but the player could in fact see them.  Now the NPC's facing is
		// irrelevant and the player's viewcone is more authorative. -- jdw

		CBasePlayer *pLocalPlayer = AI_GetSinglePlayer();
		if (pLocalPlayer->FInViewCone(EyePosition()))
			return false;

		return true;
	}

	return bBase;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::IsCommandable()
{
	return (!HasSpawnFlags(SF_COMBINE_NOT_COMMANDABLE) && IsInPlayerSquad());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::IsPlayerAlly(CBasePlayer *pPlayer)
{
	// if (Classify() == CLASS_CITIZEN_PASSIVE && GlobalEntity_GetState("gordon_precriminal") == GLOBAL_ON)
	if (Classify() == CLASS_COMBINE)
	{
		// Robin: Citizens use friendly speech semaphore in trainstation
		return true;
	}

	return BaseClass::IsPlayerAlly(pPlayer);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::CanJoinPlayerSquad()
{
	if (!AI_IsSinglePlayer())
		return false;

	if (m_NPCState == NPC_STATE_SCRIPT || m_NPCState == NPC_STATE_PRONE)
		return false;

	if (HasSpawnFlags(SF_COMBINE_NOT_COMMANDABLE))
		return false;

	if (IsInAScript())
		return false;

	// Don't bother people who don't want to be bothered
	if (!CanBeUsedAsAFriend())
		return false;

	if (IRelationType(UTIL_GetLocalPlayer()) != D_LI)
		return false;

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::WasInPlayerSquad()
{
	return m_bWasInPlayerSquad;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::HaveCommandGoal() const
{
	if (GetCommandGoal() != vec3_invalid)
		return true;
	return false;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::IsCommandMoving()
{
	if (AI_IsSinglePlayer() && IsInPlayerSquad())
	{
		if (m_FollowBehavior.GetFollowTarget() == UTIL_GetLocalPlayer() ||
			IsFollowingCommandPoint())
		{
			return (m_FollowBehavior.IsMovingToFollowTarget());
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::ShouldAutoSummon()
{
	if (!AI_IsSinglePlayer() || !IsFollowingCommandPoint() || !IsInPlayerSquad())
		return false;

	CHL2_Player *pPlayer = (CHL2_Player *)UTIL_GetLocalPlayer();

	float distMovedSq = (pPlayer->GetAbsOrigin() - m_vAutoSummonAnchor).LengthSqr();
	float moveTolerance = player_squad_autosummon_move_tolerance.GetFloat() * 12;
	const Vector &vCommandGoal = GetCommandGoal();

	if (distMovedSq < Square(moveTolerance * 10) && (GetAbsOrigin() - vCommandGoal).LengthSqr() > Square(10 * 12) && IsCommandMoving())
	{
		m_AutoSummonTimer.Set(player_squad_autosummon_time.GetFloat());
		if (player_squad_autosummon_debug.GetBool())
			DevMsg("Waiting for arrival before initiating autosummon logic\n");
	}
	else if (m_AutoSummonTimer.Expired())
	{
		bool bSetFollow = false;
		bool bTestEnemies = true;

		// Auto summon unconditionally if a significant amount of time has passed
		if (gpGlobals->curtime - m_AutoSummonTimer.GetNext() > player_squad_autosummon_time.GetFloat() * 2)
		{
			bSetFollow = true;
			if (player_squad_autosummon_debug.GetBool())
				DevMsg("Auto summoning squad: long time (%f)\n", (gpGlobals->curtime - m_AutoSummonTimer.GetNext()) + player_squad_autosummon_time.GetFloat());
		}

		// Player must move for autosummon
		if (distMovedSq > Square(12))
		{
			bool bCommandPointIsVisible = pPlayer->FVisible(vCommandGoal + pPlayer->GetViewOffset());

			// Auto summon if the player is close by the command point
			if (!bSetFollow && bCommandPointIsVisible && distMovedSq > Square(24))
			{
				float closenessTolerance = player_squad_autosummon_player_tolerance.GetFloat() * 12;
				if ((pPlayer->GetAbsOrigin() - vCommandGoal).LengthSqr() < Square(closenessTolerance) &&
					((m_vAutoSummonAnchor - vCommandGoal).LengthSqr() > Square(closenessTolerance)))
				{
					bSetFollow = true;
					if (player_squad_autosummon_debug.GetBool())
						DevMsg("Auto summoning squad: player close to command point (%f)\n", (GetAbsOrigin() - vCommandGoal).Length());
				}
			}

			// Auto summon if moved a moderate distance and can't see command point, or moved a great distance
			if (!bSetFollow)
			{
				if (distMovedSq > Square(moveTolerance * 2))
				{
					bSetFollow = true;
					bTestEnemies = (distMovedSq < Square(moveTolerance * 10));
					if (player_squad_autosummon_debug.GetBool())
						DevMsg("Auto summoning squad: player very far from anchor (%f)\n", sqrt(distMovedSq));
				}
				else if (distMovedSq > Square(moveTolerance))
				{
					if (!bCommandPointIsVisible)
					{
						bSetFollow = true;
						if (player_squad_autosummon_debug.GetBool())
							DevMsg("Auto summoning squad: player far from anchor (%f)\n", sqrt(distMovedSq));
					}
				}
			}
		}

		// Auto summon only if there are no readily apparent enemies
		if (bSetFollow && bTestEnemies)
		{
			for (int i = 0; i < g_AI_Manager.NumAIs(); i++)
			{
				CAI_BaseNPC *pNpc = g_AI_Manager.AccessAIs()[i];
				float timeSinceCombatTolerance = player_squad_autosummon_time_after_combat.GetFloat();

				if (pNpc->IsInPlayerSquad())
				{
					if (gpGlobals->curtime - pNpc->GetLastAttackTime() > timeSinceCombatTolerance ||
						gpGlobals->curtime - pNpc->GetLastDamageTime() > timeSinceCombatTolerance)
						continue;
				}
				else if (pNpc->GetEnemy())
				{
					CBaseEntity *pNpcEnemy = pNpc->GetEnemy();
					if (/*!IsSniper(pNpc) &&*/ (gpGlobals->curtime - pNpc->GetEnemyLastTimeSeen()) > timeSinceCombatTolerance)
						continue;

					if (pNpcEnemy == pPlayer)
					{
						if (pNpc->CanBeAnEnemyOf(pPlayer))
						{
							bSetFollow = false;
							break;
						}
					}
					else if (pNpcEnemy->IsNPC() && (pNpcEnemy->MyNPCPointer()->GetSquad() == GetSquad() || pNpcEnemy->Classify() == CLASS_COMBINE))
					{
						if (pNpc->CanBeAnEnemyOf(this))
						{
							bSetFollow = false;
							break;
						}
					}
				}
			}
			if (!bSetFollow && player_squad_autosummon_debug.GetBool())
				DevMsg("Auto summon REVOKED: Combat recent \n");
		}

		return bSetFollow;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Is this entity something that the citizen should interact with (return true)
// or something that he should try to get close to (return false)
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::IsValidCommandTarget(CBaseEntity *pTarget)
{
	return false;
}

//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::SpeakCommandResponse(AIConcept_t concept, const char *modifiers)
{
	return true;
}



//-----------------------------------------------------------------------------
// Purpose: return TRUE if the commander mode should try to give this order
//			to more people. return FALSE otherwise. For instance, we don't
//			try to send all 3 selectedcitizens to pick up the same gun.
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::TargetOrder(CBaseEntity *pTarget, CAI_BaseNPC **Allies, int numAllies)
{
	if (pTarget->IsPlayer())
	{
		// I'm the target! Toggle follow!
		if (m_FollowBehavior.GetFollowTarget() != pTarget)
		{
			ClearFollowTarget();
			SetCommandGoal(vec3_invalid);

			// Turn follow on!
			m_AssaultBehavior.Disable();
			m_FollowBehavior.SetFollowTarget(pTarget);
			m_FollowBehavior.SetParameters(AIF_SIMPLE);
			SpeakCommandResponse(TLK_STARTFOLLOW);

			m_OnFollowOrder.FireOutput(this, this);
		}
		else if (m_FollowBehavior.GetFollowTarget() == pTarget)
		{
			// Stop following.
			m_FollowBehavior.SetFollowTarget(NULL);
			SpeakCommandResponse(TLK_STOPFOLLOW);
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Turn off following before processing a move order.
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::MoveOrder(const Vector &vecDest, CAI_BaseNPC **Allies, int numAllies)
{
	if (!AI_IsSinglePlayer())
		return;

	if (hl2_episodic.GetBool() && m_iszDenyCommandConcept != NULL_STRING)
	{
		SpeakCommandResponse(STRING(m_iszDenyCommandConcept));
		return;
	}

	CHL2_Player *pPlayer = (CHL2_Player *)UTIL_GetLocalPlayer();

	m_AutoSummonTimer.Set(player_squad_autosummon_time.GetFloat());
	m_vAutoSummonAnchor = pPlayer->GetAbsOrigin();

	if (m_StandoffBehavior.IsRunning())
	{
		m_StandoffBehavior.SetStandoffGoalPosition(vecDest);
	}

	// If in assault, cancel and move.
	if (m_AssaultBehavior.HasHitRallyPoint() && !m_AssaultBehavior.HasHitAssaultPoint())
	{
		m_AssaultBehavior.Disable();
		ClearSchedule("Moving from rally point to assault point");
	}

	bool spoke = false;

	CAI_BaseNPC *pClosest = NULL;
	float closestDistSq = FLT_MAX;

	for (int i = 0; i < numAllies; i++)
	{
		if (Allies[i]->IsInPlayerSquad())
		{
			Assert(Allies[i]->IsCommandable());
			float distSq = (pPlayer->GetAbsOrigin() - Allies[i]->GetAbsOrigin()).LengthSqr();
			if (distSq < closestDistSq)
			{
				pClosest = Allies[i];
				closestDistSq = distSq;
			}
		}
	}

	if (m_FollowBehavior.GetFollowTarget() && !IsFollowingCommandPoint())
	{
		ClearFollowTarget();
		if ((pPlayer->GetAbsOrigin() - GetAbsOrigin()).LengthSqr() < Square(180) &&
			((vecDest - pPlayer->GetAbsOrigin()).LengthSqr() < Square(120) ||
			(vecDest - GetAbsOrigin()).LengthSqr() < Square(120)))
		{

			if (pClosest == this)
				m_Sentences.Speak("COMBINE_CLEAR");
				//SpeakIfAllowed(TLK_STOPFOLLOW);
			spoke = true;
		}
	}

	if (!spoke && pClosest == this)
	{
		float destDistToPlayer = (vecDest - pPlayer->GetAbsOrigin()).Length();
		float destDistToClosest = (vecDest - GetAbsOrigin()).Length();
		CFmtStr modifiers("commandpoint_dist_to_player:%.0f,"
			"commandpoint_dist_to_npc:%.0f",
			destDistToPlayer,
			destDistToClosest);

		// SpeakCommandResponse(TLK_COMMANDED, modifiers);
		m_Sentences.Speak("COMBINE_ANSWER");
	}

	m_OnStationOrder.FireOutput(this, this);

	BaseClass::MoveOrder(vecDest, Allies, numAllies);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::OnMoveOrder()
{
	// SetReadinessLevel(AIRL_STIMULATED, false, false);
	BaseClass::OnMoveOrder();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::CommanderUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	m_OnPlayerUse.FireOutput(pActivator, pCaller);

	// Under these conditions, citizens will refuse to go with the player.
	// Robin: NPCs should always respond to +USE even if someone else has the semaphore.
	if (!AI_IsSinglePlayer() || !CanJoinPlayerSquad())
	{
		SimpleUse(pActivator, pCaller, useType, value);
		return;
	}

	if (pActivator == UTIL_GetLocalPlayer())
	{
		// Don't say hi after you've been addressed by the player
		SetSpokeConcept(TLK_HELLO, NULL);
	
		if (npc_combines_auto_player_squad_allow_use.GetBool())
		{
			if (!ShouldAutosquad())
				TogglePlayerSquadState();
			else if (!IsInPlayerSquad() && npc_combines_auto_player_squad_allow_use.GetBool())
				AddToPlayerSquad();
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::ShouldSpeakRadio(CBaseEntity *pListener)
{
	if (!pListener)
		return false;

	const float		radioRange = 384 * 384;
	Vector			vecDiff;

	vecDiff = WorldSpaceCenter() - pListener->WorldSpaceCenter();

	if (vecDiff.LengthSqr() > radioRange)
	{
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::OnMoveToCommandGoalFailed()
{
	// Clear the goal.
	SetCommandGoal(vec3_invalid);

	// Announce failure.
	SpeakCommandResponse(TLK_COMMAND_FAILED);
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::AddToPlayerSquad()
{
	Assert(!IsInPlayerSquad());

	AddToSquad(AllocPooledString(PLAYER_SQUADNAME));
	m_hSavedFollowGoalEnt = m_FollowBehavior.GetFollowGoal();
	m_FollowBehavior.SetFollowGoalDirect(NULL);

	FixupPlayerSquad();

	SetCondition(COND_PLAYER_ADDED_TO_SQUAD);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::RemoveFromPlayerSquad()
{
	Assert(IsInPlayerSquad());

	ClearFollowTarget();
	ClearCommandGoal();
	if (m_iszOriginalSquad != NULL_STRING && strcmp(STRING(m_iszOriginalSquad), PLAYER_SQUADNAME) != 0)
		AddToSquad(m_iszOriginalSquad);
	else
		RemoveFromSquad();

	if (m_hSavedFollowGoalEnt)
		m_FollowBehavior.SetFollowGoal(m_hSavedFollowGoalEnt);

	SetCondition(COND_PLAYER_REMOVED_FROM_SQUAD);

	// Don't evaluate the player squad for 2 seconds. 
	gm_PlayerSquadEvaluateTimer.Set(2.0);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::TogglePlayerSquadState()
{
	if (!AI_IsSinglePlayer())
		return;

	if (!IsInPlayerSquad())
	{
		AddToPlayerSquad();

		if (HaveCommandGoal())
		{
			SpeakCommandResponse(TLK_COMMANDED);
		}
		else if (m_FollowBehavior.GetFollowTarget() == UTIL_GetLocalPlayer())
		{
			SpeakCommandResponse(TLK_STARTFOLLOW);
		}
	}
	else
	{
		SpeakCommandResponse(TLK_STOPFOLLOW);
		RemoveFromPlayerSquad();
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct SquadCandidate_t
{
	CNPC_CombineS_Ally *pCombine;
	bool		  bIsInSquad;
	float		  distSq;
	int			  iSquadIndex;
};

void CNPC_CombineS_Ally::UpdatePlayerSquad()
{
	if (!AI_IsSinglePlayer())
		return;

	CBasePlayer *pPlayer = UTIL_GetLocalPlayer();
	if ((pPlayer->GetAbsOrigin().AsVector2D() - GetAbsOrigin().AsVector2D()).LengthSqr() < Square(20 * 12))
		m_flTimeLastCloseToPlayer = gpGlobals->curtime;

	if (!gm_PlayerSquadEvaluateTimer.Expired())
		return;

	gm_PlayerSquadEvaluateTimer.Set(2.0);

	// Remove stragglers
	CAI_Squad *pPlayerSquad = g_AI_SquadManager.FindSquad(MAKE_STRING(PLAYER_SQUADNAME));
	if (pPlayerSquad)
	{
		CUtlVectorFixed<CNPC_CombineS_Ally *, MAX_PLAYER_SQUAD> squadMembersToRemove;
		AISquadIter_t iter;

		for (CAI_BaseNPC *pPlayerSquadMember = pPlayerSquad->GetFirstMember(&iter); pPlayerSquadMember; pPlayerSquadMember = pPlayerSquad->GetNextMember(&iter))
		{
			if (pPlayerSquadMember->GetClassname() != GetClassname())
				continue;

			CNPC_CombineS_Ally *pCombine = assert_cast<CNPC_CombineS_Ally *>(pPlayerSquadMember);

			if (!pCombine->m_bNeverLeavePlayerSquad &&
				pCombine->m_FollowBehavior.GetFollowTarget() &&
				!pCombine->m_FollowBehavior.FollowTargetVisible() &&
				pCombine->m_FollowBehavior.GetNumFailedFollowAttempts() > 0 &&
				gpGlobals->curtime - pCombine->m_FollowBehavior.GetTimeFailFollowStarted() > 20 &&
				(fabsf((pCombine->m_FollowBehavior.GetFollowTarget()->GetAbsOrigin().z - pCombine->GetAbsOrigin().z)) > 196 ||
				(pCombine->m_FollowBehavior.GetFollowTarget()->GetAbsOrigin().AsVector2D() - pCombine->GetAbsOrigin().AsVector2D()).LengthSqr() > Square(50 * 12)))
			{
				if (DebuggingCommanderMode())
				{
					DevMsg("Player follower is lost (%d, %f, %d)\n",
						pCombine->m_FollowBehavior.GetNumFailedFollowAttempts(),
						gpGlobals->curtime - pCombine->m_FollowBehavior.GetTimeFailFollowStarted(),
						(int)((pCombine->m_FollowBehavior.GetFollowTarget()->GetAbsOrigin().AsVector2D() - pCombine->GetAbsOrigin().AsVector2D()).Length()));
				}

				squadMembersToRemove.AddToTail(pCombine);
			}
		}

		for (int i = 0; i < squadMembersToRemove.Count(); i++)
		{
			squadMembersToRemove[i]->RemoveFromPlayerSquad();
		}
	}

	// Autosquadding
	const float JOIN_PLAYER_XY_TOLERANCE_SQ = Square(36 * 12);
	const float UNCONDITIONAL_JOIN_PLAYER_XY_TOLERANCE_SQ = Square(12 * 12);
	const float UNCONDITIONAL_JOIN_PLAYER_Z_TOLERANCE = 5 * 12;
	const float SECOND_TIER_JOIN_DIST_SQ = Square(48 * 12);
	if (pPlayer && ShouldAutosquad() && !(pPlayer->GetFlags() & FL_NOTARGET) && pPlayer->IsAlive())
	{
		CAI_BaseNPC **ppAIs = g_AI_Manager.AccessAIs();
		CUtlVector<SquadCandidate_t> candidates;
		const Vector &vPlayerPos = pPlayer->GetAbsOrigin();
		bool bFoundNewGuy = false;
		int i;

		for (i = 0; i < g_AI_Manager.NumAIs(); i++)
		{
			if (ppAIs[i]->GetState() == NPC_STATE_DEAD)
				continue;

			if (ppAIs[i]->GetClassname() != GetClassname())
				continue;

			CNPC_CombineS_Ally *pCombine = assert_cast<CNPC_CombineS_Ally *>(ppAIs[i]);
			int iNew;

			if (pCombine->IsInPlayerSquad())
			{
				iNew = candidates.AddToTail();
				candidates[iNew].pCombine = pCombine;
				candidates[iNew].bIsInSquad = true;
				candidates[iNew].distSq = 0;
				candidates[iNew].iSquadIndex = pCombine->GetSquad()->GetSquadIndex(pCombine);
			}
			else
			{
				float distSq = (vPlayerPos.AsVector2D() - pCombine->GetAbsOrigin().AsVector2D()).LengthSqr();
				if (distSq > JOIN_PLAYER_XY_TOLERANCE_SQ &&
					(pCombine->m_flTimeJoinedPlayerSquad == 0 || gpGlobals->curtime - pCombine->m_flTimeJoinedPlayerSquad > 60.0) &&
					(pCombine->m_flTimeLastCloseToPlayer == 0 || gpGlobals->curtime - pCombine->m_flTimeLastCloseToPlayer > 15.0))
					continue;

				if (!pCombine->CanJoinPlayerSquad())
					continue;

				bool bShouldAdd = false;

				if (pCombine->HasCondition(COND_SEE_PLAYER))
					bShouldAdd = true;
				else
				{
					bool bPlayerVisible = pCombine->FVisible(pPlayer);
					if (bPlayerVisible)
					{
						if (pCombine->HasCondition(COND_HEAR_PLAYER))
							bShouldAdd = true;
						else if (distSq < UNCONDITIONAL_JOIN_PLAYER_XY_TOLERANCE_SQ && fabsf(vPlayerPos.z - pCombine->GetAbsOrigin().z) < UNCONDITIONAL_JOIN_PLAYER_Z_TOLERANCE)
							bShouldAdd = true;
					}
				}

				if (bShouldAdd)
				{
					// @TODO (toml 05-25-04): probably everyone in a squad should be a candidate if one of them sees the player
					AI_Waypoint_t *pPathToPlayer = pCombine->GetPathfinder()->BuildRoute(pCombine->GetAbsOrigin(), vPlayerPos, pPlayer, 5 * 12, NAV_NONE, true);
					GetPathfinder()->UnlockRouteNodes(pPathToPlayer);

					if (!pPathToPlayer)
						continue;

					CAI_Path tempPath;
					tempPath.SetWaypoints(pPathToPlayer); // path object will delete waypoints

					iNew = candidates.AddToTail();
					candidates[iNew].pCombine = pCombine;
					candidates[iNew].bIsInSquad = false;
					candidates[iNew].distSq = distSq;
					candidates[iNew].iSquadIndex = -1;

					bFoundNewGuy = true;
				}
			}
		}

		if (bFoundNewGuy)
		{
			// Look for second order guys
			int initialCount = candidates.Count();
			for (i = 0; i < initialCount; i++)
				candidates[i].pCombine->AddSpawnFlags(SF_COMBINE_NOT_COMMANDABLE); // Prevents double-add
			for (i = 0; i < initialCount; i++)
			{
				if (candidates[i].iSquadIndex == -1)
				{
					for (int j = 0; j < g_AI_Manager.NumAIs(); j++)
					{
						if (ppAIs[j]->GetState() == NPC_STATE_DEAD)
							continue;

						if (ppAIs[j]->GetClassname() != GetClassname())
							continue;

						if (ppAIs[j]->HasSpawnFlags(SF_COMBINE_NOT_COMMANDABLE))
							continue;

						CNPC_CombineS_Ally *pCombine = assert_cast<CNPC_CombineS_Ally *>(ppAIs[j]);

						float distSq = (vPlayerPos - pCombine->GetAbsOrigin()).Length2DSqr();
						if (distSq > JOIN_PLAYER_XY_TOLERANCE_SQ)
							continue;

						distSq = (candidates[i].pCombine->GetAbsOrigin() - pCombine->GetAbsOrigin()).Length2DSqr();
						if (distSq > SECOND_TIER_JOIN_DIST_SQ)
							continue;

						if (!pCombine->CanJoinPlayerSquad())
							continue;

						if (!pCombine->FVisible(pPlayer))
							continue;

						int iNew = candidates.AddToTail();
						candidates[iNew].pCombine = pCombine;
						candidates[iNew].bIsInSquad = false;
						candidates[iNew].distSq = distSq;
						candidates[iNew].iSquadIndex = -1;
						pCombine->AddSpawnFlags(SF_COMBINE_NOT_COMMANDABLE); // Prevents double-add
					}
				}
			}
			for (i = 0; i < candidates.Count(); i++)
				candidates[i].pCombine->RemoveSpawnFlags(SF_COMBINE_NOT_COMMANDABLE);

			if (candidates.Count() > MAX_PLAYER_SQUAD)
			{
				candidates.Sort(PlayerSquadCandidateSortFunc);

				for (i = MAX_PLAYER_SQUAD; i < candidates.Count(); i++)
				{
					if (candidates[i].pCombine->IsInPlayerSquad())
					{
						candidates[i].pCombine->RemoveFromPlayerSquad();
					}
				}
			}

			if (candidates.Count())
			{
				CNPC_CombineS_Ally *pClosest = NULL;
				float closestDistSq = FLT_MAX;
				int nJoined = 0;

				for (i = 0; i < candidates.Count() && i < MAX_PLAYER_SQUAD; i++)
				{
					if (!candidates[i].pCombine->IsInPlayerSquad())
					{
						candidates[i].pCombine->AddToPlayerSquad();
						nJoined++;

						if (candidates[i].distSq < closestDistSq)
						{
							pClosest = candidates[i].pCombine;
							closestDistSq = candidates[i].distSq;
						}
					}
				}

				if (pClosest)
				{
					if (!pClosest->SpokeConcept(TLK_JOINPLAYER))
					{
						pClosest->SpeakCommandResponse(TLK_JOINPLAYER, CFmtStr("numjoining:%d", nJoined));
					}
					else
					{
						pClosest->SpeakCommandResponse(TLK_STARTFOLLOW);
					}

					for (i = 0; i < candidates.Count() && i < MAX_PLAYER_SQUAD; i++)
					{
						candidates[i].pCombine->SetSpokeConcept(TLK_JOINPLAYER, NULL);
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int CNPC_CombineS_Ally::PlayerSquadCandidateSortFunc(const SquadCandidate_t *pLeft, const SquadCandidate_t *pRight)
{
	// "Bigger" means less approprate 
	CNPC_CombineS_Ally *pLeftCombine = pLeft->pCombine;
	CNPC_CombineS_Ally *pRightCombine = pRight->pCombine;

	// Medics are better than anyone
	if (pLeftCombine->IsMedic() && !pRightCombine->IsMedic())
		return -1;

	if (!pLeftCombine->IsMedic() && pRightCombine->IsMedic())
		return 1;

	CBaseCombatWeapon *pLeftWeapon = pLeftCombine->GetActiveWeapon();
	CBaseCombatWeapon *pRightWeapon = pRightCombine->GetActiveWeapon();

	// People with weapons are better than those without
	if (pLeftWeapon && !pRightWeapon)
		return -1;

	if (!pLeftWeapon && pRightWeapon)
		return 1;

	// Existing squad members are better than non-members
	if (pLeft->bIsInSquad && !pRight->bIsInSquad)
		return -1;

	if (!pLeft->bIsInSquad && pRight->bIsInSquad)
		return 1;

	// New squad members are better than older ones
	if (pLeft->bIsInSquad && pRight->bIsInSquad)
		return pRight->iSquadIndex - pLeft->iSquadIndex;

	// Finally, just take the closer
	return (int)(pRight->distSq - pLeft->distSq);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::FixupPlayerSquad()
{
	if (!AI_IsSinglePlayer())
		return;

	m_flTimeJoinedPlayerSquad = gpGlobals->curtime;
	m_bWasInPlayerSquad = true;
	if (m_pSquad->NumMembers() > MAX_PLAYER_SQUAD)
	{
		CAI_BaseNPC *pFirstMember = m_pSquad->GetFirstMember(NULL);
		m_pSquad->RemoveFromSquad(pFirstMember);
		pFirstMember->ClearCommandGoal();

		CNPC_CombineS_Ally *pFirstMemberCombine = dynamic_cast< CNPC_CombineS_Ally * >(pFirstMember);
		if (pFirstMemberCombine)
		{
			pFirstMemberCombine->ClearFollowTarget();
		}
		else
		{
			CAI_FollowBehavior *pOldMemberFollowBehavior;
			if (pFirstMember->GetBehavior(&pOldMemberFollowBehavior))
			{
				pOldMemberFollowBehavior->SetFollowTarget(NULL);
			}
		}
	}

	ClearFollowTarget();

	CAI_BaseNPC *pLeader = NULL;
	AISquadIter_t iter;
	for (CAI_BaseNPC *pAllyNpc = m_pSquad->GetFirstMember(&iter); pAllyNpc; pAllyNpc = m_pSquad->GetNextMember(&iter))
	{
		if (pAllyNpc->IsCommandable())
		{
			pLeader = pAllyNpc;
			break;
		}
	}

	if (pLeader && pLeader != this)
	{
		const Vector &commandGoal = pLeader->GetCommandGoal();
		if (commandGoal != vec3_invalid)
		{
			SetCommandGoal(commandGoal);
			SetCondition(COND_RECEIVED_ORDERS);
			OnMoveOrder();
		}
		else
		{
			CAI_FollowBehavior *pLeaderFollowBehavior;
			if (pLeader->GetBehavior(&pLeaderFollowBehavior))
			{
				m_FollowBehavior.SetFollowTarget(pLeaderFollowBehavior->GetFollowTarget());
				m_FollowBehavior.SetParameters(m_FollowBehavior.GetFormation());
			}

		}
	}
	else
	{
		m_FollowBehavior.SetFollowTarget(UTIL_GetLocalPlayer());
		m_FollowBehavior.SetParameters(AIF_SIMPLE);
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::ClearFollowTarget()
{
	m_FollowBehavior.SetFollowTarget(NULL);
	m_FollowBehavior.SetParameters(AIF_SIMPLE);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::UpdateFollowCommandPoint()
{
	if (!AI_IsSinglePlayer())
		return;

	if (IsInPlayerSquad())
	{
		if (HaveCommandGoal())
		{
			CBaseEntity *pFollowTarget = m_FollowBehavior.GetFollowTarget();
			CBaseEntity *pCommandPoint = gEntList.FindEntityByClassname(NULL, COMMAND_POINT_CLASSNAME);

			if (!pCommandPoint)
			{
				DevMsg("**\nVERY BAD THING\nCommand point vanished! Creating a new one\n**\n");
				pCommandPoint = CreateEntityByName(COMMAND_POINT_CLASSNAME);
			}

			if (pFollowTarget != pCommandPoint)
			{
				pFollowTarget = pCommandPoint;
				m_FollowBehavior.SetFollowTarget(pFollowTarget);
				m_FollowBehavior.SetParameters(AIF_COMMANDER);
			}

			if ((pCommandPoint->GetAbsOrigin() - GetCommandGoal()).LengthSqr() > 0.01)
			{
				UTIL_SetOrigin(pCommandPoint, GetCommandGoal(), false);
			}
		}
		else
		{
			if (IsFollowingCommandPoint())
				ClearFollowTarget();
			if (m_FollowBehavior.GetFollowTarget() != UTIL_GetLocalPlayer())
			{
				DevMsg("Expected to be following player, but not\n");
				m_FollowBehavior.SetFollowTarget(UTIL_GetLocalPlayer());
				m_FollowBehavior.SetParameters(AIF_SIMPLE);
			}
		}
	}
	else if (IsFollowingCommandPoint())
		ClearFollowTarget();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CNPC_CombineS_Ally::IsFollowingCommandPoint()
{
	CBaseEntity *pFollowTarget = m_FollowBehavior.GetFollowTarget();
	if (pFollowTarget)
		return FClassnameIs(pFollowTarget, COMMAND_POINT_CLASSNAME);
	return false;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct CombineSquadMemberInfo_t
{
	CNPC_CombineS_Ally *	pMember;
	bool			bSeesPlayer;
	float			distSq;
};

int __cdecl SquadSortFunc(const CombineSquadMemberInfo_t *pLeft, const CombineSquadMemberInfo_t *pRight)
{
	if (pLeft->bSeesPlayer && !pRight->bSeesPlayer)
	{
		return -1;
	}

	if (!pLeft->bSeesPlayer && pRight->bSeesPlayer)
	{
		return 1;
	}

	return (pLeft->distSq - pRight->distSq);
}

CAI_BaseNPC *CNPC_CombineS_Ally::GetSquadCommandRepresentative()
{
	if (!AI_IsSinglePlayer())
		return NULL;

	if (IsInPlayerSquad())
	{
		static float lastTime;
		static AIHANDLE hCurrent;

		if (gpGlobals->curtime - lastTime > 2.0 || !hCurrent || !hCurrent->IsInPlayerSquad()) // hCurrent will be NULL after level change
		{
			lastTime = gpGlobals->curtime;
			hCurrent = NULL;

			CUtlVectorFixed<CombineSquadMemberInfo_t, MAX_SQUAD_MEMBERS> candidates;
			CBasePlayer *pPlayer = UTIL_GetLocalPlayer();

			if (pPlayer)
			{
				AISquadIter_t iter;
				for (CAI_BaseNPC *pAllyNpc = m_pSquad->GetFirstMember(&iter); pAllyNpc; pAllyNpc = m_pSquad->GetNextMember(&iter))
				{
					if (pAllyNpc->IsCommandable() && dynamic_cast<CNPC_CombineS_Ally *>(pAllyNpc))
					{
						int i = candidates.AddToTail();
						candidates[i].pMember = (CNPC_CombineS_Ally *)(pAllyNpc);
						candidates[i].bSeesPlayer = pAllyNpc->HasCondition(COND_SEE_PLAYER);
						candidates[i].distSq = (pAllyNpc->GetAbsOrigin() - pPlayer->GetAbsOrigin()).LengthSqr();
					}
				}

				if (candidates.Count() > 0)
				{
					candidates.Sort(SquadSortFunc);
					hCurrent = candidates[0].pMember;
				}
			}
		}

		if (hCurrent != NULL)
		{
			Assert(dynamic_cast<CNPC_CombineS_Ally *>(hCurrent.Get()) && hCurrent->IsInPlayerSquad());
			return hCurrent;
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::SetSquad(CAI_Squad *pSquad)
{
	bool bWasInPlayerSquad = IsInPlayerSquad();

	BaseClass::SetSquad(pSquad);

	if (IsInPlayerSquad() && !bWasInPlayerSquad)
	{
		m_OnJoinedPlayerSquad.FireOutput(this, this);
	}
	else if (!IsInPlayerSquad() && bWasInPlayerSquad)
	{
		m_OnLeftPlayerSquad.FireOutput(this, this);
	}
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::InputOutsideTransition(inputdata_t &inputdata)
{
	if (!AI_IsSinglePlayer())
		return;

	// Must want to do this
	if (ShouldAlwaysTransition() == false)
		return;

	// If we're in a vehicle, that vehicle will transition with us still inside (which is preferable)
	if (IsInAVehicle())
		return;

	CBaseEntity *pPlayer = UTIL_GetLocalPlayer();
	const Vector &playerPos = pPlayer->GetAbsOrigin();

	// Mark us as already having succeeded if we're vital or always meant to come with the player
	bool bAlwaysTransition = ((Classify() == CLASS_COMBINE) || m_bAlwaysTransition);
	bool bPathToPlayer = bAlwaysTransition;

	if (bAlwaysTransition == false)
	{
		AI_Waypoint_t *pPathToPlayer = GetPathfinder()->BuildRoute(GetAbsOrigin(), playerPos, pPlayer, 0);

		if (pPathToPlayer)
		{
			bPathToPlayer = true;
			CAI_Path tempPath;
			tempPath.SetWaypoints(pPathToPlayer); // path object will delete waypoints
			GetPathfinder()->UnlockRouteNodes(pPathToPlayer);
		}
	}


#ifdef USE_PATHING_LENGTH_REQUIREMENT_FOR_TELEPORT
	float pathLength = tempPath.GetPathDistanceToGoal(GetAbsOrigin());

	if (pathLength > 150 * 12)
		return;
#endif

	bool bMadeIt = false;
	Vector teleportLocation;

	CAI_Hint *pHint = CAI_HintManager::FindHint(this, HINT_PLAYER_SQUAD_TRANSITON_POINT, bits_HINT_NODE_NEAREST, COMBINE_TRANSITION_SEARCH_DISTANCE, &playerPos);
	while (pHint)
	{
		pHint->Lock(this);
		pHint->Unlock(0.5); // prevent other squadmates and self from using during transition. 

		pHint->GetPosition(GetHullType(), &teleportLocation);
		if (GetNavigator()->CanFitAtPosition(teleportLocation, MASK_NPCSOLID))
		{
			bMadeIt = true;
			if (!bPathToPlayer && (playerPos - GetAbsOrigin()).LengthSqr() > Square(40 * 12))
			{
				AI_Waypoint_t *pPathToTeleport = GetPathfinder()->BuildRoute(GetAbsOrigin(), teleportLocation, pPlayer, 0);

				if (!pPathToTeleport)
				{
					DevMsg(2, "NPC \"%s\" failed to teleport to transition a point because there is no path\n", STRING(GetEntityName()));
					bMadeIt = false;
				}
				else
				{
					CAI_Path tempPath;
					GetPathfinder()->UnlockRouteNodes(pPathToTeleport);
					tempPath.SetWaypoints(pPathToTeleport); // path object will delete waypoints
				}
			}

			if (bMadeIt)
			{
				DevMsg(2, "NPC \"%s\" teleported to transition point %d\n", STRING(GetEntityName()), pHint->GetNodeId());
				break;
			}
		}
		else
		{
			if (g_debug_transitions.GetBool())
			{
				NDebugOverlay::Box(teleportLocation, GetHullMins(), GetHullMaxs(), 255, 0, 0, 8, 999);
			}
		}
		pHint = CAI_HintManager::FindHint(this, HINT_PLAYER_SQUAD_TRANSITON_POINT, bits_HINT_NODE_NEAREST, COMBINE_TRANSITION_SEARCH_DISTANCE, &playerPos);
	}
	if (!bMadeIt)
	{
		// Force us if we didn't find a normal route
		if (bAlwaysTransition)
		{
			bMadeIt = FindSpotForNPCInRadius(&teleportLocation, pPlayer->GetAbsOrigin(), this, 32.0*1.414, true);
			if (!bMadeIt)
				bMadeIt = FindSpotForNPCInRadius(&teleportLocation, pPlayer->GetAbsOrigin(), this, 32.0*1.414, false);
		}
	}

	if (bMadeIt)
	{
		Teleport(&teleportLocation, NULL, NULL);
	}
	else
	{
		DevMsg(2, "NPC \"%s\" failed to find a suitable transition a point\n", STRING(GetEntityName()));
	}

	BaseClass::InputOutsideTransition(inputdata);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CNPC_CombineS_Ally::InputSetCommandable(inputdata_t &inputdata)
{
	RemoveSpawnFlags(SF_COMBINE_NOT_COMMANDABLE);
	gm_PlayerSquadEvaluateTimer.Force();
}


//-----------------------------------------------------------------------------
// Purpose: Always transition along with the player
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::InputEnableAlwaysTransition(inputdata_t &inputdata)
{
	m_bAlwaysTransition = true;
}

//-----------------------------------------------------------------------------
// Purpose: Stop always transitioning along with the player
//-----------------------------------------------------------------------------
void CNPC_CombineS_Ally::InputDisableAlwaysTransition(inputdata_t &inputdata)
{
	m_bAlwaysTransition = false;
}
