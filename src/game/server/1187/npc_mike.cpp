//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_playercompanion.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MIKE_MODEL		"models/humans/group01/male_09.mdl"

class CNPC_Mike : public CNPC_PlayerCompanion
{
	DECLARE_CLASS(CNPC_Mike, CNPC_PlayerCompanion);
	DEFINE_CUSTOM_AI;
public:
	virtual void SelectModel(void);

	virtual void Precache(void);
	virtual void Spawn(void);
};

LINK_ENTITY_TO_CLASS(npc_mike, CNPC_Mike);

AI_BEGIN_CUSTOM_NPC(npc_mike, CNPC_Mike)

AI_END_CUSTOM_NPC()

void CNPC_Mike::SelectModel()
{
	SetModelName(AllocPooledString(MIKE_MODEL));
}

void CNPC_Mike::Precache(void)
{
	BaseClass::Precache();
	PrecacheModel(STRING(GetModelName()));
}

void CNPC_Mike::Spawn(void)
{
	BaseClass::Spawn();
	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_STANDABLE);
	SetMoveType(MOVETYPE_STEP);

	CapabilitiesAdd(bits_CAP_MOVE_GROUND | bits_CAP_DOORS_GROUP | bits_CAP_TURN_HEAD | bits_CAP_DUCK | bits_CAP_SQUAD);
	CapabilitiesAdd(bits_CAP_ANIMATEDFACE);
	CapabilitiesAdd(bits_CAP_FRIENDLY_DMG_IMMUNE);
	CapabilitiesRemove(bits_CAP_USE_SHOT_REGULATOR);

	m_FollowBehavior.SetFollowTarget(UTIL_GetLocalPlayer());
	m_FollowBehavior.SetParameters(AIF_SIMPLE);

	AddEFlags(EFL_NO_DISSOLVE | EFL_NO_MEGAPHYSCANNON_RAGDOLL | EFL_NO_PHYSCANNON_INTERACTION);

	m_iHealth = 100;
	m_iMaxHealth = 100;

	NPCInit();
}