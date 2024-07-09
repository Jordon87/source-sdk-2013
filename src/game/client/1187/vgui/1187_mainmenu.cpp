//========= Copyright © 2006, Valve Productions, All rights reserved. ============//
//
// Purpose: Display Main Menu images, handles rollovers as well
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "1187_mainmenu.h"
#include "vgui_controls/Frame.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>

#include "vgui_controls/ImagePanel.h"
#include "vgui_imagebutton.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void CC_ReturnToMenu()
{
	engine->ClientCmd("startupmenu");
}
ConCommand returntomenu("returntomenu", CC_ReturnToMenu, "Acivate this to return to the main menu!");

//-----------------------------------------------------------------------------
// Purpose: Displays the logo panel
//-----------------------------------------------------------------------------
class CMainMenuPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CMainMenuPanel, vgui::Frame);

public:
	CMainMenuPanel(vgui::VPANEL parent);
	~CMainMenuPanel();

	virtual void OnCommand(const char* command);

	virtual void OnTick();

	virtual bool BackgroundMap();

private:

	vgui::ImagePanel* m_pBackgroundImage;

	vgui::ImageButton* m_pResumeButton;
	vgui::ImageButton* m_pReturntoMenuButton;
	vgui::ImageButton* m_pCrashCourseButton;
	vgui::ImageButton* m_pNewGameButton;
	vgui::ImageButton* m_pLoadGameButton;
	vgui::ImageButton* m_pSaveGameButton;
	vgui::ImageButton* m_pManualButton;
	vgui::ImageButton* m_pOptionsButton;
	vgui::ImageButton* m_pSettingsButton;
	vgui::ImageButton* m_pBonusMapsButton;
	vgui::ImageButton* m_pQuitButton;
	vgui::ImageButton* m_pCommentaryButton;

	bool unk_0x1e4;
};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMainMenuPanel::CMainMenuPanel(vgui::VPANEL parent) : BaseClass(NULL, "1187MainMenu")
{
	PrecacheMaterial("vgui/mainmenu/1187menu");

	m_pResumeButton = new vgui::ImageButton(this, "resumegame_button", "mainmenu/resumegame_static", "mainmenu/resumegame_over", "mainmenu/resumegame_static", "ResumeGame");
	m_pReturntoMenuButton = new vgui::ImageButton(this, "returntomenu_button", "mainmenu/returntomenu_static", "mainmenu/returntomenu_over", "mainmenu/returntomenu_static", "ReturnToMenu");
	m_pCrashCourseButton = new vgui::ImageButton(this, "crashcourse_button", "mainmenu/crashcourse_static", "mainmenu/crashcourse_over", "mainmenu/crashcourse_static", "PlayCrashCourse");
	m_pNewGameButton = new vgui::ImageButton(this, "newgame_button", "mainmenu/newgame_static", "mainmenu/newgame_over", "mainmenu/newgame_static", "OpenNewGameDialog");
	m_pLoadGameButton = new vgui::ImageButton(this, "loadgame_button", "mainmenu/loadgame_static", "mainmenu/loadgame_over", "mainmenu/loadgame_static", "OpenLoadGameDialog");
	m_pSaveGameButton = new vgui::ImageButton(this, "savegame_button", "mainmenu/savegame_static", "mainmenu/savegame_over", "mainmenu/savegame_static", "OpenSaveGameDialog");
	m_pManualButton = new vgui::ImageButton(this, "manual_button", "mainmenu/manual_static", "mainmenu/manual_over", "mainmenu/manual_static", "OpenManualDialog");
	m_pOptionsButton = new vgui::ImageButton(this, "options_button", "mainmenu/options_static", "mainmenu/options_over", "mainmenu/options_static", "OpenOptionsDialog");
	m_pSettingsButton = new vgui::ImageButton(this, "settings_button", "mainmenu/settings_static", "mainmenu/settings_over", "mainmenu/settings_static", "OpenSettingsDialog");
	m_pBonusMapsButton = new vgui::ImageButton(this, "bonusmaps_button", "mainmenu/bonusmaps_static", "mainmenu/bonusmaps_over", "mainmenu/bonusmaps_static", "OpenBonusMapsDialog");
	m_pQuitButton = new vgui::ImageButton(this, "quit_button", "mainmenu/quit_static", "mainmenu/quit_over", "mainmenu/quit_static", "Quit");
	m_pCommentaryButton = new vgui::ImageButton(this, "commentary_button", "mainmenu/commentary_static", "mainmenu/commentary_over", "mainmenu/commentary_static", "OpenCommentaryDialog");

	m_pBackgroundImage = new vgui::ImagePanel(this, "background_image");

	SetParent(parent);
	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
	SetSize(800,600);

	vgui::ivgui()->AddTickSignal(GetVPanel(), 500);

	SetVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetMinimizeToSysTrayButtonVisible(false);
	SetMouseInputEnabled(true);
	SetKeyBoardInputEnabled(true);
	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);

	LoadControlSettings("Resource/GameMainMenu.res");

	m_pBackgroundImage->SetPos(0,0);
	m_pBackgroundImage->SetSize(800,600);
	m_pBackgroundImage->SetImage("mainmenu/1187menu");

	m_pResumeButton->SetSize(512,32);
	m_pReturntoMenuButton->SetSize(512,32);
	m_pCrashCourseButton->SetSize(512,32);
	m_pNewGameButton->SetSize(512,32);
	m_pLoadGameButton->SetSize(512,32);
	m_pSaveGameButton->SetSize(512,32);
	m_pManualButton->SetSize(512,32);
	m_pOptionsButton->SetSize(512,32);
	m_pSettingsButton->SetSize(512,32);
	m_pBonusMapsButton->SetSize(512,32);
	m_pQuitButton->SetSize(512,32);
	m_pCommentaryButton->SetSize(512,32);
	m_pCommentaryButton->SetSize(512,32);

	m_pResumeButton->SetVisible(false);
	m_pReturntoMenuButton->SetVisible(false);
	m_pSaveGameButton->SetVisible(false);

	m_pCrashCourseButton->SetPos(150,193);
	m_pNewGameButton->SetPos(150,232);
	m_pLoadGameButton->SetPos(150,271);
	m_pCommentaryButton->SetPos(150,310);
	m_pManualButton->SetPos(150,349);
	m_pOptionsButton->SetPos(150,388);
	m_pSettingsButton->SetPos(150,427);
	m_pBonusMapsButton->SetPos(150,466);
	m_pBonusMapsButton->SetPos(150,505);

	unk_0x1e4 = true;

}

