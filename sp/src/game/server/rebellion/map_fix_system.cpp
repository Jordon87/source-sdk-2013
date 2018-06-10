#include "cbase.h"
#include "fmtstr.h"
#include "ai_basenpc.h"
#include "npc_citizen17.h"
#include "player.h"
#include "scripted.h"

static void AddEntityOutput(CBaseEntity* escapeRelay, const char* action, const char* actionValues)
{
	inputdata_t output;
	output.pActivator = escapeRelay;
	output.pCaller = escapeRelay;
	output.nOutputID = (USE_TYPE)USE_ON;
	output.value.SetString(AllocPooledString(CFmtStr("%s %s", action, actionValues)));
	escapeRelay->InputAddOutput(output);
}

static void RemoveNamedEntities(const char* targetnameOfEntitiesToRemove)
{
	CBaseEntity* entityToRemove = NULL;
	while ((entityToRemove = gEntList.FindEntityByName(entityToRemove, targetnameOfEntitiesToRemove)) != NULL)
	{
		UTIL_Remove(entityToRemove);
	}
}

//
// Entity to handle weapon visibility for NPCs waiting in PVS on map 03_2. 
//
class CMap032FixEntitiesInPVS : public CLogicalEntity
{
	DECLARE_CLASS(CMap032FixEntitiesInPVS, CLogicalEntity);
public:
	DECLARE_DATADESC();

	CMap032FixEntitiesInPVS()
	{
		m_entityNameIndex = 0;
	}

	void Spawn(void)
	{
		AddEffects(EF_NODRAW);
	}

	void InputWake(inputdata_t& inputdata)
	{
		ApplyOperationToEntities(WakeEntity);
	}

	void InputSleep(inputdata_t& inputdata)
	{
		ApplyOperationToEntities(PutEntityToSleep);
	}

	void AddEntity(const char* entityTargetname)
	{
		if (m_entityNameIndex == MAX_ENTITIES)
			return;

		m_entityNames[m_entityNameIndex] = AllocPooledString( entityTargetname );
		m_entityNameIndex++;
	}

protected:

	void ApplyOperationToEntities(void (*operationPerEntity)(CBaseEntity*))
	{
		Assert(operationPerEntity != NULL);

		CBaseEntity* entity = NULL;
		for (int i = 0; i < MAX_ENTITIES; i++)
		{
			if (m_entityNames[i] != NULL_STRING)
			{
				while ((entity = gEntList.FindEntityByName(entity, m_entityNames[i])) != NULL)
				{
					operationPerEntity(entity);
				}
			}
		}
	}

private:

	static const int MAX_ENTITIES = 8;

	static void WakeEntity(CBaseEntity* entityToWake)
	{
		CAI_BaseNPC* entityToWakeAsNPC = entityToWake->MyNPCPointer();
		if (entityToWakeAsNPC)
		{
			if (entityToWakeAsNPC->GetSleepState() != AIE_NORMAL)
				entityToWakeAsNPC->Wake();

			if (entityToWakeAsNPC->GetActiveWeapon())
				entityToWakeAsNPC->GetActiveWeapon()->RemoveEffects(EF_NODRAW);

			entityToWakeAsNPC->m_takedamage = DAMAGE_YES;
			entityToWakeAsNPC->RemoveSolidFlags(FSOLID_NOT_SOLID);
			entityToWakeAsNPC->SetCollisionGroup(COLLISION_GROUP_NPC);
		}
	}

	static void PutEntityToSleep(CBaseEntity* entityToSleep)
	{
		CAI_BaseNPC* entityToSleepAsNPC = entityToSleep->MyNPCPointer();
		if (entityToSleepAsNPC)
		{
			if (entityToSleepAsNPC->GetSleepState() == AIE_NORMAL)
				entityToSleepAsNPC->Sleep();

			if (entityToSleepAsNPC->GetActiveWeapon())
				entityToSleepAsNPC->GetActiveWeapon()->AddEffects(EF_NODRAW);

			entityToSleepAsNPC->m_takedamage = DAMAGE_NO;
			entityToSleepAsNPC->AddSolidFlags(FSOLID_NOT_SOLID);
			entityToSleepAsNPC->SetCollisionGroup(COLLISION_GROUP_NONE);
		}
	}

