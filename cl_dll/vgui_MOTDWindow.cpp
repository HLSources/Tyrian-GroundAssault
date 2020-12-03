//=========== (C) Copyright 1996-2002 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
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
#include "const.h"
#include <VGUI_StackLayout.h>
#include "VGUI_Font.h"
#include "VGUI_ScrollPanel.h"
#include "VGUI_TextImage.h"
#include "vgui_XDMViewport.h"
#include "vgui_MOTDWindow.h"
#include "vgui_ServerBrowser.h"
#include "../engine/keydefs.h"


//-----------------------------------------------------------------------------
// Purpose: Constructs a message panel
//-----------------------------------------------------------------------------
CMessageWindowPanel::CMessageWindowPanel(const char *szMOTD, const char *szTitle, bool ShadeFullscreen, int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iRemoveMe, x, y, wide, tall)
{
	setCaptureInput(true);

	// Get the scheme used for the Titles
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();

	// schemes
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle("Title Font");
	SchemeHandle_t hMOTDText = pSchemes->getSchemeHandle("Briefing Text");

//	if (ShadeFullscreen)
//		setBackgroundMode(BG_FILL_SCREEN);
//	else
		setBackgroundMode(BG_FILL_NORMAL);

	// color schemes
	int r, g, b, a;

	setBgColor(0,0,0, ShadeFullscreen ? 100:PANEL_DEFAULT_ALPHA);// reversed alpha!
	m_bShowCursor = true;

	// since drawFilledRect cannot fill area outside the window, we need another fullscreen panel in the background
	// TODO: make the viewport draw that drawFilledRect
	// Create the window
/*	m_pBackgroundPanel = new CTransparentPanel( ShadeFullscreen ? 255 : 100, MOTD_WINDOW_X, MOTD_WINDOW_Y, MOTD_WINDOW_SIZE_X, MOTD_WINDOW_SIZE_Y );
	m_pBackgroundPanel->setParent( this );
	pSchemes->getFgColor( hTitleScheme, r, g, b, a );// XDM
	m_pBackgroundPanel->setBorder( new LineBorder( Color(r,g,b,a)) );// XDM
	m_pBackgroundPanel->setVisible( true );
	int iXSize,iYSize,iXPos,iYPos;
	m_pBackgroundPanel->getPos( iXPos,iYPos );
	m_pBackgroundPanel->getSize( iXSize,iYSize );
*/
	int iXPos = 0,iYPos = 0;
	int iXSize = wide,iYSize = tall;

	// Create the title
	if (szTitle == NULL || szTitle[0] == 0)
		szTitle =  BufferedLocaliseTextString("#Title_mapinfo");

	Label *pLabelTitle = new Label(szTitle, iXPos + XRES(MOTD_BORDER), iYPos + YRES(MOTD_BORDER));
	pLabelTitle->setParent(this);
	pLabelTitle->setFont(pSchemes->getFont(hTitleScheme));
	pLabelTitle->setFont(Scheme::sf_primary1);

	pSchemes->getFgColor(hTitleScheme, r, g, b, a);
	pLabelTitle->setFgColor(r, g, b, a);
//	pLabelTitle->setFgColor(Scheme::sc_primary1);

	pSchemes->getBgColor(hTitleScheme, r, g, b, a);
	pLabelTitle->setBgColor( r, g, b, a );
//	pLabelTitle->setBgColor(255,0,0,255);
	pLabelTitle->setContentAlignment(vgui::Label::a_west);
//	pLabelTitle->setText(szTitle);

	// Create the Scroll panel
	m_pScrollPanel = new CCustomScrollPanel(iXPos + XRES(MOTD_BORDER), iYPos + (YRES(MOTD_BORDER)*2 + pLabelTitle->getTall()),
														iXSize - XRES(MOTD_BORDER*2), iYSize - (YRES(48) + BUTTON_SIZE_Y*2));
	m_pScrollPanel->setParent(this);
	// force the scrollbars on so clientClip will take them in account after the validate
	m_pScrollPanel->setScrollBarAutoVisible(false, false);
	m_pScrollPanel->setScrollBarVisible(true, true);
	m_pScrollPanel->validate();

	// Create the text panel
	TextPanel *m_pTextPanel = new TextPanel(szMOTD, iXPos,iYPos, 64,64);
	m_pTextPanel->setParent(m_pScrollPanel->getClient());
	// get the font and colors from the scheme
	m_pTextPanel->setFont(pSchemes->getFont(hMOTDText));
	pSchemes->getFgColor(hMOTDText, r, g, b, a);
	m_pTextPanel->setFgColor(r, g, b, a);
	pSchemes->getBgColor(hMOTDText, r, g, b, a);
	m_pTextPanel->setBgColor(r, g, b, a);
//	m_pTextPanel->setText(szMOTD);
	// Get the total size of the MOTD text and resize the text panel
	int iScrollSizeX, iScrollSizeY;
	// First, set the size so that the client's wdith is correct at least because the
	//  width is critical for getting the "wrapped" size right.
	// You'll see a horizontal scroll bar if there is a single word that won't wrap in the
	//  specified width.
	m_pTextPanel->getTextImage()->setSize(m_pScrollPanel->getClientClip()->getWide(), m_pScrollPanel->getClientClip()->getTall());
	m_pTextPanel->getTextImage()->getTextSizeWrapped(iScrollSizeX, iScrollSizeY);
	// Now resize the textpanel to fit the scrolled size
	m_pTextPanel->setSize(iScrollSizeX , iScrollSizeY);

	// turn the scrollbars back into automode
	m_pScrollPanel->setScrollBarAutoVisible(true, true);
	m_pScrollPanel->setScrollBarVisible(false, false);
	m_pScrollPanel->validate();

	m_pButtonClose = new CommandButton(BufferedLocaliseTextString("#Menu_OK"),
		(iXPos + iXSize)-XRES(MOTD_BORDER)-BUTTON_SIZE_X, (iYPos + iYSize)-YRES(MOTD_BORDER)-BUTTON_SIZE_Y, BUTTON_SIZE_X, BUTTON_SIZE_Y);
			//iXPos + XRES(MOTD_BORDER), (iYPos + iYSize)-YRES(MOTD_BORDER)-BUTTON_SIZE_Y, BUTTON_SIZE_X, BUTTON_SIZE_Y);
	m_pButtonClose->setParent(this);
	m_pButtonClose->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));

	pSchemes->getFgColor(pSchemes->getSchemeHandle("Basic Text"), r, g, b, a);
	setBorder(new CDefaultLineBorder());
	setPaintBorderEnabled(true);
//	setPos(iXPos,iYPos);
}

void CMessageWindowPanel::Open(void)
{
	PlaySound("vgui/window2.wav", VOL_NORM);// XDM
	CMenuPanel::Open();
}

void CMessageWindowPanel::Close(void)
{
	PlaySound("vgui/menu_close.wav", VOL_NORM);// XDM
	CMenuPanel::Close();
}

//-----------------------------------------------------------------------------
// Purpose: Catch hotkeys
// Input  : &down - 
//			&keynum - 
//			*pszCurrentBinding - 
// Output : int - 0: handled, 1: unhandled, allow other panels to recieve key
//-----------------------------------------------------------------------------
int CMessageWindowPanel::KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding)
{
	if (down && (keynum == K_ENTER || keynum == K_KP_ENTER || keynum == K_SPACE || keynum == K_ESCAPE))
	{
		Close();
		return 0;
	}
	return 1;
}
