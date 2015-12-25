//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_hoe_playerlocaldata.h"
#include "dt_recv.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_RECV_TABLE_NOBASE( C_HoePlayerLocalData, DT_HoeLocal )
END_RECV_TABLE()

BEGIN_PREDICTION_DATA_NO_BASE( C_HoePlayerLocalData )
END_PREDICTION_DATA()

C_HoePlayerLocalData::C_HoePlayerLocalData()
{
}

