//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_prop_vehicle.h"
#include "movevars_shared.h"
#include "view.h"
#include "c_baseplayer.h"
#include "c_te_effect_dispatch.h"
#include "fx.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar default_fov;

ConVar r_CarViewBlendTo( "r_CarViewBlendTo", "1", FCVAR_CHEAT );
ConVar r_CarViewBlendToScale( "r_CarViewBlendToScale", "0.03", FCVAR_CHEAT );
ConVar r_CarViewBlendToTime( "r_CarViewBlendToTime", "1.5", FCVAR_CHEAT );

#define CAR_DELTA_LENGTH_MAX	12.0f			// 1 foot
#define CAR_FRAMETIME_MIN		1e-6
#define CAR_HEADLIGHT_DISTANCE 1000

class C_PropCar : public C_PropVehicleDriveable
{

	DECLARE_CLASS(C_PropCar, C_PropVehicleDriveable);

public:

	DECLARE_CLIENTCLASS();
	DECLARE_INTERPOLATION();

	C_PropCar();
	~C_PropCar();

public:

	void Simulate(void);
	void UpdateViewAngles(C_BasePlayer* pLocalPlayer, CUserCmd* pCmd);
	void DampenEyePosition(Vector& vecVehicleEyePos, QAngle& vecVehicleEyeAngles);

	void OnEnteredVehicle(C_BasePlayer* pPlayer);

private:

	void DampenForwardMotion(Vector& vecVehicleEyePos, QAngle& vecVehicleEyeAngles, float flFrameTime);
	void DampenUpMotion(Vector& vecVehicleEyePos, QAngle& vecVehicleEyeAngles, float flFrameTime);
	void ComputePDControllerCoefficients(float* pCoefficientsOut, float flFrequency, float flDampening, float flDeltaTime);

private:

	Vector		m_vecLastEyePos;
	Vector		m_vecLastEyeTarget;
	Vector		m_vecEyeSpeed;
	Vector		m_vecTargetSpeed;

	float		m_flViewAngleDeltaTime;

	float		m_flJeepFOV;
	bool		m_bHeadlightIsOn;
};

IMPLEMENT_CLIENTCLASS_DT( C_PropCar, DT_PropCar, CPropCar )
END_RECV_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
C_PropCar::C_PropCar()
{
	m_vecEyeSpeed.Init();
	m_flViewAngleDeltaTime = 0.0f;
	
	ConVarRef r_CarFOV( "r_CarFOV" );
	m_ViewSmoothingData.flFOV = r_CarFOV.GetFloat();
}

//-----------------------------------------------------------------------------
// Purpose: Deconstructor
//-----------------------------------------------------------------------------
C_PropCar::~C_PropCar()
{
}

void C_PropCar::Simulate(void)
{
	BaseClass::Simulate();
}

