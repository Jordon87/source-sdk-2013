//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "1187_npc_basezombie_headless.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar	sk_zombie_health;

class CNPC_BurstZombie : public C1187_NPC_BaseZombie_Headless
{
	DECLARE_CLASS(CNPC_BurstZombie, C1187_NPC_BaseZombie_Headless);
public:

	void Spawn(void);
	void Precache(void);

	void SetZombieModel(void);

};

LINK_ENTITY_TO_CLASS(npc_burstzombie, CNPC_BurstZombie);

void CNPC_BurstZombie::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel("models/zombie/burstclassic.mdl");
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

void CNPC_BurstZombie::Spawn(void)
{
	BaseClass::Spawn();

	m_iHealth = sk_zombie_health.GetFloat();
	SetMaxHealth(m_iHealth);

	m_fIsHeadless = true;
}