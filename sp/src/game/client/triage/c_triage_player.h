//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $NoKeywords: $
//=============================================================================//

#if !defined( C_TRIAGE_PLAYER_H )
#define C_TRIAGE_PLAYER_H
#ifdef _WIN32
#pragma once
#endif


#include "c_basehlplayer.h"
#include "c_triage_playerlocaldata.h"

class C_Triage_Player : public C_BaseHLPlayer
{
public:
	DECLARE_CLASS(C_Triage_Player, C_BaseHLPlayer);
	DECLARE_CLIENTCLASS();
	DECLARE_PREDICTABLE();

public:
	C_Triage_Player();

	C_TriagePlayerLocalData		m_TriageLocal;

private:
	C_Triage_Player(const C_Triage_Player &); // not defined, not accessible


friend class CTriageGameMovement;
};


inline C_Triage_Player *ToTriagePlayer(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsPlayer())
		return NULL;

	return dynamic_cast<C_Triage_Player*>(pEntity);
}

#endif
