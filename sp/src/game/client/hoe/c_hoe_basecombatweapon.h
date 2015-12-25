//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "hoe_basecombatweapon_shared.h"

#ifndef C_HOE_BASECOMBATWEAPON_H
#define C_HOE_BASECOMBATWEAPON_H
#ifdef _WIN32
#pragma once
#endif

class C_Hoe_MachineGun : public C_Hoe_BaseCombatWeapon
{
public:
	DECLARE_CLASS(C_Hoe_MachineGun, C_Hoe_BaseCombatWeapon);
	DECLARE_CLIENTCLASS();
};

class C_Hoe_SelectFireMachineGun : public C_Hoe_MachineGun
{
public:
	DECLARE_CLASS(C_Hoe_SelectFireMachineGun, C_Hoe_MachineGun);
	DECLARE_CLIENTCLASS();
};

class C_Hoe_BaseBludgeonWeapon : public C_Hoe_BaseCombatWeapon
{
public:
	DECLARE_CLASS(C_Hoe_BaseBludgeonWeapon, C_Hoe_BaseCombatWeapon);
	DECLARE_CLIENTCLASS();
};

#endif // C_HOE_BASECOMBATWEAPON_H
