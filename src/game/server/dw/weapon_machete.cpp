//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "weapon_crowbar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar    sk_plr_dmg_machete("sk_plr_dmg_machete", "0");
ConVar    sk_npc_dmg_machete("sk_npc_dmg_machete", "0");

//-----------------------------------------------------------------------------
// CWeaponCrowbar
//-----------------------------------------------------------------------------

class CWeaponMachete : public CWeaponCrowbar
{
	DECLARE_CLASS(CWeaponMachete, CWeaponCrowbar);
public:
	DECLARE_SERVERCLASS();

	float GetDamageForActivity(Activity hitActivity)
	{
		if ((GetOwner() != NULL) && (GetOwner()->IsPlayer()))
			return sk_plr_dmg_machete.GetFloat();

		return sk_npc_dmg_machete.GetFloat();
	}
};

IMPLEMENT_SERVERCLASS_ST(CWeaponMachete, DT_WeaponMachete)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_machete, CWeaponMachete);
PRECACHE_WEAPON_REGISTER(weapon_machete);