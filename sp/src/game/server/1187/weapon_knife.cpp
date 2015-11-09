//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "weapon_crowbar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sk_plr_dmg_knife;
extern ConVar sk_npc_dmg_knife;

class CWeaponKnife : public CWeaponCrowbar
{
	DECLARE_CLASS(CWeaponKnife, CWeaponCrowbar);
public:
	DECLARE_SERVERCLASS();

	float		GetDamageForActivity(Activity hitActivity);
};

IMPLEMENT_SERVERCLASS_ST(CWeaponKnife, DT_WeaponKnife)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_knife, CWeaponKnife);
PRECACHE_WEAPON_REGISTER(weapon_knife);

float CWeaponKnife::GetDamageForActivity(Activity hitActivity)
{
	if ((GetOwner() != NULL) && (GetOwner()->IsPlayer()))
		return sk_plr_dmg_knife.GetFloat();

	return sk_npc_dmg_knife.GetFloat();
}
