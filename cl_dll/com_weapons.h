//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// com_weapons.h
// Shared weapons common function prototypes
#if !defined( COM_WEAPONSH )
#define COM_WEAPONSH
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include "eiface.h"

extern "C"
{
	void EXPORT HUD_PostRunCmd( struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed );
}

void AlertMessage(ALERT_TYPE atype, char *szFmt, ...);
void COM_Log(char *pszFile, char *fmt, ...);
bool CL_IsDead(void);

int UTIL_SharedRandomLong(const unsigned int &seed, const int &low, const int &high);
float UTIL_SharedRandomFloat(const unsigned int &seed, const float &low, const float &high);

void HUD_SendWeaponAnim( int iAnim, int body, int force );
int HUD_GetWeaponAnim( void );
void HUD_PlaySound( char *sound, float volume );
void HUD_PlaybackEvent( int flags, const struct edict_s *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 );
void HUD_SetMaxSpeed( const struct edict_s *ed, float speed );
int stub_PrecacheModel( char* s );
int stub_PrecacheSound( char* s );
//unsigned short stub_PrecacheEvent( int type, const char *s );
const char *stub_NameForFunction(uint32 function);
void stub_SetModel(struct edict_s *e, const char *m);
int32 stub_RandomLong(int32  lLow,  int32  lHigh);// XDM3034

extern bool g_runfuncs;
extern vec3_t v_angles;
extern float g_lastFOV;
extern struct local_state_s *g_finalstate;

#endif
