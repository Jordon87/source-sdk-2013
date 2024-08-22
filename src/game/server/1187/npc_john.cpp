//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npc_playercompanion.h"
#include "sceneentity.h"
#include "ai_memory.h"
#include "ai_squadslot.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SF_UNK_JOHN_FLAG	(1<<16)

#define JOHH_MODEL		"models/humans/group01/male_04.mdl"

ConVar	ahhthanksman("ahhthanksman", "0");
ConVar	g_johnhealth("g_johnhealth", "9000");
ConVar	g_johnfallhealth("g_johnfallhealth", "8900");
ConVar	g_johnmeleedamage("g_johnmeleedamage", "60");
ConVar	g_johnspeaklimit("g_johnspeaklimit", "2.5");
extern ConVar skill;

enum JohnScenes_t
{
	JOHN_TOUCH = 0x0,
	JOHN_DAMAGE_HIT = 0x1,
	JOHN_DAMAGE_SHOT = 0x2,
	JOHN_KILLED_ZOMBIE = 0x3,
	JOHN_KILLED_ARMOURED_ZOMBIE = 0x4,
	JOHN_KILLED_BURST_ZOMBIE = 0x5,
	JOHN_KILLED_POISON_ZOMBIE = 0x6,
	JOHN_KILLED_HEADCRAB = 0x7,
	JOHN_KILLED_MINI_HEADCRAB = 0x8,
	JOHN_KILLED_VORTIGAUNT = 0x9,
	JOHN_KILLED_MARINE = 0xa,
	JOHN_KILLED_PLAYER = 0xb,
	JOHN_SPOT_ZOMBIES = 0xc,
	JOHN_SPOT_CREATURES = 0xd,
	JOHN_SPOT_VORTIGAUNT = 0xe,
	JOHN_SPOT_MARINE = 0xf,
	JOHN_SEEPLAYER_KILL = 0x10,
	JOHN_SEEPLAYER_HEADSHOT = 0x11,
	JOHN_SEEPLAYER_ONESHOT = 0x12,
	JOHN_RELOAD = 0x13,
	JOHN_MELEE = 0x14,
};

const char* g_JohnScenesList[] =
{
	"scenes/johndown_help_hard1.vcd",
	"scenes/johndown_help_hard2.vcd",
	"scenes/johndown_help_medium1.vcd",
	"scenes/johndown_help_medium2.vcd",
	"scenes/johndown_help_easy1.vcd",
	"scenes/johndown_help_easy2.vcd",
	"scenes/john_melee01.vcd",
	"scenes/john_melee01.vcd",
	"scenes/johndown_thanks1.vcd",
	"scenes/johndown_thanks2.vcd",
	"scenes/JohnTouch/1187_john_touch1.vcd",
	"scenes/JohnTouch/1187_john_touch2.vcd",
	"scenes/JohnTouch/1187_john_touch3.vcd",
	"scenes/JohnTouch/1187_john_touch4.vcd",
	"scenes/JohnTouch/1187_john_touch5.vcd",
	"scenes/JohnHit/john_damage_hit1.vcd",
	"scenes/JohnHit/john_damage_hit2.vcd",
	"scenes/JohnHit/john_damage_hit3.vcd",
	"scenes/JohnHit/john_damage_hit4.vcd",
	"scenes/JohnShot/john_damage_shot1.vcd",
	"scenes/JohnShot/john_damage_shot2.vcd",
	"scenes/JohnShot/john_damage_shot3.vcd",
	"scenes/JohnShot/john_damage_shot4.vcd",
	"scenes/JohnKilledZombie/john_killed_zombie1.vcd",
	"scenes/JohnKilledZombie/john_killed_zombie2.vcd",
	"scenes/JohnKilledZombie/john_killed_zombie3.vcd",
	"scenes/JohnKillerArmoured/john_killed_armouredzombie1.vcd",
	"scenes/JohnKillerArmoured/john_killed_armouredzombie2.vcd",
	"scenes/JohnKilledBurster/john_killed_bursterzombie1.vcd",
	"scenes/JohnKilledBurster/john_killed_bursterzombie2.vcd",
	"scenes/JohnKilledPoison/john_killed_poisonzombie1.vcd",
	"scenes/JohnKilledHeadcrab/john_killed_headcrab1.vcd",
	"scenes/JohnKilledHeadcrab/john_killed_headcrab2.vcd",
	"scenes/JohnKilledMini/john_killed_miniheadcrab1.vcd",
	"scenes/JohnKilledMini/john_killed_miniheadcrab2.vcd",
	"scenes/JohnKilledMini/john_killed_miniheadcrab2.vcd",
	"scenes/JohnKilledVort/john_killed_vort1.vcd",
	"scenes/JohnKilledMarine/john_killed_marine1.vcd",
	"scenes/JohnKilledMarine/john_killed_marine2.vcd",
	"scenes/JohnKilledPlayer/john_killed_player1.vcd",
	"scenes/JohnSpotCreatures/john_spot_creatures1.vcd",
	"scenes/JohnSpotCreatures/john_spot_creatures2.vcd",
	"scenes/JohnSpotZombies/john_spot_zombies1.vcd",
	"scenes/JohnSpotZombies/john_spot_zombies2.vcd",
	"scenes/JohnSpotVort/john_spot_vort1.vcd",
	"scenes/JohnSpotVort/john_spot_vort2.vcd",
	"scenes/JohnSpotMarine/john_spot_marine1.vcd",
	"scenes/JohnSpotMarine/john_spot_marine2.vcd",
	"scenes/JohnSpotMarine/john_spot_marine3.vcd",
	"scenes/JohnSeeKill/john_seeplayer_kill1.vcd",
	"scenes/JohnSeeKill/john_seeplayer_kill2.vcd",
	"scenes/JohnSeeKill/john_seeplayer_kill3.vcd",
	"scenes/JohnSeeHeadshot/john_seeplayer_headshot1.vcd",
	"scenes/JohnSeeHeadshot/john_seeplayer_headshot2.vcd",
	"scenes/JohnSeeOneshot/john_seeplayer_oneshot1.vcd",
	"scenes/JohnReload/john_reload1.vcd",
	"scenes/JohnReload/john_reload2.vcd",
	"scenes/JohnMelee/john_melee1.vcd",
	"scenes/JohnMelee/john_melee2.vcd",
	"scenes/JohnMelee/john_melee3.vcd"
};

