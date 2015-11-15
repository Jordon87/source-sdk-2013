//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar	g_johnhealth("g_johnhealth", "0");
ConVar	g_johnfallhealth("g_johnfallhealth", "0");

class CNPC_John : public CNPC_Citizen
{
	DECLARE_CLASS(CNPC_John, CNPC_Citizen);
public:

	void Spawn(void);
};

LINK_ENTITY_TO_CLASS(npc_john, CNPC_John);

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

void CNPC_John::Spawn()
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