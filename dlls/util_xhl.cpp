//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "gamerules.h"
#include "colors.h"
#include "animation.h"
#include "weapons.h"
#include "globals.h"
#include "trains.h"
#include "msg_fx.h"
#include "pm_materials.h"
#ifndef _WIN32
#include <dirent.h> // scandir()
#endif

//-----------------------------------------------------------------------------
// Purpose: not a brush model
// Input  : *pEnt - 
// Output : int
//-----------------------------------------------------------------------------
int IsPointEntity(CBaseEntity *pEnt)
{
	if (pEnt->pev->modelindex == 0)
		return 1;
//	if ( FClassnameIs( pEnt->pev, "info_target" ) || FClassnameIs( pEnt->pev, "info_landmark" ) || FClassnameIs( pEnt->pev, "path_corner" ) )
//		return 1;
	if (!pEnt->IsBSPModel())// XDM3035a
		return 1;

	return 0;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &iSpeed - current speed
//			&iMax - maximum speed
// Output : int - speed mode value (for client HUD). Lowest starts from 1
//-----------------------------------------------------------------------------
int TrainSpeed(const int &iSpeed, const int &iMax)
{
	float delta = iMax/TRACKTRAIN_NUMSPEEDMODES;
//	float fSpeed = (float)iSpeed/(float)iMax;
//	iRet = (int)(fSpeed*TRACKTRAIN_NUMSPEEDMODES);

	if (delta == 0.0f)
		return 0;

	int iRet = (int)((float)iSpeed/delta);// now it should be -4 to +4
	iRet += TRACKTRAIN_NUMSPEEDMODES+1;// now 1 to 9
	return iRet;
}

//-----------------------------------------------------------------------------
// Purpose: Show to pCaller
//-----------------------------------------------------------------------------
/*void UTIL_HUDMessage(CBaseEntity *pCaller, int fx, float x, float y,
					byte r1, byte g1, byte b1, byte a1,
					byte r2, byte g2, byte b2, byte a2,
					float fadeinTime, float fadeoutTime, float holdTime, float fxTime, const char *message)
{
	hudtextparms_t hText;
	memset(&hText, 0, sizeof(hText));
	hText.channel = 1; hText.effect = fx;
	hText.x = x; hText.y = y;
	hText.r1 = r1; hText.r2 = r2;
	hText.g1 = g1; hText.g2 = g2;
	hText.b1 = b1; hText.b2 = b2;
	hText.a1 = a1; hText.a2 = a2;
	hText.fadeinTime = fadeinTime; hText.fadeoutTime = fadeoutTime;
	hText.holdTime = holdTime; hText.fxTime = fxTime;
	UTIL_HudMessage(pCaller, hText, message);
}*/

//-----------------------------------------------------------------------------
// Purpose: Show to everyone
//-----------------------------------------------------------------------------
/*void UTIL_HUDMessageAll(int fx, float x, float y,
						byte r1, byte g1, byte b1, byte a1,
						byte r2, byte g2, byte b2, byte a2,
						float fadein, float fadeout, float holdtime, float fxtime, const char *message)
{
	hudtextparms_t hText;
	memset(&hText, 0, sizeof(hText));
	hText.channel = 1; hText.effect = fx;
	hText.x = x; hText.y = y;
	hText.r1 = r1; hText.r2 = r2;
	hText.g1 = g1; hText.g2 = g2;
	hText.b1 = b1; hText.b2 = b2;
	hText.a1 = a1; hText.a2 = a2;
	hText.fadeinTime = fadein; hText.fadeoutTime = fadeout;
	hText.holdTime = holdtime; hText.fxTime = fxtime;
	UTIL_HudMessageAll(hText, message);
}*/

//-----------------------------------------------------------------------------
// Purpose: Show to nearby clients
// Input  : *pString - 
//			&center - 
//			radius - 
//-----------------------------------------------------------------------------
void UTIL_ShowMessageRadius(const char *pString, const Vector &center, int radius)
{
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)
		{
			Vector delta = center - pPlayer->pev->origin;
			if (delta.Length() <= radius)
				UTIL_ShowMessage(pString, pPlayer);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Universal, for internal and external APIs, track numbers only
// Input  : *pPlayer - 
//			iTrack - 
//			loop - 
//-----------------------------------------------------------------------------
void PlayCDTrack(CBaseEntity *pPlayer, int iTrack, int loop)
{
	ALERT(at_aiconsole, "SV: PlayCDTrack(%d %d %d)\n", pPlayer?pPlayer->entindex():0, iTrack, loop);

	if (!g_pGameRules->FAllowMapMusic())
		return;

	if (pPlayer && pPlayer->IsPlayer())
		MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, gmsgAudioTrack, NULL, pPlayer->edict());
	else if (pPlayer == NULL)
		MESSAGE_BEGIN(MSG_BROADCAST, gmsgAudioTrack);
	else
		return;

		WRITE_CHAR(iTrack);// WRITE_CHAR? HL track index: -1 == pause which should be transferred to client side safely
		WRITE_BYTE(loop);// loop
	MESSAGE_END();
}

//-----------------------------------------------------------------------------
// Purpose: For named tracks
// Input  : *pPlayer - 
//			*track - 
//			loop - 
//-----------------------------------------------------------------------------
void PlayAudioTrack(CBaseEntity *pPlayer, const char *track, int loop)
{
	ALERT(at_aiconsole, "SV: PlayAudioTrack(%d %s %d)\n", pPlayer?pPlayer->entindex():0, track, loop);

	if (!g_pGameRules->FAllowMapMusic())
		return;

	edict_t *pClient = NULL;
	for (int i = 1; i <= gpGlobals->maxClients; ++i)// XDM3035c
	{
		if (pPlayer)
			pClient = pPlayer->edict();
		else
			pClient = UTIL_ClientEdictByIndex(i);

		if (pClient)
			CLIENT_COMMAND(pClient, "bgm_play \"%s\" %d\n", track, loop);

		if (pPlayer)
			return;
	}
}

//-----------------------------------------------------------------------------
// Purpose: creates a cloud of particles
//-----------------------------------------------------------------------------
void ParticlesCustom(const Vector &vecPos, float rnd_vel, float life, byte color_pal, byte number)
{
	MESSAGE_BEGIN(MSG_PVS, gmsgParticles, vecPos);
		WRITE_COORD(vecPos.x);
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_SHORT((int)rnd_vel*10);
		WRITE_SHORT((int)life*10);
		WRITE_BYTE(color_pal);
		WRITE_BYTE(number);
	MESSAGE_END();
}

//-----------------------------------------------------------------------------
// Purpose: GlowSprite
//-----------------------------------------------------------------------------
void GlowSprite(const Vector &vecPos, int mdl_idx, int life, int scale, int fade)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecPos);
		WRITE_BYTE(TE_GLOWSPRITE);
		WRITE_COORD(vecPos.x);
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_SHORT(mdl_idx);
		WRITE_BYTE(min(life, 255));
		WRITE_BYTE(min(scale, 255));
		WRITE_BYTE(min(fade, 255));
	MESSAGE_END();
}

