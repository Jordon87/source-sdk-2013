//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "basegrenade_shared.h"
#include "grenade_frag.h"
#include "soundent.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define FRAG_GRENADE_WARN_TIME 1.5f

const float GRENADE_COEFFICIENT_OF_RESTITUTION = 0.2f;

ConVar sk_plr_dmg_fraggrenade("sk_plr_dmg_fraggrenade", "0");
ConVar sk_npc_dmg_fraggrenade("sk_npc_dmg_fraggrenade", "0");
ConVar sk_fraggrenade_radius("sk_fraggrenade_radius", "0");

#define GRENADE_MODEL "models/w_grenade.mdl"

class CHoe_Grenade_Frag : public CBaseGrenade
{
	DECLARE_CLASS(CHoe_Grenade_Frag, CBaseGrenade);

#if !defined( CLIENT_DLL )
	DECLARE_DATADESC();
#endif

	~CHoe_Grenade_Frag(void);

public:
	void	Spawn(void);
	void	OnRestore(void);
	void	Precache(void);
	bool	CreateVPhysics(void);
	void	SetTimer(float detonateDelay, float warnDelay);
	void	SetVelocity(const Vector &velocity, const AngularImpulse &angVelocity);
	int		OnTakeDamage(const CTakeDamageInfo &inputInfo);
	void	DelayThink();
	void	VPhysicsUpdate(IPhysicsObject *pPhysics);
	void	OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason);
	void	SetCombineSpawned(bool combineSpawned) { m_combineSpawned = combineSpawned; }
	bool	IsCombineSpawned(void) const { return m_combineSpawned; }
	void	SetPunted(bool punt) { m_punted = punt; }
	bool	WasPunted(void) const { return m_punted; }

	// this function only used in episodic.
#if defined(HL2_EPISODIC) && 0 // FIXME: HandleInteraction() is no longer called now that base grenade derives from CBaseAnimating
	bool	HandleInteraction(int interactionType, void *data, CBaseCombatCharacter* sourceEnt);
#endif 

	void	InputSetTimer(inputdata_t &inputdata);

protected:

	bool	m_inSolid;
	bool	m_combineSpawned;
	bool	m_punted;
};

LINK_ENTITY_TO_CLASS(npc_grenade_frag, CHoe_Grenade_Frag);

BEGIN_DATADESC(CHoe_Grenade_Frag)

// Fields
DEFINE_FIELD(m_inSolid, FIELD_BOOLEAN),
DEFINE_FIELD(m_combineSpawned, FIELD_BOOLEAN),
DEFINE_FIELD(m_punted, FIELD_BOOLEAN),

// Function Pointers
DEFINE_THINKFUNC(DelayThink),

// Inputs
DEFINE_INPUTFUNC(FIELD_FLOAT, "SetTimer", InputSetTimer),

END_DATADESC()


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHoe_Grenade_Frag::~CHoe_Grenade_Frag(void)
{
}

