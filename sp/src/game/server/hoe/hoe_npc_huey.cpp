//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//

#include "cbase.h"
#include "ai_network.h"
#include "ai_default.h"
#include "ai_schedule.h"
#include "ai_hull.h"
#include "ai_node.h"
#include "ai_task.h"
#include "entitylist.h"
#include "basecombatweapon.h"
#include "soundenvelope.h"
#include "gib.h"
#include "gamerules.h"
#include "ammodef.h"
#include "grenade_homer.h"
#include "cbasehelicopter.h"
#include "engine/IEngineSound.h"
#include "IEffects.h"
#include "globals.h"
#include "explode.h"
#include "movevars_shared.h"
#include "smoke_trail.h"
#include "ar2_explosion.h"
#include "collisionutils.h"
#include "props.h"
#include "EntityFlame.h"
#include "decals.h"
#include "effect_dispatch_data.h"
#include "te_effect_dispatch.h"
#include "ai_spotlight.h"
#include "vphysics/constraints.h"
#include "physics_saverestore.h"
#include "ai_memory.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar	sk_huey_health("sk_huey_health", "100");

//---------------------------------------------------------
//---------------------------------------------------------
#define SF_PASSENGER			0x80
#define SF_ROCKETS				0x200
#define SF_GRENADELAUNCHER		0x400


// -------------------------------------
// Speed
#define HUEY_MAX_SPEED			(60 * 17.6f)
#define HUEY_MAX_FIRING_SPEED	250.0f
#define HUEY_MAX_GUN_DIST		2000.0f

#define HUEY_ACCEL_RATE			500
#define HUEY_ACCEL_RATE_BOOST	1500

#define DEFAULT_FREE_KNOWLEDGE_DURATION 5.0f

// -------------------------------------
// Pathing data
#define	HUEY_LEAD_DISTANCE				800.0f
#define	HUEY_MIN_CHASE_DIST_DIFF		128.0f	// Distance threshold used to determine when a target has moved enough to update our navigation to it
#define HUEY_MIN_AGGRESSIVE_CHASE_DIST_DIFF 64.0f
#define	HUEY_AVOID_DIST					512.0f
#define	HUEY_ARRIVE_DIST				128.0f

enum ERappelAttachment
{
	RAPPEL0_ATTACH = 0,
	RAPPEL1_ATTACH,
	RAPPEL2_ATTACH,
	RAPPEL3_ATTACH,

	RAPPEL_ATTACH_COUNT,
};

class CHoe_NPC_Huey : public CBaseHelicopter
{
public:
	DECLARE_CLASS(CHoe_NPC_Huey, CBaseHelicopter);
	DECLARE_DATADESC();
	DEFINE_CUSTOM_AI;

	CHoe_NPC_Huey();
	~CHoe_NPC_Huey();

	virtual void	Precache(void);
	virtual void	Spawn(void);
	virtual void	Activate(void);

	virtual bool	CreateVPhysics(void);

	// Gets the max speed of the helicopter
	virtual float GetMaxSpeed();
	virtual float GetMaxSpeedFiring();

	// Returns the max firing distance
	float GetMaxFiringDistance();

	// Updates the enemy
	virtual float	EnemySearchDistance();

private:
	enum GunState_t
	{
		GUN_STATE_IDLE = 0,
		GUN_STATE_CHARGING,
		GUN_STATE_FIRING,
	};

	int			m_iAmmoType;
	GunState_t	m_nGunState;

	int			m_nLadderAttachment;
	int			m_nExhaustAttachment;
	int			m_nGrenadeBaseAttachment;
	int			m_nGrenadeMuzzleAttachment;

	int			m_nLeftGunBaseAttachment;
	int			m_nLeftGunMuzzleAttachment;

	int			m_nRightGunBaseAttachment;
	int			m_nRightGunMuzzleAttachment;

	int			m_nPassengerGunBaseAttachment;
	int			m_nPassengerGunMuzzleAttachment;

	int			m_nLeftMissileAttachment;
	int			m_nRightMissileAttachment;

	int			m_nRappelAttachments[RAPPEL_ATTACH_COUNT];
};

LINK_ENTITY_TO_CLASS(npc_huey, CHoe_NPC_Huey);

BEGIN_DATADESC(CHoe_NPC_Huey)
	DEFINE_FIELD(m_iAmmoType, FIELD_INTEGER),
	DEFINE_FIELD(m_nGunState, FIELD_INTEGER),

	// Attachments
	DEFINE_FIELD(m_nLadderAttachment, FIELD_INTEGER),
	DEFINE_FIELD(m_nExhaustAttachment, FIELD_INTEGER),

	DEFINE_FIELD(m_nGrenadeBaseAttachment, FIELD_INTEGER),
	DEFINE_FIELD(m_nGrenadeMuzzleAttachment, FIELD_INTEGER),

	DEFINE_FIELD(m_nLeftGunBaseAttachment, FIELD_INTEGER),
	DEFINE_FIELD(m_nLeftGunMuzzleAttachment, FIELD_INTEGER),
	DEFINE_FIELD(m_nRightGunBaseAttachment, FIELD_INTEGER),
	DEFINE_FIELD(m_nRightGunMuzzleAttachment, FIELD_INTEGER),

	DEFINE_FIELD(m_nPassengerGunBaseAttachment, FIELD_INTEGER),
	DEFINE_FIELD(m_nPassengerGunMuzzleAttachment, FIELD_INTEGER),

	DEFINE_FIELD(m_nLeftMissileAttachment, FIELD_INTEGER),
	DEFINE_FIELD(m_nRightMissileAttachment, FIELD_INTEGER),

	DEFINE_ARRAY(m_nRappelAttachments, FIELD_INTEGER, RAPPEL_ATTACH_COUNT),
