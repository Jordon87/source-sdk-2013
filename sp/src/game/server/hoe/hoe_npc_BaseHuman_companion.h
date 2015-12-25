//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef	HOE_NPC_BASEHUMAN_COMPANION_H
#define	HOE_NPC_BASEHUMAN_COMPANION_H

#include "hoe_npc_BaseHuman.h"

#include "ai_behavior_functank.h"

struct SquadCandidate_t;

#define SF_HOE_HUMAN_COMPANION_FOLLOW				( 1 << 21 )	// follow the player as soon as I spawn.
#define	SF_HOE_HUMAN_COMPANION_MEDIC				( 1 << 22 )
#define SF_HOE_HUMAN_COMPANION_RANDOM_HEAD			( 1 << 23 )
#define SF_HOE_HUMAN_COMPANION_AMMORESUPPLIER		( 1 << 24 )
#define SF_HOE_HUMAN_COMPANION_NOT_COMMANDABLE		( 1 << 25 )
#define SF_HOE_HUMAN_COMPANION_IGNORE_SEMAPHORE		( 1 << 26 ) // Work outside the speech semaphore system
#define SF_HOE_HUMAN_COMPANION_RANDOM_HEAD_MALE		( 1 << 27 )
#define SF_HOE_HUMAN_COMPANION_RANDOM_HEAD_FEMALE	( 1 << 28 )
#define SF_HOE_HUMAN_COMPANION_USE_RENDER_BOUNDS	( 1 << 29 )

//-----------------------------------------------------------------------------
//
// CLASS: CHoe_NPC_BaseHuman_Companion
//
//-----------------------------------------------------------------------------


//-------------------------------------

class CHoe_NPC_BaseHuman_Companion : public CHoe_NPC_BaseHuman
{
	DECLARE_CLASS(CHoe_NPC_BaseHuman_Companion, CHoe_NPC_BaseHuman);
public:
	CHoe_NPC_BaseHuman_Companion()
		: m_iHead(-1)
	{
	}

	//---------------------------------
	bool			CreateBehaviors();
	void			Precache();
	void			Spawn();
	void			PostNPCInit();
	void			Activate();

	virtual float	GetJumpGravity() const		{ return 1.8f; }

	void			OnRestore();

	bool			IsSilentSquadMember() const;

	//---------------------------------

	Class_T 		Classify();

	bool 			ShouldAlwaysThink();

	//---------------------------------
	// Behavior
	//---------------------------------
	bool			ShouldBehaviorSelectSchedule(CAI_BehaviorBase *pBehavior);
	void 			GatherConditions();
	void			PredictPlayerPush();
	void 			PrescheduleThink();
	void			BuildScheduleTestBits();

	bool			FInViewCone(CBaseEntity *pEntity);

	int				SelectFailSchedule(int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode);
	int				SelectSchedule();

	int 			SelectSchedulePriorityAction();
	int 			SelectScheduleHeal();
	int 			SelectScheduleRetrieveItem();
	int 			SelectScheduleNonCombat();
	int 			SelectScheduleCombat();
	bool			ShouldDeferToFollowBehavior();
	int 			TranslateSchedule(int scheduleType);

	bool			ShouldAcceptGoal(CAI_BehaviorBase *pBehavior, CAI_GoalEntity *pGoal);
	void			OnClearGoal(CAI_BehaviorBase *pBehavior, CAI_GoalEntity *pGoal);

	void 			StartTask(const Task_t *pTask);
	void 			RunTask(const Task_t *pTask);

	Activity		NPC_TranslateActivity(Activity eNewActivity);
	void 			HandleAnimEvent(animevent_t *pEvent);
	void			TaskFail(AI_TaskFailureCode_t code);

	void 			PickupItem(CBaseEntity *pItem);

	void 			SimpleUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	bool			IgnorePlayerPushing(void);

	//---------------------------------
	// Combat
	//---------------------------------
	bool 			OnBeginMoveAndShoot();
	void 			OnEndMoveAndShoot();

	virtual bool	UseAttackSquadSlots()	{ return false; }

	Vector 			GetActualShootPosition(const Vector &shootOrigin);
	void 			OnChangeActiveWeapon(CBaseCombatWeapon *pOldWeapon, CBaseCombatWeapon *pNewWeapon);

	bool			ShouldLookForBetterWeapon();


