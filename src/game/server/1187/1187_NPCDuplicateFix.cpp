//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: The system for handling director's commentary style production info in-game.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "tier0/icommandline.h"
#include "igamesystem.h"
#include "filesystem.h"
#include <KeyValues.h>
#include "in_buttons.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"
#include "utldict.h"
#include "isaverestore.h"
#include "eventqueue.h"
#include "saverestore_utlvector.h"
#include "gamestats.h"
#include "ai_basenpc.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


class C1187NPCDumplicateFixSystem : public CAutoGameSystemPerFrame
{
public:
	virtual char const *Name() { return "1187NPCDuplicateFix"; }

	C1187NPCDumplicateFixSystem(char const *name) : CAutoGameSystemPerFrame(name)
	{
		m_bLevelInitted = false;
	}

	virtual void LevelInitPostEntity(void)
	{
		// Don't remove duplicates when loading a saved game.
		if (gpGlobals->eLoadType == MapLoad_LoadGame || gpGlobals->eLoadType == MapLoad_Background)
			return;

		static const char* levels[] =
		{
			"1187d3",
			"1187d4",
			"1187d5",
			"1187d6",
			"1187d7",
			"1187d8",
			"1187d9",
			"1187d10",
		};

		size_t i;
		bool found = false;

		for (i = 0; i < ARRAYSIZE(levels) && !found; i++)
		{
			if (FStrEq(STRING(gpGlobals->mapname), levels[i]))
			{
				found = true;
			}
		}

		m_bLevelInitted = true;
	}

	virtual void FrameUpdatePostEntityThink(void)
	{
		if (!m_bLevelInitted)
			return;

		static const char* classnames[] =
		{
			"npc_chris",
			"npc_john",
			"npc_mike",
		};

		static const char* names[] =
		{
			"chris",
			"john",
			"mike",
		};

		size_t i;
		for (i = 0; i < ARRAYSIZE(classnames); i++)
		{
			CBaseEntity* pEnt = NULL;
			size_t instanceCount = 0;
			while ((pEnt = gEntList.FindEntityByClassname(pEnt, classnames[i])) != NULL)
			{
				if (instanceCount > 0)
				{
					UTIL_Remove(pEnt);
				}
				instanceCount++;
				pEnt->SetName(AllocPooledString( names[i] ) );
			}
		}
	}

	bool m_bLevelInitted;
};

C1187NPCDumplicateFixSystem	g_1187NPCDuplicateFixSystem("1187NPCDuplicateFix");