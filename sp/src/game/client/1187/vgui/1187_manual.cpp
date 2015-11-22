//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 1187 Game manual
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

#include "KeyValues.h"
#include "filesystem.h"

#include "ienginevgui.h"
#include "iclientmode.h"
#include "baseviewport.h"

#include <vgui_controls/Controls.h>
#include <vgui_controls/HTML.h>
#include <vgui_controls/Frame.h>
#include "vguitextwindow.h"

#include "vgui/IPanel.h"
#include "vgui/IVGui.h"
#include "vgui/ISurface.h"
#include "vgui/IInput.h"
#include "vgui/ILocalize.h"
#include "vgui_controls/PHandle.h"

#include "1187_manual.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

#if 1

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
C1187Manual::C1187Manual(IViewPort *pViewPort) : CTextWindow(pViewPort)
{
	m_pViewPort = pViewPort;

	SetTitle("#GameUI_1187_Manual", false);

	SetProportional(true);
	SetMoveable(true);
	SetSizeable(true);

	SetCloseButtonVisible(true);
	SetMinimizeButtonVisible(true);
	SetMaximizeButtonVisible(true);

	LoadControlSettings("Resource/Manual.res");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
C1187Manual::~C1187Manual()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187Manual::Update()
{
	BaseClass::Update();

	m_pOK->RequestFocus();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187Manual::SetVisible(bool state)
{
	BaseClass::SetVisible(state);

	if ( state )
	{
		m_pOK->RequestFocus();
	}
}

//-----------------------------------------------------------------------------
// Purpose: shows the text window
//-----------------------------------------------------------------------------
void C1187Manual::ShowPanel(bool bShow)
{
	if (BaseClass::IsVisible() == bShow)
		return;

	if (bShow)
	{
		Activate();
		SetMouseInputEnabled(true);

		const char* pszManual = "Resource/manual/decmanual.txt";

		if (g_pFullFileSystem->FileExists(pszManual))
		{
			ShowFile(pszManual);
		}
	}
	else
	{
		SetVisible(false);
		SetMouseInputEnabled(false);
	}


	m_pViewPort->ShowBackGround(bShow);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187Manual::OnKeyCodePressed(KeyCode code)
{
	BaseClass::OnKeyCodePressed( code );
}

//-----------------------------------------------------------------------------
// Purpose: The CS background is painted by image panels, so we should do nothing
//-----------------------------------------------------------------------------
void C1187Manual::PaintBackground()
{
	BaseClass::PaintBackground();
}

//-----------------------------------------------------------------------------
// Purpose: Scale / center the window
//-----------------------------------------------------------------------------
void C1187Manual::PerformLayout()
{
	int w, h;
	GetHudSize(w, h);
	// GetParent()->GetSize(w, h);
	SetBounds(0, 0, w, h);
	SetPos(0, 0);

	BaseClass::PerformLayout();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187Manual::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	Frame::ApplySchemeSettings( pScheme );

	LoadControlSettings("Resource/Manual.res");

	int wide, tall;
	GetHudSize(wide, tall);
	SetSize(wide, tall);
	SetPos(0, 0);
	SetBounds(0, 0, wide, tall);

	Reset();
}


void CC_Show1187Manual(const CCommand& args)
{
	CBaseViewport *pViewport = dynamic_cast<CBaseViewport *>(g_pClientMode->GetViewport());
	if (pViewport)
	{
		IViewPortPanel *pManual = pViewport->FindPanelByName(PANEL_1187_MANUAL);
		if (!pManual)
		{
			pManual = pViewport->CreatePanelByName(PANEL_1187_MANUAL);
			pViewport->AddNewPanel(pManual, "PANEL_1187_MANUAL");
		}

		if (pManual)
		{
			pViewport->ShowPanel(pManual, true);
		}
	}
}

static ConCommand show1187manual("show1187manual", CC_Show1187Manual, "Show 1187 manual.\n", FCVAR_CHEAT);

#else

class C1187Manual : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(C1187Manual, vgui::Frame);
public:
	C1187Manual(vgui::VPANEL parent);
	~C1187Manual();

	bool ShowDoc(const char* szDoc);

	virtual void OnThink();

private:

	vgui::HTML* m_pHTML;
};

C1187Manual::C1187Manual(vgui::VPANEL parent)
	: BaseClass(NULL, "1187Manual")
{
	SetParent(parent);

	SetProportional(true);

	SetSizeable(true);
	SetMoveable(true);

	SetMouseInputEnabled(true);
	SetKeyBoardInputEnabled(true);

	SetTitle("#GameUI_Manual" , true );

	LoadControlSettings( "Resource/Manual.res" );

	m_pHTML = new vgui::HTML(this, "1187manualhtml", true, true);

	if (m_pHTML)
	{
		static const char* szManual = "decmanual";

		if (!ShowDoc(szManual))
		{
			DevMsg("Failed to open HTML file %s.\n", szManual);
		}
	}
}

C1187Manual::~C1187Manual()
{
	if (m_pHTML)
	{
		delete m_pHTML;
		m_pHTML = NULL;
	}
}

void C1187Manual::OnThink()
{
	BaseClass::OnThink();

	if (m_pHTML)
	{
		m_pHTML->SetPos(5, 5);
		m_pHTML->SetSize(800, 600);
	}
}

bool C1187Manual::ShowDoc(const char* szDoc)
{
	if (!m_pHTML)
		return false;

	//Msg("SwarmopediaPanel::ShowDoc %s\n", szDoc);
	char fileRES[MAX_PATH];

	Q_snprintf(fileRES, sizeof(fileRES), "resource/manual/%s.html", szDoc);

	if (g_pFullFileSystem->FileExists(fileRES))
	{
		// it's a local HTML file
		char localURL[_MAX_PATH + 7];
		Q_strncpy(localURL, "file://", sizeof(localURL));

		char pPathData[_MAX_PATH];
		g_pFullFileSystem->GetLocalPath(fileRES, pPathData, sizeof(pPathData));
		Q_strncat(localURL, pPathData, sizeof(localURL), COPY_ALL_CHARACTERS);

		// force steam to dump a local copy
		g_pFullFileSystem->GetLocalCopy(pPathData);

		m_pHTML->SetVisible(true);
		m_pHTML->OpenURL(localURL, NULL);

		InvalidateLayout();
		Repaint();
		return true;
	}
	else
	{
		Msg("Couldn't find html %s\n", fileRES);
		m_pHTML->SetVisible(false);
	}
	return false;
}

void CC_Show1187Manual(const CCommand& args)
{
	VPANEL panel = enginevgui->GetPanel(PANEL_INGAMESCREENS);

	C1187Manual* pManual = new C1187Manual(panel);
	if (pManual)
	{
		pManual->SetDeleteSelfOnClose(true);
		pManual->Activate();
	}
}

static ConCommand show1187manual("show1187manual", CC_Show1187Manual, "Show 1187 manual.\n", FCVAR_CHEAT);

#endif