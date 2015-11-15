//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_basecombatweapon.h"
#include "player.h"
#include "gamerules.h"
#include "grenade_frag.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "items.h"
#include "in_buttons.h"
#include "soundent.h"
#include "gamestats.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define HEALTHPACK_RADIUS			4.0f // inches
#define HEALTHPACK_THROW_DELAY		1.4f

extern ConVar sk_healthkit;

//-----------------------------------------------------------------------------
// C1187WeaponHealthPack
//-----------------------------------------------------------------------------
class C1187WeaponHealthPack : public CBase1187CombatWeapon
{
	DECLARE_CLASS(C1187WeaponHealthPack, CBase1187CombatWeapon);
public:
	DECLARE_SERVERCLASS();

public:
	C1187WeaponHealthPack();

	void	Precache(void);
	void	PrimaryAttack(void);
	void	DecrementAmmo(CBaseCombatCharacter *pOwner);
	void	ItemPostFrame(void);

	bool	Deploy(void);
	bool	Holster(CBaseCombatWeapon *pSwitchingTo = NULL);

	int		CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }

	bool	Reload(void);

	bool	ShouldDisplayHUDHint() { return true; }

	virtual float	GetThrowEventDelay(void) { return HEALTHPACK_THROW_DELAY; };

	// Ironsights
	virtual bool	HasIronsights(void) { return false; }

private:
	void	UseHealthPack(CBasePlayer *pPlayer);
	void	ThrowHealthPack(CBasePlayer *pPlayer);
	// check a throw from vecSrc.  If not valid, move the position back along the line to vecEye
	void	CheckThrowPosition(CBasePlayer *pPlayer, const Vector &vecEye, Vector &vecSrc);

	bool	m_bRedraw;	//Draw the weapon again after throwing a grenade

	float	m_flThrowEventTime;
	bool	m_bThrowEvent;

	DECLARE_ACTTABLE();

	DECLARE_DATADESC();
};


BEGIN_DATADESC(C1187WeaponHealthPack)
DEFINE_FIELD(m_bRedraw, FIELD_BOOLEAN),
DEFINE_FIELD(m_flThrowEventTime, FIELD_TIME),
DEFINE_FIELD(m_bThrowEvent, FIELD_BOOLEAN),
END_DATADESC()

acttable_t	C1187WeaponHealthPack::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_SLAM, true },
};

IMPLEMENT_ACTTABLE(C1187WeaponHealthPack);

IMPLEMENT_SERVERCLASS_ST(C1187WeaponHealthPack, DT_1187WeaponHealthPack)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_healthpack, C1187WeaponHealthPack);
PRECACHE_WEAPON_REGISTER(weapon_healthpack);


