//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include "vgui/IPanel.h"
#include "vgui/IVGui.h"
#include "vgui/ISurface.h"
#include "vgui/IInput.h"
#include "vgui/ILocalize.h"

#include "ienginevgui.h"

#include "1187_vmainmenu.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAINMENU_MANUAL_DISABLED	1

using namespace vgui;

C1187MainMenu::C1187MainMenu(vgui::VPANEL parent)
	: BaseClass(NULL, "1187MainMenu")
{
	SetParent( parent );

	SetAutoDelete(true);

	SetProportional(true);
	SetCloseButtonVisible(false);

	SetMoveable(false);
	SetSizeable(false);
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);

	SetScheme("ClientScheme.res");

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	SetTitle("C1187MainMenuFrame", false);

	MoveToCenterOfScreen();

	LoadControlSettings("Resource/GameMainMenu.res");

	int sw, sh;
	GetHudSize(sw, sh);
	ResizeMenuElements(sw, sh);

	SetPos(0, 0);
	SetBounds(0, 0, sw, sh);
	SetSize(sw, sh);

	m_menuButtons[MAINMENU_RESUMEGAME] = new C1187ImageButton(this, "resumegame_button", "RESUME GAME", this, "1187_mainmenu_resumegame" );
	m_menuButtons[MAINMENU_RESUMEGAME]->SetButtonDefaultTexture("vgui/mainmenu/resumegame_static");
	m_menuButtons[MAINMENU_RESUMEGAME]->SetButtonArmedTexture("vgui/mainmenu/resumegame_over");

	m_menuButtons[MAINMENU_RETURNTOMENU] = new C1187ImageButton(this, "returntomenu_button", "RETURN TO MENU", this, "1187_mainmenu_returntomenu");
	m_menuButtons[MAINMENU_RETURNTOMENU]->SetButtonDefaultTexture("vgui/mainmenu/returntomenu_static");
	m_menuButtons[MAINMENU_RETURNTOMENU]->SetButtonArmedTexture("vgui/mainmenu/returntomenu_over");

	m_menuButtons[MAINMENU_CRASHCOURSE] = new C1187ImageButton(this, "crashcourse_button", "CRASH COURSE", this, "1187_mainmenu_crashcourse");
	m_menuButtons[MAINMENU_CRASHCOURSE]->SetButtonDefaultTexture("vgui/mainmenu/crashcourse_static");
	m_menuButtons[MAINMENU_CRASHCOURSE]->SetButtonArmedTexture("vgui/mainmenu/crashcourse_over");

	m_menuButtons[MAINMENU_NEWGAME] = new C1187ImageButton(this, "newgame_button", "NEW GAME", this, "1187_mainmenu_newgame");
	m_menuButtons[MAINMENU_NEWGAME]->SetButtonDefaultTexture("vgui/mainmenu/newgame_static");
	m_menuButtons[MAINMENU_NEWGAME]->SetButtonArmedTexture("vgui/mainmenu/newgame_over");

	m_menuButtons[MAINMENU_LOADGAME] = new C1187ImageButton(this, "loadgame_button", "LOAD GAME", this, "1187_mainmenu_loadgame");
	m_menuButtons[MAINMENU_LOADGAME]->SetButtonDefaultTexture("vgui/mainmenu/loadgame_static");
	m_menuButtons[MAINMENU_LOADGAME]->SetButtonArmedTexture("vgui/mainmenu/loadgame_over");

	m_menuButtons[MAINMENU_SAVEGAME] = new C1187ImageButton(this, "savegame_button", "SAVE GAME", this, "1187_mainmenu_savegame");
	m_menuButtons[MAINMENU_SAVEGAME]->SetButtonDefaultTexture("vgui/mainmenu/savegame_static");
	m_menuButtons[MAINMENU_SAVEGAME]->SetButtonArmedTexture("vgui/mainmenu/savegame_over");

	m_menuButtons[MAINMENU_COMMENTARY] = new C1187ImageButton(this, "commentary_button", "COMMENTARY", this, "1187_mainmenu_commentary");
	m_menuButtons[MAINMENU_COMMENTARY]->SetButtonDefaultTexture("vgui/mainmenu/commentary_static");
	m_menuButtons[MAINMENU_COMMENTARY]->SetButtonArmedTexture("vgui/mainmenu/commentary_over");

	m_menuButtons[MAINMENU_MANUAL] = new C1187ImageButton(this, "manual_button", "MANUAL", this, "1187_mainmenu_manual");
	m_menuButtons[MAINMENU_MANUAL]->SetButtonDefaultTexture("vgui/mainmenu/manual_static");
	m_menuButtons[MAINMENU_MANUAL]->SetButtonArmedTexture("vgui/mainmenu/manual_over");

	m_menuButtons[MAINMENU_OPTIONS] = new C1187ImageButton(this, "updates_button", "OPTIONS", this, "1187_mainmenu_options");
	m_menuButtons[MAINMENU_OPTIONS]->SetButtonDefaultTexture("vgui/mainmenu/options_static");
	m_menuButtons[MAINMENU_OPTIONS]->SetButtonArmedTexture("vgui/mainmenu/options_over");

	m_menuButtons[MAINMENU_SETTINGS] = new C1187ImageButton(this, "settings_button", "SETTINGS", this, "1187_mainmenu_settings");
	m_menuButtons[MAINMENU_SETTINGS]->SetButtonDefaultTexture("vgui/mainmenu/settings_static");
	m_menuButtons[MAINMENU_SETTINGS]->SetButtonArmedTexture("vgui/mainmenu/settings_over");

	m_menuButtons[MAINMENU_BONUSMAPS] = new C1187ImageButton(this, "bonusmaps_button", "BONUS MAPS", this, "1187_mainmenu_bonusmaps");
	m_menuButtons[MAINMENU_BONUSMAPS]->SetButtonDefaultTexture("vgui/mainmenu/bonusmaps_static");
	m_menuButtons[MAINMENU_BONUSMAPS]->SetButtonArmedTexture("vgui/mainmenu/bonusmaps_over");

	m_menuButtons[MAINMENU_QUIT] = new C1187ImageButton(this, "quit_button", "QUIT", this, "1187_mainmenu_quit");
	m_menuButtons[MAINMENU_QUIT]->SetButtonDefaultTexture("vgui/mainmenu/quit_static");
	m_menuButtons[MAINMENU_QUIT]->SetButtonArmedTexture("vgui/mainmenu/quit_over");

	for (size_t i = 0; i < MAINMENU_LAST; i++)
	{
		if (m_menuButtons[i])
			m_menuButtons[i]->SetSize(512, 32);
	}

	// Disable some buttons.
	SetMenuButtonState(MAINMENU_RESUMEGAME, false);
	SetMenuButtonState(MAINMENU_RETURNTOMENU, false);
	SetMenuButtonState(MAINMENU_SAVEGAME, false);

	if (MAINMENU_MANUAL_DISABLED)
	{
		// Disable game manual.
		SetMenuButtonState(MAINMENU_MANUAL, false);
	}

	// Logo
	

	m_pLogo = new vgui::ImagePanel(this, "background_image");
	if (m_pLogo)
	{
		m_pLogo->SetImage("mainmenu/1187Menu");
		m_pLogo->SetMouseInputEnabled(false);
		m_pLogo->SetKeyBoardInputEnabled(false);
		m_pLogo->SetZPos(-999);
		m_pLogo->SetVisible(true);
		m_pLogo->SetShouldScaleImage(true);
	}
}

