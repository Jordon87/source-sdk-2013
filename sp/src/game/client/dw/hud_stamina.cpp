#include "cbase.h" 
#include "hud.h" 
#include "hud_macros.h" 
#include "c_baseplayer.h"
#include "c_basehlplayer.h"
#include "iclientmode.h" 
#include "vgui/ISurface.h"
#include "hudelement.h"
#include "hud_numericdisplay.h"
#include "vgui_controls/AnimationController.h"


using namespace vgui;
 
#include "tier0/memdbgon.h" 


 
//-----------------------------------------------------------------------------
// Purpose: Shows the hull bar
//-----------------------------------------------------------------------------
 
class CHudStamina : public CHudElement, public vgui::Panel
{
 
	DECLARE_CLASS_SIMPLE(CHudStamina, vgui::Panel);
 
public:
	CHudStamina(const char * pElementName);
 
	virtual void Init (void);
	virtual void Reset (void);
	virtual void OnThink (void);
 
protected:
	virtual void Paint();
	virtual void PaintBackground();

	int m_nTexture_Bar[50];
	int m_nTexture_BG;
	int m_nTexture_FG;
	int m_nTexture_Icon;
 
private:
	CPanelAnimationVar(Color, m_HullColor, "TextColor", "255 255 255 255");
	CPanelAnimationVar(float, m_BGAlpha, "My_Alpha", "256");
	CPanelAnimationVar(int, m_iHullDisabledAlpha, "HullDisabledAlpha", "50");
	CPanelAnimationVarAliasType(float, m_flBarInsetX, "BarInsetX", "26", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flBarInsetY, "BarInsetY", "3", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flBarWidth, "BarWidth", "84", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flBarHeight, "BarHeight", "4", "proportional_float");
	CPanelAnimationVarAliasType(float, icon_xpos, "icon_xpos", "2", "proportional_float");
	CPanelAnimationVarAliasType(float, icon_ypos, "icon_ypos", "2", "proportional_float");
	CPanelAnimationVarAliasType(float, icon_tall, "icon_tall", "2", "proportional_float");
	CPanelAnimationVarAliasType(float, icon_wide, "icon_wide", "2", "proportional_float");
	float m_flHull;
	int m_nHullLow;
 
};
 
DECLARE_HUDELEMENT (CHudStamina);
 
# define HULL_INIT 80 
 
//------------------------------------------------------------------------
// Purpose: Constructor
//------------------------------------------------------------------------
 
CHudStamina::CHudStamina (const char * pElementName) : 
	CHudElement (pElementName), BaseClass (NULL, "HudStamina")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent (pParent);

	m_nTexture_BG = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_nTexture_BG, "vgui/hud/stamina_bg", true, false );

	m_nTexture_FG = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_nTexture_FG, "vgui/hud/stamina_fg", true, false );

	m_nTexture_Icon = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_nTexture_Icon, "vgui/hud/stamina_icon", true, false );

	for (int i = 0;i <= 49;i++)
	{
		m_nTexture_Bar[i] = surface()->CreateNewTextureID();
		
	}
	surface()->DrawSetTextureFile( m_nTexture_Bar[0], "VGUI/hud/bar_0",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[1], "VGUI/hud/bar_1",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[2], "VGUI/hud/bar_2",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[3], "VGUI/hud/bar_3",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[4], "VGUI/hud/bar_4",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[5], "VGUI/hud/bar_5",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[6], "VGUI/hud/bar_6",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[7], "VGUI/hud/bar_7",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[8], "VGUI/hud/bar_8",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[9], "VGUI/hud/bar_9",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[10], "VGUI/hud/bar_10",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[11], "VGUI/hud/bar_11",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[12], "VGUI/hud/bar_12",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[13], "VGUI/hud/bar_13",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[14], "VGUI/hud/bar_14",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[15], "VGUI/hud/bar_15",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[16], "VGUI/hud/bar_16",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[17], "VGUI/hud/bar_17",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[18], "VGUI/hud/bar_18",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[19], "VGUI/hud/bar_19",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[20], "VGUI/hud/bar_20",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[21], "VGUI/hud/bar_21",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[22], "VGUI/hud/bar_22",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[23], "VGUI/hud/bar_23",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[24], "VGUI/hud/bar_24",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[25], "VGUI/hud/bar_25",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[26], "VGUI/hud/bar_26",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[27], "VGUI/hud/bar_27",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[28], "VGUI/hud/bar_28",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[29], "VGUI/hud/bar_29",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[30], "VGUI/hud/bar_30",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[31], "VGUI/hud/bar_31",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[32], "VGUI/hud/bar_32",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[33], "VGUI/hud/bar_33",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[34], "VGUI/hud/bar_34",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[35], "VGUI/hud/bar_35",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[36], "VGUI/hud/bar_36",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[37], "VGUI/hud/bar_37",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[38], "VGUI/hud/bar_38",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[39], "VGUI/hud/bar_39",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[40], "VGUI/hud/bar_40",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[41], "VGUI/hud/bar_41",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[42], "VGUI/hud/bar_42",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[43], "VGUI/hud/bar_43",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[44], "VGUI/hud/bar_44",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[45], "VGUI/hud/bar_45",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[46], "VGUI/hud/bar_46",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[47], "VGUI/hud/bar_47",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[48], "VGUI/hud/bar_48",  true, false );
	surface()->DrawSetTextureFile( m_nTexture_Bar[49], "VGUI/hud/bar_49",  true, false );


	//surface()->CreateFont();
	//surface()->DrawSetTextF
 
	SetHiddenBits ( HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT);
}
 
