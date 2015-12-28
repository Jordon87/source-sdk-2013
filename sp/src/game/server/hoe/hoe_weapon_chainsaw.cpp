//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hoe_basecombatweapon.h"
#include "player.h"
#include "gamerules.h"
#include "ammodef.h"
#include "mathlib/mathlib.h"
#include "in_buttons.h"
#include "soundent.h"
#include "hoe_basebludgeonweapon.h"
#include "vstdlib/random.h"
#include "npcevent.h"
#include "ai_basenpc.h"
#include "weapon_crowbar.h"
#include "gamestats.h"
#include "rumble_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define CHAINSAW_HULL_DIM		16

static const Vector g_chainsawMins(-CHAINSAW_HULL_DIM, -CHAINSAW_HULL_DIM, -CHAINSAW_HULL_DIM);
static const Vector g_chainsawMaxs(CHAINSAW_HULL_DIM, CHAINSAW_HULL_DIM, CHAINSAW_HULL_DIM);

extern ConVar  sk_chainsaw_dmg;

//-----------------------------------------------------------------------------
// CHoe_Weapon_Chainsaw
//-----------------------------------------------------------------------------

class CHoe_Weapon_Chainsaw : public CHoe_BaseBludgeonWeapon
{
public:
	DECLARE_CLASS(CHoe_Weapon_Chainsaw, CHoe_BaseBludgeonWeapon);

	DECLARE_SERVERCLASS();
	DECLARE_ACTTABLE();

	DECLARE_DATADESC();

	CHoe_Weapon_Chainsaw();


	bool		Deploy(void);
	bool		Holster(CBaseCombatWeapon *pSwitchingTo);
	void		WeaponIdle(void);

	Activity	GetPrimaryAttackActivity(void)		{ return ACT_CHAINSAW_ATTACK; }
	Activity	GetSecondaryAttackActivity(void)	{ return ACT_CHAINSAW_IDLE_HIT; }

	Activity	GetPrimaryMissActivity(void)		{ return ACT_CHAINSAW_ATTACK; }
	Activity	GetSecondaryMissActivity(void)		{ return ACT_CHAINSAW_IDLE_HIT; }

	float		GetRange(void)			{ return	CROWBAR_RANGE; }
	float		GetFireRate(void)		{ return	0.1f; }

	void		AddViewKick(void);
	float		GetDamageForActivity(Activity hitActivity);

	virtual int WeaponMeleeAttack1Condition(float flDot, float flDist);
	void		PrimaryAttack(void);
	void		SecondaryAttack(void);

	// Animation event
	virtual void Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	void		ItemPostFrame(void);

protected:

	void			Swing(int bIsSecondary);

private:
	// Animation event handlers
	void HandleAnimEventMeleeHit(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	enum StartState { START_NONE = 0, START_READY, START_FAIL, START_SUCCESS, START_EMPTY, START_POST_EMPTY, START_POST_SUCCESS };

	bool m_bEngineOn;
	bool m_bInSwing;
	bool m_bDidHit;
	float m_flNextHitTime;
	float m_flNextRumbleTime;
	float m_flNextIdleSoundTime;

	static bool m_bFirstStart;

	int m_iStartState;
};

//-----------------------------------------------------------------------------
// CHoe_Weapon_Chainsaw
//-----------------------------------------------------------------------------

bool CHoe_Weapon_Chainsaw::m_bFirstStart = false;

BEGIN_DATADESC(CHoe_Weapon_Chainsaw)
	DEFINE_FIELD(m_iStartState, FIELD_INTEGER),
	DEFINE_FIELD(m_bEngineOn, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bInSwing, FIELD_BOOLEAN),
	DEFINE_FIELD(m_bDidHit, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flNextHitTime, FIELD_TIME),
	DEFINE_FIELD(m_flNextRumbleTime, FIELD_TIME),
	DEFINE_FIELD(m_flNextIdleSoundTime, FIELD_TIME),
	//DEFINE_FIELD(m_bFirstStart, FIELD_BOOLEAN),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CHoe_Weapon_Chainsaw, DT_Hoe_Weapon_Chainsaw)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_chainsaw, CHoe_Weapon_Chainsaw);
PRECACHE_WEAPON_REGISTER(weapon_chainsaw);

acttable_t CHoe_Weapon_Chainsaw::m_acttable[] =
{
	{ ACT_MELEE_ATTACK1, ACT_MELEE_ATTACK_SWING, true },
	{ ACT_IDLE, ACT_IDLE_ANGRY_MELEE, false },
	{ ACT_IDLE_ANGRY, ACT_IDLE_ANGRY_MELEE, false },
};

IMPLEMENT_ACTTABLE(CHoe_Weapon_Chainsaw);

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CHoe_Weapon_Chainsaw::CHoe_Weapon_Chainsaw(void)
{
	m_iStartState = START_NONE;
	m_bEngineOn = false;
	m_bInSwing = false;
	m_bDidHit = false;
	m_flNextHitTime = 0.0f;
	m_flNextRumbleTime = 0.0f;
	m_flNextIdleSoundTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Get the damage amount for the animation we're doing
// Input  : hitActivity - currently played activity
// Output : Damage amount
//-----------------------------------------------------------------------------
float CHoe_Weapon_Chainsaw::GetDamageForActivity(Activity hitActivity)
{
	if (hitActivity == ACT_CHAINSAW_IDLE_HIT)
		return sk_chainsaw_dmg.GetFloat() * 0.25f;

	return sk_chainsaw_dmg.GetFloat();
}

//-----------------------------------------------------------------------------
// Purpose: Add in a view kick for this weapon
//-----------------------------------------------------------------------------
void CHoe_Weapon_Chainsaw::AddViewKick(void)
{
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());

	if (pPlayer == NULL)
		return;

	QAngle punchAng;

	punchAng.x = random->RandomFloat(1.0f, 2.0f);
	punchAng.y = random->RandomFloat(-2.0f, -1.0f);
	punchAng.z = 0.0f;

	pPlayer->ViewPunch(punchAng);
}


//-----------------------------------------------------------------------------
// Attempt to lead the target (needed because citizens can't hit manhacks with the crowbar!)
//-----------------------------------------------------------------------------
extern ConVar sk_crowbar_lead_time;

int CHoe_Weapon_Chainsaw::WeaponMeleeAttack1Condition(float flDot, float flDist)
{
	// Attempt to lead the target (needed because citizens can't hit manhacks with the crowbar!)
	CAI_BaseNPC *pNPC = GetOwner()->MyNPCPointer();
	CBaseEntity *pEnemy = pNPC->GetEnemy();
	if (!pEnemy)
		return COND_NONE;

	Vector vecVelocity;
	vecVelocity = pEnemy->GetSmoothedVelocity();

	// Project where the enemy will be in a little while
	float dt = sk_crowbar_lead_time.GetFloat();
	dt += random->RandomFloat(-0.3f, 0.2f);
	if (dt < 0.0f)
		dt = 0.0f;

	Vector vecExtrapolatedPos;
	VectorMA(pEnemy->WorldSpaceCenter(), dt, vecVelocity, vecExtrapolatedPos);

	Vector vecDelta;
	VectorSubtract(vecExtrapolatedPos, pNPC->WorldSpaceCenter(), vecDelta);

	if (fabs(vecDelta.z) > 70)
	{
		return COND_TOO_FAR_TO_ATTACK;
	}

	Vector vecForward = pNPC->BodyDirection2D();
	vecDelta.z = 0.0f;
	float flExtrapolatedDist = Vector2DNormalize(vecDelta.AsVector2D());
	if ((flDist > 64) && (flExtrapolatedDist > 64))
	{
		return COND_TOO_FAR_TO_ATTACK;
	}

	float flExtrapolatedDot = DotProduct2D(vecDelta.AsVector2D(), vecForward.AsVector2D());
	if ((flDot < 0.7) && (flExtrapolatedDot < 0.7))
	{
		return COND_NOT_FACING_ATTACK;
	}

	return COND_CAN_MELEE_ATTACK1;
}


//-----------------------------------------------------------------------------
// Animation event handlers
//-----------------------------------------------------------------------------
void CHoe_Weapon_Chainsaw::HandleAnimEventMeleeHit(animevent_t *pEvent, CBaseCombatCharacter *pOperator)
{
	// Trace up or down based on where the enemy is...
	// But only if we're basically facing that direction
	Vector vecDirection;
	AngleVectors(GetAbsAngles(), &vecDirection);

	CBaseEntity *pEnemy = pOperator->MyNPCPointer() ? pOperator->MyNPCPointer()->GetEnemy() : NULL;
	if (pEnemy)
	{
		Vector vecDelta;
		VectorSubtract(pEnemy->WorldSpaceCenter(), pOperator->Weapon_ShootPosition(), vecDelta);
		VectorNormalize(vecDelta);

		Vector2D vecDelta2D = vecDelta.AsVector2D();
		Vector2DNormalize(vecDelta2D);
		if (DotProduct2D(vecDelta2D, vecDirection.AsVector2D()) > 0.8f)
		{
			vecDirection = vecDelta;
		}
	}
	
	Vector vecEnd;
	VectorMA(pOperator->Weapon_ShootPosition(), 50, vecDirection, vecEnd);
	CBaseEntity *pHurt = pOperator->CheckTraceHullAttack(pOperator->Weapon_ShootPosition(), vecEnd,
		Vector(-16, -16, -16), Vector(36, 36, 36), sk_chainsaw_dmg.GetFloat(), DMG_SLASH | DMG_ALWAYSGIB, 0.75);

	// did I hit someone?
	if (pHurt)
	{
		// play sound
		WeaponSound(MELEE_HIT);

		// Fake a trace impact, so the effects work out like a player's crowbaw
		trace_t traceHit;
		UTIL_TraceLine(pOperator->Weapon_ShootPosition(), pHurt->GetAbsOrigin(), MASK_SHOT_HULL, pOperator, COLLISION_GROUP_NONE, &traceHit);
		ImpactEffect(traceHit);
	}
	else
	{
		WeaponSound(MELEE_MISS);
	}
}


//-----------------------------------------------------------------------------
// Animation event
//-----------------------------------------------------------------------------
void CHoe_Weapon_Chainsaw::Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator)
{
	switch (pEvent->event)
	{
	case EVENT_WEAPON_MELEE_HIT:
		HandleAnimEventMeleeHit(pEvent, pOperator);
		break;

	default:
		BaseClass::Operator_HandleAnimEvent(pEvent, pOperator);
		break;
	}
}