void CHoe_Grenade_Frag::Spawn(void)
{
	Precache();

	SetModel(GRENADE_MODEL);

	if (GetOwnerEntity() && GetOwnerEntity()->IsPlayer())
	{
		m_flDamage = sk_plr_dmg_fraggrenade.GetFloat();
		m_DmgRadius = sk_fraggrenade_radius.GetFloat();
	}
	else
	{
		m_flDamage = sk_npc_dmg_fraggrenade.GetFloat();
		m_DmgRadius = sk_fraggrenade_radius.GetFloat();
	}

	m_takedamage = DAMAGE_YES;
	m_iHealth = 1;

	SetSize(-Vector(4, 4, 4), Vector(4, 4, 4));
	SetCollisionGroup(COLLISION_GROUP_WEAPON);
	CreateVPhysics();

	AddSolidFlags(FSOLID_NOT_STANDABLE);

	m_combineSpawned = false;
	m_punted = false;

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_Grenade_Frag::OnRestore(void)
{
	BaseClass::OnRestore();
}

bool CHoe_Grenade_Frag::CreateVPhysics()
{
	// Create the object in the physics system
	VPhysicsInitNormal(SOLID_BBOX, 0, false);
	return true;
}

// this will hit only things that are in newCollisionGroup, but NOT in collisionGroupAlreadyChecked
class CTraceFilterCollisionGroupDelta : public CTraceFilterEntitiesOnly
{
public:
	// It does have a base, but we'll never network anything below here..
	DECLARE_CLASS_NOBASE(CTraceFilterCollisionGroupDelta);

	CTraceFilterCollisionGroupDelta(const IHandleEntity *passentity, int collisionGroupAlreadyChecked, int newCollisionGroup)
		: m_pPassEnt(passentity), m_collisionGroupAlreadyChecked(collisionGroupAlreadyChecked), m_newCollisionGroup(newCollisionGroup)
	{
	}

	virtual bool ShouldHitEntity(IHandleEntity *pHandleEntity, int contentsMask)
	{
		if (!PassServerEntityFilter(pHandleEntity, m_pPassEnt))
			return false;
		CBaseEntity *pEntity = EntityFromEntityHandle(pHandleEntity);

		if (pEntity)
		{
			if (g_pGameRules->ShouldCollide(m_collisionGroupAlreadyChecked, pEntity->GetCollisionGroup()))
				return false;
			if (g_pGameRules->ShouldCollide(m_newCollisionGroup, pEntity->GetCollisionGroup()))
				return true;
		}

		return false;
	}

protected:
	const IHandleEntity *m_pPassEnt;
	int		m_collisionGroupAlreadyChecked;
	int		m_newCollisionGroup;
};

void CHoe_Grenade_Frag::VPhysicsUpdate(IPhysicsObject *pPhysics)
{
	BaseClass::VPhysicsUpdate(pPhysics);
	Vector vel;
	AngularImpulse angVel;
	pPhysics->GetVelocity(&vel, &angVel);

	Vector start = GetAbsOrigin();
	// find all entities that my collision group wouldn't hit, but COLLISION_GROUP_NONE would and bounce off of them as a ray cast
	CTraceFilterCollisionGroupDelta filter(this, GetCollisionGroup(), COLLISION_GROUP_NONE);
	trace_t tr;

	// UNDONE: Hull won't work with hitboxes - hits outer hull.  But the whole point of this test is to hit hitboxes.
#if 0
	UTIL_TraceHull(start, start + vel * gpGlobals->frametime, CollisionProp()->OBBMins(), CollisionProp()->OBBMaxs(), CONTENTS_HITBOX | CONTENTS_MONSTER | CONTENTS_SOLID, &filter, &tr);
#else
	UTIL_TraceLine(start, start + vel * gpGlobals->frametime, CONTENTS_HITBOX | CONTENTS_MONSTER | CONTENTS_SOLID, &filter, &tr);
#endif
	if (tr.startsolid)
	{
		if (!m_inSolid)
		{
			// UNDONE: Do a better contact solution that uses relative velocity?
			vel *= -GRENADE_COEFFICIENT_OF_RESTITUTION; // bounce backwards
			pPhysics->SetVelocity(&vel, NULL);
		}
		m_inSolid = true;
		return;
	}
	m_inSolid = false;
	if (tr.DidHit())
	{
		Vector dir = vel;
		VectorNormalize(dir);
		// send a tiny amount of damage so the character will react to getting bonked
		CTakeDamageInfo info(this, GetThrower(), pPhysics->GetMass() * vel, GetAbsOrigin(), 0.1f, DMG_CRUSH);
		tr.m_pEnt->TakeDamage(info);

		// reflect velocity around normal
		vel = -2.0f * tr.plane.normal * DotProduct(vel, tr.plane.normal) + vel;

		// absorb 80% in impact
		vel *= GRENADE_COEFFICIENT_OF_RESTITUTION;
		angVel *= -0.5f;
		pPhysics->SetVelocity(&vel, &angVel);
	}
}


void CHoe_Grenade_Frag::Precache(void)
{
	PrecacheModel(GRENADE_MODEL);

	BaseClass::Precache();
}

void CHoe_Grenade_Frag::SetTimer(float detonateDelay, float warnDelay)
{
	m_flDetonateTime = gpGlobals->curtime + detonateDelay;
	m_flWarnAITime = gpGlobals->curtime + warnDelay;
	SetThink(&CHoe_Grenade_Frag::DelayThink);
	SetNextThink(gpGlobals->curtime);
}

void CHoe_Grenade_Frag::OnPhysGunPickup(CBasePlayer *pPhysGunUser, PhysGunPickup_t reason)
{
	SetThrower(pPhysGunUser);


	if (IsX360())
	{
		// Give 'em a couple of seconds to aim and throw. 
		SetTimer(2.0f, 1.0f);
	}

	SetPunted(true);

	BaseClass::OnPhysGunPickup(pPhysGunUser, reason);
}

void CHoe_Grenade_Frag::DelayThink()
{
	if (gpGlobals->curtime > m_flDetonateTime)
	{
		Detonate();
		return;
	}

	if (!m_bHasWarnedAI && gpGlobals->curtime >= m_flWarnAITime)
	{
#if !defined( CLIENT_DLL )
		CSoundEnt::InsertSound(SOUND_DANGER, GetAbsOrigin(), 400, 1.5, this);
#endif
		m_bHasWarnedAI = true;
	}

	SetNextThink(gpGlobals->curtime + 0.1);
}

void CHoe_Grenade_Frag::SetVelocity(const Vector &velocity, const AngularImpulse &angVelocity)
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();
	if (pPhysicsObject)
	{
		pPhysicsObject->AddVelocity(&velocity, &angVelocity);
	}
}

