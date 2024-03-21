//========= Copyright Valve Corporation, All rights reserved. ============//
//
//
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "npc_headcrab.h"

extern int AE_POISONHEADCRAB_FLINCH_HOP;
extern int AE_POISONHEADCRAB_FOOTSTEP;
extern int AE_POISONHEADCRAB_THREAT_SOUND;

ConVar sk_headcrab_burst_health("sk_headcrab_burst_health", "1", FCVAR_NONE);

class CBurstHeadcrab : public CBaseHeadcrab
{
	DECLARE_CLASS(CBurstHeadcrab, CBaseHeadcrab);

public:
	void Eject(const QAngle& vecAngles, float flVelocityScale, CBaseEntity* pEnemy);
	void EjectTouch(CBaseEntity* pOther);

	//
	// CBaseHeadcrab implementation.
	//
	void TouchDamage(CBaseEntity* pOther);
	void BiteSound(void);
	void AttackSound(void);

	//
	// CAI_BaseNPC implementation.
	//
	virtual void PrescheduleThink(void);
	virtual void BuildScheduleTestBits(void);
	virtual int SelectSchedule(void);
	virtual int TranslateSchedule(int scheduleType);

	virtual Activity NPC_TranslateActivity(Activity eNewActivity);
	virtual void HandleAnimEvent(animevent_t* pEvent);
	virtual float MaxYawSpeed(void);

	virtual int	GetSoundInterests(void) { return (BaseClass::GetSoundInterests() | SOUND_DANGER | SOUND_BULLET_IMPACT); }

	bool IsHeavyDamage(const CTakeDamageInfo& info);

	virtual void PainSound(const CTakeDamageInfo& info);
	virtual void DeathSound(const CTakeDamageInfo& info);
	virtual void IdleSound(void);
	virtual void AlertSound(void);
	virtual void ImpactSound(void);
	virtual void TelegraphSound(void);

	//
	// CBaseEntity implementation.
	//
	virtual void Precache(void);
	virtual void Spawn(void);

	DEFINE_CUSTOM_AI;
	DECLARE_DATADESC();

private:


	void JumpFlinch(const Vector* pvecAwayFromPos);
	void Panic(float flDuration);

	bool m_bPanicState;
	float m_flPanicStopTime;
	float m_flNextHopTime;		// Keeps us from hopping too often due to damage.
};

BEGIN_DATADESC(CBurstHeadcrab)
	
	DEFINE_FIELD(m_bPanicState, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flPanicStopTime, FIELD_TIME),
	DEFINE_FIELD(m_flNextHopTime, FIELD_TIME),

	DEFINE_ENTITYFUNC(EjectTouch),

END_DATADESC()

LINK_ENTITY_TO_CLASS(npc_headcrab_burst, CBurstHeadcrab);

//-----------------------------------------------------------------------------
// Purpose: Make the sound of this headcrab chomping a target.
//-----------------------------------------------------------------------------
void CBurstHeadcrab::BiteSound(void)
{
	EmitSound("NPC_BlackHeadcrab.Bite");
}


