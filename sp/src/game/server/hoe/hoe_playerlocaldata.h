//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef HOE_PLAYERLOCALDATA_H
#define HOE_PLAYERLOCALDATA_H
#ifdef _WIN32
#pragma once
#endif

#include "networkvar.h"

#include "hl_movedata.h"

//-----------------------------------------------------------------------------
// Purpose: Player specific data for Hoe ( sent only to local player, too )
//-----------------------------------------------------------------------------
class CHoePlayerLocalData
{
public:
	// Save/restore
	DECLARE_SIMPLE_DATADESC();
	DECLARE_CLASS_NOBASE( CHoePlayerLocalData );
	DECLARE_EMBEDDED_NETWORKVAR();

	CHoePlayerLocalData();
};

EXTERN_SEND_TABLE(DT_HoeLocal);


#endif // HOE_PLAYERLOCALDATA_H
