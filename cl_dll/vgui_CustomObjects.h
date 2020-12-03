#ifndef VGUI_CUSTOMOBJECTS_H
#define VGUI_CUSTOMOBJECTS_H

#include <VGUI_Panel.h>
#include <VGUI_Frame.h>
#include <VGUI_TextPanel.h>
#include <VGUI_Label.h>
#include <VGUI_Button.h>
#include <VGUI_ActionSignal.h>
#include <VGUI_InputSignal.h>
#include <VGUI_Image.h>
#include <VGUI_BitmapTGA.h>
#include <VGUI_LineBorder.h>
#include <VGUI_ScrollPanel.h>
#include <VGUI_ScrollBar.h>
#include <VGUI_Slider.h>
#include <VGUI_TextEntry.h>
#include "vgui_SchemeManager.h"
#include "vgui_defaultinputsignal.h"

using namespace vgui;

class CViewport;
class CCommandMenu;
class CMenuPanel;
class DragNDropPanel;

// Arrows
enum
{
	ARROW_UP,
	ARROW_DOWN,
	ARROW_LEFT,
	ARROW_RIGHT,
};

#define MAX_BUTTON_SIZE			32
#define MAX_BUTTONS				100

//============================================================
// Labels
//============================================================

//-----------------------------------------------------------------------------
// Purpose: Wrapper for an Image Label without a background
//-----------------------------------------------------------------------------
class CImageLabel : public Label
{
public:
	CImageLabel();
	CImageLabel(const char *pImageName, int x, int y);
	CImageLabel(const char *pImageName, int x, int y, int wide, int tall);

	bool LoadImage(const char *pImageName, bool fullpath = false);
	virtual void getImageSize(int &wide, int &tall);
//	virtual void getImageCenter(int &x, int &y);
//	virtual int getImageTall();
//	virtual int getImageWide();
//	virtual void paintBackground() {};// Do nothing, so the background's left transparent.

public:
	BitmapTGA	*m_pImage;
};

//-----------------------------------------------------------------------------
// Purpose: Overridden label so we can darken it when submenus open
//-----------------------------------------------------------------------------
class CommandLabel : public Label
{
public:
	CommandLabel(const char *text, int x,int y, int wide,int tall) : Label(text,x,y,wide,tall)
	{
		m_iState = false;
	}

	void PushUp(void)
	{
		m_iState = false;
		repaint();
	}
	void PushDown(void)
	{
		m_iState = true;
		repaint();
	}

private:
	int		m_iState;
};




//============================================================
// Command Buttons
//============================================================

//-----------------------------------------------------------------------------
// Purpose: Special button for command menu
//-----------------------------------------------------------------------------
class CommandButton : public Button
{
public:
	CommandButton(void);
	CommandButton(const char *text,int x,int y,int wide,int tall, bool bNoHighlight = false);
	CommandButton(const char *text,int x,int y,int wide,int tall, bool bNoHighlight, bool bFlat, bool bShowHotKey, char *iconfile = NULL);

	virtual void Init(void);
	virtual int IsNotValid(void) { return false; }

	// Overloaded vgui functions
	virtual void paint(void);
	virtual void setText(const char *text);
	virtual void paintBackground(void);

	virtual void cursorEntered(void);
	virtual void cursorExited(void);
	virtual void OnActionPerformed(void);

	// Menu Handling
	void AddSubMenu(CCommandMenu *pNewMenu);
	void AddSubLabel(CommandLabel *pSubLabel);
	void UpdateSubMenus(int iAdjustment);
	CCommandMenu *GetSubMenu(void) { return m_pSubMenu; };
	CCommandMenu *getParentMenu(void);
	void setParentMenu(CCommandMenu *pParentMenu);
//	void BindKey(char key);
	void setBoundKey(char boundKey);
	char getBoundKey(void);
	void SetIcon(BitmapTGA *pIcon);
	void LoadIcon(char *pFileName);

	void RecalculateText(void);

public:
	bool	m_bNoHighlight;
	bool	m_bShowHotKey;
	vgui::Color m_ColorNormal;
	vgui::Color m_ColorArmed;
	vgui::Color m_ColorBgNormal;
	vgui::Color m_ColorBgArmed;
	vgui::Color m_ColorBorderNormal;
	vgui::Color m_ColorBorderArmed;
	unsigned long m_ulID;// for dialogs

protected:
	bool	m_bFlat;

