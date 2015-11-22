//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#include "gamerules.h"		// For g_pGameRules
#include "in_buttons.h"
#include "soundent.h"
#include "vstdlib/random.h"
#include "gamestats.h"
#include "1187_baseweapon_dualsmg.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_DATADESC( C1187_BaseWeapon_DualSMG )
	DEFINE_FIELD(m_iClipLeft, FIELD_INTEGER),
	DEFINE_FIELD(m_iClipRight, FIELD_INTEGER),
	DEFINE_FIELD(m_iMaxClipLeft, FIELD_INTEGER),
	DEFINE_FIELD(m_iMaxClipRight, FIELD_INTEGER),
END_DATADESC()

C1187_BaseWeapon_DualSMG::C1187_BaseWeapon_DualSMG()
{
	m_iClipLeft = 0;
	m_iClipRight = 0;

	m_iMaxClipLeft = 0;
	m_iMaxClipRight = 0;
}


int C1187_BaseWeapon_DualSMG::GetMaxClipLeft() const
{
	return GetMaxClip1() / 2;
}

int C1187_BaseWeapon_DualSMG::GetMaxClipRight() const
{
	return GetMaxClip1() / 2;
}

bool C1187_BaseWeapon_DualSMG::Deploy(void)
{
	bool bRet = BaseClass::Deploy();
	if (bRet)
	{
		m_iMaxClipLeft = GetMaxClipLeft();
		m_iMaxClipRight = GetMaxClipRight();

		if (m_iClip1 > GetMaxClipRight())
		{
			m_iClipRight = m_iClip1 - m_iMaxClipRight;
			m_iClipLeft = m_iClip1 - m_iClipRight;
		}
		else
		{
			m_iClipRight = m_iClip1;
			m_iClipLeft = 0;
		}
	}

	return bRet;
}

//-----------------------------------------------------------------------------
// Purpose: Reload has finished.
//-----------------------------------------------------------------------------
void C1187_BaseWeapon_DualSMG::FinishReload(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();

	if (pOwner)
	{
		// If I use primary clips, reload primary
		if (UsesClipsForAmmo1())
		{
			int primary = MIN(GetMaxClip1() - m_iClip1, pOwner->GetAmmoCount(m_iPrimaryAmmoType));
			m_iClip1 += primary;
			pOwner->RemoveAmmo(primary, m_iPrimaryAmmoType);
		}

		// If I use secondary clips, reload secondary
		if (UsesClipsForAmmo2())
		{
			int secondary = MIN(GetMaxClip2() - m_iClip2, pOwner->GetAmmoCount(m_iSecondaryAmmoType));
			m_iClip2 += secondary;
			pOwner->RemoveAmmo(secondary, m_iSecondaryAmmoType);
		}

		if (m_bReloadsSingly)
		{
			m_bInReload = false;
		}

		// Adjust clips for both weapons.
		if (m_iClip1 > GetMaxClipRight())
		{
			m_iClipRight = m_iClip1 - m_iMaxClipRight;
			m_iClipLeft = m_iClip1 - m_iClipRight;
		}
		else
		{
			m_iClipRight = m_iClip1;
			m_iClipLeft = 0;
		}
	}
}

