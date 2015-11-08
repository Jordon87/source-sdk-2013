//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
//
// Dangerous World - hud_battery.cpp
//
// implementation of CHudBatteryDW class
//
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include "vgui/ISurface.h"

#include "vgui_controls/AnimationController.h"
#include "vgui/ILocalize.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

#define INIT_BAT	-1

//-----------------------------------------------------------------------------
// Purpose: Displays suit power (armor) on hud
//-----------------------------------------------------------------------------
class CHudBatteryDW : public CHudNumericDisplay, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CHudBatteryDW, CHudNumericDisplay);

public:
	CHudBatteryDW(const char *pElementName);
	void Init( void );
	void Reset( void );
	void VidInit( void );
	void OnThink( void );
	void MsgFunc_Battery(bf_read &msg );
	bool ShouldDraw();

private:
	int		m_iBat;	
	int		m_iNewBat;

	int m_nTexture_BG;
	int m_nTexture_FG;
};

DECLARE_HUDELEMENT(CHudBatteryDW);
DECLARE_HUD_MESSAGE(CHudBatteryDW, Battery);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHudBatteryDW::CHudBatteryDW(const char *pElementName) : BaseClass(NULL, "HudSuit"), CHudElement(pElementName)
{
	SetHiddenBits( HIDEHUD_HEALTH | HIDEHUD_NEEDSUIT );

	m_nTexture_BG = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile(m_nTexture_BG, "vgui/hud/stamina_bg", true, false);	// Originally healhbar_bg

	m_nTexture_FG = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile(m_nTexture_FG, "vgui/hud/stamina_fg", true, false); // Originally healhbar_fg
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryDW::Init(void)
{
	HOOK_HUD_MESSAGE(CHudBatteryDW, Battery);
	Reset();
	m_iBat		= INIT_BAT;
	m_iNewBat   = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryDW::Reset(void)
{
	// SetLabelText(g_pVGuiLocalize->Find("#Valve_Hud_SUIT"));
	SetDisplayValue(m_iBat);
	SetPaintBackgroundEnabled(false);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryDW::VidInit(void)
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CHudBatteryDW::ShouldDraw(void)
{
	bool bNeedsDraw = ( m_iBat != m_iNewBat ) || ( GetAlpha() > 0 );

	return ( bNeedsDraw && CHudElement::ShouldDraw() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryDW::OnThink(void)
{
	if ( m_iBat == m_iNewBat )
		return;

	m_iBat = m_iNewBat;

	SetDisplayValue(m_iBat);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudBatteryDW::MsgFunc_Battery(bf_read &msg)
{
	m_iNewBat = msg.ReadShort();
}