	// Submenus under this button
	CCommandMenu	*m_pSubMenu;
	CCommandMenu	*m_pParentMenu;
	CommandLabel	*m_pSubLabel;
	BitmapTGA		*m_pIcon;

	char m_sMainText[MAX_BUTTON_SIZE];
	char m_cBoundKey;
};


//-----------------------------------------------------------------------------
// Purpose: CommandButton which toggles a cvar
//-----------------------------------------------------------------------------
class ToggleCommandButton : public CommandButton, public CDefaultInputSignal
{
public:
	ToggleCommandButton(struct cvar_s *pCVar, const char *text, int x, int y, int wide, int tall, bool flat, bool bShowHotKey);
	ToggleCommandButton(const char *cvarname, const char *text, int x, int y, int wide, int tall, bool flat, bool bShowHotKey);
	virtual void cursorEntered(Panel *panel);
//	virtual void cursorExited(Panel *panel);
	virtual void mousePressed(MouseCode code, Panel *panel);
	virtual void paint(void);
//	virtual void OnActionPerformed(void);

private:
	struct cvar_s *m_pCVar;
	CImageLabel *pLabelOn;
	CImageLabel *pLabelOff;
};


//-----------------------------------------------------------------------------
// Purpose: Special button with filled background
//-----------------------------------------------------------------------------
/* obsolete
class ColorButton : public CommandButton
{
public:
	ColorButton(const char *text, int x, int y, int wide, int tall, bool bNoHighlight, bool bFlat) : CommandButton(text, x, y, wide, tall, bNoHighlight, bFlat)
	{
		ArmedColor = NULL;
		UnArmedColor = NULL;
		ArmedBorderColor = NULL;
		UnArmedBorderColor = NULL;
	}

	virtual void paintBackground(void)
	{
		int r, g, b, a;
		vgui::Color bgcolor;

		CommandButton::paintBackground();
		if (isArmed())
		{
			if (ArmedBorderColor)
			{
				ArmedBorderColor->getColor(r, g, b, a);
				drawSetColor(r, g, b, a);
				drawOutlinedRect(0,0,_size[0],_size[1]);
			}
		}
		else
		{
			if (UnArmedBorderColor)
			{
				UnArmedBorderColor->getColor(r, g, b, a);
				drawSetColor(r, g, b, a);
				drawOutlinedRect(0,0,_size[0],_size[1]);
			}
		}
	}
	void paint(void)
	{
		int r, g, b, a;
		if (isArmed())
		{
			if (ArmedColor)
			{
				ArmedColor->getColor(r, g, b, a);
				setFgColor(r, g, b, a);
			}
			else
				setFgColor(Scheme::sc_secondary1);
		}
		else
		{
			if (UnArmedColor)
			{
				UnArmedColor->getColor(r, g, b, a);
				setFgColor(r, g, b, a);
			}
			else
				setFgColor(Scheme::sc_primary1);
		}
		
		Button::paint();
	}
	
	void setArmedColor(int r, int g, int b, int a)
	{
		if (ArmedColor)
			ArmedColor->setColor(r, g, b, a);
		else
			ArmedColor = new vgui::Color(r, g, b, a);
	}
	void setUnArmedColor(int r, int g, int b, int a)
	{
		if (UnArmedColor)
			UnArmedColor->setColor(r, g, b, a);
		else
			UnArmedColor = new vgui::Color(r, g, b, a);
	}
	void setArmedBorderColor( int r, int g, int b, int a )
	{
		if (ArmedBorderColor)
			ArmedBorderColor->setColor(r, g, b, a);
		else
			ArmedBorderColor = new vgui::Color(r, g, b, a);
	}
	void setUnArmedBorderColor( int r, int g, int b, int a )
	{
		if (UnArmedBorderColor)
			UnArmedBorderColor->setColor(r, g, b, a);
		else
			UnArmedBorderColor = new vgui::Color(r, g, b, a);
	}
private:
	vgui::Color *ArmedColor;
	vgui::Color *UnArmedColor;
	vgui::Color *ArmedBorderColor;
	vgui::Color *UnArmedBorderColor;
};*/


