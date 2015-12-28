//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "ai_basenpc.h"
#include "ai_senses.h"
#include "ai_tacticalservices.h"
#include "ai_route.h"
#include "ai_squad.h"
#include "basegrenade_shared.h"
#include "soundent.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sk_snark_health("sk_snark_health", "2");
ConVar sk_snark_dmg_bite("sk_snark_dmg_bite", "10");
ConVar sk_snark_dmg_pop("sk_snark_dmg_pop", "5");

#define SNARK_DAMAGE_RADIUS	30

class CHoe_NPC_Snark : public CAI_BaseNPC
{
	DECLARE_CLASS(CHoe_NPC_Snark, CAI_BaseNPC);
public:
	DECLARE_DATADESC();

	void Spawn(void);
	void Precache(void);
	Class_T  Classify(void);
	void SuperBounceTouch(CBaseEntity *pOther);
	void HuntThink(void);
	int  BloodColor(void) { return BLOOD_COLOR_YELLOW; }
	void Event_Killed(const CTakeDamageInfo& info);
	void GibMonster(void);

	float GetDamageRadius() { return SNARK_DAMAGE_RADIUS; }

	static float m_flNextBounceSoundTime;

	// CBaseEntity *m_pTarget;
	float m_flDie;
	Vector m_vecTarget;
	float m_flNextHunt;
	float m_flNextHit;
	Vector m_posPrev;
	EHANDLE m_hOwner;
	Class_T  m_iMyClass;
};

float CHoe_NPC_Snark::m_flNextBounceSoundTime = 0;

LINK_ENTITY_TO_CLASS(monster_snark, CHoe_NPC_Snark);

BEGIN_DATADESC(CHoe_NPC_Snark)
	DEFINE_FIELD(m_flDie, FIELD_TIME),
	DEFINE_FIELD(m_vecTarget, FIELD_VECTOR),
	DEFINE_FIELD(m_flNextHunt, FIELD_TIME),
	DEFINE_FIELD(m_flNextHit, FIELD_TIME),
	DEFINE_FIELD(m_posPrev, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(m_hOwner, FIELD_EHANDLE),

	DEFINE_ENTITYFUNC(SuperBounceTouch),
	DEFINE_THINKFUNC(HuntThink),

END_DATADESC()

#define SQUEEK_DETONATE_DELAY	15.0

Class_T CHoe_NPC_Snark::Classify(void)
{
	if (m_iMyClass != 0)
		return m_iMyClass; // protect against recursion

	if (GetEnemy() != NULL)
	{
		m_iMyClass = CLASS_INSECT; // no one cares about it
		switch (GetEnemy()->Classify())
		{
		case CLASS_PLAYER:
		case CLASS_HUMAN_PASSIVE:
		case CLASS_HUMAN_MILITARY:
			m_iMyClass = CLASS_NONE;
			return CLASS_ALIEN_MILITARY; // barney's get mad, grunts get mad at it
		}
		m_iMyClass = CLASS_NONE;
	}

	return CLASS_ALIEN_BIOWEAPON;
}

void CHoe_NPC_Snark::Spawn(void)
{
	Precache();

	// motor
	SetMoveType(MOVETYPE_FLYGRAVITY);

	SetSolid(SOLID_BBOX);

	SetModel("models/w_squeak.mdl");
	UTIL_SetSize(this, Vector(-4, -4, 0), Vector(4, 4, 8));
	UTIL_SetOrigin(this,  GetAbsOrigin());

	SetTouch(&CHoe_NPC_Snark::SuperBounceTouch);
	SetThink(&CHoe_NPC_Snark::HuntThink);
	SetNextThink(gpGlobals->curtime + 0.1);
	m_flNextHunt = gpGlobals->curtime + 1E6;

	AddFlag( FL_NPC );
	m_takedamage = DAMAGE_AIM;

	m_iHealth = m_iMaxHealth = sk_snark_health.GetInt();
	SetGravity( 0.5f );
	SetFriction( 0.5f );
	SetDamage(sk_snark_dmg_pop.GetFloat());

	m_flDie = gpGlobals->curtime + SQUEEK_DETONATE_DELAY;

	// m_flFieldOfView = 0; // 180 degrees

	if (GetOwnerEntity())
		m_hOwner = GetOwnerEntity();

	m_flNextBounceSoundTime = gpGlobals->curtime;// reset each time a snark is spawned.
	SetSequence( LookupSequence( "run" ) );
	ResetSequenceInfo();
}

void CHoe_NPC_Snark::Precache(void)
{
	PrecacheModel("models/w_squeak.mdl");

	PrecacheScriptSound("Snark.Die");
	PrecacheScriptSound("Snark.Gibbed");
	PrecacheScriptSound("Snark.Squeak");
	PrecacheScriptSound("Snark.Deploy");
	PrecacheScriptSound("Snark.Bounce");
}

void CHoe_NPC_Snark::Event_Killed(const CTakeDamageInfo& info)
{
	AddFlag( EF_NODRAW );
	
	// pev->model = iStringNull;// make invisible
	SetThink(&CHoe_NPC_Snark::SUB_Remove);
	SetTouch(NULL);
	SetNextThink( gpGlobals->curtime + 0.1f );

	// since squeak grenades never leave a body behind, clear out their takedamage now.
	// Squeaks do a bit of radius damage when they pop, and that radius damage will
	// continue to call this function unless we acknowledge the Squeak's death now. (sjb)
	m_takedamage = DAMAGE_NO;

	// play squeek blast
	EmitSound("Snark.Die");

	CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), BASEGRENADE_EXPLOSION_VOLUME, 3.0);

	UTIL_BloodDrips( GetAbsOrigin(), vec3_origin, BloodColor(), 80);

	RadiusDamage(info, GetAbsOrigin(), GetDamageRadius(), CLASS_NONE, this);

	if (m_hOwner != NULL)
		RadiusDamage(info, GetAbsOrigin(), GetDamageRadius(), CLASS_NONE, m_hOwner);
	else
		RadiusDamage(info, GetAbsOrigin(), GetDamageRadius(), CLASS_NONE, NULL);

	// reset owner so death message happens
	//if (m_hOwner != NULL)
	//	pev->owner = m_hOwner->edict();

	BaseClass::Event_Killed(info);
}

