//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "basehlcombatweapon_shared.h"
#include "1187_weapon_parse.h"

#ifndef ELEVENEIGHTYSEVEN_BASECOMBATWEAPON_SHARED_H
#define ELEVENEIGHTYSEVEN_BASECOMBATWEAPON_SHARED_H
#ifdef _WIN32
#pragma once
#endif

class CBaseViewModel;

#if defined( CLIENT_DLL )
#define CBase1187CombatWeapon C_Base1187CombatWeapon
#define CBaseViewModel	C_BaseViewModel
#endif

class CBase1187CombatWeapon : public CBaseHLCombatWeapon
{
#if !defined( CLIENT_DLL )
#ifndef _XBOX
	DECLARE_DATADESC();
#else
protected:
	DECLARE_DATADESC();
private:
#endif
#endif

	DECLARE_CLASS(CBase1187CombatWeapon, CBaseHLCombatWeapon);
public:
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

							CBase1187CombatWeapon();
	virtual 				~CBase1187CombatWeapon();

	// Override these to allow/disallow fire.
	virtual bool			IsAllowedToFire(void);
	virtual bool			IsPrimaryAttackAllowed(void);
	virtual bool			IsSecondaryAttackAllowed(void);

	// Subtypes are used to manage multiple weapons of the same type on the player.
	virtual void			Drop(const Vector &vecVelocity);

	// Weapon selection
	virtual bool			Holster(CBaseCombatWeapon *pSwitchingTo);

	// Weapon idle.
	virtual void			WeaponIdle(void);

	// Reloading
	bool					DefaultReload(int iClipSize1, int iClipSize2, int iActivity);

	// Weapon behaviour
	virtual void			ItemPostFrame(void); // called each frame by the player PostThink

	// Sprint.
	bool					Sprint_IsWeaponLowered(void);
	virtual bool			Sprint_WeaponShouldBeLowered(void);
	void					Sprint_SetState(bool state);
	void					Sprint_Lower(void);
	void					Sprint_Leave(void);

	//---------------------------------------
	//
	// Override these in your derived classes.
	//
	//---------------------------------------

	//---------------------------------------
	// React to player jumping.
	//---------------------------------------
	virtual void			Operator_HandleJumpEvent(float fImpulse, CBaseCombatCharacter *pOperator);

	//---------------------------------------
	// React to player landing.
	//---------------------------------------
	virtual void			Operator_HandleLandEvent(float fVelocity, CBaseCombatCharacter *pOperator);

	//---------------------------------------
	// React to player's melee attack.
	//---------------------------------------
	virtual void			Operator_HandleMeleeEvent(CBaseCombatCharacter *pOperator);

#ifndef CLIENT_DLL
	//---------------------------------------
	// Melee swing
	//---------------------------------------
	void					MeleeSwing(void);
	void					MeleeHit(trace_t &traceHit);
	void					ImpactEffect(trace_t &traceHit);
	bool					ImpactWater(const Vector &start, const Vector &end);
	void					ChooseIntersectionPoint(trace_t &hitTrace, const Vector &mins, const Vector &maxs, CBasePlayer *pOwner);
	virtual void			AddMeleeViewKick(void) { return; }
#endif


	virtual Activity		GetPrimaryAttackActivity(void);
	virtual Activity		GetSecondaryAttackActivity(void);

	virtual const Vector&	GetDefaultBulletSpread(void);
	virtual const Vector&	GetIronsightsBulletSpread(void);
	virtual const Vector&	GetBulletSpread(void);


	// Ironsights
	Activity				GetIronsightsPrimaryAttackActivity(void);
	Activity				GetIronsightsSecondaryAttackActivity(void);

	virtual bool			HasIronsights(void) { return true; } //default yes; override and return false for weapons with no ironsights (like weapon_crowbar)
	bool					IsIronsighted(void);
	void					ToggleIronsights(void);
	void					EnableIronsights(void);
	void					DisableIronsights(void);
	void					SetIronsightTime(void);

	const C1187FileWeaponInfo_t* Get1187WpnData(void) const;

	Vector					GetIronsightPositionOffset(void) const;
	QAngle					GetIronsightAngleOffset(void) const;
	float					GetIronsightFOVOffset(void) const;

	Vector					GetFragPositionOffset(void) const;

protected:

	bool			m_bLoweredOnSprint;

	CNetworkVar(bool, m_bIsIronsighted);
	CNetworkVar(float, m_flIronsightedTime);

	friend class CBaseViewModel;
};

#if defined ( CLIENT_DLL )
inline C_Base1187CombatWeapon *ToBase1187CombatWeapon(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsBaseCombatWeapon())
		return NULL;

	return dynamic_cast<C_Base1187CombatWeapon*>(pEntity);
}
#else
inline CBase1187CombatWeapon *ToBase1187CombatWeapon(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsBaseCombatWeapon())
		return NULL;

	return dynamic_cast<CBase1187CombatWeapon*>(pEntity);
}
#endif // defined ( CLIENT_DLL )

#endif // ELEVENEIGHTYSEVEN_BASECOMBATWEAPON_SHARED_H
