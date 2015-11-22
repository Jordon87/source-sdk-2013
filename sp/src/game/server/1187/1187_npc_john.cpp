//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define JOHH_MODEL		"models/humans/group01/male_04.mdl"

ConVar	g_johnhealth("g_johnhealth", "0");
ConVar	g_johnfallhealth("g_johnfallhealth", "0");

class CNPC_John : public CNPC_Citizen
{
	DECLARE_CLASS(CNPC_John, CNPC_Citizen);
public:

	virtual void SelectModel(void);

	void Spawn(void);
};

LINK_ENTITY_TO_CLASS(npc_john, CNPC_John);

void CNPC_John::SelectModel()
{
	SetModelName(AllocPooledString(JOHH_MODEL));
}

void CNPC_John::Spawn()
{
	RemoveSpawnFlags( SF_CITIZEN_RANDOM_HEAD | SF_CITIZEN_RANDOM_HEAD_MALE | SF_CITIZEN_RANDOM_HEAD_FEMALE );

	BaseClass::Spawn();

	m_iHealth = m_iMaxHealth = g_johnhealth.GetInt();

	SetModel(JOHH_MODEL);
}