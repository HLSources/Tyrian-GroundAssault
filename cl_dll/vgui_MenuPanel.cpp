#include "hud.h"
#include "cl_util.h"
#include "vgui_XDMViewport.h"

//-----------------------------------------------------------------------------
// Purpose: UNSONE: Panel that can easily apply scheme to controls
// Input  : *pScheme - 
//			x y - 
//			wide tall - 
//-----------------------------------------------------------------------------
CSchemeEnabledPanel::CSchemeEnabledPanel(CScheme *pScheme, int x, int y, int wide, int tall) : Panel(x,y,wide,tall)
{
	setScheme(pScheme);
}

void CSchemeEnabledPanel::paintBackground(void)
{
	Panel::paintBackground();
}

CScheme *CSchemeEnabledPanel::getScheme(void)
{
	return m_pScheme;
}

void CSchemeEnabledPanel::setScheme(CScheme *pScheme)
{
	m_pScheme = pScheme;
}

void CSchemeEnabledPanel::ApplyScheme(CScheme *pScheme, Panel *pControl)
{
//	isArmed
	if (pScheme && pControl)
	{
		pControl->setBgColor(pScheme->BgColor.r, pScheme->BgColor.g, pScheme->BgColor.b, pScheme->BgColor.a);
		pControl->setFgColor(pScheme->FgColor.r, pScheme->FgColor.g, pScheme->FgColor.b, pScheme->FgColor.a);
	}
}

//-----------------------------------------------------------------------------
// Purpose: DragNDropPanel
// Input  : x y - 
//			wide tall - 
// WARNING: do not use setPos(x,y) or setBounds(x, y, wide, tall) on self in subclasses!
//-----------------------------------------------------------------------------
DragNDropPanel::DragNDropPanel(bool dragenabled, int x, int y, int wide, int tall) : Panel(x,y,wide,tall)
{
	if (x < 0)
		x = ScreenWidth/2 - wide/2;
	if (y < 0)
		y = ScreenHeight/2 - tall/2;

	setPos(x,y);
//we only changed position	setBounds(x, y, wide, tall);

	m_OriginalPos[0] = x;
	m_OriginalPos[1] = y;
	m_bBeingDragged = false;
	m_bDragEnabled = dragenabled;
	addInputSignal(new CDragNDropHandler(this));// Create the Drag Handler

/*	m_pOriginalBorder = NULL;
	m_pDragBorder = new LineBorder();// Create the border (for dragging)

	if (gViewPort)
	{
		CSchemeManager *pSchemeMgr = gViewPort->GetSchemeManager();
		if (pSchemeMgr)
		{
			CScheme *pScheme = pSchemeMgr->getSafeScheme(pSchemeMgr->getSchemeHandle("Basic Text"));
			if (pScheme)
				m_pDragBorder->setLineColor(pScheme->FgColorArmed[0], pScheme->FgColorArmed[1], pScheme->FgColorArmed[2], pScheme->BorderColor[3]);
		}
	}*/
}

//-----------------------------------------------------------------------------
// Purpose: Don't allow setting position outside the screen
// Input  : x y - 
//-----------------------------------------------------------------------------
void DragNDropPanel::setPos(int x, int y)
{
	if (x < 0)
	{
		x = 0;
	}
	else
	{
		int mx = ScreenWidth - getWide();
		if (x > mx)
			x = mx;
	}

	if (y < 0)
	{
		y = 0;
	}
	else
	{
		int my = ScreenHeight - getTall();
		if (y > my)
			y = my;
	}

	Panel::setPos(x, y);
}

