//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for 1187.
//
// $NoKeywords: $
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_PLAYER_H
#define ELEVENEIGHTYSEVEN_PLAYER_H

#ifdef _WIN32
#pragma once
#endif

#include "hl2_player.h"
#include "1187_playerlocaldata.h"

class C1187_Player;
#include "1187_playeranimstate.h"

//=============================================================================
// >> 1187 PLAYER
//=============================================================================
class C1187_Player : public CHL2_Player
{
public:
	DECLARE_CLASS(C1187_Player, CHL2_Player);

	C1187_Player();
	~C1187_Player(void);
	
	static C1187_Player *CreatePlayer(const char *className, edict_t *ed)
	{
		C1187_Player::s_PlayerEdict = ed;
		return (C1187_Player*)CreateEntityByName(className);
	}

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	virtual void		Precache(void);
	virtual void		Spawn(void);

	void				PostThink(void);
	virtual void		SetAnimation(PLAYER_ANIM playerAnim);

	virtual void		CreateCorpse( void ) { CopyToBodyQue( this ); };

	QAngle GetAnimEyeAngles(void) { return m_angEyeAngles.Get(); }

	virtual void		CheatImpulseCommands(int iImpulse);
	virtual bool		ClientCommand(const CCommand &args);

	virtual void		OnJumping(float fImpulse);
	virtual void		OnLanding(float fVelocity);

protected:
	void				Event_OnJump(float fImpulse);
	void				Event_OnLand(float fVelocity);

private:

	CNetworkQAngle(m_angEyeAngles);
	C1187PlayerAnimState   m_PlayerAnimState;
	
	// This player's HL2 specific data that should only be replicated to 
	//  the player and not to other players.
	CNetworkVarEmbedded( C1187PlayerLocalData, m_1187Local );

	friend class C1187GameMovement;
};

inline C1187_Player *To1187Player(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsPlayer())
		return NULL;

	return dynamic_cast<C1187_Player*>(pEntity);
}


#endif	//ELEVENEIGHTYSEVEN_PLAYER_H
