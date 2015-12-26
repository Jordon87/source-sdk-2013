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
// RPG7 Round
// ========================================================================
class CHoe_Item_RPG7_Round : public CItem
{
public:
	DECLARE_CLASS(CHoe_Item_RPG7_Round, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/w_rpgammo.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/w_rpgammo.mdl");
	}
	bool MyTouch(CBasePlayer *pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, SIZE_AMMO_RPG_ROUND, "RPG_Round"))
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

LINK_ENTITY_TO_CLASS(weapon_rpgclip, CHoe_Item_RPG7_Round);