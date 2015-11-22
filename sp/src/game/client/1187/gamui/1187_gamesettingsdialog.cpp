//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "fmtstr.h"

#include "ienginevgui.h"

#include <vgui_controls/Controls.h>
#include <vgui_controls/PropertyDialog.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Label.h>

#include "../vgui/1187_gamesettings.h"
#include "../1187_gamesettings_config.h"
#include "1187_gamesettingsdialog.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
C1187GameSettingsDialog::C1187GameSettingsDialog(vgui::VPANEL parent) : PropertyDialog(NULL, "1187Options")
{
	DevMsg("C1187GameSettingsDialog::C1187GameSettingsDialog\n");

	SetParent( parent );

	SetSizeable(true);
	SetMoveable(true);
	SetMouseInputEnabled(true);
	SetKeyBoardInputEnabled(true);
	
	// SetDeleteSelfOnClose(true);

	SetTitle("#GameUI_1187_Options", true);
	SetOKButtonText("#GameUI_Apply");

	m_pViewRealism = new vgui::CheckButton(this, "viewrealism", "#GameUI_1187_Options_ViewRealism");
	m_pViewBobbing = new vgui::CheckButton(this, "viewbobbing", "#GameUI_1187_Options_ViewBobbing");
	m_pIronBlur = new vgui::CheckButton(this, "ironblur", "#GameUI_1187_Options_IronsightBlur");
	m_pSprintBlur = new vgui::CheckButton(this, "sprintblur", "#GameUI_1187_Options_SprintingBlur");
	m_pHudStyle = new vgui::CheckButton(this, "hudstyle", "#GameUI_1187_Options_AlternateHUD");

	m_pEggList = new vgui::Label(this, "eggslist", "#GameUI_1187_Options_EggList");
	m_pInfo = new vgui::Label(this, "Info", "#GameUI_1187_Options_Info");
	m_pLabel1 = new vgui::Label(this, "Label1", "#GameUI_1187_Options_Label1");

	LoadControlSettings("Resource/Options.res");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
C1187GameSettingsDialog::~C1187GameSettingsDialog()
{
	DevMsg("C1187GameSettingsDialog::~C1187GameSettingsDialog\n");

	if (m_pViewRealism)
	{
		delete m_pViewRealism;
		m_pViewRealism = NULL;
	}

	if (m_pViewBobbing)
	{
		delete m_pViewBobbing;
		m_pViewBobbing = NULL;
	}

	if (m_pIronBlur)
	{
		delete m_pIronBlur;
		m_pIronBlur = NULL;
	}

	if (m_pSprintBlur)
	{
		delete m_pSprintBlur;
		m_pSprintBlur = NULL;
	}

	if (m_pHudStyle)
	{
		delete m_pHudStyle;
		m_pHudStyle = NULL;
	}

	if (m_pEggList)
	{
		delete m_pEggList;
		m_pEggList = NULL;
	}

	if (m_pInfo)
	{
		delete m_pInfo;
		m_pInfo = NULL;
	}

	if (m_pLabel1)
	{
		delete m_pLabel1;
		m_pLabel1 = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: runs the server when the OK button is pressed
//-----------------------------------------------------------------------------
bool C1187GameSettingsDialog::OnOK(bool applyOnly)
{
	DevMsg("C1187GameSettingsDialog::OnOK\n");

	static Eleven87GameSettings_t settings;

	settings.m_viewbobbing	= (m_pViewBobbing && m_pViewBobbing->IsSelected());
	settings.m_viewrealism	= (m_pViewRealism && m_pViewRealism->IsSelected());
	settings.m_ironblur		= (m_pIronBlur && m_pIronBlur->IsSelected());
	settings.m_sprintblur	= (m_pSprintBlur && m_pSprintBlur->IsSelected());
	settings.m_hudstyle		= (m_pHudStyle && m_pHudStyle->IsSelected());

	g_p1187gamesettings->Apply(&settings);
	
	return true;
}

void C1187GameSettingsDialog::SetSettings(void* data)
{
	Eleven87GameSettings_t* settings = (Eleven87GameSettings_t*)data;

	if (settings)
	{
		if (m_pViewRealism)
			m_pViewRealism->SetSelected(settings->m_viewrealism);

		if (m_pViewBobbing)
			m_pViewBobbing->SetSelected(settings->m_viewbobbing);

		if (m_pIronBlur)
			m_pIronBlur->SetSelected(settings->m_ironblur);

		if (m_pSprintBlur)
			m_pSprintBlur->SetSelected(settings->m_sprintblur);

		if (m_pHudStyle)
			m_pHudStyle->SetSelected(settings->m_hudstyle);

		if (m_pEggList)
			m_pEggList->SetText(CFmtStr("You have (%d) egg(s).\n", settings->m_eggcount));
	}
}

//------------------------------------------------------------------------------
void CC_Show1187GameSettings(const CCommand& args)
{
	if (g_p1187gamesettings)
	{
		g_p1187gamesettings->SetVisible(true);
	}
	else
	{
		DevMsg("Couldn't open 1187 game settings.\n");
	}
}

static ConCommand show1187gamesettings("show1187gamesettings", CC_Show1187GameSettings, "Show 1187 game settings.\n", FCVAR_CHEAT);