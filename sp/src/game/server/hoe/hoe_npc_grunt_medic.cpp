//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hoe_npc_BaseHuman_companion.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define NPC_GRUNT_MEDIC_MODEL	"models/gruntmedic/gruntmedic.mdl"

extern ConVar sk_hoe_human_health;

class CHoe_NPC_Grunt_Medic : public CHoe_NPC_BaseHuman_Companion
{
	DECLARE_CLASS(CHoe_NPC_Grunt_Medic, CHoe_NPC_BaseHuman_Companion);
public:
	DECLARE_DATADESC();

	void SelectModel();
	void Spawn(void);

	//---------------------------------
	Class_T 		Classify() { return CLASS_COMBINE; }

	//---------------------------------
	// Special abilities
	//---------------------------------
	virtual bool 	IsMedic() 			{ return true; }
};

LINK_ENTITY_TO_CLASS(npc_grunt_medic, CHoe_NPC_Grunt_Medic);

BEGIN_DATADESC(CHoe_NPC_Grunt_Medic)
END_DATADESC()

void CHoe_NPC_Grunt_Medic::SelectModel()
{
	SetModelName(AllocPooledString(NPC_GRUNT_MEDIC_MODEL));
}

void CHoe_NPC_Grunt_Medic::Spawn(void)
{
	m_nBody = 0;
	m_nSkin = 0;

	BaseClass::Spawn();

	m_iHealth = sk_hoe_human_health.GetFloat();

	CapabilitiesRemove(bits_CAP_FRIENDLY_DMG_IMMUNE);

	NPCInit();
}