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
// util.cpp
//
// implementation of class-less helper functions
//

#include "hud.h"
#include <stdio.h>
#include <io.h>
#include <assert.h>
#include <ctype.h>
#include "cl_util.h"
#include "com_model.h"
#include "vgui_XDMViewport.h"
#include "pm_defs.h"
#include "event_api.h"


#ifdef _DEBUG
bool DBG_AssertFunction(bool fExpr, const char *szExpr, const char *szFile, int szLine, const char *szMessage)
{
	if (fExpr)
		return true;

	if (szMessage != NULL)
		CON_PRINTF("CL: ASSERT FAILED: %s (%s@%d)\n%s", szExpr, szFile, szLine, szMessage);
	else
		CON_PRINTF("CL: ASSERT FAILED: %s (%s@%d)\n", szExpr, szFile, szLine);

	_assert((void *)szExpr, (void *)szFile, szLine);
	return fExpr;
}
#else// XDM: ASSERT function for release build
bool NDB_AssertFunction(bool fExpr, const char *szExpr, const char *szMessage)
{
	if (fExpr)
		return true;

	if (szMessage != NULL)
		CON_PRINTF("CL: ASSERT FAILED: %s %s\n", szExpr, szMessage);
	else
		CON_PRINTF("CL: ASSERT FAILED: %s\n", szExpr);

	return fExpr;
}
#endif	// DEBUG

//-----------------------------------------------------------------------------
// Purpose: Uses suit volume
// Input  : *szSound - 
//-----------------------------------------------------------------------------
void PlaySoundSuit(char *szSound)
{
	PlaySound(szSound, VOL_NORM);
}

//-----------------------------------------------------------------------------
// Purpose: Special wrapper with common parameters
// Input  : *szSound - 
//-----------------------------------------------------------------------------
void PlaySoundAnnouncer(char *szSound, float duration)
{
	if (g_pCvarAnnouncer->value > 0.0f)
	{
		PlaySound(szSound, g_pCvarAnnouncer->value);

#ifdef CLDLL_NEWFUNCTIONS
		gHUD.m_flNextAnnounceTime = gHUD.m_flTime + gEngfuncs.pfnGetApproxWavePlayLen(szSound);
#else
		gHUD.m_flNextAnnounceTime = gHUD.m_flTime + duration;
#endif
	}
}

//-----------------------------------------------------------------------------
// Purpose: Weapon shared code
// Output : float
//-----------------------------------------------------------------------------
float UTIL_WeaponTimeBase(void)
{
#if defined(CLIENT_WEAPONS)
	return 0.0f;
#else
	return gHUD.m_flTime;
#endif
}

static unsigned int glSeed = 0; 

unsigned int seed_table[256] =
{
	28985, 27138, 26457, 9451, 17764, 10909, 28790, 8716, 6361, 4853, 17798, 21977, 19643, 20662, 10834, 20103,
	27067, 28634, 18623, 25849, 8576, 26234, 23887, 18228, 32587, 4836, 3306, 1811, 3035, 24559, 18399, 315,
	26766, 907, 24102, 12370, 9674, 2972, 10472, 16492, 22683, 11529, 27968, 30406, 13213, 2319, 23620, 16823,
	10013, 23772, 21567, 1251, 19579, 20313, 18241, 30130, 8402, 20807, 27354, 7169, 21211, 17293, 5410, 19223,
	10255, 22480, 27388, 9946, 15628, 24389, 17308, 2370, 9530, 31683, 25927, 23567, 11694, 26397, 32602, 15031,
	18255, 17582, 1422, 28835, 23607, 12597, 20602, 10138, 5212, 1252, 10074, 23166, 19823, 31667, 5902, 24630,
	18948, 14330, 14950, 8939, 23540, 21311, 22428, 22391, 3583, 29004, 30498, 18714, 4278, 2437, 22430, 3439,
	28313, 23161, 25396, 13471, 19324, 15287, 2563, 18901, 13103, 16867, 9714, 14322, 15197, 26889, 19372, 26241,
	31925, 14640, 11497, 8941, 10056, 6451, 28656, 10737, 13874, 17356, 8281, 25937, 1661, 4850, 7448, 12744,
	21826, 5477, 10167, 16705, 26897, 8839, 30947, 27978, 27283, 24685, 32298, 3525, 12398, 28726, 9475, 10208,
	617, 13467, 22287, 2376, 6097, 26312, 2974, 9114, 21787, 28010, 4725, 15387, 3274, 10762, 31695, 17320,
	18324, 12441, 16801, 27376, 22464, 7500, 5666, 18144, 15314, 31914, 31627, 6495, 5226, 31203, 2331, 4668,
	12650, 18275, 351, 7268, 31319, 30119, 7600, 2905, 13826, 11343, 13053, 15583, 30055, 31093, 5067, 761,
	9685, 11070, 21369, 27155, 3663, 26542, 20169, 12161, 15411, 30401, 7580, 31784, 8985, 29367, 20989, 14203,
	29694, 21167, 10337, 1706, 28578, 887, 3373, 19477, 14382, 675, 7033, 15111, 26138, 12252, 30996, 21409,
	25678, 18555, 13256, 23316, 22407, 16727, 991, 9236, 5373, 29402, 6117, 15241, 27715, 19291, 19888, 19847
};

unsigned int U_Random(void) 
{ 
	glSeed *= 69069; 
	glSeed += seed_table[glSeed & 0xff];
 	return (++glSeed & 0x0fffffff);
} 

void U_Srand(const unsigned int &seed)
{
	glSeed = seed_table[seed & 0xff];
}

/*
=====================
UTIL_SharedRandomLong
=====================
*/
int UTIL_SharedRandomLong(const unsigned int &seed, const int &low, const int &high)
{
	U_Srand((int)seed + low + high);
	unsigned int range = high - low + 1;
	if (!(range - 1))
	{
		return low;
	}
	else
	{
		int rnum = U_Random();
		int offset = rnum % range;
		return (low + offset);
	}
}

