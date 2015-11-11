//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "vehicle_jeep.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define CAR_BODYGROUP_HANDS					1

#define CAR_BODYGROUP_HANDS_VISIBLE			0
#define CAR_BODYGROUP_HANDS_INVISIBLE		1

class CPropCar : public CPropJeep
{
	DECLARE_CLASS(CPropCar, CPropJeep);
public:

	virtual void	Spawn(void);

	virtual void	EnterVehicle(CBaseCombatCharacter *pPassenger);
	virtual void	ExitVehicle(int nRole);

protected:
	virtual void	ShowPassengerHands(void);
	virtual void	HidePassengerHands(void);
};

LINK_ENTITY_TO_CLASS(prop_vehicle_car, CPropCar);

void CPropCar::Spawn(void)
{
	m_bHasGun = false;

	BaseClass::Spawn();

	HidePassengerHands();
}

void CPropCar::EnterVehicle(CBaseCombatCharacter *pPassenger)
{
	BaseClass::EnterVehicle(pPassenger);

	ShowPassengerHands();
}

void CPropCar::ExitVehicle(int nRole)
{
	BaseClass::ExitVehicle(nRole);

	HidePassengerHands();
}

void CPropCar::ShowPassengerHands(void)
{
	SetBodygroup(1, 1);
}

void CPropCar::HidePassengerHands(void)
{
	SetBodygroup(1, 0);
}