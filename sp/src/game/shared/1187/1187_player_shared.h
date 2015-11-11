//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_PLAYER_SHARED_H
#define ELEVENEIGHTYSEVEN_PLAYER_SHARED_H
#ifdef _WIN32
#pragma once
#endif

// Shared header file for players
#if defined( CLIENT_DLL )
#define C1187_Player C_1187_Player	//FIXME: Lovely naming job between server and client here...
#include "c_1187_player.h"
#else
#include "1187_player.h"
#endif

#endif // ELEVENEIGHTYSEVEN_PLAYER_SHARED_H
