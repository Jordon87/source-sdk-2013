//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Player for Triage.
//
// $NoKeywords: $
//=============================================================================//

#ifndef TRIAGE_PLAYER_H
#define TRIAGE_PLAYER_H
#pragma once


#include "triage_playerlocaldata.h"
#include "hl2_player.h"

//=============================================================================
// >> TRIAGE PLAYER
//=============================================================================
class CTriage_Player : public CHL2_Player
{
public:
	DECLARE_CLASS(CTriage_Player, CHL2_Player);
	
	static CTriage_Player *CreatePlayer(const char *className, edict_t *ed)
	{
		CTriage_Player::s_PlayerEdict = ed;
		return (CTriage_Player*)CreateEntityByName(className);
	}

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CTriage_Player();

	virtual void		CreateCorpse( void ) { CopyToBodyQue( this ); };

	virtual bool		ClientCommand(const CCommand &args);

	virtual int			OnTakeDamage(const CTakeDamageInfo &info);

	virtual bool		BumpWeapon(CBaseCombatWeapon *pWeapon);

	// Apply a battery
	virtual bool		ApplyBattery(float powerMultiplier = 1.0);

	// -----------------------
	// Weapons
	// -----------------------
	virtual bool		Weapon_SlotOccupied(CBaseCombatWeapon *pWeapon);

	// Sprint Device
	virtual void		StartSprinting(void);
	virtual void		StopSprinting(void);

protected:
	virtual void		PreThink(void);
	virtual	void		PostThink(void);

private:

	// This player's Triage specific data that should only be replicated to 
	//  the player and not to other players.
	CNetworkVarEmbedded( CTriagePlayerLocalData, m_TriageLocal );
	
	// Regenerating health
	float     m_fRegenRemander;
	float	  m_fTimeLastHurt;
	float	  m_fTimeNextRegen;
	bool	  m_bFadeDamageRemoved;

	friend class CTriageGameMovement;
};

inline CTriage_Player *ToTriagePlayer(CBaseEntity *pEntity)
{
	if (!pEntity || !pEntity->IsPlayer())
		return NULL;

	return dynamic_cast<CTriage_Player*>(pEntity);
}

#endif	//TRIAGE_PLAYER_H
