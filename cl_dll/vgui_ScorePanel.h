//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef SCOREPANEL_H
#define SCOREPANEL_H

#include <ctype.h>
#include <VGUI_Panel.h>
#include <VGUI_TablePanel.h>
#include <VGUI_HeaderPanel.h>
#include <VGUI_TextGrid.h>
#include <VGUI_Label.h>
#include <VGUI_TextImage.h>
#include "..\game_shared\vgui_listbox.h"
#include "..\game_shared\vgui_grid.h"
#include "..\game_shared\vgui_defaultinputsignal.h"

// Scoreboard positions
// space to leave between scoreboard borders and screen borders
#define SBOARD_INDENT_X			XRES(104)
#define SBOARD_INDENT_Y			YRES(40)
// low-res scoreboard indents
#define SBOARD_INDENT_X_512		30
#define SBOARD_INDENT_Y_512		30
// ultra-low-res
#define SBOARD_INDENT_X_400		0
#define SBOARD_INDENT_Y_400		20

//#define MAX_SCORES				10
#define MAX_SCOREBOARD_TEAMS	MAX_TEAMS+1

#define NUM_COLUMNS		8
#define NUM_ROWS		(MAX_PLAYERS + (4*MAX_SCOREBOARD_TEAMS))

#define SBOARD_BOTTOM_TEXT_LEN	128

// Scoreboard dimensions
#define SBOARD_TITLE_SIZE_Y		YRES(22)
#define SBOARD_BOTTOM_LABEL_Y	YRES(8)
#define X_BORDER				XRES(4)

#define SB_LAST_KILLER_HIGHLIGHT_TIME	20
/*
#define SBOARD_COLOR_KILLER_BG_R		255
#define SBOARD_COLOR_KILLER_BG_G		0
#define SBOARD_COLOR_KILLER_BG_B		0
*/
#define SBOARD_COLOR_SPECTARGET_BG_R	127
#define SBOARD_COLOR_SPECTARGET_BG_G	127
#define SBOARD_COLOR_SPECTARGET_BG_B	200
#define SBOARD_COLOR_SPECTARGET_BG_A	191

// Scoreboard cells
typedef enum
{
	COLUMN_TRACKER = 0,
	COLUMN_NAME,
	COLUMN_TSCORE,
	COLUMN_KILLS,
	COLUMN_DEATHS,
	COLUMN_LATENCY,
	COLUMN_VOICE,
	COLUMN_BLANK
} sboard_column_types;

typedef enum
{
	SBOARD_ROW_PLAYER = 0,
	SBOARD_ROW_TEAM,
//	SBOARD_ROW_SPECTATORS,
	SBOARD_ROW_BLANK
} sboard_row_types;



// Column sizes
class SBColumnInfo
{
public:
	char				*m_pTitle;		// If null, ignore, if starts with #, it's localized, otherwise use the string directly.
	int					m_Width;		// Based on 640 width. Scaled to fit other resolutions.
	Label::Alignment	m_Alignment;	
};

using namespace vgui;

class CTextImage2 : public Image
{
	typedef Image BaseClass;
public:
	CTextImage2()
	{
		_image[0] = new TextImage("");
		_image[1] = new TextImage("");
	}

	~CTextImage2()
	{
		delete _image[0];
		delete _image[1];
	}

	TextImage *GetImage(int image)
	{
		return _image[image];
	}

	virtual void getSize(int &wide, int &tall)
	{
		int w1, w2, t1, t2;
		_image[0]->getTextSize(w1, t1);
		_image[1]->getTextSize(w2, t2);

		wide = w1 + w2;
		tall = max(t1, t2);
		setSize(wide, tall);
	}

	virtual void doPaint(Panel *panel)
	{
		_image[0]->doPaint(panel);
		_image[1]->doPaint(panel);
	}

	virtual void setPos(int x, int y)
	{
		_image[0]->setPos(x, y);
		int swide, stall;
		_image[0]->getSize(swide, stall);
		int wide, tall;
		_image[1]->getSize(wide, tall);
		_image[1]->setPos(x + wide, y + (int)((float)stall * 0.9f) - tall);// XDM: TODO: revisit
	}

	virtual void setColor(vgui::Color color)
	{
		_image[0]->setColor(color);
	}

	virtual void setColor2(vgui::Color color)
	{
		_image[1]->setColor(color);
	}

private:
	TextImage *_image[2];

};

//-----------------------------------------------------------------------------
// Purpose: Custom label for cells in the Scoreboard's Table Header
//-----------------------------------------------------------------------------
class CLabelHeader : public Label
{
	typedef Label BaseClass;
public:
	CLabelHeader() : Label("")
	{
		_dualImage = new CTextImage2();
		_dualImage->setColor2(vgui::Color(0, 255, 0, 0));
		_row = -2;
		_useFgColorAsImageColor = true;
		_offset[0] = 0;
		_offset[1] = 0;
	}

	~CLabelHeader()
	{
		delete _dualImage;
	}

	inline void setRow(int row)
	{
		_row = row;
	}

	inline void setFgColorAsImageColor(bool state)
	{
		_useFgColorAsImageColor = state;
	}

	virtual void setText(int textBufferLen, const char *text)
	{
		_dualImage->GetImage(0)->setText(text);
		// calculate the text size
		Font *font = _dualImage->GetImage(0)->getFont();
		_gap = 0;
		for (const char *ch = text; *ch != 0; ch++)
		{
			int a, b, c;
			font->getCharABCwide(*ch, a, b, c);
			_gap += (a + b + c);
		}
		_gap += XRES(5);
	}

