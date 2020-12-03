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
#include "pm_shared.h"
#include "../engine/keydefs.h"
#include "demo.h"
#include "demo_api.h"
#include "../game_shared/voice_status.h"
#include <VGUI_App.h>
#include <VGUI_Scheme.h>
#include <VGUI_Font.h>
#include <VGUI_BuildGroup.h>
#include <VGUI_Cursor.h>
#include <VGUI_Frame.h>
#include <VGUI_Label.h>
#include <VGUI_Surface.h>
#include <VGUI_BorderLayout.h>
#include <VGUI_Panel.h>
#include <VGUI_ImagePanel.h>
#include <VGUI_Button.h>
#include <VGUI_ActionSignal.h>
#include <VGUI_InputSignal.h>
#include <VGUI_MenuSeparator.h>
#include <VGUI_TextPanel.h>
#include <VGUI_LoweredBorder.h>
#include <VGUI_LineBorder.h>
#include "vgui_int.h"
#include "vgui_XDMViewport.h"
#include "vgui_ServerBrowser.h"
#include "vgui_ScorePanel.h"
#include "vgui_SpectatorPanel.h"
#include "vgui_TeamMenu.h"// XDM
#include "vgui_MOTDWindow.h"
#include "vgui_MusicPlayer.h"
//#include "vgui_ConsolePanel.h"
//#include "vgui_ControlConfigPanel.h"
#include "voice_vgui_tweakdlg.h"


CViewport *gViewPort = NULL;

// WARNING! we count from 1 because entindex 0 is the world, so MAX_PLAYERS (32) is a valid index!
hud_player_info_t	g_PlayerInfoList[MAX_PLAYERS+1];// player info from the engine
extra_player_info_t g_PlayerExtraInfo[MAX_PLAYERS+1];// additional player info sent directly to the client dll
//byte				g_IsSpectator[MAX_PLAYERS+1];// separate array because it is filled externally at unpredictable time
team_info_t			g_TeamInfo[MAX_TEAMS+1];

#define MAX_GRINTRO_LENGTH 64

static const int MAX_TITLE_LENGTH = 64;
static const int MAX_TEXT_LENGTH = MAX_MOTD_LENGTH + MAX_GRINTRO_LENGTH + MAX_TITLE_LENGTH + 2048;
static const int SZ_LEN = 256;


using namespace vgui;


//-----------------------------------------------------------------------------
// Purpose: Makes sure the memory allocated for Viewport is nulled out
// Input  : stAllocateBlock - 
// Output : void *
//-----------------------------------------------------------------------------
void *CViewport::operator new(size_t stAllocateBlock)
{
//	void *mem = Panel::operator new(stAllocateBlock);
	void *mem = ::operator new(stAllocateBlock);
	memset(mem, 0, stAllocateBlock);
	return mem;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
// Input  : x y - 
//			wide tall - 
//-----------------------------------------------------------------------------
CViewport::CViewport(int x,int y,int wide,int tall) : Panel(x,y,wide,tall), m_SchemeManager(wide,tall)
{
	gViewPort = this;
	m_iInitialized = false;
	m_pTeamMenu = NULL;
	m_pScoreBoard = NULL;
	m_pSpectatorPanel = NULL;
	m_pCurrentMenu = NULL;
	m_pCurrentCommandMenu = NULL;
//	m_pInventoryPanel = NULL;// XDM
	m_pMusicPlayer = NULL;

	Initialize();
	addInputSignal(new CViewPortInputHandler);

	// VGUI MENUS
	CreateTeamMenu();
	CreateSpectatorMenu();
	CreateScoreBoard();
//	CreateInventoryPanel();// XDM
	CreateMusicPlayerPanel();

	// Init command menus
	m_iNumMenus = 0;
	m_iCurrentTeamNumber = m_iUser1 = m_iUser2 = m_iUser3 = 0;

	m_StandardMenu = CreateCommandMenu("scripts/commandmenu.txt", CMENU_DIR_DOWN, CMENU_TOP, false, CMENU_SIZE_X, CBUTTON_SIZE_Y, 0);
	m_SpectatorOptionsMenu = CreateCommandMenu("scripts/spectatormenu.txt", CMENU_DIR_UP, YRES(32), true, CMENU_SIZE_X, BUTTON_SIZE_Y / 2, 0 );// above bottom bar, flat design
	m_SpectatorCameraMenu = CreateCommandMenu("scripts/spectcammenu.txt", CMENU_DIR_UP, YRES(32), true, XRES(200), BUTTON_SIZE_Y / 2, ScreenWidth - (XRES(200) + 15));// above bottom bar, flat design
	CreateServerBrowser();
}

//-----------------------------------------------------------------------------
// Purpose: Called everytime a new level is started. Viewport clears out it's data.
//-----------------------------------------------------------------------------
void CViewport::Initialize(void)
{
//	memset(g_PlayerExtraInfo, 0, sizeof(g_PlayerExtraInfo));
//	memset(g_TeamInfo, 0, sizeof(g_TeamInfo));
	memset(g_PlayerExtraInfo, 0, sizeof(extra_player_info_t)*(MAX_PLAYERS+1));
	memset(g_TeamInfo, 0, sizeof(team_info_t)*(MAX_TEAMS+1));

	// reset player info
	gHUD.m_iTeamNumber = TEAM_NONE;

	// Force each menu to Initialize
	if (m_pTeamMenu)
		m_pTeamMenu->Initialize();

	if (m_pScoreBoard)
	{
		m_pScoreBoard->Initialize();
		HideScoreBoard();
	}

	if (m_pSpectatorPanel)
		m_pSpectatorPanel->setVisible(false);// Spectator menu doesn't need initializing

//	if (m_pInventoryPanel)// XDM
//		m_pInventoryPanel->Initialize();

	if (m_pMusicPlayer)// XDM
		m_pMusicPlayer->Initialize();

	// Make sure all menus are hidden
	HideVGUIMenu();
	HideCommandMenu();

	// Clear out some data
	m_iGotAllMOTD = true;
//	m_iRandomPC = false;
	m_flScoreBoardLastUpdated = 0;
	m_flSpectatorPanelLastUpdated = 0;

	strcpy(m_sMapName, "");
	strcpy(m_szServerName, "");
//	strncpy(g_TeamInfo[0].name, "Unassigned", MAX_TEAM_NAME);// XDM
	LocaliseTextString("#Spectators", g_TeamInfo[0].name, MAX_TEAM_NAME);// XDM3035a

	App::getInstance()->setCursorOveride(App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_none));

	LoadScheme();
}

