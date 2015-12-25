//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hoe_basecombatweapon_shared.h"

#include "hoe_player_shared.h"
#include "hoe_weapon_parse.h"

#if defined ( CLIENT_DLL )
#include "prediction.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//forward declarations of callbacks used by viewmodel_adjust_enable and viewmodel_adjust_fov
void vm_adjust_enable_callback( IConVar *pConVar, char const *pOldString, float flOldValue );
void vm_adjust_fov_callback( IConVar *pConVar, const char *pOldString, float flOldValue );
 
ConVar viewmodel_adjust_forward( "viewmodel_adjust_forward", "0", FCVAR_REPLICATED );
ConVar viewmodel_adjust_right( "viewmodel_adjust_right", "0", FCVAR_REPLICATED );
ConVar viewmodel_adjust_up( "viewmodel_adjust_up", "0", FCVAR_REPLICATED );
ConVar viewmodel_adjust_pitch( "viewmodel_adjust_pitch", "0", FCVAR_REPLICATED );
ConVar viewmodel_adjust_yaw( "viewmodel_adjust_yaw", "0", FCVAR_REPLICATED );
ConVar viewmodel_adjust_roll( "viewmodel_adjust_roll", "0", FCVAR_REPLICATED );
ConVar viewmodel_adjust_fov( "viewmodel_adjust_fov", "0", FCVAR_REPLICATED, "Note: this feature is not available during any kind of zoom", vm_adjust_fov_callback );
ConVar viewmodel_adjust_enabled( "viewmodel_adjust_enabled", "0", FCVAR_REPLICATED|FCVAR_CHEAT, "enabled viewmodel adjusting", vm_adjust_enable_callback );

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
	C_Hoe_BaseCombatWeapon *pWeapon = (C_Hoe_BaseCombatWeapon*)pStruct;
	if (pData->m_Value.m_Int)
		pWeapon->EnableIronsights();
	else
		pWeapon->DisableIronsights();
}
#endif // defined ( CLIENT_DLL )

LINK_ENTITY_TO_CLASS( hoebasecombatweapon, CHoe_BaseCombatWeapon );

IMPLEMENT_NETWORKCLASS_ALIASED( Hoe_BaseCombatWeapon , DT_Hoe_BaseCombatWeapon )

BEGIN_NETWORK_TABLE( CHoe_BaseCombatWeapon , DT_Hoe_BaseCombatWeapon )
#if !defined( CLIENT_DLL )
	SendPropBool(SENDINFO(m_bIsIronsighted)),
	SendPropFloat( SENDINFO( m_flIronsightedTime ) ),
#else
	RecvPropInt( RECVINFO( m_bIsIronsighted ), 0, RecvProxy_ToggleSights ), //note: RecvPropBool is actually RecvPropInt (see its implementation), but we need a proxy
	RecvPropFloat(RECVINFO(m_flIronsightedTime)),
#endif
END_NETWORK_TABLE()


#if !defined( CLIENT_DLL )

#include "globalstate.h"

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CHoe_BaseCombatWeapon )
	DEFINE_FIELD(m_bIsIronsighted, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flIronsightedTime, FIELD_FLOAT),
END_DATADESC()

#else

BEGIN_PREDICTION_DATA( CHoe_BaseCombatWeapon )
	DEFINE_PRED_FIELD(m_bIsIronsighted, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
	DEFINE_PRED_FIELD(m_flIronsightedTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()

#endif

CHoe_BaseCombatWeapon::CHoe_BaseCombatWeapon() : BaseClass()
{
	m_bIsIronsighted = false;
	m_flIronsightedTime = 0.0f;
}

void CHoe_BaseCombatWeapon::Drop(const Vector &vecVelocity)
{
	DisableIronsights();

	BaseClass::Drop(vecVelocity);
}

bool CHoe_BaseCombatWeapon::DefaultReload(int iClipSize1, int iClipSize2, int iActivity)
{
	bool bRet = BaseClass::DefaultReload(iClipSize1, iClipSize2, iActivity);

	if (bRet)
	{
		DisableIronsights();
	}

	return bRet;
}

bool CHoe_BaseCombatWeapon::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	bool bRet = BaseClass::Holster(pSwitchingTo);

	if (bRet)
	{
		DisableIronsights();
	}

	return bRet;
}

bool CHoe_BaseCombatWeapon::IsIronsighted(void)
{
	return (m_bIsIronsighted || viewmodel_adjust_enabled.GetBool());
}

void CHoe_BaseCombatWeapon::ToggleIronsights(void)
{
	if (m_bIsIronsighted)
		DisableIronsights();
	else
		EnableIronsights();
}

void CHoe_BaseCombatWeapon::EnableIronsights(void)
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

void CHoe_BaseCombatWeapon::DisableIronsights(void)
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

void CHoe_BaseCombatWeapon::SetIronsightTime(void)
{
	m_flIronsightedTime = gpGlobals->curtime;
}

const HoeFileWeaponInfo_t* CHoe_BaseCombatWeapon::GetHoeWpnData() const
{
	return reinterpret_cast<const HoeFileWeaponInfo_t*>(&GetWpnData());
}

Vector CHoe_BaseCombatWeapon::GetIronsightPositionOffset(void) const
{
	if (viewmodel_adjust_enabled.GetBool())
		return Vector( viewmodel_adjust_forward.GetFloat(), viewmodel_adjust_right.GetFloat(), viewmodel_adjust_up.GetFloat() );

	Assert( GetHoeWpnData() );

	return GetHoeWpnData()->vecIronsightPosOffset;
}

QAngle CHoe_BaseCombatWeapon::GetIronsightAngleOffset(void) const
{
	if (viewmodel_adjust_enabled.GetBool())
		return QAngle(viewmodel_adjust_pitch.GetFloat(), viewmodel_adjust_yaw.GetFloat(), viewmodel_adjust_roll.GetFloat());

	Assert(GetHoeWpnData());

	return GetHoeWpnData()->angIronsightAngOffset;
}

float CHoe_BaseCombatWeapon::GetIronsightFOVOffset(void) const
{
	if (viewmodel_adjust_enabled.GetBool())
		return viewmodel_adjust_fov.GetFloat();

	Assert(GetHoeWpnData());

	return GetHoeWpnData()->flIronsightFOVOffset;
}


#if defined( CLIENT_DLL )


#else

// Server stubs

#endif