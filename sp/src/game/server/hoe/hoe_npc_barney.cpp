//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hoe_npc_BaseHuman_companion.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define NPC_BARNEY_MODEL	"models/barney/barney.mdl"

ConVar sk_barney_health("sk_barney_health", "0");

class CHoe_NPC_Barney : public CHoe_NPC_BaseHuman_Companion
{
	DECLARE_CLASS(CHoe_NPC_Barney, CHoe_NPC_BaseHuman_Companion);
public:
	DECLARE_DATADESC();

	void SelectModel();
	void Spawn(void);

	Class_T Classify(void) { return CLASS_PLAYER_ALLY_VITAL; }
};

LINK_ENTITY_TO_CLASS(npc_barney, CHoe_NPC_Barney);

BEGIN_DATADESC(CHoe_NPC_Barney)
DEFINE_USEFUNC(CommanderUse),
DEFINE_USEFUNC(SimpleUse),
END_DATADESC()

void CHoe_NPC_Barney::SelectModel()
{
	SetModelName( AllocPooledString( NPC_BARNEY_MODEL ) );
}

void CHoe_NPC_Barney::Spawn(void)
{
	BaseClass::Spawn();

	m_iHealth = sk_barney_health.GetFloat();

	AddSpawnFlags( SF_HOE_HUMAN_COMPANION_FOLLOW );

	CapabilitiesRemove( bits_CAP_FRIENDLY_DMG_IMMUNE );
}