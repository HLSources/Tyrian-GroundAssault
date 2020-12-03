/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// saytext.cpp
//
// implementation of CHudSayText class
//
// XDM3035: TODO: this whole code really sucks. But nobody wants to rewrite it.

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_XDMViewport.h"


DECLARE_MESSAGE( m_SayText, SayText );


CHudSayText::CHudSayText() : CHudBase()
{
	m_pSayTextList = NULL;
	m_iMaxLines = 0;
}

CHudSayText::~CHudSayText()
{
	FreeData();
// XDM3035	m_fLastKillAward = 0;
}

void CHudSayText::FreeData(void)
{
	if (m_pSayTextList)
	{
		delete [] m_pSayTextList;
		m_pSayTextList = NULL;
		m_iMaxLines = 0;
	}
}

int CHudSayText::Init(void)
{
	gHUD.AddHudElem(this);

	HOOK_MESSAGE(SayText);

	m_pCvarSayText =			CVAR_CREATE("hud_saytext", "1", FCVAR_ARCHIVE|FCVAR_CLIENTDLL);
	m_pCvarSayTextTime =		CVAR_CREATE("hud_saytext_time", "5", FCVAR_ARCHIVE|FCVAR_CLIENTDLL);
	m_pCvarSayTextHighlight =	CVAR_CREATE("hud_saytext_hlight", "1", FCVAR_ARCHIVE|FCVAR_CLIENTDLL);
	m_pCvarSayTextLines =		CVAR_CREATE("hud_saytext_lines", "6", FCVAR_ARCHIVE|FCVAR_CLIENTDLL);

	FreeData();
	InitHUDData();

	m_iFlags |= (HUD_INTERMISSION|HUD_DRAW_ALWAYS); // is always drawn during an intermission // XDM
	return 1;
}

void CHudSayText::InitHUDData(void)
{
/*	memset(m_szLineBuffer, 0, sizeof(m_szLineBuffer));
	memset(m_pflNameColors, 0, sizeof(m_pflNameColors));
	memset(m_pflStringColors, 0, sizeof(m_pflStringColors));// XDM3035
	memset(m_iNameLengths, 0, sizeof(m_iNameLengths));
*/
	if (m_pSayTextList == NULL)
	{
		if (m_pCvarSayTextLines->value < 2)
			m_pCvarSayTextLines->value = 2;
		else if (m_pCvarSayTextLines->value > MAX_LINES)
			m_pCvarSayTextLines->value = MAX_LINES;

		m_iMaxLines = (int)m_pCvarSayTextLines->value;
		m_pSayTextList = new SayTextItem[m_iMaxLines];// XDM3035: this should really be a queue, not an array!
	}

	if (m_pSayTextList)
		memset(m_pSayTextList, 0, sizeof(SayTextItem)*m_iMaxLines);// XDM3036

	m_flScrollTime = 0;  // the time at which the lines next scroll up
	Y_START = 0;
	line_height = 0;
	LocaliseTextString(" (#TEAM)\0", m_szLocalizedTeam, sizeof(m_szLocalizedTeam));
}

