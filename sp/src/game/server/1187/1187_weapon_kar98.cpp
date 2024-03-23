//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "gamestats.h"
#include "soundent.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// CWeaponKar98
//-----------------------------------------------------------------------------

class CWeaponKar98 : public CBaseHLCombatWeapon
{
	DECLARE_CLASS(CWeaponKar98, CBaseHLCombatWeapon);
public:

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual const Vector& GetBulletSpread(void)
	{
		static Vector Cone = VECTOR_CONE_1DEGREES;
		static Vector Cone2 = VECTOR_CONE_10DEGREES;

		if (m_bIsIronsighted)
		{
			return Cone;
		}
		else
		{
			return Cone2;
		}
	}

	virtual void PrimaryAttack(void);

private:
	void FUN_1037dc30();
};

LINK_ENTITY_TO_CLASS(weapon_kar98, CWeaponKar98);
PRECACHE_WEAPON_REGISTER(weapon_kar98);

IMPLEMENT_SERVERCLASS_ST(CWeaponKar98, DT_WeaponKar98)
END_SEND_TABLE()

BEGIN_DATADESC(CWeaponKar98)
END_DATADESC()

void CWeaponKar98::PrimaryAttack(void)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner && pOwner->IsPlayer())
	{
		if (m_iClip1 > 0)
		{
			++m_iPrimaryAttacks;
			gamestats->Event_WeaponFired(pOwner, true, GetClassname());
		
			WeaponSound(SINGLE);
			WeaponAutoAimScale();
			SendWeaponAnim(ACT_VM_PRIMARYATTACK);

			m_flNextPrimaryAttack = SequenceDuration() + gpGlobals->curtime;
			m_flNextSecondaryAttack = SequenceDuration() + gpGlobals->curtime;

			m_iClip1 -= 1;
			float flAim;
			if (IsIronsighted())
				flAim = 0.13917311f;
			else
				flAim = 1.0f;

			FireBulletsInfo_t info;
			info.m_iShots = 1;
			info.m_vecSrc = pOwner->Weapon_ShootPosition();
			info.m_vecDirShooting = pOwner->GetAutoaimVector(flAim);
			info.m_vecSpread = pOwner->GetAttackSpread(this);
			info.m_flDistance = MAX_TRACE_LENGTH;
			info.m_iAmmoType = m_iPrimaryAmmoType;
			info.m_iTracerFreq = 0;
			FireBullets(info);

			pOwner->SetMuzzleFlashTime(gpGlobals->curtime + 0.5f);

			QAngle angPunch = QAngle(-10.0f, random->RandomFloat(-2.0,2.0), 0.0f);
			pOwner->ViewPunch(angPunch);

			CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pOwner);

			if (!m_iClip1 && pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
			{
				// HEV suit - indicate out of ammo condition
				pOwner->SetSuitUpdate("!HEV_AMO0", FALSE, 0);
			}

			if (m_iClip1 <= 0)
				Reload();
			else
				FUN_1037dc30();
		}
		else
		{
			if (m_bFireOnEmpty)
			{
				WeaponSound(EMPTY);
				m_flNextPrimaryAttack += 0.15f;
			}
			else
			{
				Reload();
			}
		}
	}
}

void CWeaponKar98::FUN_1037dc30()
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner)
	{
		WeaponSound(SPECIAL1);

		if (!IsIronsighted())
			SendWeaponAnim(ACT_VM_PULLPIN);
		else
			SendWeaponAnim(ACT_VM_RELEASE);

		pOwner->SetNextAttack(gpGlobals->curtime + SequenceDuration());
		m_flNextPrimaryAttack = SequenceDuration() + gpGlobals->curtime;

	}
}
