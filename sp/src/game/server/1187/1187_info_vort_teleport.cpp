//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "baseentity.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C1187InfoVortTeleport : public CPointEntity
{
public:
	DECLARE_CLASS(C1187InfoVortTeleport, CPointEntity);

	void	Spawn(void);
};

//info targets are like point entities except you can force them to spawn on the client
void C1187InfoVortTeleport::Spawn(void)
{
	BaseClass::Spawn();

	if (HasSpawnFlags(0x01))
	{
		SetEFlags(EFL_FORCE_CHECK_TRANSMIT);
	}
}

LINK_ENTITY_TO_CLASS(info_vort_teleport, C1187InfoVortTeleport);