//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Game rules for Half-Life 2.
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_GAMERULES_H
#define ELEVENEIGHTYSEVEN_GAMERULES_H
#ifdef _WIN32
#pragma once
#endif

#include "hl2_gamerules.h"

#ifdef CLIENT_DLL
	#define CElevenEightySeven C_ElevenEightySeven
	#define CElevenEightySevenProxy C_ElevenEightySevenProxy
#endif

class CElevenEightySevenProxy : public CGameRulesProxy
{
public:
	DECLARE_CLASS(CElevenEightySevenProxy, CGameRulesProxy);
	DECLARE_NETWORKCLASS();
};


class CElevenEightySeven : public CHalfLife2
{
public:
	DECLARE_CLASS(CElevenEightySeven, CHalfLife2);

#ifndef CLIENT_DLL
#endif

	// Easter eggs
	int					GetEasterEggs(void) { return m_nEasterEggs; }
	void				SetEasterEggs(int value) { m_nEasterEggs = value; }
	void				IncrementEasterEggs(int value) { m_nEasterEggs += value; }
	void				DecrementEasterEggs(int value) { m_nEasterEggs -= value; }

#if defined ( ROGUETRAIN_DLL ) || defined ( ROGUETRAIN_CLIENT_DLL )
	virtual void		LevelInitPreEntity();
#ifndef CLIENT_DLL
	bool				IsRogueTrain() const { return m_bIsRogueTrain; }
#endif
#endif // defined ( ROGUETRAIN_DLL ) || defined ( ROGUETRAIN_CLIENT_DLL )

private:

	CNetworkVar(int, m_nEasterEggs);

#if defined ( ROGUETRAIN_DLL )
#ifndef CLIENT_DLL
	bool m_bIsRogueTrain;
#endif
#endif

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.

	virtual ~CElevenEightySeven() {}


	virtual bool			ClientCommand( CBaseEntity *pEdict, const CCommand &args );

	virtual void			InitDefaultAIRelationships( void );
	virtual const char*		AIClassText(int classType);
	virtual const char*		GetGameDescription( void ) { return "ElevenEightySeven"; }
#endif

};


//-----------------------------------------------------------------------------
// Gets us at the 1187 game rules
//-----------------------------------------------------------------------------
inline CElevenEightySeven * ElevenEightySevenGameRules()
{
#if ( !defined( ELEVENEIGHTYSEVEN_DLL ) && !defined( ELEVENEIGHTYSEVEN_CLIENT_DLL ) )
	Assert( 0 );	// g_pGameRules is NOT an instance of C1187 and bad things happen
#endif

	return static_cast<CElevenEightySeven*>(g_pGameRules);
}





#endif // HL2_GAMERULES_H
