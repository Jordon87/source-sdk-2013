//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#include "cbase.h"
#include "particles_simple.h"
#include "citadel_effects_shared.h"
#include "particles_attractor.h"
#include "iefx.h"
#include "dlight.h"
#include "clienteffectprecachesystem.h"
#include "c_te_effect_dispatch.h"
#include "fx_quad.h"

#include "c_ai_basenpc.h"
#include "episodic/c_vort_charge_token.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_NPC_Vortigaunt_Enhanced : public C_NPC_Vortigaunt
{
	DECLARE_CLASS(C_NPC_Vortigaunt_Enhanced, C_NPC_Vortigaunt);
	DECLARE_CLIENTCLASS();

public:
};

IMPLEMENT_CLIENTCLASS_DT(C_NPC_Vortigaunt_Enhanced, DT_NPC_Vortigaunt_Enhanced, CNPC_Vortigaunt_Enhanced)
END_RECV_TABLE()