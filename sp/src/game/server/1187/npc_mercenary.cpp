//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_combines.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define NPC_MERCENARY_MODEL		"models/mercenary/merc_01.mdl"

ConVar	sk_mercenary_health("sk_mercenary_health", "0");
ConVar	sk_mercenary_kick("sk_mercenary_kick", "0");

ConVar	sk_mercenary_elite_health("sk_mercenary_elite_health", "0");
ConVar	sk_mercenary_elite_kick("sk_mercenary_elite_kick", "0");

//=========================================================
//	>> CNPC_Mercenary
//=========================================================
class CNPC_Mercenary : public CNPC_Combine
{
	DECLARE_CLASS(CNPC_Mercenary, CNPC_Combine);
public:
	void		Spawn(void);

};

LINK_ENTITY_TO_CLASS(npc_mercenary, CNPC_Mercenary);

void CNPC_Mercenary::Spawn(void)
{
	char* szModel = (char*)STRING( GetModelName() );

	if (!szModel || !*szModel)
	{
		szModel = NPC_MERCENARY_MODEL;
		SetModelName(AllocPooledString(szModel));
	}

	BaseClass::Spawn();

	if (IsElite())
	{
		// Stronger, tougher.
		SetHealth(sk_mercenary_elite_health.GetFloat());
		SetMaxHealth(sk_mercenary_elite_health.GetFloat());
		SetKickDamage(sk_mercenary_elite_kick.GetFloat());
	}
	else
	{
		SetHealth(sk_mercenary_health.GetFloat());
		SetMaxHealth(sk_mercenary_health.GetFloat());
		SetKickDamage(sk_mercenary_kick.GetFloat());
	}
}