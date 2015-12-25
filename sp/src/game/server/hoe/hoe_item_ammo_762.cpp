//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "items.h"
#include "ammodef.h"
#include "eventlist.h"
#include "npcevent.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

int ITEM_GiveAmmo(CBasePlayer *pPlayer, float flCount, const char *pszAmmoName, bool bSuppressSound = false);

// ========================================================================
//	>> CHoe_Item_Box762
// ========================================================================
class CHoe_Item_Box762 : public CItem
{
public:
	DECLARE_CLASS(CHoe_Item_Box762, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/M60/w_m60box/w_m60box.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/M60/w_m60box/w_m60box.mdl");
	}
	bool MyTouch(CBasePlayer *pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, SIZE_AMMO_SMG1_LARGE, "7_62mm"))
		{
			if (g_pGameRules->ItemShouldRespawn(this) == GR_ITEM_RESPAWN_NO)
			{
				UTIL_Remove(this);
			}

			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(ammo_762mmbox, CHoe_Item_Box762);