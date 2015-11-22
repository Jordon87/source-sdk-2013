//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_1187_player.h"
#include "playerandobjectenumerator.h"
#include "engine/ivdebugoverlay.h"
#include "c_ai_basenpc.h"
#include "in_buttons.h"
#include "collisionutils.h"
#include "view.h"
#include "iviewrender.h"
#include "view_shared.h"
#include "view_scene.h"
#include "cam_thirdperson.h"

#include "iviewrender_beams.h"			// flashlight beam
#include "r_efx.h"
#include "dlight.h"
#include "flashlighteffect.h"
#include "c_1187_basecombatweapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef C1187_Player
#undef C1187_Player
#endif

#define FLASHLIGHT_DISTANCE		1000

// How fast to avoid collisions with center of other object, in units per second
#define AVOID_SPEED 2000.0f
extern ConVar cl_forwardspeed;
extern ConVar cl_backspeed;
extern ConVar cl_sidespeed;

extern ConVar zoom_sensitivity_ratio;
extern ConVar default_fov;
extern ConVar sensitivity;

extern ConVar mat_motion_blur_enabled;
extern ConVar mat_motion_blur_falling_min;
extern ConVar mat_motion_blur_falling_max;

static ConVar cl_1187_ironblur_enabled("cl_1187_ironblur_enabled", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, "Enable/Disable Ironsights motion blur.\n");
static ConVar cl_1187_sprintblur_enabled("cl_1187_sprintblur_enabled", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE, "Enable/Disable sprint motion blur.\n");

IMPLEMENT_CLIENTCLASS_DT(C_1187_Player, DT_1187_Player, C1187_Player)
	RecvPropDataTable( RECVINFO_DT(m_1187Local),0, &REFERENCE_RECV_TABLE(DT_1187Local) ),

#if 0
	RecvPropFloat(RECVINFO(m_angEyeAngles[0])),
	RecvPropFloat(RECVINFO(m_angEyeAngles[1])),
#endif

	RecvPropBool(RECVINFO(m_bAdjacentToWall)),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_1187_Player )
	DEFINE_PRED_TYPEDESCRIPTION( m_1187Local, C_1187PlayerLocalData ),
END_PREDICTION_DATA()

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
#if 1
C_1187_Player::C_1187_Player()
#else
C_1187_Player::C_1187_Player() : m_PlayerAnimState(this), m_iv_angEyeAngles("C_HL2MP_Player::m_iv_angEyeAngles")
#endif
{
#if 0
	m_angEyeAngles.Init();

	AddVar(&m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR);
#endif

	m_bAdjacentToWall = false;
}

C_1187_Player::~C_1187_Player()
{

}

void C_1187_Player::PreThink(void)
{
	QAngle vTempAngles = GetLocalAngles();

	if (GetLocalPlayer() == this)
	{
		vTempAngles[PITCH] = EyeAngles()[PITCH];
	}
	else
	{
#if 0
		vTempAngles[PITCH] = m_angEyeAngles[PITCH];
#endif
	}

	if (vTempAngles[YAW] < 0.0f)
	{
		vTempAngles[YAW] += 360.0f;
	}

	SetLocalAngles(vTempAngles);

	BaseClass::PreThink();
}