C1187MainMenu::~C1187MainMenu()
{
}

// command handling
void C1187MainMenu::OnCommand(const char *command)
{

	if (!Q_stricmp(command, "1187_mainmenu_resumegame"))
	{
		engine->ClientCmd("gamemenucommand ResumeGame");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_returntomenu"))
	{
		engine->ClientCmd("gamemenucommand Disconnect");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_crashcourse"))
	{
		engine->ClientCmd("map 1187_crashcourse");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_newgame"))
	{
		engine->ClientCmd("gamemenucommand OpenNewGameDialog");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_loadgame"))
	{
		engine->ClientCmd("gamemenucommand OpenLoadGameDialog");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_savegame"))
	{
		engine->ClientCmd("gamemenucommand OpenSaveGameDialog");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_commentary"))
	{
		engine->ClientCmd("gamemenucommand OpenLoadSingleplayerCommentaryDialog");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_manual"))
	{
		engine->ClientCmd("show1187manual");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_options"))
	{
		engine->ClientCmd("show1187gamesettings");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_settings"))
	{
		engine->ClientCmd("gamemenucommand OpenOptionsDialog");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_bonusmaps"))
	{
		engine->ClientCmd("gamemenucommand OpenBonusMapsDialog");
	}
	else if (!Q_stricmp(command, "1187_mainmenu_quit"))
	{
		engine->ClientCmd("gamemenucommand Quit");
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void C1187MainMenu::Paint()
{
	// surface()->DrawSetColor(GetFgColor());

	BaseClass::Paint();

	float alpha = 128;

	if (!engine->IsInGame())
		alpha = 255;

	surface()->DrawSetColor(Color(0, 0, 0, alpha));

	int width, height;
	GetHudSize(width, height);
	surface()->DrawFilledRect(0, 0, width, height);
}

void C1187MainMenu::PaintBackground()
{
	BaseClass::PaintBackground();
}

void C1187MainMenu::OnThink()
{
	UpdateMenuButtons();

	BaseClass::OnThink();

	SetPos(0, 0);

	PositionButtons();
	PositionLogo();
}

void C1187MainMenu::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetProportional(true);
	SetCloseButtonVisible(false);

	SetMoveable(false);
	SetSizeable(false);
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
}

void C1187MainMenu::OnScreenSizeChanged(int iOldWide, int iOldTall)
{
	int wide, tall;
	GetHudSize(wide, tall);

	SetSize(wide, tall);
	SetBounds( 0, 0, wide, tall );

	ResizeFor(wide, tall);
}

void C1187MainMenu::SetMenuButtonState(int button, bool state)
{
	Assert(button >= 0 && button < MAINMENU_LAST);

	m_menuButtons[button]->SetVisible(state);
	m_menuButtons[button]->SetEnabled(state);
}

void C1187MainMenu::UpdateMenuButtons()
{
	bool bInGame = engine->IsInGame();

	vgui::Button* pButton = NULL;

	pButton = m_menuButtons[MAINMENU_RESUMEGAME];

	if (pButton)
	{
		pButton->SetVisible(bInGame);
		pButton->SetEnabled(bInGame);
	}

	pButton = m_menuButtons[MAINMENU_RETURNTOMENU];

	if (pButton)
	{
		pButton->SetVisible(bInGame);
		pButton->SetEnabled(bInGame);
	}

	pButton = m_menuButtons[MAINMENU_CRASHCOURSE];

	if (pButton)
	{
		pButton->SetVisible(!bInGame);
		pButton->SetEnabled(!bInGame);
	}

	pButton = m_menuButtons[MAINMENU_SAVEGAME];

	if (pButton)
	{
		pButton->SetVisible(bInGame);
		pButton->SetEnabled(bInGame);
	}

	/*
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
		*/
}

void C1187MainMenu::ResizeFor(int width, int height)
{
	ResizeMenuElements( width, height );

	PositionButtons();
	PositionLogo();

	InvalidateButtonsLayout(true);
	InvalidateLayout(true);
}

void C1187MainMenu::ResizeMenuElements(int width, int height)
{
	bool bInvalidResolution = false;

	int sw, sh;
	GetHudSize( sw, sh );

	m_logo_width = sw;
	m_logo_height = sh;

	//
	// Width adjustments
	//

	switch (width)
	{
	case 640:
	{
		m_button_width = 320;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 72;
	}
	break;

	case 720:
	{
		m_button_width = 360;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 72;
	}
	break;

	case 800:
	{
		m_button_width = 400;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 72;
	}
	break;

	case 1024:
	{
		m_button_width = 512;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 112;
	}
	break;

	case 1152:
	{
		m_button_width = 512;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 124;
	}
	break;

	case 1280:
	{
		m_button_width = 512;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 136;
	}
	break;

	case 1440:
	{
		m_button_width = 512;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 136;
	}
	break;

	case 1600:
	{
		m_button_width = 512;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 168;
	}
	break;

	case 1680:
	{
		m_button_width = 512;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 176;
	}
	break;

	case 1776:
	{
		m_button_width = 512;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 188;
	}
	break;

	case 1920:
	{
		m_button_width = 512;
		m_menu_x = sw / 2 - m_button_width / 2;
		m_logo_x = 208;
	}
	break;

	default:
		bInvalidResolution = true;
		break;
	}

	//
	// Height adjustments
	//

	switch (height)
	{
	case 480:
	{
		m_button_height = 20;
		m_menu_y = 142;
		m_logo_y = 56;
	}
	break;

	case 576:
	{
		m_button_height = 22;
		m_menu_y = 188;
		m_logo_y = 88;
	}
	break;

	case 600:
	{
		m_button_height = 25;
		m_menu_y = 188;
		m_logo_y = 88;
	}
	break;

	case 648:
	{
		m_button_height = 20;
		m_menu_y = 242;
		m_logo_y = 112;
	}
	break;

	case 720:
	{
		m_button_height = 32;
		m_menu_y = 222;
		m_logo_y = 72;
	}
	break;

	case 768:
	{
		m_button_height = 32;
		m_menu_y = 272;
		m_logo_y = 128;
	}
	break;

	case 800:
	{
		m_button_height = 32;
		m_menu_y = 272;
		m_logo_y = 128;
	}
	break;

	case 864:
	{
		m_button_height = 32;
		m_menu_y = 272;
		m_logo_y = 128;
	}
	break;

	case 900:
	{
		m_button_height = 32;
		m_menu_y = 328;
		m_logo_y = 162;
	}
	break;

	case 960:
	{
		m_button_height = 32;
		m_menu_y = 328;
		m_logo_y = 162;
	}
	break;

	case 1000:
	{
		m_button_height = 32;
		m_menu_y = 384;
		m_logo_y = 182;
	}
	break;

	case 1024:
	{
		m_button_height = 32;
		m_menu_y = 328;
		m_logo_y = 162;
	}
	break;

	case 1050:
	{
		m_button_height = 32;
		m_menu_y = 404;
		m_logo_y = 202;
	}
	break;

	case 1080:
	{
		m_button_height = 32;
		m_menu_y = 390;
		m_logo_y = 182;
	}
	break;

	default:
		bInvalidResolution = true;
		break;
	}

	if (bInvalidResolution)
	{
		Warning("Unknown screen resolution: (%dx%d).\n", width, height);
	}

}

void C1187MainMenu::PositionButtons()
{
	int x, y;
	x = m_menu_x;
	y = m_menu_y;

	for (size_t i = 0; i < MAINMENU_LAST; i++)
	{
		if (m_menuButtons[i] && m_menuButtons[i]->IsEnabled() && m_menuButtons[i]->IsVisible())
		{
			m_menuButtons[i]->SetPos(x, y);
			m_menuButtons[i]->SetSize(m_button_width, m_button_height);

			y += m_button_height;
		}
	}
}

void C1187MainMenu::PositionLogo()
{
	if (m_pLogo)
	{
		m_pLogo->SetPos(m_logo_x, m_logo_y);
		m_pLogo->SetSize(m_logo_width, m_logo_height);
	}
}


void C1187MainMenu::InvalidateButtonsLayout(bool layoutNow, bool reloadScheme)
{
	for (int i = 0; i < MAINMENU_LAST; i++)
	{
		if (m_menuButtons[i])
			m_menuButtons[i]->InvalidateLayout(layoutNow, reloadScheme);
	}
}

//------------------------------------------------------------------------------
void CC_Show1187MainMenu(const CCommand& args)
{
	VPANEL panel = enginevgui->GetPanel(PANEL_GAMEUIDLL);

	C1187MainMenu* pGameSettings = new C1187MainMenu(panel);
	if (pGameSettings)
	{
		pGameSettings->SetDeleteSelfOnClose(true);
		pGameSettings->Activate();
	}
}

static ConCommand show1187mainmenu("show1187mainmenu", CC_Show1187MainMenu, "Show 1187 main menu.\n", FCVAR_CHEAT);