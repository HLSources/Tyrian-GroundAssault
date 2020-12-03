//=========== (C) Copyright 1996-2002 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: TFC Team Menu
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
#include "vgui_int.h"
#include "VGUI_Font.h"
#include "VGUI_ScrollPanel.h"
#include "VGUI_TextImage.h"
#include "vgui_XDMViewport.h"
#include "vgui_TeamMenu.h"

//-----------------------------------------------------------------------------
// Purpose: Creation
//-----------------------------------------------------------------------------
CTeamMenuPanel::CTeamMenuPanel(int iRemoveMe, int x,int y,int wide,int tall) : CMenuPanel(iRemoveMe, x,y,wide,tall)
{
	// Get the scheme used for the Titles
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	// schemes
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle("Title Font");
	SchemeHandle_t hTeamWindowText = pSchemes->getSchemeHandle("Briefing Text");
	SchemeHandle_t hTeamInfoText = pSchemes->getSchemeHandle("Team Info Text");
	// get the Font used for the Titles
	Font *pTitleFont = pSchemes->getFont(hTitleScheme);
	int r, g, b, a;

	m_bShowCursor = true;
	setBgColor(0,0,0,PANEL_DEFAULT_ALPHA);
//	setBackgroundMode(BG_FILL_SCREEN);

	// Create the title
	m_pLabelTitle = new Label("", TEAMMENU_TITLE_X, TEAMMENU_TITLE_Y);
	m_pLabelTitle->setParent(this);
	m_pLabelTitle->setFont(pTitleFont);
	pSchemes->getFgColor(hTitleScheme, r, g, b, a);
	m_pLabelTitle->setFgColor(r, g, b, a);
	pSchemes->getBgColor(hTitleScheme, r, g, b, a);
	m_pLabelTitle->setBgColor(r, g, b, a);
	m_pLabelTitle->setContentAlignment(vgui::Label::a_west);
	m_pLabelTitle->setText(BufferedLocaliseTextString("#Title_TeamMenu"));

	// Create the Info Window
//	m_pTeamWindow = new CTransparentPanel(BG_FILL_NORMAL, 255, TEAMMENU_WINDOW_X, TEAMMENU_WINDOW_Y, TEAMMENU_WINDOW_SIZE_X, TEAMMENU_WINDOW_SIZE_Y);
	m_pTeamWindow = new Panel(TEAMMENU_WINDOW_X, TEAMMENU_WINDOW_Y, TEAMMENU_WINDOW_SIZE_X, TEAMMENU_WINDOW_SIZE_Y);
	m_pTeamWindow->setParent(this);
	m_pTeamWindow->setBgColor(0,0,0,63);
	m_pTeamWindow->setBorder(new CDefaultLineBorder());

	// Create the Map Name Label
	m_pMapTitle = new Label("", TEAMMENU_WINDOW_TITLE_X, TEAMMENU_WINDOW_TITLE_Y);
	m_pMapTitle->setFont(pTitleFont); 
	m_pMapTitle->setParent(m_pTeamWindow);
	pSchemes->getFgColor(hTitleScheme, r, g, b, a);
	m_pMapTitle->setFgColor( r, g, b, a );
	pSchemes->getBgColor(hTitleScheme, r, g, b, a);
	m_pMapTitle->setBgColor( r, g, b, a );
	m_pMapTitle->setContentAlignment(vgui::Label::a_west);

	// Create the Scroll panel
	m_pScrollPanel = new CCustomScrollPanel(TEAMMENU_WINDOW_TEXT_X, TEAMMENU_WINDOW_TEXT_Y, TEAMMENU_WINDOW_SIZE_X - (TEAMMENU_WINDOW_TEXT_X * 2), TEAMMENU_WINDOW_TEXT_SIZE_Y);
	m_pScrollPanel->setParent(m_pTeamWindow);
	m_pScrollPanel->setScrollBarVisible(false, false);

	// Create the Map Briefing panel
	m_pBriefing = new TextPanel("", 0,0, TEAMMENU_WINDOW_SIZE_X - TEAMMENU_WINDOW_TEXT_X, TEAMMENU_WINDOW_TEXT_SIZE_Y);
	m_pBriefing->setParent(m_pScrollPanel->getClient());
	m_pBriefing->setFont(pSchemes->getFont(hTeamWindowText));
	pSchemes->getFgColor(hTeamWindowText, r, g, b, a);
	m_pBriefing->setFgColor(r, g, b, a);
	pSchemes->getBgColor(hTeamWindowText, r, g, b, a);
	m_pBriefing->setBgColor(r, g, b, a);
	m_pBriefing->setText(BufferedLocaliseTextString("#Map_DescNA"));

	// Team Menu buttons
	char sz[16];
	int iYPos = TEAMMENU_TOPLEFT_BUTTON_Y;
	for (int i = 0; i < NUM_TEAM_BUTTONS; ++i)
	{
//		GetTeamColor(i, r,g,b);// XDM: right now game rules are not set!
		// Team button
//		m_pButtons[i] = new CommandButton("null", TEAMMENU_TOPLEFT_BUTTON_X, iYPos, TEAMMENU_BUTTON_SIZE_X, TEAMMENU_BUTTON_SIZE_Y, true/*, false, "resource/icon_xhl16.tga" looks bad*/);
		m_pButtons[i] = new CommandButton("null", TEAMMENU_TOPLEFT_BUTTON_X, iYPos, TEAMMENU_BUTTON_SIZE_X, TEAMMENU_BUTTON_SIZE_Y, true, false, true);
		m_pButtons[i]->setParent(this);
		m_pButtons[i]->setContentAlignment(vgui::Label::a_west);
/*		m_pButtons[i]->setFgColor(r,g,b,0);
		m_pButtons[i]->m_ColorNormal.setColor(r,g,b,127);
		m_pButtons[i]->m_ColorArmed.setColor(r,g,b,0);
		m_pButtons[i]->m_ColorBorderNormal.setColor(r,g,b,0);
		m_pButtons[i]->m_ColorBorderArmed.setColor(255,255,255,0);
*/
		// AutoAssign button uses special case
		if (i == TBUTTON_AUTO)
		{
			m_pButtons[i]->setBoundKey('0');
			m_pButtons[i]->setText(BufferedLocaliseTextString("#Team_Auto"));
			m_pButtons[i]->setVisible(true);
//			strcpy(sz, "jointeam -1");
		}
		else
			m_pButtons[i]->setVisible(false);

		sprintf(sz, "jointeam \"%d\"", i);
		// !!! XDM3034 UPDATE FIX TODO: use PlayerInfo_SetValueForKey("team", teamname); but it requires writing a new ActionSignal handler
		// Create the Signals
		m_pButtons[i]->addActionSignal(new CMenuHandler_StringCommand(NULL, sz, true));// XDM: was CMenuHandler_StringCommandWatch
		m_pButtons[i]->addInputSignal(new CHandler_MenuButtonOver(this, i));

		// Create the Team Info panel
		m_pTeamInfoPanel[i] = new TextPanel("", TEAMMENU_WINDOW_INFO_X, TEAMMENU_WINDOW_INFO_Y, TEAMMENU_WINDOW_SIZE_X - TEAMMENU_WINDOW_INFO_X, TEAMMENU_WINDOW_SIZE_X - TEAMMENU_WINDOW_INFO_Y);
		m_pTeamInfoPanel[i]->setParent(m_pTeamWindow);
		m_pTeamInfoPanel[i]->setFont(pSchemes->getFont(hTeamInfoText));
		m_pTeamInfoPanel[i]->setFgColor(r,g,b, 0);
		m_pTeamInfoPanel[i]->setBgColor(0,0,0, 255);
		m_pTeamInfoPanel[i]->setPaintBackgroundEnabled(false);

		iYPos += TEAMMENU_BUTTON_SIZE_Y + TEAMMENU_BUTTON_SPACER_Y;
	}

	GetTeamColor(TEAM_NONE, r,g,b);// XDM
	// Create the Spectate button
	m_pSpectateButton = new SpectateButton(BufferedLocaliseTextString("#Menu_Spectate"), TEAMMENU_TOPLEFT_BUTTON_X, 0, TEAMMENU_BUTTON_SIZE_X, TEAMMENU_BUTTON_SIZE_Y, true);
	m_pSpectateButton->setParent(this);
	m_pSpectateButton->addActionSignal(new CMenuHandler_StringCommand(NULL, "spectate", true));
	m_pSpectateButton->setBoundKey('6');
	m_pSpectateButton->addInputSignal(new CHandler_MenuButtonOver(this, TBUTTON_SPECTATE));
	m_pSpectateButton->setFgColor(r,g,b, 0);

	// Create the Cancel button
	m_pCancelButton = new CommandButton(BufferedLocaliseTextString("#Menu_Cancel"), TEAMMENU_TOPLEFT_BUTTON_X, 0, TEAMMENU_BUTTON_SIZE_X, TEAMMENU_BUTTON_SIZE_Y);
	m_pCancelButton->setParent(this);
	m_pCancelButton->addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));

	Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: Called each time a new level is started.
