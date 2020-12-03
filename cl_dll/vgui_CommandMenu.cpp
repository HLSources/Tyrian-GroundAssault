#include "hud.h"
#include "cl_util.h"
#include "vgui_XDMViewport.h"
#include "../engine/keydefs.h"


// XDM: TODO: inspect this code. New button pressing mechanism is not tested!

//-----------------------------------------------------------------------------
// Purpose: Constructor
// Input  : *pParentMenu - 
//			x y - 
//			wide - 
//			tall - 
//-----------------------------------------------------------------------------
CCommandMenu::CCommandMenu(CCommandMenu *pParentMenu, int x, int y, int wide, int tall) : Panel(x,y,wide,tall)
{
	m_pParentMenu = pParentMenu;
	m_iXOffset = x;
	m_iYOffset = y;
	m_iButtons = 0;
	m_iDirection = 0;
	Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
// Input  : *pParentMenu - 
//			direction - 
//			x y - 
//			wide - 
//			tall - 
//-----------------------------------------------------------------------------
CCommandMenu::CCommandMenu(CCommandMenu *pParentMenu, int direction, int x, int y, int wide, int tall) : Panel(x,y,wide,tall)
{
	m_pParentMenu = pParentMenu;
	m_iXOffset = x;
	m_iYOffset = y;
	m_iButtons = 0;
	m_iDirection = direction;
	Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCommandMenu::Initialize(void)
{
	hCommandMenuScheme = gViewPort->GetSchemeManager()->getSchemeHandle("CommandMenu Text");
}

//-----------------------------------------------------------------------------
// Purpose: Open
//-----------------------------------------------------------------------------
void CCommandMenu::Open(void)
{
	MakeVisible(NULL);
	if (m_pParentMenu == NULL)// this is a root menu
		PlaySound("vgui/menu_activate.wav", VOL_NORM);// XDM
}

//-----------------------------------------------------------------------------
// Purpose: Close
//-----------------------------------------------------------------------------
void CCommandMenu::Close(void)
{
	ClearButtonsOfArmedState();

	if (isVisible())
	{
		setVisible(false);
		if (m_pParentMenu == NULL)// this is a root menu
			PlaySound("vgui/menu_close.wav", VOL_NORM);// XDM
	}
	gViewPort->SetCurrentCommandMenu(NULL);
	gViewPort->UpdateCursorState();
}

//-----------------------------------------------------------------------------
// Purpose: AddButton
// Input  : *pButton - 
//-----------------------------------------------------------------------------
void CCommandMenu::AddButton(CommandButton *pButton)
{
	if (m_iButtons >= MAX_BUTTONS)
		return;

	m_aButtons[m_iButtons] = pButton;
	m_iButtons++;
	pButton->setParent(this);
	pButton->setFont(Scheme::sf_primary3);

	int r,g,b,a;
	gViewPort->GetSchemeManager()->getBgColor(hCommandMenuScheme, r,g,b,a);
	pButton->setBgColor(r,g,b,a);

	// give the button a default key binding
	if (m_iButtons < 10)
	{
		pButton->setBoundKey('0' + m_iButtons);
	}
	else if (m_iButtons == 10)
	{
		pButton->setBoundKey('0');
	}
}

//-----------------------------------------------------------------------------
// Purpose: Tries to find a button that has a key bound to the input, and
//			presses the button if found
// Input  : keyNum - the character number of the input key
// Output : Returns true if the command menu should close, false otherwise
//-----------------------------------------------------------------------------
//bool CCommandMenu::KeyInput(int keyNum)

//-----------------------------------------------------------------------------
// Purpose: Tries to find a button that has a key bound to the input, and
//			presses the button if found
// Input  : &down - 
//			&keynum - 
//			*pszCurrentBinding - 
// Output : int - 0: handled, 1: unhandled, allow other panels to recieve key
//-----------------------------------------------------------------------------
int CCommandMenu::KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding)
{
	if (down)
	{
		if (keynum == K_ESCAPE || keynum == K_SPACE)// K_ESCAPE should not get here! Must be handled by UI outside!
		{
			Close();
			return 0;
		}
		if (keynum >= '0' && keynum <= '9')
		{
			// loop through all our buttons looking for one bound to keyNum
			for (int i = 0; i < m_iButtons; ++i)
			{
				if (!m_aButtons[i]->IsNotValid())
				{
					if (m_aButtons[i]->getBoundKey() == keynum)
					{
						// hit the button
						if (m_aButtons[i]->GetSubMenu())
						{
							// open the sub menu
							gViewPort->SetCurrentCommandMenu(m_aButtons[i]->GetSubMenu());
		//					return false;
							return 0;
						}
						else
						{
							// run the bound command
							m_aButtons[i]->fireActionSignal();
		//					return true;
							return 0;
						}
					}
				}
			}
		}
	}
//	return false;
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: clears the current menus buttons of any armed (highlighted) 
//			state, and all their sub buttons
//-----------------------------------------------------------------------------
void CCommandMenu::ClearButtonsOfArmedState(void)
{
	for (int i = 0; i < GetNumButtons(); ++i)
	{
		m_aButtons[i]->setArmed(false);

		if (m_aButtons[i]->GetSubMenu())
			m_aButtons[i]->GetSubMenu()->ClearButtonsOfArmedState();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pSubMenu - 
// Output : CommandButton
//-----------------------------------------------------------------------------
CommandButton *CCommandMenu::FindButtonWithSubmenu(CCommandMenu *pSubMenu)
{
	for (int i = 0; i < GetNumButtons(); ++i)
	{
		if ( m_aButtons[i]->GetSubMenu() == pSubMenu )
			return m_aButtons[i];
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Recalculate the visible buttons
// Input  : iYOffset - 
//			bHideAll - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CCommandMenu::RecalculateVisibles(int iYOffset, bool bHideAll)
{
	int		i, iCurrentY = 0;
	int		iVisibleButtons = 0;

	// Cycle through all the buttons in this menu, and see which will be visible
	for (i = 0; i < m_iButtons; ++i)
	{
		if (m_aButtons[i]->IsNotValid() || bHideAll)
		{
			m_aButtons[i]->setVisible( false );
			if (m_aButtons[i]->GetSubMenu() != NULL)
				(m_aButtons[i]->GetSubMenu())->RecalculateVisibles( 0, true );
		}
		else
		{
 			// If it's got a submenu, force it to check visibilities
			if ( m_aButtons[i]->GetSubMenu() != NULL )
			{
				if (!(m_aButtons[i]->GetSubMenu())->RecalculateVisibles( 0 , false ))
				{
					// The submenu had no visible buttons, so don't display this button
					m_aButtons[i]->setVisible( false );
					continue;
				}
			}

			m_aButtons[i]->setVisible( true );
			iVisibleButtons++;
		}
	}

	// Set Size
	setSize( _size[0], (iVisibleButtons * (m_iButtonSizeY-1)) + 1 );

	if (iYOffset)
	{
		m_iYOffset = iYOffset;
	}

	for (i = 0; i < m_iButtons; ++i)
	{
		if (m_aButtons[i]->isVisible())
		{
			if (m_aButtons[i]->GetSubMenu() != NULL)
				(m_aButtons[i]->GetSubMenu())->RecalculateVisibles( iCurrentY + m_iYOffset, false );

			// Make sure it's at the right Y position
			// m_aButtons[i]->getPos( iXPos, iYPos );
			if (m_iDirection)
			{
				m_aButtons[i]->setPos(0, (iVisibleButtons-1) * (m_iButtonSizeY-1) - iCurrentY);
			}
			else
			{
				m_aButtons[i]->setPos(0, iCurrentY);
			}

			iCurrentY += (m_iButtonSizeY-1);
		}
	}

	return iVisibleButtons?true:false;
}

//-----------------------------------------------------------------------------
// Purpose: Make sure all submenus can fit on the screen
// Input  : iYOffset - 
//-----------------------------------------------------------------------------
void CCommandMenu::RecalculatePositions(int iYOffset)
{
	int iTop;
	int iAdjust = 0;

	m_iYOffset+= iYOffset;

	if ( m_iDirection )
		iTop = ScreenHeight - (m_iYOffset + _size[1]);
	else
		iTop = m_iYOffset;

	if ( iTop < 0 )
		iTop = 0;

	// Calculate if this is going to fit onscreen, and shuffle it up if it won't
	int iBottom = iTop + _size[1];

	if ( iBottom > ScreenHeight )
	{
		// Move in increments of button sizes
		while (iAdjust < (iBottom - ScreenHeight))
		{
			iAdjust += m_iButtonSizeY - 1;
		}

		iTop -= iAdjust;

		// Make sure it doesn't move off the top of the screen (the menu's too big to fit it all)
		if ( iTop < 0 )
		{
			iAdjust -= (0 - iTop);
			iTop = 0;
		}
	}

	setPos( _pos[0], iTop );

	// We need to force all menus below this one to update their positions now, because they
	// might have submenus riding off buttons in this menu that have just shifted.
	for (int i = 0; i < m_iButtons; ++i)
		m_aButtons[i]->UpdateSubMenus( iAdjust );
}

//-----------------------------------------------------------------------------
// Purpose: Make this menu and all menus above it in the chain visible
// Input  : *pChildMenu - 
//-----------------------------------------------------------------------------
void CCommandMenu::MakeVisible(CCommandMenu *pChildMenu)
{
/*
	// Push down the button leading to the child menu
	for (int i = 0; i < m_iButtons; i++)
	{
		if ( (pChildMenu != NULL) && (m_aButtons[i]->GetSubMenu() == pChildMenu) )
		{
			m_aButtons[i]->setArmed( true );
		}
		else
		{
			m_aButtons[i]->setArmed( false );
		}
	}
*/
	setVisible(true);
//	PlaySound("vgui/menu_activate.wav", VOL_NORM);// XDM

	if (m_pParentMenu)
		m_pParentMenu->MakeVisible(this);
}

//-----------------------------------------------------------------------------
// Purpose: obsolete: don't override buttons!
//-----------------------------------------------------------------------------
/*void CCommandMenu::paintBackground(void)
{
	drawSetColor(Scheme::sc_primary3);
	drawFilledRect(0,0,_size[0],_size[1]);

  test
	int wid = _size[0]/Scheme::sc_last;
	int r,g,b,a;
	for (int i=0; i< Scheme::sc_last; ++i)
	{
		App::getInstance()->getScheme()->getColor((Scheme::SchemeColor)i, r,g,b,a);
//		drawSetColor((Scheme::SchemeColor)i);
		drawSetColor(r,g,b,255);
		drawFilledRect(wid*i,0,wid,_size[1]);
	}
}*/