class CNPC_John : public CNPC_PlayerCompanion
{
	DECLARE_CLASS(CNPC_John, CNPC_PlayerCompanion);
	DECLARE_DATADESC();
	DEFINE_CUSTOM_AI;

public:

	void SelectModel(void);

	void Precache(void);
	void Spawn(void);
	Class_T	Classify(void);
	void TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator);
	bool IgnorePlayerPushing(void);
	void StartTask(const Task_t* pTask);
	bool IsValidEnemy(CBaseEntity* pEnemy);
	void PredictPlayerPush();
	void PrescheduleThink() { return BaseClass::PrescheduleThink(); }
	int TranslateSchedule(int scheduleType);
	void Think(void);
	void Event_KilledOther(CBaseEntity* pVictim, const CTakeDamageInfo& info);
	int  OnTakeDamage_Alive(const CTakeDamageInfo& info);
	void Touch(CBaseEntity* pOther);
	void DeathSound(const CTakeDamageInfo& info);
	void OnPlayerKilledOther(CBaseEntity* pVictim, const CTakeDamageInfo& info);

	virtual void CallingForHelpScene(char *sceneName);
	virtual bool IsDownDisabled(void);
	virtual const char *CallingForHelp(void);

private:
	void MeleeAttack();
	int SelectCombatSchedule();
	void PlayAction(JohnScenes_t actionName, bool a3);
	
	bool b_IsDown;
	bool b_CanIdle;
	bool b_WaitingToDie;
	bool b_ProtectionOn;
	bool b_CanMelee;
	bool unk_0x1415;

	const char* m_szCallingForHelp;

	float m_flMeleeDelayTimer;
	float m_flProtectionTimer;
	float m_flTimerToDie;
	float m_flBleed;
	float m_flDelayIdle;
	float unk_0x1430;
	float m_flSpeakAgain;

	CBaseEntity *unk_0x143c; //???

};

LINK_ENTITY_TO_CLASS(npc_john, CNPC_John);

