//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "1187_basecombatweapon_shared.h"

#include "1187_player_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//forward declarations of callbacks used by viewmodel_adjust_enable and viewmodel_adjust_fov
void vm_adjust_enable_callback(IConVar *pConVar, char const *pOldString, float flOldValue);
void vm_adjust_fov_callback(IConVar *pConVar, const char *pOldString, float flOldValue);

ConVar viewmodel_adjust_forward("viewmodel_adjust_forward", "0", FCVAR_REPLICATED);
ConVar viewmodel_adjust_right("viewmodel_adjust_right", "0", FCVAR_REPLICATED);
ConVar viewmodel_adjust_up("viewmodel_adjust_up", "0", FCVAR_REPLICATED);
ConVar viewmodel_adjust_pitch("viewmodel_adjust_pitch", "0", FCVAR_REPLICATED);
ConVar viewmodel_adjust_yaw("viewmodel_adjust_yaw", "0", FCVAR_REPLICATED);
ConVar viewmodel_adjust_roll("viewmodel_adjust_roll", "0", FCVAR_REPLICATED);
ConVar viewmodel_adjust_fov("viewmodel_adjust_fov", "0", FCVAR_REPLICATED, "Note: this feature is not available during any kind of zoom", vm_adjust_fov_callback);
ConVar viewmodel_adjust_enabled("viewmodel_adjust_enabled", "0", FCVAR_REPLICATED | FCVAR_CHEAT, "enabled viewmodel adjusting", vm_adjust_enable_callback);

void vm_adjust_enable_callback(IConVar *pConVar, char const *pOldString, float flOldValue)
{
	ConVarRef sv_cheats("sv_cheats");
	if (!sv_cheats.IsValid() || sv_cheats.GetBool())
		return;

	ConVarRef var(pConVar);

	if (var.GetBool())
		var.SetValue("0");
}

void vm_adjust_fov_callback(IConVar *pConVar, char const *pOldString, float flOldValue)
{
	if (!viewmodel_adjust_enabled.GetBool())
		return;

	ConVarRef var(pConVar);

	CBasePlayer *pPlayer =
#ifdef GAME_DLL
		UTIL_GetCommandClient();
#else
		C_BasePlayer::GetLocalPlayer();
#endif
	if (!pPlayer)
		return;

	if (!pPlayer->SetFOV(pPlayer, pPlayer->GetDefaultFOV() + var.GetFloat(), 0.1f))
	{
		Warning("Could not set FOV\n");
		var.SetValue("0");
	}
}

#if defined ( CLIENT_DLL )
void RecvProxy_ToggleSights(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	CBase1187CombatWeapon *pWeapon = (CBase1187CombatWeapon*)pStruct;
	if (pData->m_Value.m_Int)
		pWeapon->EnableIronsights();
	else
		pWeapon->DisableIronsights();
}
#endif

LINK_ENTITY_TO_CLASS( base1187combatweapon, CBase1187CombatWeapon );

IMPLEMENT_NETWORKCLASS_ALIASED( Base1187CombatWeapon , DT_Base1187CombatWeapon )

BEGIN_NETWORK_TABLE( CBase1187CombatWeapon , DT_Base1187CombatWeapon )
#if !defined( CLIENT_DLL )
	SendPropBool(SENDINFO(m_bIsIronsighted)),
	SendPropFloat( SENDINFO( m_flIronsightedTime ) ),
#else
	RecvPropInt( RECVINFO( m_bIsIronsighted ), 0, RecvProxy_ToggleSights ), //note: RecvPropBool is actually RecvPropInt (see its implementation), but we need a proxy
	RecvPropFloat(RECVINFO(m_flIronsightedTime)),
#endif
END_NETWORK_TABLE()

#if !defined( CLIENT_DLL )

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CBase1187CombatWeapon )
	DEFINE_FIELD(m_bIsIronsighted, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flIronsightedTime, FIELD_FLOAT),
END_DATADESC()

#else

