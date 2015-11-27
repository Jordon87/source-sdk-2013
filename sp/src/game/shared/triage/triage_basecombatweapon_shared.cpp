//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "in_buttons.h"
#include "gamestats.h"
#include "rumble_shared.h"
#include "triage_basecombatweapon_shared.h"
#include "triage_player_shared.h"

#if !defined ( CLIENT_DLL )
// SERVER SIDE
#else
// CLIENT SIDE
#include "prediction.h"
#endif

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

LINK_ENTITY_TO_CLASS( basetriagecombatweapon, CBaseTriageCombatWeapon );

IMPLEMENT_NETWORKCLASS_ALIASED( BaseTriageCombatWeapon , DT_BaseTriageCombatWeapon )

#if defined ( CLIENT_DLL )
void RecvProxy_ToggleSights(const CRecvProxyData* pData, void* pStruct, void* pOut)
{
	CBaseTriageCombatWeapon *pWeapon = (CBaseTriageCombatWeapon*)pStruct;
	if (pData->m_Value.m_Int)
		pWeapon->EnableIronsights();
	else
		pWeapon->DisableIronsights();
}
#endif

BEGIN_NETWORK_TABLE( CBaseTriageCombatWeapon , DT_BaseTriageCombatWeapon )
#if !defined( CLIENT_DLL )
	SendPropBool(SENDINFO(m_bIsIronsighted)),
	SendPropFloat( SENDINFO( m_flIronsightedTime ) ),
#else
	RecvPropInt( RECVINFO( m_bIsIronsighted ), 0, RecvProxy_ToggleSights ), //note: RecvPropBool is actually RecvPropInt (see its implementation), but we need a proxy
	RecvPropFloat(RECVINFO(m_flIronsightedTime)),
#endif
END_NETWORK_TABLE()


#if !defined( CLIENT_DLL )
// SERVER SIDE

#include "globalstate.h"

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CBaseTriageCombatWeapon )
	DEFINE_FIELD(m_bIsIronsighted, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flIronsightedTime, FIELD_FLOAT),
END_DATADESC()

#else

// CLIENT SIDE

BEGIN_PREDICTION_DATA( CBaseTriageCombatWeapon )
	DEFINE_PRED_FIELD(m_bIsIronsighted, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE),
	DEFINE_PRED_FIELD(m_flIronsightedTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE),
END_PREDICTION_DATA()
#endif

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

CBaseTriageCombatWeapon::CBaseTriageCombatWeapon()
{
	m_bIsIronsighted = false;
	m_flIronsightedTime = 0.0f;
}

CBaseTriageCombatWeapon::~CBaseTriageCombatWeapon()
{
}

Activity CBaseTriageCombatWeapon::GetPrimaryAttackActivity()
{
	return (!HasIronsights() || !IsIronsighted()) ? BaseClass::GetPrimaryAttackActivity() : GetIronsightsPrimaryAttackActivity();
}

Activity CBaseTriageCombatWeapon::GetSecondaryAttackActivity()
{
	return (!HasIronsights() || !IsIronsighted()) ? BaseClass::GetSecondaryAttackActivity() : GetIronsightsSecondaryAttackActivity();
}

Vector CBaseTriageCombatWeapon::GetBulletSpread(WeaponProficiency_t proficiency)
{
	if (m_bIsIronsighted)
	{
		static const Vector cone = VECTOR_CONE_1DEGREES;
		return cone;
	}
	else
	{
		static const Vector cone = VECTOR_CONE_5DEGREES;
		return cone;
	}
}

//====================================================================================
// WEAPON BEHAVIOUR
//====================================================================================