int CHudSayText::VidInit(void)
{
	FreeData();
	InitHUDData();
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : int number of lines moved
//-----------------------------------------------------------------------------
int CHudSayText::ScrollTextUp(void)
{
/*
	ConsolePrint(m_szLineBuffer[0]); // move the first line into the console buffer
	m_szLineBuffer[MAX_LINES][0] = 0;
	memmove( m_szLineBuffer[0], m_szLineBuffer[1], sizeof(m_szLineBuffer) - sizeof(m_szLineBuffer[0]) ); // overwrite the first line
	memmove( &m_pflNameColors[0], &m_pflNameColors[1], sizeof(m_pflNameColors) - sizeof(m_pflNameColors[0]) );
	memmove( &m_pflStringColors[0], &m_pflStringColors[1], sizeof(m_pflStringColors) - sizeof(m_pflStringColors[0]) );// XDM3035
	memmove( &m_iNameLengths[0], &m_iNameLengths[1], sizeof(m_iNameLengths) - sizeof(m_iNameLengths[0]) );
	m_szLineBuffer[MAX_LINES-1][0] = 0;

	if (m_szLineBuffer[0][0] == ' ') // also scroll up following lines
	{
		m_szLineBuffer[0][0] = 2;
		return 1 + ScrollTextUp();
	}*/
/*	ConsolePrint(m_pSayTextList[0].szLineBuffer);// print the line that is about to be deleted
	memmove(&m_pSayTextList[0], &m_pSayTextList[1], sizeof(SayTextItem) * (m_iMaxLines-1));// move memory range from second line to last line over the first line, last line must be erased.
	m_pSayTextList[m_iMaxLines-1].szLineBuffer[0] = 0;// erase

	if (m_pSayTextList[0].szLineBuffer[0] == ' ') // also scroll up following lines
	{
		m_pSayTextList[0].szLineBuffer[0] = 2;// wtf? wrapped line maybe?
		return 1 + ScrollTextUp();
	}*/

	int iNumLinesToErase = 1;
	int i;
	for (i=iNumLinesToErase; i<m_iMaxLines; ++i)// count lines that are actually one erapped line
	{
		if (m_pSayTextList[0].szLineBuffer[0] == ' ')// continuation of the wrapped line
			++iNumLinesToErase;
		else
			break;// only count contingous sequence
	}

	for (i=0; i<iNumLinesToErase; ++i)// print lines that are about to be deleted
		ConsolePrint(m_pSayTextList[i].szLineBuffer);

	memmove(&m_pSayTextList[0], &m_pSayTextList[iNumLinesToErase], sizeof(SayTextItem) * (m_iMaxLines-iNumLinesToErase));// move memory range from second line to last line over the first line, last line must be erased.

	for (i=0; i<iNumLinesToErase; ++i)// erase remaining (duplicate) lines
		m_pSayTextList[m_iMaxLines-1-i].szLineBuffer[0] = 0;

	return iNumLinesToErase;
}

int CHudSayText::Draw(const float &flTime)
{
//	if (m_pSayTextList == NULL)
//		return 0;

	if ((gViewPort && gViewPort->AllowedToPrintText() == FALSE) || !m_pCvarSayText->value)
		return 1;

	int y = Y_START;

	// make sure the scrolltime is within reasonable bounds,  to guard against the clock being reset
	m_flScrollTime = min( m_flScrollTime, flTime + m_pCvarSayTextTime->value );

	if ( m_flScrollTime <= flTime )
	{
//		if ( *m_szLineBuffer[0] )
		if (m_pSayTextList[0].szLineBuffer[0])
		{
			m_flScrollTime = flTime + m_pCvarSayTextTime->value;
			// push the console up
			ScrollTextUp();
		}
		else
		{ // buffer is empty,  just disable drawing of this section
			m_iFlags &= ~HUD_ACTIVE;
		}
	}

	for (int i = 0; i < m_iMaxLines; ++i)// MAX_LINES
	{
//		if ( *m_szLineBuffer[i] )
		if (m_pSayTextList[i].szLineBuffer[0])
		{
//			if (m_pflNameColors[i][0] || m_pflNameColors[i][1] || m_pflNameColors[i][2])// XDM: HL HACK!
			if (m_pSayTextList[i].iNameLength > 0)
			{
				// it's a saytext string
//				static char buf[MAX_PLAYER_NAME_LENGTH];//+32];
//				static size_t l = min(m_pSayTextList[i].iNameLength, MAX_PLAYER_NAME_LENGTH);

				// draw the first x characters in the player color
//old				strncpy( buf, m_szLineBuffer[i], min(m_iNameLengths[i], MAX_PLAYER_NAME_LENGTH+32) );
//old				buf[ min(m_iNameLengths[i], MAX_PLAYER_NAME_LENGTH+31) ] = 0;
//old				DrawSetTextColor(m_pflNameColors[i][0], m_pflNameColors[i][1], m_pflNameColors[i][2]);// XDM

//				strncpy(buf, m_pSayTextList[i].szLineBuffer, l);
//				buf[min(l, MAX_PLAYER_NAME_LENGTH-1)] = 0;

				char replaced = m_pSayTextList[i].szLineBuffer[m_pSayTextList[i].iNameLength];
				m_pSayTextList[i].szLineBuffer[m_pSayTextList[i].iNameLength] = 0;
				DrawSetTextColor(m_pSayTextList[i].NameColor[0], m_pSayTextList[i].NameColor[1], m_pSayTextList[i].NameColor[2]);
				int x = DrawConsoleString(LINE_START, y, m_pSayTextList[i].szLineBuffer);
				m_pSayTextList[i].szLineBuffer[m_pSayTextList[i].iNameLength] = replaced;
//				int x = DrawConsoleString(LINE_START, y, buf);

				// color is reset after each string draw
				// XDM: this prints the real saytext
//old				if (m_pflStringColors[i][0] > 0 || m_pflStringColors[i][1] > 0 || m_pflStringColors[i][2] > 0)
//old					DrawSetTextColor(m_pflStringColors[i][0], m_pflStringColors[i][1], m_pflStringColors[i][2]);// XDM

				if (m_pSayTextList[i].StringColor[0] + m_pSayTextList[i].StringColor[1] + m_pSayTextList[i].StringColor[2] > 0)// faster than || ?
					DrawSetTextColor(m_pSayTextList[i].StringColor[0], m_pSayTextList[i].StringColor[1], m_pSayTextList[i].StringColor[2]);

//old				DrawConsoleString( x, y, m_szLineBuffer[i] + m_iNameLengths[i] );
//				DrawConsoleString(x, y, m_pSayTextList[i].szLineBuffer + l);
				DrawConsoleString(x, y, m_pSayTextList[i].szLineBuffer + m_pSayTextList[i].iNameLength);
			}
			else// normal draw
				DrawConsoleString(LINE_START, y, m_pSayTextList[i].szLineBuffer);
//old				DrawConsoleString( LINE_START, y, m_szLineBuffer[i] );
		}
		y += line_height;
	}
	return 1;
}

// XDM3035: now we don't send sender name explicitly in the text, just the index (which is 0 for announcements)
void CHudSayText::SayTextPrint(const char *pszBuf, byte clientIndex, bool teamonly)
{
	const char *pSayTest = (pszBuf[0] == 2)?pszBuf+1:pszBuf;// XDM: HL HACK!

	if (gViewPort && gViewPort->AllowedToPrintText() == FALSE)
	{
		// Print it straight to the console
		if (IsValidPlayerIndex(clientIndex))
			CON_PRINTF("%s: %s%s\n", g_PlayerInfoList[clientIndex].name, pSayTest, (teamonly?m_szLocalizedTeam:""));
//			CON_PRINTF("%s: %s\n", g_PlayerInfoList[clientIndex].name, pSayTest);
		else
			ConsolePrint(pSayTest);

		return;
	}

	int i = 0;
	// find an empty string slot
	for (i = 0; i < m_iMaxLines; ++i)// MAX_LINES
	{
//		if (*m_szLineBuffer[i] == 0)
		if (m_pSayTextList[i].szLineBuffer[0] == 0)
			break;
	}
	if (i == m_iMaxLines)// MAX_LINES
	{
		// force scroll buffer up
		i = m_iMaxLines-ScrollTextUp();//MAX_LINES - 1;
	}

/*
	m_iNameLengths[i] = 0;
//	m_pflNameColors[i] = NULL;
	m_pflNameColors[i][0] = 0;// XDM
	m_pflNameColors[i][1] = 0;
	m_pflNameColors[i][2] = 0;
	m_pflStringColors[i][0] = 0;// XDM
	m_pflStringColors[i][1] = 0;
	m_pflStringColors[i][2] = 0;
*/
	memset(&m_pSayTextList[i], 0, sizeof(SayTextItem));// XDM

	int localindex = gEngfuncs.GetLocalPlayer()->index;
	bool local_highlighted = FALSE;// XDM3035
	const char *pSenderName = "";

	// if it's a say message, search for the players name in the string
	if (*pszBuf == 2 && IsValidPlayerIndex(clientIndex))
	{
		pSayTest = pszBuf+1;
		GetPlayerInfo(clientIndex, &g_PlayerInfoList[clientIndex]);
		pSenderName = g_PlayerInfoList[clientIndex].name;
		//const char *nameInString = NULL;

		if (pSenderName)
		{
//			nameInString = strstr(pszBuf, pSenderName);
//			if (nameInString)
			{
//				m_iNameLengths[i] = strlen(pSenderName);// + (nameInString - pszBuf);
				m_pSayTextList[i].iNameLength = strlen(pSenderName);// + (nameInString - pszBuf);

/*				if (gHUD.m_pCvarUsePlayerColor->value > 0.0f)
					GetPlayerColor(clientIndex, m_pflNameColors[i][0], m_pflNameColors[i][1], m_pflNameColors[i][2]);
				else
					GetTeamColor(g_PlayerExtraInfo[clientIndex].teamnumber, m_pflNameColors[i][0], m_pflNameColors[i][1], m_pflNameColors[i][2]);// XDM
*/
				if (gHUD.m_pCvarUsePlayerColor->value > 0.0f)
					GetPlayerColor(clientIndex, m_pSayTextList[i].NameColor[0], m_pSayTextList[i].NameColor[1], m_pSayTextList[i].NameColor[2]);
				else
					GetTeamColor(g_PlayerExtraInfo[clientIndex].teamnumber, m_pSayTextList[i].NameColor[0], m_pSayTextList[i].NameColor[1], m_pSayTextList[i].NameColor[2]);// XDM

				if (IsTeamGame(gHUD.m_iGameType) || clientIndex == localindex)// XDM3035: highlight the whole string if sent by teammate
				{
					if (g_PlayerExtraInfo[clientIndex].teamnumber == g_PlayerExtraInfo[localindex].teamnumber)
					{
/*						m_pflStringColors[i][0] = m_pflNameColors[i][0];
						m_pflStringColors[i][1] = m_pflNameColors[i][1];
						m_pflStringColors[i][2] = m_pflNameColors[i][2];*/
						m_pSayTextList[i].StringColor[0] = m_pSayTextList[i].NameColor[0];
						m_pSayTextList[i].StringColor[1] = m_pSayTextList[i].NameColor[1];
						m_pSayTextList[i].StringColor[2] = m_pSayTextList[i].NameColor[2];
//						GetTeamColor(g_PlayerExtraInfo[clientIndex].teamnumber, m_pflStringColors[i][0], m_pflStringColors[i][1], m_pflStringColors[i][2]);// XDM
					}
				}
			}
		}

		// XDM3035: chat highlights
		if (m_pCvarSayTextHighlight->value > 0.0f)// highlight if others talking about me ot this is my message
		{
			char *pLocalName = g_PlayerInfoList[localindex].name;// get local player name
			if (pLocalName)
			{
//				const char *nameInString = strstr(pSayTest, pLocalName);// search for it UNDONE: should NOT be case-sensitive!z
//				if (nameInString)// somebody caled me
				if (clientIndex == localindex || strstr(pSayTest, pLocalName))
				{
					local_highlighted = TRUE;
					//GetTeamColor(g_PlayerExtraInfo[clientIndex].teamnumber, m_pflStringColors[i][0], m_pflStringColors[i][1], m_pflStringColors[i][2]);// XDM
//					UnpackRGB(m_pflStringColors[i][0], m_pflStringColors[i][1], m_pflStringColors[i][2], RGB_YELLOW);
					UnpackRGB(m_pSayTextList[i].StringColor[0], m_pSayTextList[i].StringColor[1], m_pSayTextList[i].StringColor[2], RGB_YELLOW);
				}
			}
		}
	}
	else
		pSayTest = pszBuf;

	if (IsValidPlayerIndex(clientIndex))
//		_snprintf(m_szLineBuffer[i], MAX_CHARS_PER_LINE, "%s: %s%s\0", pSenderName, pSayTest, (teamonly?m_szLocalizedTeam:""));
		_snprintf(m_pSayTextList[i].szLineBuffer, MAX_CHARS_PER_LINE, "%s: %s%s\0", pSenderName, pSayTest, (teamonly?m_szLocalizedTeam:""));
	else
//		strncpy(m_szLineBuffer[i], pSayTest, max(strlen(pszBuf), MAX_CHARS_PER_LINE-1));
		strncpy(m_pSayTextList[i].szLineBuffer, pSayTest, max(strlen(pszBuf), MAX_CHARS_PER_LINE-1));

	// make sure the text fits in one line
	EnsureTextFitsInOneLineAndWrapIfHaveTo(i);

	// Set scroll time
	if (i == 0)
		m_flScrollTime = gHUD.m_flTime + m_pCvarSayTextTime->value;

	m_iFlags |= HUD_ACTIVE;

	// XDM3035: don't interrupt the announcer
	if (g_pCvarAnnouncer->value <= 0.0f || gHUD.m_flNextAnnounceTime < gHUD.m_flTime)
	{
		if (local_highlighted && clientIndex != localindex)// don't react on my own message
			PlaySound("misc/talk_hl.wav", 1.0f);
		else
			PlaySound("misc/talk.wav", 1.0f);
	}

/*	if (ScreenHeight >= 480)
		Y_START = ScreenHeight - 60;
	else
		Y_START = ScreenHeight - 45;

	Y_START -= (line_height * (MAX_LINES+1));
*/
//	Y_START = (int)( (int)((float)ScreenHeight*(7/8)) - (line_height * m_iMaxLines));// 0.875 == (1 - 1/8)
//	Y_START = ScreenHeight * (7/8) - (line_height * m_iMaxLines);// 0.875 == (1 - 1/8)
	Y_START = (float)ScreenHeight * 0.865f;//(7/8);
	Y_START = (int)((float)ScreenHeight * 0.865f) - (line_height * m_iMaxLines);//(7/8);
}

void CHudSayText::EnsureTextFitsInOneLineAndWrapIfHaveTo(int line)
{
	int line_width = 0;
//	DrawConsoleStringLen( m_szLineBuffer[line], &line_width, &line_height );
	DrawConsoleStringLen(m_pSayTextList[line].szLineBuffer, &line_width, &line_height);

	if ( (line_width + LINE_START) > MAX_LINE_WIDTH )
	{ // string is too long to fit on line
		// scan the string until we find what word is too long,  and wrap the end of the sentence after the word
		int length = LINE_START;
		int tmp_len = 0;
		char *last_break = NULL;
//		for ( char *x = m_szLineBuffer[line]; *x != 0; ++x )
		for (char *x = m_pSayTextList[line].szLineBuffer; *x != 0; ++x)
		{
			// check for a color change, if so skip past it
			if ( x[0] == '/' && x[1] == '(' )
			{
				x += 2;
				// skip forward until past mode specifier
				while ( *x != 0 && *x != ')' )
					x++;

				if ( *x != 0 )
					x++;

				if ( *x == 0 )
					break;
			}

			char buf[2];
			buf[1] = 0;

//			if ( *x == ' ' && x != m_szLineBuffer[line] )  // store each line break,  except for the very first character
			if (*x == ' ' && x != m_pSayTextList[line].szLineBuffer)// isspace(*x)?
				last_break = x;

			buf[0] = *x;  // get the length of the current character
			DrawConsoleStringLen( buf, &tmp_len, &line_height );
			length += tmp_len;

			if ( length > MAX_LINE_WIDTH )
			{  // needs to be broken up
				if ( !last_break )
					last_break = x-1;

				x = last_break;

				// find an empty string slot
				int j;
				do 
				{
					for (j = 0; j < m_iMaxLines; ++j)// MAX_LINES
					{
//						if ( ! *m_szLineBuffer[j] )
						if (m_pSayTextList[j].szLineBuffer[0] == 0)
							break;
					}
					if (j == m_iMaxLines)// MAX_LINES
					{
						// need to make more room to display text, scroll stuff up then fix the pointers
						int linesmoved = ScrollTextUp();
						line -= linesmoved;
//						last_break = last_break - (sizeof(m_szLineBuffer[0]) * linesmoved);
						last_break = last_break - (MAX_CHARS_PER_LINE * linesmoved);
					}
				}
				while (j == m_iMaxLines);// MAX_LINES

				// copy remaining string into next buffer,  making sure it starts with a space character
				if ( (char)*last_break == (char)' ' )//				if (isspace(*last_break))
				{
//					int linelen = strlen(m_szLineBuffer[j]);
					int linelen = strlen(m_pSayTextList[j].szLineBuffer);
					int remaininglen = strlen(last_break);

					if ( (linelen - remaininglen) <= MAX_CHARS_PER_LINE )
						strcat(m_pSayTextList[j].szLineBuffer, last_break);
//						strcat( m_szLineBuffer[j], last_break );
				}
				else
				{
//					if ( (strlen(m_szLineBuffer[j]) - strlen(last_break) - 2) < MAX_CHARS_PER_LINE )
					if ((strlen(m_pSayTextList[j].szLineBuffer) - strlen(last_break) - 2) < MAX_CHARS_PER_LINE)
					{
						strcat(m_pSayTextList[j].szLineBuffer, " ");
						strcat(m_pSayTextList[j].szLineBuffer, last_break);
//						strcat( m_szLineBuffer[j], " " );
//						strcat( m_szLineBuffer[j], last_break );
					}
				}
				*last_break = 0; // cut off the last string
				EnsureTextFitsInOneLineAndWrapIfHaveTo(j);
				break;
			}
		}
	}
}

int CHudSayText::MsgFunc_SayText(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	byte client_index = READ_BYTE();		// the client who spoke the message
	bool teamonly = false;
	if (client_index & 128)
	{
		teamonly = true;
		client_index &= ~128;
	}
	char *pBuf = READ_STRING();
	END_READ();
	SayTextPrint(pBuf, client_index, teamonly);
//	SayTextPrint(BufferedLocaliseTextString(pBuf), client_index, teamonly);// XDM3035c: NO!
	return 1;
}
