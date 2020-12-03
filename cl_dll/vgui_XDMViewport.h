#ifndef XDMVIEWPORT_H
#define XDMVIEWPORT_H

// XDM: OMFD! This all code is so dirty... Normal people should rewrite all of this from scratch.

#include <VGUI_Panel.h>
#include <VGUI_Frame.h>
#include <VGUI_TextPanel.h>
#include <VGUI_Label.h>
#include <VGUI_Button.h>
#include <VGUI_ActionSignal.h>
#include <VGUI_InputSignal.h>
#include <VGUI_Scheme.h>
#include <VGUI_Image.h>
#include <VGUI_FileInputStream.h>
#include <VGUI_BitmapTGA.h>
#include <VGUI_DesktopIcon.h>
#include <VGUI_App.h>
#include <VGUI_MiniApp.h>
#include <VGUI_LineBorder.h>
#include <VGUI_String.h>
#include <VGUI_ScrollPanel.h>
#include <VGUI_ScrollBar.h>
#include <VGUI_Slider.h>

// custom scheme handling
#include "vgui_SchemeManager.h"
#include "vgui_defaultinputsignal.h"
#include "vgui_CustomObjects.h"

using namespace vgui;

class Cursor;
class ScorePanel;
class SpectatorPanel;
//class CCommandMenu;
//class CommandLabel;
//class CommandButton;
class CMenuPanel;
class ServerBrowser;
//class DragNDropPanel;
class CTeamMenuPanel;
class CMusicPlayerPanel;// XDM

//char* GetVGUITGAName(const char *pszName);
BitmapTGA *LoadTGAForRes(const char *pImageName);

#define MAX_SERVERNAME_LENGTH	32
//#define MAX_MAP_NAME				256

// Command Menu positions 
#define MAX_MENUS				80

#define PANEL_DEFAULT_ALPHA		127

#define PANEL_INNER_OFFSET		2
//#define PANEL_TITLE_POS_X		XRES(PANEL_INNER_OFFSET)
//#define PANEL_TITLE_POS_Y		XRES(PANEL_INNER_OFFSET)

#define BUTTON_SIZE_X			XRES(120)
#define BUTTON_SIZE_Y			YRES(30)

#define TEXTENTRY_SIZE_Y		YRES(20)

#define CBUTTON_SIZE_Y			YRES(24)
#define CMENU_SIZE_X			XRES(160)

#define SUBMENU_SIZE_X			(CMENU_SIZE_X / 8)
#define SUBMENU_SIZE_Y			(BUTTON_SIZE_Y / 6)

#define CMENU_TOP				(BUTTON_SIZE_Y * 4)

#define CMENU_DIR_DOWN			0
#define CMENU_DIR_UP			1
#define CMENU_DIR_AUTO			5// undone

// Map Briefing Window
#define MAPBRIEF_INDENT			30
// Team Menu
#define TMENU_INDENT_X			(30 * ((float)ScreenHeight / 640))
#define TMENU_HEADER			100
#define TMENU_SIZE_X			(ScreenWidth - (TMENU_INDENT_X * 2))
#define TMENU_SIZE_Y			(TMENU_HEADER + BUTTON_SIZE_Y * 7)
#define TMENU_PLAYER_INDENT		(((float)TMENU_SIZE_X / 3) * 2)
#define TMENU_INDENT_Y			(((float)ScreenHeight - TMENU_SIZE_Y) / 2)

// CreateTextWindow
enum
{
	SHOW_NOTHING = 0,
	SHOW_MAPBRIEFING,
	SHOW_CLASSDESC,
	SHOW_MOTD,
	SHOW_SPECHELP,
};


//-----------------------------------------------------------------------------
// Purpose: InputSignal handler for the main viewport
// Everything you see here will be called ALWAYS disregarding which panels
// are currently shown e.g. if you click outside windows too.
//-----------------------------------------------------------------------------
class CViewPortInputHandler : public CDefaultInputSignal
{
public:
	virtual void mousePressed(MouseCode code, Panel *panel);
//	virtual void mouseDoublePressed(MouseCode code, Panel *panel);
};


//-----------------------------------------------------------------------------
// Purpose: The main UI class
// Virtually persists all the time the client process is running
//-----------------------------------------------------------------------------
class CViewport : public Panel
{
	typedef Panel BaseClass;
public:
	void *operator new(size_t stAllocateBlock);

	CViewport(int x,int y,int wide,int tall);
	void Initialize(void);
	void LoadScheme(void);

	int CreateCommandMenu(char *menuFile, int direction, int yOffset, bool flatDesign, int flButtonSizeX, int flButtonSizeY, int xOffset);