bool CHoe_Weapon_Chainsaw::Deploy(void)
{
	bool bRet = BaseClass::Deploy();

	if (bRet)
	{
		m_bEngineOn = false;
		m_bInSwing = false;
		m_bDidHit = false;
		m_iStartState = START_NONE;
	}

	return bRet;
}

bool CHoe_Weapon_Chainsaw::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	bool bRet = BaseClass::Holster(pSwitchingTo);

	if (bRet)
	{
		m_bEngineOn = false;
		m_bInSwing = false;
		m_bDidHit = false;
		m_iStartState = START_NONE;
	}

	return bRet;
}

//=========================================================
void CHoe_Weapon_Chainsaw::WeaponIdle(void)
{
	BaseClass::WeaponIdle();

	if (m_bEngineOn)
	{
		if (gpGlobals->curtime > m_flNextIdleSoundTime)
		{
			WeaponSound( SPECIAL2 );

			m_flNextIdleSoundTime = gpGlobals->curtime + 0.8f;
		}

		if (gpGlobals->curtime > m_flNextHitTime)
		{
			Swing(true);
			m_flNextHitTime = gpGlobals->curtime + 0.1f;
		}

		if (gpGlobals->curtime > m_flNextRumbleTime)
		{
			UTIL_ScreenShake(GetAbsOrigin(), 1.0, 96, 0.5f, 750, SHAKE_START);
			m_flNextRumbleTime = gpGlobals->curtime + 0.25f;
		}
	}

	if (m_bDidHit)
	{
		if (GetActivity() != ACT_CHAINSAW_IDLE_HIT || HasWeaponIdleTimeElapsed())
		{
			SendWeaponAnim(ACT_CHAINSAW_IDLE_HIT);
		}
	}
	//Idle again if we've finished
	else if (HasWeaponIdleTimeElapsed())
	{	
		int iAnim = 0;

		if (m_bEngineOn)
		{
			iAnim = ACT_CHAINSAW_IDLE;
		}
		else
		{
			iAnim = ACT_CHAINSAW_IDLE_OFF;
		}

		SendWeaponAnim(iAnim);
	}
}

void CHoe_Weapon_Chainsaw::PrimaryAttack(void)
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	// play 'attack' animation.
	pOwner->SetAnimation( PLAYER_ATTACK1 );

	// Play attack sequence.
	SendWeaponAnim( ACT_CHAINSAW_ATTACK );

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) > 0 && m_bEngineOn)
	{
		// Send attack sound.
		WeaponSound(SINGLE);

		m_iPrimaryAttacks++;

		gamestats->Event_WeaponFired(pOwner, true, GetClassname());

		m_bInSwing = true;
	}

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
}

void CHoe_Weapon_Chainsaw::SecondaryAttack(void)
{
	if (!m_bEngineOn)
	{
		m_iStartState = START_READY;
	}
	else
	{
		m_iStartState = START_POST_EMPTY;

		m_bEngineOn = false;

		SendWeaponAnim( ACT_CHAINSAW_IDLE_OFF );

		SetCycle( 0 );

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + 0.3f;
	}
}

