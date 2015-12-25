//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

#ifndef HOE_GAMESTATS_H
#define HOE_GAMESTATS_H
#ifdef _WIN32
#pragma once
#endif

#include "gamestats.h"

class CHoeGameStats : public CBaseGameStats
{
	typedef CBaseGameStats BaseClass;

public:
	CHoeGameStats( void );
};

#endif // HOE_GAMESTATS_H
