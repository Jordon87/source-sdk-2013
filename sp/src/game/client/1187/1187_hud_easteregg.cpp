//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include "c_basehlplayer.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imesh.h"
#include "materialsystem/imaterialvar.h"
#include "../hud_crosshair.h"

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/AnimationController.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Display the 'EasterEgg' icon
//-----------------------------------------------------------------------------
class C1187HudEasterEggDisplay : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(C1187HudEasterEggDisplay, vgui::Panel);

public:
	C1187HudEasterEggDisplay(const char *pElementName);

	bool	ShouldDraw(void);
	void	Init(void);
	void	LevelInit(void);
	void	OnThink(void);

	void	MsgFunc_EasterEgg(bf_read &msg);

protected:

	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void Paint(void);

private:

	float m_flIconHoldStartTime;
	float m_flIconFadeStartTime;
	bool m_bVisible;
	bool m_bPainted;

	CPanelAnimationVar(Color, m_IconColor, "icon_color", "FgColor");
	CPanelAnimationVarAliasType(int, m_nIconTextureId, "icon_texture", "sprites/640hudeasteregg", "textureid");
	CPanelAnimationVarAliasType(int, m_iIconX, "icon_xpos", "8", "proportional_int");
	CPanelAnimationVarAliasType(int, m_iIconY, "icon_ypos", "8", "proportional_int");
	CPanelAnimationVarAliasType(int, m_iIconWide, "icon_width", "8", "proportional_int");
	CPanelAnimationVarAliasType(int, m_iIconTall, "icon_height", "8", "proportional_int");
	CPanelAnimationVarAliasType(float, m_flIconHoldTime, "icon_holdtime", "2", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flIconFadeTime, "icon_fadetime", "5", "proportional_float");

};

DECLARE_HUDELEMENT(C1187HudEasterEggDisplay);
DECLARE_HUD_MESSAGE(C1187HudEasterEggDisplay, EasterEgg);

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
C1187HudEasterEggDisplay::C1187HudEasterEggDisplay(const char *pElementName) : BaseClass(NULL, "HudEasterEggDisplay"), CHudElement(pElementName)
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT);

	m_bVisible = false;
	m_bPainted = false;
	m_flIconHoldStartTime = 0.0f;
	m_flIconFadeStartTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void C1187HudEasterEggDisplay::LevelInit(void)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void C1187HudEasterEggDisplay::Init(void)
{
	HOOK_HUD_MESSAGE(C1187HudEasterEggDisplay, EasterEgg);

	m_bVisible = false;
	m_bPainted = false;
	m_flIconHoldStartTime = 0.0f;
	m_flIconFadeStartTime = 0.0f;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C1187HudEasterEggDisplay::OnThink(void)
{
	BaseClass::OnThink();

	// if item icon hold time has elapsed, send the fade animation. 
	if (m_bVisible && (gpGlobals->curtime - m_flIconHoldStartTime > m_flIconHoldTime))
	{
		if (!m_bPainted)
		{
			g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("EasterEggDisplayHide");

			m_flIconHoldStartTime = 0.0f;
			m_flIconFadeStartTime = gpGlobals->curtime;
			m_bPainted = true;
		}
		else if (gpGlobals->curtime - m_flIconFadeStartTime > m_flIconFadeTime)
		{
			m_bVisible = false;
			m_bPainted = false;
			m_flIconFadeStartTime = 0.0f;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool C1187HudEasterEggDisplay::ShouldDraw(void)
{
	bool bNeedsDraw = false;

	if (m_bVisible)
	{
		bNeedsDraw = true;
	}

	return bNeedsDraw;
}

//-----------------------------------------------------------------------------
// Purpose: sets scheme colors
//-----------------------------------------------------------------------------
void C1187HudEasterEggDisplay::ApplySchemeSettings(vgui::IScheme *scheme)
{
	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
	SetFgColor(m_IconColor);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C1187HudEasterEggDisplay::Paint(void)
{
	BaseClass::Paint();

	surface()->DrawSetColor(m_IconColor);
	surface()->DrawSetTexture(m_nIconTextureId);
	surface()->DrawTexturedRect(m_iIconX, m_iIconY, m_iIconWide, m_iIconTall);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187HudEasterEggDisplay::MsgFunc_EasterEgg(bf_read &msg)
{
	g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("EasterEggDisplayShow");

	m_flIconHoldStartTime = gpGlobals->curtime;
	m_bVisible = true;
	m_bPainted = false;
}