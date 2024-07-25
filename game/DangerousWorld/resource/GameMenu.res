"GameMenu"
{
	"1"
	{
		"label" "#GameUI_GameMenu_ResumeGame"
		"command" "ResumeGame"
		"InGameOrder" "10"
		"OnlyInGame" "1"
	}
	"2"
	{
		"label" "#GameUI_GameMenu_NewGame"
		"command" "OpenNewGameDialog"
		"InGameOrder" "40"
		"notmulti" "1"
	}
	"3"
	{
		"label" "#GameUI_GameMenu_BonusMaps"
		"command" "OpenBonusMapsDialog"
		"InGameOrder" "50"
		"notmulti" "1"
	}
	"4"
	{
		"label" "#GameUI_GameMenu_LoadGame"
		"command" "OpenLoadGameDialog"
		"InGameOrder" "30"
		"notmulti" "1"
	}
	"5"
	{
		"label" "#GameUI_GameMenu_SaveGame"
		"command" "OpenSaveGameDialog"
		"InGameOrder" "20"
		"notmulti" "1"
		"OnlyInGame" "1"
	}
	"7"
	{
		"label" "#GameUI_GameMenu_Options"
		"command" "OpenOptionsDialog"
		"InGameOrder" "90"
	}
	"8"
	{
		"label" "#GameUI_GameMenu_Trailer"
		"command" "engine playvideo Trailer"
		"InGameOrder" "95"
	}
	"9"
	{
		"label" "#GameUI_GameMenu_Quit"
		"command" "Quit"
		"InGameOrder" "100"
	}
	"11"
	{
		"label" "#GameUI_GameMenu_Web"
		"command" "engine exec manual_workaround.cfg"
		"InGameOrder" "150"
	}
}

