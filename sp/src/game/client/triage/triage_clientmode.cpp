//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "ivmodemanager.h"
#include "clientmode_triagenormal.h"
#include "panelmetaclassmgr.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// default FOV for HL2
ConVar default_fov( "default_fov", "75", FCVAR_CHEAT );

// The current client mode. Always ClientModeNormal in HL.
IClientMode *g_pClientMode = NULL;

#define SCREEN_FILE		"scripts/vgui_screens.txt"

class CTriageModeManager : public IVModeManager
{
public:
				 CTriageModeManager(void);
	virtual		~CTriageModeManager(void);

	virtual void	Init( void );
	virtual void	SwitchMode( bool commander, bool force );
	virtual void	OverrideView( CViewSetup *pSetup );
	virtual void	CreateMove( float flInputSampleTime, CUserCmd *cmd );
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
};

CTriageModeManager::CTriageModeManager(void)
{
}

CTriageModeManager::~CTriageModeManager(void)
{
}

void CTriageModeManager::Init(void)
{
	g_pClientMode = GetClientModeNormal();
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void CTriageModeManager::SwitchMode(bool commander, bool force)
{
}

void CTriageModeManager::OverrideView(CViewSetup *pSetup)
{
}

void CTriageModeManager::CreateMove(float flInputSampleTime, CUserCmd *cmd)
{
}

void CTriageModeManager::LevelInit(const char *newmap)
{
	g_pClientMode->LevelInit( newmap );
}

void CTriageModeManager::LevelShutdown(void)
{
	g_pClientMode->LevelShutdown();
}


static CTriageModeManager g_TriageModeManager;
IVModeManager *modemanager = &g_TriageModeManager;

