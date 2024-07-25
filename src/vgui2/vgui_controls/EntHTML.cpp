//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
// This class is a message box that has two buttons, ok and cancel instead of
// just the ok button of a message box. We use a message box class for the ok button
// and implement another button here.
//
// $NoKeywords: $
//=============================================================================//

#include "vgui_controls/pch_vgui_controls.h"
#include "vgui_controls/EntHTML.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/Menu.h>
#include <vgui_controls/MessageBox.h>

#include "filesystem.h"
#include "../vgui2/src/vgui_key_translation.h"

#undef PostMessage
#undef MessageBox

#include "OfflineMode.h"

#include "../../game/client/cdll_client_int.h"
#include <string>

// memdbgon must be the last include file in a .cpp file
#include "tier0/memdbgon.h"

using namespace vgui;
using namespace std;

#define DEFAULT_ACTION		"trigger"

#define HTML_ALLOW			true
#define HTML_DISALLOW		false

const int k_nMaxCustomCursors = 2; // the max number of custom cursors we keep cached PER html control

bool EntityParse (const char *url)
{
	string addr = url;
	string newadr = "";
	string entity = "";
	string entity_name = "";
	string entity_action = "";
	char   buffer[128];

		strcpy(buffer, "EntityParse(");
		strcat(buffer, url);
		strcat(buffer, ")\n");
		DevMsg(buffer);

		// Check error url's
		if(addr == "\\" || addr == "/")
		{
			strcpy(buffer, "Ignoring html target: ");
			strcat(buffer, url);
			strcat(buffer, "\n");
			DevMsg(buffer);
			return HTML_DISALLOW;
		}

		// Check for entity:// escaping
		if(addr.find("entity://") != 0)
		{
			strcpy(buffer, "Not escaping url: ");
			strcat(buffer, url);
			strcat(buffer, "\n");
			DevMsg(buffer);
			return HTML_ALLOW;
		}

		{
			strcpy(buffer, "Escaping url: ");
			strcat(buffer, url);
			strcat(buffer, "\n");
			DevMsg(buffer);
		}

		// We're escaping into entity outputs
		// Find the end of entity stuff
		unsigned int entity_end = addr.find(";");
		if(entity_end == string::npos)
			entity_end = addr.length() - 9;	// ; not found. no html redirect
		else {
			entity_end -= 9;
			if(entity_end <= 0) {
				DevMsg("Aborting url escape\n");
				return HTML_DISALLOW;			// Our address is "entity://;" ? weird
			}
		}
		entity = addr.substr(9, entity_end);

		// Get the url to redirect to
		unsigned int url_end = addr.length() - (entity_end + 1);
		if(url_end <= 0)
			newadr = "";				// No url given
		else
			newadr = addr.substr(entity_end + 10, url_end);

		// Get the entity name and action
		entity_end = entity.find("->");
		if(entity_end != string::npos) {
			// We have our "->", grab the name and action
			entity_name = entity.substr(0, entity_end);
			int action_end = entity.length() - (entity_end + 2);
			if(action_end <= 0)
				entity_action = DEFAULT_ACTION;	// Action not given ("ent->")
			else
				entity_action = entity.substr(entity_end + 2, action_end);
		} else {
			// No action ("ent")
			entity_action = DEFAULT_ACTION;
			entity_name   = entity;
		}

		// Open the new url
		if(newadr != "" && newadr != "/")	// "/" is odd, workaround
		{
			{
				strcpy(buffer, "Running OpenURL(");
				strcat(buffer, newadr.c_str());
				strcat(buffer, ")\n");
				DevMsg(buffer);
			}
			char command[255];

			strcpy(command, "cl_htmltarget ");
			strcat(command, newadr.c_str());
			
			strcpy(buffer, "Execing command: ");
			strcat(buffer, command);
			strcat(buffer, "\n");
			DevMsg(buffer);

			engine->ClientCmd(command);
		}

		// Fire the event
		{
			char command[255];

			strcpy(command, "sv_ent_fire ");
			strcat(command, entity_name.c_str());
			strcat(command, " ");
			strcat(command, entity_action.c_str());
			{
				strcpy(buffer, "Execing command: ");
				strcat(buffer, command);
				strcat(buffer, "\n");
				DevMsg(buffer);
			}
			engine->ServerCmd(command);
		}

	return HTML_DISALLOW;
}

//-----------------------------------------------------------------------------
// Purpose: A simple passthrough panel to render the border onto the HTML widget
//-----------------------------------------------------------------------------
class EntHTMLInterior : public Panel
{
	DECLARE_CLASS_SIMPLE(EntHTMLInterior, Panel);
public:
	EntHTMLInterior(EntHTML* parent) : BaseClass(parent, "EntHTMLInterior")
	{
		m_pEntHTML = parent;
		SetPaintBackgroundEnabled(false);
		SetKeyBoardInputEnabled(false);
		SetMouseInputEnabled(false);
	}

private:
	EntHTML* m_pEntHTML;
};


//-----------------------------------------------------------------------------
// Purpose: a vgui container for popup menus displayed by a control, only 1 menu for any control can be visible at a time
//-----------------------------------------------------------------------------
class EntHTMLComboBoxHost : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE(EntHTMLComboBoxHost, EditablePanel);
public:
	EntHTMLComboBoxHost(EntHTML* parent, const char* panelName) : EditablePanel(parent, panelName)
	{
		m_pParent = parent;
		MakePopup(false);
	}
	~EntHTMLComboBoxHost() {}

	virtual void PaintBackground();

	virtual void OnMousePressed(MouseCode code);
	virtual void OnMouseReleased(MouseCode code);
	virtual void OnCursorMoved(int x, int y);
	virtual void OnMouseDoublePressed(MouseCode code);
	virtual void OnKeyTyped(wchar_t unichar);
	virtual void OnKeyCodeTyped(KeyCode code);
	virtual void OnKeyCodeReleased(KeyCode code);
	virtual void OnMouseWheeled(int delta);

	virtual void OnKillFocus()
	{
		if (vgui::input()->GetFocus() != m_pParent->GetVPanel()) // if its not our parent trying to steal focus
		{
			BaseClass::OnKillFocus();
			if (m_pParent)
				m_pParent->HidePopup();
		}
	}

	virtual void PerformLayout()
	{
		// no op the perform layout as we just render the html controls popup texture into it
		// we don't want the menu logic trying to play with its size
	}


private:
	EntHTML* m_pParent;
};


//-----------------------------------------------------------------------------
// Purpose: container class for any external popup windows the browser requests
//-----------------------------------------------------------------------------
class EntHTMLPopup : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(EntHTMLPopup, vgui::Frame);
	class PopupHTML : public vgui::EntHTML
	{
		DECLARE_CLASS_SIMPLE(PopupHTML, vgui::EntHTML);
	public:
		PopupHTML(Frame* parent, const char* pchName, bool allowJavaScript, bool bPopupWindow) : EntHTML(parent, pchName, allowJavaScript, bPopupWindow) { m_pParent = parent; }

		virtual void OnSetHTMLTitle(const char* pchTitle)
		{
			BaseClass::OnSetHTMLTitle(pchTitle);
			m_pParent->SetTitle(pchTitle, true);
		}

	private:
		Frame* m_pParent;
	};
public:
	EntHTMLPopup(Panel* parent, const char* pchURL, const char* pchTitle) : Frame(NULL, "HtmlPopup", true)
	{
		m_pHTML = new PopupHTML(this, "htmlpopupchild", true, true);
		m_pHTML->OpenURL(pchURL, NULL, false);
		SetTitle(pchTitle, true);
	}

	~EntHTMLPopup()
	{
	}

	enum
	{
		vert_inset = 40,
		horiz_inset = 6
	};

	void PerformLayout()
	{
		BaseClass::PerformLayout();
		int wide, tall;
		GetSize(wide, tall);
		m_pHTML->SetPos(horiz_inset, vert_inset);
		m_pHTML->SetSize(wide - horiz_inset * 2, tall - vert_inset * 2);
	}

	void SetBounds(int x, int y, int wide, int tall)
	{
		BaseClass::SetBounds(x, y, wide + horiz_inset * 2, tall + vert_inset * 2);
	}

	MESSAGE_FUNC(OnCloseWindow, "OnCloseWindow")
	{
		Close();
	}
