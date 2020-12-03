#ifndef	XBM_UTIL_H// don't interfere with server DLL headers
#define	XBM_UTIL_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include "activity.h"
#include "enginecallback.h"
#include "protocol.h"
#include "gamedefs.h"

extern DLL_GLOBAL globalvars_t *gpGlobals;

//class CBaseEntity;

// Dot products for view cone checking
#define VIEW_FIELD_FULL		(float)-1.0 // +-180 degrees
#define	VIEW_FIELD_WIDE		(float)-0.7 // +-135 degrees 0.1 // +-85 degrees, used for full FOV checks 
#define	VIEW_FIELD_NARROW	(float)0.7 // +-45 degrees, more narrow check used to set up ranged attacks
#define	VIEW_FIELD_ULTRA_NARROW	(float)0.9 // +-25 degrees, more narrow check used to set up ranged attacks

// All monsters need this data
#define DONT_BLEED			-1
#define BLOOD_COLOR_RED		(BYTE)247
#define BLOOD_COLOR_YELLOW	(BYTE)195
#define BLOOD_COLOR_GREEN	BLOOD_COLOR_YELLOW

#ifdef PROTOCOL_H

#define SVC_TEMPENTITY		svc_temp_entity//23
#define SVC_INTERMISSION	svc_intermission//30
#define SVC_CDTRACK			svc_cdtrack//32
#define SVC_WEAPONANIM		svc_weaponanim//35
#define SVC_ROOMTYPE		svc_roomtype//37
#define SVC_HLTV			svc_hltv//50
#define SVC_DIRECTOR		svc_director//51

#else// hardcode

#define SVC_TEMPENTITY		23
#define SVC_INTERMISSION	30
#define SVC_CDTRACK			32
#define SVC_WEAPONANIM		35
#define SVC_ROOMTYPE		37
#define SVC_HLTV			50
#define SVC_DIRECTOR		51

#endif


// triggers
#define	SF_TRIGGER_ALLOWMONSTERS	1// monsters allowed to fire this trigger
#define	SF_TRIGGER_NOCLIENTS		2// players not allowed to fire this trigger
#define SF_TRIGGER_PUSHABLES		4// only pushables can fire this trigger
#define SF_TRIGGER_EVERYTHING		8// everything else can fire this trigger (e.g. gibs, rockets)
#define SF_TRIGGER_CLEARVELOCITY	16// XDM: clear velocity
#define SF_TRIGGER_KEEPANGLES		32// XDM: keep view angles
#define SF_TRIGGER_SOUNDACTIVATE	64// XDM: activated by sound

#define SF_LIGHT_START_OFF		1

#define SF_TRIG_PUSH_ONCE		1


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
/*
#define LINK_ENTITY_TO_CLASS(mapClassName,DLLClassName) \
	extern "C" EXPORT void mapClassName( entvars_t *pev ); \
	void mapClassName( entvars_t *pev ) { GetClassPtr( (DLLClassName *)pev ); }
*/

//
// Conversion among the three types of "entity", including identity-conversions.
//
#ifdef DEBUG
	extern edict_t *DBG_EntOfVars(const entvars_t *pev);
	inline edict_t *ENT(const entvars_t *pev)	{ return DBG_EntOfVars(pev); }
#else
	inline edict_t *ENT(const entvars_t *pev)	{ return pev->pContainingEntity; }
#endif
inline edict_t *ENT(edict_t *pent)		{ return pent; }
inline edict_t *ENT(EOFFSET eoffset)			{ return (*g_engfuncs.pfnPEntityOfEntOffset)(eoffset); }
inline EOFFSET OFFSET(EOFFSET eoffset)			{ return eoffset; }
inline EOFFSET OFFSET(const edict_t *pent)	
{ 
#if _DEBUG
	if ( !pent )
		ALERT( at_error, "Bad ent in OFFSET()\n" );
#endif
	return (*g_engfuncs.pfnEntOffsetOfPEntity)(pent); 
}
inline EOFFSET OFFSET(entvars_t *pev)				
{ 
#if _DEBUG
	if ( !pev )
		ALERT( at_error, "Bad pev in OFFSET()\n" );
#endif
	return OFFSET(ENT(pev)); 
}
inline entvars_t *VARS(entvars_t *pev)					{ return pev; }

inline entvars_t *VARS(edict_t *pent)			
{ 
	if ( !pent )
		return NULL;

	return &pent->v; 
}

