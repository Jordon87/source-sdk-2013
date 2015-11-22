//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "../gamui/1187/1187_vmainmenu.h"
#include "1187_mainmenu.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

class C1187MainMenuPanelInterface : public I1187MainMenu
{
private:
	C1187MainMenu *m_pMainMenu;
public:
	C1187MainMenuPanelInterface()
	{
		m_pMainMenu = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		m_pMainMenu = new C1187MainMenu(parent);
		m_pMainMenu->Activate();
	}
	void Destroy()
	{
		if (m_pMainMenu)
		{
			m_pMainMenu->SetParent((vgui::Panel *)NULL);
			delete m_pMainMenu;
		}
	}
};
static C1187MainMenuPanelInterface g_1187mainmenu;
I1187MainMenu* g_p1187mainmenu = (I1187MainMenu*)&g_1187mainmenu;