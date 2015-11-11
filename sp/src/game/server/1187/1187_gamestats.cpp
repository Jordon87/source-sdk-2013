//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "1187_gamestats.h"
#include "achievementmgr.h"

static C1187GameStats s_1187GameStats;

C1187GameStats::C1187GameStats( void )
{
	gamestats = &s_1187GameStats;
}
