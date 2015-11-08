//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef DANGEROUSWORLD_WEAPON_CSTUN_H
#define DANGEROUSWORLD_WEAPON_CSTUN_H

#ifdef _WIN32
#pragma once
#endif

#ifdef CLIENT_DLL
class C_WeaponCStun;
#define CWeaponCStun C_WeaponCStun
#endif

class CWeaponCStun : public CBaseHLBludgeonWeapon
{
	DECLARE_CLASS(CWeaponCStun, CBaseHLBludgeonWeapon);
public:

	CWeaponCStun();

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

#ifndef CLIENT_DLL
	DECLARE_ACTTABLE();
#endif

#ifdef CLIENT_DLL
	virtual int				DrawModel(int flags);
	virtual void			ClientThink(void);
	virtual void			OnDataChanged(DataUpdateType_t updateType);
	virtual RenderGroup_t	GetRenderGroup(void);
	virtual void			ViewModelDrawn(C_BaseViewModel *pBaseViewModel);

#endif

	virtual void Precache();

	void		Spawn();

	virtual void AddViewKick(void);

	float		GetRange(void);
	float		GetFireRate(void);

	bool		Deploy(void);
	bool		Holster(CBaseCombatWeapon *pSwitchingTo = NULL);

	void		Drop(const Vector &vecVelocity);
	void		ImpactEffect(trace_t &traceHit);
	void		SecondaryAttack(void)	{}
	void		SetStunState(bool state);
	bool		GetStunState(void);

#ifndef CLIENT_DLL
	void		Operator_HandleAnimEvent(animevent_t *pEvent, CBaseCombatCharacter *pOperator);
	int			WeaponMeleeAttack1Condition(float flDot, float flDist);
#endif

	float		GetDamageForActivity(Activity hitActivity);

	virtual void	MeleeHit(trace_t &trace);
	virtual void	MeleeHitWorld(trace_t &trace);

	CWeaponCStun(const CWeaponCStun &);

private:

#ifdef CLIENT_DLL

#define	NUM_BEAM_ATTACHMENTS	9

	struct stunstickBeamInfo_t
	{
		int IDs[2];		// 0 - top, 1 - bottom
	};

	stunstickBeamInfo_t		m_BeamAttachments[NUM_BEAM_ATTACHMENTS];	// Lookup for arc attachment points on the head of the stick
	int						m_BeamCenterAttachment;						// "Core" of the effect (center of the head)

	void	SetupAttachmentPoints(void);
	void	DrawFirstPersonEffects(void);
	void	DrawThirdPersonEffects(void);
	void	DrawEffects(void);
	bool	InSwing(void);

	bool	m_bSwungLastFrame;

#define	FADE_DURATION	0.25f

	float	m_flFadeTime;

#endif

	CNetworkVar(bool, m_bActive);
};

#endif // DANGEROUSWORLD_WEAPON_CSTUN_H