void CMainMenuPanel::OnCommand(const char* command)
{
	if (!_stricmp(command, "ResumeGame"))
	{
		if (BackgroundMap())
		{
			engine->ClientCmd("gamemenucommand resumegame");
			return;
		}
		surface()->PlaySound("ui/1187menuerror.wav");
		return;
	}

	if (!_stricmp(command, "PlayCrashCourse"))
	{
		engine->ClientCmd("map 1187_crashcourse");
		return;
	}

	if (!_stricmp(command, "OpenNewGameDialog"))
	{
		engine->ClientCmd("gamemenucommand OpenNewGameDialog");
		return;
	}

	if (!_stricmp(command, "OpenLoadGameDialog"))
	{
		engine->ClientCmd("gamemenucommand OpenLoadGameDialog");
		return;
	}

	if (!_stricmp(command, "OpenSaveGameDialog"))
	{
		if (BackgroundMap())
		{
			engine->ClientCmd("gamemenucommand OpenSaveGameDialog");
			return;
		}
		surface()->PlaySound("ui/1187menuerror.wav");
		return;
	}

	if (!_stricmp(command, "OpenManualDialog"))
	{
		engine->ClientCmd("g_showmanual 1");
		return;
	}

	if (!_stricmp(command, "OpenOptionsDialog"))
	{
		engine->ClientCmd("g_showoptions 1");
		return;
	}

	if (!_stricmp(command, "OpenBonusMapsDialog"))
	{
		engine->ClientCmd("gamemenucommand OpenBonusMapsDialog");
		return;
	}

	if (!_stricmp(command, "OpenSettingsDialog"))
	{
		engine->ClientCmd("gamemenucommand OpenOptionsDialog");
		return;
	}

	if (!_stricmp(command, "OpenCommentaryDialog"))
	{
		engine->ClientCmd("gamemenucommand OpenLoadSingleplayerCommentaryDialog");
		return;
	}

	if (!_stricmp(command, "ReturnToMenu"))
	{
		if (BackgroundMap())
		{
			engine->ClientCmd("startupmenu");
			return;
		}
		surface()->PlaySound("ui/1187menuerror.wav");
		return;
	}

	if (!_stricmp(command, "Quit"))
		engine->ClientCmd("gamemenucommand quit");
	else
		Msg("The Fuck? \n");
}

