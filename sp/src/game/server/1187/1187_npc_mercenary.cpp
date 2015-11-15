//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar	sk_mercenary_health("sk_mercenary_health", "0");
ConVar	sk_mercenary_kick("sk_mercenary_kick", "0");

ConVar	sk_mercenary_elite_health("sk_mercenary_elite_health", "0");
ConVar	sk_mercenary_elite_kick("sk_mercenary_elite_kick", "0");

//=========================================================
//	>> CNPC_Mercenary
//=========================================================
class CNPC_Mercenary : public CNPC_Citizen
{
	DECLARE_CLASS(CNPC_Mercenary, CNPC_Citizen);
public:

};

LINK_ENTITY_TO_CLASS(npc_mercenary, CNPC_Mercenary);