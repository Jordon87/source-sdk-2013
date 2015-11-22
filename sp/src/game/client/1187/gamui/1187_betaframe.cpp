//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"

#include "ienginevgui.h"

#include <vgui_controls/Controls.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/Label.h>

#include "1187_betaframe.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
C1187BetaFrame::C1187BetaFrame(vgui::VPANEL parent) : vgui::Frame(NULL, "1187BetaFrame")
{
	DevMsg("C1187BetaFrame::C1187BetaFrame\n");

	SetParent( parent );

	SetScheme("Resource/SourceScheme.res");

	SetSizeable(true);
	SetMoveable(true);
	SetMouseInputEnabled(true);
	SetKeyBoardInputEnabled(true);

	SetPaintBackgroundEnabled(true);
	SetPaintBackgroundType(2);

	SetTitle("#GameUI_1187_Beta_Title", true);

	m_pInfo = new vgui::Label(this, "Info", "#GameUI_1187_Beta_Info");
	m_pButton1 = new vgui::Button(this, "Button1", "#GameUI_OK", this, "Close");

	LoadControlSettings("Resource/BetaFrame.res");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
C1187BetaFrame::~C1187BetaFrame()
{
	DevMsg("C1187BetaFrame::~C1187BetaFrame\n");

	if (m_pInfo)
	{
		delete m_pInfo;
		m_pInfo = NULL;
	}

	if (m_pButton1)
	{
		delete m_pButton1;
		m_pButton1 = NULL;
	}
}

void C1187BetaFrame::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetPaintBackgroundEnabled(true);
	SetPaintBackgroundType(2);
}