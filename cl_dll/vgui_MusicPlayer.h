#ifndef VGUI_MUSICPLAYER_H
#define VGUI_MUSICPLAYER_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include <VGUI_LineBorder.h>
#include <VGUI_IntChangeSignal.h>
#include "VGUI_ScrollBar2.h"
#include "VGUI_Slider2.h"

#define MPLAYER_PANEL_SIZEX			240
#define MPLAYER_PANEL_SIZEY			100
#define MPLAYER_WINDOW_MARGIN		2
#define MPLAYER_TIMEWINDOW_WIDTH	40
#define MPLAYER_TIMEWINDOW_HEIGHT	24
#define MPLAYER_TITLE_HEIGHT		16
#define MPLAYER_VOLSLIDER_HEIGHT	8
#define MPLAYER_SEEKBAR_HEIGHT		12
#define BUTTON_SIZEX				20
#define BUTTON_SIZEY				20
#define BUTTON_INTERVAL				2
#define BUTTON_BOTTOM_MARGIN		2

#define MPLAYER_SEEKBAR_VALMIN		0
#define MPLAYER_SEEKBAR_VALMAX		100

typedef enum
{
	MPCMD_CLOSE = 0,
	MPCMD_PREV,
	MPCMD_PLAY,
	MPCMD_PAUSE,
	MPCMD_STOP,
	MPCMD_NEXT,
	MPCMD_LOAD,
	MPCMD_SEEK,
	MPCMD_REPEAT,
	MPCMD_SHUFFLE,
} mplayer_cmd;

typedef enum
{
	MPSI_SEEK = 0,
	MPSI_VOLUME,
	MPSI_BALANCE,
} mplayer_sliders;

class CMusicPlayerHandler_Command : public CMenuActionHandler
{
	typedef CMenuActionHandler BaseClass;
public:
	CMusicPlayerHandler_Command(CMusicPlayerPanel *panel, mplayer_cmd cmd);
	virtual void actionPerformed(Panel *panel);

protected:
	CMusicPlayerPanel *m_pPanel;
	mplayer_cmd m_iCmd;
};


class CMusicPlayerHandler_Slider : public IntChangeSignal
{
	typedef IntChangeSignal BaseClass;
public:
	CMusicPlayerHandler_Slider(CMusicPlayerPanel *panel, mplayer_sliders slider);
	virtual void intChanged(int value, Panel *panel);
	virtual void SetEnabled(bool enable);

protected:
	CMusicPlayerPanel *m_pPanel;
	mplayer_sliders m_usSlider;
	bool m_bEnabled;
};


class CMusicPlayerInputHandler : public CDefaultInputSignal
{
	typedef CDefaultInputSignal BaseClass;
public:
	CMusicPlayerInputHandler(CMusicPlayerPanel *panel, mplayer_cmd cmd);

	virtual void mousePressed(MouseCode code, Panel *panel);
	virtual void mouseReleased(MouseCode code, Panel *panel);

protected:
	CMusicPlayerPanel *m_pPanel;
	mplayer_cmd m_iCmd;
	bool m_bMousePressed;
};



class CMusicPlayerPanel : public CMenuPanel
{
	typedef CMenuPanel BaseClass;
public:
	CMusicPlayerPanel(int iRemoveMe, int x, int y, int wide, int tall);
	virtual void Open(void);
	virtual void Close(void);
	virtual int KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding);
/*
	virtual void internalCursorMoved(int x,int y);
	virtual void internalCursorEntered();
	virtual void internalCursorExited();
	virtual void internalMousePressed(MouseCode code);
	virtual void internalMouseReleased(MouseCode code);
*/
	void Initialize(void);
	void Update(void);
	void ActionSignal(mplayer_cmd cmd);
	void SliderSignal(mplayer_sliders slider, int value);
//	int	KeyInput(int down, int keynum, const char *pszCurrentBinding);
//DNW	virtual void internalKeyTyped(KeyCode code);
	void SetTrackInfo(const char *name);

	int m_iLoopMode;

private:
	CommandButton		*m_pButtonPrev;
	CommandButton		*m_pButtonPlay;
	CommandButton		*m_pButtonPause;
	CommandButton		*m_pButtonStop;
	CommandButton		*m_pButtonNext;
	CommandButton		*m_pButtonLoad;
	CommandButton		*m_pButtonClose;
	ToggleCommandButton	*m_pButtonRepeat;
	Label				*m_pTrackTitle;
	Label				*m_pTrackTime;
	ScrollBar2			*m_pSeekBar;
	Slider2				*m_pSliderVolume;
//	Slider				*m_pSliderBalance;
//	bool	m_bDisableSeekAutoUpdate;

//	CMusicPlayerInputHandler *m_pSliderInputHandler;
	CMusicPlayerHandler_Slider *m_pSeekChangeHandler;
/*
	bool m_MouseInside;
	bool m_MousePressed;
	int m_iMouseLastX;
	int m_iMouseLastY;
	*/
};


#endif // VGUI_MUSICPLAYER_H
