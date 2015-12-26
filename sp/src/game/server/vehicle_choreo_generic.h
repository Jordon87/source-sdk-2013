//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//


#ifndef VEHICLE_CHOREO_GENERIC_H
#define VEHICLE_CHOREO_GENERIC_H

#ifdef _WIN32
#pragma once
#endif


#if defined ( HOE_DLL )
class CPropVehicleChoreoGeneric;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CChoreoGenericServerVehicle : public CBaseServerVehicle
{
	typedef CBaseServerVehicle BaseClass;

	// IServerVehicle
public:
	void GetVehicleViewPosition(int nRole, Vector *pAbsOrigin, QAngle *pAbsAngles, float *pFOV = NULL);
	virtual void ItemPostFrame(CBasePlayer *pPlayer);

protected:

	CPropVehicleChoreoGeneric *GetVehicle(void);
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CPropVehicleChoreoGeneric : public CDynamicProp, public IDrivableVehicle
{
	DECLARE_CLASS(CPropVehicleChoreoGeneric, CDynamicProp);

public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CPropVehicleChoreoGeneric(void)
	{
		m_ServerVehicle.SetVehicle(this);
		m_bIgnoreMoveParent = false;
		m_bForcePlayerEyePoint = false;
	}

	~CPropVehicleChoreoGeneric(void)
	{
	}

	// CBaseEntity
	virtual void	Precache(void);
	void			Spawn(void);
	void			Think(void);
	virtual int		ObjectCaps(void) { return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE; };
	virtual void	Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual void	DrawDebugGeometryOverlays(void);

	virtual Vector	BodyTarget(const Vector &posSrc, bool bNoisy = true);
	virtual void	TraceAttack(const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr);
	virtual int		OnTakeDamage(const CTakeDamageInfo &info);

	void			PlayerControlInit(CBasePlayer *pPlayer);
	void			PlayerControlShutdown(void);
	void			ResetUseKey(CBasePlayer *pPlayer);

	virtual bool OverridePropdata() { return true; }

	bool			ParseViewParams(const char *pScriptName);

	void			GetVectors(Vector* pForward, Vector* pRight, Vector* pUp) const;

	bool CreateVPhysics()
	{
		SetSolid(SOLID_VPHYSICS);
		SetMoveType(MOVETYPE_NONE);
		return true;
	}
	bool ShouldForceExit() { return m_bForcedExit; }
	void ClearForcedExit() { m_bForcedExit = false; }

	// CBaseAnimating
	void HandleAnimEvent(animevent_t *pEvent);

	// Inputs
	void InputEnterVehicleImmediate(inputdata_t &inputdata);
	void InputEnterVehicle(inputdata_t &inputdata);
	void InputExitVehicle(inputdata_t &inputdata);
	void InputLock(inputdata_t &inputdata);
	void InputUnlock(inputdata_t &inputdata);
	void InputOpen(inputdata_t &inputdata);
	void InputClose(inputdata_t &inputdata);
	void InputViewlock(inputdata_t &inputdata);

	bool ShouldIgnoreParent(void) { return m_bIgnoreMoveParent; }

	// Tuned to match HL2s definition, but this should probably return false in all cases
	virtual bool	PassengerShouldReceiveDamage(CTakeDamageInfo &info) { return (info.GetDamageType() & (DMG_BLAST | DMG_RADIATION)) == 0; }

	CNetworkHandle(CBasePlayer, m_hPlayer);

	CNetworkVarEmbedded(vehicleview_t, m_vehicleView);
private:
	vehicleview_t m_savedVehicleView; // gets saved out for viewlock/unlock input

	// IDrivableVehicle
public:

	virtual CBaseEntity *GetDriver(void);
	virtual void ProcessMovement(CBasePlayer *pPlayer, CMoveData *pMoveData) { return; }
	virtual void FinishMove(CBasePlayer *player, CUserCmd *ucmd, CMoveData *move) { return; }
	virtual bool CanEnterVehicle(CBaseEntity *pEntity);
	virtual bool CanExitVehicle(CBaseEntity *pEntity);
	virtual void SetVehicleEntryAnim(bool bOn);
	virtual void SetVehicleExitAnim(bool bOn, Vector vecEyeExitEndpoint) { m_bExitAnimOn = bOn; if (bOn) m_vecEyeExitEndpoint = vecEyeExitEndpoint; }
	virtual void EnterVehicle(CBaseCombatCharacter *pPassenger);

	virtual bool AllowBlockedExit(CBaseCombatCharacter *pPassenger, int nRole) { return true; }
	virtual bool AllowMidairExit(CBaseCombatCharacter *pPassenger, int nRole) { return true; }
	virtual void PreExitVehicle(CBaseCombatCharacter *pPassenger, int nRole) {}
	virtual void ExitVehicle(int nRole);

	virtual void ItemPostFrame(CBasePlayer *pPlayer) {}
	virtual void SetupMove(CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move) {}
	virtual string_t GetVehicleScriptName() { return m_vehicleScript; }

	// If this is a vehicle, returns the vehicle interface
	virtual IServerVehicle *GetServerVehicle() { return &m_ServerVehicle; }

	bool ShouldCollide(int collisionGroup, int contentsMask) const;

	bool				m_bForcePlayerEyePoint;			// Uses player's eyepoint instead of 'vehicle_driver_eyes' attachment

protected:

	// Contained IServerVehicle
	CChoreoGenericServerVehicle m_ServerVehicle;

private:

	// Entering / Exiting
	bool				m_bLocked;
	CNetworkVar(bool, m_bEnterAnimOn);
	CNetworkVar(bool, m_bExitAnimOn);
	CNetworkVector(m_vecEyeExitEndpoint);
	bool				m_bForcedExit;
	bool				m_bIgnoreMoveParent;
	bool				m_bIgnorePlayerCollisions;

	// Vehicle script filename
	string_t			m_vehicleScript;

	COutputEvent		m_playerOn;
	COutputEvent		m_playerOff;
	COutputEvent		m_OnOpen;
	COutputEvent		m_OnClose;
};
#endif // defined ( HOE_DLL )


#endif // VEHICLE_CHOREO_GENERIC_H