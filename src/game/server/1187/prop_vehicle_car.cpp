//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "vehicle_base.h"
#include "movevars_shared.h"
#include "te_effect_dispatch.h"
#include "soundent.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define CAR_WHEEL_COUNT	4
#define LOCK_SPEED 10
#define CAR_FRAMETIME_MIN		1e-6
#define CAR_DELTA_LENGTH_MAX	12.0f			// 1 foot

#define OVERTURNED_EXIT_WAITTIME	2.0f

ConVar	g_carexitspeed("g_carexitspeed", "800", FCVAR_CHEAT);

struct CarWaterData_t
{
	bool		m_bWheelInWater[CAR_WHEEL_COUNT];
	bool		m_bWheelWasInWater[CAR_WHEEL_COUNT];
	Vector		m_vecWheelContactPoints[CAR_WHEEL_COUNT];
	float		m_flNextRippleTime[CAR_WHEEL_COUNT];
	bool		m_bBodyInWater;
	bool		m_bBodyWasInWater;

	DECLARE_SIMPLE_DATADESC();
};

BEGIN_SIMPLE_DATADESC( CarWaterData_t )
	DEFINE_ARRAY( m_bWheelInWater,			FIELD_BOOLEAN,	CAR_WHEEL_COUNT ),
	DEFINE_ARRAY( m_bWheelWasInWater,			FIELD_BOOLEAN,	CAR_WHEEL_COUNT ),
	DEFINE_ARRAY( m_vecWheelContactPoints,	FIELD_VECTOR,	CAR_WHEEL_COUNT ),
	DEFINE_ARRAY( m_flNextRippleTime,			FIELD_TIME,		CAR_WHEEL_COUNT ),
	DEFINE_FIELD( m_bBodyInWater,				FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bBodyWasInWater,			FIELD_BOOLEAN ),
END_DATADESC()	

class CCarFourWheelServerVehicle : public CFourWheelServerVehicle
{
	typedef CFourWheelServerVehicle BaseClass;
	// IServerVehicle
public:
	bool		NPC_HasPrimaryWeapon(void) { return false; }
	int			GetExitAnimToUse(Vector& vecEyeExitEndpoint, bool& bAllPointsBlocked);
};

class CPropCar : public CPropVehicleDriveable
{
public:
	DECLARE_CLASS(CPropCar, CPropVehicleDriveable);

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CPropCar(void);

	// CPropVehicle
	void	ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMoveData );
	void	DriveVehicle( float flFrameTime, CUserCmd *ucmd, int iButtonsDown, int iButtonsReleased );
	void	SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move );
	void	Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	void	DampenEyePosition(Vector& vecVehicleEyePos, QAngle& vecVehicleEyeAngles);
	virtual bool	AllowBlockedExit(CBasePlayer* pPlayer, int nRole) { return false; }
	bool	CanExitVehicle(CBaseEntity* pEntity);
	bool	IsVehicleBodyInWater() { return m_WaterData.m_bBodyInWater; }

	bool PassengerShouldReceiveDamage(CTakeDamageInfo& info)
	{
		if (GetServerVehicle() && GetServerVehicle()->IsPassengerExiting())
			return false;

		if (info.GetDamageType() & DMG_VEHICLE)
			return true;

		return (info.GetDamageType() & (DMG_RADIATION | DMG_BLAST)) == 0;
	}

	// CBaseEntity
	void	Think(void);
	void	Precache(void);
	void	Spawn(void);
	void	Activate(void);

	void	CreateServerVehicle(void);
	Vector	BodyTarget( const Vector &posSrc, bool bNoisy = true );
	void	TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator );
	int		OnTakeDamage(const CTakeDamageInfo& info);
	virtual float	PassengerDamageModifier(const CTakeDamageInfo& info) { return 1.0f; }
	
	void	EnterVehicle(CBaseCombatCharacter* pPassenger);
	void	ExitVehicle(int nRole);

