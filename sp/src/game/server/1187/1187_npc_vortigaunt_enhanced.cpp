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

	virtual void Spawn(void);

	virtual  Class_T Classify();

	virtual bool	IsPlayerAlly(void) { return false; }
};

LINK_ENTITY_TO_CLASS(npc_vortigaunt_enhanced, CNPC_Vortigaunt);

void CNPC_Vortigaunt_Enhanced::Spawn(void)
{
	// Allow multiple models (for slaves), but default to vortigaunt.mdl
	char *szModel = (char *)STRING(GetModelName());
	if (!szModel || !*szModel)
	{
		szModel = "models/vortigaunt_enchanced.mdl";
		SetModelName(AllocPooledString(szModel));
	}

	BaseClass::Spawn();
}

Class_T CNPC_Vortigaunt_Enhanced::Classify()
{
	return CLASS_VORTIGAUNT_ENHANCED;
}