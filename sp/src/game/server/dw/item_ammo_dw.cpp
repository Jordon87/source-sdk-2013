//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The various ammo types for Dangerous World 2	
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
//	>> BoxBuckshot
// ========================================================================
class CItem_BoxBuckshotDW : public CItem
{
public:
	DECLARE_CLASS(CItem_BoxBuckshotDW, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/items/boxbuckshot.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/items/boxbuckshot.mdl");
	}
	bool MyTouch(CBasePlayer *pPlayer)
	{
		if (ITEM_GiveAmmo(pPlayer, SIZE_AMMO_BUCKSHOT, "Buckshot"))
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
LINK_ENTITY_TO_CLASS(item_box_buckshot_dw, CItem_BoxBuckshotDW);