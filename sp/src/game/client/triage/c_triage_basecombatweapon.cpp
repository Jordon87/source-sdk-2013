//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_triage_basecombatweapon.h"
#include "igamemovement.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT( C_TriageMachineGun, DT_TriageMachineGun, CTriageMachineGun )
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_TriageSelectFireMachineGun, DT_TriageSelectFireMachineGun, CTriageSelectFireMachineGun )
END_RECV_TABLE()

IMPLEMENT_CLIENTCLASS_DT( C_BaseTriageBludgeonWeapon, DT_BaseTriageBludgeonWeapon, CBaseTriageBludgeonWeapon )
END_RECV_TABLE()

void CC_ToggleIronSights(void)
{
	CBasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if (pPlayer == NULL)
		return;

	CBaseTriageCombatWeapon *pWeapon = ToBaseTriageCombatWeapon(pPlayer->GetActiveWeapon());
	if (pWeapon == NULL)
		return;

	pWeapon->ToggleIronsights();

	engine->ServerCmd("toggle_ironsight"); //forward to server
}

static ConCommand toggle_ironsight("toggle_ironsight", CC_ToggleIronSights);