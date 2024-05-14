#include "cbase.h"
#include "basehlcombatweapon.h"
#include "in_buttons.h"
#include "gamestats.h"
#include "IEffects.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SECRET_MUZZLE		1

class CWeaponSecret : public CBaseHLCombatWeapon
{
	DECLARE_CLASS( CWeaponSecret, CBaseHLCombatWeapon );

public:
	DECLARE_SERVERCLASS();

	CWeaponSecret();

	void	Spawn();
	float	WeaponAutoAimScale() { return 1.6f; }

	bool	Deploy();
	bool	Holster( CBaseCombatWeapon* pSwitchingTo = NULL );
	void	ItemPostFrame();
	void	PrimaryAttack();
};

LINK_ENTITY_TO_CLASS( weapon_secret, CWeaponSecret );

IMPLEMENT_SERVERCLASS_ST( CWeaponSecret, DT_WeaponSecret )
END_SEND_TABLE()

CWeaponSecret::CWeaponSecret()
{
	m_bReloadsSingly = false;
	m_bFiresUnderwater = true;
}

void CWeaponSecret::Spawn()
{
#if 0
	ConVarRef easteregg( "_dec3542568236206973268276836829" );

	if ( easteregg.GetInt() )
		BaseClass::Spawn();
	else
		UTIL_Remove( this );
#endif

	BaseClass::Spawn();
}

bool CWeaponSecret::Deploy()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( !pOwner || !pOwner->IsPlayer() )
		return BaseClass::Deploy();

	SetGravity( 0.8f );
	return BaseClass::Deploy();
}

bool CWeaponSecret::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( !pOwner || !pOwner->IsPlayer() )
		return BaseClass::Deploy();

	SetGravity( 1.0f );
	return BaseClass::Holster( pSwitchingTo );
}

void CWeaponSecret::ItemPostFrame()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner || pOwner->IsPlayer() )
	{
		if ( ( pOwner->m_nButtons & IN_ATTACK ) != 0 && gpGlobals->curtime >= m_flNextPrimaryAttack
			|| ( pOwner->m_afButtonPressed & IN_ALT2 ) != 0 && gpGlobals->curtime >= m_flNextSecondaryAttack )
		{
			PrimaryAttack();
		}
		else
		{
			WeaponIdle();
		}
	}
}

void CWeaponSecret::PrimaryAttack()
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );

	if ( pOwner && pOwner->IsPlayer() )
	{
		++m_iPrimaryAttacks;
		gamestats->Event_WeaponFired( pOwner, true, GetClassname() );

		WeaponSound( SINGLE );
		pOwner->DoMuzzleFlash();

		SendWeaponAnim( ACT_VM_PRIMARYATTACK );

		m_flNextPrimaryAttack = SequenceDuration() + gpGlobals->curtime;
		m_flNextSecondaryAttack = SequenceDuration() + gpGlobals->curtime;
	
		Vector vecSrc = pOwner->Weapon_ShootPosition();

		Vector vForward;

		trace_t	tr;
		Vector vecEye = pOwner->EyePosition();

		pOwner->EyeVectors( &vForward, NULL, NULL );

		UTIL_TraceLine( vecEye, vecEye + vForward * 9000, MASK_SHOT, pOwner, COLLISION_GROUP_NONE, &tr );
	
		int iMuzzle = LookupAttachment( "muzzle" );
		EntityMessageBegin( this, true );
			WRITE_BYTE( SECRET_MUZZLE );
			WRITE_BYTE( iMuzzle );
			WRITE_VEC3COORD( tr.endpos );
		MessageEnd();

		if ( tr.m_pEnt )
		{
			VectorNormalize( vForward );

			CTakeDamageInfo info( GetOwner(), GetOwner(), 180.0f, DMG_BLAST );
			
			if ( tr.m_pEnt->IsNPC() )
				info.AdjustPlayerDamageInflictedForSkillLevel();

			CalculateMeleeDamageForce( &info, vForward, tr.endpos );
			
			tr.m_pEnt->DispatchTraceAttack( info, vForward, &tr );
			ApplyMultiDamage();

			TraceAttackToTriggers( info, tr.startpos, tr.endpos, vForward );

			pOwner->TakeHealth( 100.0f, DMG_GENERIC );

			UTIL_ImpactTrace( &tr, DMG_GENERIC );
			UTIL_DecalTrace( &tr, "SmallScorch" );
		}

		g_pEffects->Sparks( tr.endpos, 5, 1 );
		pOwner->ViewPunch( QAngle ( -10.0f, random->RandomFloat( -2.0f, 2.0f ), 0.0f ) );
	}
}

