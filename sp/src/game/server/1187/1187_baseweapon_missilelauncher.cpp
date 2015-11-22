
//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_basecombatweapon.h"
#include "basecombatcharacter.h"
#include "movie_explosion.h"
#include "soundent.h"
#include "player.h"
#include "rope.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "explode.h"
#include "util.h"
#include "in_buttons.h"
#include "shake.h"
#include "ai_basenpc.h"
#include "ai_squad.h"
#include "te_effect_dispatch.h"
#include "triggers.h"
#include "smoke_trail.h"
#include "collisionutils.h"
#include "hl2_shareddefs.h"
#include "rumble_shared.h"
#include "gamestats.h"

#include "1187_baseweapon_missilelauncher.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	RPG_SPEED	1500

extern ConVar law_missle_use_custom_detonators("law_missle_use_custom_detonators", "1");

extern int g_interactionPlayerLaunchedRPG;

CLaserDot *GetLaserDotList();

BEGIN_DATADESC(C1187Missile)

DEFINE_FIELD(m_hOwner, FIELD_EHANDLE),
DEFINE_FIELD(m_hRocketTrail, FIELD_EHANDLE),
DEFINE_FIELD(m_flAugerTime, FIELD_TIME),
DEFINE_FIELD(m_flMarkDeadTime, FIELD_TIME),
DEFINE_FIELD(m_flGracePeriodEndsAt, FIELD_TIME),
DEFINE_FIELD(m_flDamage, FIELD_FLOAT),
DEFINE_FIELD(m_bCreateDangerSounds, FIELD_BOOLEAN),

// Function Pointers
DEFINE_FUNCTION(MissileTouch),
DEFINE_FUNCTION(AccelerateThink),
DEFINE_FUNCTION(AugerThink),
DEFINE_FUNCTION(IgniteThink),
DEFINE_FUNCTION(SeekThink),

END_DATADESC()
LINK_ENTITY_TO_CLASS(law_missile, C1187Missile);

class C1187_BaseWeapon_MissileLauncher;


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
C1187Missile::C1187Missile()
{
	m_hRocketTrail = NULL;
	m_bCreateDangerSounds = false;
}

C1187Missile::~C1187Missile()
{
}


//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void C1187Missile::Precache(void)
{
	PrecacheModel("models/weapons/w_missile.mdl");
	PrecacheModel("models/weapons/w_missile_launch.mdl");
	PrecacheModel("models/weapons/w_missile_closed.mdl");
}


//-----------------------------------------------------------------------------
// Purpose: 
//
//
//-----------------------------------------------------------------------------
void C1187Missile::Spawn(void)
{
	Precache();

	SetSolid(SOLID_BBOX);
	SetModel("models/weapons/w_missile_launch.mdl");
	UTIL_SetSize(this, -Vector(4, 4, 4), Vector(4, 4, 4));

	SetTouch(&C1187Missile::MissileTouch);

	SetMoveType(MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE);
	SetThink(&C1187Missile::IgniteThink);

	SetNextThink(gpGlobals->curtime + 0.3f);
	SetDamage(200.0f);

	m_takedamage = DAMAGE_YES;
	m_iHealth = m_iMaxHealth = 100;
	m_bloodColor = DONT_BLEED;
	m_flGracePeriodEndsAt = 0;

	AddFlag(FL_OBJECT);
}


//---------------------------------------------------------
//---------------------------------------------------------
void C1187Missile::Event_Killed(const CTakeDamageInfo &info)
{
	m_takedamage = DAMAGE_NO;

	ShotDown();
}

unsigned int C1187Missile::PhysicsSolidMaskForEntity(void) const
{
	return BaseClass::PhysicsSolidMaskForEntity() | CONTENTS_HITBOX;
}

//---------------------------------------------------------
//---------------------------------------------------------
int C1187Missile::OnTakeDamage_Alive(const CTakeDamageInfo &info)
{
	if ((info.GetDamageType() & (DMG_MISSILEDEFENSE | DMG_AIRBOAT)) == false)
		return 0;

	bool bIsDamaged;
	if (m_iHealth <= AugerHealth())
	{
		// This missile is already damaged (i.e., already running AugerThink)
		bIsDamaged = true;
	}
	else
	{
		// This missile isn't damaged enough to wobble in flight yet
		bIsDamaged = false;
	}

	int nRetVal = BaseClass::OnTakeDamage_Alive(info);

	if (!bIsDamaged)
	{
		if (m_iHealth <= AugerHealth())
		{
			ShotDown();
		}
	}

	return nRetVal;
}


