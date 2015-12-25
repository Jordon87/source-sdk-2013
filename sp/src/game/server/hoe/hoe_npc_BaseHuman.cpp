//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hoe_npc_BaseHuman.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar	sk_hoe_human_health("sk_hoe_human_health", "40");

BEGIN_DATADESC(CHoe_NPC_BaseHuman)
	DEFINE_INPUTFUNC(FIELD_VOID, "Behead", InputBehead),
END_DATADESC()

void CHoe_NPC_BaseHuman::Behead(void)
{

}

void CHoe_NPC_BaseHuman::InputBehead(inputdata_t& inputdata)
{
	Behead();
}