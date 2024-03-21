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
// CWeaponColt
//-----------------------------------------------------------------------------

class CWeaponColt : public CBaseHLCombatWeapon
{
	DECLARE_CLASS(CWeaponColt, CBaseHLCombatWeapon);
public:

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual const Vector& GetBulletSpread(void)
	{
		static Vector Cone = VECTOR_CONE_2DEGREES;
		static Vector Cone2 = VECTOR_CONE_6DEGREES;

		if (m_bIsIronsighted)
		{
			return Cone;
		}
		else
		{
			return Cone2;
		}
	}

	virtual float	WeaponAutoAimScale() { return 0.8f; };
	virtual void PrimaryAttack(void);

};

LINK_ENTITY_TO_CLASS(weapon_colt, CWeaponColt);
PRECACHE_WEAPON_REGISTER(weapon_colt);

IMPLEMENT_SERVERCLASS_ST(CWeaponColt, DT_WeaponColt)
END_SEND_TABLE()

BEGIN_DATADESC(CWeaponColt)
END_DATADESC()

void CWeaponColt::PrimaryAttack(void)
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

			FireBulletsInfo_t info;
			info.m_iShots = 1;
			info.m_vecSrc = pOwner->Weapon_ShootPosition();
			info.m_vecDirShooting = pOwner->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT);
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
}