void SpriteTrail(const Vector &vecPos, const Vector &vecEnd, int mdl_idx, int count, int life, int scale, int vel, int rnd_vel)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecPos);
		WRITE_BYTE(TE_SPRITETRAIL);
		WRITE_COORD(vecPos.x);
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_COORD(vecEnd.x);
		WRITE_COORD(vecEnd.y);
		WRITE_COORD(vecEnd.z);
		WRITE_SHORT(mdl_idx);
		WRITE_BYTE(count);
		WRITE_BYTE(life);
		WRITE_BYTE(scale);
		WRITE_BYTE(vel);
		WRITE_BYTE(rnd_vel);
	MESSAGE_END();
}

void DynamicLight(const Vector &vecPos, int radius, int r, int g, int b, int life, int decay)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecPos);
		WRITE_BYTE(TE_DLIGHT);
		WRITE_COORD(vecPos.x);
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_BYTE(min(radius, 255));
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
		WRITE_BYTE(life);
		WRITE_BYTE(decay);
	MESSAGE_END();
}

// radius and decay should probable be floats
void EntityLight(int entidx, const Vector &vecPos, int radius, int r, int g, int b, int life, int decay)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecPos);
		WRITE_BYTE(TE_ELIGHT);
		WRITE_SHORT(entidx);
		WRITE_COORD(vecPos.x);
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_COORD((float)radius);
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
		WRITE_BYTE(life);
		WRITE_COORD((float)decay);
	MESSAGE_END();
}

void PartSystem(const Vector &vecPos, const Vector &vecDir, const Vector &vecSpreadSize, int sprindex, int rendermode, int type, int max_parts, int life, int flags, int ent)
{
	int msg;
//	if (flags & PARTSYSTEM_FLAG_REMOVE)// send 'remove' message to all clients
	if (type == PARTSYSTEM_TYPE_REMOVEANY)
		msg = MSG_ALL;
	else
		msg = MSG_PVS;

	MESSAGE_BEGIN(msg, gmsgPartSys, vecPos);
		WRITE_COORD(vecPos.x);// origin
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_COORD(vecDir.x);// direction
		WRITE_COORD(vecDir.y);
		WRITE_COORD(vecDir.z);
		WRITE_COORD(vecSpreadSize.x);// spread for flame
		WRITE_COORD(vecSpreadSize.y);// or spark size
		WRITE_COORD(vecSpreadSize.z);
		WRITE_SHORT(sprindex);	// sprite name
		WRITE_BYTE(rendermode);	// render mode
		WRITE_BYTE(type);		// flame/sparks
		WRITE_SHORT(max_parts);	// max particles
		WRITE_SHORT(life);		// life
		WRITE_SHORT(flags);		// flags XDM3035: extended
		WRITE_SHORT(ent);		// follow entity index
	MESSAGE_END();
}

// use TE_BEAMPOINTS, TE_BEAMTORUS, TE_BEAMDISK or TE_BEAMCYLINDER as types, Parameters are NOT converted (e.g. life*0.1, etc.)
void BeamEffect(int type, const Vector &vecPos, const Vector &vecAxis, int spriteindex, int startframe, int fps, int life, int width, int noise, const Vector &color, int brightness, int speed)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecPos);
		WRITE_BYTE(type);
		WRITE_COORD(vecPos.x);
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_COORD(vecAxis.x);
		WRITE_COORD(vecAxis.y);
		WRITE_COORD(vecAxis.z);
		WRITE_SHORT(spriteindex);
		WRITE_BYTE(min(startframe, 255));
		WRITE_BYTE(min(fps, 255));
		WRITE_BYTE(min(life, 255));
		WRITE_BYTE(min(width, 255));
		WRITE_BYTE(min(noise, 255));
		WRITE_BYTE(min(color.x, 255));
		WRITE_BYTE(min(color.y, 255));
		WRITE_BYTE(min(color.z, 255));
		WRITE_BYTE(min(brightness, 255));
		WRITE_BYTE(min(speed, 255));
	MESSAGE_END();
}

// convert blood color to streak color (see "e_efx.h")
int UTIL_BloodToStreak(int color)
{
	switch (color)
	{
	default:
	case BLOOD_COLOR_BLACK:		return 4; break;
	case BLOOD_COLOR_GRAY:		return 4; break;
	case BLOOD_COLOR_MAGENTA:	return 1; break;
	case BLOOD_COLOR_YELLOW:	return 6; break;
	case BLOOD_COLOR_BLUE:		return 3; break;
	case BLOOD_COLOR_GREEN:		return 2; break;
	case BLOOD_COLOR_HUM_SKN:	return 5; break;
	case BLOOD_COLOR_CYAN:		return 3; break;
	case BLOOD_COLOR_RED:		return 1; break;
	case BLOOD_COLOR_WHITE:		return 0; break;
	}
}

float clip(const float &a, const float &min, const float &max)
{
	float r = a;
	while(a < min)
	{
		float d = fabs(min - a);
		r = max - d;
	}
	while(a > max)
	{
		float d = fabs(a - max);
		r = min + d;
	}
	return r;
}

// from garg, see "r_efx.h" for colors
void StreakSplash(const Vector &origin, const Vector &direction, int color, int count, int speed, int velocityRange)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, origin);
		WRITE_BYTE(TE_STREAK_SPLASH);
		WRITE_COORD(origin.x);
		WRITE_COORD(origin.y);
		WRITE_COORD(origin.z);
		WRITE_COORD(direction.x);
		WRITE_COORD(direction.y);
		WRITE_COORD(direction.z);
		WRITE_BYTE(min(color, 255));
		WRITE_SHORT(count);
		WRITE_SHORT(speed);
		WRITE_SHORT(velocityRange);// Random velocity modifier
	MESSAGE_END();
}

void ParticleBurst(const Vector &origin, int radius, int color, int duration)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, origin);
		WRITE_BYTE(TE_PARTICLEBURST);
		WRITE_COORD(origin.x);
		WRITE_COORD(origin.y);
		WRITE_COORD(origin.z);
		WRITE_SHORT(radius);
		WRITE_BYTE(min(color, 255));
		WRITE_BYTE(min(duration, 255));
	MESSAGE_END();
}

