//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BASEWEAPON_DUALPISTOL_H
#define ELEVENEIGHTYSEVEN_BASEWEAPON_DUALPISTOL_H

#include "1187_baseweapon_pistol.h"

#if defined( _WIN32 )
#pragma once
#endif

class C1187_BaseWeapon_DualPistol : public C1187_BaseWeapon_Pistol
{
	DECLARE_CLASS(C1187_BaseWeapon_DualPistol, C1187_BaseWeapon_Pistol);
public:
	DECLARE_DATADESC();

	C1187_BaseWeapon_DualPistol();

	virtual bool	Deploy(void);
	virtual void	FinishReload(void);
	virtual void	ItemPostFrame(void);

	void	PrimaryAttack(void);
	void	SecondaryAttack(void);

	virtual int		GetMaxClipLeft() const;
	virtual int		GetMaxClipRight() const;

	// Ironsights
	virtual bool			HasIronsights(void) { return false; }

private:

	void	DoFire(CBaseCombatCharacter* pOperator, Vector vecAiming, bool bSecondary = false);

private:
	int m_iClipLeft;
	int m_iClipRight;

	int m_iMaxClipLeft;
	int m_iMaxClipRight;
};

#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_DUALPISTOL_H