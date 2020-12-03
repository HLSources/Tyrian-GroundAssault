//=========== (C) Copyright 1996-2002 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Contains implementation of various VGUI-derived objects
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================
#include "hud.h"
#include "cl_util.h"
#include "camera.h"
#include "kbutton.h"
#include "cvardef.h"
#include "usercmd.h"
#include "const.h"
#include "camera.h"
#include "in_defs.h"
#include "parsemsg.h"
#include "vgui_int.h"
#include "VGUI_Font.h"
#include "vgui_CustomObjects.h"
#include "vgui_XDMViewport.h"
//#include "vgui_ServerBrowser.h"
#include "..\game_shared\vgui_LoadTGA.h"

// Arrow filenames XDM3035a: updated for new HL engine
char *sArrowFilenames[] =
{
	"arrowup",
	"arrowdown", 
	"arrowleft",
	"arrowright", 
};

//-----------------------------------------------------------------------------
// Purpose: Loads a .tga file and returns a pointer to the VGUI tga object
//-----------------------------------------------------------------------------
BitmapTGA *LoadTGAForRes(const char *pImageName)
{
	if (pImageName == NULL)
		return NULL;

	int i;
	if (ScreenWidth < 640)
		i = 320;
	else
		i = 640;

	char sz[256];
	sprintf(sz, "gfx/vgui/%d_%s.tga", i, pImageName);
	BitmapTGA *pTGA = vgui_LoadTGA(sz);

	if (pTGA == NULL)
		CON_DPRINTF("LoadTGAForRes(%s)\n error!\n", sz);

	return pTGA;
}



