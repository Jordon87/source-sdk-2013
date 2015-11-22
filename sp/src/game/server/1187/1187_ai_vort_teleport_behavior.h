
#ifndef ELEVENEIGHTYSEVEN_AI_TELEPORT_BEHAVIOR_H
#define ELEVENEIGHTYSEVEN_AI_TELEPORT_BEHAVIOR_H

#ifdef _WIN32
#pragma once
#endif

class ITeleportHelper
{
public:

	virtual void TeleportHelper_OnVisibilityChanged(bool bVisible) = 0;
	virtual void TeleportHelper_OnTeleport(void) = 0;

	virtual void TeleportHelper_OnWarmupComplete(void) = 0;
	virtual void TeleportHelper_OnWarmupStart(void) = 0;
};

class CAI_VortTeleportBehavior : public CAI_SimpleBehavior
{
	DECLARE_CLASS(CAI_VortTeleportBehavior, CAI_SimpleBehavior);
public:

	DECLARE_DATADESC();
	DEFINE_CUSTOM_SCHEDULE_PROVIDER;

	CAI_VortTeleportBehavior();

	virtual const char *GetName() { return "VortTeleport"; }

	virtual void Spawn();
	virtual bool CanSelectSchedule();

	virtual void GatherConditions(void);
	virtual void GatherConditionsNotActive(void);

	virtual void GatherCommonConditions(void);

	virtual int SelectSchedule();

	virtual void StartTask(const Task_t* pTask);
	virtual void RunTask(const Task_t* pTask);

	bool	HasSpawnDestinations(void);

	void	SetWarmupDuration(float flDuration) { m_flWarmupDuration = flDuration; }
	float	GetWarmupDuration(void) { return m_flWarmupDuration; }

	void	SetNextTeleportTimeMin(float flMin) { m_flNextTeleportTimeMin = flMin; }
	void	SetNextTeleportTimeMax(float flMax) { m_flNextTeleportTimeMax = flMax; }
	void	SetNextTeleportTimeIntervals(float flMin, float flMax) { m_flNextTeleportTimeMin = flMin; m_flNextTeleportTimeMax = flMax; }
	void	SetNextTeleportTime(float flTime) { m_flNextTeleportTime = flTime; }
	void	SetGracePeriodDuration(float flDuration) { m_flGracePeriodDuration = flDuration; }
	float	GetGracePeriodDuration(void) { return m_flGracePeriodDuration; }

private:
	static void		SetupDestinations(CAI_BehaviorBase* pBehavior, const char* classname);


	int				m_lastDestIndex;
	int				m_destIndex;
	float			m_flWarmupDuration;
	float			m_flWarmupTime;
	float			m_flNextTeleportTimeMin;
	float			m_flNextTeleportTimeMax;
	float			m_flNextTeleportTime;
	float			m_flLastSeenTime;
	bool			m_bLastSeenTimeRegistered;
	float			m_flGracePeriodDuration;

	static CUtlVector<EHANDLE> m_destinations;
	static bool		m_destSetup;

	enum
	{
		SCHED_VORTTELE_TELEPORT = BaseClass::NEXT_SCHEDULE,
		SCHED_VORTTELE_TELEPORT_FAIL,
		NEXT_SCHEDULE,
	};

	enum
	{
		TASK_VORTTELE_SET_VISIBILITY = BaseClass::NEXT_TASK,
		TASK_VORTTELE_FIND_TELEPORT_DESTINATION,
		TASK_VORTTELE_TELEPORT,
		TASK_VORTTELE_WARMUP,
		TASK_VORTTELE_NEXT_TELEPORT_TIME,
		TAST_VORTTELE_CLEAR_LAST_ENEMY,

		NEXT_TASK,
	};

	enum
	{
		COND_VORTTELE_CAN_TELEPORT = BaseClass::NEXT_CONDITION,
		NEXT_CONDITION,
	};

};

#endif // ELEVENEIGHTYSEVEN_AI_TELEPORT_BEHAVIOR_H