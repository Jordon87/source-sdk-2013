//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_1187_playerlocaldata.h"
#include "dt_recv.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_RECV_TABLE_NOBASE( C_1187PlayerLocalData, DT_1187Local )
	RecvPropBool(RECVINFO(m_bWeaponLoweredOnSprint)),
	RecvPropBool(RECVINFO(m_bAdjacentToWall)),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA_NO_BASE(C_1187PlayerLocalData)
END_PREDICTION_DATA()

C_1187PlayerLocalData::C_1187PlayerLocalData()
{
	m_bWeaponLoweredOnSprint = false;
	m_bAdjacentToWall = false;
}

