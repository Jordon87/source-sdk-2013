//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "items.h"
#include "1187_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sv_eastereggs;

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
		// Increment easter egg game rules.
		CElevenEightySeven* pGameRules = dynamic_cast<CElevenEightySeven*>(g_pGameRules);
		if (pGameRules)
		{
			pGameRules->IncrementEasterEggs(1);

			static long nEasterEggs = 0;
			nEasterEggs = pGameRules->GetEasterEggs();

			CSingleUserRecipientFilter user(pPlayer);
			user.MakeReliable();
			UserMessageBegin(user, "EasterEgg");
				WRITE_LONG(nEasterEggs);	// Num easter eggs.
			MessageEnd();
		}

		UTIL_Remove(this);
		return true;
	}
};


LINK_ENTITY_TO_CLASS(item_easteregg, C1187_Item_EasterEgg);