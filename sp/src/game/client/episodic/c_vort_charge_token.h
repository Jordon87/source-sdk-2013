//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=====================================================================================//

#ifndef C_VORT_CHARGE_TOKEN_H
#define C_VORT_CHARGE_TOKEN_H

#if defined ( ELEVENEIGHTYSEVEN_CLIENT_DLL )

#include "c_ai_basenpc.h"

class C_NPC_Vortigaunt : public C_AI_BaseNPC
{
	DECLARE_CLASS(C_NPC_Vortigaunt, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

public:
	virtual void	OnDataChanged(DataUpdateType_t updateType);
	virtual void	ClientThink(void);
	virtual void	ReceiveMessage(int classID, bf_read &msg);

public:
	bool  m_bIsBlue;           ///< wants to fade to blue
	float m_flBlueEndFadeTime; ///< when to end fading from one skin to another

	bool  m_bIsBlack;    ///< wants to fade to black (networked)
	float m_flBlackFade; ///< [0.00 .. 1.00] where 1.00 is all black. Locally interpolated.
};
#endif

#endif // C_VORT_CHARGE_TOKEN_H