void CHoe_NPC_Snark::GibMonster(void)
{
	EmitSound("Snark.Gibbed");
}

void CHoe_NPC_Snark::HuntThink(void)
{
	// ALERT( at_console, "think\n" );

	if (!IsInWorld())
	{
		SetTouch(NULL);
		UTIL_Remove(this);
		return;
	}

	StudioFrameAdvance();
	SetNextThink(gpGlobals->curtime + 0.1);

	// explode when ready
	if (gpGlobals->curtime >= m_flDie)
	{
		m_iHealth = -1;

		CTakeDamageInfo info(this, this, GetDamage(), DMG_BLAST);
		info.SetDamageForce( GetAbsVelocity().Normalized() );

		Event_Killed(info);
		return;
	}

	// float
	if ( GetWaterLevel() != WL_NotInWater)
	{
		if (GetMoveType() == MOVETYPE_FLYGRAVITY)
		{
			SetMoveType(MOVETYPE_FLY);
		}
		Vector vNewVelocity = GetAbsVelocity() * 0.9f;
		vNewVelocity.z += 8.0f;

		SetAbsVelocity(vNewVelocity);

	}
	else if (GetMoveType() == MOVETYPE_FLY)
	{
		SetMoveType(MOVETYPE_FLYGRAVITY);
	}

	// return if not time to hunt
	if (m_flNextHunt > gpGlobals->curtime)
		return;

	m_flNextHunt = gpGlobals->curtime + 2.0;

	// CBaseEntity *pOther = NULL;
	Vector vecDir;
	trace_t tr;

	Vector vecFlat = GetAbsVelocity();
	vecFlat.z = 0;
	vecFlat = vecFlat.Normalized();
	
	// UTIL_MakeVectors(pev->angles);

	if (GetEnemy() == NULL || !GetEnemy()->IsAlive())
	{
		// find target, bounce a bit towards it.
		GetSenses()->Look(512);
		SetEnemy( BestEnemy() );
	}

	// squeek if it's about time blow up
	if ((m_flDie - gpGlobals->curtime <= 0.5) && (m_flDie - gpGlobals->curtime >= 0.3))
	{
		EmitSound("Snark.Squeak");
		CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 256, 0.25);
	}

	// higher pitch as squeeker gets closer to detonation time
	float flpitch = 155.0 - 60.0 * ((m_flDie - gpGlobals->curtime) / SQUEEK_DETONATE_DELAY);
	if (flpitch < 80)
		flpitch = 80;

	Vector vNewVel;

	if (GetEnemy() != NULL)
	{
		if (FVisible(GetEnemy()))
		{
			vecDir = GetEnemy()->EyePosition() - GetAbsOrigin();
			m_vecTarget = vecDir.Normalized();
		}

		float flVel = GetAbsVelocity().Length();
		float flAdj = 50.0 / (flVel + 10.0);

		if (flAdj > 1.2)
			flAdj = 1.2;

		// ALERT( at_console, "think : enemy\n");

		// ALERT( at_console, "%.0f %.2f %.2f %.2f\n", flVel, m_vecTarget.x, m_vecTarget.y, m_vecTarget.z );

		vNewVel = GetAbsVelocity();
		vNewVel = vNewVel * flAdj + m_vecTarget * 300;

		SetAbsVelocity(vNewVel);
	}

	if ( GetFlags() & FL_ONGROUND)
	{
		SetLocalAngularVelocity(QAngle(0, 0, 0));
	}
	else
	{
		QAngle qAngVelocity = GetLocalAngularVelocity();

		if (qAngVelocity == vec3_angle)
		{
			qAngVelocity.x = random->RandomFloat(-100, 100);
			qAngVelocity.z = random->RandomFloat(-100, 100);
		}

		SetLocalAngularVelocity(qAngVelocity);
	}

	vNewVel = GetAbsVelocity();

	if ((GetAbsOrigin() - m_posPrev).Length() < 1.0f)
	{
		vNewVel.x = random->RandomFloat(-100, 100);
		vNewVel.z = random->RandomFloat(-100, 100);
		SetAbsVelocity(vNewVel);
	}
	m_posPrev = GetAbsOrigin();

	QAngle angles;
	VectorAngles(vNewVel, angles);

	angles.z = 0;
	angles.x = 0;

	SetAbsAngles( angles );
}