Vector UTIL_RandomVector(void)
{
	Vector out;
	out.x = RANDOM_FLOAT(-1.0, 1.0);
	out.y = RANDOM_FLOAT(-1.0, 1.0);
	out.z = RANDOM_FLOAT(-1.0, 1.0);
	return out;
}

Vector RandomVector(const float &x, const float &y, const float &z)
{
	Vector out;
	out.x = RANDOM_FLOAT(-x, x);
	out.y = RANDOM_FLOAT(-y, y);
	out.z = RANDOM_FLOAT(-z, z);
	return out;
}

Vector RandomVector(const Vector &source)
{
	Vector out;
	out.x = RANDOM_FLOAT(-source.x, source.x);
	out.y = RANDOM_FLOAT(-source.y, source.y);
	out.z = RANDOM_FLOAT(-source.z, source.z);
	return out;
}

Vector RandomVectors(const Vector &min, const Vector &max)
{
	Vector out;
	out.x = RANDOM_FLOAT(min.x, max.x);
	out.y = RANDOM_FLOAT(min.y, max.y);
	out.z = RANDOM_FLOAT(min.z, max.z);
	return out;
}

bool UTIL_LiquidContents(const Vector &vec)
{
	int pc = POINT_CONTENTS(vec);
	if (pc < CONTENTS_SOLID && pc > CONTENTS_SKY)
		return true;
	else
		return false;
}

void UTIL_ShowLine(const Vector &start, const Vector &end, float life, byte r, byte g, byte b)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, start);
		WRITE_BYTE(TE_LINE);
		WRITE_COORD(start.x);
		WRITE_COORD(start.y);
		WRITE_COORD(start.z);
		WRITE_COORD(end.x);
		WRITE_COORD(end.y);
		WRITE_COORD(end.z);
		WRITE_SHORT((int)(life*10.0f));
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
	MESSAGE_END();
}

// how to maket this work properly?
void UTIL_ShowBox(const Vector &origin, const Vector &mins, const Vector &maxs, float life, byte r, byte g, byte b)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, origin);
		WRITE_BYTE(TE_BOX);
		WRITE_COORD(origin.x + mins.x);
		WRITE_COORD(origin.y + mins.y);
		WRITE_COORD(origin.z + mins.z);
		WRITE_COORD(origin.x + maxs.x);
		WRITE_COORD(origin.y + maxs.y);
		WRITE_COORD(origin.z + maxs.z);
		WRITE_SHORT((int)(life*10.0f));
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
	MESSAGE_END();
}

void UTIL_DebugBeam(const Vector &vecSrc, const Vector &vecEnd, float life)
{
	BeamEffect(TE_BEAMPOINTS, vecSrc, vecEnd, g_iModelIndexBeamsAll, BLAST_SKIN_SHOCKWAVE,0, (int)(life*10.0f), 8, 0, Vector(255,0,0), 255, 0);
}

void UTIL_DecalPoints(const Vector &src, const Vector &end, edict_t *pent, int decalIndex)
{
	TraceResult tr;
	UTIL_TraceLine(src, end, dont_ignore_monsters, pent, &tr);
	UTIL_DecalTrace(&tr, decalIndex);
}