private:
	void		InitWaterData(void);
	void		CheckWaterLevel(void);
	void		CreateSplash(const Vector& vecPosition);
	void		CreateRipple(const Vector& vecPosition);

	void		CreateDangerSounds(void);

	void		ComputePDControllerCoefficients(float* pCoefficientsOut, float flFrequency, float flDampening, float flDeltaTime);
	void		DampenForwardMotion(Vector& vecVehicleEyePos, QAngle& vecVehicleEyeAngles, float flFrameTime);
	void		DampenUpMotion(Vector& vecVehicleEyePos, QAngle& vecVehicleEyeAngles, float flFrameTime);

protected:
	virtual void HandleWater(void);
	bool		 CheckWater(void);

	float			m_flDangerSoundTime;
	float			m_throttleDisableTime;

	// handbrake after the fact to keep vehicles from rolling
	float			m_flHandbrakeTime;
	bool			m_bInitialHandbrake;

	float			m_flOverturnedTime;

	Vector			m_vecLastEyePos;
	Vector			m_vecLastEyeTarget;
	Vector			m_vecEyeSpeed;
	Vector			m_vecTargetSpeed;

	CarWaterData_t	m_WaterData;

	int				m_iNumberOfEntries;

	float			m_flPlayerExitedTime;	// Time at which the player last left this vehicle
	float			m_flLastSawPlayerAt;	// Time at which we last saw the player
	EHANDLE			m_hLastPlayerInVehicle;
};

LINK_ENTITY_TO_CLASS(prop_vehicle_car, CPropCar);

BEGIN_DATADESC( CPropCar )
	DEFINE_FIELD( m_flDangerSoundTime, FIELD_TIME ),
	DEFINE_FIELD( m_throttleDisableTime, FIELD_TIME ),
	DEFINE_FIELD( m_flHandbrakeTime, FIELD_TIME ),
	DEFINE_FIELD( m_bInitialHandbrake, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flOverturnedTime, FIELD_TIME ),
	DEFINE_FIELD( m_vecLastEyePos, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecLastEyeTarget, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecEyeSpeed, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecTargetSpeed, FIELD_POSITION_VECTOR ),
	DEFINE_EMBEDDED( m_WaterData ),

	DEFINE_FIELD( m_iNumberOfEntries, FIELD_INTEGER ),

	DEFINE_FIELD( m_flPlayerExitedTime, FIELD_TIME ),
	DEFINE_FIELD( m_flLastSawPlayerAt, FIELD_TIME ),
	DEFINE_FIELD( m_hLastPlayerInVehicle, FIELD_EHANDLE ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CPropCar, DT_PropCar )
END_SEND_TABLE();

CPropCar::CPropCar()
{
	m_flOverturnedTime = 0.0f;
	m_iNumberOfEntries = 0;
	m_vecEyeSpeed.Init();

	InitWaterData();
}

void CPropCar::CreateServerVehicle(void)
{
	// Create our armed server vehicle
	m_pServerVehicle = new CCarFourWheelServerVehicle();
	m_pServerVehicle->SetVehicle(this);
}

void CPropCar::Precache(void)
{
	BaseClass::Precache();
}

void CPropCar::Spawn(void)
{
	SetVehicleType(VEHICLE_TYPE_CAR_WHEELS);

	BaseClass::Spawn();
	m_flHandbrakeTime = gpGlobals->curtime + 0.1f;
	m_bInitialHandbrake = false;

	m_flMinimumSpeedToEnterExit = LOCK_SPEED;

	AddSolidFlags(FSOLID_NOT_STANDABLE);
}

void CPropCar::Activate(void)
{
	BaseClass::Activate();

	CBaseServerVehicle* pServerVehicle = dynamic_cast<CBaseServerVehicle*>(GetServerVehicle());
	if (pServerVehicle)
	{
		if (pServerVehicle->GetPassenger())
		{
			// If a jeep comes back from a save game with a driver, make sure the engine rumble starts up.
			pServerVehicle->StartEngineRumble();
		}
	}
}

