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
// Misc utility code
//
#ifndef	UTIL_H
#define	UTIL_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include "activity.h"
#include "enginecallback.h"
#include "protocol.h"
//#include "gloabls.h"

extern DLL_GLOBAL globalvars_t *gpGlobals;

class CBaseEntity;
class CBasePlayer;
class CBasePlayerItem;


//#define cchMapNameMost 32

// Dot products for view cone checking
#define VIEW_FIELD_FULL		(float)-1.0 // +-180 degrees
#define	VIEW_FIELD_WIDE		(float)-0.7 // +-135 degrees 0.1 // +-85 degrees, used for full FOV checks
#define	VIEW_FIELD_NARROW	(float)0.7 // +-45 degrees, more narrow check used to set up ranged attacks
#define	VIEW_FIELD_QUITE_NARROW	(float)0.83 // +-30 degrees, more narrow check used to set up ranged attacks
#define	VIEW_FIELD_ULTRA_NARROW	(float)0.9 // +-25 degrees, more narrow check used to set up ranged attacks

// All monsters need this data
#define	DONT_BLEED			-1
#define	BLOOD_COLOR_RED			(byte)247//(104,0,0)
#define	BLOOD_COLOR_YELLOW		(byte)192//(255,255,0)
#define	BLOOD_COLOR_GREEN		(byte)216//(0,255,0)
#define	BLOOD_COLOR_BLUE		(byte)208//(0,0,255)

//#ifdef PROTOCOL_H
#define SVC_TEMPENTITY		svc_temp_entity//23
#define SVC_INTERMISSION	svc_intermission//30
#define SVC_CDTRACK			svc_cdtrack//32
#define SVC_WEAPONANIM		svc_weaponanim//35
#define SVC_ROOMTYPE		svc_roomtype//37
#define SVC_HLTV			svc_hltv//50
#define SVC_DIRECTOR		svc_director//51
/*#else// hardcode
#define SVC_TEMPENTITY		23
#define SVC_INTERMISSION	30
#define SVC_CDTRACK			32
#define SVC_WEAPONANIM		35
#define SVC_ROOMTYPE		37
#define SVC_HLTV			50
#define SVC_DIRECTOR		51
#endif*/

// triggers
#define	SF_TRIGGER_ALLOWMONSTERS	1// monsters allowed to fire this trigger
#define	SF_TRIGGER_NOCLIENTS		2// players not allowed to fire this trigger
#define SF_TRIGGER_PUSHABLES		4// only pushables can fire this trigger
#define SF_TRIGGER_EVERYTHING		8// everything else can fire this trigger (e.g. gibs, rockets)
#define SF_TRIGGER_CLEARVELOCITY	16// XDM: clear velocity
#define SF_TRIGGER_KEEPANGLES		32// XDM: keep view angles
#define SF_TRIGGER_SOUNDACTIVATE	64// XDM: activated by sound
#define SF_TRIGGER_START_OFF		128// XDM3035c

#define SF_LIGHT_START_OFF			1

#define SF_TRIG_PUSH_ONCE			1

// when we are within this close to running out of entities,  items
// marked with the ITEM_FLAG_LIMITINWORLD will delay their respawn
#define ENTITY_INTOLERANCE	100

#ifndef SWAP
#define SWAP(a,b,temp)	((temp)=(a),(a)=(b),(b)=(temp))
#endif

// Use this instead of ALLOC_STRING on constant strings
#define STRING(offset)		(const char *)(gpGlobals->pStringBase + (int)offset)
#define STRINGV(offset)		(char *)(gpGlobals->pStringBase + (int)offset)
#define MAKE_STRING(str)	((int)str - (int)STRING(0))

// Keeps clutter down a bit, when writing key-value pairs
/*
#define WRITEKEY_INT(pf, szKeyName, iKeyValue)			ENGINE_FPRINTF(pf, "\"%s\" \"%d\"\n", szKeyName, iKeyValue)
#define WRITEKEY_FLOAT(pf, szKeyName, flKeyValue)		ENGINE_FPRINTF(pf, "\"%s\" \"%f\"\n", szKeyName, flKeyValue)
#define WRITEKEY_STRING(pf, szKeyName, szKeyValue)		ENGINE_FPRINTF(pf, "\"%s\" \"%s\"\n", szKeyName, szKeyValue)
#define WRITEKEY_VECTOR(pf, szKeyName, flX, flY, flZ)	ENGINE_FPRINTF(pf, "\"%s\" \"%f %f %f\"\n", szKeyName, flX, flY, flZ)
*/

