//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "items.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

int ITEM_GiveAmmo(CBasePlayer *pPlayer, float flCount, const char *pszAmmoName, bool bSuppressSound = false);

// ========================================================================
//	>> C1187_Item_Ammo_M4
// ========================================================================
class C1187_Item_Ammo_M4 : public CItem
{
public:
	DECLARE_CLASS(C1187_Item_Ammo_M4, CItem);

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
		if (ITEM_GiveAmmo(pPlayer, SIZE_AMMO_SMG1, "M4"))
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

LINK_ENTITY_TO_CLASS(item_ammo_m4, C1187_Item_Ammo_M4);