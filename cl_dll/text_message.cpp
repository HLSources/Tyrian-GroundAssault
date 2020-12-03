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
// text_message.cpp
//
// implementation of CHudTextMessage class
//
// this class routes messages through titles.txt for localisation
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_XDMViewport.h"

DECLARE_MESSAGE(m_TextMessage, TextMsg);

int CHudTextMessage::Init(void)
{
	HOOK_MESSAGE(TextMsg);
	gHUD.AddHudElem(this);
	Reset();
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Message handler for text messages
// displays a string, looking them up from the titles.txt file, which can be localised
// parameters:
//   byte:   message direction  ( HUD_PRINTCONSOLE, HUD_PRINTNOTIFY, HUD_PRINTCENTER, HUD_PRINTTALK )
//   string: message
// optional parameters:
//   string: message parameter 1
//   string: message parameter 2
//   string: message parameter 3
//   string: message parameter 4
// any string that starts with the character '#' is a message name, and is used to look up the real message in titles.txt
// the next (optional) one to four strings are parameters for that string (which can also be message names if they begin with '#')
// Output : int
//-----------------------------------------------------------------------------
int CHudTextMessage::MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf)
{
	static char szBuf[5][MSG_STRING_BUFFER];

	BEGIN_READ(pbuf, iSize);
	int msg_dest = READ_BYTE();

	char *msg_text = LocaliseTextString(READ_STRING(), szBuf[0], MSG_STRING_BUFFER);// XDM3035c
//	char *msg_text = LookupString(READ_STRING(), &msg_dest);
//	msg_text = strcpy(szBuf[0], msg_text);

	// keep reading strings and using C format strings for subsituting the strings into the localised text string
	char *sstr1 = LookupString(READ_STRING());
	sstr1 = strcpy(szBuf[1], sstr1);
	StripEndNewlineFromString(sstr1);  // these strings are meant for subsitution into the main strings, so cull the automatic end newlines

	char *sstr2 = LookupString(READ_STRING());
	sstr2 = strcpy(szBuf[2], sstr2);
	StripEndNewlineFromString(sstr2);

	char *sstr3 = LookupString(READ_STRING());
	sstr3 = strcpy(szBuf[3], sstr3);
	StripEndNewlineFromString(sstr3);

	char *sstr4 = LookupString(READ_STRING());
	sstr4 = strcpy(szBuf[4], sstr4);
	StripEndNewlineFromString(sstr4);

	END_READ();

	if (gViewPort && gViewPort->AllowedToPrintText() == FALSE)
		return 1;

	char psz[MSG_STRING_BUFFER*4];

	if (msg_dest == HUD_PRINTCENTER)
	{
		sprintf(psz, msg_text, sstr1, sstr2, sstr3, sstr4);
		CenterPrint(BufferedLocaliseTextString(ConvertCRtoNL(psz)));// XDM3035: added localization
	}
	else if (msg_dest == HUD_PRINTNOTIFY)
	{
		psz[0] = 1;  // mark this message to go into the notify buffer
		sprintf(psz+1, msg_text, sstr1, sstr2, sstr3, sstr4);
		ConsolePrint(ConvertCRtoNL(psz));
	}
	else if (msg_dest == HUD_PRINTTALK)
	{
		sprintf(psz, /*BufferedLocaliseTextString(*/ConvertCRtoNL(msg_text), sstr1, sstr2, sstr3, sstr4);
		gHUD.m_SayText.SayTextPrint(psz, 0, 0);
	}
	else// if (msg_dest == HUD_PRINTCONSOLE)
	{
		sprintf(psz, msg_text, sstr1, sstr2, sstr3, sstr4);
		ConsolePrint(ConvertCRtoNL(psz));
	}
	return 1;
}
