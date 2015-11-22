//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Pistol - hand gun
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_missilelauncher.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponLAW
//-----------------------------------------------------------------------------
class C1187WeaponLAW : public C1187_BaseWeapon_MissileLauncher
{
	DECLARE_CLASS(C1187WeaponLAW, C1187_BaseWeapon_MissileLauncher);
public:
	DECLARE_SERVERCLASS();

	virtual void AddMeleeViewKick(void);
	virtual void AddMeleeViewMiss(void);
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponLAW, DT_1187WeaponLAW)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_rpg, C1187WeaponLAW);
PRECACHE_WEAPON_REGISTER(weapon_rpg);

void C1187WeaponLAW::AddMeleeViewKick(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		//Disorient the player
		QAngle angles = pPlayer->GetLocalAngles();

		angles.x += random->RandomInt(-1, 1);
		angles.y += random->RandomInt(-1, 1);
		angles.z = 0;

		pPlayer->SnapEyeAngles(angles);

		pPlayer->ViewPunch(QAngle(random->RandomFloat(1, 2), random->RandomFloat(1, 2), 0));
	}
}

void C1187WeaponLAW::AddMeleeViewMiss(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		//Disorient the player
		QAngle angles = pPlayer->GetLocalAngles();

		angles.x += random->RandomInt(-2, 2);
		angles.y += random->RandomInt(-2, 2);
		angles.z = 0;

		pPlayer->SnapEyeAngles(angles);

		pPlayer->ViewPunch(QAngle(random->RandomFloat(2, 4), random->RandomFloat(2, 4), 0));
	}
}