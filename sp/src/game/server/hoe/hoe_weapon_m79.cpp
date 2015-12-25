//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "hoe_basecombatweapon.h"
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
#include "grenade_ar2.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sk_plr_dmg_smg1_grenade;

//-----------------------------------------------------------------------------
// CWeapon357
//-----------------------------------------------------------------------------

class CHoe_Weapon_M79 : public CHoe_BaseCombatWeapon
{
	DECLARE_CLASS(CHoe_Weapon_M79, CHoe_BaseCombatWeapon);
public:

	CHoe_Weapon_M79(void);

	void	Precache(void);
	void	PrimaryAttack(void);
	float	WeaponAutoAimScale()	{ return 0.6f; }

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(weapon_m79, CHoe_Weapon_M79);
PRECACHE_WEAPON_REGISTER(weapon_m79);

IMPLEMENT_SERVERCLASS_ST(CHoe_Weapon_M79, DT_Hoe_Weapon_M79)
END_SEND_TABLE()

BEGIN_DATADESC(CHoe_Weapon_M79)
END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHoe_Weapon_M79::CHoe_Weapon_M79(void)
{
	m_bReloadsSingly = false;
	m_bFiresUnderwater = false;
}

void CHoe_Weapon_M79::Precache(void)
{
	BaseClass::Precache();

	UTIL_PrecacheOther( "grenade_ar2" );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHoe_Weapon_M79::PrimaryAttack(void)
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (!pPlayer)
	{
		return;
	}

	if (m_iClip1 <= 0)
	{
		if (!m_bFireOnEmpty)
		{
			Reload();
		}
		else
		{
			WeaponSound(EMPTY);
			m_flNextPrimaryAttack = 0.15;
		}

		return;
	}

	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	Vector	vecThrow;
	// Don't autoaim on grenade tosses
	AngleVectors(pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), &vecThrow);
	VectorScale(vecThrow, 1000.0f, vecThrow);

	//Create the grenade
	QAngle angles;
	VectorAngles(vecThrow, angles);
	CGrenadeAR2 *pGrenade = (CGrenadeAR2*)Create("grenade_ar2", vecSrc, angles, pPlayer);
	pGrenade->SetAbsVelocity(vecThrow);

	pGrenade->SetLocalAngularVelocity(RandomAngle(-400, 400));
	pGrenade->SetMoveType(MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE);
	pGrenade->SetThrower(GetOwner());
	pGrenade->SetDamage(sk_plr_dmg_smg1_grenade.GetFloat());

	WeaponSound(SINGLE);
	pPlayer->DoMuzzleFlash();

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();

	m_iClip1--;

	//Disorient the player
	angles = pPlayer->GetLocalAngles();

	angles.x += random->RandomInt(-1, 1);
	angles.y += random->RandomInt(-1, 1);
	angles.z = 0;

	pPlayer->SnapEyeAngles(angles);

	pPlayer->ViewPunch(QAngle(-8, random->RandomFloat(-2, 2), 0));

	CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 600, 0.2, GetOwner());

	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired(pPlayer, true, GetClassname());
}