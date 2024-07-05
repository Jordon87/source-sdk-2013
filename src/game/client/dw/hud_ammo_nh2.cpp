 
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "iclientmode.h"
#include "c_basehlplayer.h"
#include "vgui_controls/Panel.h"
#include "vgui_controls/AnimationController.h"
#include "vgui/ISurface.h"
#include <vgui/ILocalize.h>
 
using namespace vgui;
 
#include "tier0/memdbgon.h" 
//-----------------------------------------------------------------------------
// Purpose: Shows the ammo
//-----------------------------------------------------------------------------
 
class CHudAmmoNH2 : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CHudAmmoNH2, vgui::Panel);
 
public:
	CHudAmmoNH2(const char * pElementName);
 
	virtual void Init (void);
	virtual void Reset (void);
	virtual void OnThink (void);

	void SetAmmo(int ammo);
	void SetAmmo2(int ammo2);
 
protected:

	void UpdatePlayerAmmo( C_BasePlayer *player );
	virtual void Paint();
	virtual void PaintBackground();

	int m_nTexture_BG;
	int m_nTexture_Line;

	int		m_iAmmo;
	int		m_iAmmo2;
	CHudTexture *m_iconPrimaryAmmo;
	CHandle< C_BaseCombatWeapon > m_hCurrentActiveWeapon;
	CHandle< C_BaseEntity > m_hCurrentVehicle;
 
private:

	CPanelAnimationVar(Color, m_HullColor, "TextColor", "255 255 255 255");
	CPanelAnimationVar(float, m_BGAlpha, "My_Alpha", "256");

	CPanelAnimationVarAliasType(float, icon_xpos, "icon_xpos", "30", "proportional_float");
	CPanelAnimationVarAliasType(float, icon_ypos, "icon_ypos", "-12", "proportional_float");

	CPanelAnimationVarAliasType(float, line_xpos, "line_xpos", "30", "proportional_float");
	CPanelAnimationVarAliasType(float, line_ypos, "line_ypos", "-12", "proportional_float");
	CPanelAnimationVarAliasType(float, line_wide, "line_wide", "2", "proportional_float");
	CPanelAnimationVarAliasType(float, line_tall, "line_tall", "20", "proportional_float");
	CPanelAnimationVar(Color, line_color, "line_color", "255 255 255 255");

	CPanelAnimationVarAliasType(float, ammo1_xpos, "ammo1_xpos", "10", "proportional_float");
	CPanelAnimationVarAliasType(float, ammo1_ypos, "ammo1_ypos", "5", "proportional_float");
	CPanelAnimationVarAliasType(float, ammo2_xpos, "ammo2_xpos", "75", "proportional_float");
	CPanelAnimationVarAliasType(float, ammo2_ypos, "ammo2_ypos", "5", "proportional_float");
	CPanelAnimationVar(vgui::HFont, m_hTextFont, "TextFont", "HUD_NH2_Health");
 
};
 
DECLARE_HUDELEMENT (CHudAmmoNH2);

//------------------------------------------------------------------------
// Purpose: Constructor
//------------------------------------------------------------------------
 
CHudAmmoNH2::CHudAmmoNH2 (const char * pElementName) : CHudElement (pElementName), BaseClass (NULL, "HudAmmoNH2")
{
	vgui:: Panel * pParent = g_pClientMode-> GetViewport ();
	SetParent (pParent);

	m_nTexture_BG = surface()->CreateNewTextureID();
	m_nTexture_Line = surface()->CreateNewTextureID();
	surface()->DrawSetTextureFile( m_nTexture_BG, "vgui/hud/ammo_bar", true, false );
 
	SetHiddenBits( HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT | HIDEHUD_WEAPONSELECTION );
}
 
//------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------
 
void CHudAmmoNH2::Init()
{
	Reset();
}
 
//------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------
 
void CHudAmmoNH2::Reset (void)
{
	SetPaintEnabled(true);
	SetPaintBackgroundEnabled(true);
	SetBgColor (Color (255,255,255,255));
	SetFgColor (m_HullColor);
	SetAlpha( 255 );
}
 
 
//------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------
 
