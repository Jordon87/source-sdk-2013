//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef COMBINE_PLAYER_ALLY_H
#define COMBINE_PLAYER_ALLY_H

#ifdef _WIN32
#pragma once
#endif

#include "ai_playerally.h"

#include "npc_combines.h"


#define SF_COMBINE_FOLLOW				( 1 << 19 )	//524288 follow the player as soon as I spawn.
#define SF_COMBINE_NOT_COMMANDABLE		( 1 << 20 ) //1048576
#define SF_COMBINE_USE_RENDER_BOUNDS	( 1 << 21 ) //2097152


struct SquadCandidate_t;

class CNPC_CombineS_Ally : public CNPC_CombineS
{
	DECLARE_CLASS(CNPC_CombineS_Ally, CNPC_CombineS);
public:
	DECLARE_DATADESC();

	void			Spawn(void);
	void			PostNPCInit();
	void			OnRestore();

	bool			ShouldAlwaysThink();
	bool			ShouldBehaviorSelectSchedule(CAI_BehaviorBase *pBehavior);
	bool			ShouldDeferToFollowBehavior();

	virtual void	GatherConditions(void);
	void			PrescheduleThink();


	void			BuildScheduleTestBits();

	int				SelectSchedule();
	virtual int 	SelectSchedulePriorityAction();
	int 			SelectSchedulePlayerPush();

	bool			ShouldAcceptGoal(CAI_BehaviorBase *pBehavior, CAI_GoalEntity *pGoal);
	void			OnClearGoal(CAI_BehaviorBase *pBehavior, CAI_GoalEntity *pGoal);


	void			PredictPlayerPush();

	void 			SimpleUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void 			Touch(CBaseEntity *pOther);

	virtual bool	IgnorePlayerPushing(void);

	virtual int		OnTakeDamage_Alive(const CTakeDamageInfo &info);

	virtual bool	IsNavigationUrgent(void);

	//---------------------------------
	// Hints
	//---------------------------------
	bool			FValidateHintType(CAI_Hint *pHint);


	//---------------------------------
	// Navigation
	//---------------------------------
	float			GetIdealSpeed() const;
	float			GetIdealAccel() const;
	bool			OnObstructionPreSteer(AILocalMoveGoal_t *pMoveGoal, float distClear, AIMoveResult_t *pResult);

	bool			ShouldAlwaysTransition(void);

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
	void			AddInsignia();
	void			RemoveInsignia();
	bool			SpeakCommandResponse(AIConcept_t concept, const char *modifiers = NULL);

protected:
	//---------------------------------
	// Inputs
	//---------------------------------
	void 			InputOutsideTransition(inputdata_t &inputdata);
	void			InputRemoveFromPlayerSquad(inputdata_t &inputdata) { RemoveFromPlayerSquad(); }
	void			InputSetCommandable(inputdata_t &inputdata);

	void			InputEnableAlwaysTransition(inputdata_t &inputdata);
	void			InputDisableAlwaysTransition(inputdata_t &inputdata);

	//-----------------------------------------------------
	//	Outputs
	//-----------------------------------------------------
	COutputEvent		m_OnJoinedPlayerSquad;
	COutputEvent		m_OnLeftPlayerSquad;
	COutputEvent		m_OnFollowOrder;
	COutputEvent		m_OnStationOrder;
	COutputEvent		m_OnPlayerUse;


private:

	bool			m_bMovingAwayFromPlayer;
	float			m_flBoostSpeed;
	bool			m_bAlwaysTransition;


	string_t		m_iszOriginalSquad;
	float			m_flTimeJoinedPlayerSquad;
	bool			m_bWasInPlayerSquad;
	float			m_flTimeLastCloseToPlayer;
	string_t		m_iszDenyCommandConcept;

	CSimpleSimTimer	m_AutoSummonTimer;
	Vector			m_vAutoSummonAnchor;

	static CSimpleSimTimer gm_PlayerSquadEvaluateTimer;

	CHandle<CAI_FollowGoal>	m_hSavedFollowGoalEnt;

	bool					m_bNeverLeavePlayerSquad; // Don't leave the player squad unless killed, or removed via Entity I/O. 

};


#endif // COMBINE_PLAYER_ALLY_H