	//---------------------------------
	// Damage handling
	//---------------------------------
	int 			OnTakeDamage_Alive(const CTakeDamageInfo &info);

	//---------------------------------
	// Commander mode
	//---------------------------------
	bool 			IsCommandable();
	bool			IsPlayerAlly(CBasePlayer *pPlayer = NULL);
	bool			CanJoinPlayerSquad();
	bool			WasInPlayerSquad();
	bool			HaveCommandGoal() const;
	bool			IsCommandMoving();
	bool			ShouldAutoSummon();
	bool 			IsValidCommandTarget(CBaseEntity *pTarget);
	bool 			NearCommandGoal();
	bool 			VeryFarFromCommandGoal();
	bool 			TargetOrder(CBaseEntity *pTarget, CAI_BaseNPC **Allies, int numAllies);
	void 			MoveOrder(const Vector &vecDest, CAI_BaseNPC **Allies, int numAllies);
	void			OnMoveOrder();
	void 			CommanderUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	bool			ShouldSpeakRadio(CBaseEntity *pListener);
	void			OnMoveToCommandGoalFailed();
	void			AddToPlayerSquad();
	void			RemoveFromPlayerSquad();
	void 			TogglePlayerSquadState();
	void			UpdatePlayerSquad();
	static int __cdecl PlayerSquadCandidateSortFunc(const SquadCandidate_t *, const SquadCandidate_t *);
	void 			FixupPlayerSquad();
	void 			ClearFollowTarget();
	void 			UpdateFollowCommandPoint();
	bool			IsFollowingCommandPoint();
	CAI_BaseNPC *	GetSquadCommandRepresentative();
	void			SetSquad(CAI_Squad *pSquad);
	bool			SpeakCommandResponse(AIConcept_t concept, const char *modifiers = NULL);

	//---------------------------------
	// Scanner interaction
	//---------------------------------
	float 			GetNextScannerInspectTime() { return m_fNextInspectTime; }
	void			SetNextScannerInspectTime(float flTime) { m_fNextInspectTime = flTime; }
	bool			HandleInteraction(int interactionType, void *data, CBaseCombatCharacter* sourceEnt);

	//---------------------------------
	// Hints
	//---------------------------------
	bool			FValidateHintType(CAI_Hint *pHint);

	//---------------------------------
	// Special abilities
	//---------------------------------
	virtual bool 	IsMedic() 			{ return false; }
	virtual bool 	IsAmmoResupplier() 	{ return false; }

	bool 			CanHeal();
	bool 			ShouldHealTarget(CBaseEntity *pTarget, bool bActiveUse = false);
	bool 			ShouldHealTossTarget(CBaseEntity *pTarget, bool bActiveUse = false);

	void 			Heal();

	bool			ShouldLookForHealthItem();

	void			TossHealthKit(CBaseCombatCharacter *pThrowAt, const Vector &offset); // create a healthkit and throw it at someone
	void			InputForceHealthKitToss(inputdata_t &inputdata);

	//---------------------------------
	// Inputs
	//---------------------------------
	void			InputRemoveFromPlayerSquad(inputdata_t &inputdata) { RemoveFromPlayerSquad(); }
	void 			InputStartPatrolling(inputdata_t &inputdata);
	void 			InputStopPatrolling(inputdata_t &inputdata);
	void			InputSetCommandable(inputdata_t &inputdata);
	void			InputSetMedicOn(inputdata_t &inputdata);
	void			InputSetMedicOff(inputdata_t &inputdata);
	void			InputSetAmmoResupplierOn(inputdata_t &inputdata);
	void			InputSetAmmoResupplierOff(inputdata_t &inputdata);
	void			InputSpeakIdleResponse(inputdata_t &inputdata);

	//---------------------------------
	//	Sounds & speech
	//---------------------------------
	bool			UseSemaphore(void);

	virtual void	OnChangeRunningBehavior(CAI_BehaviorBase *pOldBehavior, CAI_BehaviorBase *pNewBehavior);

private:
	//-----------------------------------------------------
	// Schedules
	//-----------------------------------------------------
	enum
	{
		SCHED_COMPANION_PLAY_INSPECT_ACTIVITY = BaseClass::NEXT_SCHEDULE,
		SCHED_COMPANION_HEAL,
		SCHED_COMPANION_RANGE_ATTACK1_RPG,
		SCHED_COMPANION_PATROL,
		SCHED_COMPANION_MOURN_PLAYER,
		SCHED_COMPANION_SIT_ON_TRAIN,
		SCHED_COMPANION_STRIDER_RANGE_ATTACK1_RPG,
		SCHED_COMPANION_HEAL_TOSS,

