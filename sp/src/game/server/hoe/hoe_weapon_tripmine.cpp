//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hoe_basecombatweapon.h"
#include "player.h"
#include "gamerules.h"
#include "hoe_grenade_tripmine.h"
#include "entitylist.h"
#include "hoe_weapon_tripmine.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "engine/IEngineSound.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	SLAM_PRIMARY_VOLUME		450

BEGIN_DATADESC(CHoe_Weapon_Tripmine)

DEFINE_FIELD(m_tSlamState, FIELD_INTEGER),
DEFINE_FIELD(m_bNeedReload, FIELD_BOOLEAN),
DEFINE_FIELD(m_bClearReload, FIELD_BOOLEAN),
DEFINE_FIELD(m_bAttachTripmine, FIELD_BOOLEAN),

// Function Pointers
DEFINE_FUNCTION(TripmineThink),
DEFINE_FUNCTION(TripmineTouch),

END_DATADESC()


IMPLEMENT_SERVERCLASS_ST(CHoe_Weapon_Tripmine, DT_Hoe_Weapon_Tripmine)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(weapon_tripmine, CHoe_Weapon_Tripmine);
PRECACHE_WEAPON_REGISTER(weapon_tripmine);

acttable_t	CHoe_Weapon_Tripmine::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_SLAM, true },
};

IMPLEMENT_ACTTABLE(CHoe_Weapon_Tripmine);


void CHoe_Weapon_Tripmine::Spawn()
{
	BaseClass::Spawn();

	Precache();

	UTIL_SetSize(this, Vector(-4, -4, -2), Vector(4, 4, 2));

	FallInit();// get ready to fall down

	SetThink(NULL);

	m_tSlamState = SLAM_TRIPMINE_READY;

	// Give 1 piece of default ammo when first picked up
	m_iClip2 = 1;
}

void CHoe_Weapon_Tripmine::Precache(void)
{
	BaseClass::Precache();

	UTIL_PrecacheOther("npc_tripmine");
}

//------------------------------------------------------------------------------
// Purpose : Override to use slam's pickup touch function
// Input   :
// Output  :
//------------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::SetPickupTouch(void)
{
	SetTouch(&CHoe_Weapon_Tripmine::TripmineTouch);
}

//-----------------------------------------------------------------------------
// Purpose: Override so give correct ammo
// Input  : pOther - the entity that touched me
// Output :
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::TripmineTouch(CBaseEntity *pOther)
{
	CBaseCombatCharacter* pBCC = ToBaseCombatCharacter(pOther);

	// Can I even pick stuff up?
	if (pBCC && !pBCC->IsAllowedToPickupWeapons())
		return;

	// ---------------------------------------------------
	//  First give weapon to touching entity if allowed
	// ---------------------------------------------------
	BaseClass::DefaultTouch(pOther);

	// ----------------------------------------------------
	//  Give slam ammo if touching client
	// ----------------------------------------------------
	if (pOther->GetFlags() & FL_CLIENT)
	{
		// ------------------------------------------------
		//  If already owned weapon of this type remove me
		// ------------------------------------------------
		CHoe_Weapon_Tripmine* oldWeapon = (CHoe_Weapon_Tripmine*)pBCC->Weapon_OwnsThisType(GetClassname());
		if (oldWeapon != this)
		{
			UTIL_Remove(this);
		}
		else
		{
			pBCC->GiveAmmo(1, m_iPrimaryAmmoType);
			SetThink(NULL);
		}
	}
}

//------------------------------------------------------------------------------
// Purpose :
// Input   :
// Output  :
//------------------------------------------------------------------------------
bool CHoe_Weapon_Tripmine::Holster(CBaseCombatWeapon *pSwitchingTo)
{
	SetThink(NULL);
	return BaseClass::Holster(pSwitchingTo);
}

