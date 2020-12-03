//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// vgui_SpectatorPanel.cpp: implementation of the SpectatorPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "hud.h"
#include "cl_util.h"
#include "const.h"
#include "entity_state.h"
#include "cl_entity.h"
#include "pm_shared.h"
#include "vgui_XDMViewport.h"
#include "vgui_SpectatorPanel.h"
#include "vgui_scorepanel.h"

#define PANEL_HEIGHT 32
#define BANNER_WIDTH	256
#define BANNER_HEIGHT	64
#define OPTIONS_BUTTON_X 96
#define CAMOPTIONS_BUTTON_X 200

const char *gSpectatorLabels[] =
{
"#OBS_NONE",
"#OBS_CHASE_LOCKED",
"#OBS_CHASE_FREE",
"#OBS_ROAMING",
"#OBS_IN_EYE",
"#OBS_MAP_FREE",
"#OBS_MAP_CHASE",
"#OBS_INTERMISSION"
};

const char *GetSpectatorLabel(int iMode)
{
	if (iMode >= OBS_NONE && iMode <= OBS_INTERMISSION)
		return gSpectatorLabels[iMode];

	return "";
}


int SpectateButton::IsNotValid(void)
{
	// Only visible if the server allows it
	if (gViewPort->GetAllowSpectators() != 0)
		return false;

	// XDM: we're already in spectator mode
	// XDM3035a: allow toggle?
//	if (gViewPort->m_pSpectatorPanel->isVisible())
//		return false;

	return true;
}


SpectatorPanel::SpectatorPanel(int x, int y, int wide, int tall) : CMenuPanel(false, x,y, wide,tall)//Panel(x,y,wide,tall)
{
}

SpectatorPanel::~SpectatorPanel()
{
}

void SpectatorPanel::Close(void)
{
	m_menuVisible = false;
	m_insetVisible = false;
	CMenuPanel::Close();
}

void SpectatorPanel::ActionSignal(int cmd)
{
	if (cmd == SPECTATOR_PANEL_CMD_NONE)
	{
	}
	else if (cmd == SPECTATOR_PANEL_CMD_OPTIONS)
	{
		gViewPort->ShowCommandMenu(gViewPort->m_SpectatorOptionsMenu);
	}
	else if (cmd == SPECTATOR_PANEL_CMD_NEXTPLAYER)
	{
		gHUD.m_Spectator.FindNextPlayer(false);
	}
	else if (cmd == SPECTATOR_PANEL_CMD_PREVPLAYER)
	{
		gHUD.m_Spectator.FindNextPlayer(true);
	}
	else if (cmd == SPECTATOR_PANEL_CMD_HIDEMENU)
	{
		ShowMenu(false);
	}
	else if (cmd == SPECTATOR_PANEL_CMD_CAMERA)
	{
		gViewPort->ShowCommandMenu(gViewPort->m_SpectatorCameraMenu);
	}
	else if (cmd == SPECTATOR_PANEL_CMD_TOGGLE_INSET)
	{
		gHUD.m_Spectator.SetModes(-1, gHUD.m_Spectator.ToggleInset(gHUD.m_Spectator.m_iInsetMode + 1, false));
	}
	else
		CON_DPRINTF("Unknown SpectatorPanel ActionSingal %i.\n",cmd);
}

