//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_weapon__stubs.h"
#include "triage_basecombatweapon_shared.h"
#include "c_triage_basecombatweapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Default HL2 weapons.
STUB_WEAPON_CLASS(weapon_357, TriageWeapon357, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_ar2, TriageWeaponAR2, C_TriageMachineGun);
STUB_WEAPON_CLASS(weapon_bugbait, TriageWeaponBugBait, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_crossbow, TriageWeaponCrossbow, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_crowbar, TriageWeaponCrowbar, C_BaseTriageBludgeonWeapon);
STUB_WEAPON_CLASS(weapon_frag, TriageWeaponFrag, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_pistol, TriageWeaponPistol, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_rpg, TriageWeaponRPG, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_shotgun, TriageWeaponShotgun, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_smg1, TriageWeaponSMG1, C_TriageSelectFireMachineGun);

// Triage weapons.
STUB_WEAPON_CLASS(weapon_ak47, TriageWeaponAK47, C_TriageSelectFireMachineGun);
STUB_WEAPON_CLASS(weapon_ak47_s, TriageWeaponAK47Silenced, C_TriageSelectFireMachineGun);
STUB_WEAPON_CLASS(weapon_huntingshotgun, TriageWeaponHuntingShotgun, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_pistol_t, TriageWeaponPistolTactical, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_pistol_t_s, TriageWeaponPistolTacticalSilenced, C_BaseTriageCombatWeapon);
STUB_WEAPON_CLASS(weapon_smg1_s, TriageWeaponSMG1Silenced, C_TriageSelectFireMachineGun);