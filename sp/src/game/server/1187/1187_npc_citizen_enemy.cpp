//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CNPC_Citizen_Enemy : public CNPC_Citizen
{
	DECLARE_CLASS(CNPC_Citizen_Enemy, CNPC_Citizen);
public:

	Class_T Classify() { return CLASS_COMBINE; }

	virtual bool 	IsMedic() 			{ return false; }
	virtual bool 	IsAmmoResupplier() 	{ return false; }

	virtual bool 	CanHeal()			{ return false; }
};

LINK_ENTITY_TO_CLASS(npc_citizen, CNPC_Citizen_Enemy);