//-----------------------------------------------------------------------------
// Purpose: Blend view angles.
//-----------------------------------------------------------------------------
void C_PropCar::UpdateViewAngles( C_BasePlayer *pLocalPlayer, CUserCmd *pCmd )
{
	int eyeAttachmentIndex = LookupAttachment("vehicle_driver_eyes");
	Vector vehicleEyeOrigin;
	QAngle vehicleEyeAngles;
	GetAttachmentLocal(eyeAttachmentIndex, vehicleEyeOrigin, vehicleEyeAngles);

	// Limit the yaw.
	float flAngleDiff = AngleDiff(pCmd->viewangles.y, vehicleEyeAngles.y);
	flAngleDiff = clamp(flAngleDiff, -70.0f, 70.0f);
	pCmd->viewangles.y = vehicleEyeAngles.y + flAngleDiff;

	// Limit the pitch -- don't let them look down into the empty pod!
	flAngleDiff = AngleDiff(pCmd->viewangles.x, vehicleEyeAngles.x);
	flAngleDiff = clamp(flAngleDiff, -10.0f, 8.0f);
	pCmd->viewangles.x = vehicleEyeAngles.x + flAngleDiff;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_PropCar::DampenEyePosition( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles )
{
#ifdef HL2_CLIENT_DLL
	// Get the frametime. (Check to see if enough time has passed to warrent dampening).
	float flFrameTime = gpGlobals->frametime;

	if ( flFrameTime < CAR_FRAMETIME_MIN )
	{
		vecVehicleEyePos = m_vecLastEyePos;
		DampenUpMotion( vecVehicleEyePos, vecVehicleEyeAngles, 0.0f );
		return;
	}

	// Keep static the sideways motion.
	// Dampen forward/backward motion.
	DampenForwardMotion( vecVehicleEyePos, vecVehicleEyeAngles, flFrameTime );

	// Blend up/down motion.
	DampenUpMotion( vecVehicleEyePos, vecVehicleEyeAngles, flFrameTime );
#endif
}


//-----------------------------------------------------------------------------
// Use the controller as follows:
// speed += ( pCoefficientsOut[0] * ( targetPos - currentPos ) + pCoefficientsOut[1] * ( targetSpeed - currentSpeed ) ) * flDeltaTime;
//-----------------------------------------------------------------------------
void C_PropCar::ComputePDControllerCoefficients( float *pCoefficientsOut,
												  float flFrequency, float flDampening,
												  float flDeltaTime )
{
	float flKs = 9.0f * flFrequency * flFrequency;
	float flKd = 4.5f * flFrequency * flDampening;

	float flScale = 1.0f / ( 1.0f + flKd * flDeltaTime + flKs * flDeltaTime * flDeltaTime );

	pCoefficientsOut[0] = flKs * flScale;
	pCoefficientsOut[1] = ( flKd + flKs * flDeltaTime ) * flScale;
}
 
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_PropCar::DampenForwardMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime )
{
	// vecVehicleEyePos = real eye position this frame

	// m_vecLastEyePos = eye position last frame
	// m_vecEyeSpeed = eye speed last frame
	// vecPredEyePos = predicted eye position this frame (assuming no acceleration - it will get that from the pd controller).
	// vecPredEyeSpeed = predicted eye speed
	Vector vecPredEyePos = m_vecLastEyePos + m_vecEyeSpeed * flFrameTime;
	Vector vecPredEyeSpeed = m_vecEyeSpeed;

	// m_vecLastEyeTarget = real eye position last frame (used for speed calculation).
	// Calculate the approximate speed based on the current vehicle eye position and the eye position last frame.
	Vector vecVehicleEyeSpeed = ( vecVehicleEyePos - m_vecLastEyeTarget ) / flFrameTime;
	m_vecLastEyeTarget = vecVehicleEyePos;
	if (vecVehicleEyeSpeed.Length() == 0.0)
		return;

	// Calculate the delta between the predicted eye position and speed and the current eye position and speed.
	Vector vecDeltaSpeed = vecVehicleEyeSpeed - vecPredEyeSpeed;
	Vector vecDeltaPos = vecVehicleEyePos - vecPredEyePos;

	// Forward vector.
	Vector vecForward;
	AngleVectors( vecVehicleEyeAngles, &vecForward );

	float flDeltaLength = vecDeltaPos.Length();
	if ( flDeltaLength > CAR_DELTA_LENGTH_MAX )
	{
		// Clamp.
		float flDelta = flDeltaLength - CAR_DELTA_LENGTH_MAX;
		if ( flDelta > 40.0f )
		{
			// This part is a bit of a hack to get rid of large deltas (at level load, etc.).
			m_vecLastEyePos = vecVehicleEyePos;
			m_vecEyeSpeed = vecVehicleEyeSpeed;
		}
		else
		{
			// Position clamp.
			float flRatio = CAR_DELTA_LENGTH_MAX / flDeltaLength;
			vecDeltaPos *= flRatio;
			Vector vecForwardOffset = vecForward * ( vecForward.Dot( vecDeltaPos ) );
			vecVehicleEyePos -= vecForwardOffset;
			m_vecLastEyePos = vecVehicleEyePos;

			// Speed clamp.
			vecDeltaSpeed *= flRatio;
			float flCoefficients[2];
			ComputePDControllerCoefficients( flCoefficients, r_CarViewDampenFreq.GetFloat(), r_CarViewDampenDamp.GetFloat(), flFrameTime );
			m_vecEyeSpeed += ( ( flCoefficients[0] * vecDeltaPos + flCoefficients[1] * vecDeltaSpeed ) * flFrameTime );
		}
	}
	else
	{
		// Generate an updated (dampening) speed for use in next frames position prediction.
		float flCoefficients[2];
		ComputePDControllerCoefficients( flCoefficients, r_CarViewDampenFreq.GetFloat(), r_CarViewDampenDamp.GetFloat(), flFrameTime );
		m_vecEyeSpeed += ( ( flCoefficients[0] * vecDeltaPos + flCoefficients[1] * vecDeltaSpeed ) * flFrameTime );
		
		// Save off data for next frame.
		m_vecLastEyePos = vecPredEyePos;
		
		// Move eye forward/backward.
		Vector vecForwardOffset = vecForward * ( vecForward.Dot( vecDeltaPos ) );
		vecVehicleEyePos -= vecForwardOffset;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_PropCar::DampenUpMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime )
{
	// Get up vector.
	Vector vecUp;
	AngleVectors( vecVehicleEyeAngles, NULL, NULL, &vecUp );
	vecUp.z = clamp( vecUp.z, 0.0f, vecUp.z );
	vecVehicleEyePos.z += r_CarViewZHeight.GetFloat() * vecUp.z;

	// NOTE: Should probably use some damped equation here.
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_PropCar::OnEnteredVehicle( C_BasePlayer *pPlayer )
{
	int eyeAttachmentIndex = LookupAttachment( "vehicle_driver_eyes" );
	Vector vehicleEyeOrigin;
	QAngle vehicleEyeAngles;
	GetAttachment( eyeAttachmentIndex, vehicleEyeOrigin, vehicleEyeAngles );

	m_vecLastEyeTarget = vehicleEyeOrigin;
	m_vecLastEyePos = vehicleEyeOrigin;
	m_vecEyeSpeed = vec3_origin;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &data - 
//-----------------------------------------------------------------------------
void CarWheelDustCallback( const CEffectData &data )
{
	CSmartPtr<CSimpleEmitter> pSimple = CSimpleEmitter::Create( "dust" );
	pSimple->SetSortOrigin( data.m_vOrigin );
	pSimple->SetNearClip( 32, 64 );

	SimpleParticle	*pParticle;

	Vector	offset;

	//FIXME: Better sampling area
	offset = data.m_vOrigin + ( data.m_vNormal * data.m_flScale );
	
	//Find area ambient light color and use it to tint smoke
	Vector	worldLight = WorldGetLightForPoint( offset, true );

	//Throw puffs
	offset.Random( -(data.m_flScale*16.0f), data.m_flScale*16.0f );
	offset.z = 0.0f;
	offset += data.m_vOrigin + ( data.m_vNormal * data.m_flScale );

	pParticle = (SimpleParticle *) pSimple->AddParticle( sizeof(SimpleParticle), g_Mat_DustPuff[0], offset );

	if ( pParticle != NULL )
	{			
		pParticle->m_flLifetime		= 0.0f;
		pParticle->m_flDieTime		= random->RandomFloat( 0.25f, 0.5f );
		
		pParticle->m_vecVelocity = RandomVector( -1.0f, 1.0f );
		VectorNormalize( pParticle->m_vecVelocity );
		pParticle->m_vecVelocity[2] += random->RandomFloat( 16.0f, 32.0f ) * (data.m_flScale*2.0f);

		int	color = random->RandomInt( 100, 150 );

		pParticle->m_uchColor[0] = 16 + ( worldLight[0] * (float) color );
		pParticle->m_uchColor[1] = 8 + ( worldLight[1] * (float) color );
		pParticle->m_uchColor[2] = ( worldLight[2] * (float) color );

		pParticle->m_uchStartAlpha	= random->RandomInt( 64.0f*data.m_flScale, 128.0f*data.m_flScale );
		pParticle->m_uchEndAlpha	= 0;
		pParticle->m_uchStartSize	= random->RandomInt( 16, 24 ) * data.m_flScale;
		pParticle->m_uchEndSize		= random->RandomInt( 32, 48 ) * data.m_flScale;
		pParticle->m_flRoll			= random->RandomInt( 0, 360 );
		pParticle->m_flRollDelta	= random->RandomFloat( -2.0f, 2.0f );
	}
}

DECLARE_CLIENT_EFFECT( "WheelDust", CarWheelDustCallback );