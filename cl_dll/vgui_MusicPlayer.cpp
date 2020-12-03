#include "hud.h"
#include "cl_util.h"
#include "vgui_XDMViewport.h"
#include "vgui_MusicPlayer.h"
#include "musicplayer.h"
#include "../engine/keydefs.h"

extern cvar_t *bgm_defplaylist;

// BUGBUG: what's wrong with the volume slider?!
CMusicPlayerPanel::CMusicPlayerPanel(int iRemoveMe, int x, int y, int wide, int tall) : CMenuPanel(iRemoveMe, x,y,wide,tall)
{
	m_iLoopMode = 0;

	int r, g, b, a;
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle("Title Font");
	SchemeHandle_t hButtonScheme = pSchemes->getSchemeHandle("Primary Button Text");
	Font *pButtonFont = pSchemes->getFont(hTitleScheme);

	setBgColor(0,0,0,PANEL_DEFAULT_ALPHA);
	setBorder(new CDefaultLineBorder());
	setPaintBorderEnabled(true);
	setCaptureInput(true);
	m_bShowCursor = true;
 
	m_pTrackTitle = new Label("----", XRES(MPLAYER_WINDOW_MARGIN), YRES(MPLAYER_WINDOW_MARGIN), wide-XRES(MPLAYER_WINDOW_MARGIN*2 + BUTTON_SIZEX), YRES(MPLAYER_TITLE_HEIGHT));
	m_pTrackTitle->setParent(this);
	m_pTrackTitle->setFont(pSchemes->getFont(hButtonScheme));// lesser font for long names hTitleScheme));
	m_pTrackTitle->setContentAlignment(Label::a_east);
	pSchemes->getFgColor(hTitleScheme, r, g, b, a);
	m_pTrackTitle->setFgColor(r, g, b, a);
	pSchemes->getBgColor(hTitleScheme, r, g, b, a);
	m_pTrackTitle->setBgColor(r, g, b, a);
//	m_pTrackTitle->setAsMouseArena(false);
//	m_pTrackTitle->setAsMouseCapture(false);

	m_pTrackTime = new Label("--:--", XRES(MPLAYER_WINDOW_MARGIN), m_pTrackTitle->getTall()+YRES(MPLAYER_WINDOW_MARGIN+2), XRES(MPLAYER_TIMEWINDOW_WIDTH), YRES(MPLAYER_TIMEWINDOW_HEIGHT));
	m_pTrackTime->setParent(this);
	m_pTrackTime->setFont(pSchemes->getFont(hTitleScheme));
	m_pTrackTime->setContentAlignment(Label::a_west);
	m_pTrackTime->setFgColor(Scheme::sc_primary1);

	m_pSliderVolume = new Slider2(m_pTrackTime->getWide()+XRES(MPLAYER_WINDOW_MARGIN), m_pTrackTitle->getTall()+YRES(MPLAYER_WINDOW_MARGIN+2), XRES(64), YRES(MPLAYER_VOLSLIDER_HEIGHT), false);
	m_pSliderVolume->setRange(0, 255);
	m_pSliderVolume->setValue(255);
	m_pSliderVolume->setPaintBorderEnabled(true);
	m_pSliderVolume->setPaintBackgroundEnabled(true);
	m_pSliderVolume->setPaintEnabled(true);
	m_pSliderVolume->addIntChangeSignal(new CMusicPlayerHandler_Slider(this, MPSI_VOLUME));

	m_pSeekBar = new ScrollBar2(XRES(MPLAYER_WINDOW_MARGIN), tall/2, wide-XRES(MPLAYER_WINDOW_MARGIN)*2, YRES(MPLAYER_SEEKBAR_HEIGHT), false);
	m_pSeekBar->setParent(this);
//	m_pSeekBar->setButton(new CCustomScrollButton(ARROW_LEFT, "<", 0,0, 16,16), 0);
//	m_pSeekBar->setButton(new CCustomScrollButton(ARROW_RIGHT, ">", 0,0, 16,16), 1);
//	m_pSeekBar->setSlider(new Slider(0,0,8,32,false));
	Slider2 *pSlider = m_pSeekBar->getSlider();
	if (pSlider)
	{
		pSlider->setValue(0);
//		pSlider->setRange(MPLAYER_SEEKBAR_VALMIN, MPLAYER_SEEKBAR_VALMAX);
		pSlider->setMinimumSize(XRES(4), m_pSeekBar->getTall());
		pSlider->addInputSignal(new CMusicPlayerInputHandler(this, MPCMD_SEEK));
	}
	m_pSeekChangeHandler = new CMusicPlayerHandler_Slider(this, MPSI_SEEK);
	m_pSeekBar->setRange(MPLAYER_SEEKBAR_VALMIN, MPLAYER_SEEKBAR_VALMAX);
	m_pSeekBar->setButtonPressedScrollValue(10);// +- 10
	m_pSeekBar->setPaintBorderEnabled(true);
//	m_pSeekBar->setPaintBackgroundEnabled(true);
	m_pSeekBar->setPaintEnabled(true);
	m_pSeekBar->setRangeWindow(10);
	m_pSeekBar->setRangeWindowEnabled(false);
	m_pSeekBar->addIntChangeSignal(m_pSeekChangeHandler);
	m_pSeekBar->validate();

	m_pButtonClose = new CommandButton("X", wide - XRES(BUTTON_SIZEX + MPLAYER_WINDOW_MARGIN), YRES(MPLAYER_WINDOW_MARGIN), XRES(BUTTON_SIZEX), YRES(BUTTON_SIZEY));
	m_pButtonClose->setFont(pButtonFont);
	m_pButtonClose->setContentAlignment(Label::a_center);
	m_pButtonClose->setParent(this);
	m_pButtonClose->addActionSignal(new CMusicPlayerHandler_Command(this, MPCMD_CLOSE));
//	m_pButtonClose->m_bShowHotKey = false;
	m_pButtonClose->setBoundKey((char)255);

	int posX = XRES(16);
	int posY = tall - YRES(BUTTON_SIZEY + BUTTON_BOTTOM_MARGIN + MPLAYER_WINDOW_MARGIN);

	m_pButtonPrev = new CommandButton("<|", posX, posY, XRES(BUTTON_SIZEX), YRES(BUTTON_SIZEY), false, false, false, "resource/icon_start.tga");
	m_pButtonPrev->setFont(pButtonFont);
	m_pButtonPrev->setContentAlignment(Label::a_center);
	m_pButtonPrev->setParent(this);
	m_pButtonPrev->addActionSignal(new CMusicPlayerHandler_Command(this, MPCMD_PREV));
//	m_pButtonPrev->m_bShowHotKey = false;
	m_pButtonPrev->setBoundKey('z');// these do nothing by themselves
	posX += XRES(BUTTON_SIZEX+BUTTON_INTERVAL);

	m_pButtonPlay = new CommandButton("|>", posX, posY, XRES(BUTTON_SIZEX), YRES(BUTTON_SIZEY), false, false, false, "resource/icon_play.tga");
	m_pButtonPlay->setFont(pButtonFont);
	m_pButtonPlay->setContentAlignment(Label::a_center);
	m_pButtonPlay->setParent(this);
	m_pButtonPlay->addActionSignal(new CMusicPlayerHandler_Command(this, MPCMD_PLAY));
//	m_pButtonPlay->m_bShowHotKey = false;
	m_pButtonPlay->setBoundKey('x');
	posX += XRES(BUTTON_SIZEX+BUTTON_INTERVAL);

	m_pButtonPause = new CommandButton("||", posX, posY, XRES(BUTTON_SIZEX), YRES(BUTTON_SIZEY), false, false, false, "resource/icon_pause.tga");
	m_pButtonPause->setFont(pButtonFont);
	m_pButtonPause->setContentAlignment(Label::a_center);
	m_pButtonPause->setParent(this);
	m_pButtonPause->addActionSignal(new CMusicPlayerHandler_Command(this, MPCMD_PAUSE));
//	m_pButtonPause->m_bShowHotKey = false;
	m_pButtonPause->setBoundKey('c');
	posX += XRES(BUTTON_SIZEX+BUTTON_INTERVAL);

	m_pButtonStop = new CommandButton("[]", posX, posY, XRES(BUTTON_SIZEX), YRES(BUTTON_SIZEY), false, false, false, "resource/icon_stop.tga");
	m_pButtonStop->setFont(pButtonFont);
	m_pButtonStop->setContentAlignment(Label::a_center);
	m_pButtonStop->setParent(this);
	m_pButtonStop->addActionSignal(new CMusicPlayerHandler_Command(this, MPCMD_STOP));
//	m_pButtonStop->m_bShowHotKey = false;
	m_pButtonStop->setBoundKey('v');
	posX += XRES(BUTTON_SIZEX+BUTTON_INTERVAL);

	m_pButtonNext = new CommandButton("|>", posX, posY, XRES(BUTTON_SIZEX), YRES(BUTTON_SIZEY), false, false, false, "resource/icon_end.tga");
	m_pButtonNext->setFont(pButtonFont);
	m_pButtonNext->setContentAlignment(Label::a_center);
	m_pButtonNext->setParent(this);
	m_pButtonNext->addActionSignal(new CMusicPlayerHandler_Command(this, MPCMD_NEXT));
//	m_pButtonNext->m_bShowHotKey = false;
	m_pButtonNext->setBoundKey('b');
	posX += XRES(BUTTON_SIZEX+BUTTON_INTERVAL*2);

	m_pButtonLoad = new CommandButton("^", posX, posY, XRES(BUTTON_SIZEX), YRES(BUTTON_SIZEY), false, false, false, "resource/icon_load.tga");
	m_pButtonLoad->setFont(pButtonFont);
	m_pButtonLoad->setContentAlignment(Label::a_center);
	m_pButtonLoad->setParent(this);
	m_pButtonLoad->addActionSignal(new CMusicPlayerHandler_Command(this, MPCMD_LOAD));
//	m_pButtonLoad->m_bShowHotKey = false;
	m_pButtonLoad->setBoundKey('l');

	posX = wide-XRES(16)-XRES(BUTTON_SIZEX);
	m_pButtonRepeat = new ToggleCommandButton(bgm_pls_loop, "@", posX, posY, XRES(BUTTON_SIZEX), YRES(BUTTON_SIZEY), false, false);
	m_pButtonRepeat->setFont(pButtonFont);
//	m_pButtonRepeat->setContentAlignment(Label::a_center);
	m_pButtonRepeat->setParent(this);
//	m_pButtonRepeat->m_bShowHotKey = false;
	m_pButtonRepeat->setBoundKey('r');
	m_pButtonRepeat->addActionSignal(new CMenuHandler_ToggleCvar(NULL, bgm_pls_loop));

	setDragEnabled(true);
//	m_bDisableSeekAutoUpdate = false;
}

