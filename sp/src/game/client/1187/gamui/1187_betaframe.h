//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BETAFRAME_H
#define ELEVENEIGHTYSEVEN_BETAFRAME_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Frame.h>

class C1187BetaFrame : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(C1187BetaFrame, vgui::Frame);
public:
	C1187BetaFrame(vgui::VPANEL parent);
	~C1187BetaFrame();

protected:

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:

	vgui::Label* m_pInfo;
	vgui::Button* m_pButton1;
};

#endif // ELEVENEIGHTYSEVEN_BETAFRAME_H