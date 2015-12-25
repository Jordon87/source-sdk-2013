//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef HOE_NPC_HOUNDEYE_H
#define HOE_NPC_HOUNDEYE_H

#ifdef _WIN32
#pragma once
#endif

#include	"ai_basenpc.h"

#if 0
#include	"energy_wave.h"
#endif

class CHoe_NPC_Houndeye : public CAI_BaseNPC
{
	DECLARE_CLASS(CHoe_NPC_Houndeye, CAI_BaseNPC);

public:
	void			Spawn(void);
	void			Precache(void);
	Class_T			Classify(void);
	void			HandleAnimEvent(animevent_t *pEvent);
	float			MaxYawSpeed(void);
	void			WarmUpSound(void);
	void			AlertSound(void);
	void			DeathSound(const CTakeDamageInfo &info);
	void			WarnSound(void);
	void			PainSound(const CTakeDamageInfo &info);
	void			IdleSound(void);
	void			StartTask(const Task_t *pTask);
	void			RunTask(const Task_t *pTask);
	int				GetSoundInterests(void);
	void			SonicAttack(void);
	void			PrescheduleThink(void);
	void			WriteBeamColor(void);
	int				RangeAttack1Conditions(float flDot, float flDist);
	bool			FCanActiveIdle(void);
	virtual int		TranslateSchedule(int scheduleType);
	Activity		NPC_TranslateActivity(Activity eNewActivity);
	virtual int		SelectSchedule(void);
	bool			HandleInteraction(int interactionType, void *data, CBaseCombatCharacter* sourceEnt);
	void			NPCThink(void);
	int				OnTakeDamage_Alive(const CTakeDamageInfo &info);
	void			Event_Killed(const CTakeDamageInfo &info);
	bool			IsAnyoneInSquadAttacking(void);
	void			SpeakSentence(int sentenceType);

	float			m_flNextSecondaryAttack;
	bool			m_bLoopClockwise;

#if 0
	CEnergyWave*	m_pEnergyWave;
#endif
	float			m_flEndEnergyWaveTime;

	bool			m_fAsleep;// some houndeyes sleep in idle mode if this is set, the houndeye is lying down
	bool			m_fDontBlink;// don't try to open/close eye if this bit is set!

	DEFINE_CUSTOM_AI;

	DECLARE_DATADESC();
};


#endif // HOE_NPC_HOUNDEYE_H