/*
=====================
UTIL_SharedRandomFloat
=====================
*/
float UTIL_SharedRandomFloat(const unsigned int &seed, const float &low, const float &high)
{
	U_Srand((int)seed + *(int *)&low + *(int *)&high);
	U_Random();
	U_Random();

	float range = high - low;// XDM3035c: TESTME
	if (range == 0.0f)
	{
		return low;
	}
	else
	{
		int tensixrand = U_Random() & 65535;
		float offset = (float)tensixrand / 65536.0f;
		return (low + offset * range);
	}
}

int UTIL_PointContents(const Vector &vec)// XDM3035
{
//	vec3_t point = vec;
	return gEngfuncs.PM_PointContents((float *)&vec, NULL);
}

bool UTIL_LiquidContents(const Vector &vec)// XDM3035
{
	int pc = UTIL_PointContents(vec);
	if (pc < CONTENTS_SOLID && pc > CONTENTS_SKY)
		return true;
	else
		return false;
}

float UTIL_WaterLevel(const Vector &position, float minz, float maxz)
{
	Vector midUp = position;
	midUp.z = minz;

	if (UTIL_PointContents(midUp) != CONTENTS_WATER)
		return minz;

	midUp.z = maxz;
	if (UTIL_PointContents(midUp) == CONTENTS_WATER)
		return maxz;

	float diff = maxz - minz;
	while (diff > 1.0)
	{
		midUp.z = minz + diff/2.0f;
		if (UTIL_PointContents(midUp) == CONTENTS_WATER)
		{
			minz = midUp.z;
		}
		else
		{
			maxz = midUp.z;
		}
		diff = maxz - minz;
	}

	return midUp.z;
//	gEngfuncs.PM_PointContents(point, NULL) == CONTENTS_WATER)
}


extern cvar_t *sensitivity;

//-----------------------------------------------------------------------------
// Purpose: XDM - adjust mouse sensitivity for modified FOV
// Input  : newfov - 
// Output : float
//-----------------------------------------------------------------------------
float GetSensitivityByFOV(float newfov)
{
	float def_fov = gHUD.GetUpdatedDefaultFOV();
	if (newfov == def_fov)
		return 0.0f;

	float k = 1.0f - ((def_fov - newfov) / max(1.0f, def_fov));// % of default fov
	float s = sensitivity->value * g_pCvarZSR->value * k;
//	CON_DPRINTF("FOV = %f k = %f s = %f\n", newfov, k, s);
	return s;
}

//-----------------------------------------------------------------------------
// Purpose: Simple resolution-aware sprite loading function
// Input  : *pszName - 
// Output : HSPRITE
//-----------------------------------------------------------------------------
HSPRITE LoadSprite(const char *pszName)
{
	int i;
	char sz[256]; 

	if (ScreenWidth < 640)
		i = 320;
	else
		i = 640;

	sprintf(sz, pszName, i);
	return SPR_Load(sz);
}