void CPropCar::TraceAttack(const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator)
{
	if (GetDriver() && GetDriver()->IsPlayer())
	{
		GetDriver()->TakeDamage(info);
	}
	BaseClass::TraceAttack(info, vecDir, ptr, pAccumulator);
}

int CPropCar::OnTakeDamage(const CTakeDamageInfo& inputInfo)
{
	//Do scaled up physics damage to the car
	CTakeDamageInfo info = inputInfo;

	// HACKHACK: Scale up grenades until we get a better explosion/pressure damage system
	if (inputInfo.GetDamageType() & DMG_BLAST)
	{
		info.SetDamageForce(inputInfo.GetDamageForce() * 10);
	}

	VPhysicsTakeDamage(info);

	// reset the damage
	info.SetDamage(inputInfo.GetDamage());

	//Check to do damage to driver
	if (GetDriver() && GetDriver()->IsPlayer())
	{
		GetDriver()->TakeDamage(info);
		return 0;
	}

	return 0;
}

Vector CPropCar::BodyTarget(const Vector& posSrc, bool bNoisy)
{
	Vector	shotPos;
	matrix3x4_t	matrix;

	int eyeAttachmentIndex = LookupAttachment("vehicle_driver_eyes_coop");
	GetAttachment(eyeAttachmentIndex, matrix);
	MatrixGetColumn(matrix, 3, shotPos);

	if (bNoisy)
	{
		shotPos[0] += random->RandomFloat(-8.0f, 8.0f);
		shotPos[1] += random->RandomFloat(-8.0f, 8.0f);
		shotPos[2] += random->RandomFloat(-8.0f, 8.0f);
	}

	return shotPos;
}

void CPropCar::InitWaterData(void)
{
	m_WaterData.m_bBodyInWater = false;
	m_WaterData.m_bBodyWasInWater = false;

	for (int iWheel = 0; iWheel < CAR_WHEEL_COUNT; ++iWheel)
	{
		m_WaterData.m_bWheelInWater[iWheel] = false;
		m_WaterData.m_bWheelWasInWater[iWheel] = false;
		m_WaterData.m_vecWheelContactPoints[iWheel].Init();
		m_WaterData.m_flNextRippleTime[iWheel] = 0;
	}
}

void CPropCar::HandleWater(void)
{
	// Only check the wheels and engine in water if we have a driver (player).
	if (!GetDriver())
		return;

	// Check to see if we are in water.
	if (CheckWater())
	{
		for (int iWheel = 0; iWheel < CAR_WHEEL_COUNT; ++iWheel)
		{
			// Create an entry/exit splash!
			if (m_WaterData.m_bWheelInWater[iWheel] != m_WaterData.m_bWheelWasInWater[iWheel])
			{
				CreateSplash(m_WaterData.m_vecWheelContactPoints[iWheel]);
				CreateRipple(m_WaterData.m_vecWheelContactPoints[iWheel]);
			}

			// Create ripples.
			if (m_WaterData.m_bWheelInWater[iWheel] && m_WaterData.m_bWheelWasInWater[iWheel])
			{
				if (m_WaterData.m_flNextRippleTime[iWheel] < gpGlobals->curtime)
				{
					// Stagger ripple times
					m_WaterData.m_flNextRippleTime[iWheel] = gpGlobals->curtime + RandomFloat(0.1, 0.3);
					CreateRipple(m_WaterData.m_vecWheelContactPoints[iWheel]);
				}
			}
		}
	}

	// Save of data from last think.
	for (int iWheel = 0; iWheel < CAR_WHEEL_COUNT; ++iWheel)
	{
		m_WaterData.m_bWheelWasInWater[iWheel] = m_WaterData.m_bWheelInWater[iWheel];
	}
}

