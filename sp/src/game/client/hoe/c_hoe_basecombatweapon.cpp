//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_hoe_basecombatweapon.h"
#include "igamemovement.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_Hoe_MachineGun, DT_Hoe_MachineGun, CHoe_MachineGun )
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_Hoe_SelectFireMachineGun, DT_Hoe_SelectFireMachineGun, CHoe_SelectFireMachineGun )
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_Hoe_BaseBludgeonWeapon, DT_Hoe_BaseBludgeonWeapon, CHoe_BaseBludgeonWeapon )
END_RECV_TABLE()


void CC_ToggleIronSights(void)
{
	CBasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (pPlayer == NULL)
		return;

	CHoe_BaseCombatWeapon *pWeapon = ToHoeBaseCombatWeapon( pPlayer->GetActiveWeapon() );
	if (pWeapon == NULL)
		return;

	pWeapon->ToggleIronsights();

	engine->ServerCmd("toggle_ironsight"); //forward to server
}

static ConCommand toggle_ironsight("toggle_ironsight", CC_ToggleIronSights);