//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// eventscripts.h
#if !defined ( EVENTSCRIPTSH )
#define EVENTSCRIPTSH

void FX_BrassShell( float *origin, float rotation, int soundtype, int body, float iLife);
void EV_GetGunPosition(struct event_args_s *args, float *pos, const float *origin);
void FX_GunSmoke(const Vector &pos, int entindex, short attachment, bool black, float scale);
bool EV_IsLocal(int idx);
bool EV_IsPlayer(int idx);

void EV_MuzzleFlash(int entindex);
void EV_PrintParams(event_args_t *args);// XDM: debug

float PlayTextureSound(char chTextureType, float *origin);
int EV_DamageDecal(int iBulletType, char chTextureType);
bool EV_ShouldDoSmoke(int iBulletType, char chTextureType);

#define EMIT_SOUND					(*gEngfuncs.pEventAPI->EV_PlaySound)
#define STOP_SOUND					(*gEngfuncs.pEventAPI->EV_StopSound)
/*
inline void STOP_SOUND(int ent, int channel, const char *sample)
{
	EMIT_SOUND(ent, vec3_origin, channel, sample, 0.0f, 0.0f, SND_STOP, PITCH_NORM);
}
*/

#endif // EVENTSCRIPTSH