// Keeps clutter down a bit, when using a float as a bit-vector
#define SetBits(flBitVector, bits)		((flBitVector) = (int)(flBitVector) | (bits))
#define ClearBits(flBitVector, bits)	((flBitVector) = (int)(flBitVector) & ~(bits))
#define FBitSet(flBitVector, bit)		((int)(flBitVector) & (bit))

// This is the glue that hooks .MAP entity class names to our CPP classes
// The _declspec forces them to be exported by name so we can do a lookup with GetProcAddress()
// The function is used to intialize / allocate the object for the entity
#define LINK_ENTITY_TO_CLASS(mapClassName,DLLClassName) \
	extern "C" void DLLEXPORT mapClassName(entvars_t *pev); \
	void mapClassName(entvars_t *pev) { GetClassPtr((DLLClassName *)pev); }

//
// Conversion among the three types of "entity", including identity-conversions.
//
#ifdef _DEBUG
	edict_t *DBG_EntOfVars(const entvars_t *pev);
	inline edict_t *ENT(const entvars_t *pev)	{ return DBG_EntOfVars(pev); }
#else
	inline edict_t *ENT(const entvars_t *pev)	{ return pev->pContainingEntity; }
#endif

inline EOFFSET OFFSET(entvars_t *pev)
{
#ifdef _DEBUG
	if (pev == NULL)
		ALERT(at_error, "Bad pev in OFFSET()\n");
#endif
	return OFFSET(ENT(pev));
}

//inline entvars_t *VARS(EOFFSET eoffset) { return VARS(ENT(eoffset)); }

// Testing the three types of "entity" for nullity
#define eoNullEntity 0

inline bool FNullEnt(EOFFSET eoffset) { return eoffset == 0; }
inline bool FNullEnt(const edict_t *pent) { return pent == NULL || FNullEnt(OFFSET(pent)); }
inline bool FNullEnt(entvars_t *pev) { return pev == NULL || FNullEnt(OFFSET(pev)); }

// Testing strings for nullity
#define iStringNull 0

inline bool FStringNull(int iString) { return iString == iStringNull; }
inline bool FStrEq(const char *sz1, const char *sz2) { return (strcmp(sz1, sz2) == 0); }
inline bool FStrnEq(const char *sz1, const char *sz2, int len) { return (strncmp(sz1, sz2, len) == 0); }
inline bool FClassnameIs(edict_t *pent, const char *szClassname) { return FStrEq(STRING(VARS(pent)->classname), szClassname); }
inline bool FClassnameIs(entvars_t *pev, const char *szClassname) { return FStrEq(STRING(pev->classname), szClassname); }

// Misc. Prototypes
void UTIL_SetSize(entvars_t *pev, const Vector &vecMin, const Vector &vecMax);
void UTIL_SetSize(CBaseEntity *pEntity, const Vector &vecMin, const Vector &vecMax);// XDM: WTF? Why extern?
void UTIL_SetSize(CBaseEntity *pEntity, const float &radius);// XDM3034

float UTIL_VecToYaw(const Vector &vec);
float UTIL_AngleMod(float a);
Vector UTIL_VecToAngles(const Vector &vec);
void UTIL_MoveToOrigin(edict_t *pent, const Vector &vecGoal, float flDist, int iMoveType);

// TODO: use UTIL_FindEntityByClassname
inline edict_t *FIND_ENTITY_BY_CLASSNAME(edict_t *entStart, const char *pszName)
{
	return FIND_ENTITY_BY_STRING(entStart, "classname", pszName);
}
// TODO: use UTIL_FindEntityByTargetname
inline edict_t *FIND_ENTITY_BY_TARGETNAME(edict_t *entStart, const char *pszName)
{
	return FIND_ENTITY_BY_STRING(entStart, "targetname", pszName);
}
// TODO: use UTIL_FindEntityByTarget
inline edict_t *FIND_ENTITY_BY_TARGET(edict_t *entStart, const char *pszName)
{
	return FIND_ENTITY_BY_STRING(entStart, "target", pszName);
}