void SpectatorPanel::Initialize(void)
{
	int x,y,wide,tall;
	getBounds(x,y,wide,tall);

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hSmallScheme = pSchemes->getSchemeHandle("Team Info Text");
/*	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle("Title Font");// XDM
	SchemeHandle_t hPrimaryButtonText = pSchemes->getSchemeHandle("Primary Button Text");// XDM

	int bg_r1, bg_g1, bg_b1, bg_a1;// XDM
	int bg_r2, bg_g2, bg_b2, bg_a2;
	int fg_r1, fg_g1, fg_b1, fg_a1;
	int fg_r2, fg_g2, fg_b2, fg_a2;
	pSchemes->getBgColor(hPrimaryButtonText,		bg_r1, bg_g1, bg_b1, bg_a1);
	pSchemes->getBgArmedColor(hPrimaryButtonText,	bg_r2, bg_g2, bg_b2, bg_a2);
	pSchemes->getFgColor(hPrimaryButtonText,		fg_r1, fg_g1, fg_b1, fg_a1);
	pSchemes->getFgArmedColor(hPrimaryButtonText,	fg_r2, fg_g2, fg_b2, fg_a2);
*/
//	m_TopBorder = new CTransparentPanel(BG_FILL_NORMAL, PANEL_DEFAULT_ALPHA, 0, 0, ScreenWidth, YRES(PANEL_HEIGHT));
	m_TopBorder = new Panel(0, 0, ScreenWidth, YRES(PANEL_HEIGHT));
	m_TopBorder->setParent(this);
	m_TopBorder->setBgColor(0,0,0,PANEL_DEFAULT_ALPHA);
//	m_BottomBorder = new CTransparentPanel(BG_FILL_NORMAL, PANEL_DEFAULT_ALPHA, 0, ScreenHeight - YRES(32), ScreenWidth, YRES(PANEL_HEIGHT));
	m_BottomBorder = new Panel(0, ScreenHeight - YRES(32), ScreenWidth, YRES(PANEL_HEIGHT));
	m_BottomBorder->setParent(this);
	m_BottomBorder->setBgColor(0,0,0,PANEL_DEFAULT_ALPHA);

	setCaptureInput(true);
	setShowCursor(false);// for now
	setPaintBackgroundEnabled(false);
//	m_TitleIcon.setVisible(false);

	m_ExtraInfo = new Label("Extra Info", 0, 0, wide, YRES(PANEL_HEIGHT));
	m_ExtraInfo->setParent(m_TopBorder);
	m_ExtraInfo->setFont(pSchemes->getFont(hSmallScheme));
	m_ExtraInfo->setPaintBackgroundEnabled(false);
//	m_ExtraInfo->setFgColor(fg_r1, fg_g1, fg_b1, fg_a1);// XDM
	m_ExtraInfo->setFgColor(Scheme::sc_primary1);
	m_ExtraInfo->setContentAlignment(vgui::Label::a_west);

	m_TimerImage = new CImageLabel("timer", 0, 0, 14, 14);
	m_TimerImage->setParent(m_TopBorder);

	m_TopBanner = new CImageLabel("banner", 0, 0, XRES(BANNER_WIDTH), YRES(BANNER_HEIGHT));
	m_TopBanner->setParent(this);

	m_CurrentTime = new Label("00:00", 0, 0, wide, YRES(PANEL_HEIGHT));
	m_CurrentTime->setParent(m_TopBorder);
	m_CurrentTime->setFont(pSchemes->getFont(hSmallScheme));
	m_CurrentTime->setPaintBackgroundEnabled(false);
//	m_CurrentTime->setFgColor(fg_r1, fg_g1, fg_b1, 0);// XDM
	m_CurrentTime->setFgColor(Scheme::sc_primary1);
	m_CurrentTime->setContentAlignment(vgui::Label::a_west);
/*
	m_Separator = new Panel(0, 0, XRES(6), YRES(96));
	m_Separator->setParent(m_TopBorder);
	m_Separator->setFgColor(fg_r1, fg_g1, fg_b1, fg_a1);
	m_Separator->setBgColor(bg_r1, bg_g1, bg_b1, bg_a1);// XDM
*/
//	int tr,tg,tb,ta;// XDM
//	pSchemes->getFgColor(hSmallScheme, tr, tg, tb, ta);
	for (int j=0; j < MAX_TEAMS; ++j)
	{
		m_TeamScores[j] = new Label("   ", 0, 0, wide, YRES(PANEL_HEIGHT));
		m_TeamScores[j]->setParent(m_TopBorder);
		m_TeamScores[j]->setFont(pSchemes->getFont(hSmallScheme));
		m_TeamScores[j]->setPaintBackgroundEnabled(false);
//		m_TeamScores[j]->setFgColor(tr, tg, tb, ta);// XDM: fail
		m_TeamScores[j]->setFgColor(Scheme::sc_primary1);
		m_TeamScores[j]->setContentAlignment(vgui::Label::a_west);
		m_TeamScores[j]->setVisible(false);
	}

	// XDM: title.
	m_TopMainLabel = new Label(BufferedLocaliseTextString("#SPECT_TOP"), XRES(0), YRES(0), wide, YRES(32));
	m_TopMainLabel->setParent(m_TopBorder);
	m_TopMainLabel->setFont(pSchemes->getFont(hSmallScheme));
	m_TopMainLabel->setPaintBackgroundEnabled(false);
	m_TopMainLabel->setFgColor(Scheme::sc_primary1);
	m_TopMainLabel->setContentAlignment(vgui::Label::a_center);

	// Initialize command buttons.
	m_OptionButton = new CommandButton(BufferedLocaliseTextString("#SPECT_OPTIONS"), XRES(15), YRES(6), XRES(OPTIONS_BUTTON_X), YRES(20), false, false, false);
	m_OptionButton->setParent(m_BottomBorder);
	m_OptionButton->setContentAlignment(vgui::Label::a_center);
	m_OptionButton->setBoundKey((char)255);	// special no bound to avoid leading spaces in name 
	m_OptionButton->addActionSignal(new CSpectatorHandler_Command(this, SPECTATOR_PANEL_CMD_OPTIONS));
/*	m_OptionButton->setUnArmedBorderColor(bg_r1, bg_g1, bg_b1, bg_a1);
	m_OptionButton->setArmedBorderColor(bg_r2, bg_g2, bg_b2, bg_a2);
	m_OptionButton->setUnArmedColor(fg_r1, fg_g1, fg_b1, fg_a1);
	m_OptionButton->setArmedColor(fg_r2, fg_g2, fg_b2, fg_a2);
*/
	m_CamButton = new CommandButton(BufferedLocaliseTextString("#CAM_OPTIONS"),  ScreenWidth - (XRES(CAMOPTIONS_BUTTON_X) + 15), YRES(6), XRES(CAMOPTIONS_BUTTON_X), YRES(20), false, false, false);
	m_CamButton->setParent(m_BottomBorder);
	m_CamButton->setContentAlignment(vgui::Label::a_center);
	m_CamButton->setBoundKey((char)255);	// special no bound to avoid leading spaces in name 
	m_CamButton->addActionSignal(new CSpectatorHandler_Command(this, SPECTATOR_PANEL_CMD_CAMERA));
/*	m_CamButton->setUnArmedBorderColor(bg_r1, bg_g1, bg_b1, bg_a1);
	m_CamButton->setArmedBorderColor(bg_r2, bg_g2, bg_b2, bg_a2);
	m_CamButton->setUnArmedColor(fg_r1, fg_g1, fg_b1, fg_a1);
	m_CamButton->setArmedColor(fg_r2, fg_g2, fg_b2, fg_a2);
*/
	m_PrevPlayerButton= new CommandButton("<", XRES(15 + OPTIONS_BUTTON_X + 15), YRES(6), XRES(24), YRES(20), false, false, false);
	m_PrevPlayerButton->setParent(m_BottomBorder);
	m_PrevPlayerButton->setContentAlignment(vgui::Label::a_center);
	m_PrevPlayerButton->setBoundKey((char)255);	// special no bound to avoid leading spaces in name 
	m_PrevPlayerButton->addActionSignal(new CSpectatorHandler_Command(this, SPECTATOR_PANEL_CMD_PREVPLAYER));
/*	m_PrevPlayerButton->setUnArmedBorderColor(bg_r1, bg_g1, bg_b1, bg_a1);
	m_PrevPlayerButton->setArmedBorderColor(bg_r2, bg_g2, bg_b2, bg_a2);
	m_PrevPlayerButton->setUnArmedColor(fg_r1, fg_g1, fg_b1, fg_a1);
	m_PrevPlayerButton->setArmedColor(fg_r2, fg_g2, fg_b2, fg_a2);
*/
	m_NextPlayerButton= new CommandButton(">", (ScreenWidth - (XRES(CAMOPTIONS_BUTTON_X) + 15)) - XRES(24 + 15), YRES(6), XRES(24), YRES(20), false, false, false);
	m_NextPlayerButton->setParent(m_BottomBorder);
	m_NextPlayerButton->setContentAlignment(vgui::Label::a_center);
	m_NextPlayerButton->setBoundKey((char)255);	// special no bound to avoid leading spaces in name 
	m_NextPlayerButton->addActionSignal(new CSpectatorHandler_Command(this, SPECTATOR_PANEL_CMD_NEXTPLAYER));
/*	m_NextPlayerButton->setUnArmedBorderColor(bg_r1, bg_g1, bg_b1, bg_a1);
	m_NextPlayerButton->setArmedBorderColor(bg_r2, bg_g2, bg_b2, bg_a2);
	m_NextPlayerButton->setUnArmedColor(fg_r1, fg_g1, fg_b1, fg_a1);
	m_NextPlayerButton->setArmedColor(fg_r2, fg_g2, fg_b2, fg_a2);
	*/
	// Initialize the bottom title.
	int iLabelSize = ((ScreenWidth - (XRES(CAMOPTIONS_BUTTON_X) + 15)) - XRES(24 + 15)) - XRES((15 + OPTIONS_BUTTON_X + 15) + 38);
	m_BottomMainLabel = new Label(BufferedLocaliseTextString("#SPECT_BOTTOM"), XRES((15 + OPTIONS_BUTTON_X + 15) + 31), YRES(6), iLabelSize, YRES(20));
	m_BottomMainLabel->setParent(m_BottomBorder);
	m_BottomMainLabel->setPaintBackgroundEnabled(false);
	m_BottomMainLabel->setFgColor(Scheme::sc_primary1);
	m_BottomMainLabel->setContentAlignment(vgui::Label::a_center);
	m_BottomMainLabel->setBorder(new CDefaultLineBorder());
//	m_BottomMainLabel->setBorder(new LineBorder(vgui::Color(bg_r1, bg_g1, bg_b1, 48)));// XDM

	// make the inset view window clickable
	m_InsetViewButton = new CommandButton("", XRES(2), YRES(2), XRES(240), YRES(180), false, false, false);
	m_InsetViewButton->setParent(this);
	m_InsetViewButton->setBoundKey((char)255);
	m_InsetViewButton->addActionSignal(new CSpectatorHandler_Command(this,SPECTATOR_PANEL_CMD_TOGGLE_INSET));
/*	m_InsetViewButton->setUnArmedBorderColor(bg_r1, bg_g1, bg_b1, bg_a1);
	m_InsetViewButton->setArmedBorderColor(bg_r2, bg_g2, bg_b2, bg_a2);
	m_InsetViewButton->setUnArmedColor(fg_r1, fg_g1, fg_b1, fg_a1);
	m_InsetViewButton->setArmedColor(fg_r2, fg_g2, fg_b2, fg_a2);
*/
	m_menuVisible = false;
	m_insetVisible = false;
	m_CamButton->setVisible(false);
	m_OptionButton->setVisible(false);
	m_NextPlayerButton->setVisible(false);
	m_PrevPlayerButton->setVisible(false);
	m_TopBanner->setVisible(false);
	m_ExtraInfo->setVisible(false);
//	m_Separator->setVisible(false);
	m_TimerImage->setVisible(false);
	
	m_fHideHelpTime = gHUD.m_flTime + SPECTATOR_PANEL_HELP_DISPLAY_TIME;
}

