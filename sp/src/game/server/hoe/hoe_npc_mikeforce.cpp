//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hoe_npc_BaseHuman_companion.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define NPC_MIKEFORCE_MODEL	"models/mikeforce/mikeforce.mdl"

extern ConVar sk_hoe_human_health;

class CHoe_NPC_MikeForce : public CHoe_NPC_BaseHuman_Companion
{
	DECLARE_CLASS(CHoe_NPC_MikeForce, CHoe_NPC_BaseHuman_Companion);
public:
	DECLARE_DATADESC();

	void SelectModel();
	void Spawn(void);

	//---------------------------------
	Class_T 		Classify() { return CLASS_PLAYER_ALLY; }
};

LINK_ENTITY_TO_CLASS(npc_mikeforce, CHoe_NPC_MikeForce);

BEGIN_DATADESC(CHoe_NPC_MikeForce)
DEFINE_USEFUNC(CommanderUse),
DEFINE_USEFUNC(SimpleUse),
END_DATADESC()

void CHoe_NPC_MikeForce::SelectModel()
{
	SetModelName(AllocPooledString(NPC_MIKEFORCE_MODEL));
}

void CHoe_NPC_MikeForce::Spawn(void)
{
	m_nBody = 0;
	m_nSkin = 0;

	BaseClass::Spawn();

	m_iHealth = sk_hoe_human_health.GetFloat();

	AddSpawnFlags( SF_HOE_HUMAN_COMPANION_FOLLOW );

	CapabilitiesRemove(bits_CAP_FRIENDLY_DMG_IMMUNE);
}