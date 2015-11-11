//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 1187 game movement
//
//=============================================================================//
#include "cbase.h"
#include "1187_gamemovement.h"
#include "in_buttons.h"
#include "utlrbtree.h"
#include "hl2_shareddefs.h"

#if !defined ( CLIENT_DLL )
#include "1187_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar cl_viewbob_enabled("cl_viewbob_enabled", "1", FCVAR_CLIENTDLL | FCVAR_CHEAT);

ConVar cl_viewbob_amplitude_min("cl_viewbob_amplitude_min", "0.03", FCVAR_CLIENTDLL | FCVAR_CHEAT);
ConVar cl_viewbob_amplitude_max("cl_viewbob_amplitude_max", "0.04", FCVAR_CLIENTDLL | FCVAR_CHEAT);

ConVar cl_viewbob_frequency_min("cl_viewbob_frequency_min", "2.0", FCVAR_CLIENTDLL | FCVAR_CHEAT);
ConVar cl_viewbob_frequency_max("cl_viewbob_frequency_max", "2.5", FCVAR_CLIENTDLL | FCVAR_CHEAT);

ConVar cl_viewbob_roll_min("cl_viewbob_roll_min", "0.02", FCVAR_CLIENTDLL | FCVAR_CHEAT);
ConVar cl_viewbob_roll_max("cl_viewbob_roll_max", "0.03", FCVAR_CLIENTDLL | FCVAR_CHEAT);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C1187GameMovement::C1187GameMovement()
{

}

// Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
void C1187GameMovement::WalkMove(void)
{
	BaseClass::WalkMove();

	if (ShouldCalcViewbob())
		CalcViewbob();
}

// allow overridden versions to respond to jumping
void C1187GameMovement::OnJump(float fImpulse)
{
#if !defined ( CLIENT_DLL )
	C1187_Player* p1187Player = Get1187Player();
	if (p1187Player)
	{
		p1187Player->Event_OnJump(fImpulse);
	}
#endif
}

void C1187GameMovement::OnLand(float fVelocity)
{
#if !defined ( CLIENT_DLL )
	C1187_Player* p1187Player = Get1187Player();
	if (p1187Player)
	{
		p1187Player->Event_OnLand(fVelocity);
	}
#endif
}

bool C1187GameMovement::ShouldCalcViewbob(void) const
{
#if defined ( CLIENT_DLL )
	if (engine->IsPaused())
		return false;
#endif
	// Do not calculate if no player.
	if (!player)
		return false;

	// Do not calculate if the player is in the air.
	if (!(player->GetFlags() & FL_ONGROUND))
		return false;

	// Do not calculate if the player is not moving.
	if (player->GetAbsVelocity().Length2D() <= 0)
		return false;

	return cl_viewbob_enabled.GetBool();
}

void C1187GameMovement::CalcViewbob(void)
{
	float speed = player->GetAbsVelocity().Length2D();
	float maxspeed = mv->m_flClientMaxSpeed;

	Assert( maxspeed != 0.0f );

	float speedprop = speed / maxspeed;

	float pitch, yaw, roll;

	pitch = yaw = roll = 0.0f;

	QAngle angles;
	angles.Init();

	if ((mv->m_nButtons & IN_MOVELEFT) || (mv->m_nButtons & IN_MOVERIGHT))
	{
		roll = RemapValClamped(speedprop, 0, 1, cl_viewbob_roll_min.GetFloat(), cl_viewbob_roll_max.GetFloat());

		roll = (mv->m_nButtons & IN_MOVERIGHT) ? roll : -roll;
	}
	else if ((mv->m_nButtons & IN_FORWARD) || (mv->m_nButtons & IN_BACK))
	{
		float amplitude, frequency;

		amplitude = RemapValClamped(speedprop, 0, 1, cl_viewbob_amplitude_min.GetFloat(), cl_viewbob_amplitude_max.GetFloat());
		frequency = RemapValClamped(speedprop, 0, 1, cl_viewbob_frequency_min.GetFloat(), cl_viewbob_frequency_max.GetFloat());

		roll = sin(gpGlobals->curtime * frequency) * amplitude;
	}

	angles.x = pitch;
	angles.y = yaw;
	angles.z = roll;

	player->ViewPunch(angles);
}

#ifndef PORTAL	// Portal inherits from this but needs to declare it's own global interface
	// Expose our interface.
	static C1187GameMovement g_GameMovement;
	IGameMovement *g_pGameMovement = ( IGameMovement * )&g_GameMovement;

	EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameMovement, IGameMovement,INTERFACENAME_GAMEMOVEMENT, g_GameMovement );
#endif