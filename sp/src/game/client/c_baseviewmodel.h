//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Client side view model implementation. Responsible for drawing
//			the view model.
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_BASEVIEWMODEL_H
#define C_BASEVIEWMODEL_H
#ifdef _WIN32
#pragma once
#endif

#if defined ( ELEVENEIGHTYSEVEN_DLL ) || defined ( ELEVENEIGHTYSEVEN_CLIENT_DLL )
#include "c_1187_baseviewmodel.h"
#else
#include "c_baseanimating.h"
#include "utlvector.h"
#include "baseviewmodel_shared.h"
#endif

#endif // C_BASEVIEWMODEL_H
