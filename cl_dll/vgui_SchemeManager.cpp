//=========== (C) Copyright 1996-2002 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#include "hud.h"
#include "vgui_SchemeManager.h"
#include "cvardef.h"
#include "cl_util.h"

cvar_t *g_CV_BitmapFonts;

void Scheme_Init()
{
	g_CV_BitmapFonts = CVAR_CREATE("bitmapfonts", "0", FCVAR_CLIENTDLL);
}

//-----------------------------------------------------------------------------
// Purpose: Scheme managers data container
//-----------------------------------------------------------------------------
CScheme::CScheme()
{
	schemeName[0] = 0;
	fontName[0] = 0;
	fontSize = 0;
	fontWeight = 0;
	font = NULL;
	ownFontPointer = false;
}

CScheme::~CScheme()
{
	// only delete our font pointer if we own it
	if ( ownFontPointer )
	{
		delete font;
	}
	font = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: resolution information
//			!! needs to be shared out
//-----------------------------------------------------------------------------
static int g_ResArray[] =
{
	320,
	400,
	512,
	640,
	800,
	1024,
	1152,
	1280,
	1600
};
static int g_NumReses = sizeof(g_ResArray) / sizeof(int);

static byte *LoadFileByResolution( const char *filePrefix, int xRes, const char *filePostfix)
{
	// find our resolution in the res array
	int resNum = g_NumReses - 1;
	while ( g_ResArray[resNum] > xRes )
	{
		resNum--;

		if ( resNum < 0 )
			return NULL;
	}

	// try open the file
	byte *pFile = NULL;

	while (1)
	{
		// try load
		char fname[128];// XDM: 256 is too much!
		sprintf( fname, "%s%d%s", filePrefix, g_ResArray[resNum], filePostfix );

		pFile = gEngfuncs.COM_LoadFile( fname, 5, NULL );

		if (pFile)
			break;
		else
			gEngfuncs.Con_DPrintf("Unable to load %s\n", fname);// XDM

		if (resNum == 0)
			return NULL;

		resNum--;
	}
	return pFile;
}
/*
static void ParseRGBAFromString( byte colorArray[4], const char *colorVector )
{
	int r, g, b, a;
	sscanf( colorVector, "%d %d %d %d", &r, &g, &b, &a );
	colorArray[0] = r;
	colorArray[1] = g;
	colorArray[2] = b;
	colorArray[3] = a;
}

static void ParseRGBAFromString(Color &c, const char *colorVector )
{
	int r, g, b, a;
	sscanf( colorVector, "%d %d %d %d", &r, &g, &b, &a );
	c[0] = r;
	c[1] = g;
	c[2] = b;
	c[3] = a;
}
*/
//-----------------------------------------------------------------------------
// Purpose: initializes the scheme manager
//			loading the scheme files for the current resolution
// Input  : xRes - 
//			yRes - dimensions of output window
//-----------------------------------------------------------------------------
CSchemeManager::CSchemeManager( int xRes, int yRes )
{
	// basic setup
	m_pSchemeList = NULL;
	m_iNumSchemes = 0;
	m_xRes = xRes;
	m_yRes = yRes;

	LoadScheme();// XDM
}

void CSchemeManager::LoadScheme(void)// XDM
{
	// find the closest matching scheme file to our resolution
	char str[80] = "schemes/";

	if (g_pCvarScheme && g_pCvarScheme->string != NULL && strlen(g_pCvarScheme->string) > 0)
	{
		strcat(str, g_pCvarScheme->string);
		gEngfuncs.Con_Printf("Loading color scheme from %s\n", str);// XDM
		strcat(str, "/");
	}

	char *pFile = (char*)LoadFileByResolution(str, m_xRes, ".txt");
	char *pFileStart = pFile;

	byte *pFontData;
	int fontFileLength;
	char fontFilename[512];

	//
	// Read the scheme descriptions from the text file, into a temporary array
	// format is simply:
	// <paramName name> = <paramValue>
	//
	// a <paramName name> of "SchemeName" signals a new scheme is being described
	//

	const static int numTmpSchemes = 64;
	static CScheme tmpSchemes[numTmpSchemes];
	memset( tmpSchemes, 0, sizeof(tmpSchemes) );
	int currentScheme = -1;
	CScheme *pScheme = NULL;

	if ( pFile )
	{
		// record what has been entered so we can create defaults from the different values
		bool hasFgColor = false, hasBgColor = false, hasFgColorArmed = false, hasBgColorArmed = false, hasFgColorClicked = false, hasBgColorClicked = false;
		char token[1024];

		pFile = gEngfuncs.COM_ParseFile( pFile, token );
		while ( strlen(token) > 0 && (currentScheme < numTmpSchemes) )
		{
			// get the paramName name
			static const int tokenSize = 64;
			char paramName[tokenSize], paramValue[tokenSize];

			strncpy( paramName, token, tokenSize );
			paramName[tokenSize-1] = 0; // ensure null termination

			// get the '=' character
			pFile = gEngfuncs.COM_ParseFile( pFile, token );
			if (stricmp(token, "="))
			{
				if ( currentScheme < 0 )
					gEngfuncs.Con_Printf( "error parsing font scheme text file at file start - expected '=', found '%s''\n", token );
				else
					gEngfuncs.Con_Printf( "error parsing font scheme text file at scheme '%s' - expected '=', found '%s''\n", tmpSchemes[currentScheme].schemeName, token );

				break;
			}

			// get paramValue
			pFile = gEngfuncs.COM_ParseFile( pFile, token );
			strncpy( paramValue, token, tokenSize );
			paramValue[tokenSize-1] = 0; // ensure null termination

			// is this a new scheme?
			if (!stricmp(paramName, "SchemeName"))
			{
				// setup the defaults for the current scheme
				if ( pScheme )
				{
					// foreground color defaults (normal -> armed -> mouse down)
					if ( !hasFgColor )
						pScheme->FgColor.SetWhite();
						//pScheme->FgColor[0] = pScheme->FgColor[1] = pScheme->FgColor[2] = pScheme->FgColor[3] = 255;

					if ( !hasFgColorArmed )
						pScheme->FgColorArmed = pScheme->FgColor;
						//memcpy( pScheme->FgColorArmed, pScheme->FgColor, sizeof(pScheme->FgColorArmed) );

					if ( !hasFgColorClicked )
						pScheme->FgColorClicked = pScheme->FgColorArmed;
						//memcpy( pScheme->FgColorClicked, pScheme->FgColorArmed, sizeof(pScheme->FgColorClicked) );

					// background color (normal -> armed -> mouse down)
					if ( !hasBgColor )
						pScheme->BgColor.SetColor(0,0,0,0);
						//pScheme->BgColor[0] = pScheme->BgColor[1] = pScheme->BgColor[2] = pScheme->BgColor[3] = 0;

					if ( !hasBgColorArmed )
						pScheme->BgColorArmed = pScheme->BgColor;
						//memcpy( pScheme->BgColorArmed, pScheme->BgColor, sizeof(pScheme->BgColorArmed) );

					if ( !hasBgColorClicked )
						pScheme->BgColorClicked = pScheme->BgColorArmed;
						//memcpy( pScheme->BgColorClicked, pScheme->BgColorArmed, sizeof(pScheme->BgColorClicked) );

					// font size
					if ( !pScheme->fontSize )
						pScheme->fontSize = 17;

					if ( !pScheme->fontName[0] )
						strcpy( pScheme->fontName, "Arial" );
				}

				// create the new scheme
				currentScheme++;
				pScheme = &tmpSchemes[currentScheme];
				hasFgColor = hasBgColor = hasFgColorArmed = hasBgColorArmed = hasFgColorClicked = hasBgColorClicked = false;

				strncpy( pScheme->schemeName, paramValue, SCHEME_NAME_LENGTH );
				pScheme->schemeName[SCHEME_NAME_LENGTH-1] = '\0'; // ensure null termination of string
			}

			if ( !pScheme )
			{
				gEngfuncs.Con_Printf( "font scheme text file MUST start with a 'SchemeName'\n");
				break;
			}

			// pull the data out into the scheme
			if (!stricmp(paramName, "SchemeName"))
			{
				// WTF? we get here, skip and don't show any errors
			}
			else if (!stricmp(paramName, "FontName"))
			{
				strncpy( pScheme->fontName, paramValue, FONT_NAME_LENGTH );
				pScheme->fontName[FONT_NAME_LENGTH-1] = 0;
			}
			else if (!stricmp(paramName, "FontSize"))
			{
				pScheme->fontSize = atoi( paramValue );
			}
			else if (!stricmp(paramName, "FontWeight"))
			{
				pScheme->fontWeight = atoi( paramValue );
			}
			else if (!stricmp(paramName, "FgColor"))
			{
//				ParseRGBAFromString( pScheme->FgColor, paramValue );
				StringToColor(paramValue, pScheme->FgColor);
				hasFgColor = true;
			}
			else if (!stricmp(paramName, "BgColor"))
			{
//				ParseRGBAFromString( pScheme->BgColor, paramValue );
				StringToColor(paramValue, pScheme->BgColor);
				hasBgColor = true;
			}
			else if (!stricmp(paramName, "FgColorArmed"))
			{
//				ParseRGBAFromString( pScheme->FgColorArmed, paramValue );
				StringToColor(paramValue, pScheme->FgColorArmed);
				hasFgColorArmed = true;
			}	
			else if (!stricmp(paramName, "BgColorArmed"))
			{
//				ParseRGBAFromString( pScheme->BgColorArmed, paramValue );
				StringToColor(paramValue, pScheme->BgColorArmed);
				hasBgColorArmed = true;
			}
			else if (!stricmp(paramName, "FgColorMousedown"))
			{
//				ParseRGBAFromString( pScheme->FgColorClicked, paramValue );
				StringToColor(paramValue, pScheme->FgColorClicked);
				hasFgColorClicked = true;
			}
			else if (!stricmp(paramName, "BgColorMousedown"))
			{
//				ParseRGBAFromString( pScheme->BgColorClicked, paramValue );
				StringToColor(paramValue, pScheme->BgColorClicked);
				hasBgColorClicked = true;
			}
			else if (!stricmp(paramName, "BorderColor"))
			{
//				ParseRGBAFromString( pScheme->BorderColor, paramValue );
				StringToColor(paramValue, pScheme->BorderColor);
			}
			else if (!stricmp(paramName, "BorderColorArmed"))
			{
//				ParseRGBAFromString( pScheme->BorderColorArmed, paramValue );
				StringToColor(paramValue, pScheme->BorderColorArmed);
			}
			else if (!stricmp(paramName, "Border"))
			{
				pScheme->BorderThickness = atoi(paramValue);
			}
			else if (!stricmp(paramName, "BrightColor"))
			{
//				ParseRGBAFromString(pScheme->BrightColor, paramValue);
				StringToColor(paramValue, pScheme->BrightColor);
			}
			else if (!stricmp(paramName, "DarkColor"))
			{
//				ParseRGBAFromString(pScheme->DarkColor, paramValue);
				StringToColor(paramValue, pScheme->DarkColor);
			}
			else
				CON_DPRINTF("CSchemeManager::LoadScheme(): unknown parameter: %s (%s) int scheme %s\n", paramName, paramValue, pScheme->schemeName);

//			CON_DPRINTF("CSchemeManager::LoadScheme(): parsed: %s = %s\n", paramName, paramValue);
			// get the new token last, so we now if the loop needs to be continued or not
			pFile = gEngfuncs.COM_ParseFile( pFile, token );
		}
		// free the file
		gEngfuncs.COM_FreeFile( pFileStart );
	}// XDM: buildDefaultFont:
	else
		return;

	// make sure we have at least 1 valid font
	if ( currentScheme < 0 )
	{
		currentScheme = 0;
		strcpy( tmpSchemes[0].schemeName, "Default Scheme" );
		strcpy( tmpSchemes[0].fontName, "Arial" );
		tmpSchemes[0].fontSize = 0;
		tmpSchemes[0].FgColor[0] = tmpSchemes[0].FgColor[1] = tmpSchemes[0].FgColor[2] = tmpSchemes[0].FgColor[3] = 255;
		tmpSchemes[0].FgColorArmed[0] = tmpSchemes[0].FgColorArmed[1] = tmpSchemes[0].FgColorArmed[2] = tmpSchemes[0].FgColorArmed[3] = 255;
		tmpSchemes[0].FgColorClicked[0] = tmpSchemes[0].FgColorClicked[1] = tmpSchemes[0].FgColorClicked[2] = tmpSchemes[0].FgColorClicked[3] = 255;
	}

	// we have the full list of schemes in the tmpSchemes array
	// now allocate the correct sized list
	m_iNumSchemes = currentScheme + 1; // 0-based index
	m_pSchemeList = new CScheme[ m_iNumSchemes ];

	// copy in the data
	memcpy( m_pSchemeList, tmpSchemes, sizeof(CScheme) * m_iNumSchemes );

	// create the fonts
	for ( int i = 0; i < m_iNumSchemes; ++i)
	{
		m_pSchemeList[i].font = NULL;

		// see if the current font values exist in a previously loaded font
		for ( int j = 0; j < i; ++j)
		{
			// check if the font name, size, and weight are the same
			if ( !stricmp(m_pSchemeList[i].fontName, m_pSchemeList[j].fontName)  
				&& m_pSchemeList[i].fontSize == m_pSchemeList[j].fontSize
				&& m_pSchemeList[i].fontWeight == m_pSchemeList[j].fontWeight )
			{
				// copy the pointer, but mark i as not owning it
				m_pSchemeList[i].font = m_pSchemeList[j].font;
				m_pSchemeList[i].ownFontPointer = false;
			}
		}

		// if we haven't found the font already, load it ourselves
		if ( !m_pSchemeList[i].font )
		{
			fontFileLength = -1;
			pFontData = NULL;

			if (g_CV_BitmapFonts && g_CV_BitmapFonts->value)
			{
				sprintf(fontFilename, "gfx\\vgui\\fonts\\%d_%s.tga", m_xRes, m_pSchemeList[i].schemeName);
				pFontData = gEngfuncs.COM_LoadFile( fontFilename, 5, &fontFileLength );
				if(!pFontData)
					gEngfuncs.Con_Printf("Missing bitmap font: %s\n", fontFilename);
			}

			m_pSchemeList[i].font = new vgui::Font(
				m_pSchemeList[i].fontName, 
				pFontData,
				fontFileLength,
				m_pSchemeList[i].fontSize, 
				0, 
				0, 
				m_pSchemeList[i].fontWeight, 
				false, 
				false, 
				false, 
				false);
			
			m_pSchemeList[i].ownFontPointer = true;
		}

		// fix up alpha values; VGUI uses 1-A (A=0 being solid, A=255 transparent)
		m_pSchemeList[i].FgColor[3] = 255 - m_pSchemeList[i].FgColor[3];
		m_pSchemeList[i].BgColor[3] = 255 - m_pSchemeList[i].BgColor[3];
		m_pSchemeList[i].FgColorArmed[3] = 255 - m_pSchemeList[i].FgColorArmed[3];
		m_pSchemeList[i].BgColorArmed[3] = 255 - m_pSchemeList[i].BgColorArmed[3];
		m_pSchemeList[i].FgColorClicked[3] = 255 - m_pSchemeList[i].FgColorClicked[3];
		m_pSchemeList[i].BgColorClicked[3] = 255 - m_pSchemeList[i].BgColorClicked[3];
	}
}

//-----------------------------------------------------------------------------
// Purpose: frees all the memory used by the scheme manager
//-----------------------------------------------------------------------------
CSchemeManager::~CSchemeManager()
{
	delete [] m_pSchemeList;
	m_iNumSchemes = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Finds a scheme in the list, by name
// Input  : char *schemeName - string name of the scheme
// Output : SchemeHandle_t handle to the scheme
//-----------------------------------------------------------------------------
SchemeHandle_t CSchemeManager::getSchemeHandle( const char *schemeName )
{
	// iterate through the list
	for ( int i = 0; i < m_iNumSchemes; ++i)
	{
		if ( !stricmp(schemeName, m_pSchemeList[i].schemeName) )
			return i;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Finds a scheme in the list, by name
// Input  : char *schemeName - string name of the scheme
// Output : CScheme *
//-----------------------------------------------------------------------------
CScheme *CSchemeManager::getScheme(const char *schemeName)
{
	// iterate through the list
	for ( int i = 0; i < m_iNumSchemes; ++i)
	{
		if ( !stricmp(schemeName, m_pSchemeList[i].schemeName) )
			return &m_pSchemeList[i];
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: always returns a valid scheme handle
// Input  : schemeHandle - 
// Output : CScheme
//-----------------------------------------------------------------------------
CScheme *CSchemeManager::getSafeScheme( SchemeHandle_t schemeHandle )
{
	if ( schemeHandle < m_iNumSchemes )
		return m_pSchemeList + schemeHandle;

	return m_pSchemeList;
}


//-----------------------------------------------------------------------------
// Purpose: Returns the schemes pointer to a font
// Input  : schemeHandle - 
// Output : vgui::Font
//-----------------------------------------------------------------------------
vgui::Font *CSchemeManager::getFont( SchemeHandle_t schemeHandle )
{
	if (schemeHandle)
		return getSafeScheme( schemeHandle )->font;

	return NULL;
}

void CSchemeManager::getFgColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	if (pScheme)// XDM3035a: somehow it was 0
	{
	r = pScheme->FgColor[0];
	g = pScheme->FgColor[1];
	b = pScheme->FgColor[2];
	a = pScheme->FgColor[3];
	}
}

void CSchemeManager::getBgColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	if (pScheme)// XDM3035a: somehow it was 0
	{
	r = pScheme->BgColor[0];
	g = pScheme->BgColor[1];
	b = pScheme->BgColor[2];
	a = pScheme->BgColor[3];
	}
}

void CSchemeManager::getFgArmedColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	if (pScheme)// XDM3035a: somehow it was 0
	{
	r = pScheme->FgColorArmed[0];
	g = pScheme->FgColorArmed[1];
	b = pScheme->FgColorArmed[2];
	a = pScheme->FgColorArmed[3];
	}
}

void CSchemeManager::getBgArmedColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	if (pScheme)// XDM3035a: somehow it was 0
	{
	r = pScheme->BgColorArmed[0];
	g = pScheme->BgColorArmed[1];
	b = pScheme->BgColorArmed[2];
	a = pScheme->BgColorArmed[3];
	}
}

void CSchemeManager::getFgMousedownColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	if (pScheme)// XDM3035a: somehow it was 0
	{
	r = pScheme->FgColorClicked[0];
	g = pScheme->FgColorClicked[1];
	b = pScheme->FgColorClicked[2];
	a = pScheme->FgColorClicked[3];
	}
}

void CSchemeManager::getBgMousedownColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	if (pScheme)// XDM3035a: somehow it was 0
	{
	r = pScheme->BgColorClicked[0];
	g = pScheme->BgColorClicked[1];
	b = pScheme->BgColorClicked[2];
	a = pScheme->BgColorClicked[3];
	}
}

void CSchemeManager::getBorderColor( SchemeHandle_t schemeHandle, int &r, int &g, int &b, int &a )
{
	CScheme *pScheme = getSafeScheme( schemeHandle );
	if (pScheme)// XDM3035a: somehow it was 0
	{
	r = pScheme->BorderColor[0];
	g = pScheme->BorderColor[1];
	b = pScheme->BorderColor[2];
	a = pScheme->BorderColor[3];
	}
}