		NEXT_SCHEDULE,
	};

	//-----------------------------------------------------
	// Tasks
	//-----------------------------------------------------
	enum
	{
		TASK_COMPANION_HEAL = BaseClass::NEXT_TASK,
		TASK_COMPANION_RPG_AUGER,
		TASK_COMPANION_PLAY_INSPECT_SEQUENCE,
		TASK_COMPANION_SIT_ON_TRAIN,
		TASK_COMPANION_LEAVE_TRAIN,
		TASK_COMPANION_SPEAK_MOURNING,
		TASK_COMPANION_HEAL_TOSS,

		NEXT_TASK,
	};


	//-----------------------------------------------------
	// Conditions
	//-----------------------------------------------------
	enum
	{
		COND_COMPANION_PLAYERHEALREQUEST = BaseClass::NEXT_CONDITION,
		COND_COMPANION_COMMANDHEAL,
		COND_COMPANION_HURTBYFIRE,
		COND_COMPANION_START_INSPECTION,

		NEXT_CONDITION,
	};

	//-----------------------------------------------------

	int				m_nInspectActivity;
	float			m_flNextFearSoundTime;
	float			m_flStopManhackFlinch;
	float			m_fNextInspectTime;		// Next time I'm allowed to get inspected by a scanner
	float			m_flPlayerHealTime;
	float			m_flNextHealthSearchTime; // Next time I'm allowed to look for a healthkit
	float			m_flAllyHealTime;
	float			m_flPlayerGiveAmmoTime;
	string_t		m_iszAmmoSupply;
	int				m_iAmmoAmount;
	bool			m_bRPGAvoidPlayer;
	bool			m_bShouldPatrol;
	string_t		m_iszOriginalSquad;
	float			m_flTimeJoinedPlayerSquad;
	bool			m_bWasInPlayerSquad;
	float			m_flTimeLastCloseToPlayer;
	string_t		m_iszDenyCommandConcept;

	CSimpleSimTimer	m_AutoSummonTimer;
	Vector			m_vAutoSummonAnchor;

	int				m_iHead;

	static CSimpleSimTimer gm_PlayerSquadEvaluateTimer;

	float			m_flTimePlayerStare;	// The game time at which the player started staring at me.
	float			m_flTimeNextHealStare;	// Next time I'm allowed to heal a player who is staring at me.

	//-----------------------------------------------------
	//	Outputs
	//-----------------------------------------------------
	COutputEvent		m_OnJoinedPlayerSquad;
	COutputEvent		m_OnLeftPlayerSquad;
	COutputEvent		m_OnFollowOrder;
	COutputEvent		m_OnStationOrder;
	COutputEvent		m_OnPlayerUse;
	COutputEvent		m_OnNavFailBlocked;

	//-----------------------------------------------------
	CAI_FuncTankBehavior	m_FuncTankBehavior;

	CHandle<CAI_FollowGoal>	m_hSavedFollowGoalEnt;

	bool					m_bNotifyNavFailBlocked;
	bool					m_bNeverLeavePlayerSquad; // Don't leave the player squad unless killed, or removed via Entity I/O. 

	//-----------------------------------------------------

	DECLARE_DATADESC();
#ifdef _XBOX
protected:
#endif
	DEFINE_CUSTOM_SCHEDULE_PROVIDER;
};

//---------------------------------------------------------
//---------------------------------------------------------
inline bool CHoe_NPC_BaseHuman_Companion::NearCommandGoal()
{
	const float flDistSqr = COMMAND_GOAL_TOLERANCE * COMMAND_GOAL_TOLERANCE;
	return ((GetAbsOrigin() - GetCommandGoal()).LengthSqr() <= flDistSqr);
}

//---------------------------------------------------------
//---------------------------------------------------------
inline bool CHoe_NPC_BaseHuman_Companion::VeryFarFromCommandGoal()
{
	const float flDistSqr = (12 * 50) * (12 * 50);
	return ((GetAbsOrigin() - GetCommandGoal()).LengthSqr() > flDistSqr);
}

#endif	//HOE_NPC_BASEHUMAN_COMPANION_H
