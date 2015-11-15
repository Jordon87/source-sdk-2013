//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include <vgui_controls/Controls.h>

#include "ienginevgui.h"

#include "1187_vmainmenu.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

C1187MainMenu::C1187MainMenu(vgui::VPANEL parent)
	: BaseClass(NULL, "1187MainMenu")
{
	SetParent( parent );

	SetAutoDelete(true);

	SetMoveable(true);
	SetSizeable(true);
	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetPaintBackgroundEnabled(true);


}

C1187MainMenu::~C1187MainMenu()
{

}

void C1187MainMenu::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

//------------------------------------------------------------------------------
void CC_Show1187MainMenu(const CCommand& args)
{
	VPANEL panel = enginevgui->GetPanel(PANEL_GAMEUIDLL);

	C1187MainMenu* pGameSettings = new C1187MainMenu(panel);
	if (pGameSettings)
	{
		pGameSettings->SetAutoDelete(true);
		pGameSettings->Activate();
		pGameSettings->SetVisible(true);
	}
}

static ConCommand show1187mainmenu("show1187mainmenu", CC_Show1187MainMenu, "Show 1187 main menu.\n", FCVAR_CHEAT);