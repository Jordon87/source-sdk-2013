//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_NPC_BASEZOMBIE_HEADLESS_H
#define ELEVENEIGHTYSEVEN_NPC_BASEZOMBIE_HEADLESS_H

#ifdef _WIN32
#pragma once
#endif

#include "npc_basezombie.h"

class C1187_NPC_BaseZombie_Headless : public CNPC_BaseZombie
{
	DECLARE_CLASS(C1187_NPC_BaseZombie_Headless, CNPC_BaseZombie);
public:

	virtual void Spawn(void);

	HeadcrabRelease_t ShouldReleaseHeadcrab(const CTakeDamageInfo &info, float flDamageThreshold);
};

#endif // ELEVENEIGHTYSEVEN_NPC_BASEZOMBIE_HEADLESS_H