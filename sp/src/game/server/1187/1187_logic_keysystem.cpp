//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "baseentity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C1187LogicKeySystem : public CLogicalEntity
{
	DECLARE_CLASS(C1187LogicKeySystem, CLogicalEntity);
public:
	DECLARE_DATADESC();

protected:

	// Inputs
	void InputFoundKey1(inputdata_t &inputdata);
	void InputFoundKey2(inputdata_t &inputdata);
	void InputFoundKey3(inputdata_t &inputdata);
	void InputFoundSpecialKey(inputdata_t &inputdata);

	// Outputs
	COutputEvent m_OnFoundKey1;
	COutputEvent m_OnFoundKey2;
	COutputEvent m_OnFoundKey3;
	COutputEvent m_OnFoundSpecialKey;

private:

	void SendFoundKeyMessage(CBasePlayer* pPlayer);

};

LINK_ENTITY_TO_CLASS(logic_keysystem, C1187LogicKeySystem);

//-----------------------------
// Save & Restore
//-----------------------------

BEGIN_DATADESC(C1187LogicKeySystem)

	//
	// Inputs
	//
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey1", InputFoundKey1),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey2", InputFoundKey2),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundKey3", InputFoundKey3),
	DEFINE_INPUTFUNC(FIELD_VOID, "FoundSpecialKey", InputFoundSpecialKey),

	//
	// Outputs
	//
	DEFINE_OUTPUT(m_OnFoundKey1, "OnFoundKey1"),
	DEFINE_OUTPUT(m_OnFoundKey2, "OnFoundKey2"),
	DEFINE_OUTPUT(m_OnFoundKey3, "OnFoundKey3"),
	DEFINE_OUTPUT(m_OnFoundSpecialKey, "OnFoundSpecialKey"),

END_DATADESC()

void C1187LogicKeySystem::SendFoundKeyMessage(CBasePlayer* pPlayer)
{
	Assert( pPlayer );

	CSingleUserRecipientFilter user(pPlayer);
	user.MakeReliable();
	UserMessageBegin(user, "HintKeyDisplay");
		WRITE_BYTE(1);	// So that the message has a size.
	MessageEnd();
}

void C1187LogicKeySystem::InputFoundKey1(inputdata_t &inputdata)
{
	// Send message.
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (pPlayer)
	{
		SendFoundKeyMessage(pPlayer);
	}

	m_OnFoundKey1.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void C1187LogicKeySystem::InputFoundKey2(inputdata_t &inputdata)
{
	// Send message.
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (pPlayer)
	{
		SendFoundKeyMessage(pPlayer);
	}

	m_OnFoundKey2.FireOutput(inputdata.pActivator, inputdata.pCaller);
}


void C1187LogicKeySystem::InputFoundKey3(inputdata_t &inputdata)
{
	// Send message.
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (pPlayer)
	{
		SendFoundKeyMessage(pPlayer);
	}

	m_OnFoundKey3.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

void C1187LogicKeySystem::InputFoundSpecialKey(inputdata_t &inputdata)
{
	// Send message.
	CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
	if (pPlayer)
	{
		SendFoundKeyMessage(pPlayer);
	}

	m_OnFoundSpecialKey.FireOutput(inputdata.pActivator, inputdata.pCaller);
}