CBaseEntity	*UTIL_FindEntityInSphere(CBaseEntity *pStartEntity, const Vector &vecCenter, float flRadius);
CBaseEntity *UTIL_FindEntityInBox(CBaseEntity *pStartEntity, const Vector &mins, const Vector &maxs);// XDM3037
CBaseEntity	*UTIL_FindEntityByString(CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue);
CBaseEntity	*UTIL_FindEntityByClassname(CBaseEntity *pStartEntity, const char *szName);
CBaseEntity	*UTIL_FindEntityByTargetname(CBaseEntity *pStartEntity, const char *szName);
CBaseEntity *UTIL_FindEntityByTargetname(CBaseEntity *pStartEntity, const char *szName, CBaseEntity *pActivator);
CBaseEntity *UTIL_FindEntityByTarget(CBaseEntity *pStartEntity, const char *szName);
CBaseEntity	*UTIL_FindEntities(CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue, const Vector &vecCenter, float flRadius);
// Pass in an array of pointers and an array size, it fills the array and returns the number inserted
int UTIL_MonstersInSphere(CBaseEntity **pList, int listMax, const Vector &center, float radius);
int UTIL_EntitiesInBox(CBaseEntity **pList, int listMax, const Vector &mins, const Vector &maxs, int flagMask);

inline void UTIL_MakeVectors(const Vector &vecAngles)
{
	MAKE_VECTORS(vecAngles);
}
/*
inline void UTIL_MakeVectorsPrivate(const Vector &vecAngles, float *p_vForward, float *p_vRight, float *p_vUp)
{
	ANGLE_VECTORS(vecAngles, p_vForward, p_vRight, p_vUp);
}
*/

void UTIL_MakeAimVectors(const Vector &vecAngles); // like MakeVectors, but assumes pitch isn't inverted
void UTIL_MakeInvVectors(const Vector &vec, globalvars_t *pgv);

inline void UTIL_SetOrigin(entvars_t *pev, const Vector &vecOrigin)
{
	SET_ORIGIN(ENT(pev), vecOrigin);
}

void UTIL_SetOrigin(CBaseEntity *pEntity, const Vector &vecOrigin);// UNDONE: should be inline
void UTIL_SetAngles(CBaseEntity *pEntity, const Vector &vecAngles);

/*
inline int UTIL_PointContents(const Vector &vec)
{
	return POINT_CONTENTS(vec);
}
*/

void UTIL_EmitAmbientSound(edict_t *entity, const Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch);

void UTIL_ScreenShakeOne(CBaseEntity *pPlayer, const Vector &center, float amplitude, float frequency, float duration);// XDM
void UTIL_ScreenShake(const Vector &center, float amplitude, float frequency, float duration, float radius);
void UTIL_ScreenShakeAll(const Vector &center, float amplitude, float frequency, float duration);
void UTIL_ScreenFade(CBaseEntity *pEntity, const Vector &color, float fadeTime, float fadeHold, int alpha, int flags);
void UTIL_ScreenFadeAll(const Vector &color, float fadeTime, float holdTime, int alpha, int flags);

typedef struct hudtextparms_s
{
	float		x;
	float		y;
	int			effect;
	byte		r1, g1, b1, a1;
	byte		r2, g2, b2, a2;
	float		fadeinTime;
	float		fadeoutTime;
	float		holdTime;
	float		fxTime;
	int			channel;
} hudtextparms_t;

// prints as transparent 'title' to the HUD
void UTIL_HudMessage(CBaseEntity *pEntity, const hudtextparms_t &textparms, const char *pMessage);
void UTIL_HudMessageAll(const hudtextparms_t &textparms, const char *pMessage);

// prints messages through the HUD
void ClientPrint(entvars_t *client, int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL);
// prints a message to each client
void UTIL_ClientPrintAll(int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL);
// prints a message to the HUD say (chat)
//void UTIL_SayText(const char *pText, CBaseEntity *pEntity = NULL, bool reliable = true);
//void UTIL_SayTextAll(const char *pText, CBaseEntity *pEntity = NULL, bool reliable = true);
void UTIL_ShowMessage(const char *pString, CBaseEntity *pPlayer);
void UTIL_ShowMessageAll(const char *pString);

// for handy use with ClientPrint params
char *UTIL_dtos1(int d);

// Writes message to console with timestamp and FragLog header.
void UTIL_LogPrintf(char *fmt, ...);


typedef enum { ignore_monsters=1, dont_ignore_monsters=0, missile=2 } IGNORE_MONSTERS;
typedef enum { ignore_glass=1, dont_ignore_glass=0 } IGNORE_GLASS;

