#ifndef VGUI_TEAMMENU_H
#define VGUI_TEAMMENU_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

// Team Menu Dimensions
#define TEAMMENU_TITLE_X				XRES(40)
#define TEAMMENU_TITLE_Y				YRES(32)
#define TEAMMENU_TOPLEFT_BUTTON_X		XRES(40)
#define TEAMMENU_TOPLEFT_BUTTON_Y		YRES(80)
#define TEAMMENU_BUTTON_SIZE_X			XRES(124)
#define TEAMMENU_BUTTON_SIZE_Y			YRES(24)
#define TEAMMENU_BUTTON_SPACER_Y		YRES(8)
#define TEAMMENU_WINDOW_X				XRES(176)
#define TEAMMENU_WINDOW_Y				YRES(80)
#define TEAMMENU_WINDOW_SIZE_X			XRES(424)
#define TEAMMENU_WINDOW_SIZE_Y			YRES(312)
#define TEAMMENU_WINDOW_TITLE_X			XRES(16)
#define TEAMMENU_WINDOW_TITLE_Y			YRES(16)
#define TEAMMENU_WINDOW_TEXT_X			XRES(16)
#define TEAMMENU_WINDOW_TEXT_Y			YRES(48)
#define TEAMMENU_WINDOW_TEXT_SIZE_Y		YRES(178)
#define TEAMMENU_WINDOW_INFO_X			XRES(16)
#define TEAMMENU_WINDOW_INFO_Y			YRES(234)

#define NUM_TEAM_BUTTONS				MAX_TEAMS + 1// XDM: +auto
#define	TBUTTON_AUTO					0// XDM
#define	TBUTTON_SPECTATE				NUM_TEAM_BUTTONS// last one

class CTeamMenuPanel : public CMenuPanel
{
	typedef CMenuPanel BaseClass;
public:
	CTeamMenuPanel(int iRemoveMe, int x, int y, int wide, int tall);

	virtual bool SlotInput(int iSlot);
	virtual void Open(void);
	virtual void Update(void);
	virtual void SetActiveInfo(int iInput);
	virtual void paintBackground(void);
	virtual void Initialize(void);
	virtual void Reset(void);

protected:
	ScrollPanel         *m_pScrollPanel;
	Panel				*m_pTeamWindow;
	Label				*m_pLabelTitle;
	Label				*m_pMapTitle;
	TextPanel			*m_pBriefing;
	TextPanel			*m_pTeamInfoPanel[NUM_TEAM_BUTTONS];
	CommandButton		*m_pButtons[NUM_TEAM_BUTTONS];
	CommandButton		*m_pCancelButton;
	CommandButton		*m_pSpectateButton;

	bool				m_bUpdatedMapName;
	int					m_iCurrentInfo;
};

#endif // VGUI_TEAMMENU_H
