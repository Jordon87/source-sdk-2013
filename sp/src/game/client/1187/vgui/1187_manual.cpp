#include "cbase.h"
#include "1187_manual.h"
#include "vgui_controls/Frame.h"
#include <vgui/IVGui.h>
#include <vgui_controls/HTML.h>
#include <filesystem.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

ConVar g_showmanual("g_showmanual", "0", FCVAR_CLIENTDLL);

class CManualPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CManualPanel, vgui::Frame);

public:
	CManualPanel(vgui::VPANEL parent);

	virtual void OnCommand(const char* command);

	virtual void OnTick();
private:
	vgui::HTML *m_pManual;
};

CManualPanel::CManualPanel(vgui::VPANEL parent) : BaseClass(NULL, "1187Manual")
{
	SetVisible(false);

	m_pManual = new vgui::HTML(this, "1187manualhtml", true);
	
	m_pManual->SetScrollbarsEnabled(false);
	m_pManual->SetPos(5,30);
	m_pManual->SetSize(790,565);
	SetParent(parent);
	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
	SetSize(800, 600);

	vgui::ivgui()->AddTickSignal(GetVPanel(), 64);

	SetSizeable(false);
	SetMoveable(true);
	SetMinimizeToSysTrayButtonVisible(true);
	SetMouseInputEnabled(true);
	SetKeyBoardInputEnabled(true);
	LoadControlSettings("Resource/Manual.res");

	int x, y, wide, tall;
	GetBounds(x, y, wide, tall);

	int iHeight = (ScreenHeight() - tall) / 2;
	int iWeight = ScreenWidth();

	SetPos((iWeight - wide) / 2, iHeight);
	SetTitle("1187 Game Manual", true);
	SetZPos(200);
	MoveToFront();
	SetEnabled(true);

	char localURL[_MAX_PATH + 7];
	V_strncpy(localURL, "file://", sizeof(localURL));
	
	char pPathData[_MAX_PATH];
	g_pFullFileSystem->GetLocalPath("resource/manual/DecManual.html", pPathData, sizeof(pPathData));
	V_strncat(localURL, pPathData, sizeof(localURL), COPY_ALL_CHARACTERS);

	m_pManual->OpenURL(localURL, NULL, false);

}

void CManualPanel::OnCommand(const char* command)
{
	if (!_stricmp(command, "CloseManual"))
		g_showmanual.SetValue(0);
}

void CManualPanel::OnTick()
{
	if (IsVisible() && !g_showmanual.GetInt())
	{
		SetVisible(false);
		SetEnabled(false);
	}

	if (!IsVisible())
	{
		if (g_showmanual.GetInt())
		{
			SetVisible(true);
			SetZPos(200);
			MoveToFront();
			RequestFocus(0);
			SetEnabled(true);
		}
	}
}

class C1187ManualPanel : public I1187Manual
{
private:
	CManualPanel* Options;
	vgui::VPANEL m_hParent;

public:
	C1187ManualPanel(void)
	{
		Options = NULL;
	}

	void Create(vgui::VPANEL parent)
	{
		// Create immediately
		Options = new CManualPanel(parent);
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

static C1187ManualPanel g_SManual;
I1187Manual* SManual = (I1187Manual*)&g_SManual;