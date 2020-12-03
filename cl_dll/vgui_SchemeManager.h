//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef VGUI_SCHEMEMANAGER_H
#define VGUI_SCHEMEMANAGER_H

#include "color.h"
#include <VGUI_Font.h>


// handle to an individual scheme
typedef int SchemeHandle_t;


// Register console variables, etc..
void Scheme_Init();

#define SCHEME_NAME_LENGTH		32
#define FONT_NAME_LENGTH		48
#define FONT_FILENAME_LENGTH	64

class CScheme
{
public:
	// construction/destruction
	CScheme();
	~CScheme();

	// name
	char schemeName[SCHEME_NAME_LENGTH];
	// font
	char fontName[FONT_NAME_LENGTH];

	int fontSize;
	int fontWeight;

	vgui::Font *font;
	bool ownFontPointer; // true if the font is ours to delete

	// scheme
	Color FgColor;// not vgui::Color
	Color BgColor;
	Color FgColorArmed;
	Color BgColorArmed; 
	Color FgColorClicked;
	Color BgColorClicked;
	Color BorderColor;
	Color BorderColorArmed;
	Color BrightColor;
	Color DarkColor;

	int BorderThickness;
};


//-----------------------------------------------------------------------------
// Purpose: Handles the loading of text scheme description from disk
//			supports different font/color/size schemes at different resolutions 
//-----------------------------------------------------------------------------
class CSchemeManager
{
public:
	// initialization
	CSchemeManager( int xRes, int yRes );
	virtual ~CSchemeManager();

	// scheme handling
	SchemeHandle_t getSchemeHandle( const char *schemeName );
	CScheme *getScheme(const char *schemeName);
	CScheme *getSafeScheme( SchemeHandle_t schemeHandle );

	// getting info from schemes
	vgui::Font *getFont( SchemeHandle_t schemeHandle );
	void getFgColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getBgColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getFgArmedColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getBgArmedColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getFgMousedownColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getBgMousedownColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );
	void getBorderColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a );

	void LoadScheme(void);// XDM

private:
	CScheme *m_pSchemeList;
	int m_iNumSchemes;

	// Resolution we were initted at.
	int		m_xRes;
	int		m_yRes;
};


#endif // VGUI_SCHEMEMANAGER_H
