//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "npc_basezombie.h"
#include "ai_memory.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	SF_AZ_RANDOM_SKINS					(1 << 16) 

ConVar sk_armoured_zombie_health("sk_armoured_zombie_health", "0");
ConVar sk_armoured_zombie_dmg_one_slash("sk_armoured_zombie_dmg_one_slash", "0");

int ACT_ZOMBINE_ATTACK_FAST;

class CNPC_ArmouredZombie : public CNPC_BaseZombie
{
	DECLARE_CLASS(CNPC_ArmouredZombie, CNPC_BaseZombie);
	DEFINE_CUSTOM_AI;

public:

	void Spawn(void);
	void Precache(void);

	void SetZombieModel(void);

	void HandleAnimEvent(animevent_t *pEvent);

	virtual Activity NPC_TranslateActivity(Activity baseAct);

	void RunTask(const Task_t* pTask);

	bool ShouldBecomeTorso(const CTakeDamageInfo& info, float flDamageThreshold) { return false; }
	
	void PainSound(const CTakeDamageInfo& info);
	void DeathSound(const CTakeDamageInfo& info);
	void AlertSound(void);
	void IdleSound(void);
	void AttackSound(void) { return; }
	void AttackHitSound(void);
	void AttackMissSound(void);
	void FootstepSound(bool fRightFoot);
	void FootscuffSound(bool fRightFoot);

	virtual const char* GetMoanSound(int nSound);

	virtual const char* GetLegsModel(void);
	virtual const char* GetTorsoModel(void);
	virtual const char* GetHeadcrabClassname(void);
	virtual const char* GetHeadcrabModel(void);

protected:
	static const char* pMoanSounds[];
};

LINK_ENTITY_TO_CLASS(npc_armouredzombie, CNPC_ArmouredZombie);

AI_BEGIN_CUSTOM_NPC(npc_armouredzombie, CNPC_ArmouredZombie)
	DECLARE_ACTIVITY(ACT_ZOMBINE_ATTACK_FAST)
AI_END_CUSTOM_NPC()

const char* CNPC_ArmouredZombie::pMoanSounds[] =
{
	 "ATV_engine_null",
};

void CNPC_ArmouredZombie::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel("models/zombie/ArmouredClassic.mdl");

	PrecacheScriptSound("Zombie.FootstepRight");
	PrecacheScriptSound("Zombie.FootstepLeft");
	PrecacheScriptSound("Zombine.ScuffRight");
	PrecacheScriptSound("Zombine.ScuffLeft");
	PrecacheScriptSound("Zombie.AttackHit");
	PrecacheScriptSound("Zombie.AttackMiss");
	PrecacheScriptSound("Zombine.Pain");
	PrecacheScriptSound("Zombine.Die");
	PrecacheScriptSound("Zombine.Alert");
	PrecacheScriptSound("Zombine.Idle");
	PrecacheScriptSound("ATV_engine_null");
	PrecacheScriptSound("Zombine.Charge");
	PrecacheScriptSound("Zombie.Attack");
}

void CNPC_ArmouredZombie::SetZombieModel(void)
{
	int skin;

	SetModel("models/zombie/ArmouredClassic.mdl");
	SetHullType(HULL_HUMAN);

	if (HasSpawnFlags(SF_AZ_RANDOM_SKINS))
		skin = random->RandomInt(2, 5);
	else
		skin = 1;

	m_nSkin = skin;

	SetBodygroup(ZOMBIE_BODYGROUP_HEADCRAB, !m_fIsHeadless);

	SetHullSizeNormal(true);
	SetDefaultEyeOffset();
	SetActivity(ACT_IDLE);
}

void CNPC_ArmouredZombie::Spawn(void)
{
	float flHeatlh;

	Precache();

	m_fIsTorso = false;
	m_fIsHeadless = true;

	SetBloodColor(BLOOD_COLOR_RED);

	if (HasSpawnFlags(SF_AZ_RANDOM_SKINS))
		flHeatlh = (sk_armoured_zombie_health.GetFloat() + 60.0f);
	else
		flHeatlh = sk_armoured_zombie_health.GetFloat();
	
	m_iHealth = flHeatlh;
	SetMaxHealth(m_iHealth);

	m_flFieldOfView = -1.0f;

	BaseClass::Spawn();

	GetEnemies()->SetFreeKnowledgeDuration(90.0f);

	m_NPCState = NPC_STATE_COMBAT;

	m_flNextMoanSound = (random->RandomFloat(1.0f, 4.0f) + gpGlobals->curtime);
}


