//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "hoe_gamestats.h"
#include "achievementmgr.h"

static CHoeGameStats s_HoeGameStats;

CHoeGameStats::CHoeGameStats(void)
{
	gamestats = &s_HoeGameStats;
}
