//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for Hoe.
//
// $NoKeywords: $
//=============================================================================//

#ifndef HOE_PLAYER_H
#define HOE_PLAYER_H
#pragma once

#include "hl2_player.h"
#include "hoe_playerlocaldata.h"


//=============================================================================
// >> HOE_PLAYER
//=============================================================================
class CHoe_Player : public CHL2_Player
{
public:
	DECLARE_CLASS(CHoe_Player, CHL2_Player);

	CHoe_Player();
	~CHoe_Player(void);
	
	static CHoe_Player *CreatePlayer(const char *className, edict_t *ed)
	{
		CHoe_Player::s_PlayerEdict = ed;
		return (CHoe_Player*)CreateEntityByName(className);
	}

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void		CheatImpulseCommands(int iImpulse);
	virtual bool		ClientCommand(const CCommand &args);



private:
	// This player's Hoe specific data that should only be replicated to 
	//  the player and not to other players.
	CNetworkVarEmbedded( CHoePlayerLocalData, m_HoeLocal );

private:
	
	friend class CHoeGameMovement;
};

inline CHoe_Player *ToHoePlayer(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsPlayer())
		return NULL;

	return dynamic_cast<CHoe_Player*>(pEntity);
}

#endif	//HOE_PLAYER_H
