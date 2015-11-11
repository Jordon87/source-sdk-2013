//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "1187_playerlocaldata.h"
#include "1187_player.h"
#include "mathlib/mathlib.h"
#include "entitylist.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_SEND_TABLE_NOBASE( C1187PlayerLocalData, DT_1187Local )
END_SEND_TABLE()

BEGIN_SIMPLE_DATADESC(C1187PlayerLocalData)
END_DATADESC()

C1187PlayerLocalData::C1187PlayerLocalData()
{
}

