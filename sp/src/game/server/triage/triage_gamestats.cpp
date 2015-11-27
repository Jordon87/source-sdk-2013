//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "triage_gamestats.h"
#include "achievementmgr.h"

static CTriageGameStats s_TriageGameStats;

CTriageGameStats::CTriageGameStats( void )
{
	gamestats = &s_TriageGameStats;
}