void CHoe_Weapon_Chainsaw::ItemPostFrame(void)
{
	if (m_iStartState != START_NONE)
	{
		if (m_iStartState == START_READY)
		{
			m_iStartState = START_SUCCESS;

			SendWeaponAnim(ACT_CHAINSAW_START_READY);

			m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
		}
		else if (IsViewModelSequenceFinished())
		{
			switch (m_iStartState)
			{
			case START_FAIL:
			{
				m_iStartState = START_EMPTY;

				SendWeaponAnim(ACT_CHAINSAW_START_FAIL);

				WeaponSound(SPECIAL1);

				m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
			}
			break;

			case START_SUCCESS:
			{
				m_iStartState = START_POST_SUCCESS;

				SendWeaponAnim(ACT_CHAINSAW_START);

				WeaponSound(WPN_DOUBLE);

				m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
			}
			break;

			case START_EMPTY:
			{
				m_iStartState = START_POST_EMPTY;

				SendWeaponAnim(ACT_CHAINSAW_START_EMPTY);

				m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
			}
			break;

			case START_POST_EMPTY:
			{
				m_iStartState = START_NONE;

				m_bEngineOn = false;
			}
			break;

			case START_POST_SUCCESS:
			{
				m_iStartState = START_NONE;

				m_bEngineOn = true;
			}
			break;

			default:
				DevMsg("Warning: invalid chainsaw start state!\n");
				break;
			}
		}

		return;
	}
	else if (m_bInSwing)
	{
		if (IsViewModelSequenceFinished())
		{
			m_bInSwing = false;
		}
		else
		{
			if (gpGlobals->curtime > m_flNextHitTime)
			{
				Swing(false);
				m_flNextHitTime = gpGlobals->curtime + GetFireRate();
			}
		}

		return;
	}
	else
	{
		BaseClass::ItemPostFrame();

		// Reset all hit data.
		m_bDidHit = false;
	}
}

void CHoe_Weapon_Chainsaw::Swing(int bIsSecondary)
{
	trace_t traceHit;

	// Try a ray
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	pOwner->RumbleEffect(RUMBLE_CROWBAR_SWING, 0, RUMBLE_FLAG_RESTART);

	Vector swingStart = pOwner->Weapon_ShootPosition();
	Vector forward;

	forward = pOwner->GetAutoaimVector(AUTOAIM_SCALE_DEFAULT, GetRange());

	Vector swingEnd = swingStart + forward * GetRange();
	UTIL_TraceLine(swingStart, swingEnd, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &traceHit);
	Activity nHitActivity = !bIsSecondary ? ACT_CHAINSAW_ATTACK : ACT_CHAINSAW_IDLE_HIT;

	// Like bullets, bludgeon traces have to trace against triggers.
	CTakeDamageInfo triggerInfo(GetOwner(), GetOwner(), GetDamageForActivity(nHitActivity), DMG_CLUB);
	triggerInfo.SetDamagePosition(traceHit.startpos);
	triggerInfo.SetDamageForce(forward);
	TraceAttackToTriggers(triggerInfo, traceHit.startpos, traceHit.endpos, forward);

	if (traceHit.fraction == 1.0)
	{
		float bludgeonHullRadius = 1.732f * CHAINSAW_HULL_DIM;  // hull is +/- 16, so use cuberoot of 2 to determine how big the hull is from center to the corner point

		// Back off by hull "radius"
		swingEnd -= forward * bludgeonHullRadius;

		UTIL_TraceHull(swingStart, swingEnd, g_chainsawMins, g_chainsawMaxs, MASK_SHOT_HULL, pOwner, COLLISION_GROUP_NONE, &traceHit);
		if (traceHit.fraction < 1.0 && traceHit.m_pEnt)
		{
			Vector vecToTarget = traceHit.m_pEnt->GetAbsOrigin() - swingStart;
			VectorNormalize(vecToTarget);

			float dot = vecToTarget.Dot(forward);

			// YWB:  Make sure they are sort of facing the guy at least...
			if (dot < 0.70721f)
			{
				// Force amiss
				traceHit.fraction = 1.0f;
			}
			else
			{
				nHitActivity = ChooseIntersectionPointAndActivity(traceHit, g_chainsawMins, g_chainsawMaxs, pOwner, bIsSecondary);
			}
		}
	}

	// -------------------------
	//	Miss
	// -------------------------
	if (traceHit.fraction == 1.0f)
	{
		// We want to test the first swing again
		Vector testEnd = swingStart + forward * GetRange();

		// See if we happened to hit water
		ImpactWater(swingStart, testEnd);
	}
	else
	{
		Hit(traceHit, nHitActivity, bIsSecondary ? true : false);

		m_bDidHit = true;
	}
}