BEGIN_DATADESC(CNPC_John)

	DEFINE_FIELD(b_IsDown, FIELD_BOOLEAN),
	DEFINE_FIELD(b_CanIdle, FIELD_BOOLEAN),
	DEFINE_FIELD(b_WaitingToDie, FIELD_BOOLEAN),
	DEFINE_FIELD(b_ProtectionOn, FIELD_BOOLEAN),
	DEFINE_FIELD(b_CanMelee, FIELD_BOOLEAN),
	DEFINE_FIELD(m_flMeleeDelayTimer, FIELD_TIME),
	DEFINE_FIELD(m_flProtectionTimer, FIELD_TIME),
	DEFINE_FIELD(m_flTimerToDie, FIELD_TIME),
	DEFINE_FIELD(m_flBleed, FIELD_TIME),
	DEFINE_FIELD(m_flDelayIdle, FIELD_TIME),
	DEFINE_FIELD(m_flSpeakAgain, FIELD_TIME),

END_DATADESC()

AI_BEGIN_CUSTOM_NPC(npc_john, CNPC_John)

AI_END_CUSTOM_NPC()

void CNPC_John::SelectModel()
{
	char *szModel = (char *)STRING(GetModelName());
	if (!szModel || !*szModel)
	{
		SetModelName(AllocPooledString(JOHH_MODEL));
	}
}

void CNPC_John::Precache(void)
{
	BaseClass::Precache();

	PrecacheScriptSound("npc_john.die");
	PrecacheScriptSound("NPC_Combine.WeaponBash");

	PrecacheModel(STRING(GetModelName()));

	int iJohnSceneNames = ARRAYSIZE(g_JohnScenesList);
	int i;

	for ( i = 0; i < iJohnSceneNames; ++i )
	{
		PrecacheInstancedScene(g_JohnScenesList[i]);
	}
}

void CNPC_John::Spawn()
{
	if (HasSpawnFlags(SF_UNK_JOHN_FLAG))
	{
		UTIL_Remove(this);
	}

	BaseClass::Spawn();
	SetSolid(SOLID_BBOX);
	AddSolidFlags(FSOLID_NOT_STANDABLE);
	SetMoveType(MOVETYPE_STEP);

	CapabilitiesAdd(bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP | bits_CAP_MOVE_GROUND);
	CapabilitiesAdd(bits_CAP_USE_WEAPONS);
	CapabilitiesAdd(bits_CAP_ANIMATEDFACE);
	CapabilitiesAdd(bits_CAP_FRIENDLY_DMG_IMMUNE);
	CapabilitiesAdd(bits_CAP_AIM_GUN);
	CapabilitiesAdd(bits_CAP_MOVE_SHOOT);
	CapabilitiesRemove(bits_CAP_NO_HIT_PLAYER | bits_CAP_NO_HIT_SQUADMATES | bits_CAP_USE_SHOT_REGULATOR);

	m_flMeleeDelayTimer = 0.0f;
	b_IsDown = false;
	m_flProtectionTimer = 0.0f;
	b_CanIdle = false;
	m_flTimerToDie = 0.0f;
	b_WaitingToDie = false;
	m_flBleed = 0.0f;
	b_ProtectionOn = false;
	m_flDelayIdle = 0.0f;
	b_CanMelee = false;
	unk_0x1430 = 0.0f;
	unk_0x1415 = false;
	m_szCallingForHelp = "scenes/johndown_help_easy1.vcd";
	m_flSpeakAgain = gpGlobals->curtime + 1.0f;

	m_FollowBehavior.SetFollowTarget(UTIL_GetLocalPlayer());
	m_FollowBehavior.SetParameters(AIF_SIDEKICK);

	AddEFlags(EFL_NO_DISSOLVE | EFL_NO_MEGAPHYSCANNON_RAGDOLL | EFL_NO_PHYSCANNON_INTERACTION);

	m_iHealth = g_johnhealth.GetInt();
	m_iMaxHealth = 9000;

	m_flFieldOfView = -1.0f;

	NPCInit();
}

Class_T CNPC_John::Classify(void)
{
	if (b_IsDown)
	{
		return (CLASS_NONE);
	}
	else
	{
		return (CLASS_PLAYER_ALLY_VITAL);
	}
}

void CNPC_John::TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator)
{
	BaseClass::TraceAttack(info, vecDir, ptr, pAccumulator);

	// FIXME: hack until some way of removing decals after healing
	m_fNoDamageDecal = true;
}

bool CNPC_John::IgnorePlayerPushing(void)
{
	return b_IsDown != false;
}

