//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_basezombie.h"
#include "ai_memory.h"
#include "beam_flags.h"
#include "npcevent.h"
#include "movevars_shared.h"

#define HOUNDEYE_MELEE_REACH	190.0f

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		HOUND_AE_WARN			1
#define		HOUND_AE_STARTATTACK	2
#define		HOUND_AE_THUMP			3
#define		HOUND_AE_ANGERSOUND1	4
#define		HOUND_AE_ANGERSOUND2	5
#define		HOUND_AE_HOPBACK		6
#define		HOUND_AE_CLOSE_EYE		7
#define		HOUND_AE_LEAP_HIT		8

ConVar sk_houndeye_health("sk_houndeye_health", "80");
ConVar sk_houndeye_dmg_blast("sk_houndeye_dmg_blast", "0", FCVAR_NONE);

class CNPC_HoundeyeNew : public CNPC_BaseZombie
{
	DECLARE_CLASS(CNPC_HoundeyeNew, CNPC_BaseZombie);
	DEFINE_CUSTOM_AI;

public:
	void Precache();
	void Spawn();
	Class_T Classify(void);

	void HandleAnimEvent(animevent_t* pEvent);

	int MeleeAttack1Conditions(float flDot, float flDist);
	virtual float GetClawAttackRange() const { return HOUNDEYE_MELEE_REACH; }

	virtual Activity NPC_TranslateActivity(Activity baseAct);

	void RunTask(const Task_t* pTask);

	void PrescheduleThink(void);

	virtual void SetZombieModel();

	void PainSound(const CTakeDamageInfo& info);
	void DeathSound(const CTakeDamageInfo& info);
	void AlertSound(void);
	void IdleSound(void);
	void AttackSound(void) { return; }
	void AttackHitSound(void) { return; }
	void AttackMissSound(void) { return; }
	void FootstepSound(bool fRightFoot) { return; }
	void FootscuffSound(bool fRightFoot) { return; }

	virtual const char* GetMoanSound(int nSound);

	virtual const char* GetLegsModel(void);
	virtual const char* GetTorsoModel(void);
	virtual const char* GetHeadcrabClassname(void);
	virtual const char* GetHeadcrabModel(void);

protected:
	static const char* pMoanSounds[];

private:
	void SonicAttack();
	int m_iLightningEffect;
};

LINK_ENTITY_TO_CLASS(npc_houndeye, CNPC_HoundeyeNew);

AI_BEGIN_CUSTOM_NPC(npc_houndeye, CNPC_HoundeyeNew)

AI_END_CUSTOM_NPC()

const char* CNPC_HoundeyeNew::pMoanSounds[] =
{
	 "ATV_engine_null",
};

void CNPC_HoundeyeNew::Precache()
{
	BaseClass::Precache();

	PrecacheModel("models/houndeye.mdl");

	PrecacheScriptSound("NPC_Houndeye.Anger1");
	PrecacheScriptSound("NPC_Houndeye.Anger2");
	PrecacheScriptSound("NPC_Houndeye.SpeakSentence");
	PrecacheScriptSound("NPC_Houndeye.Idle");
	PrecacheScriptSound("NPC_Houndeye.WarmUp");
	PrecacheScriptSound("NPC_Houndeye.Warn");
	PrecacheScriptSound("NPC_Houndeye.Alert");
	PrecacheScriptSound("NPC_Houndeye.Die");
	PrecacheScriptSound("NPC_Houndeye.Pain");
	PrecacheScriptSound("NPC_Houndeye.Retreat");
	PrecacheScriptSound("NPC_Houndeye.SonicAttack");
	PrecacheScriptSound("NPC_Houndeye.GroupAttack");
	PrecacheScriptSound("NPC_Houndeye.GroupFollow");

	m_iLightningEffect = PrecacheModel("sprites/lgtning.vmt");
}

void CNPC_HoundeyeNew::Spawn()
{
	Precache();

	m_fIsTorso = false;
	m_fIsHeadless = true;

	SetBloodColor(DONT_BLEED);

	m_iHealth = sk_houndeye_health.GetFloat();
	SetMaxHealth(m_iHealth);

	m_flFieldOfView = -1.0f;

	BaseClass::Spawn();

	GetEnemies()->SetFreeKnowledgeDuration(90.0f);

	m_NPCState = NPC_STATE_COMBAT;

	m_flNextMoanSound = (random->RandomFloat(1.0f,4.0f) + gpGlobals->curtime);

}