bool CPropCar::CheckWater(void)
{
	bool bInWater = false;

	// Check all four wheels.
	for (int iWheel = 0; iWheel < CAR_WHEEL_COUNT; ++iWheel)
	{
		// Get the current wheel and get its contact point.
		IPhysicsObject* pWheel = m_VehiclePhysics.GetWheel(iWheel);
		if (!pWheel)
			continue;

		// Check to see if we hit water.
		if (pWheel->GetContactPoint(&m_WaterData.m_vecWheelContactPoints[iWheel], NULL))
		{
			m_WaterData.m_bWheelInWater[iWheel] = (UTIL_PointContents(m_WaterData.m_vecWheelContactPoints[iWheel]) & MASK_WATER) ? true : false;
			if (m_WaterData.m_bWheelInWater[iWheel])
			{
				bInWater = true;
			}
		}
	}

	// Check the body and the BONNET.
	int iEngine = LookupAttachment("vehicle_engine");
	Vector vecEnginePoint;
	QAngle vecEngineAngles;
	GetAttachment(iEngine, vecEnginePoint, vecEngineAngles);

	m_WaterData.m_bBodyInWater = (UTIL_PointContents(vecEnginePoint) & MASK_WATER) ? true : false;
	if (m_WaterData.m_bBodyInWater)
	{

		if (!m_VehiclePhysics.IsEngineDisabled())
		{
			m_VehiclePhysics.SetDisableEngine(true);
		}
	}
	else
	{
		if (m_VehiclePhysics.IsEngineDisabled())
		{
			m_VehiclePhysics.SetDisableEngine(false);
		}
	}

	if (bInWater)
	{
		// Check the player's water level.
		CheckWaterLevel();
	}

	return bInWater;
}

void CPropCar::CheckWaterLevel(void)
{
	CBaseEntity* pEntity = GetDriver();
	if (pEntity && pEntity->IsPlayer())
	{
		CBasePlayer* pPlayer = static_cast<CBasePlayer*>(pEntity);

		Vector vecAttachPoint;
		QAngle vecAttachAngles;

		// Check eyes. (vehicle_driver_eyes point)
		int iAttachment = LookupAttachment("vehicle_driver_eyes_coop");
		GetAttachment(iAttachment, vecAttachPoint, vecAttachAngles);

		// Add the jeep's Z view offset
		Vector vecUp;
		AngleVectors(vecAttachAngles, NULL, NULL, &vecUp);
		vecUp.z = clamp(vecUp.z, 0.0f, vecUp.z);
		vecAttachPoint.z += r_JeepViewZHeight.GetFloat() * vecUp.z;

		bool bEyes = (UTIL_PointContents(vecAttachPoint) & MASK_WATER) ? true : false;
		if (bEyes)
		{
			pPlayer->SetWaterLevel(WL_Eyes);
			return;
		}

		// Check waist.  (vehicle_engine point -- see parent function).
		if (m_WaterData.m_bBodyInWater)
		{
			pPlayer->SetWaterLevel(WL_Waist);
			return;
		}

		// Check feet. (vehicle_feet_passenger0 point)
		iAttachment = LookupAttachment("vehicle_feet_passenger0");
		GetAttachment(iAttachment, vecAttachPoint, vecAttachAngles);
		bool bFeet = (UTIL_PointContents(vecAttachPoint) & MASK_WATER) ? true : false;
		if (bFeet)
		{
			pPlayer->SetWaterLevel(WL_Feet);
			return;
		}

		// Not in water.
		pPlayer->SetWaterLevel(WL_NotInWater);
	}
}

void CPropCar::CreateSplash(const Vector& vecPosition)
{
	// Splash data.
	CEffectData	data;
	data.m_fFlags = 0;
	data.m_vOrigin = vecPosition;
	data.m_vNormal.Init(0.0f, 0.0f, 1.0f);
	VectorAngles(data.m_vNormal, data.m_vAngles);
	data.m_flScale = 10.0f + random->RandomFloat(0, 2);

	// Create the splash..
	DispatchEffect("watersplash", data);
}

