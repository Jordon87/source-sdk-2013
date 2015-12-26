//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "baseanimating.h"
#include "vstdlib/random.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define PROP_PALM_SMALL_MODEL	"models/palm/palm1/palm1.mdl"
#define PROP_PALM_MEDIUM_MODEL	"models/palm/palm2/palm2.mdl"
#define PROP_PALM_LARGE_MODEL	"models/palm/palm3/palm3.mdl"

static const char* g_pszRandomPalmModels[] =
{
	PROP_PALM_SMALL_MODEL,
	PROP_PALM_MEDIUM_MODEL,
	PROP_PALM_LARGE_MODEL,
};

enum
{
	PALM_BODY_RANDOM = -1,
	PALM_BODY_SMALL = 0,
	PALM_BODY_MEDIUM,
	PALM_BODY_LARGE,

	// Add new body Ids...

	PALM_BODY_COUNT, // Last value in enum.
};

class CHoe_Prop_Palm : public CBaseAnimating
{
	DECLARE_CLASS(CHoe_Prop_Palm, CBaseAnimating);
public:
	DECLARE_DATADESC();

	void Spawn(void);
	void Precache(void);
	void SelectModel(void);

	void PalmThink(void);
};

LINK_ENTITY_TO_CLASS(prop_palm, CHoe_Prop_Palm);

BEGIN_DATADESC(CHoe_Prop_Palm)
	DEFINE_THINKFUNC(PalmThink),
END_DATADESC()

void CHoe_Prop_Palm::Spawn(void)
{
	SelectModel();

	Precache();

	SetModel( STRING( GetModelName() ) );
	UTIL_SetSize(this, -Vector(6, 6, 0), Vector(6, 6, 155));

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_CUSTOMRAYTEST | FSOLID_CUSTOMBOXTEST);
	AddEffects(EF_NOSHADOW);

	// Setup for animation
	ResetSequence(LookupSequence("idle1"));
	SetThink(&CHoe_Prop_Palm::PalmThink);
	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CHoe_Prop_Palm::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel( STRING( GetModelName() ) );
}

void CHoe_Prop_Palm::PalmThink(void)
{
	StudioFrameAdvance();
	DispatchAnimEvents(this);

	SetNextThink( gpGlobals->curtime + 0.1f );
}

void CHoe_Prop_Palm::SelectModel(void)
{
	char *szModel = NULL;

	// Is this a random model?
	if (m_nBody < PALM_BODY_SMALL)
	{
		// Select a random model.
		szModel = (char*)g_pszRandomPalmModels[random->RandomInt(0, PALM_BODY_COUNT - 1)];
	}
	else
	{
		// Select a pre-selected model.
		szModel = (char*)g_pszRandomPalmModels[m_nBody];
	}

	// If no proper model, set a default one.
	if (!szModel || !*szModel)
		szModel = PROP_PALM_SMALL_MODEL;

	Assert(szModel);

	// Set new model name.
	SetModelName(AllocPooledString(szModel));

	// Avoid crash.
	m_nBody = 0;
	m_nSkin = 0;
}