//-----------------------------------------------------------------------------
// Purpose: LoadScheme
//-----------------------------------------------------------------------------
void CViewport::LoadScheme(void)
{
	Scheme *pScheme = App::getInstance()->getScheme();
	// Upload colors from local scheme into vgui scheme
	//!! two different types of scheme here, need to integrate
	SchemeHandle_t hPrimaryScheme = m_SchemeManager.getSchemeHandle("Primary Button Text");
	CScheme *pLocalScheme = m_SchemeManager.getSafeScheme(hPrimaryScheme);
	if (pLocalScheme)
	{
		m_iBorderThickness = pLocalScheme->BorderThickness;
		// font
		pScheme->setFont(Scheme::sf_primary1, m_SchemeManager.getFont(hPrimaryScheme));
		// 3d border colors
		pScheme->setColor(Scheme::sc_black, pLocalScheme->DarkColor[0], pLocalScheme->DarkColor[1], pLocalScheme->DarkColor[2], pLocalScheme->DarkColor[3]);
		pScheme->setColor(Scheme::sc_white, pLocalScheme->BrightColor[0], pLocalScheme->BrightColor[1], pLocalScheme->BrightColor[2], pLocalScheme->BrightColor[3]);
		// text color
		pScheme->setColor(Scheme::sc_primary1, pLocalScheme->FgColor[0], pLocalScheme->FgColor[1], pLocalScheme->FgColor[2], pLocalScheme->FgColor[3]);
		pScheme->setColor(Scheme::sc_secondary1, pLocalScheme->FgColorArmed[0], pLocalScheme->FgColorArmed[1], pLocalScheme->FgColorArmed[2], pLocalScheme->FgColorArmed[3]);
		// background color
		pScheme->setColor(Scheme::sc_primary2, pLocalScheme->BgColor[0], pLocalScheme->BgColor[1], pLocalScheme->BgColor[2], pLocalScheme->BgColor[3]);
		pScheme->setColor(Scheme::sc_secondary2, pLocalScheme->BgColorArmed[0], pLocalScheme->BgColorArmed[1], pLocalScheme->BgColorArmed[2], pLocalScheme->BgColorArmed[3]);
		// borders around buttons
		pScheme->setColor(Scheme::sc_primary3, pLocalScheme->BorderColor[0], pLocalScheme->BorderColor[1], pLocalScheme->BorderColor[2], pLocalScheme->BorderColor[3]);
		pScheme->setColor(Scheme::sc_secondary3, pLocalScheme->BorderColorArmed[0], pLocalScheme->BorderColorArmed[1], pLocalScheme->BorderColorArmed[2], pLocalScheme->BorderColorArmed[3]);
		// mouse down foreground
		pScheme->setColor(Scheme::sc_user, pLocalScheme->FgColorClicked[0], pLocalScheme->FgColorClicked[1], pLocalScheme->FgColorClicked[2], pLocalScheme->FgColorClicked[3]);
/* old
		// 3d border colors
		pScheme->setColor(Scheme::sc_black, pLocalScheme->darkColor[0], pLocalScheme->darkColor[1], pLocalScheme->darkColor[2], pLocalScheme->darkColor[3]);
		pScheme->setColor(Scheme::sc_white, pLocalScheme->brightColor[0], pLocalScheme->brightColor[1], pLocalScheme->brightColor[2], pLocalScheme->brightColor[3]);
		// text color
		pScheme->setColor(Scheme::sc_primary1, pLocalScheme->fgColor[0], pLocalScheme->fgColor[1], pLocalScheme->fgColor[2], pLocalScheme->fgColor[3]);
		// armed background color
		pScheme->setColor(Scheme::sc_primary2, pLocalScheme->BgColorArmed[0], pLocalScheme->BgColorArmed[1], pLocalScheme->BgColorArmed[2], pLocalScheme->BgColorArmed[3]);
		// background color (transparent black)
		pScheme->setColor(Scheme::sc_primary3, pLocalScheme->bgColor[0], pLocalScheme->bgColor[1], pLocalScheme->bgColor[2], pLocalScheme->bgColor[3]);
		// borders around buttons
		pScheme->setColor(Scheme::sc_secondary1, pLocalScheme->borderColor[0], pLocalScheme->borderColor[1], pLocalScheme->borderColor[2], pLocalScheme->borderColor[3]);
		// armed foreground color
		pScheme->setColor(Scheme::sc_secondary2, pLocalScheme->FgColorArmed[0], pLocalScheme->FgColorArmed[1], pLocalScheme->FgColorArmed[2], pLocalScheme->FgColorArmed[3]);
		// armed foreground color
		pScheme->setColor(Scheme::sc_secondary3, pLocalScheme->FgColorClicked[0], pLocalScheme->FgColorClicked[1], pLocalScheme->FgColorClicked[2], pLocalScheme->FgColorClicked[3]);
*/
	}

	// Change the second primary font (used in the scoreboard)
	SchemeHandle_t hScoreboardScheme = m_SchemeManager.getSchemeHandle("Scoreboard Text");
	pScheme->setFont(Scheme::sf_primary2, m_SchemeManager.getFont(hScoreboardScheme));

	// Change the third primary font (used in command menu)
	SchemeHandle_t hCommandMenuScheme = m_SchemeManager.getSchemeHandle("CommandMenu Text");
	pScheme->setFont(Scheme::sf_primary3, m_SchemeManager.getFont(hCommandMenuScheme));

	App::getInstance()->setScheme(pScheme);
}

