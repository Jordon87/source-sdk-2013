//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "1187_npc_basezombie_headless.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void C1187_NPC_BaseZombie_Headless::Spawn(void)
{
	BaseClass::Spawn();

	m_fIsHeadless = true;
}

HeadcrabRelease_t C1187_NPC_BaseZombie_Headless::ShouldReleaseHeadcrab(const CTakeDamageInfo &info, float flDamageThreshold)
{
	return RELEASE_NO;
}