void CMusicPlayerPanel::Initialize(void)
{
	setVisible(false);
	if (m_pSeekBar)
		m_pSeekBar->setValue(0);
	if (m_pSliderVolume)
		m_pSliderVolume->setValue(255);
	m_iLoopMode = 0;
	// TODO: Clear out all the values here
}

void CMusicPlayerPanel::Update(void)
{
	//if (m_bDisableSeekAutoUpdate == false)
	if (!m_pSeekBar->isMouseDown(MOUSE_LEFT) && BGM_IsPlaying())
	{
		m_pSeekChangeHandler->SetEnabled(false);// HACK!?
		m_pSeekBar->setValue(BGM_GetPosition());
		m_pTrackTitle->setText(BGM_PLS_GetTrackName());
		m_pTrackTime->setText("%.2f", (float)BGM_GetTimeMs()/1000.0f);
		m_pSeekChangeHandler->SetEnabled(true);
	}

	if (isVisible())
	{
		m_pTrackTime->repaint();
		m_pSeekBar->validate();
		m_pSeekBar->repaint();
		m_pTrackTitle->repaint();
	}
}

void CMusicPlayerPanel::SetTrackInfo(const char *name)
{
	m_pTrackTitle->setText(name);
}

void CMusicPlayerPanel::Open(void)
{
	if (!BGM_Initialized())
	{
		PlaySound("vgui/menu_close.wav", VOL_NORM);
		return;
	}
	PlaySound("vgui/menu_activate.wav", VOL_NORM);
	m_pSeekBar->setEnabled(true);
	CMenuPanel::Open();
	Update();
//	m_pTrackTitle->repaint();
}

