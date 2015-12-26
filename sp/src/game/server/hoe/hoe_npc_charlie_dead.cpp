//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hoe_prop_corpse_grunt.h"
#include "vstdlib/random.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CHoe_NPC_Charlie_Dead : public CHoe_Prop_Corpse_Grunt
{
	DECLARE_CLASS(CHoe_NPC_Charlie_Dead, CHoe_Prop_Corpse_Grunt);
public:
	void		SelectModel(void);
};

LINK_ENTITY_TO_CLASS(npc_charlie_dead, CHoe_NPC_Charlie_Dead);

void CHoe_NPC_Charlie_Dead::SelectModel(void)
{
	char* szModel = (char*)STRING(GetModelName());
	if (!szModel || !*szModel)
	{
		szModel = "models/charlie/charlie.mdl";
		SetModelName(AllocPooledString(szModel));
	}

	// Avoid crash.
	m_nBody = 0;
	m_nSkin = 0;
}