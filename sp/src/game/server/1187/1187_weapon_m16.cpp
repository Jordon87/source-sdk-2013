//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "basehlcombatweapon.h"
#include "soundent.h"
#include "in_buttons.h"
#include "grenade_ar2.h"
#include "npcevent.h"
#include "ai_basenpc.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar    sk_plr_dmg_smg1_grenade;

//-----------------------------------------------------------------------------
// C1187WeaponM16
//-----------------------------------------------------------------------------
class CWeaponM16 : public CBaseHLCombatWeapon
{
	DECLARE_CLASS(CWeaponM16, CBaseHLCombatWeapon);
public:
	DECLARE_SERVERCLASS();

	CWeaponM16();

	void	Precache(void);

	virtual float GetScopeStuff();

	void	ItemPostFrame();
	void	WeaponIdle(void);

	float	GetFireRate(void) { return 0.4f; }

	virtual float WeaponAutoAimScale() { return 0.6f; };
	virtual const Vector& GetBulletSpread(void)
	{
		static Vector Cone = vec3_origin;
		static Vector Cone2 = VECTOR_CONE_2DEGREES;

		if (!m_bIsIronsighted)
		{
			return Cone2;
		}
		else
		{
			return Cone;
		}
	}

	void	PrimaryAttack(void);
	void	SecondaryAttack(void);
	
	void	Operator_HandleAnimEvent(animevent_t* pEvent, CBaseCombatCharacter* pOperator);
	void	Operator_ForceNPCFire(CBaseCombatCharacter* pOperator, bool bSecondary);
	void	FireNPCPrimaryAttack(CBaseCombatCharacter* pOperator, Vector& vecShootOrigin, Vector& vecShootDir);

	virtual bool FUN_1037e3d0();
private:
	float unk_0x550;

protected:
	DECLARE_ACTTABLE();
};

IMPLEMENT_SERVERCLASS_ST(CWeaponM16, DT_WeaponM16)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_m16, CWeaponM16);
PRECACHE_WEAPON_REGISTER(weapon_m16);

acttable_t CWeaponM16::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_AR2,			true },
	{ ACT_RELOAD,					ACT_RELOAD_SMG1,				true },		// FIXME: hook to AR2 unique
	{ ACT_IDLE,						ACT_IDLE_SMG1,					true },		// FIXME: hook to AR2 unique
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_SMG1,			true },		// FIXME: hook to AR2 unique

	{ ACT_WALK,						ACT_WALK_RIFLE,					true },

	// Readiness activities (not aiming)
		{ ACT_IDLE_RELAXED,				ACT_IDLE_SMG1_RELAXED,			false },//never aims
		{ ACT_IDLE_STIMULATED,			ACT_IDLE_SMG1_STIMULATED,		false },
		{ ACT_IDLE_AGITATED,			ACT_IDLE_ANGRY_SMG1,			false },//always aims

		{ ACT_WALK_RELAXED,				ACT_WALK_RIFLE_RELAXED,			false },//never aims
		{ ACT_WALK_STIMULATED,			ACT_WALK_RIFLE_STIMULATED,		false },
		{ ACT_WALK_AGITATED,			ACT_WALK_AIM_RIFLE,				false },//always aims

		{ ACT_RUN_RELAXED,				ACT_RUN_RIFLE_RELAXED,			false },//never aims
		{ ACT_RUN_STIMULATED,			ACT_RUN_RIFLE_STIMULATED,		false },
		{ ACT_RUN_AGITATED,				ACT_RUN_AIM_RIFLE,				false },//always aims

		// Readiness activities (aiming)
			{ ACT_IDLE_AIM_RELAXED,			ACT_IDLE_SMG1_RELAXED,			false },//never aims	
			{ ACT_IDLE_AIM_STIMULATED,		ACT_IDLE_AIM_RIFLE_STIMULATED,	false },
			{ ACT_IDLE_AIM_AGITATED,		ACT_IDLE_ANGRY_SMG1,			false },//always aims

			{ ACT_WALK_AIM_RELAXED,			ACT_WALK_RIFLE_RELAXED,			false },//never aims
			{ ACT_WALK_AIM_STIMULATED,		ACT_WALK_AIM_RIFLE_STIMULATED,	false },
			{ ACT_WALK_AIM_AGITATED,		ACT_WALK_AIM_RIFLE,				false },//always aims

			{ ACT_RUN_AIM_RELAXED,			ACT_RUN_RIFLE_RELAXED,			false },//never aims
			{ ACT_RUN_AIM_STIMULATED,		ACT_RUN_AIM_RIFLE_STIMULATED,	false },
			{ ACT_RUN_AIM_AGITATED,			ACT_RUN_AIM_RIFLE,				false },//always aims
			//End readiness activities

				{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,				true },
				{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,			true },
				{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,		true },
				{ ACT_RUN,						ACT_RUN_RIFLE,					true },
				{ ACT_RUN_AIM,					ACT_RUN_AIM_RIFLE,				true },
				{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,			true },
				{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,		true },
				{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_AR2,	false },
				{ ACT_COVER_LOW,				ACT_COVER_SMG1_LOW,				false },		// FIXME: hook to AR2 unique
				{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_AR2_LOW,			false },
				{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_SMG1_LOW,		true },		// FIXME: hook to AR2 unique
				{ ACT_RELOAD_LOW,				ACT_RELOAD_SMG1_LOW,			false },
				{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_SMG1,		true },
				//	{ ACT_RANGE_ATTACK2, ACT_RANGE_ATTACK_AR2_GRENADE, true },
};