void CBaseTriageCombatWeapon::ItemPostFrame(void)
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
		return;

	UpdateAutoFire();

	//Track the duration of the fire
	//FIXME: Check for IN_ATTACK2 as well?
	//FIXME: What if we're calling ItemBusyFrame?
	m_fFireDuration = (pOwner->m_nButtons & IN_ATTACK) ? (m_fFireDuration + gpGlobals->frametime) : 0.0f;

	if (UsesClipsForAmmo1())
	{
		CheckReload();
	}

	bool bFired = false;

	// Secondary attack has priority
	if ((pOwner->m_nButtons & IN_ATTACK2) && IsSecondaryAttackAllowed() && (m_flNextSecondaryAttack <= gpGlobals->curtime))
	{
		if (UsesSecondaryAmmo() && pOwner->GetAmmoCount(m_iSecondaryAmmoType) <= 0)
		{
			if (m_flNextEmptySoundTime < gpGlobals->curtime)
			{
				WeaponSound(EMPTY);
				m_flNextSecondaryAttack = m_flNextEmptySoundTime = gpGlobals->curtime + 0.5;
			}
		}
		else if (pOwner->GetWaterLevel() == 3 && m_bAltFiresUnderwater == false)
		{
			// This weapon doesn't fire underwater
			WeaponSound(EMPTY);
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
			return;
		}
		else
		{
			// FIXME: This isn't necessarily true if the weapon doesn't have a secondary fire!
			// For instance, the crossbow doesn't have a 'real' secondary fire, but it still 
			// stops the crossbow from firing on the 360 if the player chooses to hold down their
			// zoom button. (sjb) Orange Box 7/25/2007
#if !defined(CLIENT_DLL)
			if (!IsX360() || !ClassMatches("weapon_crossbow"))
#endif
			{
				bFired = ShouldBlockPrimaryFire();
			}

			SecondaryAttack();

			// Secondary ammo doesn't have a reload animation
			if (UsesClipsForAmmo2())
			{
				// reload clip2 if empty
				if (m_iClip2 < 1)
				{
					pOwner->RemoveAmmo(1, m_iSecondaryAmmoType);
					m_iClip2 = m_iClip2 + 1;
				}
			}
		}
	}

	if (!bFired && (pOwner->m_nButtons & IN_ATTACK) && IsPrimaryAttackAllowed() && (m_flNextPrimaryAttack <= gpGlobals->curtime))
	{
		// Clip empty? Or out of ammo on a no-clip weapon?
		if (!IsMeleeWeapon() &&
			((UsesClipsForAmmo1() && m_iClip1 <= 0) || (!UsesClipsForAmmo1() && pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)))
		{
			HandleFireOnEmpty();
		}
		else if (pOwner->GetWaterLevel() == 3 && m_bFiresUnderwater == false)
		{
			// This weapon doesn't fire underwater
			WeaponSound(EMPTY);
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
			return;
		}
		else
		{
			//NOTENOTE: There is a bug with this code with regards to the way machine guns catch the leading edge trigger
			//			on the player hitting the attack key.  It relies on the gun catching that case in the same frame.
			//			However, because the player can also be doing a secondary attack, the edge trigger may be missed.
			//			We really need to hold onto the edge trigger and only clear the condition when the gun has fired its
			//			first shot.  Right now that's too much of an architecture change -- jdw

			// If the firing button was just pressed, or the alt-fire just released, reset the firing time
			if ((pOwner->m_afButtonPressed & IN_ATTACK) || (pOwner->m_afButtonReleased & IN_ATTACK2))
			{
				m_flNextPrimaryAttack = gpGlobals->curtime;
			}

			PrimaryAttack();

			if (AutoFiresFullClip())
			{
				m_bFiringWholeClip = true;
			}

#ifdef CLIENT_DLL
			pOwner->SetFiredWeapon(true);
#endif
		}
	}

	// -----------------------
	//  Reload pressed / Clip Empty
	// -----------------------
	if ((pOwner->m_nButtons & IN_RELOAD) && UsesClipsForAmmo1() && !m_bInReload)
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
		m_fFireDuration = 0.0f;
	}

	// -----------------------
	//  No buttons down
	// -----------------------
	if (!((pOwner->m_nButtons & IN_ATTACK) || (pOwner->m_nButtons & IN_ATTACK2) || (CanReload() && pOwner->m_nButtons & IN_RELOAD)))
	{
		// no fire buttons down or reloading
		if (!ReloadOrSwitchWeapons() && (m_bInReload == false))
		{
			WeaponIdle();
		}
	}
}

void CBaseTriageCombatWeapon::WeaponIdle(void)
{
	//See if we should idle high or low
	if (Sprint_WeaponShouldBeLowered())
	{
		// Move to lowered position if we're not there yet
		if (GetActivity() != ACT_VM_IDLE_LOWERED && GetActivity() != ACT_VM_IDLE_TO_LOWERED
			&& GetActivity() != ACT_TRANSITION)
		{
			SendWeaponAnim(ACT_VM_IDLE_LOWERED);
		}
		else if (HasWeaponIdleTimeElapsed())
		{
			// Keep idling low
			SendWeaponAnim(ACT_VM_IDLE_LOWERED);
		}
		return;
	}
	else
	{
		BaseClass::WeaponIdle();
	}
}

void CBaseTriageCombatWeapon::Drop(const Vector &vecVelocity)
{
	BaseClass::Drop(vecVelocity);

	DisableIronsights();
}

bool CBaseTriageCombatWeapon::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	bool bRet = BaseClass::Holster(pSwitchingTo);

	if (bRet)
	{
		DisableIronsights();
	}

	return bRet;
}

bool CBaseTriageCombatWeapon::Reload(void)
{
	bool bRet = BaseClass::Reload();

	if (bRet)
	{
		// Disable ironsights.
		DisableIronsights();
	}

	return bRet;
}

