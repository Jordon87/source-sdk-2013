//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================

#ifndef ELEVENEIGHTYSEVEN_MANUAL_H
#define ELEVENEIGHTYSEVEN_MANUAL_H

#ifdef _WIN32
#pragma once
#endif

#include "vguitextwindow.h"

#define PANEL_1187_MANUAL		"1187Manual"

class C1187Manual : public CTextWindow
{
private:
	DECLARE_CLASS_SIMPLE(C1187Manual, CTextWindow);

public:
	C1187Manual(IViewPort *pViewPort);
	virtual ~C1187Manual();

	virtual void Update();
	virtual void SetVisible(bool state);
	virtual void ShowPanel(bool bShow);
	virtual void OnKeyCodePressed(vgui::KeyCode code);

	virtual const char *GetName(void) { return PANEL_1187_MANUAL; }

	// Background panel -------------------------------------------------------

public:
	virtual void PaintBackground();
	virtual void PerformLayout();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	// End background panel ---------------------------------------------------
};




#endif // ELEVENEIGHTYSEVEN_MANUAL_H