//-----------------------------------------------------------------------------
// Purpose: SLAM has no reload, but must call weapon idle to update state
// Input  :
// Output :
//-----------------------------------------------------------------------------
bool CHoe_Weapon_Tripmine::Reload(void)
{
	WeaponIdle();
	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::PrimaryAttack(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();
	if (!pOwner)
	{
		return;
	}

	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		return;
	}

	switch (m_tSlamState)
	{
	case SLAM_TRIPMINE_READY:
		if (CanAttachTripmine())
		{
			StartTripmineAttach();
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Secondary attack switches between satchel charge and tripmine mode
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::SecondaryAttack(void)
{
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::TripmineAttach(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();
	if (!pOwner)
	{
		return;
	}

	m_bAttachTripmine = false;

	Vector vecSrc = pOwner->Weapon_ShootPosition();
	Vector vecAiming = pOwner->EyeDirection3D();

	trace_t tr;

	UTIL_TraceLine(vecSrc, vecSrc + (vecAiming * 128), MASK_SOLID, pOwner, COLLISION_GROUP_NONE, &tr);

	if (tr.fraction < 1.0)
	{
		CBaseEntity *pEntity = tr.m_pEnt;
		if (pEntity && !(pEntity->GetFlags() & FL_CONVEYOR))
		{
			QAngle angles;
			VectorAngles(tr.plane.normal, angles);
			//angles.x += 90;

			CBaseEntity *pEnt = CBaseEntity::Create("npc_tripmine", tr.endpos + tr.plane.normal /** 3*/ * 8, angles, NULL);

			CHoe_Grenade_Tripmine *pMine = (CHoe_Grenade_Tripmine *)pEnt;
			pMine->m_hOwner = GetOwner();

			pOwner->RemoveAmmo(1, m_iPrimaryAmmoType);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::StartTripmineAttach(void)
{
	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer(GetOwner());
	if (!pPlayer)
	{
		return;
	}

	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	Vector vecAiming = pPlayer->BodyDirection3D();

	trace_t tr;

	UTIL_TraceLine(vecSrc, vecSrc + (vecAiming * 128), MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr);

	if (tr.fraction < 1.0)
	{
		// ALERT( at_console, "hit %f\n", tr.flFraction );

		CBaseEntity *pEntity = tr.m_pEnt;
		if (pEntity && !(pEntity->GetFlags() & FL_CONVEYOR))
		{
			// player "shoot" animation
			pPlayer->SetAnimation(PLAYER_ATTACK1);

			// -----------------------------------------
			//  Play attach animation
			// -----------------------------------------
			SendWeaponAnim(ACT_VM_TRIPMINE_ARM1);

			m_bNeedReload = true;
			m_bAttachTripmine = true;
		}
		else
		{
			// ALERT( at_console, "no deploy\n" );
		}
	}
	m_flNextPrimaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
	m_flNextSecondaryAttack = gpGlobals->curtime + GetViewModelSequenceDuration();
	//	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::SetSlamState(SlamState_t newState)
{
	// Set set and set idle time so animation gets updated with state change
	m_tSlamState = newState;
	SetWeaponIdleTime(gpGlobals->curtime);
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::TripmineThink(void)
{
	SetNextThink(gpGlobals->curtime + 0.1f);
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
bool CHoe_Weapon_Tripmine::CanAttachTripmine(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();
	if (!pOwner)
	{
		return false;
	}

	Vector vecSrc = pOwner->Weapon_ShootPosition();
	Vector vecAiming = pOwner->BodyDirection2D();

	trace_t tr;

	Vector	vecEnd = vecSrc + (vecAiming * 42);
	UTIL_TraceLine(vecSrc, vecEnd, MASK_SOLID, pOwner, COLLISION_GROUP_NONE, &tr);

	if (tr.fraction < 1.0)
	{
		// Don't attach to a living creature
		if (tr.m_pEnt)
		{
			CBaseEntity *pEntity = tr.m_pEnt;
			CBaseCombatCharacter *pBCC = ToBaseCombatCharacter(pEntity);
			if (pBCC)
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::ItemPostFrame(void)
{
	CBasePlayer *pOwner = ToBasePlayer(GetOwner());
	if (!pOwner)
	{
		return;
	}

	if (!m_bNeedReload && (pOwner->m_nButtons & IN_ATTACK) && (m_flNextPrimaryAttack <= gpGlobals->curtime))
	{
		PrimaryAttack();
	}

	// -----------------------
	//  No buttons down
	// -----------------------
	else
	{
		WeaponIdle();
		return;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Switch to next best weapon
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::WeaponSwitch(void)
{
	// Note that we may pick the SLAM again, when we switch
	// weapons, in which case we have to save and restore the 
	// detonator armed state.
	// The SLAMs may be about to blow up, but haven't done so yet
	// and the deploy function will find the undetonated charges
	// and we are armed
	CBaseCombatCharacter *pOwner = GetOwner();
	pOwner->SwitchToNextBestWeapon(pOwner->GetActiveWeapon());

	// If not armed and have no ammo
	if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
	{
		pOwner->ClearActiveWeapon();
	}

}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CHoe_Weapon_Tripmine::WeaponIdle(void)
{
	// Ready to switch animations?
	if (HasWeaponIdleTimeElapsed())
	{
		if (m_bClearReload)
		{
			m_bNeedReload = false;
			m_bClearReload = false;
		}
		CBaseCombatCharacter *pOwner = GetOwner();
		if (!pOwner)
		{
			return;
		}

		int iAnim = 0;

		if (m_bAttachTripmine)
		{
			TripmineAttach();
			iAnim = ACT_VM_TRIPMINE_PLACE;
		}
		else if (m_bNeedReload)
		{
			// If owner had ammo draw the correct SLAM type
			if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) > 0)
			{
				switch (m_tSlamState)
				{
				case SLAM_TRIPMINE_READY:
				{
					iAnim = ACT_VM_DRAW;
				}
				break;
				}
				m_bClearReload = true;
			}
			else
			{
				pOwner->Weapon_Drop(this);
				UTIL_Remove(this);
			}
		}
		else if (pOwner->GetAmmoCount(m_iPrimaryAmmoType) <= 0)
		{
			pOwner->Weapon_Drop(this);
			UTIL_Remove(this);
		}

		// If I don't need to reload just do the appropriate idle
		else
		{
			switch (m_tSlamState)
			{
			case SLAM_TRIPMINE_READY:
			{
				iAnim = ACT_VM_IDLE;
			}
			break;
			}
		}
		SendWeaponAnim(iAnim);
	}
}

bool CHoe_Weapon_Tripmine::Deploy(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();
	if (!pOwner)
	{
		return false;
	}

	SetThink(&CHoe_Weapon_Tripmine::TripmineThink);
	SetNextThink(gpGlobals->curtime + 0.1f);

	SetModel(GetViewModel());

	// If detonator is already armed
	m_bNeedReload = false;

	return DefaultDeploy((char*)GetViewModel(), (char*)GetWorldModel(), ACT_VM_DRAW, (char*)GetAnimPrefix());
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
// Input  :
// Output :
//-----------------------------------------------------------------------------
CHoe_Weapon_Tripmine::CHoe_Weapon_Tripmine(void)
{
	m_tSlamState = SLAM_SATCHEL_THROW;
	m_bNeedReload = true;
	m_bClearReload = false;
	m_bAttachTripmine = false;

}