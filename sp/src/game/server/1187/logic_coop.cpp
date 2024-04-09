//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "baseentity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CLogicCooop : public CLogicalEntity
{
	DECLARE_CLASS(CLogicCooop, CLogicalEntity);
public:
	DECLARE_DATADESC();

	void Activate(void);

protected:

	// Outputs
	COutputEvent m_OnMapSpawn;

};

LINK_ENTITY_TO_CLASS(logic_coop, CLogicCooop);

//-----------------------------
// Save & Restore
//-----------------------------

BEGIN_DATADESC(CLogicCooop)

	//
	// Outputs
	//
	DEFINE_OUTPUT(m_OnMapSpawn, "OnMapSpawn"),

END_DATADESC()

void CLogicCooop::Activate(void)
{
	m_OnMapSpawn.FireOutput(this, this);
	BaseClass::Activate();
}

