//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_GAMESETTINGS_INTERFACE_H
#define ELEVENEIGHTYSEVEN_GAMESETTINGS_INTERFACE_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/IVGUI.h>

class I1187GameSettings
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;

	virtual void		Init(void) = 0;
	virtual void		Shutdown(void) = 0;

	virtual void		Apply( void* settings ) = 0;

	virtual void		SetVisible(bool visible) = 0;

	virtual void		SetEasterEggs(int count) = 0;
};

extern I1187GameSettings* g_p1187gamesettings;

#endif // ELEVENEIGHTYSEVEN_GAMESETTINGS_INTERFACE_H