//-----------------------------------------------------------------------------
// Purpose: Special button...?
//-----------------------------------------------------------------------------
class SpectButton : public CommandButton
{
public:
	SpectButton(const char* text,int x,int y,int wide,int tall) : CommandButton(text, x, y, wide, tall, false)
	{
		Init();
		setText(text);
	}
	virtual void paintBackground()
	{
		if (isArmed())
		{
			drawSetColor(Scheme::sc_secondary2);// XDM
			drawFilledRect(5, 0,_size[0] - 5,_size[1]);
		}
	}
	virtual void paint()
	{
		if (isArmed())
			setFgColor(Scheme::sc_secondary1);// XDM
		else
			setFgColor(Scheme::sc_primary1);// XDM

		Button::paint();
	}
};



//-----------------------------------------------------------------------------
// Purpose: Special button for team menus (react on gameplay)
//-----------------------------------------------------------------------------
/*class TeamButton : public CommandButton
{
public:
	TeamButton(int iTeam, const char *text,int x,int y,int wide,int tall) : CommandButton(text,x,y,wide,tall)
	{
		m_iTeamNumber = iTeam;
	}

	virtual int IsNotValid()
	{
		int iTeams = gViewPort->GetNumberOfTeams();
		// Never valid if there's only 1 team
		if (iTeams == 1)
			return true;

		// Auto Team's always visible
		if (m_iTeamNumber > MAX_TEAMS)
			return false;

		if (iTeams >= m_iTeamNumber && m_iTeamNumber != gHUD.m_iTeamNumber)
			return false;

		return true;
	}

private:
	int	m_iTeamNumber;
};*/

/*
//-----------------------------------------------------------------------------
// Purpose: Special button
//-----------------------------------------------------------------------------
class FeignButton : public CommandButton
{
public:
	FeignButton( int iState, const char* text,int x,int y,int wide,int tall ) : CommandButton( text,x,y,wide,tall)
	{
		m_iFeignState = iState;
	}

	virtual int IsNotValid()
	{
//		if (m_iFeignState == gViewPort->GetIsFeigning())
//			return false;

		return true;
	}

private:
	int	m_iFeignState;
};
*/

//-----------------------------------------------------------------------------
// Purpose: Special button for spectator menu
//-----------------------------------------------------------------------------
class SpectateButton : public CommandButton
{
public:
	SpectateButton(const char* text,int x,int y,int wide,int tall, bool bNoHighlight) : CommandButton(text,x,y,wide,tall, bNoHighlight)
	{
	}
	virtual int IsNotValid();
};


//-----------------------------------------------------------------------------
// Purpose: Special button which is only valid for specified map
//-----------------------------------------------------------------------------
class MapButton : public CommandButton
{
public:
	MapButton(const char *pMapName, const char* text,int x,int y,int wide,int tall);
	virtual int IsNotValid(void);

private:
	char m_szMapName[MAX_MAPNAME];
};


//-----------------------------------------------------------------------------
// Purpose: CommandButton which is only displayed if the player is on team X
//-----------------------------------------------------------------------------
class TeamOnlyCommandButton : public CommandButton
{
public:
	TeamOnlyCommandButton(int iTeamNum, const char *text, int x, int y, int wide, int tall, bool flat, bool bShowHotKey) : CommandButton(text, x, y, wide, tall, false, flat, bShowHotKey)
	{
		m_iTeamNum = iTeamNum;
	}

	virtual int IsNotValid(void)
	{
		if (gHUD.m_iTeamNumber != m_iTeamNum)
			return true;

		return CommandButton::IsNotValid();
	}
/*	virtual void paintBackground(void)
	{
		if (isArmed())
		{
			drawSetColor(Scheme::sc_primary2);// XDM
			drawFilledRect(5, 0,_size[0] - 5,_size[1]);
		}
	}
	virtual void paint(void)
	{
		if (isArmed())
			setFgColor(Scheme::sc_secondary1);// XDM
		else
			setFgColor(Scheme::sc_primary1);// XDM

		Button::paint();
	}*/

private:
	int m_iTeamNum;
};


//-----------------------------------------------------------------------------
// Purpose: CommandButton which toggles spectator mode
//-----------------------------------------------------------------------------
class SpectToggleButton : public CommandButton, public CDefaultInputSignal
{
public:
	SpectToggleButton(const char *cvarname, const char *text, int x, int y, int wide, int tall, bool flat);

