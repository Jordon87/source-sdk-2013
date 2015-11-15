//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_NPC_BASEZOMBIE_HEADLESS_CLASSIC_H
#define ELEVENEIGHTYSEVEN_NPC_BASEZOMBIE_HEADLESS_CLASSIC_H

#ifdef _WIN32
#pragma once
#endif

#include "1187_npc_basezombie_headless.h"

//=============================================================================
//=============================================================================

class C1187_NPC_BaseZombie_Headless_Classic : public CAI_BlendingHost<C1187_NPC_BaseZombie_Headless>
{
	DECLARE_DATADESC();
	DECLARE_CLASS(C1187_NPC_BaseZombie_Headless_Classic, CAI_BlendingHost<C1187_NPC_BaseZombie_Headless>);

public:
	C1187_NPC_BaseZombie_Headless_Classic()
		: m_DurationDoorBash(2, 6),
		m_NextTimeToStartDoorBash(3.0)
	{
	}

	bool ShouldBecomeTorso(const CTakeDamageInfo &info, float flDamageThreshold);
	bool CanBecomeLiveTorso() { return !m_fIsHeadless; }

	void GatherConditions(void);

	int SelectFailSchedule(int failedSchedule, int failedTask, AI_TaskFailureCode_t taskFailCode);
	int TranslateSchedule(int scheduleType);

#ifndef HL2_EPISODIC
	void CheckFlinches() {} // Zombie has custom flinch code
#endif // HL2_EPISODIC

	Activity NPC_TranslateActivity(Activity newActivity);

	void OnStateChange(NPC_STATE OldState, NPC_STATE NewState);

	void StartTask(const Task_t *pTask);
	void RunTask(const Task_t *pTask);

	virtual bool OnObstructingDoor(AILocalMoveGoal_t *pMoveGoal,
		CBaseDoor *pDoor,
		float distClear,
		AIMoveResult_t *pResult);

	Activity SelectDoorBash();

	void Ignite(float flFlameLifetime, bool bNPCOnly = true, float flSize = 0.0f, bool bCalledByLevelDesigner = false);
	void Extinguish();
	int OnTakeDamage_Alive(const CTakeDamageInfo &inputInfo);
	bool IsHeavyDamage(const CTakeDamageInfo &info);
	bool IsSquashed(const CTakeDamageInfo &info);
	void BuildScheduleTestBits(void);

	void PrescheduleThink(void);
	int SelectSchedule(void);

public:
	DEFINE_CUSTOM_AI;


private:
	CHandle< CBaseDoor > m_hBlockingDoor;
	float				 m_flDoorBashYaw;

	CRandSimTimer 		 m_DurationDoorBash;
	CSimTimer 	  		 m_NextTimeToStartDoorBash;

	Vector				 m_vPositionCharged;


private:


	//=========================================================
	// Conditions
	//=========================================================
	enum
	{
		COND_HEADLESS_CLASSIC_BLOCKED_BY_DOOR = LAST_BASE_ZOMBIE_CONDITION,
		COND_HEADLESS_CLASSIC_DOOR_OPENED,
		COND_HEADLESS_CLASSIC_CHARGE_TARGET_MOVED,

		NEXT_CONDITION,
	};

	//=========================================================
	// Schedules
	//=========================================================
	enum
	{
		SCHED_HEADLESS_CLASSIC_BASH_DOOR = LAST_BASE_ZOMBIE_SCHEDULE,
		SCHED_HEADLESS_CLASSIC_WANDER_ANGRILY,
		SCHED_HEADLESS_CLASSIC_CHARGE_ENEMY,
		SCHED_HEADLESS_CLASSIC_FAIL,

		NEXT_SCHEDULE,
	};

	//=========================================================
	// Tasks
	//=========================================================
	enum
	{
		TASK_HEADLESS_CLASSIC_EXPRESS_ANGER = LAST_BASE_ZOMBIE_TASK,
		TASK_HEADLESS_CLASSIC_YAW_TO_DOOR,
		TASK_HEADLESS_CLASSIC_ATTACK_DOOR,
		TASK_HEADLESS_CLASSIC_CHARGE_ENEMY,

		NEXT_TASK,
	};
};


#endif // ELEVENEIGHTYSEVEN_NPC_BASEZOMBIE_HEADLESS_CLASSIC_H
