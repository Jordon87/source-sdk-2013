//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_triage_player.h"
#include "playerandobjectenumerator.h"
#include "engine/ivdebugoverlay.h"
#include "c_ai_basenpc.h"
#include "in_buttons.h"
#include "collisionutils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_CLIENTCLASS_DT(C_Triage_Player, DT_Triage_Player, CTriage_Player)
	RecvPropDataTable( RECVINFO_DT(m_TriageLocal),0, &REFERENCE_RECV_TABLE(DT_TriageLocal) ),
END_RECV_TABLE()

BEGIN_PREDICTION_DATA( C_Triage_Player )
	DEFINE_PRED_TYPEDESCRIPTION( m_TriageLocal, C_TriagePlayerLocalData ),
END_PREDICTION_DATA()

C_Triage_Player::C_Triage_Player()
{

}