//-----------------------------------------------------------------------------
// Purpose: Called by signal
// Input  : bState - 
//-----------------------------------------------------------------------------
void DragNDropPanel::setDragged(bool bState)
{
	if (m_bDragEnabled)
	{
/*		if (bState)// change broder disregarding _paintBorderEnabled
		{
			m_pOriginalBorder = _border;// save old border
			setBorder(m_pDragBorder);// BUGBUG: this function deletes old instance
		}	
		else
		{
			setBorder(m_pOriginalBorder);
			m_pOriginalBorder = NULL;// in case it will be deleted by the window itself
		}*/
		m_bBeingDragged = bState;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Enable or disable DragNDrop functionality
// Input  : enable - 
//-----------------------------------------------------------------------------
void DragNDropPanel::setDragEnabled(bool enable)
{
	m_bDragEnabled = enable;
}

//-----------------------------------------------------------------------------
// Purpose: is DragNDrop functionality enabled?
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool DragNDropPanel::getDragEnabled(void)
{
	return m_bDragEnabled;
}



//-----------------------------------------------------------------------------
// Purpose: Menu Panel that supports buffering of menus and basic action signals
// Input  : iRemoveMe - delete this when closed
//			x y - position
//			wide - 
//			tall - 
// Note   : All internal controls use RELATIVE coordinates, where (0,0) is panel's top-left corner!
//-----------------------------------------------------------------------------
CMenuPanel::CMenuPanel(int RemoveMe, int x, int y, int wide, int tall) : DragNDropPanel(false, x,y,wide,tall)//CTransparentPanel(BG_FILL_NORMAL, 127, x,y,wide,tall)
{
	m_TitleIcon.setParent(this);
	m_TitleIcon.LoadImage("resource/icon_xhl16.tga", true);
	m_TitleIcon.setPos(PANEL_INNER_OFFSET, PANEL_INNER_OFFSET);
	m_TitleIcon.setContentAlignment(vgui::Label::a_center);
	m_TitleIcon.setPaintBackgroundEnabled(false);

	setBackgroundMode(BG_FILL_NORMAL);
	Reset();
	m_iRemoveMe = RemoveMe;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMenuPanel::Open(void)
{
	setVisible(true);
	// Note the open time, so we can delay input for a bit
	m_flOpenTime = gHUD.m_flTime;

//	PlaySound("vgui/window2.wav", VOL_NORM);// XDM
}

//-----------------------------------------------------------------------------
// Purpose: Close
//-----------------------------------------------------------------------------
void CMenuPanel::Close(void)
{
	setVisible(false);
	this->OnClose();
	m_iIsActive = false;

//	PlaySound("vgui/menu_close.wav", VOL_NORM);// XDM
	gViewPort->UpdateCursorState();// XDM3035c

/*	return */gViewPort->OnMenuPanelClose(this); //removeChild(this);
	// This MenuPanel has now been deleted. Don't append code here.
}

//-----------------------------------------------------------------------------
// Purpose: Reset
//-----------------------------------------------------------------------------
void CMenuPanel::Reset(void)
{
	m_pNextMenu = NULL;
	m_iIsActive = false;
	m_flOpenTime = 0;
}

//-----------------------------------------------------------------------------
// Purpose: HACK! Overridden to identify self
// Input  : *buf - 
//			bufLen - 
//-----------------------------------------------------------------------------
void CMenuPanel::getPersistanceText(char *buf, int bufLen)
{
	if (buf)
		strncpy(buf, "CMenuPanel", bufLen);

//	Panel::getPersistanceText() returns "->SetBounds(0,0,123,123)" - WTF?!
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CMenuPanel::paintBackground(void)
{
	if (getBackgroundMode() == BG_FILL_NORMAL)
		BaseClass::paintBackground();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : signal - 
//-----------------------------------------------------------------------------
void CMenuPanel::OnActionSignal(int signal)
{
}

//-----------------------------------------------------------------------------
// Purpose: capture use input when active and visible
// Input  : enable - 
//-----------------------------------------------------------------------------
void CMenuPanel::setCaptureInput(bool enable)
{
	m_bCaptureInput = enable;
}

//-----------------------------------------------------------------------------
// Purpose: show mouse cursor when active and visible
// Input  : show - 
//-----------------------------------------------------------------------------
void CMenuPanel::setShowCursor(bool show)
{
	m_bShowCursor = show;
}

//-----------------------------------------------------------------------------
// Purpose: Indicates that this panel should capture all keyboard and mouse input
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CMenuPanel::IsCapturingInput(void)
{
	return m_bCaptureInput;
}

//-----------------------------------------------------------------------------
// Purpose: Indicates that mouse cursor should be visible when this panel is visible
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CMenuPanel::IsShowingCursor(void)
{
	return m_bShowCursor;
}

int CMenuPanel::getBackgroundMode(void)
{
	return m_iBackgroundMode;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : BackgroundMode - 
//-----------------------------------------------------------------------------
void CMenuPanel::setBackgroundMode(int BackgroundMode)
{
	m_iBackgroundMode = BackgroundMode;
}

//-----------------------------------------------------------------------------
// Purpose: Overloadable. Panel is going to close
//-----------------------------------------------------------------------------
void CMenuPanel::OnClose(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: Catch hotkeys
// Input  : &down - 
//			&keynum - 
//			*pszCurrentBinding - 
// Output : int - 0: handled, 1: unhandled, allow other panels to recieve key
//-----------------------------------------------------------------------------
int CMenuPanel::KeyInput(const int &down, const int &keynum, const char *pszCurrentBinding)
{
	return 0;
}

//-----------------------------------------------------------------------------
// XDM3035a: TODO: FIXME: this system is UGLY! Need to iterate panels through normal dynamic array!
// cache pNextPanel for display once this one's finished
// caution: recursion
// Input  : *pNextPanel - 
//-----------------------------------------------------------------------------
void CMenuPanel::SetNextMenu(CMenuPanel *pNextPanel)
{
	if (pNextPanel == this)// this may and will happen!
		return;

	if (m_pNextMenu)
		m_pNextMenu->SetNextMenu(pNextPanel);
	else
		m_pNextMenu = pNextPanel;
}



//-----------------------------------------------------------------------------
// Purpose: Simple integer signal handler
// Input  : *pPanel - 
//			signal - 
//-----------------------------------------------------------------------------
CMenuPanelActionSignalHandler::CMenuPanelActionSignalHandler(CMenuPanel *pPanel, int signal)
{
	m_pPanel = pPanel;
	m_iSignal = signal;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *panel - 
//-----------------------------------------------------------------------------
void CMenuPanelActionSignalHandler::actionPerformed(Panel *panel)
{
	if (m_pPanel)
		m_pPanel->OnActionSignal(m_iSignal);
}







//-----------------------------------------------------------------------------
// Purpose: Dialog Panel with standardized sets of buttons and actions
// CDialogPanel *pDialog = (CDialogPanel *)gViewPort->ShowMenu(new CDialogPanel(1, x,y,w,t, MB_OKCANCEL));
// Input  : iRemoveMe - delete this when closed
//			x y - position
//			wide - 
//			tall - 
//			type - WinAPI-like option
// Note   : All internal controls use RELATIVE coordinates, where (0,0) is panel's top-left corner!
//-----------------------------------------------------------------------------
CDialogPanel::CDialogPanel(int RemoveMe, int x, int y, int wide, int tall, unsigned long type) : CMenuPanel(RemoveMe, x,y,wide,tall)
{
	m_iSignalRecieved = 0;
	m_iNumDialogButtons = 0;
	switch (type)
	{
		case MB_OK:
		{
			m_iNumDialogButtons = 1;
			m_pButtons = new CommandButton[m_iNumDialogButtons];
			m_pButtons[0].m_ulID = IDOK;
			m_pButtons[0].setArmed(true);
			m_pButtons[0].setText(BufferedLocaliseTextString("#Menu_OK"));
		}
		break;
		case MB_OKCANCEL:
		{
			m_iNumDialogButtons = 2;
			m_pButtons = new CommandButton[m_iNumDialogButtons];
			m_pButtons[0].m_ulID = IDOK;
			m_pButtons[0].setText(BufferedLocaliseTextString("#Menu_OK"));
			m_pButtons[1].m_ulID = IDCANCEL;
			m_pButtons[1].setText(BufferedLocaliseTextString("#Menu_Cancel"));
		}
		break;
		case MB_ABORTRETRYIGNORE:
		{
			m_iNumDialogButtons = 3;
			m_pButtons = new CommandButton[m_iNumDialogButtons];
			m_pButtons[0].m_ulID = IDABORT;
			m_pButtons[0].setText(BufferedLocaliseTextString("#Menu_Abort"));
			m_pButtons[1].m_ulID = IDRETRY;
			m_pButtons[1].setText(BufferedLocaliseTextString("#Menu_Retry"));
			m_pButtons[2].m_ulID = IDIGNORE;
			m_pButtons[2].setText(BufferedLocaliseTextString("#Menu_Ignore"));
		}
		break;
		case MB_YESNOCANCEL:
		{
			m_iNumDialogButtons = 3;
			m_pButtons = new CommandButton[m_iNumDialogButtons];
			m_pButtons[0].m_ulID = IDYES;
			m_pButtons[0].setText(BufferedLocaliseTextString("#Menu_Yes"));
			m_pButtons[1].m_ulID = IDNO;
			m_pButtons[1].setText(BufferedLocaliseTextString("#Menu_No"));
			m_pButtons[2].m_ulID = IDCANCEL;
			m_pButtons[2].setText(BufferedLocaliseTextString("#Menu_Cancel"));
		}
		break;
		case MB_YESNO:
		{
			m_iNumDialogButtons = 2;
			m_pButtons = new CommandButton[m_iNumDialogButtons];
			m_pButtons[0].m_ulID = IDYES;
			m_pButtons[0].setText(BufferedLocaliseTextString("#Menu_Yes"));
			m_pButtons[1].m_ulID = IDNO;
			m_pButtons[1].setText(BufferedLocaliseTextString("#Menu_No"));
		}
		break;
		case MB_RETRYCANCEL:
		{
			m_iNumDialogButtons = 2;
			m_pButtons = new CommandButton[m_iNumDialogButtons];
			m_pButtons[0].m_ulID = IDRETRY;
			m_pButtons[0].setText(BufferedLocaliseTextString("#Menu_Retry"));
			m_pButtons[1].m_ulID = IDCANCEL;
			m_pButtons[1].setText(BufferedLocaliseTextString("#Menu_Cancel"));
		}
		break;
	}

	for (unsigned long i=0; i<m_iNumDialogButtons; ++i)
	{
		m_pButtons[i].setParent(this);
		m_pButtons[i].setBounds(XRES(PANEL_INNER_OFFSET) + BUTTON_SIZE_X*i,
			tall-BUTTON_SIZE_Y-YRES(PANEL_INNER_OFFSET), BUTTON_SIZE_X, BUTTON_SIZE_Y);
		m_pButtons[i].setText(BufferedLocaliseTextString("#Menu_OK"));
		m_pButtons[i].addActionSignal(new CMenuPanelActionSignalHandler(this, m_pButtons[i].m_ulID));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Reset
//-----------------------------------------------------------------------------
void CDialogPanel::Reset(void)
{
	m_iSignalRecieved = 0;
	CMenuPanel::Reset();
}

//-----------------------------------------------------------------------------
// Purpose: Close
//-----------------------------------------------------------------------------
void CDialogPanel::Close(void)
{
	for (unsigned long i=0; i<m_iNumDialogButtons; ++i)
		removeChild(&m_pButtons[i]);

	delete [] m_pButtons;
	m_pButtons = NULL;

	CMenuPanel::Close();
}

//-----------------------------------------------------------------------------
// Purpose: Overloadable. Do cleanup here.
//-----------------------------------------------------------------------------
void CDialogPanel::OnClose(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: Overloadable. OK
//-----------------------------------------------------------------------------
void CDialogPanel::OnOK(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: Overloadable. Cancel
//-----------------------------------------------------------------------------
void CDialogPanel::OnCancel(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: Overloadable. User code goes here!
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CDialogPanel::DoExecCommand(void)
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Button pressed or something
// Undone : button policy is unclear
// Input  : signal - button ID
//-----------------------------------------------------------------------------
void CDialogPanel::OnActionSignal(int signal)
{
	m_iSignalRecieved = signal;
	switch (signal)
	{
	case IDOK:
		{
			if (DoExecCommand())
				PlaySound("vgui/button_press.wav", VOL_NORM);
			else
				PlaySound("vgui/menu_close.wav", VOL_NORM);

			OnOK();
		}
		break;
	case IDCANCEL:
		{
			OnCancel();
			PlaySound("vgui/button_exit.wav", VOL_NORM);
		}
		break;
	case IDABORT:
		{
			OnCancel();
			PlaySound("vgui/button_exit.wav", VOL_NORM);
		}
		break;
	case IDRETRY:
		{
			if (DoExecCommand())
				PlaySound("vgui/button_press.wav", VOL_NORM);
			else
				PlaySound("vgui/button_enter.wav", VOL_NORM);
		}
		break;
	case IDIGNORE:
	case IDYES:
	case IDNO:
	case IDCLOSE:
	case IDHELP:
		{
			PlaySound("vgui/button_press.wav", VOL_NORM);
		}
		break;
	}

	Close();
}
