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
//  parsemsg.cpp
//
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
/*
#ifndef byte
typedef unsigned char byte;
#endif
#ifndef true
#define true 1
#endif
*/
static byte *gpBuf = NULL;
static unsigned int giSize = 0;
static unsigned int giRead = 0;
static bool gBadRead = false;

void BEGIN_READ(void *buffer, int size)
{
// too strict	ASSERT(gpBuf == NULL);
	giRead = 0;
	gBadRead = 0;
	giSize = size;
	gpBuf = (byte*)buffer;
}

int READ_REMAINING(void)// XDM3035
{
	return giSize - giRead;
}

int END_READ(void)// XDM3035
{
	int remaining = READ_REMAINING();
#ifdef _DEBUG
	if (remaining > 0)
	{
		CON_DPRINTF("WARNING! Message has %d remaining bytes unread!\n", remaining);
	}
#endif
	if (gBadRead)
	{
		CON_DPRINTF("WARNING! Message has encountered read errors!\n", remaining);
	}
	giRead = 0;
	gBadRead = 0;
	giSize = 0;
	gpBuf = NULL;
	return remaining;
}

signed char READ_CHAR(void)
{
	signed char c;
	if (giRead+sizeof(signed char) > giSize)
	{
		gBadRead = true;
		return -1;
	}
	c = (signed char)gpBuf[giRead];
	giRead += sizeof(signed char);
	return c;
}

byte READ_BYTE(void)
{
	unsigned char c;
	if (giRead+sizeof(unsigned char) > giSize)
	{
		gBadRead = true;
		return 0;
	}
	c = (unsigned char)gpBuf[giRead];
	giRead += sizeof(unsigned char);
	return c;
}

short READ_SHORT(void)
{
	short c;
	if (giRead+sizeof(short) > giSize)
	{
		gBadRead = true;
		return -1;
	}
	c = (short)(gpBuf[giRead] + (gpBuf[giRead+1] << 8));
	giRead += sizeof(short);
	return c;
}

word READ_WORD(void)
{
	short c;
	if (giRead+sizeof(word) > giSize)
	{
		gBadRead = true;
		return 0;
	}
	c = (word)(gpBuf[giRead] + (gpBuf[giRead+1] << 8));
	giRead += sizeof(word);
	return c;
}

long READ_LONG(void)
{
	long c;
	if (giRead+sizeof(long) > giSize)
	{
		gBadRead = true;
		return -1;
	}
 	c = gpBuf[giRead] + (gpBuf[giRead + 1] << 8) + (gpBuf[giRead + 2] << 16) + (gpBuf[giRead + 3] << 24);
	giRead += sizeof(long);
	return c;
}

float READ_FLOAT(void)
{
	if (giRead+4 > giSize)
	{
		gBadRead = true;
		return 0.0f;
	}
	union
	{
		byte    b[4];
		float   f;
		int     l;
	} dat;
	dat.b[0] = gpBuf[giRead];
	dat.b[1] = gpBuf[giRead+1];
	dat.b[2] = gpBuf[giRead+2];
	dat.b[3] = gpBuf[giRead+3];
	giRead += 4;
//	dat.l = LittleLong(dat.l);
	return dat.f;   
}

float READ_COORD(void)
{
	return (float)((float)READ_SHORT() * (1.0f/8));
}

float READ_ANGLE(void)
{
	return (float)((float)READ_CHAR() * (360.0f/256));
}

float READ_HIRESANGLE(void)
{
	return (float)((float)READ_SHORT() * (360.0f/65536));
}

char *READ_STRING(void)
{
	static char string[MSG_STRING_BUFFER];
	unsigned int l = 0;
	signed char c;
	string[0] = 0;
	do
	{
		if (giRead+1 > giSize)
			break;// no more characters

		c = READ_CHAR();
		if (c == 0)// XDM c == -1
			break;

		string[l] = c;
		++l;
	} while (l < /*sizeof(string)*/MSG_STRING_BUFFER-1);

	string[l] = 0;
	return string;
}
