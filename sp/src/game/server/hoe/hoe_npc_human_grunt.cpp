//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hoe_npc_BaseHuman_companion.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define NPC_HUMAN_GRUNT_MODEL	"models/namgrunt/namgrunt.mdl"

extern ConVar sk_hoe_human_health;

class CHoe_NPC_Human_Grunt : public CHoe_NPC_BaseHuman_Companion
{
	DECLARE_CLASS(CHoe_NPC_Human_Grunt, CHoe_NPC_BaseHuman_Companion);
public:
	DECLARE_DATADESC();

	void SelectModel();
	void Spawn(void);

	//---------------------------------
	Class_T 		Classify() { return CLASS_COMBINE; }
};

LINK_ENTITY_TO_CLASS(npc_human_grunt, CHoe_NPC_Human_Grunt);

BEGIN_DATADESC(CHoe_NPC_Human_Grunt)
END_DATADESC()

void CHoe_NPC_Human_Grunt::SelectModel()
{
	SetModelName(AllocPooledString(NPC_HUMAN_GRUNT_MODEL));
}

void CHoe_NPC_Human_Grunt::Spawn(void)
{
	m_nBody = 0;
	m_nSkin = 0;

	BaseClass::Spawn();

	m_iHealth = sk_hoe_human_health.GetFloat();

	CapabilitiesRemove(bits_CAP_FRIENDLY_DMG_IMMUNE);

	NPCInit();
}