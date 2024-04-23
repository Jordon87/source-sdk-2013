//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ENV_LIGHT_H
#define ENV_LIGHT_H
#ifdef _WIN32
#pragma once
#endif

#define	SF_LIGHT_NO_DLIGHT	0x00000001
#define	SF_LIGHT_NO_SMOKE	0x00000002
#define	SF_LIGHT_INFINITE	0x00000004
#define	SF_DLIGHT_START_OFF	0x00000008

#define	LIGHT_DURATION		30.0f
#define LIGHT_DECAY_TIME	10.0f
#define LIGHT_BLIND_TIME	6.0f

//---------------------
// Flare
//---------------------

class CDLight : public CBaseCombatCharacter
{
public:
	DECLARE_CLASS( CDLight, CBaseCombatCharacter );

	CDLight();
	~CDLight();

	virtual unsigned int PhysicsSolidMaskForEntity( void ) const;

	void	Spawn( void );
	void	Precache( void );
	int		Restore( IRestore &restore );
	void	Activate( void );

	void	Start( float lifeTime );
	void	Die( float fadeTime );
	void	Launch( const Vector &direction, float speed );

	Class_T Classify( void );

	void	FlareThink( void );

	void	InputStart( inputdata_t &inputdata );
	void	InputDie( inputdata_t &inputdata );
	void	InputLaunch( inputdata_t &inputdata );

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	static CDLight*activeFlares;

	CBaseEntity *m_pOwner;
	int			m_nBounces;			// how many times has this flare bounced?
	CNetworkVar( float, m_flTimeBurnOut );	// when will the flare burn out?
	CNetworkVar( float, m_flScale );
	float		m_flDuration;
	float		m_flNextDamage;
	
	bool		m_bFading;
	CNetworkVar( bool, m_bLight );
	CNetworkVar( bool, m_bPropFlare );

	bool		m_bInActiveList;
	CDLight *	m_pNextFlare;

	void		RemoveFromActiveFlares( void );
	void		AddToActiveFlares( void );
};

#endif // ENV_LIGHT_H