Class_T CNPC_HoundeyeNew::Classify(void)
{
	return CLASS_HOUNDEYE;
}

void CNPC_HoundeyeNew::HandleAnimEvent(animevent_t* pEvent)
{
	switch (pEvent->event)
	{
	case HOUND_AE_WARN:
		EmitSound("NPC_Houndeye.Warn");
		break;

	case HOUND_AE_STARTATTACK:
		EmitSound("NPC_Houndeye.WarmUp");
		break;

	case HOUND_AE_THUMP:
		// emit the shockwaves
		SonicAttack();
		m_flNextAttack = gpGlobals->curtime + random->RandomFloat(5.0, 8.0);
		break;

	case HOUND_AE_ANGERSOUND1:
	{
		EmitSound("NPC_Houndeye.Anger1");
	}
	break;

	case HOUND_AE_ANGERSOUND2:
	{
		EmitSound("NPC_Houndeye.Anger2");
	}
	break;
	
	case HOUND_AE_HOPBACK:
	{
		float flGravity = sv_gravity.GetFloat();

		SetGroundEntity(NULL);

		Vector forward;
		AngleVectors(GetLocalAngles(), &forward);
		Vector vecNewVelocity = forward * -200;
		//jump up 36 inches
		vecNewVelocity.z += sqrt(72 * flGravity);
		SetAbsVelocity(vecNewVelocity);
		break;
	}

	case HOUND_AE_CLOSE_EYE:
		break;

	case HOUND_AE_LEAP_HIT:
	{
		//<<TEMP>>return;//<<TEMP>>
		SetGroundEntity(NULL);

		//
		// Take him off ground so engine doesn't instantly reset FL_ONGROUND.
		//
		UTIL_SetOrigin(this, GetLocalOrigin() + Vector(0, 0, 1));
		Vector vecJumpDir;
		if (GetEnemy() != NULL)
		{
			Vector vecEnemyEyePos = GetEnemy()->EyePosition();

			float gravity = GetCurrentGravity();
			if (gravity <= 1)
			{
				gravity = 1;
			}

			//
			// How fast does the houndeye need to travel to reach my enemy's eyes given gravity?
			//
			float height = (vecEnemyEyePos.z - GetAbsOrigin().z);
			if (height < 16)
			{
				height = 16;
			}
			else if (height > 120)
			{
				height = 120;
			}
			float speed = sqrt(2 * gravity * height);
			float time = speed / gravity;

			//
			// Scale the sideways velocity to get there at the right time
			//
			vecJumpDir = vecEnemyEyePos - GetAbsOrigin();
			vecJumpDir = vecJumpDir / time;

			//
			// Speed to offset gravity at the desired height.
			//
			vecJumpDir.z = speed;

			//
			// Don't jump too far/fast.
			//
			float distance = vecJumpDir.Length();
			if (distance > 650)
			{
				vecJumpDir = vecJumpDir * (650.0 / distance);
			}
		}
		else
		{
			Vector forward, up;
			AngleVectors(GetLocalAngles(), &forward, NULL, &up);
			//
			// Jump hop, don't care where.
			//
			vecJumpDir = Vector(forward.x, forward.y, up.z) * 350;
		}

		SetAbsVelocity(vecJumpDir);
		m_flNextAttack = gpGlobals->curtime + 2;
		break;
	}
	default:
		BaseClass::HandleAnimEvent(pEvent);
		break;
	}
}

int CNPC_HoundeyeNew::MeleeAttack1Conditions(float flDot, float flDist)
{
	if ((GetClawAttackRange()) < flDist)
		return COND_TOO_FAR_TO_ATTACK;

	if (flDot >= 0.7f)
		return COND_CAN_MELEE_ATTACK1;

	return COND_NOT_FACING_ATTACK;
}

Activity CNPC_HoundeyeNew::NPC_TranslateActivity(Activity baseAct)
{
	if (baseAct == ACT_MELEE_ATTACK1)
		return ACT_RANGE_ATTACK1;
	else
		return BaseClass::NPC_TranslateActivity(baseAct);
}