	void UpdateCursorState(void);
	void UpdateCommandMenu(int menuIndex);
	void UpdateOnPlayerInfo(void);
	void UpdateHighlights(void);
	void UpdateSpectatorPanel(void);

	int	KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding);
//	void InputPlayerSpecial(void);
	void GetAllPlayersInfo(void);
	void DeathMsg(int killer, int victim);

	void ShowCommandMenu(int menuIndex);
	void InputSignalHideCommandMenu(void);
	void HideCommandMenu(void );
	void SetCurrentCommandMenu(CCommandMenu *pNewMenu);
	void SetCurrentMenu(CMenuPanel *pMenu);

	void ShowScoreBoard(void);
	void HideScoreBoard(void);
	bool IsScoreBoardVisible(void);
//	void ShowInventoryPanel(void);// XDM

	bool AllowedToPrintText(void);

	CMenuPanel *ShowMenu(int iMenu);
	CMenuPanel *ShowMenu(CMenuPanel *pNewMenu);
	void OnMenuPanelClose(CMenuPanel *pMenu);
	void HideVGUIMenu(void);
	void HideTopMenu(void);

	void ToggleServerBrowser(void);
//	void ToggleInventoryPanel(void);// XDM
	void ToggleMusicPlayer(void);

	CMenuPanel *CreateTextWindow(int iTextToShow);// SHOW_MAPBRIEFING
	CCommandMenu *CreateSubMenu(CommandButton *pButton, CCommandMenu *pParentMenu, int iYOffset, int iXOffset = 0);

	// Data Handlers
	const byte GetNumberOfTeams(void) const { return m_iNumberOfTeams; };
	char *GetMapName(void);// XDM
	char *GetTeamName(const TEAM_ID &team_id);
	char *GetMOTD(void);// XDM
	int GetAllowSpectators(void);

	// Message Handlers
	int MsgFunc_TeamNames(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_ShowMenu(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_MOTD(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_ServerName(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_ScoreInfo(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_TeamScore(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_TeamInfo(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_Spectator(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AllowSpec(const char *pszName, int iSize, void *pbuf);

	// Input
	virtual bool SlotInput(int iSlot);
	virtual void paintBackground();

	CSchemeManager *GetSchemeManager(void) { return &m_SchemeManager; }
	ScorePanel *GetScoreBoard(void) { return m_pScoreBoard; }
	CMusicPlayerPanel *GetMusicPlayer(void)  { return m_pMusicPlayer; }
	SpectatorPanel *GetSpectatorPanel(void)  { return m_pSpectatorPanel; }

protected:
	void CreateScoreBoard(void);
	void CreateServerBrowser(void);
//	void CreateInventoryPanel(void);// XDM
	void CreateMusicPlayerPanel(void);
	void CreateSpectatorMenu(void);
	void CreateTeamMenu(void);
	CMenuPanel *ShowTeamMenu(void);
//	CommandButton *CreateCustomButton(char *pButtonText, char *pButtonName, int iYOffset);

public:
	int			m_StandardMenu;	// indexs in m_pCommandMenus
	int			m_SpectatorOptionsMenu;
	int			m_SpectatorCameraMenu;

	int			m_iBorderThickness;
	char		m_szServerName[MAX_SERVERNAME_LENGTH];

protected:
	int			m_iInitialized;
	byte		m_iNumberOfTeams;
	char		m_sMapName[MAX_MAPNAME];

	int			m_iGotAllMOTD;
	char		m_szMOTD[MAX_MOTD_LENGTH];

	float		m_flMenuOpenTime;
	float		m_flScoreBoardLastUpdated;
	float		m_flSpectatorPanelLastUpdated;

	int			m_iNumMenus;
	int			m_iCurrentTeamNumber;
	int			m_iUser1;
	int			m_iUser2;
	int			m_iUser3;

	// Scheme handler
	CSchemeManager m_SchemeManager;

	vgui::Cursor* _cursorNone;
	vgui::Cursor* _cursorArrow;
	CCommandMenu *m_pCommandMenus[MAX_MENUS];
	CCommandMenu *m_pCurrentCommandMenu;

	// VGUI Menus
	CMenuPanel			*m_pCurrentMenu;
	CTeamMenuPanel		*m_pTeamMenu;
//	InventoryPanel		*m_pInventoryPanel;
	CMusicPlayerPanel	*m_pMusicPlayer;
	ServerBrowser		*m_pServerBrowser;
	SpectatorPanel		*m_pSpectatorPanel;
	ScorePanel			*m_pScoreBoard;
};












//============================================================
// Panel that uses XDM (non-vgui) schemes
class CSchemeEnabledPanel : public Panel
{
	typedef Panel BaseClass;
public:
	CSchemeEnabledPanel(CScheme *pScheme, int x, int y, int wide, int tall);
	virtual void paintBackground(void);

	virtual CScheme *getScheme(void);
	virtual void setScheme(CScheme *pScheme);

	void ApplyScheme(CScheme *pScheme, Panel *pControl);

protected:
	CScheme		*m_pScheme;
};


//============================================================
// Panel that can be dragged around
class DragNDropPanel : public Panel
{
private:
	typedef Panel BaseClass;
public:
	DragNDropPanel(bool dragenabled, int x, int y, int wide, int tall);
	virtual void setPos(int x, int y);// safe version
	virtual void setDragged(bool bState);
	virtual void setDragEnabled(bool enable);
	virtual bool getDragEnabled(void);

protected:
//	LineBorder	*m_pDragBorder;
//	Border		*m_pOriginalBorder;
	bool		m_bBeingDragged;
	bool		m_bDragEnabled;
	int			m_OriginalPos[2];// set on creation
};


enum
{
	BG_FILL_NONE = 0,
	BG_FILL_NORMAL,
	BG_FILL_SCREEN
};


//============================================================
// Menu Panel that supports buffering of menus
// main class for all windows in XHL
class CMenuPanel : public DragNDropPanel
{
private:
	typedef DragNDropPanel BaseClass;
public:
	CMenuPanel(int RemoveMe, int x, int y, int wide, int tall);
	virtual void Open(void);
	virtual void Close(void);
	virtual void Reset(void);
	virtual void getPersistanceText(char *buf, int bufLen);// HACK
	virtual void paintBackground(void);
	virtual void OnActionSignal(int signal);
	virtual void setCaptureInput(bool enable);
	virtual void setShowCursor(bool show);
	virtual bool IsCapturingInput(void);
	virtual bool IsShowingCursor(void);
	virtual int KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding);
	virtual int getBackgroundMode(void);
	virtual void setBackgroundMode(int BackgroundMode);
	virtual void OnClose(void);

	void SetNextMenu(CMenuPanel *pNextPanel);
	inline int ShouldBeRemoved(void) { return m_iRemoveMe; }
	inline void SetMenuID(int iID) { m_iMenuID = iID; }
	inline void SetActive(int iState) { m_iIsActive = iState; }
	inline CMenuPanel *GetNextMenu(void) { return m_pNextMenu; }
	inline int GetMenuID(void) { return m_iMenuID; }
	inline int IsActive(void) { return m_iIsActive; }
	inline float GetOpenTime(void) { return m_flOpenTime; }

	// Numeric input
	virtual bool SlotInput(int iSlot) { return false; }
	virtual void SetActiveInfo(int iInput) {}

protected:
	CImageLabel		m_TitleIcon;// TODO: button
//	Label			m_LabelTitle;
	CMenuPanel *m_pNextMenu;
	int			m_iMenuID;
	int			m_iRemoveMe;
	int			m_iIsActive;
	int			m_iBackgroundMode;// BG_FILL_NONE 0 - none, 1 - normal, 2 - fullscreen
	float		m_flOpenTime;
	bool		m_bCaptureInput;
	bool		m_bShowCursor;
};


// Simple int signal handler
class CMenuPanelActionSignalHandler : public ActionSignal
{
	typedef ActionSignal BaseClass;
public:
	CMenuPanelActionSignalHandler(CMenuPanel *pPanel, int signal);
	virtual void actionPerformed(Panel *panel);

private:
	CMenuPanel *m_pPanel;
	int m_iSignal;
};


enum
{
	MB_OK = 0,
	MB_OKCANCEL,
	MB_ABORTRETRYIGNORE,
	MB_YESNOCANCEL,
	MB_YESNO,
	MB_RETRYCANCEL
};

enum
{
	IDOK = 1,
	IDCANCEL,
	IDABORT,
	IDRETRY,
	IDIGNORE,
	IDYES,
	IDNO,
	IDCLOSE,
	IDHELP
};

class CDialogPanel : public CMenuPanel
{
	typedef CMenuPanel BaseClass;
public:
	CDialogPanel(int RemoveMe, int x, int y, int wide, int tall, unsigned long type);
	virtual void Reset(void);
	virtual void Close(void);

	virtual void OnClose(void);
	virtual void OnActionSignal(int signal);
	virtual void OnOK(void);
	virtual void OnCancel(void);
	virtual bool DoExecCommand(void);

	unsigned long GetResult(void) { return m_iSignalRecieved; }

	CommandButton *m_pButtons;
	unsigned long m_iNumDialogButtons;
	unsigned long m_iSignalRecieved;
};




extern CViewport *gViewPort;

#endif // XDMVIEWPORT_H
