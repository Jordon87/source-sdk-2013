//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// C1187WeaponKnife
//-----------------------------------------------------------------------------
class CWeaponKnife : public CBaseHLCombatWeapon
{
	DECLARE_CLASS(CWeaponKnife, CBaseHLCombatWeapon);
public:
	DECLARE_SERVERCLASS();
	DECLARE_ACTTABLE();

	CWeaponKnife();

	bool		HasAnyAmmo() { return true; }
	bool		HasIronsights() { return false; }

	void		PrimaryAttack(void);
	void		ItemPostFrame(void);
};

IMPLEMENT_SERVERCLASS_ST(CWeaponKnife, DT_WeaponKnife)
END_SEND_TABLE()

acttable_t CWeaponKnife::m_acttable[] =
{
	{ ACT_MELEE_ATTACK1, ACT_MELEE_ATTACK_SWING, true },
	{ ACT_IDLE, ACT_IDLE_ANGRY_MELEE, false },
	{ ACT_IDLE_ANGRY, ACT_IDLE_ANGRY_MELEE, false },
};

IMPLEMENT_ACTTABLE(CWeaponKnife);

LINK_ENTITY_TO_CLASS(weapon_knife, CWeaponKnife);
PRECACHE_WEAPON_REGISTER(weapon_knife);

CWeaponKnife::CWeaponKnife()
{
	m_bReloadsSingly = false;
	m_bFiresUnderwater = true;
}

void CWeaponKnife::PrimaryAttack(void)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner && pOwner->IsPlayer())
	{
		SendWeaponAnim(ACT_VM_PRIMARYATTACK);

		m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
		m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();
	}
}

void CWeaponKnife::ItemPostFrame(void)
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner && pOwner->IsPlayer())
	{
		if ((pOwner->m_afButtonPressed & IN_JUMP) != 0)
		{
			if (m_flNextPrimaryAttack - 0.1f <= gpGlobals->curtime && m_flNextSecondaryAttack - 0.1f <= gpGlobals->curtime)
			{
				SendWeaponAnim(ACT_JUMP);
				m_flNextPrimaryAttack = gpGlobals->curtime + 0.64f;
				m_flNextPrimaryAttack = gpGlobals->curtime + 0.64f;
				return;
			}
		}

		if ((pOwner->m_afButtonPressed & IN_DUCK) != 0 && (pOwner->m_nButtons & IN_SPEED) == 0 && GetActivity() == ACT_VM_IDLE)
		{
			if (m_flNextPrimaryAttack - 0.2f <= gpGlobals->curtime && m_flNextSecondaryAttack - 0.2f <= gpGlobals->curtime)
			{
				SendWeaponAnim(ACT_CROUCH);
				m_flNextPrimaryAttack = gpGlobals->curtime + 0.34f;
				m_flNextSecondaryAttack = gpGlobals->curtime + 0.34f;
				return;
			}
		}

		if ((pOwner->m_nButtons & IN_MELEE) != 0 && GetActivity() == ACT_VM_IDLE)
		{
			if (m_flNextPrimaryAttack - 0.2f <= gpGlobals->curtime && m_flNextSecondaryAttack - 0.2f <= gpGlobals->curtime)
			{
				DisableIronsights();
				SendWeaponAnim(ACT_VM_SWINGHARD);
				m_flNextPrimaryAttack = gpGlobals->curtime + 5.0f;
				m_flNextSecondaryAttack = gpGlobals->curtime + 5.0f;
				return;
			}
		}

		if ((pOwner->m_nButtons & IN_FRAG) != 0 && GetActivity() == ACT_VM_IDLE)
		{
			if (gpGlobals->curtime >= m_flNextPrimaryAttack && gpGlobals->curtime >= m_flNextSecondaryAttack)
			{
				if (pOwner->GetAmmoCount("grenade") > 0)
				{
					DisableIronsights();
					SendWeaponAnim(ACT_VM_HAULBACK);
					m_flNextPrimaryAttack = gpGlobals->curtime + 5.0f;
					m_flNextSecondaryAttack = gpGlobals->curtime + 5.0f;
					return;
				}
			}
		}

		if ((pOwner->m_afButtonPressed & IN_ATTACK) != 0 || (pOwner->m_afButtonPressed & IN_ATTACK2) != 0 && gpGlobals->curtime >= m_flNextPrimaryAttack)
			PrimaryAttack();
		else
			WeaponIdle();
	}
}