	string_t m_entityNames[MAX_ENTITIES];
	int m_entityNameIndex;
};

LINK_ENTITY_TO_CLASS(map_032_fix_entities_in_pvs, CMap032FixEntitiesInPVS);

BEGIN_DATADESC(CMap032FixEntitiesInPVS)
	DEFINE_ARRAY(m_entityNames, FIELD_STRING, CMap032FixEntitiesInPVS::MAX_ENTITIES),
	DEFINE_FIELD(m_entityNameIndex, FIELD_INTEGER),

	DEFINE_INPUTFUNC(FIELD_VOID, "Wake", InputWake),
	DEFINE_INPUTFUNC(FIELD_VOID, "Sleep", InputSleep)
END_DATADESC()

//
// Entity to use as replacement for weapon stripper on map 03_2. 
//
class CMap032FixWeaponStrip : public CPointEntity
{
	DECLARE_CLASS(CMap032FixWeaponStrip, CPointEntity);
public:
	DECLARE_DATADESC();

	void InputStripWeaponsAndSuit(inputdata_t &data)
	{
		CBasePlayer* player = UTIL_GetLocalPlayer();
		if (player)
			player->RemoveAllItems(false);

		UTIL_Remove(this);
	}
};

LINK_ENTITY_TO_CLASS(map_032_fix_weapon_strip, CMap032FixWeaponStrip);

BEGIN_DATADESC(CMap032FixWeaponStrip)
	DEFINE_INPUTFUNC(FIELD_VOID, "StripWeaponsAndSuit", InputStripWeaponsAndSuit),
END_DATADESC()


class CMapFixSystem : public CAutoGameSystem
{
public:
	CMapFixSystem(char const *name) : CAutoGameSystem(name)
	{
	}

	virtual void LevelInitPostEntity()
	{
		if (FStrEq(STRING(gpGlobals->mapname), "02"))
		{
			FixMap02CitizenIgnoreDropship();
		}
		if (FStrEq(STRING(gpGlobals->mapname), "03_1"))
		{
			FixMap031ThreepwoodRaillingSequence();
			FixMap031InitEscapeRelay();
		}

		if (FStrEq(STRING(gpGlobals->mapname), "03_2"))
		{
			FixMap032SoldiersInPVS();
			FixMap032CitizensInPVS();
			FixMap032SuitStrip();
		}
	}

private:

	// This code is intended to fix an issue on map 02 where the
	// dropship is unable to be destroyed due to broken relationship
	// between dropship and RPG citizen. Since the destruction event
	// can only occur if the citizen fires the OnFoundEnemy output,
	// we create and spawn a relationship adapting both citizen and
	// dropship.
	void FixMap02CitizenIgnoreDropship()
	{
		if (gpGlobals->eLoadType != MapLoadType_t::MapLoad_NewGame && 
			gpGlobals->eLoadType != MapLoadType_t::MapLoad_Transition)
			return;

		CBaseEntity* relationShip = CreateEntityByName("ai_relationship");
		if (relationShip)
		{
			relationShip->KeyValue("subject", "citizenRpg01");
			relationShip->KeyValue("target", "dropShip01");
			relationShip->KeyValue("disposition", "1"); // "Dislike" disposition
			relationShip->KeyValue("StartActive", "1");
			relationShip->SetAbsOrigin(vec3_origin);
			relationShip->SetAbsAngles(vec3_angle);
			relationShip->Precache();
			DispatchSpawn(relationShip);
			relationShip->Activate();
		}
	}

