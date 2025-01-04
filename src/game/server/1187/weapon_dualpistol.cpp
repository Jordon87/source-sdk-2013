//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "te_effect_dispatch.h"
#include "soundent.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// CWeaponDualPistol
//-----------------------------------------------------------------------------
class CWeaponDualPistol : public CBaseHLCombatWeapon
{
	DECLARE_CLASS(CWeaponDualPistol, CBaseHLCombatWeapon);
public:
	DECLARE_SERVERCLASS();

	CWeaponDualPistol();

	virtual const Vector& GetBulletSpread(void)
	{
		static Vector Cone = VECTOR_CONE_5DEGREES;
		static Vector IronsightCone = VECTOR_CONE_15DEGREES;
		if (!IsIronsighted())
		{
			return IronsightCone;
		}
		else
		{
			return Cone;
		}
	}

	void	FinishReload(void);
	void	PrimaryAttack(void);
	void	SecondaryAttack(void);

private:
	int m_iUnk_0x540;
	int m_nNumPrimaryShotsFired;

	void	DoesPrimary(bool a2);
	void	DoesSecondary(bool a2);
};

IMPLEMENT_SERVERCLASS_ST(CWeaponDualPistol, DT_WeaponDualPistol)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dualpistol, CWeaponDualPistol);
PRECACHE_WEAPON_REGISTER(weapon_dualpistol);

CWeaponDualPistol::CWeaponDualPistol()
{
	m_bReloadsSingly = false;
	m_bFiresUnderwater = false;
	m_iUnk_0x540 = 0;
	m_nNumPrimaryShotsFired = 0;
}

void CWeaponDualPistol::FinishReload(void)
{
	BaseClass::FinishReload();
	m_iUnk_0x540 = 0;
	m_nNumPrimaryShotsFired = 0;
}

void CWeaponDualPistol::PrimaryAttack(void)
{
	if (m_iClip1 < 1)
	{
		Reload();
	}

	if (GetMaxClip1() / 2 <= m_nNumPrimaryShotsFired)
	{
		SecondaryAttack();
	}

	m_nNumPrimaryShotsFired = m_nNumPrimaryShotsFired + 1;
	SendWeaponAnim(ACT_VM_SECONDARYATTACK);
	DoesPrimary(true);
	DoesSecondary(false);
}

void CWeaponDualPistol::SecondaryAttack(void)
{
	if (m_iClip1 < 1)
	{
		Reload();
	}

	if (GetMaxClip1() / 2 <= m_iUnk_0x540)
	{
		PrimaryAttack(); 
	}

	m_iUnk_0x540 = m_iUnk_0x540 + 1;
	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	DoesPrimary(false);
	DoesSecondary(true);
}

void CWeaponDualPistol::DoesPrimary(bool a2)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (!pOwner && !pOwner->IsPlayer())
		return;

	CEffectData data;

	data.m_nEntIndex = pOwner->GetViewModel()->entindex();
	int iAttachment;

	if (!a2)
		iAttachment = LookupAttachment("eject3");
	else
		iAttachment = LookupAttachment("eject2");

	Vector vecEject;
	QAngle angEject;

	pOwner->GetViewModel()->GetAttachment(iAttachment, vecEject, angEject);
	data.m_flScale = 1.0f;
	data.m_vOrigin = vecEject;
	data.m_vAngles = angEject;

	DispatchEffect("ShellEject", data);

	int iMuzzleflash;
	if (!a2)
		iMuzzleflash = LookupAttachment("muzzleleft");
	else
		iMuzzleflash = LookupAttachment("muzzleright");

	Vector vecFlash;
	QAngle angFlash;
	pOwner->GetViewModel()->GetAttachment(iMuzzleflash, vecFlash, angFlash);

	CRecipientFilter filter;
	filter.AddRecipientsByPVS(vecFlash);

	te->MuzzleFlash(filter, 0.0f, vecFlash, angFlash, 1.0f, MUZZLEFLASH_TYPE_STRIDER);
}

void CWeaponDualPistol::DoesSecondary(bool a2)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (!pOwner && !pOwner->IsPlayer())
		return;

	WeaponSound(SINGLE);

	if ((m_iUnk_0x540 < GetMaxClip1() / 2) && (m_nNumPrimaryShotsFired < GetMaxClip1() / 2))
	{
		if (!a2)
		{
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.15f;
		}
		m_flNextPrimaryAttack = gpGlobals->curtime + 0.1f;
	}
	else
	{
		m_flNextPrimaryAttack = gpGlobals->curtime + 0.15f; 
	}

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.15f;
	m_iClip1 -= 1;
	
	float flaim;
	if (IsIronsighted())
		flaim = 0.13917311f;
	else
		flaim = 1.0f;

	Vector vecSrc = pOwner->Weapon_ShootPosition();
	Vector vecAiming = pOwner->GetAutoaimVector(flaim);

	FireBulletsInfo_t bulletInfo(1, vecSrc, vecAiming, GetBulletSpread(), MAX_COORD_RANGE, m_iPrimaryAmmoType);
	bulletInfo.m_iTracerFreq = 0;
	bulletInfo.m_pAttacker = pOwner;

	pOwner->FireBullets(bulletInfo);

	CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 600.0f, 0.2, GetOwner());
}
