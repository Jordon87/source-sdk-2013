//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef TRIAGE_BASECOMBATWEAPON_SHARED_H
#define TRIAGE_BASECOMBATWEAPON_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "basehlcombatweapon_shared.h"
#include "triage_weapon_parse.h"
#include "triage_weapon_slots.h"

class CBaseViewModel;

#if defined( CLIENT_DLL )
#define CBaseTriageCombatWeapon C_BaseTriageCombatWeapon
#define CBaseViewModel	C_BaseViewModel
#endif

class CBaseTriageCombatWeapon : public CBaseHLCombatWeapon
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

	DECLARE_CLASS(CBaseTriageCombatWeapon, CBaseHLCombatWeapon);
public:
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CBaseTriageCombatWeapon();
	virtual ~CBaseTriageCombatWeapon();

	virtual void	Drop(const Vector &vecVelocity);
	virtual bool	Holster(CBaseCombatWeapon *pSwitchingTo);
	virtual bool	DefaultReload(int iClipSize1, int iClipSize2, int iActivity);
	virtual bool	Reload(void);

	// Firing animations
	virtual Activity		GetPrimaryAttackActivity();
	virtual Activity		GetSecondaryAttackActivity();

	// Bullet launch information
	virtual Vector	GetBulletSpread(WeaponProficiency_t proficiency);
	
	// Weapon behaviour
	virtual void			ItemPostFrame(void);
	virtual void			WeaponIdle(void);

	const CTriageFileWeaponInfo_t* GetTriageWpnData() const;

	// Override these to allow/disallow fire.
	virtual bool			IsAllowedToFire(void);
	virtual bool			IsPrimaryAttackAllowed(void);
	virtual bool			IsSecondaryAttackAllowed(void);

	// Lower weapons on sprint
	virtual bool			Sprint_WeaponShouldBeLowered(void);

	// Ironsights
	virtual Activity		GetIronsightsPrimaryAttackActivity();
	virtual Activity		GetIronsightsSecondaryAttackActivity();

	virtual bool			HasIronsights(void) { return true; } //default yes; override and return false for weapons with no ironsights (like weapon_crowbar)
	bool					IsIronsighted(void);
	void					ToggleIronsights(void);
	void					EnableIronsights(void);
	void					DisableIronsights(void);
	void					SetIronsightTime(void);

	// Weapon slots
	virtual const WeaponSlot_t	GetWeaponSlot() const { return WEAPON_SLOT_INVALID; }


	// GetWpnData()
	Vector					GetIronsightPositionOffset(void) const;
	QAngle					GetIronsightAngleOffset(void) const;
	float					GetIronsightFOVOffset(void) const;

	const unsigned char&	GetEquipIcon(void) const;

protected:

	CNetworkVar(bool, m_bIsIronsighted);
	CNetworkVar(float, m_flIronsightedTime);

	friend class CBaseViewModel;
};

#if defined ( CLIENT_DLL )
inline C_BaseTriageCombatWeapon *ToBaseTriageCombatWeapon(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsBaseCombatWeapon())
		return NULL;

	return dynamic_cast<C_BaseTriageCombatWeapon*>(pEntity);
}
#else
inline CBaseTriageCombatWeapon *ToBaseTriageCombatWeapon(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsBaseCombatWeapon())
		return NULL;

	return dynamic_cast<CBaseTriageCombatWeapon*>(pEntity);
}
#endif // defined ( CLIENT_DLL )

#endif // TRIAGE_BASECOMBATWEAPON_SHARED_H