void SpectatorPanel::ShowMenu(bool bVisible)
{
	m_OptionButton->setVisible(bVisible);		m_OptionButton->setArmed(false);
	m_CamButton->setVisible(bVisible);			m_CamButton->setArmed(false);
	m_NextPlayerButton->setVisible(bVisible);	m_NextPlayerButton->setArmed(false);
	m_PrevPlayerButton->setVisible(bVisible);	m_PrevPlayerButton->setArmed(false);
	m_BottomMainLabel->setPaintBorderEnabled(bVisible);
	m_TopMainLabel->setVisible(bVisible);

	if (bVisible)
	{
		m_BottomMainLabel->setPos(XRES((15 + OPTIONS_BUTTON_X + 15) + 31), YRES(6));
		m_fHideHelpTime = gHUD.m_flTime;
	}
	else
	{
		int iLabelSizeX, iLabelSizeY;
		m_BottomMainLabel->getSize(iLabelSizeX, iLabelSizeY);
		m_BottomMainLabel->setPos((ScreenWidth / 2) - (iLabelSizeX/2), YRES(6));
		gViewPort->HideCommandMenu();
		if (m_menuVisible && this->isVisible())// if switching from visible menu to invisible menu, show help text
		{
			m_BottomMainLabel->setText(BufferedLocaliseTextString("#Spec_Duck"));// XDM
			m_fHideHelpTime = gHUD.m_flTime + SPECTATOR_PANEL_HELP_DISPLAY_TIME;
		}
	}
	m_menuVisible = bVisible;
	setShowCursor(bVisible);
	gViewPort->UpdateCursorState();
}

