//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_BASEWEAPON_REVOLVER_H
#define ELEVENEIGHTYSEVEN_BASEWEAPON_REVOLVER_H

#include "1187_basecombatweapon_shared.h"

//-----------------------------------------------------------------------------
// C1187_BaseWeapon_Revolver
//-----------------------------------------------------------------------------

class C1187_BaseWeapon_Revolver : public CBase1187CombatWeapon
{
	DECLARE_CLASS(C1187_BaseWeapon_Revolver, CBase1187CombatWeapon);
public:

	C1187_BaseWeapon_Revolver(void);

	void	PrimaryAttack(void);
	void	Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);

	float	WeaponAutoAimScale()	{ return 0.6f; }
};

#endif // ELEVENEIGHTYSEVEN_BASEWEAPON_REVOLVER_H