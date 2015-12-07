//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_baseweapon_shotgun.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponShotgun
//-----------------------------------------------------------------------------
class C1187WeaponShotgun : public C1187_BaseWeapon_Shotgun
{
	DECLARE_CLASS(C1187WeaponShotgun, C1187_BaseWeapon_Shotgun);
public:
	DECLARE_SERVERCLASS();

	virtual bool HasBuiltInFlashlight(void) { return true; }

	virtual void	MeleeSwing(void);

#if defined ( ROGUETRAIN_DLL )
#ifndef CLIENT_DLL
	virtual int				GetIdealSkin() const;
#endif
#endif // defined ( ROGUETRAIN_DLL )
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponShotgun, DT_1187WeaponShotgun)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_shotgun, C1187WeaponShotgun);
PRECACHE_WEAPON_REGISTER(weapon_shotgun);

void C1187WeaponShotgun::MeleeSwing(void)
{
	BaseClass::MeleeSwing();

	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		pPlayer->ViewPunch(QAngle(-random->RandomFloat(2, 4), 0, 0));
	}
}

#if defined ( ROGUETRAIN_DLL )
#ifndef CLIENT_DLL
int C1187WeaponShotgun::GetIdealSkin() const
{
	return 0;
}
#endif // !CLIENT_DLL
#endif // defined ( ROGUETRAIN_DLL )