//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_PLAYERANIMSTATE_H
#define ELEVENEIGHTYSEVEN_PLAYERANIMSTATE_H

#ifdef _WIN32
#pragma once
#endif

#if defined( CLIENT_DLL )
#define C1187_Player C_1187_Player
#endif

class C1187PlayerAnimState
{
public:
	enum
	{
		TURN_NONE = 0,
		TURN_LEFT,
		TURN_RIGHT
	};

	C1187PlayerAnimState(C1187_Player *outer);

	Activity			BodyYawTranslateActivity(Activity activity);

	void				Update();

	const QAngle&		GetRenderAngles();

	void				GetPoseParameters(CStudioHdr *pStudioHdr, float poseParameter[MAXSTUDIOPOSEPARAM]);

	C1187_Player*		GetOuter();

private:
	void				GetOuterAbsVelocity(Vector& vel);

	int					ConvergeAngles(float goal, float maxrate, float dt, float& current);

	void				EstimateYaw(void);
	void				ComputePoseParam_BodyYaw(void);
	void				ComputePoseParam_BodyPitch(CStudioHdr *pStudioHdr);
	void				ComputePoseParam_BodyLookYaw(void);

	void				ComputePlaybackRate();

	C1187_Player*		m_pOuter;

	float				m_flGaitYaw;
	float				m_flStoredCycle;

	// The following variables are used for tweaking the yaw of the upper body when standing still and
	//  making sure that it smoothly blends in and out once the player starts moving
	// Direction feet were facing when we stopped moving
	float				m_flGoalFeetYaw;
	float				m_flCurrentFeetYaw;

	float				m_flCurrentTorsoYaw;

	// To check if they are rotating in place
	float				m_flLastYaw;
	// Time when we stopped moving
	float				m_flLastTurnTime;

	// One of the above enums
	int					m_nTurningInPlace;

	QAngle				m_angRender;

	float				m_flTurnCorrectionTime;
};

#endif // ELEVENEIGHTYSEVEN_PLAYERANIMSTATE_H