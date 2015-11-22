//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"

#include <vgui_controls/Controls.h>
#include <vgui_controls/Button.h>

#include "tier1/KeyValues.h"
#include "vgui/ISurface.h"
#include "vgui/IScheme.h"

#include "1187_vimagebutton.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(C1187ImageButton, C1187ImageButton);

C1187ImageButton::C1187ImageButton(Panel *parent, const char *panelName, const char *text, Panel *pActionSignalTarget, const char *pCmd)
	: BaseClass(parent, panelName, text, pActionSignalTarget, pCmd)
{
	SetAutoDelete(true);
	SetMouseInputEnabled(true);
	
	m_textureId_default		= -1;
	m_textureId_armed		= -1;
	m_textureId_selected	= -1;
	m_textureId_depressed	= -1;
}

C1187ImageButton::C1187ImageButton(Panel *parent, const char *panelName, const wchar_t *text, Panel *pActionSignalTarget, const char *pCmd)
	: BaseClass(parent, panelName, text, pActionSignalTarget, pCmd)
{
	SetAutoDelete(true);
	SetMouseInputEnabled(true);

	m_textureId_default		= -1;
	m_textureId_armed		= -1;
	m_textureId_selected	= -1;
	m_textureId_depressed	= -1;
}


void C1187ImageButton::Paint(void)
{
	surface()->DrawSetTextColor(GetFgColor());
	SetFgColor(Color(0, 0, 0, 0));

	BaseClass::Paint();

	surface()->DrawSetColor(GetBgColor());

	if (HasArmedTexture() && IsArmed())
	{
		surface()->DrawSetTexture(m_textureId_armed);
	}
	else if (HasSelectedTexture() && IsSelected())
	{
		surface()->DrawSetTexture(m_textureId_selected);
	}
	else if (HasDepressedTexture() && IsDepressed())
	{
		surface()->DrawSetTexture(m_textureId_depressed);
	}
	else if (HasDefaultTexture())
	{
		surface()->DrawSetTexture(m_textureId_default);
	}

	int wide, tall;
	GetSize(wide, tall);

	surface()->DrawTexturedRect(0, 0, wide, tall);
}

void C1187ImageButton::PaintBackground(void)
{
	BaseClass::PaintBackground();

	surface()->DrawSetColor( GetBgColor() );

	if ( HasArmedTexture() && IsArmed())
	{
		surface()->DrawSetTexture( m_textureId_armed );
	}
	else if (HasSelectedTexture() && IsSelected())
	{
		surface()->DrawSetTexture( m_textureId_selected );
	}
	else if (HasDepressedTexture() && IsDepressed())
	{
		surface()->DrawSetTexture( m_textureId_depressed );
	}
	else if (HasDefaultTexture())
	{
		surface()->DrawSetTexture( m_textureId_default );
	}

	int wide, tall;
	GetSize(wide, tall);

	surface()->DrawTexturedRect(0, 0, wide, tall);
}

void C1187ImageButton::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetButtonBorderEnabled(false);
	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);

	SetFgColor(Color(0, 0, 0, 0));
	SetBgColor(Color(0, 0, 0, 0));

	SetArmedSound("UI/1187menuover.wav");
	SetReleasedSound("UI/1187menuclick.wav");
}

void C1187ImageButton::SetButtonDefaultTexture(const char* texture)
{
	if (!SetupButtonTexture(texture, m_textureId_default))
	{
		Warning("Failed to properly setup default button texture %s.\n", texture);
	}
}

void C1187ImageButton::SetButtonArmedTexture(const char* texture)
{
	if (!SetupButtonTexture(texture, m_textureId_armed))
	{
		Warning("Failed to properly setup armed button texture %s.\n", texture);
	}
}

void C1187ImageButton::SetButtonSelectedTexture(const char* texture)
{
	if (!SetupButtonTexture(texture, m_textureId_selected))
	{
		Warning("Failed to properly setup selected button texture %s.\n", texture);
	}
}

void C1187ImageButton::SetButtonDepressedTexture(const char* texture)
{
	if (!SetupButtonTexture(texture, m_textureId_depressed))
	{
		Warning("Failed to properly setup depressed button texture %s.\n", texture);
	}
}

bool C1187ImageButton::CreateButtonTextureId(const char* texture, int& id, bool autoBindTexture)
{
	Assert( texture );

	bool success = false;

	id = surface()->CreateNewTextureID();

	if (surface()->IsTextureIDValid(id))
	{
		if (autoBindTexture)
		{
			surface()->DrawSetTextureFile(id, texture, true, false);
		}

		success = true;
	}
	else
	{
		id = -1;
	}

	return success;
}

bool C1187ImageButton::SetupButtonTexture(const char* texture, int& id)
{
	bool success = false;

	if (!texture)
		return success;

	if (CreateButtonTextureId(texture, id))
	{
		success = true;
	}

	return success;
}