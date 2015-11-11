//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_PLAYERLOCALDATA_H
#define ELEVENEIGHTYSEVEN_PLAYERLOCALDATA_H
#ifdef _WIN32
#pragma once
#endif

#include "networkvar.h"

#include "1187_movedata.h"

//-----------------------------------------------------------------------------
// Purpose: Player specific data for 1187 ( sent only to local player, too )
//-----------------------------------------------------------------------------
class C1187PlayerLocalData
{
public:
	// Save/restore
	DECLARE_SIMPLE_DATADESC();
	DECLARE_CLASS_NOBASE(C1187PlayerLocalData);
	DECLARE_EMBEDDED_NETWORKVAR();

	C1187PlayerLocalData();
};

EXTERN_SEND_TABLE(DT_1187Local);


#endif // ELEVENEIGHTYSEVEN_PLAYERLOCALDATA_H
