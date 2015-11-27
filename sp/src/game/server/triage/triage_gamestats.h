//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#ifndef TRIAGE_GAMESTATS_H
#define TRIAGE_GAMESTATS_H
#ifdef _WIN32
#pragma once
#endif

#include "gamestats.h"

class CTriageGameStats : public CBaseGameStats
{
	typedef CBaseGameStats BaseClass;

public:
	CTriageGameStats( void );
};

#endif // TRIAGE_GAMESTATS_H