//-----------------------------------------------------------------------------
void CTeamMenuPanel::Initialize(void)
{
	m_bUpdatedMapName = false;
	m_iCurrentInfo = 0;
	m_pScrollPanel->setScrollValue(0, 0);
}

//-----------------------------------------------------------------------------
// Purpose: Called everytime the Team Menu is displayed
//-----------------------------------------------------------------------------
void CTeamMenuPanel::Update(void)
{
	int	 iYPos = TEAMMENU_TOPLEFT_BUTTON_Y;
//	int r,g,b;// XDM
	// Set the team buttons
	for (int i = 1; i < NUM_TEAM_BUTTONS; ++i)// XDM: start from TBUTTON_AUTO
	{
		if (m_pButtons[i])
		{
			if (i <= gViewPort->GetNumberOfTeams())// XDM <= 
			{
				m_pButtons[i]->setText(gViewPort->GetTeamName(i));
				// bound key replacement
				char sz[4]; 
				sprintf(sz, "%d", i);
				m_pButtons[i]->setBoundKey(sz[0]);
				m_pButtons[i]->setVisible(true);
				m_pButtons[i]->setPos(TEAMMENU_TOPLEFT_BUTTON_X, iYPos);
				iYPos += TEAMMENU_BUTTON_SIZE_Y + TEAMMENU_BUTTON_SPACER_Y;
/*
				GetTeamColor(i, r,g,b);// XDM
				m_pButtons[i]->m_ColorNormal.setColor(r,g,b,127);
				m_pButtons[i]->m_ColorArmed.setColor(r,g,b,0);
				m_pButtons[i]->m_ColorBorderNormal.setColor(r,g,b,0);
				m_pButtons[i]->m_ColorBorderArmed.setColor(255,255,255,0);
*/
				// Start with the first option up
				if (m_iCurrentInfo == 0)
					continue;// XDM3035c
//					SetActiveInfo(i);

				if (m_iCurrentInfo != i)
					m_pButtons[i]->setSelected(false);// a little trick to unstuck bugged buttons

				char szPlayerList[(MAX_PLAYER_NAME_LENGTH + 3) * MAX_PLAYERS];  // name + ", "
				strcpy(szPlayerList, "\n");
				// Now count the number of teammembers of this class
				int iTotal = 0;
				for (int j = 1; j <= MAX_PLAYERS; ++j)
				{
					if (g_PlayerInfoList[j].name == NULL)
						continue; // empty player slot, skip
//why?					if (g_PlayerInfoList[j].thisplayer)
//						continue; // skip this player
					if (g_PlayerExtraInfo[j].teamnumber != i)
						continue; // skip over players in other teams

					iTotal++;
					if (iTotal > 1)
						strncat(szPlayerList, ", ", sizeof(szPlayerList) - strlen(szPlayerList));

					strncat(szPlayerList, g_PlayerInfoList[j].name, sizeof(szPlayerList) - strlen(szPlayerList));
					szPlayerList[sizeof(szPlayerList) - 1] = '\0';
				}

				if (iTotal > 0)
				{
					// Set the text of the info Panel
					char szText[((MAX_PLAYER_NAME_LENGTH + 3) * 31) + 256]; 
					sprintf(szText, "%s: %d %s (%d points)", gViewPort->GetTeamName(i), iTotal, BufferedLocaliseTextString(iTotal == 1?"#Player":"#Player_plural"), g_TeamInfo[i].frags);
					strncat(szText, szPlayerList, sizeof(szText) - strlen(szText));
					szText[sizeof(szText) - 1] = '\0';
					m_pTeamInfoPanel[i]->setText(szText);
//					m_pTeamInfoPanel[i]->setFgColor(r,g,b, 0);
				}
				else
					m_pTeamInfoPanel[i]->setText("");
			}
			else// Hide the button (may be visible from previous maps)
				m_pButtons[i]->setVisible(false);
		}
	}

	// Move the AutoAssign button into place
	m_pButtons[TBUTTON_AUTO]->setPos(TEAMMENU_TOPLEFT_BUTTON_X, iYPos);
	iYPos += TEAMMENU_BUTTON_SIZE_Y + TEAMMENU_BUTTON_SPACER_Y;

	// Spectate button
	if (m_pSpectateButton->IsNotValid())
	{
//		m_pSpectateButton->DON'T ADD! addActionSignal(new CMenuHandler_TextWindow(HIDE_TEXTWINDOW));
		m_pSpectateButton->setVisible(false);
	}
	else
	{
		m_pSpectateButton->setPos(TEAMMENU_TOPLEFT_BUTTON_X, iYPos);
		m_pSpectateButton->setVisible(true);
		iYPos += TEAMMENU_BUTTON_SIZE_Y + TEAMMENU_BUTTON_SPACER_Y;
	}

	// If the player is already in a team, make the cancel button visible
	if (gHUD.m_iTeamNumber > TEAM_NONE || gHUD.IsSpectator())// XDM3033: is this safe thing to do?
	{
		m_pCancelButton->setPos(TEAMMENU_TOPLEFT_BUTTON_X, iYPos);
		iYPos += TEAMMENU_BUTTON_SIZE_Y + TEAMMENU_BUTTON_SPACER_Y;
		m_pCancelButton->setVisible(true);
	}
	else
	{
		m_pCancelButton->setVisible(false);
	}

	// Set the Map Title
	if (!m_bUpdatedMapName)
	{
		const char *level = gViewPort->GetMapName();
		if (level && level[0])
		{
			m_pMapTitle->setText(level);
			m_pBriefing->setText(gViewPort->GetMOTD());
			m_bUpdatedMapName = true;
		}
	}

	m_pScrollPanel->validate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTeamMenuPanel::SlotInput(int iSlot)
{
	// Check for AutoAssign
	if (iSlot == TBUTTON_AUTO)
	{
		m_pButtons[iSlot]->fireActionSignal();
		return true;
	}

	// Spectate
	if (iSlot == 6)
	{
		m_pSpectateButton->fireActionSignal();
		return true;
	}

	// Otherwise, see if a particular team is selectable
	if ((iSlot < 0) || (iSlot > gViewPort->GetNumberOfTeams()))
		return false;

	if (!m_pButtons[iSlot])
		return false;

	// Is the button pushable?
	if (m_pButtons[iSlot]->isVisible())
	{
		m_pButtons[iSlot]->fireActionSignal();
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Update the Team menu before opening it
//-----------------------------------------------------------------------------
void CTeamMenuPanel::Open(void)
{
	int r,g,b;// XDM
	// Set the team buttons
	for (int i = 1; i < NUM_TEAM_BUTTONS; ++i)// XDM: start from TBUTTON_AUTO
	{
		GetTeamColor(i, r,g,b);// XDM
		if (m_pButtons[i])
		{
			m_pButtons[i]->m_ColorNormal.setColor(r,g,b,127);
			m_pButtons[i]->m_ColorArmed.setColor(r,g,b,0);
			m_pButtons[i]->m_ColorBorderNormal.setColor(r,g,b,0);
			m_pButtons[i]->m_ColorBorderArmed.setColor(255,255,255,0);
			m_pButtons[i]->setSelected(false);
		}
		if (m_pTeamInfoPanel[i])
		{
			m_pTeamInfoPanel[i]->setFgColor(min(255,r+32),min(255,g+32),min(255,b+32), 0);
			m_pTeamInfoPanel[i]->setBgColor(0,0,0, 255);
		}
	}
	m_bUpdatedMapName = false;// force
	Update();
	CMenuPanel::Open();
}

//-----------------------------------------------------------------------------
// Purpose: paintBackground
//-----------------------------------------------------------------------------
void CTeamMenuPanel::paintBackground(void)
{
	// make sure we get the map briefing up
	if (!m_bUpdatedMapName)
		Update();

	CMenuPanel::paintBackground();
}

//-----------------------------------------------------------------------------
// Purpose: Mouse is over a team button, bring up the class info
//-----------------------------------------------------------------------------
void CTeamMenuPanel::SetActiveInfo(int iInput)
{
	// Remove all the Info panels and bring up the specified one
	m_pSpectateButton->setArmed(false);
	for (int i = 0; i < NUM_TEAM_BUTTONS; ++i)
	{
		m_pButtons[i]->setArmed(false);
		m_pTeamInfoPanel[i]->setVisible(false);
	}

	if (iInput == TBUTTON_SPECTATE)// Spectate
	{
		m_pSpectateButton->setArmed(true);
	}
	else
	{
		m_pButtons[iInput]->setArmed(true);
		m_pTeamInfoPanel[iInput]->setVisible(true);
	}

	m_iCurrentInfo = iInput;
	m_pScrollPanel->validate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTeamMenuPanel::Reset(void)
{
	CMenuPanel::Reset();
	m_iCurrentInfo = 0;
}