END_DATADESC()

CHoe_NPC_Huey::CHoe_NPC_Huey()
{
	m_iAmmoType = -1;
}

CHoe_NPC_Huey::~CHoe_NPC_Huey()
{

}

//-----------------------------------------------------------------------------
// Purpose :
//-----------------------------------------------------------------------------
void CHoe_NPC_Huey::Spawn(void)
{
	Precache();

	SetModel("models/huey/huey.mdl");

	ExtractBbox(SelectHeaviestSequence(ACT_IDLE), m_cullBoxMins, m_cullBoxMaxs);
	GetEnemies()->SetFreeKnowledgeDuration(DEFAULT_FREE_KNOWLEDGE_DURATION);

	float flLoadedSpeed = m_flMaxSpeed;
	BaseClass::Spawn();

	InitPathingData(HUEY_ARRIVE_DIST, HUEY_MIN_CHASE_DIST_DIFF, HUEY_AVOID_DIST);
	SetFarthestPathDist(GetMaxFiringDistance());

	m_takedamage = DAMAGE_YES;
	m_nGunState = GUN_STATE_IDLE;
	SetHullType(HULL_LARGE_CENTERED);

	SetHullSizeNormal();

	CreateVPhysics();

	SetPauseState(PAUSE_NO_PAUSE);

	m_iMaxHealth = m_iHealth = sk_huey_health.GetInt();

	m_flMaxSpeed = flLoadedSpeed;
	if (m_flMaxSpeed <= 0)
	{
		m_flMaxSpeed = HUEY_MAX_SPEED;
	}

	m_flFieldOfView = -1.0; // 360 degrees
	m_iAmmoType = GetAmmoDef()->Index("HelicopterGun");

	InitBoneControllers();

	m_fHelicopterFlags = BITS_HELICOPTER_GUN_ON;
	m_bSuppressSound = false;

	SetActivity(ACT_IDLE);

	AddFlag(FL_AIMTARGET);
}

//-----------------------------------------------------------------------------
// Purpose :
//-----------------------------------------------------------------------------
void CHoe_NPC_Huey::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel("models/huey/huey.mdl");

	PrecacheScriptSound("NPC_Huey.FireGun");
	PrecacheScriptSound("NPC_Huey.Grenade");
	PrecacheScriptSound("NPC_Huey.Rotors");
	PrecacheScriptSound("NPC_Huey.Explode");
	PrecacheScriptSound("NPC_Huey.PassengerFireGun");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHoe_NPC_Huey::CreateVPhysics(void)
{
	return BaseClass::CreateVPhysics();
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CHoe_NPC_Huey::Activate(void)
{
	BaseClass::Activate();

	m_nLadderAttachment = LookupAttachment("ladder");
	m_nExhaustAttachment = LookupAttachment("exhaust");
	m_nGrenadeBaseAttachment = LookupAttachment("grenademuzzle");
	m_nGrenadeMuzzleAttachment = LookupAttachment("grenademuzzle");

	m_nLeftGunBaseAttachment = LookupAttachment("leftgunbase");
	m_nLeftGunMuzzleAttachment = LookupAttachment("leftgunmuzzle");

	m_nRightGunBaseAttachment = LookupAttachment("rightgunbase");
	m_nRightGunMuzzleAttachment = LookupAttachment("rightgunmuzzle");

	m_nPassengerGunBaseAttachment = LookupAttachment("pgunbase");
	m_nPassengerGunMuzzleAttachment = LookupAttachment("pgunmuzzle");

	m_nLeftMissileAttachment = LookupAttachment("leftmissile");
	m_nRightMissileAttachment = LookupAttachment("rightmissile");

	m_nRappelAttachments[RAPPEL0_ATTACH] = LookupAttachment("rappel0");
	m_nRappelAttachments[RAPPEL1_ATTACH] = LookupAttachment("rappel1");
	m_nRappelAttachments[RAPPEL2_ATTACH] = LookupAttachment("rappel2");
	m_nRappelAttachments[RAPPEL3_ATTACH] = LookupAttachment("rappel3");
}


//------------------------------------------------------------------------------
// Gets the max speed of the helicopter
//------------------------------------------------------------------------------
float CHoe_NPC_Huey::GetMaxSpeed()
{
	return 3000.0f;
}

float CHoe_NPC_Huey::GetMaxSpeedFiring()
{
	return 3000.0f;
}

//------------------------------------------------------------------------------
// Returns the max firing distance
//------------------------------------------------------------------------------
float CHoe_NPC_Huey::GetMaxFiringDistance()
{
	return 8000.0f;
}


//------------------------------------------------------------------------------
// Updates the enemy
//------------------------------------------------------------------------------
float CHoe_NPC_Huey::EnemySearchDistance()
{
	return 6000.0f;
}


//-----------------------------------------------------------------------------
//
// Schedules
//
//----------------------------------------------------------------------------
AI_BEGIN_CUSTOM_NPC(npc_huey, CHoe_NPC_Huey)
AI_END_CUSTOM_NPC()