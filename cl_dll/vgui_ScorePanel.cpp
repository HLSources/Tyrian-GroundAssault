//=========== (C) Copyright 1996-2002 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: VGUI scoreboard
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
#include "pm_shared.h"// XDM: spectator modes
#include "vgui_XDMViewport.h"
#include <VGUI_LineBorder.h>
#include "vgui_ScorePanel.h"
#include "vgui_SpectatorPanel.h"
#include "..\game_shared\vgui_helpers.h"
#include "..\game_shared\voice_status.h"

// grid size is marked out for 640x480 screen
SBColumnInfo g_ColumnInfo[NUM_COLUMNS] =
{
	{NULL,			24,			Label::a_east},
	{"#NAME",		140,		Label::a_west},
	{"#TEAMSCORE",	56,			Label::a_west},// UNDINE: don't draw this in teamplay?
	{"#SCORE",		40,			Label::a_east},
	{"#DEATHS",		46,			Label::a_east},
	{"#LATENCY",	46,			Label::a_east},
	{"#VOICE",		40,			Label::a_east},
	{NULL,			2,			Label::a_east},		// blank column to take up the slack
};

//-----------------------------------------------------------------------------
// Purpose: sort array of TEAM IDs in DECREASING order
// Input  : *arg1 - int*
//			*arg2 - int*
// Output : int <>==0
//-----------------------------------------------------------------------------
int teaminfocmp(const void *arg1, const void *arg2)
{
	// push invalid teams to the end of list
/*
	if (*(int *)arg1 <= TEAM_NONE || *(int *)arg1 > gViewPort->GetNumberOfTeams())
		return 1;
	if (*(int *)arg2 <= TEAM_NONE || *(int *)arg2 > gViewPort->GetNumberOfTeams())
		return -1;
*/
	if (!IsActiveTeam(*(TEAM_ID *)arg1))// TEAM_NONE must be invalid here!
		return 1;
	if (!IsActiveTeam(*(TEAM_ID *)arg2))
		return -1;

	team_info_t *team1 = &g_TeamInfo[*(TEAM_ID *)arg1];
	team_info_t *team2 = &g_TeamInfo[*(TEAM_ID *)arg2];

	if (gHUD.m_iGameType > GT_TEAMPLAY)// these game types use extra team score
	{
		if (team1->scores_overriden > team2->scores_overriden)
			return -1;
		else if (team1->scores_overriden < team2->scores_overriden)
			return 1;
		// else sort by frags
	}
	if (team1->frags > team2->frags)
		return -1;
	else if (team1->frags < team2->frags)
		return 1;
	// else sort by deaths

	if (team1->deaths < team2->deaths)
		return -1;
	else if (team1->deaths > team2->deaths)
		return 1;
	// else sort by ID?

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: sort array of player IDs in DECREASING order
// Input  : *arg1 - int*
//			*arg2 - int*
// Output : int <>==0
//-----------------------------------------------------------------------------
int playerinfocmp(const void *arg1, const void *arg2)
{
	short player1 = *(short *)arg1;
	short player2 = *(short *)arg2;

	if (player1 < player2)
		return -1;
	else if (player1 > player2)
		return 1;
/*
	if (!IsValidPlayerIndex(player1) && IsValidPlayerIndex(player2))
		return -1;
	else if (IsValidPlayerIndex(player1) && !IsValidPlayerIndex(player2))
		return 1;

	if (!IsSpectator(player1) && IsSpectator(player2))
		return -1;
	else if (IsSpectator(player1) && !IsSpectator(player2))
		return 1;

	if (g_PlayerExtraInfo[player1].frags > g_PlayerExtraInfo[player2].frags)
		return -1;
	else if (g_PlayerExtraInfo[player1].frags < g_PlayerExtraInfo[player2].frags)
		return 1;
	// else sort by deaths

	if (g_PlayerExtraInfo[player1].deaths < g_PlayerExtraInfo[player2].deaths)
		return -1;
	else if (g_PlayerExtraInfo[player1].deaths > g_PlayerExtraInfo[player2].deaths)
		return 1;

	// don't have to check for != 0 here
	if (g_PlayerExtraInfo[player1].lastscoretime < g_PlayerExtraInfo[player2].lastscoretime)
		return -1;
	else if (g_PlayerExtraInfo[player1].lastscoretime > g_PlayerExtraInfo[player2].lastscoretime)
		return 1;
*/
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *arg1 - 
//			*arg2 - 
// Output : int
//-----------------------------------------------------------------------------
int playerinfocmp_lms(const void *arg1, const void *arg2)
{
	short player1 = *(short *)arg1;
	short player2 = *(short *)arg2;

	if (!IsValidPlayerIndex(player1) && IsValidPlayerIndex(player2))
		return -1;
	else if (IsValidPlayerIndex(player1) && !IsValidPlayerIndex(player2))
		return 1;

	if (!IsSpectator(player1) && IsSpectator(player2))
		return -1;
	else if (IsSpectator(player1) && !IsSpectator(player2))
		return 1;

	if (g_PlayerExtraInfo[player1].deaths < g_PlayerExtraInfo[player2].deaths)
		return -1;
	else if (g_PlayerExtraInfo[player1].deaths > g_PlayerExtraInfo[player2].deaths)
		return 1;

	if (g_PlayerExtraInfo[player1].frags > g_PlayerExtraInfo[player2].frags)
		return -1;
	else if (g_PlayerExtraInfo[player1].frags < g_PlayerExtraInfo[player2].frags)
		return 1;

	// don't have to check for != 0 here
	if (g_PlayerExtraInfo[player1].lastscoretime < g_PlayerExtraInfo[player2].lastscoretime)
		return -1;
	else if (g_PlayerExtraInfo[player1].lastscoretime > g_PlayerExtraInfo[player2].lastscoretime)
		return 1;

	return 0;
}

//-----------------------------------------------------------------------------
// ScorePanel::HitTestPanel.
//-----------------------------------------------------------------------------
void ScorePanel::HitTestPanel::internalMousePressed(MouseCode code)
{
	for(int i=0;i<_inputSignalDar.getCount();++i)
		_inputSignalDar[i]->mousePressed(code,this);
}


//-----------------------------------------------------------------------------
// Purpose: Create the ScoreBoard panel
//-----------------------------------------------------------------------------
ScorePanel::ScorePanel(int x, int y, int wide, int tall) : CMenuPanel(0, x,y,wide,tall)
{
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	ASSERT(pSchemes != NULL);
	SchemeHandle_t hScheme = pSchemes->getSchemeHandle("Scoreboard Text");
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle("Scoreboard Title Text");
	SchemeHandle_t hSmallScheme = pSchemes->getSchemeHandle("Scoreboard Small Text");
	m_pFontScore = pSchemes->getFont(hScheme);
	m_pFontTitle = pSchemes->getFont(hTitleScheme);
	m_pFontSmall = pSchemes->getFont(hSmallScheme);
	ASSERT(m_pFontSmall != NULL);
	ASSERT(m_pFontTitle != NULL);
	ASSERT(m_pFontSmall != NULL);

	m_pCurrentHighlightLabel = NULL;
	m_iHighlightRow = -1;

	setBgColor(0, 0, 0, 96);
	setBorder(new CDefaultLineBorder());
	setPaintBorderEnabled(true);
	setBackgroundMode(BG_FILL_NORMAL);

//	int xpos = g_ColumnInfo[0].m_Width + 3;
//	if (ScreenWidth >= 640)
//		xpos = XRES(xpos);// only expand column size for res greater than 640

	const int title_border_offset = 2;
	int lw = 16, lh = 16;
	m_TitleIcon.getImageSize(lw, lh);

//	int xpos = XRES(title_border_offset*2 + lw);
	// Initialize the top title.
	m_TitleLabel.setFont(m_pFontTitle);
//	m_TitleLabel.setText("");
//	m_TitleLabel.setBgColor(0,0,0,255);
//	m_TitleLabel.setBgColor(255,255,255,255);// TEST
//	m_TitleLabel.setBgColor(Scheme::sc_black);
	m_TitleLabel.setFgColor(Scheme::sc_primary1);
	m_TitleLabel.setContentAlignment(vgui::Label::a_west);
	m_TitleLabel.setBounds(XRES(title_border_offset*2 + lw), YRES(title_border_offset), wide-1, SBOARD_TITLE_SIZE_Y);// XDM3035: YRES(4), wide-1 to not overlap the border
	m_TitleLabel.setContentFitted(false);
	m_TitleLabel.setParent(this);

	// Setup the header (labels like "name", "class", etc..).
	m_HeaderGrid.setParent(this);
	m_HeaderGrid.SetDimensions(NUM_COLUMNS, 1);
	m_HeaderGrid.SetSpacing(0, 0);

	for (int i=0; i < NUM_COLUMNS; ++i)
	{
		if (g_ColumnInfo[i].m_pTitle && g_ColumnInfo[i].m_pTitle[0] == '#')
			m_HeaderLabels[i].setText(BufferedLocaliseTextString(g_ColumnInfo[i].m_pTitle));
		else if (g_ColumnInfo[i].m_pTitle)
			m_HeaderLabels[i].setText(g_ColumnInfo[i].m_pTitle);

		int xwide = g_ColumnInfo[i].m_Width;
		if (ScreenWidth >= 640)
		{
			xwide = XRES(xwide);
		}
		else if (ScreenWidth == 400)
		{
			// hack to make 400x300 resolution scoreboard fit
			if (i == 1)// reduces size of player name cell
				xwide -= 28;
			else if (i == 0)
				xwide -= 8;
		}

		m_HeaderGrid.SetColumnWidth(i, xwide);
		m_HeaderGrid.SetEntry(i, 0, &m_HeaderLabels[i]);

		m_HeaderLabels[i].setBgColor(0,0,0,255);
//		m_HeaderLabels[i].setBgColor(255,255,255,255);// TEST
		m_HeaderLabels[i].setFgColor(Scheme::sc_primary1);
		m_HeaderLabels[i].setFont(m_pFontSmall);
		m_HeaderLabels[i].setContentAlignment(g_ColumnInfo[i].m_Alignment);

		int yres = 12;
		if (ScreenHeight >= 480)
			yres = YRES(yres);

		m_HeaderLabels[i].setSize(50, yres);
	}
	// Set the width of the last column to be the remaining space.
	int ex, ey, ew, eh;
	m_HeaderGrid.GetEntryBox(NUM_COLUMNS - 2, 0, ex, ey, ew, eh);
	m_HeaderGrid.SetColumnWidth(NUM_COLUMNS - 1, (wide - X_BORDER) - (ex + ew));
	m_HeaderGrid.AutoSetRowHeights();
	m_HeaderGrid.setBounds(X_BORDER, SBOARD_TITLE_SIZE_Y, wide - X_BORDER*2, m_HeaderGrid.GetRowHeight(0));
	m_HeaderGrid.setBgColor(0,0,0,255);
	m_HeaderGrid.setPaintBackgroundEnabled(false);

	// Now setup the listbox with the actual player data in it.
	int headerX, headerY, headerWidth, headerHeight;
	m_HeaderGrid.getBounds(headerX, headerY, headerWidth, headerHeight);
	m_PlayerList.setBounds(headerX, headerY+headerHeight, headerWidth, tall - headerY - headerHeight - YRES(SBOARD_BOTTOM_LABEL_Y));
	m_PlayerList.setParent(this);
	m_PlayerList.setBgColor(0,0,0,255);
//	m_PlayerList.setBgColor(191,255,255,127);// TEST
	m_PlayerList.setPaintBackgroundEnabled(false);

	for (int row=0; row < NUM_ROWS; ++row)
	{
		CGrid *pGridRow = &m_PlayerGrids[row];
		pGridRow->SetDimensions(NUM_COLUMNS, 1);
		for(int col=0; col < NUM_COLUMNS; ++col)
		{
			m_PlayerEntries[col][row].setContentFitted(false);
			m_PlayerEntries[col][row].setRow(row);
			m_PlayerEntries[col][row].addInputSignal(this);
			pGridRow->SetEntry(col, 0, &m_PlayerEntries[col][row]);
		}
		pGridRow->setBgColor(0,0,0,255);
//		pGridRow->SetSpacing(2, 0);
		pGridRow->SetSpacing(0, 0);
		pGridRow->CopyColumnWidths(&m_HeaderGrid);
		pGridRow->AutoSetRowHeights();
		pGridRow->setSize(PanelWidth(pGridRow), pGridRow->CalcDrawHeight());
		pGridRow->RepositionContents();
		m_PlayerList.AddItem(pGridRow);
	}

	// Add the hit test panel. It is invisible and traps mouse clicks so we can go into squelch mode.
	m_HitTestPanel.setBgColor(0,0,0,255);
	m_HitTestPanel.setPaintBackgroundEnabled(false);
	m_HitTestPanel.setParent(this);
	m_HitTestPanel.setBounds(0, 0, wide, tall);
	m_HitTestPanel.addInputSignal(this);

	m_pCloseButton = new CommandButton("x", wide-XRES(12 + 4), YRES(2), XRES(12), YRES(12));
	m_pCloseButton->setParent(this);
	m_pCloseButton->addActionSignal(new CMenuHandler_StringCommand(NULL, "-showscores", true));
	m_pCloseButton->setBgColor(0,0,0,255);
	m_pCloseButton->setFgColor(255,255,255,0);
	m_pCloseButton->setFont(m_pFontTitle);
	m_pCloseButton->setBoundKey((char)255);
	m_pCloseButton->setContentAlignment(Label::a_center);

	const char *label = "00:00";
	m_pFontSmall->getTextSize(label, lw, lh);

	// XDM3035: status bar indicated game limits: score and frags
//	LocaliseTextString("%s @ %s, #Score_limit: #TEAMSCORE %d, #SCORE %d\0", m_szScoreLimitLabelFmt, SBOARD_BOTTOM_TEXT_LEN/*sizeof(char)*/);
	m_BottomLabel.setParent(this);
	m_BottomLabel.setBounds(X_BORDER, tall-SBOARD_BOTTOM_LABEL_Y-YRES(PANEL_INNER_OFFSET), wide-m_CurrentTimeLabel.getWide()-X_BORDER*2, SBOARD_BOTTOM_LABEL_Y);
	m_BottomLabel.setFont(m_pFontSmall);
	m_BottomLabel.setFgColor(Scheme::sc_primary1);
	m_BottomLabel.setContentAlignment(vgui::Label::a_west);
//	m_BottomLabel.setContentFitted(true);
//test	m_BottomLabel.setBgColor(0,255,255,127);
	m_BottomLabel.setPaintBackgroundEnabled(false);
//	m_BottomLabel.setText(m_szScoreLimitLabelFmt);

	m_CurrentTimeLabel.setParent(this);
//	m_CurrentTimeLabel.setPos(wide-XRES(lw+2), tall-YRES(lh+1));// XDM
	m_CurrentTimeLabel.setBounds(wide-XRES(lw+2)-X_BORDER, tall-SBOARD_BOTTOM_LABEL_Y-YRES(PANEL_INNER_OFFSET)/*YRES(lh+1)*/, lw+2, SBOARD_BOTTOM_LABEL_Y);
	m_CurrentTimeLabel.setFont(m_pFontSmall);
	m_CurrentTimeLabel.setFgColor(Scheme::sc_primary1);
	m_CurrentTimeLabel.setContentAlignment(vgui::Label::a_east);
//	m_CurrentTimeLabel.setContentFitted(true);
	m_CurrentTimeLabel.setPaintBackgroundEnabled(false);
//	m_CurrentTimeLabel.setText(label);

	Initialize();
}


//-----------------------------------------------------------------------------
// Purpose: Called each time a new level is started.
//-----------------------------------------------------------------------------
void ScorePanel::Initialize(void)
{
	// Clear out scoreboard data
	m_iLastKilledBy = 0;
	m_fLastKillDisplayStopTime = 0;
	m_iPlayerNum = 0;
	m_iShowscoresHeld = 0;
	m_iHighlightRow = 0;

//	memset(g_PlayerExtraInfo, 0, sizeof(extra_player_info_t)*(MAX_PLAYERS+1));
//	memset(g_TeamInfo, 0, sizeof(team_info_t)*(MAX_TEAMS+1));

	unsigned short i = 0;
	for (i = 0; i < NUM_ROWS; ++i)
	{
		m_iSortedRows[i] = 0;
		m_iRowType[i] = 0;
	}

	for (i = 0; i <= MAX_PLAYERS; ++i)
	{
//		m_SortedPlayers[i] = 0;
		m_bHasBeenSorted[i] = false;
	}

//	for (i = 0; i < MAX_TEAMS; ++i)
//		m_bHasBeenSortedTeam[i] = false;

	for (i = 0; i <= MAX_TEAMS; ++i)
		m_SortedTeams[i] = TEAM_NONE;

	if (IsRoundBasedGame(gHUD.m_iGameType))// XDM3037
		LocaliseTextString("%s @ %s, #ROUND %d/%d, #Score_limit: #TEAMSCORE %d, #SCORE %d\0", m_szScoreLimitLabelFmt, SBOARD_BOTTOM_TEXT_LEN/*sizeof(char)*/);
	else
		LocaliseTextString("%s @ %s, #Score_limit: #TEAMSCORE %d, #SCORE %d\0", m_szScoreLimitLabelFmt, SBOARD_BOTTOM_TEXT_LEN/*sizeof(char)*/);
}

//-----------------------------------------------------------------------------
// Purpose: OBSOLETE: Update scoreboard title text
//-----------------------------------------------------------------------------
/*void ScorePanel::UpdateTitle(void)
{
	// Set the title
	if (gViewPort->m_szServerName && gViewPort->GetMapName())
	{
cut
	}
}*/

//-----------------------------------------------------------------------------
// Purpose: Recalculate the internal scoreboard data
//-----------------------------------------------------------------------------
void ScorePanel::Update(void)
{
	if (gViewPort->m_szServerName && gViewPort->GetMapName())
	{
		// Fill score limit text at the bottom
		int fraglimit = (int)CVAR_GET_FLOAT("mp_fraglimit");// XDM3035: these won't get sent from a dedicated server, but when available are useful
		int scorelimit = (int)CVAR_GET_FLOAT("mp_scorelimit");
		if (fraglimit > 0)
			gHUD.m_iFragLimit = fraglimit;
		if (scorelimit > 0)
			gHUD.m_iScoreLimit = scorelimit;

//		if (gHUD.m_iFragLimit > 0 || gHUD.m_iScoreLimit > 0)
		{
			char szScoreLimitLabelText[128];
			if (IsRoundBasedGame(gHUD.m_iGameType))// XDM3037: print round info
				sprintf(szScoreLimitLabelText, m_szScoreLimitLabelFmt, GetGameDescription(gHUD.m_iGameType), gViewPort->GetMapName(), gHUD.m_iRoundsPlayed, gHUD.m_iRoundsLimit, gHUD.m_iScoreLimit, gHUD.m_iFragLimit);
			else
				sprintf(szScoreLimitLabelText, m_szScoreLimitLabelFmt, GetGameDescription(gHUD.m_iGameType), gViewPort->GetMapName(), gHUD.m_iScoreLimit, gHUD.m_iFragLimit);

			m_BottomLabel.setText(szScoreLimitLabelText);
		}

//		UpdateTitle();
//		CON_PRINTF(" -- g_iUser1 %d g_iUser2 %d g_iUser3 %d\n", g_iUser1, g_iUser2, g_iUser3);
		if (g_iUser1 == OBS_INTERMISSION && g_iUser2 > 0)// XDM3035: INTERMISSION: g_iUser2 is the winner
		{
			if (!IsTeamGame(gHUD.m_iGameType))// single winner
			{
				if (g_PlayerInfoList[g_iUser2].name != NULL)// get winner's name
				{
					char label[48];
					client_textmessage_t *msg = TextMessageGet("MP_WIN_PLAYER");
					if (msg)
						sprintf(label, msg->pMessage, g_PlayerInfoList[g_iUser2].name); 
					else
						sprintf(label, "%s is the winner!\0", g_PlayerInfoList[g_iUser2].name); 

					m_TitleLabel.setText(label);
				}
			}
			else// team wins, iuser2 is the best player
			{
				TEAM_ID team = g_PlayerExtraInfo[g_iUser2].teamnumber;// get winner's team ID
				if (IsActiveTeam(team))
				{
					char label[48];
					client_textmessage_t *msg = TextMessageGet("MP_WIN_TEAM");
					if (msg)
						sprintf(label, msg->pMessage, g_TeamInfo[team].name); 
					else
						sprintf(label, "Team %s wins!\0", g_TeamInfo[team].name); 

					m_TitleLabel.setText(label);
				}
			}
		}
		else// game continues, display normal title
		{
			char szTitleText[MAX_SERVERNAME_LENGTH + 48];
			sprintf(szTitleText, "%s - %s\0", gViewPort->m_szServerName, gViewPort->GetMapName());// XDM
			m_TitleLabel.setText(szTitleText);
		}
	}

	m_iRows = 0;
// XDM3037: TESTME	gViewPort->GetAllPlayersInfo();

	// Clear out sorts
	int i = 0;
	for (i = 0; i < NUM_ROWS; ++i)
	{
		m_iSortedRows[i] = 0;
		m_iRowType[i] = SBOARD_ROW_BLANK;
	}

//		m_SortedPlayers[0...31] = i+1;

	for (i = 0; i <= MAX_PLAYERS; ++i)// XDM3037: fixed incl 32
		m_bHasBeenSorted[i] = false;

	m_iBestPlayer = 0;// XDM3037

	// XDM3037: this operates with ALL players at one time so don't need to repeat this for every team
	// UNDONE: why does this cause memory corruption?!!!
/*	if (gHUD.m_iGameType == GT_LMS)
		qsort((void *)m_SortedPlayers, MAX_PLAYERS, sizeof(m_SortedPlayers[0]), playerinfocmp_lms);
	else
		qsort((void *)m_SortedPlayers, MAX_PLAYERS, sizeof(m_SortedPlayers[0]), playerinfocmp);
*/
	// If it's not teamplay, sort all the players. Otherwise, sort the teams.
	if (IsTeamGame(gHUD.m_iGameType))
	{
		SortTeams();
//		SortPlayers(TEAM_NONE, true);// XDM3035a
	}
	else
	{
		SortPlayers(TEAM_NONE, false);
		AddRow(TEAM_NONE, SBOARD_ROW_TEAM);// XDM3035c: spectators
		SortPlayers(TEAM_NONE, true);// XDM3035a
	}

	// set scrollbar range
	m_PlayerList.SetScrollRange(m_iRows);

	FillGrid();
	UpdateCounters();

	if (gViewPort->GetSpectatorPanel()->m_menuVisible)
		 m_pCloseButton->setVisible(true);
	else 
		 m_pCloseButton->setVisible(false);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ScorePanel::UpdateCounters(void)
{
	if (isVisible() == false)
		return;

	if (gHUD.m_flTimeLeft > 0.0f)
	{
		char szTimeLeftText[16];
		_snprintf(szTimeLeftText, 16, "%d:%02d\0", (int)(gHUD.m_flTimeLeft/60), ((int)gHUD.m_flTimeLeft % 60));
		szTimeLeftText[15] = 0;
		m_CurrentTimeLabel.setText(szTimeLeftText);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sort all the teams
//-----------------------------------------------------------------------------
void ScorePanel::SortTeams(void)
{
	int i = 0;
//	for (i = 0; i < MAX_TEAMS; ++i)
//		m_bHasBeenSortedTeam[i] = false;
	// clear out team scores
	for (i = TEAM_NONE; i <= gViewPort->GetNumberOfTeams(); ++i)// XDM: 0 <=
	{
		g_TeamInfo[i].frags = 0;
// don't touch g_TeamInfo[i].scores_overriden;// XDM: extra score
		g_TeamInfo[i].deaths = 0;
		g_TeamInfo[i].ping = 0;
		g_TeamInfo[i].packetloss = 0;
//		m_bHasBeenSortedTeam[i] = false;
	}

	// recalc the team scores, then draw them
	for (i = 1; i <= MAX_PLAYERS; ++i)
	{
//		m_bHasBeenSorted[i] = false;
		if (g_PlayerInfoList[i].name == NULL)
			continue; // empty player slot, skip

// XDM3035a		if (g_PlayerExtraInfo[i].teamnumber < TEAM_NONE)// XDM
//			continue; // skip over players who are not in a team

		TEAM_ID playerteam = g_PlayerExtraInfo[i].teamnumber;
		ASSERT(IsValidTeam(playerteam));

// XDM3035a		if (j <= TEAM_NONE || j >= gViewPort->GetNumberOfTeams())  // player is not in a team, skip to the next guy
//			continue;

		if (IsSpectator(i))
		{
			if (playerteam != TEAM_NONE)
				playerteam = TEAM_NONE;
		}

		if (IsActivePlayer(i))//IsValidPlayerIndex(i))// IsActivePlayer filters out spectators
		{
			g_TeamInfo[playerteam].frags += g_PlayerExtraInfo[i].frags;
			g_TeamInfo[playerteam].deaths += g_PlayerExtraInfo[i].deaths;
			g_TeamInfo[playerteam].ping += g_PlayerInfoList[i].ping;
			g_TeamInfo[playerteam].packetloss += g_PlayerInfoList[i].packetloss;
		}
	}

	memset(m_SortedTeams, 0, sizeof(m_SortedTeams));

	// find team ping/packetloss averages
	for (i = 0; i <= gViewPort->GetNumberOfTeams(); ++i)// XDM3035c: 0 <=
	{
//		m_bHasBeenSortedTeam[i] = false;
		if (g_TeamInfo[i].players > 0)
		{
			g_TeamInfo[i].ping /= g_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
			g_TeamInfo[i].packetloss /= g_TeamInfo[i].players;
		}
		m_SortedTeams[i] = i;
	}

	// XDM: speedup???
	qsort((void *)m_SortedTeams, MAX_TEAMS+1, sizeof(m_SortedTeams[0]), teaminfocmp);

	TEAM_ID team_id;
	// Iterate from the beginning (if some teams are inactive, they're left at the end of list)
	// 'i' is the index in the m_SortedTeams array
	for (i = 0; i < gViewPort->GetNumberOfTeams()/*-1 to skip spectators*/; ++i)// XDM3035c: TEST < or <=
	{
		team_id = m_SortedTeams[i];// use team from the sorted list
//		m_bHasBeenSortedTeam[team_id] = true;// set to TRUE, so this team won't get sorted again

		AddRow(team_id, SBOARD_ROW_TEAM);

		// Now sort all the players on this team
		SortPlayers(/*SBOARD_ROW_PLAYER, */team_id, (team_id == TEAM_NONE));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sort a list of players
// Note: XDM3037: now it merely fills the grid
// Fills rows, must be called with proper sequence in mind (e.g. after writing each team header)
//-----------------------------------------------------------------------------
void ScorePanel::SortPlayers(/*const int &iRowType, */const TEAM_ID &teamindex, bool bIncludeSpectators)
{
	int highest_frags = -99999;
	int lowest_deaths = 262144;
	float lastscoretime = 262144.0f;
	int best_player = 0;
	bool first = true;

	// draw the players, in order, and restricted to team if set
	while (1)
	{
		// Find the top ranking player
		highest_frags = -99999;
		lowest_deaths = 262144;
		lastscoretime = 262144.0f;
		best_player = 0;

		for (int i = 1; i <= MAX_PLAYERS; ++i)
		{
			if (m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name && g_PlayerExtraInfo[i].frags >= highest_frags)
			{
//				CON_DPRINTF("SortPlayers: processing %d (%s)\n", i, g_PlayerInfoList[i].name);
				if (IsSpectator(i) && !bIncludeSpectators)//iRowType == SBOARD_ROW_PLAYER)//!= SBOARD_ROW_SPECTATORS)// XDM
				{
//					CON_DPRINTF("SortPlayers: dropped spectator %d %s\n", i, g_PlayerInfoList[i].name);
					continue;
				}
				if (g_PlayerExtraInfo[i].teamnumber == teamindex)
				{
					if ((g_PlayerExtraInfo[i].frags > highest_frags) ||
						(g_PlayerExtraInfo[i].frags == highest_frags && (g_PlayerExtraInfo[i].deaths < lowest_deaths ||
																		(g_PlayerExtraInfo[i].deaths == lowest_deaths && g_PlayerExtraInfo[i].lastscoretime < lastscoretime))))// can leave lastscoretime != 0 unchecked here
					{
						best_player = i;
						lowest_deaths = g_PlayerExtraInfo[i].deaths;
						highest_frags = g_PlayerExtraInfo[i].frags;
						lastscoretime = g_PlayerExtraInfo[i].lastscoretime;
					}
				}
			}
		}

		if (best_player <= 0)
			break;

		m_bHasBeenSorted[best_player] = true;// restart excluding this player
		if (first)
		{
			if (teamindex == GetBestTeam())// XDM3037: absolute best player
				m_iBestPlayer = best_player;

			first = false;
		}
//		CON_DPRINTF("SortPlayers(%d): AddRow %d (%s)\n", teamindex, best_player, g_PlayerInfoList[best_player].name);
		AddRow(best_player, SBOARD_ROW_PLAYER);//iRowType);
	}

/*	for (unsigned short i = 0; i < gEngfuncs.GetMaxClients(); ++i)// i is index of array, not player ID. Empty player slots are at the bottom (higher indexes), so ignore them
	{
		if (IsSpectator(m_SortedPlayers[i]) && !bIncludeSpectators)
			continue;

		if (g_PlayerExtraInfo[m_SortedPlayers[i]].teamnumber == teamindex)
			AddRow(m_SortedPlayers[i], SBOARD_ROW_PLAYER);
	}*/
	AddRow(0, SBOARD_ROW_BLANK);
}

//-----------------------------------------------------------------------------
// Purpose: Recalculate the existing teams in the match
// Note: Includes call to Update();
//-----------------------------------------------------------------------------
void ScorePanel::RebuildTeams(void)
{
	int i = 0;
	// clear out player counts from teams
	for (i = 0; i < MAX_TEAMS+1; ++i)// XDM3035: clear all and spectators even in non-teamplay
		g_TeamInfo[i].players = 0;

	// rebuild the team list
	gViewPort->GetAllPlayersInfo();

	for (i = 1; i <= MAX_PLAYERS; ++i)
	{
		if (g_PlayerInfoList[i].name == NULL)
			continue;

		if (g_PlayerInfoList[i].thisplayer)// XDM: moved from GetAllPlayersInfo()
			m_iPlayerNum = i;

		if (/*g_PlayerExtraInfo[i].teamnumber < TEAM_NONE || */g_PlayerExtraInfo[i].teamnumber > MAX_TEAMS)// XDM: 0 means spectators
			continue; // skip over players who are not in a team

		g_TeamInfo[g_PlayerExtraInfo[i].teamnumber].players++;
	}

	// clear out any empty teams
	for (i = 0; i <= MAX_TEAMS; ++i)
	{
		if (g_TeamInfo[i].players < 1)
		{
// XDM: DON'T CLEAR NAME!!			memset(&g_TeamInfo[i], 0, sizeof(team_info_t));
//			g_TeamInfo[i].already_drawn = 0;
			g_TeamInfo[i].deaths = 0;
			g_TeamInfo[i].frags = 0;
//			g_TeamInfo[i].ownteam = 0;
			g_TeamInfo[i].packetloss = 0;
			g_TeamInfo[i].ping = 0;
			g_TeamInfo[i].scores_overriden = 0;
		}
	}

	// Update the scoreboard
	Update();
}

//-----------------------------------------------------------------------------
// Purpose: FillGrid
//-----------------------------------------------------------------------------
void ScorePanel::FillGrid(void)
{
	// update highlight position
	int x, y;
	getApp()->getCursorPos(x, y);
	cursorMoved(x, y, this);

	// remove highlight row if we're not in squelch mode
	if (!GetClientVoiceMgr()->IsInSquelchMode())
		m_iHighlightRow = -1;

	bool bNextRowIsGap = false;

	for(int row=0; row < NUM_ROWS; ++row)
	{
		CGrid *pGridRow = &m_PlayerGrids[row];
		pGridRow->SetRowUnderline(0, false, 0, 0, 0, 0, 0);

		if (row >= m_iRows)
		{
			for(int col=0; col < NUM_COLUMNS; ++col)
				m_PlayerEntries[col][row].setVisible(false);

			continue;
		}

		bool bRowIsGap = false;
		if (bNextRowIsGap)
		{
			bNextRowIsGap = false;
			bRowIsGap = true;
		}

		byte r,g,b;// XDM
		char sz[128];
		hud_player_info_t *pl_info;
		team_info_t *team_info;
		for (int col=0; col < NUM_COLUMNS; ++col)
		{
			pl_info = NULL;
			team_info = NULL;
			CLabelHeader *pLabel = &m_PlayerEntries[col][row];

			pLabel->setVisible(true);
			pLabel->setText2("");
			pLabel->setImage(NULL);
			pLabel->setFont(m_pFontScore);
			pLabel->setTextOffset(0, 0);
			
			int rowheight = 13;
			if (ScreenHeight > 480)
				rowheight = YRES(rowheight);
			else
				rowheight = 15;// more tweaking, make sure icons fit at low res

			pLabel->setSize(pLabel->getWide(), rowheight);
			pLabel->setPaintBackgroundEnabled(true);
//			pLabel->setBgColor(0,0,0, 255);

			// Set color
			if (m_iRowType[row] == SBOARD_ROW_PLAYER)// XDM: fastest check should go first
			{
				// Get the player's data
				pl_info = &g_PlayerInfoList[m_iSortedRows[row]];
				// special text color for player names
//XDM useless :(				if (pl_info->spectator)
				if (IsSpectator(m_iSortedRows[row]))// XDM
					GetTeamColor(TEAM_NONE, r,g,b);
				else
				{
					if (gHUD.m_pCvarUsePlayerColor->value > 0.0f)
						GetPlayerColor(m_iSortedRows[row], r,g,b);// XDM3035: TESTME!! May be slow!
					else
						GetTeamColor(g_PlayerExtraInfo[m_iSortedRows[row]].teamnumber, r,g,b);
				}
				pLabel->setFgColor(r,g,b,0);
				// Set background color
				if (pl_info->thisplayer) // if it is their name, draw it a different color
				{
					pLabel->setFgColor(Scheme::sc_white);// Highlight LOCAL player
					pLabel->setBgColor(r,g,b, 196);// XDM
				}
				else if (m_iLastKilledBy > 0 && m_iSortedRows[row] == m_iLastKilledBy && ((gHUD.m_iRevengeMode > 0) || m_fLastKillDisplayStopTime > gHUD.m_flTime))// check time here too, so bg color will stay default
				{
					// This function (luckily) doesn't get called often, so don't update fade effect here, just set the color.
					UnpackRGB(r,g,b, RGB_RED);
					pLabel->setBgColor(r,g,b,127);// just set color, but don't do any calculations
				}
				else if ((g_iUser1 > OBS_NONE/*== OBS_CHASE_LOCKED || g_iUser1 == OBS_CHASE_FREE || g_iUser1 == OBS_IN_EYE*/) && g_iUser2 == m_iSortedRows[row])// XDM: spectator's target
				{
					UnpackRGB(r,g,b, RGB_CYAN);
					pLabel->setBgColor(r,g,b,191);
					//pLabel->setBgColor(SBOARD_COLOR_SPECTARGET_BG_R,SBOARD_COLOR_SPECTARGET_BG_G,SBOARD_COLOR_SPECTARGET_BG_B,SBOARD_COLOR_SPECTARGET_BG_A);
//					setBgColor(Scheme::sc_secondary2);
				}
/* useless because user may move mouse between FillFrid() calls				else if (row == m_iHighlightRow)
				{
					pLabel->setBgColor(Scheme::sc_secondary2);
				}*/
				else
				{
					pLabel->setBgColor(0,0,0, 255);
					pLabel->setPaintBackgroundEnabled(false);// don't bother
					// but this makes m_iHighlightRow transparent too. I consider this a harmless side-effect.
				}
			}
			else if (m_iRowType[row] == SBOARD_ROW_TEAM)// || m_iRowType[row] == SBOARD_ROW_SPECTATORS)
			{
				pLabel->setBgColor(0,0,0, 255);
				if (IsValidTeam(m_iSortedRows[row]))
				{
					team_info = &g_TeamInfo[m_iSortedRows[row]];
					// different height for team header rows
					rowheight = 20;
					if (ScreenHeight >= 480)
						rowheight = YRES(rowheight);
					// team color text for team names
					GetTeamColor(m_iSortedRows[row], r,g,b);// XDM
					pLabel->setFgColor(r,g,b,0);
					pLabel->setSize(pLabel->getWide(), rowheight);
					pLabel->setFont(m_pFontTitle);
					pGridRow->SetRowUnderline(0, true, YRES(3), r,g,b,0);// XDM
				}
				else
				{
					pLabel->setText("ERROR");
				}
			}
			else if (m_iRowType[row] == SBOARD_ROW_BLANK)
			{
				pLabel->setBgColor(0,0,0, 255);
				pLabel->setText(" ");
				continue;
			}

			// Align 
			if (col == COLUMN_NAME || col == COLUMN_TSCORE)
			{
				pLabel->setContentAlignment(vgui::Label::a_west);
			}
			else if (col == COLUMN_TRACKER)
			{
				pLabel->setContentAlignment(vgui::Label::a_center);
			}
			else
			{
				pLabel->setContentAlignment(vgui::Label::a_east);
			}

			// Fill out with the correct data
			strcpy(sz, "");

			if (m_iRowType[row] == SBOARD_ROW_PLAYER)
			{
			if (pl_info)// XDM3035a: can be null?
			{
				if (col == COLUMN_NAME)// XDM: some optimization
				{
					if (g_PlayerExtraInfo[m_iSortedRows[row]].ready)
					{
						sz[0] = '*';
						sz[1] = 0;
					}
					if (pl_info->name)
						strcat(sz, pl_info->name);
					//sprintf(sz, "%s  ", pl_info->name);
				}
//column for teams only				else if (col == COLUMN_TSCORE)
//				{
//					sprintf(sz, "");
//				}
				else if (col == COLUMN_KILLS)
				{
					sprintf(sz, "%d",  g_PlayerExtraInfo[m_iSortedRows[row]].frags);
				}
				else if (col == COLUMN_DEATHS)
				{
					sprintf(sz, "%d",  g_PlayerExtraInfo[m_iSortedRows[row]].deaths);
				}
				else if (col == COLUMN_LATENCY)
				{
					sprintf(sz, "%d", g_PlayerInfoList[m_iSortedRows[row]].ping);
				}
				else if (col == COLUMN_VOICE)
				{
					sz[0] = 0;
					// in HLTV mode allow spectator to turn on/off commentator voice
					if (!pl_info->thisplayer || gEngfuncs.IsSpectateOnly())
						GetClientVoiceMgr()->UpdateSpeakerImage(pLabel, m_iSortedRows[row]);
				}
			}
			}
			else if (m_iRowType[row] == SBOARD_ROW_TEAM)// || m_iRowType[row] == SBOARD_ROW_SPECTATORS)
			{
				if (col == COLUMN_NAME)// XDM: some optimization
				{
//					if (m_iRowType[row] == SBOARD_ROW_SPECTATORS)// XDM3035a: server sends an empty string
					if (m_iSortedRows[row] == TEAM_NONE)
						sprintf(sz, BufferedLocaliseTextString("#Spectators"));
					else
						sprintf(sz, gViewPort->GetTeamName(m_iSortedRows[row]));//team_info->teamnumber));

					// Append the number of players
					if (IsTeamGame(gHUD.m_iGameType) && team_info)// were used for spectators && m_iRowType[row] != SBOARD_ROW_PLAYER)// 3033 == SBOARD_ROW_TEAM)
					{
						char sz2[128];
						_snprintf(sz2, 128, "(%d %s)\0", team_info->players, BufferedLocaliseTextString((team_info->players == 1)?"#Player":"#Player_plural"));
						pLabel->setText2(sz2);
						pLabel->setFont2(m_pFontSmall);
					}
				}
				else
				{
					if (m_iRowType[row] == SBOARD_ROW_TEAM)
					{
						//if (m_iSortedRows[row] != TEAM_NONE)// not for spectators
						if (IsActiveTeam(m_iSortedRows[row]))// not for spectators
						{
						if (col == COLUMN_TSCORE)
							sprintf(sz, "%d",  team_info->scores_overriden);
						else if (col == COLUMN_KILLS)
							sprintf(sz, "%d",  team_info->frags);
						else if (col == COLUMN_DEATHS)
							sprintf(sz, "%d",  team_info->deaths);
						else if (col == COLUMN_LATENCY)
							sprintf(sz, "%d", team_info->ping);
						}
					}
				}
			}
			pLabel->setText(sz);
		}
	}

	for (row=0; row < NUM_ROWS; ++row)
	{
//		CGrid *pGridRow = &m_PlayerGrids[row];
		m_PlayerGrids[row].AutoSetRowHeights();
		m_PlayerGrids[row].setSize(m_PlayerGrids[row].getWide(), m_PlayerGrids[row].CalcDrawHeight());
		m_PlayerGrids[row].RepositionContents();
	}

	// hack, for the thing to resize
	m_PlayerList.getSize(x, y);
	m_PlayerList.setSize(x, y);
}


//-----------------------------------------------------------------------------
// Purpose: Setup highlights for player names in scoreboard
//-----------------------------------------------------------------------------
void ScorePanel::DeathMsg(int killer, int victim)
{
	if (IsValidPlayerIndex(killer))
	{
		g_PlayerExtraInfo[killer].lastscoretime = gHUD.m_flTime;
		// if we were the one killed, or the world killed us, set the scoreboard to indicate suicide
		if (victim == m_iPlayerNum)
		{
			m_iLastKilledBy = killer;// ? killer : m_iPlayerNum;
			m_fLastKillDisplayStopTime = gHUD.m_flTime + SB_LAST_KILLER_HIGHLIGHT_TIME;	// display who we were killed by for 10 seconds
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Open
//-----------------------------------------------------------------------------
void ScorePanel::Open(void)
{
	RebuildTeams();
	setVisible(true);
	m_HitTestPanel.setVisible(true);
}

//-----------------------------------------------------------------------------
// Purpose: mousePressed
// Input  : code - MOUSE_LEFT
//			*panel - 
//-----------------------------------------------------------------------------
void ScorePanel::mousePressed(MouseCode code, Panel *panel)
{
	if (panel == NULL)
		return;

//WTF?!	if (gHUD.m_iIntermission)// XDM3035: need to show mouse so players can use scoreboard
//		return;

	if (!GetClientVoiceMgr()->IsInSquelchMode())
	{
		GetClientVoiceMgr()->StartSquelchMode();
		m_HitTestPanel.setVisible(false);
	}
	else if (m_iHighlightRow >= 0)
	{
		// mouse has been pressed, toggle mute state
		int iPlayer = m_iSortedRows[m_iHighlightRow];
		if (iPlayer > 0)
		{
			// print text message
			hud_player_info_t *pl_info = &g_PlayerInfoList[iPlayer];

			if (pl_info && pl_info->name && pl_info->name[0])
			{
				char string[128];
				string[0] = 0;
				if (GetClientVoiceMgr()->IsPlayerBlocked(iPlayer))
				{
					if (GetClientVoiceMgr()->SetPlayerBlockedState(iPlayer, false))// remove mute
						_snprintf(string, 128, BufferedLocaliseTextString("#Unmuted"), pl_info->name);
				}
				else
				{
					if (GetClientVoiceMgr()->SetPlayerBlockedState(iPlayer, true))// mute the player
						_snprintf(string, 128, BufferedLocaliseTextString("#Muted"), pl_info->name);
					
				}
				if (string[0] == 0)
					LocaliseTextString("#ERROR", string, 128);

				gHUD.m_SayText.SayTextPrint(string, 0, false);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Lower level of mouse movement handling
// Input  : x y - 
//			*panel - 
//-----------------------------------------------------------------------------
void ScorePanel::cursorMoved(int x, int y, Panel *panel)
{
	if (GetClientVoiceMgr()->IsInSquelchMode())
	{
		// look for which cell the mouse is currently over
		for (int i = 0; i < NUM_ROWS; ++i)
		{
			int row, col;
			if (m_PlayerGrids[i].getCellAtPoint(x, y, row, col))
			{
				MouseOverCell(i, col);
				return;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handles mouse movement over a cell
// Input  : row - 
//			col - 
//-----------------------------------------------------------------------------
void ScorePanel::MouseOverCell(const int &row, const int &col)
{
	CLabelHeader *label = &m_PlayerEntries[col][row];

	// clear the previously highlighted label
	if (m_pCurrentHighlightLabel != label)
	{
		m_pCurrentHighlightLabel = NULL;
		m_iHighlightRow = -1;
	}
	if (!label)
		return;

	// don't act on teams
	if (m_iRowType[row] != SBOARD_ROW_PLAYER)
		return;

	// don't act on disconnected players or ourselves
	hud_player_info_t *pl_info = &g_PlayerInfoList[m_iSortedRows[row]];

	if (!pl_info->name || !pl_info->name[0])
		return;

	if (pl_info->thisplayer && !gEngfuncs.IsSpectateOnly())
		return;

	// setup the new highlight
	m_pCurrentHighlightLabel = label;
	m_iHighlightRow = row;
}

//-----------------------------------------------------------------------------
// Purpose: dump debug information to console. Use to verify data.
//-----------------------------------------------------------------------------
void ScorePanel::DumpInfo(void)
{
//#ifdef _DEBUG// smaller, faster
	CON_PRINTF("-------- ScorePanel::DumpInfo() --------\n");

	int i = 0;
//	CON_PRINTF(" -- m_iSortedRows\tm_iRowType --\n");
//	for (i = 0; i < NUM_ROWS; i++)
//		CON_PRINTF(" %d  %d\t%d\n", i, m_iSortedRows[i], m_iRowType[i]);


	CON_PRINTF(" Players: %d\n id\tteam\tfrg\tdth\tname\n", gEngfuncs.GetMaxClients());
	for (i = 1; i <= gEngfuncs.GetMaxClients(); ++i)
	{
		CON_PRINTF(" #%d\t%d\t%d\t%d\t(%s)\tLST %g\n", i,
			g_PlayerExtraInfo[i].teamnumber,
			g_PlayerExtraInfo[i].frags,
			g_PlayerExtraInfo[i].deaths,
			g_PlayerInfoList[i].name,
			g_PlayerExtraInfo[i].lastscoretime);
	}
	int bestplayer = GetBestPlayer();
	if (bestplayer > 0)
		CON_PRINTF(" Best Player: %d (%s)\n", bestplayer, g_PlayerInfoList[bestplayer].name);
	else
		CON_PRINTF(" Best Player: none\n");

	if (IsTeamGame(gHUD.m_iGameType))
	{
		CON_PRINTF(" Teams: %d\n id\tfrg\tdth\tpng\tpls\tplr\t\tsc\t(nm)--\n", gViewPort->GetNumberOfTeams());
		for (i = 0; i <= gViewPort->GetNumberOfTeams(); ++i)// XDM: 0 <
		{
			CON_PRINTF(" #%d  %d %d %d %d %d %d (%s)\n", i,
			g_TeamInfo[i].frags,
			g_TeamInfo[i].deaths,
			g_TeamInfo[i].ping,
			g_TeamInfo[i].packetloss,
			g_TeamInfo[i].players,
			g_TeamInfo[i].scores_overriden,
			g_TeamInfo[i].name);
		}

		TEAM_ID bestteam = GetBestTeam();
		CON_PRINTF(" Best Team: %d (%s)\n", bestteam, g_TeamInfo[bestteam].name);
	}

	CON_PRINTF("-------- ScorePanel::DumpInfo end --------\n");
//#endif
}

//-----------------------------------------------------------------------------
// Purpose: A clone of CGameRulesMultiplay::GetBestPlayer
// Output : int player index
//-----------------------------------------------------------------------------
int ScorePanel::GetBestPlayer(void)
{
//	return m_SortedPlayers[0];
	return m_iBestPlayer;
#if 0
	// add team filter maybe?
	int bestplayer = 0;
	int score = 0;
	int bestscore = 0;
	float bestlastscoretime = gHUD.m_flTime + 1.0f;// we use this to determine the first player to achieve his score.

/* first to win mode
	if (gHUD.m_iGameType == GT_COOP && gHUD.m_iGameMode == COOP_MODE_LEVEL)// in this mode, first to touch the trigger is the winner
	{
		for (int i = 1; i <= gEngfuncs.GetMaxClients(); ++i)
		{
			if (IsActivePlayer(i))
			{
				score = g_PlayerExtraInfo[i].frags;
				if (g_PlayerExtraInfo[i].lastscoretime != 0.0f)
				{
					if (g_PlayerExtraInfo[i].lastscoretime < bestlastscoretime)
					{
						bestlastscoretime = g_PlayerExtraInfo[i].lastscoretime;
						bestplayer = i;
					}
					else if (score > bestscore)
					{
						bestscore = score;
//						bestlooses = g_PlayerExtraInfo[i].deaths;
						bestlastscoretime = g_PlayerExtraInfo[i].lastscoretime;
						bestplayer = i;
					}
				}
			}
		}
		return bestplayer;
	}*/

	int bestlooses = 65535;
	TEAM_ID bestteam = GetBestTeam();
	for (int i = 1; i <= gEngfuncs.GetMaxClients(); ++i)
	{
		if (IsActivePlayer(i))
		{
			if (IsSpectator(i))//if (pPlayer->IsObserver())// XDM3035: don't award spectators!
				continue;

			if (IsTeamGame(gHUD.m_iGameType))//if (team != TEAM_NONE && pPlayer->pev->team != team)
			{
				if (g_PlayerExtraInfo[i].teamnumber != bestteam)// player is not inthe best team, skip
					continue;
//				continue;// skip if particular team specified and it didn't match
			}

			score = g_PlayerExtraInfo[i].frags;
			if (score > bestscore)
			{
				bestscore = score;
				bestlooses = g_PlayerExtraInfo[i].deaths;
				bestlastscoretime = g_PlayerExtraInfo[i].lastscoretime;
				bestplayer = i;
			}
			else if (score == bestscore)
			{
				if (g_PlayerExtraInfo[i].deaths < bestlooses)
				{
//					bestscore = score;
					bestlooses = g_PlayerExtraInfo[i].deaths;
					bestlastscoretime = g_PlayerExtraInfo[i].lastscoretime;
					bestplayer = i;
				}
				else if (g_PlayerExtraInfo[i].deaths == bestlooses)// the first one to achieve this score shoud win!
				{
					if (g_PlayerExtraInfo[i].lastscoretime < bestlastscoretime)
					{
//						bestscore = score;
//						bestlooses = g_PlayerExtraInfo[i].deaths;
						bestlastscoretime = g_PlayerExtraInfo[i].lastscoretime;
						bestplayer = i;
					}
				}
			}
		}
	}
	return bestplayer;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: GetBestTeam
// Output : TEAM_ID
//-----------------------------------------------------------------------------
TEAM_ID ScorePanel::GetBestTeam(void)
{
	return m_SortedTeams[0];
}

//-----------------------------------------------------------------------------
// Purpose: Auto
// Input  : iRowData - 
//			iRowType - 
//-----------------------------------------------------------------------------
void ScorePanel::AddRow(int iRowData, int iRowType)
{
	m_iSortedRows[m_iRows] = iRowData;
	m_iRowType[m_iRows] = iRowType;
	++m_iRows;
}









//-----------------------------------------------------------------------------
// Purpose: Label paint functions - take into account current highlight status
//-----------------------------------------------------------------------------
void CLabelHeader::paintBackground(void)
{
	if (_row < 0)// XDM3035c
		return;

//	ScorePanel *pScorePanel = (ScorePanel *)(getParent()->getParent());// FAIL
	ScorePanel *pScorePanel = gViewPort->GetScoreBoard();
	if (pScorePanel)
	{
		vgui::Color oldBg;
		getBgColor(oldBg);// save old color in case cell gets un-highlighted

		if (pScorePanel->m_iRowType[_row] == SBOARD_ROW_PLAYER)
		{
			if (pScorePanel->m_iHighlightRow == _row)// XDM3035c: background drawing is disabled anyway
			{
				setBgColor(Scheme::sc_secondary2);// XDM: mouse over
			}
			else if (pScorePanel->m_iLastKilledBy > 0 && pScorePanel->m_iSortedRows[_row] == pScorePanel->m_iLastKilledBy)
			{
//				byte r,g,b;
//				UnpackRGB(r,g,b, RGB_RED);
				// FillGrid() sets color, here we just paint
				if (gHUD.m_iRevengeMode > 0)
				{
//					setBgColor(r,g,b,191);
					setBgColor(oldBg[0],oldBg[1],oldBg[2], 191);// BUGBUG: oldBg relies on pLabel->setBgColor() in FillGrid, which sometimes fails
				}
				else if (pScorePanel->m_fLastKillDisplayStopTime > gHUD.m_flTime)// fade out
				{
					float k = 1.0f - (pScorePanel->m_fLastKillDisplayStopTime - gHUD.m_flTime)/SB_LAST_KILLER_HIGHLIGHT_TIME;
//					setBgColor(r,g,b, max(0,(int)(k*255.0f)));
					setBgColor(oldBg[0],oldBg[1],oldBg[2], max(0,(int)(k*255.0f)));// redraw last killer's name background every time nescessary
				}
//				else
			}
		}
		Panel::paintBackground();
		setBgColor(oldBg);
	}
}
		
//-----------------------------------------------------------------------------
// Purpose: Label paint functions - take into account current highlight status
//-----------------------------------------------------------------------------
void CLabelHeader::paint(void)
{
	vgui::Color oldFg;
	getFgColor(oldFg);


//	ASSERT(((ScorePanel *)((CGrid *)getParent())->getParent()) == gViewPort->GetScoreBoard());

//	if (((ScorePanel *)((CGrid *)getParent())->getParent())->m_iHighlightRow == _row)// XDM: FAIL. I hate hacks!
	if (gViewPort->GetScoreBoard()->m_iHighlightRow == _row)
		setFgColor(255, 255, 255, 0);

	// draw text
	int x, y, iwide, itall;
	getTextSize(iwide, itall);
	calcAlignment(iwide, itall, x, y);
	_dualImage->setPos(x, y);

	int x1, y1;
	_dualImage->GetImage(1)->getPos(x1, y1);
	_dualImage->GetImage(1)->setPos(_gap, y1);
	_dualImage->doPaint(this);

	// get size of the panel and the image
	if (_image)
	{
		vgui::Color imgColor;
		getFgColor(imgColor);
		if (_useFgColorAsImageColor)
			_image->setColor(imgColor);

		_image->getSize(iwide, itall);
		calcAlignment(iwide, itall, x, y);
		_image->setPos(x, y);
		_image->doPaint(this);
	}

	setFgColor(oldFg[0], oldFg[1], oldFg[2], oldFg[3]);
}

//-----------------------------------------------------------------------------
// Purpose: calcAlignment
//-----------------------------------------------------------------------------
void CLabelHeader::calcAlignment(const int &iwide, const int &itall, int &x, int &y)
{
	// calculate alignment ourselves, since vgui is so broken
	int wide, tall;
	getSize(wide, tall);
	x = 0, y = 0;
	// align left/right
	switch (_contentAlignment)
	{
		// left
		case Label::a_northwest:
		case Label::a_west:
		case Label::a_southwest:
		{
			x = 0;
			break;
		}
		// center
		case Label::a_north:
		case Label::a_center:
		case Label::a_south:
		{
			x = (wide - iwide) / 2;
			break;
		}
		// right
		case Label::a_northeast:
		case Label::a_east:
		case Label::a_southeast:
		{
			x = wide - iwide;
			break;
		}
	}
	// top/down
	switch (_contentAlignment)
	{
		// top
		case Label::a_northwest:
		case Label::a_north:
		case Label::a_northeast:
		{
			y = 0;
			break;
		}
		// center
		case Label::a_west:
		case Label::a_center:
		case Label::a_east:
		{
			y = (tall - itall) / 2;
			break;
		}
		// south
		case Label::a_southwest:
		case Label::a_south:
		case Label::a_southeast:
		{
			y = tall - itall;
			break;
		}
	}
// don't clip to Y
//	if (y < 0)
//		y = 0;

	if (x < 0)
		x = 0;

	x += _offset[0];
	y += _offset[1];
}
