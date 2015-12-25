//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hoe_npc_BaseHuman.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define NPC_CHARLIE_MODEL	"models/charlie/charlie.mdl"

extern ConVar sk_hoe_human_health;

class CHoe_NPC_Charlie : public CHoe_NPC_BaseHuman
{
	DECLARE_CLASS(CHoe_NPC_Charlie, CHoe_NPC_BaseHuman);
public:
	DECLARE_DATADESC();

	void SelectModel();
	void Spawn(void);

	Class_T Classify(void) { return CLASS_COMBINE; }
};

LINK_ENTITY_TO_CLASS(npc_charlie, CHoe_NPC_Charlie);

BEGIN_DATADESC(CHoe_NPC_Charlie)
END_DATADESC()

void CHoe_NPC_Charlie::SelectModel()
{
	SetModelName(AllocPooledString(NPC_CHARLIE_MODEL));
}

void CHoe_NPC_Charlie::Spawn(void)
{
	m_nBody = 0;
	m_nSkin = 0;

	BaseClass::Spawn();

	m_iHealth = sk_hoe_human_health.GetFloat();

	CapabilitiesRemove(bits_CAP_FRIENDLY_DMG_IMMUNE);

	NPCInit();
}