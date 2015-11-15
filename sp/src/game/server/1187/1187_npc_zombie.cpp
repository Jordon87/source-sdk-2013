//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "1187_npc_basezombie_headless_classic.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sk_zombie_health;

class C1187_NPC_Zombie : public C1187_NPC_BaseZombie_Headless_Classic
{
	DECLARE_CLASS(C1187_NPC_Zombie, C1187_NPC_BaseZombie_Headless_Classic);
public:

	void Spawn(void);
	void Precache(void);

	void SetZombieModel(void);
	void MoanSound(envelopePoint_t *pEnvelope, int iEnvelopeSize);

	virtual const char *GetLegsModel(void);
	virtual const char *GetTorsoModel(void);
	virtual const char *GetHeadcrabClassname(void);
	virtual const char *GetHeadcrabModel(void);

	void PainSound(const CTakeDamageInfo &info);
	void DeathSound(const CTakeDamageInfo &info);
	void AlertSound(void);
	void IdleSound(void);
	void AttackSound(void);
	void AttackHitSound(void);
	void AttackMissSound(void);
	void FootstepSound(bool fRightFoot);
	void FootscuffSound(bool fRightFoot);

	const char *GetMoanSound(int nSound);

protected:
	static const char *pMoanSounds[];
};

LINK_ENTITY_TO_CLASS(npc_zombie, C1187_NPC_Zombie);
LINK_ENTITY_TO_CLASS(npc_zombie_torso, C1187_NPC_Zombie);

