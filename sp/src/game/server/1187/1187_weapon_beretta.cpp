//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Pistol - hand gun
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_pistol.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//-----------------------------------------------------------------------------
// C1187WeaponBeretta
//-----------------------------------------------------------------------------

class C1187WeaponBeretta : public C1187_BaseWeapon_Pistol
{
	DECLARE_CLASS(C1187WeaponBeretta, C1187_BaseWeapon_Pistol);
public:
	DECLARE_SERVERCLASS();

	virtual void			AddMeleeViewKick( void );
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponBeretta, DT_1187WeaponBeretta)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_pistol, C1187WeaponBeretta);
PRECACHE_WEAPON_REGISTER(weapon_pistol);

void C1187WeaponBeretta::AddMeleeViewKick(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
		return;

	//Disorient the player
	QAngle angles = pPlayer->GetLocalAngles();

	angles.x += random->RandomInt(-4, 4);
	angles.y += random->RandomInt(-4, 4);
	angles.z = 0;

	pPlayer->SnapEyeAngles(angles);

	pPlayer->ViewPunch(QAngle(random->RandomInt(-8, -12), random->RandomInt(1, 2), 0));
}