extern void			UTIL_TraceLine			(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr);
extern void			UTIL_TraceLine			(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr);
typedef enum { point_hull=0, human_hull=1, large_hull=2, head_hull=3 } HULLTYPES;
extern void			UTIL_TraceHull			(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr);
extern TraceResult	UTIL_GetGlobalTrace		(void);
extern void			UTIL_TraceModel			(const Vector &vecStart, const Vector &vecEnd, int hullNumber, edict_t *pentModel, TraceResult *ptr);
extern Vector		UTIL_GetAimVector		(edict_t *pent, float flSpeed);
extern int			UTIL_IsMasterTriggered	(string_t sMaster, CBaseEntity *pActivator);

extern void			UTIL_ParticleEffect(const Vector &vecOrigin, const Vector &vecDirection, ULONG ulColor, ULONG ulCount);
extern void			UTIL_BloodStream(const Vector &origin, const Vector &direction, const int &color, const int &amount);
extern void			UTIL_BloodDrips(const Vector &origin, const Vector &direction, const int &color, const int &amount);
extern Vector		UTIL_RandomBloodVector(void);
extern bool			UTIL_ShouldShowBlood(const int &color);
int UTIL_BloodDecalIndex(const int &bloodColor);
void UTIL_BloodDecalTrace(TraceResult *pTrace, const int &bloodColor);
void UTIL_DecalTrace(TraceResult *pTrace, int decalNumber);
extern void			UTIL_PlayerDecalTrace(TraceResult *pTrace, int playernum, int decalNumber, bool bIsCustom);
extern void			UTIL_GunshotDecalTrace(TraceResult *pTrace, int decalNumber);
extern void			UTIL_Sparks(const Vector &position);
extern void			UTIL_Ricochet(const Vector &position, float scale);
//extern void			UTIL_StringToVector(float *pVector, const char *pString);
//extern void			UTIL_StringToIntArray(int *pVector, int count, const char *pString);

Vector		UTIL_ClampVectorToBox(const Vector &input, const Vector &clampSize);
float		UTIL_Approach(const float &target, float value, const float &speed);
float		UTIL_ApproachAngle(float target, float value, float speed);
float		UTIL_AngleDistance(const float &next, const float &cur);
// Use for ease-in, ease-out style interpolation (accel/decel)
float		UTIL_SplineFraction(float value, const float &scale);

char *UTIL_VarArgs(char *format, ...);
void UTIL_Remove(CBaseEntity *pEntity);

// Search for water transition along a vertical line
float UTIL_WaterLevel(const Vector &position, float minz, float maxz);
void UTIL_Bubbles(const Vector &mins, const Vector &maxs, int count);
void UTIL_BubbleTrail(const Vector &from, const Vector &to, int count);
void FX_BubblesPoint(const Vector &center, const Vector &spread, int count);
void FX_BubblesSphere(const Vector &center, float radius, int count);
void FX_BubblesBox(const Vector &center, const Vector &halfbox, int count);
void FX_BubblesLine(const Vector &start, const Vector &end, int count);

// allows precacheing of other entities
void UTIL_PrecacheOther(const char *szClassname);
void UTIL_PrecacheOtherWeapon(const char *szClassname);
void UTIL_PrecacheMaterial(struct material_s *pMaterial);

// returns a CBaseEntity pointer to a player by index.  Only returns if the player is spawned and connected otherwise returns NULL. Index is 1 based
edict_t	*UTIL_ClientEdictByIndex(int playerIndex);
CBasePlayer	*UTIL_ClientByIndex(int playerIndex);
CBaseEntity	*UTIL_EntityByIndex(int index);
bool UTIL_IsValidEntity(edict_t *pent);
bool UTIL_IsValidEntity(CBaseEntity *pEntity);


// Sorta like FInViewCone, but for nonmonsters.
float UTIL_DotPoints(const Vector &vecSrc, const Vector &vecCheck, const Vector &vecDir);
void UTIL_StripToken(const char *pKey, char *pDest);// for redundant keynames

// Misc functions
void SetMovedir(entvars_t *pev);

Vector VecBModelOrigin(entvars_t *pevBModel);
int BuildChangeList(LEVELLIST *pLevelList, int maxList);

// NOTE: use EMIT_SOUND_DYN to set the pitch of a sound. Pitch of 100
// is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
// down to 1 is a lower pitch.   150 to 70 is the realistic range.
// EMIT_SOUND_DYN with pitch != 100 should be used sparingly, as it's not quite as
// fast as EMIT_SOUND (the pitchshift mixer is not native coded).