private:
	PopupHTML* m_pHTML;
};


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
EntHTML::EntHTML(Panel* parent, const char* name, bool allowJavaScript, bool bPopupWindow) : Panel(parent, name),
m_NeedsPaint(this, &EntHTML::BrowserNeedsPaint),
m_ComboNeedsPaint(this, &EntHTML::BrowserComboNeedsPaint),
m_StartRequest(this, &EntHTML::BrowserStartRequest),
m_URLChanged(this, &EntHTML::BrowserURLChanged),
m_FinishedRequest(this, &EntHTML::BrowserFinishedRequest),
m_ShowPopup(this, &EntHTML::BrowserShowPopup),
m_HidePopup(this, &EntHTML::BrowserHidePopup),
m_SizePopup(this, &EntHTML::BrowserSizePopup),
m_LinkInNewTab(this, &EntHTML::BrowserOpenNewTab),
m_ChangeTitle(this, &EntHTML::BrowserSetHTMLTitle),
m_FileLoadDialog(this, &EntHTML::BrowserFileLoadDialog),
m_SearchResults(this, &EntHTML::BrowserSearchResults),
m_CloseBrowser(this, &EntHTML::BrowserClose),
m_HorizScroll(this, &EntHTML::BrowserHorizontalScrollBarSizeResponse),
m_VertScroll(this, &EntHTML::BrowserVerticalScrollBarSizeResponse),
m_LinkAtPosResp(this, &EntHTML::BrowserLinkAtPositionResponse),
m_JSAlert(this, &EntHTML::BrowserJSAlert),
m_JSConfirm(this, &EntHTML::BrowserJSConfirm),
m_CanGoBackForward(this, &EntHTML::BrowserCanGoBackandForward),
m_NewWindow(this, &EntHTML::BrowserPopupHTMLWindow),
m_SetCursor(this, &EntHTML::BrowserSetCursor),
m_StatusText(this, &EntHTML::BrowserStatusText),
m_ShowTooltip(this, &EntHTML::BrowserShowToolTip),
m_UpdateTooltip(this, &EntHTML::BrowserUpdateToolTip),
m_HideTooltip(this, &EntHTML::BrowserHideToolTip)
{
	m_iHTMLTextureID = 0;
	m_iComboBoxTextureID = 0;
	m_bCanGoBack = false;
	m_bCanGoForward = false;
	m_bInFind = false;
	m_bRequestingDragURL = false;
	m_bRequestingCopyLink = false;
	m_flZoom = 100.0f;
	m_bNeedsFullTextureUpload = false;

	m_pInteriorPanel = new EntHTMLInterior(this);
	SetPostChildPaintEnabled(true);

	m_unBrowserHandle = INVALID_HTTMLBROWSER;
	m_SteamAPIContext.Init();
	if (m_SteamAPIContext.SteamHTMLSurface())
	{
		m_SteamAPIContext.SteamHTMLSurface()->Init();
		SteamAPICall_t hSteamAPICall = m_SteamAPIContext.SteamHTMLSurface()->CreateBrowser(surface()->GetWebkitHTMLUserAgentString(), NULL);
		m_SteamCallResultBrowserReady.Set(hSteamAPICall, this, &EntHTML::OnBrowserReady);
	}
	else
	{
		Warning("Unable to access SteamHTMLSurface");
	}
	m_iScrollBorderX = m_iScrollBorderY = 0;
	m_bScrollBarEnabled = true;
	m_bContextMenuEnabled = true;
	m_bNewWindowsOnly = false;
	m_iMouseX = m_iMouseY = 0;
	m_iDragStartX = m_iDragStartY = 0;
	m_nViewSourceAllowedIndex = -1;
	m_iWideLastHTMLSize = m_iTalLastHTMLSize = 0;

	_hbar = new ScrollBar(this, "HorizScrollBar", false);
	_hbar->SetVisible(false);
	_hbar->AddActionSignalTarget(this);

	_vbar = new ScrollBar(this, "VertScrollBar", true);
	_vbar->SetVisible(false);
	_vbar->AddActionSignalTarget(this);

	m_pFindBar = new EntHTML::CEntHTMLFindBar(this);
	m_pFindBar->SetZPos(2);
	m_pFindBar->SetVisible(false);

	m_pComboBoxHost = new EntHTMLComboBoxHost(this, "ComboBoxHost");
	m_pComboBoxHost->SetPaintBackgroundEnabled(true);
	m_pComboBoxHost->SetVisible(false);

	m_pContextMenu = new Menu(this, "contextmenu");
	m_pContextMenu->AddMenuItem("#vgui_HTMLBack", new KeyValues("Command", "command", "back"), this);
	m_pContextMenu->AddMenuItem("#vgui_HTMLForward", new KeyValues("Command", "command", "forward"), this);
	m_pContextMenu->AddMenuItem("#vgui_HTMLReload", new KeyValues("Command", "command", "reload"), this);
	m_pContextMenu->AddMenuItem("#vgui_HTMLStop", new KeyValues("Command", "command", "stop"), this);
	m_pContextMenu->AddSeparator();
	m_pContextMenu->AddMenuItem("#vgui_HTMLCopyUrl", new KeyValues("Command", "command", "copyurl"), this);
	m_iCopyLinkMenuItemID = m_pContextMenu->AddMenuItem("#vgui_HTMLCopyLink", new KeyValues("Command", "command", "copylink"), this);
	m_pContextMenu->AddMenuItem("#TextEntry_Copy", new KeyValues("Command", "command", "copy"), this);
	m_pContextMenu->AddMenuItem("#TextEntry_Paste", new KeyValues("Command", "command", "paste"), this);
	m_pContextMenu->AddSeparator();
	m_nViewSourceAllowedIndex = m_pContextMenu->AddMenuItem("#vgui_HTMLViewSource", new KeyValues("Command", "command", "viewsource"), this);
}


