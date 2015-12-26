//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HOE_PROP_CORPSE_GRUNT_H
#define HOE_PROP_CORPSE_GRUNT_H

#ifdef _WIN32
#pragma once
#endif

#include "hoe_prop_corpse.h"

class CHoe_Prop_Corpse_Grunt : public CHoe_Prop_Corpse
{
	DECLARE_CLASS(CHoe_Prop_Corpse_Grunt, CHoe_Prop_Corpse);
public:
	const char*	GetPose(int i)			{ return m_pszPoses[i]; }
	int			GetPoseCount() const	{ return ARRAYSIZE(m_pszPoses); }

private:
	static const char* m_pszPoses[8];
};

#endif // HOE_PROP_CORPSE_GRUNT_H