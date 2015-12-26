//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "items.h"
#include "ammodef.h"
#include "eventlist.h"
#include "npcevent.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=================================================================
// CHoe_Item_Medcrate
//=================================================================
class CHoe_Item_Medcrate : public CBaseAnimating
{
public:
	DECLARE_CLASS(CHoe_Item_Medcrate, CBaseAnimating);

	void	Spawn(void);
	void	Precache(void);
	bool	CreateVPhysics(void);

	virtual void HandleAnimEvent(animevent_t *pEvent);

	void	SetupCrate(void);
	void	OnRestore(void);

	//FIXME: May not want to have this used in a radius
	int		ObjectCaps(void) { return (BaseClass::ObjectCaps() | (FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS)); };
	void	Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void	InputKill(inputdata_t &data);
	void	CrateThink(void);

	virtual int OnTakeDamage(const CTakeDamageInfo &info);

protected:

	int	m_maxhealth;
	int m_usedhealth;
	int m_numbodygroups;
	bool m_bIsEmpty;

	float	m_flCloseTime;

	COutputEvent	m_OnUsed;
	CHandle< CBasePlayer > m_hActivator;


	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(item_med_crate, CHoe_Item_Medcrate);

BEGIN_DATADESC(CHoe_Item_Medcrate)

	DEFINE_KEYFIELD(m_maxhealth, FIELD_INTEGER, "health"),
	DEFINE_FIELD(m_usedhealth, FIELD_INTEGER),
	DEFINE_FIELD(m_numbodygroups, FIELD_INTEGER),
	DEFINE_FIELD(m_bIsEmpty, FIELD_BOOLEAN),

	DEFINE_FIELD(m_flCloseTime, FIELD_FLOAT),
	DEFINE_FIELD(m_hActivator, FIELD_EHANDLE),

	DEFINE_OUTPUT(m_OnUsed, "OnUsed"),

	DEFINE_INPUTFUNC(FIELD_VOID, "Kill", InputKill),

