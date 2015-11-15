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
	SetModelName( AllocPooledString( CHRIS_MODEL ) );
}

static const char* g_pszDuplicateMapNames[] =
{
	"1187d1",
	"1187d2",
	"1187d3",
	"1187d4",
	"1187d5",
	"1187d6",
	"1187d7",
	"1187d8",
	"1187d9",
	"1187d10",
};

void CNPC_Chris::Spawn()
{
	BaseClass::Spawn();

	m_iHealth = m_iMaxHealth = g_johnhealth.GetInt();

	// Fix a bug where multiple instances of npc_john would
	// spawn. Remove those unnamed.
	if (GetEntityName() == NULL_STRING)
	{
		for (size_t i = 0; i < ARRAYSIZE(g_pszDuplicateMapNames); i++)
		{
			if (FStrEq(STRING(gpGlobals->mapname), g_pszDuplicateMapNames[i]))
			{
				UTIL_Remove(this);
				break;
			}
		}
	}
}