//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CNPC_Citizen_Old : public CNPC_Citizen
{
	DECLARE_CLASS(CNPC_Citizen_Old, CNPC_Citizen);
public:
	
};

LINK_ENTITY_TO_CLASS(npc_citizen_old, CNPC_Citizen_Old);