//-----------------------------------------------------------------------------
// Purpose: Read the Command Menu structure from the txt file and create the menu.
//			Returns Index of menu in m_pCommandMenus
//-----------------------------------------------------------------------------
int CViewport::CreateCommandMenu(char *menuFile, int direction, int yOffset, bool flatDesign, int flButtonSizeX, int flButtonSizeY, int xOffset)
{
	// Create the root of this new Command Menu
	int newIndex = m_iNumMenus;
	m_pCommandMenus[newIndex] = new CCommandMenu(NULL, direction, xOffset, yOffset, flButtonSizeX, 300);	// This will be resized once we know how many items are in it
	m_pCommandMenus[newIndex]->setParent(this);
	m_pCommandMenus[newIndex]->setVisible(false);
	m_pCommandMenus[newIndex]->m_iButtonSizeY = (int)flButtonSizeY;
//	m_pCommandMenus[newIndex]->m_iSpectCmdMenu = direction;

	m_iNumMenus++;

	// Read Command Menu from the txt file
	char token[1024];
	char *pfile = (char*)gEngfuncs.COM_LoadFile(menuFile, 5, NULL);
	if (!pfile)
	{
		gEngfuncs.Con_DPrintf("Unable to open %s\n", menuFile);
		SetCurrentCommandMenu(NULL);
		return newIndex;
	}
	char *pFileStart = pfile;// XDM3035c

try
{
	// First, read in the localisation strings
	// Now start parsing the menu structure
	m_pCurrentCommandMenu = m_pCommandMenus[newIndex];
	char szLastButtonText[32] = "file start";
	pfile = gEngfuncs.COM_ParseFile(pfile, token);
	while ((strlen(token) > 0) && (m_iNumMenus < MAX_MENUS))
	{
		// Keep looping until we hit the end of this menu
		while (token[0] != '}' && (strlen(token) > 0))
		{
			char cText[32] = "";
			char cBoundKey[32] = "";
			char cCustom[32] = "";
			static const int cCommandLength = 128;
			char cCommand[cCommandLength] = "";
			char szMap[MAX_MAPNAME] = "";
			int  iCustom = false;
			int  iTeamOnly = -1;
			int  iToggle = 0;
			int  iButtonY;
			bool bGetExtraToken = true;
			CommandButton *pButton = NULL;
			
			// We should never be here without a Command Menu
			if (!m_pCurrentCommandMenu)
			{
				gEngfuncs.Con_Printf("Error in %s file after '%s'.\n",menuFile, szLastButtonText );
				m_iInitialized = false;
				return newIndex;
			}

			// token should already be the bound key, or the custom name
			strncpy(cCustom, token, 32);
			cCustom[31] = '\0';

			// See if it's a custom button
			if (!strcmp(cCustom, "CUSTOM"))
			{
				iCustom = true;
				// Get the next token
				pfile = gEngfuncs.COM_ParseFile(pfile, token);
			}
			// See if it's a map
			else if (!strcmp(cCustom, "MAP"))
			{
				// Get the mapname
				pfile = gEngfuncs.COM_ParseFile(pfile, token);
				strncpy(szMap, token, MAX_MAPNAME);
				szMap[MAX_MAPNAME-1] = '\0';
				// Get the next token
				pfile = gEngfuncs.COM_ParseFile(pfile, token);
			}
			else if (!strncmp(cCustom, "TEAM", 4)) // TEAM1, TEAM2, TEAM3, TEAM4
			{
				// make it a team only button
				iTeamOnly = atoi( cCustom + 4 );
				// Get the next token
				pfile = gEngfuncs.COM_ParseFile(pfile, token);
			}
			else if (!strncmp(cCustom, "TOGGLE", 6)) 
			{
				iToggle = true;
				// Get the next token
				pfile = gEngfuncs.COM_ParseFile(pfile, token);
			}

			// Get the button bound key
			strncpy(cBoundKey, token, 32);
			cText[31] = '\0';

			// Get the button text
			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			strncpy(cText, token, 32);
			cText[31] = '\0';

			// save off the last button text we've come across (for error reporting)
			strcpy( szLastButtonText, cText );

			// Get the button command
			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			strncpy(cCommand, token, cCommandLength);
			cCommand[cCommandLength - 1] = '\0';

			iButtonY = (BUTTON_SIZE_Y-1) * m_pCurrentCommandMenu->GetNumButtons();
			
			// Custom button handling
/*			if ( iCustom )
			{
				pButton = CreateCustomButton( cText, cCommand, iButtonY );
				// Get the next token to see if we're a menu
				pfile = gEngfuncs.COM_ParseFile(pfile, token);
				if ( token[0] == '{' )
					strcpy( cCommand, token );
				else
					bGetExtraToken = false;
			}
			else */if (szMap[0] != '\0')// create a map button
			{
				pButton = new MapButton(szMap, cText, xOffset, iButtonY, flButtonSizeX, flButtonSizeY);
			}
			else if (iTeamOnly != -1)// button that only shows up if the player is on team iTeamOnly
			{
				pButton = new TeamOnlyCommandButton(iTeamOnly, cText, xOffset, iButtonY, flButtonSizeX, flButtonSizeY, flatDesign, true);
			}
			else if (iToggle && direction == 0)
			{
				pButton = new ToggleCommandButton(cCommand, cText, xOffset, iButtonY, flButtonSizeX, flButtonSizeY, flatDesign, true);
			}
			else if (direction == 1)
			{
				if (iToggle)
					pButton = new SpectToggleButton(cCommand, cText, xOffset, iButtonY, flButtonSizeX, flButtonSizeY, flatDesign);
				else
					pButton = new SpectButton(cText, xOffset, iButtonY, flButtonSizeX, flButtonSizeY);
			}
			else// normal button
			{
				pButton = new CommandButton(cText, xOffset, iButtonY, flButtonSizeX, flButtonSizeY, false, flatDesign, true);
			}

			// add the button into the command menu
			if (pButton)
			{
				m_pCurrentCommandMenu->AddButton(pButton);
				pButton->setBoundKey(cBoundKey[0]);
				pButton->setParentMenu(m_pCurrentCommandMenu);
				// Override font in CommandMenu
// XDM3035a: ?				pButton->setFont(Scheme::sf_primary3);
			}

			// Find out if it's a submenu or a button we're dealing with
			if (cCommand[0] == '{')
			{
				if (m_iNumMenus >= MAX_MENUS)
				{
					gEngfuncs.Con_Printf("Too many menus in %s past '%s'\n",menuFile, szLastButtonText);
				}
				else
				{
					// Create the menu
					m_pCommandMenus[m_iNumMenus] = CreateSubMenu(pButton, m_pCurrentCommandMenu, iButtonY);
					m_pCurrentCommandMenu = m_pCommandMenus[m_iNumMenus];
					m_iNumMenus++;
				}
			}
			else if (!iCustom)
			{
				// Create the button and attach it to the current menu
				if (iToggle)
					pButton->addActionSignal(new CMenuHandler_ToggleCvar(m_pCurrentCommandMenu, cCommand));
				else
					pButton->addActionSignal(new CMenuHandler_StringCommand(m_pCurrentCommandMenu, cCommand, true));

				// Create an input signal that'll popup the current menu
				pButton->addInputSignal(new CMenuHandler_PopupSubMenuInput(pButton, m_pCurrentCommandMenu));
			}

			// Get the next token
			if (bGetExtraToken)
			{
				pfile = gEngfuncs.COM_ParseFile(pfile, token);
			}
		}
		// Move back up a menu
		m_pCurrentCommandMenu = m_pCurrentCommandMenu->GetParentMenu();

		pfile = gEngfuncs.COM_ParseFile(pfile, token);
	}
}

	catch(...)
	{
	CON_PRINTF("CViewport::CreateCommandMenu() exception!\n");
	m_iInitialized = false;
	return newIndex;
	}

	SetCurrentMenu(NULL);
	SetCurrentCommandMenu(NULL);
	if (pFileStart)
		gEngfuncs.COM_FreeFile(pFileStart);

	m_iInitialized = true;
	return newIndex;
}

//-----------------------------------------------------------------------------
// Purpose: CreateSubMenu
// Input  : *pButton - 
//			*pParentMenu - 
//			iYOffset - 
//			iXOffset - 
// Output : CCommandMenu
//-----------------------------------------------------------------------------
CCommandMenu *CViewport::CreateSubMenu(CommandButton *pButton, CCommandMenu *pParentMenu, int iYOffset, int iXOffset)
{
	int iXPos = 0;
	int iYPos = 0;
	int iWide = CMENU_SIZE_X;
	int iTall = 0;
	int iDirection = 0;

	if (pParentMenu)
	{
		iXPos = m_pCurrentCommandMenu->GetXOffset() + (CMENU_SIZE_X - 1) + iXOffset;
		iYPos = m_pCurrentCommandMenu->GetYOffset() + iYOffset;
		iDirection = pParentMenu->GetDirection();
	}

	CCommandMenu *pMenu = new CCommandMenu(pParentMenu, iDirection, iXPos, iYPos, iWide, iTall);
	pMenu->setParent(this);
	pButton->AddSubMenu(pMenu);
	pButton->setFont(Scheme::sf_primary3);
	pMenu->m_iButtonSizeY = m_pCurrentCommandMenu->m_iButtonSizeY;

	// Create the Submenu-open signal
	InputSignal *pISignal = new CMenuHandler_PopupSubMenuInput(pButton, pMenu);
	pButton->addInputSignal(pISignal);

	// Put a > to show it's a submenu
	CImageLabel *pLabel = new CImageLabel("arrowright", CMENU_SIZE_X - SUBMENU_SIZE_X, SUBMENU_SIZE_Y);
	pLabel->setParent(pButton);
	pLabel->addInputSignal(pISignal);

	// Reposition
	pLabel->getPos(iXPos, iYPos);
	int iw,it;
	pLabel->getImageSize(iw,it);
	pLabel->setPos(CMENU_SIZE_X - iw, (BUTTON_SIZE_Y - it) / 2);

	// Create the mouse off signal for the Label too
	if (!pButton->m_bNoHighlight)
		pLabel->addInputSignal(new CHandler_CommandButtonHighlight(pButton));

	return pMenu;
}

