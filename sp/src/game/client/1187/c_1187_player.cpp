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

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef C1187_Player
#undef C1187_Player
#endif

// How fast to avoid collisions with center of other object, in units per second
#define AVOID_SPEED 2000.0f
extern ConVar cl_forwardspeed;
extern ConVar cl_backspeed;
extern ConVar cl_sidespeed;

extern ConVar zoom_sensitivity_ratio;
extern ConVar default_fov;
extern ConVar sensitivity;

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

	// Build the leg and upper body transformations.
	// BuildFirstPersonTransformations(hdr, pos, q, cameraTransform, boneMask, boneComputed);
}


void C_1187_Player::BuildFirstPersonTransformations(CStudioHdr *hdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed)
{
	// Handle meathook mode. If we aren't rendering, just use last frame's transforms
	if (!InFirstPersonView())
		return;

	// Do not draw when driving a vehicle.
	if (IsInAVehicle())
		return;

	// If we're in third-person view, don't do anything special.
	// If we're in first-person view rendering the main view and using the viewmodel, we shouldn't have even got here!
	// If we're in first-person view rendering the main view(s), meathook and headless.
	// If we're in first-person view rendering shadowbuffers/reflections, don't do anything special either (we could do meathook but with a head?)
	if (IsAboutToRagdoll())
	{
		// We're re-animating specifically to set up the ragdoll.
		// Meathook can push the player through the floor, which makes the ragdoll fall through the world, which is no good.
		// So do nothing.
		return;
	}

	if (!DrawingMainView())
	{
		return;
	}

	// If we aren't drawing the player anyway, don't mess with the bones. This can happen in Portal.
	if (!ShouldDrawThisPlayer())
	{
		return;
	}

	m_BoneAccessor.SetWritableBones(BONE_USED_BY_ANYTHING);

	QAngle bodyAngles = GetLocalAngles();

	bodyAngles.x = 0;

	Vector forward;
	AngleVectors(bodyAngles, &forward);

	Vector vLowerOffset, vUpperOffset;
	vLowerOffset = forward * -16;
	vUpperOffset = forward * -32;

	int iBoneStart, iBoneEnd;

	iBoneStart = 0;
	iBoneEnd = LookupBone("ValveBiped.Bip01_Spine4");

	if (iBoneStart == -1 || iBoneEnd == -1)
	{
		return;
	}

	int i;

	for (i = 0; i < iBoneEnd; i++)
	{
		matrix3x4_t& bone = GetBoneForWrite(i);
		Vector vBonePos;
		MatrixGetTranslation(bone, vBonePos);
		vBonePos += vLowerOffset;
		MatrixSetTranslation(vBonePos, bone);
	}

	iBoneStart = iBoneEnd;
	iBoneEnd = LookupBone("ValveBiped.Bip01_R_Thigh");

	if (iBoneStart == -1 || iBoneEnd == -1)
	{
		return;
	}

	for (i = iBoneStart; i < iBoneEnd; i++)
	{
		// Only update bones reference by the bone mask.
		if (!(hdr->boneFlags(i) & boneMask))
		{
			continue;
		}

		matrix3x4_t& bone = GetBoneForWrite(i);
		Vector vBonePos;
		MatrixGetTranslation(bone, vBonePos);
		vBonePos += vUpperOffset;
		MatrixSetTranslation(vBonePos, bone);
		MatrixScaleByZero(bone);
	}

	// Start from where we left.
	iBoneStart = iBoneEnd;

	// Define the new end.
	iBoneEnd = LookupBone("ValveBiped.Bip01_L_Finger4");

	if (iBoneStart == -1 || iBoneEnd == -1)
	{
		return;
	}

	// For each bone in this, add a small offset.
	for (i = iBoneStart; i < iBoneEnd; i++)
	{
		// Only update bones reference by the bone mask.
		if (!(hdr->boneFlags(i) & boneMask))
		{
			continue;
		}

		matrix3x4_t& bone = GetBoneForWrite(i);
		Vector vBonePos;
		MatrixGetTranslation(bone, vBonePos);
		vBonePos += vLowerOffset;
		MatrixSetTranslation(vBonePos, bone);
	}

	iBoneStart = iBoneEnd;

	if (iBoneStart == -1)
	{
		return;
	}

	// Traverse the skeleton to add an offset to the 
	for (i = iBoneStart; i < hdr->numbones(); i++)
	{
		// Only update bones reference by the bone mask.
		if (!(hdr->boneFlags(i) & boneMask))
		{
			continue;
		}

		matrix3x4_t& bone = GetBoneForWrite(i);
		Vector vBonePos;
		MatrixGetTranslation(bone, vBonePos);
		vBonePos += vUpperOffset;
		MatrixSetTranslation(vBonePos, bone);
		MatrixScaleByZero(bone);
	}
}