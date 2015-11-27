//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TRIAGE_PLAYERLOCALDATA_H
#define TRIAGE_PLAYERLOCALDATA_H
#ifdef _WIN32
#pragma once
#endif

#include "networkvar.h"
#include "triage_movedata.h"

//-----------------------------------------------------------------------------
// Purpose: Player specific data for Triage ( sent only to local player, too )
//-----------------------------------------------------------------------------
class CTriagePlayerLocalData
{
public:
	// Save/restore
	DECLARE_SIMPLE_DATADESC();
	DECLARE_CLASS_NOBASE( CTriagePlayerLocalData );
	DECLARE_EMBEDDED_NETWORKVAR();

	CTriagePlayerLocalData();

	CNetworkVar(bool, m_bBumpWeapon);
	CNetworkVar(EHANDLE, m_hBumpWeapon);
};

EXTERN_SEND_TABLE(DT_TriageLocal);


#endif // TRIAGE_PLAYERLOCALDATA_H
