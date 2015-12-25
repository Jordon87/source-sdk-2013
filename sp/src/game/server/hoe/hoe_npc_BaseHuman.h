//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef HOE_NPC_BASEHUMAN_H
#define HOE_NPC_BASEHUMAN_H

#ifdef _WIN32
#pragma once
#endif

#include "npc_playercompanion.h"

//-------------------------------------
// Spawnflags
//-------------------------------------

#define SF_HOE_HUMAN_FRIENDLY			( 1 << 16 )
#define SF_HOE_HUMAN_SQUADLEADER		( 1 << 17 )
#define SF_HOE_HUMAN_HANDGRENADES		( 1 << 18 )
#define SF_HOE_HUMAN_PREDISASTER		( 1 << 19 )
#define SF_HOE_HUMAN_SERVERSIDE_RAGDOLL	( 1 << 20 )

class CHoe_NPC_BaseHuman : public CNPC_PlayerCompanion
{
	DECLARE_CLASS(CHoe_NPC_BaseHuman, CNPC_PlayerCompanion);
public:
	DECLARE_DATADESC();

	virtual void Behead(void);

protected:
	void InputBehead(inputdata_t& inputdata);
};


#endif // HOE_NPC_BASEHUMAN_H