//------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------
 
void CHudStamina:: Init()
{
	Reset();
}
 
//------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
 
void CHudStamina:: Reset (void)
{
	m_flHull = HULL_INIT;
	m_nHullLow = -1;
	SetBgColor (Color (255,255,255,255));
	SetFgColor (Color (255,255,255,255));
	SetAlpha( 0 );
}
 
 
//------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------
 
void CHudStamina:: OnThink (void)
{
	float newHull = 0;
	C_BaseHLPlayer *pPlayer = (C_BaseHLPlayer *)C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return;

	newHull = pPlayer->m_HL2Local.m_flSuitPower;

	//Msg("Thinking once!\n");

	if (newHull < 100.0f)
	{
		//Msg("Thinking on!\n");
		g_pClientMode->GetViewportAnimationController()->RunAnimationCommand( this, "alpha", m_BGAlpha, 0.0f, 0.4f, AnimationController::INTERPOLATOR_LINEAR);
		//SetAlpha( 255.0f );
	}
	else
	{
		//Msg("Thinking off!\n");
		g_pClientMode->GetViewportAnimationController()->RunAnimationCommand( this, "alpha", 0.0f, 0.0f, 0.4f, AnimationController::INTERPOLATOR_LINEAR);
		//SetAlpha( 0.0f );
	}
		
	if (newHull == m_flHull)
		return;
 
	m_flHull = newHull;
}
 
 
//------------------------------------------------------------------------
// Purpose: draws the power bar
//------------------------------------------------------------------------
 
void CHudStamina::Paint()
{

	//Draw bar

	vgui::surface()->DrawSetColor(GetFgColor());
	//surface()->DrawSetTexture( m_nTexture_FG );
	int i = m_flHull / 2;
	if ( i > 0 )
		i--;

	surface()->DrawSetTexture( m_nTexture_Bar[i] );
	surface()->DrawTexturedRect( m_flBarInsetX, m_flBarInsetY, m_flBarWidth /** ( m_flHull / 100 )*/ + m_flBarInsetX , m_flBarHeight + m_flBarInsetY );
	

	//Draw icon
	vgui::surface()->DrawSetColor(GetFgColor());
	
	surface()->DrawSetTexture( m_nTexture_Icon );	
	surface()->DrawTexturedRect( icon_xpos, icon_ypos, icon_xpos + icon_wide, icon_ypos + icon_tall );

}



void CHudStamina::PaintBackground()
{
	SetBgColor(Color(0,0,0));
	surface()->DrawSetColor(GetFgColor());
	surface()->DrawSetTexture( m_nTexture_BG );
	surface()->DrawTexturedRect( 0, 0, GetWide() , GetTall() );

	SetPaintBorderEnabled(false);

	BaseClass::PaintBackground();

}