const QAngle &C_1187_Player::EyeAngles()
{
#if 0
	if (IsLocalPlayer())
	{
		return BaseClass::EyeAngles();
	}
	else
	{
		return m_angEyeAngles;
	}
#else
	return BaseClass::EyeAngles();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_1187_Player::ClientThink(void)
{
	BaseClass::ClientThink();

	UpdatePlayerEffects();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_1187_Player::AddEntity(void)
{
	BaseClass::AddEntity();

#if 0
	QAngle vTempAngles = GetLocalAngles();
	vTempAngles[PITCH] = m_angEyeAngles[PITCH];

	SetLocalAngles(vTempAngles);

	m_PlayerAnimState.Update();
#endif

	// Zero out model pitch, blending takes care of all of it.
	SetLocalAnglesDim(X_INDEX, 0);
}


const QAngle& C_1187_Player::GetRenderAngles()
{
#if 0
	if (IsRagdoll())
	{
		return vec3_angle;
	}
	else
	{
		return m_PlayerAnimState.GetRenderAngles();
	}
#else
	return BaseClass::GetRenderAngles();
#endif
}

void C_1187_Player::PostThink(void)
{
	BaseClass::PostThink();

#if 0
	// Store the eye angles pitch so the client can compute its animation state correctly.
	m_angEyeAngles = EyeAngles();
#endif
}

void C_1187_Player::UpdateFlashlight(void)
{
	// The dim light is the flashlight.
	if (IsEffectActive(EF_DIMLIGHT))
	{
		CBaseCombatWeapon* pWeapon = GetActiveWeapon();

		if (pWeapon)
		{
			CBase1187CombatWeapon* p1187Weapon = ToBase1187CombatWeapon(pWeapon);
			if (p1187Weapon && p1187Weapon->HasBuiltInFlashlight())
			{
				if (m_pFlashlight)
				{
					delete m_pFlashlight;
					m_pFlashlight = NULL;
				}
				return;
			}
		}

		if (!m_pFlashlight)
		{
			// Turned on the headlight; create it.
			m_pFlashlight = new CFlashlightEffect(index);

			if (!m_pFlashlight)
				return;

			m_pFlashlight->TurnOn();
		}

		Vector vecForward, vecRight, vecUp;
		EyeVectors(&vecForward, &vecRight, &vecUp);

		// Update the light with the new position and direction.		
		m_pFlashlight->UpdateLight(EyePosition(), vecForward, vecRight, vecUp, FLASHLIGHT_DISTANCE);
	}
	else if (m_pFlashlight)
	{
		// Turned off the flashlight; delete it.
		delete m_pFlashlight;
		m_pFlashlight = NULL;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool C_1187_Player::ShouldDraw()
{
	if (IsInAVehicle())
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Input handling
//-----------------------------------------------------------------------------
void C_1187_Player::BuildTransformations(CStudioHdr *hdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed)
{
	BaseClass::BuildTransformations(hdr, pos, q, cameraTransform, boneMask, boneComputed);
	BuildFirstPersonMeathookTransformations(hdr, pos, q, cameraTransform, boneMask, boneComputed, "ValveBiped.Bip01_Head1");
}

void C_1187_Player::UpdatePlayerEffects()
{
	if (ShouldUpdateMotionBlur())
		UpdateMotionBlur();
}

bool C_1187_Player::ShouldUpdateMotionBlur()
{
	return mat_motion_blur_enabled.GetBool();
}

bool C_1187_Player::ShouldUpdateMotionBlurIronsights()
{
	if (!cl_1187_ironblur_enabled.GetBool())
		return false;

	if (!GetActiveWeapon())
		return false;

	return true;
}

bool C_1187_Player::ShouldUpdateMotionBlurOnSprint()
{
	return cl_1187_sprintblur_enabled.GetBool();
}

void C_1187_Player::UpdateMotionBlur()
{
	bool bSprintBlur = false;
	bool bIronBlur = false;

	if (ShouldUpdateMotionBlurOnSprint())
	{
		bSprintBlur = m_fIsSprinting;
	}

	if (ShouldUpdateMotionBlurIronsights())
	{
		Assert( GetActiveWeapon() );

		C_Base1187CombatWeapon* pWeapon = ToBase1187CombatWeapon(GetActiveWeapon());

		if (pWeapon)
			bIronBlur = pWeapon->HasIronsights() && pWeapon->IsIronsighted();
	}

	if (bSprintBlur || bIronBlur)
	{
		mat_motion_blur_falling_max.SetValue(1);
		mat_motion_blur_falling_min.SetValue(20);
	}
	else
	{
		mat_motion_blur_falling_max.SetValue(mat_motion_blur_falling_max.GetDefault());
		mat_motion_blur_falling_min.SetValue(mat_motion_blur_falling_min.GetDefault());
	}
}