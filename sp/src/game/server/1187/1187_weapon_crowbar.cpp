//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_melee.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sk_plr_dmg_crowbar;
extern ConVar sk_npc_dmg_crowbar;

//-----------------------------------------------------------------------------
// C1187WeaponCrowbar
//-----------------------------------------------------------------------------
class C1187WeaponCrowbar : public C1187_BaseWeapon_Melee
{
	DECLARE_CLASS(C1187WeaponCrowbar, C1187_BaseWeapon_Melee);
public:
	DECLARE_SERVERCLASS();

	virtual void		Operator_HandleHitEvent(bool bIsSecondary, CBaseCombatCharacter *pOperator);

	virtual float		GetPrimaryAttackHitDelay(void) const { return 0.5f; }

	float				GetDamageForActivity(Activity hitActivity);
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponCrowbar, DT_1187WeaponCrowbar)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_crowbar, C1187WeaponCrowbar);
PRECACHE_WEAPON_REGISTER(weapon_crowbar);

void C1187WeaponCrowbar::Operator_HandleHitEvent(bool bIsSecondary, CBaseCombatCharacter *pOperator)
{
	BaseClass::Operator_HandleHitEvent(bIsSecondary, pOperator);

	CBasePlayer* pPlayer = ToBasePlayer( pOperator );
	if (pPlayer)
	{
		//Disorient the player
		QAngle angles = pPlayer->GetLocalAngles();

		angles.x += random->RandomInt(-4, 4);
		angles.y += random->RandomInt(-4, 4);
		angles.z = 0;

		pPlayer->SnapEyeAngles(angles);

		pPlayer->ViewPunch(QAngle(random->RandomFloat(8, 10), random->RandomFloat(8, 10), 0));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get the damage amount for the animation we're doing
// Input  : hitActivity - currently played activity
// Output : Damage amount
//-----------------------------------------------------------------------------
float C1187WeaponCrowbar::GetDamageForActivity(Activity hitActivity)
{
	if ((GetOwner() != NULL) && (GetOwner()->IsPlayer()))
		return sk_plr_dmg_crowbar.GetFloat();

	return sk_npc_dmg_crowbar.GetFloat();
}