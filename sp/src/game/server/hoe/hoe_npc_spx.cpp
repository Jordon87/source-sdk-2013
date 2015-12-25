//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

// UNDONE: Don't flinch every time you get hit

#include "cbase.h"
#include "npcevent.h"
#include "ai_basenpc.h"
#include "ai_hull.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define NPC_SPX_CHARLIE_MODEL	"models/spx_crossbreed/charlie.mdl"
#define NPC_SPX_FEMALE1_MODEL	"models/spx_crossbreed/female1.mdl"
#define NPC_SPX_FEMALE2_MODEL	"models/spx_crossbreed/female2.mdl"
#define NPC_SPX_FEMALE3_MODEL	"models/spx_crossbreed/female3.mdl"
#define NPC_SPX_MALE1_MODEL		"models/spx_crossbreed/male1.mdl"
#define NPC_SPX_MALE2_MODEL		"models/spx_crossbreed/male2.mdl"
#define NPC_SPX_MALE3_MODEL		"models/spx_crossbreed/male3.mdl"
#define NPC_SPX_MIKE_MODEL		"models/spx_crossbreed/mike.mdl"
#define NPC_SPX_NAMGRUNT_MODEL	"models/spx_crossbreed/namgrunt.mdl"
#define NPC_SPX_DEFAULT_MODEL	NPC_SPX_NAMGRUNT_MODEL

enum
{
	SPX_BODY_RANDOM			= -1,
	SPX_BODY_GRUNT			= 0,
	SPX_BODY_MALE_PEASANT1,
	SPX_BODY_MALE_PEASANT2,
	SPX_BODY_MALE_PEASANT3,
	SPX_BODY_FEMALE_PEASANT1,
	SPX_BODY_FEMALE_PEASANT2,
	SPX_BODY_FEMALE_PEASANT3,
	SPX_BODY_MIKE,
	SPX_BODY_CHARLIE,

	//
	// Add new body IDs here...
	//

	SPX_BODY_COUNT, // <-- Last body count.
};

extern ConVar	sk_zombie_dmg_one_slash;
extern ConVar	sk_zombie_dmg_both_slash;

ConVar sk_spx_health("sk_spx_health", "0");

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
int AE_SPX_ATTACK_RIGHT;
int AE_SPX_ATTACK_LEFT;
int AE_SPX_ATTACK_BOTH;


#define ZOMBIE_FLINCH_DELAY			2		// at most one flinch every n secs

class CHoe_NPC_SPX : public CAI_BaseNPC
{
	DECLARE_CLASS(CHoe_NPC_SPX, CAI_BaseNPC);
public:
	DECLARE_DATADESC();
	DEFINE_CUSTOM_AI;

	void Spawn(void);
	void Precache(void);
	float MaxYawSpeed(void);
	Class_T  Classify(void);
	void HandleAnimEvent(animevent_t *pEvent);
#if 0
	int IgnoreConditions(void);
#endif

	float m_flNextFlinch;

	void PainSound(const CTakeDamageInfo &info);
	void AlertSound(void);
	void IdleSound(void);
	void AttackSound(void);

	// No range attacks
	int		RangeAttack1Conditions(float flDot, float flDist) { return COND_NONE; }
	int		RangeAttack2Conditions(float flDot, float flDist) { return COND_NONE; }
	int		OnTakeDamage_Alive(const CTakeDamageInfo &info);
};

LINK_ENTITY_TO_CLASS(npc_spx, CHoe_NPC_SPX);

BEGIN_DATADESC(CHoe_NPC_SPX)
	DEFINE_FIELD(m_flNextFlinch, FIELD_TIME),
