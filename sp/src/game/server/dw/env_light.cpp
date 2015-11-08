//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "weapon_flaregun.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CEnvLightDW : public CFlare
{
	DECLARE_CLASS(CEnvLightDW, CFlare);
};

LINK_ENTITY_TO_CLASS(env_light, CEnvLightDW);