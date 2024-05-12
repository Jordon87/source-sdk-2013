//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Crowbar - an old favorite
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "ammodef.h"
#include "mathlib/mathlib.h"
#include "in_buttons.h"
#include "soundent.h"
#include "vstdlib/random.h"
#include "npcevent.h"
#include "ai_basenpc.h"
#include "weapon_crowbar.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// CWeaponCrowbar
//-----------------------------------------------------------------------------

IMPLEMENT_SERVERCLASS_ST(CWeaponCrowbar, DT_WeaponCrowbar)
END_SEND_TABLE()

#ifndef HL2MP
LINK_ENTITY_TO_CLASS( weapon_crowbar, CWeaponCrowbar );
PRECACHE_WEAPON_REGISTER( weapon_crowbar );
#endif

acttable_t CWeaponCrowbar::m_acttable[] = 
{
	{ ACT_MELEE_ATTACK1,	ACT_MELEE_ATTACK_SWING, true },
	{ ACT_IDLE,				ACT_IDLE_ANGRY_MELEE,	false },
	{ ACT_IDLE_ANGRY,		ACT_IDLE_ANGRY_MELEE,	false },
};

IMPLEMENT_ACTTABLE(CWeaponCrowbar);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CWeaponCrowbar::CWeaponCrowbar( void )
{
	m_bReloadsSingly = false;
	m_bFiresUnderwater = true;
}

void CWeaponCrowbar::PrimaryAttack(void)
{

	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner && pOwner->IsPlayer())
	{
		SendWeaponAnim(ACT_VM_PRIMARYATTACK);

		m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
		m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();
	}
}

void CWeaponCrowbar::ItemPostFrame(void)
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
