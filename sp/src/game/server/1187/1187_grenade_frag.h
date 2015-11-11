//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//


#ifndef ELEVENEIGHTYSEVEN_GRENADE_FRAG_H
#define ELEVENEIGHTYSEVEN_GRENADE_FRAG_H

#include "basegrenade_shared.h"

class C1187GrenadeFrag : public CBaseGrenade
{
	DECLARE_CLASS(C1187GrenadeFrag, CBaseGrenade);

#if !defined( CLIENT_DLL )
	DECLARE_DATADESC();
#endif

	~C1187GrenadeFrag(void);

public:
	void	Spawn(void);
	void	OnRestore(void);
	void	Precache(void);
	bool	CreateVPhysics(void);
	void	SetTimer(float detonateDelay, float warnDelay);
	void	SetVelocity(const Vector &velocity, const AngularImpulse &angVelocity);
	int		OnTakeDamage(const CTakeDamageInfo &inputInfo);
	void	DelayThink();
	void	VPhysicsUpdate(IPhysicsObject *pPhysics);

	void	InputSetTimer(inputdata_t &inputdata);

protected:

	bool	m_inSolid;
};

CBaseGrenade *Fraggrenade_Create_1187(const Vector &position, const QAngle &angles, const Vector &velocity, const AngularImpulse &angVelocity, CBaseEntity *pOwner, float timer);

#endif // ELEVENEIGHTYSEVEN_GRENADE_FRAG_H