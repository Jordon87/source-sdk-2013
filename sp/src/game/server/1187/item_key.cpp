//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "items.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CItem_Key : public CItem
{
	DECLARE_CLASS(CItem_Key, CItem);
public:
	void Spawn(void)
	{
		Precache();
		SetModel("models/lostcoast/fisherman/Keys.mdl");
		BaseClass::Spawn();
	}

	void Precache(void)
	{
		PrecacheModel("models/lostcoast/fisherman/Keys.mdl");
		PrecacheScriptSound("Key.Pickup");
	}

	bool MyTouch(CBasePlayer *pPlayer)
	{
		CSingleUserRecipientFilter user(pPlayer);
		user.MakeReliable();
		UserMessageBegin(user, "ItemPickup");
			WRITE_STRING(GetClassname());
		MessageEnd();

		CPASAttenuationFilter filter(pPlayer, "Key.Pickup");
		EmitSound(filter, pPlayer->entindex(), "Key.Pickup");

		UTIL_Remove(this);
		return true;
	}
};

LINK_ENTITY_TO_CLASS(item_key, CItem_Key);
