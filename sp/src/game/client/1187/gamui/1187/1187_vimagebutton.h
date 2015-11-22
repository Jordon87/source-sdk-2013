//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef ELEVENEIGHTYSEVEN_VIMAGEBUTTON_H
#define ELEVENEIGHTYSEVEN_VIMAGEBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Button.h>

class C1187ImageButton : public vgui::Button
{
	DECLARE_CLASS_SIMPLE(C1187ImageButton, vgui::Button);
public:

	C1187ImageButton(vgui::Panel *parent, const char *panelName, const char *text, vgui::Panel *pActionSignalTarget = NULL, const char *pCmd = NULL);
	C1187ImageButton(vgui::Panel *parent, const char *panelName, const wchar_t *text, vgui::Panel *pActionSignalTarget = NULL, const char *pCmd = NULL);

	bool HasDefaultTexture() const { return m_textureId_default != -1; }
	bool HasArmedTexture() const { return m_textureId_armed != -1; }
	bool HasSelectedTexture() const { return m_textureId_selected != -1; }
	bool HasDepressedTexture() const { return m_textureId_depressed != -1; }

	int	GetButtonDefaultTextureId() const { return m_textureId_default; }
	int	GetButtonArmedTextureId() const { return m_textureId_armed; }
	int	GetButtonSelectedTextureId() const { return m_textureId_selected; }
	int	GetButtonDepressedTextureId() const { return m_textureId_depressed; }

	void SetButtonDefaultTexture(const char* texture);
	void SetButtonArmedTexture(const char* texture);
	void SetButtonSelectedTexture(const char* texture);
	void SetButtonDepressedTexture(const char* texture);

protected:

	virtual void Paint(void);
	virtual void PaintBackground(void);

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:

	bool CreateButtonTextureId(const char* texture, int& id, bool autoBindTexture = true);
	bool SetupButtonTexture(const char* texture, int& id);

protected:

	int m_textureId_default;
	int m_textureId_armed;
	int m_textureId_selected;
	int m_textureId_depressed;
};


#endif // ELEVENEIGHTYSEVEN_VIMAGEBUTTON_H