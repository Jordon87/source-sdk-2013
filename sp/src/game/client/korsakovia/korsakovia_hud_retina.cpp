//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include "iclientmode.h"
#include "c_basehlplayer.h"
#include "VGuiMatSurface/IMatSystemSurface.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imesh.h"
#include "materialsystem/imaterialvar.h"
#include "../hud_crosshair.h"

#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include <vgui_controls/AnimationController.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Draws the zoom screen
//-----------------------------------------------------------------------------
class CKorsakoviaHudRetina : public vgui::Panel, public CHudElement
{
	DECLARE_CLASS_SIMPLE(CKorsakoviaHudRetina, vgui::Panel);

public:
	CKorsakoviaHudRetina(const char *pElementName);
	
	bool	ShouldDraw( void );
	void	Init( void );
	void	LevelInit( void );

protected:
	virtual void ApplySchemeSettings(vgui::IScheme *scheme);
	virtual void Paint( void );

private:
	bool	m_bZoomOn;
	float	m_flZoomStartTime;
	bool	m_bPainted;

	CMaterialReference m_retina[4];
	CMaterialReference m_health[4];
};

DECLARE_HUDELEMENT(CKorsakoviaHudRetina);

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CKorsakoviaHudRetina::CKorsakoviaHudRetina(const char *pElementName) : CHudElement(pElementName), BaseClass(NULL, "HudZoom")
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );
	
	// SetHiddenBits( HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD | HIDEHUD_NEEDSUIT );
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CKorsakoviaHudRetina::Init(void)
{
	m_bZoomOn = false;
	m_bPainted = false;
	m_flZoomStartTime = -999.0f;

	m_retina[0].Init("HUD/healthtl", TEXTURE_GROUP_VGUI);
	m_retina[1].Init("HUD/healthtr", TEXTURE_GROUP_VGUI);
	m_retina[2].Init("HUD/healthbr", TEXTURE_GROUP_VGUI);
	m_retina[3].Init("HUD/healthbl", TEXTURE_GROUP_VGUI);

	m_health[0].Init("HUD/health10", TEXTURE_GROUP_VGUI);
	m_health[1].Init("HUD/health25", TEXTURE_GROUP_VGUI);
	m_health[2].Init("HUD/health50", TEXTURE_GROUP_VGUI);
	m_health[3].Init("HUD/health75", TEXTURE_GROUP_VGUI);
}

//-----------------------------------------------------------------------------
// Purpose: standard hud element init function
//-----------------------------------------------------------------------------
void CKorsakoviaHudRetina::LevelInit(void)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: sets scheme colors
//-----------------------------------------------------------------------------
void CKorsakoviaHudRetina::ApplySchemeSettings(vgui::IScheme *scheme)
{
	BaseClass::ApplySchemeSettings(scheme);

	SetPaintBackgroundEnabled(false);
	SetPaintBorderEnabled(false);
	SetFgColor(scheme->GetColor("ZoomReticleColor", GetFgColor()));

	SetForceStereoRenderToFrameBuffer( true );
	int x, y;
	int screenWide, screenTall;
	surface()->GetFullscreenViewport( x, y, screenWide, screenTall );
	SetBounds(0, 0, screenWide, screenTall);
}

//-----------------------------------------------------------------------------
// Purpose: Save CPU cycles by letting the HUD system early cull
// costly traversal.  Called per frame, return true if thinking and 
// painting need to occur.
//-----------------------------------------------------------------------------
bool CKorsakoviaHudRetina::ShouldDraw(void)
{
	bool bNeedsDraw = false;

	C_BaseHLPlayer *pPlayer = dynamic_cast<C_BaseHLPlayer *>(C_BasePlayer::GetLocalPlayer());
	if ( pPlayer == NULL )
		return false;

#if 1
	if (pPlayer->GetHealth() < 100)
	{
		bNeedsDraw = true; 
	}
#else
	if ( pPlayer->m_HL2Local.m_bZooming )
	{
		// need to paint
		bNeedsDraw = true;
	}
	else if ( m_bPainted )
	{
		// keep painting until state is finished
		bNeedsDraw = true;
	}
#endif

	return ( bNeedsDraw && CHudElement::ShouldDraw() );
}