void EMIT_SOUND_DYN(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int flags, int pitch);

inline void EMIT_SOUND(edict_t *entity, int channel, const char *sample, float volume, float attenuation)
{
	EMIT_SOUND_DYN(entity, channel, sample, volume, attenuation, 0, PITCH_NORM);
}

inline void STOP_SOUND(edict_t *entity, int channel, const char *sample)
{
	EMIT_SOUND_DYN(entity, channel, sample, 0, 0, SND_STOP, PITCH_NORM);
}

void EMIT_SOUND_SUIT(edict_t *entity, const char *sample);
void EMIT_GROUPID_SUIT(edict_t *entity, int isentenceg);
void EMIT_GROUPNAME_SUIT(edict_t *entity, const char *groupname);

#define PRECACHE_SOUND_ARRAY(a) { for (int i = 0; i < ARRAYSIZE(a); ++i) PRECACHE_SOUND((char *)a[i]); }
#define EMIT_SOUND_ARRAY_DYN(chan, array) EMIT_SOUND_DYN(ENT(pev), chan, array[RANDOM_LONG(0,ARRAYSIZE(array)-1)], VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(95,105));
#define RANDOM_SOUND_ARRAY(array) (array)[RANDOM_LONG(0,ARRAYSIZE((array))-1)]

#define PLAYBACK_EVENT(flags, who, index) PLAYBACK_EVENT_FULL(flags, who, index, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0);
//#define PLAYBACK_EVENT_DELAY(flags, who, index, delay) PLAYBACK_EVENT_FULL(flags, who, index, delay, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0);


#define GROUP_OP_AND	0
#define GROUP_OP_NAND	1

extern int g_groupmask;
extern int g_groupop;

class UTIL_GroupTrace
{
public:
	UTIL_GroupTrace(int groupmask, int op);
	~UTIL_GroupTrace(void);

private:
	int m_oldgroupmask, m_oldgroupop;
};

void UTIL_SetGroupTrace(const int &groupmask, const int &op);
void UTIL_UnsetGroupTrace(void);
int UTIL_SharedRandomLong(const unsigned int &seed, const int &low, const int &high);
float UTIL_SharedRandomFloat(const unsigned int &seed, const float &low, const float &high);

float UTIL_WeaponTimeBase(void);

bool IsFacing(const Vector &origin, const Vector &v_angle, const Vector &reference);

int InTransitionVolume(CBaseEntity *pEntity, const char *pVolumeName);

int IsPointEntity(CBaseEntity *pEnt);
int TrainSpeed(const int &iSpeed, const int &iMax);

CBaseEntity *UTIL_FindEntityForward(CBaseEntity *pMe);

//void UTIL_HUDMessage(CBaseEntity *pCaller, int fx, float x, float y, byte r1, byte g1, byte b1, byte a1, byte r2, byte g2, byte b2, byte a2, float fadeinTime, float fadeoutTime, float holdTime, float fxTime, const char *message);
//void UTIL_HUDMessageAll(int fx, float x, float y, byte r1, byte g1, byte b1, byte a1, byte r2, byte g2, byte b2, byte a2, float fadein, float fadeout, float holdtime, float fxtime, const char *message);
void UTIL_ShowMessageRadius(const char *pString, const Vector &center, int radius);

void PlayCDTrack(CBaseEntity *pPlayer, int iTrack, int loop);
void PlayAudioTrack(CBaseEntity *pPlayer, const char *track, int loop);

void FindHullIntersection(const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity);

void ParticlesCustom(const Vector &vecPos, float rnd_vel, float life, byte color_pal, byte number);
void GlowSprite(const Vector &vecPos, int mdl_idx, int life, int scale, int fade);
void SpriteTrail(const Vector &vecPos, const Vector vecEnd, int mdl_idx, int count, int life, int scale, int vel, int rnd_vel);
void DynamicLight(const Vector &vecPos, int radius, int r, int g, int b, int life, int decay);
void EntityLight(int entidx, const Vector &vecPos, int radius, int r, int g, int b, int life, int decay);
void PartSystem(const Vector &vecPos, const Vector &vecDir, const Vector &vecSpreadSize, int sprindex, int rendermode, int type, int max_parts, int life, int flags, int ent);
void BeamEffect(int type, const Vector &vecPos, const Vector &vecAxis, int mdl_idx, int startframe, int fps, int life, int width, int noise, const Vector &color, int brightness, int speed);