void CMusicPlayerPanel::Close(void)
{
	m_pSeekBar->setEnabled(false);
	PlaySound("vgui/menu_close.wav", VOL_NORM);
	CMenuPanel::Close();
}

void CMusicPlayerPanel::ActionSignal(mplayer_cmd cmd)
{
	switch (cmd)
	{
	case MPCMD_CLOSE:
		{
			Close();// don't execute any code after this
			return;
		}
		break;
	case MPCMD_PREV:	BGM_PLS_Prev(); break;
	case MPCMD_PLAY:	BGM_PLS_Play(-1, -1); break;// XDM3035
	case MPCMD_PAUSE:	BGM_Pause(); break;
	case MPCMD_STOP:	BGM_Stop(); break;
	case MPCMD_NEXT:	BGM_PLS_Next(); break;
	case MPCMD_LOAD:
		{
			if (bgm_defplaylist && bgm_defplaylist->string)
			{
				if (BGM_PLS_Load(bgm_defplaylist->string) > 0)// XDM3035
				{
					//BGM_PLS_SetLoopMode(m_iLoopMode);
					BGM_PLS_Play(0, m_iLoopMode);// is autoplay nescessary?
				}
			}
		}
		break;
	case MPCMD_SEEK:
		{
			BGM_SetPosition((float)m_pSeekBar->getValue());
//			m_bDisableSeekAutoUpdate = false;
		}
		break;
	}
	m_pTrackTitle->setText(BGM_PLS_GetTrackName());
	Update();
}

