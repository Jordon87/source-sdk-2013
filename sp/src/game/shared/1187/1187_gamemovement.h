//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Special handling for hl2 usable ladders
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_GAMEMOVEMENT_H
#define ELEVENEIGHTYSEVEN_GAMEMOVEMENT_H

#include "hl_gamemovement.h"

#if defined( CLIENT_DLL )

#include "c_1187_player.h"
#define C1187_Player C_1187_Player
#else

#include "1187_player.h"

#endif

//-----------------------------------------------------------------------------
// Purpose: 1187 specific movement code
//-----------------------------------------------------------------------------
class C1187GameMovement : public CHL2GameMovement
{
	typedef CHL2GameMovement BaseClass;
public:

	C1187GameMovement();

	// Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
	virtual void	WalkMove(void);

	// allow overridden versions to respond to jumping
	virtual void	OnJump(float fImpulse);
	virtual void	OnLand(float fVelocity);

protected:

	virtual bool	ShouldCalcViewbob(void) const;
	virtual void	CalcViewbob( void );

private:

	C1187_Player *Get1187Player();
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline C1187_Player	*C1187GameMovement::Get1187Player()
{
	return static_cast< C1187_Player * >(player);
}

#endif // ELEVENEIGHTYSEVEN_GAMEMOVEMENT_H