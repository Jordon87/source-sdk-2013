//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "triage_playerlocaldata.h"
#include "triage_player.h"
#include "mathlib/mathlib.h"
#include "entitylist.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_SEND_TABLE_NOBASE( CTriagePlayerLocalData, DT_TriageLocal )
	SendPropBool(SENDINFO(m_bBumpWeapon)),
	SendPropEHandle(SENDINFO(m_hBumpWeapon)),
END_SEND_TABLE()

BEGIN_SIMPLE_DATADESC( CTriagePlayerLocalData )
	DEFINE_FIELD(m_bBumpWeapon, FIELD_BOOLEAN),
	DEFINE_FIELD(m_hBumpWeapon, FIELD_EHANDLE),
END_DATADESC()

CTriagePlayerLocalData::CTriagePlayerLocalData()
{
	m_bBumpWeapon = false;
	m_hBumpWeapon = NULL;
}