	// This code is intended to fix an issue on map 03_1 where
	// Threepwood is unable to exit it's scripted sequence because
	// of no CancelSequence is called before sumonning the next
	// script. In this case, we add a CancelSequence output to
	// the entity responsible for triggering outputs.
	//
	// Since the original BeginSequence outputs start at zero, it
	// could happen that CancelSequence would get triggered after
	// BeginSequence series, so we add them again at the end to make
	// sure they get called after the cancellation of the blocking
	// sequence.
	void FixMap031ThreepwoodRaillingSequence()
	{
		if (gpGlobals->eLoadType != MapLoadType_t::MapLoad_NewGame &&
			gpGlobals->eLoadType != MapLoadType_t::MapLoad_Transition)
			return;

		CBaseEntity* scene07 = gEntList.FindEntityByName(NULL, "scene07");
		if (scene07)
		{
			AddEntityOutput(scene07, "OnCompletion", "script_threepwood03,CancelSequence,,0,-1");
			AddEntityOutput(scene07, "OnCompletion", "script_threepwood04,BeginSequence,,1,-1");
			AddEntityOutput(scene07, "OnCompletion", "script_zero04,BeginSequence,,1,-1");
		}
	}

	// This code is intended to fix an issue on map 03_1 where
	// the escape event would not be fired due to it's relay entity
	// deleted. In this case, we create an identical entity with
	// the correct outputs then bind it to the entity responsible
	// for triggering it.
	void FixMap031InitEscapeRelay()
	{
		if (gpGlobals->eLoadType != MapLoadType_t::MapLoad_NewGame &&
			gpGlobals->eLoadType != MapLoadType_t::MapLoad_Transition)
			return;

		// Delete the original escape relay entity.
		RemoveNamedEntities("relay_initEscape");

		// Create an identical escape relay entity with all necessary outputs.
		CBaseEntity* newEscapeRelay = CreateEntityByName("logic_relay");
		if (newEscapeRelay)
		{
			newEscapeRelay->KeyValue("targetname", "relay_initEscape2");
			newEscapeRelay->KeyValue("StartDisabled", "0");

			AddEntityOutput(newEscapeRelay, "OnTrigger", "goalAssault_scene04_combine03,BeginAssault,,4,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "goalAssault_scene04_combine01,BeginAssault,,4,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "sound_striderSound,PlaySound,,3,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "combine_strider01,Wake,,3,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "relay_scene03_fireFight,Trigger,,2,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "script_zero07,BeginSequence,,0.15,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "script_threepwood07,BeginSequence,,0.05,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "sound_mandella07,PlaySound,,0.04,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "script_threepwood06,CancelSequence,,0,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "script_zero06,CancelSequence,,0,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "autosave,Save,,0,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "sound_background02,PlaySound,,0.25,1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "goalAssault_scene04_combine03,Activate,,2,-1");
			AddEntityOutput(newEscapeRelay, "OnTrigger", "!self,!kill,,5,-1");

			newEscapeRelay->SetAbsOrigin(vec3_origin);
			newEscapeRelay->SetAbsAngles(vec3_angle);
			DispatchSpawn(newEscapeRelay);
		}

		CBaseEntity* scene10 = gEntList.FindEntityByName(NULL, "scene10");
		if (scene10)
		{
			// Bind the new escape relay entity.
			AddEntityOutput(scene10, "OnCompletion", "relay_initEscape2,Trigger,,0,-1");
		}
	}

