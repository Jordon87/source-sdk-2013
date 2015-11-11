//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 1187 specific input handling
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "kbutton.h"
#include "input.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: 1187 Input interface
//-----------------------------------------------------------------------------
class C1187Input : public CInput
{
public:

};


static C1187Input g_Input;

// Expose this interface
IInput *input = ( IInput * )&g_Input;
