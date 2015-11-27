

#include "cbase.h"
#include "ai_basenpc.h"
#include "npc_crow.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Duck. Crow with a different model.
//-----------------------------------------------------------------------------
class CTriage_NPC_Duck : public CNPC_Crow
{
	DECLARE_CLASS(CTriage_NPC_Duck, CNPC_Crow);

public:
	void Precache(void)
	{
		BaseClass::Precache();

		PrecacheModel("models/duck.mdl");
	}

	void Spawn(void)
	{
		SetModelName(AllocPooledString("models/duck.mdl"));
		BaseClass::Spawn();

		m_iBirdType = BIRDTYPE_PIGEON;
	}

	void IdleSound(void)
	{
		EmitSound("NPC_Pigeon.Idle");
	}
};

LINK_ENTITY_TO_CLASS(npc_duck, CTriage_NPC_Duck);