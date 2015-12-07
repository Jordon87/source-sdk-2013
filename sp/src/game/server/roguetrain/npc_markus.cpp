//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npc_citizen17.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MARKUS_MODEL	"models/monk.mdl"

class CNPC_Markus : public CNPC_Citizen
{
public:
	Class_T 		Classify();

	virtual void	SelectModel();
};

LINK_ENTITY_TO_CLASS(npc_markus, CNPC_Markus);

Class_T CNPC_Markus::Classify()
{
	return CLASS_NONE;
}

void CNPC_Markus::SelectModel()
{
	SetModelName(AllocPooledString(MARKUS_MODEL));
}