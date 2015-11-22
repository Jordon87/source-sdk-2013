//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//-----------------------------------------------------------------------------
// Purpose: The default response system for expressive AIs
//-----------------------------------------------------------------------------
class C1187GameSettingsSystem : public CAutoGameSystem
{
	typedef CAutoGameSystem BaseClass;

public:

	C1187GameSettingsSystem() : CAutoGameSystem("C1187GameSettingsSystem")
	{
		m_bInitialized = false;
	}

	virtual void LevelInitPostEntity()
	{
		if (gpGlobals->eLoadType != MapLoad_NewGame)
			return;

		engine->ServerCommand("sv_1187_reseteastereggs");
	}

	virtual void Release()
	{
		Assert(0);
	}

	virtual void LevelInitPreEntity()
	{
	}

private:

	bool m_bInitialized;
};

C1187GameSettingsSystem g_1187GameSettingsSystem;

//-----------------------------------------------------------------------------
// Purpose: Quickly switch to the physics cannon, or back to previous item
//-----------------------------------------------------------------------------
void CC_SV_1187_ResetEasterEggs(void)
{
	// Reset easter eggs.
	CElevenEightySeven* pGameRules = dynamic_cast<CElevenEightySeven*>(g_pGameRules);
	if (pGameRules)
	{
		pGameRules->SetEasterEggs(0);
	}
}
static ConCommand sv_1187_reseteastereggs("sv_1187_reseteastereggs", CC_SV_1187_ResetEasterEggs, "Reset 1187 easter eggs count.");