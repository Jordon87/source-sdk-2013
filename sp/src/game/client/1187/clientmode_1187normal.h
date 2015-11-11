//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( CLIENTMODE_1187NORMAL_H )
#define CLIENTMODE_1187NORMAL_H
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
class ClientMode1187Normal : public ClientModeShared
{
public:
	DECLARE_CLASS(ClientMode1187Normal, ClientModeShared);

	ClientMode1187Normal();
	~ClientMode1187Normal();

	virtual void	Init();
	virtual bool	ShouldDrawCrosshair( void );
};

extern IClientMode *GetClientModeNormal();
extern vgui::HScheme g_hVGuiCombineScheme;

#endif // CLIENTMODE_1187NORMAL_H