//-----------------------------------------------------------------------------
// Purpose: defcon
//-----------------------------------------------------------------------------
CommandButton::CommandButton(void) : Button("",0,0,1,1)
{
	m_bNoHighlight = false;
	m_bShowHotKey = true;
	m_bFlat = false;
	m_pIcon = NULL;// XDM3035a
	m_sMainText[0] = NULL;
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Base class for all HUD buttons
// Input  : *text - 
//			x y - 
//			wide tall - 
//			bNoHighlight - 
//-----------------------------------------------------------------------------
CommandButton::CommandButton(const char *text, int x, int y, int wide, int tall, bool bNoHighlight) : Button("",x,y,wide,tall)
{
	m_bNoHighlight = bNoHighlight;
	m_bShowHotKey = true;
	m_bFlat = false;
	m_pIcon = NULL;// XDM3035a
	Init();
	setText(text);
}

//-----------------------------------------------------------------------------
// Purpose: Base class for all HUD buttons
// Input  : *text - 
//			x y - 
//			wide tall - 
//			bNoHighlight - 
//			bFlat - 
//			*iconfile - 
//-----------------------------------------------------------------------------
CommandButton::CommandButton(const char *text, int x, int y, int wide, int tall, bool bNoHighlight, bool bFlat, bool bShowHotKey, char *iconfile) : Button("",x,y,wide,tall)
{
	m_bFlat = bFlat;
	m_bNoHighlight = bNoHighlight;
	m_pIcon = NULL;

	if (iconfile)// XDM3035a
		SetIcon(vgui_LoadTGA(iconfile));

	Init();
	m_bShowHotKey = bShowHotKey;
//	if (m_pIcon == NULL)
		setText(text);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CommandButton::Init(void)
{
	m_ulID = 0;
	m_pSubMenu = NULL;
	m_pSubLabel = NULL;
	m_pParentMenu = NULL;

	m_ColorNormal.setColor(Scheme::sc_primary1);
	m_ColorArmed.setColor(Scheme::sc_secondary1);
	m_ColorBgNormal.setColor(Scheme::sc_primary2);
	m_ColorBgArmed.setColor(Scheme::sc_secondary2);
	m_ColorBorderNormal.setColor(Scheme::sc_primary3);
	m_ColorBorderArmed.setColor(Scheme::sc_secondary3);
	// Set text color to orange
	setFgColor(Scheme::sc_primary1);
	setBgColor(Scheme::sc_primary2);

	// left align
	setContentAlignment(vgui::Label::a_west);

	if (m_pIcon)// XDM3035a
		setImage(m_pIcon);

	// Add the Highlight signal
	if (!m_bNoHighlight)
		addInputSignal(new CHandler_CommandButtonHighlight(this));

	// not bound to any button yet
	m_cBoundKey = 0;
//	m_bShowHotKey = false;
}

//-----------------------------------------------------------------------------
// Purpose: Prepends the button text with the current bound key
//			if no bound key, then a clear space ' ' instead
//-----------------------------------------------------------------------------
void CommandButton::RecalculateText(void)
{
	if (m_bShowHotKey)// XDM3035a
	{
		char szBuf[128];
		if (m_cBoundKey != 0)
		{
			if (m_cBoundKey == (char)255)
			{
				strcpy(szBuf, m_sMainText);
			}
			else
			{
				sprintf(szBuf, "  %c  %s", m_cBoundKey, m_sMainText);
			}
	//		szBuf[MAX_BUTTON_SIZE-1] = 0;
		}
		else// just draw a space if no key bound
		{
			sprintf(szBuf, "     %s", m_sMainText);
	//		szBuf[MAX_BUTTON_SIZE-1] = 0;
		}
		szBuf[MAX_BUTTON_SIZE-1] = 0;
		Button::setText(szBuf);
	}
	else
		Button::setText(m_sMainText);
}

void CommandButton::setText(const char *text)
{
	strncpy(m_sMainText, text, MAX_BUTTON_SIZE);
	m_sMainText[MAX_BUTTON_SIZE-1] = 0;
	RecalculateText();
}

void CommandButton::setBoundKey(char boundKey)
{
	m_cBoundKey = boundKey;
	RecalculateText();
}

char CommandButton::getBoundKey(void)
{
	return m_cBoundKey;
}

void CommandButton::SetIcon(BitmapTGA *pIcon)
{
	if (m_pIcon)
		delete m_pIcon;

	m_pIcon = pIcon;
}

void CommandButton::LoadIcon(char *pFileName)
{
	if (pFileName)
	{
		BitmapTGA *pIcon = vgui_LoadTGA(pFileName);
		if (pIcon)
			SetIcon(pIcon);
	}
}

void CommandButton::AddSubMenu(CCommandMenu *pNewMenu)
{
	m_pSubMenu = pNewMenu;
	// Prevent this button from being pushed
	setMouseClickEnabled(MOUSE_LEFT, false);
}

void CommandButton::AddSubLabel(CommandLabel *pSubLabel)
{
	m_pSubLabel = pSubLabel;
}

void CommandButton::UpdateSubMenus(int iAdjustment)
{
	if (m_pSubMenu)
		m_pSubMenu->RecalculatePositions(iAdjustment);
}

//-----------------------------------------------------------------------------
// Purpose: paint
//-----------------------------------------------------------------------------
void CommandButton::paint(void)
{
	if (m_pSubLabel)// Make the sub label paint the same as the button
	{
		if (isSelected())
			m_pSubLabel->PushDown();
		else
			m_pSubLabel->PushUp();
	}

	int r,g,b,a;
	if (isArmed())
	{
//		setFgColor(Scheme::sc_secondary1);
		m_ColorArmed.getColor(r,g,b,a);
		if (m_pIcon)
			m_pIcon->setColor(m_ColorArmed);
//			m_pIcon->setColor(vgui::Color(255,255,255, 0));
	}
	else
	{
//		setFgColor(Scheme::sc_primary1);
		m_ColorNormal.getColor(r,g,b,a);
		if (m_pIcon)
			m_pIcon->setColor(m_ColorNormal);
//			m_pIcon->setColor(vgui::Color(255,255,255, 127));
	}
	setFgColor(r,g,b,a);

	Button::paint();

//	if (m_pIcon)
//		m_pIcon->doPaint(this);
}

//-----------------------------------------------------------------------------
// Purpose: paintBackground
//-----------------------------------------------------------------------------
void CommandButton::paintBackground(void)
{
//	if (m_pIcon)
//		return;

	int r,g,b,a;
	if (m_bFlat)// no bg?
	{
		if (isArmed())
			m_ColorBorderArmed.getColor(r,g,b,a);// drawSetColor(Scheme::sc_secondary3);
		else
			m_ColorBorderNormal.getColor(r,g,b,a);// drawSetColor(Scheme::sc_primary3);

		drawSetColor(r,g,b,255-a);
		drawOutlinedRect(0,0,_size[0],_size[1]);
	}
	else
	{
//		if (isArmed())
//			drawSetColor(Scheme::sc_secondary2);
//		else
//			drawSetColor(Scheme::sc_primary2);

		if (isArmed())
			m_ColorBgArmed.getColor(r,g,b,a);// drawSetColor(Scheme::sc_secondary3);
		else
			m_ColorBgNormal.getColor(r,g,b,a);// drawSetColor(Scheme::sc_primary3);

//			a = isArmed()?0:127;

//			int r,g,b,a;
//			getBgColor(r,g,b,a);
			drawSetColor(r,g,b,a);// warning! drawSetColor uses reversed alpha!
			setBgColor(r,g,b,a);
/*
		drawFilledRect(0,0,_size[0],_size[1]);

		drawSetColor(Scheme::sc_primary3);
		drawOutlinedRect(0,0,_size[0],_size[1]);*/

		Button::paintBackground();
	}

	if (m_pIcon)
		m_pIcon->doPaint(this);
}

//-----------------------------------------------------------------------------
// Purpose: Highlights the current button, and all it's parent menus
//-----------------------------------------------------------------------------
void CommandButton::cursorEntered(void)
{
	// unarm all the other buttons in this menu
	CCommandMenu *containingMenu = getParentMenu();
	if (containingMenu)
	{
		containingMenu->ClearButtonsOfArmedState();
		// make all our higher buttons armed
		CCommandMenu *pCParent = containingMenu->GetParentMenu();
		if (pCParent)
		{
			CommandButton *pParentButton = pCParent->FindButtonWithSubmenu(containingMenu);
			pParentButton->cursorEntered();
		}
	}

	PlaySound("vgui/button_enter.wav", VOL_NORM);// XDM
	// arm ourselves
	setArmed(true);
}

//-----------------------------------------------------------------------------
// Purpose: cursorExited
//-----------------------------------------------------------------------------
void CommandButton::cursorExited(void)
{
	// only clear ourselves if we have do not have a containing menu
	// only stay armed if we have a sub menu
	// the buttons only unarm themselves when another button is armed instead
	if (!getParentMenu() || !GetSubMenu())
	{
		PlaySound("vgui/button_exit.wav", VOL_NORM);// XDM
		setArmed(false);
		setSelected(false);// XDM3035c: un-stuck hack
	}
}

//-----------------------------------------------------------------------------
// Purpose: custom callback, used by CMenuActionHandler::actionPerformed
//-----------------------------------------------------------------------------
void CommandButton::OnActionPerformed(void)
{
	PlaySound("vgui/button_press.wav", VOL_NORM);
}

//-----------------------------------------------------------------------------
// Purpose: Returns the command menu that the button is part of, if any
// Output : CCommandMenu *
//-----------------------------------------------------------------------------
CCommandMenu *CommandButton::getParentMenu(void)
{ 
	return m_pParentMenu; 
}

//-----------------------------------------------------------------------------
// Purpose: Sets the menu that contains this button
// Input  : *pParentMenu - 
//-----------------------------------------------------------------------------
void CommandButton::setParentMenu(CCommandMenu *pParentMenu)
{
	m_pParentMenu = pParentMenu;
}



//-----------------------------------------------------------------------------
// Purpose: ToggleCommandButton constructor
// Input  : *cvarname - 
//			*text - 
//			x y - 
//			wide tall - 
//			flat - 
//			bShowHotKey - 
//-----------------------------------------------------------------------------
ToggleCommandButton::ToggleCommandButton(struct cvar_s *pCVar, const char *text, int x, int y, int wide, int tall, bool flat, bool bShowHotKey) : CommandButton(text, x, y, wide, tall, false, flat, bShowHotKey)
{
	m_pCVar = pCVar;

	// Put a > to show it's a submenu
	pLabelOn = new CImageLabel("checked", 0, 0);
	pLabelOn->setParent(this);
//	pLabelOn->setEnabled(false);
	pLabelOn->addInputSignal(this);

	pLabelOff = new CImageLabel("unchecked", 0, 0);
	pLabelOff->setParent(this);
	pLabelOff->setEnabled(true);
	pLabelOff->addInputSignal(this);

	int textwide, texttall;
	getTextSize(textwide, texttall);
	// Reposition
	pLabelOn->setPos(textwide, (tall - pLabelOn->getTall()) / 2);
	pLabelOff->setPos(textwide, (tall - pLabelOff->getTall()) / 2);
	// Set text color to orange
	setFgColor(Scheme::sc_primary1);
}

//-----------------------------------------------------------------------------
// Purpose: ToggleCommandButton constructor
// Input  : *cvarname - 
//			*text - 
//			x y - 
//			wide tall - 
//			flat - 
//			bShowHotKey - 
//-----------------------------------------------------------------------------
ToggleCommandButton::ToggleCommandButton(const char *cvarname, const char *text, int x, int y, int wide, int tall, bool flat, bool bShowHotKey) : CommandButton(text, x, y, wide, tall, false, flat, bShowHotKey)
{
//	strcpy(m_szCVarName, cvarname);
	m_pCVar = CVAR_GET_POINTER(cvarname);

	// Put a > to show it's a submenu
	pLabelOn = new CImageLabel("checked", 0, 0);
	pLabelOn->setParent(this);
//	pLabelOn->setEnabled(false);
	pLabelOn->addInputSignal(this);

	pLabelOff = new CImageLabel("unchecked", 0, 0);
	pLabelOff->setParent(this);
	pLabelOff->setEnabled(true);
	pLabelOff->addInputSignal(this);

	int textwide, texttall;
	getTextSize(textwide, texttall);
	// Reposition
	pLabelOn->setPos(textwide, (tall - pLabelOn->getTall()) / 2);
	pLabelOff->setPos(textwide, (tall - pLabelOff->getTall()) / 2);
	// Set text color to orange
	setFgColor(Scheme::sc_primary1);
}

void ToggleCommandButton::cursorEntered(Panel *panel)
{
//	if (m_pCVar == NULL)// try to find it again? XDM: this helps if cvar is created after this button
//		m_pCVar = CVAR_GET_POINTER(m_szCVarName);

	CommandButton::cursorEntered();
}
/*
void ToggleCommandButton::cursorExited(Panel *panel)
{
	CommandButton::cursorExited();
}
*/
void ToggleCommandButton::mousePressed(MouseCode code, Panel *panel)
{
//	CommandButton::mousePressed(code, panel);// XDM
	doClick();
}

void ToggleCommandButton::paint(void)
{
	if (m_pCVar == NULL)
	{
		pLabelOff->setVisible(false);
		pLabelOn->setVisible(false);
	} 
	else if (m_pCVar->value > 0.0f)
	{
		pLabelOff->setVisible(false);
		pLabelOn->setVisible(true);
	}
	else
	{
		pLabelOff->setVisible(true);
		pLabelOn->setVisible(false);
	}
	CommandButton::paint();
}

/*void ToggleCommandButton::OnActionPerformed(void)
{
	if (m_pCVar)
	{
		if (m_pCVar->value > 0.0f)
			m_pCVar->value = 0.0f;
		else
			m_pCVar->value = 1.0f;
	}
}*/



//-----------------------------------------------------------------------------
// Purpose: SpectToggleButton
// Input  : cvarname - 
//			text - 
//			x - y - 
//			wide tall - 
//			flat - 
//-----------------------------------------------------------------------------
SpectToggleButton::SpectToggleButton(const char *cvarname, const char *text,int x,int y,int wide,int tall, bool flat) : CommandButton(text, x, y, wide, tall, false, flat, true)
{
	m_cvar = CVAR_GET_POINTER(cvarname);
	// Put a > to show it's a submenu
	pLabelOn = new CImageLabel("checked", 0, 0);
	pLabelOn->setParent(this);
	pLabelOn->addInputSignal(this);
	int textwide, texttall;
	getTextSize(textwide, texttall);
	// Reposition
	pLabelOn->setPos(textwide, (tall - pLabelOn->getTall()) / 2);
}

void SpectToggleButton::cursorEntered(Panel* panel)
{
	CommandButton::cursorEntered();
}

void SpectToggleButton::cursorExited(Panel* panel)
{
	CommandButton::cursorExited();
}

void SpectToggleButton::mousePressed(MouseCode code,Panel* panel)
{
	doClick();
}

void SpectToggleButton::paintBackground(void)
{
	if (isArmed())
	{
		drawSetColor(Scheme::sc_primary2);// XDM
		drawFilledRect(5, 0,_size[0] - 5,_size[1]);
	}
}

void SpectToggleButton::paint(void)
{
	if (isArmed())
		setFgColor(Scheme::sc_secondary1);// XDM
	else
		setFgColor(Scheme::sc_primary1);// XDM

	if (m_cvar == NULL)
	{
		pLabelOn->setVisible(false);
	} 
	else if (m_cvar->value)
	{
		pLabelOn->setVisible(true);
	}
	else
	{
		pLabelOn->setVisible(false);
	}
	Button::paint();
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pMapName - 
//			text - 
//			x y - 
//			wide tall - 
//-----------------------------------------------------------------------------
MapButton::MapButton(const char *pMapName, const char* text,int x,int y,int wide,int tall) : CommandButton(text,x,y,wide,tall)
{
	sprintf(m_szMapName, "maps/%s.bsp", pMapName);
}

int MapButton::IsNotValid(void)
{
	const char *level = GET_LEVEL_NAME();
	if (!level)
		return true;

	// Does it match the current map name?
	if (stricmp(m_szMapName, level))// XDM: case insensitive!
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Label with image
//-----------------------------------------------------------------------------
CImageLabel::CImageLabel() : Label()
{
	m_pImage = NULL;
	setPaintBackgroundEnabled(false);
}

CImageLabel::CImageLabel(const char *pImageName, int x, int y) : Label("", x,y)
{
	m_pImage = NULL;// !!!
	setPaintBackgroundEnabled(false);
	setContentFitted(true);
	LoadImage(pImageName);
	if (m_pImage == NULL)
		setText(pImageName);
}

CImageLabel::CImageLabel(const char *pImageName, int x, int y, int wide, int tall) : Label("", x, y, wide, tall)
{
	m_pImage = NULL;// !!!
	setPaintBackgroundEnabled(false);
	setContentFitted(true);
	LoadImage(pImageName);
	if (m_pImage)
		setSize(wide, tall);// the size was reset by LoadImage()
	else
		setText(pImageName);
}

// Image size
/*int CImageLabel::getImageWide(void)
{
	if (m_pImage)
	{
		int iXSize, iYSize;
		m_pImage->getSize(iXSize, iYSize);
		return iXSize;
	}
	else
		return 1;
}

int CImageLabel::getImageTall(void)
{
	if (m_pImage)
	{
		int iXSize, iYSize;
		m_pImage->getSize(iXSize, iYSize);
		return iYSize;
	}
	else
		return 1;
}*/

bool CImageLabel::LoadImage(const char *pImageName, bool fullpath)
{
	if (m_pImage)
		delete m_pImage;

	// Load the Image
	if (fullpath)// don't make path, it's already specified
		m_pImage = vgui_LoadTGA(pImageName);
	else
		m_pImage = LoadTGAForRes(pImageName);
/*
	if (m_pImage == NULL)
	{
		// we didn't find a matching image file for this resolution
		// try to load file resolution independent
		char sz[256];
		sprintf(sz, "%s/%s",GET_GAME_DIR(), pImageName);
		FileInputStream *fis = new FileInputStream(sz, false);
		m_pImage = new BitmapTGA(fis,true);
		fis->close();
	}
*/
	if (m_pImage == NULL)
		return false;	// unable to load image

	int w,t;
	m_pImage->getSize(w, t);
	setSize(XRES(w), YRES(t));
	setImage(m_pImage);
	return true;
}

void CImageLabel::getImageSize(int &wide, int &tall)
{
	if (m_pImage)
		m_pImage->getSize(wide, tall);
}
/*
void CImageLabel::getImageCenter(int &x, int &y)
{
	if (m_pImage)
	{
		m_pImage->getSize(x, y);
		x /= 2;
		y /= 2;
	}
}*/



//-----------------------------------------------------------------------------
// Purpose: CCustomScrollButton
// Input  : iArrow - 
//			text - 
//			x y - 
//			wide tall - 
//-----------------------------------------------------------------------------
CCustomScrollButton::CCustomScrollButton(int iArrow, const char *text, int x, int y, int wide, int tall) : CommandButton(text,x,y,wide,tall)
{
	// Load in the arrow
	m_pImage = LoadTGAForRes(sArrowFilenames[iArrow]);
	if (m_pImage)
		setImage(m_pImage);

	setFgColor(Scheme::sc_primary1);
	setContentAlignment(Label::a_center);
	setContentFitted(true);
	// Highlight signal
	addInputSignal(new CHandler_CommandButtonHighlight(this));
}

void CCustomScrollButton::paint(void)
{
	if (!m_pImage)
		return;

	if (isArmed())
		m_pImage->setColor(vgui::Color(255,255,255, 0));
	else
		m_pImage->setColor(vgui::Color(255,255,255, 127));

	m_pImage->doPaint(this);
}

/*void CCustomScrollButton::paintBackground(void)
{
	if (isArmed())
	{
		// Orange highlight background
		drawSetColor(Scheme::sc_primary2);
		drawFilledRect(0,0,_size[0],_size[1]);
	}

	// Orange Border
	drawSetColor(Scheme::sc_secondary1);
	drawOutlinedRect(0,0,_size[0]-1,_size[1]);
}*/


//-----------------------------------------------------------------------------
// Purpose: CCustomSlider: does this thing work?
// Input  : x y - 
//			wide tall - 
//			vertical - 
//-----------------------------------------------------------------------------
CCustomSlider::CCustomSlider(int x, int y, int wide, int tall, bool vertical) : Slider(x,y,wide,tall,vertical)
{
}

void CCustomSlider::paintBackground(void)
{
	int wide,tall,nobx,noby;
	getPaintSize(wide,tall);
	getNobPos(nobx,noby);

	// Border
	drawSetColor(Scheme::sc_primary3);
	drawOutlinedRect(0,0,wide,tall);

	if (isVertical())
	{
		// Nob Fill
		drawSetColor(Scheme::sc_primary1);
		drawFilledRect(0,nobx,wide,noby);
		// Nob Outline
//		if (_dragging)
//			drawSetColor(Scheme::sc_secondary3);
//		else
			drawSetColor(Scheme::sc_primary3);

		drawOutlinedRect(0,nobx,wide,noby);
	}
	else
	{
		// Nob Fill
		drawSetColor(Scheme::sc_primary1);
		drawFilledRect(nobx,0,noby,tall);
		// Nob Outline
		drawSetColor(Scheme::sc_primary3);
		drawOutlinedRect(nobx,0,noby,tall);
	}
}


//-----------------------------------------------------------------------------
// Purpose: CCustomScrollPanel
// Input  : x y - 
//			wide tall - 
//-----------------------------------------------------------------------------
CCustomScrollPanel::CCustomScrollPanel(int x, int y, int wide, int tall) : ScrollPanel(x,y,wide,tall)
{
	ScrollBar *pScrollBar = getVerticalScrollBar();
	pScrollBar->setButton(new CCustomScrollButton(ARROW_UP, "", 0,0,16,16), 0);
	pScrollBar->setButton(new CCustomScrollButton(ARROW_DOWN, "", 0,0,16,16), 1);
	pScrollBar->setSlider(new CCustomSlider(0,wide-1,wide,(tall-(wide*2))+2,true)); 
	pScrollBar->setPaintBorderEnabled(false);
	pScrollBar->setPaintBackgroundEnabled(false);
	pScrollBar->setPaintEnabled(false);

	pScrollBar = getHorizontalScrollBar();
	pScrollBar->setButton(new CCustomScrollButton(ARROW_LEFT, "", 0,0,16,16), 0);
	pScrollBar->setButton(new CCustomScrollButton(ARROW_RIGHT, "", 0,0,16,16), 1);
	pScrollBar->setSlider(new CCustomSlider(tall,0,wide-(tall*2),tall,false));
	pScrollBar->setPaintBorderEnabled(false);
	pScrollBar->setPaintBackgroundEnabled(false);
	pScrollBar->setPaintEnabled(false);
}



//============================================================
// ActionSignals
//============================================================

//-----------------------------------------------------------------------------
// Purpose: for CCommandMenu buttons
//-----------------------------------------------------------------------------
CMenuActionHandler::CMenuActionHandler(void)
{
	m_pParentMenu = NULL;
	m_bCloseMenu = false;
}

CMenuActionHandler::CMenuActionHandler(CCommandMenu	*pParentMenu, bool bCloseMenu)
{
	m_pParentMenu = pParentMenu;
	m_bCloseMenu = bCloseMenu;
}

void CMenuActionHandler::actionPerformed(Panel *panel)
{
//	if (panel->getParent == (Panel *)m_pParentMenu)
	{
		CommandButton *pButton = (CommandButton *)panel;
		pButton->OnActionPerformed();
//	PlaySound("vgui/button_press.wav", VOL_NORM);
	}
}


//-----------------------------------------------------------------------------
// Purpose: Simple command menu handler
// Input  : CCommandMenu	*pParentMenu - 
//			*pszCommand - 
//			bCloseMenu - 
//-----------------------------------------------------------------------------
CMenuHandler_StringCommand::CMenuHandler_StringCommand(CCommandMenu	*pParentMenu, char *pszCommand, bool bCloseMenu) : CMenuActionHandler(pParentMenu, bCloseMenu)
{
	strncpy(m_pszCommand, pszCommand, MAX_COMMAND_SIZE);
	m_pszCommand[MAX_COMMAND_SIZE-1] = '\0';
//	m_iCloseVGUIMenu = iClose;
}

void CMenuHandler_StringCommand::actionPerformed(Panel *panel)
{
	CLIENT_COMMAND(m_pszCommand);

	CMenuActionHandler::actionPerformed(panel);// call BEFORE, so it will not interrupt the menu sound

	if (m_pParentMenu)// XDM3035a
	{
		m_pParentMenu->Close();
	}
	else
	{
		if (m_bCloseMenu)
			gViewPort->HideTopMenu();
		else
			gViewPort->HideCommandMenu();
	}
}


//-----------------------------------------------------------------------------
// Purpose: Special signal for closing text windows
// Input  : iState - 
//-----------------------------------------------------------------------------
CMenuHandler_TextWindow::CMenuHandler_TextWindow(int iState)
{
	m_iState = iState;
}

void CMenuHandler_TextWindow::actionPerformed(Panel *panel)
{
	if (m_iState == HIDE_TEXTWINDOW)
	{
		gViewPort->HideTopMenu();
	}
	else 
	{
		gViewPort->HideCommandMenu();
		gViewPort->ShowMenu(m_iState);
	}
	CMenuActionHandler::actionPerformed(panel);
}



//-----------------------------------------------------------------------------
// Purpose: Special signal which toggles client console variables
// Input  : *pParentMenu - 
//			*pCVar - 
//-----------------------------------------------------------------------------
CMenuHandler_ToggleCvar::CMenuHandler_ToggleCvar(CCommandMenu *pParentMenu, struct cvar_s *pCVar) : CMenuActionHandler(pParentMenu, false)
{
	m_pCVar = pCVar;
}

CMenuHandler_ToggleCvar::CMenuHandler_ToggleCvar(CCommandMenu *pParentMenu, const char *cvarname) : CMenuActionHandler(pParentMenu, false)
{
	m_pCVar = CVAR_GET_POINTER(cvarname);
}

void CMenuHandler_ToggleCvar::actionPerformed(Panel *panel)
{
	if (m_pCVar != NULL)// XDM
	{
		if (m_pCVar->value > 0.0f)
			m_pCVar->value = 0.0f;
		else
			m_pCVar->value = 1.0f;
	}
	gViewPort->UpdateSpectatorPanel();
	CMenuActionHandler::actionPerformed(panel);
}




//============================================================
// InputSignals
//============================================================

//-----------------------------------------------------------------------------
// Purpose: Show submenu
// Input  : *pButton - 
//			*pSubMenu - 
//-----------------------------------------------------------------------------
CMenuHandler_PopupSubMenuInput::CMenuHandler_PopupSubMenuInput(Button *pButton, CCommandMenu *pSubMenu)
{
	m_pSubMenu = pSubMenu;
	m_pButton = pButton;
}

void CMenuHandler_PopupSubMenuInput::cursorEntered(Panel *panel) 
{
	gViewPort->SetCurrentCommandMenu(m_pSubMenu);

	if (m_pButton)
		m_pButton->setArmed(true);
}

void CMenuHandler_PopupSubMenuInput::cursorExited(Panel *panel) 
{
//	gViewPort->SetCurrentCommandMenu(m_pSubMenu);
	if (m_pButton)
		m_pButton->setArmed(false);// TEST
}


//-----------------------------------------------------------------------------
// Purpose: Input Handler for Drag N Drop panels
// Input  : *pPanel - 
//-----------------------------------------------------------------------------
CDragNDropHandler::CDragNDropHandler(DragNDropPanel *pPanel)
{
	m_pPanel = pPanel;
	m_bDragging = false;
	m_bMouseInside = false;
	m_PanelStartPos[0] = 0;
	m_PanelStartPos[1] = 0;
	m_LastMousePos[0] = 0;
	m_LastMousePos[1] = 0;
}

void CDragNDropHandler::cursorMoved(int x, int y, Panel *panel)
{
	if (m_bDragging)
	{
		App::getInstance()->getCursorPos(x,y);// sorry, we need these
		if (m_pPanel)
		{
			int wx, wy;
			m_pPanel->getPos(wx, wy);
			m_pPanel->setPos(wx+(x-m_LastMousePos[0]), wy+(y-m_LastMousePos[1]));// currentpos + lastmousedelta
//			m_pPanel->setPos(m_iaDragOrgPos[0]+(x-m_iaDragStart[0]), m_iaDragOrgPos[1]+(y-m_iaDragStart[1]));
			if (m_pPanel->getParent() != NULL)
				m_pPanel->getParent()->repaint();

//			PlaySound("slider_move.wav", VOL_NORM);
		}
		m_LastMousePos[0] = x;
		m_LastMousePos[1] = y;
	}
}

void CDragNDropHandler::cursorEntered(Panel *panel)
{
//	CON_PRINTF("cursorEntered\n");
	m_bMouseInside = 1;

	if (m_pPanel && m_pPanel->getDragEnabled())
		App::getInstance()->setCursorOveride(App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_sizeall));
//does not work	if (m_pPanel)
//		m_pPanel->setCursor(Scheme::SchemeCursor::scu_sizeall);
}

void CDragNDropHandler::cursorExited(Panel *panel)
{
//	CON_PRINTF("cursorExited\n");
	m_bMouseInside = 0;

	if (m_pPanel)
		App::getInstance()->setCursorOveride(App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_arrow));
//	if (m_pPanel)
//		m_pPanel->setCursor(Scheme::SchemeCursor::scu_last);
}

void CDragNDropHandler::mousePressed(MouseCode code, Panel *panel)
{
//	CON_PRINTF("mousePressed\n");
	if (code == MOUSE_LEFT)
	{
		if (m_bMouseInside)
		{
//			m_bMousePressed = 1;
			Drag();
		}
	}
	else if (code == MOUSE_RIGHT)// cancel
	{
		if (m_bDragging)
		{
			m_pPanel->setPos(m_PanelStartPos[0], m_PanelStartPos[1]);
			//Drop(m_PanelStartPos[0], m_PanelStartPos[1]);
			Drop();
		}
	}
} 

void CDragNDropHandler::mouseReleased(MouseCode code, Panel *panel)
{
//	CON_PRINTF("mouseReleased\n");
//	m_bMousePressed = 0;
//	Drop(m_LastMousePos[0]+m_PanelPosMouseDelta[0], m_LastMousePos[1]+m_PanelPosMouseDelta[1]);
	Drop();
}

void CDragNDropHandler::Drag(void)
{
//	CON_PRINTF("CDragNDropHandler::Drag()\n");
	if (m_pPanel)
	{
		if (m_pPanel->getDragEnabled())
		{
			int x,y;
			App::getInstance()->getCursorPos(x,y);
			m_bDragging = true;
			m_LastMousePos[0] = x;
			m_LastMousePos[1] = y;
			m_pPanel->getPos(m_PanelStartPos[0], m_PanelStartPos[1]);
//			m_PanelPosMouseDelta[0] = m_PanelStartPos[0] - x;
//			m_PanelPosMouseDelta[1] = m_PanelStartPos[1] - y;
			App::getInstance()->setMouseCapture(m_pPanel);
			m_pPanel->setDragged(m_bDragging);
			m_pPanel->requestFocus();
			PlaySound("vgui/slider_move.wav", VOL_NORM);// XDM
		}
	}
}

void CDragNDropHandler::Drop(void)//(int x, int y)
{
//	CON_PRINTF("CDragNDropHandler::Drop(%d, %d)\n", x, y);
	m_bDragging = false;
	if (m_pPanel)
	{
// allow always		if (m_pPanel->getDragEnabled())
		m_pPanel->setDragged(m_bDragging);
//		m_pPanel->setPos(x,y);
		PlaySound("vgui/button_press.wav", VOL_NORM);// XDM
		App::getInstance()->setMouseCapture(null);
	}
}


//-----------------------------------------------------------------------------
// Purpose: CHandler_MenuButtonOver
// Input  : *panel - 
//-----------------------------------------------------------------------------
void CHandler_MenuButtonOver::cursorEntered(Panel *panel)
{
	if (gViewPort && m_pMenuPanel)
		m_pMenuPanel->SetActiveInfo(m_iButton);
}



//-----------------------------------------------------------------------------
// Purpose: Default TextEntry
//-----------------------------------------------------------------------------
CDefaultTextEntry::CDefaultTextEntry(void) : TextEntry("", 0,0, 8,8)
{
//	setFgColor(Scheme::sc_primary1);
//	setBgColor(Scheme::sc_primary2);
}

CDefaultTextEntry::CDefaultTextEntry(const char *text, int x, int y, int wide, int tall) : TextEntry(text,x,y,wide,tall)
{
//	setFgColor(Scheme::sc_primary1);
//	setBgColor(Scheme::sc_primary2);
}


//-----------------------------------------------------------------------------
// Purpose: LineBorder with default color and thickness
//-----------------------------------------------------------------------------
CDefaultLineBorder::CDefaultLineBorder():LineBorder(gViewPort?gViewPort->m_iBorderThickness:1, vgui::Color(Scheme::sc_primary3))
{
}
