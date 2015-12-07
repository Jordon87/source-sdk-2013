//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"
#if defined ( ROGUETRAIN_DLL )
#include "1187_gamerules.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if defined ( ROGUETRAIN_DLL )
#define ROGUETRAIN_CITIZEN_MODEL	"models/mercenary/merc_heavy.mdl"
#endif

class CNPC_Citizen_Enemy : public CNPC_Citizen
{
	DECLARE_CLASS(CNPC_Citizen_Enemy, CNPC_Citizen);
public:

#if defined ( ROGUETRAIN_DLL )
	virtual void SelectModel(void);
#endif

	Class_T Classify() { return CLASS_COMBINE; }

	virtual bool 	IsMedic() 			{ return false; }
	virtual bool 	IsAmmoResupplier() 	{ return false; }

	virtual bool 	CanHeal()			{ return false; }

};

LINK_ENTITY_TO_CLASS(npc_citizen, CNPC_Citizen_Enemy);

#if defined ( ROGUETRAIN_DLL )
void CNPC_Citizen_Enemy::SelectModel()
{
	CElevenEightySeven* pGamerules = ElevenEightySevenGameRules();
	if (pGamerules && pGamerules->IsRogueTrain())
	{
		// 1187 - Rogue train
		//
		// Different citizen model.
		//
		SetModelName(AllocPooledString( ROGUETRAIN_CITIZEN_MODEL ));
	}
	else
	{
		BaseClass::SelectModel();
	}
}
#endif //defined ( ROGUETRAIN_DLL )