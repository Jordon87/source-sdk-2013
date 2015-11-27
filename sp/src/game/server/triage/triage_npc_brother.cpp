//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define BROTHER_MODEL			"models/humans/brother.mdl"

class CTriage_NPC_Brother : public CNPC_Citizen
{
	DECLARE_CLASS(CTriage_NPC_Brother, CNPC_Citizen);
public:
	virtual void SelectModel(void);
};

LINK_ENTITY_TO_CLASS(npc_brother, CTriage_NPC_Brother);

void CTriage_NPC_Brother::SelectModel(void)
{
	char* szModel = (char*)STRING(GetModelName());
	if (!szModel || !*szModel)
	{
		SetModelName(AllocPooledString(BROTHER_MODEL));
	}
}