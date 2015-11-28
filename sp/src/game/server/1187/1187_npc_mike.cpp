//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MIKE_MODEL		"models/humans/group01/male_09.mdl"

extern ConVar g_johnhealth;

class CNPC_Mike : public CNPC_Citizen
{
	DECLARE_CLASS(CNPC_Mike, CNPC_Citizen);
public:
	virtual void SelectModel(void);

	virtual void Spawn(void);
};

LINK_ENTITY_TO_CLASS(npc_mike, CNPC_Mike);

void CNPC_Mike::SelectModel()
{
	SetModelName(AllocPooledString(MIKE_MODEL));
}

void CNPC_Mike::Spawn(void)
{
	BaseClass::Spawn();

	m_iHealth = m_iMaxHealth = g_johnhealth.GetInt();
}