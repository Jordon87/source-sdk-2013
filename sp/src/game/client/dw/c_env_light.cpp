//========= Copyright Valve Corporation, All rights reserved. ============//
//
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "iefx.h"
#include "dlight.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_DLight : public C_BaseCombatCharacter, CSimpleEmitter
{
public:
	DECLARE_CLASS( C_DLight, C_BaseCombatCharacter );
	DECLARE_CLIENTCLASS();

	C_DLight();

	void	OnDataChanged( DataUpdateType_t updateType );
	void	Update( float timeDelta );
	void	NotifyShouldTransmit( ShouldTransmitState_t state );

	float	m_flTimeBurnOut;
	float	m_flScale;
	bool	m_bLight;
	bool	m_bPropFlare;
	pixelvis_handle_t m_queryHandle;


private:
	C_DLight( const C_DLight & );
	TimedEvent	m_teSmokeSpawn;

	int		m_iAttachment;
	
	SimpleParticle	*m_pParticle[2];
};

IMPLEMENT_CLIENTCLASS_DT( C_DLight, DT_DLight, CDLight )
	RecvPropFloat( RECVINFO( m_flTimeBurnOut ) ),
	RecvPropFloat( RECVINFO( m_flScale ) ),
	RecvPropInt( RECVINFO( m_bLight ) ),
	RecvPropInt( RECVINFO( m_bPropFlare ) ),
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
C_DLight::C_DLight() : CSimpleEmitter( "C_DLight" )
{
	m_pParticle[0]	= NULL;
	m_pParticle[1]	= NULL;
	m_flTimeBurnOut	= 0.0f;

	m_bLight		= true;
	m_bPropFlare	= false;

	SetDynamicallyAllocated( false );
	m_queryHandle = 0;

	m_iAttachment = -1;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : state - 
//-----------------------------------------------------------------------------
void C_DLight::NotifyShouldTransmit( ShouldTransmitState_t state )
{
	if ( state == SHOULDTRANSMIT_END )
	{
		AddEffects( EF_NODRAW );
	}
	else if ( state == SHOULDTRANSMIT_START )
	{
		RemoveEffects( EF_NODRAW );
	}

	BaseClass::NotifyShouldTransmit( state );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : bool - 
//-----------------------------------------------------------------------------
void C_DLight::OnDataChanged( DataUpdateType_t updateType )
{
	if ( updateType == DATA_UPDATE_CREATED )
	{
		SetSortOrigin( GetAbsOrigin() );
	}

	BaseClass::OnDataChanged( updateType );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : timeDelta - 
//-----------------------------------------------------------------------------
void C_DLight::Update( float timeDelta )
{
	if ( !IsVisible() )
		return;

	CSimpleEmitter::Update( timeDelta );

	//Don't do this if the console is down
	if ( timeDelta <= 0.0f )
		return;

	//Check for LOS
	pixelvis_queryparams_t params;
	params.Init(GetAbsOrigin());
	params.proxySize = 8.0f; // Inches
	
	float visible = PixelVisibility_FractionVisible( params, &m_queryHandle );

	float	fColor;
#ifdef HL2_CLIENT_DLL
	float	baseScale = m_flScale;
#else
	// NOTE!!!  This is bigger by a factor of 1.2 to deal with fixing a bug from HL2.  See dlight_t.h
	float	baseScale = m_flScale * 1.2f;
#endif

	//Account for fading out
	if ( ( m_flTimeBurnOut != -1.0f ) && ( ( m_flTimeBurnOut - gpGlobals->curtime ) <= 10.0f ) )
	{
		baseScale *= ( ( m_flTimeBurnOut - gpGlobals->curtime ) / 10.0f );
	}

	bool bVisible = (baseScale < 0.01f || visible == 0.0f) ? false : true;
	//Clamp the scale if vanished
	if ( !bVisible )
	{
		if ( m_pParticle[0] != NULL )
		{	
			m_pParticle[0]->m_flDieTime		= gpGlobals->curtime;
			m_pParticle[0]->m_uchStartSize	= m_pParticle[0]->m_uchEndSize = 0;
			m_pParticle[0]->m_uchColor[0]	= 0;
			m_pParticle[0]->m_uchColor[1]	= 0;
			m_pParticle[0]->m_uchColor[2]	= 0;
		}

		if ( m_pParticle[1] != NULL )
		{	
			m_pParticle[1]->m_flDieTime		= gpGlobals->curtime;
			m_pParticle[1]->m_uchStartSize	= m_pParticle[1]->m_uchEndSize = 0;
			m_pParticle[1]->m_uchColor[0]	= 0;
			m_pParticle[1]->m_uchColor[1]	= 0;
			m_pParticle[1]->m_uchColor[2]	= 0;
		}
	}

	if ( baseScale < 0.01f )
		return;
	//
	// Dynamic light
	//

	if ( m_bLight )
	{
		dlight_t *dl= effects->CL_AllocDlight( index );

		

		if ( m_bPropFlare == false )
		{
			dl->origin	= GetAbsOrigin();
			dl->color.r = 255;
			dl->die		= gpGlobals->curtime + 0.1f;

			dl->radius	= baseScale * random->RandomFloat( 110.0f, 128.0f );
			dl->color.g = dl->color.b = random->RandomInt( 32, 64 );
		}
		else
		{
			if ( m_iAttachment == -1 )
			{
				m_iAttachment =  LookupAttachment( "fuse" );
			}

			if ( m_iAttachment != -1 )
			{
				Vector effect_origin;
				QAngle effect_angles;
				
				GetAttachment( m_iAttachment, effect_origin, effect_angles );

				//Raise the light a little bit away from the flare so it lights it up better.
				dl->origin	= effect_origin + Vector( 0, 0, 4 );
				dl->color.r = 255;
				dl->die		= gpGlobals->curtime + 0.1f;

				dl->radius	= baseScale * random->RandomFloat( 245.0f, 256.0f );
				dl->color.g = dl->color.b = random->RandomInt( 95, 128 );
		
				dlight_t *el= effects->CL_AllocElight( index );

				el->origin	= effect_origin;
				el->color.r = 255;
				el->color.g = dl->color.b = random->RandomInt( 95, 128 );
				el->radius	= baseScale * random->RandomFloat( 260.0f, 290.0f );
				el->die		= gpGlobals->curtime + 0.1f;
			}
		}
	}

	//
	// Smoke
	//

	if ( !bVisible )
		return;

	//
	// Outer glow
	//

	Vector	offset;
	
	//Cause the base of the effect to shake
	offset.Random( -0.5f * baseScale, 0.5f * baseScale );
	offset += GetAbsOrigin();

	if ( m_pParticle[0] != NULL )
	{
		m_pParticle[0]->m_Pos			= offset;
		m_pParticle[0]->m_flLifetime	= 0.0f;
		m_pParticle[0]->m_flDieTime		= 2.0f;
		
		m_pParticle[0]->m_vecVelocity.Init();

		fColor = random->RandomInt( 100.0f, 128.0f ) * visible;

		m_pParticle[0]->m_uchColor[0]	= fColor;
		m_pParticle[0]->m_uchColor[1]	= fColor;
		m_pParticle[0]->m_uchColor[2]	= fColor;
		m_pParticle[0]->m_uchStartAlpha	= fColor;
		m_pParticle[0]->m_uchEndAlpha	= fColor;
		m_pParticle[0]->m_uchStartSize	= baseScale * (float) random->RandomInt( 32, 48 );
		m_pParticle[0]->m_uchEndSize	= m_pParticle[0]->m_uchStartSize;
		m_pParticle[0]->m_flRollDelta	= 0.0f;
		
		if ( random->RandomInt( 0, 4 ) == 3 )
		{
			m_pParticle[0]->m_flRoll	+= random->RandomInt( 2, 8 );
		}
	}

	//
	// Inner core
	//

	//Cause the base of the effect to shake
	offset.Random( -1.0f * baseScale, 1.0f * baseScale );
	offset += GetAbsOrigin();

	if ( m_pParticle[1] != NULL )
	{
		m_pParticle[1]->m_Pos			= offset;
		m_pParticle[1]->m_flLifetime	= 0.0f;
		m_pParticle[1]->m_flDieTime		= 2.0f;
		
		m_pParticle[1]->m_vecVelocity.Init();

		fColor = 255 * visible;

		m_pParticle[1]->m_uchColor[0]	= fColor;
		m_pParticle[1]->m_uchColor[1]	= fColor;
		m_pParticle[1]->m_uchColor[2]	= fColor;
		m_pParticle[1]->m_uchStartAlpha	= fColor;
		m_pParticle[1]->m_uchEndAlpha	= fColor;
		m_pParticle[1]->m_uchStartSize	= baseScale * (float) random->RandomInt( 2, 4 );
		m_pParticle[1]->m_uchEndSize	= m_pParticle[0]->m_uchStartSize;
		m_pParticle[1]->m_flRoll		= random->RandomInt( 0, 360 );
	}
}