int UTIL_BloodToStreak(int color);
void UTIL_BloodAdditive(const Vector &origin, const Vector &velocity, const char *pSpriteName, int brightness, int framerate, float scale);

Vector UTIL_RandomVector(void);
Vector RandomVector(const float &x, const float &y, const float &z);
Vector RandomVector(const Vector &source);
Vector RandomVectors(const Vector &min, const Vector &max);

bool UTIL_LiquidContents(const Vector &vec);
void UTIL_ShowLine(const Vector &start, const Vector &end, float life, byte r, byte g, byte b);
void UTIL_ShowBox(const Vector &origin, const Vector &mins, const Vector &maxs, float life, byte r, byte g, byte b);
void UTIL_DebugBeam(const Vector &vecSrc, const Vector &vecEnd, float life);
//void UTIL_DecalForward(entvars_t *ent, edict_t *pentIgnore, int decalIndex);
void UTIL_DecalPoints(const Vector &src, const Vector &end, edict_t *pent, int decalIndex);

//void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int body, int soundtype);

int GetEntBodyCount(edict_t *ent);

void UTIL_PrintEntInfo(CBaseEntity *pEntity);

float clip(const float &a, const float &min, const float &max);

#define RGBA2INT(r,g,b,a)	((unsigned long)(((byte)(r)|((WORD)((byte)(g))<<8))|(((DWORD)(byte)(b))<<16)|(((DWORD)(byte)(a))<<24)))
#define RGB2INT(r,g,b)		((unsigned long)(((byte)(r)|((WORD)((byte)(g))<<8))|(((DWORD)(byte)(b))<<16)))
#define RGB2R(rgb)			((unsigned char)(rgb))
#define RGB2G(rgb)			((unsigned char)(((unsigned short)(rgb)) >> 8))
#define RGB2B(rgb)			((unsigned char)((rgb)>>16))
#define RGB2A(rgba)			((unsigned char)((rgba)>>24))
#define RGBA2A(rgba)		((unsigned char)((rgba)>>24))

// Extracts "X Y Z" from string. Returns FALSE if cannot extract.
int RANDOM_INT2(int a, int b);
bool StringToVec(const char *str, float *vec);
bool StringToRGB(const char *str, byte &r, byte &g, byte &b);
bool StringToRGBA(const char *str, byte &r, byte &g, byte &b, byte &a);
bool StringToRGBA(const char *str, float &r, float &g, float &b, float &a);
bool UTIL_StringToRandomVector(float *pVector, const char *str);

// XDM3035
void RGB2HSL(float &r, float &g, float &b, float &h, float &s, float &l);// r,g,b 0...1; h,s,l must be != 0 to be calculated
void RGB2HSL(byte &rb, byte &gb, byte &bb, float &h, float &s, float &l);// h,s,l 0...255; must be != 0 to be calculated
void HSL2RGB(float h, float s, float l, float &r, float &g, float &b);// r,g,b 0...1
void HSL2RGB(float h, float s, float l, byte &rb, byte &gb, byte &bb);// r,g,b 0...255

void UTIL_FixRenderColor(const int &rendermode, float *rendercolor);// XDM3035a: fix default zeroes


void StreakSplash(const Vector &origin, const Vector &direction, int color, int count, int speed, int velocityRange);
void ParticleBurst(const Vector &origin, int radius, int color, int duration);

char *memfgets(byte *pMemFile, int fileSize, int &filePos, char *pBuffer, int bufferSize);

FILE *LoadFile(const char *name, const char *mode);
int CONFIG_GenerateFromList(const char *listfilename, const char *configfilename);
int CONFIG_GenerateFromTemplate(const char *templatefilename, const char *configfilename);
//void ParseFileKV(const char *name, void (*kvcallback) (char *key, char *value, unsigned short structurestate));

char *COM_Token(void);
char *COM_Parse(char *data);
int COM_TokenWaiting(char *buffer);
void ExtractCommandString(char *s, char *szCommand);

unsigned int UTIL_ListFiles(const char *search);

float UTIL_GetWeaponWorldScale(void);// XDM3035b

void UTIL_SetView(edict_t *pClient, edict_t *pViewent);// XDM3035c
void UTIL_SetModel( edict_t *e, const char *model );
int UTIL_PrecacheModel( char* s );
#endif // UTIL_H
