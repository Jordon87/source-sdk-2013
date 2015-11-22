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

	virtual void		Operator_HandleHitEvent(bool bIsSecondary, CBaseCombatCharacter *pOperator);

	virtual float		GetPrimaryAttackHitDelay(void) const { return 0.1f; }

	float		GetDamageForActivity(Activity hitActivity);

	virtual void		AddMeleeViewKick(void);
	virtual void		AddMeleeViewMiss(void);
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponKnife, DT_1187WeaponKnife)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_knife, C1187WeaponKnife);
PRECACHE_WEAPON_REGISTER(weapon_knife);

void C1187WeaponKnife::Operator_HandleHitEvent(bool bIsSecondary, CBaseCombatCharacter *pOperator)
{
	BaseClass::Operator_HandleHitEvent(bIsSecondary, pOperator);

	CBasePlayer* pPlayer = ToBasePlayer(pOperator);
	if (pPlayer)
	{
		//Disorient the player
		QAngle angles = pPlayer->GetLocalAngles();

		angles.x += random->RandomInt(-4, 4);
		angles.y += random->RandomInt(-4, 4);
		angles.z = 0;

		pPlayer->SnapEyeAngles(angles);

		pPlayer->ViewPunch(QAngle(random->RandomFloat(8, 10), -random->RandomFloat(8, 10), 0));
	}
}


float C1187WeaponKnife::GetDamageForActivity(Activity hitActivity)
{
	if ((GetOwner() != NULL) && (GetOwner()->IsPlayer()))
		return sk_plr_dmg_knife.GetFloat();

	return sk_npc_dmg_knife.GetFloat();
}

void C1187WeaponKnife::AddMeleeViewKick(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		//Disorient the player
		QAngle angles = pPlayer->GetLocalAngles();

		angles.x += random->RandomInt(0, 1);
		angles.y += -random->RandomInt(4, 6);
		angles.z = 0;

		pPlayer->SnapEyeAngles(angles);

		pPlayer->ViewPunch(QAngle(1, -random->RandomFloat(6, 8), 0));
	}
}

void C1187WeaponKnife::AddMeleeViewMiss(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		//Disorient the player
		QAngle angles = pPlayer->GetLocalAngles();

		angles.x += random->RandomInt(0, 1);
		angles.y += -random->RandomInt(6, 8);
		angles.z = 0;

		pPlayer->SnapEyeAngles(angles);

		pPlayer->ViewPunch(QAngle(1, -random->RandomFloat(8, 10), 0));
	}
}