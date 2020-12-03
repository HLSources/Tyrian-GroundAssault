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

// Com_Weapons.cpp
// Shared weapons common/shared functions
#include <stdarg.h>
#include "hud.h"
#include "cl_util.h"
#include "archtypes.h"
#include "com_weapons.h"
#include "const.h"
#include "entity_state.h"
#include "r_efx.h"
#include "event_api.h"// XDM

// g_runfuncs is true if this is the first time we've "predicated" a particular movement/firing
//  command.  If it is 1, then we should play events/sounds etc., otherwise, we just will be
//  updating state info, but not firing events
bool g_runfuncs = 0;

// During our weapon prediction processing, we'll need to reference some data that is part of
//  the final state passed into the postthink functionality.  We'll set this pointer and then
//  reset it to NULL as appropriate
struct local_state_s *g_finalstate = NULL;



int AddAmmoNameToAmmoRegistry(const char *szAmmoname)
{
	return 0;
}

int GetAmmoIndexFromRegistry(const char *psz)
{
	return 0;
}

// XDM3035b: TODO: customizeable client version
int GetNextBestWeapon(int iCurrentWeaponID)
{
	return 0;
}

/*
======================
AlertMessage

Print debug messages to console
======================
*/
void AlertMessage(ALERT_TYPE atype, char *szFmt, ...)
{
//	(gHUD.m_pCvarDeveloper->value < 2.0f)
	if (atype == at_aiconsole && CVAR_GET_FLOAT("developer") < 2)// XDM
		return;

	static char	string[1024];
	va_list argptr;
	va_start(argptr, szFmt);
	vsprintf(string, szFmt,argptr);
	va_end(argptr);
	gEngfuncs.Con_Printf("cl:  ");
	gEngfuncs.Con_Printf(string);
}

/*
====================
COM_Log

Log debug messages to file ( appends )
====================
*/
void COM_Log(char *pszFile, char *fmt, ...)
{
	va_list		argptr;
	char		string[1024];
	FILE *fp;
	char *pfilename;

	if (pszFile == NULL)
	{
		pfilename = "XDM.log";
	}
	else
	{
		pfilename = pszFile;
	}

	va_start (argptr,fmt);
	vsprintf (string, fmt,argptr);
	va_end (argptr);

	fp = fopen( pfilename, "a+t");
	if (fp)
	{
		fprintf(fp, "%s", string);
		fclose(fp);
	}
}


//Returns if it's multiplayer.
//Mostly used by the client side weapons.
bool bIsMultiplayer(void)
{
	return gEngfuncs.GetMaxClients() == 1 ? 0 : 1;
}

// remember the current animation for the view model, in case we get out of sync with
//  server.
static int g_currentanim;

/*
=====================
HUD_SendWeaponAnim

Change weapon model animation
=====================
*/
void HUD_SendWeaponAnim( int iAnim, int body, int force )
{
	// Don't actually change it.
	if ( !g_runfuncs && !force )
		return;

	g_currentanim = iAnim;

	// Tell animation system new info
	gEngfuncs.pfnWeaponAnim( iAnim, body );
}

/*
=====================
HUD_GetWeaponAnim

Retrieve current predicted weapon animation
=====================
*/
int HUD_GetWeaponAnim( void )
{
	return g_currentanim;
}

/*
=====================
HUD_PlaySound

Play a sound, if we are seeing this command for the first time
=====================
*/
void HUD_PlaySound( char *sound, float volume )
{
	if ( !g_runfuncs || !g_finalstate )
		return;

	gEngfuncs.pfnPlaySoundByNameAtLocation( sound, volume, (float *)&g_finalstate->playerstate.origin );
}

/*
=====================
HUD_PlaybackEvent

Directly queue up an event on the client
=====================
*/
void HUD_PlaybackEvent( int flags, const edict_t *pInvoker, unsigned short eventindex, float delay,
	float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 )
{
	vec3_t org;
	vec3_t ang;

	if ( !g_runfuncs || !g_finalstate )
	     return;

	// Weapon prediction events are assumed to occur at the player's origin
	org			= g_finalstate->playerstate.origin;
	ang			= v_angles;
	gEngfuncs.pfnPlaybackEvent( flags, pInvoker, eventindex, delay, (float *)&org, (float *)&ang, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2 );
}

/*
=====================
HUD_SetMaxSpeed

=====================
*/
void HUD_SetMaxSpeed( const edict_t *ed, float speed )
{
//	if (ed)
//		ed->v.maxspeed = speed;
}


/*
=====================
UTIL_WeaponTimeBase

Always 0.0 on client, even if not predicting weapons ( won't get called
 in that case )
=====================
*/


/*
======================
stub_*

stub functions for such things as precaching.  So we don't have to modify weapons code that
 is compiled into both game and client .dlls.
======================
*/

int				stub_PrecacheModel		( char* s ) { return gEngfuncs.pEventAPI->EV_FindModelIndex(s); }// XDM
int				stub_PrecacheSound		( char* s ) { return 0; }
//unsigned short	stub_PrecacheEvent		( int type, const char *s ) { return gEngfuncs.pEventAPI->EV_PrecacheEvent(type, s); }// XDM
const char		*stub_NameForFunction	( uint32 function ) { return "func"; }
void			stub_SetModel			( edict_t *e, const char *m ) { /*if (e) {e->v.model = ALLOC_STRING(m); }*/ }
int32			stub_RandomLong			(int32  lLow,  int32  lHigh) { return (int32)gEngfuncs.pfnRandomLong((int32)lLow, (int32)lHigh); };
