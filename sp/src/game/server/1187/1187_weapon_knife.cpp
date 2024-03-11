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

	void		PrimaryAttack(void);
	void		ItemPostFrame(void);
};

IMPLEMENT_SERVERCLASS_ST(CWeaponKnife, DT_WeaponKnife)
END_SEND_TABLE()

acttable_t CWeaponKnife::m_acttable[] =
{
	{ ACT_MELEE_ATTACK1,	ACT_MELEE_ATTACK_SWING, true },
	{ ACT_IDLE,				ACT_IDLE_ANGRY_MELEE,	false },
	{ ACT_IDLE_ANGRY,		ACT_IDLE_ANGRY_MELEE,	false },
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
		if ((pOwner->m_afButtonPressed & IN_ATTACK) != 0 || (pOwner->m_afButtonPressed & IN_ATTACK2) != 0 && gpGlobals->curtime >= m_flNextPrimaryAttack)
			PrimaryAttack();
		else
			WeaponIdle();
	}
}
