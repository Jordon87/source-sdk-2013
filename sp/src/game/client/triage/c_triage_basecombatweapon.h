//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "triage_basecombatweapon_shared.h"

#ifndef C_TRIAGE_BASECOMBATWEAPON_H
#define C_TRIAGE_BASECOMBATWEAPON_H
#ifdef _WIN32
#pragma once
#endif

class C_TriageMachineGun : public C_BaseTriageCombatWeapon
{
public:
	DECLARE_CLASS( C_TriageMachineGun, C_BaseTriageCombatWeapon );
	DECLARE_CLIENTCLASS();
};

class C_TriageSelectFireMachineGun : public C_TriageMachineGun
{
public:
	DECLARE_CLASS( C_TriageSelectFireMachineGun, C_TriageMachineGun );
	DECLARE_CLIENTCLASS();
};

class C_BaseTriageBludgeonWeapon : public C_BaseTriageCombatWeapon
{
public:
	DECLARE_CLASS( C_BaseTriageBludgeonWeapon, C_BaseTriageCombatWeapon );
	DECLARE_CLIENTCLASS();
};

#endif // C_TRIAGE_BASECOMBATWEAPON_H
