//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Base combat character with no AI
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "ammodef.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Return a pointer to the Ammo at the Index passed in
//-----------------------------------------------------------------------------
Ammo_t *CAmmoDef::GetAmmoOfIndex(int nAmmoIndex)
{
	if ( nAmmoIndex >= m_nAmmoIndex )
		return NULL;

	return &m_AmmoType[ nAmmoIndex ];
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int CAmmoDef::Index(const char *psz)
{
	int i;

	if (!psz)
		return -1;

	for (i = 1; i < m_nAmmoIndex; i++)
	{
		if (stricmp( psz, m_AmmoType[i].pName ) == 0)
			return i;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int	CAmmoDef::PlrDamage(int nAmmoIndex)
{
	if ( nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex )
		return 0;

	if ( m_AmmoType[nAmmoIndex].pPlrDmg == USE_CVAR )
	{
		if ( m_AmmoType[nAmmoIndex].pPlrDmgCVar )
		{
			return m_AmmoType[nAmmoIndex].pPlrDmgCVar->GetFloat();
		}

		return 0;
	}
	else
	{
		return m_AmmoType[nAmmoIndex].pPlrDmg;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int	CAmmoDef::NPCDamage(int nAmmoIndex)
{
	if ( nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex )
		return 0;

	if ( m_AmmoType[nAmmoIndex].pNPCDmg == USE_CVAR )
	{
		if ( m_AmmoType[nAmmoIndex].pNPCDmgCVar )
		{
			return m_AmmoType[nAmmoIndex].pNPCDmgCVar->GetFloat();
		}

		return 0;
	}
	else
	{
		return m_AmmoType[nAmmoIndex].pNPCDmg;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int	CAmmoDef::MaxCarry(int nAmmoIndex)
{
	if ( nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex )
		return 0;

	if ( m_AmmoType[nAmmoIndex].pMaxCarry == USE_CVAR )
	{
		if ( m_AmmoType[nAmmoIndex].pMaxCarryCVar )
			return m_AmmoType[nAmmoIndex].pMaxCarryCVar->GetFloat();

		return 0;
	}
	else
	{
		return m_AmmoType[nAmmoIndex].pMaxCarry;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int	CAmmoDef::DamageType(int nAmmoIndex)
{
	if (nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex)
		return 0;

	return m_AmmoType[nAmmoIndex].nDamageType;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CAmmoDef::Flags(int nAmmoIndex)
{
	if (nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex)
		return 0;

	return m_AmmoType[nAmmoIndex].nFlags;
}


//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int	CAmmoDef::MinSplashSize(int nAmmoIndex)
{
	if (nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex)
		return 4;

	return m_AmmoType[nAmmoIndex].nMinSplashSize;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int	CAmmoDef::MaxSplashSize(int nAmmoIndex)
{
	if (nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex)
		return 8;

	return m_AmmoType[nAmmoIndex].nMaxSplashSize;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
int	CAmmoDef::TracerType(int nAmmoIndex)
{
	if (nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex)
		return 0;

	return m_AmmoType[nAmmoIndex].eTracerType;
}

float CAmmoDef::DamageForce(int nAmmoIndex)
{
	if ( nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex )
		return 0;

	return m_AmmoType[nAmmoIndex].physicsForceImpulse;
}

//-----------------------------------------------------------------------------
// Purpose: Create an Ammo type with the name, decal, and tracer.
// Does not increment m_nAmmoIndex because the functions below do so and 
//  are the only entry point.
//-----------------------------------------------------------------------------
#if defined ( HUMANERROR_DLL ) || defined ( HUMANERROR_CLIENT_DLL )
bool CAmmoDef::AddAmmoType(char const* name, int damageType, int tracerType, int nFlags, int minSplashSize, int maxSplashSize, wchar_t *icon, wchar_t *AmmoTitle)
#else
bool CAmmoDef::AddAmmoType(char const* name, int damageType, int tracerType, int nFlags, int minSplashSize, int maxSplashSize )
#endif
{
	if (m_nAmmoIndex == MAX_AMMO_TYPES)
		return false;

	int len = strlen(name);
	m_AmmoType[m_nAmmoIndex].pName = new char[len+1];
	Q_strncpy(m_AmmoType[m_nAmmoIndex].pName, name,len+1);
	m_AmmoType[m_nAmmoIndex].nDamageType	= damageType;
	m_AmmoType[m_nAmmoIndex].eTracerType	= tracerType;
	m_AmmoType[m_nAmmoIndex].nMinSplashSize	= minSplashSize;
	m_AmmoType[m_nAmmoIndex].nMaxSplashSize	= maxSplashSize;
	m_AmmoType[m_nAmmoIndex].nFlags	= nFlags;

#if defined ( HUMANERROR_DLL ) || defined ( HUMANERROR_CLIENT_DLL )
	wcsncpy(m_AmmoType[m_nAmmoIndex].m_Icon, icon, sizeof(m_AmmoType[m_nAmmoIndex].m_Icon) / sizeof(wchar_t));
	m_AmmoType[m_nAmmoIndex].m_Icon[(sizeof(m_AmmoType[m_nAmmoIndex].m_Icon) / sizeof(wchar_t)) - 1] = 0;

	wcsncpy(m_AmmoType[m_nAmmoIndex].m_Title, AmmoTitle, sizeof(m_AmmoType[m_nAmmoIndex].m_Title) / sizeof(wchar_t));
	m_AmmoType[m_nAmmoIndex].m_Title[(sizeof(m_AmmoType[m_nAmmoIndex].m_Title) / sizeof(wchar_t)) - 1] = 0;
#endif

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Add an ammo type with it's damage & carrying capability specified via cvars
//-----------------------------------------------------------------------------
#if defined ( HUMANERROR_DLL ) || defined ( HUMANERROR_CLIENT_DLL )
void CAmmoDef::AddAmmoType(char const* name, int damageType, int tracerType,
	char const* plr_cvar, char const* npc_cvar, char const* carry_cvar, 
	float physicsForceImpulse, int nFlags, wchar_t *icon, wchar_t *AmmoTitle, int minSplashSize, int maxSplashSize )
#else
void CAmmoDef::AddAmmoType(char const* name, int damageType, int tracerType, 
	char const* plr_cvar, char const* npc_cvar, char const* carry_cvar, 
	float physicsForceImpulse, int nFlags, int minSplashSize, int maxSplashSize)
#endif
{
#if defined ( HUMANERROR_DLL ) || defined ( HUMANERROR_CLIENT_DLL )
	if ( AddAmmoType( name, damageType, tracerType, nFlags, minSplashSize, maxSplashSize, icon, AmmoTitle ) == false )
		return;
#else
	if ( AddAmmoType( name, damageType, tracerType, nFlags, minSplashSize, maxSplashSize ) == false )
		return;
#endif

	if (plr_cvar)
	{
		m_AmmoType[m_nAmmoIndex].pPlrDmgCVar	= cvar->FindVar(plr_cvar);
		if (!m_AmmoType[m_nAmmoIndex].pPlrDmgCVar)
		{
			Msg("ERROR: Ammo (%s) found no CVar named (%s)\n",name,plr_cvar);
		}
		m_AmmoType[m_nAmmoIndex].pPlrDmg = USE_CVAR;
	}
	if (npc_cvar)
	{
		m_AmmoType[m_nAmmoIndex].pNPCDmgCVar	= cvar->FindVar(npc_cvar);
		if (!m_AmmoType[m_nAmmoIndex].pNPCDmgCVar)
		{
			Msg("ERROR: Ammo (%s) found no CVar named (%s)\n",name,npc_cvar);
		}
		m_AmmoType[m_nAmmoIndex].pNPCDmg = USE_CVAR;
	}
	if (carry_cvar)
	{
		m_AmmoType[m_nAmmoIndex].pMaxCarryCVar= cvar->FindVar(carry_cvar);
		if (!m_AmmoType[m_nAmmoIndex].pMaxCarryCVar)
		{
			Msg("ERROR: Ammo (%s) found no CVar named (%s)\n",name,carry_cvar);
		}
		m_AmmoType[m_nAmmoIndex].pMaxCarry = USE_CVAR;
	}
	m_AmmoType[m_nAmmoIndex].physicsForceImpulse = physicsForceImpulse;
	m_nAmmoIndex++;
}

//-----------------------------------------------------------------------------
// Purpose: Add an ammo type with it's damage & carrying capability specified via integers
//-----------------------------------------------------------------------------
#if defined ( HUMANERROR_DLL ) || defined ( HUMANERROR_CLIENT_DLL )
void CAmmoDef::AddAmmoType(char const* name, int damageType, int tracerType,
	int plr_dmg, int npc_dmg, int carry, float physicsForceImpulse, 
	int nFlags, wchar_t *icon, wchar_t *AmmoTitle, int minSplashSize, int maxSplashSize )
#else
void CAmmoDef::AddAmmoType(char const* name, int damageType, int tracerType, 
	int plr_dmg, int npc_dmg, int carry, float physicsForceImpulse, 
	int nFlags, int minSplashSize, int maxSplashSize )
#endif
{
#if defined ( HUMANERROR_DLL ) || defined ( HUMANERROR_CLIENT_DLL )
	if ( AddAmmoType( name, damageType, tracerType, nFlags, minSplashSize, maxSplashSize, icon, AmmoTitle ) == false )
		return;
#else
	if ( AddAmmoType( name, damageType, tracerType, nFlags, minSplashSize, maxSplashSize ) == false )
		return;
#endif

	m_AmmoType[m_nAmmoIndex].pPlrDmg = plr_dmg;
	m_AmmoType[m_nAmmoIndex].pNPCDmg = npc_dmg;
	m_AmmoType[m_nAmmoIndex].pMaxCarry = carry;
	m_AmmoType[m_nAmmoIndex].physicsForceImpulse = physicsForceImpulse;

	m_nAmmoIndex++;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
// Input  :
// Output :
//-----------------------------------------------------------------------------
CAmmoDef::CAmmoDef(void)
{
	// Start with an index of 1.  Client assumes 0 is an invalid ammo type
	m_nAmmoIndex = 1;
	memset( m_AmmoType, 0, sizeof( m_AmmoType ) );
}

CAmmoDef::~CAmmoDef( void )
{
	for ( int i = 1; i < MAX_AMMO_TYPES; i++ )
	{
		delete[] m_AmmoType[ i ].pName;
	}
}