//-----------------------------------------------------------------------------
// Input  : &vecSrc -
//			&tr -
//			*mins -
//			*maxs -
//			*pEntity -
//-----------------------------------------------------------------------------
void FindHullIntersection(const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity)
{
	TraceResult tmpTrace;
	Vector vecHullEnd = vecSrc + ((tr.vecEndPos - vecSrc)*2.0f);// XDM3037: fixed
	UTIL_TraceLine(vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace);
	if (tmpTrace.flFraction < 1.0f)
	{
		tr = tmpTrace;
		return;
	}
	byte	i, j, k;
	float	distance = 1e6f;
	float	*minmaxs[2] = {mins, maxs};
	Vector vecEnd;
	for (i = 0; i < 2; ++i)
	{
		for (j = 0; j < 2; ++j)
		{
			for (k = 0; k < 2; ++k)
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];
				UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace);
				if (tmpTrace.flFraction < 1.0f)
				{
					float thisDistance = (tmpTrace.vecEndPos - vecSrc).Length();
					if (thisDistance < distance)
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Simplified function
// Input  : *pMe -
// Output : CBaseEntity
//-----------------------------------------------------------------------------
CBaseEntity *UTIL_FindEntityForward(CBaseEntity *pMe)
{
	TraceResult tr;
	Vector forward;
	if (pMe->IsPlayer())
		ANGLE_VECTORS(pMe->pev->v_angle, forward, NULL, NULL);
	else
		ANGLE_VECTORS(pMe->pev->angles, forward, NULL, NULL);

	UTIL_TraceLine(pMe->pev->origin + pMe->pev->view_ofs, pMe->pev->origin + pMe->pev->view_ofs + forward * g_psv_zmax->value, dont_ignore_monsters, ENT(pMe->pev), &tr);
	if (tr.flFraction != 1.0f && !FNullEnt(tr.pHit))
	{
		CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
		return pHit;
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Get entity model's body count
// Input  : *ent -
// Output : int
//-----------------------------------------------------------------------------
int GetEntBodyCount(edict_t *ent)
{
	if (FNullEnt(ent))
		return 0;

	void *pmodel = GET_MODEL_PTR(ent);
	if (!pmodel)
		return 0;
//	int cnt = GetBodyCount(pmodel, 0);
//	ALERT(at_console, "GetEntBodyCount: %s %d\n", STRING(ent->v.classname), cnt);
	return GetBodyCount(pmodel, 0);
}

//-----------------------------------------------------------------------------
// Purpose: Print detailed entity information (dev/debug)
// Input  : *pEntity - 
//-----------------------------------------------------------------------------
void UTIL_PrintEntInfo(CBaseEntity *pEntity)
{
	if (pEntity == NULL)
		return;

	ALERT(at_console, "%d: %s\n targetname: %s\n globalname: %s\n target: %s, netname: %s\n health: %g, model: %s, rmode: %d, fx: %d, color: %g %g %g amt: %g\n", pEntity->entindex(), STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname), STRING(pEntity->pev->globalname), STRING(pEntity->pev->target), STRING(pEntity->pev->netname), pEntity->pev->health, STRING(pEntity->pev->model), pEntity->pev->rendermode, pEntity->pev->renderfx, pEntity->pev->rendercolor.x, pEntity->pev->rendercolor.y, pEntity->pev->rendercolor.z, pEntity->pev->renderamt);
}

//-----------------------------------------------------------------------------
// Purpose: precache all resources needed by the specified material
// Input  : *pMaterial
//-----------------------------------------------------------------------------
void UTIL_PrecacheMaterial(material_t *pMaterial)
{
	if (pMaterial == NULL)
		return;

	int i;

//	pStringList = pMaterial->ShardSounds;
//	count = ARRAYSIZE(*pStringList);// pMaterial->ShardSoundsNum
	for(i = 0; i < NUM_SHARD_SOUNDS; ++i)
		PRECACHE_SOUND((char *)pMaterial->ShardSounds[i]);

//	pStringList = pMaterial->BreakSounds;
//	count = ARRAYSIZE(*pStringList);// pMaterial->BreakSoundsNum
	for(i = 0; i < NUM_BREAK_SOUNDS; ++i)
		PRECACHE_SOUND((char *)pMaterial->BreakSounds[i]);

//	count = ARRAYSIZE(*pStringList);// pMaterial->PushSoundsNum
	for(i = 0; i < NUM_PUSH_SOUNDS; ++i)
		PRECACHE_SOUND((char *)pMaterial->PushSounds[i]);

//	count = ARRAYSIZE(*pStringList);// pMaterial->BreakStepSounds
	for(i = 0; i < NUM_STEP_SOUNDS; ++i)
		PRECACHE_SOUND((char *)pMaterial->StepSounds[i]);
}

//-----------------------------------------------------------------------------
// Purpose: returns players and bots as edict_t pointer
// Input  : playerIndex - player/entity index
//-----------------------------------------------------------------------------
edict_t	*UTIL_ClientEdictByIndex(int playerIndex)
{
	if (playerIndex > 0 && playerIndex <= gpGlobals->maxClients)
	{
		edict_t *e = INDEXENT(playerIndex);
		if (UTIL_IsValidEntity(e))
			return e;
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: returns players and bots as CBasePlayer pointer
// Input  : playerIndex - player/entity index
//-----------------------------------------------------------------------------
CBasePlayer	*UTIL_ClientByIndex(int playerIndex)
{
	if (playerIndex > 0 && playerIndex <= gpGlobals->maxClients)
	{
		edict_t *pPlayerEdict = INDEXENT(playerIndex);
		if (UTIL_IsValidEntity(pPlayerEdict))
			return (CBasePlayer *)CBasePlayer::Instance(pPlayerEdict);
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: returns entity
// Input  : index - entity index
//-----------------------------------------------------------------------------
CBaseEntity	*UTIL_EntityByIndex(int index)
{
	if (index > 0 && index <= gpGlobals->maxEntities)
	{
		edict_t *e = INDEXENT(index);
		if (UTIL_IsValidEntity(e))
			return CBaseEntity::Instance(e);// XDM3035c: TESTME
	}
	return NULL;
}


//-----------------------------------------------------------------------------
// Purpose: Edict version
// Input  : *pent - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool UTIL_IsValidEntity(edict_t *pent)
{
	if (FNullEnt(pent) || pent->free || (pent->v.flags & FL_KILLME))// XDM3037
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Validate entity by all means! MUST BE BULLETPROOF!
// Input  : *pEntity - test subject
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool UTIL_IsValidEntity(CBaseEntity *pEntity)
{
	try
	{
		if (pEntity)
		{
			if (pEntity->pev == NULL)
				return false;
			if (pEntity->pev->flags & FL_KILLME)
				return false;
			if (pEntity->edict() == NULL)
				return false;
			if (pEntity->edict()->free)
				return false;

			return true;
		}
	}
	catch (...)
	{
		SERVER_PRINT("ERROR: UTIL_IsValidEntity() exception!\n");
		DBG_FORCEBREAK
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: same as fopen, but no need to include gamedir
// Input  : *name - 'filename.ext'
//			*mode - 'r'
// Output : FILE
//-----------------------------------------------------------------------------
FILE *LoadFile(const char *name, const char *mode)
{
	FILE *f = NULL;
	char gamedir[MAX_PATH];
	GET_GAME_DIR(gamedir);
	char file[MAX_PATH];
//	sprintf(file, "%s%s%s", gamedir, PATHSEPARATOR, name);
#ifdef	WIN32
	sprintf(file, "%s\\%s", gamedir, name);
#else	// UNIX/LINUX
	sprintf(file, "%s/%s", gamedir, name);
#endif	// WIN32

	f = fopen(file, mode);
	if (f == NULL)
	{
		ALERT(at_console, "ERROR: Unable to load file '%s'!\n", name);
//wtf		fclose(f);
		return NULL;
	}
	return f;
}

//-----------------------------------------------------------------------------
// Purpose: Generate config file by cvars in list (plain string-by-string fmt)
// Input  : *listfilename - profile.lst
//			*configfilename - profile.cfg
// Output : int 0 = failure
//-----------------------------------------------------------------------------
int CONFIG_GenerateFromList(const char *listfilename, const char *configfilename)
{
	FILE *f = LoadFile(listfilename, "rt");
	if (f == NULL)
	{
		ALERT(at_console, "Unable to load profile list file!\n");
		return 0;
	}

	FILE *cfg = LoadFile(configfilename, "wt");
	if (cfg == NULL)
	{
		ALERT(at_console, "Unable to load profile config file %s for writing!\n", configfilename);
		fclose(f);// don't forget to close previous file
		return 0;
	}

	char cur_str[MAX_PATH];
	fprintf(cfg, "// Profile config file for %s\n", configfilename);
	while(!feof(f))
	{
		if (fscanf(f, "%s\n", cur_str) <= 0)
		{
			ALERT(at_console, "ProfileSave: fscanf error!\n");
			break;
		}

		if (cur_str[0] == '/' && cur_str[1] == '/')
		{
			fprintf(cfg, "%s\n", cur_str);
			continue;// write original comment lines
		}
		if (cur_str[0] == NULL)
		{
			fprintf(cfg, "\n");
			continue;// write original blank lines
		}

		fprintf(cfg, "%s \"%s\"\n", cur_str, CVAR_GET_STRING(cur_str));
	}
	fclose(f);
	fclose(cfg);
	ALERT(at_console, "Profile saved to config: %s\n", configfilename);
	return 1;
}

//-----------------------------------------------------------------------------
// "Valve script" parsing routines
//-----------------------------------------------------------------------------
#define SCRIPT_VERSION				1.0f
#ifdef CLIENT_DLL
#define SCRIPT_DESCRIPTION_TYPE		"INFO_OPTIONS"
#else
#define SCRIPT_DESCRIPTION_TYPE		"SERVER_OPTIONS"
#endif

enum objtype_t
{
	O_BADTYPE,
	O_BOOL,
	O_NUMBER,
	O_LIST,
	O_STRING
};

typedef struct
{
	objtype_t type;
	char szDescription[32];
} objtypedesc_t;

objtypedesc_t objtypes[] =
{
	{ O_BOOL  , "BOOL" }, 
	{ O_NUMBER, "NUMBER" }, 
	{ O_LIST  , "LIST" }, 
	{ O_STRING, "STRING" }
};

//-----------------------------------------------------------------------------
// Purpose: Recognize block type by its text description
// Input  : *pszType - "LIST"
// Output : objtype_t
//-----------------------------------------------------------------------------
objtype_t CONFIG_ParseScriptObjectType(char *pszType)
{
	int nTypes = sizeof(objtypes)/sizeof(objtypedesc_t);
	for (int i = 0; i < nTypes; ++i)
	{
		if (!stricmp(objtypes[i].szDescription, pszType))
			return objtypes[i].type;
	}
	return O_BADTYPE;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pBuffer - file buffer
//			*pName - output pointer
//			*pValue - output pointer
//			*pDescription - output pointer
//			&handled - output 1 = success, 0 = failure
// Output : char * - remaining file buffer
//-----------------------------------------------------------------------------
char *CONFIG_ParseScriptObject(char *pBuffer, char *pName, char *pValue, char *pDescription, byte &handled)
{
//	pOutput->fHandled = false;
	handled = 0;
	pBuffer = COM_Parse(pBuffer);
	char *token = COM_Token();
	if (strlen(token) <= 0)
		return pBuffer;

//	pOutput->szKeyName = token;// pointer must last outside
	strcpy(pName, token);

	// Parse the {
	pBuffer = COM_Parse(pBuffer);
	token = COM_Token();
	if (strlen(token) <= 0)
		return pBuffer;

	if (strcmp(token, "{"))
	{
		ALERT(at_console, "Expecting '{', got '%s'", token);
		return false;
	}

	// Parse the Prompt
	pBuffer = COM_Parse(pBuffer);
	token = COM_Token();
	if (strlen(token) <= 0)
		return pBuffer;

//	pOutput->szClassName = token;
	strcpy(pDescription, token);

	// Parse the next {
	pBuffer = COM_Parse(pBuffer);
	token = COM_Token();
	if (strlen(token) <= 0)
		return pBuffer;

	if (strcmp(token, "{"))
	{
		ALERT(at_console, "Expecting '{', got '%s'", token);
		return false;
	}

	// Now parse the type:
	pBuffer = COM_Parse(pBuffer);
	token = COM_Token();
	if (strlen(token) <= 0)
		return pBuffer;

	objtype_t type = CONFIG_ParseScriptObjectType(token);
	if (type == O_BADTYPE)
	{
		ALERT(at_console, "Type '%s' unknown", token);
		return false;
	}

	switch (type)
	{
	case O_BOOL:
		// Parse the next {
		pBuffer = COM_Parse(pBuffer);
		token = COM_Token();
		if (strlen(token) <= 0)
			return pBuffer;

		if (strcmp(token, "}"))
		{
			ALERT(at_console, "Expecting '{', got '%s'", token);
			return false;
		}
		break;
	case O_NUMBER:
		// Parse the Min
		pBuffer = COM_Parse(pBuffer);
		token = COM_Token();
		if (strlen(token) <= 0)
			return pBuffer;

//		fMin = (float)atof(token);
		// Parse the Max
		pBuffer = COM_Parse(pBuffer);
		token = COM_Token();
		if (strlen(token) <= 0)
			return pBuffer;

//		fMax = (float)atof(token);
		// Parse the next {
		pBuffer = COM_Parse(pBuffer);
		token = COM_Token();
		if (strlen(token) <= 0)
			return pBuffer;

		if (strcmp(token, "}"))
		{
			ALERT(at_console, "Expecting '{', got '%s'", token);
			return false;
		}
		break;
	case O_STRING:
		// Parse the next {
		pBuffer = COM_Parse(pBuffer);
		token = COM_Token();
		if (strlen(token) <= 0)
			return pBuffer;

		if (strcmp(token, "}"))
		{
			ALERT(at_console, "Expecting '{', got '%s'", token);
			return false;
		}
		break;
	case O_LIST:
		while (pBuffer)// XDM3035c: was (1)// Parse items until we get the }
		{
			// Parse the next {
			pBuffer = COM_Parse(pBuffer);
			token = COM_Token();
			if (strlen(token) <= 0)
				return pBuffer;

			if (!strcmp(token, "}"))
				break;

			// Add the item to a list somewhere
			// AddItem( token )
/*			char strItem[128];
			char strValue[128];
			strcpy(strItem, token);*/

			// Parse the value
			pBuffer = COM_Parse(pBuffer);
			token = COM_Token();
			if (strlen(token) <= 0)
				return pBuffer;

//			strcpy(strValue, token);
//			CScriptListItem *pItem = new CScriptListItem(strItem, strValue);
//			AddItem(pItem);
		}
		break;
	}

	// Now read in the default value

	// Parse the {
	pBuffer = COM_Parse(pBuffer);
	token = COM_Token();
	if (strlen(token) <= 0)
		return pBuffer;

	if (strcmp(token, "{"))
	{
		ALERT(at_console, "Expecting '{', got '%s'", token);
		return false;
	}

	// Parse the default
	pBuffer = COM_Parse(pBuffer);
	token = COM_Token();
//	if (strlen(token) <= 0)
//		return pBuffer;

	// Set the values
//	pOutput->szValue = token;// pointer lasts outside
	strcpy(pValue, token);
//	fdefValue = (float)atof(token);

	// Parse the }
	pBuffer = COM_Parse(pBuffer);
	token = COM_Token();
	if (strlen(token) <= 0)
		return pBuffer;

	if (strcmp(token, "}"))
	{
		ALERT(at_console, "Expecting '{', got '%s'", token);
		return false;
	}

	// Parse the final }
	pBuffer = COM_Parse(pBuffer);
	token = COM_Token();
	if (strlen(token) <= 0)
		return pBuffer;

	if (!stricmp(token, "SetInfo"))
	{
//		bSetInfo = true;
		// Parse the final }
		pBuffer = COM_Parse(pBuffer);
		token = COM_Token();
		if (strlen(token) <= 0)
			return pBuffer;
	}

	if (strcmp(token, "}"))
	{
		ALERT(at_console, "Expecting '{', got '%s'", token);
		return pBuffer;
	}
	handled = 1;
//	pOutput->fHandled = true;
	return pBuffer;
}


//-----------------------------------------------------------------------------
// Purpose: Generate config file by cvars in "script" (valve {{}} fmt)
// Input  : *templatefilename - settings.scr
//			*configfilename - settings.cfg
// Output : int 0 = failure
//-----------------------------------------------------------------------------
int CONFIG_GenerateFromTemplate(const char *templatefilename, const char *configfilename)
{
	int length = 0;
	char *pData;
	char *aFileStart = pData = (char *)LOAD_FILE_FOR_ME((char *)templatefilename, &length);
	FILE *cfg = NULL;
	float fVer = 0.0f;
	int ret = 0;

	// Get the first token.
	pData = COM_Parse(pData);
	char *token = COM_Token();
	if (strlen(token) <= 0)
		goto finish;

	// Read VERSION #
	if (stricmp(token, "VERSION"))
	{
		ALERT(at_console, "Expecting 'VERSION', got '%s' in %s\n", token, templatefilename);
		goto finish;
	}

	// Parse in the version #
	// Get the first token.
	pData = COM_Parse(pData);
	token = COM_Token();
	if (strlen(token) <= 0)
	{
		ALERT(at_console, "Expecting version #");
		goto finish;
	}

	fVer = (float)atof(token);
	if (fVer != SCRIPT_VERSION)
	{
		ALERT(at_console, "Version mismatch, expecting %f, got %f in %s\n", SCRIPT_VERSION, fVer, templatefilename);
		goto finish;
	}

	// Get the "DESCRIPTION"
	pData = COM_Parse(pData);
	token = COM_Token();
	if (strlen(token) <= 0)
		goto finish;

	// Read DESCRIPTION
	if (stricmp(token, "DESCRIPTION"))
	{
		ALERT(at_console, "Expecting 'DESCRIPTION', got '%s' in %s\n", token, templatefilename);
		goto finish;
	}

	// Parse in the description type
	pData = COM_Parse(pData);
	token = COM_Token();
	if (strlen(token) <= 0)
	{
		ALERT(at_console, "Expecting '%s'", SCRIPT_DESCRIPTION_TYPE);
		goto finish;
	}

	if (stricmp(token, SCRIPT_DESCRIPTION_TYPE))
	{
		ALERT(at_console, "Expecting %s, got %s in %s\n", SCRIPT_DESCRIPTION_TYPE, token, templatefilename);
		goto finish;
	}

	// Parse the {
	pData = COM_Parse(pData);
	token = COM_Token();
	if (strlen(token) <= 0)
		goto finish;

	if (strcmp(token, "{"))
	{
		ALERT(at_console, "Expecting '{', got '%s'", token);
		goto finish;
	}

	cfg = LoadFile(configfilename, "wt");
	if (cfg == NULL)
	{
		ALERT(at_console, "Unable to load profile config file %s for writing!\n", configfilename);
		goto finish;
	}

	char *pStart;
	byte cvHandled;
	char cvName[64];
	char cvValue[192];
	char cvDescription[256];
	while (pData)// while(1) TESTME
	{
		pStart = pData;// remember start of the block

		pData = COM_Parse(pData);
		token = COM_Token();
		if (strlen(token) <= 0)
			goto finish;

		if (!stricmp(token, "}"))// EOF
			break;

		// Create a new object
		pData = CONFIG_ParseScriptObject(pStart, cvName, cvValue, cvDescription, cvHandled);
		// Get value and write it to the config file
		if (cvHandled)//KVD.fHandled)
		{
			fprintf(cfg, "%s \"%s\"// %s (%s)\n", cvName, CVAR_GET_STRING(cvName), cvDescription, cvValue);
			++ret;
		}
		else// there was some error
		{
			ALERT(at_console, "Some cvar(s) left unread in %s!\n", templatefilename);
			goto finish;// since we cannot possibly find the end of this block or anythin else now
		}
#ifdef _DEBUG
		if (ret >= 1000)
			ALERT(at_logged, "Possible endless loop in %s!\n", templatefilename);
#endif
	}

finish:
	if (aFileStart)
		FREE_FILE(aFileStart);
	if (cfg)
		fclose(cfg);// don't forget to close previous file

	return ret;
}

//-----------------------------------------------------------------------------
// Purpose: returns the next item in this file, and bumps the buffer pointer accordingly
// Input  : *dest - out
//			**line - from fgets()?
//			delimiter - '=' for .ini files
//			comment - '/'
//-----------------------------------------------------------------------------
/*void ParseNextItem(char *dest, char **line, char delimiter, char comment)
{
	while(**line != delimiter && **line && **line != comment && !isspace(**line))
	{
		*dest = **line;
		dest++;
		(*line)++;
	}
	if ( **line != comment && strlen(*line) != 0)
		(*line)++; //preserve comments
	*dest = '\0';	//add null terminator
}
*/
//const char separators[] = "\"\n";

//-----------------------------------------------------------------------------
// Purpose: Another method of getting key-value pair from a script.
// Input  : name - file name (not absolute path!)
//			*kvcallback - function which handles KV pair
//-----------------------------------------------------------------------------
/*void ParseFileKV(const char *name, void (*kvcallback) (char *key, char *value, unsigned short structurestate))
{
	FILE *pFile = LoadFile(name, "rt");
	if (!pFile)// error message already displayed
		return;

	char str[256];
	char *param = NULL;
	char *value = NULL;
	unsigned short structurestate = 0;
	unsigned int line = 0;
	while (!feof(pFile))
	{
		param = NULL;
		value = NULL;
		fgets(str, 256, pFile);
		line ++;

		if (str[0] == '/' || str[0] == ';' || str[0] == '#')
			continue;

		if (str[0] == '{')
		{
			if (structurestate == 0)
				structurestate = 1;
			else
				ALERT(at_console, "WARNING: Found unexpected '{' while parsing '%s' (line %d)!\n", name, line);

			continue;
		}

		if (str[0] == '}')
		{
			if (structurestate == 1)
				structurestate = 2;
			else
				ALERT(at_console, "WARNING: Found unexpected '}' while parsing '%s' (line %d)!\n", name, line);

//			continue;
		}
		else
		{
			param = strtok(str, separators);
			if (param == NULL)
				break;

			strtok(NULL, separators);

			value = strtok(NULL, separators);
			if (value == NULL)
				continue;
		}

		kvcallback(param, value, structurestate);

		if (structurestate == 2)
			structurestate = 0;
	}
	fclose(pFile);
}*/

// EjectBrass - tosses a brass shell from passed origin at passed velocity
//void EjectBrass( const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype )
/*
void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int body, int soundtype)
{
	// FIX: when the player shoots, their gun isn't in the same position as it is on the model other players see.
//	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
//		WRITE_BYTE(TE_MODEL);
	MESSAGE_BEGIN(MSG_PVS, gmsgTEModel, vecOrigin);
		WRITE_COORD(vecOrigin.x);
		WRITE_COORD(vecOrigin.y);
		WRITE_COORD(vecOrigin.z);
		WRITE_COORD(vecVelocity.x);
		WRITE_COORD(vecVelocity.y);
		WRITE_COORD(vecVelocity.z);
		WRITE_ANGLE(rotation);
		WRITE_SHORT(model);
		WRITE_BYTE(body);
		WRITE_BYTE(soundtype);
		WRITE_BYTE(25);// 2.5 seconds
	MESSAGE_END();
}*/

#ifdef _WIN32
#include <io.h>
#endif
static const int TMP_LEN = 256;

unsigned int UTIL_ListFiles(const char *search)
{
#ifdef _WIN32
	if (search == NULL)
		return 0;

	char tmp[TMP_LEN];
	GET_GAME_DIR(tmp);//	strncpy(tmp, gEngfuncs.pfnGetGameDirectory(), TMP_LEN);

	strncat(tmp, PATHSEPARATOR, TMP_LEN);
	strncat(tmp, search, TMP_LEN);
//	CON_PRINTF(">>> %s\n", tmp);

	unsigned int count = 0;
	long hFile = 0;
	_finddata_t fdata;

	ALERT(at_console, "--- Listing of \"%s\" ---\n NAME\t\t\tSIZE\n", search);
	if ((hFile = _findfirst(tmp, &fdata)) == -1L)
		ALERT(at_console, "Nothing found for \"%s\"\n", search);
	else
	{
		do// Find the rest of the files
		{
			ALERT(at_console, " %-32s %8d B\n", fdata.name, fdata.size);// nice format, but only useful with constant width fonts
			++count;
		}
		while (_findnext(hFile, &fdata) == 0);
		ALERT(at_console, "--- %u items ---\n", count);
		_findclose(hFile);
	}
	return count;
#else
/*
	too complicated to write in 10 minutes

	if (strcmp(name,".") == 0 || strcmp(name,"..") == 0) return 0;

	n = scandir(dir, &data->namelist, FileSelect, alphasort);
	if (n < 0)
	{
		return 0;
	}*/

	ALERT(at_console, "UTIL_ListFiles() doesn't work in linux\n");
	return 0;
#endif
}

// X:\...\Half-Life\XDM\searchdir\searchname.searchext
/*int UTIL_ListFiles(const char *searchdir, const char *searchname, const char *searchext)
{
#ifdef _WIN32
	char tmp[TMP_LEN];
//	strncpy(tmp, gEngfuncs.pfnGetGameDirectory(), TMP_LEN);
	GET_GAME_DIR(tmp);

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

	ALERT(at_console, "--- Searching for %s files ---\n NAME\t\tSIZE\n", searchext);
	if ((hFile = _findfirst(tmp, &fdata)) == -1L)
		ALERT(at_console, "No %s files in %s directory!\n", searchext, searchdir);
	else
	{
		do// Find the rest of the files
		{
			ALERT(at_console, " %s\t\t%d\n", fdata.name, fdata.size);
			count ++;
		}
		while (_findnext(hFile, &fdata) == 0);
		ALERT(at_console, "--- %d %s files found ---\n", count, searchext);
		_findclose(hFile);
	}
	return count;
#else
	ALERT(at_console, "UTIL_ListFiles() doesn't work in linux\n");
	return 0;
#endif
}*/

int RANDOM_INT2(int a, int b)
{
	if (RANDOM_LONG(0,1) == 1)
		return b;

	return a;
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
// Purpose: Parse RGBA from string
// Input  : *str - "1 1 1 1"
//			&r &g &b &a - output
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool StringToRGBA(const char *str, float &r, float &g, float &b, float &a)
{
//	float cr,cg,cb,ca;
//	if (str && sscanf(str, "%f %f %f %f", &cr, &cg, &cb, &ca) == 4)
	if (str && sscanf(str, "%f %f %f %f", &r, &g, &b, &a) == 4)
	{
//		r = cr; g = cg; b = cb; a = ca;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// SHL - randomized vectors of the form "x y z .. a b c"
// Purpose: LRC shure did write a lot of useless code...
// Output : Returns true on success, false on failure (and also zeroes the output vector).
//-----------------------------------------------------------------------------
bool UTIL_StringToRandomVector(float *pVector, const char *str)
{
	float pVecMin[3];
	float pVecMax[3];
	int fields = sscanf(str, "%f %f %f .. %f %f %f", &pVecMin[0], &pVecMin[1], &pVecMin[2], &pVecMax[0], &pVecMax[1], &pVecMax[2]);
	if (fields == 6)
	{
		pVector[0] = RANDOM_FLOAT(pVecMin[0], pVecMax[0]);
		pVector[1] = RANDOM_FLOAT(pVecMin[1], pVecMax[1]);
		pVector[2] = RANDOM_FLOAT(pVecMin[2], pVecMax[2]);
		return true;
	}
/*	if (StringToVec(str, pVecMax))
	{
		pVector[0] = RANDOM_FLOAT(0, pVecMax[0]);
		pVector[1] = RANDOM_FLOAT(0, pVecMax[1]);
		pVector[2] = RANDOM_FLOAT(0, pVecMax[2]);
	}*/
	else
	{
		for (int j = 0; j < 3; ++j)
			pVector[j] = 0.0f;
		return false;
	}
}


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

	if (s || l)// SL requested
	{
		s = 0.0f;
		l = (*pmin + *pmax) / 2.0f;

		if (l > 0.0f && l < 1.0f)
			s = delta / (l < 0.5f ? (2.0f*l) : (2.0f - 2.0f*l));
	}
/*
	/double/ float themin,themax,delta;
	themin = min(r,min(g,b));
	themax = max(r,max(g,b));
	delta = themax - themin;

	if (oh)// H requested
	{
		h = 0;
		if (delta > 0.0f)
		{
			if (themax == r && themax != g)
				h += (g - b) / delta;
			if (themax == g && themax != b)
				h += (2 + (b - r) / delta);
			if (themax == b && themax != r)
				h += (4 + (r - g) / delta);

			h *= 60.0f;
		}
	}

	if (os && ol)// SL requested
	{
		s = 0;
		l = (themin + themax) / 2.0f;

		if (l > 0.0f && l < 1.0f)
			s = delta / (l < 0.5f ? (2.0f*l) : (2.0f - 2.0f*l));
	}*/
}

// h,s,l 0...255; must be not null to be calculated
void RGB2HSL(byte &rb, byte &gb, byte &bb, float &h, float &s, float &l)
{
	float r = (float)rb/255.0f;
	float g = (float)gb/255.0f;
	float b = (float)bb/255.0f;
	RGB2HSL(r,g,b, h,s,l);// try rewriting this algorithm for bytes
}

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
		while (h < 0)
			h += 360;
		while (h > 360)
			h -= 360;

		if (h == 1.0f)
			h = 0.0f;

		h *= 6.0f;
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
// Purpose: FIX: hammer and other poorly-written stuff may set color to 0,0,0 by default
// Warning: Use only in KeyValue/Spawn functions!
// Input  : &rendermode - 
//			*rendercolor - 
//-----------------------------------------------------------------------------
void UTIL_FixRenderColor(const int &rendermode, float *rendercolor)
{
	if (rendermode == kRenderTransAdd || rendermode == kRenderTransTexture || rendermode == kRenderGlow)
	{
		if (rendercolor[0] == 0.0f && rendercolor[1] == 0.0f && rendercolor[2] == 0.0f)
		{
			rendercolor[0] = 255.0f;
			rendercolor[1] = 255.0f;
			rendercolor[2] = 255.0f;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: GetWeaponWorldScale. May get called while g_pGameRules is NULL!
// Output : float
//-----------------------------------------------------------------------------
float UTIL_GetWeaponWorldScale(void)
{
	return 1.0f;
}


//-----------------------------------------------------------------------------
// Purpose: Use this instead of SET_VIEW!
// Input  : *pClient - 
//			*pViewent - 
//-----------------------------------------------------------------------------
void UTIL_SetView(edict_t *pClient, edict_t *pViewent)
{
	if (pClient && UTIL_IsValidEntity(pViewent))
	{
		SET_VIEW(pClient, pViewent);
		if (pClient == pViewent)
			pClient->v.euser2 = NULL;
		else
			pClient->v.euser2 = pViewent;
	}
}


enum
{
	BUBBLES_TYPE_POINT = 0,
	BUBBLES_TYPE_SPHERE,
	BUBBLES_TYPE_BOX,
	BUBBLES_TYPE_LINE
};

void FX_BubblesPoint(const Vector &center, const Vector &spread, int count)
{
	MESSAGE_BEGIN(MSG_PVS, gmsgBubbles, center);
		WRITE_BYTE(BUBBLES_TYPE_POINT);
		WRITE_BYTE(count);
		WRITE_COORD(center.x);
		WRITE_COORD(center.y);
		WRITE_COORD(center.z);
		WRITE_COORD(spread.x);
		WRITE_COORD(spread.y);
		WRITE_COORD(spread.z);
	MESSAGE_END();
}

void FX_BubblesSphere(const Vector &center, float radius, int count)
{
	MESSAGE_BEGIN(MSG_PVS, gmsgBubbles, center);
		WRITE_BYTE(BUBBLES_TYPE_SPHERE);
		WRITE_BYTE(count);
		WRITE_COORD(center.x);
		WRITE_COORD(center.y);
		WRITE_COORD(center.z);
		WRITE_COORD(0);
		WRITE_COORD(0);
		WRITE_COORD(radius);
	MESSAGE_END();
}

void FX_BubblesBox(const Vector &center, const Vector &halfbox, int count)
{
	MESSAGE_BEGIN(MSG_PVS, gmsgBubbles, center);
		WRITE_BYTE(BUBBLES_TYPE_BOX);
		WRITE_BYTE(count);
		WRITE_COORD(center.x);
		WRITE_COORD(center.y);
		WRITE_COORD(center.z);
		WRITE_COORD(halfbox.x);
		WRITE_COORD(halfbox.y);
		WRITE_COORD(halfbox.z);
	MESSAGE_END();
}

void FX_BubblesLine(const Vector &start, const Vector &end, int count)
{
	MESSAGE_BEGIN(MSG_PVS, gmsgBubbles, (start+end)/2);
		WRITE_BYTE(BUBBLES_TYPE_LINE);
		WRITE_BYTE(count);
		WRITE_COORD(start.x);
		WRITE_COORD(start.y);
		WRITE_COORD(start.z);
		WRITE_COORD(end.x);
		WRITE_COORD(end.y);
		WRITE_COORD(end.z);
	MESSAGE_END();
}







//-----------------------------------------------------------------------------
// Purpose: Returns constant string for USE_TYPE (for debugging purposes?)
// Input  : useType -
// Output : char
//-----------------------------------------------------------------------------
char *GetStringForUseType(USE_TYPE useType)
{
	switch(useType)
	{
	case USE_OFF: return "USE_OFF";
	case USE_ON: return "USE_ON";
	case USE_SET: return "USE_SET";
	case USE_TOGGLE: return "USE_TOGGLE";
	case USE_KILL: return "USE_KILL";
	case USE_SAME: return "USE_SAME";
	case USE_NOT: return "USE_NOT";
	default: return "USE_UNKNOWN";
	}
}

//-----------------------------------------------------------------------------
// Purpose: one of shitty SHL things
// Input  : state - 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetStringForState(STATE state)
{
	switch (state)
	{
	case STATE_ON: return "ON";
	case STATE_OFF: return "OFF";
	case STATE_TURN_ON: return "TURN ON";
	case STATE_TURN_OFF: return "TURN OFF";
	case STATE_IN_USE: return "IN USE";
//	case STATE_DEAD: return "DEAD";
	default: return "UNKNOWN";
	}
}

//-----------------------------------------------------------------------------
// Purpose: one of shitty SHL things
// Input  : *string - 
// Output : STATE
//-----------------------------------------------------------------------------
STATE GetStateForString(const char *string)
{
	if (!stricmp(string, "ON"))
		return STATE_ON;
	else if (!stricmp(string, "OFF"))
		return STATE_OFF;
	else if (!stricmp(string, "TURN ON"))
		return STATE_TURN_ON;
	else if (!stricmp(string, "TURN OFF"))
		return STATE_TURN_OFF;
	else if (!stricmp(string, "IN USE"))
		return STATE_IN_USE;
//	else if (!stricmp(string, "DEAD"))
//		return STATE_DEAD;
	else if(isdigit(string[0]))
		return (STATE)atoi(string);

	// assume error
#ifdef _DEBUG
	ALERT(at_error, "GetStateForString(%s): unknown state!\n", string);
#endif
	return (STATE)-1;
}