void CMainMenuPanel::OnTick()
{
	if (unk_0x1e4)
	{
		SetVisible(true);
		unk_0x1e4 = false;
	}

	int height = 193;

	if (BackgroundMap())
	{
		m_pResumeButton->SetVisible(true);
		m_pReturntoMenuButton->SetVisible(true);
		m_pCrashCourseButton->SetVisible(false);

		m_pResumeButton->SetPos(150,193);
		m_pReturntoMenuButton->SetPos(150, 232);
		height = 232;
	}
	else
	{
		m_pCrashCourseButton->SetVisible(true);
		m_pResumeButton->SetVisible(false);
		m_pReturntoMenuButton->SetVisible(false);

		m_pCrashCourseButton->SetPos(150, 193);
	}

	m_pNewGameButton->SetPos(150, height + 39);
	m_pLoadGameButton->SetPos(150, height + 78);

	if (BackgroundMap())
	{
		m_pCommentaryButton->SetVisible(false);
		m_pSaveGameButton->SetVisible(true);
	}
	else
	{
		m_pSaveGameButton->SetVisible(false);
		m_pCommentaryButton->SetVisible(true);
	}

	m_pSaveGameButton->SetPos(150, height + 117);
	m_pManualButton->SetPos(150, height + 153);
	m_pOptionsButton->SetPos(150, height + 195);
	m_pSettingsButton->SetPos(150, height + 234);
	m_pBonusMapsButton->SetPos(150, height + 273);
	m_pQuitButton->SetPos(150, height + 312);

	int x1, y1, w1, h1;
	GetBounds(x1, y1, w1, h1);

	int iHeight = (ScreenHeight() - h1) / 2;
	int iWeight = ScreenWidth();

	SetPos((iWeight - w1) / 2, iHeight);
	RequestFocus(-50);
	SetZPos(-100);

}

bool CMainMenuPanel::BackgroundMap()
{
	if (engine->IsLevelMainMenuBackground())
		return false;
	
	if (!strcmp(engine->GetLevelName(), "maps/1187bg01.bsp") || !strcmp(engine->GetLevelName(), ""))
	{
		return false;
	}
	
	engine->IsInGame();
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CMainMenuPanel::~CMainMenuPanel()
{
}

// Class
// Change CSMenu to CModMenu if you want. Salient is the name of the source mod, 
// hence SMenu. If you change CSMenu, change ISMenu too where they all appear.
class C1187Menu : public I1187Menu
{
private:
	CMainMenuPanel* MainMenu;
	vgui::VPANEL m_hParent;

public:
	C1187Menu(void)
	{
		MainMenu = NULL;
	}

	void Create(vgui::VPANEL parent)
	{
		// Create immediately
		MainMenu = new CMainMenuPanel(parent);
	}

	void Destroy(void)
	{
		if (MainMenu)
		{
			MainMenu->SetParent((vgui::Panel*)NULL);
			delete MainMenu;
		}
	}

};

static C1187Menu g_SMenu;
I1187Menu* SMenu = (I1187Menu*)&g_SMenu;