END_DATADESC()

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
Class_T	CHoe_NPC_SPX::Classify(void)
{
	return CLASS_ZOMBIE;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : float
//-----------------------------------------------------------------------------
float CHoe_NPC_SPX::MaxYawSpeed(void)
{
	return 120.0f;
}


int	CHoe_NPC_SPX::OnTakeDamage_Alive(const CTakeDamageInfo &info)
{
#if 0
	// Take 30% damage from bullets
	if (bitsDamageType == DMG_BULLET)
	{
		Vector vecDir = pev->origin - (pevInflictor->absmin + pevInflictor->absmax) * 0.5;
		vecDir = vecDir.Normalize();
		float flForce = DamageForce(flDamage);
		pev->velocity = pev->velocity + vecDir * flForce;
		flDamage *= 0.3;
	}
#endif

	// HACK HACK -- until we fix this.
	if (IsAlive())
		PainSound(info);

	return BaseClass::OnTakeDamage_Alive(info);
}

void CHoe_NPC_SPX::PainSound(const CTakeDamageInfo &info)
{
	EmitSound("NPC_HeadCrab.Pain");
}

void CHoe_NPC_SPX::AlertSound(void)
{
	EmitSound("NPC_Spx.Idle");
}

void CHoe_NPC_SPX::IdleSound(void)
{
	// Play a random idle sound
	EmitSound("NPC_Spx.Idle");
}

void CHoe_NPC_SPX::AttackSound(void)
{
	// Play a random attack sound
	EmitSound("NPC_Spx.AnnounceAttack");
}


//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CHoe_NPC_SPX::HandleAnimEvent(animevent_t *pEvent)
{
	Vector vecMins = GetHullMins();
	Vector vecMaxs = GetHullMaxs();
	vecMins.z = vecMins.x;
	vecMaxs.z = vecMaxs.x;

	if (pEvent->event == AE_SPX_ATTACK_RIGHT)
	{
		// do stuff for this event.
		//		ALERT( at_console, "Slash right!\n" );
		CBaseEntity *pHurt = CheckTraceHullAttack(70, vecMins, vecMaxs, sk_zombie_dmg_one_slash.GetInt(), DMG_SLASH);
		if (pHurt)
		{
			if (pHurt->IsPlayer())
			{
				QAngle angles;
				Vector right, velocity;

				angles = GetLocalAngles();
				angles.x = 0;

				AngleVectors(angles, NULL, &right, NULL);

				velocity = pHurt->GetAbsVelocity() - right * 100;

				pHurt->VelocityPunch(velocity);
				pHurt->ViewPunch(QAngle(5, 0, -18));
			}

			// Play a random attack hit sound
			EmitSound("NPC_Spx.AttackHit");
		}
		else // Play a random attack miss sound
			EmitSound("NPC_Spx.AttackMiss");

		if (random->RandomInt(0, 1))
			AttackSound();
	}
	else if (pEvent->event == AE_SPX_ATTACK_LEFT)
	{
		// do stuff for this event.
		//		ALERT( at_console, "Slash right!\n" );
		CBaseEntity *pHurt = CheckTraceHullAttack(70, vecMins, vecMaxs, sk_zombie_dmg_one_slash.GetInt(), DMG_SLASH);
		if (pHurt)
		{
			if (pHurt->IsPlayer())
			{
				QAngle angles;
				Vector right, velocity;

				angles = GetLocalAngles();
				angles.x = 0;

				AngleVectors(angles, NULL, &right, NULL);

				velocity = pHurt->GetAbsVelocity() + right * 100;

				pHurt->VelocityPunch(velocity);
				pHurt->ViewPunch(QAngle(5, 0, 18));
			}

			// Play a random attack hit sound
			EmitSound("NPC_Spx.AttackHit");
		}
		else // Play a random attack miss sound
			EmitSound("NPC_Spx.AttackMiss");

		if (random->RandomInt(0, 1))
			AttackSound();
	}
	else if (pEvent->event == AE_SPX_ATTACK_BOTH)
	{
		// do stuff for this event.
		CBaseEntity *pHurt = CheckTraceHullAttack(70, vecMins, vecMaxs, sk_zombie_dmg_both_slash.GetInt(), DMG_SLASH);
		if (pHurt)
		{
			if (pHurt->IsPlayer())
			{
				QAngle angles;
				Vector forward, velocity;

				angles = GetLocalAngles();
				angles.x = 0;

				AngleVectors(angles, &forward, NULL, NULL);

				velocity = pHurt->GetAbsVelocity() + forward * -100;

				pHurt->VelocityPunch(velocity);
				pHurt->ViewPunch(QAngle(5, 0, 0));
			}

			EmitSound("NPC_Spx.AttackHit");
		}
		else
			EmitSound("NPC_Spx.AttackMiss");

		if (random->RandomInt(0, 1))
			AttackSound();
	}
	else
	{
		BaseClass::HandleAnimEvent(pEvent);
	}
}

//=========================================================
// Spawn
//=========================================================
void CHoe_NPC_SPX::Spawn()
{
	char* szModel = (char*)STRING(GetModelName());

	if (!szModel || !*szModel)
	{
		switch (m_nBody)
		{
		default:
		case SPX_BODY_RANDOM:
		case SPX_BODY_GRUNT:
			szModel = NPC_SPX_DEFAULT_MODEL;
			break;

		case SPX_BODY_MALE_PEASANT1:
			szModel = NPC_SPX_MALE1_MODEL;
			break;

		case SPX_BODY_MALE_PEASANT2:
			szModel = NPC_SPX_MALE2_MODEL;
			break;

		case SPX_BODY_MALE_PEASANT3:
			szModel = NPC_SPX_MALE3_MODEL;
			break;

		case SPX_BODY_FEMALE_PEASANT1:
			szModel = NPC_SPX_FEMALE1_MODEL;
			break;

		case SPX_BODY_FEMALE_PEASANT2:
			szModel = NPC_SPX_FEMALE2_MODEL;
			break;

		case SPX_BODY_FEMALE_PEASANT3:
			szModel = NPC_SPX_FEMALE3_MODEL;
			break;

		case SPX_BODY_MIKE:
			szModel = NPC_SPX_MIKE_MODEL;
			break;

		case SPX_BODY_CHARLIE:
			szModel = NPC_SPX_CHARLIE_MODEL;
			break;
		}

		Assert(szModel);

		SetModelName(AllocPooledString(szModel));
	}

	Precache();

	SetModel(szModel);
	SetHullType(HULL_HUMAN);
	SetHullSizeNormal();

	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_STANDABLE );
	SetMoveType( MOVETYPE_STEP );

	SetBloodColor(BLOOD_COLOR_GREEN);

	m_iHealth = m_iMaxHealth = sk_spx_health.GetInt();
	m_flFieldOfView = 0.5f;
	SetViewOffset(VEC_VIEW);
	m_NPCState = NPC_STATE_NONE;

	CapabilitiesClear();
	CapabilitiesAdd(bits_CAP_MOVE_GROUND | bits_CAP_INNATE_MELEE_ATTACK1 | bits_CAP_INNATE_MELEE_ATTACK2);
	CapabilitiesAdd(bits_CAP_DOORS_GROUP);

	NPCInit();
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CHoe_NPC_SPX::Precache()
{
	BaseClass::Precache();

	PrecacheModel( STRING( GetModelName() ) );

	PrecacheScriptSound("NPC_Spx.FootstepRight");
	PrecacheScriptSound("NPC_Spx.FootstepLeft");
	PrecacheScriptSound("NPC_Spx.AnnounceAttack");
	PrecacheScriptSound("NPC_Spx.AttackHit");
	PrecacheScriptSound("NPC_Spx.AttackMiss");
	PrecacheScriptSound("NPC_Spx.Eat");
	PrecacheScriptSound("NPC_Spx.Pain");
	PrecacheScriptSound("NPC_Spx.Death");
	PrecacheScriptSound("NPC_Spx.Idle");
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

#if 0
int CZombie::IgnoreConditions(void)
{
	int iIgnore = CBaseMonster::IgnoreConditions();

	if ((m_Activity == ACT_MELEE_ATTACK1) || (m_Activity == ACT_MELEE_ATTACK1))
	{
#if 0
		if (pev->health < 20)
			iIgnore |= (bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE);
		else
#endif			
			if (m_flNextFlinch >= gpGlobals->time)
				iIgnore |= (bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE);
	}

	if ((m_Activity == ACT_SMALL_FLINCH) || (m_Activity == ACT_BIG_FLINCH))
	{
		if (m_flNextFlinch < gpGlobals->time)
			m_flNextFlinch = gpGlobals->time + ZOMBIE_FLINCH_DELAY;
	}

	return iIgnore;

}
#endif

AI_BEGIN_CUSTOM_NPC(npc_spx, CHoe_NPC_SPX)

	DECLARE_ANIMEVENT(AE_SPX_ATTACK_RIGHT);
	DECLARE_ANIMEVENT(AE_SPX_ATTACK_LEFT);
	DECLARE_ANIMEVENT(AE_SPX_ATTACK_BOTH);

AI_END_CUSTOM_NPC()