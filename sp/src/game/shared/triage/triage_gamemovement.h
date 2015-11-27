//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Triage game movement.
//
//=============================================================================//

#ifndef TRIAGE_GAMEMOVEMENT_H
#define TRIAGE_GAMEMOVEMENT_H

#include "hl_gamemovement.h"

#if defined( CLIENT_DLL )

#include "c_triage_player.h"
#define CTriage_Player C_Triage_Player
#else

#include "triage_player.h"

#endif


//-----------------------------------------------------------------------------
// Purpose: Triage specific movement code
//-----------------------------------------------------------------------------
class CTriageGameMovement : public CHL2GameMovement
{
	typedef CHL2GameMovement BaseClass;
public:

	// Only used by players.  Moves along the ground when player is a MOVETYPE_WALK.
	virtual void	WalkMove(void);

protected:

	virtual bool	ShouldCalcViewbob(void) const;
	virtual void	CalcViewbob(void);

private:
	CTriage_Player	*GetTriagePlayer();
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline CTriage_Player	*CTriageGameMovement::GetTriagePlayer()
{
	return static_cast< CTriage_Player * >(player);
}

#endif // TRIAGE_GAMEMOVEMENT_H