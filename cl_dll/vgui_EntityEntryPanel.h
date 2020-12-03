//====================================================================
//
// Purpose: An input box used to create an entity
//
//====================================================================

#ifndef VGUI_ENTITYENTRYPANEL_H
#define VGUI_ENTITYENTRYPANEL_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif


#define EEP_WIDTH			300
#define EEP_HEIGHT			100
#define EEP_MARGIN			4

//typedef int (*pfnOnOKCallback)(char *classname, char *targetname);

class CEntityEntryPanel : public CMenuPanel
{
public:
//	CEntityEntryPanel();
	CEntityEntryPanel(int x, int y, int wide, int tall);
public:
//	virtual void setSize(int wide,int tall);
	virtual void OnActionSignal(int signal);
	virtual void OnClose(void);
	void OnOK(void);
	void OnCancel(void);
	bool doExecCommand(void);

private:
	CDefaultTextEntry		m_ClassName;
	CDefaultTextEntry		m_TargetName;
	CommandButton	m_ButtonOK;
	CommandButton	m_ButtonCancel;
//	TextEntry *m_pTextEntry;
};


#endif // VGUI_ENTITYENTRYPANEL_H
