//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//

#if !defined( C_TRIAGE_PLAYERLOCALDATA_H )
#define C_TRIAGE_PLAYERLOCALDATA_H
#ifdef _WIN32
#pragma once
#endif


#include "dt_recv.h"

#include "triage/triage_movedata.h"

EXTERN_RECV_TABLE( DT_TriageLocal );


class C_TriagePlayerLocalData
{
public:
	DECLARE_PREDICTABLE();
	DECLARE_CLASS_NOBASE( C_TriagePlayerLocalData );
	DECLARE_EMBEDDED_NETWORKVAR();

	C_TriagePlayerLocalData();

	bool		m_bBumpWeapon;
	EHANDLE		m_hBumpWeapon;
};


#endif // C_TRIAGE_PLAYERLOCALDATA_H