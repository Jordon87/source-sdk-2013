//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "in_buttons.h"
#include "gamestats.h"
#include "rumble_shared.h"
#include "1187_basecombatweapon_shared.h"

#include "1187_player_shared.h"

#if defined ( CLIENT_DLL )
#include "iviewrender_beams.h"			// flashlight beam
#include "r_efx.h"
#include "dlight.h"
#include "flashlighteffect.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if defined ( CLIENT_DLL )
#define FLASHLIGHT_DISTANCE		1000
#endif

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
	DEFINE_FIELD(m_bLoweredOnSprint, FIELD_BOOLEAN),
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
	m_bLoweredOnSprint = false;

#if defined ( CLIENT_DLL )
	m_pFlashlight = NULL;
	m_pFlashlightBeam = NULL;
#endif
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
CBase1187CombatWeapon::~CBase1187CombatWeapon()
{
#if defined ( CLIENT_DLL )
	DestroyFlashlightEffects();
#endif
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBase1187CombatWeapon::IsAllowedToFire(void)
{
	if (Sprint_IsWeaponLowered())
		return false;

#if defined ( CLIENT_DLL )
	C_1187_Player* pPlayer = To1187Player(GetOwner());
#else
	C1187_Player* pPlayer = To1187Player(GetOwner());
#endif

	if (pPlayer)
	{
		if (pPlayer->IsSprinting())
			return false;

		if (pPlayer->WallProximity_IsAdjacentToWall())
			return false;
	}

	return true;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBase1187CombatWeapon::IsPrimaryAttackAllowed(void)
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
bool CBase1187CombatWeapon::IsSecondaryAttackAllowed(void)
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

#if defined ( CLIENT_DLL )
		DestroyFlashlightEffects();
#endif
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


//====================================================================================
// WEAPON BEHAVIOUR
//====================================================================================
void CBase1187CombatWeapon::ItemPostFrame(void)
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


//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::WeaponIdle(void)
{
	if (Sprint_WeaponShouldBeLowered())
	{
		// Move to lowered position if we're not there yet
		if (GetActivity() != ACT_VM_SPRINT_IDLE && GetActivity() != ACT_VM_SPRINT_ENTER
			&& GetActivity() != ACT_TRANSITION)
		{
			SendWeaponAnim(ACT_VM_SPRINT_IDLE);
		}
		else if (HasWeaponIdleTimeElapsed())
		{
			// Keep idling low
			SendWeaponAnim(ACT_VM_SPRINT_IDLE);
		}
		return;
	}
	else
	{
		if (GetOwner() && GetOwner()->IsPlayer() && (GetOwner()->GetFlags() & FL_DUCKING))
		{
			float speed = GetOwner()->GetAbsVelocity().Length2D();

			if (speed > 1.0f)
			{
				// Move to lowered position if we're not there yet
				if (GetActivity() != GetPrimaryAttackActivity() && GetActivity() != GetSecondaryAttackActivity() &&
					GetActivity() != ACT_CROUCH && GetActivity() != ACT_TRANSITION)
				{
					SendWeaponAnim(ACT_CROUCH);
				}
				else if (HasWeaponIdleTimeElapsed())
				{
					// Keep idling low
					SendWeaponAnim(ACT_CROUCH);
				}
			}
			else
			{
				BaseClass::WeaponIdle();
			}
		}
		else
		{
			BaseClass::WeaponIdle();
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool CBase1187CombatWeapon::Sprint_IsWeaponLowered(void)
{
	return m_bLoweredOnSprint;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBase1187CombatWeapon::Sprint_WeaponShouldBeLowered(void)
{
	// Can't be in the middle of another animation
	if (GetIdealActivity() != ACT_VM_SPRINT_IDLE && GetIdealActivity() != ACT_VM_IDLE &&
		GetIdealActivity() != ACT_VM_SPRINT_ENTER && GetIdealActivity() != ACT_VM_SPRINT_LEAVE)
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
		else if (m_bLoweredOnSprint)
		{
			m_bLoweredOnSprint = false;
			return false;
		}
	}

	if (m_bLoweredOnSprint)
		return true;

	return false;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::Sprint_SetState(bool state)
{
	m_bLoweredOnSprint = state;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::Sprint_Lower(void)
{
	Sprint_SetState(true);
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
void CBase1187CombatWeapon::Sprint_Leave(void)
{
	Sprint_SetState(false);
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
void CBase1187CombatWeapon::Operator_HandleMeleeEvent(CBaseCombatCharacter *pOperator)
{
#ifndef CLIENT_DLL
	MeleeSwing();
#endif
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

const Vector& CBase1187CombatWeapon::GetDefaultBulletSpread(void)
{
	static Vector cone = VECTOR_CONE_15DEGREES;
	return cone;
}

const Vector& CBase1187CombatWeapon::GetIronsightsBulletSpread(void)
{
	static Vector cone = VECTOR_CONE_PRECALCULATED;
	return cone;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
const Vector& CBase1187CombatWeapon::GetBulletSpread(void)
{
	return (!HasIronsights() || IsIronsighted()) ? GetDefaultBulletSpread() : GetIronsightsBulletSpread();
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
Vector CBase1187CombatWeapon::GetFragPositionOffset(void) const
{
#ifdef _DEBUG
	Assert(Get1187WpnData());
#endif

	return Get1187WpnData()->vecFragPosOffset;
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

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
bool CBase1187CombatWeapon::HasBuiltInFlashlight(void) const
{
#ifdef _DEBUG
	Assert(Get1187WpnData());
#endif

	return Get1187WpnData()->bHasFlashlight;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
int CBase1187CombatWeapon::GetWeaponDamage(void) const
{
#ifdef _DEBUG
	Assert(Get1187WpnData());
#endif

	return Get1187WpnData()->iDamage;
}

//----------------------------------------------------------------------------
// Purpose:
//----------------------------------------------------------------------------
int	CBase1187CombatWeapon::GetWeaponMeleeDamage(void) const
{
#ifdef _DEBUG
	Assert(Get1187WpnData());
#endif

	return Get1187WpnData()->iMeleeDamage;
}

#if defined ( CLIENT_DLL )
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_Base1187CombatWeapon::NotifyShouldTransmit(ShouldTransmitState_t state)
{
	if (state == SHOULDTRANSMIT_END)
	{
		DestroyFlashlightEffects();
	}

	BaseClass::NotifyShouldTransmit(state);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_Base1187CombatWeapon::Simulate()
{
	BaseClass::Simulate();

	C_BaseCombatCharacter* pOwner = GetOwner();
	if (!pOwner)
		return;

	if (pOwner->GetActiveWeapon() == this && HasBuiltInFlashlight())
	{
		UpdateFlashlight();
	}
	else if ( m_pFlashlight )
	{
		DestroyFlashlightEffects();
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void C_Base1187CombatWeapon::UpdateFlashlight(void)
{
	C_BasePlayer* pPlayer = dynamic_cast<C_BasePlayer*>(GetOwner());
	if (!pPlayer)
		return;

	// The dim light is the flashlight.
	if (pPlayer->IsEffectActive(EF_DIMLIGHT))
	{
		C_BaseViewModel* vm = pPlayer->GetViewModel();
		if (!vm)
			return;

		int iAttachment = vm->LookupAttachment("muzzle");

		if (iAttachment < 0)
			return;

		if (!m_pFlashlight)
		{
			// Turned on the headlight; create it.
			m_pFlashlight = new CFlashlightEffect(index);

			if (!m_pFlashlight)
				return;

			m_pFlashlight->TurnOn();
		}


		QAngle angles;
		Vector vecOrigin, vecForward, vecRight, vecUp;
		vm->GetAttachment(iAttachment, vecOrigin, angles);

		AngleVectors(angles, &vecForward, &vecRight, &vecUp);

		// Update the light with the new position and direction.		
		m_pFlashlight->UpdateLight(vecOrigin, vecForward, vecRight, vecUp, FLASHLIGHT_DISTANCE);

		trace_t tr;
		UTIL_TraceLine(vecOrigin, vecOrigin + (vecForward * 200), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr);

		if (!m_pFlashlightBeam)
		{
			BeamInfo_t beamInfo;
			beamInfo.m_nType = TE_BEAMPOINTS;
			beamInfo.m_vecStart = tr.startpos;
			beamInfo.m_vecEnd = tr.endpos;
			beamInfo.m_pszModelName = "sprites/glow01.vmt";
			beamInfo.m_pszHaloName = "sprites/glow01.vmt";
			beamInfo.m_flHaloScale = 3.0;
			beamInfo.m_flWidth = 8.0f;
			beamInfo.m_flEndWidth = 35.0f;
			beamInfo.m_flFadeLength = 300.0f;
			beamInfo.m_flAmplitude = 0;
			beamInfo.m_flBrightness = 60.0;
			beamInfo.m_flSpeed = 0.0f;
			beamInfo.m_nStartFrame = 0.0;
			beamInfo.m_flFrameRate = 0.0;
			beamInfo.m_flRed = 255.0;
			beamInfo.m_flGreen = 255.0;
			beamInfo.m_flBlue = 255.0;
			beamInfo.m_nSegments = 8;
			beamInfo.m_bRenderable = true;
			beamInfo.m_flLife = 0.5;
			beamInfo.m_nFlags = FBEAM_FOREVER | FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

			m_pFlashlightBeam = beams->CreateBeamPoints(beamInfo);
		}

		if (m_pFlashlightBeam)
		{
			BeamInfo_t beamInfo;
			beamInfo.m_vecStart = tr.startpos;
			beamInfo.m_vecEnd = tr.endpos;
			beamInfo.m_flRed = 255.0;
			beamInfo.m_flGreen = 255.0;
			beamInfo.m_flBlue = 255.0;

			beams->UpdateBeamInfo(m_pFlashlightBeam, beamInfo);

			dlight_t *el = effects->CL_AllocDlight(0);
			el->origin = tr.endpos;
			el->radius = 50;
			el->color.r = 200;
			el->color.g = 200;
			el->color.b = 200;
			el->die = gpGlobals->curtime + 0.1;
		}
	}
	else if (m_pFlashlight)
	{
		ReleaseFlashlight();

		// Turned off the flashlight; delete it.
		delete m_pFlashlight;
		m_pFlashlight = NULL;
	}
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBase1187CombatWeapon::DestroyFlashlightEffects()
{
	if (m_pFlashlight)
	{
		ReleaseFlashlight();

		delete m_pFlashlight;
		m_pFlashlight = NULL;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CBase1187CombatWeapon::ReleaseFlashlight(void)
{
	if (m_pFlashlightBeam)
	{
		m_pFlashlightBeam->flags = 0;
		m_pFlashlightBeam->die = gpGlobals->curtime - 1;

		m_pFlashlightBeam = NULL;
	}
}
#endif