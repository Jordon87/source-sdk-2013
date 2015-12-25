//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_hoe_player.h"
#include "playerandobjectenumerator.h"
#include "engine/ivdebugoverlay.h"
#include "c_ai_basenpc.h"
#include "in_buttons.h"
#include "collisionutils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT(C_Hoe_Player, DT_Hoe_Player, CHoe_Player)
	RecvPropDataTable( RECVINFO_DT(m_HoeLocal),0, &REFERENCE_RECV_TABLE(DT_HoeLocal) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA(C_Hoe_Player)
	DEFINE_PRED_TYPEDESCRIPTION( m_HoeLocal, C_HoePlayerLocalData ),
END_PREDICTION_DATA()


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
C_Hoe_Player::C_Hoe_Player() : C_BaseHLPlayer()
{
}