//---------------------------------------------------------
//---------------------------------------------------------
const char *C1187_NPC_Zombie::pMoanSounds[] =
{
	"NPC_BaseZombie.Moan1",
	"NPC_BaseZombie.Moan2",
	"NPC_BaseZombie.Moan3",
	"NPC_BaseZombie.Moan4",
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel("models/zombie/classic.mdl");
	PrecacheModel("models/zombie/classic_torso.mdl");
	PrecacheModel("models/zombie/classic_legs.mdl");

	PrecacheScriptSound("Zombie.FootstepRight");
	PrecacheScriptSound("Zombie.FootstepLeft");
	PrecacheScriptSound("Zombie.FootstepLeft");
	PrecacheScriptSound("Zombie.ScuffRight");
	PrecacheScriptSound("Zombie.ScuffLeft");
	PrecacheScriptSound("Zombie.AttackHit");
	PrecacheScriptSound("Zombie.AttackMiss");
	PrecacheScriptSound("Zombie.Pain");
	PrecacheScriptSound("Zombie.Die");
	PrecacheScriptSound("Zombie.Alert");
	PrecacheScriptSound("Zombie.Idle");
	PrecacheScriptSound("Zombie.Attack");

	PrecacheScriptSound("NPC_BaseZombie.Moan1");
	PrecacheScriptSound("NPC_BaseZombie.Moan2");
	PrecacheScriptSound("NPC_BaseZombie.Moan3");
	PrecacheScriptSound("NPC_BaseZombie.Moan4");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::Spawn(void)
{
	Precache();

	if (FClassnameIs(this, "npc_zombie"))
	{
		m_fIsTorso = false;
	}
	else
	{
		// This was placed as an npc_zombie_torso
		m_fIsTorso = true;
	}

	m_fIsHeadless = false;

#ifdef HL2_EPISODIC
	SetBloodColor(BLOOD_COLOR_ZOMBIE);
#else
	SetBloodColor(BLOOD_COLOR_GREEN);
#endif // HL2_EPISODIC

	m_iHealth = sk_zombie_health.GetFloat();
	m_flFieldOfView = 0.2;

	CapabilitiesClear();

	//GetNavigator()->SetRememberStaleNodes( false );

	BaseClass::Spawn();

	m_flNextMoanSound = gpGlobals->curtime + random->RandomFloat(1.0, 4.0);
}


//-----------------------------------------------------------------------------
// Purpose: Sound of a footstep
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::FootstepSound(bool fRightFoot)
{
	if (fRightFoot)
	{
		EmitSound("Zombie.FootstepRight");
	}
	else
	{
		EmitSound("Zombie.FootstepLeft");
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sound of a foot sliding/scraping
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::FootscuffSound(bool fRightFoot)
{
	if (fRightFoot)
	{
		EmitSound("Zombie.ScuffRight");
	}
	else
	{
		EmitSound("Zombie.ScuffLeft");
	}
}

//-----------------------------------------------------------------------------
// Purpose: Play a random attack hit sound
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::AttackHitSound(void)
{
	EmitSound("Zombie.AttackHit");
}

//-----------------------------------------------------------------------------
// Purpose: Play a random attack miss sound
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::AttackMissSound(void)
{
	// Play a random attack miss sound
	EmitSound("Zombie.AttackMiss");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::PainSound(const CTakeDamageInfo &info)
{
	// We're constantly taking damage when we are on fire. Don't make all those noises!
	if (IsOnFire())
	{
		return;
	}

	EmitSound("Zombie.Pain");
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::DeathSound(const CTakeDamageInfo &info)
{
	EmitSound("Zombie.Die");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::AlertSound(void)
{
	EmitSound("Zombie.Alert");

	// Don't let a moan sound cut off the alert sound.
	m_flNextMoanSound += random->RandomFloat(2.0, 4.0);
}

//-----------------------------------------------------------------------------
// Purpose: Returns a moan sound for this class of zombie.
//-----------------------------------------------------------------------------
const char *C1187_NPC_Zombie::GetMoanSound(int nSound)
{
	return pMoanSounds[nSound % ARRAYSIZE(pMoanSounds)];
}

//-----------------------------------------------------------------------------
// Purpose: Play a random idle sound.
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::IdleSound(void)
{
	if (GetState() == NPC_STATE_IDLE && random->RandomFloat(0, 1) == 0)
	{
		// Moan infrequently in IDLE state.
		return;
	}

	if (IsSlumped())
	{
		// Sleeping zombies are quiet.
		return;
	}

	EmitSound("Zombie.Idle");
	MakeAISpookySound(360.0f);
}

//-----------------------------------------------------------------------------
// Purpose: Play a random attack sound.
//-----------------------------------------------------------------------------
void C1187_NPC_Zombie::AttackSound(void)
{
	EmitSound("Zombie.Attack");
}

//-----------------------------------------------------------------------------
// Purpose: Returns the classname (ie "npc_headcrab") to spawn when our headcrab bails.
//-----------------------------------------------------------------------------
const char *C1187_NPC_Zombie::GetHeadcrabClassname(void)
{
	return "npc_headcrab";
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char *C1187_NPC_Zombie::GetHeadcrabModel(void)
{
	return "models/headcrabclassic.mdl";
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *C1187_NPC_Zombie::GetLegsModel(void)
{
	return "models/zombie/classic_legs.mdl";
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const char *C1187_NPC_Zombie::GetTorsoModel(void)
{
	return "models/zombie/classic_torso.mdl";
}


//---------------------------------------------------------
//---------------------------------------------------------
void C1187_NPC_Zombie::SetZombieModel(void)
{
	Hull_t lastHull = GetHullType();

	if (m_fIsTorso)
	{
		SetModel("models/zombie/classic_torso.mdl");
		SetHullType(HULL_TINY);
	}
	else
	{
		SetModel("models/zombie/classic.mdl");
		SetHullType(HULL_HUMAN);
	}

	SetBodygroup(ZOMBIE_BODYGROUP_HEADCRAB, !m_fIsHeadless);

	SetHullSizeNormal(true);
	SetDefaultEyeOffset();
	SetActivity(ACT_IDLE);

	// hull changed size, notify vphysics
	// UNDONE: Solve this generally, systematically so other
	// NPCs can change size
	if (lastHull != GetHullType())
	{
		if (VPhysicsGetObject())
		{
			SetupVPhysicsHull();
		}
	}
}

//---------------------------------------------------------
// Classic zombie only uses moan sound if on fire.
//---------------------------------------------------------
void C1187_NPC_Zombie::MoanSound(envelopePoint_t *pEnvelope, int iEnvelopeSize)
{
	if (IsOnFire())
	{
		BaseClass::MoanSound(pEnvelope, iEnvelopeSize);
	}
}