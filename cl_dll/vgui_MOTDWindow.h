#ifndef VGUI_MOTDWINDOW_H
#define VGUI_MOTDWINDOW_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#define MOTD_WINDOW_X			112
#define MOTD_WINDOW_Y			80
#define MOTD_WINDOW_SIZE_X		480// in 640x480 space XRES
#define MOTD_WINDOW_SIZE_Y		360

#define MOTD_BORDER				16

//-----------------------------------------------------------------------------
// Displays the MOTD and basic server information
//-----------------------------------------------------------------------------
class CMessageWindowPanel : public CMenuPanel
{
public:
	CMessageWindowPanel(const char *szMOTD, const char *szTitle, bool ShadeFullScreen, int iRemoveMe, int x, int y, int wide, int tall);
	virtual void Open(void);// XDM
	virtual void Close(void);
	virtual int KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding);

private:
//	CTransparentPanel *m_pBackgroundPanel;
	Label			*m_pLabelTitle;
	ScrollPanel		*m_pScrollPanel;
	TextPanel		*m_pTextPanel;
	CommandButton	*m_pButtonClose;
};

#endif // VGUI_MOTDWINDOW_H
