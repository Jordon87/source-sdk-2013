//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_melee.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sk_plr_dmg_knife;
extern ConVar sk_npc_dmg_knife;

//-----------------------------------------------------------------------------
// C1187WeaponKnife
//-----------------------------------------------------------------------------
class C1187WeaponKnife : public C1187_BaseWeapon_Melee
{
	DECLARE_CLASS(C1187WeaponKnife, C1187_BaseWeapon_Melee);
public:
	DECLARE_SERVERCLASS();

	float		GetDamageForActivity(Activity hitActivity);
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponKnife, DT_1187WeaponKnife)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_knife, C1187WeaponKnife);
PRECACHE_WEAPON_REGISTER(weapon_knife);

float C1187WeaponKnife::GetDamageForActivity(Activity hitActivity)
{
	if ((GetOwner() != NULL) && (GetOwner()->IsPlayer()))
		return sk_plr_dmg_knife.GetFloat();

	return sk_npc_dmg_knife.GetFloat();
}