	DEFINE_THINKFUNC(CrateThink),

END_DATADESC()

#define	AMMO_CRATE_CLOSE_DELAY	1.5f

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_Item_Medcrate::Spawn(void)
{
	Precache();

	BaseClass::Spawn();

	SetModel(STRING(GetModelName()));
	SetMoveType(MOVETYPE_NONE);
	SetSolid(SOLID_VPHYSICS);
	CreateVPhysics();

	ResetSequence(LookupSequence("Idle"));
	SetBodygroup(1, 0);

	m_numbodygroups = GetBodygroupCount(1);

	m_flCloseTime = gpGlobals->curtime;
	m_flAnimTime = gpGlobals->curtime;
	m_flPlaybackRate = 0.0;
	SetCycle(0);

	m_usedhealth = 0;
	m_bIsEmpty = (m_maxhealth <= 0) ? true : false;

	m_takedamage = DAMAGE_EVENTS_ONLY;

}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
bool CHoe_Item_Medcrate::CreateVPhysics(void)
{
	return (VPhysicsInitStatic() != NULL);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_Item_Medcrate::Precache(void)
{
	SetupCrate();
	PrecacheModel(STRING(GetModelName()));

	PrecacheScriptSound("MedCrate.Open");
	PrecacheScriptSound("MedCrate.Close");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_Item_Medcrate::SetupCrate(void)
{
	SetModelName(AllocPooledString("models/medkit/w_medcrate.mdl"));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_Item_Medcrate::OnRestore(void)
{
	BaseClass::OnRestore();

	// Restore our internal state
	SetupCrate();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pActivator - 
//			*pCaller - 
//			useType - 
//			value - 
//-----------------------------------------------------------------------------
void CHoe_Item_Medcrate::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBasePlayer *pPlayer = ToBasePlayer(pActivator);

	if (pPlayer == NULL)
		return;

	// if we are empty, return.
	if (m_bIsEmpty)
		return;

	m_OnUsed.FireOutput(pActivator, this);

	int iSequence = LookupSequence("Open");

	// See if we're not opening already
	if (GetSequence() != iSequence)
	{
		Vector mins, maxs;
		trace_t tr;

		CollisionProp()->WorldSpaceAABB(&mins, &maxs);

		Vector vOrigin = GetAbsOrigin();
		vOrigin.z += (maxs.z - mins.z);
		mins = (mins - GetAbsOrigin()) * 0.2f;
		maxs = (maxs - GetAbsOrigin()) * 0.2f;
		mins.z = (GetAbsOrigin().z - vOrigin.z);

		UTIL_TraceHull(vOrigin, vOrigin, mins, maxs, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr);

		if (tr.startsolid || tr.allsolid)
			return;

		m_hActivator = pPlayer;

		// Animate!
		ResetSequence(iSequence);

		// Make sound
		CPASAttenuationFilter sndFilter(this, "MedCrate.Open");
		EmitSound(sndFilter, entindex(), "MedCrate.Open");

		// Start thinking to make it return
		SetThink(&CHoe_Item_Medcrate::CrateThink);
		SetNextThink(gpGlobals->curtime + 0.1f);
	}

	// Don't close again for two seconds
	m_flCloseTime = gpGlobals->curtime + AMMO_CRATE_CLOSE_DELAY;
}

//-----------------------------------------------------------------------------
// Purpose: allows the crate to open up when hit by a machete
//-----------------------------------------------------------------------------
int CHoe_Item_Medcrate::OnTakeDamage(const CTakeDamageInfo &info)
{
	// if it's the player hitting us with a machete, open up
	CBasePlayer *player = ToBasePlayer(info.GetAttacker());
	if (player)
	{
		CBaseCombatWeapon *weapon = player->GetActiveWeapon();

		if (weapon && !stricmp(weapon->GetName(), "weapon_machete"))
		{
			// play the normal use sound
			player->EmitSound("HL2Player.Use");
			// open the crate
			Use(info.GetAttacker(), info.GetAttacker(), USE_TOGGLE, 0.0f);
		}
	}

	// don't actually take any damage
	return 0;
}


//-----------------------------------------------------------------------------
// Purpose: Catches the monster-specific messages that occur when tagged
//			animation frames are played.
// Input  : *pEvent - 
//-----------------------------------------------------------------------------
void CHoe_Item_Medcrate::HandleAnimEvent(animevent_t *pEvent)
{
	if (pEvent->event == AE_AMMOCRATE_PICKUP_AMMO)
	{
		if (m_hActivator)
		{
#define ITEM_HEALTH_CLASSNAME "item_healthkit"

			if (m_hActivator->GetHealth() >= m_hActivator->GetMaxHealth())
			{
				CBaseEntity *pEntity = CreateEntityByName(ITEM_HEALTH_CLASSNAME);
				pEntity->SetAbsOrigin(m_hActivator->GetAbsOrigin());
				pEntity->Spawn();
				UTIL_Remove(pEntity);
			}
			else
			{
				int activatorHealth = m_hActivator->GetHealth();
				int activatorHealthDiff = m_hActivator->GetMaxHealth() - activatorHealth;
				int availableHealth = m_maxhealth - m_usedhealth;

				int health = (availableHealth < activatorHealthDiff) ? availableHealth : activatorHealthDiff;

				if (m_hActivator->TakeHealth(health, DMG_GENERIC))
				{
					if (m_hActivator->IsPlayer())
					{
						CSingleUserRecipientFilter user(m_hActivator);
						user.MakeReliable();

						UserMessageBegin(user, "ItemPickup");
							WRITE_STRING(ITEM_HEALTH_CLASSNAME);
						MessageEnd();
					}

					CPASAttenuationFilter filter(m_hActivator, "HealthVial.Touch");
					EmitSound(filter, m_hActivator->entindex(), "HealthVial.Touch");

					// Increase used health.
					m_usedhealth += health;

					// Check if we are empty at this time.
					if (m_usedhealth >= m_maxhealth)
					{
						m_usedhealth = m_maxhealth;
						m_bIsEmpty = true;
					}

					int body = 0;

					if (m_bIsEmpty)
					{
						// Empty body group.
						body = m_numbodygroups;
					}
					else
					{
						// Calculate ideal bodygroup from used health.
						body = m_usedhealth * m_numbodygroups / m_maxhealth;
					}

					// Setup ideal bodygroup.
					SetBodygroup(1, body);
				}
			}
			m_hActivator = NULL;
		}
		return;
	}

	BaseClass::HandleAnimEvent(pEvent);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_Item_Medcrate::CrateThink(void)
{
	StudioFrameAdvance();
	DispatchAnimEvents(this);

	SetNextThink(gpGlobals->curtime + 0.1f);

	// Start closing if we're not already
	if (GetSequence() != LookupSequence("Close"))
	{
		// Not ready to close?
		if (m_flCloseTime <= gpGlobals->curtime)
		{
			m_hActivator = NULL;

			ResetSequence(LookupSequence("Close"));
		}
	}
	else
	{
		// See if we're fully closed
		if (IsSequenceFinished())
		{
			// Stop thinking
			SetThink(NULL);
			CPASAttenuationFilter sndFilter(this, "MedCrate.Close");
			EmitSound(sndFilter, entindex(), "MedCrate.Close");

			// Choose appropriate sequence.
			// If we are empty, play empty sequence, otherwise play default idle sequence.
			int sequence = !m_bIsEmpty ? LookupSequence("Idle") : LookupSequence("Empty");

			// FIXME: We're resetting the sequence here
			// but setting Think to NULL will cause this to never have
			// StudioFrameAdvance called. What are the consequences of that?
			ResetSequence(sequence);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &data - 
//-----------------------------------------------------------------------------
void CHoe_Item_Medcrate::InputKill(inputdata_t &data)
{
	UTIL_Remove(this);
}