void SpectatorPanel::EnableInsetView(bool isEnabled)
{
	int x = gHUD.m_Spectator.m_OverviewData.insetWindowX;
	int y = gHUD.m_Spectator.m_OverviewData.insetWindowY;
	int wide = gHUD.m_Spectator.m_OverviewData.insetWindowWidth;
	int tall = gHUD.m_Spectator.m_OverviewData.insetWindowHeight;
	int offset = x + wide + 2;

	if (isEnabled)
	{
		// short black bar to see full inset
		m_TopBorder->setBounds(XRES(offset), 0, XRES(640 - offset), YRES(PANEL_HEIGHT));

		if (gEngfuncs.IsSpectateOnly())
		{
			m_TopBanner->setVisible(true);
			m_TopBanner->setPos(XRES(offset), 0);
		}
		else
			m_TopBanner->setVisible(false);
		
		m_TopMainLabel->setBounds(0, 0, XRES(640 - (x+wide+2)), YRES(PANEL_HEIGHT));// XDM
		m_InsetViewButton->setBounds(XRES(x), YRES(y), XRES(wide), YRES(tall));
		m_InsetViewButton->setVisible(true);
	}
	else
	{	
		// full black bar, no inset border
		// show banner only in real HLTV mode
		if (gEngfuncs.IsSpectateOnly())
		{
			m_TopBanner->setVisible(true);
			m_TopBanner->setPos(0,0);
		}
		else
			m_TopBanner->setVisible(false);

		m_TopBorder->setBounds(0, 0, ScreenWidth, YRES(PANEL_HEIGHT));
		m_TopMainLabel->setBounds(0, 0, ScreenWidth, YRES(PANEL_HEIGHT));// XDM
		m_InsetViewButton->setVisible(false);
	}
	m_insetVisible = isEnabled;
	Update();
	m_CamButton->setText(BufferedLocaliseTextString(GetSpectatorLabel(g_iUser1)));
}

