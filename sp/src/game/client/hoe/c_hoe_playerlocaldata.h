//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//

#if !defined( C_HOE_PLAYERLOCALDATA_H )
#define C_HOE_PLAYERLOCALDATA_H
#ifdef _WIN32
#pragma once
#endif


#include "dt_recv.h"

#include "hoe/hoe_movedata.h"

EXTERN_RECV_TABLE( DT_HoeLocal );


class C_HoePlayerLocalData
{
public:
	DECLARE_PREDICTABLE();
	DECLARE_CLASS_NOBASE(C_HoePlayerLocalData);
	DECLARE_EMBEDDED_NETWORKVAR();

	C_HoePlayerLocalData();
};


#endif // C_HOE_PLAYERLOCALDATA_H