void CHoe_NPC_Snark::SuperBounceTouch(CBaseEntity *pOther)
{
	float	flpitch;

	const trace_t& tr = pOther->GetTouchTrace();

	// don't hit the guy that launched this grenade
	if (GetOwnerEntity() && pOther == GetOwnerEntity())
		return;

	// at least until we've bounced once
	SetOwnerEntity(NULL);

	QAngle angles = GetAbsAngles();
	angles.x = 0;
	angles.z = 0;

	SetAbsAngles(angles);

	// avoid bouncing too much
	if (m_flNextHit > gpGlobals->curtime)
		return;

	// higher pitch as squeeker gets closer to detonation time
	flpitch = 155.0 - 60.0 * ((m_flDie - gpGlobals->curtime) / SQUEEK_DETONATE_DELAY);

	if (pOther->m_takedamage && m_flNextAttack < gpGlobals->curtime)
	{
		// attack!

		// make sure it's me who has touched them
		if (tr.m_pEnt == pOther)
		{
			// and it's not another squeakgrenade
			if (tr.m_pEnt->GetModelIndex() != GetModelIndex())
			{
				// ALERT( at_console, "hit enemy\n");
				ClearMultiDamage();


				CTakeDamageInfo info(this, this, sk_snark_dmg_bite.GetFloat(), DMG_SLASH);
				pOther->TakeDamage(info);

				ApplyMultiDamage();

				// pev->dmg += gSkillData.snarkDmgPop; // add more explosion damage
				// m_flDie += 2.0; // add more life

				// make bite sound
				EmitSound("Snark.Deploy");

				SetNextAttack(gpGlobals->curtime + 0.5);
			}
		}
		else
		{
			// ALERT( at_console, "been hit\n");
		}
	}

	m_flNextHit = gpGlobals->curtime + 0.1;
	m_flNextHunt = gpGlobals->curtime;

	if (g_pGameRules->IsMultiplayer())
	{
		// in multiplayer, we limit how often snarks can make their bounce sounds to prevent overflows.
		if (gpGlobals->curtime < m_flNextBounceSoundTime)
		{
			// too soon!
			return;
		}
	}

	if (!(GetFlags() & FL_ONGROUND))
	{
		// play bounce sound
		EmitSound("Snark.Bounce");
		CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 256, 0.25);
	}
	else
	{
		// skittering sound
		CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 100, 0.1);
	}

	m_flNextBounceSoundTime = gpGlobals->curtime + 0.5;// half second.
}