void CPropCar::CreateRipple(const Vector& vecPosition)
{
	// Ripple data.
	CEffectData	data;
	data.m_fFlags = 0;
	data.m_vOrigin = vecPosition;
	data.m_vNormal.Init(0.0f, 0.0f, 1.0f);
	VectorAngles(data.m_vNormal, data.m_vAngles);
	data.m_flScale = 10.0f + random->RandomFloat(0, 2);
	if (GetWaterType() & CONTENTS_SLIME)
	{
		data.m_fFlags |= FX_WATER_IN_SLIME;
	}

	// Create the ripple.
	DispatchEffect("waterripple", data);
}

void CPropCar::Think(void)
{
	BaseClass::Think();

	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();

	if (m_bEngineLocked)
	{
		m_bUnableToFire = true;

		if (pPlayer != NULL)
		{
			pPlayer->m_Local.m_iHideHUD |= HIDEHUD_VEHICLE_CROSSHAIR;
		}
	}

	int iDriver = LookupAttachment("vehicle_driver_eyes_coop");
	if (iDriver != -1)
	{
		if (GetDriver())
		{
			Vector driverOrigin;
			GetAttachment(iDriver, driverOrigin, NULL, NULL, NULL);
			GetDriver()->SetAbsAngles(GetAbsAngles());
			GetDriver()->SetLocalAngles(GetLocalAngles());
		}
	}

	GetBaseAnimating()->SetBodygroup(1, pPlayer->IsInAVehicle());

	// Water!?
	HandleWater();

	SetSimulationTime(gpGlobals->curtime);

	SetNextThink(gpGlobals->curtime);
	SetAnimatedEveryTick(true);

	if (!m_bInitialHandbrake)	// after initial timer expires, set the handbrake
	{
		m_bInitialHandbrake = true;
		m_VehiclePhysics.SetHandbrake(true);
		m_VehiclePhysics.Think();
	}

	// Check overturned status.
	if (!IsOverturned())
	{
		m_flOverturnedTime = 0.0f;
	}
	else
	{
		m_flOverturnedTime += gpGlobals->frametime;
	}

	StudioFrameAdvance();

	// If the enter or exit animation has finished, tell the server vehicle
	if (IsSequenceFinished() && (m_bExitAnimOn || m_bEnterAnimOn))
	{
		if (m_bEnterAnimOn)
		{
			m_VehiclePhysics.ReleaseHandbrake();
			StartEngine();

			// HACKHACK: This forces the jeep to play a sound when it gets entered underwater
			if (m_VehiclePhysics.IsEngineDisabled())
			{
				CBaseServerVehicle* pServerVehicle = dynamic_cast<CBaseServerVehicle*>(GetServerVehicle());
				if (pServerVehicle)
				{
					pServerVehicle->SoundStartDisabled();
				}
			}
		}
		GetServerVehicle()->HandleEntryExitFinish(m_bExitAnimOn, m_bExitAnimOn);
	}
}

void CPropCar::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	BaseClass::Use(pActivator, pCaller, useType, value);
}

bool CPropCar::CanExitVehicle(CBaseEntity* pEntity)
{
	return (!m_bEnterAnimOn && !m_bExitAnimOn && !m_bLocked && (m_nSpeed <= g_carexitspeed.GetFloat()));
}

void CPropCar::DampenEyePosition(Vector& vecVehicleEyePos, QAngle& vecVehicleEyeAngles)
{
	// Get the frametime. (Check to see if enough time has passed to warrent dampening).
	float flFrameTime = gpGlobals->frametime;
	if (flFrameTime < CAR_FRAMETIME_MIN)
	{
		vecVehicleEyePos = m_vecLastEyePos;
		DampenUpMotion(vecVehicleEyePos, vecVehicleEyeAngles, 0.0f);
		return;
	}

	// Keep static the sideways motion.

	// Dampen forward/backward motion.
	DampenForwardMotion(vecVehicleEyePos, vecVehicleEyeAngles, flFrameTime);

	// Blend up/down motion.
	DampenUpMotion(vecVehicleEyePos, vecVehicleEyeAngles, flFrameTime);
}

