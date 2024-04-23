//========= Copyright Valve Corporation, All rights reserved. ============//
//
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "env_light.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	LIGHT_LAUNCH_SPEED	1500

LINK_ENTITY_TO_CLASS( env_light, CDLight );

BEGIN_DATADESC( CDLight )

	DEFINE_FIELD( m_pOwner,			FIELD_CLASSPTR ),
	DEFINE_FIELD( m_nBounces,		FIELD_INTEGER ),
	DEFINE_FIELD( m_flTimeBurnOut,	FIELD_TIME ),
	DEFINE_KEYFIELD( m_flScale,		FIELD_FLOAT, "scale" ),
	DEFINE_KEYFIELD( m_flDuration,	FIELD_FLOAT, "duration" ),
	DEFINE_FIELD( m_flNextDamage,	FIELD_TIME ),
	DEFINE_FIELD( m_bFading,		FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bLight,			FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bPropFlare,		FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bInActiveList,	FIELD_BOOLEAN ),
	DEFINE_FIELD( m_pNextFlare,		FIELD_CLASSPTR ),
	
	//Input functions
	DEFINE_INPUTFUNC( FIELD_FLOAT, "Start", InputStart ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "Die", InputDie ),
	DEFINE_INPUTFUNC( FIELD_FLOAT, "Launch", InputLaunch),

	// Function Pointers
	DEFINE_FUNCTION( FlareThink ),

END_DATADESC()

//Data-tables
IMPLEMENT_SERVERCLASS_ST( CDLight, DT_DLight )
	SendPropFloat( SENDINFO( m_flTimeBurnOut ), 0,	SPROP_NOSCALE ),
	SendPropFloat( SENDINFO( m_flScale ), 0, SPROP_NOSCALE ),
	SendPropInt( SENDINFO( m_bLight ), 1, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_bPropFlare ), 1, SPROP_UNSIGNED ),
END_SEND_TABLE()

CDLight *CDLight::activeFlares = NULL;

