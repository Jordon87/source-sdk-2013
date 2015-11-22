//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "1187_basecombatweapon.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "gamerules.h"
#include "in_buttons.h"
#include "soundent.h"
#include "game.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "te_effect_dispatch.h"
#include "gamestats.h"

#include "1187_baseweapon_boltaction.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponKar98
//-----------------------------------------------------------------------------

class C1187WeaponKar98 : public C1187_BaseWeapon_BoltAction
{
	DECLARE_CLASS(C1187WeaponKar98, C1187_BaseWeapon_BoltAction);
public:

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void AddViewKick(void);

	virtual void AddMeleeViewKick(void);
	virtual void AddMeleeViewMiss(void);
};

LINK_ENTITY_TO_CLASS(weapon_kar98, C1187WeaponKar98);
PRECACHE_WEAPON_REGISTER(weapon_kar98);

IMPLEMENT_SERVERCLASS_ST(C1187WeaponKar98, DT_1187WeaponKar98)
END_SEND_TABLE()

BEGIN_DATADESC(C1187WeaponKar98)
END_DATADESC()

void C1187WeaponKar98::AddViewKick(void)
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

		pPlayer->ViewPunch(QAngle(-5, random->RandomFloat(-1, 1), 0));
	}
}

void C1187WeaponKar98::AddMeleeViewKick(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		pPlayer->ViewPunch(QAngle(-random->RandomInt(1, 2), random->RandomInt(-1, 1), 0));
	}
}

void C1187WeaponKar98::AddMeleeViewMiss(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer)
	{
		pPlayer->ViewPunch(QAngle(-random->RandomInt(2, 4), random->RandomInt(-1, 1), 0));
	}
}