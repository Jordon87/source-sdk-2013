//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HOE_PROP_CORPSE_H
#define HOE_PROP_CORPSE_H

#ifdef _WIN32
#pragma once
#endif

#include "baseanimating.h"


class CHoe_Prop_Corpse : public CBaseAnimating
{
	DECLARE_CLASS(CHoe_Prop_Corpse, CBaseAnimating);
public:
	DECLARE_DATADESC();

	void Spawn(void);
	void Precache(void);
	bool KeyValue(const char *szKeyName, const char *szValue);
	virtual void SelectModel(void) {}

	virtual const char*		GetPose(int i)			= 0;
	virtual int				GetPoseCount() const	= 0;

	void CorpseThink(void);

private:
	void InitCorpse();

private:
	int m_iPose;
};

#endif // HOE_PROP_CORPSE_H