//-----------------------------------------------------------------------------
// Purpose: Obsolete
// Input  : *pButtonText - 
//			*pButtonName - 
// Output : CommandButton
//-----------------------------------------------------------------------------
/*CommandButton *CViewport::CreateCustomButton( char *pButtonText, char *pButtonName, int iYOffset )
{
	CommandButton *pButton = NULL;
	CCommandMenu  *pMenu = NULL;

/	if (!strcmp(pButtonName, "!CHANGETEAM"))
	{
		// ChangeTeam Submenu
		pButton = new CommandButton(pButtonText, 0, BUTTON_SIZE_Y * 2, CMENU_SIZE_X, BUTTON_SIZE_Y);

		// Create the submenu
		pMenu = CreateSubMenu(pButton, m_pCurrentCommandMenu, iYOffset );
		m_pCommandMenus[m_iNumMenus] = pMenu;
		m_iNumMenus++;

		// Auto Assign button
		m_pTeamButtons[0] = new TeamButton(0, gHUD.m_TextMessage.BufferedLocaliseTextString("#Team_AutoAssign"), 0, BUTTON_SIZE_Y, CMENU_SIZE_X, BUTTON_SIZE_Y);
		m_pTeamButtons[0]->addActionSignal(new CMenuHandler_StringCommand("jointeam 0"));
		pMenu->AddButton(m_pTeamButtons[0]); 

		// ChangeTeam buttons
		for (int i = 1; i < MAX_TEAMS+1; i++)// XDM: team index
		{
			char sz[16]; 
			sprintf(sz, "jointeam %d", i);
			m_pTeamButtons[i] = new TeamButton(i, "teamname", 0, BUTTON_SIZE_Y, CMENU_SIZE_X, BUTTON_SIZE_Y);
			m_pTeamButtons[i]->addActionSignal(new CMenuHandler_StringCommand(sz));// XDM: was CMenuHandler_StringCommandWatch
			pMenu->AddButton(m_pTeamButtons[i]); 
		}

		// Spectate button
		m_pTeamButtons[MAX_TEAMS+1] = new SpectateButton(BufferedLocaliseTextString("#Menu_Spectate"), 0, BUTTON_SIZE_Y, CMENU_SIZE_X, BUTTON_SIZE_Y, false);
		m_pTeamButtons[MAX_TEAMS+1]->addActionSignal(new CMenuHandler_StringCommand("spectate"));
		pMenu->AddButton(m_pTeamButtons[MAX_TEAMS+1]); 
	}
	if ( !strcmp( pButtonName, "!MAPBRIEFING" ) )
	{
		pButton = new CommandButton(pButtonText, 0, BUTTON_SIZE_Y * m_pCurrentCommandMenu->GetNumButtons(), CMENU_SIZE_X, BUTTON_SIZE_Y, false);
		pButton->addActionSignal(new CMenuHandler_TextWindow(MENU_MAPBRIEFING));
		// Create an input signal that'll popup the current menu
		pButton->addInputSignal( new CMenuHandler_PopupSubMenuInput(pButton, m_pCurrentCommandMenu) );
	}
	else if ( !strcmp( pButtonName, "!SERVERINFO" ) )
	{
		pButton = new CommandButton(pButtonText, 0, BUTTON_SIZE_Y * m_pCurrentCommandMenu->GetNumButtons(), CMENU_SIZE_X, BUTTON_SIZE_Y, false, false);
		pButton->addActionSignal(new CMenuHandler_TextWindow(MENU_INTRO));
		// Create an input signal that'll popup the current menu
		pButton->addInputSignal( new CMenuHandler_PopupSubMenuInput(pButton, m_pCurrentCommandMenu) );
	}
	return pButton;
}*/

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CViewport::ToggleServerBrowser(void)
{
	if (!m_iInitialized)
		return;

	if (!m_pServerBrowser)
		return;

	if (m_pServerBrowser->isVisible())
		m_pServerBrowser->setVisible(false);
	else
		m_pServerBrowser->setVisible(true);

	UpdateCursorState();
}
/*
void CViewport::ToggleInventoryPanel()// XDM
{
	if (!m_iInitialized)
		return;

	if (!m_pInventoryPanel)
		return;

	if (m_pInventoryPanel->isVisible())
		m_pInventoryPanel->Close();
	else
		m_pInventoryPanel->Open();

	UpdateCursorState();
}
*/

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CViewport::ToggleMusicPlayer(void)// XDM
{
	if (!m_iInitialized)
		return;

	if (!m_pMusicPlayer)
		return;

	if (m_pMusicPlayer->isVisible())
		m_pMusicPlayer->Close();
	else
	{
		ShowMenu(m_pMusicPlayer);
//		m_pMusicPlayer->Open();
//		m_pCurrentMenu = m_pMusicPlayer;
	}

	UpdateCursorState();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : menuIndex - 
//-----------------------------------------------------------------------------
void CViewport::ShowCommandMenu(int menuIndex)
{
	if (!m_iInitialized)
		return;

	//Already have a menu open.
	if (m_pCurrentMenu)
		return;

	// is the command menu open?
	if (m_pCurrentCommandMenu == m_pCommandMenus[menuIndex])
	{
		HideCommandMenu();
		return;
	}

	// Not visible while in intermission
	if (gHUD.m_iIntermission)
		return;

	// Recalculate visible menus
	UpdateCommandMenu(menuIndex);
	HideVGUIMenu();

	SetCurrentCommandMenu(m_pCommandMenus[menuIndex]);
	m_flMenuOpenTime = gHUD.m_flTime;
	UpdateCursorState();

	// get command menu parameters
	for (int i = 2; i < CMD_ARGC(); ++i)
	{
		const char *param = CMD_ARGV(i - 1);
		if (param)
		{
			//if (m_pCurrentCommandMenu->KeyInput(param[0]))
			if (m_pCurrentCommandMenu->KeyInput(1, param[0], "") == 0)
				HideCommandMenu();// kill the menu open time, since the key input is final
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handles the key input of "-commandmenu"
//-----------------------------------------------------------------------------
void CViewport::InputSignalHideCommandMenu(void)
{
	if (!m_iInitialized)
		return;

	// if they've just tapped the command menu key, leave it open
	if ((m_flMenuOpenTime + 0.3) > gHUD.m_flTime)
		return;

	HideCommandMenu();
}

//-----------------------------------------------------------------------------
// Purpose: Hides the command menu
//-----------------------------------------------------------------------------
void CViewport::HideCommandMenu(void)
{
	if (!m_iInitialized)
		return;

	if (m_pCommandMenus[m_StandardMenu])
		m_pCommandMenus[m_StandardMenu]->Close();//ClearButtonsOfArmedState();

	if (m_pCommandMenus[m_SpectatorOptionsMenu])
		m_pCommandMenus[m_SpectatorOptionsMenu]->Close();//ClearButtonsOfArmedState();

	if (m_pCommandMenus[m_SpectatorCameraMenu])
		m_pCommandMenus[m_SpectatorCameraMenu]->Close();//ClearButtonsOfArmedState();

	m_flMenuOpenTime = 0.0f;
//	SetCurrentCommandMenu(NULL);
//	UpdateCursorState();
}

//-----------------------------------------------------------------------------
// Purpose: Bring up the scoreboard
//-----------------------------------------------------------------------------
void CViewport::ShowScoreBoard(void)
{
	if (m_pScoreBoard)
	{
		// No Scoreboard in single-player
//		if (gHUD.m_iGameType != GT_SINGLE)
		if (gEngfuncs.GetMaxClients() > 1)
		{
			m_pScoreBoard->Open();
			UpdateCursorState();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if the scoreboard is up
//-----------------------------------------------------------------------------
bool CViewport::IsScoreBoardVisible(void)
{
	if (m_pScoreBoard)
		return m_pScoreBoard->isVisible();

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Hide the scoreboard
//-----------------------------------------------------------------------------
void CViewport::HideScoreBoard(void)
{
	// Prevent removal of scoreboard during intermission
	if (gHUD.m_iIntermission)
		return;

	if (m_pScoreBoard)
	{
		m_pScoreBoard->setVisible(false);
		GetClientVoiceMgr()->StopSquelchMode();
		UpdateCursorState();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Show the InventoryPanel
//-----------------------------------------------------------------------------
/*void CViewport::ShowInventoryPanel(void)// XDM
{
	if (m_pInventoryPanel)
	{
		m_pInventoryPanel->Open();
		m_pInventoryPanel->setVisible(true);
	}
}

//-----------------------------------------------------------------------------
// Purpose: CreateInventoryPanel
//-----------------------------------------------------------------------------
void CViewport::CreateInventoryPanel(void)// XDM
{
	m_pInventoryPanel = new InventoryPanel(100, 0,0, ScreenWidth, ScreenHeight);
	m_pInventoryPanel->setParent(this);
	m_pInventoryPanel->setVisible(false);
}*/

//-----------------------------------------------------------------------------
// Purpose: CreateMusicPlayerPanel
//-----------------------------------------------------------------------------
void CViewport::CreateMusicPlayerPanel(void)// XDM
{
	m_pMusicPlayer = new CMusicPlayerPanel(false, 0, 0, XRES(MPLAYER_PANEL_SIZEX), YRES(MPLAYER_PANEL_SIZEY));
	m_pMusicPlayer->setParent(this);
	m_pMusicPlayer->setVisible(false);
}


//-----------------------------------------------------------------------------
// Purpose: SetCurrentCommandMenu
// Set the submenu of the Command Menu
// Unfortunately, this is called for the root menu and for all submenus
// Input  : *pNewMenu - 
//-----------------------------------------------------------------------------
void CViewport::SetCurrentCommandMenu(CCommandMenu *pNewMenu)
{
	for (int i = 0; i < m_iNumMenus; ++i)
	{
		if (m_pCommandMenus[i] && m_pCommandMenus[i] != pNewMenu)
			m_pCommandMenus[i]->setVisible(false);
	}

	m_pCurrentCommandMenu = pNewMenu;

	if (m_pCurrentCommandMenu)
		m_pCurrentCommandMenu->Open();//MakeVisible(NULL);
}

//-----------------------------------------------------------------------------
// Purpose: UpdateCommandMenu
// Input  : menuIndex - 
//-----------------------------------------------------------------------------
void CViewport::UpdateCommandMenu(int menuIndex)
{
	m_pCommandMenus[menuIndex]->RecalculateVisibles(0, false);
	m_pCommandMenus[menuIndex]->RecalculatePositions(0);
}

//-----------------------------------------------------------------------------
// Purpose: UpdateSpectatorPanel
//-----------------------------------------------------------------------------
void CViewport::UpdateSpectatorPanel(void)
{
	m_iUser1 = g_iUser1;
	m_iUser2 = g_iUser2;
	m_iUser3 = g_iUser3;

	if (!m_pSpectatorPanel)
		return;

	if (gHUD.IsSpectator() && gHUD.m_pCvarDraw->value && !gHUD.m_iIntermission)	// don't draw in dev_overview mode
	{
		// check if spectator combinations are still valid
		gHUD.m_Spectator.CheckSettings();

		if (!m_pSpectatorPanel->isVisible())
		{
			m_pSpectatorPanel->Open();//setVisible(true);	// show spectator panel, but
			m_pSpectatorPanel->ShowMenu(false);	// dsiable all menus/buttons
			// done inside UpdateCursorState();
		}
		// update spectator panel
		gViewPort->m_pSpectatorPanel->Update();
	}
	else
	{
		if (m_pSpectatorPanel->isVisible())
		{
			m_pSpectatorPanel->setVisible(false);
			m_pSpectatorPanel->ShowMenu(false);// dsiable all menus/buttons
		}
	}

	m_flSpectatorPanelLastUpdated = gHUD.m_flTime + 1.0f; // update every seconds
}

//-----------------------------------------------------------------------------
// Purpose: CreateScoreBoard
//-----------------------------------------------------------------------------
void CViewport::CreateScoreBoard(void)
{
	int xdent = SBOARD_INDENT_X, ydent = SBOARD_INDENT_Y;
	if (ScreenWidth <= 400)
	{
		xdent = SBOARD_INDENT_X_400; 
		ydent = SBOARD_INDENT_Y_400;
	}
	else if (ScreenWidth <= 512)
	{
		xdent = SBOARD_INDENT_X_512; 
		ydent = SBOARD_INDENT_Y_512;
	}

	m_pScoreBoard = new ScorePanel(xdent, ydent, ScreenWidth - (xdent * 2), ScreenHeight - (ydent * 2));
	if (m_pScoreBoard)
	{
		m_pScoreBoard->setParent(this);
		m_pScoreBoard->setVisible(false);
	}
}

//-----------------------------------------------------------------------------
// Purpose: CreateServerBrowser
//-----------------------------------------------------------------------------
void CViewport::CreateServerBrowser(void)
{
	m_pServerBrowser = new ServerBrowser(0, 0, ScreenWidth, ScreenHeight);
	if (m_pServerBrowser)
	{
		m_pServerBrowser->setParent(this);
		m_pServerBrowser->setVisible(false);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set the VGUI Menu
// Input  : *pMenu - 
//-----------------------------------------------------------------------------
void CViewport::SetCurrentMenu(CMenuPanel *pMenu)
{
	m_pCurrentMenu = pMenu;
	if (m_pCurrentMenu)
	{
		// Don't open menus in demo playback
		if (gEngfuncs.pDemoAPI->IsPlayingback())
			return;

		m_pCurrentMenu->Open();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Text Window WARNING: always check for overflows!
// Input  : iTextToShow - SHOW_MAPBRIEFING
// Output : CMenuPanel *
//-----------------------------------------------------------------------------
CMenuPanel *CViewport::CreateTextWindow(int iTextToShow)
{
	char sz[SZ_LEN];
	char *pFile = NULL;
	char *pFileStart = NULL;// XDM3035c
	char cTitle[MAX_TITLE_LENGTH] = "\0";
	char cText[MAX_TEXT_LENGTH] = "\0";// XDM: !!!
	bool fullscreen = 1;

	if (iTextToShow == SHOW_MAPBRIEFING || iTextToShow == SHOW_MOTD)// XDM
	{
		// Get the current mapname, and open it's map briefing text
		_snprintf(sz, SZ_LEN, "maps/%s.txt", GetMapName());
		pFile = (char*)gEngfuncs.COM_LoadFile(sz, 5, NULL);
		pFileStart = pFile;

		if (iTextToShow == SHOW_MOTD)// XDM: append text from file to the MOTD
		{
			LocaliseTextString("#TextSeparatorLine", sz, SZ_LEN);
			if (pFile)
			{
				LocaliseTextString("#Title_mapinfo", cTitle, MAX_TITLE_LENGTH);// just to temporary store "Map information:"
				_snprintf(cText, MAX_TEXT_LENGTH, "%s\n%s\n\n%s\n%s\n\n%s\n\n%s\n", GetMOTD(), sz, GetGameRulesIntroText(gHUD.m_iGameType, gHUD.m_iGameMode), sz, cTitle, pFile);
			}
			else
				_snprintf(cText, MAX_TEXT_LENGTH, "%s\n%s\n\n%s\n", GetMOTD(), sz, GetGameRulesIntroText(gHUD.m_iGameType, gHUD.m_iGameMode));
//				strncpy(cText, GetMOTD(), MAX_MOTD_LENGTH);

			_snprintf(cTitle, MAX_TITLE_LENGTH, "%s - %s %s\0", m_szServerName[0]?m_szServerName:"XHL Server", GetMapName(), GetGameDescription(gHUD.m_iGameType));
		}
		else
		{
			if (pFile)
				strncpy(cText, pFile, MAX_TEXT_LENGTH);
			else
				strcpy(cText, sz);

			_snprintf(cTitle, MAX_TITLE_LENGTH, "%s %s\0", GetMapName(), GetGameDescription(gHUD.m_iGameType));
//			strncpy(cTitle, GetMapName(), MAX_TITLE_LENGTH);
		}
		cTitle[MAX_TITLE_LENGTH-1] = 0;
	}
	else if (iTextToShow == SHOW_SPECHELP)
	{
		LocaliseTextString("#Spec_Help_Title", cTitle, MAX_TITLE_LENGTH);
		cTitle[MAX_TITLE_LENGTH-1] = 0;

		LocaliseTextString("#Spec_Help_Text", cText, MAX_TEXT_LENGTH);
		cText[MAX_TEXT_LENGTH-1] = 0;

		fullscreen = 0;
	}

	if (m_pSpectatorPanel->isVisible())// XDM
		fullscreen = 0;

	// if we're in the game, flag the menu to be only grayed in the dialog box, instead of full screen
	CMenuPanel *pMOTDPanel = new CMessageWindowPanel(cText, cTitle, fullscreen, 0, -1, -1, XRES(MOTD_WINDOW_SIZE_X), YRES(MOTD_WINDOW_SIZE_Y));
	pMOTDPanel->setParent(this);

	if (pFileStart)
		gEngfuncs.COM_FreeFile(pFileStart);

	return pMOTDPanel;
}

//-----------------------------------------------------------------------------
// Purpose: ShowMenu by ID
// Input  : iMenu - 
//-----------------------------------------------------------------------------
CMenuPanel *CViewport::ShowMenu(int iMenu)
{
	CMenuPanel *pNewMenu = NULL;

	// Don't open menus in demo playback
	if (gEngfuncs.pDemoAPI->IsPlayingback())
		return pNewMenu;

	// Don't open any menus except the MOTD during intermission
	// MOTD needs to be accepted because it's sent down to the client 
	// after map change, before intermission's turned off
	if ( gHUD.m_iIntermission && iMenu != MENU_INTRO )
		return pNewMenu;

	// Don't create one if it's already in the list
	if (m_pCurrentMenu)
	{
		CMenuPanel *pMenu = m_pCurrentMenu;
		while (pMenu != NULL)
		{
			if (pMenu->GetMenuID() == iMenu)
				return pMenu;
			pMenu = pMenu->GetNextMenu();
		}
	}

	switch (iMenu)
	{
	case MENU_TEAM:		
		pNewMenu = ShowTeamMenu(); 
		break;

	case MENU_MAPBRIEFING:
		pNewMenu = CreateTextWindow(SHOW_MAPBRIEFING);
		break;

	case MENU_INTRO:
		pNewMenu = CreateTextWindow(SHOW_MOTD);
		break;

	case MENU_SPECHELP:
		pNewMenu = CreateTextWindow(SHOW_SPECHELP);
		break;

//	case MENU_CONSOLE: STUB
//		pNewMenu = new ConsolePanel(0,0, getWide(), getTall()/2);
//		break;

//	case MENU_: STUB
//		pNewMenu = new ControlConfigPanel(0,0, getWide(), getTall()/2);
//		break;
//	case MENU_ENTITYCREATE:
//		pNewMenu = new CEntityEntryPanel(ScreenWidth/2 - EEP_WIDTH/2, ScreenHeight/2 - EEP_HEIGHT/2, EEP_WIDTH, EEP_HEIGHT);
//		break;
#ifdef _DEBUG// too risky
	case MENU_VOICETWEAK:
		pNewMenu = GetVoiceTweakDlg();//CVoiceVGUITweakDlg();
		break;
#endif
	default:
		CON_PRINTF("ShowVGUIMenu(): Cannot open menu %d!\n", iMenu);
		break;
	}

	if (pNewMenu)
	{
		pNewMenu->SetMenuID(iMenu);
		return ShowMenu(pNewMenu);
	}
	return pNewMenu;
}

//-----------------------------------------------------------------------------
// Purpose: ShowMenu directly
// Input  : *pMenu - 
// Output : CMenuPanel
//-----------------------------------------------------------------------------
CMenuPanel *CViewport::ShowMenu(CMenuPanel *pNewMenu)
{
	if (pNewMenu == NULL)
		return NULL;

	// Close the Command Menu if it's open
	HideCommandMenu();

//	pNewMenu->SetMenuID(iMenu);
	pNewMenu->SetActive(true);
	pNewMenu->setParent(this);

	// See if another menu is visible, and if so, cache this one for display once the other one's finished
	if (m_pCurrentMenu)
	{
		m_pCurrentMenu->SetNextMenu(pNewMenu);
	}
	else
	{
		m_pCurrentMenu = pNewMenu;
		m_pCurrentMenu->Open();
		UpdateCursorState();
	}
	return pNewMenu;
}

//-----------------------------------------------------------------------------
// Purpose: delete this
//-----------------------------------------------------------------------------
void CViewport::OnMenuPanelClose(CMenuPanel *pMenu)
{
	if (pMenu)
	{
		if (m_pCurrentMenu == pMenu)
			SetCurrentMenu(m_pCurrentMenu->GetNextMenu());
//			m_pCurrentMenu = NULL;

		if (pMenu->ShouldBeRemoved())
			removeChild(pMenu);

		UpdateCursorState();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Removes all VGUI Menu's onscreen
//-----------------------------------------------------------------------------
void CViewport::HideVGUIMenu(void)
{
	while (m_pCurrentMenu)
	{
		HideTopMenu();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Remove the top VGUI menu, and bring up the next one
//-----------------------------------------------------------------------------
void CViewport::HideTopMenu(void)
{
	if (m_pCurrentMenu)
	{
		// Close the top one
//		m_pCurrentMenu->Close();// this may get destroyed?
		// Bring up the next one
//		gViewPort->SetCurrentMenu(m_pCurrentMenu->GetNextMenu());
		// TESTME
		m_pCurrentMenu->Close();
	}
	UpdateCursorState();
}

//-----------------------------------------------------------------------------
// Purpose:  AllowedToPrintText
// Output : Returns TRUE if the HUD's allowed to print text messages
//-----------------------------------------------------------------------------
bool CViewport::AllowedToPrintText(void)
{
	// Prevent text messages when fullscreen menus are up

#if 0// XDM3035a: nowai
	if (m_pCurrentMenu)
	{
		int iId = m_pCurrentMenu->GetMenuID();
		if (iId == MENU_TEAM || /*iId == MENU_CLASS ||*/ iId == MENU_INTRO || iId == MENU_CLASSHELP)
			return FALSE;
	}
#endif
	return TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: Bring up the Team selection Menu
// Output : CMenuPanel*
//-----------------------------------------------------------------------------
CMenuPanel *CViewport::ShowTeamMenu(void)
{
	// Don't open menus in demo playback
	if (gEngfuncs.pDemoAPI->IsPlayingback())
		return NULL;

// 3033 Why? I forgot! =)
//	if (m_pSpectatorPanel->isVisible())// XDM: don't allow team menu in spectator mode
//		return NULL;

	m_pTeamMenu->Reset();
	return m_pTeamMenu;
}

//-----------------------------------------------------------------------------
// Purpose: CreateTeamMenu
//-----------------------------------------------------------------------------
void CViewport::CreateTeamMenu(void)
{
	// Create the panel
	m_pTeamMenu = new CTeamMenuPanel(false, 0, 0, ScreenWidth, ScreenHeight);
	m_pTeamMenu->setParent(this);
	m_pTeamMenu->setVisible(false);
}

//-----------------------------------------------------------------------------
// Purpose: Spectator "Menu" explaining the Spectator buttons
//-----------------------------------------------------------------------------
void CViewport::CreateSpectatorMenu(void)
{
	// Create the Panel
	m_pSpectatorPanel = new SpectatorPanel(0, 0, ScreenWidth, ScreenHeight);
	m_pSpectatorPanel->setParent(this);
	m_pSpectatorPanel->setVisible(false);
	m_pSpectatorPanel->Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: UPDATE HUD SECTIONS
// We've got an update on player info
// Recalculate any menus that use it.
//-----------------------------------------------------------------------------
void CViewport::UpdateOnPlayerInfo(void)
{
	UpdateCommandMenu(m_StandardMenu);
//	m_SpectatorOptionsMenu

	if (m_pTeamMenu)
		m_pTeamMenu->Update();

	if (m_pScoreBoard)
		m_pScoreBoard->Update();

//	if (m_pInventoryPanel)// XDM
//		m_pInventoryPanel->Update();

//	if (m_pMusicPlayer)// XDM: ?
//		m_pMusicPlayer->Update();
}

//-----------------------------------------------------------------------------
// Purpose: UpdateCursorState
//-----------------------------------------------------------------------------
void CViewport::UpdateCursorState(void)
{
//	if (m_pCurrentMenu->IsShowingCursor())
	// Need cursor if any VGUI window is up
	if (m_pCurrentMenu && m_pCurrentMenu->isVisible() && m_pCurrentMenu->IsShowingCursor() ||
		m_pSpectatorPanel->isVisible() && m_pSpectatorPanel->IsShowingCursor() ||
		m_pTeamMenu->isVisible() ||
		m_pServerBrowser->isVisible() ||
//		m_pInventoryPanel->isVisible() ||
		m_pMusicPlayer->isVisible() ||
		g_iMouseManipulationMode > 0 ||// XDM3035a: test
		GetClientVoiceMgr()->IsInSquelchMode())// XDM
	{
		g_iVisibleMouse = true;
		App::getInstance()->setCursorOveride(App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_arrow));
		return;
	}
	else if (m_pCurrentCommandMenu)
	{
		// commandmenu doesn't have cursor if hud_capturemouse is turned off
		if (gHUD.m_pCvarStealMouse->value > 0.0f)
		{
			g_iVisibleMouse = true;
			App::getInstance()->setCursorOveride(App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_arrow));
			return;
		}
	}

	// Don't reset mouse in demo playback
	if (!gEngfuncs.pDemoAPI->IsPlayingback())
	{
		IN_ResetMouse();
	}

	g_iVisibleMouse = false;
	App::getInstance()->setCursorOveride(App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_none));
}

//-----------------------------------------------------------------------------
// Purpose: UpdateHighlights
//-----------------------------------------------------------------------------
void CViewport::UpdateHighlights(void)
{
	if (m_pCurrentCommandMenu)
		m_pCurrentCommandMenu->Open();//MakeVisible(NULL);
}

//-----------------------------------------------------------------------------
// Purpose: GetAllPlayersInfo from engine
//-----------------------------------------------------------------------------
void CViewport::GetAllPlayersInfo(void)
{
	for (int i = 1; i <= MAX_PLAYERS; ++i)// XDM3035a: include index 32!!!
		GetPlayerInfo(i, &g_PlayerInfoList[i]);
}

//-----------------------------------------------------------------------------
// Purpose: XDM3035a
// Output : char
//-----------------------------------------------------------------------------
char *CViewport::GetMapName(void)
{
	if (!m_sMapName || !m_sMapName[0])
		_splitpath(GET_LEVEL_NAME(), NULL, NULL, m_sMapName, NULL);
//		ExtractFileName(GET_LEVEL_NAME(), NULL, m_sMapName, NULL);

	return m_sMapName;
}

//-----------------------------------------------------------------------------
// Purpose: GetTeamName
// Input  : &team_id - 
// Output : char
//-----------------------------------------------------------------------------
char *CViewport::GetTeamName(const TEAM_ID &team_id)
{
	if (IsActiveTeam(team_id))//if (IsValidTeam(team_id))
		return g_TeamInfo[team_id].name;

//DBG_FORCEBREAK
	return "";
}

//-----------------------------------------------------------------------------
// Purpose: Get MOTD text
// Output : char
//-----------------------------------------------------------------------------
char *CViewport::GetMOTD(void)
{
//	if (m_iGotAllMOTD)
		return m_szMOTD;
}

//-----------------------------------------------------------------------------
// Purpose: GetAllowSpectators
// Output : int
//-----------------------------------------------------------------------------
int CViewport::GetAllowSpectators(void)
{
	return (gHUD.m_iGameFlags & GAME_FLAG_ALLOW_SPECTATORS)?1:0;
}

//-----------------------------------------------------------------------------
// Purpose: draw "fullscreen" background for menus that require it
//-----------------------------------------------------------------------------
void CViewport::paintBackground(void)
{
	if (m_pCurrentMenu)
	{
		if (m_pCurrentMenu->getBackgroundMode() > BG_FILL_NONE)// TODO: revisit this when fill bug is fixed //== BG_FILL_SCREEN)
		{
			vgui::Color color;
			m_pCurrentMenu->getBgColor(color);
			drawSetColor(color[0],color[1],color[2], color[3]);// m_pCurrentMenu->m_iTransparency);
			drawFilledRect(0,0,_size[0],_size[1]);//ScreenWidth,ScreenHeight);
			// BUGBUG: this shit does not work
//			FillRGBA(0,0, ScreenWidth,ScreenHeight, color[0],color[1],color[2], color[3]);
		}
	}

	if (m_pScoreBoard)
	{
		int x, y;
		getApp()->getCursorPos(x, y);
		m_pScoreBoard->cursorMoved(x, y, m_pScoreBoard);
	}

	// See if the command menu is visible and needs recalculating due to some external change
	if (gHUD.m_iTeamNumber != m_iCurrentTeamNumber)
	{
		UpdateCommandMenu(m_StandardMenu);
		m_iCurrentTeamNumber = gHUD.m_iTeamNumber;
	}

	// See if the Spectator Menu needs to be update
	if ((g_iUser1 != m_iUser1 || g_iUser2 != m_iUser2) || (m_flSpectatorPanelLastUpdated < gHUD.m_flTime))
	{
		UpdateSpectatorPanel();
	}

	// Update the Scoreboard, if it's visible
	if (m_pScoreBoard->isVisible() && (m_flScoreBoardLastUpdated < gHUD.m_flTime))
	{
		m_pScoreBoard->Update();
		m_flScoreBoardLastUpdated = gHUD.m_flTime + 0.5f;
	}

	if (m_pMusicPlayer && m_pMusicPlayer->isVisible())// XDM
		m_pMusicPlayer->Update();

	int extents[4];
	getAbsExtents(extents[0],extents[1],extents[2],extents[3]);
	VGui_ViewportPaintBackground(extents);
}

//-----------------------------------------------------------------------------
// Purpose: Number Key Input If there's a menu up, give it the input
// Input  : iSlot - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CViewport::SlotInput(int iSlot)
{
	if (m_pCurrentMenu)
		return m_pCurrentMenu->SlotInput(iSlot);

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Direct Key Input
// Input  : down - 
//			keynum - 
//			*pszCurrentBinding - 
//-----------------------------------------------------------------------------
int CViewport::KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding)
{
	// Open Text Window?
	if (m_pCurrentMenu && gEngfuncs.Con_IsVisible() == false)
	{
		int iMenuID = m_pCurrentMenu->GetMenuID();

		// Get number keys as Input for Team/Class menus
		if (iMenuID == MENU_TEAM/* || iMenuID == MENU_CLASS*/)
		{
			// Escape gets you out of Team/Class menus if the Cancel button is visible
			if (keynum == K_ESCAPE)
			{
				if (iMenuID == MENU_TEAM && gHUD.m_iTeamNumber)
				{
					HideTopMenu();
					return 0;
				}
			}

			for (int i = '0'; i <= '9'; ++i)
			{
				if (down && (keynum == i))
				{
					SlotInput(i - '0');
					return 0;
				}
			}
		}
	}
	int passnext = 1;
	// if we're in a command menu, try hit one of it's buttons
	if (passnext)
	{
		if (m_pCurrentCommandMenu)
		{
			if (down)
			{
				if (keynum == K_ESCAPE)// Escape hides the command menu
				{
					HideCommandMenu();
					return 0;
				}
			}
			passnext = m_pCurrentCommandMenu->KeyInput(down, keynum, pszCurrentBinding);
		}
	}
	if (passnext)
	{
		if (m_pCurrentMenu)// XDM3035a: TESTME
		{
			if (m_pCurrentMenu->IsCapturingInput() && m_pCurrentMenu->isVisible())//hasFocus())
				passnext = m_pCurrentMenu->KeyInput(down, keynum, pszCurrentBinding);
		}
	}

/*	for (int i=0; passnext && i<getChildCount(); ++i)// right thing to do
	{
		Panel *pPanel = getChild(i);
		if (pPanel->isVisible())
		{
			char str[16];
			pPanel->getPersistanceText(str, 16);// HACK
//			CON_PRINTF(" VPPT: %s\n", str);
//			if (IsMenuPanel(pPanel))
			if (strcmp(str, "CMenuPanel") == 0)
			{
				passnext = ((CMenuPanel *)pPanel)->KeyInput(down, keynum, pszCurrentBinding);
			}
		}
	}*/

	return passnext;
}

//-----------------------------------------------------------------------------
// Purpose: DeathMsg incoming from the server
// Input  : killer - 
//			victim - 
//-----------------------------------------------------------------------------
void CViewport::DeathMsg(int killer, int victim)
{
	m_pScoreBoard->DeathMsg(killer, victim);
}

//-----------------------------------------------------------------------------
// Purpose: MOTD
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : int
//-----------------------------------------------------------------------------
int CViewport::MsgFunc_MOTD(const char *pszName, int iSize, void *pbuf)
{
	if (m_iGotAllMOTD)
		m_szMOTD[0] = 0;

	BEGIN_READ(pbuf, iSize);

	m_iGotAllMOTD = READ_BYTE();
	int roomInArray = sizeof(m_szMOTD) - strlen(m_szMOTD) - 1;
	strncat(m_szMOTD, READ_STRING(), roomInArray >= 0 ? roomInArray : 0);
	m_szMOTD[sizeof(m_szMOTD)-1] = '\0';

	END_READ();

	// don't show MOTD for HLTV spectators
	if (m_iGotAllMOTD && !gEngfuncs.IsSpectateOnly())
		ShowMenu(MENU_INTRO);

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: ServerName
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : int
//-----------------------------------------------------------------------------
int CViewport::MsgFunc_ServerName(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	strncpy(m_szServerName, READ_STRING(), MAX_SERVERNAME_LENGTH);
	END_READ();
	m_szServerName[MAX_SERVERNAME_LENGTH-1] = '\0';// XDM: overflow!!
	m_pScoreBoard->Update();//XDM3035c://UpdateTitle();
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: ScoreInfo
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : int
//-----------------------------------------------------------------------------
int CViewport::MsgFunc_ScoreInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	short cl = READ_BYTE();
	short frags = READ_SHORT();
	short deaths = READ_SHORT();
	END_READ();

	if (IsValidPlayerIndex(cl))
	{
		g_PlayerExtraInfo[cl].frags = frags;
		g_PlayerExtraInfo[cl].deaths = deaths;
		UpdateOnPlayerInfo();
	}
	else
		CON_DPRINTF("MsgFunc_ScoreInfo: invalid player index: %d!\n", cl);

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Team Names from server
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// byte		number of teams
// -- for each team:
// byte // byte // byte - color RGB
// string	team name
// ...
// Output : int
//-----------------------------------------------------------------------------
int CViewport::MsgFunc_TeamNames(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int n = READ_BYTE();// Number of entries transmitted WARNING! +1 for 0th team!

//	char *str = NULL;
	float h=1.0f,s=0.0f,l=0.0f;// XDM3035
	for (int i = 0; i < n; ++i)
	{
// XDM3035		g_TeamInfo[i].id = i;// XDM3035
		g_TeamInfo[i].color[0] = READ_BYTE();
		g_TeamInfo[i].color[1] = READ_BYTE();
		g_TeamInfo[i].color[2] = READ_BYTE();

		RGB2HSL(g_TeamInfo[i].color[0],
				g_TeamInfo[i].color[1],
				g_TeamInfo[i].color[2],
				h,s,l);

		// Half-Life colormap is hue in range of 0...255
		g_TeamInfo[i].colormap = (int)(255.0f*(h/360.0f));//RGBtoHSV(g_TeamInfo[i].color);// no READ_BYTE();
		g_TeamInfo[i].colormap |= g_TeamInfo[i].colormap << 8;

		strncpy(g_TeamInfo[i].name, READ_STRING(), MAX_TEAM_NAME);// XDM
		// Set the team name buttons
/*		if (m_pTeamButtons[i])
			m_pTeamButtons[i]->setText(GetTeamName(i));*/
	}
	END_READ();

	m_iNumberOfTeams = n-1;// this must represent the number of REAL TEAMS
	ASSERT(m_iNumberOfTeams <= MAX_TEAMS);

	// Update the Team Menu
	if (m_pTeamMenu)
		m_pTeamMenu->Update();

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : int
//-----------------------------------------------------------------------------
int CViewport::MsgFunc_ShowMenu(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int menu = READ_BYTE();
	int flags = READ_BYTE();
	END_READ();
	CMenuPanel *pMenu = ShowMenu(menu);
	if (pMenu && flags > 0)// just nothing
		pMenu->setBackgroundMode(flags);
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: adds extra score to specified team
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : int
//-----------------------------------------------------------------------------
int CViewport::MsgFunc_TeamScore(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	byte team = READ_BYTE();
	if (IsValidTeam(team))
		g_TeamInfo[team].scores_overriden = READ_SHORT();

	END_READ();
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: "player is on team" notification
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : int
//-----------------------------------------------------------------------------
int CViewport::MsgFunc_TeamInfo(const char *pszName, int iSize, void *pbuf)
{
	if (!m_pScoreBoard)
		return 1;

	BEGIN_READ(pbuf, iSize);
	byte cl = READ_BYTE();
	short teamNum = READ_BYTE();
	END_READ();

	if (IsValidPlayerIndex(cl))// set the players team
	{
//		GetPlayerInfo(cl, &g_PlayerInfoList[cl]);
		if (g_PlayerExtraInfo[cl].teamnumber > TEAM_NONE && g_PlayerExtraInfo[cl].teamnumber != teamNum)
		{
			char str[MAX_CHARS_PER_LINE];// XDM: player has changed team
			_snprintf(str, MAX_CHARS_PER_LINE, BufferedLocaliseTextString("* #CHANGED_TEAM"), g_PlayerInfoList[cl].name, GetTeamName(teamNum));
			str[63] = 0;
			gHUD.m_SayText.SayTextPrint(str, 0, 0);
		}

		g_PlayerExtraInfo[cl].teamnumber = teamNum;// XDM

		if (g_PlayerInfoList[cl].thisplayer)
			gHUD.m_iTeamNumber = teamNum;
	}

	// rebuild the list of teams
	m_pScoreBoard->RebuildTeams();
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: "a player is a spectator" notification
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : int
//-----------------------------------------------------------------------------
int CViewport::MsgFunc_Spectator(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	short cl = READ_BYTE();
	if (IsValidPlayerIndex(cl))
	{
//old		g_IsSpectator[cl] = READ_BYTE();
		g_PlayerExtraInfo[cl].observer = READ_BYTE();
//		g_PlayerInfoList[cl].spectator = g_IsSpectator[cl];// XDM: we can't store data g_PlayerInfoList[cl].spectator because the engine will overwrite it :(

//		if (g_IsSpectator[cl] > 0)// player has become a spectator
		if (g_PlayerExtraInfo[cl].observer > 0)
		{
			g_PlayerExtraInfo[cl].teamnumber = TEAM_NONE;
			g_PlayerExtraInfo[cl].ready = 0;
		}
		// XDM3033: player will have to select team again on rejoin
	}
	END_READ();
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Obsolete!
// Input  : *pszName - 
//			iSize - 
//			*pbuf - 
// Output : int
//-----------------------------------------------------------------------------
int CViewport::MsgFunc_AllowSpec(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iAllowSpectators = READ_BYTE();
	END_READ();

	// Force the menu to update
	UpdateCommandMenu(m_StandardMenu);

	// If the team menu is up, update it too
	if (m_pTeamMenu)
		m_pTeamMenu->Update();

	CON_PRINTF("WARNING! Obsolete message! %s\n", pszName);
	if (iAllowSpectators > 0)
		gHUD.m_iGameFlags |= GAME_FLAG_ALLOW_SPECTATORS;// unsafe!
	else
		gHUD.m_iGameFlags &= ~GAME_FLAG_ALLOW_SPECTATORS;

	return 1;
}




//-----------------------------------------------------------------------------
// Purpose: InputSignal handler for the main viewport
// Everything you see here will be called ALWAYS disregarding which panels
// are currently shown e.g. if you click outside windows too.
//-----------------------------------------------------------------------------
void CViewPortInputHandler::mousePressed(MouseCode code, Panel *panel) 
{
//		if (panel && panel != gViewPort && panel->isEnabled() && panel->isVisible())
//			PlaySound("vgui/button_press.wav", VOL_NORM);// XDM

	if (code != MOUSE_LEFT)
	{
		// send a message to close the command menu
		// this needs to be a message, since a direct call screws the timing
		CLIENT_COMMAND("ForceCloseCommandMenu\n");
	}
}

/*void CViewPortInputHandler::mouseDoublePressed(MouseCode code, Panel *panel)
{
	if (panel && panel != gViewPort && panel->isEnabled() && panel->isVisible())
		PlaySound("vgui/button_press.wav", VOL_NORM);
}*/
