//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_NPC_BASEZOMBIE_HEADLESS_RUNNER_H
#define ELEVENEIGHTYSEVEN_NPC_BASEZOMBIE_HEADLESS_RUNNER_H

#include "ai_basenpc.h"
#include "ai_blended_movement.h"
#include "player_pickup.h"
#include "1187_npc_basezombie_headless.h"

class C1187_NPC_BaseZombie_Headless_Runner : public CAI_BlendingHost<C1187_NPC_BaseZombie_Headless>, public CDefaultPlayerPickupVPhysics
{
	DECLARE_DATADESC();
	DECLARE_CLASS(C1187_NPC_BaseZombie_Headless_Runner, CAI_BlendingHost<C1187_NPC_BaseZombie_Headless>);

public:

	void Spawn(void);
	void Precache(void);

	void SetZombieModel(void);

	virtual void PrescheduleThink(void);

	virtual const char *GetLegsModel(void);
	virtual const char *GetTorsoModel(void);
	virtual const char *GetHeadcrabClassname(void);
	virtual const char *GetHeadcrabModel(void);

	virtual void PainSound(const CTakeDamageInfo &info);
	virtual void DeathSound(const CTakeDamageInfo &info);
	virtual void AlertSound(void);
	virtual void IdleSound(void);
	virtual void AttackSound(void);
	virtual void AttackHitSound(void);
	virtual void AttackMissSound(void);
	virtual void FootstepSound(bool fRightFoot);
	virtual void FootscuffSound(bool fRightFoot);
	virtual void MoanSound(envelopePoint_t *pEnvelope, int iEnvelopeSize);

	virtual void RunTask(const Task_t *pTask);

	virtual bool ShouldBecomeTorso(const CTakeDamageInfo &info, float flDamageThreshold);

	virtual void OnScheduleChange(void);

	virtual Activity NPC_TranslateActivity(Activity baseAct);

	const char *GetMoanSound(int nSound);

	bool AllowedToSprint(void);
	void Sprint(bool bMadSprint = false);
	void StopSprint(void);

	void DropGrenade(Vector vDir);

	bool IsSprinting(void) { return m_flSprintTime > gpGlobals->curtime; }

	void InputStartSprint(inputdata_t &inputdata);

public:
	DEFINE_CUSTOM_AI;

private:

	float	m_flSprintTime;
	float	m_flSprintRestTime;

	float	m_flSuperFastAttackTime;

protected:
	static const char *pMoanSounds[];

};

#endif // ELEVENEIGHTYSEVEN_NPC_BASEZOMBIE_HEADLESS_RUNNER_H