void CNPC_HoundeyeNew::RunTask(const Task_t* pTask)
{
	if ( ( pTask->iTask - TASK_WAIT_FOR_MOVEMENT ) > TASK_RESET_ACTIVITY )
	{
		BaseClass::RunTask(pTask);
	}
	else
	{
		BaseClass::RunTask(pTask);
		GetNavigator()->SetMovementActivity(ACT_RUN);
	}
}

void CNPC_HoundeyeNew::PrescheduleThink(void)
{
	if (gpGlobals->curtime > m_flNextMoanSound)
	{
		if (CanPlayMoanSound())
		{
			IdleSound();

			m_flNextMoanSound = gpGlobals->curtime + random->RandomFloat(15.0f, 10.0f);
		}
		else
		{
			m_flNextMoanSound = gpGlobals->curtime + random->RandomFloat(5.0f, 2.5f);
		}
	}

	BaseClass::PrescheduleThink();
}

void CNPC_HoundeyeNew::SetZombieModel()
{
	SetModel("models/houndeye.mdl");
	SetHullType(HULL_WIDE_SHORT);
	SetHullSizeNormal(true);
	SetBodygroup(ZOMBIE_BODYGROUP_HEADCRAB, !m_fIsHeadless);
	SetDefaultEyeOffset();
	SetActivity(ACT_IDLE);
}

void CNPC_HoundeyeNew::PainSound(const CTakeDamageInfo& info)
{
	if (IsOnFire())
	{
		return;
	}

	EmitSound("NPC_Houndeye.Pain");
}

void CNPC_HoundeyeNew::DeathSound(const CTakeDamageInfo& info)
{
	EmitSound("NPC_Houndeye.Die");
}

void CNPC_HoundeyeNew::AlertSound(void)
{
	EmitSound("NPC_Houndeye.Alert");

	m_flNextMoanSound += random->RandomFloat(2.0f, 4.0f);
}

void CNPC_HoundeyeNew::IdleSound(void)
{
	if ( (GetState() != NPC_STATE_IDLE || random->RandomFloat(0, 1) != 0.0f) && !IsSlumped() )
	{
		EmitSound("NPC_Houndeye.Idle");
		MakeAISpookySound(360.0f);
	}
}

const char* CNPC_HoundeyeNew::GetMoanSound(int nSound)
{
	return pMoanSounds[nSound % ARRAYSIZE(pMoanSounds)];
}

const char* CNPC_HoundeyeNew::GetLegsModel(void)
{
	return "models/zombie/zombie_soldier_legs.mdl";
}

const char* CNPC_HoundeyeNew::GetTorsoModel(void)
{
	return "models/zombie/zombie_soldier_torso.mdl";
}

const char* CNPC_HoundeyeNew::GetHeadcrabClassname(void)
{
	return "npc_headcrab";
}

const char* CNPC_HoundeyeNew::GetHeadcrabModel(void)
{
	return "models/headcrabclassic.mdl";
}

void CNPC_HoundeyeNew::SonicAttack()
{
	EmitSound("NPC_Houndeye.SonicAttack");

	UTIL_ScreenShake(GetAbsOrigin(), 20.0f, 150.0f, 1.0f, 1.0f, SHAKE_START, false);

	CBroadcastRecipientFilter filter;
	filter.AddAllPlayers();

	te->BeamRingPoint(filter, 0.0f, GetAbsOrigin(), 50.0f, 700.0f, m_iLightningEffect, 0, 0, 2, 0.1f, 128.0f, 0, 0.0f, 255, 255, 255, 32, 0, FBEAM_FADEOUT);
	
	te->BeamRingPoint(filter, 0.0f, GetAbsOrigin() + Vector(0,0,4), 50.0f, 700.0f, m_iLightningEffect, 0, 0, 2, 0.2f, 64.0f, 0, 0.0f, 255, 255, 255, 200, 0, FBEAM_FADEOUT);

	CTakeDamageInfo info(this, this, sk_houndeye_dmg_blast.GetFloat(), DMG_SHOCK);
	RadiusDamage(info, GetAbsOrigin(), 500.0f, CLASS_HOUNDEYE, this);
}
