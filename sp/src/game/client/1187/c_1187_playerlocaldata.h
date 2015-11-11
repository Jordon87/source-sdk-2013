//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//

#if !defined( C_1187_PLAYERLOCALDATA_H )
#define C_1187_PLAYERLOCALDATA_H
#ifdef _WIN32
#pragma once
#endif

#include "dt_recv.h"
#include "1187_movedata.h"

EXTERN_RECV_TABLE( DT_1187Local );


class C_1187PlayerLocalData
{
public:
	DECLARE_PREDICTABLE();
	DECLARE_CLASS_NOBASE(C_1187PlayerLocalData);
	DECLARE_EMBEDDED_NETWORKVAR();

	C_1187PlayerLocalData();
};


#endif // C_1187_PLAYERLOCALDATA_H
