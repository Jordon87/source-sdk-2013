
#include "cbase.h"
#include "ai_behavior.h"
#include "1187_ai_vort_teleport_behavior.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define VORT_ENHANCED_TELEPORT_CLASSNAME	"info_vort_teleport"

BEGIN_DATADESC(CAI_VortTeleportBehavior)
DEFINE_FIELD(m_destIndex, FIELD_INTEGER),
DEFINE_FIELD(m_lastDestIndex, FIELD_INTEGER),

DEFINE_FIELD(m_flWarmupDuration, FIELD_FLOAT),
DEFINE_FIELD(m_flWarmupTime, FIELD_TIME),

DEFINE_FIELD(m_flNextTeleportTimeMin, FIELD_FLOAT),
DEFINE_FIELD(m_flNextTeleportTimeMax, FIELD_FLOAT),
DEFINE_FIELD(m_flNextTeleportTime, FIELD_TIME),

DEFINE_FIELD(m_flLastSeenTime, FIELD_TIME),
DEFINE_FIELD(m_bLastSeenTimeRegistered, FIELD_BOOLEAN),
END_DATADESC()

CUtlVector<EHANDLE> CAI_VortTeleportBehavior::m_destinations;
bool CAI_VortTeleportBehavior::m_destSetup = false;


CAI_VortTeleportBehavior::CAI_VortTeleportBehavior()
{
	m_lastDestIndex = -1;
	m_destIndex = -1;

	m_flWarmupDuration = 0.0f;
	m_flWarmupTime = 0.0f;

	m_flNextTeleportTimeMin = 0.0f;
	m_flNextTeleportTimeMax = 0.0f;
	m_flNextTeleportTime = 0.0f;

	m_flLastSeenTime = 0.0f;
	m_bLastSeenTimeRegistered = false;
}

void CAI_VortTeleportBehavior::Spawn()
{
	BaseClass::Spawn();

	// Find all possible destinations within the level.
	CAI_VortTeleportBehavior::SetupDestinations(this, VORT_ENHANCED_TELEPORT_CLASSNAME);
}

bool CAI_VortTeleportBehavior::HasSpawnDestinations(void)
{
	return !m_destinations.IsEmpty();
}



void CAI_VortTeleportBehavior::GatherConditions(void)
{
	BaseClass::GatherConditions();

	GatherCommonConditions();
}

void CAI_VortTeleportBehavior::GatherConditionsNotActive(void)
{
	BaseClass::GatherConditionsNotActive();

	GatherCommonConditions();
}

void CAI_VortTeleportBehavior::GatherCommonConditions(void)
{
	if (!m_bLastSeenTimeRegistered && GetEnemy() && (GetEnemy()->IsPlayer() || GetEnemy()->IsNPC()))
	{
		CBaseCombatCharacter* pBBC = GetEnemy()->MyCombatCharacterPointer();

		if (pBBC && pBBC->FInViewCone(GetOuter()))
		{
			m_flLastSeenTime = gpGlobals->curtime;
			m_bLastSeenTimeRegistered = true;
		}
	}
	else if (gpGlobals->curtime - m_flLastSeenTime > m_flGracePeriodDuration)
	{
		SetCondition(COND_VORTTELE_CAN_TELEPORT);
	}
}

bool CAI_VortTeleportBehavior::CanSelectSchedule()
{
	if (!HasSpawnDestinations())
		return false;

	if (GetOuter()->IsInAScript())
		return false;

	if (m_flNextTeleportTime >= gpGlobals->curtime)
		return false;

	if (!GetEnemy() || !HasCondition(COND_SEE_ENEMY))
		return false;

	if (!m_bLastSeenTimeRegistered)
		return false;

	if (!HasCondition(COND_VORTTELE_CAN_TELEPORT))
		return false;

	return true;
}

int CAI_VortTeleportBehavior::SelectSchedule()
{
	return SCHED_VORTTELE_TELEPORT;
}

void CAI_VortTeleportBehavior::StartTask(const Task_t* pTask)
{
	switch (pTask->iTask)
	{
	case TASK_VORTTELE_SET_VISIBILITY:
	{
		bool bVisible = (pTask->flTaskData > 0) ? true : false;

		if (bVisible)
		{
			GetOuter()->RemoveEffects(EF_NODRAW);
			GetOuter()->m_takedamage = DAMAGE_YES;
			GetOuter()->RemoveSolidFlags(FSOLID_NOT_SOLID);
		}
		else
		{
			GetOuter()->AddEffects(EF_NODRAW);
			GetOuter()->m_takedamage = DAMAGE_NO;
			GetOuter()->AddSolidFlags(FSOLID_NOT_SOLID);
		}

		ITeleportHelper* pHelper = dynamic_cast<ITeleportHelper*>(GetOuter());
		if (pHelper)
		{
			pHelper->TeleportHelper_OnVisibilityChanged(bVisible);
		}

		TaskComplete();
	}
	break;

	case TASK_VORTTELE_FIND_TELEPORT_DESTINATION:
	{
		int maxDestIndex = m_destinations.Count() - 1;
		int destIndex = random->RandomInt(0, maxDestIndex);

		int attempts = 0;
		while ((destIndex = random->RandomInt(0, maxDestIndex)) == m_lastDestIndex)
		{
			if (attempts >= 5)
			{
				destIndex = 0;
				break;
			}
			attempts++;
		}

		Assert(destIndex >= 0 && destIndex <= maxDestIndex);

		m_lastDestIndex = m_destIndex;
		m_destIndex = destIndex;

		TaskComplete();
	}
	break;

	case TASK_VORTTELE_TELEPORT:
	{
		Assert(m_destIndex >= 0 && (m_destIndex < m_destinations.Count()));

		if (!m_destinations[m_destIndex])
		{
			TaskFail(FAIL_NO_TARGET);
			return;
		}

		Vector vDestPos = m_destinations[m_destIndex]->GetAbsOrigin();
		QAngle qDestAngles = m_destinations[m_destIndex]->GetAbsAngles();

		Teleport(&vDestPos, &qDestAngles, NULL);

		ITeleportHelper* pHelper = dynamic_cast<ITeleportHelper*>(GetOuter());
		if (pHelper)
		{
			pHelper->TeleportHelper_OnTeleport();
		}

		TaskComplete();
	}
	break;

	case TASK_VORTTELE_WARMUP:
	{
		m_flWarmupTime = gpGlobals->curtime + m_flWarmupDuration;

		ITeleportHelper* pHelper = dynamic_cast<ITeleportHelper*>(GetOuter());
		if (pHelper)
		{
			pHelper->TeleportHelper_OnWarmupStart();
		}
	}
	break;

	case TASK_VORTTELE_NEXT_TELEPORT_TIME:
	{
		m_flNextTeleportTime = gpGlobals->curtime + random->RandomFloat(m_flNextTeleportTimeMin, m_flNextTeleportTimeMax);

		TaskComplete();
	}
	break;

	case TAST_VORTTELE_CLEAR_LAST_ENEMY:
	{
		m_flLastSeenTime = 0.0f;
		m_bLastSeenTimeRegistered = false;

		TaskComplete();
	}
	break;

	default:
		BaseClass::StartTask(pTask);
		break;
	}
}

