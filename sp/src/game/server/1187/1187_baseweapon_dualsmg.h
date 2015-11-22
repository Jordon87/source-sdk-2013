//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BASEWEAPON_DUALSMG_H
#define ELEVENEIGHTYSEVEN_BASEWEAPON_DUALSMG_H

#include "1187_baseweapon_smg.h"

#if defined( _WIN32 )
#pragma once
#endif

class C1187_BaseWeapon_DualSMG : public C1187_BaseWeapon_SMG
{
	DECLARE_CLASS(C1187_BaseWeapon_DualSMG, C1187_BaseWeapon_SMG);
public:
	DECLARE_DATADESC();

	C1187_BaseWeapon_DualSMG();

	virtual bool	Deploy(void);
	virtual void	FinishReload(void);
	virtual void	ItemPostFrame(void);

	void	PrimaryAttack(void);
	void	SecondaryAttack(void);

	virtual void	AddViewKickLeft(void) { return; }
	virtual void	AddViewKickRight(void) { return; }

	virtual int		GetMaxClipLeft() const;
	virtual int		GetMaxClipRight() const;

	// Ironsights
	virtual bool			HasIronsights(void) { return false; }

private:

	void	DoFire( CBaseCombatCharacter* pOperator, Vector vecAiming, bool bSecondary = false );

private:
	int m_iClipLeft;
	int m_iClipRight;

	int m_iMaxClipLeft;
	int m_iMaxClipRight;
};

#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_DUALSMG_H