void CNPC_John::StartTask(const Task_t* pTask)
{
	switch ( pTask->iTask )
	{
	case TASK_MOVE_AWAY_PATH:
		{
			if ( HasCondition( COND_PLAYER_PUSHING ) && AI_IsSinglePlayer() && !IgnorePlayerPushing() )
			{
				GetMotor()->SetIdealYawToTarget( UTIL_GetLocalPlayer()->WorldSpaceCenter() );
			}
			BaseClass::StartTask( pTask );
			break;
		}

	default:
		BaseClass::StartTask( pTask );
	}
}

bool CNPC_John::IsValidEnemy(CBaseEntity* pEnemy)
{
	if (gpGlobals->curtime - GetEnemies()->FirstTimeSeen(pEnemy) < 15.0f)
	{
		if (FClassnameIs(pEnemy, "npc_zombie") || FClassnameIs(pEnemy, "npc_armouredzombie") || FClassnameIs(pEnemy, "npc_burstzombie") || FClassnameIs(pEnemy, "npc_poisonzombie"))
		{
			PlayAction(JOHN_SPOT_ZOMBIES, true);
		}
		else if (FClassnameIs(pEnemy, "npc_headcrab") || FClassnameIs(pEnemy, "npc_headcrabfast") || FClassnameIs(pEnemy, "npc_headcrabblack") || FClassnameIs(pEnemy, "npc_burstheadcrab"))
		{
			PlayAction(JOHN_SPOT_CREATURES, true);
		}
		else if (FClassnameIs(pEnemy, "npc_vortigaunt"))
		{
			PlayAction(JOHN_SPOT_VORTIGAUNT, true);
		}
		else if (FClassnameIs(pEnemy, "npc_citizen"))
		{
			PlayAction(JOHN_SPOT_MARINE, true);
		}
	}
	return BaseClass::IsValidEnemy(pEnemy);
}

void CNPC_John::PredictPlayerPush()
{
	if ( AI_IsSinglePlayer() && !b_IsDown )
		BaseClass::PredictPlayerPush();
}

void CNPC_John::MeleeAttack()
{
	if (unk_0x143c)
	{
		Vector vecDir, vecUp;
		AngleVectors(GetAbsAngles(), &vecDir, NULL, &vecUp);

		CTakeDamageInfo dmgInfo(this, this, g_johnmeleedamage.GetFloat(), DMG_CLUB);
		CalculateExplosiveDamageForce(&dmgInfo, vecDir, unk_0x143c->GetAbsOrigin());
		unk_0x143c->TakeDamage(dmgInfo);
		EmitSound("NPC_Combine.WeaponBash");
		PlayAction(JOHN_MELEE, true);
	}

	SetCondition(COND_NPC_UNFREEZE);
	SetMoveType(MOVETYPE_STEP);

	b_CanIdle = true;
	m_flDelayIdle = GetSceneDuration(GetExpression()) + gpGlobals->curtime;

	m_FollowBehavior.SetFollowTarget(UTIL_GetLocalPlayer());
	m_FollowBehavior.SetParameters(AIF_SIDEKICK);

	b_CanMelee = false;
}

int CNPC_John::SelectCombatSchedule()
{
	if ( HasCondition( COND_ENEMY_DEAD ) )
	{
		return BaseClass::SelectCombatSchedule();
	}

	if ( HasCondition( COND_NEW_ENEMY ) && ( GetEnemy() ) && HasCondition( COND_SEE_ENEMY ) )
	{
		if (FClassnameIs(GetEnemy(), "npc_zombie") || FClassnameIs(GetEnemy(), "npc_armouredzombie") || FClassnameIs(GetEnemy(), "npc_burstzombie") || FClassnameIs(GetEnemy(), "npc_poisonzombie"))
		{
			PlayAction(JOHN_SPOT_ZOMBIES, true);
		}
		else
		{
			if (FClassnameIs(GetEnemy(), "npc_headcrab") || FClassnameIs(GetEnemy(), "npc_headcrabfast") || FClassnameIs(GetEnemy(), "npc_headcrabblack") || FClassnameIs(GetEnemy(), "npc_burstheadcrab"))
			{
				PlayAction(JOHN_SPOT_CREATURES, true);
				return BaseClass::SelectCombatSchedule();
			}
			if (FClassnameIs(GetEnemy(), "npc_vortigaunt"))
			{
				PlayAction(JOHN_SPOT_VORTIGAUNT, true);
				return BaseClass::SelectCombatSchedule();
			}
			if (FClassnameIs(GetEnemy(), "npc_citizen"))
			{
				PlayAction(JOHN_SPOT_MARINE, true);
				return BaseClass::SelectCombatSchedule();
			}
		}
	}
	return BaseClass::SelectCombatSchedule();
}

