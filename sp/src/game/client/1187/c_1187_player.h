//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//

#if !defined( C_1187_PLAYER_H )
#define C_1187_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "c_basehlplayer.h"
#include "c_1187_playerlocaldata.h"

class C_1187_Player;
#include "1187_playeranimstate.h"

class C_1187_Player : public C_BaseHLPlayer
{
public:
	DECLARE_CLASS(C_1187_Player, C_BaseHLPlayer);
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

	C_1187_Player();

	virtual void AddEntity(void);

#if 0
	QAngle GetAnimEyeAngles(void) { return m_angEyeAngles; }
#endif

	virtual const QAngle& GetRenderAngles();
	virtual void PreThink(void);
	virtual const QAngle& EyeAngles(void);

	virtual void PostThink(void);

	bool			ShouldDraw();
	bool			ShouldDrawThisPlayer() { return true; }

	void			BuildTransformations(CStudioHdr *hdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed);
	virtual void	BuildFirstPersonTransformations(CStudioHdr *hdr, Vector *pos, Quaternion q[], const matrix3x4_t& cameraTransform, int boneMask, CBoneBitList &boneComputed);

	bool			WeaponLoweredOnSprint_IsActive() const { return m_1187Local.m_bWeaponLoweredOnSprint; }
	bool			WallProximity_IsAdjacentToWall(void) const { return m_bAdjacentToWall; }

public:

	C_1187PlayerLocalData	m_1187Local;

private:
	C_1187_Player(const C_1187_Player &); // not defined, not accessible

#if 0
	C1187PlayerAnimState m_PlayerAnimState;

	QAngle	m_angEyeAngles;

	CInterpolatedVar< QAngle >	m_iv_angEyeAngles;
#endif

	bool m_bAdjacentToWall;

friend class C1187GameMovement;
};

inline C_1187_Player *To1187Player(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsPlayer())
		return NULL;

	return dynamic_cast<C_1187_Player*>(pEntity);
}



#endif // C_1187_PLAYER_H
