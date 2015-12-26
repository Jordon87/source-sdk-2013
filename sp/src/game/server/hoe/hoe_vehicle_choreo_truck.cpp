//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "vehicle_base.h"
#include "engine/IEngineSound.h"
#include "in_buttons.h"
#include "soundenvelope.h"
#include "soundent.h"
#include "physics_saverestore.h"
#include "vphysics/constraints.h"
#include "vcollide_parse.h"
#include "ndebugoverlay.h"
#include "hl2_player.h"
#include "props.h"
#include "vehicle_choreo_generic_shared.h"
#include "ai_utils.h"
#include "vehicle_choreo_generic.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CHoe_PropVehicleChoreoTruck : public CPropVehicleChoreoGeneric
{
	DECLARE_CLASS(CHoe_PropVehicleChoreoTruck, CPropVehicleChoreoGeneric);
public:
	DECLARE_DATADESC();

protected:

	//
	// Inputs
	//
	void InputOpenDriverDoor(inputdata_t& inputdata);
	void InputCloseDriverDoor(inputdata_t& inputdata);
	void InputPassengerOpenVehicle(inputdata_t& inputdata);
	void InputPassengerCloseVehicle(inputdata_t& inputdata);
	void InputUnlockDriverSideExit(inputdata_t& inputdata);
	void InputVehicleSound(inputdata_t& inputdata);

	// Outputs
	COutputEvent m_OnDriverDoorOpen;
	COutputEvent m_OnDriverDoorClosed;
	COutputEvent m_OnPlayerAttemptedDriverEntry;
};

LINK_ENTITY_TO_CLASS(prop_vehicle_choreo_truck, CHoe_PropVehicleChoreoTruck);

BEGIN_DATADESC(CHoe_PropVehicleChoreoTruck)

	// Inputs
	DEFINE_INPUTFUNC(FIELD_VOID,	"OpenDriverDoor",		InputOpenDriverDoor),
	DEFINE_INPUTFUNC(FIELD_VOID,	"CloseDriverDoor",		InputCloseDriverDoor),
	DEFINE_INPUTFUNC(FIELD_VOID,	"PassengerOpenVehicle",	InputPassengerOpenVehicle),
	DEFINE_INPUTFUNC(FIELD_VOID,	"PassengerCloseVehicle",InputPassengerCloseVehicle),
	DEFINE_INPUTFUNC(FIELD_VOID,	"UnlockDriverSideExit",	InputUnlockDriverSideExit),
	DEFINE_INPUTFUNC(FIELD_STRING,	"VehicleSound",			InputVehicleSound),

	// Outputs
	DEFINE_OUTPUT(m_OnDriverDoorOpen,				"OnDriverDoorOpen"),
	DEFINE_OUTPUT(m_OnDriverDoorClosed,				"OnDriverDoorClosed"),
	DEFINE_OUTPUT(m_OnPlayerAttemptedDriverEntry,	"OnPlayerAttemptedDriverEntry"),

END_DATADESC()

void CHoe_PropVehicleChoreoTruck::InputOpenDriverDoor(inputdata_t& inputdata)
{
	
}

void CHoe_PropVehicleChoreoTruck::InputCloseDriverDoor(inputdata_t& inputdata)
{

}

void CHoe_PropVehicleChoreoTruck::InputPassengerOpenVehicle(inputdata_t& inputdata)
{

}

void CHoe_PropVehicleChoreoTruck::InputPassengerCloseVehicle(inputdata_t& inputdata)
{

}

void CHoe_PropVehicleChoreoTruck::InputUnlockDriverSideExit(inputdata_t& inputdata)
{

}

void CHoe_PropVehicleChoreoTruck::InputVehicleSound(inputdata_t& inputdata)
{

}