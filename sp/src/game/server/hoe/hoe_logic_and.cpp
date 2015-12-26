//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================

#include "cbase.h"
#include "entityinput.h"
#include "entityoutput.h"
#include "eventqueue.h"
#include "mathlib/mathlib.h"
#include "globalstate.h"
#include "ndebugoverlay.h"
#include "saverestore_utlvector.h"
#include "vstdlib/random.h"
#include "gameinterface.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define LOGIC_AND_VALUES_MAX	8

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CHoe_Logic_And : public CLogicalEntity
{
	DECLARE_CLASS(CHoe_Logic_And, CLogicalEntity);

protected:

	void Test(void);

	void ToggleValue(int value);

private:

	bool m_Values[LOGIC_AND_VALUES_MAX];

	// Inputs
	void InputToggleValue01(inputdata_t &inputdata);
	void InputToggleValue02(inputdata_t &inputdata);
	void InputToggleValue03(inputdata_t &inputdata);
	void InputToggleValue04(inputdata_t &inputdata);
	void InputToggleValue05(inputdata_t &inputdata);
	void InputToggleValue06(inputdata_t &inputdata);
	void InputToggleValue07(inputdata_t &inputdata);
	void InputToggleValue08(inputdata_t &inputdata);

	// Outputs
	COutputEvent m_OnAllTrue;
	COutputEvent m_OnAllFalse;
	COutputEvent m_OnMixed;

	DECLARE_DATADESC();
};

LINK_ENTITY_TO_CLASS(logic_and, CHoe_Logic_And);


BEGIN_DATADESC(CHoe_Logic_And)

	// Keys
	DEFINE_KEYFIELD(m_Values[0], FIELD_BOOLEAN, "Value01"),
	DEFINE_KEYFIELD(m_Values[1], FIELD_BOOLEAN, "Value02"),
	DEFINE_KEYFIELD(m_Values[2], FIELD_BOOLEAN, "Value03"),
	DEFINE_KEYFIELD(m_Values[3], FIELD_BOOLEAN, "Value04"),
	DEFINE_KEYFIELD(m_Values[4], FIELD_BOOLEAN, "Value05"),
	DEFINE_KEYFIELD(m_Values[5], FIELD_BOOLEAN, "Value06"),
	DEFINE_KEYFIELD(m_Values[6], FIELD_BOOLEAN, "Value07"),
	DEFINE_KEYFIELD(m_Values[7], FIELD_BOOLEAN, "Value08"),


	// Inputs
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleValue01", InputToggleValue01),
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleValue02", InputToggleValue02),
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleValue03", InputToggleValue03),
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleValue04", InputToggleValue04),
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleValue05", InputToggleValue05),
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleValue06", InputToggleValue06),
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleValue07", InputToggleValue07),
	DEFINE_INPUTFUNC(FIELD_VOID, "ToggleValue08", InputToggleValue08),

	// Outputs
	DEFINE_OUTPUT(m_OnAllTrue, "OnAllTrue"),
	DEFINE_OUTPUT(m_OnAllFalse, "OnAllFalse"),
	DEFINE_OUTPUT(m_OnMixed, "OnMixed"),

END_DATADESC()

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoe_Logic_And::InputToggleValue01(inputdata_t &inputdata)
{
	ToggleValue(1);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoe_Logic_And::InputToggleValue02(inputdata_t &inputdata)
{
	ToggleValue(2);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoe_Logic_And::InputToggleValue03(inputdata_t &inputdata)
{
	ToggleValue(3);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoe_Logic_And::InputToggleValue04(inputdata_t &inputdata)
{
	ToggleValue(4);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoe_Logic_And::InputToggleValue05(inputdata_t &inputdata)
{
	ToggleValue(5);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoe_Logic_And::InputToggleValue06(inputdata_t &inputdata)
{
	ToggleValue(6);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoe_Logic_And::InputToggleValue07(inputdata_t &inputdata)
{
	ToggleValue(7);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoe_Logic_And::InputToggleValue08(inputdata_t &inputdata)
{
	ToggleValue(8);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

void CHoe_Logic_And::ToggleValue(int value)
{
	if (value <= 0 || value > LOGIC_AND_VALUES_MAX)
	{
		Warning("%s: Invalid value param specified %d.\n", GetClassname(), value);
		return;
	}
		
	// Fix up index.
	value--;

	// Toggle indexed value.
	m_Values[value] = !m_Values[value];

	// Test values.
	Test();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CHoe_Logic_And::Test(void)
{
	enum
	{
		VALUE_NONE	= 0,
		VALUE_FALSE = 1,
		VALUE_TRUE	= 2,
	};

	int fOneIsTrue = VALUE_NONE, fOneIsFalse = VALUE_NONE;

	for (size_t i = 0; i < LOGIC_AND_VALUES_MAX; i++)
	{
		if (m_Values[i])
			fOneIsTrue = VALUE_TRUE;
		else
			fOneIsFalse = VALUE_TRUE;
	}

	//  No false values implies all are true. 
	if (fOneIsFalse == VALUE_NONE)
	{
		m_OnAllTrue.FireOutput(this, this);
	}
	//  No true values implies all are false. 
	else if (fOneIsTrue == VALUE_NONE)
	{
		m_OnAllFalse.FireOutput(this, this);
	}
	// If at least one of each is set, then it is mixed.
	else
	{
		m_OnMixed.FireOutput(this, this);
	}
}