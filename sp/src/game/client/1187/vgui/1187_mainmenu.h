

#ifndef ELEVENEIGHTYSEVEN_MAINMENU_INTERFACE_H
#define ELEVENEIGHTYSEVEN_MAINMENU_INTERFACE_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/IVGUI.h>

class I1187MainMenu
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
};

extern I1187MainMenu* g_p1187mainmenu;

#endif // ELEVENEIGHTYSEVEN_MAINMENU_INTERFACE_H