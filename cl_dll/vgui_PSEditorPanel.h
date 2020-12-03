//====================================================================
//
// Purpose: Particle system editor dialog
//
//====================================================================

#ifndef VGUI_PSEDITORPANEL_H
#define VGUI_PSEDITORPANEL_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include "RenderManager.h"
#include "RenderSystem.h"
#include "ParticleSystem.h"

#define PSEP_WIDTH			300
#define PSEP_HEIGHT			100
#define PSEP_MARGIN			4// internal margin


class CPSEditorPanel : public CDialogPanel
{
public:
//	CPSEditorPanel();
	CPSEditorPanel(int x, int y, int wide, int tall, CParticleSystem *pSystem);
public:
	virtual void OnClose(void);
	virtual bool DoExecCommand(void);

private:
	Label					m_Label;
	CDefaultTextEntry		m_TargetName;
	CParticleSystem *m_pSystem;// safer to use system index, but this one is faster
};


#endif // VGUI_PSEDITORPANEL_H