void CPropCar::ComputePDControllerCoefficients(float* pCoefficientsOut, float flFrequency, float flDampening, float flDeltaTime)
{
	float flKs = 9.0f * flFrequency * flFrequency;
	float flKd = 4.5f * flFrequency * flDampening;

	float flScale = 1.0f / (1.0f + flKd * flDeltaTime + flKs * flDeltaTime * flDeltaTime);

	pCoefficientsOut[0] = flKs * flScale;
	pCoefficientsOut[1] = (flKd + flKs * flDeltaTime) * flScale;
}

void CPropCar::DampenForwardMotion(Vector& vecVehicleEyePos, QAngle& vecVehicleEyeAngles, float flFrameTime)
{
	// Get forward vector.
	Vector vecForward;
	AngleVectors(vecVehicleEyeAngles, &vecForward);

	// Simulate the eye position forward based on the data from last frame
	// (assumes no acceleration - it will get that from the "spring").
	Vector vecCurrentEyePos = m_vecLastEyePos + m_vecEyeSpeed * flFrameTime;

	// Calculate target speed based on the current vehicle eye position and the last vehicle eye position and frametime.
	Vector vecVehicleEyeSpeed = (vecVehicleEyePos - m_vecLastEyeTarget) / flFrameTime;
	m_vecLastEyeTarget = vecVehicleEyePos;

	// Calculate the speed and position deltas.
	Vector vecDeltaSpeed = vecVehicleEyeSpeed - m_vecEyeSpeed;
	Vector vecDeltaPos = vecVehicleEyePos - vecCurrentEyePos;

	// Clamp.
	if (vecDeltaPos.Length() > CAR_DELTA_LENGTH_MAX)
	{
		float flSign = vecForward.Dot(vecVehicleEyeSpeed) >= 0.0f ? -1.0f : 1.0f;
		vecVehicleEyePos += flSign * (vecForward * CAR_DELTA_LENGTH_MAX);
		m_vecLastEyePos = vecVehicleEyePos;
		m_vecEyeSpeed = vecVehicleEyeSpeed;
		return;
	}

	// Generate an updated (dampening) speed for use in next frames position extrapolation.
	float flCoefficients[2];
	ComputePDControllerCoefficients(flCoefficients, r_CarViewDampenFreq.GetFloat(), r_CarViewDampenDamp.GetFloat(), flFrameTime);
	m_vecEyeSpeed += ((flCoefficients[0] * vecDeltaPos + flCoefficients[1] * vecDeltaSpeed) * flFrameTime);

	// Save off data for next frame.
	m_vecLastEyePos = vecCurrentEyePos;

	// Move eye forward/backward.
	Vector vecForwardOffset = vecForward * (vecForward.Dot(vecDeltaPos));
	vecVehicleEyePos -= vecForwardOffset;
}

void CPropCar::DampenUpMotion(Vector& vecVehicleEyePos, QAngle& vecVehicleEyeAngles, float flFrameTime)
{
	// Get up vector.
	Vector vecUp;
	AngleVectors(vecVehicleEyeAngles, NULL, NULL, &vecUp);
	vecUp.z = clamp(vecUp.z, 0.0f, vecUp.z);
	vecVehicleEyePos.z += r_CarViewZHeight.GetFloat() * vecUp.z;

	// NOTE: Should probably use some damped equation here.
}

void CPropCar::SetupMove(CBasePlayer* player, CUserCmd* ucmd, IMoveHelper* pHelper, CMoveData* move)
{
	// If we are overturned and hit any key - leave the vehicle (IN_USE is already handled!).
	if (m_flOverturnedTime > OVERTURNED_EXIT_WAITTIME)
	{
		if ((ucmd->buttons & (IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT | IN_SPEED | IN_JUMP | IN_ATTACK | IN_ATTACK2)) && !m_bExitAnimOn)
		{
			// Can't exit yet? We're probably still moving. Swallow the keys.
			if (!CanExitVehicle(player))
				return;

			if (!GetServerVehicle()->HandlePassengerExit(m_hPlayer) && (m_hPlayer != NULL))
			{
				m_hPlayer->PlayUseDenySound();
			}
			return;
		}
	}

	// If the throttle is disabled or we're upside-down, don't allow throttling (including turbo)
	CUserCmd tmp;
	if ((m_throttleDisableTime > gpGlobals->curtime) || (IsOverturned()))
	{
		m_bUnableToFire = true;

		tmp = (*ucmd);
		tmp.buttons &= ~(IN_FORWARD | IN_BACK | IN_SPEED);
		ucmd = &tmp;
	}

	BaseClass::SetupMove(player, ucmd, pHelper, move);
}

