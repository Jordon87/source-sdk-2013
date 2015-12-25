//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Special handling for hl2 usable ladders
//
//=============================================================================//

#include "hl_gamemovement.h"

#if defined( CLIENT_DLL )

#include "c_hoe_player.h"
#define CHoe_Player C_Hoe_Player
#else

#include "hoe_player.h"

#endif

//-----------------------------------------------------------------------------
// Purpose: Hoe specific movement code
//-----------------------------------------------------------------------------
class CHoeGameMovement : public CHL2GameMovement
{
	typedef CHL2GameMovement BaseClass;
public:

	CHoeGameMovement();

	CHoe_Player	*GetHoePlayer();
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline CHoe_Player	*CHoeGameMovement::GetHoePlayer()
{
	return static_cast< CHoe_Player * >(player);
}