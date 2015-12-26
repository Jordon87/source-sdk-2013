//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hoe_prop_corpse.h"
#include "vstdlib/random.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC(CHoe_Prop_Corpse)
	DEFINE_FIELD(m_iPose, FIELD_INTEGER),
	DEFINE_THINKFUNC(CorpseThink),
END_DATADESC()

bool CHoe_Prop_Corpse::KeyValue(const char *szKeyName, const char *szValue)
{
	if (FStrEq(szKeyName, "pose"))
	{
		m_iPose = atoi(szValue);
		return(true);
	}

	return(BaseClass::KeyValue(szKeyName, szValue));
}

void CHoe_Prop_Corpse::Spawn(void)
{
	SelectModel();

	Precache();

	SetModel(STRING(GetModelName()));
	SetCollisionBoundsFromModel();

	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_SOLID | FSOLID_NOT_STANDABLE | FSOLID_CUSTOMRAYTEST | FSOLID_CUSTOMBOXTEST);
	AddEffects(EF_NOSHADOW);

	InitCorpse();
}

void CHoe_Prop_Corpse::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel(STRING(GetModelName()));
}

void CHoe_Prop_Corpse::CorpseThink(void)
{
	StudioFrameAdvance();
	DispatchAnimEvents(this);

	SetNextThink(gpGlobals->curtime + 0.1f);
}

void CHoe_Prop_Corpse::InitCorpse()
{
	Assert(m_iPose >= 0 && m_iPose < GetPoseCount());

	int seq = LookupSequence(GetPose(m_iPose));

	if (seq < 0)
	{
		DevMsg("error: %s has invalid pose sequence.\n", GetClassname());
		UTIL_Remove(this);
		return;
	}

	ResetSequence(seq);
	SetThink(&CHoe_Prop_Corpse::CorpseThink);
	SetNextThink(gpGlobals->curtime + 0.1f);
}