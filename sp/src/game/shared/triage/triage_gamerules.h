//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Game rules for Triage.
//
//=============================================================================//

#ifndef TRIAGE_GAMERULES_H
#define TRIAGE_GAMERULES_H
#ifdef _WIN32
#pragma once
#endif

#include "hl2_gamerules.h"

#ifdef CLIENT_DLL
	#define CTriage C_Triage
	#define CTriageProxy C_TriageProxy
#endif

class CTriageProxy : public CHalfLife2Proxy
{
public:
	DECLARE_CLASS(CTriageProxy, CHalfLife2Proxy);
	DECLARE_NETWORKCLASS();
};


class CTriage : public CHalfLife2
{
public:
	DECLARE_CLASS(CTriage, CHalfLife2);
	DECLARE_NETWORKCLASS_NOBASE();
private:

#ifdef CLIENT_DLL

	// DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else

	// DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.

	virtual bool			ClientCommand( CBaseEntity *pEdict, const CCommand &args );

	virtual const char *	GetGameDescription( void ) { return "Triage"; }
public:

private:
#endif
};


//-----------------------------------------------------------------------------
// Gets us at the Triage game rules
//-----------------------------------------------------------------------------
inline CTriage* TriageGameRules()
{
#if !defined ( TRIAGE_DLL ) && !defined ( TRIAGE_CLIENT_DLL )  
	Assert( 0 );	// g_pGameRules is NOT an instance of CTriage and bad things happen
#endif

	return static_cast<CTriage*>(g_pGameRules);
}



#endif // TRIAGE_GAMERULES_H
