//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_triage_playerlocaldata.h"
#include "dt_recv.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_RECV_TABLE_NOBASE( C_TriagePlayerLocalData, DT_TriageLocal )
	RecvPropBool(RECVINFO(m_bBumpWeapon)),
	RecvPropEHandle(RECVINFO(m_hBumpWeapon)),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA_NO_BASE( C_TriagePlayerLocalData )
END_PREDICTION_DATA()

C_TriagePlayerLocalData::C_TriagePlayerLocalData()
{
	m_bBumpWeapon = false;
	m_hBumpWeapon = NULL;
}

