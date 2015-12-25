//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//

#if !defined( C_HOE_PLAYER_H )
#define C_HOE_PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "c_basehlplayer.h"
#include "c_hoe_playerlocaldata.h"

class C_Hoe_Player : public C_BaseHLPlayer
{
public:
	DECLARE_CLASS(C_Hoe_Player, C_BaseHLPlayer);
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

						C_Hoe_Player();

public:

	C_HoePlayerLocalData		m_HoeLocal;

private:
	C_Hoe_Player(const C_Hoe_Player &); // not defined, not accessible
	


friend class CHoeGameMovement;
};

inline C_Hoe_Player *ToHoePlayer(C_BaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsPlayer())
		return NULL;

	return dynamic_cast<C_Hoe_Player*>(pEntity);
}


#endif // C_HOE_PLAYER_H
