//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

//=========================================================
// Alien slave monster
//=========================================================

#include "cbase.h"
#include "npcevent.h"
#include "ai_basenpc.h"
#include "ai_navigator.h"
#include "ai_senses.h"
#include "ai_squad.h"
#include "ai_tacticalservices.h"
#include "ai_route.h"
#include "ai_speech.h"

#include "player.h"
#include "vstdlib/random.h"
#include "soundent.h"
#include "engine/IEngineSound.h"

#include "beam_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sk_vortigaunt_health;
extern ConVar sk_vortigaunt_dmg_claw;
extern ConVar sk_vortigaunt_dmg_rake;
extern ConVar sk_vortigaunt_dmg_zap;

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define		ISLAVE_AE_CLAW		( 1 )
#define		ISLAVE_AE_CLAWRAKE	( 2 )
#define		ISLAVE_AE_ZAP_POWERUP	( 3 )
#define		ISLAVE_AE_ZAP_SHOOT		( 4 )
#define		ISLAVE_AE_ZAP_DONE		( 5 )

#define		ISLAVE_MAX_BEAMS	8

class CHoe_NPC_ISlave : public CAI_BaseNPC
{
	DECLARE_CLASS(CHoe_NPC_ISlave, CAI_BaseNPC);
public:
	DECLARE_DATADESC();
	DEFINE_CUSTOM_AI;

	void Spawn(void);
	void Precache(void);
	float MaxYawSpeed(void);
	int GetSoundInterests();
	Class_T  Classify(void);
	Disposition_t IRelationType(CBaseEntity *pTarget);
	void HandleAnimEvent(animevent_t *pEvent);
	int RangeAttack1Conditions(float flDot, float flDist);
	int RangeAttack2Conditions(float flDot, float flDist);

	void CallForHelp(char *szClassname, float flDist, EHANDLE hEnemy, Vector &vecLocation);
	void TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator);
	int	 OnTakeDamage_Alive(const CTakeDamageInfo &info);

	void PainSound(const CTakeDamageInfo &info);
	void DeathSound(const CTakeDamageInfo &info);
	void AlertSound(void);
	void IdleSound(void);

	void Event_Killed(const CTakeDamageInfo &info);

	void StartTask(const Task_t *pTask);
	int SelectSchedule(void);
	int TranslateSchedule(int scheduleType);

	void ClearBeams();
	void ArmBeam(int side);
	void WackBeam(int side, CBaseEntity *pEntity);
	void ZapBeam(int side);
	void BeamGlow(void);

	int m_iBravery;

	CBeam *m_pBeam[ISLAVE_MAX_BEAMS];

	int m_iBeams;
	float m_flNextAttack;

	int	m_voicePitch;

	EHANDLE m_hDead;

private:

	//=========================================================
	// Schedules
	//=========================================================
	enum
	{
		SCHED_SLAVE_RANGE_ATTACK1 = BaseClass::NEXT_SCHEDULE,
		NEXT_SCHEDULE,
	};
};

LINK_ENTITY_TO_CLASS(npc_vortigaunt, CHoe_NPC_ISlave);

BEGIN_DATADESC(CHoe_NPC_ISlave)

	DEFINE_FIELD(m_iBravery, FIELD_INTEGER),

	DEFINE_ARRAY(m_pBeam, FIELD_CLASSPTR, ISLAVE_MAX_BEAMS),
	DEFINE_FIELD(m_iBeams, FIELD_INTEGER),
	DEFINE_FIELD(m_flNextAttack, FIELD_TIME),

	DEFINE_FIELD(m_voicePitch, FIELD_INTEGER),

	DEFINE_FIELD(m_hDead, FIELD_EHANDLE),

END_DATADESC()

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
Class_T	CHoe_NPC_ISlave::Classify(void)
{
	return	CLASS_ALIEN_MILITARY;
}

Disposition_t CHoe_NPC_ISlave::IRelationType(CBaseEntity *pTarget)
{
	if ((pTarget->IsPlayer()))
	{
		if (( GetSpawnFlags() & SF_NPC_WAIT_FOR_SCRIPT) && !(m_afMemory & bits_MEMORY_PROVOKED))
			return D_NU;
	}

	return BaseClass::IRelationType(pTarget);
}


