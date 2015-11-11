//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_1187_basecombatweapon.h"
#include "igamemovement.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_1187MachineGun, DT_1187MachineGun, C1187MachineGun )
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_1187SelectFireMachineGun, DT_1187SelectFireMachineGun, C1187SelectFireMachineGun )
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_Base1187BludgeonWeapon, DT_Base1187BludgeonWeapon, CBase1187BludgeonWeapon )
END_RECV_TABLE()

void CC_ToggleIronSights(void)
{
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (pPlayer == NULL)
		return;

	C_Base1187CombatWeapon *pWeapon = ToBase1187CombatWeapon(pPlayer->GetActiveWeapon());
	if (pWeapon == NULL)
		return;

	pWeapon->ToggleIronsights();

	engine->ServerCmd("toggle_ironsight"); //forward to server
}

static ConCommand toggle_ironsight("toggle_ironsight", CC_ToggleIronSights);