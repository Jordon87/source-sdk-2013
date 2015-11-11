//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "1187_basecombatweapon_shared.h"

#ifndef C_1187_BASECOMBATWEAPON_H
#define C_1187_BASECOMBATWEAPON_H
#ifdef _WIN32
#pragma once
#endif

class C_1187MachineGun : public C_Base1187CombatWeapon
{
public:
	DECLARE_CLASS( C_1187MachineGun, C_Base1187CombatWeapon );
	DECLARE_CLIENTCLASS();
};

class C_1187SelectFireMachineGun : public C_1187MachineGun
{
public:
	DECLARE_CLASS( C_1187SelectFireMachineGun, C_1187MachineGun );
	DECLARE_CLIENTCLASS();
};

class C_Base1187BludgeonWeapon : public C_Base1187CombatWeapon
{
public:
	DECLARE_CLASS( C_Base1187BludgeonWeapon, C_Base1187CombatWeapon );
	DECLARE_CLIENTCLASS();
};

#endif // C_1187_BASECOMBATWEAPON_H
