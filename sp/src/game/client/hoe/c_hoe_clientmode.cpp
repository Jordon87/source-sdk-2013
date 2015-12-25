//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "ivmodemanager.h"
#include "c_hoe_clientmode_normal.h"
#include "panelmetaclassmgr.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// default FOV for HL2
ConVar default_fov( "default_fov", "75", FCVAR_CHEAT );

// The current client mode. Always ClientModeNormal in HL.
IClientMode *g_pClientMode = NULL;

#define SCREEN_FILE		"scripts/vgui_screens.txt"

class CHoeModeManager : public IVModeManager
{
public:
				CHoeModeManager( void );
	virtual		~CHoeModeManager( void );

	virtual void	Init( void );
	virtual void	SwitchMode( bool commander, bool force );
	virtual void	OverrideView( CViewSetup *pSetup );
	virtual void	CreateMove( float flInputSampleTime, CUserCmd *cmd );
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
};

CHoeModeManager::CHoeModeManager(void)
{
}

CHoeModeManager::~CHoeModeManager(void)
{
}

void CHoeModeManager::Init(void)
{
	g_pClientMode = GetClientModeNormal();
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void CHoeModeManager::SwitchMode(bool commander, bool force)
{
}

void CHoeModeManager::OverrideView(CViewSetup *pSetup)
{
}

void CHoeModeManager::CreateMove(float flInputSampleTime, CUserCmd *cmd)
{
}

void CHoeModeManager::LevelInit(const char *newmap)
{
	g_pClientMode->LevelInit( newmap );
}

void CHoeModeManager::LevelShutdown(void)
{
	g_pClientMode->LevelShutdown();
}


static CHoeModeManager g_HoeModeManager;
IVModeManager *modemanager = &g_HoeModeManager;