inline entvars_t* VARS(EOFFSET eoffset)				{ return VARS(ENT(eoffset)); }
inline int	  ENTINDEX(edict_t *pEdict)			{ return (*g_engfuncs.pfnIndexOfEdict)(pEdict); }
inline edict_t* INDEXENT( int iEdictNum )		{ return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum); }
inline void MESSAGE_BEGIN( int msg_dest, int msg_type, const float *pOrigin, entvars_t *ent ) {
	(*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ENT(ent));
}

// Testing the three types of "entity" for nullity
#define eoNullEntity 0

inline bool FNullEnt(EOFFSET eoffset)			{ return eoffset == 0; }
inline bool FNullEnt(const edict_t* pent)	{ return pent == NULL || FNullEnt(OFFSET(pent)); }
inline bool FNullEnt(entvars_t* pev)				{ return pev == NULL || FNullEnt(OFFSET(pev)); }

// Testing strings for nullity
#define iStringNull 0

inline bool FStringNull(int iString)
	{ return iString == iStringNull; }
inline bool FStrEq(const char*sz1, const char*sz2)
	{ return (strcmp(sz1, sz2) == 0); }
inline bool FStrnEq(const char*sz1, const char*sz2, int len)// XBM
	{ return (strncmp(sz1, sz2, len) == 0); }
inline bool FClassnameIs(edict_t* pent, const char* szClassname)
	{ return FStrEq(STRING(VARS(pent)->classname), szClassname); }
inline bool FClassnameIs(entvars_t* pev, const char* szClassname)
	{ return FStrEq(STRING(pev->classname), szClassname); }


// Misc. Prototypes
extern float		UTIL_VecToYaw			(const Vector &vec);
extern Vector		UTIL_VecToAngles		(const Vector &vec);
extern float		UTIL_AngleMod			(float a);
extern float		UTIL_AngleDiff			( float destAngle, float srcAngle );

inline void MESSAGE_BEGIN( int msg_dest, int msg_type, const float *pOrigin, entvars_t *ent );  // implementation later in this file

inline edict_t *FIND_ENTITY_BY_CLASSNAME(edict_t *entStart, const char *pszName) 
{
	return FIND_ENTITY_BY_STRING(entStart, "classname", pszName);
}

inline edict_t *FIND_ENTITY_BY_TARGETNAME(edict_t *entStart, const char *pszName) 
{
	return FIND_ENTITY_BY_STRING(entStart, "targetname", pszName);
}

// for doing a reverse lookup. Say you have a door, and want to find its button.
inline edict_t *FIND_ENTITY_BY_TARGET(edict_t *entStart, const char *pszName) 
{
	return FIND_ENTITY_BY_STRING(entStart, "target", pszName);
}
/*
extern CBaseEntity	*UTIL_FindEntityInSphere(CBaseEntity *pStartEntity, const Vector &vecCenter, float flRadius);
extern CBaseEntity	*UTIL_FindEntityByString(CBaseEntity *pStartEntity, const char *szKeyword, const char *szValue );
extern CBaseEntity	*UTIL_FindEntityByClassname(CBaseEntity *pStartEntity, const char *szName );
extern CBaseEntity	*UTIL_FindEntityByTargetname(CBaseEntity *pStartEntity, const char *szName );
extern CBaseEntity	*UTIL_FindEntityGeneric(const char *szName, Vector &vecSrc, float flRadius );
*/
// returns a CBaseEntity pointer to a player by index.  Only returns if the player is spawned and connected
// otherwise returns NULL
// Index is 1 based
//extern CBaseEntity	*UTIL_PlayerByIndex( int playerIndex );

#define UTIL_EntitiesInPVS(pent)			(*g_engfuncs.pfnEntitiesInPVS)(pent)

inline void UTIL_MakeVectors(const Vector &vecAngles)
{
	MAKE_VECTORS(vecAngles);
}

inline void UTIL_MakeVectorsPrivate( const Vector &vecAngles, float *p_vForward, float *p_vRight, float *p_vUp )
{
	g_engfuncs.pfnAngleVectors( vecAngles, p_vForward, p_vRight, p_vUp );
}

// Pass in an array of pointers and an array size, it fills the array and returns the number inserted
//int UTIL_MonstersInSphere(CBaseEntity **pList, int listMax, const Vector &center, float radius );
//int UTIL_EntitiesInBox(CBaseEntity **pList, int listMax, const Vector &mins, const Vector &maxs, int flagMask );

