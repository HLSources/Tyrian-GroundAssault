//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// vgui_SpectatorPanel.h: interface for the SpectatorPanel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SPECTATORPANEL_H
#define SPECTATORPANEL_H

#include <VGUI_Panel.h>
#include <VGUI_Label.h>
#include <VGUI_Button.h>

using namespace vgui;

#define SPECTATOR_PANEL_CMD_NONE				0
#define SPECTATOR_PANEL_CMD_OPTIONS				1
#define	SPECTATOR_PANEL_CMD_PREVPLAYER			2
#define SPECTATOR_PANEL_CMD_NEXTPLAYER			3
#define	SPECTATOR_PANEL_CMD_HIDEMENU			4
#define	SPECTATOR_PANEL_CMD_TOGGLE_INSET		5
#define SPECTATOR_PANEL_CMD_CAMERA				6

#define SPECTATOR_PANEL_HELP_DISPLAY_TIME		5

const char *GetSpectatorLabel(int iMode);


class SpectatorPanel : public CMenuPanel//Panel //, public vgui::CDefaultInputSignal
{
	typedef CMenuPanel BaseClass;
public:
	SpectatorPanel(int x,int y,int wide,int tall);
	virtual ~SpectatorPanel();

	virtual void Close(void);
	void ActionSignal(int cmd);
	// InputSignal overrides.
	void Initialize(void);
	void Update(void);
	void EnableInsetView(bool isEnabled);
	void ShowMenu(bool bVisible);
	void SetBanner(char *image);

	bool			m_menuVisible;
	bool			m_insetVisible;

protected:
	CommandButton *m_OptionButton;
	CommandButton *m_PrevPlayerButton;
	CommandButton *m_NextPlayerButton;
	CommandButton *m_CamButton;	
	CommandButton *m_InsetViewButton;

	Panel			*m_TopBorder;
	Panel			*m_BottomBorder;
	Label			*m_TopMainLabel;// XDM
	Label			*m_BottomMainLabel;
	CImageLabel		*m_TimerImage;
	Label			*m_CurrentTime;
	Label			*m_ExtraInfo;
//	Panel			*m_Separator;
	Label			*m_TeamScores[MAX_TEAMS];
	CImageLabel		*m_TopBanner;
	float			m_fHideHelpTime;
};


class CSpectatorHandler_Command : public ActionSignal
{
	typedef ActionSignal BaseClass;
private:
	SpectatorPanel *m_pFather;
	int				m_cmd;

public:
	CSpectatorHandler_Command(SpectatorPanel *panel, int cmd)
	{
		m_pFather = panel;
		m_cmd = cmd;
	}

	virtual void actionPerformed(Panel *panel)
	{
		m_pFather->ActionSignal(m_cmd);
	}
};

#endif // !defined SPECTATORPANEL_H
