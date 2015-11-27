//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef TRIAGE_PLAYER_SHARED_H
#define TRIAGE_PLAYER_SHARED_H
#ifdef _WIN32
#pragma once
#endif

// Shared header file for players
#if defined( CLIENT_DLL )
#define CTriage_Player C_Triage_Player	//FIXME: Lovely naming job between server and client here...
#include "c_triage_player.h"
#else
#include "triage_player.h"
#endif

#endif // TRIAGE_PLAYER_SHARED_H