//-----------------------------------------------------------------------------
// Purpose: Stops any kind of tracking and shoots dumb
//-----------------------------------------------------------------------------
void C1187Missile::DumbFire(void)
{
	SetThink(NULL);
	SetMoveType(MOVETYPE_FLY);

	SetModel("models/weapons/w_missile.mdl");
	UTIL_SetSize(this, vec3_origin, vec3_origin);

	EmitSound("Missile.Ignite");

	// Smoke trail.
	CreateSmokeTrail();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187Missile::SetGracePeriod(float flGracePeriod)
{
	m_flGracePeriodEndsAt = gpGlobals->curtime + flGracePeriod;

	// Go non-solid until the grace period ends
	AddSolidFlags(FSOLID_NOT_SOLID);
}

//---------------------------------------------------------
//---------------------------------------------------------
void C1187Missile::AccelerateThink(void)
{
	Vector vecForward;

	// !!!UNDONE - make this work exactly the same as HL1 RPG, lest we have looping sound bugs again!
	EmitSound("Missile.Accelerate");

	// SetEffects( EF_LIGHT );

	AngleVectors(GetLocalAngles(), &vecForward);
	SetAbsVelocity(vecForward * RPG_SPEED);

	SetThink(&C1187Missile::SeekThink);
	SetNextThink(gpGlobals->curtime + 0.1f);
}

#define AUGER_YDEVIANCE 20.0f
#define AUGER_XDEVIANCEUP 8.0f
#define AUGER_XDEVIANCEDOWN 1.0f

//---------------------------------------------------------
//---------------------------------------------------------
void C1187Missile::AugerThink(void)
{
	// If we've augered long enough, then just explode
	if (m_flAugerTime < gpGlobals->curtime)
	{
		Explode();
		return;
	}

	if (m_flMarkDeadTime < gpGlobals->curtime)
	{
		m_lifeState = LIFE_DYING;
	}

	QAngle angles = GetLocalAngles();

	angles.y += random->RandomFloat(-AUGER_YDEVIANCE, AUGER_YDEVIANCE);
	angles.x += random->RandomFloat(-AUGER_XDEVIANCEDOWN, AUGER_XDEVIANCEUP);

	SetLocalAngles(angles);

	Vector vecForward;

	AngleVectors(GetLocalAngles(), &vecForward);

	SetAbsVelocity(vecForward * 1000.0f);

	SetNextThink(gpGlobals->curtime + 0.05f);
}

//-----------------------------------------------------------------------------
// Purpose: Causes the missile to spiral to the ground and explode, due to damage
//-----------------------------------------------------------------------------
void C1187Missile::ShotDown(void)
{
	CEffectData	data;
	data.m_vOrigin = GetAbsOrigin();

	DispatchEffect("RPGShotDown", data);

	if (m_hRocketTrail != NULL)
	{
		m_hRocketTrail->m_bDamaged = true;
	}

	SetThink(&C1187Missile::AugerThink);
	SetNextThink(gpGlobals->curtime);
	m_flAugerTime = gpGlobals->curtime + 1.5f;
	m_flMarkDeadTime = gpGlobals->curtime + 0.75;

	// Let the RPG start reloading immediately
	if (m_hOwner != NULL)
	{
		m_hOwner->NotifyRocketDied();
		m_hOwner = NULL;
	}
}


//-----------------------------------------------------------------------------
// The actual explosion 
//-----------------------------------------------------------------------------
void C1187Missile::DoExplosion(void)
{
	// Explode
	ExplosionCreate(GetAbsOrigin(), GetAbsAngles(), GetOwnerEntity(), GetDamage(), CMissile::EXPLOSION_RADIUS,
		SF_ENVEXPLOSION_NOSPARKS | SF_ENVEXPLOSION_NODLIGHTS | SF_ENVEXPLOSION_NOSMOKE, 0.0f, this);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187Missile::Explode(void)
{
	// Don't explode against the skybox. Just pretend that 
	// the missile flies off into the distance.
	Vector forward;

	GetVectors(&forward, NULL, NULL);

	trace_t tr;
	UTIL_TraceLine(GetAbsOrigin(), GetAbsOrigin() + forward * 16, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);

	m_takedamage = DAMAGE_NO;
	SetSolid(SOLID_NONE);
	if (tr.fraction == 1.0 || !(tr.surface.flags & SURF_SKY))
	{
		DoExplosion();
	}

	if (m_hRocketTrail)
	{
		m_hRocketTrail->SetLifetime(0.1f);
		m_hRocketTrail = NULL;
	}

	if (m_hOwner != NULL)
	{
		m_hOwner->NotifyRocketDied();
		m_hOwner = NULL;
	}

	StopSound("Missile.Ignite");
	UTIL_Remove(this);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void C1187Missile::MissileTouch(CBaseEntity *pOther)
{
	Assert(pOther);

	// Don't touch triggers (but DO hit weapons)
	if (pOther->IsSolidFlagSet(FSOLID_TRIGGER | FSOLID_VOLUME_CONTENTS) && pOther->GetCollisionGroup() != COLLISION_GROUP_WEAPON)
	{
		// Some NPCs are triggers that can take damage (like antlion grubs). We should hit them.
		if ((pOther->m_takedamage == DAMAGE_NO) || (pOther->m_takedamage == DAMAGE_EVENTS_ONLY))
			return;
	}

	Explode();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187Missile::CreateSmokeTrail(void)
{
	if (m_hRocketTrail)
		return;

	// Smoke trail.
	if ((m_hRocketTrail = RocketTrail::CreateRocketTrail()) != NULL)
	{
		m_hRocketTrail->m_Opacity = 0.2f;
		m_hRocketTrail->m_SpawnRate = 100;
		m_hRocketTrail->m_ParticleLifetime = 0.5f;
		m_hRocketTrail->m_StartColor.Init(0.65f, 0.65f, 0.65f);
		m_hRocketTrail->m_EndColor.Init(0.0, 0.0, 0.0);
		m_hRocketTrail->m_StartSize = 8;
		m_hRocketTrail->m_EndSize = 32;
		m_hRocketTrail->m_SpawnRadius = 4;
		m_hRocketTrail->m_MinSpeed = 2;
		m_hRocketTrail->m_MaxSpeed = 16;

		m_hRocketTrail->SetLifetime(999);
		m_hRocketTrail->FollowEntity(this, "0");
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187Missile::IgniteThink(void)
{
	SetMoveType(MOVETYPE_FLY);
	SetModel("models/weapons/w_missile.mdl");
	UTIL_SetSize(this, vec3_origin, vec3_origin);
	RemoveSolidFlags(FSOLID_NOT_SOLID);

	//TODO: Play opening sound

	Vector vecForward;

	EmitSound("Missile.Ignite");

	AngleVectors(GetLocalAngles(), &vecForward);
	SetAbsVelocity(vecForward * RPG_SPEED);

	SetThink(&C1187Missile::SeekThink);
	SetNextThink(gpGlobals->curtime);

	if (m_hOwner && m_hOwner->GetOwner())
	{
		CBasePlayer *pPlayer = ToBasePlayer(m_hOwner->GetOwner());

		if (pPlayer)
		{
			color32 white = { 255, 225, 205, 64 };
			UTIL_ScreenFade(pPlayer, white, 0.1f, 0.0f, FFADE_IN);

			pPlayer->RumbleEffect(RUMBLE_RPG_MISSILE, 0, RUMBLE_FLAG_RESTART);
		}
	}

	CreateSmokeTrail();
}


//-----------------------------------------------------------------------------
// Gets the shooting position 
//-----------------------------------------------------------------------------
void C1187Missile::GetShootPosition(CLaserDot *pLaserDot, Vector *pShootPosition)
{
	if (pLaserDot->GetOwnerEntity() != NULL)
	{
		//FIXME: Do we care this isn't exactly the muzzle position?
		*pShootPosition = pLaserDot->GetOwnerEntity()->WorldSpaceCenter();
	}
	else
	{
		*pShootPosition = pLaserDot->GetChasePosition();
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
#define	RPG_HOMING_SPEED	0.125f

void C1187Missile::ComputeActualDotPosition(CLaserDot *pLaserDot, Vector *pActualDotPosition, float *pHomingSpeed)
{
	*pHomingSpeed = RPG_HOMING_SPEED;
	if (pLaserDot->GetTargetEntity())
	{
		*pActualDotPosition = pLaserDot->GetChasePosition();
		return;
	}

	Vector vLaserStart;
	GetShootPosition(pLaserDot, &vLaserStart);

	//Get the laser's vector
	Vector vLaserDir;
	VectorSubtract(pLaserDot->GetChasePosition(), vLaserStart, vLaserDir);

	//Find the length of the current laser
	float flLaserLength = VectorNormalize(vLaserDir);

	//Find the length from the missile to the laser's owner
	float flMissileLength = GetAbsOrigin().DistTo(vLaserStart);

	//Find the length from the missile to the laser's position
	Vector vecTargetToMissile;
	VectorSubtract(GetAbsOrigin(), pLaserDot->GetChasePosition(), vecTargetToMissile);
	float flTargetLength = VectorNormalize(vecTargetToMissile);

	// See if we should chase the line segment nearest us
	if ((flMissileLength < flLaserLength) || (flTargetLength <= 512.0f))
	{
		*pActualDotPosition = UTIL_PointOnLineNearestPoint(vLaserStart, pLaserDot->GetChasePosition(), GetAbsOrigin());
		*pActualDotPosition += (vLaserDir * 256.0f);
	}
	else
	{
		// Otherwise chase the dot
		*pActualDotPosition = pLaserDot->GetChasePosition();
	}

	//	NDebugOverlay::Line( pLaserDot->GetChasePosition(), vLaserStart, 0, 255, 0, true, 0.05f );
	//	NDebugOverlay::Line( GetAbsOrigin(), *pActualDotPosition, 255, 0, 0, true, 0.05f );
	//	NDebugOverlay::Cross3D( *pActualDotPosition, -Vector(4,4,4), Vector(4,4,4), 255, 0, 0, true, 0.05f );
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187Missile::SeekThink(void)
{
	CBaseEntity	*pBestDot = NULL;
	float		flBestDist = MAX_TRACE_LENGTH;
	float		dotDist;

	// If we have a grace period, go solid when it ends
	if (m_flGracePeriodEndsAt)
	{
		if (m_flGracePeriodEndsAt < gpGlobals->curtime)
		{
			RemoveSolidFlags(FSOLID_NOT_SOLID);
			m_flGracePeriodEndsAt = 0;
		}
	}

	//Search for all dots relevant to us
	for (CLaserDot *pEnt = GetLaserDotList(); pEnt != NULL; pEnt = pEnt->m_pNext)
	{
		if (!pEnt->IsOn())
			continue;

		if (pEnt->GetOwnerEntity() != GetOwnerEntity())
			continue;

		dotDist = (GetAbsOrigin() - pEnt->GetAbsOrigin()).Length();

		//Find closest
		if (dotDist < flBestDist)
		{
			pBestDot = pEnt;
			flBestDist = dotDist;
		}
	}

	if (hl2_episodic.GetBool())
	{
		if (flBestDist <= (GetAbsVelocity().Length() * 2.5f) && FVisible(pBestDot->GetAbsOrigin()))
		{
			// Scare targets
			CSoundEnt::InsertSound(SOUND_DANGER, pBestDot->GetAbsOrigin(), CMissile::EXPLOSION_RADIUS, 0.2f, pBestDot, SOUNDENT_CHANNEL_REPEATED_DANGER, NULL);
		}
	}

	if (law_missle_use_custom_detonators.GetBool())
	{
		for (int i = gm_CustomDetonators.Count() - 1; i >= 0; --i)
		{
			CustomDetonator_t &detonator = gm_CustomDetonators[i];
			if (!detonator.hEntity)
			{
				gm_CustomDetonators.FastRemove(i);
			}
			else
			{
				const Vector &vPos = detonator.hEntity->CollisionProp()->WorldSpaceCenter();
				if (detonator.halfHeight > 0)
				{
					if (fabsf(vPos.z - GetAbsOrigin().z) < detonator.halfHeight)
					{
						if ((GetAbsOrigin().AsVector2D() - vPos.AsVector2D()).LengthSqr() < detonator.radiusSq)
						{
							Explode();
							return;
						}
					}
				}
				else
				{
					if ((GetAbsOrigin() - vPos).LengthSqr() < detonator.radiusSq)
					{
						Explode();
						return;
					}
				}
			}
		}
	}

	//If we have a dot target
	if (pBestDot == NULL)
	{
		//Think as soon as possible
		SetNextThink(gpGlobals->curtime);
		return;
	}

	CLaserDot *pLaserDot = (CLaserDot *)pBestDot;
	Vector	targetPos;

	float flHomingSpeed;
	Vector vecLaserDotPosition;
	ComputeActualDotPosition(pLaserDot, &targetPos, &flHomingSpeed);

	if (IsSimulatingOnAlternateTicks())
		flHomingSpeed *= 2;

	Vector	vTargetDir;
	VectorSubtract(targetPos, GetAbsOrigin(), vTargetDir);
	float flDist = VectorNormalize(vTargetDir);

	if (pLaserDot->GetTargetEntity() != NULL && flDist <= 240.0f && hl2_episodic.GetBool())
	{
		// Prevent the missile circling the Strider like a Halo in ep1_c17_06. If the missile gets within 20
		// feet of a Strider, tighten up the turn speed of the missile so it can break the halo and strike. (sjb 4/27/2006)
		if (pLaserDot->GetTargetEntity()->ClassMatches("npc_strider"))
		{
			flHomingSpeed *= 1.75f;
		}
	}

	Vector	vDir = GetAbsVelocity();
	float	flSpeed = VectorNormalize(vDir);
	Vector	vNewVelocity = vDir;
	if (gpGlobals->frametime > 0.0f)
	{
		if (flSpeed != 0)
		{
			vNewVelocity = (flHomingSpeed * vTargetDir) + ((1 - flHomingSpeed) * vDir);

			// This computation may happen to cancel itself out exactly. If so, slam to targetdir.
			if (VectorNormalize(vNewVelocity) < 1e-3)
			{
				vNewVelocity = (flDist != 0) ? vTargetDir : vDir;
			}
		}
		else
		{
			vNewVelocity = vTargetDir;
		}
	}

	QAngle	finalAngles;
	VectorAngles(vNewVelocity, finalAngles);
	SetAbsAngles(finalAngles);

	vNewVelocity *= flSpeed;
	SetAbsVelocity(vNewVelocity);

	if (GetAbsVelocity() == vec3_origin)
	{
		// Strange circumstances have brought this missile to halt. Just blow it up.
		Explode();
		return;
	}

	// Think as soon as possible
	SetNextThink(gpGlobals->curtime);

#ifdef HL2_EPISODIC

	if (m_bCreateDangerSounds == true)
	{
		trace_t tr;
		UTIL_TraceLine(GetAbsOrigin(), GetAbsOrigin() + GetAbsVelocity() * 0.5, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr);

		CSoundEnt::InsertSound(SOUND_DANGER, tr.endpos, 100, 0.2, this, SOUNDENT_CHANNEL_REPEATED_DANGER);
	}
#endif
}


//-----------------------------------------------------------------------------
// Purpose: 
//
// Input  : &vecOrigin - 
//			&vecAngles - 
//			NULL - 
//
// Output : CMissile
//-----------------------------------------------------------------------------
C1187Missile *C1187Missile::Create(const Vector &vecOrigin, const QAngle &vecAngles, edict_t *pentOwner = NULL)
{
	//CMissile *pMissile = (CMissile *)CreateEntityByName("rpg_missile" );
	C1187Missile *pMissile = (C1187Missile *)CBaseEntity::Create("law_missile", vecOrigin, vecAngles, CBaseEntity::Instance(pentOwner));
	pMissile->SetOwnerEntity(Instance(pentOwner));
	pMissile->Spawn();
	pMissile->AddEffects(EF_NOSHADOW);

	Vector vecForward;
	AngleVectors(vecAngles, &vecForward);

	pMissile->SetAbsVelocity(vecForward * 300 + Vector(0, 0, 128));

	return pMissile;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
CUtlVector<C1187Missile::CustomDetonator_t> C1187Missile::gm_CustomDetonators;

void C1187Missile::AddCustomDetonator(CBaseEntity *pEntity, float radius, float height)
{
	int i = gm_CustomDetonators.AddToTail();
	gm_CustomDetonators[i].hEntity = pEntity;
	gm_CustomDetonators[i].radiusSq = Square(radius);
	gm_CustomDetonators[i].halfHeight = height * 0.5f;
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187Missile::RemoveCustomDetonator(CBaseEntity *pEntity)
{
	for (int i = 0; i < gm_CustomDetonators.Count(); i++)
	{
		if (gm_CustomDetonators[i].hEntity == pEntity)
		{
			gm_CustomDetonators.FastRemove(i);
			break;
		}
	}
}

//=============================================================================
// LAW Missile launcher
//=============================================================================

#define	RPG_BEAM_SPRITE		"effects/laser1_noz.vmt"
#define	RPG_LASER_SPRITE	"sprites/redglow1.vmt"

BEGIN_DATADESC(C1187_BaseWeapon_MissileLauncher)

DEFINE_FIELD(m_bInitialStateUpdate, FIELD_BOOLEAN),
DEFINE_FIELD(m_bGuiding, FIELD_BOOLEAN),
DEFINE_FIELD(m_vecNPCLaserDot, FIELD_POSITION_VECTOR),
DEFINE_FIELD(m_hLaserDot, FIELD_EHANDLE),
DEFINE_FIELD(m_hMissile, FIELD_EHANDLE),
DEFINE_FIELD(m_hLaserMuzzleSprite, FIELD_EHANDLE),
// DEFINE_FIELD(m_hLaserBeam, FIELD_EHANDLE),
DEFINE_FIELD(m_bHideGuiding, FIELD_BOOLEAN),

END_DATADESC()

acttable_t	C1187_BaseWeapon_MissileLauncher::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_RPG, true },

	{ ACT_IDLE_RELAXED, ACT_IDLE_RPG_RELAXED, true },
	{ ACT_IDLE_STIMULATED, ACT_IDLE_ANGRY_RPG, true },
	{ ACT_IDLE_AGITATED, ACT_IDLE_ANGRY_RPG, true },

	{ ACT_IDLE, ACT_IDLE_RPG, true },
	{ ACT_IDLE_ANGRY, ACT_IDLE_ANGRY_RPG, true },
	{ ACT_WALK, ACT_WALK_RPG, true },
	{ ACT_WALK_CROUCH, ACT_WALK_CROUCH_RPG, true },
	{ ACT_RUN, ACT_RUN_RPG, true },
	{ ACT_RUN_CROUCH, ACT_RUN_CROUCH_RPG, true },
	{ ACT_COVER_LOW, ACT_COVER_LOW_RPG, true },
};

IMPLEMENT_ACTTABLE(C1187_BaseWeapon_MissileLauncher);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C1187_BaseWeapon_MissileLauncher::C1187_BaseWeapon_MissileLauncher()
{
	m_bReloadsSingly = true;
	m_bInitialStateUpdate = false;
	m_bHideGuiding = false;
	m_bGuiding = false;

	m_fMinRange1 = m_fMinRange2 = 40 * 12;
	m_fMaxRange1 = m_fMaxRange2 = 500 * 12;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C1187_BaseWeapon_MissileLauncher::~C1187_BaseWeapon_MissileLauncher()
{
	if (m_hLaserDot != NULL)
	{
		UTIL_Remove(m_hLaserDot);
		m_hLaserDot = NULL;
	}

	if (m_hLaserMuzzleSprite)
	{
		UTIL_Remove(m_hLaserMuzzleSprite);
	}

#if 0
	if (m_hLaserBeam)
	{
		UTIL_Remove(m_hLaserBeam);
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::Precache(void)
{
	BaseClass::Precache();

	PrecacheScriptSound("Missile.Ignite");
	PrecacheScriptSound("Missile.Accelerate");

	// Laser dot...
	PrecacheModel("sprites/redglow1.vmt");
	PrecacheModel(RPG_LASER_SPRITE);
	PrecacheModel(RPG_BEAM_SPRITE);

	UTIL_PrecacheOther("law_missile");
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::Activate(void)
{
	BaseClass::Activate();

	// Restore the laser pointer after transition
	if (m_bGuiding)
	{
		CBasePlayer *pOwner = ToBasePlayer(GetOwner());

		if (pOwner == NULL)
			return;

		if (pOwner->GetActiveWeapon() == this)
		{
			StartGuiding();
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pEvent - 
//			*pOperator - 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator)
{
	switch (pEvent->event)
	{
	case EVENT_WEAPON_SMG1:
	{
		if (m_hMissile != NULL)
			return;

		Vector	muzzlePoint;
		QAngle	vecAngles;

		muzzlePoint = GetOwner()->Weapon_ShootPosition();

		CAI_BaseNPC *npc = pOperator->MyNPCPointer();
		ASSERT(npc != NULL);

		Vector vecShootDir = npc->GetActualShootTrajectory(muzzlePoint);

		// look for a better launch location
		Vector altLaunchPoint;
		if (GetAttachment("missile", altLaunchPoint))
		{
			// check to see if it's relativly free
			trace_t tr;
			AI_TraceHull(altLaunchPoint, altLaunchPoint + vecShootDir * (10.0f*12.0f), Vector(-24, -24, -24), Vector(24, 24, 24), MASK_NPCSOLID, NULL, &tr);

			if (tr.fraction == 1.0)
			{
				muzzlePoint = altLaunchPoint;
			}
		}

		VectorAngles(vecShootDir, vecAngles);

		m_hMissile = C1187Missile::Create(muzzlePoint, vecAngles, GetOwner()->edict());
		m_hMissile->m_hOwner = this;

		// NPCs always get a grace period
		m_hMissile->SetGracePeriod(0.5);

		pOperator->DoMuzzleFlash();

		WeaponSound(SINGLE_NPC);

		// Make sure our laserdot is off
		m_bGuiding = false;

		if (m_hLaserDot)
		{
			m_hLaserDot->TurnOff();
		}
	}
	break;

	default:
		BaseClass::Operator_HandleAnimEvent(pEvent, pOperator);
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_BaseWeapon_MissileLauncher::HasAnyAmmo(void)
{
	if (m_hMissile != NULL)
		return true;

	return BaseClass::HasAnyAmmo();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C1187_BaseWeapon_MissileLauncher::WeaponShouldBeLowered(void)
{
	// Lower us if we're out of ammo
	if (!HasAnyAmmo())
		return true;

	return BaseClass::WeaponShouldBeLowered();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::PrimaryAttack(void)
{
	// Can't have an active missile out
	if (m_hMissile != NULL)
		return;

	// Can't be reloading
	if (GetActivity() == ACT_VM_RELOAD)
		return;

	Vector vecOrigin;
	Vector vecForward;

	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;

	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (pOwner == NULL)
		return;

	Vector	vForward, vRight, vUp;

	pOwner->EyeVectors(&vForward, &vRight, &vUp);

	Vector	muzzlePoint = pOwner->Weapon_ShootPosition() + vForward * 12.0f + vRight * 6.0f + vUp * -3.0f;

	QAngle vecAngles;
	VectorAngles(vForward, vecAngles);
	m_hMissile = C1187Missile::Create(muzzlePoint, vecAngles, GetOwner()->edict());

	m_hMissile->m_hOwner = this;

	// If the shot is clear to the player, give the missile a grace period
	trace_t	tr;
	Vector vecEye = pOwner->EyePosition();
	UTIL_TraceLine(vecEye, vecEye + vForward * 128, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);
	if (tr.fraction == 1.0)
	{
		m_hMissile->SetGracePeriod(0.3);
	}

	DecrementAmmo(GetOwner());

	// Register a muzzleflash for the AI
	pOwner->SetMuzzleFlashTime(gpGlobals->curtime + 0.5);

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	WeaponSound(SINGLE);

	pOwner->RumbleEffect(RUMBLE_SHOTGUN_SINGLE, 0, RUMBLE_FLAG_RESTART);

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired(pOwner, true, GetClassname());

	CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 1000, 0.2, GetOwner(), SOUNDENT_CHANNEL_WEAPON);

	// Check to see if we should trigger any RPG firing triggers
	int iCount = g_hWeaponFireTriggers.Count();
	for (int i = 0; i < iCount; i++)
	{
		if (g_hWeaponFireTriggers[i]->IsTouching(pOwner))
		{
			if (FClassnameIs(g_hWeaponFireTriggers[i], "trigger_rpgfire"))
			{
				g_hWeaponFireTriggers[i]->ActivateMultiTrigger(pOwner);
			}
		}
	}

	if (hl2_episodic.GetBool())
	{
		CAI_BaseNPC **ppAIs = g_AI_Manager.AccessAIs();
		int nAIs = g_AI_Manager.NumAIs();

		string_t iszStriderClassname = AllocPooledString("npc_strider");

		for (int i = 0; i < nAIs; i++)
		{
			if (ppAIs[i]->m_iClassname == iszStriderClassname)
			{
				ppAIs[i]->DispatchInteraction(g_interactionPlayerLaunchedRPG, NULL, m_hMissile);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOwner - 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::DecrementAmmo(CBaseCombatCharacter *pOwner)
{
	// Take away our primary ammo type
	pOwner->RemoveAmmo(1, m_iPrimaryAmmoType);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : state - 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::SuppressGuiding(bool state)
{
	m_bHideGuiding = state;

	if (m_hLaserDot == NULL)
	{
		StartGuiding();

		//STILL!?
		if (m_hLaserDot == NULL)
			return;
	}

	if (state)
	{
		m_hLaserDot->TurnOff();
	}
	else
	{
		m_hLaserDot->TurnOn();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Override this if we're guiding a missile currently
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C1187_BaseWeapon_MissileLauncher::Lower(void)
{
	if (m_hMissile != NULL)
		return false;

	return BaseClass::Lower();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::ItemPostFrame(void)
{
	BaseClass::ItemPostFrame();

	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	//If we're pulling the weapon out for the first time, wait to draw the laser
	if ((m_bInitialStateUpdate) && (GetActivity() != ACT_VM_DRAW))
	{
		StartGuiding();
		m_bInitialStateUpdate = false;
	}

	// Supress our guiding effects if we're lowered
	if ( ( !HasIronsights() || !IsIronsighted() ) || 
		 ( GetIdealActivity() == ACT_VM_IDLE_LOWERED || GetIdealActivity() == ACT_VM_RELOAD) )
	{
		SuppressGuiding();
	}
	else
	{
		SuppressGuiding(false);
	}

	//Player has toggled guidance state
	//Adrian: Players are not allowed to remove the laser guide in single player anymore, bye!
	if (g_pGameRules->IsMultiplayer() == true)
	{
		if (pPlayer->m_afButtonPressed & IN_ATTACK2)
		{
			ToggleGuiding();
		}
	}

	if (HasIronsights() && IsIronsighted())
	{
		//Move the laser
		UpdateLaserPosition();
		UpdateLaserEffects();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Vector
//-----------------------------------------------------------------------------
Vector C1187_BaseWeapon_MissileLauncher::GetLaserPosition(void)
{
	CreateLaserPointer();

	if (m_hLaserDot != NULL)
		return m_hLaserDot->GetAbsOrigin();

	//FIXME: The laser dot sprite is not active, this code should not be allowed!
	assert(0);
	return vec3_origin;
}

//-----------------------------------------------------------------------------
// Purpose: NPC RPG users cheat and directly set the laser pointer's origin
// Input  : &vecTarget - 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::UpdateNPCLaserPosition(const Vector &vecTarget)
{
	CreateLaserPointer();
	// Turn the laserdot on
	m_bGuiding = true;
	m_hLaserDot->TurnOn();

	Vector muzzlePoint = GetOwner()->Weapon_ShootPosition();
	Vector vecDir = (vecTarget - muzzlePoint);
	VectorNormalize(vecDir);
	vecDir = muzzlePoint + (vecDir * MAX_TRACE_LENGTH);
	UpdateLaserPosition(muzzlePoint, vecDir);

	SetNPCLaserPosition(vecTarget);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::SetNPCLaserPosition(const Vector &vecTarget)
{
	m_vecNPCLaserDot = vecTarget;
	//NDebugOverlay::Box( m_vecNPCLaserDot, -Vector(10,10,10), Vector(10,10,10), 255,0,0, 8, 3 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const Vector &C1187_BaseWeapon_MissileLauncher::GetNPCLaserPosition(void)
{
	return m_vecNPCLaserDot;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true if the rocket is being guided, false if it's dumb
//-----------------------------------------------------------------------------
bool C1187_BaseWeapon_MissileLauncher::IsGuiding(void)
{
	return m_bGuiding;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C1187_BaseWeapon_MissileLauncher::Deploy(void)
{
	m_bInitialStateUpdate = true;

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_BaseWeapon_MissileLauncher::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	//Can't have an active missile out
	if (m_hMissile != NULL)
		return false;

	StopGuiding();
	return BaseClass::Holster(pSwitchingTo);
}

//-----------------------------------------------------------------------------
// Purpose: Turn on the guiding laser
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::StartGuiding(void)
{
	// Don't start back up if we're overriding this
	if (m_bHideGuiding)
		return;

	m_bGuiding = true;

	WeaponSound(SPECIAL1);

	CreateLaserPointer();
	StartLaserEffects();
}

//-----------------------------------------------------------------------------
// Purpose: Turn off the guiding laser
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::StopGuiding(void)
{
	m_bGuiding = false;

	WeaponSound(SPECIAL2);

	StopLaserEffects();

	// Kill the dot completely
	if (m_hLaserDot != NULL)
	{
		m_hLaserDot->TurnOff();
		UTIL_Remove(m_hLaserDot);
		m_hLaserDot = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Toggle the guiding laser
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::ToggleGuiding(void)
{
	if (IsGuiding())
	{
		StopGuiding();
	}
	else
	{
		StartGuiding();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::Drop(const Vector &vecVelocity)
{
	StopGuiding();

	BaseClass::Drop(vecVelocity);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::UpdateLaserPosition(Vector vecMuzzlePos, Vector vecEndPos)
{
	if (vecMuzzlePos == vec3_origin || vecEndPos == vec3_origin)
	{
		CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
		if (!pPlayer)
			return;

		vecMuzzlePos = pPlayer->Weapon_ShootPosition();
		Vector	forward;

		if (g_pGameRules->GetAutoAimMode() == AUTOAIM_ON_CONSOLE)
		{
			forward = pPlayer->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT);
		}
		else
		{
			pPlayer->EyeVectors(&forward);
		}

		vecEndPos = vecMuzzlePos + (forward * MAX_TRACE_LENGTH);
	}

	//Move the laser dot, if active
	trace_t	tr;

	// Trace out for the endpoint
#ifdef PORTAL
	g_bBulletPortalTrace = true;
	Ray_t rayLaser;
	rayLaser.Init(vecMuzzlePos, vecEndPos);
	UTIL_Portal_TraceRay(rayLaser, (MASK_SHOT & ~CONTENTS_WINDOW), this, COLLISION_GROUP_NONE, &tr);
	g_bBulletPortalTrace = false;
#else
	UTIL_TraceLine(vecMuzzlePos, vecEndPos, (MASK_SHOT & ~CONTENTS_WINDOW), this, COLLISION_GROUP_NONE, &tr);
#endif

	// Move the laser sprite
	if (m_hLaserDot != NULL)
	{
		Vector	laserPos = tr.endpos;
		m_hLaserDot->SetLaserPosition(laserPos, tr.plane.normal);

		if (tr.DidHitNonWorldEntity())
		{
			CBaseEntity *pHit = tr.m_pEnt;

			if ((pHit != NULL) && (pHit->m_takedamage))
			{
				m_hLaserDot->SetTargetEntity(pHit);
			}
			else
			{
				m_hLaserDot->SetTargetEntity(NULL);
			}
		}
		else
		{
			m_hLaserDot->SetTargetEntity(NULL);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::CreateLaserPointer(void)
{
	if (m_hLaserDot != NULL)
		return;

	m_hLaserDot = CLaserDot::Create(GetAbsOrigin(), GetOwnerEntity());
	m_hLaserDot->TurnOff();

	UpdateLaserPosition();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::NotifyRocketDied(void)
{
	m_hMissile = NULL;

	Reload();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187_BaseWeapon_MissileLauncher::Reload(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();

	if (pOwner == NULL)
		return false;

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		return false;

	WeaponSound(RELOAD);

	SendWeaponAnim(ACT_VM_RELOAD);

	if (HasIronsights() && IsIronsighted())
	{
		DisableIronsights();
	}

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool C1187_BaseWeapon_MissileLauncher::WeaponLOSCondition(const Vector &ownerPos, const Vector &targetPos, bool bSetConditions)
{
	bool bResult = BaseClass::WeaponLOSCondition(ownerPos, targetPos, bSetConditions);

	if (bResult)
	{
		CAI_BaseNPC* npcOwner = GetOwner()->MyNPCPointer();

		if (npcOwner)
		{
			trace_t tr;

			Vector vecRelativeShootPosition;
			VectorSubtract(npcOwner->Weapon_ShootPosition(), npcOwner->GetAbsOrigin(), vecRelativeShootPosition);
			Vector vecMuzzle = ownerPos + vecRelativeShootPosition;
			Vector vecShootDir = npcOwner->GetActualShootTrajectory(vecMuzzle);

			// Make sure I have a good 10 feet of wide clearance in front, or I'll blow my teeth out.
			AI_TraceHull(vecMuzzle, vecMuzzle + vecShootDir * (10.0f*12.0f), Vector(-24, -24, -24), Vector(24, 24, 24), MASK_NPCSOLID, NULL, &tr);

			if (tr.fraction != 1.0f)
				bResult = false;
		}
	}

	return bResult;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flDot - 
//			flDist - 
// Output : int
//-----------------------------------------------------------------------------
int C1187_BaseWeapon_MissileLauncher::WeaponRangeAttack1Condition(float flDot, float flDist)
{
	if (m_hMissile != NULL)
		return 0;

	// Ignore vertical distance when doing our RPG distance calculations
	CAI_BaseNPC *pNPC = GetOwner()->MyNPCPointer();
	if (pNPC)
	{
		CBaseEntity *pEnemy = pNPC->GetEnemy();
		Vector vecToTarget = (pEnemy->GetAbsOrigin() - pNPC->GetAbsOrigin());
		vecToTarget.z = 0;
		flDist = vecToTarget.Length();
	}

	if (flDist < MIN(m_fMinRange1, m_fMinRange2))
		return COND_TOO_CLOSE_TO_ATTACK;

	if (m_flNextPrimaryAttack > gpGlobals->curtime)
		return 0;

	// See if there's anyone in the way!
	CAI_BaseNPC *pOwner = GetOwner()->MyNPCPointer();
	ASSERT(pOwner != NULL);

	if (pOwner)
	{
		// Make sure I don't shoot the world!
		trace_t tr;

		Vector vecMuzzle = pOwner->Weapon_ShootPosition();
		Vector vecShootDir = pOwner->GetActualShootTrajectory(vecMuzzle);

		// Make sure I have a good 10 feet of wide clearance in front, or I'll blow my teeth out.
		AI_TraceHull(vecMuzzle, vecMuzzle + vecShootDir * (10.0f*12.0f), Vector(-24, -24, -24), Vector(24, 24, 24), MASK_NPCSOLID, NULL, &tr);

		if (tr.fraction != 1.0)
		{
			return COND_WEAPON_SIGHT_OCCLUDED;
		}
	}

	return COND_CAN_RANGE_ATTACK1;
}

//-----------------------------------------------------------------------------
// Purpose: Start the effects on the viewmodel of the RPG
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::StartLaserEffects(void)
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (pOwner == NULL)
		return;

	// CBaseViewModel *pBeamEnt = static_cast<CBaseViewModel *>(pOwner->GetViewModel());

#if 0
	if (m_hLaserBeam == NULL)
	{
		m_hLaserBeam = CBeam::BeamCreate(RPG_BEAM_SPRITE, 1.0f);

		if (m_hLaserBeam == NULL)
		{
			// We were unable to create the beam
			Assert(0);
			return;
		}

		m_hLaserBeam->EntsInit(pBeamEnt, pBeamEnt);

		int	startAttachment = LookupAttachment("laser");
		int endAttachment = LookupAttachment("laser_end");

		m_hLaserBeam->FollowEntity(pBeamEnt);
		m_hLaserBeam->SetStartAttachment(startAttachment);
		m_hLaserBeam->SetEndAttachment(endAttachment);
		m_hLaserBeam->SetNoise(0);
		m_hLaserBeam->SetColor(255, 0, 0);
		m_hLaserBeam->SetScrollRate(0);
		m_hLaserBeam->SetWidth(0.5f);
		m_hLaserBeam->SetEndWidth(0.5f);
		m_hLaserBeam->SetBrightness(128);
		m_hLaserBeam->SetBeamFlags(SF_BEAM_SHADEIN);
#ifdef PORTAL
		m_hLaserBeam->m_bDrawInMainRender = true;
		m_hLaserBeam->m_bDrawInPortalRender = false;
#endif
	}
	else
	{
		m_hLaserBeam->SetBrightness(128);
	}
#endif

	if (m_hLaserMuzzleSprite == NULL)
	{
		m_hLaserMuzzleSprite = CSprite::SpriteCreate(RPG_LASER_SPRITE, GetAbsOrigin(), false);

		if (m_hLaserMuzzleSprite == NULL)
		{
			// We were unable to create the sprite
			Assert(0);
			return;
		}

#ifdef PORTAL
		m_hLaserMuzzleSprite->m_bDrawInMainRender = true;
		m_hLaserMuzzleSprite->m_bDrawInPortalRender = false;
#endif

		m_hLaserMuzzleSprite->SetAttachment(pOwner->GetViewModel(), LookupAttachment("laser"));
		m_hLaserMuzzleSprite->SetTransparency(kRenderTransAdd, 255, 255, 255, 255, kRenderFxNoDissipation);
		m_hLaserMuzzleSprite->SetBrightness(255, 0.5f);
		m_hLaserMuzzleSprite->SetScale(0.25f, 0.5f);
		m_hLaserMuzzleSprite->TurnOn();
	}
	else
	{
		m_hLaserMuzzleSprite->TurnOn();
		m_hLaserMuzzleSprite->SetScale(0.25f, 0.25f);
		m_hLaserMuzzleSprite->SetBrightness(255);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Stop the effects on the viewmodel of the RPG
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::StopLaserEffects(void)
{
#if 0
	if (m_hLaserBeam != NULL)
	{
		m_hLaserBeam->SetBrightness(0);
	}
#endif

	if (m_hLaserMuzzleSprite != NULL)
	{
		m_hLaserMuzzleSprite->SetScale(0.01f);
		m_hLaserMuzzleSprite->SetBrightness(0, 0.5f);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Pulse all the effects to make them more... well, laser-like
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_MissileLauncher::UpdateLaserEffects(void)
{
	if (!m_bGuiding)
		return;

#if 0
	if (m_hLaserBeam != NULL)
	{
		m_hLaserBeam->SetBrightness(128 + random->RandomInt(-8, 8));
	}
#endif

	if (m_hLaserMuzzleSprite != NULL)
	{
		m_hLaserMuzzleSprite->SetScale(0.1f + random->RandomFloat(-0.025f, 0.025f));
	}
}