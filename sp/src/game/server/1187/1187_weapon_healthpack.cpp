//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "1187_basecombatweapon.h"
#include "player.h"
#include "gamerules.h"
#include "vstdlib/random.h"
#include "gamestats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponHealthPack
//-----------------------------------------------------------------------------
class C1187WeaponHealthPack : public CBase1187CombatWeapon
{
	DECLARE_CLASS(C1187WeaponHealthPack, CBase1187CombatWeapon);
public:
	DECLARE_SERVERCLASS();

	void Precache(void);
	virtual void PrimaryAttack(void);
};

IMPLEMENT_SERVERCLASS_ST(C1187WeaponHealthPack, DT_1187WeaponHealthPack)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_healthpack, C1187WeaponHealthPack);
PRECACHE_WEAPON_REGISTER(weapon_healthpack);

void C1187WeaponHealthPack::Precache(void)
{
	BaseClass::Precache();

	UTIL_PrecacheOther("item_healthkit");
}

void C1187WeaponHealthPack::PrimaryAttack(void)
{
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
	if (!pPlayer)
		return;

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	// player "shoot" animation
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	m_iClip1 -= 1;

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired(pPlayer, true, GetClassname());

	Vector origin, forward, right, up;
	pPlayer->EyePositionAndVectors(&origin, &forward, &right, &up);

	Vector tossVelocity;

	pPlayer->GetVelocity(&tossVelocity, NULL);

	CBaseEntity *pHealthKit = CreateEntityByName("item_healthkit");
	Assert(pHealthKit);
	if (pHealthKit)
	{
		pHealthKit->SetAbsOrigin(origin);
		pHealthKit->SetOwnerEntity(this);
		DispatchSpawn(pHealthKit);
		{
			IPhysicsObject *pPhysicsObject = pHealthKit->VPhysicsGetObject();
			Assert(pPhysicsObject);
			if (pPhysicsObject)
			{
				unsigned int cointoss = random->RandomInt(0, 0xFF); // int bits used for bools

				// some random precession
				Vector angDummy(random->RandomFloat(-200, 200), random->RandomFloat(-200, 200),
					cointoss & 0x01 ? random->RandomFloat(200, 600) : -1.0f * random->RandomFloat(200, 600));
				pPhysicsObject->SetVelocity(&tossVelocity, &angDummy);
			}
		}
	}
}