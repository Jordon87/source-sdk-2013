//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Game rules for Hoe.
//
//=============================================================================//

#ifndef HOE_GAMERULES_H
#define HOE_GAMERULES_H
#ifdef _WIN32
#pragma once
#endif

#include "hl2_gamerules.h"
#include "hoe_shareddefs.h"

#ifdef CLIENT_DLL
	#define CHeartofEvil C_HeartofEvil
	#define CHeartofEvilProxy C_HeartofEvilProxy
#endif


class CHeartofEvilProxy : public CHalfLife2Proxy
{
public:
	DECLARE_CLASS(CHeartofEvilProxy, CHalfLife2Proxy);
	DECLARE_NETWORKCLASS();
};


class CHeartofEvil : public CHalfLife2
{
public:
	DECLARE_CLASS(CHeartofEvil, CHalfLife2);

private:

#ifdef CLIENT_DLL

	DECLARE_CLIENTCLASS_NOBASE(); // This makes datatables able to access our private vars.

#else

	DECLARE_SERVERCLASS_NOBASE(); // This makes datatables able to access our private vars.

	CHeartofEvil();
	virtual ~CHeartofEvil() {}

	virtual bool			ClientCommand( CBaseEntity *pEdict, const CCommand &args );

	virtual void			InitDefaultAIRelationships(void);
	virtual const char*		AIClassText(int classType);
	virtual const char *GetGameDescription( void ) { return "Heart of Evil"; }

#endif
};


//-----------------------------------------------------------------------------
// Gets us at the Heart of Evil game rules
//-----------------------------------------------------------------------------
inline CHeartofEvil* HoeGameRules()
{
#if !defined( HOE_DLL ) && !defined( HOE_CLIENT_DLL )
	Assert( 0 );	// g_pGameRules is NOT an instance of CHeartofEvil and bad things happen
#endif

	return static_cast<CHeartofEvil*>(g_pGameRules);
}



#endif // HOE_GAMERULES_H
