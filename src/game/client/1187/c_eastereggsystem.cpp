// WARNING:
// Right now this is currently broken and unfinished (due to a section of it crashing the game).
// Feel free to fix/touch up.. -jordyporgie

#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar _egg_1187d1("_egg_1187d1", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar _egg_1187d2("_egg_1187d2", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar _egg_1187d3("_egg_1187d3", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar _egg_1187d4("_egg_1187d4", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar _egg_1187d5("_egg_1187d5", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar _egg_1187d6("_egg_1187d6", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar _egg_1187d7("_egg_1187d7", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar _egg_1187d8("_egg_1187d8", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar _egg_1187d9("_egg_1187d9", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar _egg_1187d10("_egg_1187d10", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);

ConVar _dec3542568236206973268276836829("_dec3542568236206973268276836829", "0", FCVAR_CLIENTDLL | FCVAR_HIDDEN);
ConVar dec_1187_encryptedeggs("dec_1187_encryptedeggs", "", FCVAR_CLIENTDLL | FCVAR_HIDDEN | FCVAR_ARCHIVE);

static void CC_75646754887676789089083428847867(void)
{
	Msg("You found an easter egg, congrats!\n");

	char *mapFileNames[]
	{
		"maps/1187d1.bsp",
		"maps/1187d2.bsp",
		"maps/1187d3.bsp",
		"maps/1187d4.bsp",
		"maps/1187d5.bsp",
		"maps/1187d6.bsp",
		"maps/1187d7.bsp",
		"maps/1187d8.bsp",
		"maps/1187d9.bsp",
		"maps/1187d10.bsp",
	};

	const char* p1187EggNames[]
	{
		"_egg_1187d1",
		"_egg_1187d2",
		"_egg_1187d3",
		"_egg_1187d4",
		"_egg_1187d5",
		"_egg_1187d6",
		"_egg_1187d7",
		"_egg_1187d8",
		"_egg_1187d9",
		"_egg_1187d10",
	};

	for (int i = 0; i < 10; ++i)
	{
		if (!strcmp(engine->GetLevelName(), mapFileNames[i]))
		{
			ConVarRef eastereggs(p1187EggNames[i]);
			eastereggs.SetValue(1);
		}
	}

	int v2 = 0;
	for (int j = 0; j < 10; ++j)
	{
		ConVarRef eastereggs2(p1187EggNames[j]);
		if (eastereggs2.GetInt() == 1)
			++v2;
	}

	ConVarRef byte("xbox_forcebytes");
	byte.SetValue(v2);
	
	char value[2];
	strcpy(value, "0");

	int v6 = -9;
	int v7 = 100;

	char *mapname;
	const char *mapbuffer;
	int mapcount;

	ConVarRef easteregg1("_egg_1187d1");
	ConVarRef easteregg2("_egg_1187d2");
	ConVarRef easteregg3("_egg_1187d3");
	ConVarRef easteregg4("_egg_1187d4");
	ConVarRef easteregg5("_egg_1187d5");
	ConVarRef easteregg6("_egg_1187d6");
	ConVarRef easteregg7("_egg_1187d7");
	ConVarRef easteregg8("_egg_1187d8");
	ConVarRef easteregg9("_egg_1187d9");
	ConVarRef easteregg10("_egg_1187d10");

	do
	{
		switch (v6)
		{
			case 0:
				mapcount = strlen(easteregg1.GetString()) + 1;
				mapbuffer = easteregg1.GetString();
				mapname = mapFileNames[0];
				while (++mapname);
				break;
			
			case 9:
				mapcount = strlen(easteregg2.GetString()) + 1;
				mapbuffer = easteregg2.GetString();
				mapname = mapFileNames[1];
				while (++mapname);
				break;
			
			case 15:
				mapcount = strlen(easteregg3.GetString()) + 1;
				mapbuffer = easteregg3.GetString();
				mapname = mapFileNames[2];
				while (++mapname);
				break;
			
			case 21:
				mapcount = strlen(easteregg4.GetString()) + 1;
				mapbuffer = easteregg4.GetString();
				mapname = mapFileNames[3];
				while (++mapname);
				break;
			
			case 24:
				mapcount = strlen(easteregg5.GetString()) + 1;
				mapbuffer = easteregg5.GetString();
				mapname = mapFileNames[4];
				while (++mapname);
				break;
			
			case 45:
				mapcount = strlen(easteregg6.GetString()) + 1;
				mapbuffer = easteregg6.GetString();
				mapname = mapFileNames[5];
				while (++mapname);
				break;
			
			case 46:
				mapcount = strlen(easteregg7.GetString()) + 1;
				mapbuffer = easteregg7.GetString();
				mapname = mapFileNames[6];
				while (++mapname);
				break;
			
			case 52:
				mapcount = strlen(easteregg8.GetString()) + 1;
				mapbuffer = easteregg8.GetString();
				mapname = mapFileNames[7];
				while (++mapname);
				break;
			
			case 56:
				mapcount = strlen(easteregg9.GetString()) + 1;
				mapbuffer = easteregg9.GetString();
				mapname = mapFileNames[8];
				while (++mapname);
				break;
			
			case 68:
				mapcount = strlen(easteregg10.GetString()) + 1;
				mapbuffer = easteregg10.GetString();
				mapname = mapFileNames[9];
				while (++mapname);
				break;

			default:
				char buffer[4];
				sprintf(buffer, "%d", random->RandomInt(0,1));
				mapcount = strlen(buffer) + 1;
				mapbuffer = buffer;
				while (++mapname);
				break;
		}
		Q_memcpy(mapname, mapbuffer, mapcount);
		++v6;
		--v7;
	}
	while (v7);

	dec_1187_encryptedeggs.SetValue(value);
	engine->ClientCmd("host_writeconfig");
}

ConCommand _75646754887676789089083428847867("_75646754887676789089083428847867", CC_75646754887676789089083428847867, "List easter eggs.", FCVAR_SERVER_CAN_EXECUTE | FCVAR_CLIENTDLL);

static void CC_ListEggs(void)
{
	const char *p1187EggNames[]
	{
		"_egg_1187d1",
		"_egg_1187d2",
		"_egg_1187d3",
		"_egg_1187d4",
		"_egg_1187d5",
		"_egg_1187d6",
		"_egg_1187d7",
		"_egg_1187d8",
		"_egg_1187d9",
		"_egg_1187d10",
	};

	int i;

	for (i = 0; i < 10; ++i)
	{
		Msg("Egg [%s] is ", p1187EggNames[i]);

		ConVarRef maps(p1187EggNames[i]);

		if (maps.GetInt() == 1)
			Msg("Found.\n");
		else
			Msg("Not Found.\n");
	}

	ConVarRef maps2("xbox_forcebytes");
	Msg("Your total collected eggs is: [%i]\n", maps2.GetInt());
	ConVarRef maps3("xbox_forcebytes");

	if (maps3.GetInt() == 10)
		Msg("You have unlocked all easter eggs.\n");
}

ConCommand cl_listeggs("cl_listeggs", CC_ListEggs, "List easter eggs.", FCVAR_SERVER_CAN_EXECUTE | FCVAR_CLIENTDLL);
