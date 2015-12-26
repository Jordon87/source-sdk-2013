//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HOE_NPC_PEASANT_DEFS_H
#define HOE_NPC_PEASANT_DEFS_H

#ifdef _WIN32
#pragma once
#endif

//
// Peasant models
//
#define NPC_PEASANT_FEMALE1_MODEL	"models/peasant/female1.mdl"
#define NPC_PEASANT_FEMALE2_MODEL	"models/peasant/female2.mdl"
#define NPC_PEASANT_FEMALE3_MODEL	"models/peasant/female3.mdl"
#define NPC_PEASANT_MALE1_MODEL		"models/peasant/male1.mdl"
#define NPC_PEASANT_MALE2_MODEL		"models/peasant/male2.mdl"
#define NPC_PEASANT_MALE3_MODEL		"models/peasant/male3.mdl"

//
// Peasant body
//
enum
{
	PEASANT_BODY_RANDOM = -1,
	PEASANT_BODY_RANDOM_MALE = -2,
	PEASANT_BODY_RANDOM_FEMALE = -3,
	PEASANT_BODY_MALE1 = 0,
	PEASANT_BODY_MALE2,
	PEASANT_BODY_MALE3,
	PEASANT_BODY_FEMALE1,
	PEASANT_BODY_FEMALE2,
	PEASANT_BODY_FEMALE3,

	//
	// Add new body IDs here...
	//

	PEASANT_BODY_COUNT, // <-- Last body count.
};

#endif // HOE_NPC_PEASANT_DEFS_H