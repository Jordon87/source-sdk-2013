//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hoe_npc_BaseHuman_companion.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define NPC_MIKEFORCE_MEDIC_MODEL	"models/armymedic/armymedic.mdl"

extern ConVar sk_hoe_human_health;

class CHoe_NPC_MikeForce_Medic : public CHoe_NPC_BaseHuman_Companion
{
	DECLARE_CLASS(CHoe_NPC_MikeForce_Medic, CHoe_NPC_BaseHuman_Companion);
public:
	DECLARE_DATADESC();

	void SelectModel();
	void Spawn(void);

	//---------------------------------

	Class_T 		Classify() { return CLASS_PLAYER_ALLY; }

	//---------------------------------
	// Special abilities
	//---------------------------------
	virtual bool 	IsMedic() 			{ return true; }
};

LINK_ENTITY_TO_CLASS(npc_mikeforce_medic, CHoe_NPC_MikeForce_Medic);

BEGIN_DATADESC(CHoe_NPC_MikeForce_Medic)
DEFINE_USEFUNC(CommanderUse),
DEFINE_USEFUNC(SimpleUse),
END_DATADESC()

void CHoe_NPC_MikeForce_Medic::SelectModel()
{
	SetModelName(AllocPooledString(NPC_MIKEFORCE_MEDIC_MODEL));
}

void CHoe_NPC_MikeForce_Medic::Spawn(void)
{
	m_nBody = 0;
	m_nSkin = 0;

	BaseClass::Spawn();

	m_iHealth = sk_hoe_human_health.GetFloat();

	AddSpawnFlags(SF_HOE_HUMAN_COMPANION_FOLLOW);
	AddSpawnFlags(SF_HOE_HUMAN_COMPANION_MEDIC);

	CapabilitiesRemove(bits_CAP_FRIENDLY_DMG_IMMUNE);
}