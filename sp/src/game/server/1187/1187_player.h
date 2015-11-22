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

	void				PreThink(void);
	void				PostThink(void);
	virtual void		SetAnimation(PLAYER_ANIM playerAnim);

	virtual void		CreateCorpse( void ) { CopyToBodyQue( this ); };

#if 0
	QAngle GetAnimEyeAngles(void) { return m_angEyeAngles.Get(); }
#endif

	virtual void		CheatImpulseCommands(int iImpulse);
	virtual bool		ClientCommand(const CCommand &args);
	virtual bool		Weapon_CanUse(CBaseCombatWeapon *pWeapon);

	virtual void		OnJumping(float fImpulse);
	virtual void		OnLanding(float fVelocity);
	virtual void		OnHaulBack(void);
	virtual void		OnMelee(void);

	virtual void		EquipSuit(bool bPlayEffects = true);

	void				StartSprinting(void);
	void				StopSprinting(void);

	// Interface for lowering weapon on sprint.
	bool				WeaponLoweredOnSprint_IsActive() const { return m_1187Local.m_bWeaponLoweredOnSprint; };
	void				WeaponLoweredOnSprint_SetState(bool state);
	void				WeaponLoweredOnSprint_Start();
	void				WeaponLoweredOnSprint_Stop();
	void				WeaponLoweredOnSprint_Update();


	// Wall proximity
	bool				WallProximity_ShouldCheck(void) const;
	void				WallProximity_Update(void);
	bool				WallProximity_Check(void);
	bool				WallProximity_IsAdjacentToWall(void) const { return m_bAdjacentToWall; }

	// Haul back (throwing grenades)
	bool				CanHaulBack(void) const;
	bool				IsHaulBackActive(void) const;
	void				HandleHaulBackAnimation(void);
	void				StartHaulBackAnimation(void);
	void				CheckHaulBackInput(void);

	// Player melee attack.
	bool				CanMelee(void) const;
	bool				IsInMelee(void) const;
	void				HandleMeleeAnimation(void);
	void				StartMeleeAnimation(void);
	void				CheckMeleeInput(void);

protected:
	void				Event_OnJump(float fImpulse);
	void				Event_OnLand(float fVelocity);
	void				Event_HaulBack(void);
	void				Event_Melee(void);

private:

	CNetworkVar(bool, m_bAdjacentToWall);
	
#if 0
	CNetworkQAngle(m_angEyeAngles);
	C1187PlayerAnimState   m_PlayerAnimState;
#endif

	// Haul back members
	float				m_flHaulBackAnimTime;
	float				m_flHaulBackAnimEventTime;
	bool				m_bIsHaulBackActive;
	bool				m_bHaulBackEventHandled;

	// Melee attack members.
	float				m_flMeleeAnimTime;
	float				m_flMeleeAnimEventTime;
	bool				m_bIsInMelee;
	bool				m_bMeleeEventHandled;

	
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