void CAI_VortTeleportBehavior::RunTask(const Task_t* pTask)
{
	switch (pTask->iTask)
	{

	case TASK_VORTTELE_SET_VISIBILITY:
		break;

	case TASK_VORTTELE_FIND_TELEPORT_DESTINATION:
		break;

	case TASK_VORTTELE_TELEPORT:
		break;

	case TASK_VORTTELE_WARMUP:
	{
		if (m_flWarmupTime < gpGlobals->curtime)
		{
			ITeleportHelper* pHelper = dynamic_cast<ITeleportHelper*>(GetOuter());
			if (pHelper)
			{
				pHelper->TeleportHelper_OnWarmupComplete();
			}

			TaskComplete();
			return;
		}
	}
	break;

	case TASK_VORTTELE_NEXT_TELEPORT_TIME:
		break;

	case TAST_VORTTELE_CLEAR_LAST_ENEMY:
		break;

	default:
		BaseClass::RunTask(pTask);
		break;
	}
}

void CAI_VortTeleportBehavior::SetupDestinations(CAI_BehaviorBase* pBehavior, const char* classname)
{
	if (!pBehavior)
		return;

	if (m_destSetup)
		return;

	m_destSetup = true;

	CBaseEntity* pDestination = NULL;

	while ((pDestination = gEntList.FindEntityByClassname(pDestination, classname)) != NULL)
	{
		m_destinations.AddToHead(pDestination);
	}

	DevMsg("%s behavior found %d destinations.\n", pBehavior->GetName(), m_destinations.Count());
}

AI_BEGIN_CUSTOM_SCHEDULE_PROVIDER(CAI_VortTeleportBehavior)

DECLARE_TASK(TASK_VORTTELE_SET_VISIBILITY);
DECLARE_TASK(TASK_VORTTELE_FIND_TELEPORT_DESTINATION);
DECLARE_TASK(TASK_VORTTELE_TELEPORT);
DECLARE_TASK(TASK_VORTTELE_WARMUP);
DECLARE_TASK(TASK_VORTTELE_NEXT_TELEPORT_TIME);
DECLARE_TASK(TAST_VORTTELE_CLEAR_LAST_ENEMY);

DECLARE_CONDITION(COND_VORTTELE_CAN_TELEPORT);

//=========================================================
// > SCHED_VORTTELE_TELEPORT
//=========================================================
DEFINE_SCHEDULE
(
SCHED_VORTTELE_TELEPORT,

"	Tasks"
"		TASK_SET_FAIL_SCHEDULE							SCHEDULE:SCHED_VORTTELE_TELEPORT_FAIL"
"		TASK_STOP_MOVING								0"
"		TASK_FACE_IDEAL									0"
"		TASK_VORTTELE_FIND_TELEPORT_DESTINATION			0"
"		TASK_VORTTELE_WARMUP							0"
"		TASK_VORTTELE_SET_VISIBILITY					0"
"		TASK_WAIT_RANDOM								5.0"
"		TASK_VORTTELE_TELEPORT							0"
"		TASK_VORTTELE_WARMUP							0"
"		TASK_VORTTELE_SET_VISIBILITY					1"
"		TASK_WAIT_FOR_MOVEMENT							0"
"		TASK_VORTTELE_NEXT_TELEPORT_TIME				0"
"		TAST_VORTTELE_CLEAR_LAST_ENEMY					0"
"		TASK_WAIT										0.1"
""
"	Interrupts"
""
);

//=========================================================
// > SCHED_VORTTELE_TELEPORT_FAIL
//=========================================================
DEFINE_SCHEDULE
(
SCHED_VORTTELE_TELEPORT_FAIL,

"	Tasks"
"		TASK_STOP_MOVING								0"
"		TASK_SET_ACTIVITY								ACTIVITY:ACT_IDLE"
"		TASK_WAIT										0.1"
"		TASK_VORTTELE_NEXT_TELEPORT_TIME				0"
"		TAST_VORTTELE_CLEAR_LAST_ENEMY					0"
""
"	Interrupts"
""
);


AI_END_CUSTOM_SCHEDULE_PROVIDER()