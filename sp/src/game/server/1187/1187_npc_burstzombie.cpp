//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_basezombie.h"
#include "npc_headcrab.h"
#include "te_effect_dispatch.h"
#include "gib.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sk_burst_zombie_health("sk_burst_zombie_health", "35", FCVAR_NONE);
ConVar sk_burst_zombie_numcrabs("sk_burst_zombie_numcrabs", "20", FCVAR_NONE, "Number of little crabs this fucker gives out.", true, 1.0f, true, 20.0f);

class CNPC_BurstZombie : public CNPC_BaseZombie
{
	DECLARE_CLASS(CNPC_BurstZombie, CNPC_BaseZombie );
public:

	void Spawn(void);
	void Precache(void);

	void RunTask(const Task_t* pTask);
	void Event_Killed(const CTakeDamageInfo& info);
	void TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator);
	int OnTakeDamage_Alive(const CTakeDamageInfo& info);

	void SetZombieModel(void);

	void PainSound(const CTakeDamageInfo& info);
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
private:
	void SpawnCrabs();
};

LINK_ENTITY_TO_CLASS(npc_burstzombie, CNPC_BurstZombie);

const char* CNPC_BurstZombie::pMoanSounds[] =
{
	 "ATV_engine_null",
};

void CNPC_BurstZombie::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel("models/zombie/burstclassic.mdl");
	PrecacheModel("models/zombie/burstclassic_arml.mdl");
	PrecacheModel("models/zombie/burstclassic_armr.mdl");
	PrecacheModel("models/zombie/burstclassic_legs.mdl");

	PrecacheScriptSound("BurstZombie.Burst");
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

void CNPC_BurstZombie::SetZombieModel(void)
{
	SetModel("models/zombie/burstclassic.mdl");
	SetHullType(HULL_HUMAN);

	SetBodygroup(ZOMBIE_BODYGROUP_HEADCRAB, !m_fIsHeadless);

	SetHullSizeNormal(true);
	SetDefaultEyeOffset();
	SetActivity(ACT_IDLE);
}

void CNPC_BurstZombie::PainSound(const CTakeDamageInfo& info)
{
	if (IsOnFire())
	{
		return;
	}

	EmitSound("Zombine.Pain");
}

void CNPC_BurstZombie::AlertSound(void)
{
	EmitSound("Zombine.Alert");

	m_flNextMoanSound += random->RandomFloat(2.0f, 4.0f);
}

void CNPC_BurstZombie::IdleSound(void)
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

void CNPC_BurstZombie::AttackHitSound(void)
{
	EmitSound("Zombie.AttackHit");
}

void CNPC_BurstZombie::AttackMissSound(void)
{
	EmitSound("Zombie.AttackMiss");
}

void CNPC_BurstZombie::FootstepSound(bool fRightFoot)
{
	if (fRightFoot)
	{
		EmitSound("Zombie.FootstepRight");
	}
}

void CNPC_BurstZombie::FootscuffSound(bool fRightFoot)
{
	if (fRightFoot)
	{
		EmitSound("Zombine.ScuffRight");
	}
}

const char* CNPC_BurstZombie::GetMoanSound(int nSound)
{
	return pMoanSounds[nSound % ARRAYSIZE(pMoanSounds)];
}

const char* CNPC_BurstZombie::GetLegsModel(void)
{
	return "models/zombie/zombie_soldier_legs.mdl";
}

const char* CNPC_BurstZombie::GetTorsoModel(void)
{
	return "models/zombie/zombie_soldier_torso.mdl";
}

void CNPC_BurstZombie::Spawn(void)
{
	Precache();

	m_fIsTorso = false;
	m_fIsHeadless = true;

	SetBloodColor(BLOOD_COLOR_RED);

	m_iHealth = sk_burst_zombie_health.GetFloat();
	SetMaxHealth(m_iHealth);

	m_flFieldOfView = 0.2f;

	CapabilitiesClear();
	BaseClass::Spawn();

	m_flNextMoanSound = (random->RandomFloat(1.0f, 4.0f) + gpGlobals->curtime);
}

void CNPC_BurstZombie::RunTask(const Task_t* pTask)
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

