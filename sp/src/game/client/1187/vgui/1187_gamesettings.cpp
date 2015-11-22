//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "filesystem.h"

#include "../gamui/1187_gamesettingsdialog.h"
#include "../1187_gamesettings_config.h"
#include "1187_gamesettings.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

#define GAMESETTINGS_FILENAME		"data/game.dat"

class C1187GameSettingsPanelInterface : public I1187GameSettings
{
private:
	Eleven87GameSettings_t m_settings;
	C1187GameSettingsDialog *m_pGameSettings;
public:
	C1187GameSettingsPanelInterface()
	{
		m_pGameSettings = NULL;

		Q_memset(&m_settings, 0, sizeof(Eleven87GameSettings_t));
	}
	void Create(vgui::VPANEL parent)
	{
		m_pGameSettings = new C1187GameSettingsDialog(parent);
	}
	void Destroy()
	{
		if (m_pGameSettings)
		{
			m_pGameSettings->SetParent((vgui::Panel *)NULL);
			delete m_pGameSettings;
		}
	}

	virtual void Init(void)
	{
		if (!LoadSettings(&m_settings))
			return;

		if (m_pGameSettings)
			m_pGameSettings->SetSettings(&m_settings);
	}

	virtual void Shutdown(void)
	{
		if (!SaveSettings(&m_settings))
			return;
	}

	virtual void SetVisible(bool visible)
	{
		if (m_pGameSettings)
		{
			m_pGameSettings->SetSettings(&m_settings);
			m_pGameSettings->SetVisible(visible);
			m_pGameSettings->Activate();
			m_pGameSettings->MoveToFront();
		}
	}

	bool LoadSettings(Eleven87GameSettings_t* settings)
	{
		if (!settings)
			return false;

		Assert(settings);
		if (g_pFullFileSystem->FileExists(GAMESETTINGS_FILENAME, "MOD"))
		{
			FileHandle_t fh = g_pFullFileSystem->Open(GAMESETTINGS_FILENAME, "rb");

			if (FILESYSTEM_INVALID_HANDLE != fh)
			{
				g_pFullFileSystem->Read(settings, sizeof(Eleven87GameSettings_t), fh);
				g_pFullFileSystem->Close(fh);
				return true;
			}
			else
			{
				Warning("Unable to open '%s' for reading\n", GAMESETTINGS_FILENAME);
			}
		}

		return false;
	}

	bool SaveSettings(Eleven87GameSettings_t* settings)
	{
		if (!settings)
			return false;

		Assert(settings);

		FileHandle_t fh = filesystem->Open(GAMESETTINGS_FILENAME, "wb");

		if (!fh)
		{
			DevWarning(2, "Couldn't create %s!\n", GAMESETTINGS_FILENAME);
			return false;
		}

		g_pFullFileSystem->Write(settings, sizeof(Eleven87GameSettings_t), fh);
		g_pFullFileSystem->Close(fh);

		return true;
	}

	void Apply(void* data)
	{
		Eleven87GameSettings_t* settings = (Eleven87GameSettings_t*)data;

		if (!settings)
			return;

		Q_memcpy(&m_settings, settings, sizeof(Eleven87GameSettings_t));

		ConVar* pVar = cvar->FindVar("cl_viewbob_enabled");

		if (pVar)
		{
			pVar->SetValue(settings->m_viewbobbing);
		}
		else
		{
			Warning("Couldn't find cvar named cl_viewbob_enabled.\n");
		}

		pVar = cvar->FindVar("cl_1187_ironblur_enabled");

		if (pVar)
		{
			pVar->SetValue(settings->m_ironblur);
		}
		else
		{
			Warning("Couldn't find cvar named cl_1187_ironblur_enabled.\n");
		}

		pVar = cvar->FindVar("cl_1187_sprintblur_enabled");

		if (pVar)
		{
			pVar->SetValue(settings->m_sprintblur);
		}
		else
		{
			Warning("Couldn't find cvar named cl_1187_sprintblur_enabled.\n");
		}

		pVar = cvar->FindVar("hud_fastswitch");

		if (pVar)
		{
			pVar->SetValue((settings->m_hudstyle == 1) ? 3 : 0);
		}
		else
		{
			Warning("Couldn't find cvar named hud_fastswitch.\n");
		}

		if (SaveSettings(&m_settings))
		{
			DevMsg("Successfully saved 1187 game config to %s.\n", GAMESETTINGS_FILENAME);
		}
		else
		{
			DevMsg("Failed to save 1187 game config to %s.\n", GAMESETTINGS_FILENAME);
		}
	}

	virtual void SetEasterEggs(int count)
	{
		if (count < 0)
			count = 0;

		m_settings.m_eggcount = count;

		SaveSettings(&m_settings);
	}
};

static C1187GameSettingsPanelInterface g_1187gamesettings;
I1187GameSettings* g_p1187gamesettings = (I1187GameSettings*)&g_1187gamesettings;