int CNPC_John::TranslateSchedule(int scheduleType)
{
	switch( scheduleType )
	{
	case SCHED_IDLE_STAND:
	case SCHED_ALERT_STAND:
		if( GetActiveWeapon() )
		{
			CBaseCombatWeapon *pWeapon = GetActiveWeapon();

			if( CanReload() && pWeapon->UsesClipsForAmmo1() && pWeapon->Clip1() < ( pWeapon->GetMaxClip1() * .5 ) && OccupyStrategySlot( SQUAD_SLOT_EXCLUSIVE_RELOAD ) )
			{
				PlayAction(JOHN_RELOAD, false);
				return SCHED_RELOAD;
			}
		}
		break;
	}
	
	return BaseClass::TranslateSchedule( scheduleType );
}

void CNPC_John::Think(void)
{
	if (b_WaitingToDie && gpGlobals->curtime > m_flTimerToDie)
	{
		CTakeDamageInfo info(this, this, GetMaxHealth(), DMG_CRUSH);
		TakeDamage(info);
		m_flTimerToDie = 0.0f;
		b_WaitingToDie = false;
	}

	if (b_IsDown && gpGlobals->curtime > m_flBleed)
	{
		if (strcmp(GetExpression(), CallingForHelp()))
		{
			SetExpression(CallingForHelp());
		}
	
		m_flBleed = gpGlobals->curtime + 1.0f;
	
		trace_t tr;
		UTIL_TraceLine(GetAbsOrigin() + Vector(0, 0, 8), GetAbsOrigin() - Vector(0, 0, 24), MASK_SOLID_BRUSHONLY, this, NULL, &tr);
		UTIL_BloodDecalTrace(&tr, BLOOD_COLOR_RED);
	}

	if (b_ProtectionOn && gpGlobals->curtime > m_flProtectionTimer)
	{
		b_ProtectionOn = false;
		m_flProtectionTimer = 0.0f;
	}

	if (b_CanIdle && gpGlobals->curtime > m_flDelayIdle)
	{
		b_CanIdle = false;
		b_WaitingToDie = false;
		ClearExpression();
		m_flTimerToDie = 0.0f;
	}

	if (b_CanMelee && gpGlobals->curtime > m_flMeleeDelayTimer)
	{
		MeleeAttack();
	}

	BaseClass::Think();
}

void CNPC_John::Event_KilledOther(CBaseEntity* pVictim, const CTakeDamageInfo& info)
{
	if (pVictim && pVictim->IsPlayer())
		PlayAction(JOHN_KILLED_PLAYER, true);

	if (FClassnameIs(pVictim, "npc_zombie") || FClassnameIs(pVictim, "npc_fastzombie"))
	{
		PlayAction(JOHN_KILLED_ZOMBIE, true);
	}
	else if (FClassnameIs(pVictim, "npc_armouredzombie"))
	{
		PlayAction(JOHN_KILLED_ARMOURED_ZOMBIE, true);
	}
	else if (FClassnameIs(pVictim, "npc_burstzombie"))
	{
		PlayAction(JOHN_KILLED_BURST_ZOMBIE, true);
	}
	else if (FClassnameIs(pVictim, "npc_poisonzombie"))
	{
		PlayAction(JOHN_KILLED_POISON_ZOMBIE, true);
	}
	else if (FClassnameIs(pVictim, "npc_headcrab") || FClassnameIs(pVictim, "npc_headcrab_fast") || FClassnameIs(pVictim, "npc_headcrab_black"))
	{
		PlayAction(JOHN_KILLED_HEADCRAB, true);
	}
	else if (FClassnameIs(pVictim, "npc_headcrab_burst"))
	{
		PlayAction(JOHN_KILLED_MINI_HEADCRAB, true);
	}
	else if (FClassnameIs(pVictim, "npc_vortigaunt"))
	{
		PlayAction(JOHN_KILLED_VORTIGAUNT, true);
	}
	else if (FClassnameIs(pVictim, "npc_citizen"))
	{
		PlayAction(JOHN_KILLED_MARINE, true);
	}
}