void CHoe_NPC_ISlave::CallForHelp(char *szClassname, float flDist, EHANDLE hEnemy, Vector &vecLocation)
{
	// ALERT( at_aiconsole, "help " );

	// skip ones not on my netname
	if (GetEntityName() == NULL_STRING)
		return;

	CBaseEntity *pEntity = NULL;

	while ((pEntity = gEntList.FindEntityByName(pEntity, GetEntityName())) != NULL)
	{
		float d = (GetAbsOrigin() - pEntity->GetAbsOrigin()).Length();
		if (d < flDist)
		{
			CAI_BaseNPC *pNPC = pEntity->MyNPCPointer();
			if (pNPC)
			{
				pNPC->Remember(bits_MEMORY_PROVOKED);
				// pNPC->PushEnemy(hEnemy, vecLocation);
			}
		}
	}
}


//=========================================================
// ALertSound - scream
//=========================================================
void CHoe_NPC_ISlave::AlertSound(void)
{
	if (GetEnemy() != NULL)
	{
		SENTENCEG_PlayRndSz(edict(), "SLV_ALERT", 0.85, SNDLVL_NORM, 0, 1);

		CallForHelp("monster_alien_slave", 512, GetEnemy(), Vector(GetEnemyLKP()));
	}
}

//=========================================================
// IdleSound
//=========================================================
void CHoe_NPC_ISlave::IdleSound(void)
{
	if (random->RandomInt(0, 2) == 0)
	{
		SENTENCEG_PlayRndSz(edict(), "SLV_IDLE", 0.85, SNDLVL_NORM, 0, 1);
	}
}

//=========================================================
// PainSound
//=========================================================
void CHoe_NPC_ISlave::PainSound(const CTakeDamageInfo& info)
{
	if (random->RandomInt(0, 2) == 0)
	{
		EmitSound("NPC_Vortigaunt.Pain");
	}
}

//=========================================================
// DieSound
//=========================================================

void CHoe_NPC_ISlave::DeathSound(const CTakeDamageInfo& info)
{
	EmitSound("NPC_Vortigaunt.Die");
}


//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards. 
//=========================================================
int CHoe_NPC_ISlave::GetSoundInterests(void)
{
	return  SOUND_WORLD | 
		SOUND_COMBAT | 
		SOUND_DANGER | 
		SOUND_PLAYER;
}