void CPropCar::DriveVehicle(float flFrameTime, CUserCmd* ucmd, int iButtonsDown, int iButtonsReleased)
{
	BaseClass::DriveVehicle(flFrameTime,ucmd,iButtonsDown,iButtonsReleased);
}

void CPropCar::ProcessMovement(CBasePlayer* pPlayer, CMoveData* pMoveData)
{
	BaseClass::ProcessMovement(pPlayer, pMoveData);

	CreateDangerSounds();
}

void CPropCar::CreateDangerSounds(void)
{
	if (m_flDangerSoundTime > gpGlobals->curtime)
		return;

	QAngle vehicleAngles = GetLocalAngles();
	Vector vecStart = GetAbsOrigin();
	Vector vecDir, vecRight;

	GetVectors(&vecDir, &vecRight, NULL);

	const float soundDuration = 0.25;
	float speed = m_VehiclePhysics.GetHLSpeed();

	// Make danger sounds ahead of the jeep
	if (fabs(speed) > 120)
	{
		Vector	vecSpot;

		float steering = m_VehiclePhysics.GetSteering();
		if (steering != 0)
		{
			if (speed > 0)
			{
				vecDir += vecRight * steering * 0.5;
			}
			else
			{
				vecDir -= vecRight * steering * 0.5;
			}
			VectorNormalize(vecDir);
		}
		const float radius = speed * 0.4;

		// 0.3 seconds ahead of the jeep
		vecSpot = vecStart + vecDir * (speed * 1.1f);
		CSoundEnt::InsertSound(SOUND_DANGER | SOUND_CONTEXT_PLAYER_VEHICLE, vecSpot, radius, soundDuration, this, 0);
		CSoundEnt::InsertSound(SOUND_PHYSICS_DANGER | SOUND_CONTEXT_PLAYER_VEHICLE, vecSpot, radius, soundDuration, this, 1);
		//NDebugOverlay::Box(vecSpot, Vector(-radius,-radius,-radius),Vector(radius,radius,radius), 255, 0, 255, 0, soundDuration);

	}

	// Make engine sounds even when we're not going fast.
	CSoundEnt::InsertSound(SOUND_PLAYER | SOUND_CONTEXT_PLAYER_VEHICLE, GetAbsOrigin(), 800, soundDuration, this, 0);

	m_flDangerSoundTime = gpGlobals->curtime + 0.1;
}

void CPropCar::EnterVehicle( CBaseCombatCharacter *pPassenger )
{
	CBasePlayer *pPlayer = ToBasePlayer( pPassenger );
	if ( !pPlayer )
		return;

	CheckWater();
	BaseClass::EnterVehicle( pPassenger );

	// Start looking for seagulls to land
	m_hLastPlayerInVehicle = m_hPlayer;
}

void CPropCar::ExitVehicle(int nRole)
{
	BaseClass::ExitVehicle(nRole);

	// Remember when we last saw the player
	m_flPlayerExitedTime = gpGlobals->curtime;
	m_flLastSawPlayerAt = gpGlobals->curtime;
}

int CCarFourWheelServerVehicle::GetExitAnimToUse(Vector& vecEyeExitEndpoint, bool& bAllPointsBlocked)
{
	bAllPointsBlocked = false;

	if (!m_bParsedAnimations)
	{
		// Load the entry/exit animations from the vehicle
		ParseEntryExitAnims();
		m_bParsedAnimations = true;
	}

	return BaseClass::GetExitAnimToUse(vecEyeExitEndpoint, bAllPointsBlocked);
}
