"MainMenu"
{	
	"ResumeGame"
	{
		"text"			"#GameUI_GameMenu_ResumeGame"
		"command"		"cmd gamemenucommand resumegame"
		"priority"		"8"
		"family"		"ingame"
	}
	
	"CrashCourse"
	{
		"text"			"#GameUI_GameMenu_CrashCourse"
		"command"		"cmd map 1187_crashcourse"
		"priority"		"7"
		"family"		"all"
	}
	
	"NewGame"
	{
		"text"			"#GameUI_GameMenu_NewGame"
		"command"		"cmd gamepadui_opennewgamedialog"
		"priority"		"6"
		"family"		"all"
	}
	
	"SaveGame"
	{
		"text"			"#GameUI_GameMenu_SaveGame"
		"command"		"cmd gamepadui_opensavegamedialog"
		"priority"		"5"
		"family"		"ingame"
	}

	"LoadGame"
	{
		"text"			"#GameUI_GameMenu_LoadGame"
		"command"		"cmd gamepadui_openloadgamedialog"
		"priority"		"4"
		"family"		"all"
	}

	"GameManual"
	{
		"text"			"#GameUI_GameMenu_GameManual"
		"command"		"cmd g_showmanual 1"
		"priority"		"3"
		"family"		"all"
	}

	"Options"
	{
		"text"			"#GameUI_GameMenu_Options"
		"command"		"cmd g_showoptions 1"
		"priority"		"2"
		"family"		"all"
	}

	"Settings"
	{
		"text"			"#GameUI_GameMenu_Settings"
		"command"		"cmd gamepadui_openoptionsdialog"
		"priority"		"1"
		"family"		"all"
	}

	"Quit"
	{
		"text"			"#GameUI_GameMenu_Quit"
		"command"		"cmd gamepadui_openquitgamedialog"
		"priority"		"0"
		"family"		"all"
	}
}