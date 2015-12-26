//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//


//-----------------------------------------------------------------------------
// Generic NPC - purely for scripted sequence work.
//-----------------------------------------------------------------------------
#include	"cbase.h"
#include	"npcevent.h"
#include	"ai_basenpc.h"
#include	"ai_hull.h"
#include	"ai_baseactor.h"
#include	"hoe_npc_peasant_defs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHoe_NPC_Peasant : public CAI_BaseActor
{
public:
	DECLARE_CLASS(CHoe_NPC_Peasant, CAI_BaseActor);

	void	Spawn(void);
	void	Precache(void);
	void	SelectModel(void);

	Class_T Classify(void);
	void	HandleAnimEvent(animevent_t *pEvent);
	int		GetSoundInterests(void);
	bool	UseSemaphore(void);

	int		SelectSchedule(void);

private:
	static const char* m_pszPeasantModels[];
};

const char* CHoe_NPC_Peasant::m_pszPeasantModels[] =
{
	NPC_PEASANT_MALE1_MODEL,
	NPC_PEASANT_MALE2_MODEL,
	NPC_PEASANT_MALE3_MODEL,
	NPC_PEASANT_FEMALE1_MODEL,
	NPC_PEASANT_FEMALE2_MODEL,
	NPC_PEASANT_FEMALE3_MODEL,
};

LINK_ENTITY_TO_CLASS(npc_peasant, CHoe_NPC_Peasant);


//-----------------------------------------------------------------------------
// Classify - indicates this NPC's place in the 
// relationship table.
//-----------------------------------------------------------------------------
Class_T	CHoe_NPC_Peasant::Classify(void)
{
	return CLASS_CITIZEN_PASSIVE;
}

//-----------------------------------------------------------------------------
// HandleAnimEvent - catches the NPC-specific messages
// that occur when tagged animation frames are played.
//-----------------------------------------------------------------------------
void CHoe_NPC_Peasant::HandleAnimEvent(animevent_t *pEvent)
{
	switch (pEvent->event)
	{
	case 1:
	default:
		BaseClass::HandleAnimEvent(pEvent);
		break;
	}
}

//-----------------------------------------------------------------------------
// GetSoundInterests - generic NPC can't hear.
//-----------------------------------------------------------------------------
int CHoe_NPC_Peasant::GetSoundInterests(void)
{
	return SOUND_WORLD | SOUND_COMBAT | SOUND_PLAYER | SOUND_DANGER | SOUND_PHYSICS_DANGER | SOUND_BULLET_IMPACT | SOUND_MOVE_AWAY;
}

//-----------------------------------------------------------------------------
// Spawn
//-----------------------------------------------------------------------------
void CHoe_NPC_Peasant::Spawn()
{
	// m_nBody = 0;
	// m_nSkin = 0;

	SelectModel();

	Precache();
	SetModel(STRING(GetModelName()));

	BaseClass::Spawn();

	SetHullType(HULL_HUMAN);
	SetHullSizeNormal();

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_STANDABLE);
	SetMoveType(MOVETYPE_STEP);
	SetBloodColor(BLOOD_COLOR_RED);
	m_iHealth = 8;
	m_flFieldOfView = 0.5;// indicates the width of this NPC's forward view cone ( as a dotproduct result )
	m_NPCState = NPC_STATE_NONE;

	CapabilitiesAdd(bits_CAP_MOVE_GROUND | bits_CAP_OPEN_DOORS | bits_CAP_ANIMATEDFACE | bits_CAP_TURN_HEAD);
	CapabilitiesAdd(bits_CAP_FRIENDLY_DMG_IMMUNE);
	AddEFlags(EFL_NO_DISSOLVE | EFL_NO_MEGAPHYSCANNON_RAGDOLL | EFL_NO_PHYSCANNON_INTERACTION);

	NPCInit();
}

//-----------------------------------------------------------------------------
// Precache - precaches all resources this NPC needs
//-----------------------------------------------------------------------------
void CHoe_NPC_Peasant::Precache()
{
	PrecacheModel(STRING(GetModelName()));
	BaseClass::Precache();
}

bool CHoe_NPC_Peasant::UseSemaphore(void)
{
	return BaseClass::UseSemaphore();
}

int CHoe_NPC_Peasant::SelectSchedule(void)
{
	if (m_NPCState == NPC_STATE_SCRIPT || m_NPCState == NPC_STATE_PRONE)
	{
		return BaseClass::SelectSchedule();
	}

	switch ( m_NPCState )
	{
	case NPC_STATE_IDLE:
	{
		return SCHED_IDLE_STAND;
	}
	break;

	case NPC_STATE_ALERT:
	{
		if (HasCondition(COND_HEAR_COMBAT) || HasCondition(COND_HEAR_DANGER))
		{
			CSound* pSound = GetBestSound();

			if (pSound)
			{
				if (!FInViewCone(pSound->GetSoundOrigin()))
				{
					if (pSound->IsSoundType(SOUND_COMBAT))
					{
						return SCHED_ALERT_REACT_TO_COMBAT_SOUND;
					}

					return SCHED_ALERT_FACE_BESTSOUND;
				}
			}
		}

		if (HasCondition(COND_SEE_PLAYER))
		{
			return SCHED_ALERT_FACE;
		}

		return SCHED_ALERT_STAND;
	}

	case NPC_STATE_COMBAT:
	{
		if (HasCondition(COND_SEE_ENEMY))
		{
			return SCHED_RUN_FROM_ENEMY;
		}
		else
		{
			if (HasCondition(COND_HEAR_DANGER))
			{
				return SCHED_TAKE_COVER_FROM_BEST_SOUND;
			}
		}

		return SCHED_COMBAT_STAND;
	}
	break;

	default:
		break;
	}

	return BaseClass::SelectSchedule();
}

void CHoe_NPC_Peasant::SelectModel(void)
{
	char *szModel = NULL;

	// Is this a random model?
	if (m_nBody < PEASANT_BODY_MALE1)
	{
		// Is this a random male model?
		if (m_nBody < PEASANT_BODY_RANDOM)
		{
			// Is this a random female model?
			if (m_nBody < PEASANT_BODY_RANDOM_MALE)
			{
				// Select a random female model.
				szModel = (char*)m_pszPeasantModels[random->RandomInt(PEASANT_BODY_FEMALE1, PEASANT_BODY_FEMALE3)];
			}
			else
			{
				// Select a random male model.
				szModel = (char*)m_pszPeasantModels[random->RandomInt(PEASANT_BODY_MALE1, PEASANT_BODY_MALE3)];
			}
		}
		else
		{
			// Select a random model.
			szModel = (char*)m_pszPeasantModels[random->RandomInt(0, PEASANT_BODY_COUNT - 1)];
		}
	}
	else
	{
		// This is a pre-selected model, set it correctly.
		szModel = (char*)m_pszPeasantModels[m_nBody];
	}

	// If no valid model, set default one.
	if (!szModel || !*szModel)
		szModel = NPC_PEASANT_MALE1_MODEL;

	Assert(szModel);

	SetModelName(AllocPooledString(szModel));

	// Avoid crash.
	m_nBody = 0;
}

//-----------------------------------------------------------------------------
// AI Schedules Specific to this NPC
//-----------------------------------------------------------------------------