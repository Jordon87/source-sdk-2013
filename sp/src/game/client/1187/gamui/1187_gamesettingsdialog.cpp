//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"

#include "ienginevgui.h"

#include <vgui_controls/Controls.h>
#include <vgui_controls/PropertyDialog.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Label.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

class C1187GameSettingsDialog : public vgui::PropertyDialog
{
	DECLARE_CLASS_SIMPLE(C1187GameSettingsDialog, vgui::PropertyDialog);
public:
	C1187GameSettingsDialog(vgui::VPANEL parent);
	~C1187GameSettingsDialog();
protected:
	virtual bool OnOK(bool applyOnly);

	vgui::CheckButton* m_pViewRealism;
	vgui::CheckButton* m_pViewBobbing;
	vgui::CheckButton* m_pIronBlur;
	vgui::CheckButton* m_pSprintBlur;
	vgui::CheckButton* m_pHudStyle;

	vgui::Label* m_pEggList;
	vgui::Label* m_pInfo;
	vgui::Label* m_pLabel1;
};

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
	
	SetDeleteSelfOnClose(true);

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
		m_pViewRealism->MarkForDeletion();
		m_pViewRealism = NULL;
	}

	if (m_pViewBobbing)
	{
		m_pViewBobbing->MarkForDeletion();
		m_pViewBobbing = NULL;
	}

	if (m_pIronBlur)
	{
		m_pIronBlur->MarkForDeletion();
		m_pIronBlur = NULL;
	}

	if (m_pSprintBlur)
	{
		m_pSprintBlur->MarkForDeletion();
		m_pSprintBlur = NULL;
	}

	if (m_pHudStyle)
	{
		m_pHudStyle->MarkForDeletion();
		m_pHudStyle = NULL;
	}

	if (m_pEggList)
	{
		m_pEggList->MarkForDeletion();
		m_pEggList = NULL;
	}

	if (m_pInfo)
	{
		m_pInfo->MarkForDeletion();
		m_pInfo = NULL;
	}

	if (m_pLabel1)
	{
		m_pLabel1->MarkForDeletion();
		m_pLabel1 = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: runs the server when the OK button is pressed
//-----------------------------------------------------------------------------
bool C1187GameSettingsDialog::OnOK(bool applyOnly)
{
	DevMsg("C1187GameSettingsDialog::OnOK\n");

	return true;
}



//------------------------------------------------------------------------------
void CC_Show1187GameSettings(const CCommand& args)
{
	VPANEL panel = enginevgui->GetPanel( PANEL_GAMEUIDLL );

	C1187GameSettingsDialog* pGameSettings = new C1187GameSettingsDialog(panel);
	if (pGameSettings)
	{
		pGameSettings->SetAutoDelete(true);
		pGameSettings->Activate();
		pGameSettings->SetVisible(true);
	}
}

static ConCommand show1187gamesettings("show1187gamesettings", CC_Show1187GameSettings, "Show 1187 game settings.\n", FCVAR_CHEAT);