BEGIN_PREDICTION_DATA( CBase1187CombatWeapon )
	DEFINE_PRED_FIELD(m_bIsIronsighted, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
	DEFINE_PRED_FIELD(m_flIronsightedTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()

#endif

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
CBase1187CombatWeapon::CBase1187CombatWeapon()
{
	m_bIsIronsighted = false;
	m_flIronsightedTime = 0.0f;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
CBase1187CombatWeapon::~CBase1187CombatWeapon()
{
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::Drop(const Vector &vecVelocity)
{
	BaseClass::Drop(vecVelocity);

	DisableIronsights();
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBase1187CombatWeapon::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	bool bRet = BaseClass::Holster(pSwitchingTo);

	if (bRet)
	{
		DisableIronsights();
	}

	return bRet;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBase1187CombatWeapon::DefaultReload(int iClipSize1, int iClipSize2, int iActivity)
{
	bool bRet = BaseClass::DefaultReload(iClipSize1, iClipSize2, iActivity);

	if (bRet)
	{
		DisableIronsights();
	}

	return bRet;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::Operator_HandleJumpEvent(float fImpulse, CBaseCombatCharacter *pOperator)
{
	if (SelectWeightedSequence(ACT_JUMP) != ACTIVITY_NOT_AVAILABLE)
	{
		SendWeaponAnim(ACT_JUMP);
	}
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::Operator_HandleLandEvent(float fVelocity, CBaseCombatCharacter *pOperator)
{
	if (SelectWeightedSequence(ACT_LAND) != ACTIVITY_NOT_AVAILABLE)
	{
		SendWeaponAnim(ACT_LAND);
	}
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
Activity CBase1187CombatWeapon::GetPrimaryAttackActivity(void)
{
	return (!HasIronsights() || !IsIronsighted()) ? BaseClass::GetPrimaryAttackActivity() : GetIronsightsPrimaryAttackActivity();
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
Activity CBase1187CombatWeapon::GetSecondaryAttackActivity(void)
{
	return (!HasIronsights() || !IsIronsighted()) ? BaseClass::GetSecondaryAttackActivity() : GetIronsightsSecondaryAttackActivity();
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
Activity CBase1187CombatWeapon::GetIronsightsPrimaryAttackActivity(void)
{
	return ACT_VM_RECOIL1;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
Activity CBase1187CombatWeapon::GetIronsightsSecondaryAttackActivity(void)
{
	return ACT_VM_RECOIL1;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBase1187CombatWeapon::IsIronsighted(void)
{
	return (m_bIsIronsighted || viewmodel_adjust_enabled.GetBool());
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::ToggleIronsights(void)
{
	if (m_bIsIronsighted)
		DisableIronsights();
	else
		EnableIronsights();
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::EnableIronsights(void)
{
/*#ifdef CLIENT_DLL
	if (!prediction->IsFirstTimePredicted())
		return;
#endif*/
	if (!HasIronsights() || m_bIsIronsighted)
		return;

	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (!pOwner)
		return;

	if (pOwner->SetFOV(this, pOwner->GetDefaultFOV() + GetIronsightFOVOffset(), 1.0f)) //modify the last value to adjust how fast the fov is applied
	{
		m_bIsIronsighted = true;
		SetIronsightTime();
	}
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::DisableIronsights(void)
{
/*#ifdef CLIENT_DLL
	if (!prediction->IsFirstTimePredicted())
		return;
#endif*/
	if (!HasIronsights() || !m_bIsIronsighted)
		return;

	CBasePlayer *pOwner = ToBasePlayer(GetOwner());

	if (!pOwner)
		return;

	if (pOwner->SetFOV(this, 0, 0.4f)) //modify the last value to adjust how fast the fov is applied
	{
		m_bIsIronsighted = false;
		SetIronsightTime();
	}
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::SetIronsightTime(void)
{
	m_flIronsightedTime = gpGlobals->curtime;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
const C1187FileWeaponInfo_t* CBase1187CombatWeapon::Get1187WpnData(void) const
{
	return reinterpret_cast<const C1187FileWeaponInfo_t*>(&GetWpnData());
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
Vector CBase1187CombatWeapon::GetIronsightPositionOffset(void) const
{
	if (viewmodel_adjust_enabled.GetBool())
		return Vector(viewmodel_adjust_forward.GetFloat(), viewmodel_adjust_right.GetFloat(), viewmodel_adjust_up.GetFloat());

#ifdef _DEBUG
	Assert(Get1187WpnData());
#endif

	return Get1187WpnData()->vecIronsightPosOffset;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
QAngle CBase1187CombatWeapon::GetIronsightAngleOffset(void) const
{
	if (viewmodel_adjust_enabled.GetBool())
		return QAngle(viewmodel_adjust_pitch.GetFloat(), viewmodel_adjust_yaw.GetFloat(), viewmodel_adjust_roll.GetFloat());

#ifdef _DEBUG
	Assert(Get1187WpnData());
#endif

	return Get1187WpnData()->angIronsightAngOffset;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
float CBase1187CombatWeapon::GetIronsightFOVOffset(void) const
{
	if (viewmodel_adjust_enabled.GetBool())
		return viewmodel_adjust_fov.GetFloat();

#ifdef _DEBUG
	Assert(Get1187WpnData());
#endif

	return Get1187WpnData()->flIronsightFOVOffset;
}