int CNPC_John::OnTakeDamage_Alive(const CTakeDamageInfo& info)
{
	CTakeDamageInfo subInfo = info;

	if (b_ProtectionOn)
	{
		m_iHealth = g_johnhealth.GetInt();
		return BaseClass::OnTakeDamage_Alive(subInfo);
	}
	else if (m_iHealth > g_johnfallhealth.GetInt()
		|| b_IsDown
		|| b_CanMelee
		|| IsInAScript()
		|| IsInChoreo()
		|| IsInLockedScene())
	{
		if (info.GetDamageType() & DMG_BULLET)
			PlayAction(JOHN_DAMAGE_SHOT, true);
		else
			PlayAction(JOHN_DAMAGE_HIT, true);
		return BaseClass::OnTakeDamage_Alive(subInfo);
	}
	else
	{
		b_CanMelee = false;
		m_flMeleeDelayTimer = gpGlobals->curtime + 90.0f;
		b_WaitingToDie = false;
		Classify();
		SetCondition(COND_NPC_FREEZE);
		SetMoveType(MOVETYPE_NONE);
		ClearCondition(COND_PLAYER_PUSHING);
		AddSpawnFlags(SF_NPC_NO_PLAYER_PUSHAWAY);

		if (skill.GetInt() == 1)
		{
			if (random->RandomInt(1, 2) == 1)
				CallingForHelpScene("scenes/johndown_help_easy1.vcd");
			else
				CallingForHelpScene("scenes/johndown_help_easy2.vcd");

			m_iHealth = 9000;
			m_flTimerToDie = gpGlobals->curtime + 40.0f;
		}
		else if (skill.GetInt() == 2)
		{
			if (random->RandomInt(1, 2) == 1)
				CallingForHelpScene("scenes/johndown_help_medium1.vcd");
			else
				CallingForHelpScene("scenes/johndown_help_medium2.vcd");

			m_iHealth = 100;
			m_flTimerToDie = gpGlobals->curtime + 30.0f;
		}
		else
		{
			if (random->RandomInt(1, 2) == 1)
				CallingForHelpScene("scenes/johndown_help_hard1.vcd");
			else
				CallingForHelpScene("scenes/johndown_help_hard2.vcd");

			m_iHealth = 100;
			m_flTimerToDie = gpGlobals->curtime + 20.0f;
		}

		SetExpression(CallingForHelp());
		b_WaitingToDie = true;
		b_IsDown = true;
		m_flBleed = gpGlobals->curtime + 0.1f;
	}
	return 0;
}

void CNPC_John::Touch(CBaseEntity* pOther)
{
	BaseClass::Touch(pOther);

	if (pOther->IsPlayer())
	{
		PlayAction(JOHN_TOUCH, false);

		CBasePlayer *pPlayer = AI_GetSinglePlayer();

		if (pPlayer->GetActiveWeapon())
		{
			if (FClassnameIs(pPlayer->GetActiveWeapon(), "weapon_healthpack"))
			{
				if (b_IsDown)
				{
					if (pPlayer->GetActiveWeapon())
					{
						pPlayer->GetActiveWeapon()->SendWeaponAnim(ACT_VM_THROW);
						pPlayer->GetActiveWeapon()->WeaponSound(SINGLE);
						pPlayer->GetActiveWeapon()->m_iClip1 -= 1;
					}

					color32 colors = { 255,255,255,64};
					UTIL_ScreenFade(pPlayer, colors, 0.05f, 0.0f, FFADE_IN);

					SetCondition(COND_PLAYER_PUSHING);
					Classify();

					b_IsDown = false;

					SetCondition(COND_NPC_UNFREEZE);
					SetMoveType(MOVETYPE_STEP);

					RemoveSpawnFlags(SF_NPC_NO_PLAYER_PUSHAWAY);

					m_FollowBehavior.SetFollowTarget(UTIL_GetLocalPlayer());
					m_FollowBehavior.SetParameters(AIF_SIDEKICK);

					if (random->RandomInt(1, 2) == 1)
						SetExpression("scenes/johndown_thanks1.vcd");
					else
						SetExpression("scenes/johndown_thanks2.vcd");

					b_CanMelee = false;
					b_WaitingToDie = false;
					m_flMeleeDelayTimer = gpGlobals->curtime + 0.1f;
					m_flTimerToDie = 0.0f;
					m_iHealth = g_johnhealth.GetInt();
					b_CanIdle = true;
					b_ProtectionOn = true;
					m_flDelayIdle = GetSceneDuration(GetExpression()) + gpGlobals->curtime;
					m_flProtectionTimer = gpGlobals->curtime + 10.0f;
				}
			}
		}
	}
}

