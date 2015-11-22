//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

#include <vgui_controls/Frame.h>

#include "../gamui/1187_betaframe.h"
#include "1187_beta.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

class C1187BetaPanelInterface : public I1187Beta
{
private:
	C1187BetaFrame* m_pBeta;
public:
	C1187BetaPanelInterface()
	{
		m_pBeta = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		m_pBeta = new C1187BetaFrame(parent);
		m_pBeta->Activate();
	}
	void Destroy()
	{
		if (m_pBeta)
		{
			m_pBeta->SetParent((vgui::Panel *)NULL);
			delete m_pBeta;
			m_pBeta = NULL;
		}
	}
};
static C1187BetaPanelInterface g_1187beta;
I1187Beta* g_p1187beta = (I1187Beta*)&g_1187beta;