C1187WeaponHealthPack::C1187WeaponHealthPack() :
CBase1187CombatWeapon(),
m_bRedraw(false), m_bThrowEvent(false), m_flThrowEventTime(0.0f)
{
	NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187WeaponHealthPack::Precache(void)
{
	BaseClass::Precache();

	UTIL_PrecacheOther("item_ammo_healthpack");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C1187WeaponHealthPack::Deploy(void)
{
	m_bRedraw = false;
	m_bThrowEvent = false;
	m_flThrowEventTime = 0.0f;

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C1187WeaponHealthPack::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	m_bRedraw = false;
	m_bThrowEvent = false;
	m_flThrowEventTime = 0.0f;

	return BaseClass::Holster(pSwitchingTo);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool C1187WeaponHealthPack::Reload(void)
{
	bool bRet = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_DRAW);

	if ( bRet )
	{
		//Update our times
		m_flNextPrimaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
		m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
		m_flTimeWeaponIdle = gpGlobals->curtime + GetViewModelSequenceDuration();

		//Mark this as done
		m_bRedraw = false;
	}

	return bRet;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187WeaponHealthPack::PrimaryAttack(void)
{
	if (m_bRedraw)
		return;

	if (m_iClip1 <= 0)
		return;

	CBaseCombatCharacter *pOwner = GetOwner();

	if (pOwner == NULL)
		return;

	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
		return;

	if (pPlayer->GetHealth() >= pPlayer->GetMaxHealth())
		return;

	// Note that this is a primary attack and prepare the grenade attack to pause.
	SendWeaponAnim(ACT_VM_THROW);

	m_bRedraw				= true;

	m_flTimeWeaponIdle		= FLT_MAX;
	m_flNextPrimaryAttack	= gpGlobals->curtime + GetViewModelSequenceDuration();
	m_flThrowEventTime		= gpGlobals->curtime + GetThrowEventDelay();
	m_bThrowEvent			= true;

	// If I'm now out of ammo, switch away
	if (!HasPrimaryAmmo())
	{
		pPlayer->SwitchToNextBestWeapon(this);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOwner - 
//-----------------------------------------------------------------------------
void C1187WeaponHealthPack::DecrementAmmo(CBaseCombatCharacter *pOwner)
{
	// pOwner->RemoveAmmo(1, m_iPrimaryAmmoType);
	m_iClip1--;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C1187WeaponHealthPack::ItemPostFrame(void)
{
	BaseClass::ItemPostFrame();

	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
	if (pPlayer)
	{
		if (m_bThrowEvent && m_flThrowEventTime != 0.0f && m_flThrowEventTime < gpGlobals->curtime)
		{
			UseHealthPack(pPlayer);
			//ThrowHealthPack(pPlayer);
		}
	}

	if (m_bRedraw)
	{
		if (IsViewModelSequenceFinished())
		{
			Reload();
		}
	}
}

// check a throw from vecSrc.  If not valid, move the position back along the line to vecEye
void C1187WeaponHealthPack::CheckThrowPosition(CBasePlayer *pPlayer, const Vector &vecEye, Vector &vecSrc)
{
	trace_t tr;

	UTIL_TraceHull(vecEye, vecSrc, -Vector(HEALTHPACK_RADIUS + 2, HEALTHPACK_RADIUS + 2, HEALTHPACK_RADIUS + 2), Vector(HEALTHPACK_RADIUS + 2, HEALTHPACK_RADIUS + 2, HEALTHPACK_RADIUS + 2),
		pPlayer->PhysicsSolidMaskForEntity(), pPlayer, pPlayer->GetCollisionGroup(), &tr);

	if (tr.DidHit())
	{
		vecSrc = tr.endpos;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOwner - 
//-----------------------------------------------------------------------------
void C1187WeaponHealthPack::UseHealthPack(CBasePlayer *pPlayer)
{
	if (!pPlayer)
		return;

	if (pPlayer->TakeHealth(sk_healthkit.GetFloat(), DMG_GENERIC))
	{
		// player 'shoot' animation.
		pPlayer->SetAnimation( PLAYER_ATTACK1 );

		// Player heal sound.
		WeaponSound(SINGLE);

		// Remove a healthpack.
		m_iClip1--;
	}

	m_bThrowEvent = false;
	m_flThrowEventTime = 0.0f;

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired(pPlayer, true, GetClassname());
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void C1187WeaponHealthPack::ThrowHealthPack(CBasePlayer *pPlayer)
{
	Vector	vecEye = pPlayer->EyePosition();
	Vector	vForward, vRight;

	pPlayer->EyeVectors(&vForward, &vRight, NULL);
	Vector vecSrc = vecEye + vForward * 18.0f + vRight * 8.0f + Vector(0, 0, -8);
	CheckThrowPosition(pPlayer, vecEye, vecSrc);

	Vector vecThrow;
	pPlayer->GetVelocity(&vecThrow, NULL);
	vecThrow += vForward * 350 + Vector(0, 0, 50);

	// create a healthkit and toss it into the world
	CBaseEntity *pHealthKit = CreateEntityByName("item_ammo_healthpack");
	Assert(pHealthKit);
	if (pHealthKit)
	{
		pHealthKit->SetAbsOrigin(vecSrc);
		pHealthKit->SetOwnerEntity(this);
		// pHealthKit->SetAbsVelocity( tossVelocity );
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
				pPhysicsObject->SetVelocity(&vecThrow, &angDummy);
			}
		}
	}

	// player 'shoot' animation.
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	// Remove a healthpack.
	m_iClip1--;

	m_bThrowEvent = false;
	m_flThrowEventTime = 0.0f;

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired(pPlayer, true, GetClassname());
}