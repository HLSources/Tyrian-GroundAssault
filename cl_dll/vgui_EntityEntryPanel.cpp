#include "hud.h"
#include "cl_util.h"
#include "kbutton.h"
#include "cvardef.h"
#include "vgui_int.h"
#include "vgui_XDMViewport.h"
#include "vgui_EntityEntryPanel.h"
#include "in_defs.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSModel.h"

extern unsigned int g_iEntityCreationRS;

// XDM: in order to inherit updated coordinates from base class,
// this one should NOT use x y arguments (only getPos() if really nescessasry),
CEntityEntryPanel::CEntityEntryPanel(int x, int y, int wide, int tall) : CMenuPanel(1/*remove*/, x, y, wide, tall)
{
	setBgColor(0,0,0,PANEL_DEFAULT_ALPHA);

	char nullstring[32];
	memset(nullstring, 0, sizeof(nullstring));
	int offsetY = PANEL_INNER_OFFSET + m_TitleIcon.getTall() + EEP_MARGIN;
	m_ClassName.setParent(this);
	m_ClassName.setBounds(EEP_MARGIN,offsetY,wide-EEP_MARGIN*2, TEXTENTRY_SIZE_Y);
	m_ClassName.addActionSignal(new CMenuPanelActionSignalHandler(this, IDOK));
	m_ClassName.setText(nullstring, 0);

	offsetY += EEP_MARGIN + m_ClassName.getTall();
	m_TargetName.setParent(this);
	m_TargetName.setBounds(EEP_MARGIN,offsetY, wide-EEP_MARGIN*2, TEXTENTRY_SIZE_Y);
	m_TargetName.addActionSignal(new CMenuPanelActionSignalHandler(this, IDOK));
	m_TargetName.setText(nullstring, 0);

//	offsetY += EEP_MARGIN + m_TargetName.getTall();
	m_ButtonOK.setParent(this);
	m_ButtonOK.setBounds(XRES(EEP_MARGIN),						tall-BUTTON_SIZE_Y-YRES(EEP_MARGIN),	BUTTON_SIZE_X, BUTTON_SIZE_Y);
	m_ButtonOK.setText(BufferedLocaliseTextString("#Menu_OK"));
	m_ButtonOK.addActionSignal(new CMenuPanelActionSignalHandler(this, IDOK));
//	m_ButtonOK.setArmed(true);

	m_ButtonCancel.setParent(this);
	m_ButtonCancel.setBounds(wide-XRES(EEP_MARGIN)-BUTTON_SIZE_X,	tall-BUTTON_SIZE_Y-YRES(EEP_MARGIN),	BUTTON_SIZE_X, BUTTON_SIZE_Y);
	m_ButtonCancel.setText(BufferedLocaliseTextString("#Menu_Cancel"));
	m_ButtonCancel.addActionSignal(new CMenuPanelActionSignalHandler(this, IDCANCEL));

	setBorder(new CDefaultLineBorder());
	setPaintBorderEnabled(true);
	setDragEnabled(true);
	setCaptureInput(true);

	m_ClassName.requestFocus();
}

void CEntityEntryPanel::OnActionSignal(int signal)
{
	if (signal == IDOK)
		OnOK();
	else if (signal == IDCANCEL)
		OnCancel();
}

void CEntityEntryPanel::OnClose(void)
{
	if (g_iMouseManipulationMode == MMM_CREATE)
		g_iMouseManipulationMode = MMM_NONE;

	if (g_iEntityCreationRS > 0)// cancel creation
	{
		g_pRenderManager->DeleteSystem(g_pRenderManager->FindSystem(g_iEntityCreationRS));
		g_iEntityCreationRS = 0;
	}
}

void CEntityEntryPanel::OnOK(void)
{
	if (doExecCommand())
		PlaySound("vgui/button_press.wav", VOL_NORM);
	else
		PlaySound("vgui/button_exit.wav", VOL_NORM);

	Close();
}

void CEntityEntryPanel::OnCancel(void)
{
	PlaySound("vgui/button_exit.wav", VOL_NORM);
	Close();
}

bool CEntityEntryPanel::doExecCommand(void)
{
	char classname[80];
	char targetname[80];
	m_ClassName.getText(0,classname,80);
	m_TargetName.getText(0,targetname,80);
//	m_pTextEntry.setText(null,0);
	if (classname[0] && g_iEntityCreationRS != 0)
	{
		CRSModel *pSys = (CRSModel *)g_pRenderManager->FindSystem(g_iEntityCreationRS);
		if (pSys)
		{
			char scmd[256];
			if (targetname[0])
				sprintf(scmd, ".c \"%s\" \"%g %g %g\" \"%g %g %g\" \"%s\"\0", classname, pSys->m_vecOrigin[0], pSys->m_vecOrigin[1], pSys->m_vecOrigin[2], pSys->m_vecAngles[0], pSys->m_vecAngles[1], pSys->m_vecAngles[2], targetname);
			else
				sprintf(scmd, ".c \"%s\" \"%g %g %g\" \"%g %g %g\"\0", classname, pSys->m_vecOrigin[0], pSys->m_vecOrigin[1], pSys->m_vecOrigin[2], pSys->m_vecAngles[0], pSys->m_vecAngles[1], pSys->m_vecAngles[2]);

			SERVER_COMMAND(scmd);

//			g_pRenderManager->DeleteSystem(pSys);
//			g_iEntityCreationRS = 0;
			return true;
		}
	}
	return false;
}
