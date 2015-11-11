//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "items.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// ========================================================================
//	>> C1187_Item_EasterEgg
// ========================================================================
class C1187_Item_EasterEgg : public CItem
{
public:
	DECLARE_CLASS(C1187_Item_EasterEgg, CItem);

	void Spawn(void)
	{
		Precache();
		SetModel("models/items/healthkit.mdl");
		BaseClass::Spawn();
	}
	void Precache(void)
	{
		PrecacheModel("models/items/healthkit.mdl");
	}
	bool MyTouch(CBasePlayer *pPlayer)
	{
		UTIL_Remove(this);
		return true;
	}
};


LINK_ENTITY_TO_CLASS(item_easteregg, C1187_Item_EasterEgg);