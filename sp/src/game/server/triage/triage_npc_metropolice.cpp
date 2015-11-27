

#include "cbase.h"
#include "ai_basenpc.h"
#include "npc_metropolice.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


#define POLICE_DEFAULT_MODEL	"models/police.mdl"
#define POLICE_REBEL_MODEL		"models/policerebelanims.mdl"

class CTriage_NPC_Metropolice : public CNPC_MetroPolice
{
	DECLARE_CLASS(CTriage_NPC_Metropolice, CNPC_MetroPolice);

public:
	virtual void Precache(void);

};

LINK_ENTITY_TO_CLASS(npc_metropolice, CTriage_NPC_Metropolice);

void CTriage_NPC_Metropolice::Precache(void)
{
	// Call base class to precache default models.
	BaseClass::Precache();

	if (m_spawnEquipment != NULL_STRING)
	{
		// Since Triage features metropolice carrying certain
		// weapons that are not entirely supported by default
		// metropolice models, switch to a proper model.
		static const char* equipment[] =
		{
			"weapon_shotgun",
			"weapon_smg1",
			"weapon_smg1_s",
		};

		for (size_t i = 0; i < ARRAYSIZE(equipment); i++)
		{
			// Check if equipment classname matches with one
			// of the restricted weapons.
			if (FStrEq(STRING(m_spawnEquipment), equipment[i]))
			{
				// Set new model name.
				SetModelName(AllocPooledString(POLICE_REBEL_MODEL));

				// Precache new model.
				PrecacheModel(POLICE_REBEL_MODEL);
			}
		}
	}

	// Fix:
	// Only set the default metropolice model for a particular cop.
	if (FStrEq(GetEntityName().ToCStr(), "copfight"))
	{
		SetModelName(AllocPooledString(POLICE_DEFAULT_MODEL));
	}
}