void CHudAmmoNH2::OnThink (void)
{
	C_BaseCombatWeapon* wpn = GetActiveWeapon();

	C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
	UpdatePlayerAmmo( player );
	
	IClientVehicle *pVehicle = player ? player->GetVehicle() : NULL;

	if ( !wpn || !player || pVehicle )
	{
		m_hCurrentActiveWeapon = NULL;
		SetPaintEnabled( false );
		SetPaintBackgroundEnabled( false );
	}
	else
	{
		SetPaintEnabled( true );
		SetPaintBackgroundEnabled( true );
	}
}
 
 
//------------------------------------------------------------------------
// Purpose: draws the power bar
//------------------------------------------------------------------------
 
void CHudAmmoNH2::Paint()
{
#if 0
	if (m_iconPrimaryAmmo)
	{
		m_iconPrimaryAmmo->DrawSelf( icon_xpos + ( m_iconPrimaryAmmo->Width()/2 ) , icon_ypos + ( m_iconPrimaryAmmo->Height()/2 ), GetFgColor() );
	}
#endif
	
	surface()->DrawSetColor(GetFgColor());
	surface()->DrawSetTextFont( m_hTextFont );
	surface()->DrawSetTextColor( GetFgColor() );
	wchar_t unicode[6];

	//draw ammo 1
	surface()->DrawSetTextPos( ammo1_xpos, ammo1_ypos );

	swprintf(unicode, L"%d", (int)m_iAmmo);
	surface()->DrawPrintText(unicode,wcslen(unicode));

	//draw ammo 2
	surface()->DrawSetTextPos( ammo2_xpos, ammo2_ypos );
	swprintf(unicode, L"%d", (int)m_iAmmo2);
	surface()->DrawPrintText(unicode,wcslen(unicode));

	//draw line
	surface()->DrawSetColor( m_HullColor );
	surface()->DrawSetTexture( m_nTexture_Line );
	surface()->DrawTexturedRect( line_xpos, line_ypos, line_xpos + line_wide , line_ypos + line_tall );

}



void CHudAmmoNH2::PaintBackground()
{
	SetBgColor(Color(0,0,0));
	vgui::surface()->DrawSetColor(GetFgColor());
	surface()->DrawSetTexture( m_nTexture_BG );
	surface()->DrawTexturedRect( 0, 0, GetWide() , GetTall() );

	SetPaintBorderEnabled(false);

	BaseClass::PaintBackground();

}

void CHudAmmoNH2::UpdatePlayerAmmo( C_BasePlayer *player )
{
	// Clear out the vehicle entity
	m_hCurrentVehicle = NULL;

	C_BaseCombatWeapon *wpn = GetActiveWeapon();

	if ( !wpn || !player || !wpn->UsesPrimaryAmmo() )
	{
		g_pClientMode->GetViewportAnimationController()->RunAnimationCommand( this, "alpha", 0.0f, 0.0f, 0.4f, AnimationController::INTERPOLATOR_LINEAR);
		return;
	}

	
	g_pClientMode->GetViewportAnimationController()->RunAnimationCommand( this, "alpha", m_BGAlpha, 0.0f, 0.4f, AnimationController::INTERPOLATOR_LINEAR);

	// Get our icons for the ammo types
	m_iconPrimaryAmmo = gWR.GetAmmoIconFromWeapon( wpn->GetPrimaryAmmoType() );

	// get the ammo in our clip
	int ammo1 = wpn->Clip1();
	int ammo2;
	if (ammo1 < 0)
	{
		// we don't use clip ammo, just use the total ammo count
		ammo1 = player->GetAmmoCount(wpn->GetPrimaryAmmoType());
		ammo2 = 0;
	}
	else
	{
		// we use clip ammo, so the second ammo is the total ammo
		ammo2 = player->GetAmmoCount(wpn->GetPrimaryAmmoType());
	}

	if (wpn != m_hCurrentActiveWeapon)
	{
		m_hCurrentActiveWeapon = wpn;
	}
		
	SetAmmo(ammo1);
	SetAmmo2(ammo2);
}

void CHudAmmoNH2::SetAmmo(int ammo)
{
	if (ammo != m_iAmmo)
	{
		m_iAmmo = ammo;
	}
}

void CHudAmmoNH2::SetAmmo2(int ammo2)
{
	if (ammo2 != m_iAmmo2)
	{
		m_iAmmo2 = ammo2;
	}
}