int CHoe_Grenade_Frag::OnTakeDamage(const CTakeDamageInfo &inputInfo)
{
	// Manually apply vphysics because BaseCombatCharacter takedamage doesn't call back to CBaseEntity OnTakeDamage
	VPhysicsTakeDamage(inputInfo);

	// Grenades only suffer blast damage and burn damage.
	if (!(inputInfo.GetDamageType() & (DMG_BLAST | DMG_BURN)))
		return 0;

	return BaseClass::OnTakeDamage(inputInfo);
}

void CHoe_Grenade_Frag::InputSetTimer(inputdata_t &inputdata)
{
	SetTimer(inputdata.value.Float(), inputdata.value.Float() - FRAG_GRENADE_WARN_TIME);
}

CBaseGrenade *Fraggrenade_Create(const Vector &position, const QAngle &angles, const Vector &velocity, const AngularImpulse &angVelocity, CBaseEntity *pOwner, float timer, bool combineSpawned)
{
	// Don't set the owner here, or the player can't interact with grenades he's thrown
	CHoe_Grenade_Frag *pGrenade = (CHoe_Grenade_Frag *)CBaseEntity::Create("npc_grenade_frag", position, angles, pOwner);

	pGrenade->SetTimer(timer, timer - FRAG_GRENADE_WARN_TIME);
	pGrenade->SetVelocity(velocity, angVelocity);
	pGrenade->SetThrower(ToBaseCombatCharacter(pOwner));
	pGrenade->m_takedamage = DAMAGE_EVENTS_ONLY;
	pGrenade->SetCombineSpawned(combineSpawned);

	return pGrenade;
}

bool Fraggrenade_WasPunted(const CBaseEntity *pEntity)
{
	const CHoe_Grenade_Frag *pFrag = dynamic_cast<const CHoe_Grenade_Frag *>(pEntity);
	if (pFrag)
	{
		return pFrag->WasPunted();
	}

	return false;
}

bool Fraggrenade_WasCreatedByCombine(const CBaseEntity *pEntity)
{
	const CHoe_Grenade_Frag *pFrag = dynamic_cast<const CHoe_Grenade_Frag *>(pEntity);
	if (pFrag)
	{
		return pFrag->IsCombineSpawned();
	}

	return false;
}