void C1187_BaseWeapon_DualSMG::ItemPostFrame(void)
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	UpdateAutoFire();

	//Track the duration of the fire
	//FIXME: Check for IN_ATTACK2 as well?
	//FIXME: What if we're calling ItemBusyFrame?
	m_fFireDuration = (pOwner->m_nButtons & IN_ATTACK) ? (m_fFireDuration + gpGlobals->frametime) : 0.0f;

	if (UsesClipsForAmmo1())
	{
		CheckReload();
	}

	bool bFired = false;

	// Secondary attack has priority
	if (!bFired && (pOwner->m_nButtons & IN_ATTACK2) && IsSecondaryAttackAllowed() && (m_flNextSecondaryAttack <= gpGlobals->curtime))
	{
		if (UsesSecondaryAmmo() && pOwner->GetAmmoCount(m_iSecondaryAmmoType) <= 0)
		{
			if (m_flNextEmptySoundTime < gpGlobals->curtime)
			{
				WeaponSound(EMPTY);
				m_flNextSecondaryAttack = m_flNextEmptySoundTime = gpGlobals->curtime + 0.5;
			}
		}
		else if (pOwner->GetWaterLevel() == 3 && m_bAltFiresUnderwater == false)
		{
			// This weapon doesn't fire underwater
			WeaponSound(EMPTY);
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
			return;
		}
		else
		{
			// FIXME: This isn't necessarily true if the weapon doesn't have a secondary fire!
			// For instance, the crossbow doesn't have a 'real' secondary fire, but it still 
			// stops the crossbow from firing on the 360 if the player chooses to hold down their
			// zoom button. (sjb) Orange Box 7/25/2007
#if !defined(CLIENT_DLL)
			if (!IsX360() || !ClassMatches("weapon_crossbow"))
#endif
			{
				bFired = ShouldBlockPrimaryFire();
			}

			if (m_iClipLeft > 0)
			{
				SecondaryAttack();
			}
			else 
			{
				if (m_iClipRight > 0)
				{
					PrimaryAttack();
				}
				else
				{
					Reload();
					m_fFireDuration = 0.0f;
					return;
				}
			}

			// Secondary ammo doesn't have a reload animation
			if (UsesClipsForAmmo2())
			{
				// reload clip2 if empty
				if (m_iClip2 < 1)
				{
					pOwner->RemoveAmmo(1, m_iSecondaryAmmoType);
					m_iClip2 = m_iClip2 + 1;
				}
			}
		}
	}

	if (!bFired && (pOwner->m_nButtons & IN_ATTACK) && IsPrimaryAttackAllowed() && (m_flNextPrimaryAttack <= gpGlobals->curtime))
	{
		// Clip empty? Or out of ammo on a no-clip weapon?
		if (!IsMeleeWeapon() &&
			((UsesClipsForAmmo1() && m_iClip1 <= 0) || (!UsesClipsForAmmo1() && pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)))
		{
			HandleFireOnEmpty();
		}
		else if (pOwner->GetWaterLevel() == 3 && m_bFiresUnderwater == false)
		{
			// This weapon doesn't fire underwater
			WeaponSound(EMPTY);
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
			return;
		}
		else
		{
			//NOTENOTE: There is a bug with this code with regards to the way machine guns catch the leading edge trigger
			//			on the player hitting the attack key.  It relies on the gun catching that case in the same frame.
			//			However, because the player can also be doing a secondary attack, the edge trigger may be missed.
			//			We really need to hold onto the edge trigger and only clear the condition when the gun has fired its
			//			first shot.  Right now that's too much of an architecture change -- jdw

			// If the firing button was just pressed, or the alt-fire just released, reset the firing time
			if ((pOwner->m_afButtonPressed & IN_ATTACK) || (pOwner->m_afButtonReleased & IN_ATTACK2))
			{
				m_flNextPrimaryAttack = gpGlobals->curtime;
			}

			if (m_iClipRight > 0)
			{
				PrimaryAttack();
			}
			else
			{
				if (m_iClipLeft > 0)
				{
					SecondaryAttack();
				}
				else
				{
					Reload();
					m_fFireDuration = 0.0f;
					return;
				}
			}

			if (AutoFiresFullClip())
			{
				m_bFiringWholeClip = true;
			}

#ifdef CLIENT_DLL
			pOwner->SetFiredWeapon(true);
#endif
		}
	}

	// -----------------------
	//  Reload pressed / Clip Empty
	// -----------------------
	if ((pOwner->m_nButtons & IN_RELOAD) && UsesClipsForAmmo1() && !m_bInReload)
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
		m_fFireDuration = 0.0f;
	}

	// -----------------------
	//  No buttons down
	// -----------------------
	if (!((pOwner->m_nButtons & IN_ATTACK) || (pOwner->m_nButtons & IN_ATTACK2) || (CanReload() && pOwner->m_nButtons & IN_RELOAD)))
	{
		// no fire buttons down or reloading
		if (!ReloadOrSwitchWeapons() && (m_bInReload == false))
		{
			WeaponIdle();
		}
	}
}

void C1187_BaseWeapon_DualSMG::PrimaryAttack(void)
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
		return;

	// player "shoot" animation
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	SendWeaponAnim(GetPrimaryAttackActivity());

	pPlayer->SetMuzzleFlashTime(gpGlobals->curtime + 1.0);

	Vector	vecAiming = pPlayer->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT);

	// Fire bullet.
	DoFire(pPlayer, vecAiming);

	AddViewKickRight();

	// Set next fire time.
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetFireRate();

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired(pPlayer, true, GetClassname());
}

void C1187_BaseWeapon_DualSMG::SecondaryAttack(void)
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
		return;

	// player "shoot" animation
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	SendWeaponAnim(GetSecondaryAttackActivity());

	pPlayer->SetMuzzleFlashTime(gpGlobals->curtime + 1.0);

	Vector	vecAiming = pPlayer->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT);

	// Fire bullet.
	DoFire(pPlayer, vecAiming, true);

	AddViewKickLeft();

	// pPlayer->ViewPunch(QAngle(random->RandomFloat(-2, -1), random->RandomFloat(-2, 2), 0));

	// Set next fire time.
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetFireRate();

	m_iSecondaryAttacks++;
	gamestats->Event_WeaponFired(pPlayer, false, GetClassname());
}

void C1187_BaseWeapon_DualSMG::DoFire(CBaseCombatCharacter* pOperator, Vector vecAiming,bool bSecondary)
{
	if (!pOperator)
		return;

	// MUST call sound before removing a round from the clip of a CMachineGun
	WeaponSound(SINGLE);

	pOperator->DoMuzzleFlash();

	// Decrement clip.
	m_iClip1 -= 1;

	Vector	vecSrc = pOperator->Weapon_ShootPosition();

	// Fire the bullets, and force the first shot to be perfectly accuracy
	pOperator->FireBullets(1, vecSrc, vecAiming, GetBulletSpread(), MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 0, -1, -1, 0, NULL, true, true);

	CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), SOUNDENT_VOLUME_SHOTGUN, 0.2, GetOwner());

	if (!bSecondary)
	{
		if (m_iClip1)
			m_iClipRight--;
	}
	else
	{
		if (m_iClip1)
			m_iClipLeft--;
	}
}