//-----------------------------------------------------------------------------
// Purpose: The sound we make when leaping at our enemy.
//-----------------------------------------------------------------------------
void CBurstHeadcrab::AttackSound(void)
{
	EmitSound("NPC_BlackHeadcrab.Attack");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBurstHeadcrab::TelegraphSound(void)
{
	EmitSound("NPC_BlackHeadcrab.Telegraph");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBurstHeadcrab::Spawn(void)
{
	Precache();
	SetModel("models/headcrabburst.mdl");

	BaseClass::Spawn();

	m_bPanicState = false;
	m_iHealth = sk_headcrab_burst_health.GetFloat();

	NPCInit();
	HeadcrabInit();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBurstHeadcrab::Precache(void)
{
	PrecacheModel("models/headcrabburst.mdl");

	PrecacheScriptSound("NPC_BlackHeadcrab.Telegraph");
	PrecacheScriptSound("NPC_BlackHeadcrab.Attack");
	PrecacheScriptSound("NPC_BlackHeadcrab.Bite");
	PrecacheScriptSound("NPC_BlackHeadcrab.Threat");
	PrecacheScriptSound("NPC_BlackHeadcrab.Alert");
	PrecacheScriptSound("NPC_BlackHeadcrab.Idle");
	PrecacheScriptSound("NPC_BlackHeadcrab.Talk");
	PrecacheScriptSound("NPC_BlackHeadcrab.AlertVoice");
	PrecacheScriptSound("NPC_BlackHeadcrab.Pain");
	PrecacheScriptSound("NPC_BlackHeadcrab.Die");
	PrecacheScriptSound("NPC_BlackHeadcrab.Impact");
	PrecacheScriptSound("NPC_BlackHeadcrab.ImpactAngry");

	PrecacheScriptSound("NPC_BlackHeadcrab.FootstepWalk");
	PrecacheScriptSound("NPC_BlackHeadcrab.Footstep");

	BaseClass::Precache();
}


//-----------------------------------------------------------------------------
// Purpose: Returns the max yaw speed for the current activity.
//-----------------------------------------------------------------------------
float CBurstHeadcrab::MaxYawSpeed(void)
{
	switch (GetActivity())
	{
	case ACT_WALK:
	case ACT_RUN:
	{
		return 10;
	}

	case ACT_TURN_LEFT:
	case ACT_TURN_RIGHT:
	{
		return(30);
	}

	case ACT_RANGE_ATTACK1:
	{
		return(30);
	}

	default:
	{
		return(30);
	}
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Activity CBurstHeadcrab::NPC_TranslateActivity(Activity eNewActivity)
{
	if (eNewActivity == ACT_WALK)
	{
		return ACT_RUN;
	}

	return BaseClass::NPC_TranslateActivity(eNewActivity);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBurstHeadcrab::PrescheduleThink(void)
{
	BaseClass::PrescheduleThink();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CBurstHeadcrab::TranslateSchedule(int scheduleType)
{
	switch (scheduleType)
	{
		// Keep trying to take cover for at least a few seconds.
	case SCHED_FAIL_TAKE_COVER:
	{
		if ((m_bPanicState) && (gpGlobals->curtime > m_flPanicStopTime))
		{
			//DevMsg( "I'm sick of panicking\n" );
			m_bPanicState = false;
			return SCHED_CHASE_ENEMY;
		}

		break;
	}
	}

	return BaseClass::TranslateSchedule(scheduleType);
}


//-----------------------------------------------------------------------------
// Purpose: Allows for modification of the interrupt mask for the current schedule.
//			In the most cases the base implementation should be called first.
//-----------------------------------------------------------------------------
void CBurstHeadcrab::BuildScheduleTestBits(void)
{
	// Ignore damage if we're attacking or are fleeing and recently flinched.
	if (IsCurSchedule(SCHED_HEADCRAB_CRAWL_FROM_CANISTER) || IsCurSchedule(SCHED_RANGE_ATTACK1) || (IsCurSchedule(SCHED_TAKE_COVER_FROM_ENEMY) && HasMemory(bits_MEMORY_FLINCHED)))
	{
		ClearCustomInterruptCondition(COND_LIGHT_DAMAGE);
		ClearCustomInterruptCondition(COND_HEAVY_DAMAGE);
	}
	else
	{
		SetCustomInterruptCondition(COND_LIGHT_DAMAGE);
		SetCustomInterruptCondition(COND_HEAVY_DAMAGE);
	}

	// If we're committed to jump, carry on even if our enemy hides behind a crate. Or a barrel.
	if (IsCurSchedule(SCHED_RANGE_ATTACK1) && m_bCommittedToJump)
	{
		ClearCustomInterruptCondition(COND_ENEMY_OCCLUDED);
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
// Output : 
//-----------------------------------------------------------------------------
int CBurstHeadcrab::SelectSchedule(void)
{
	// don't override inherited behavior when hanging from ceiling
	if (!IsHangingFromCeiling())
	{
		if (HasSpawnFlags(SF_NPC_WAIT_TILL_SEEN))
		{
			return SCHED_IDLE_STAND;
		}

		if (HasCondition(COND_LIGHT_DAMAGE) || HasCondition(COND_HEAVY_DAMAGE))
		{
			if ((gpGlobals->curtime >= m_flNextHopTime) && SelectWeightedSequence(ACT_SMALL_FLINCH) != -1)
			{
				m_flNextHopTime = gpGlobals->curtime + random->RandomFloat(1, 3);
				return SCHED_SMALL_FLINCH;
			}
		}

		if (m_bPanicState)
		{
			// We're looking for a place to hide, and we've found one. Lurk!
			if (HasMemory(bits_MEMORY_INCOVER))
			{
				m_bPanicState = false;
				m_flPanicStopTime = gpGlobals->curtime;

				return SCHED_HEADCRAB_AMBUSH;
			}

			return SCHED_TAKE_COVER_FROM_ENEMY;
		}
	}

	return BaseClass::SelectSchedule();
}

//-----------------------------------------------------------------------------
// Purpose: Black headcrab's touch attack damage. Evil!
//-----------------------------------------------------------------------------
void CBurstHeadcrab::TouchDamage(CBaseEntity* pOther)
{
	if (pOther->m_iHealth > 1)
	{
		CTakeDamageInfo info;
		if (CalcDamageInfo(&info) >= pOther->m_iHealth)
			info.SetDamage(pOther->m_iHealth - 1);

		pOther->TakeDamage(info);

		if (pOther->IsAlive() && pOther->m_iHealth > 1)
		{
			// Episodic change to avoid NPCs dying too quickly from poison bites
			if (hl2_episodic.GetBool())
			{
				if (pOther->IsPlayer())
				{
					// That didn't finish them. Take them down to one point with poison damage. It'll heal.
					pOther->TakeDamage(CTakeDamageInfo(this, this, pOther->m_iHealth - 1, DMG_POISON));
				}
				else
				{
					// Just take some amount of slash damage instead
					pOther->TakeDamage(CTakeDamageInfo(this, this, sk_headcrab_poison_npc_damage.GetFloat(), DMG_SLASH));
				}
			}
			else
			{
				// That didn't finish them. Take them down to one point with poison damage. It'll heal.
				pOther->TakeDamage(CTakeDamageInfo(this, this, pOther->m_iHealth - 1, DMG_POISON));
			}
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Bails out of our host zombie, either because he died or was blown
//			into two pieces by an explosion.
// Input  : vecAngles - The yaw direction we should face.
//			flVelocityScale - A multiplier for our ejection velocity.
//			pEnemy - Who we should acquire as our enemy. Usually our zombie host's enemy.
//-----------------------------------------------------------------------------
void CBurstHeadcrab::Eject(const QAngle& vecAngles, float flVelocityScale, CBaseEntity* pEnemy)
{
	SetGroundEntity(NULL);
	m_spawnflags |= SF_NPC_FALL_TO_GROUND;

	SetIdealState(NPC_STATE_ALERT);

	if (pEnemy)
	{
		SetEnemy(pEnemy);
		UpdateEnemyMemory(pEnemy, pEnemy->GetAbsOrigin());
	}

	SetActivity(ACT_RANGE_ATTACK1);

	SetNextThink(gpGlobals->curtime);
	PhysicsSimulate();

	GetMotor()->SetIdealYaw(vecAngles.y);

	SetAbsVelocity(flVelocityScale * random->RandomInt(20, 50) *
		Vector(random->RandomFloat(-1.0, 1.0), random->RandomFloat(-1.0, 1.0), random->RandomFloat(0.5, 1.0)));

	m_bMidJump = false;
	SetTouch(&CBurstHeadcrab::EjectTouch);
}


//-----------------------------------------------------------------------------
// Purpose: Touch function for when we are ejected from the poison zombie.
//			Panic when we hit the ground.
//-----------------------------------------------------------------------------
void CBurstHeadcrab::EjectTouch(CBaseEntity* pOther)
{
	LeapTouch(pOther);
	if (GetFlags() & FL_ONGROUND)
	{
		// Keep trying to take cover for at least a few seconds.
		Panic(random->RandomFloat(2, 8));
	}
}


//-----------------------------------------------------------------------------
// Purpose: Puts us in a state in which we just want to hide. We'll stop
//			hiding after the given duration.
//-----------------------------------------------------------------------------
void CBurstHeadcrab::Panic(float flDuration)
{
	m_flPanicStopTime = gpGlobals->curtime + flDuration;
	m_bPanicState = true;
}


//-----------------------------------------------------------------------------
// Purpose: Does a spastic hop in a random or provided direction.
// Input  : pvecDir - 2D direction to hop, NULL picks a random direction.
//-----------------------------------------------------------------------------
void CBurstHeadcrab::JumpFlinch(const Vector* pvecDir)
{
	SetGroundEntity(NULL);

	//
	// Take him off ground so engine doesn't instantly reset FL_ONGROUND.
	//
	if (HasHeadroom())
	{
		MoveOrigin(Vector(0, 0, 1));
	}

	//
	// Jump in a random direction.
	//
	Vector up;
	AngleVectors(GetLocalAngles(), NULL, NULL, &up);

	if (pvecDir)
	{
		SetAbsVelocity(Vector(pvecDir->x * 4, pvecDir->y * 4, up.z) * random->RandomFloat(40, 80));
	}
	else
	{
		SetAbsVelocity(Vector(random->RandomFloat(-4, 4), random->RandomFloat(-4, 4), up.z) * random->RandomFloat(40, 80));
	}
}


//-----------------------------------------------------------------------------
// Purpose: Catches the monster-specific messages that occur when tagged
//			animation frames are played.
// Input  : pEvent - 
//-----------------------------------------------------------------------------
void CBurstHeadcrab::HandleAnimEvent(animevent_t* pEvent)
{
	if (pEvent->event == AE_POISONHEADCRAB_FOOTSTEP)
	{
		bool walk = (GetActivity() == ACT_WALK);   // ? 1.0 : 0.6; !!cgreen! old code had bug

		if (walk)
		{
			EmitSound("NPC_BlackHeadcrab.FootstepWalk");
		}
		else
		{
			EmitSound("NPC_BlackHeadcrab.Footstep");
		}

		return;
	}

	if (pEvent->event == AE_HEADCRAB_JUMP_TELEGRAPH)
	{
		EmitSound("NPC_BlackHeadcrab.Telegraph");

		CBaseEntity* pEnemy = GetEnemy();

		if (pEnemy)
		{
			// Once we telegraph, we MUST jump. This is also when commit to what point
			// we jump at. Jump at our enemy's eyes.
			m_vecCommittedJumpPos = pEnemy->EyePosition();
			m_bCommittedToJump = true;
		}

		return;
	}

	if (pEvent->event == AE_POISONHEADCRAB_THREAT_SOUND)
	{
		EmitSound("NPC_BlackHeadcrab.Threat");
		EmitSound("NPC_BlackHeadcrab.Alert");

		return;
	}

	if (pEvent->event == AE_POISONHEADCRAB_FLINCH_HOP)
	{
		//
		// Hop in a random direction, then run and hide. If we're already running
		// to hide, jump forward -- hopefully that will take us closer to a hiding spot.
		//			
		if (m_bPanicState)
		{
			Vector vecForward;
			AngleVectors(GetLocalAngles(), &vecForward);
			JumpFlinch(&vecForward);
		}
		else
		{
			JumpFlinch(NULL);
		}

		Panic(random->RandomFloat(2, 5));

		return;
	}

	BaseClass::HandleAnimEvent(pEvent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CBurstHeadcrab::IsHeavyDamage(const CTakeDamageInfo& info)
{
	if (!HasMemory(bits_MEMORY_FLINCHED) && info.GetDamage() > 1.0f)
	{
		// If I haven't flinched lately, any amount of damage is interpreted as heavy.
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBurstHeadcrab::IdleSound(void)
{
	// TODO: hook up "Marco" / "Polo" talking with nearby buddies
	if (m_NPCState == NPC_STATE_IDLE)
	{
		EmitSound("NPC_BlackHeadcrab.Idle");
	}
	else if (m_NPCState == NPC_STATE_ALERT)
	{
		EmitSound("NPC_BlackHeadcrab.Talk");
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBurstHeadcrab::AlertSound(void)
{
	EmitSound("NPC_BlackHeadcrab.AlertVoice");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBurstHeadcrab::PainSound(const CTakeDamageInfo& info)
{
	if (IsOnFire() && random->RandomInt(0, HEADCRAB_BURN_SOUND_FREQUENCY) > 0)
	{
		// Don't squeak every think when burning.
		return;
	}

	EmitSound("NPC_BlackHeadcrab.Pain");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBurstHeadcrab::DeathSound(const CTakeDamageInfo& info)
{
	EmitSound("NPC_BlackHeadcrab.Die");
}


//-----------------------------------------------------------------------------
// Purpose: Played when we jump and hit something that we can't bite.
//-----------------------------------------------------------------------------
void CBurstHeadcrab::ImpactSound(void)
{
	EmitSound("NPC_BlackHeadcrab.Impact");

	if (!(GetFlags() & FL_ONGROUND))
	{
		// Hit a wall - make a pissed off sound.
		EmitSound("NPC_BlackHeadcrab.ImpactAngry");
	}
}