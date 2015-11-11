//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "ivmodemanager.h"
#include "clientmode_1187normal.h"
#include "panelmetaclassmgr.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// default FOV for HL2
ConVar default_fov( "default_fov", "75", FCVAR_CHEAT );

// The current client mode. Always ClientModeNormal in 1187.
IClientMode *g_pClientMode = NULL;

#define SCREEN_FILE		"scripts/vgui_screens.txt"

class C1187ModeManager : public IVModeManager
{
public:
				C1187ModeManager( void );
	virtual		~C1187ModeManager( void );

	virtual void	Init( void );
	virtual void	SwitchMode( bool commander, bool force );
	virtual void	OverrideView( CViewSetup *pSetup );
	virtual void	CreateMove( float flInputSampleTime, CUserCmd *cmd );
	virtual void	LevelInit( const char *newmap );
	virtual void	LevelShutdown( void );
};

C1187ModeManager::C1187ModeManager( void )
{
}

C1187ModeManager::~C1187ModeManager( void )
{
}

void C1187ModeManager::Init( void )
{
	g_pClientMode = GetClientModeNormal();
	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void C1187ModeManager::SwitchMode( bool commander, bool force )
{
}

void C1187ModeManager::OverrideView( CViewSetup *pSetup )
{
}

void C1187ModeManager::CreateMove( float flInputSampleTime, CUserCmd *cmd )
{
}

void C1187ModeManager::LevelInit( const char *newmap )
{
	g_pClientMode->LevelInit( newmap );
}

void C1187ModeManager::LevelShutdown( void )
{
	g_pClientMode->LevelShutdown();
}


static C1187ModeManager g_1187ModeManager;
IVModeManager *modemanager = &g_1187ModeManager;