#define	ZOOM_FADE_TIME	0.4f
//-----------------------------------------------------------------------------
// Purpose: draws the zoom effect
//-----------------------------------------------------------------------------
void CKorsakoviaHudRetina::Paint(void)
{
	m_bPainted = false;

	// see if we're zoomed any
	C_BaseHLPlayer *pPlayer = dynamic_cast<C_BaseHLPlayer *>(C_BasePlayer::GetLocalPlayer());
	if ( pPlayer == NULL )
		return;
	
	float alpha = 1.0f;

	Color col(255, 255, 255, alpha * 255);

	surface()->DrawSetColor( col );
	
	// draw zoom circles
	float fX, fY;
	bool bBehindCamera = false;
	CHudCrosshair::GetDrawPosition( &fX, &fY, &bBehindCamera );
	if( bBehindCamera )
		return;

	int wide, tall;
	GetSize( wide, tall );

	float x0 = 0.0f, x1 = fX * 0.5f, x2 = wide - fX * 0.5f, x3 = wide;
	float y0 = 0.0f, y1 = fY * 0.5f, y2 = tall - fY * 0.5f, y3 = tall;

	float uv1 = 1.0f - (1.0f / 255.0f);
	float uv2 = 0.0f + (1.0f / 255.0f);

	struct coord_t
	{
		float x, y;
		float u, v;
	};
	coord_t coords[20] = 
	{
		// top-left
		{ x0, y0, uv2, uv2 }, //  uv1, uv2
		{ x1, y0, uv1, uv2 }, //  uv2, uv2
		{ x1, y1, uv1, uv1 }, //  uv2, uv1
		{ x0, y1, uv2, uv1 }, //  uv1, uv1

		// top-right
		{ x2, y0, uv2, uv2 }, //  uv2, uv2
		{ x3, y0, uv1, uv2 }, //  uv1, uv2
		{ x3, y1, uv1, uv1 }, //  uv1, uv1
		{ x2, y1, uv2, uv1 }, //  uv2, uv1

		// bottom-right
		{ x2, y2, uv2, uv2 }, // uv2, uv1
		{ x3, y2, uv1, uv2 }, // uv1, uv1
		{ x3, y3, uv1, uv1 }, // uv1, uv2
		{ x2, y3, uv2, uv1 }, // uv2, uv2

		// bottom-left
		{ x0, y2, uv2, uv2 }, // uv1, uv1
		{ x1, y2, uv1, uv2 }, // uv2, uv1
		{ x1, y3, uv1, uv1 }, // uv2, uv2
		{ x0, y3, uv2, uv1 }, // uv1, uv2

		// full screen.
		{ x0, y0, uv2, uv2 }, // uv1, uv1
		{ x3, y0, uv1, uv2 }, // uv2, uv1
		{ x3, y3, uv1, uv1 }, // uv2, uv2
		{ x0, y3, uv2, uv1 }, // uv1, uv2
	};

	int i;

	CMatRenderContextPtr pRenderContext(materials);
	IMesh *pMesh = pRenderContext->GetDynamicMesh(true, NULL, NULL, NULL);
	
	CMeshBuilder meshBuilder;

	// Draw the red health border.
	if (pPlayer->GetHealth() <= 75)
	{
		if (pPlayer->GetHealth() <= 50)
		{
			if (pPlayer->GetHealth() <= 25)
			{
				if (pPlayer->GetHealth() <= 10)
				{
					pRenderContext->Bind(m_health[0]);

					meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

					for (i = 16; i < 20; i++)
					{
						meshBuilder.Color4f(0.0, 0.0, 0.0, alpha);
						meshBuilder.TexCoord2f(0, coords[i].u, coords[i].v);
						meshBuilder.Position3f(coords[i].x, coords[i].y, 0.0f);
						meshBuilder.AdvanceVertex();
					}

					meshBuilder.End();

					pMesh->Draw();
				}
				else
				{
					pRenderContext->Bind(m_health[1]);

					meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

					for (i = 16; i < 20; i++)
					{
						meshBuilder.Color4f(0.0, 0.0, 0.0, alpha);
						meshBuilder.TexCoord2f(0, coords[i].u, coords[i].v);
						meshBuilder.Position3f(coords[i].x, coords[i].y, 0.0f);
						meshBuilder.AdvanceVertex();
					}

					meshBuilder.End();

					pMesh->Draw();
				}
			}
			else
			{
				pRenderContext->Bind(m_health[2]);

				meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

				for (i = 16; i < 20; i++)
				{
					meshBuilder.Color4f(0.0, 0.0, 0.0, alpha);
					meshBuilder.TexCoord2f(0, coords[i].u, coords[i].v);
					meshBuilder.Position3f(coords[i].x, coords[i].y, 0.0f);
					meshBuilder.AdvanceVertex();
				}

				meshBuilder.End();

				pMesh->Draw();
			}
		}
		else
		{
			pRenderContext->Bind(m_health[3]);

			meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

			for (i = 16; i < 20; i++)
			{
				meshBuilder.Color4f(0.0, 0.0, 0.0, alpha);
				meshBuilder.TexCoord2f(0, coords[i].u, coords[i].v);
				meshBuilder.Position3f(coords[i].x, coords[i].y, 0.0f);
				meshBuilder.AdvanceVertex();
			}

			meshBuilder.End();

			pMesh->Draw();
		}
	}

	// Draw the retina.
	pRenderContext->Bind(m_retina[0]);

	meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

	for (i = 0; i < 4; i++)
	{
		meshBuilder.Color4f( 0.0, 0.0, 0.0, alpha );
		meshBuilder.TexCoord2f( 0, coords[i].u, coords[i].v );
		meshBuilder.Position3f( coords[i].x, coords[i].y, 0.0f );
		meshBuilder.AdvanceVertex();
	}

	meshBuilder.End();

	pMesh->Draw();

	if (pPlayer->GetHealth() < 75)
	{
		pRenderContext->Bind(m_retina[1]);

		meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

		for (i = 4; i < 8; i++)
		{
			meshBuilder.Color4f(0.0, 0.0, 0.0, alpha);
			meshBuilder.TexCoord2f(0, coords[i].u, coords[i].v);
			meshBuilder.Position3f(coords[i].x, coords[i].y, 0.0f);
			meshBuilder.AdvanceVertex();
		}

		meshBuilder.End();

		pMesh->Draw();

		if (pPlayer->GetHealth() < 50)
		{
			pRenderContext->Bind(m_retina[2]);

			meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

			for (i = 8; i < 12; i++)
			{
				meshBuilder.Color4f(0.0, 0.0, 0.0, alpha);
				meshBuilder.TexCoord2f(0, coords[i].u, coords[i].v);
				meshBuilder.Position3f(coords[i].x, coords[i].y, 0.0f);
				meshBuilder.AdvanceVertex();
			}

			meshBuilder.End();

			pMesh->Draw();

			if (pPlayer->GetHealth() < 25)
			{
				pRenderContext->Bind(m_retina[3]);

				meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

				for (i = 12; i < 16; i++)
				{
					meshBuilder.Color4f(0.0, 0.0, 0.0, alpha);
					meshBuilder.TexCoord2f(0, coords[i].u, coords[i].v);
					meshBuilder.Position3f(coords[i].x, coords[i].y, 0.0f);
					meshBuilder.AdvanceVertex();
				}

				meshBuilder.End();

				pMesh->Draw();
			}
		}
	}

	m_bPainted = true;
}
