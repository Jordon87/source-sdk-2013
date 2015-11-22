//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_GAMESETTINGSDIALOG_H
#define ELEVENEIGHTYSEVEN_GAMESETTINGSDIALOG_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyDialog.h>

class I1187GameSettings;

class C1187GameSettingsDialog : public vgui::PropertyDialog
{
	DECLARE_CLASS_SIMPLE(C1187GameSettingsDialog, vgui::PropertyDialog);
public:
	C1187GameSettingsDialog(vgui::VPANEL parent);
	~C1187GameSettingsDialog();

	virtual void SetSettings(void* data);

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

	I1187GameSettings* m_pSettings;
};

#endif // ELEVENEIGHTYSEVEN_GAMESETTINGSDIALOG_H