void CMusicPlayerPanel::SliderSignal(mplayer_sliders slider, int value)
{
	switch (slider)
	{
	case MPSI_SEEK:
		{
//			m_bDisableSeekAutoUpdate = true;
			// TODO: if (mousereleased)
//NO!			BGM_SetPosition(value);// 0-100
			//m_pTrackTime->setText("%.2f", ((float)BGM_GetLengthMs()/1000.0f)*(value/(MPLAYER_SEEKBAR_VALMAX-MPLAYER_SEEKBAR_VALMIN)));
			m_pTrackTime->setText("%d%%", value);
			PlaySound("vgui/slider_move.wav", VOL_NORM);
		}
		break;
	case MPSI_VOLUME:	BGM_SetVolume(value); break;// 0-255
	case MPSI_BALANCE:	BGM_SetBalance(value); break;// 0-255
	}
}

//-----------------------------------------------------------------------------
// Purpose: Catch hotkeys
// Input  : &down - 
//			&keynum - 
//			*pszCurrentBinding - 
// Output : int - 0: handled, 1: unhandled, allow other panels to recieve key
//-----------------------------------------------------------------------------
int CMusicPlayerPanel::KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding)
{
	if (down)
	{
		if (/*keynum == K_ENTER || keynum == K_KP_ENTER || */keynum == K_SPACE || keynum == K_ESCAPE)
		{
//			m_pButtonClose->fireActionSignal();
//			ActionSignal(MPCMD_CLOSE);
			Close();
			return 0;
		}
		else if (keynum == m_pButtonPrev->getBoundKey())
		{
			m_pButtonPrev->fireActionSignal();
//			ActionSignal(MPCMD_PREV);
			return 0;
		}
		else if (keynum == m_pButtonPlay->getBoundKey())
		{
			m_pButtonPlay->fireActionSignal();
//			ActionSignal(MPCMD_PLAY);
			return 0;
		}
		else if (keynum == m_pButtonPause->getBoundKey())
		{
			m_pButtonPause->fireActionSignal();
//			ActionSignal(MPCMD_PAUSE);
			return 0;
		}
		else if (keynum == m_pButtonStop->getBoundKey())
		{
			m_pButtonStop->fireActionSignal();
//			ActionSignal(MPCMD_STOP);
			return 0;
		}
		else if (keynum == m_pButtonNext->getBoundKey())
		{
			m_pButtonNext->fireActionSignal();
//			ActionSignal(MPCMD_NEXT);
			return 0;
		}
		else if (keynum == m_pButtonLoad->getBoundKey())
		{
			m_pButtonLoad->fireActionSignal();
//			ActionSignal(MPCMD_LOAD);
			return 0;
		}
	}
	return 1;
}




CMusicPlayerHandler_Command::CMusicPlayerHandler_Command(CMusicPlayerPanel *panel, mplayer_cmd cmd) : CMenuActionHandler()
{
	m_pPanel = panel;
	m_iCmd = cmd;
}

void CMusicPlayerHandler_Command::actionPerformed(Panel *panel)
{
	if (m_pPanel)
		m_pPanel->ActionSignal(m_iCmd);
	CMenuActionHandler::actionPerformed(panel);
}


CMusicPlayerHandler_Slider::CMusicPlayerHandler_Slider(CMusicPlayerPanel *panel, mplayer_sliders slider)
{
	m_pPanel = panel;
	m_usSlider = slider;
	SetEnabled(true);
}

void CMusicPlayerHandler_Slider::intChanged(int value, Panel *panel)
{
	if (m_pPanel && m_bEnabled)
		m_pPanel->SliderSignal(m_usSlider, value);
}

void CMusicPlayerHandler_Slider::SetEnabled(bool enable)
{
	m_bEnabled = enable;
}


CMusicPlayerInputHandler::CMusicPlayerInputHandler(CMusicPlayerPanel *panel, mplayer_cmd cmd) : CDefaultInputSignal()
{
	m_pPanel = panel;
	m_iCmd = cmd;
}

void CMusicPlayerInputHandler::mousePressed(MouseCode code, Panel *panel)
{
	m_bMousePressed = true;
}

void CMusicPlayerInputHandler::mouseReleased(MouseCode code, Panel *panel)
{
	if (m_pPanel && m_bMousePressed)
	{
		m_pPanel->ActionSignal(m_iCmd);
	}
	m_bMousePressed = false;
}
