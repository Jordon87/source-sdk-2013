#include "cbase.h"
#include "hud.h"
#include "hud_numericdisplay.h"
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
// Purpose: Displays the secondary ammunition level
//-----------------------------------------------------------------------------
class CHudSecondaryAmmoNH2 : public CHudNumericDisplay, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CHudSecondaryAmmoNH2, CHudNumericDisplay);

public:
	CHudSecondaryAmmoNH2(const char *pElementName) : BaseClass(NULL, "HudAmmoSecondary"), CHudElement(pElementName)
	{
		m_iAmmo = -1;

		m_nTexture_BG = surface()->CreateNewTextureID();
		surface()->DrawSetTextureFile(m_nTexture_BG, "vgui/hud/stamina_bg", true, false);	// Originally healhbar_bg

		SetHiddenBits(HIDEHUD_HEALTH | HIDEHUD_WEAPONSELECTION | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT);
	}

	void Init(void)
	{
	}

	void VidInit(void)
	{
	}

	void SetAmmo(int ammo)
	{
		if (ammo != m_iAmmo)
		{
			m_iAmmo = ammo;
		}
		SetDisplayValue(ammo);
	}

	void Reset()
	{
		// hud reset, update ammo state
		BaseClass::Reset();
		m_iAmmo = 0;
		m_hCurrentActiveWeapon = NULL;
		SetAlpha(0);
		UpdateAmmoState();
	}

	virtual void Paint(void)
	{
		BaseClass::Paint();
	}


	void PaintBackground()
	{
		SetBgColor(Color(0, 0, 0));
		vgui::surface()->DrawSetColor(GetFgColor());
		surface()->DrawSetTexture(m_nTexture_BG);

		surface()->DrawTexturedRect(0, 0, GetWide(), GetTall());

		SetPaintBorderEnabled(false);

		BaseClass::PaintBackground();

	}

protected:

	virtual void OnThink()
	{
		// set whether or not the panel draws based on if we have a weapon that supports secondary ammo
		C_BaseCombatWeapon *wpn = GetActiveWeapon();
		C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();
		IClientVehicle *pVehicle = player ? player->GetVehicle() : NULL;
		if (!wpn || !player || pVehicle)
		{
			m_hCurrentActiveWeapon = NULL;
			SetPaintEnabled(false);
			SetPaintBackgroundEnabled(false);
			return;
		}
		else
		{
			SetPaintEnabled(true);
			SetPaintBackgroundEnabled(true);
		}

		UpdateAmmoState();
	}

	void UpdateAmmoState()
	{
		C_BaseCombatWeapon *wpn = GetActiveWeapon();
		C_BasePlayer *player = C_BasePlayer::GetLocalPlayer();

		if (player && wpn && wpn->UsesSecondaryAmmo())
		{
			SetAmmo(player->GetAmmoCount(wpn->GetSecondaryAmmoType()));
		}

		if (m_hCurrentActiveWeapon != wpn)
		{
			if (wpn->UsesSecondaryAmmo())
			{
				// we've changed to a weapon that uses secondary ammo
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponUsesSecondaryAmmo");
			}
			else
			{
				// we've changed away from a weapon that uses secondary ammo
				g_pClientMode->GetViewportAnimationController()->StartAnimationSequence("WeaponDoesNotUseSecondaryAmmo");
			}
			m_hCurrentActiveWeapon = wpn;
		}
	}

private:
	CHandle< C_BaseCombatWeapon > m_hCurrentActiveWeapon;
	int		m_iAmmo;

	int m_nTexture_BG;
};

DECLARE_HUDELEMENT(CHudSecondaryAmmoNH2);