void CNPC_BurstZombie::Event_Killed(const CTakeDamageInfo& info)
{
	if (info.GetDamageType() & DMG_BLAST)
	{
		Vector vecForce = vec3_origin;
		vecForce.x = random->RandomFloat(-400, 400);
		vecForce.y = random->RandomFloat(-400, 400);
		vecForce.z = random->RandomFloat(0, 250) * 30.0f;

		CreateRagGib("models/headcrabburst.mdl", GetAbsOrigin(), GetAbsAngles(), vecForce, 15.0f);
		CreateRagGib("models/headcrabburst.mdl", GetAbsOrigin(), GetAbsAngles(), vecForce, 15.0f);
		CreateRagGib("models/headcrabburst.mdl", GetAbsOrigin(), GetAbsAngles(), vecForce, 15.0f);
		CreateRagGib("models/headcrabburst.mdl", GetAbsOrigin(), GetAbsAngles(), vecForce, 15.0f);
		CreateRagGib("models/headcrabburst.mdl", GetAbsOrigin(), GetAbsAngles(), vecForce, 15.0f);
		CreateRagGib("models/headcrabburst.mdl", GetAbsOrigin(), GetAbsAngles(), vecForce, 15.0f);
		
		CreateRagGib("models/zombie/burstclassic_armr.mdl", GetAbsOrigin(), GetAbsAngles(), vecForce, 15.0f);
		CreateRagGib("models/zombie/burstclassic_arml.mdl", GetAbsOrigin(), GetAbsAngles(), vecForce, 15.0f);
		
		Vector dir;
		dir = Vector(0,0,-1);
		UTIL_BloodSpray(GetAbsOrigin(), dir, BLOOD_COLOR_RED, 8, FX_BLOODSPRAY_ALL);
		EmitSound("BurstZombie.Burst");
		UTIL_Remove(this);
	}
	else
	{
		UTIL_ScreenShake(GetAbsOrigin(), 100.0f, 20.0f, 0.89999998f, 512.0f, SHAKE_START);
	
		Vector vecLegsForce(4200,4200,4800);

		CreateRagGib("models/zombie/burstclassic_legs.mdl", GetAbsOrigin(), GetAbsAngles(), vecLegsForce, 15.0f);
	
		Vector vecRightArmForce;
		vecRightArmForce.x = random->RandomFloat(-400, 400);
		vecRightArmForce.y = random->RandomFloat(-400, 400);
		vecRightArmForce.z = random->RandomFloat(200, 250) * 30.0f;

		CreateRagGib("models/zombie/burstclassic_armr.mdl", GetAbsOrigin(), GetAbsAngles(), vecRightArmForce, 15.0f);
	
		Vector vecLeftArmForce;
		vecLeftArmForce.x = random->RandomFloat(-400, 400) * -20.0f;
		vecLeftArmForce.y = random->RandomFloat(-400, 400);
		vecLeftArmForce.z = random->RandomFloat(200, 250) * 20.0f;

		CreateRagGib("models/zombie/burstclassic_arml.mdl", GetAbsOrigin(), GetAbsAngles(), vecLeftArmForce, 15.0f);
	
		Vector dir;
		dir = Vector(0, 0, -1);
		UTIL_BloodSpray(GetAbsOrigin(), dir, BLOOD_COLOR_RED, 8, FX_BLOODSPRAY_ALL);
		EmitSound("BurstZombie.Burst");

		int i;
		for (i = 0; i < sk_burst_zombie_numcrabs.GetInt(); ++i)
			SpawnCrabs();

		CEffectData	data;
		
		data.m_flScale = 3.0f;

		DispatchEffect("ZombieGib", data);
		
		Vector vecAbsStart;

		vecAbsStart.x = GetAbsOrigin().x;
		vecAbsStart.y = GetAbsOrigin().y;
		vecAbsStart.z = GetAbsOrigin().z + 8.0f;

		Vector dirEnd;

		dirEnd.x = vecAbsStart.x;
		dirEnd.y = vecAbsStart.y;
		dirEnd.z = vecAbsStart.z - 24.0f;

		trace_t	tr;
		UTIL_TraceLine(vecAbsStart, dirEnd, MASK_SOLID_BRUSHONLY, this, NULL, &tr);
		UTIL_Remove(this);
	}
}

void CNPC_BurstZombie::TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator)
{
	CTakeDamageInfo dmginfo(info);
	BaseClass::TraceAttack(info, vecDir, ptr, pAccumulator);
}

int CNPC_BurstZombie::OnTakeDamage_Alive(const CTakeDamageInfo& info)
{
	CTakeDamageInfo dmginfo(info);
	return BaseClass::OnTakeDamage_Alive(dmginfo);
}

const char* CNPC_BurstZombie::GetHeadcrabClassname(void)
{
	return "npc_headcrab_burst";
}

const char* CNPC_BurstZombie::GetHeadcrabModel(void)
{
	return "models/headcrabblack.mdl";
}

void CNPC_BurstZombie::SpawnCrabs()
{
	Vector vecAbsVelocity = vec3_origin;

	vecAbsVelocity.x = random->RandomInt(-8, 8);
	vecAbsVelocity.y = random->RandomInt(-8, 8);
	vecAbsVelocity.z = 96.0f;

	Vector vecPosition;

	vecPosition.x = GetAbsOrigin().x + random->RandomInt(-1, 1);
	vecPosition.y = GetAbsOrigin().y + random->RandomInt(-1, 1);
	vecPosition.z = GetAbsOrigin().z + random->RandomInt(30, 60);

	QAngle vecAngles;

	vecAngles.x = 0.0f;
	vecAngles.y = random->RandomFloat(0.0f, 360.0f);
	vecAngles.z = 0.0f;

	CBurstHeadcrab* pCrab = (CBurstHeadcrab*)CreateNoSpawn(GetHeadcrabClassname(), vecPosition, vecAngles, this);
	if (pCrab)
	{
		pCrab->AddFlag(FL_GRAPHED);
		pCrab->SetAbsVelocity(vecAbsVelocity);
		pCrab->SetOwnerEntity(this);
		pCrab->Spawn();
	}
}
