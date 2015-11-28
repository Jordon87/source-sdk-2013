//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define CHRIS_MODEL		"models/humans/group03/male_02.mdl"

extern ConVar sk_citizen_health;
extern ConVar g_johnhealth;

class CNPC_Chris : public CNPC_Citizen
{
	DECLARE_CLASS(CNPC_Chris, CNPC_Citizen);
public:

	virtual void Spawn(void);

	virtual void SelectModel();
};

LINK_ENTITY_TO_CLASS(npc_chris, CNPC_Chris);

void CNPC_Chris::SelectModel()
{
	SetModelName(AllocPooledString(CHRIS_MODEL));
}

void CNPC_Chris::Spawn()
{
	BaseClass::Spawn();

	m_iHealth = m_iMaxHealth = g_johnhealth.GetInt();
}