//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_revolver.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponPPC
//-----------------------------------------------------------------------------
class C1187WeaponPPC : public C1187_BaseWeapon_Revolver
{
	DECLARE_CLASS(C1187WeaponPPC, C1187_BaseWeapon_Revolver);
public:
	DECLARE_SERVERCLASS();

	virtual void AddViewKick(void);
	virtual void AddMeleeViewKick(void);
	virtual void AddMeleeViewMiss(void);
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponPPC, DT_1187WeaponPPC)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_357, C1187WeaponPPC);
PRECACHE_WEAPON_REGISTER(weapon_357);

void C1187WeaponPPC::AddViewKick(void)
{
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );

	if (pPlayer)
	{
		//Disorient the player
		QAngle angles = pPlayer->GetLocalAngles();

		angles.x += random->RandomInt(-1, 1);
		angles.y += random->RandomInt(-1, 1);
		angles.z = 0;

		pPlayer->SnapEyeAngles(angles);

		pPlayer->ViewPunch(QAngle(-8, random->RandomFloat(-2, 2), 0));
	}
}

void C1187WeaponPPC::AddMeleeViewKick(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		//Disorient the player
		QAngle angles = pPlayer->GetLocalAngles();

		angles.x +=  random->RandomInt(-1, 1);
		angles.y += -random->RandomInt(6, 8);
		angles.z = 0;

		pPlayer->SnapEyeAngles(angles);

		pPlayer->ViewPunch(QAngle(-random->RandomInt(1, 2), -random->RandomInt(8, 12), 0));
	}
}

void C1187WeaponPPC::AddMeleeViewMiss(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		//Disorient the player
		QAngle angles = pPlayer->GetLocalAngles();

		angles.x +=  random->RandomInt(-1, 1);
		angles.y += -random->RandomInt(10, 12);
		angles.z = 0;

		pPlayer->SnapEyeAngles(angles);

		pPlayer->ViewPunch(QAngle(-random->RandomInt(1, 2), -random->RandomInt(12, 14), 0));
	}
}