	virtual void setText(const char *text)
	{
		// strip any non-alnum characters from the end
		char buf[512];
		strcpy(buf, text);
		size_t len = strlen(buf);
		while (len && isspace(buf[--len]))
			buf[len] = 0;

		CLabelHeader::setText(0, buf);
	}

	virtual void setText2(const char *text)
	{
		_dualImage->GetImage(1)->setText(text);
	}

	virtual void getTextSize(int &wide, int &tall)
	{
		_dualImage->getSize(wide, tall);
	}

	virtual void setFgColor(int r,int g,int b,int a)
	{
		Label::setFgColor(r,g,b,a);
		vgui::Color color(r,g,b,a);
		_dualImage->setColor(color);
		_dualImage->setColor2(color);
		repaint();
	}

	virtual void setFgColor(Scheme::SchemeColor sc)
	{
		int r, g, b, a;
		Label::setFgColor(sc);
		Label::getFgColor(r, g, b, a);
		// Call the r,g,b,a version so it sets the color in the dualImage..
		setFgColor(r, g, b, a);
	}

	virtual void setFont(Font *font)
	{
		_dualImage->GetImage(0)->setFont(font);
	}

	virtual void setFont2(Font *font)
	{
		_dualImage->GetImage(1)->setFont(font);
	}

	// this adjust the absolute position of the text after alignment is calculated
	virtual void setTextOffset(int x, int y)
	{
		_offset[0] = x;
		_offset[1] = y;
	}

	virtual void paint();
	virtual void paintBackground();
	virtual void calcAlignment(const int &iwide, const int &itall, int &x, int &y);

private:
	CTextImage2 *_dualImage;
	int _row;
	int _gap;
	int _offset[2];
	bool _useFgColorAsImageColor;
//	ScorePanel *m_pScorePanel;
};



//-----------------------------------------------------------------------------
// Purpose: Scoreboard back panel
//-----------------------------------------------------------------------------
class ScorePanel : public CMenuPanel, public vgui::CDefaultInputSignal
{
	friend CLabelHeader;
	typedef CMenuPanel BaseClass;
private:
	// Default panel implementation doesn't forward mouse messages when there is no cursor and we need them.
	class HitTestPanel : public Panel
	{
	public:
		virtual void internalMousePressed(MouseCode code);
	};

	CLabelHeader *GetPlayerEntry(const int &x, const int &y)	{return &m_PlayerEntries[x][y];}

public:
	ScorePanel(int x,int y,int wide,int tall);
	virtual void Open(void);
// InputSignal overrides.
	virtual void mousePressed(MouseCode code, Panel* panel);
	virtual void cursorMoved(int x, int y, Panel *panel);

	void Update(void);
	void UpdateCounters(void);// update time/frags left counters
//	void UpdateTitle(void);// updates separately
	void SortTeams(void);
	void SortPlayers(/*const int &iRowType, */const TEAM_ID &teamindex, bool bIncludeSpectators);
	void RebuildTeams(void);
	void FillGrid(void);
	void DeathMsg(int killer, int victim);
	void Initialize(void);
	void MouseOverCell(const int &row, const int &col);
	void DumpInfo(void);// XDM: debug
	int GetBestPlayer(void);
	TEAM_ID GetBestTeam(void);
	void AddRow(int iRowData, int iRowType);// XDM3035c

//	int				m_iNumTeams;
	int				m_iPlayerNum;
	int				m_iShowscoresHeld;
	int				m_iRows;
	int				m_iSortedRows[NUM_ROWS];// contains player or team indexes
	unsigned short	m_iRowType[NUM_ROWS];// XDM3035a
	bool			m_bHasBeenSorted[MAX_PLAYERS+1];// XDM3035: TESTME
//	bool			m_bHasBeenSortedTeam[MAX_TEAMS+1];// XDM3035a: TESTME
	int				m_iBestPlayer;
	int				m_iLastKilledBy;
	float			m_fLastKillDisplayStopTime;// XDM: was int
	TEAM_ID			m_SortedTeams[MAX_TEAMS+1];// one for spectators
//	short			m_SortedPlayers[MAX_PLAYERS];// max of 32 players

protected:
	char			m_szScoreLimitLabelFmt[SBOARD_BOTTOM_TEXT_LEN];// XDM3035: localized string
	Label			m_TitleLabel;
	Label			m_BottomLabel;// XDM3035
	Label			m_CurrentTimeLabel;

	// Here is how these controls are arranged hierarchically.
	// m_HeaderGrid
	//     m_HeaderLabels
	CGrid			m_HeaderGrid;
	CLabelHeader	m_HeaderLabels[NUM_COLUMNS];			// Labels above the grid
	CLabelHeader	*m_pCurrentHighlightLabel;
	int				m_iHighlightRow;

	// m_PlayerList
	//     m_PlayerGrids
	//         m_PlayerEntries 
	CListBox		m_PlayerList;
	CGrid			m_PlayerGrids[NUM_ROWS];				// The grid with player and team info. 
	CLabelHeader	m_PlayerEntries[NUM_COLUMNS][NUM_ROWS];	// Labels for the grid entries.

	ScorePanel::HitTestPanel	m_HitTestPanel;
	CommandButton				*m_pCloseButton;

	Font *m_pFontScore;
	Font *m_pFontTitle;
	Font *m_pFontSmall;
};

#endif // SCOREPANEL_H