//-----------------------------------------------------------------------------
// Purpose: Catches the monster-specific events that occur when tagged animation
//			frames are played.
// Input  : pEvent - 
//-----------------------------------------------------------------------------
void CNPC_ArmouredZombie::HandleAnimEvent(animevent_t *pEvent)
{
	if (pEvent->event == ACT_ZOMBINE_ATTACK_FAST)
	{
		Vector right, forward;
		AngleVectors(GetLocalAngles(), &forward, &right, NULL);

		right = right * 100;
		forward = forward * 200;

		QAngle qa(-15, -20, -10);
		Vector vec = right + forward;
		ClawAttack(GetClawAttackRange(), sk_armoured_zombie_dmg_one_slash.GetFloat(), qa, vec, ZOMBIE_BLOOD_RIGHT_HAND);
		return;
	}
	BaseClass::HandleAnimEvent(pEvent);
}

Activity CNPC_ArmouredZombie::NPC_TranslateActivity(Activity baseAct)
{
	if (baseAct == ACT_MELEE_ATTACK1)
		return (Activity)ACT_ZOMBINE_ATTACK_FAST;
	else
		return BaseClass::NPC_TranslateActivity(baseAct);
}

void CNPC_ArmouredZombie::RunTask(const Task_t* pTask)
{
	if (pTask->iTask)
	{
		BaseClass::RunTask(pTask);
	}
	else
	{
		BaseClass::RunTask(pTask);
		if (!IsOnFire())
			GetNavigator()->SetMovementActivity(ACT_RANGE_ATTACK1);

		GetEnemy();
		GetNavigator()->SetMovementActivity(ACT_RANGE_ATTACK1);
	}
}

void CNPC_ArmouredZombie::PainSound(const CTakeDamageInfo& info)
{
	if (IsOnFire())
	{
		return;
	}

	EmitSound("Zombine.Pain");
}

void CNPC_ArmouredZombie::DeathSound(const CTakeDamageInfo& info)
{
	EmitSound("Zombine.Die");
}

void CNPC_ArmouredZombie::AlertSound(void)
{
	EmitSound("Zombine.Alert");

	m_flNextMoanSound += random->RandomFloat(2.0f, 4.0f);
}

void CNPC_ArmouredZombie::IdleSound(void)
{
	if (GetState() == NPC_STATE_IDLE && random->RandomFloat(0, 1) == 0)
	{
		return;
	}

	if (IsSlumped())
	{
		return;
	}

	EmitSound("Zombine.Idle");
	MakeAISpookySound(360.0f);
}

void CNPC_ArmouredZombie::AttackHitSound(void)
{
	EmitSound("Zombie.AttackHit");
}

void CNPC_ArmouredZombie::AttackMissSound(void)
{
	EmitSound("Zombie.AttackMiss");
}

void CNPC_ArmouredZombie::FootstepSound(bool fRightFoot)
{
	if (fRightFoot)
	{
		EmitSound("Zombie.FootstepRight");
	}
}

void CNPC_ArmouredZombie::FootscuffSound(bool fRightFoot)
{
	if (fRightFoot)
	{
		EmitSound("Zombine.ScuffRight");
	}
}

const char* CNPC_ArmouredZombie::GetMoanSound(int nSound)
{
	return pMoanSounds[nSound % ARRAYSIZE(pMoanSounds)];
}

const char* CNPC_ArmouredZombie::GetLegsModel(void)
{
	return "models/zombie/zombie_soldier_legs.mdl";
}

const char* CNPC_ArmouredZombie::GetTorsoModel(void)
{
	return "models/zombie/zombie_soldier_torso.mdl";
}

const char* CNPC_ArmouredZombie::GetHeadcrabClassname(void)
{
	return "npc_headcrab";
}

const char* CNPC_ArmouredZombie::GetHeadcrabModel(void)
{
	return "models/headcrabclassic.mdl";
}

void CNPC_ArmouredZombie::InitCustomSchedules(void)
{
	INIT_CUSTOM_AI(CNPC_ArmouredZombie);

	ADD_CUSTOM_ACTIVITY(CNPC_ArmouredZombie, ACT_ZOMBINE_ATTACK_FAST);
}