IMPLEMENT_ACTTABLE(CWeaponM16);

CWeaponM16::CWeaponM16()
{
	unk_0x550 = 30.0f;
	m_bReloadsSingly = false;
	m_fMinRange1 = 78.0f;
	m_bFiresUnderwater = false;
	m_bAltFiresUnderwater = false;
	m_fMaxRange1 = 8400.0f;
}

void CWeaponM16::Precache(void)
{
	UTIL_PrecacheOther("grenade_ar2");
	BaseClass::Precache();
}

float CWeaponM16::GetScopeStuff()
{
	float v1 = 30.0f;

	if (unk_0x550 <= 30.0f)
	{
		v1 = 5.0f;
		if (unk_0x550 >= 5.0f)
			v1 = unk_0x550;
	}

	unk_0x550 = v1;
	ConVarRef g_tempscope("g_tempscope");
	g_tempscope.SetValue(v1);

	return unk_0x550;
}

void CWeaponM16::ItemPostFrame()
{
	CBasePlayer* pOwner = ToBasePlayer(GetOwner());

	if (pOwner && pOwner->IsPlayer())
	{
		if ( IsIronsighted() && (pOwner->m_nButtons & IN_ATTACK2) != 0)
		{
			WeaponIdle();
			SecondaryAttack();
		}
		else
		{
			BaseClass::ItemPostFrame();
		}
	}
}

void CWeaponM16::WeaponIdle(void)
{
	if (FUN_1037e3d0())
	{
		if (GetActivity() != ACT_FIRE_LOOP && GetActivity() != ACT_FIRE_START && GetActivity() != ACT_TRANSITION || HasWeaponIdleTimeElapsed())
			SendWeaponAnim(ACT_FIRE_LOOP);
	}
	else if (IsIronsighted())
	{
		if (GetActivity() != ACT_FIRE_END && GetActivity() != ACT_FIRE_START && GetActivity() != ACT_TRANSITION || HasWeaponIdleTimeElapsed())
			SendWeaponAnim(ACT_VM_FIDGET);
	}
	else
	{
		BaseClass::WeaponIdle();
	}
}

void CWeaponM16::PrimaryAttack(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer && pPlayer->IsPlayer())
	{
		if ((m_iClip1) > 0)
		{
			if (!IsIronsighted())
				SendWeaponAnim(ACT_VM_PRIMARYATTACK);
			else
				SendWeaponAnim(ACT_VM_RECOIL1);

			WeaponSound(SINGLE);
			pPlayer->DoMuzzleFlash();

			m_flNextPrimaryAttack = SequenceDuration() + gpGlobals->curtime;
			m_flNextSecondaryAttack = SequenceDuration() + gpGlobals->curtime;

			m_iClip1 = m_iClip1 - 1;

			Vector vecSrc = pPlayer->Weapon_ShootPosition();

			IsIronsighted();

			Vector vecAiming = pPlayer->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT);
			pPlayer->FireBullets(1, vecSrc, vecAiming, GetBulletSpread(), MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 1);
	
			pPlayer->SetMuzzleFlashTime(gpGlobals->curtime + 0.5f);

			CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 600, 0.2f, GetOwner());
		
			if (!IsIronsighted())
			{
				QAngle angPunch = QAngle(random->RandomFloat(-14.0f, -19.0f), random->RandomFloat(-2.0f, 2.0f), 0.0f);
				pPlayer->ViewPunch(angPunch);
			}
		}
		else
		{
			Reload();
		}
	}
}

