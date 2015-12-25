//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HOE_PLAYER_SHARED_H
#define HOE_PLAYER_SHARED_H
#ifdef _WIN32
#pragma once
#endif

// Shared header file for players
#if defined( CLIENT_DLL )
#define CHoe_Player C_Hoe_Player	//FIXME: Lovely naming job between server and client here...
#include "c_hoe_player.h"
#else
#include "hoe_player.h"
#endif

#endif // HOE_PLAYER_SHARED_H
