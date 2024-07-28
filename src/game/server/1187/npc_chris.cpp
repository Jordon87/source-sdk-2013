//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_playercompanion.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SF_UNK_CHRIS_FLAG	(1<<16)

#define CHRIS_MODEL		"models/humans/group03/male_02.mdl"

ConVar g_chrishealth("g_chrishealth", "9000", FCVAR_NONE);

class CNPC_Chris : public CNPC_PlayerCompanion
{
	DECLARE_CLASS(CNPC_Chris, CNPC_PlayerCompanion);
	DEFINE_CUSTOM_AI;
public:

	virtual void Precache(void);
	virtual void Spawn(void);
	Class_T Classify(void);
	void TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator);
	void PredictPlayerPush();
	void PrescheduleThink();
	int  OnTakeDamage_Alive(const CTakeDamageInfo& info);
	void DeathSound(const CTakeDamageInfo& info);

	virtual void SelectModel();
};

LINK_ENTITY_TO_CLASS(npc_chris, CNPC_Chris);

AI_BEGIN_CUSTOM_NPC(npc_chris, CNPC_Chris)

AI_END_CUSTOM_NPC()

void CNPC_Chris::SelectModel()
{
	char *szModel = (char *)STRING(GetModelName());
	if (!szModel || !*szModel)
	{
		SetModelName(AllocPooledString(CHRIS_MODEL));
	}
}

void CNPC_Chris::Precache(void)
{
	BaseClass::Precache();
	PrecacheModel(STRING(GetModelName()));
}

void CNPC_Chris::Spawn()
{
	if (HasSpawnFlags(SF_UNK_CHRIS_FLAG))
	{
		UTIL_Remove(this);
	}
	
	KeyValue("additionalequipment", "weapon_m4");

	BaseClass::Spawn();
	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_STANDABLE);
	SetMoveType(MOVETYPE_STEP);

	CapabilitiesAdd(bits_CAP_MOVE_GROUND | bits_CAP_DOORS_GROUP | bits_CAP_TURN_HEAD | bits_CAP_DUCK | bits_CAP_SQUAD);
	CapabilitiesAdd(bits_CAP_USE_WEAPONS);
	CapabilitiesAdd(bits_CAP_ANIMATEDFACE);
	CapabilitiesAdd(bits_CAP_FRIENDLY_DMG_IMMUNE);
	CapabilitiesAdd(bits_CAP_AIM_GUN);
	CapabilitiesAdd(bits_CAP_MOVE_SHOOT);
	CapabilitiesRemove(bits_CAP_USE_SHOT_REGULATOR);

	m_FollowBehavior.SetFollowTarget(UTIL_GetLocalPlayer());
	m_FollowBehavior.SetParameters(AIF_SIDEKICK);

	m_iHealth = g_chrishealth.GetInt();
	m_iMaxHealth = m_iHealth;

	NPCInit();
}

Class_T CNPC_Chris::Classify(void)
{
	return CLASS_PLAYER_ALLY_VITAL;
}

void CNPC_Chris::TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator)
{
	BaseClass::TraceAttack(info, vecDir, ptr, pAccumulator);

	// FIXME: hack until some way of removing decals after healing
	m_fNoDamageDecal = true;
}

void CNPC_Chris::PredictPlayerPush()
{
	if (AI_IsSinglePlayer())
		BaseClass::PredictPlayerPush();
}

void CNPC_Chris::PrescheduleThink()
{
	BaseClass::PrescheduleThink();

	if (HasCondition(COND_TALKER_PLAYER_DEAD))
	{
		SpeakIfAllowed(TLK_PLDEAD);
	}
}

int CNPC_Chris::OnTakeDamage_Alive(const CTakeDamageInfo& info)
{
	CTakeDamageInfo dmgInfo(info);
	return BaseClass::OnTakeDamage_Alive(dmgInfo);
}

void CNPC_Chris::DeathSound(const CTakeDamageInfo& info)
{
	SentenceStop();
}