void CNPC_John::DeathSound(const CTakeDamageInfo& info)
{
	SentenceStop();
	EmitSound("npc_john.die");
}

void CNPC_John::OnPlayerKilledOther(CBaseEntity* pVictim, const CTakeDamageInfo& info)
{
	CAI_BaseNPC* pNPC = dynamic_cast<CAI_BaseNPC*>(pVictim);
	JohnScenes_t scene;

	if (!pNPC)
	{
		return;
	}

	if ((pNPC->LastHitGroup() == HITGROUP_HEAD) && (info.GetDamageType() & DMG_BULLET))
	{
		scene = JOHN_SEEPLAYER_HEADSHOT;
	}

	if ((pNPC->GetDamageCount() == 1) && (info.GetDamageType() & DMG_BULLET))
	{
		scene = JOHN_SEEPLAYER_ONESHOT;
	}
	
	scene = JOHN_SEEPLAYER_KILL;
	PlayAction(scene, true);

	BaseClass::OnPlayerKilledOther(pVictim, info);
}

void CNPC_John::CallingForHelpScene(char* sceneName)
{
	m_szCallingForHelp = sceneName;
}

bool CNPC_John::IsDownDisabled(void)
{
	return b_IsDown == false;
}

const char* CNPC_John::CallingForHelp(void)
{
	return m_szCallingForHelp;
}