extern void			UTIL_MakeAimVectors		( const Vector &vecAngles ); // like MakeVectors, but assumes pitch isn't inverted
extern void			UTIL_MakeInvVectors		( const Vector &vec, globalvars_t *pgv );

extern void			UTIL_SetOrigin			( entvars_t *pev, const Vector &vecOrigin );
extern void			UTIL_EmitAmbientSound	( edict_t *entity, const Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch );
extern void			UTIL_ParticleEffect		( const Vector &vecOrigin, const Vector &vecDirection, ULONG ulColor, ULONG ulCount );
//extern void			UTIL_ScreenShakeOne		( CBaseEntity *pPlayer, const Vector &center, float amplitude, float frequency, float duration);// XDM
extern void			UTIL_ScreenShake		( const Vector &center, float amplitude, float frequency, float duration, float radius );
extern void			UTIL_ScreenShakeAll		( const Vector &center, float amplitude, float frequency, float duration );
//extern void			UTIL_ShowMessage		( const char *pString, CBaseEntity *pPlayer );
extern void			UTIL_ShowMessageAll		( const char *pString );
extern void			UTIL_ScreenFadeAll		( const Vector &color, float fadeTime, float holdTime, int alpha, int flags );
//extern void			UTIL_ScreenFade			( CBaseEntity *pEntity, const Vector &color, float fadeTime, float fadeHold, int alpha, int flags );
typedef enum { ignore_monsters=1, dont_ignore_monsters=0, missile=2 } IGNORE_MONSTERS;
typedef enum { ignore_glass=1, dont_ignore_glass=0 } IGNORE_GLASS;
extern void			UTIL_TraceLine			(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr);
extern void			UTIL_TraceLine			(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr);
typedef enum { point_hull=0, human_hull=1, large_hull=2, head_hull=3 } HULLTYPES;
extern void			UTIL_TraceHull			(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr);
extern TraceResult	UTIL_GetGlobalTrace		(void);
extern void			UTIL_TraceModel			(const Vector &vecStart, const Vector &vecEnd, int hullNumber, edict_t *pentModel, TraceResult *ptr);
extern Vector		UTIL_GetAimVector		(edict_t *pent, float flSpeed);
//extern int			UTIL_IsMasterTriggered(string_t sMaster, CBaseEntity *pActivator);
extern void			UTIL_BloodStream(const Vector &origin, const Vector &direction, const int &color, const int &amount);
extern void			UTIL_BloodDrips(const Vector &origin, const Vector &direction, const int &color, const int &amount);
extern Vector		UTIL_RandomBloodVector( void );
extern bool			UTIL_ShouldShowBlood(const int &color);
extern void			UTIL_BloodDecalTrace(TraceResult *pTrace, const int &bloodColor);
extern void			UTIL_DecalTrace( TraceResult *pTrace, int decalNumber );
extern void			UTIL_PlayerDecalTrace( TraceResult *pTrace, int playernum, int decalNumber, bool bIsCustom );
extern void			UTIL_GunshotDecalTrace( TraceResult *pTrace, int decalNumber );
extern void			UTIL_Sparks( const Vector &position );
extern void			UTIL_Ricochet( const Vector &position, float scale );
//extern void			UTIL_StringToVector( float *pVector, const char *pString );
//extern void			UTIL_StringToIntArray( int *pVector, int count, const char *pString );

Vector		UTIL_ClampVectorToBox(const Vector &input, const Vector &clampSize);
float		UTIL_Approach(const float &target, float value, const float &speed);
float		UTIL_ApproachAngle(float target, float value, float speed);
float		UTIL_AngleDistance(const float &next, const float &cur);
// Use for ease-in, ease-out style interpolation (accel/decel)
float		UTIL_SplineFraction(float value, const float &scale);

char *UTIL_VarArgs(char *format, ...);
//void UTIL_Remove(CBaseEntity *pEntity);

bool IsValidTeam(const TEAM_ID &team_id);
bool IsActiveTeam(const TEAM_ID &team_id);

bool UTIL_IsValidEntity(edict_t *pent);

// Search for water transition along a vertical line
float UTIL_WaterLevel(const Vector &position, float minz, float maxz);
//void UTIL_Bubbles(Vector mins, Vector maxs, int count);
//void UTIL_BubbleTrail(Vector from, Vector to, int count);

