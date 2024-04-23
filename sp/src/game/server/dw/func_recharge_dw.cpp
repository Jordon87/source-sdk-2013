//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
/*

===== h_battery.cpp ========================================================

  battery-related code

*/

#include "cbase.h"
#include "gamerules.h"
#include "player.h"
#include "engine/IEngineSound.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar	sk_suitcharger_dw( "sk_suitcharger_dw","0" );

class CRechargeDW : public CBaseToggle
{
public:
	DECLARE_CLASS( CRechargeDW, CBaseToggle );

	void Spawn( );
	bool CreateVPhysics();
	int DrawDebugTextOverlays(void);
	void Off(void);
	void Recharge(void);
	bool KeyValue( const char *szKeyName, const char *szValue );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int	ObjectCaps( void ) { return (BaseClass::ObjectCaps() | FCAP_CONTINUOUS_USE); }

private:
	void InputRecharge( inputdata_t &inputdata );
	
	float MaxJuice() const;
	void UpdateJuice( int newJuice );

	DECLARE_DATADESC();

	float	m_flNextCharge; 
	int		m_iReactivate ; // DeathMatch Delay until reactvated
	int		m_iJuice;
	int		m_iOn;			// 0 = off, 1 = startup, 2 = going
	float   m_flSoundTime;
	
	int		m_nState;
	
	COutputFloat m_OutRemainingCharge;
	COutputEvent m_OnHalfEmpty;
	COutputEvent m_OnEmpty;
	COutputEvent m_OnFull;
	COutputEvent m_OnPlayerUse;
};

BEGIN_DATADESC( CRechargeDW )

	DEFINE_FIELD( m_flNextCharge, FIELD_TIME ),
	DEFINE_FIELD( m_iReactivate, FIELD_INTEGER),
	DEFINE_FIELD( m_iJuice, FIELD_INTEGER),
	DEFINE_FIELD( m_iOn, FIELD_INTEGER),
	DEFINE_FIELD( m_flSoundTime, FIELD_TIME ),
	DEFINE_FIELD( m_nState, FIELD_INTEGER ),

	// Function Pointers
	DEFINE_FUNCTION( Off ),
	DEFINE_FUNCTION( Recharge ),

	DEFINE_OUTPUT(m_OutRemainingCharge, "OutRemainingCharge"),
	DEFINE_OUTPUT(m_OnHalfEmpty, "OnHalfEmpty" ),
	DEFINE_OUTPUT(m_OnEmpty, "OnEmpty" ),
	DEFINE_OUTPUT(m_OnFull, "OnFull" ),
	DEFINE_OUTPUT(m_OnPlayerUse, "OnPlayerUse" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Recharge", InputRecharge ),
	
END_DATADESC()


LINK_ENTITY_TO_CLASS(func_recharge_dw, CRechargeDW);


bool CRechargeDW::KeyValue( const char *szKeyName, const char *szValue )
{
	if (	FStrEq(szKeyName, "style") ||
				FStrEq(szKeyName, "height") ||
				FStrEq(szKeyName, "value1") ||
				FStrEq(szKeyName, "value2") ||
				FStrEq(szKeyName, "value3"))
	{
	}
	else if (FStrEq(szKeyName, "dmdelay"))
	{
		m_iReactivate = atoi(szValue);
	}
	else
	{
		return BaseClass::KeyValue( szKeyName, szValue );
	}

	return true;
}

void CRechargeDW::Spawn()
{
	Precache( );

	SetSolid( SOLID_BSP );
	SetMoveType( MOVETYPE_PUSH );

	SetModel( STRING( GetModelName() ) );

	UpdateJuice( MaxJuice() );

	m_nState = 0;			

	CreateVPhysics();
}

bool CRechargeDW::CreateVPhysics()
{
	VPhysicsInitStatic();
	return true;
}

int CRechargeDW::DrawDebugTextOverlays(void) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		char tempstr[512];
		Q_snprintf(tempstr,sizeof(tempstr),"Charge left: %i", m_iJuice );
		EntityText(text_offset,tempstr,0);
		text_offset++;
	}
	return text_offset;
}


