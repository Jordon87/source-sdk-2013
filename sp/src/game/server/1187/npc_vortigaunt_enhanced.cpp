//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_vortigaunt_episodic.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sk_vortigaunt_enhanced_dmg_zap("sk_vortigaunt_enhanced_dmg_zap", "0");

class CNPC_Vortigaunt_Enhanced : public CNPC_Vortigaunt
{
	DECLARE_CLASS(CNPC_Vortigaunt_Enhanced, CNPC_Vortigaunt);
public:

};

LINK_ENTITY_TO_CLASS(CNPC_Vortigaunt_Enhanced, CNPC_Vortigaunt);