// allows precacheing of other entities
/*
void UTIL_PrecacheOther(const char *szClassname);
void UTIL_PrecacheOtherWeapon(const char *szClassname);
void UTIL_PrecacheMaterial(struct material_s *pMaterial);

// prints a message to each client
extern void UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL );
inline void UTIL_CenterPrintAll( const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL ) 
{
	UTIL_ClientPrintAll( HUD_PRINTCENTER, msg_name, param1, param2, param3, param4 );
}
*/
// prints messages through the HUD
extern void ClientPrint( entvars_t *client, int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL );

// prints a message to the HUD say (chat)
//extern void UTIL_SayText(const char *pText, CBaseEntity *pEntity = NULL, BOOL reliable = TRUE);
//extern void UTIL_SayTextAll(const char *pText, CBaseEntity *pEntity = NULL, BOOL reliable = TRUE);

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
//extern void			UTIL_HudMessageAll( const hudtextparms_t &textparms, const char *pMessage );
//extern void			UTIL_HudMessage( CBaseEntity *pEntity, const hudtextparms_t &textparms, const char *pMessage );

// for handy use with ClientPrint params
extern char *UTIL_dtos1( int d );

// Writes message to console with timestamp and FragLog header.
extern void UTIL_LogPrintf( char *fmt, ... );

// Sorta like FInViewCone, but for nonmonsters. 
extern float UTIL_DotPoints ( const Vector &vecSrc, const Vector &vecCheck, const Vector &vecDir );
extern void UTIL_StripToken( const char *pKey, char *pDest );// for redundant keynames

// Misc functions
//extern void SetMovedir(entvars_t *pev);

extern Vector VecBModelOrigin(entvars_t *pevBModel);

// NOTE: use EMIT_SOUND_DYN to set the pitch of a sound. Pitch of 100
// is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
// down to 1 is a lower pitch. 150 to 70 is the realistic range.
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

#define PRECACHE_SOUND_ARRAY(a) { for (int i = 0; i < ARRAYSIZE(a); ++i) PRECACHE_SOUND((char *) a [i]); }
#define EMIT_SOUND_ARRAY_DYN(chan, array) EMIT_SOUND_DYN(ENT(pev), chan, array [RANDOM_LONG(0,ARRAYSIZE(array)-1)], VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(95,105));
#define RANDOM_SOUND_ARRAY(array) (array)[RANDOM_LONG(0,ARRAYSIZE((array))-1)]

#define PLAYBACK_EVENT(flags, who, index) PLAYBACK_EVENT_FULL( flags, who, index, 0, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
#define PLAYBACK_EVENT_DELAY(flags, who, index, delay) PLAYBACK_EVENT_FULL( flags, who, index, delay, (float *)&g_vecZero, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );


#define GROUP_OP_AND	0
#define GROUP_OP_NAND	1

extern int g_groupmask;
extern int g_groupop;

class UTIL_GroupTrace
{
public:
	UTIL_GroupTrace( int groupmask, int op );
	~UTIL_GroupTrace( void );

private:
	int m_oldgroupmask, m_oldgroupop;
};

void UTIL_SetGroupTrace( int groupmask, int op );
void UTIL_UnsetGroupTrace( void );

int UTIL_SharedRandomLong( unsigned int seed, int low, int high );
float UTIL_SharedRandomFloat( unsigned int seed, float low, float high );
float UTIL_WeaponTimeBase( void );

bool IsFacing(const Vector &origin, const Vector &v_angle, const Vector &reference);

void BeamEffect(int type, const Vector &vecPos, const Vector &vecAxis, int mdl_idx, int startframe, int fps, int life, int width, int noise, const Vector &color, int brightness, int speed);
void GlowSprite(const Vector &vecPos, int mdl_idx, int life, int scale, int fade);

bool UTIL_LiquidContents(const Vector &vec);
void UTIL_ShowLine(const Vector &start, const Vector &end, float life, byte r, byte g, byte b);
void UTIL_ShowBox(const Vector &origin, const Vector &mins, const Vector &maxs, float life, byte r, byte g, byte b);
void UTIL_DebugBeam(const Vector &vecSrc, const Vector &vecEnd, float life, byte r, byte g, byte b);
void UTIL_DebugPoint(const Vector &vecPos, float life, byte r, byte g, byte b);

void BotCheckTeamplay(void);
bool GameRulesHaveGoal(void);

#endif	//XBM_UTIL_H