Class_T CDLight::Classify( void )
{
	return CLASS_FLARE; 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CDLight::CDLight( void )
{
	m_flScale		= 1.0f;
	m_nBounces		= 0;
	m_bFading		= false;
	m_bLight		= true;
	m_flNextDamage	= gpGlobals->curtime;
	m_lifeState		= LIFE_ALIVE;
	m_iHealth		= 100;
	m_bPropFlare	= false;
	m_bInActiveList	= false;
	m_pNextFlare	= NULL;
}

CDLight::~CDLight()
{
	RemoveFromActiveFlares();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDLight::Precache( void )
{
	PrecacheModel( "models/dw/polygon.mdl" );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &restore - 
// Output : int
//-----------------------------------------------------------------------------
int CDLight::Restore( IRestore &restore )
{
	int result = BaseClass::Restore( restore );

	if ( m_spawnflags & SF_LIGHT_NO_DLIGHT )
	{
		m_bLight = false;
	}

	return result;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDLight::Spawn( void )
{
	Precache();

	SetModel( "models/dw/polygon.mdl" );

	UTIL_SetSize( this, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ) );

	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_SOLID );

	SetMoveType( MOVETYPE_NONE );
	SetFriction( 0.6f );
	SetGravity( UTIL_ScaleForGravity( 400 ) );
	m_flTimeBurnOut = gpGlobals->curtime + 30;

	AddEffects( EF_NOSHADOW|EF_NORECEIVESHADOW );

	if ( m_spawnflags & SF_LIGHT_NO_DLIGHT )
	{
		m_bLight = false;
	}

	if ( m_spawnflags & SF_LIGHT_INFINITE )
	{
		m_flTimeBurnOut = -1.0f;
	}

	if ( m_spawnflags & SF_DLIGHT_START_OFF )
	{
		AddEffects( EF_NODRAW );
	}

	AddFlag( FL_OBJECT );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDLight::Activate( void )
{
	BaseClass::Activate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
unsigned int CDLight::PhysicsSolidMaskForEntity( void ) const
{
	return MASK_NPCSOLID;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDLight::FlareThink( void )
{
	float	deltaTime = ( m_flTimeBurnOut - gpGlobals->curtime );

	if ( !m_bInActiveList && ( ( deltaTime > LIGHT_BLIND_TIME ) || ( m_flTimeBurnOut == -1.0f ) ) )
	{
		AddToActiveFlares();
	}

	if ( m_flTimeBurnOut != -1.0f )
	{

		// if flare is no longer bright, remove it from active flare list
		if ( m_bInActiveList && ( deltaTime <= LIGHT_BLIND_TIME ) )
		{
			RemoveFromActiveFlares();
		}

		//Burned out
		if ( m_flTimeBurnOut < gpGlobals->curtime )
		{
			UTIL_Remove( this );
			return;
		}
	}
	
	//Act differently underwater
	if ( GetWaterLevel() > 1 )
	{
		UTIL_Bubbles( GetAbsOrigin() + Vector( -2, -2, -2 ), GetAbsOrigin() + Vector( 2, 2, 2 ), 1 );
	}

	//Next update
	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDLight::Start( float lifeTime )
{
	if ( lifeTime > 0 )
	{
		m_flTimeBurnOut = gpGlobals->curtime + lifeTime;
	}
	else
	{
		m_flTimeBurnOut = -1.0f;
	}

	RemoveEffects( EF_NODRAW );

	SetThink( &CDLight::FlareThink );
	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDLight::Die( float fadeTime )
{
	m_flTimeBurnOut = gpGlobals->curtime + fadeTime;

	SetThink( &CDLight::FlareThink );
	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDLight::Launch( const Vector &direction, float speed )
{
	// Make sure we're visible
	if ( m_spawnflags & SF_LIGHT_INFINITE )
	{
		Start( -1 );
	}
	else
	{
		Start( 8.0f );
	}

	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );

	// Punch our velocity towards our facing
	SetAbsVelocity( direction * speed );

	SetGravity( 1.0f );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CDLight::InputStart( inputdata_t &inputdata )
{
	Start( inputdata.value.Float() );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CDLight::InputDie( inputdata_t &inputdata )
{
	Die( inputdata.value.Float() );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &inputdata - 
//-----------------------------------------------------------------------------
void CDLight::InputLaunch( inputdata_t &inputdata )
{
	Vector	direction;
	AngleVectors( GetAbsAngles(), &direction );

	float	speed = inputdata.value.Float();

	if ( speed == 0 )
	{
		speed = LIGHT_LAUNCH_SPEED;
	}

	Launch( direction, speed );
}

//-----------------------------------------------------------------------------
// Purpose: Removes flare from active flare list
//-----------------------------------------------------------------------------
void CDLight::RemoveFromActiveFlares( void )
{
	CDLight *pFlare;
	CDLight *pPrevFlare;

	if ( !m_bInActiveList )
		return;

	pPrevFlare = NULL;
	for( pFlare = CDLight::activeFlares; pFlare != NULL; pFlare = pFlare->m_pNextFlare )
	{
		if ( pFlare == this )
		{
			if ( pPrevFlare )
			{
				pPrevFlare->m_pNextFlare = m_pNextFlare;
			}
			else
			{
				activeFlares = m_pNextFlare;
			}
			break;
		}
		pPrevFlare = pFlare;
	}

	m_pNextFlare = NULL;
	m_bInActiveList = false;
}

//-----------------------------------------------------------------------------
// Purpose: Adds flare to active flare list
//-----------------------------------------------------------------------------
void CDLight::AddToActiveFlares( void )
{
	if ( !m_bInActiveList )
	{
		m_pNextFlare = CDLight::activeFlares;
		CDLight::activeFlares = this;
		m_bInActiveList = true;
	}
}
