//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		SLAM 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef	HOE_WEAPON_TRIPMINE_H
#define	HOE_WEAPON_TRIPMINE_H

#include "basegrenade_shared.h"
#include "hoe_basecombatweapon.h"

enum SlamState_t
{
	SLAM_TRIPMINE_READY,
	SLAM_SATCHEL_THROW,
	SLAM_SATCHEL_ATTACH,
};

class CHoe_Weapon_Tripmine : public CHoe_BaseCombatWeapon
{
public:
	DECLARE_CLASS(CHoe_Weapon_Tripmine, CHoe_BaseCombatWeapon);

	DECLARE_SERVERCLASS();

	SlamState_t			m_tSlamState;
	bool				m_bNeedReload;
	bool				m_bClearReload;
	bool				m_bAttachTripmine;

	void				Spawn(void);
	void				Precache(void);

	int					CapabilitiesGet(void) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
	void				PrimaryAttack(void);
	void				SecondaryAttack(void);
	void				WeaponIdle(void);
	void				WeaponSwitch(void);
	void				TripmineThink(void);

	void				SetPickupTouch(void);
	void				TripmineTouch(CBaseEntity *pOther);	// default weapon touch
	void				ItemPostFrame(void);
	bool				Reload(void);
	void				SetSlamState(SlamState_t newState);
	bool				CanAttachTripmine(void);		// In position where can attach Tripmine?
	void				StartTripmineAttach(void);
	void				TripmineAttach(void);


	bool				Deploy(void);
	bool				Holster(CBaseCombatWeapon *pSwitchingTo = NULL);


	CHoe_Weapon_Tripmine();

	DECLARE_ACTTABLE();
	DECLARE_DATADESC();
};

#endif	//HOE_WEAPON_TRIPMINE_H