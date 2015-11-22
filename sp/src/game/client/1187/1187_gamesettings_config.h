//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_GAMESETTINGS_CONFIG_H
#define ELEVENEIGHTYSEVEN_GAMESETTINGS_CONFIG_H

#ifdef _WIN32
#pragma once
#endif

struct Eleven87GameSettings_t
{
	byte	m_eggcount;
	byte	m_viewbobbing : 1;
	byte	m_viewrealism : 1;
	byte	m_ironblur : 1;
	byte	m_sprintblur : 1;
	byte	m_hudstyle : 1;
};

#endif // ELEVENEIGHTYSEVEN_GAMESETTINGS_CONFIG_H