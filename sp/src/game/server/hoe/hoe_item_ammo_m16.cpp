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
//	>> CHoe_Item_Clip556
// ========================================================================
class CHoe_Item_Clip556 : public CItem
{
public:
	DECLARE_CLASS(CHoe_Item_Clip556, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/w_m16clip.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/w_m16clip.mdl");
	}
	bool MyTouch(CBasePlayer *pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, SIZE_AMMO_SMG1, "5_56mm"))
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
LINK_ENTITY_TO_CLASS(ammo_m16clip, CHoe_Item_Clip556);

// ========================================================================
//	>> CHoe_Item_Box556
// ========================================================================
class CHoe_Item_Box556 : public CItem
{
public:
	DECLARE_CLASS(CHoe_Item_Box556, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/w_m16box.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/w_m16box.mdl");
	}
	bool MyTouch(CBasePlayer *pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, SIZE_AMMO_SMG1_LARGE, "5_56mm"))
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
LINK_ENTITY_TO_CLASS(ammo_m16box, CHoe_Item_Box556);