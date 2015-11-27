//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "triage_items.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

int ITEM_GiveAmmo(CBasePlayer *pPlayer, float flCount, const char *pszAmmoName, bool bSuppressSound = false);

// ========================================================================
//	>> CTriage_Item_AmmoAK47
// ========================================================================
class CTriage_Item_AmmoAK47 : public CItem
{
public:
	DECLARE_CLASS(CTriage_Item_AmmoAK47, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/items/boxmrounds.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/items/boxmrounds.mdl");
	}
	bool MyTouch(CBasePlayer *pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, SIZE_AMMO_AK47, "AK47"))
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
LINK_ENTITY_TO_CLASS(item_ammo_ak47, CTriage_Item_AmmoAK47);
