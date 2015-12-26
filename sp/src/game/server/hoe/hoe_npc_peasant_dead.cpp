//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hoe_prop_corpse_scientist.h"
#include "hoe_npc_peasant_defs.h"
#include "vstdlib/random.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CHoe_NPC_Peasant_Dead : public CHoe_Prop_Corpse_Scientist
{
	DECLARE_CLASS(CHoe_NPC_Peasant_Dead, CHoe_Prop_Corpse_Scientist);
public:
	void SelectModel(void);

private:

	static const char* m_pszPeasantModels[];
};

LINK_ENTITY_TO_CLASS(npc_peasant_dead, CHoe_NPC_Peasant_Dead);

const char* CHoe_NPC_Peasant_Dead::m_pszPeasantModels[] =
{
	NPC_PEASANT_MALE1_MODEL,
	NPC_PEASANT_MALE2_MODEL,
	NPC_PEASANT_MALE3_MODEL,
	NPC_PEASANT_FEMALE1_MODEL,
	NPC_PEASANT_FEMALE2_MODEL,
	NPC_PEASANT_FEMALE3_MODEL,
};

void CHoe_NPC_Peasant_Dead::SelectModel(void)
{
	char *szModel = NULL;

	// Is this a random model?
	if (m_nBody < PEASANT_BODY_MALE1)
	{
		// Is this a random male model?
		if (m_nBody < PEASANT_BODY_RANDOM)
		{
			// Is this a random female model?
			if (m_nBody < PEASANT_BODY_RANDOM_MALE)
			{
				// Select a random female model.
				szModel = (char*)m_pszPeasantModels[random->RandomInt(PEASANT_BODY_FEMALE1, PEASANT_BODY_FEMALE3)];
			}
			else
			{
				// Select a random male model.
				szModel = (char*)m_pszPeasantModels[random->RandomInt(PEASANT_BODY_MALE1, PEASANT_BODY_MALE3)];
			}
		}
		else
		{
			// Select a random model.
			szModel = (char*)m_pszPeasantModels[random->RandomInt(0, PEASANT_BODY_COUNT - 1)];
		}
	}
	else
	{
		// This is a pre-selected model, set it correctly.
		szModel = (char*)m_pszPeasantModels[m_nBody];
	}

	// If no valid model, set default one.
	if (!szModel || !*szModel)
		szModel = NPC_PEASANT_MALE1_MODEL;

	Assert(szModel);

	SetModelName(AllocPooledString(szModel));

	// Avoid crash.
	m_nBody = 0;
	m_nSkin = 0;
}