	// This code is intended to fix an issue on map 03_1 where
	// weapons of NPCs marked as 'asleep' would remain visible.
	// We add a EF_NODRAW effect to make those weapons invisible
	// and remove them to set them visible again.
	void FixMap032SoldiersInPVS()
	{
		if (gpGlobals->eLoadType != MapLoadType_t::MapLoad_NewGame &&
			gpGlobals->eLoadType != MapLoadType_t::MapLoad_Transition)
			return;

		// Create the entity responsible for handling
		// weapon visibility for targeted NPCs in PVS.
		CMap032FixEntitiesInPVS* soldierPVSFixEntity = (CMap032FixEntitiesInPVS*)CreateEntityByName("map_032_fix_entities_in_pvs");
		soldierPVSFixEntity->KeyValue("targetname", "map_032_fix_soldiers_in_pvs");
		soldierPVSFixEntity->SetAbsOrigin(vec3_origin);
		soldierPVSFixEntity->SetAbsAngles(vec3_angle);
		DispatchSpawn(soldierPVSFixEntity);

		// Add NPCs to targeted NPCs in PVS.
		soldierPVSFixEntity->AddEntity("03combineSoldier_lastScene05");
		soldierPVSFixEntity->AddEntity("03combineOfficer_lastScene");

		// Hide weapons of targeted NPCs in PVS.
		variant_t emptyVariant;
		soldierPVSFixEntity->AcceptInput("Sleep", NULL, NULL, emptyVariant, (USE_TYPE)USE_ON);

		// Bind the newly created entity to make all weapons visible
		// again when the original 'Wake' input will be fired.
		CBaseEntity* soldierRelayer = gEntList.FindEntityByName(NULL, "relay_confedAppear");
		if (soldierRelayer)
		{
			AddEntityOutput(soldierRelayer, "OnTrigger", "map_032_fix_soldiers_in_pvs,Wake,,0,-1");
		}
	}

	// This code is intended to fix an issue on map 03_1 where
	// weapons of NPCs marked as 'asleep' would remain visible.
	// We add a EF_NODRAW effect to make those weapons invisible
	// and remove them to set them visible again.
	void FixMap032CitizensInPVS()
	{
		if (gpGlobals->eLoadType != MapLoadType_t::MapLoad_NewGame &&
			gpGlobals->eLoadType != MapLoadType_t::MapLoad_Transition)
			return;

		// Create the entity responsible for handling
		// weapon visibility for targeted NPCs in PVS.
		CMap032FixEntitiesInPVS* citizenPVSFixEntity = (CMap032FixEntitiesInPVS*)CreateEntityByName("map_032_fix_entities_in_pvs");
		citizenPVSFixEntity->KeyValue("targetname", "map_032_fix_citizens_in_pvs");
		citizenPVSFixEntity->SetAbsOrigin(vec3_origin);
		citizenPVSFixEntity->SetAbsAngles(vec3_angle);
		DispatchSpawn(citizenPVSFixEntity);

		// Add NPC to targeted NPCs in PVS.
		citizenPVSFixEntity->AddEntity("citizenSoldier02");

		// Hide weapons of targeted NPCs in PVS.
		variant_t emptyVariant;
		citizenPVSFixEntity->AcceptInput("Sleep", NULL, NULL, emptyVariant, (USE_TYPE)USE_ON);

		// Bind the newly created entity to make all weapons visible
		// again when the original 'Wake' input will be fired.
		CBaseEntity* rebelRelayer = gEntList.FindEntityByName(NULL, "relay_rebelAppear");
		if (rebelRelayer)
		{
			AddEntityOutput(rebelRelayer, "OnTrigger", "map_032_fix_citizens_in_pvs,Wake,,0,-1");
		}
	}

	// This code is intended to prevent weapon stripper from
	// stripping player's suit since the player has a crowbar
	// on map 04 and may wish to view weapon selection HUD.
	void FixMap032SuitStrip()
	{
		if (gpGlobals->eLoadType != MapLoadType_t::MapLoad_NewGame &&
			gpGlobals->eLoadType != MapLoadType_t::MapLoad_Transition)
			return;

		RemoveNamedEntities("weaponstrip");

		// Create a new entity to use as weapon stripper
		// to only remove player weapons.
		CBaseEntity* newWeaponStrip = CreateEntityByName("map_032_fix_weapon_strip");
		if (newWeaponStrip)
		{
			newWeaponStrip->KeyValue("targetname", "weaponstrip");
			DispatchSpawn(newWeaponStrip);
		}
	}
};

CMapFixSystem MapFixSystem("CMapFixSystem");