void CNPC_John::PlayAction(JohnScenes_t actionName, bool a3)
{
	if (AI_GetSinglePlayer()
		&& !b_IsDown
		&& AI_IsSinglePlayer()
		&& !IsInAScript()
		&& !IsInChoreo()
		&& !IsInLockedScene()
		&& IsAlive()
		&& GetState() != NPC_STATE_SCRIPT
		&& !IsEFlagSet(EFL_IS_BEING_LIFTED_BY_BARNACLE)
		&& (actionName || random->RandomInt(0, 5) == 2)
		&& IsOkToSpeak()
		&& (a3 || gpGlobals->curtime >= m_flSpeakAgain && gpGlobals->curtime >= unk_0x1430))
	{
		if (ahhthanksman.GetInt())
		{
			SetExpression("scenes/johndown_thanks1.vcd");
			m_flDelayIdle = GetSceneDuration(GetExpression()) + gpGlobals->curtime;
			b_CanIdle = true;
			m_flSpeakAgain = GetSceneDuration(GetExpression()) + gpGlobals->curtime;
		}
		else
		{
			switch (actionName)
			{
			case JOHN_TOUCH:
				switch (random->RandomInt(1, 5))
				{
				case 1:
					SetExpression("scenes/JohnTouch/1187_john_touch1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnTouch/1187_john_touch2.vcd");
					break;
				case 3:
					SetExpression("scenes/JohnTouch/1187_john_touch3.vcd");
					break;
				case 4:
					SetExpression("scenes/JohnTouch/1187_john_touch4.vcd");
					break;
				case 5:
					SetExpression("scenes/JohnTouch/1187_john_touch5.vcd");
					break;
				}
				break;
			case JOHN_DAMAGE_HIT:
				switch (random->RandomInt(1, 4))
				{
				case 1:
					SetExpression("scenes/JohnHit/john_damage_hit1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnHit/john_damage_hit2.vcd");
					break;
				case 3:
					SetExpression("scenes/JohnHit/john_damage_hit3.vcd");
					break;
				case 4:
					SetExpression("scenes/JohnHit/john_damage_hit4.vcd");
					break;
				}
				break;
			case JOHN_DAMAGE_SHOT:
				switch (random->RandomInt(1, 4))
				{
				case 1:
					SetExpression("scenes/JohnShot/john_damage_shot1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnShot/john_damage_shot2.vcd");
					break;
				case 3:
					SetExpression("scenes/JohnShot/john_damage_shot3.vcd");
					break;
				case 4:
					SetExpression("scenes/JohnShot/john_damage_shot4.vcd");
					break;
				}
				break;
			case JOHN_KILLED_ZOMBIE:
				switch (random->RandomInt(1, 3))
				{
				case 1:
					SetExpression("scenes/JohnKilledZombie/john_killed_zombie1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnKilledZombie/john_killed_zombie2.vcd");
					break;
				case 3:
					SetExpression("scenes/JohnKilledZombie/john_killed_zombie3.vcd");
					break;
				}
				break;
			case JOHN_KILLED_ARMOURED_ZOMBIE:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnKillerArmoured/john_killed_armouredzombie1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnKillerArmoured/john_killed_armouredzombie2.vcd");
					break;
				}
				break;
			case JOHN_KILLED_BURST_ZOMBIE:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnKilledBurster/john_killed_bursterzombie1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnKilledBurster/john_killed_bursterzombie2.vcd");
					break;
				}
				break;
			case JOHN_KILLED_POISON_ZOMBIE:
				SetExpression("scenes/JohnKilledPoison/john_killed_poisonzombie1.vcd");
				break;
			case JOHN_KILLED_HEADCRAB:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnKilledHeadcrab/john_killed_headcrab1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnKilledHeadcrab/john_killed_headcrab2.vcd");
					break;
				}
				break;
			case JOHN_KILLED_MINI_HEADCRAB:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnKilledMini/john_killed_miniheadcrab1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnKilledMini/john_killed_miniheadcrab2.vcd");
					break;
				}
				break;
			case JOHN_KILLED_VORTIGAUNT:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnKilledVort/john_killed_vort1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnKilledVort/john_killed_vort2.vcd");
					break;
				}
				break;
			case JOHN_KILLED_MARINE:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnKilledMarine/john_killed_marine1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnKilledMarine/john_killed_marine2.vcd");
					break;
				}
				break;
			case JOHN_KILLED_PLAYER:
				SetExpression("scenes/JohnKilledPlayer/john_killed_player1.vcd");
				break;
			case JOHN_SPOT_ZOMBIES:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnSpotZombies/john_spot_zombies1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnSpotZombies/john_spot_zombies2.vcd");
					break;
				}
				break;
			case JOHN_SPOT_CREATURES:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnSpotCreatures/john_spot_creatures1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnSpotCreatures/john_spot_creatures2.vcd");
					break;
				}
				break;
			case JOHN_SPOT_VORTIGAUNT:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnSpotVort/john_spot_vort1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnSpotVort/john_spot_vort2.vcd");
					break;
				}
				break;
			case JOHN_SPOT_MARINE:
				switch (random->RandomInt(1, 3))
				{
				case 1:
					SetExpression("scenes/JohnSpotMarine/john_spot_marine1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnSpotMarine/john_spot_marine2.vcd");
					break;
				case 3:
					SetExpression("scenes/JohnSpotMarine/john_spot_marine3.vcd");
					break;
				}
				break;
			case JOHN_SEEPLAYER_KILL:
				switch (random->RandomInt(1, 3))
				{
				case 1:
					SetExpression("scenes/JohnSeeKill/john_seeplayer_kill1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnSeeKill/john_seeplayer_kill2.vcd");
					break;
				case 3:
					SetExpression("scenes/JohnSeeKill/john_seeplayer_kill3.vcd");
					break;
				}
				break;
			case JOHN_SEEPLAYER_HEADSHOT:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnSeeHeadshot/john_seeplayer_headshot1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnSeeHeadshot/john_seeplayer_headshot2.vcd");
					break;
				}
				break;
			case JOHN_SEEPLAYER_ONESHOT:
				SetExpression("scenes/JohnSeeOneshot/john_seeplayer_oneshot1.vcd");
				break;
			case JOHN_RELOAD:
				switch (random->RandomInt(1, 2))
				{
				case 1:
					SetExpression("scenes/JohnReload/john_reload1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnReload/john_reload2.vcd");
					break;
				}
				break;
			case JOHN_MELEE:
				switch (random->RandomInt(1, 3))
				{
				case 1:
					SetExpression("scenes/JohnMelee/john_melee1.vcd");
					break;
				case 2:
					SetExpression("scenes/JohnMelee/john_melee2.vcd");
					break;
				case 3:
					SetExpression("scenes/JohnMelee/john_melee3.vcd");
					break;
				}
				break;
			default:
				SetExpression("scenes/johndown_thanks2.vcd");
				break;
			}

			m_flDelayIdle = GetSceneDuration(GetExpression()) + gpGlobals->curtime;
			m_flSpeakAgain = GetSceneDuration(GetExpression()) + gpGlobals->curtime;

			if (a3)
			{
				b_CanIdle = true;
				unk_0x1430 = gpGlobals->curtime;
			}
			
			b_CanIdle = true;
			unk_0x1430 = GetSceneDuration(GetExpression()) + gpGlobals->curtime + g_johnspeaklimit.GetFloat();
		}
	}
}