bool CBaseTriageCombatWeapon::DefaultReload(int iClipSize1, int iClipSize2, int iActivity)
{
	bool bRet = BaseClass::DefaultReload(iClipSize1, iClipSize2, iActivity);

	if (bRet)
	{
		// Disable ironsights.
		DisableIronsights();
	}

	return bRet;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBaseTriageCombatWeapon::IsAllowedToFire(void)
{
#if defined ( CLIENT_DLL )
	C_BaseHLPlayer* pPlayer = dynamic_cast<C_BaseHLPlayer*>(GetOwner());
#else
	CHL2_Player* pPlayer = dynamic_cast<CHL2_Player*>(GetOwner());
#endif

	if (pPlayer)
	{
		if (pPlayer->IsSprinting())
			return false;
	}

	return true;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBaseTriageCombatWeapon::IsPrimaryAttackAllowed(void)
{
	bool bRet = IsAllowedToFire();

	if (bRet)
	{
		// Add custom primary attack blocking conditions.
	}

	return bRet;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBaseTriageCombatWeapon::IsSecondaryAttackAllowed(void)
{
	bool bRet = IsAllowedToFire();

	if (bRet)
	{
		// Add custom secondary attack blocking conditions.
	}

	return bRet;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBaseTriageCombatWeapon::Sprint_WeaponShouldBeLowered(void)
{
	// Can't be in the middle of another animation
	if (GetIdealActivity() != ACT_VM_IDLE_LOWERED && GetIdealActivity() != ACT_VM_IDLE &&
		GetIdealActivity() != ACT_VM_IDLE_TO_LOWERED && GetIdealActivity() != ACT_VM_LOWERED_TO_IDLE)
		return false;

#if defined ( CLIENT_DLL )
	C_BaseHLPlayer* pPlayer = dynamic_cast<C_BaseHLPlayer*>(GetOwner());
#else
	CHL2_Player* pPlayer = dynamic_cast<CHL2_Player*>(GetOwner());
#endif
	if (pPlayer)
	{
		if (pPlayer->IsSprinting())
		{
			return true;
		}
	}

	return false;
}

Activity CBaseTriageCombatWeapon::GetIronsightsPrimaryAttackActivity()
{
	return ACT_VM_PRIMARYATTACK;
}

Activity CBaseTriageCombatWeapon::GetIronsightsSecondaryAttackActivity()
{
	return ACT_VM_SECONDARYATTACK;
}

bool CBaseTriageCombatWeapon::IsIronsighted(void)
{
	return (m_bIsIronsighted || viewmodel_adjust_enabled.GetBool());
}

void CBaseTriageCombatWeapon::ToggleIronsights(void)
{
	if (m_bIsIronsighted)
		DisableIronsights();
	else
		EnableIronsights();
}

void CBaseTriageCombatWeapon::EnableIronsights(void)
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

	if (pOwner->SetFOV(this, pOwner->GetDefaultFOV() + GetIronsightFOVOffset(), 0.05f)) // 1.0f //modify the last value to adjust how fast the fov is applied
	{
		m_bIsIronsighted = true;
		SetIronsightTime();
	}
}

void CBaseTriageCombatWeapon::DisableIronsights(void)
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

	if (pOwner->SetFOV(this, 0, 0.05f)) // 0.4f //modify the last value to adjust how fast the fov is applied
	{
		m_bIsIronsighted = false;
		SetIronsightTime();
	}
}

void CBaseTriageCombatWeapon::SetIronsightTime(void)
{
	m_flIronsightedTime = gpGlobals->curtime;
}

const CTriageFileWeaponInfo_t* CBaseTriageCombatWeapon::GetTriageWpnData() const
{
	return reinterpret_cast<const CTriageFileWeaponInfo_t*>(&GetWpnData());
}

Vector CBaseTriageCombatWeapon::GetIronsightPositionOffset(void) const
{
	if (viewmodel_adjust_enabled.GetBool())
		return Vector(viewmodel_adjust_forward.GetFloat(), viewmodel_adjust_right.GetFloat(), viewmodel_adjust_up.GetFloat());

	Assert(GetTriageWpnData());

	return GetTriageWpnData()->vecIronsightPosOffset;
}

QAngle CBaseTriageCombatWeapon::GetIronsightAngleOffset(void) const
{
	if (viewmodel_adjust_enabled.GetBool())
		return QAngle(viewmodel_adjust_pitch.GetFloat(), viewmodel_adjust_yaw.GetFloat(), viewmodel_adjust_roll.GetFloat());

	Assert(GetTriageWpnData());

	return GetTriageWpnData()->angIronsightAngOffset;
}

float CBaseTriageCombatWeapon::GetIronsightFOVOffset(void) const
{
	if (viewmodel_adjust_enabled.GetBool())
		return viewmodel_adjust_fov.GetFloat();

	Assert(GetTriageWpnData());

	return GetTriageWpnData()->flIronsightFOVOffset;
}

const unsigned char& CBaseTriageCombatWeapon::GetEquipIcon(void) const
{
	Assert(GetTriageWpnData());

	return GetTriageWpnData()->cEquipIcon;
}