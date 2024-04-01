//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "items.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SF_UNKSPAWNFLAG1		(1 << 2)
#define SF_UNKSPAWNFLAG2		(1 << 3)
#define SF_UNKSPAWNFLAG3		(1 << 5)
#define SF_UNKSPAWNFLAG4		(1 << 6)
#define SF_UNKSPAWNFLAG5		(1 << 14)
#define SF_UNKSPAWNFLAG6		(1 << 15)
#define SF_UNKSPAWNFLAG7		(1 << 17)

extern ConVar sv_eastereggs;

// ========================================================================
//	>> CItemEasterEgg
// ========================================================================
class CItemEasterEgg : public CItem
{
public:
	DECLARE_CLASS(CItemEasterEgg, CItem);

	void Spawn(void)
	{
		ConVarRef noeggsforyou("noeggsforyou");

		if ( ( GetSpawnFlags() & SF_UNKSPAWNFLAG7) == 0 || noeggsforyou.GetInt())
			UTIL_Remove(this);

		Precache();
		SetModel("models/items/pda.mdl");
		BaseClass::Spawn();
		SetMoveType(MOVETYPE_NONE);

		m_nSkin = 2;
		if (!(GetSpawnFlags() & SF_UNKSPAWNFLAG6 || SF_UNKSPAWNFLAG7))
			m_nSkin = 1;
	}
	void Precache(void)
	{
		PrecacheScriptSound("Egg.TouchGold");
		PrecacheScriptSound("Egg.TouchSilver");
		PrecacheModel("models/items/pda.mdl");
	}
	bool MyTouch(CBasePlayer* pPlayer)
	{
		ConVarRef noeggsforyou("noeggsforyou");

		if (noeggsforyou.GetInt())
		{
			Warning("Noclip has previously been activated. This egg has been removed.\n");
			UTIL_Remove(this);
			return true;
		}
		else
		{
			if (pPlayer && (pPlayer->m_nButtons & IN_USE) != 0)
			{
				CSingleUserRecipientFilter user(pPlayer);
				user.MakeReliable();

				UserMessageBegin(user, "ItemPickup");

				if (!(GetSpawnFlags() & SF_UNKSPAWNFLAG6 || SF_UNKSPAWNFLAG7))
					WRITE_STRING("item_easteregg_silver");
				else
					WRITE_STRING("item_easteregg_silver");

				MessageEnd();

				if (!(GetSpawnFlags() & SF_UNKSPAWNFLAG6 || SF_UNKSPAWNFLAG7))
				{
					EmitSound("Egg.TouchSilver");
					UTIL_ClientPrintAll(HUD_PRINTTALK, "%s1 found an bonus pickup.", pPlayer->GetPlayerName());
				}
				else
				{
					EmitSound("Egg.TouchGold");
					UTIL_ClientPrintAll(HUD_PRINTTALK, "%s1 found an easter egg.", pPlayer->GetPlayerName());
				}

				if ((GetSpawnFlags() & SF_UNKSPAWNFLAG7) != 0)
				{
					if ((GetSpawnFlags() & SF_UNKSPAWNFLAG5) != 0)
					{
						pPlayer->SetMaxHealth(200);
						pPlayer->TakeHealth(200.0f, DMG_GENERIC);
						pPlayer->SetMaxHealth(100);
						pPlayer->SetArmorValue(200);
					}
					else if ((GetSpawnFlags() & SF_UNKSPAWNFLAG6) != 0)
					{
						pPlayer->GiveAmmo(999, "Pistol");
						pPlayer->GiveAmmo(999, "AR2");
						pPlayer->GiveAmmo(999, "SMG1");
						pPlayer->GiveAmmo(999, "M4");
						pPlayer->GiveAmmo(999, "Colt");
						pPlayer->GiveAmmo(999, "smg1_grenade");
						pPlayer->GiveAmmo(999, "Buckshot");
						pPlayer->GiveAmmo(999, "357");
						pPlayer->GiveAmmo(999, "98");
						pPlayer->GiveAmmo(999, "rpg_round");
						pPlayer->GiveAmmo(999, "grenade");
						pPlayer->GiveAmmo(999, "Health");
					}
					else if ((GetSpawnFlags() & SF_UNKSPAWNFLAG7) != 0)
					{
						pPlayer->RemoveAllWeapons();

						pPlayer->SetMaxHealth(200);
						pPlayer->TakeHealth(200.0f, DMG_GENERIC);
						pPlayer->SetMaxHealth(100);
						pPlayer->SetArmorValue(200);

						pPlayer->GiveAmmo(999, "Pistol");
						pPlayer->GiveAmmo(999, "AR2");
						pPlayer->GiveAmmo(999, "SMG1");
						pPlayer->GiveAmmo(999, "M4");
						pPlayer->GiveAmmo(999, "Colt");
						pPlayer->GiveAmmo(999, "smg1_grenade");
						pPlayer->GiveAmmo(999, "Buckshot");
						pPlayer->GiveAmmo(999, "357");
						pPlayer->GiveAmmo(999, "98");
						pPlayer->GiveAmmo(999, "rpg_round");
						pPlayer->GiveAmmo(999, "grenade");
						pPlayer->GiveAmmo(999, "Health");

						pPlayer->GiveNamedItem("weapon_crowbar");
						pPlayer->GiveNamedItem("weapon_knife");
						pPlayer->GiveNamedItem("weapon_pistol");
						pPlayer->GiveNamedItem("weapon_dualpistol");
						pPlayer->GiveNamedItem("weapon_357");
						pPlayer->GiveNamedItem("weapon_smg1");
						pPlayer->GiveNamedItem("weapon_dualsmg1");
						pPlayer->GiveNamedItem("weapon_m4");
						pPlayer->GiveNamedItem("weapon_m16");
						pPlayer->GiveNamedItem("weapon_shotgun");
						pPlayer->GiveNamedItem("weapon_kar98");
						pPlayer->GiveNamedItem("weapon_rpg");
						pPlayer->GiveNamedItem("weapon_healthpack");
					}
					else
					{
						engine->ClientCommand(pPlayer->edict(), "_75646754887676789089083428847867");
					}
				}
				UTIL_Remove(this);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
};


LINK_ENTITY_TO_CLASS(item_easteregg, CItemEasterEgg);