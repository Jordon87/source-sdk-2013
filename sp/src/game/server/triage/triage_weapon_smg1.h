#ifndef TRIAGE_WEAPON_SMG1_H
#define TRIAGE_WEAPON_SMG1_H

#ifdef _WIN32
#pragma once
#endif

#include "triage_baseweapon_smg.h"

class CTriageWeaponSMG1 : public CTriage_BaseWeapon_SMG
{
public:
	DECLARE_CLASS(CTriageWeaponSMG1, CTriage_BaseWeapon_SMG);
	DECLARE_SERVERCLASS();
};

#endif // TRIAGE_WEAPON_SMG1_H