//-----------------------------------------------------------------------------
// Purpose: browser is ready to show pages
//-----------------------------------------------------------------------------
void EntHTML::OnBrowserReady(HTML_BrowserReady_t* pBrowserReady, bool bIOFailure)
{
	m_unBrowserHandle = pBrowserReady->unBrowserHandle;
	BrowserResize();

	if (!m_sPendingURLLoad.IsEmpty())
	{
		PostURL(m_sPendingURLLoad, m_sPendingPostData, false);
		m_sPendingURLLoad.Clear();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
EntHTML::~EntHTML()
{
	m_pContextMenu->MarkForDeletion();

	if (m_SteamAPIContext.SteamHTMLSurface())
	{
		m_SteamAPIContext.SteamHTMLSurface()->RemoveBrowser(m_unBrowserHandle);
	}

	FOR_EACH_VEC(m_vecHCursor, i)
	{
		// BR FIXME!
//		surface()->DeleteCursor( m_vecHCursor[i].m_Cursor );
	}
	m_vecHCursor.RemoveAll();
}


//-----------------------------------------------------------------------------
// Purpose: Handle message to change our cursor
//-----------------------------------------------------------------------------
void EntHTML::OnSetCursorVGUI(int cursor)
{
	SetCursor((HCursor)cursor);
}

//-----------------------------------------------------------------------------
// Purpose: sets up colors/fonts/borders
//-----------------------------------------------------------------------------
void EntHTML::ApplySchemeSettings(IScheme* pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	BrowserResize();
}


//-----------------------------------------------------------------------------
// Purpose: overrides panel class, paints a texture of the HTML window as a background
//-----------------------------------------------------------------------------
void EntHTML::Paint()
{
	//VPROF_BUDGET( "HTML::Paint()", VPROF_BUDGETGROUP_OTHER_VGUI );
	BaseClass::Paint();

	if (m_iHTMLTextureID != 0)
	{
		surface()->DrawSetTexture(m_iHTMLTextureID);
		int tw = 0, tt = 0;
		surface()->DrawSetColor(Color(255, 255, 255, 255));
		GetSize(tw, tt);
		surface()->DrawTexturedRect(0, 0, tw, tt);
	}

	// If we have scrollbars, we need to draw the bg color under them, since the browser
	// bitmap is a checkerboard under them, and they are transparent in the in-game client
	if (m_iScrollBorderX > 0 || m_iScrollBorderY > 0)
	{
		int w, h;
		GetSize(w, h);
		IBorder* border = GetBorder();
		int left = 0, top = 0, right = 0, bottom = 0;
		if (border)
		{
			border->GetInset(left, top, right, bottom);
		}
		surface()->DrawSetColor(GetBgColor());
		if (m_iScrollBorderX)
		{
			surface()->DrawFilledRect(w - m_iScrollBorderX - right, top, w, h - bottom);
		}
		if (m_iScrollBorderY)
		{
			surface()->DrawFilledRect(left, h - m_iScrollBorderY - bottom, w - m_iScrollBorderX - right, h);
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: paint the combo box texture if we have one
//-----------------------------------------------------------------------------
void EntHTML::PaintComboBox()
{
	BaseClass::Paint();
	if (m_iComboBoxTextureID != 0)
	{
		surface()->DrawSetTexture(m_iComboBoxTextureID);
		surface()->DrawSetColor(Color(255, 255, 255, 255));
		int tw = m_allocedComboBoxWidth;
		int tt = m_allocedComboBoxHeight;
		surface()->DrawTexturedRect(0, 0, tw, tt);
	}

}


//-----------------------------------------------------------------------------
// Purpose: overrides panel class, paints a texture of the HTML window as a background
//-----------------------------------------------------------------------------
void EntHTMLComboBoxHost::PaintBackground()
{
	BaseClass::PaintBackground();

	m_pParent->PaintComboBox();
}


//-----------------------------------------------------------------------------
// Purpose: causes a repaint when the layout changes
//-----------------------------------------------------------------------------
void EntHTML::PerformLayout()
{
	BaseClass::PerformLayout();
	Repaint();
	int vbarInset = _vbar->IsVisible() ? _vbar->GetWide() : 0;
	int maxw = GetWide() - vbarInset;
	m_pInteriorPanel->SetBounds(0, 0, maxw, GetTall());

	IScheme* pClientScheme = vgui::scheme()->GetIScheme(vgui::scheme()->GetScheme("ClientScheme"));

	int iSearchInsetY = 5;
	int iSearchInsetX = 5;
	int iSearchTall = 24;
	int iSearchWide = 150;
	const char* resourceString = pClientScheme->GetResourceString("HTML.SearchInsetY");
	if (resourceString)
	{
		iSearchInsetY = atoi(resourceString);
	}
	resourceString = pClientScheme->GetResourceString("HTML.SearchInsetX");
	if (resourceString)
	{
		iSearchInsetX = atoi(resourceString);
	}
	resourceString = pClientScheme->GetResourceString("HTML.SearchTall");
	if (resourceString)
	{
		iSearchTall = atoi(resourceString);
	}
	resourceString = pClientScheme->GetResourceString("HTML.SearchWide");
	if (resourceString)
	{
		iSearchWide = atoi(resourceString);
	}

	m_pFindBar->SetBounds(GetWide() - iSearchWide - iSearchInsetX - vbarInset, m_pFindBar->BIsHidden() ? -1 * iSearchTall - 5 : iSearchInsetY, iSearchWide, iSearchTall);
}


//-----------------------------------------------------------------------------
// Purpose: updates the underlying HTML surface widgets position
//-----------------------------------------------------------------------------
void EntHTML::OnMove()
{
	BaseClass::OnMove();

	// tell cef where we are on the screen so plugins can correctly render
	int nPanelAbsX, nPanelAbsY;
	ipanel()->GetAbsPos(GetVPanel(), nPanelAbsX, nPanelAbsY);

	if (m_pComboBoxHost && m_pComboBoxHost->IsVisible())
	{
		m_pComboBoxHost->SetVisible(false);
	}
}


//-----------------------------------------------------------------------------
// Purpose: opens the URL, will accept any URL that IE accepts
//-----------------------------------------------------------------------------
void EntHTML::OpenURL(const char* URL, const char* postData, bool force)
{
	PostURL(URL, postData, force);
}

//-----------------------------------------------------------------------------
// Purpose: opens the URL, will accept any URL that IE accepts
//-----------------------------------------------------------------------------
void EntHTML::PostURL(const char* URL, const char* pchPostData, bool force)
{
	if (m_unBrowserHandle == INVALID_HTTMLBROWSER)
	{
		m_sPendingURLLoad = URL;
		m_sPendingPostData = pchPostData;
		return;
	}

	if (IsSteamInOfflineMode() && !force)
	{
		const char* baseDir = getenv("HTML_OFFLINE_DIR");
		if (baseDir)
		{
			// get the app we need to run
			char htmlLocation[_MAX_PATH];
			char otherName[128];
			char fileLocation[_MAX_PATH];

			if (!g_pFullFileSystem->FileExists(baseDir))
			{
				Q_snprintf(otherName, sizeof(otherName), "%senglish.html", OFFLINE_FILE);
				baseDir = otherName;
			}
			g_pFullFileSystem->GetLocalCopy(baseDir); // put this file on disk for IE to load

			g_pFullFileSystem->GetLocalPath(baseDir, fileLocation, sizeof(fileLocation));
			Q_snprintf(htmlLocation, sizeof(htmlLocation), "file://%s", fileLocation);

			if (m_SteamAPIContext.SteamHTMLSurface())
				m_SteamAPIContext.SteamHTMLSurface()->LoadURL(m_unBrowserHandle, htmlLocation, NULL);
		}
		else
		{
			if (m_SteamAPIContext.SteamHTMLSurface())
				m_SteamAPIContext.SteamHTMLSurface()->LoadURL(m_unBrowserHandle, URL, NULL);
		}
	}
	else
	{
		if (pchPostData && Q_strlen(pchPostData) > 0)
		{
			if (m_SteamAPIContext.SteamHTMLSurface())
				m_SteamAPIContext.SteamHTMLSurface()->LoadURL(m_unBrowserHandle, URL, pchPostData);

		}
		else
		{
			if (m_SteamAPIContext.SteamHTMLSurface())
				m_SteamAPIContext.SteamHTMLSurface()->LoadURL(m_unBrowserHandle, URL, NULL);
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: opens the URL, will accept any URL that IE accepts
//-----------------------------------------------------------------------------
bool EntHTML::StopLoading()
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->StopLoad(m_unBrowserHandle);
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: refreshes the current page
//-----------------------------------------------------------------------------
bool EntHTML::Refresh()
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->Reload(m_unBrowserHandle);
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: Tells the browser control to go back
//-----------------------------------------------------------------------------
void EntHTML::GoBack()
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->GoBack(m_unBrowserHandle);
}


//-----------------------------------------------------------------------------
// Purpose: Tells the browser control to go forward
//-----------------------------------------------------------------------------
void EntHTML::GoForward()
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->GoForward(m_unBrowserHandle);
}


//-----------------------------------------------------------------------------
// Purpose: Checks if the browser can go back further
//-----------------------------------------------------------------------------
bool EntHTML::BCanGoBack()
{
	return m_bCanGoBack;
}


//-----------------------------------------------------------------------------
// Purpose: Checks if the browser can go forward further
//-----------------------------------------------------------------------------
bool EntHTML::BCanGoFoward()
{
	return m_bCanGoForward;
}


//-----------------------------------------------------------------------------
// Purpose: handle resizing
//-----------------------------------------------------------------------------
void EntHTML::OnSizeChanged(int wide, int tall)
{
	BaseClass::OnSizeChanged(wide, tall);
	UpdateSizeAndScrollBars();
}


//-----------------------------------------------------------------------------
// Purpose: Run javascript in the page
//-----------------------------------------------------------------------------
void EntHTML::RunJavascript(const char* pchScript)
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->ExecuteJavascript(m_unBrowserHandle, pchScript);
}




//-----------------------------------------------------------------------------
// Purpose: helper to convert UI mouse codes to CEF ones
//-----------------------------------------------------------------------------
ISteamHTMLSurface::EHTMLMouseButton EntConvertMouseCodeToCEFCode(MouseCode code)
{
	switch (code)
	{
	default:
	case MOUSE_LEFT:
		return ISteamHTMLSurface::eHTMLMouseButton_Left;
		break;
	case MOUSE_RIGHT:
		return ISteamHTMLSurface::eHTMLMouseButton_Right;
		break;
	case MOUSE_MIDDLE:
		return ISteamHTMLSurface::eHTMLMouseButton_Middle;
		break;
	}
}


//-----------------------------------------------------------------------------
// Purpose: passes mouse clicks to the control
//-----------------------------------------------------------------------------
void EntHTML::OnMousePressed(MouseCode code)
{
	m_sDragURL = NULL;

	// mouse4 = back button
	if (code == MOUSE_4)
	{
		PostActionSignal(new KeyValues("HTMLBackRequested"));
		return;
	}
	if (code == MOUSE_5)
	{
		PostActionSignal(new KeyValues("HTMLForwardRequested"));
		return;
	}


	if (code == MOUSE_RIGHT && m_bContextMenuEnabled)
	{
		GetLinkAtPosition(m_iMouseX, m_iMouseY);
		Menu::PlaceContextMenu(this, m_pContextMenu);
		return;
	}

	// ask for the focus to come to this window
	RequestFocus();

	// now tell the browser about the click
	// ignore right clicks if context menu has been disabled
	if (code != MOUSE_RIGHT)
	{
		if (m_SteamAPIContext.SteamHTMLSurface())
			m_SteamAPIContext.SteamHTMLSurface()->MouseDown(m_unBrowserHandle, EntConvertMouseCodeToCEFCode(code));
	}

	if (code == MOUSE_LEFT)
	{
		input()->GetCursorPos(m_iDragStartX, m_iDragStartY);
		int htmlx, htmly;
		ipanel()->GetAbsPos(GetVPanel(), htmlx, htmly);

		GetLinkAtPosition(m_iDragStartX - htmlx, m_iDragStartY - htmly);

		m_bRequestingDragURL = true;
		// make sure we get notified when the mouse gets released
		if (!m_sDragURL.IsEmpty())
		{
			input()->SetMouseCapture(GetVPanel());
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: passes mouse up events
//-----------------------------------------------------------------------------
void EntHTML::OnMouseReleased(MouseCode code)
{
	if (code == MOUSE_LEFT)
	{
		input()->SetMouseCapture(NULL);
		input()->SetCursorOveride(0);

		if (!m_sDragURL.IsEmpty() && input()->GetMouseOver() != GetVPanel() && input()->GetMouseOver() != NULL)
		{
			// post the text as a drag drop to the target panel
			KeyValuesAD kv("DragDrop");
			if (ipanel()->RequestInfo(input()->GetMouseOver(), kv)
				&& kv->GetPtr("AcceptPanel") != NULL)
			{
				VPANEL vpanel = (VPANEL)kv->GetPtr("AcceptPanel");
				ivgui()->PostMessage(vpanel, new KeyValues("DragDrop", "text", m_sDragURL.Get()), GetVPanel());
			}
		}
		m_sDragURL = NULL;
	}

	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->MouseUp(m_unBrowserHandle, EntConvertMouseCodeToCEFCode(code));
}


//-----------------------------------------------------------------------------
// Purpose: keeps track of where the cursor is
//-----------------------------------------------------------------------------
void EntHTML::OnCursorMoved(int x, int y)
{
	// Only do this when we are over the current panel
	if (vgui::input()->GetMouseOver() == GetVPanel())
	{
		m_iMouseX = x;
		m_iMouseY = y;

		if (m_SteamAPIContext.SteamHTMLSurface())
			m_SteamAPIContext.SteamHTMLSurface()->MouseMove(m_unBrowserHandle, m_iMouseX, m_iMouseY);
	}
	else if (!m_sDragURL.IsEmpty())
	{
		if (input()->GetMouseOver() == NULL)
		{
			// we're not over any vgui window, switch to the OS implementation of drag/drop
			// BR FIXME
//			surface()->StartDragDropText( m_sDragURL );
			m_sDragURL = NULL;
		}
	}

	if (!m_sDragURL.IsEmpty() && !input()->GetCursorOveride())
	{
		// if we've dragged far enough (in global coordinates), set to use the drag cursor
		int gx, gy;
		input()->GetCursorPos(gx, gy);
		if (abs(m_iDragStartX - gx) + abs(m_iDragStartY - gy) > 3)
		{
			//			input()->SetCursorOveride( dc_alias );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: passes double click events to the browser
//-----------------------------------------------------------------------------
void EntHTML::OnMouseDoublePressed(MouseCode code)
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->MouseDoubleClick(m_unBrowserHandle, EntConvertMouseCodeToCEFCode(code));
}


//-----------------------------------------------------------------------------
// Purpose: return the bitmask of any modifier keys that are currently down
//-----------------------------------------------------------------------------
int EntGetKeyModifiers()
{
	// Any time a key is pressed reset modifier list as well
	int nModifierCodes = 0;
	if (vgui::input()->IsKeyDown(KEY_LCONTROL) || vgui::input()->IsKeyDown(KEY_RCONTROL))
		nModifierCodes |= ISteamHTMLSurface::eHTMLKeyModifier_CrtlDown;

	if (vgui::input()->IsKeyDown(KEY_LALT) || vgui::input()->IsKeyDown(KEY_RALT))
		nModifierCodes |= ISteamHTMLSurface::eHTMLKeyModifier_AltDown;

	if (vgui::input()->IsKeyDown(KEY_LSHIFT) || vgui::input()->IsKeyDown(KEY_RSHIFT))
		nModifierCodes |= ISteamHTMLSurface::eHTMLKeyModifier_ShiftDown;

#ifdef OSX
	// for now pipe through the cmd-key to be like the control key so we get copy/paste
	if (vgui::input()->IsKeyDown(KEY_LWIN) || vgui::input()->IsKeyDown(KEY_RWIN))
		nModifierCodes |= ISteamHTMLSurface::eHTMLKeyModifier_CrtlDown;
#endif

	return nModifierCodes;
}


//-----------------------------------------------------------------------------
// Purpose: passes key presses to the browser (we don't current do this)
//-----------------------------------------------------------------------------
void EntHTML::OnKeyTyped(wchar_t unichar)
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->KeyChar(m_unBrowserHandle, unichar, (ISteamHTMLSurface::EHTMLKeyModifiers)EntGetKeyModifiers());
}


//-----------------------------------------------------------------------------
// Purpose: pop up the find dialog
//-----------------------------------------------------------------------------
void EntHTML::ShowFindDialog()
{
	IScheme* pClientScheme = vgui::scheme()->GetIScheme(vgui::scheme()->GetScheme("ClientScheme"));
	if (!pClientScheme)
		return;

	m_pFindBar->SetVisible(true);
	m_pFindBar->RequestFocus();
	m_pFindBar->SetText("");
	m_pFindBar->HideCountLabel();
	m_pFindBar->SetHidden(false);
	int x = 0, y = 0, h = 0, w = 0;
	m_pFindBar->GetBounds(x, y, w, h);
	m_pFindBar->SetPos(x, -1 * h);
	int iSearchInsetY = 0;
	const char* resourceString = pClientScheme->GetResourceString("HTML.SearchInsetY");
	if (resourceString)
	{
		iSearchInsetY = atoi(resourceString);
	}
	float flAnimationTime = 0.0f;
	resourceString = pClientScheme->GetResourceString("HTML.SearchAnimationTime");
	if (resourceString)
	{
		flAnimationTime = atof(resourceString);
	}

	GetAnimationController()->RunAnimationCommand(m_pFindBar, "ypos", iSearchInsetY, 0.0f, flAnimationTime, AnimationController::INTERPOLATOR_LINEAR);
}


//-----------------------------------------------------------------------------
// Purpose: hide the find dialog
//-----------------------------------------------------------------------------
void EntHTML::HideFindDialog()
{
	IScheme* pClientScheme = vgui::scheme()->GetIScheme(vgui::scheme()->GetScheme("ClientScheme"));
	if (!pClientScheme)
		return;

	int x = 0, y = 0, h = 0, w = 0;
	m_pFindBar->GetBounds(x, y, w, h);
	float flAnimationTime = 0.0f;
	const char* resourceString = pClientScheme->GetResourceString("HTML.SearchAnimationTime");
	if (resourceString)
	{
		flAnimationTime = atof(resourceString);
	}

	GetAnimationController()->RunAnimationCommand(m_pFindBar, "ypos", -1 * h - 5, 0.0f, flAnimationTime, AnimationController::INTERPOLATOR_LINEAR);
	m_pFindBar->SetHidden(true);
	StopFind();
}


//-----------------------------------------------------------------------------
// Purpose: is the find dialog visible?
//-----------------------------------------------------------------------------
bool EntHTML::FindDialogVisible()
{
	return m_pFindBar->IsVisible() && !m_pFindBar->BIsHidden();
}


//-----------------------------------------------------------------------------
// Purpose: passes key presses to the browser 
//-----------------------------------------------------------------------------
void EntHTML::OnKeyCodeTyped(KeyCode code)
{
	switch (code)
	{
	case KEY_PAGEDOWN:
	{
		int val = _vbar->GetValue();
		val += 200;
		_vbar->SetValue(val);
		break;
	}
	case KEY_PAGEUP:
	{
		int val = _vbar->GetValue();
		val -= 200;
		_vbar->SetValue(val);
		break;
	}
	case KEY_F5:
	{
		Refresh();
		break;
	}
	case KEY_F:
	{
		if ((input()->IsKeyDown(KEY_LCONTROL) || input()->IsKeyDown(KEY_RCONTROL))
			|| (IsOSX() && (input()->IsKeyDown(KEY_LWIN) || input()->IsKeyDown(KEY_RWIN))))
		{
			if (!FindDialogVisible())
			{
				ShowFindDialog();
			}
			else
			{
				HideFindDialog();
			}
			break;
		}
	}
	case KEY_ESCAPE:
	{
		if (FindDialogVisible())
		{
			HideFindDialog();
			break;
		}
	}
	case KEY_TAB:
	{
		if (input()->IsKeyDown(KEY_LCONTROL) || input()->IsKeyDown(KEY_RCONTROL))
		{
			// pass control-tab to parent (through baseclass)
			BaseClass::OnKeyTyped(code);
			return;
		}
		break;
	}
	}

	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->KeyDown(m_unBrowserHandle, KeyCode_VGUIToVirtualKey(code), (ISteamHTMLSurface::EHTMLKeyModifiers)EntGetKeyModifiers());
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void EntHTML::OnKeyCodeReleased(KeyCode code)
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->KeyUp(m_unBrowserHandle, KeyCode_VGUIToVirtualKey(code), (ISteamHTMLSurface::EHTMLKeyModifiers)EntGetKeyModifiers());
}


//-----------------------------------------------------------------------------
// Purpose: scrolls the vertical scroll bar on a web page
//-----------------------------------------------------------------------------
void EntHTML::OnMouseWheeled(int delta)
{
	if (_vbar && ((m_pComboBoxHost && !m_pComboBoxHost->IsVisible())))
	{
		int val = _vbar->GetValue();
		val -= (delta * 100.0 / 3.0); // 100 for every 3 lines matches chromes code
		_vbar->SetValue(val);
	}

	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->MouseWheel(m_unBrowserHandle, delta * 100.0 / 3.0);
}


//-----------------------------------------------------------------------------
// Purpose: Inserts a custom URL handler
//-----------------------------------------------------------------------------
void EntHTML::AddCustomURLHandler(const char* customProtocolName, vgui::Panel* target)
{
	int index = m_CustomURLHandlers.AddToTail();
	m_CustomURLHandlers[index].hPanel = target;
	Q_strncpy(m_CustomURLHandlers[index].url, customProtocolName, sizeof(m_CustomURLHandlers[index].url));
}


//-----------------------------------------------------------------------------
// Purpose: shared code for sizing the HTML surface window
//-----------------------------------------------------------------------------
void EntHTML::BrowserResize()
{
	if (m_unBrowserHandle == INVALID_HTTMLBROWSER)
		return;

	int w, h;
	GetSize(w, h);
	int right = 0, bottom = 0;
	// TODO::STYLE
	/*
	IAppearance *pAppearance = GetAppearance();
	int left = 0, top = 0;
	if ( pAppearance )
	{
		pAppearance->GetInset( left, top, right, bottom );
	}
	*/


	if (m_iWideLastHTMLSize != (w - m_iScrollBorderX - right) || m_iTalLastHTMLSize != (h - m_iScrollBorderY - bottom))
	{
		m_iWideLastHTMLSize = w - m_iScrollBorderX - right;
		m_iTalLastHTMLSize = h - m_iScrollBorderY - bottom;
		if (m_iTalLastHTMLSize <= 0)
		{
			SetTall(64);
			m_iTalLastHTMLSize = 64 - bottom;
		}

		{
			if (m_SteamAPIContext.SteamHTMLSurface())
				m_SteamAPIContext.SteamHTMLSurface()->SetSize(m_unBrowserHandle, m_iWideLastHTMLSize, m_iTalLastHTMLSize);
		}


		// webkit forgets the scroll offset when you resize (it saves the scroll in a DC and a resize throws away the DC)
		// so just tell it after the resize
		int scrollV = _vbar->GetValue();
		int scrollH = _hbar->GetValue();

		if (m_SteamAPIContext.SteamHTMLSurface())
			m_SteamAPIContext.SteamHTMLSurface()->SetHorizontalScroll(m_unBrowserHandle, scrollH);
		if (m_SteamAPIContext.SteamHTMLSurface())
			m_SteamAPIContext.SteamHTMLSurface()->SetVerticalScroll(m_unBrowserHandle, scrollV);
	}

}


//-----------------------------------------------------------------------------
// Purpose: when a slider moves causes the IE images to re-render itself
//-----------------------------------------------------------------------------
void EntHTML::OnSliderMoved()
{
	if (_hbar->IsVisible())
	{
		int scrollX = _hbar->GetValue();
		if (m_SteamAPIContext.SteamHTMLSurface())
			m_SteamAPIContext.SteamHTMLSurface()->SetHorizontalScroll(m_unBrowserHandle, scrollX);
	}

	if (_vbar->IsVisible())
	{
		int scrollY = _vbar->GetValue();
		if (m_SteamAPIContext.SteamHTMLSurface())
			m_SteamAPIContext.SteamHTMLSurface()->SetVerticalScroll(m_unBrowserHandle, scrollY);
	}

	// post a message that the slider has moved
	PostActionSignal(new KeyValues("HTMLSliderMoved"));
}


//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
bool EntHTML::IsScrolledToBottom()
{
	if (!_vbar->IsVisible())
		return true;

	return m_scrollVertical.m_nScroll >= m_scrollVertical.m_nMax;
}


//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
bool EntHTML::IsScrollbarVisible()
{
	return _vbar->IsVisible();
}


//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void EntHTML::SetScrollbarsEnabled(bool state)
{
	m_bScrollBarEnabled = state;
}


//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void EntHTML::SetContextMenuEnabled(bool state)
{
	m_bContextMenuEnabled = state;
}


//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void EntHTML::SetViewSourceEnabled(bool state)
{
	m_pContextMenu->SetItemVisible(m_nViewSourceAllowedIndex, state);
}


//-----------------------------------------------------------------------------
// Purpose: data accessor
//-----------------------------------------------------------------------------
void EntHTML::NewWindowsOnly(bool state)
{
	m_bNewWindowsOnly = state;
}


//-----------------------------------------------------------------------------
// Purpose: called when our children have finished painting
//-----------------------------------------------------------------------------
void EntHTML::PostChildPaint()
{
	BaseClass::PostChildPaint();
	// TODO::STYLE
	//m_pInteriorPanel->SetPaintAppearanceEnabled( true ); // turn painting back on so the IE hwnd can render this border
}


//-----------------------------------------------------------------------------
// Purpose: Adds a custom header to all requests
//-----------------------------------------------------------------------------
void EntHTML::AddHeader(const char* pchHeader, const char* pchValue)
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->AddHeader(m_unBrowserHandle, pchHeader, pchValue);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void EntHTML::OnSetFocus()
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->SetKeyFocus(m_unBrowserHandle, true);

	BaseClass::OnSetFocus();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void EntHTML::OnKillFocus()
{
	if (vgui::input()->GetFocus() != m_pComboBoxHost->GetVPanel()) // if its not the menu stealing our focus
		BaseClass::OnKillFocus();

	// Don't clear the actual html focus if a context menu is what took focus
	if (m_pContextMenu->HasFocus())
		return;

	if (m_pComboBoxHost->HasFocus())
		return;

	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->SetKeyFocus(m_unBrowserHandle, false);
}


//-----------------------------------------------------------------------------
// Purpose: webkit is telling us to use this cursor type
//-----------------------------------------------------------------------------
void EntHTML::OnCommand(const char* pchCommand)
{
	if (!Q_stricmp(pchCommand, "back"))
	{
		PostActionSignal(new KeyValues("HTMLBackRequested"));
	}
	else if (!Q_stricmp(pchCommand, "forward"))
	{
		PostActionSignal(new KeyValues("HTMLForwardRequested"));
	}
	else if (!Q_stricmp(pchCommand, "reload"))
	{
		Refresh();
	}
	else if (!Q_stricmp(pchCommand, "stop"))
	{
		StopLoading();
	}
	else if (!Q_stricmp(pchCommand, "viewsource"))
	{
		if (m_SteamAPIContext.SteamHTMLSurface())
			m_SteamAPIContext.SteamHTMLSurface()->ViewSource(m_unBrowserHandle);
	}
	else if (!Q_stricmp(pchCommand, "copy"))
	{
		if (m_SteamAPIContext.SteamHTMLSurface())
			m_SteamAPIContext.SteamHTMLSurface()->CopyToClipboard(m_unBrowserHandle);
	}
	else if (!Q_stricmp(pchCommand, "paste"))
	{
		if (m_SteamAPIContext.SteamHTMLSurface())
			m_SteamAPIContext.SteamHTMLSurface()->PasteFromClipboard(m_unBrowserHandle);
	}
	else if (!Q_stricmp(pchCommand, "copyurl"))
	{
		system()->SetClipboardText(m_sCurrentURL, m_sCurrentURL.Length());
	}
	else if (!Q_stricmp(pchCommand, "copylink"))
	{
		int x, y;
		m_pContextMenu->GetPos(x, y);
		int htmlx, htmly;
		ipanel()->GetAbsPos(GetVPanel(), htmlx, htmly);

		m_bRequestingCopyLink = true;
		GetLinkAtPosition(x - htmlx, y - htmly);
	}
	else
		BaseClass::OnCommand(pchCommand);

}


//-----------------------------------------------------------------------------
// Purpose: the control wants us to ask the user what file to load
//-----------------------------------------------------------------------------
void EntHTML::OnFileSelected(const char* pchSelectedFile)
{
	const char* ppchSelectedFiles[] = { pchSelectedFile, NULL };
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->FileLoadDialogResponse(m_unBrowserHandle, ppchSelectedFiles);

	m_hFileOpenDialog->Close();
}

//-----------------------------------------------------------------------------
// Purpose: called when the user dismissed the file dialog with no selection
//-----------------------------------------------------------------------------
void EntHTML::OnFileSelectionCancelled()
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->FileLoadDialogResponse(m_unBrowserHandle, NULL);

	m_hFileOpenDialog->Close();
}

//-----------------------------------------------------------------------------
// Purpose: find any text on the html page with this sub string
//-----------------------------------------------------------------------------
void EntHTML::Find(const char* pchSubStr)
{
	m_bInFind = false;
	if (m_sLastSearchString == pchSubStr) // same string as last time, lets fine next
		m_bInFind = true;

	m_sLastSearchString = pchSubStr;

	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->Find(m_unBrowserHandle, pchSubStr, m_bInFind, false);
}


//-----------------------------------------------------------------------------
// Purpose: find any text on the html page with this sub string
//-----------------------------------------------------------------------------
void EntHTML::FindPrevious()
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->Find(m_unBrowserHandle, m_sLastSearchString, m_bInFind, true);
}


//-----------------------------------------------------------------------------
// Purpose: find any text on the html page with this sub string
//-----------------------------------------------------------------------------
void EntHTML::FindNext()
{
	Find(m_sLastSearchString);
}


//-----------------------------------------------------------------------------
// Purpose: stop an outstanding find request
//-----------------------------------------------------------------------------
void EntHTML::StopFind()
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->StopFind(m_unBrowserHandle);
	m_bInFind = false;
}


//-----------------------------------------------------------------------------
// Purpose: input handler
//-----------------------------------------------------------------------------
void EntHTML::OnEditNewLine(Panel* pPanel)
{
	OnTextChanged(pPanel);
}


//-----------------------h------------------------------------------------------
// Purpose: input handler
//-----------------------------------------------------------------------------
void EntHTML::OnTextChanged(Panel* pPanel)
{
	char rgchText[2048];
	m_pFindBar->GetText(rgchText, sizeof(rgchText));
	Find(rgchText);
}


//-----------------------------------------------------------------------------
// Purpose: passes mouse clicks to the control
//-----------------------------------------------------------------------------
void EntHTMLComboBoxHost::OnMousePressed(MouseCode code)
{
	m_pParent->OnMousePressed(code);
}


//-----------------------------------------------------------------------------
// Purpose: passes mouse up events
//-----------------------------------------------------------------------------
void EntHTMLComboBoxHost::OnMouseReleased(MouseCode code)
{
	m_pParent->OnMouseReleased(code);
}


//-----------------------------------------------------------------------------
// Purpose: keeps track of where the cursor is
//-----------------------------------------------------------------------------
void EntHTMLComboBoxHost::OnCursorMoved(int x, int y)
{
	// Only do this when we are over the current panel
	if (vgui::input()->GetMouseOver() == GetVPanel())
	{
		m_pParent->OnHTMLMouseMoved(x, y);
	}
}


//-----------------------------------------------------------------------------
// Purpose: passes double click events to the browser
//-----------------------------------------------------------------------------
void EntHTMLComboBoxHost::OnMouseDoublePressed(MouseCode code)
{
	m_pParent->OnMouseDoublePressed(code);
}


//-----------------------------------------------------------------------------
// Purpose: passes key presses to the browser (we don't current do this)
//-----------------------------------------------------------------------------
void EntHTMLComboBoxHost::OnKeyTyped(wchar_t unichar)
{
	m_pParent->OnKeyTyped(unichar);
}


//-----------------------------------------------------------------------------
// Purpose: passes key presses to the browser 
//-----------------------------------------------------------------------------
void EntHTMLComboBoxHost::OnKeyCodeTyped(KeyCode code)
{
	m_pParent->OnKeyCodeTyped(code);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void EntHTMLComboBoxHost::OnKeyCodeReleased(KeyCode code)
{
	m_pParent->OnKeyCodeReleased(code);
}


//-----------------------------------------------------------------------------
// Purpose: scrolls the vertical scroll bar on a web page
//-----------------------------------------------------------------------------
void EntHTMLComboBoxHost::OnMouseWheeled(int delta)
{
	m_pParent->OnMouseWheeled(delta);
}


//-----------------------------------------------------------------------------
// Purpose: helper class for the find bar
//-----------------------------------------------------------------------------
EntHTML::CEntHTMLFindBar::CEntHTMLFindBar(EntHTML* parent) : EditablePanel(parent, "FindBar")
{
	m_pParent = parent;
	m_bHidden = false;
	m_pFindBar = new TextEntry(this, "FindEntry");
	m_pFindBar->AddActionSignalTarget(parent);
	m_pFindBar->SendNewLine(true);
	m_pFindCountLabel = new Label(this, "FindCount", "");
	m_pFindCountLabel->SetVisible(false);
	LoadControlSettings("resource/layout/htmlfindbar.layout");
}


//-----------------------------------------------------------------------------
// Purpose: button input into the find bar
//-----------------------------------------------------------------------------
void EntHTML::CEntHTMLFindBar::OnCommand(const char* pchCmd)
{
	if (!Q_stricmp(pchCmd, "close"))
	{
		m_pParent->HideFindDialog();
	}
	else if (!Q_stricmp(pchCmd, "previous"))
	{
		m_pParent->FindPrevious();
	}
	else if (!Q_stricmp(pchCmd, "next"))
	{
		m_pParent->FindNext();
	}
	else
		BaseClass::OnCommand(pchCmd);

}


//-----------------------------------------------------------------------------
// Purpose: we have a new texture to update
//-----------------------------------------------------------------------------
void EntHTML::BrowserNeedsPaint(HTML_NeedsPaint_t* pCallback)
{
	int tw = 0, tt = 0;
	if (m_iHTMLTextureID != 0)
	{
		tw = m_allocedTextureWidth;
		tt = m_allocedTextureHeight;
	}

	if (m_iHTMLTextureID != 0 && ((_vbar->IsVisible() && pCallback->unScrollY > 0 && abs((int)pCallback->unScrollY - m_scrollVertical.m_nScroll) > 5) || (_hbar->IsVisible() && pCallback->unScrollX > 0 && abs((int)pCallback->unScrollX - m_scrollHorizontal.m_nScroll) > 5)))
	{
		m_bNeedsFullTextureUpload = true;
		return;
	}

	// update the vgui texture
	if (m_bNeedsFullTextureUpload || m_iHTMLTextureID == 0 || tw != (int)pCallback->unWide || tt != (int)pCallback->unTall)
	{
		m_bNeedsFullTextureUpload = false;
		if (m_iHTMLTextureID != 0)
			surface()->DeleteTextureByID(m_iHTMLTextureID);

		// if the dimensions changed we also need to re-create the texture ID to support the overlay properly (it won't resize a texture on the fly, this is the only control that needs
		//   to so lets have a tiny bit more code here to support that)
		m_iHTMLTextureID = surface()->CreateNewTextureID(true);
		surface()->DrawSetTextureRGBAEx(m_iHTMLTextureID, (const unsigned char*)pCallback->pBGRA, pCallback->unWide, pCallback->unTall, IMAGE_FORMAT_BGRA8888);// BR FIXME - this call seems to shift by some number of pixels?
		m_allocedTextureWidth = pCallback->unWide;
		m_allocedTextureHeight = pCallback->unTall;
	}
	else if ((int)pCallback->unUpdateWide > 0 && (int)pCallback->unUpdateTall > 0)
	{
		// same size texture, just bits changing in it, lets twiddle
		surface()->DrawUpdateRegionTextureRGBA(m_iHTMLTextureID, pCallback->unUpdateX, pCallback->unUpdateY, (const unsigned char*)pCallback->pBGRA, pCallback->unUpdateWide, pCallback->unUpdateTall, IMAGE_FORMAT_BGRA8888);
	}
	else
	{
		surface()->DrawSetTextureRGBAEx(m_iHTMLTextureID, (const unsigned char*)pCallback->pBGRA, pCallback->unWide, pCallback->unTall, IMAGE_FORMAT_BGRA8888);
	}

	// need a paint next time
	Repaint();
}


//-----------------------------------------------------------------------------
// Purpose: we have a new texture to update
//-----------------------------------------------------------------------------
void EntHTML::BrowserComboNeedsPaint(HTML_ComboNeedsPaint_t* pCallback)
{
	if (m_pComboBoxHost->IsVisible())
	{
		int tw = 0, tt = 0;
		// update the combo box texture also
		if (m_iComboBoxTextureID != 0)
		{
			tw = m_allocedComboBoxWidth;
			tt = m_allocedComboBoxHeight;
		}

		if (m_iComboBoxTextureID == 0 || tw != (int)pCallback->unWide || tt != (int)pCallback->unTall)
		{
			if (m_iComboBoxTextureID != 0)
				surface()->DeleteTextureByID(m_iComboBoxTextureID);

			// if the dimensions changed we also need to re-create the texture ID to support the overlay properly (it won't resize a texture on the fly, this is the only control that needs
			//   to so lets have a tiny bit more code here to support that)
			m_iComboBoxTextureID = surface()->CreateNewTextureID(true);
			surface()->DrawSetTextureRGBAEx(m_iComboBoxTextureID, (const unsigned char*)pCallback->pBGRA, pCallback->unWide, pCallback->unTall, IMAGE_FORMAT_BGRA8888);
			m_allocedComboBoxWidth = (int)pCallback->unWide;
			m_allocedComboBoxHeight = (int)pCallback->unTall;
		}
		else
		{
			// same size texture, just bits changing in it, lets twiddle
			surface()->DrawUpdateRegionTextureRGBA(m_iComboBoxTextureID, 0, 0, (const unsigned char*)pCallback->pBGRA, pCallback->unWide, pCallback->unTall, IMAGE_FORMAT_BGRA8888);
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: browser wants to start loading this url, do we let it?
//-----------------------------------------------------------------------------
bool EntHTML::OnStartRequest(const char* url, const char* target, const char* pchPostData, bool bIsRedirect)
{
	if (!url || !Q_stricmp(url, "about:blank"))
		return true; // this is just webkit loading a new frames contents inside an existing page

	HideFindDialog();
	// see if we have a custom handler for this
	bool bURLHandled = false;
	for (int i = 0; i < m_CustomURLHandlers.Count(); i++)
	{
		if (!Q_strnicmp(m_CustomURLHandlers[i].url, url, Q_strlen(m_CustomURLHandlers[i].url)))
		{
			// we have a custom handler
			Panel* targetPanel = m_CustomURLHandlers[i].hPanel;
			if (targetPanel)
			{
				PostMessage(targetPanel, new KeyValues("CustomURL", "url", m_CustomURLHandlers[i].url));
			}

			bURLHandled = true;
		}
	}

	if (bURLHandled)
		return false;

	if (m_bNewWindowsOnly && bIsRedirect)
	{
		if (target && (!Q_stricmp(target, "_blank") || !Q_stricmp(target, "_new"))) // only allow NEW windows (_blank ones)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	if (target && !Q_strlen(target))
	{
		m_sCurrentURL = url;

		KeyValues* pMessage = new KeyValues("OnURLChanged");
		pMessage->SetString("url", url);
		pMessage->SetString("postdata", pchPostData);
		pMessage->SetInt("isredirect", bIsRedirect ? 1 : 0);

		PostActionSignal(pMessage);
	}
	return EntityParse(url);
}

//-----------------------------------------------------------------------------
// Purpose: callback from cef thread, load a url please
//-----------------------------------------------------------------------------
void EntHTML::BrowserStartRequest(HTML_StartRequest_t* pCmd)
{
	bool bRes = OnStartRequest(pCmd->pchURL, pCmd->pchTarget, pCmd->pchPostData, pCmd->bIsRedirect);

	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->AllowStartRequest(m_unBrowserHandle, bRes);
}


//-----------------------------------------------------------------------------
// Purpose: browser went to a new url
//-----------------------------------------------------------------------------
void EntHTML::BrowserURLChanged(HTML_URLChanged_t* pCmd)
{
	m_sCurrentURL = pCmd->pchURL;

	KeyValues* pMessage = new KeyValues("OnURLChanged");
	pMessage->SetString("url", pCmd->pchURL);
	pMessage->SetString("postdata", pCmd->pchPostData);
	pMessage->SetInt("isredirect", pCmd->bIsRedirect ? 1 : 0);

	PostActionSignal(pMessage);

	OnURLChanged(m_sCurrentURL, pCmd->pchPostData, pCmd->bIsRedirect);
}


//-----------------------------------------------------------------------------
// Purpose: finished loading this page
//-----------------------------------------------------------------------------
void EntHTML::BrowserFinishedRequest(HTML_FinishedRequest_t* pCmd)
{
	PostActionSignal(new KeyValues("OnFinishRequest", "url", pCmd->pchURL));
	if (pCmd->pchPageTitle && pCmd->pchPageTitle[0])
		PostActionSignal(new KeyValues("PageTitleChange", "title", pCmd->pchPageTitle));

	CUtlMap < CUtlString, CUtlString > mapHeaders;
	SetDefLessFunc(mapHeaders);
	// headers are no longer reported on loads

	OnFinishRequest(pCmd->pchURL, pCmd->pchPageTitle, mapHeaders);
}


//-----------------------------------------------------------------------------
// Purpose: show a popup dialog
//-----------------------------------------------------------------------------
void EntHTML::BrowserShowPopup(HTML_ShowPopup_t* pCmd)
{
	m_pComboBoxHost->SetVisible(true);
}


//-----------------------------------------------------------------------------
// Purpose: hide the popup
//-----------------------------------------------------------------------------
void EntHTML::HidePopup()
{
	m_pComboBoxHost->SetVisible(false);
}


//-----------------------------------------------------------------------------
// Purpose: browser wants us to hide a popup
//-----------------------------------------------------------------------------
void EntHTML::BrowserHidePopup(HTML_HidePopup_t* pCmd)
{
	HidePopup();
}


//-----------------------------------------------------------------------------
// Purpose: browser wants us to position a popup
//-----------------------------------------------------------------------------
void EntHTML::BrowserSizePopup(HTML_SizePopup_t* pCmd)
{
	int nAbsX, nAbsY;
	ipanel()->GetAbsPos(GetVPanel(), nAbsX, nAbsY);
	m_pComboBoxHost->SetBounds(pCmd->unX + 1 + nAbsX, pCmd->unY + nAbsY, pCmd->unWide, pCmd->unTall);
}


//-----------------------------------------------------------------------------
// Purpose: browser wants to open a new tab
//-----------------------------------------------------------------------------
void EntHTML::BrowserOpenNewTab(HTML_OpenLinkInNewTab_t* pCmd)
{
	(pCmd);
	// Not suppored by default, if a child class overrides us and knows how to handle tabs, then it can do this.
}


//-----------------------------------------------------------------------------
// Purpose: display a new html window 
//-----------------------------------------------------------------------------
void EntHTML::BrowserPopupHTMLWindow(HTML_NewWindow_t* pCmd)
{
	EntHTMLPopup* p = new EntHTMLPopup(this, pCmd->pchURL, "");
	int wide = pCmd->unWide;
	int tall = pCmd->unTall;
	if (wide == 0 || tall == 0)
	{
		wide = MAX(640, GetWide());
		tall = MAX(480, GetTall());
	}

	p->SetBounds(pCmd->unX, pCmd->unY, wide, tall);
	p->SetDeleteSelfOnClose(true);
	if (pCmd->unX == 0 || pCmd->unY == 0)
		p->MoveToCenterOfScreen();
	p->Activate();

}


//-----------------------------------------------------------------------------
// Purpose: browser telling us the page title
//-----------------------------------------------------------------------------
void EntHTML::BrowserSetHTMLTitle(HTML_ChangedTitle_t* pCmd)
{
	PostMessage(GetParent(), new KeyValues("OnSetHTMLTitle", "title", pCmd->pchTitle));
	OnSetHTMLTitle(pCmd->pchTitle);
}


//-----------------------------------------------------------------------------
// Purpose: status bar details
//-----------------------------------------------------------------------------
void EntHTML::BrowserStatusText(HTML_StatusText_t* pCmd)
{
	PostActionSignal(new KeyValues("OnSetStatusText", "status", pCmd->pchMsg));
}


//-----------------------------------------------------------------------------
// Purpose: browser telling us to use this cursor
//-----------------------------------------------------------------------------
void EntHTML::BrowserSetCursor(HTML_SetCursor_t* pCmd)
{
	vgui::CursorCode cursor = dc_last;

	switch (pCmd->eMouseCursor)
	{
	case ISteamHTMLSurface::dc_user:
		cursor = dc_user;
		break;
	case ISteamHTMLSurface::dc_none:
		cursor = dc_none;
		break;
	default:
	case ISteamHTMLSurface::dc_arrow:
		cursor = dc_arrow;
		break;
	case ISteamHTMLSurface::dc_ibeam:
		cursor = dc_ibeam;
		break;
	case ISteamHTMLSurface::dc_hourglass:
		cursor = dc_hourglass;
		break;
	case ISteamHTMLSurface::dc_waitarrow:
		cursor = dc_waitarrow;
		break;
	case ISteamHTMLSurface::dc_crosshair:
		cursor = dc_crosshair;
		break;
	case ISteamHTMLSurface::dc_up:
		cursor = dc_up;
		break;
		/*case ISteamHTMLSurface::dc_sizenw:
			cursor = dc_sizenw;
			break;
		case ISteamHTMLSurface::dc_sizese:
			cursor = dc_sizese;
			break;
		case ISteamHTMLSurface::dc_sizene:
			cursor = dc_sizene;
			break;
		case ISteamHTMLSurface::dc_sizesw:
			cursor = dc_sizesw;
			break;
		case ISteamHTMLSurface::dc_sizew:
			cursor = dc_sizew;
			break;
		case ISteamHTMLSurface::dc_sizee:
			cursor = dc_sizee;
			break;
		case ISteamHTMLSurface::dc_sizen:
			cursor = dc_sizen;
			break;
		case ISteamHTMLSurface::dc_sizes:
			cursor = dc_sizes;
			break;*/
	case ISteamHTMLSurface::dc_sizewe:
		cursor = dc_sizewe;
		break;
	case ISteamHTMLSurface::dc_sizens:
		cursor = dc_sizens;
		break;
	case ISteamHTMLSurface::dc_sizeall:
		cursor = dc_sizeall;
		break;
	case ISteamHTMLSurface::dc_no:
		cursor = dc_no;
		break;
	case ISteamHTMLSurface::dc_hand:
		cursor = dc_hand;
		break;
	case ISteamHTMLSurface::dc_blank:
		cursor = dc_blank;
		break;
		/*	case ISteamHTMLSurface::dc_middle_pan:
				cursor = dc_middle_pan;
				break;
			case ISteamHTMLSurface::dc_north_pan:
				cursor = dc_north_pan;
				break;
			case ISteamHTMLSurface::dc_north_east_pan:
				cursor = dc_north_east_pan;
				break;
			case ISteamHTMLSurface::dc_east_pan:
				cursor = dc_east_pan;
				break;
			case ISteamHTMLSurface::dc_south_east_pan:
				cursor = dc_south_east_pan;
				break;
			case ISteamHTMLSurface::dc_south_pan:
				cursor = dc_south_pan;
				break;
			case ISteamHTMLSurface::dc_south_west_pan:
				cursor = dc_south_west_pan;
				break;
			case ISteamHTMLSurface::dc_west_pan:
				cursor = dc_west_pan;
				break;
			case ISteamHTMLSurface::dc_north_west_pan:
				cursor = dc_north_west_pan;
				break;
			case ISteamHTMLSurface::dc_alias:
				cursor = dc_alias;
				break;
			case ISteamHTMLSurface::dc_cell:
				cursor = dc_cell;
				break;
			case ISteamHTMLSurface::dc_colresize:
				cursor = dc_colresize;
				break;
			case ISteamHTMLSurface::dc_copycur:
				cursor = dc_copycur;
				break;
			case ISteamHTMLSurface::dc_verticaltext:
				cursor = dc_verticaltext;
				break;
			case ISteamHTMLSurface::dc_rowresize:
				cursor = dc_rowresize;
				break;
			case ISteamHTMLSurface::dc_zoomin:
				cursor = dc_zoomin;
				break;
			case ISteamHTMLSurface::dc_zoomout:
				cursor = dc_zoomout;
				break;
			case ISteamHTMLSurface::dc_custom:
				cursor = dc_custom;
				break;
			case ISteamHTMLSurface::dc_help:
				cursor = dc_help;
				break;*/

	}

	if (cursor >= dc_last)
	{
		cursor = dc_arrow;
	}

	SetCursor(cursor);
}


//-----------------------------------------------------------------------------
// Purpose: browser telling to show the file loading dialog
//-----------------------------------------------------------------------------
void EntHTML::BrowserFileLoadDialog(HTML_FileOpenDialog_t* pCmd)
{
	// couldn't access an OS-specific dialog, use the internal one
	if (m_hFileOpenDialog.Get())
	{
		delete m_hFileOpenDialog.Get();
		m_hFileOpenDialog = NULL;
	}
	m_hFileOpenDialog = new FileOpenDialog(this, pCmd->pchTitle, true);
	m_hFileOpenDialog->SetStartDirectory(pCmd->pchInitialFile);
	m_hFileOpenDialog->AddActionSignalTarget(this);
	m_hFileOpenDialog->SetAutoDelete(true);
	m_hFileOpenDialog->DoModal(false);
}


//-----------------------------------------------------------------------------
// Purpose: browser asking to show a tooltip
//-----------------------------------------------------------------------------
void EntHTML::BrowserShowToolTip(HTML_ShowToolTip_t* pCmd)
{
	/*
		BR FIXME
		Tooltip *tip = GetTooltip();
		tip->SetText( pCmd->text().c_str() );
		tip->SetTooltipFormatToMultiLine();
		tip->SetTooltipDelayMS( 250 );
		tip->SetMaxToolTipWidth( MAX( 200, GetWide()/2 ) );
		tip->ShowTooltip( this );
		*/

}


//-----------------------------------------------------------------------------
// Purpose: browser telling us to update tool tip text
//-----------------------------------------------------------------------------
void EntHTML::BrowserUpdateToolTip(HTML_UpdateToolTip_t* pCmd)
{
	//	GetTooltip()->SetText( pCmd->text().c_str() );
}


//-----------------------------------------------------------------------------
// Purpose: browser telling that it is done with the tip
//-----------------------------------------------------------------------------
void EntHTML::BrowserHideToolTip(HTML_HideToolTip_t* pCmd)
{
	//	GetTooltip()->HideTooltip();
	//	DeleteToolTip();
}


//-----------------------------------------------------------------------------
// Purpose: callback when performing a search
//-----------------------------------------------------------------------------
void EntHTML::BrowserSearchResults(HTML_SearchResults_t* pCmd)
{
	if (pCmd->unResults == 0)
		m_pFindBar->HideCountLabel();
	else
		m_pFindBar->ShowCountLabel();

	if (pCmd->unResults > 0)
		m_pFindBar->SetDialogVariable("findcount", (int)pCmd->unResults);
	if (pCmd->unCurrentMatch > 0)
		m_pFindBar->SetDialogVariable("findactive", (int)pCmd->unCurrentMatch);
	m_pFindBar->InvalidateLayout();
}


//-----------------------------------------------------------------------------
// Purpose: browser telling us it had a close requested
//-----------------------------------------------------------------------------
void EntHTML::BrowserClose(HTML_CloseBrowser_t* pCmd)
{
	PostActionSignal(new KeyValues("OnCloseWindow"));
}


//-----------------------------------------------------------------------------
// Purpose: browser telling us the size of the horizontal scrollbars
//-----------------------------------------------------------------------------
void EntHTML::BrowserHorizontalScrollBarSizeResponse(HTML_HorizontalScroll_t* pCmd)
{
	ScrollData_t scrollHorizontal;
	scrollHorizontal.m_nScroll = pCmd->unScrollCurrent;
	scrollHorizontal.m_nMax = pCmd->unScrollMax;
	scrollHorizontal.m_bVisible = pCmd->bVisible;
	scrollHorizontal.m_flZoom = pCmd->flPageScale;

	if (scrollHorizontal != m_scrollHorizontal)
	{
		m_scrollHorizontal = scrollHorizontal;
		UpdateSizeAndScrollBars();
		m_bNeedsFullTextureUpload = true;
	}
	else
		m_scrollHorizontal = scrollHorizontal;
}


//-----------------------------------------------------------------------------
// Purpose: browser telling us the size of the vertical scrollbars
//-----------------------------------------------------------------------------
void EntHTML::BrowserVerticalScrollBarSizeResponse(HTML_VerticalScroll_t* pCmd)
{
	ScrollData_t scrollVertical;
	scrollVertical.m_nScroll = pCmd->unScrollCurrent;
	scrollVertical.m_nMax = pCmd->unScrollMax;
	scrollVertical.m_bVisible = pCmd->bVisible;
	scrollVertical.m_flZoom = pCmd->flPageScale;

	if (scrollVertical != m_scrollVertical)
	{
		m_scrollVertical = scrollVertical;
		UpdateSizeAndScrollBars();
		m_bNeedsFullTextureUpload = true;
	}
	else
		m_scrollVertical = scrollVertical;
}


//-----------------------------------------------------------------------------
// Purpose: browser telling us what is at this location on the page
//-----------------------------------------------------------------------------
void EntHTML::BrowserLinkAtPositionResponse(HTML_LinkAtPosition_t* pCmd)
{
	m_LinkAtPos.m_sURL = pCmd->pchURL;
	m_LinkAtPos.m_nX = pCmd->x;
	m_LinkAtPos.m_nY = pCmd->y;

	m_pContextMenu->SetItemVisible(m_iCopyLinkMenuItemID, !m_LinkAtPos.m_sURL.IsEmpty() ? true : false);
	if (m_bRequestingDragURL)
	{
		m_bRequestingDragURL = false;
		m_sDragURL = m_LinkAtPos.m_sURL;
		// make sure we get notified when the mouse gets released
		if (!m_sDragURL.IsEmpty())
		{
			input()->SetMouseCapture(GetVPanel());
		}
	}

	if (m_bRequestingCopyLink)
	{
		m_bRequestingCopyLink = false;
		if (!m_LinkAtPos.m_sURL.IsEmpty())
			system()->SetClipboardText(m_LinkAtPos.m_sURL, m_LinkAtPos.m_sURL.Length());
		else
			system()->SetClipboardText("", 1);
	}

	OnLinkAtPosition(m_LinkAtPos.m_sURL);
}


//-----------------------------------------------------------------------------
// Purpose: browser telling us to pop a javascript alert dialog
//-----------------------------------------------------------------------------
void EntHTML::BrowserJSAlert(HTML_JSAlert_t* pCmd)
{
	MessageBox* pDlg = new MessageBox(m_sCurrentURL, (const char*)pCmd->pchMessage, this);
	pDlg->AddActionSignalTarget(this);
	pDlg->SetCommand(new KeyValues("DismissJSDialog", "result", false));
	pDlg->DoModal();
}


//-----------------------------------------------------------------------------
// Purpose: browser telling us to pop a js confirm dialog
//-----------------------------------------------------------------------------
void EntHTML::BrowserJSConfirm(HTML_JSConfirm_t* pCmd)
{
	QueryBox* pDlg = new QueryBox(m_sCurrentURL, (const char*)pCmd->pchMessage, this);
	pDlg->AddActionSignalTarget(this);
	pDlg->SetOKCommand(new KeyValues("DismissJSDialog", "result", true));
	pDlg->SetCancelCommand(new KeyValues("DismissJSDialog", "result", false));
	pDlg->DoModal();
}


//-----------------------------------------------------------------------------
// Purpose: got an answer from the dialog, tell cef
//-----------------------------------------------------------------------------
void EntHTML::DismissJSDialog(int bResult)
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->JSDialogResponse(m_unBrowserHandle, bResult);
};


//-----------------------------------------------------------------------------
// Purpose: browser telling us the state of back and forward buttons
//-----------------------------------------------------------------------------
void EntHTML::BrowserCanGoBackandForward(HTML_CanGoBackAndForward_t* pCmd)
{
	m_bCanGoBack = pCmd->bCanGoBack;
	m_bCanGoForward = pCmd->bCanGoForward;
}


//-----------------------------------------------------------------------------
// Purpose: ask the browser for what is at this x,y
//-----------------------------------------------------------------------------
void EntHTML::GetLinkAtPosition(int x, int y)
{
	if (m_SteamAPIContext.SteamHTMLSurface())
		m_SteamAPIContext.SteamHTMLSurface()->GetLinkAtPosition(m_unBrowserHandle, x, y);
}


//-----------------------------------------------------------------------------
// Purpose: update the size of the browser itself and scrollbars it shows
//-----------------------------------------------------------------------------
void EntHTML::UpdateSizeAndScrollBars()
{
	BrowserResize();
	InvalidateLayout();
}


