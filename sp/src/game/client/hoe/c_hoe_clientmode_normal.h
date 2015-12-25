//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( C_HOE_CLIENTMODE_NORMAL_H )
#define C_HOE_CLIENTMODE_NORMAL_H
#ifdef _WIN32
#pragma once
#endif

#include "clientmode_shared.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui/Cursor.h>

class CHudViewport;

namespace vgui
{
	typedef unsigned long HScheme;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHoeClientModeNormal : public ClientModeShared
{
public:
	DECLARE_CLASS(CHoeClientModeNormal, ClientModeShared);

	CHoeClientModeNormal();
	~CHoeClientModeNormal();

	virtual void	Init();
	virtual bool	ShouldDrawCrosshair( void );
};

extern IClientMode *GetClientModeNormal();
extern vgui::HScheme g_hVGuiCombineScheme;

#endif // C_HOE_CLIENTMODE_NORMAL_H
