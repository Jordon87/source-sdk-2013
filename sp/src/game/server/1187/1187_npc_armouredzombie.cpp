//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "1187_npc_basezombie_headless_runner.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sk_armoured_zombie_health("sk_armoured_zombie_health", "0");
ConVar sk_armoured_zombie_dmg_one_slash("sk_armoured_zombie_dmg_one_slash", "0");

class CNPC_ArmouredZombie : public C1187_NPC_BaseZombie_Headless_Runner
{
	DECLARE_CLASS(CNPC_ArmouredZombie, C1187_NPC_BaseZombie_Headless_Runner);
public:

	void Spawn(void);
	void Precache(void);

	void SetZombieModel(void);

	void HandleAnimEvent(animevent_t *pEvent);
};

LINK_ENTITY_TO_CLASS(npc_armouredzombie, CNPC_ArmouredZombie);

void CNPC_ArmouredZombie::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel("models/zombie/ArmouredClassic.mdl");
}

void CNPC_ArmouredZombie::SetZombieModel(void)
{
	SetModel("models/zombie/ArmouredClassic.mdl");
	SetHullType(HULL_HUMAN);

	SetBodygroup(ZOMBIE_BODYGROUP_HEADCRAB, !m_fIsHeadless);

	SetHullSizeNormal(true);
	SetDefaultEyeOffset();
	SetActivity(ACT_IDLE);
}

void CNPC_ArmouredZombie::Spawn(void)
{
	BaseClass::Spawn();

	m_iHealth = sk_armoured_zombie_health.GetFloat();
	SetMaxHealth(m_iHealth);

	m_fIsHeadless = true;
}


//-----------------------------------------------------------------------------
// Purpose: Catches the monster-specific events that occur when tagged animation
//			frames are played.
// Input  : pEvent - 
//-----------------------------------------------------------------------------
void CNPC_ArmouredZombie::HandleAnimEvent(animevent_t *pEvent)
{
	if (pEvent->event == AE_ZOMBIE_ATTACK_RIGHT)
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

	if (pEvent->event == AE_ZOMBIE_ATTACK_LEFT)
	{
		Vector right, forward;
		AngleVectors(GetLocalAngles(), &forward, &right, NULL);

		right = right * -100;
		forward = forward * 200;

		QAngle qa(-15, 20, -10);
		Vector vec = right + forward;
		ClawAttack(GetClawAttackRange(), sk_armoured_zombie_dmg_one_slash.GetFloat(), qa, vec, ZOMBIE_BLOOD_LEFT_HAND);
		return;
	}

	BaseClass::HandleAnimEvent(pEvent);
}