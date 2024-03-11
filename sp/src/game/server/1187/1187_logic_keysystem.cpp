//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "baseentity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CLogicKeySystem : public CLogicalEntity
{
	DECLARE_CLASS(CLogicKeySystem, CLogicalEntity);
public:
	DECLARE_DATADESC();

	void Activate(void);
	void Think(void);

protected:

	// Inputs
	void InputRedKey(inputdata_t &inputdata);
	void InputBlueKey(inputdata_t &inputdata);
	void InputYellowKey(inputdata_t &inputdata);
	void InputGreenKey(inputdata_t &inputdata);
	void InputKey1(inputdata_t &inputdata);
	void InputKey2(inputdata_t &inputdata);
	void InputKey3(inputdata_t &inputdata);
	void InputKey4(inputdata_t &inputdata);
	void InputKey5(inputdata_t &inputdata);
	void InputKey6(inputdata_t &inputdata);
	void InputKey7(inputdata_t &inputdata);
	void InputKey8(inputdata_t &inputdata);
	void InputKey9(inputdata_t &inputdata);
	void InputKey10(inputdata_t &inputdata);
	void InputSpecialKey(inputdata_t &inputdata);

	// Outputs
	COutputEvent m_OnFoundRedKey;
	COutputEvent m_OnFoundBlueKey;
	COutputEvent m_OnFoundYellowKey;
	COutputEvent m_OnFoundGreenKey;
	COutputEvent m_OnFoundKey1;
	COutputEvent m_OnFoundKey2;
	COutputEvent m_OnFoundKey3;
	COutputEvent m_OnFoundKey4;
	COutputEvent m_OnFoundKey5;
	COutputEvent m_OnFoundKey6;
	COutputEvent m_OnFoundKey7;
	COutputEvent m_OnFoundKey8;
	COutputEvent m_OnFoundKey9;
	COutputEvent m_OnFoundKey10;
	COutputEvent m_OnSpawn;
	COutputEvent m_OnFoundSpecialKey;

};

LINK_ENTITY_TO_CLASS(logic_keysystem, CLogicKeySystem);

//-----------------------------
// Save & Restore
//-----------------------------

BEGIN_DATADESC(CLogicKeySystem)

	//
	// Outputs
	//
	DEFINE_OUTPUT(m_OnFoundRedKey, "OnFoundRedKey"),
	DEFINE_OUTPUT(m_OnFoundBlueKey, "OnFoundBlueKey"),
	DEFINE_OUTPUT(m_OnFoundYellowKey, "OnFoundYellowKey"),
	DEFINE_OUTPUT(m_OnFoundGreenKey, "OnFoundGreenKey"),
	DEFINE_OUTPUT(m_OnFoundSpecialKey, "OnFoundSpecialKey"),
	DEFINE_OUTPUT(m_OnFoundKey1, "OnFoundKey1"),
	DEFINE_OUTPUT(m_OnFoundKey2, "OnFoundKey2"),
	DEFINE_OUTPUT(m_OnFoundKey3, "OnFoundKey3"),
	DEFINE_OUTPUT(m_OnFoundKey4, "OnFoundKey4"),
	DEFINE_OUTPUT(m_OnFoundKey5, "OnFoundKey5"),
	DEFINE_OUTPUT(m_OnFoundKey6, "OnFoundKey6"),
	DEFINE_OUTPUT(m_OnFoundKey7, "OnFoundKey7"),
	DEFINE_OUTPUT(m_OnFoundKey8, "OnFoundKey8"),
	DEFINE_OUTPUT(m_OnFoundKey9, "OnFoundKey9"),
	DEFINE_OUTPUT(m_OnFoundKey10, "OnFoundKey10"),
	DEFINE_OUTPUT(m_OnSpawn, "OnKeySystemSpawn"),

	//
	// Inputs
	//
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundRedKey", InputRedKey),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundBlueKey", InputBlueKey),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundYellowKey", InputYellowKey),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundGreenKey", InputGreenKey),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey1", InputKey1),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey2", InputKey2),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey3", InputKey3),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey4", InputKey4),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey5", InputKey5),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey6", InputKey6),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey7", InputKey7),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey8", InputKey8),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey9", InputKey9),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey10", InputKey10),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundSpecialKey", InputSpecialKey),

END_DATADESC()

void CLogicKeySystem::Activate(void)
{
	BaseClass::Activate();

	if (m_OnSpawn.NumberOfElements() > 0)
	{
		SetNextThink(gpGlobals->curtime + 0.01);
	}
}

void CLogicKeySystem::Think(void)
{
	m_OnSpawn.FireOutput(this, this);
}

void CLogicKeySystem::InputRedKey(inputdata_t& inputdata)
{
	m_OnFoundRedKey.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputBlueKey(inputdata_t& inputdata)
{
	m_OnFoundBlueKey.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputYellowKey(inputdata_t& inputdata)
{
	m_OnFoundYellowKey.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputGreenKey(inputdata_t& inputdata)
{
	m_OnFoundGreenKey.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey1(inputdata_t &inputdata)
{
	m_OnFoundKey1.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey2(inputdata_t &inputdata)
{
	m_OnFoundKey2.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey3(inputdata_t &inputdata)
{
	m_OnFoundKey3.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey4(inputdata_t &inputdata)
{
	m_OnFoundKey4.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey5(inputdata_t &inputdata)
{
	m_OnFoundKey5.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey6(inputdata_t &inputdata)
{
	m_OnFoundKey6.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey7(inputdata_t &inputdata)
{
	m_OnFoundKey7.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey8(inputdata_t &inputdata)
{
	m_OnFoundKey8.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey9(inputdata_t &inputdata)
{
	m_OnFoundKey9.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputKey10(inputdata_t &inputdata)
{
	m_OnFoundKey10.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void CLogicKeySystem::InputSpecialKey(inputdata_t &inputdata)
{
	m_OnFoundSpecialKey.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

