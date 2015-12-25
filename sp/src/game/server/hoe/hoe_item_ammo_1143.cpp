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
//	>> CHoe_Item_1143mm_Box40
// ========================================================================
class CHoe_Item_1143mm_Box40 : public CItem
{
public:
	DECLARE_CLASS(CHoe_Item_1143mm_Box40, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/colt1911A1/w_1143mm_40box/w_1143mm_40box.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/colt1911A1/w_1143mm_40box/w_1143mm_40box.mdl");
	}
	bool MyTouch(CBasePlayer *pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, SIZE_AMMO_PISTOL, "11_43mm"))
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
LINK_ENTITY_TO_CLASS(ammo_1143mm_40box, CHoe_Item_1143mm_Box40);

// ========================================================================
//	>> CHoe_Item_1143mm_Box50
// ========================================================================
class CHoe_Item_1143mm_Box50 : public CItem
{
public:
	DECLARE_CLASS(CHoe_Item_1143mm_Box50, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/colt1911A1/w_1143mm_50box/w_1143mm_50box.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/colt1911A1/w_1143mm_50box/w_1143mm_50box.mdl");
	}
	bool MyTouch(CBasePlayer *pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, SIZE_AMMO_PISTOL, "11_43mm"))
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
LINK_ENTITY_TO_CLASS(ammo_1143mm_50box, CHoe_Item_1143mm_Box50);