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
#include "c_triage_player.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imesh.h"
#include "materialsystem/imaterialvar.h"
#include "c_triage_basecombatweapon.h"

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/AnimationController.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define UNICODE_ARROW 0x2192

//-----------------------------------------------------------------------------
// Purpose: Draw weapon slot swap hud.
//-----------------------------------------------------------------------------
class CTriageHudWeapon : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CTriageHudWeapon, vgui::Panel);

public:
	CTriageHudWeapon(const char *pElementName);

	bool	ShouldDraw(void);
	void	Init(void);
	void	LevelInit(void);

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void Paint(void);

private:
	void DrawLargeWeaponBox(C_BaseCombatWeapon *pWeapon, int x, int y, int wide, int tall, Color color, float alpha);

	vgui::HFont m_hFont;
};

DECLARE_HUDELEMENT(CTriageHudWeapon);

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTriageHudWeapon::CTriageHudWeapon(const char *pElementName) : CHudElement(pElementName), BaseClass(NULL, "HudWeapon")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT);
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CTriageHudWeapon::Init(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CTriageHudWeapon::LevelInit(void)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: sets scheme colors
//-----------------------------------------------------------------------------
void CTriageHudWeapon::ApplySchemeSettings(vgui::IScheme *scheme)
{
	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(true);
	SetPaintBorderEnabled(true);
	SetFgColor(Color(255, 255, 255, 220));
	SetBgColor(Color(0, 0, 0, 80));

	vgui::HScheme pScheme = vgui::scheme()->GetScheme("ClientScheme");
	m_hFont = vgui::scheme()->GetIScheme(pScheme)->GetFont("HudWeaponArrow", true);

	SetForceStereoRenderToFrameBuffer(true);
	int x, y;
	int screenWide, screenTall;
	surface()->GetFullscreenViewport(x, y, screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CTriageHudWeapon::ShouldDraw(void)
{
	C_Triage_Player* pPlayer = ToTriagePlayer( C_BasePlayer::GetLocalPlayer() );
	if (!pPlayer)
		return false;

	bool bNeedsDraw = false;

	if (pPlayer->m_TriageLocal.m_bBumpWeapon)
	{
		bNeedsDraw = true;
	}

	if (pPlayer->m_TriageLocal.m_hBumpWeapon == NULL || pPlayer->GetActiveWeapon() == NULL)
	{
		bNeedsDraw = false;
	}

	return (bNeedsDraw && CHudElement::ShouldDraw());
}

ConVar cl_hud_weapon_arrow_frequency("cl_hud_weapon_arrow_frequency", "8.0");
ConVar cl_hud_weapon_arrow_amplitude("cl_hud_weapon_arrow_amplitude", "4.0");

//-----------------------------------------------------------------------------
// Purpose: draws the zoom effect
//-----------------------------------------------------------------------------
void CTriageHudWeapon::Paint(void)
{
	C_Triage_Player* pPlayer = ToTriagePlayer(C_BasePlayer::GetLocalPlayer());
	if (!pPlayer)
		return;

	int wide, tall;
	GetSize(wide, tall);

	int cx, cy, extray;
	cx = wide / 2; // Screen center (X)
	cy = tall / 2; // Screen center (Y)
	extray = wide / 6; // Screen extra (Y).

	// Draw the active weapon icon.
	int x, y;
	int activeWeaponWidth, activeWeaponHeight;
	int bumpWeaponWidth, bumpWeaponHeight;
	// int width, height;
	int height;

	C_BaseCombatWeapon *pActiveWeapon, *pBumpWeapon;


	// Get the currently held weapon.
	pActiveWeapon = pPlayer->GetActiveWeapon();

	// Get the bump weapon.
	pBumpWeapon = (C_BaseCombatWeapon*)pPlayer->m_TriageLocal.m_hBumpWeapon.Get();

	// Validate both.
	if (!pBumpWeapon || !pActiveWeapon)
		return;

	// Get active weapon icon dimensions.
	activeWeaponWidth = pActiveWeapon->GetWpnData().iconInactive->Width();
	activeWeaponHeight = pActiveWeapon->GetWpnData().iconInactive->Height();

	// Get bump weapon icon dimensions.
	bumpWeaponWidth = pBumpWeapon->GetWpnData().iconInactive->Width();
	bumpWeaponHeight = pBumpWeapon->GetWpnData().iconInactive->Height();

	height = (activeWeaponHeight > bumpWeaponHeight) ? activeWeaponHeight : bumpWeaponHeight;

	// Get the arrow size.
	static const int arrowSize = surface()->GetFontTall(m_hFont);

	// Draw the active weapon icon.
	x = cx - (activeWeaponWidth + (arrowSize / 2));
	y = (cy + extray) - activeWeaponHeight / 2;

	// Set correct text font.
	vgui::surface()->DrawSetTextFont(m_hFont);

	DrawLargeWeaponBox(pActiveWeapon, x, y, 43, 20, GetFgColor(), 0); // 112, 80

	// Draw the bump weapon icon.
	x = cx + ((bumpWeaponWidth + arrowSize) / 2);

	DrawLargeWeaponBox(pBumpWeapon, x, y, 43, 20, GetFgColor(), 0);


	// Draw the centered arrow.

	x = cx - arrowSize / 2;
	y = (cy + extray) - (height + arrowSize) / 2;

	// Set correct text font.
	vgui::surface()->DrawSetTextFont(m_hFont);
	vgui::surface()->DrawSetTextColor( GetFgColor() );
	vgui::surface()->DrawSetTextPos(x + sin(gpGlobals->curtime * cl_hud_weapon_arrow_frequency.GetFloat()) * cl_hud_weapon_arrow_amplitude.GetFloat(), y);
	vgui::surface()->DrawUnicodeChar(UNICODE_ARROW);
}

void CTriageHudWeapon::DrawLargeWeaponBox(C_BaseCombatWeapon *pWeapon, int xpos, int ypos, int boxWide, int boxTall, Color color, float alpha)
{
	// draw box for selected weapon
	BaseClass::DrawBox(xpos, ypos, boxWide, boxTall, color, alpha / 255.0f);

	// find the center of the box to draw in
	int iconWidth = pWeapon->GetSpriteActive()->Width();
	int iconHeight = pWeapon->GetSpriteActive()->Height();

	int x_offs = (boxWide - iconWidth) / 2;
	int y_offs = (boxTall - iconHeight) / 2;

	// draw an active version over the top
	pWeapon->GetSpriteInactive()->DrawSelf(xpos + x_offs, ypos + y_offs, color);
}