//-----------------------------------------------------------------------------
// Max juice for recharger
//-----------------------------------------------------------------------------
float CRechargeDW::MaxJuice()	const
{
	return sk_suitcharger_dw.GetFloat();
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : newJuice - 
//-----------------------------------------------------------------------------
void CRechargeDW::UpdateJuice( int newJuice )
{
	bool reduced = newJuice < m_iJuice;
	if ( reduced )
	{
		// Fire 1/2 way output and/or empyt output
		int oneHalfJuice = (int)(MaxJuice() * 0.5f);
		if ( newJuice <= oneHalfJuice && m_iJuice > oneHalfJuice )
		{
			m_OnHalfEmpty.FireOutput( this, this );
		}

		if ( newJuice <= 0 )
		{
			m_OnEmpty.FireOutput( this, this );
		}
	}
	else if ( newJuice != m_iJuice &&
		newJuice == (int)MaxJuice() )
	{
		m_OnFull.FireOutput( this, this );
	}
	m_iJuice = newJuice;
}

void CRechargeDW::InputRecharge( inputdata_t &inputdata )
{
	Recharge();
}

void CRechargeDW::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ 
	// if it's not a player, ignore
	if ( !pActivator || !pActivator->IsPlayer() )
		return;

	// Only usable if you have the HEV suit on
	if ( !((CBasePlayer *)pActivator)->IsSuitEquipped() )
	{
		if (m_flSoundTime <= gpGlobals->curtime)
		{
			m_flSoundTime = gpGlobals->curtime + 0.62;
			EmitSound( "SuitRecharge.Deny" );
		}
		return;
	}

	// if there is no juice left, turn it off
	if (m_iJuice <= 0)
	{
		m_nState = 1;			
		Off();
	}

	// if the player doesn't have the suit, or there is no juice left, make the deny noise
	if ( m_iJuice <= 0 )
	{
		if (m_flSoundTime <= gpGlobals->curtime)
		{
			m_flSoundTime = gpGlobals->curtime + 0.62;
			EmitSound( "SuitRecharge.Deny" );
		}
		return;
	}

	SetNextThink( gpGlobals->curtime + 0.25 );
	SetThink(&CRechargeDW::Off);

	// Time to recharge yet?
	if (m_flNextCharge >= gpGlobals->curtime)
		return;

	// Make sure that we have a caller
	if (!pActivator)
		return;

	m_hActivator = pActivator;

	//only recharge the player

	if (!m_hActivator->IsPlayer() )
		return;
	
	// Play the on sound or the looping charging sound
	if (!m_iOn)
	{
		m_iOn++;
		EmitSound( "SuitRecharge.Start" );
		m_flSoundTime = 0.56 + gpGlobals->curtime;

		m_OnPlayerUse.FireOutput( pActivator, this );
	}

	if ((m_iOn == 1) && (m_flSoundTime <= gpGlobals->curtime))
	{
		m_iOn++;
		CPASAttenuationFilter filter( this, "SuitRecharge.ChargingLoop" );
		filter.MakeReliable();
		EmitSound( filter, entindex(), "SuitRecharge.ChargingLoop" );
	}

	CBasePlayer *pl = (CBasePlayer *) m_hActivator.Get();

	// charge the player
	int nMaxArmor = 100;
	int nIncrementArmor = 1;

	if (pl->ArmorValue() < nMaxArmor)
	{
		UpdateJuice( m_iJuice - nIncrementArmor );
		pl->IncrementArmorValue( nIncrementArmor, nMaxArmor );
	}

	// Send the output.
	float flRemaining = m_iJuice / MaxJuice();
	m_OutRemainingCharge.Set(flRemaining, pActivator, this);

	// govern the rate of charge
	m_flNextCharge = gpGlobals->curtime + 0.1;
}

void CRechargeDW::Recharge(void)
{
	UpdateJuice( MaxJuice() );
	m_nState = 0;			
	SetThink( &CRechargeDW::SUB_DoNothing );
}

void CRechargeDW::Off(void)
{
	// Stop looping sound.
	if (m_iOn > 1)
	{
		StopSound( "SuitRecharge.ChargingLoop" );
	}

	m_iOn = 0;

	if ((!m_iJuice) &&  ( ( m_iReactivate = g_pGameRules->FlHEVChargerRechargeTime() ) > 0) )
	{
		SetNextThink( gpGlobals->curtime + m_iReactivate );
		SetThink(&CRechargeDW::Recharge);
	}
	else
	{
		SetThink( NULL );
	}
}

