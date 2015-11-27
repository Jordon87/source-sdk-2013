//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================

#ifndef WEAPON_CROSSBOW_H
#define WEAPON_CROSSBOW_H

#ifdef _WIN32
#pragma once
#endif

#if defined ( TRIAGE_DLL )
#define BOLT_AIR_VELOCITY	2500
#define BOLT_WATER_VELOCITY	1500

#define	BOLT_SKIN_NORMAL	0
#define BOLT_SKIN_GLOW		1

//-----------------------------------------------------------------------------
// Crossbow Bolt
//-----------------------------------------------------------------------------
class CCrossbowBolt : public CBaseCombatCharacter
{
	DECLARE_CLASS(CCrossbowBolt, CBaseCombatCharacter);

public:
	CCrossbowBolt() { };
	~CCrossbowBolt();

	Class_T Classify(void) { return CLASS_NONE; }

public:
	void Spawn(void);
	void Precache(void);
	void BubbleThink(void);
	void BoltTouch(CBaseEntity *pOther);
	bool CreateVPhysics(void);
	unsigned int PhysicsSolidMaskForEntity() const;
	static CCrossbowBolt *BoltCreate(const Vector &vecOrigin, const QAngle &angAngles, CBasePlayer *pentOwner = NULL);

protected:

	bool	CreateSprites(void);

	CHandle<CSprite>		m_pGlowSprite;
	//CHandle<CSpriteTrail>	m_pGlowTrail;

	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
};
#endif // defined ( TRIAGE_DLL )

#endif // WEAPON_CROSSBOW_H