/* =================================
	GetSpriteList

Finds and returns the matching 
sprite name 'psz' and resolution 'iRes'
in the given sprite list 'pList'
iCount is the number of items in the pList
================================= */
client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount)
{
	if (pList == NULL)
		return NULL;

	int i = iCount;
	client_sprite_t *p = pList;
	while (i)
	{
		if ((!strcmp(psz, p->szName)) && (p->iRes == iRes))
			return p;
		p++;
		--i;
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Parse XYZ from string
// Input  : *str - "0.0 0.0 0.0"
//			*vec - output
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool StringToVec(const char *str, float *vec)
{
/*	float x,y,z;
	if (sscanf(str, "%f %f %f", &x, &y, &z) == 3)
	{
		vec = Vector(x,y,z);
		return true;
	}*/
	if (sscanf(str, "%f %f %f", &vec[0], &vec[1], &vec[2]) == 3)
		return true;

	return false;
}
/*bool StringToVec(const char *str, Vector &vec)
{
	float x,y,z;
	if (str != NULL)
	{
		if (sscanf(str, "%f %f %f", &x, &y, &z) == 3)
		{
			vec.x = x;
			vec.y = y;
			vec.z = z;
			return true;
		}
	}
	return false;
}*/

//-----------------------------------------------------------------------------
// Purpose: Parse RGB from string
// Input  : *str - "255 255 255"
//			&r &g &b - output
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool StringToRGB(const char *str, byte &r, byte &g, byte &b)
{
	int ir, ig, ib;
	if (str && sscanf(str, "%d %d %d", &ir, &ig, &ib) == 3)// scanf will probably write 4 bytes for %d
	{
		r = ir;
		g = ig;
		b = ib;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Parse RGBA from string
// Input  : *str - "255 255 255 255"
//			&r &g &b &a - output
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool StringToRGBA(const char *str, byte &r, byte &g, byte &b, byte &a)
{
	int ir, ig, ib, ia;
	if (str && sscanf(str, "%d %d %d %d", &ir, &ig, &ib, &ia) == 4)// scanf will probably write 4 bytes for %d
	{
		r = ir;
		g = ig;
		b = ib;
		a = ia;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Parse RGBA from string to Color
// Input  : *str - 
//			&c - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool StringToColor(const char *str, ::Color &c)
{
	int ir, ig, ib, ia;
	if (str && sscanf(str, "%d %d %d %d", &ir, &ig, &ib, &ia) == 4)// scanf will probably write 4 bytes for %d
	{
		c.r = ir;
		c.g = ig;
		c.b = ib;
		c.a = ia;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Old Half-Life inheritance
// Input  : &r &g &b - output
//			colorindex - 
//-----------------------------------------------------------------------------
void UnpackRGB(byte &r, byte &g, byte &b, unsigned short colorindex)
{
	switch (colorindex)
	{
	default:
	case RGB_GREEN:
		{
			if (gHUD.m_pCvarUseTeamColor->value > 0.0f)// HACK? RGB_GREEN may be used not just to draw HUD elements
			{
				GetTeamColor(gHUD.m_iTeamNumber, r,g,b);
			}
			else
			{
				Int2RGB(gHUD.m_iDrawColorMain, r,g,b);
			}
		}
		break;
	case RGB_YELLOW:Int2RGB(gHUD.m_iDrawColorYellow, r,g,b); break;
	case RGB_BLUE:	Int2RGB(gHUD.m_iDrawColorBlue, r,g,b); break;
	case RGB_RED:	Int2RGB(gHUD.m_iDrawColorRed, r,g,b); break;
	case RGB_CYAN:	Int2RGB(gHUD.m_iDrawColorCyan, r,g,b); break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Old Half-Life inheritance
// Input  : &r &g &b - output
//			colorindex - 
//-----------------------------------------------------------------------------
void UnpackRGB(int &r, int &g, int &b, unsigned short colorindex)
{
	byte br,bg,bb;
	UnpackRGB(br,bg,bb,colorindex);
	r = br;
	g = bg;
	b = bb;
/*	switch (colorindex)
	{
	default:
	case RGB_GREEN:
		{
			if (IsTeamGame(gHUD.m_iGameType) && g_pCvarUseTeamColor->value > 0.0f)// HACK? RGB_GREEN may be used not just to draw HUD elements
			{
				GetTeamColor(gHUD.m_iTeamNumber, r,g,b);
			}
			else
			{
				Int2RGB(gHUD.m_iDrawColorMain, r,g,b);
			}
		}
		break;
	case RGB_YELLOW:Int2RGB(gHUD.m_iDrawColorYellow, r,g,b); break;
	case RGB_BLUE:	Int2RGB(gHUD.m_iDrawColorBlue, r,g,b); break;
	case RGB_RED:	Int2RGB(gHUD.m_iDrawColorRed, r,g,b); break;
	case RGB_CYAN:	Int2RGB(gHUD.m_iDrawColorCyan, r,g,b); break;
	}*/
}

void Int2RGB(int rgb, int &r, int &g, int &b)
{
	r = RGB2R(rgb);
	g = RGB2G(rgb);
	b = RGB2B(rgb);
}

void Int2RGB(DWORD rgb, byte &r, byte &g, byte &b)
{
	r = RGB2R(rgb);
	g = RGB2G(rgb);
	b = RGB2B(rgb);
}

void Int2RGBA(DWORD rgb, byte &r, byte &g, byte &b)
{
	r = RGB2R(rgb);
	g = RGB2G(rgb);
	b = RGB2B(rgb);
}


// Find the maximum among three values.
/*float max_of3(float first, float second, float third)
{
	float tmp = max( first, second );
	return max( tmp, third );
}

// Find the minimum among three values.
float min_of3(float first, float second, float third)
{
	float tmp = min( first, second );
	return min( tmp, third );
}

void rgb2hsv(float R, float G, float B, float& H, float& S, float& V)
{
	float maxV = max_of3(R, G, B);
	float minV = min_of3(R, G, B);
	float diffV = maxV - minV;
	V = maxV;

	if (maxV != 0.0f)
		S = diffV / maxV;
	else
		S = 0.0f;
	
	if (S == 0.0f)
		H = -1.0f;
	else
	{
		if (R == maxV)
			H = (G - B)/diffV;
		else if (G == maxV)
			H = 2.0f + (B - R)/diffV;
		else
			H = 4.0f + (R - G)/diffV;
		H /= 6.0f;

		if (H < 0.0f) H += 1.0f;
	}
}

void hsv2rgb(float H, float S, float V, float &R, float &G, float &B)
{
	if (S == 0.0f)
	{
		R = V; G = V; B = V;
	}
	else
	{
		if (H == 1.0f)
			H = 0.0f;

		H *= 6.0f;
		int i = (int)H; //the integer part of H
		float f = H - i;
		float p = V * (1.0f - S);
		float q = V * (1.0f - (S * f));
		float t = V * (1.0f - (S * (1 - f)));
		switch(i)
		{
		case 0: R = V; G = t; B = p; break;
		case 1: R = q; G = V; B = p; break;
		case 2: R = p; G = V; B = t; break;
		case 3: R = p; G = q; B = V; break;
		case 4: R = t; G = p; B = V; break;
		case 5: R = V; G = p; B = q; break;
		}
	}
}*/

// r,g,b 0...1; h,s,l must be not null to be calculated
void RGB2HSL(float &r, float &g, float &b, float &h, float &s, float &l)
{
	float delta;
	float *pmin, *pmax;

	pmin = &min(r,min(g,b));
	pmax = &max(r,max(g,b));
	delta = *pmax - *pmin;

	if (h)// H requested
	{
		h = 0.0f;
		if (delta > 0.0f)
		{
			if (pmax == &r && pmax != &g)
				h += (g - b) / delta;
			if (pmax == &g && pmax != &b)
				h += (2 + (b - r) / delta);
			if (pmax == &b && pmax != &r)
				h += (4 + (r - g) / delta);

			h *= 60.0f;
		}
	}

	if (s && l)// SL requested
	{
		s = 0.0f;
		l = (*pmin + *pmax) / 2.0f;

		if (l > 0.0f && l < 1.0f)
			s = delta / (l < 0.5f ? (2.0f*l) : (2.0f - 2.0f*l));
	}
}

// h,s,l 0...255; must be not null to be calculated
void RGB2HSL(byte &rb, byte &gb, byte &bb, float &h, float &s, float &l)
{
	float r = (float)rb/255.0f;
	float g = (float)gb/255.0f;
	float b = (float)bb/255.0f;
	RGB2HSL(r,g,b, h,s,l);
}


/*
   Calculate RGB from HSL, reverse of RGB2HSL()
   Hue is in degrees
   Lightness is between 0 and 1
   Saturation is between 0 and 1
*/
/*COLOUR HSL2RGB(HSL c1)
{
	COLOUR c2,sat,ctmp;
	
	while (c1.h < 0)
		c1.h += 360;
	while (c1.h > 360)
		c1.h -= 360;

	if (c1.h < 120)
	{
		sat.r = (120 - c1.h) / 60.0f;
		sat.g = c1.h / 60.0;
		sat.b = 0;
	}
	else if (c1.h < 240)
	{
		sat.r = 0;
		sat.g = (240 - c1.h) / 60.0f;
		sat.b = (c1.h - 120) / 60.0f;
	}
	else
	{
		sat.r = (c1.h - 240) / 60.0f;
		sat.g = 0;
		sat.b = (360 - c1.h) / 60.0f;
	}
	sat.r = MIN(sat.r,1);
	sat.g = MIN(sat.g,1);
	sat.b = MIN(sat.b,1);

	ctmp.r = 2 * c1.s * sat.r + (1 - c1.s);
	ctmp.g = 2 * c1.s * sat.g + (1 - c1.s);
	ctmp.b = 2 * c1.s * sat.b + (1 - c1.s);
	
	if (c1.l < 0.5)
	{
		c2.r = c1.l * ctmp.r;
		c2.g = c1.l * ctmp.g;
		c2.b = c1.l * ctmp.b;
	} else {
		c2.r = (1 - c1.l) * ctmp.r + 2 * c1.l - 1;
		c2.g = (1 - c1.l) * ctmp.g + 2 * c1.l - 1;
		c2.b = (1 - c1.l) * ctmp.b + 2 * c1.l - 1;
	}
	
	return(c2);
}*/

/*
	Hue is in degrees 0...360
	Lightness is 0...1
	Saturation is 0...1
	RGB is 0...1
*/
void HSL2RGB(float h, float s, float l, float &r, float &g, float &b)
{
	if (s == 0.0f)
	{
		r = l; g = l; b = l;
	}
	else
	{
		while (h < 0.0f)
			h += 360.0f;
		while (h >= 360.0f)
			h -= 360.0f;

		if (h == 1.0f)
			h = 0.0f;

		//h *= 6.0f; 0...1
		h /= 60.0f;
		int i = (int)h; //the integer part of H
		float f = h - i;
		float p = l * (1.0f - s);
		float q = l * (1.0f -(s * f));
		float t = l * (1.0f -(s * (1 - f)));
		switch(i)
		{
		case 0: r = l; g = t; b = p; break;
		case 1: r = q; g = l; b = p; break;
		case 2: r = p; g = l; b = t; break;
		case 3: r = p; g = q; b = l; break;
		case 4: r = t; g = p; b = l; break;
		case 5: r = l; g = p; b = q; break;
		}
	}
}

/*
	Hue is in degrees 0...360
	Lightness is 0...1
	Saturation is 0...1
	RGB is 0...255
*/
void HSL2RGB(float h, float s, float l, byte &rb, byte &gb, byte &bb)
{
	float r;
	float g;
	float b;
	HSL2RGB(h,s,l, r,g,b);
	rb = (byte)(r*255.0f);
	gb = (byte)(g*255.0f);
	bb = (byte)(b*255.0f);
}

//-----------------------------------------------------------------------------
// Purpose: Scale RGB by a/255
// Input  : &r &g &b - 0...255
//			&a - 0...1
//-----------------------------------------------------------------------------
void ScaleColors(int &r, int &g, int &b, const float &a)
{
	r = (int)((float)r * a);
	g = (int)((float)g * a);
	b = (int)((float)b * a);
}
//-----------------------------------------------------------------------------
// Purpose: Scale RGB by a/255
// Input  : &r &g &b - 0...255
//			&a - 0...255
//-----------------------------------------------------------------------------
void ScaleColors(int &r, int &g, int &b, const int &a)
{
	ScaleColors(r,g,b, (float)a/255.0f);
/*	float x = (float)a / 255.0f;
	r = (int)((float)r * x);
	g = (int)((float)g * x);
	b = (int)((float)b * x);*/
}
//-----------------------------------------------------------------------------
// Purpose: Scale RGB by a/255
// Input  : &r &g &b - 0...255
//			&a - 0...1
//-----------------------------------------------------------------------------
void ScaleColors(byte &r, byte &g, byte &b, const float &a)
{
	r = (byte)((float)r * a);
	g = (byte)((float)g * a);
	b = (byte)((float)b * a);
}
//-----------------------------------------------------------------------------
// Purpose: Scale RGB by a/255
// Input  : &r &g &b - 0...255
//			&a - 0...255
//-----------------------------------------------------------------------------
void ScaleColors(byte &r, byte &g, byte &b, const byte &a)
{
	ScaleColors(r,g,b, (float)a/255.0f);
/*	float x = (float)a / 255.0f;
	r = (byte)((float)r * x);
	g = (byte)((float)g * x);
	b = (byte)((float)b * x);*/
}

//-----------------------------------------------------------------------------
// Purpose: Get team RGB colors
// Input  : team - TEAM_ID
//			&r &g &b - output
//-----------------------------------------------------------------------------
void GetTeamColor(TEAM_ID team, byte &r, byte &g, byte &b)
{
	if (IsTeamGame(gHUD.m_iGameType))
	{
		if (team <= TEAM_NONE || team > MAX_TEAMS)//iNumberOfTeamColors)
			team = 0;

		r = g_TeamInfo[team].color[0];
		g = g_TeamInfo[team].color[1];
		b = g_TeamInfo[team].color[2];
	}
	else
	{
//		UnpackRGB(r,g,b, RGB_GREEN);// STACK OVERFLOW!!! Infinite loop!
//		if (sscanf(g_pCvarColorMain->string, "%d %d %d", &r, &g, &b) == 3)
//			return false;
//		else
			team = 0;

		Int2RGB(gHUD.m_iDrawColorMain, r,g,b);// XDM3034 // UNDONE: this is not a right place for this
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get team RGB colors, integer version
// Input  : team - TEAM_ID
//			&r &g &b - output
//-----------------------------------------------------------------------------
void GetTeamColor(TEAM_ID team, int &r, int &g, int &b)
{
	byte br=0,bg=0,bb=0;
	GetTeamColor(team, br,bg,bb);
	r=br;
	g=bg;
	b=bb;
}

//-----------------------------------------------------------------------------
// Purpose: Convert player top or bottom color into RGB color
// Input  : halfcolormap - topcolor/bottomcolor
//			&r &g &b - output
//-----------------------------------------------------------------------------
void PlayerColor(int halfcolormap, byte &r, byte &g, byte &b)
{
	HSL2RGB(((float)halfcolormap/255.0f)*360.0f, 1.0f, 1.0f, r,g,b);
}

//-----------------------------------------------------------------------------
// Purpose: Get player personal color for all game types as RGB
// Input  : *pPlayer - hud_player_info_t
//			&r &g &b - output
//-----------------------------------------------------------------------------
/*void GetPlayerColor(hud_player_info_t *pPlayer, TEAM_ID team, byte &r, byte &g, byte &b)
{
	if (IsTeamGame(gHUD.m_iGameType))
	{
		GetTeamColor(team, r,g,b);
	}
	else
	{
		if (pPlayer)
		{
			PlayerColor(pPlayer->topcolor, r,g,b);
		}
		else
		{
			r = 255;
			g = 255;
			b = 255;
		}
	}
}*/

//-----------------------------------------------------------------------------
// Purpose: Get player personal color for all game types as RGB
// Input  : client - player ID/entindex
//			&r &g &b - output
//-----------------------------------------------------------------------------
bool GetPlayerColor(int client, byte &r, byte &g, byte &b)
{
	if (IsValidPlayerIndex(client))// ActivePlayer is better, but a lot slower
	{
		if (IsTeamGame(gHUD.m_iGameType))
			GetTeamColor(g_PlayerExtraInfo[client].teamnumber, r,g,b);
		else
			PlayerColor(g_PlayerInfoList[client].topcolor, r,g,b);

		return true;
	}
	return false;
}





static const int TMP_LEN = 256;

//-----------------------------------------------------------------------------
// Purpose: ls command
// Input  : *search - 
// Output : unsigned int
//-----------------------------------------------------------------------------
unsigned int UTIL_ListFiles(const char *search)
{
#ifdef _WIN32
	if (search == NULL)
		return 0;

	char tmp[TMP_LEN];
	strncpy(tmp, GET_GAME_DIR(), TMP_LEN);

	strncat(tmp, PATHSEPARATOR, TMP_LEN);
	strncat(tmp, search, TMP_LEN);
//	CON_PRINTF(">>> %s\n", tmp);

	unsigned int count = 0;
	long hFile = 0;
	_finddata_t fdata;

	CON_PRINTF("--- Listing of \"%s\" ---\n NAME\t\t\tSIZE\n", search);
	if ((hFile = _findfirst(tmp, &fdata)) == -1L)
		CON_PRINTF("Nothing found for \"%s\"\n", search);
	else
	{
		do// Find the rest of the files
		{
			CON_PRINTF(" %-32s %8d B\n", fdata.name, fdata.size);// nice format, but only useful with constant width fonts
			++count;
		}
		while (_findnext(hFile, &fdata) == 0);
		CON_PRINTF("--- %u items ---\n", count);
		_findclose(hFile);
	}
	return count;
#else
	CON_PRINTF("UTIL_ListFiles() doesn't work in linux\n");
	return 0;
#endif
}



// X:\...\Half-Life\XDM\searchdir\searchname.searchext
/*int UTIL_ListFiles(const char *searchdir, const char *searchname, const char *searchext)
{
	char tmp[TMP_LEN];
	strncpy(tmp, GET_GAME_DIR(), TMP_LEN);
	if (searchdir != NULL && strlen(searchdir) > 0)
	{
		strncat(tmp, PATHSEPARATOR, TMP_LEN);
		strncat(tmp, searchdir, TMP_LEN);
	}
	if (searchname)
	{
		strncat(tmp, PATHSEPARATOR, TMP_LEN);
		strncat(tmp, searchname, TMP_LEN);
	}
	strncat(tmp, searchext, TMP_LEN);
//	CON_PRINTF(">>> %s\n", tmp);

	int count = 0;
	long hFile = 0;
	_finddata_t fdata;

	CON_PRINTF("--- Searching for %s files ---\n NAME\t\tSIZE\n", searchext);
	if ((hFile = _findfirst(tmp, &fdata)) == -1L)
		CON_PRINTF("No %s files in %s directory!\n", searchext, searchdir);
	else
	{
		do// Find the rest of the files
		{
			CON_PRINTF(" %s\t\t%d\n", fdata.name, fdata.size);
			++count;
		}
		while (_findnext(hFile, &fdata) == 0);
		CON_PRINTF("--- %d %s files found ---\n", count, searchext);
		_findclose(hFile);
	}
	return count;
}*/


//-----------------------------------------------------------------------------
// Purpose: \ to /
// Input  : *pathname - 
//-----------------------------------------------------------------------------
void Pathname_Convert(char *pathname)
{
	int len = strlen(pathname);
	for (int i=0; i < len; ++i)
	{
		if (pathname[i] == '\\')
			pathname[i] = '/';
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Searches through the string for any msg names (indicated by a '#')
// any found are looked up in titles.txt and the new message substituted
// the new value is pushed into dst_buffer
// Input  : *msg - 
//			*dst_buffer - 
//			buffer_size - 
// Output : char
//-----------------------------------------------------------------------------
char *LocaliseTextString(const char *msg, char *dst_buffer, int buffer_size)
{
	if (msg == dst_buffer)// writing to the source?
		return dst_buffer;// XDM3035c: this function does not support writing to the source!

	char msgname[MAX_TITLE_NAME];
	int msgnamelen = 0;
	char *msgnamestart = NULL;
	char *dst = dst_buffer;

	for (char *src = (char*)msg; *src != 0 && buffer_size > 0; --buffer_size)
	{
		if (*src == '#')
		{
			msgnamestart = src;
			++src;// skip
			msgnamelen = 0;

			while (__iscsym(*src) && msgnamelen < MAX_TITLE_NAME)// XDM3035: don't invent wheels
			{
				msgname[msgnamelen] = *src;
				++msgnamelen;
				++src;
			}
			msgname[min(msgnamelen,MAX_TITLE_NAME-1)] = 0;

			// lookup msg name in titles.txt
			client_textmessage_t *clmsg = TextMessageGet(msgname);
			if (clmsg == NULL || clmsg->pMessage == NULL)
			{
				src = msgnamestart;// revert back to the '#', copy it and skip it
				*dst = *src;
				dst++,
				src++;
				continue;
			}
			else
			{
				// copy string into message over the msg name
				for (char *wsrc = (char*)clmsg->pMessage; *wsrc != 0; ++wsrc, ++dst)
					*dst = *wsrc;

				if (*(dst-1) == '\r' || *(dst-1) == '\n')// XDM3035: HACK remove last newline symbol because we need 
				{
					--dst;
					*dst = 0;
				}
			}
			*dst = 0;
		}
		else// just copy character by character, move both the source and destionation pointers
		{
			*dst = *src;
			dst++, src++;
//WTF?			*dst = 0;
		}
	}
	*dst = 0;

	dst_buffer[buffer_size-1] = 0; // ensure null termination
	return dst_buffer;
}

#define LOCALISE_BUFFER_SIZE		1024
//-----------------------------------------------------------------------------
// Purpose: As above, but with a local static buffer
// Input  : *msg - 
// Output : char * - buffered localized string, valid until next call
//-----------------------------------------------------------------------------
char *BufferedLocaliseTextString(const char *msg)
{
	static char dst_buffer[LOCALISE_BUFFER_SIZE];
	LocaliseTextString(msg, dst_buffer, LOCALISE_BUFFER_SIZE);
//	dst_buffer[1023] = 0;// ~X~: ??
	return dst_buffer;
}

//-----------------------------------------------------------------------------
// Purpose: Simplified version of LocaliseTextString;  assumes string is only one word
// Input  : *msg - 
//			*msg_dest - 
// Output : char *
//-----------------------------------------------------------------------------
char *LookupString(const char *msg, int *msg_dest)
{
	if (msg == NULL)
		return "";

	// '#' character indicates this is a reference to a string in titles.txt, and not the string itself
	if (msg[0] == '#')
	{
		// this is a message name, so look up the real message
		client_textmessage_t *clmsg = TextMessageGet( msg+1 );

		if ( !clmsg || !(clmsg->pMessage) )
			return (char *)msg; // lookup failed, so return the original string
		
		if (msg_dest)
		{
			// check to see if titles.txt info overrides msg destination
			// if clmsg->effect is less than 0, then clmsg->effect holds -1 * message_destination
			if (clmsg->effect < 0)
				*msg_dest = -clmsg->effect;
		}

		return (char *)clmsg->pMessage;
	}
	else
	{  // nothing special about this message, so just return the same string
		return (char *)msg;
	}
}

//-----------------------------------------------------------------------------
// Purpose: StripEndNewlineFromString
// Input  : *str - i/o
//-----------------------------------------------------------------------------
// THIS CAUSES WEIRD MEMORY CORRUPTION! char *StripEndNewlineFromString(char *str)
void StripEndNewlineFromString(char *str)
{
	int s = strlen(str) - 1;
	if (str[s] == '\n' || str[s] == '\r')
		str[s] = 0;

//	return str;
}

//-----------------------------------------------------------------------------
// Purpose: converts all '\r' characters to '\n', so that the engine can deal with the properly
// Input  : *str - 
// Output : char * pointer to str
//-----------------------------------------------------------------------------
char *ConvertCRtoNL(char *str)
{
	char *ch = NULL;
	for (ch = str; *ch != 0; ch++)
		if (*ch == '\r')
			*ch = '\n';
	return str;
}

//-----------------------------------------------------------------------------
// Purpose: Obsolete: use _splitpath
// No buffer overflow checks!
// Input  : *fullpath - 
//			*dir - 
//			*name - 
//			*ext - 
//-----------------------------------------------------------------------------
void ExtractFileName(const char *fullpath, char *dir, char *name, char *ext)
{
//	Pathname_Convert(fullpath);
	unsigned int i = 0;
	unsigned int offset = 0;
	unsigned int l = 0;
	unsigned int dot = l;// assume file has no extension

	while (fullpath[i] != 0)
	{
		if (fullpath[i] == '/' || fullpath[i] == '\\')
			offset = i+1;

		i++;
	}
	l = i;
	for (i=offset; i<l; i++)
	{
		if (fullpath[i] == '.')
			dot = i;
	}
	if (dir)
	{
		strncpy(dir, fullpath, offset-1);
		dir[offset-1] = 0;
	}
	if (name)
	{
		strncpy(name, fullpath+offset, dot-offset);
		name[dot-offset] = 0;
	}
	if (ext && dot < l)
	{
		strncpy(ext, fullpath+dot+1, l-dot);
		ext[l-dot] = 0;
	}
}

// BAD!
int LoadModel(const char *pszName, struct model_s *pModel)
{
	int i = 0;
	if (pModel)
		pModel = gEngfuncs.CL_LoadModel(pszName, &i);
	else
		gEngfuncs.CL_LoadModel(pszName, &i);

	return i;
}

//-----------------------------------------------------------------------------
// Purpose: Is local player dead?
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CL_IsDead(void)
{
	return (gHUD.m_Health.m_iHealth <= 0) ? true : false;
}

//-----------------------------------------------------------------------------
// Purpose: Is this a valid team ID?
// Input  : &team_id - TEAM_ID
// Output : Returns true if TEAM_NONE to TEAM_4
//-----------------------------------------------------------------------------
bool IsValidTeam(const TEAM_ID &team_id)
{
	return (team_id >= TEAM_NONE && team_id <= MAX_TEAMS);//gViewPort->GetNumberOfTeams());
}

//-----------------------------------------------------------------------------
// Purpose: Is this a real, playable team? // TEAM_NONE must be invalid here!
// Input  : &team_id - TEAM_ID
// Output : Returns true if TEAM_1 to TEAM_4 and active
//-----------------------------------------------------------------------------
bool IsActiveTeam(const TEAM_ID &team_id)
{
	return (team_id > TEAM_NONE && team_id <= gViewPort->GetNumberOfTeams());
}

//-----------------------------------------------------------------------------
// Purpose: Real player, connected and not a spectator?
// Input  : *ent - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool IsActivePlayer(cl_entity_t *ent)
{
	return (ent &&
// let's rely on ent->player		IsValidPlayer(ent->index) &&//ent->index > 0 && ent->index < MAX_PLAYERS &&
			ent->player &&
// may be dead			ent->curstate.solid != SOLID_NOT &&
// WTF?			ent != gEngfuncs.GetLocalPlayer() &&
//old			!g_IsSpectator[ent->index] &&
			g_PlayerExtraInfo[ent->index].observer == 0 &&
			g_PlayerInfoList[ent->index].name != NULL);
}

//-----------------------------------------------------------------------------
// Purpose: Real player, connected and not a spectator?
// Input  : *ent - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool IsActivePlayer(const int &idx)
{
	if (g_PlayerInfoList[idx].name == NULL)
		return false;
	if (g_PlayerExtraInfo[idx].observer)// UNDONE TODO TESETME Can active players have this flag set in some cases?
		return false;

	return IsActivePlayer(gEngfuncs.GetEntityByIndex(idx));
}

//-----------------------------------------------------------------------------
// Purpose: Is this a possible index for a player?
// Input  : idx - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool IsValidPlayerIndex(const int &idx)
{
	if (idx >= 1 && idx <= gEngfuncs.GetMaxClients())
		return true;

//	CON_DPRINTF("IsValidPlayerIndex(%d) FALSE!\n", idx);
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Is this a spectator?
// Input  : idx - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool IsSpectator(const int &idx)
{
	//if (idx >= 1 && idx <= gEngfuncs.GetMaxClients())//MAX_PLAYERS)
	if (IsValidPlayerIndex(idx))
	{
//old		return (g_IsSpectator[idx] > 0);
		return (g_PlayerExtraInfo[idx].observer > 0);
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: GameRules->IsTeamplay()
// Input  : &gamerules - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool IsTeamGame(const int &gamerules)
{
	if (gamerules >= GT_TEAMPLAY)
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &gamerules - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool IsRoundBasedGame(const int &gamerules)
{
	if (gamerules == GT_LMS)
		return true;
	else if (gamerules == GT_ROUND)
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: COOP_MODE_SWEEP
// Output : int
//-----------------------------------------------------------------------------
int GetGameMode(void)
{
	return gHUD.m_iGameMode;
}

//-----------------------------------------------------------------------------
// Purpose: GAME_FLAG_NOSHOOTING
// Output : int
//-----------------------------------------------------------------------------
int GetGameFlags(void)
{
	return gHUD.m_iGameFlags;
}

//-----------------------------------------------------------------------------
// Purpose: GetGameDescription
// Input  : &gamerules - 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetGameDescription(const int &gamerules)
{
	char stringname[16];
	sprintf(stringname, "#%s%d\0", GAMETITLE_STRING_NAME, gamerules);
	const char *str = BufferedLocaliseTextString(stringname);
	if (str)
		return str;

	return GAMETITLE_DEFAULT_STRING;
}

//-----------------------------------------------------------------------------
// Purpose: Get game rules text
// Input  : &gametype - 
//			&gamemode - 
// Output : char
//-----------------------------------------------------------------------------
char *GetGameRulesIntroText(const short &gametype, const short &gamemode)
{
	switch (gametype)
	{
//	case GT_SINGLE:
	case GT_COOP:
		{
			if (gamemode == COOP_MODE_MONSTERFRAGS)
				return BufferedLocaliseTextString("#INTRO_COOP_MON");//"--- CoOperative monster hunting! ---\n\nGet more monsterfrags!");
			else if (gamemode == COOP_MODE_LEVEL)
				return BufferedLocaliseTextString("#INTRO_COOP_LVL");//"--- CoOperative level playing! ---\n\nAll players must reach the end of level!");
			else // if (gamemode == COOP_MODE_SWEEP)
				return BufferedLocaliseTextString("#INTRO_COOP_SWP");//"--- CoOperative monster sweeping! ---\n\nClear this level off monsters!");

			break;
		}
	case GT_DEATHMATCH:
		{
			return BufferedLocaliseTextString("#INTRO_DM");// it's probably better to automatically generate these string IDs
			break;
		}
	case GT_LMS:
		{
			return BufferedLocaliseTextString("#INTRO_LMS");
			break;
		}
	case GT_TEAMPLAY:
		{
			return BufferedLocaliseTextString("#INTRO_TDM");
			break;
		}
	case GT_CTF:
		{
			return BufferedLocaliseTextString("#INTRO_CTF");
			break;
		}
	case GT_DOMINATION:
		{
			return BufferedLocaliseTextString("#INTRO_DOM");
			break;
		}
	case GT_ASSAULT:
		{
			return BufferedLocaliseTextString("#INTRO_AS");
			break;
		}
	}
	return "";// safe?
}

//-----------------------------------------------------------------------------
// Purpose: Check for user vis.distance setting (for simple manual LOD)
// Input  : &point - point to check
// Output : float - 0...1 is in cl_viewdist, 2 is double cl_viewdist, etc.
//-----------------------------------------------------------------------------
float UTIL_PointViewDist(const Vector &point)
{
	float l = (point-g_vecViewOrigin).Length();
	float v = (g_pCvarViewDistance->value * (gHUD.GetUpdatedDefaultFOV()/gHUD.m_iFOV));// 90/60
	if (v != 0.0f)
		return l/v;

	return 100.0f;// ?
}

//-----------------------------------------------------------------------------
// Purpose: Simple check for user setting (for simple manual LOD)
// Input  : &point - point to check
// Output : Returns true if point is farther than cl_viewdist, false otherwise
//-----------------------------------------------------------------------------
bool UTIL_PointIsFar(const Vector &point, float k)
{
//	return (l > v);
	return (UTIL_PointViewDist(point) > k);
}

//-----------------------------------------------------------------------------
// Purpose: is provided vector visible by local client?
//			OPTIMIZE AS HARD AS POSSIBLE!!
// Input  : &point - point to check
//			check_backplane - check if it's behind viewport
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool UTIL_PointIsVisible(const vec3_t &point, bool check_backplane)
{
	vec3_t dir;
	float l;

	VectorSubtract(point, g_vecViewOrigin, dir);
	l = VectorNormalize(dir);

	if (g_pCvarServerZMax && l >= g_pCvarServerZMax->value*0.9f)// clipped by sv_zmax
		return false;

	if ((gHUD.m_iFogMode > 0) && (gHUD.m_flFogEnd > 32.0f) && (l >= gHUD.m_flFogEnd))// clipped by fog
		return false;

	if (check_backplane)
	{
		float dot = DotProduct(dir, g_vecViewForward);
		if (dot < cosf(gHUD.m_iFOV*0.5f))// point must NOT be behind view plane
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// UTIL_StringToVector originally from ..\dlls\util.cpp, slightly changed
//-----------------------------------------------------------------------------
/*void UTIL_StringToVector(float *pVector, const char *pString)
{
	char *pstr, *pfront, tempString[128];
	int	j;

	strcpy( tempString, pString );
	pstr = pfront = tempString;
	
	for ( j = 0; j < 3; ++j )		
	{
		pVector[j] = atof( pfront );
		
		while ( *pstr && *pstr != ' ' )
			pstr++;
		if (!*pstr)
			break;
		pstr++;
		pfront = pstr;
	}

	if (j < 2)
	{
		for (j = j+1;j < 3; ++j)
			pVector[j] = 0;
	}
}*/

//-----------------------------------------------------------------------------
// Purpose: XDM Special: find physent ID for specified entity index
// Input  : entindex - 
// Output : int
//-----------------------------------------------------------------------------
int GetPhysent(int entindex)
{
	int pei;
	int pe = -1;
	physent_t *ppe = NULL;
	for (pei = 0; pei < MAX_PHYSENTS; ++pei)
	{
		ppe = gEngfuncs.pEventAPI->EV_GetPhysent(pei);// DANGEROUS! Relies on engine safety check!
		if (ppe == NULL)// CHECKME: empty... is this list sorted? BREAK or CONTINUE?
			break;

		if (ppe->info == entindex)
		{
			pe = pei;// found, remember, exit
			break;
		}
	}
	return pe;
}

//-----------------------------------------------------------------------------
// Purpose: Get entity name or at least something useful...
// Input  : entindex - name source
//			*output - print here
//			max_len - maximum length of output string (0 will be put at this-1)
//-----------------------------------------------------------------------------
void GetEntityPrintableName(int entindex, char *output, const size_t max_len)
{
	if (entindex > 0 && output)
	{
		if (IsValidPlayerIndex(entindex))
		{
			strncpy(output, g_PlayerInfoList[entindex].name, max_len);
			output[max_len-1] = 0;
		}
//		else if (gHUD.m_iGameType == GT_COOP)
//			_snprintf(output, max_len, "monster %d", data1);
		else
			_snprintf(output, max_len, "entity %d", entindex);// there are no classnames or targetnames on client side :(
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns pointer to an enitity ONLY if it is updated (has origin and other properties)
// Input  : entindex
// Output : cl_entity_t
//-----------------------------------------------------------------------------
cl_entity_t *GetUpdatingEntity(int entindex)
{
	cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(entindex);
	if (pEntity)// may be outside PVS
	{
		if (pEntity->curstate.messagenum == gHUD.m_LocalPlayerState.messagenum)
			return pEntity;// already culled by server
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: HACK to avoid cleared attachment[]s! We really hate GoldSource, do we?
// Input  : *pEntity - 
//			attachment - 
// Output : Vector - world coordinates
//-----------------------------------------------------------------------------

//FIXME! Somehow this does not work here, in 3035 version ((
const Vector &GetCEntAttachment(const struct cl_entity_s *pEntity, const int attachment)
{
	if (pEntity == NULL)
		return g_vecZero;

	if (pEntity == gHUD.m_pLocalPlayer && g_ThirdPersonView == 0)
	{
		cl_entity_t *pVE = gEngfuncs.GetViewModel();
		if (pVE)
			return pVE->attachment[attachment];
	}
#if defined (CLDLL_FIX_PLAYER_ATTACHMENTS)
	if (pEntity->player)// this HL bullshit only affects players
	{
		if (attachment == 0)
			return pEntity->baseline.vuser1;
		else if (attachment == 1)
			return pEntity->baseline.vuser2;
		else if (attachment == 2)
			return pEntity->baseline.vuser3;
		else// if (attachment == 3)
			return pEntity->baseline.vuser4;
	}
#endif
	return pEntity->attachment[attachment];
}

//-----------------------------------------------------------------------------
// Purpose: Get color gradient by the meter value: red->yellow->green
// Input  : fMeterValue - 0...1
//			&r &g &b - output red...green
//-----------------------------------------------------------------------------
void GetMeterColor(const float &fMeterValue, byte &r, byte &g, byte &b)
{
	byte value = (byte)(255.0f * clamp(fMeterValue, 0,1));// convert to byte 0...255
	r = 255 - value;
	g = value;
	b = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Scale RGB by a/255
// Input  : &r &g &b - 0...255
//			&a - 0...1
//-----------------------------------------------------------------------------
void ScaleColorsF(int &r, int &g, int &b, const float &a)
{
	r = (int)((float)r * a);
	g = (int)((float)g * a);
	b = (int)((float)b * a);
}
