//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "particle_parse.h"
#include "te_effect_dispatch.h"
#include "npc_vortigaunt_episodic.h"

#include "1187_ai_vort_teleport_behavior.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define VORT_ENHANCED_WARMUP_DURATION				0.3f
#define VORT_ENHANCED_GRACE_PERIOD_DURATION			2.0f
#define VORT_ENHANCED_NEXT_TELEPORT_MIN				5.0f
#define VORT_ENHANCED_NEXT_TELEPORT_MAX				7.0f

ConVar sk_vortigaunt_enhanced_dmg_zap("sk_vortigaunt_enhanced_dmg_zap", "0");

class CNPC_Vortigaunt_Enhanced : public CNPC_Vortigaunt, public ITeleportHelper
{
	DECLARE_CLASS(CNPC_Vortigaunt_Enhanced, CNPC_Vortigaunt);
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	DEFINE_CUSTOM_AI;

	virtual void Precache(void);
	virtual void Spawn(void);
	virtual bool CreateBehaviors(void);

	virtual  Class_T Classify();

	virtual bool	IsPlayerAlly(void) { return false; }

	//
	// Teleport helper interface.
	//
	virtual void TeleportHelper_OnVisibilityChanged(bool bVisible);
	virtual void TeleportHelper_OnTeleport(void);

	virtual void TeleportHelper_OnWarmupComplete(void);
	virtual void TeleportHelper_OnWarmupStart(void);

protected:

	CAI_VortTeleportBehavior m_VortTeleportBehavior;
};

LINK_ENTITY_TO_CLASS(npc_vortigaunt_enhanced, CNPC_Vortigaunt_Enhanced);

IMPLEMENT_SERVERCLASS_ST(CNPC_Vortigaunt_Enhanced, DT_NPC_Vortigaunt_Enhanced)
END_SEND_TABLE()

BEGIN_DATADESC(CNPC_Vortigaunt_Enhanced)
END_DATADESC()


bool CNPC_Vortigaunt_Enhanced::CreateBehaviors(void)
{
	AddBehavior(&m_VortTeleportBehavior);

	return BaseClass::CreateBehaviors();
}

void CNPC_Vortigaunt_Enhanced::Precache(void)
{
	BaseClass::Precache();

	PrecacheParticleSystem("1187dec_teleport");
}

void CNPC_Vortigaunt_Enhanced::Spawn(void)
{
	// Allow multiple models (for slaves), but default to vortigaunt.mdl
	char *szModel = (char *)STRING(GetModelName());
	if (!szModel || !*szModel)
	{
		szModel = "models/vortigaunt_enchanced.mdl";
		SetModelName(AllocPooledString(szModel));
	}

	BaseClass::Spawn();

	m_VortTeleportBehavior.SetWarmupDuration( VORT_ENHANCED_WARMUP_DURATION );
	m_VortTeleportBehavior.SetNextTeleportTimeIntervals( VORT_ENHANCED_NEXT_TELEPORT_MIN, VORT_ENHANCED_NEXT_TELEPORT_MAX );
	m_VortTeleportBehavior.SetNextTeleportTime( 5.0f );
	m_VortTeleportBehavior.SetGracePeriodDuration( VORT_ENHANCED_GRACE_PERIOD_DURATION );
}

Class_T CNPC_Vortigaunt_Enhanced::Classify()
{
	return CLASS_VORTIGAUNT_ENHANCED;
}

//
// Teleport helper.
//
void CNPC_Vortigaunt_Enhanced::TeleportHelper_OnVisibilityChanged( bool bVisible )
{

}

void CNPC_Vortigaunt_Enhanced::TeleportHelper_OnTeleport(void)
{

}

void CNPC_Vortigaunt_Enhanced::TeleportHelper_OnWarmupComplete(void)
{

}

void CNPC_Vortigaunt_Enhanced::TeleportHelper_OnWarmupStart(void)
{
	QAngle angles = GetLocalAngles();
	angles[PITCH] = 0;

#if 0
	CEffectData	data;
	data.m_nHitBox = GetParticleSystemIndex("1187dec_teleport");
	data.m_nEntIndex = entindex();
	data.m_vOrigin = WorldSpaceCenter();
	data.m_vAngles = angles;
	data.m_fFlags |= (PARTICLE_DISPATCH_FROM_ENTITY | PARTICLE_DISPATCH_RESET_PARTICLES);
	data.m_nDamageType = PATTACH_CUSTOMORIGIN;
	data.m_nAttachmentIndex = -1;
	
	DispatchEffect("ParticleEffect", data);
#endif

	DispatchParticleEffect("1187dec_teleport", WorldSpaceCenter(), angles, NULL);
}

AI_BEGIN_CUSTOM_NPC(npc_vortigaunt_enhanced, CNPC_Vortigaunt_Enhanced)
AI_END_CUSTOM_NPC()

//------------------------------------------------------------------------------
void CC_VortEnhancedTeleport(const CCommand& args)
{
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

	if (!pPlayer)
		return;

	Vector vForward;
	AngleVectors(pPlayer->LocalEyeAngles(), &vForward);

	trace_t tr;
	UTIL_TraceLine(pPlayer->EyePosition(), pPlayer->EyePosition() + vForward * MAX_TRACE_LENGTH,MASK_SOLID, pPlayer, COLLISION_GROUP_NPC, &tr);

	if (tr.m_pEnt && tr.m_pEnt->IsNPC() && FClassnameIs(tr.m_pEnt, "npc_vortigaunt_enhanced"))
	{
		// ((CNPC_Vortigaunt_Enhanced*)tr.m_pEnt)->Debug_DoTeleportSchedule();
	}
}

static ConCommand g_vortigaunt_enhanced_teleport("g_vortigaunt_enhanced_teleport", CC_VortEnhancedTeleport, "Summon teleport schedule when facing at a npc_vortigaunt_enhanced.\n", FCVAR_CHEAT);