//-----------------------------------------------------------------------------
// Purpose: Called very often, optimize as much as possible!
//-----------------------------------------------------------------------------
void SpectatorPanel::Update(void)
{
	int iTextWidth, iTextHeight;
	int iTimeHeight, iTimeWidth;
	int offset,j;

	if (m_insetVisible)
		offset = gHUD.m_Spectator.m_OverviewData.insetWindowX + gHUD.m_Spectator.m_OverviewData.insetWindowWidth + 2;
	else
		offset = 0;

	bool visible = (gHUD.m_Spectator.m_drawstatus->value > 0);

	m_ExtraInfo->setVisible(visible);
	m_TimerImage->setVisible(visible);
	m_CurrentTime->setVisible(visible);
//	m_Separator->setVisible(visible);

	for (j=0; j < MAX_TEAMS; ++j)
		m_TeamScores[j]->setVisible(visible);

	if (!visible)
		return;
		
	m_ExtraInfo->getTextSize(iTextWidth, iTextHeight);
	m_CurrentTime->getTextSize(iTimeWidth, iTimeHeight);

	iTimeWidth += XRES(14); // +timer icon
	iTimeWidth += (4-(iTimeWidth%4));

	if (iTimeWidth > iTextWidth)
		iTextWidth = iTimeWidth;

	int xPos = ScreenWidth - (iTextWidth + XRES(4 + offset));
	m_ExtraInfo->setBounds(xPos, YRES(1), iTextWidth, iTextHeight);
	m_TimerImage->setBounds(xPos, YRES(2) + iTextHeight , XRES(14), YRES(14));
	m_CurrentTime->setBounds(xPos + XRES (14 + 1), YRES(2) + iTextHeight , iTimeWidth, iTimeHeight);
//	m_Separator->setPos(ScreenWidth - (iTextWidth + XRES(4+2+4+offset)) , YRES(1));
//	m_Separator->setSize(XRES(4), YRES(PANEL_HEIGHT - 2));

	char str[32];
	int iwidth, iheight;
	int r,g,b;// XDM
	for (j=0; j < MAX_TEAMS; ++j)
	{
		GetTeamColor(j+1, r,g,b);
		m_TeamScores[j]->setFgColor(r,g,b,0);// alpha is reversed
		sprintf(str, "%d | %d\0", g_TeamInfo[j+1].frags, g_TeamInfo[j+1].scores_overriden);
		m_TeamScores[j]->setText(str);
		m_TeamScores[j]->getTextSize(iwidth, iheight);
		m_TeamScores[j]->setBounds(ScreenWidth - (iTextWidth + XRES(4+2+4+2+offset) + iwidth), YRES(1) + (iheight * j), iwidth, iheight);
	}
	// XDM
	// update extra info field
	char szText[64];

	if (gEngfuncs.IsSpectateOnly())// in HLTV mode show number of spectators
		_snprintf(szText, 63, "%s: %d", BufferedLocaliseTextString("#Spectators"), gHUD.m_Spectator.m_iSpectatorNumber);
	else
		_snprintf(szText, 63, "%s: %s", BufferedLocaliseTextString("#Spec_Map"), gViewPort->GetMapName());

	szText[63] = 0;
	m_ExtraInfo->setText(szText);

	if (m_fHideHelpTime <= gHUD.m_flTime/* && m_fHideHelpTime != 0.0f*/)// if not, keep showing help text
	{
		// MOVED FROM UpdateSpectatorPanel()
		char *name = NULL;
		int player = 0;
		char bottomText[128];

		// check if we're locked onto a target, show the player's name
		if ((g_iUser1 != OBS_ROAMING) && IsActivePlayer(g_iUser2))
			player = g_iUser2;

		// special case in free map and inset off, don't show names
		if ((g_iUser1 == OBS_MAP_FREE) && (gHUD.m_Spectator.m_iInsetMode == INSET_OFF))
			name = NULL;
		else if (player > 0)
		{
			GetPlayerInfo(player, &g_PlayerInfoList[player]);
			name = g_PlayerInfoList[player].name;
		}

		// fill in bottomText: player or mode
		if (player > 0 && g_iUser1 != OBS_ROAMING && g_iUser1 != OBS_MAP_FREE)
		{
			int r,g,b;// XDM
			GetTeamColor(g_PlayerExtraInfo[player].teamnumber, r,g,b);
			m_BottomMainLabel->setFgColor(r,g,b,0);// alpha is reversed

			if (gHUD.m_Spectator.m_autoDirector->value)
				sprintf(bottomText,"#Spec_Auto %s", name);
			else if (name)
				strcpy(bottomText, name);//sprintf(bottomText,"%s", name);
			else
				strcpy(bottomText, "ERROR!");
		}
		else// restore GUI color
		{
			m_BottomMainLabel->setFgColor(Scheme::sc_primary1);
			sprintf(bottomText,"#Spec_Mode%d", g_iUser1);
		}
		if (gEngfuncs.IsSpectateOnly())
			strcat(bottomText, " - HLTV");

		m_BottomMainLabel->setText(BufferedLocaliseTextString(bottomText));
//		m_fHideHelpTime = 0.0f;// XDM3035c: don't do this every frame
	}

	if (gHUD.m_flTimeLeft > 0.0f)// XDM3035
	{
		char szTimeLeftText[16];
		_snprintf(szTimeLeftText, 16, "%d:%02d\n", (int)(gHUD.m_flTimeLeft/60), ((int)gHUD.m_flTimeLeft % 60));
		szTimeLeftText[15] = 0;
		m_CurrentTime->setText(szTimeLeftText);
	}
}

void SpectatorPanel::SetBanner(char *image)
{
	if (m_TopBanner)
		m_TopBanner->LoadImage(image);
}
