//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hoe_playerlocaldata.h"
#include "hoe_player.h"
#include "mathlib/mathlib.h"
#include "entitylist.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_SEND_TABLE_NOBASE(CHoePlayerLocalData, DT_HoeLocal)
END_SEND_TABLE()

BEGIN_SIMPLE_DATADESC(CHoePlayerLocalData)
END_DATADESC()

CHoePlayerLocalData::CHoePlayerLocalData()
{
}

