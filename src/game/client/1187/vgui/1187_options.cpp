#include "cbase.h"
#include "1187_options.h"
#include "vgui_controls/Frame.h"
#include <vgui/IVGui.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Label.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

ConVar g_showoptions("g_showoptions","0", FCVAR_CLIENTDLL);
class COptionsPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(COptionsPanel, vgui::Frame);

public:
	COptionsPanel(vgui::VPANEL parent);

	virtual void OnCommand(const char* command);

	virtual void OnTick();
private:

	vgui::CheckButton* m_pViewRealism;
	vgui::CheckButton* m_pViewBobbing;
	vgui::CheckButton* m_pIronSight;
	vgui::CheckButton* m_pSprintBlur;
	vgui::CheckButton* m_pAltHud;
	vgui::Label* m_pEggsList;
};

COptionsPanel::COptionsPanel(vgui::VPANEL parent) : BaseClass(NULL, "1187Options")
{
	SetVisible(false);
	
	m_pViewRealism = new vgui::CheckButton(this, "viewrealism", "View Realism");
	ConVarRef cl_viewrealism("cl_viewrealism");
	m_pViewRealism->SetSelected(cl_viewrealism.GetBool());

	m_pViewBobbing = new vgui::CheckButton(this, "viewbobbing", "View Bobbing");
	ConVarRef cl_viewbob_enabled("cl_viewbob_enabled");
	m_pViewBobbing->SetSelected(cl_viewbob_enabled.GetBool());

	m_pIronSight = new vgui::CheckButton(this, "ironblur", "Ironsight Blur");
	ConVarRef cl_ironsightblur("cl_ironsightblur");
	m_pIronSight->SetSelected(cl_ironsightblur.GetBool());

	m_pSprintBlur = new vgui::CheckButton(this, "sprintblur", "Sprinting Blur");
	ConVarRef cl_sprintblur("cl_sprintblur");
	m_pSprintBlur->SetSelected(cl_sprintblur.GetBool());

	m_pAltHud = new vgui::CheckButton(this, "hudstyle", "Alternate HUD");
	ConVarRef hud_fastswitch("hud_fastswitch");
	m_pAltHud->SetSelected(hud_fastswitch.GetBool());

	m_pEggsList = new vgui::Label(this,"eggslist", "You have some eggs...");

	SetParent(parent);
	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
	SetSize(279,284);

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	SetSizeable(false);
	SetMoveable(true);
	SetMinimizeToSysTrayButtonVisible(true);
	SetMouseInputEnabled(true);
	SetKeyBoardInputEnabled(true);
	LoadControlSettings("Resource/Options.res");

	int x, y, wide, tall;
	GetBounds(x, y, wide, tall);

	int iHeight = (ScreenHeight() - tall) / 2;
	int iWeight = ScreenWidth();

	SetPos((iWeight - wide) / 2, iHeight);
	SetTitle("1187 Game Options",true);
	SetZPos(200);
	MoveToFront();
	RequestFocus(0);
	SetEnabled(true);
}

void COptionsPanel::OnCommand(const char* command)
{
	if (!_stricmp(command, "CloseOptions"))
		g_showoptions.SetValue(0);
}

void COptionsPanel::OnTick()
{
	char EasterEggBuffer[40];

	if (IsVisible() && !g_showoptions.GetInt())
	{
		SetVisible(false);
		SetEnabled(false);
	}
	
	if (!IsVisible() && g_showoptions.GetInt())
	{
		SetVisible(true);
		SetZPos(200);
		MoveToFront();
		RequestFocus(0);
		SetEnabled(true);
	}

	if (IsVisible())
	{
		ConVarRef cl_viewrealism("cl_viewrealism");
		ConVarRef cl_viewbob_enabled("cl_viewbob_enabled");
		ConVarRef cl_ironsightblur("cl_ironsightblur");
		ConVarRef cl_sprintblur("cl_sprintblur");
		ConVarRef hud_fastswitch("hud_fastswitch");
		ConVarRef xbox_forcebytes("xbox_forcebytes");

		if (xbox_forcebytes.IsValid())
		{
			V_snprintf(EasterEggBuffer, sizeof(EasterEggBuffer), "You have %i egg(s).");
			m_pEggsList->SetText(EasterEggBuffer);
		}

		if (cl_viewrealism.IsValid())
		{
			cl_viewrealism.SetValue(m_pViewRealism->IsSelected());
		}

		if (cl_viewbob_enabled.IsValid())
		{
			cl_viewbob_enabled.SetValue(m_pViewBobbing->IsSelected());
		}

		if (cl_ironsightblur.IsValid())
		{
			cl_ironsightblur.SetValue(m_pIronSight->IsSelected());
		}

		if (cl_sprintblur.IsValid())
		{
			cl_sprintblur.SetValue(m_pSprintBlur->IsSelected());
		}

		if (hud_fastswitch.IsValid())
		{
			if (m_pAltHud->IsSelected())
				hud_fastswitch.SetValue(0);
			else
				hud_fastswitch.SetValue(3);
		}
	}
}

class C1187OptionsPanel : public I1187Options
{
private:
	COptionsPanel* Options;
	vgui::VPANEL m_hParent;

public:
	C1187OptionsPanel(void)
	{
		Options = NULL;
	}

	void Create(vgui::VPANEL parent)
	{
		// Create immediately
		Options = new COptionsPanel(parent);
	}

	void Destroy(void)
	{
		if (Options)
		{
			Options->SetParent((vgui::Panel*)NULL);
			delete Options;
		}
	}

};

static C1187OptionsPanel g_SOptions;
I1187Options* SOptions = (I1187Options*)&g_SOptions;