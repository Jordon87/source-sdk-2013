//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "basehlcombatweapon_shared.h"

#ifndef HOE_BASECOMBATWEAPON_SHARED_H
#define HOE_BASECOMBATWEAPON_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#if defined( CLIENT_DLL )
#define CHoe_BaseCombatWeapon C_Hoe_BaseCombatWeapon
#endif

class HoeFileWeaponInfo_t;

class CHoe_BaseCombatWeapon : public CBaseHLCombatWeapon
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

	DECLARE_CLASS(CHoe_BaseCombatWeapon, CBaseHLCombatWeapon);
public:
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CHoe_BaseCombatWeapon();

	virtual void			Drop(const Vector &vecVelocity);

	bool					DefaultReload(int iClipSize1, int iClipSize2, int iActivity);

	virtual bool			Holster(CBaseCombatWeapon *pSwitchingTo);

	// Ironsights
	virtual bool			HasIronsights(void) { return true; } //default yes; override and return false for weapons with no ironsights (like weapon_crowbar)
	bool					IsIronsighted(void);
	void					ToggleIronsights(void);
	void					EnableIronsights(void);
	void					DisableIronsights(void);
	void					SetIronsightTime(void);

	const HoeFileWeaponInfo_t* GetHoeWpnData() const;

	Vector					GetIronsightPositionOffset(void) const;
	QAngle					GetIronsightAngleOffset(void) const;
	float					GetIronsightFOVOffset(void) const;


	CNetworkVar(bool, m_bIsIronsighted);
	CNetworkVar(float, m_flIronsightedTime);
};

inline CHoe_BaseCombatWeapon *ToHoeBaseCombatWeapon(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsBaseCombatWeapon())
		return NULL;

	return dynamic_cast<CHoe_BaseCombatWeapon*>(pEntity);
}

#endif // HOE_BASECOMBATWEAPON_SHARED_H
