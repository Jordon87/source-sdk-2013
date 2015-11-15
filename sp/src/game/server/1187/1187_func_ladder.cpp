//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "triggers.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C1187FuncLadder : public CBaseTrigger
{
	DECLARE_CLASS(C1187FuncLadder, CBaseTrigger);
public:
	DECLARE_DATADESC();

	virtual void Spawn(void);

	virtual void Touch( CBaseEntity* pOther );
};

LINK_ENTITY_TO_CLASS(func_ladder, C1187FuncLadder);

BEGIN_DATADESC(C1187FuncLadder)
	DEFINE_ENTITYFUNC(Touch),
END_DATADESC()

void C1187FuncLadder::Spawn(void)
{
	BaseClass::Spawn();

	InitTrigger();
}

void C1187FuncLadder::Touch(CBaseEntity *pOther)
{
	BaseClass::Touch(pOther);

	if (pOther && pOther->IsPlayer())
	{
		pOther->SetMoveType( MOVETYPE_FLY );
	}
}