//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_VMAINMENU_H
#define ELEVENEIGHTYSEVEN_VMAINMENU_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Controls.h>
#include <vgui_controls/Frame.h>

class C1187MainMenu : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(C1187MainMenu, vgui::Frame);

public:
	C1187MainMenu(vgui::VPANEL parent);
	~C1187MainMenu();
public:


protected:

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

};




#endif // ELEVENEIGHTYSEVEN_VMAINMENU_H