	virtual void cursorEntered(Panel *panel);
	virtual void cursorExited(Panel *panel);
	virtual void mousePressed(MouseCode code, Panel *panel);
	virtual void paintBackground(void);
	virtual void paint(void);

private:
	struct cvar_s	*m_cvar;
	CImageLabel		*pLabelOn; 
};





//============================================================
// Command Menus
//============================================================

//-----------------------------------------------------------------------------
// Purpose: Special menu
//-----------------------------------------------------------------------------
class CCommandMenu : public Panel
{
public:
	CCommandMenu(CCommandMenu *pParentMenu, int x, int y, int wide, int tall);
	CCommandMenu(CCommandMenu *pParentMenu, int direction, int x, int y, int wide, int tall);
	virtual void Initialize(void);
	virtual void Open(void);
	virtual void Close(void);
//	virtual void paintBackground();
	virtual int KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding);

	void AddButton(CommandButton *pButton);
	bool RecalculateVisibles(int iNewYPos, bool bHideAll);
	void RecalculatePositions(int iYOffset);
	void MakeVisible(CCommandMenu *pChildMenu);
	void ClearButtonsOfArmedState(void);

	CCommandMenu *GetParentMenu(void) { return m_pParentMenu; };
	int GetXOffset(void) { return m_iXOffset; };
	int GetYOffset(void) { return m_iYOffset; };
	int GetDirection(void) { return m_iDirection; };
	int GetNumButtons(void) { return m_iButtons; };
	CommandButton *FindButtonWithSubmenu( CCommandMenu *pSubMenu );

	int m_iButtonSizeY;
protected:
	CCommandMenu *m_pParentMenu;
	int			  m_iXOffset;
	int			  m_iYOffset;
	CommandButton *m_aButtons[MAX_BUTTONS];
	SchemeHandle_t hCommandMenuScheme;

	int m_iButtons;
	int m_iDirection; // opens menu from top to bottom (0 = default), or from bottom to top (1)?
};



//============================================================
// ActionSignals
//============================================================

// Command Menu Button Handlers
#define MAX_COMMAND_SIZE	256

//-----------------------------------------------------------------------------
// Purpose: Base action handler for all menus
//-----------------------------------------------------------------------------
class CMenuActionHandler : public ActionSignal
{
public:
	CMenuActionHandler(void);
	CMenuActionHandler(CCommandMenu	*pParentMenu, bool bCloseMenu);
	virtual void actionPerformed(Panel *panel);

protected:
	CCommandMenu	*m_pParentMenu;// close on fire
	bool			m_bCloseMenu;
};

//-----------------------------------------------------------------------------
// Purpose: Special action handler for console command menu
//-----------------------------------------------------------------------------
class CMenuHandler_StringCommand : public CMenuActionHandler
{
public:
	CMenuHandler_StringCommand(CCommandMenu *pParentMenu, char *pszCommand, bool bCloseMenu = false);
	virtual void actionPerformed(Panel *panel);

protected:
	char	m_pszCommand[MAX_COMMAND_SIZE];
};


#define HIDE_TEXTWINDOW		0

//-----------------------------------------------------------------------------
// Purpose: Special action handler for activating windows
//-----------------------------------------------------------------------------
class CMenuHandler_TextWindow : public CMenuActionHandler
{
public:
	CMenuHandler_TextWindow(int iState);
	virtual void actionPerformed(Panel *panel);

protected:
	int	m_iState;
};


//-----------------------------------------------------------------------------
// Purpose: Special action handler for cvar switching
//-----------------------------------------------------------------------------
class CMenuHandler_ToggleCvar : public CMenuActionHandler
{
public:
	CMenuHandler_ToggleCvar(CCommandMenu *pParentMenu, struct cvar_s *pCVar);
	CMenuHandler_ToggleCvar(CCommandMenu *pParentMenu, const char *cvarname);
	virtual void actionPerformed(Panel *panel);

protected:
	struct cvar_s *m_pCVar;
};




//============================================================
// InputSignals
//============================================================

//-----------------------------------------------------------------------------
// Purpose: Special handler for CCommandMenu
//-----------------------------------------------------------------------------
class CMenuHandler_PopupSubMenuInput : public CDefaultInputSignal
{
public:
	CMenuHandler_PopupSubMenuInput(Button *pButton, CCommandMenu *pSubMenu);
//	virtual void cursorMoved(int x,int y,Panel *panel)
	virtual void cursorEntered(Panel *panel);
	virtual void cursorExited(Panel *panel);

protected:
	CCommandMenu *m_pSubMenu;
	Button		 *m_pButton;
};


