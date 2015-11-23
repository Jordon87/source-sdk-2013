//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Molotov grenades
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef	GRENADEMOLOTOV_H
#define	GRENADEMOLOTOV_H

#include "basegrenade_shared.h"
#include "smoke_trail.h"

class CGrenade_Molotov : public CBaseGrenade
{
public:
	DECLARE_CLASS( CGrenade_Molotov, CBaseGrenade );

	virtual void	Spawn( void );
	virtual void	Precache( void );
	virtual void	Detonate( void );
#if defined ( HUMANERROR_DLL )
	//void			CreateFireBlast( void );
	bool			CreateVPhysics();
	void			SetVelocity(const Vector &velocity, const AngularImpulse &angVelocity);
	int				OnTakeDamage(const CTakeDamageInfo &inputInfo);
	void			VPhysicsUpdate(IPhysicsObject *pPhysics);
#endif
	void			MolotovTouch( CBaseEntity *pOther );
#if defined ( HUMANERROR_DLL )
	void			WaitThink(void);
#endif
	void			MolotovThink( void );
#if defined ( HUMANERROR_DLL )
	void			FireThink(void);
	void			MolotovRemove(void);

	//	virtual	unsigned int	PhysicsSolidMaskForEntity( void ) const { return ( BaseClass::PhysicsSolidMaskForEntity() | CONTENTS_WATER ); }
#endif

protected:

	SmokeTrail		*m_pFireTrail;
#if defined ( HUMANERROR_DLL )
	bool			m_inSolid;

private:
#endif

	DECLARE_DATADESC();
};

#endif	//GRENADEMOLOTOV_H
