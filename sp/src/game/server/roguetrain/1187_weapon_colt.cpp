//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 1187 colt weapon.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_pistol.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//-----------------------------------------------------------------------------
// C1187WeaponColt
//-----------------------------------------------------------------------------

class C1187WeaponColt : public C1187_BaseWeapon_Pistol
{
	DECLARE_CLASS(C1187WeaponColt, C1187_BaseWeapon_Pistol);
public:
	DECLARE_SERVERCLASS();

	virtual void			AddMeleeViewKick(void);
	virtual void			AddMeleeViewMiss(void);

	virtual Activity		GetPrimaryAttackActivity(void);

#ifndef CLIENT_DLL
	virtual int				GetIdealSkin() const;
#endif
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponColt, DT_1187WeaponColt)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_colt, C1187WeaponColt);
PRECACHE_WEAPON_REGISTER(weapon_colt);

void C1187WeaponColt::AddMeleeViewKick(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		pPlayer->ViewPunch(QAngle(-random->RandomInt(6, 8), random->RandomInt(1, 2), 0));
	}
}

void C1187WeaponColt::AddMeleeViewMiss(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		pPlayer->ViewPunch(QAngle(-random->RandomInt(8, 10), random->RandomInt(1, 2), 0));
	}
}

Activity C1187WeaponColt::GetPrimaryAttackActivity(void)
{
	return ACT_VM_PRIMARYATTACK;
}

#ifndef CLIENT_DLL
int	C1187WeaponColt::GetIdealSkin() const
{
	// Colt has only one skin.
	return 0;
}
#endif // !CLIENT_DLL