//-----------------------------------------------------------------------------
// Purpose: Special handler for drag and drop
//-----------------------------------------------------------------------------
class CDragNDropHandler : public CDefaultInputSignal
{
public:
	CDragNDropHandler(DragNDropPanel *pPanel);
	virtual void cursorMoved(int x, int y, Panel *panel);
	virtual void cursorEntered(Panel *panel);
	virtual void cursorExited(Panel *panel);
	virtual void mousePressed(MouseCode code, Panel *panel);
	virtual void mouseReleased(MouseCode code, Panel *panel);
	virtual void Drag(void);
	virtual void Drop(void);

protected:
	DragNDropPanel	*m_pPanel;
	bool			m_bDragging;
	bool			m_bMouseInside;
	int				m_PanelStartPos[2];// panel position when it was grabbed
//	int				m_PanelPosMouseDelta[2];// vecMouseClick - vecPanelPos
	int				m_LastMousePos[2];
};


//-----------------------------------------------------------------------------
// Purpose: Special handler for CMenuPanel buttons
//-----------------------------------------------------------------------------
class CHandler_MenuButtonOver : public CDefaultInputSignal
{
public:
	CHandler_MenuButtonOver(CMenuPanel *pPanel, int iButton)
	{
		m_iButton = iButton;
		m_pMenuPanel = pPanel;
	}
	virtual void cursorEntered(Panel *panel);

protected:
	int			m_iButton;
	CMenuPanel	*m_pMenuPanel;
};


//-----------------------------------------------------------------------------
// Purpose: Special handler for highlighting of ordinary buttons
//-----------------------------------------------------------------------------
class CHandler_ButtonHighlight : public CDefaultInputSignal
{
public:
	CHandler_ButtonHighlight(Button *pButton)
	{
		m_pButton = pButton;
	}
	virtual void cursorEntered(Panel *panel) 
	{ 
		m_pButton->setArmed(true);
	}
	virtual void cursorExited(Panel *Panel) 
	{
		m_pButton->setArmed(false);
	}

protected:
	Button *m_pButton;
};

//-----------------------------------------------------------------------------
// Purpose: Special handler for highlighting of command menu buttons
//-----------------------------------------------------------------------------
class CHandler_CommandButtonHighlight : public CHandler_ButtonHighlight
{
public:
	CHandler_CommandButtonHighlight(CommandButton *pButton) : CHandler_ButtonHighlight(pButton)
	{
		m_pCommandButton = pButton;
	}
	virtual void cursorEntered(Panel *panel)
	{
		m_pCommandButton->cursorEntered();
	}
	virtual void cursorExited(Panel *panel)
	{
		m_pCommandButton->cursorExited();
	}

protected:
	CommandButton *m_pCommandButton;
};




//============================================================
// Custom scroll panel
//============================================================

// Custom drawn scroll bars
class CCustomScrollButton : public CommandButton
{
public:
	CCustomScrollButton(int iArrow, const char *text, int x, int y, int wide, int tall);
	virtual void paint(void);
//	virtual void paintBackground(void);

protected:
	BitmapTGA	*m_pImage;
};

// Custom drawn slider bar
class CCustomSlider : public Slider
{
public:
	CCustomSlider(int x, int y, int wide, int tall, bool vertical);
	virtual void paintBackground(void);
};

// Custom drawn scrollpanel
class CCustomScrollPanel : public ScrollPanel
{
public:
	CCustomScrollPanel(int x, int y, int wide, int tall);
};



//-----------------------------------------------------------------------------
// Purpose: TextEntry with default constructor for easier instantiation
//-----------------------------------------------------------------------------
class CDefaultTextEntry : public TextEntry
{
public:
	CDefaultTextEntry(void);
	CDefaultTextEntry(const char *text, int x, int y, int wide, int tall);
};


//-----------------------------------------------------------------------------
// Purpose: LineBorder with default color and thickness
//-----------------------------------------------------------------------------
class CDefaultLineBorder : public LineBorder
{
public:
	CDefaultLineBorder();
};

#endif // VGUI_CUSTOMOBJECTS_H
