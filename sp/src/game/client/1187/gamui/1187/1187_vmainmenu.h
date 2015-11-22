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

#include "1187_vimagebutton.h"

class C1187MainMenu : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(C1187MainMenu, vgui::Frame);

public:
	C1187MainMenu(vgui::VPANEL parent);
	~C1187MainMenu();
public:

	// command handling
	virtual void OnCommand(const char *command);

protected:
	virtual void OnThink(void);
	virtual void Paint();
	virtual void PaintBackground();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	virtual void OnScreenSizeChanged(int iOldWide, int iOldTall);


	void SetMenuButtonState(int button, bool state);

private:
	void UpdateMenuButtons();

	void ResizeFor(int width, int height);
	void PositionButtons();
	void PositionLogo();

	void ResizeMenuElements(int width, int height);

	void InvalidateButtonsLayout(bool layoutNow = false, bool reloadScheme = false);

private:

	enum
	{
		MAINMENU_RESUMEGAME = 0,
		MAINMENU_RETURNTOMENU,
		MAINMENU_CRASHCOURSE,
		MAINMENU_NEWGAME,
		MAINMENU_LOADGAME,
		MAINMENU_SAVEGAME,
		MAINMENU_COMMENTARY,
		MAINMENU_MANUAL,
		MAINMENU_OPTIONS,
		MAINMENU_SETTINGS,
		MAINMENU_BONUSMAPS,
		MAINMENU_QUIT,
	
		// Add new controls here...

		MAINMENU_LAST,
	};

	C1187ImageButton* m_menuButtons[MAINMENU_LAST];
	vgui::ImagePanel* m_pLogo;

	int m_menu_x;
	int m_menu_y;

	int m_button_width;
	int m_button_height;

	int m_logo_x;
	int m_logo_y;
	int m_logo_width;
	int m_logo_height;
};




#endif // ELEVENEIGHTYSEVEN_VMAINMENU_H