void CHoe_NPC_ISlave::Event_Killed(const CTakeDamageInfo& info)
{
	ClearBeams();
	BaseClass::Event_Killed(info);
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
float CHoe_NPC_ISlave::MaxYawSpeed(void)
{
	switch (GetActivity())
	{
	case ACT_WALK:
		return 50;
	case ACT_RUN:
		return 70;
	case ACT_IDLE:
		return 50;
	default:
		return 90;
	}
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//
// Returns number of events handled, 0 if none.
//=========================================================
void CHoe_NPC_ISlave::HandleAnimEvent(animevent_t *pEvent)
{
	Vector vecMins = GetHullMins();
	Vector vecMaxs = GetHullMaxs();
	vecMins.z = vecMins.x;
	vecMaxs.z = vecMaxs.x;

	// ALERT( at_console, "event %d : %f\n", pEvent->event, pev->frame );
	switch (pEvent->event)
	{
	case ISLAVE_AE_CLAW:
	{
		// SOUND HERE!
		CBaseEntity *pHurt = CheckTraceHullAttack(70, vecMins, vecMaxs, sk_vortigaunt_dmg_claw.GetInt(), DMG_SLASH);
		if (pHurt)
		{
			if (pHurt->IsPlayer())
			{
				pHurt->ViewPunch( QAngle( 5, 0, -18 ) );
			}

			// Play a random attack hit sound
			EmitSound("NPC_Vortigaunt.AttackHit");
		}
		else
		{
			// Play a random attack miss sound
			EmitSound("NPC_Vortigaunt.AttackMiss");
		}
	}
	break;

	case ISLAVE_AE_CLAWRAKE:
	{
		CBaseEntity *pHurt = CheckTraceHullAttack(70, vecMins, vecMaxs, sk_vortigaunt_dmg_rake.GetInt(), DMG_SLASH);
		if (pHurt)
		{
			if (pHurt->IsPlayer())
			{
				pHurt->ViewPunch(QAngle(5, 0, -18));
			}

			EmitSound("NPC_Vortigaunt.AttackHit");
		}
		else
		{
			EmitSound("NPC_Vortigaunt.AttackMiss");
		}
	}
	break;

	case ISLAVE_AE_ZAP_POWERUP:
	{
		// speed up attack when on hard
		if (g_pGameRules->IsSkillLevel(SKILL_HARD))
			m_flPlaybackRate = 1.5f;

		Vector forward;
		AngleVectors(GetLocalAngles(), &forward);

		if (m_iBeams == 0)
		{
			Vector vecSrc = GetAbsOrigin() + forward * 2;
			CBroadcastRecipientFilter filter;
			te->DynamicLight(filter, 0.0, &vecSrc, 255, 180, 96, 0, 12, 20 / m_flPlaybackRate, 0);
#if 0
			MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(vecSrc.x);	// X
			WRITE_COORD(vecSrc.y);	// Y
			WRITE_COORD(vecSrc.z);	// Z
			WRITE_BYTE(12);		// radius * 0.1
			WRITE_BYTE(255);		// r
			WRITE_BYTE(180);		// g
			WRITE_BYTE(96);		// b
			WRITE_BYTE(20 / pev->framerate);		// time * 10
			WRITE_BYTE(0);		// decay * 0.1
			MESSAGE_END();
#endif

		}
		if (m_hDead != NULL)
		{
			WackBeam(-1, m_hDead);
			WackBeam(1, m_hDead);
		}
		else
		{
			ArmBeam(-1);
			ArmBeam(1);
			BeamGlow();
		}

		EmitSound("NPC_Vortigaunt.ZapPowerup" );

		// EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "debris/zap4.wav", 1, ATTN_NORM, 0, 100 + m_iBeams * 10);
		m_nSkin = m_iBeams / 2;
	}
	break;

	case ISLAVE_AE_ZAP_SHOOT:
	{
		ClearBeams();

		if (m_hDead != NULL)
		{
			Vector vecDest = m_hDead->GetAbsOrigin() + Vector(0, 0, 38);
			trace_t trace;
			UTIL_TraceHull(vecDest, vecDest, GetHullMins(), GetHullMaxs(), MASK_SOLID, m_hDead, COLLISION_GROUP_NONE, &trace );

			if (!trace.startsolid)
			{
				CBaseEntity *pNew = CBaseEntity::Create(GetClassname(), m_hDead->GetAbsOrigin(), m_hDead->GetAbsAngles());
				//CAI_BaseNPC *pNewNPC = pNew->MyNPCPointer();
				pNew->AddSpawnFlags( 1 );
				WackBeam(-1, pNew);
				WackBeam(1, pNew);
				UTIL_Remove(m_hDead);
				EmitSound("NPC_Vortigaunt.ZapShoot");

				break;
			}
		}
		ClearMultiDamage();

		ZapBeam(-1);
		ZapBeam(1);

		EmitSound("NPC_Vortigaunt.ZapShoot");
		ApplyMultiDamage();

		m_flNextAttack = gpGlobals->curtime + random->RandomFloat(0.5, 4.0);
	}
	break;

	case ISLAVE_AE_ZAP_DONE:
	{
		ClearBeams();
	}
	break;

	default:
		BaseClass::HandleAnimEvent(pEvent);
		break;
	}
}

//=========================================================
// CheckRangeAttack1 - normal beam attack 
//=========================================================
int CHoe_NPC_ISlave::RangeAttack1Conditions(float flDot, float flDist)
{
	if (m_flNextAttack > gpGlobals->curtime)
	{
		return FALSE;
	}

	return BaseClass::RangeAttack1Conditions(flDot, flDist);
}

//=========================================================
// CheckRangeAttack2 - check bravery and try to resurect dead comrades
//=========================================================
BOOL CHoe_NPC_ISlave::RangeAttack2Conditions(float flDot, float flDist)
{
	return COND_NONE;
}


//=========================================================
// StartTask
//=========================================================
void CHoe_NPC_ISlave::StartTask(const Task_t *pTask)
{
	ClearBeams();

	BaseClass::StartTask(pTask);
}


//=========================================================
// Spawn
//=========================================================
void CHoe_NPC_ISlave::Spawn()
{
	Precache();

	SetModel("models/islave.mdl");

	SetHullType( HULL_HUMAN );
	SetHullSizeNormal();

	SetSolid( SOLID_BBOX );
	AddSolidFlags(  FSOLID_NOT_STANDABLE );

	SetMoveType(MOVETYPE_STEP);
	SetBloodColor(BLOOD_COLOR_GREEN);

	m_iHealth = m_iMaxHealth = sk_vortigaunt_health.GetInt();

	SetViewOffset(Vector(0, 0, 64));// position of the eyes relative to monster's origin.
	m_flFieldOfView = VIEW_FIELD_WIDE; // NOTE: we need a wide field of view so npc will notice player and say hello
	m_NPCState = NPC_STATE_NONE;

	CapabilitiesClear();
	CapabilitiesAdd(bits_CAP_MOVE_GROUND | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP);
	CapabilitiesAdd( bits_CAP_INNATE_MELEE_ATTACK1 | bits_CAP_INNATE_RANGE_ATTACK1 );

	if (GetExpresser())
		GetExpresser()->SetVoicePitch( random->RandomInt(85, 110) );

	NPCInit();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CHoe_NPC_ISlave::Precache()
{
	PrecacheModel("models/islave.mdl");
	PrecacheModel("sprites/lgtning.spr");

	PrecacheSound("debris/zap1.wav");
	PrecacheSound("debris/zap4.wav");
	PrecacheSound("weapons/electro4.wav");
	PrecacheSound("hassault/hw_shoot1.wav");
	PrecacheSound("zombie/zo_pain2.wav");
	PrecacheSound("headcrab/hc_headbite.wav");
	PrecacheSound("weapons/cbar_miss1.wav");

	PrecacheScriptSound("NPC_Vortigaunt.Pain");
	PrecacheScriptSound("NPC_Vortigaunt.Die");
	PrecacheScriptSound("NPC_Vortigaunt.AttackHit");
	PrecacheScriptSound("NPC_Vortigaunt.AttackMiss");
	PrecacheScriptSound("NPC_Vortigaunt.ZapPowerup");
	PrecacheScriptSound("NPC_Vortigaunt.ZapShoot");
	PrecacheScriptSound("NPC_Vortigaunt.WTF");
	PrecacheScriptSound("NPC_Vortigaunt.Teleport");

	UTIL_PrecacheOther("test_effect");
}


//=========================================================
// TakeDamage - get provoked when injured
//=========================================================

int CHoe_NPC_ISlave::OnTakeDamage_Alive(const CTakeDamageInfo& info)
{
	// don't slash one of your own
	if ((info.GetDamageType() & DMG_SLASH) && info.GetAttacker() && IRelationType(info.GetAttacker()) < D_FR)
		return 0;

	Remember(bits_MEMORY_PROVOKED);

	return BaseClass::OnTakeDamage_Alive(info);
}

void CHoe_NPC_ISlave::TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator)
{
	if (info.GetDamageType() & DMG_SHOCK)
		return;

	BaseClass::TraceAttack(info, vecDir, ptr, pAccumulator);
}

//=========================================================
//=========================================================
int CHoe_NPC_ISlave::SelectSchedule(void)
{
	ClearBeams();

	if (HasCondition(COND_HEAR_COMBAT) || HasCondition(COND_HEAR_DANGER))
	{
		CSound *pSound = GetBestSound();

		Assert(pSound != NULL);

		if (pSound->IsSoundType(SOUND_DANGER))
			return SCHED_TAKE_COVER_FROM_BEST_SOUND;
		if (pSound->IsSoundType(SOUND_COMBAT))
			Remember(bits_MEMORY_PROVOKED);
	}

	switch (m_NPCState)
	{
	case NPC_STATE_COMBAT:
		// dead enemy
		if (HasCondition(COND_ENEMY_DEAD))
		{
			// call base class, all code to handle dead enemies is centralized there.
			return BaseClass::SelectSchedule();
		}

		if (GetHealth() < 20 || m_iBravery < 0)
		{
			if (!HasCondition(COND_CAN_MELEE_ATTACK1))
			{
				SetDefaultFailSchedule(SCHED_CHASE_ENEMY);

				if (HasCondition( COND_LIGHT_DAMAGE ) || HasCondition( COND_HEAVY_DAMAGE ))
				{
					return SCHED_TAKE_COVER_FROM_ENEMY;
				}
				if (HasCondition(COND_SEE_ENEMY) && HasCondition(COND_ENEMY_FACING_ME))
				{
					// ALERT( at_console, "exposed\n");
					return SCHED_TAKE_COVER_FROM_ENEMY;
				}
			}
		}
		break;
	}

	return BaseClass::SelectSchedule();
}


int CHoe_NPC_ISlave::TranslateSchedule(int scheduleType)
{
	switch (scheduleType)
	{
	case SCHED_FAIL:
	{
		if (HasCondition(COND_CAN_MELEE_ATTACK1))
		{
			return BaseClass::TranslateSchedule(SCHED_MELEE_ATTACK1);
		}
	}
	break;
	case SCHED_RANGE_ATTACK1:
		return SCHED_SLAVE_RANGE_ATTACK1;
	case SCHED_RANGE_ATTACK2:
		return SCHED_SLAVE_RANGE_ATTACK1;
	}
	return BaseClass::TranslateSchedule(scheduleType);
}


//=========================================================
// ArmBeam - small beam from arm to nearby geometry
//=========================================================

void CHoe_NPC_ISlave::ArmBeam(int side)
{
	trace_t tr;
	float flDist = 1.0;

	if (m_iBeams >= ISLAVE_MAX_BEAMS)
		return;

	Vector forward, right, up;
	GetVectors(&forward, &right, &up);

	Vector vecSrc = GetAbsOrigin() + up * 36 + right * side * 16 + forward * 32;

	for (int i = 0; i < 3; i++)
	{
		Vector vecAim = right * side * random->RandomFloat(0, 1) + up * random->RandomFloat(-1, 1);
		trace_t tr1;
		UTIL_TraceLine(vecSrc, vecSrc + vecAim * 512, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr1);
		if (flDist > tr1.fraction)
		{
			tr = tr1;
			flDist = tr.fraction;
		}
	}

	// Couldn't find anything close enough
	if (flDist == 1.0)
		return;

	UTIL_ImpactTrace(&tr, DMG_SHOCK);

	// DecalGunshot(&tr, BULLET_PLAYER_CROWBAR);

	m_pBeam[m_iBeams] = CBeam::BeamCreate("sprites/lgtning.spr", 3); // 30
	if (!m_pBeam[m_iBeams])
		return;

	m_pBeam[m_iBeams]->PointEntInit(tr.endpos, this);
	m_pBeam[m_iBeams]->SetEndAttachment(side < 0 ? 2 : 1);
	// m_pBeam[m_iBeams]->SetColor( 180, 255, 96 );
	m_pBeam[m_iBeams]->SetColor(96, 128, 16);
	m_pBeam[m_iBeams]->SetBrightness(64);
	m_pBeam[m_iBeams]->SetNoise(8); // 80
	m_iBeams++;
}


//=========================================================
// BeamGlow - brighten all beams
//=========================================================
void CHoe_NPC_ISlave::BeamGlow()
{
	int b = m_iBeams * 32;
	if (b > 255)
		b = 255;

	for (int i = 0; i < m_iBeams; i++)
	{
		if (m_pBeam[i]->GetBrightness() != 255)
		{
			m_pBeam[i]->SetBrightness(b);
		}
	}
}


//=========================================================
// WackBeam - regenerate dead colleagues
//=========================================================
void CHoe_NPC_ISlave::WackBeam(int side, CBaseEntity *pEntity)
{
	Vector vecDest;
	//float flDist = 1.0;

	if (m_iBeams >= ISLAVE_MAX_BEAMS)
		return;

	if (pEntity == NULL)
		return;

	m_pBeam[m_iBeams] = CBeam::BeamCreate("sprites/lgtning.spr", 30);
	if (!m_pBeam[m_iBeams])
		return;

	m_pBeam[m_iBeams]->PointEntInit(pEntity->WorldSpaceCenter(), this);
	m_pBeam[m_iBeams]->SetEndAttachment(side < 0 ? 2 : 1);
	m_pBeam[m_iBeams]->SetColor(180, 255, 96);
	m_pBeam[m_iBeams]->SetBrightness(255);
	m_pBeam[m_iBeams]->SetNoise(80);
	m_iBeams++;
}

//=========================================================
// ZapBeam - heavy damage directly forward
//=========================================================
void CHoe_NPC_ISlave::ZapBeam(int side)
{
	Vector vecSrc, vecAim;
	trace_t tr;
	CBaseEntity *pEntity;

	if (m_iBeams >= ISLAVE_MAX_BEAMS)
		return;

	Vector right, up;
	GetVectors(NULL, &right, &up);

	vecSrc = GetAbsOrigin() + up * 36;
	vecAim = GetActualShootTrajectory(vecSrc); /*ShootAtEnemy(vecSrc);*/
	float deflection = 0.01;
	vecAim = vecAim + side * right * random->RandomFloat(0, deflection) + up * random->RandomFloat(-deflection, deflection);
	UTIL_TraceLine(vecSrc, vecSrc + vecAim * 1024, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr);

	m_pBeam[m_iBeams] = CBeam::BeamCreate("sprites/lgtning.spr", 50);
	if (!m_pBeam[m_iBeams])
		return;

	m_pBeam[m_iBeams]->PointEntInit(tr.endpos, this);
	m_pBeam[m_iBeams]->SetEndAttachment(side < 0 ? 2 : 1);
	m_pBeam[m_iBeams]->SetColor(180, 255, 96);
	m_pBeam[m_iBeams]->SetBrightness(255);
	m_pBeam[m_iBeams]->SetNoise(20);
	m_iBeams++;

	// pEntity = CBaseEntity::Instance(tr.pHit);
	pEntity = tr.m_pEnt;
	if (pEntity != NULL && pEntity->m_takedamage == DAMAGE_YES)
	{
		CTakeDamageInfo info(this, this, sk_vortigaunt_dmg_zap.GetFloat(), DMG_SHOCK);
		pEntity->TakeDamage(info);
		// pEntity->TraceAttack(pev, gSkillData.slaveDmgZap, vecAim, &tr, DMG_SHOCK);
	}

	UTIL_EmitAmbientSound(entindex(), tr.endpos, "weapons/electro4.wav", 0.5, SNDLVL_NORM, 0, random->RandomInt(140, 160));
}


//=========================================================
// ClearBeams - remove all beams
//=========================================================
void CHoe_NPC_ISlave::ClearBeams()
{
	for (int i = 0; i < ISLAVE_MAX_BEAMS; i++)
	{
		if (m_pBeam[i])
		{
			UTIL_Remove(m_pBeam[i]);
			m_pBeam[i] = NULL;
		}
	}
	m_iBeams = 0;
	m_nSkin = 0;

	StopSound(entindex(), CHAN_WEAPON, "NPC_Vortigaunt.ZapPowerup");
}

AI_BEGIN_CUSTOM_NPC(npc_vortigaunt, CHoe_NPC_ISlave)

//=========================================================
// > SCHED_SLAVE_RANGE_ATTACK1
//=========================================================
DEFINE_SCHEDULE
(
SCHED_SLAVE_RANGE_ATTACK1,

"	Tasks"
"		TASK_STOP_MOVING					0"
"		TASK_FACE_IDEAL						0"
"		TASK_RANGE_ATTACK1					0"
"		TASK_WAIT_FOR_MOVEMENT				0"
"	"
"	Interrupts"
"		COND_CAN_MELEE_ATTACK1"
//"		COND_HEAR_DANGER"
"		COND_HEAVY_DAMAGE"
)

AI_END_CUSTOM_NPC()