void CWeaponM16::SecondaryAttack(void)
{
	CBasePlayer* pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer && pPlayer->IsPlayer())
	{
		if (IsIronsighted())
		{
			if (GetIdealActivity() == ACT_FIRE_LOOP || GetActivity() == ACT_FIRE_LOOP)
			{
				// ??? need to know what 0x6000000 is.

				float v4;

				if ( (pPlayer->m_nButtons & 0x6000000) != 0 || (pPlayer->m_nButtons & IN_USE) != 0)
					v4 = unk_0x550 + 0.1f;
				else
					v4 = unk_0x550 - 0.1f;
				unk_0x550 = v4;
			}

			GetScopeStuff();

			m_flNextPrimaryAttack = gpGlobals->curtime + 0.15f;
			m_flNextSecondaryAttack = gpGlobals->curtime + 0.15f;
		}
		else if (pPlayer->GetAmmoCount(m_iSecondaryAmmoType) <= 0 || pPlayer->GetWaterLevel() == 3)
		{
			SendWeaponAnim(ACT_VM_DRYFIRE);
			WeaponSound(EMPTY);
			m_flNextSecondaryAttack = gpGlobals->curtime + 0.5f;
		}
		else
		{
			WeaponSound(WPN_DOUBLE);
			Vector vecSrc = pPlayer->Weapon_ShootPosition();
			QAngle punchAngle = pPlayer->GetPunchAngle();
			QAngle playerAngles = pPlayer->EyeAngles();

			QAngle angOffset;

			angOffset = playerAngles + punchAngle;

			Vector vecForward;
			AngleVectors(angOffset, &vecForward);
			vecForward = vecForward * 1000;

			CGrenadeAR2 *pGrenade = (CGrenadeAR2*)Create("grenade_ar2", vecSrc, vec3_angle, pPlayer);
			pGrenade->SetAbsVelocity(vecForward);
			pGrenade->SetLocalAngularVelocity(RandomAngle(-400.0,400.0));
			pGrenade->SetMoveType(MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE);
			pGrenade->SetThrower(GetOwner());
			pGrenade->SetDamage(sk_plr_dmg_smg1_grenade.GetFloat());
			pGrenade->SetDamageRadius(180.0f);

			if (pPlayer->GetAmmoCount(m_iSecondaryAmmoType) <= 1)
				SendWeaponAnim(ACT_VM_PULLPIN);
			else
				SendWeaponAnim(ACT_VM_SECONDARYATTACK);

			pPlayer->RemoveAmmo(1, m_iSecondaryAmmoType);

			m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;
			m_flNextSecondaryAttack = gpGlobals->curtime + 1.0f;

			pPlayer->SetMuzzleFlashTime(gpGlobals->curtime + 0.5f);

			CSoundEnt::InsertSound(SOUND_COMBAT, GetAbsOrigin(), 1500, 0.2f, GetOwner());

			QAngle angViewPunch = QAngle((random->RandomFloat)(-16.0f, -22.0f), (random->RandomFloat)(-2.0f, 2.0f), 0.0f);
			pPlayer->ViewPunch(angViewPunch);
		}
	}
}

void CWeaponM16::Operator_HandleAnimEvent(animevent_t* pEvent, CBaseCombatCharacter* pOperator)
{
	switch (pEvent->event)
	{
	case EVENT_WEAPON_SMG1:
	{
		Vector vecShootOrigin, vecShootDir;
		QAngle angDiscard;

		// Support old style attachment point firing
		if ((pEvent->options == NULL) || (pEvent->options[0] == '\0') || (!pOperator->GetAttachment(pEvent->options, vecShootOrigin, angDiscard)))
		{
			vecShootOrigin = pOperator->Weapon_ShootPosition();
		}

		CAI_BaseNPC* npc = pOperator->MyNPCPointer();
		ASSERT(npc != NULL);
		vecShootDir = npc->GetActualShootTrajectory(vecShootOrigin);

		FireNPCPrimaryAttack(pOperator, vecShootOrigin, vecShootDir);
	}
	break;

	default:
		BaseClass::Operator_HandleAnimEvent(pEvent, pOperator);
		break;
	}
}

void CWeaponM16::Operator_ForceNPCFire(CBaseCombatCharacter* pOperator, bool bSecondary)
{
	m_iClip1++;

	Vector vecShootOrigin, vecShootDir;
	QAngle	angShootDir;
	GetAttachment(LookupAttachment("muzzle"), vecShootOrigin, angShootDir);
	AngleVectors(angShootDir, &vecShootDir);
	FireNPCPrimaryAttack(pOperator, vecShootOrigin, vecShootDir);
}

void CWeaponM16::FireNPCPrimaryAttack(CBaseCombatCharacter* pOperator, Vector& vecShootOrigin, Vector& vecShootDir)
{
	WeaponSound(SINGLE_NPC);

	CBaseEntity* pEnemy = GetEnemy();
	CSoundEnt::InsertSound(SOUND_COMBAT|SOUND_CONTEXT_GUNFIRE, pOperator->GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2f, pOperator, SOUNDENT_CHANNEL_WEAPON, pEnemy);

	pOperator->FireBullets(1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2);

	m_iClip1 = m_iClip1 - 1;
}

bool CWeaponM16::FUN_1037e3d0()
{
	if (GetOwner())
	{
		CBasePlayer *pOwner = ToBasePlayer(GetOwner());
		if (pOwner)
		{
			if (pOwner->IsPlayer() && IsIronsighted() && (pOwner->m_nButtons & IN_ATTACK2) != 0)
				return true;
		}
	}

	return false;
}
