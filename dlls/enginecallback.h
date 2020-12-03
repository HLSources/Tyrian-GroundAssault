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
#ifndef ENGINECALLBACK_H
#define ENGINECALLBACK_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include "event_flags.h"

// Must be provided by user of this code
extern enginefuncs_t g_engfuncs;

// The actual engine callbacks
#define PRECACHE_MODEL				(*g_engfuncs.pfnPrecacheModel)
#define PRECACHE_SOUND				(*g_engfuncs.pfnPrecacheSound)
#define SET_MODEL					(*g_engfuncs.pfnSetModel)// WARNING! Must be called after proper solid/movetype was set!
#define MODEL_INDEX					(*g_engfuncs.pfnModelIndex)
#define MODEL_FRAMES				(*g_engfuncs.pfnModelFrames)
#define SET_SIZE					(*g_engfuncs.pfnSetSize)
#define CHANGE_LEVEL				(*g_engfuncs.pfnChangeLevel)
#define GET_SPAWN_PARMS				(*g_engfuncs.pfnGetSpawnParms)
#define SAVE_SPAWN_PARMS			(*g_engfuncs.pfnSaveSpawnParms)
#define VEC_TO_YAW					(*g_engfuncs.pfnVecToYaw)
#define VEC_TO_ANGLES				(*g_engfuncs.pfnVecToAngles)
#define MOVE_TO_ORIGIN				(*g_engfuncs.pfnMoveToOrigin)
#define oldCHANGE_YAW				(*g_engfuncs.pfnChangeYaw)
#define CHANGE_PITCH				(*g_engfuncs.pfnChangePitch)
#define FIND_ENTITY_BY_STRING		(*g_engfuncs.pfnFindEntityByString)
#define GETENTITYILLUM				(*g_engfuncs.pfnGetEntityIllum)
#define FIND_ENTITY_IN_SPHERE		(*g_engfuncs.pfnFindEntityInSphere)
#define FIND_CLIENT_IN_PVS			(*g_engfuncs.pfnFindClientInPVS)// WARNING! Does not return NULL!
#define ENTITIES_IN_PVS				(*g_engfuncs.pfnEntitiesInPVS)
#define MAKE_VECTORS				(*g_engfuncs.pfnMakeVectors)
#define ANGLE_VECTORS				(*g_engfuncs.pfnAngleVectors)
#define CREATE_ENTITY				(*g_engfuncs.pfnCreateEntity)
#define REMOVE_ENTITY				(*g_engfuncs.pfnRemoveEntity)
#define CREATE_NAMED_ENTITY			(*g_engfuncs.pfnCreateNamedEntity)
#define MAKE_STATIC					(*g_engfuncs.pfnMakeStatic)
#define ENT_IS_ON_FLOOR				(*g_engfuncs.pfnEntIsOnFloor)
#define DROP_TO_FLOOR				(*g_engfuncs.pfnDropToFloor)
#define WALK_MOVE					(*g_engfuncs.pfnWalkMove)
#define SET_ORIGIN					(*g_engfuncs.pfnSetOrigin)
#define EMIT_SOUND_DYN2				(*g_engfuncs.pfnEmitSound)
#define EMIT_AMBIENT_SOUND			(*g_engfuncs.pfnEmitAmbientSound)
#define TRACE_LINE					(*g_engfuncs.pfnTraceLine)
#define TRACE_TOSS					(*g_engfuncs.pfnTraceToss)
#define TRACE_MONSTER_HULL			(*g_engfuncs.pfnTraceMonsterHull)
#define TRACE_HULL					(*g_engfuncs.pfnTraceHull)
#define TRACE_MODEL					(*g_engfuncs.pfnTraceModel)
#define TRACE_TEXTURE				(*g_engfuncs.pfnTraceTexture)
//#define TRACE_SPHERE				(*g_engfuncs.pfnTraceSphere)
#define GET_AIM_VECTOR				(*g_engfuncs.pfnGetAimVector)
#define SERVER_COMMAND				(*g_engfuncs.pfnServerCommand)
#define SERVER_EXECUTE				(*g_engfuncs.pfnServerExecute)
#define CLIENT_COMMAND				(*g_engfuncs.pfnClientCommand)
#define PARTICLE_EFFECT				(*g_engfuncs.pfnParticleEffect)
#define LIGHT_STYLE					(*g_engfuncs.pfnLightStyle)
#define DECAL_INDEX					(*g_engfuncs.pfnDecalIndex)
#define POINT_CONTENTS				(*g_engfuncs.pfnPointContents)
//#define MESSAGE_BEGIN				(*g_engfuncs.pfnMessageBegin)
inline void MESSAGE_BEGIN(int msg_dest, int msg_type, const float *pOrigin = NULL, edict_t *ed = NULL)
{
#ifdef _DEBUG
	if (msg_type == 0)// XDM: trying to call non-existing message will crash the engine
		(*g_engfuncs.pfnAlertMessage)(at_console, "WARNING! MESSAGE_BEGIN(%d, %d)\n", msg_dest, msg_type);
	else
#endif// _DEBUG
		(*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ed);
}
#define MESSAGE_END					(*g_engfuncs.pfnMessageEnd)
#define WRITE_BYTE					(*g_engfuncs.pfnWriteByte)
#define WRITE_CHAR					(*g_engfuncs.pfnWriteChar)
#define WRITE_SHORT					(*g_engfuncs.pfnWriteShort)
#define WRITE_LONG					(*g_engfuncs.pfnWriteLong)
#define WRITE_ANGLE					(*g_engfuncs.pfnWriteAngle)
#define WRITE_COORD					(*g_engfuncs.pfnWriteCoord)
#define WRITE_STRING				(*g_engfuncs.pfnWriteString)
#define WRITE_ENTITY				(*g_engfuncs.pfnWriteEntity)
#define CVAR_REGISTER				(*g_engfuncs.pfnCVarRegister)
#define CVAR_GET_FLOAT				(*g_engfuncs.pfnCVarGetFloat)
#define CVAR_GET_STRING				(*g_engfuncs.pfnCVarGetString)
#define CVAR_SET_FLOAT				(*g_engfuncs.pfnCVarSetFloat)
#define CVAR_SET_STRING				(*g_engfuncs.pfnCVarSetString)
#define ALERT						(*g_engfuncs.pfnAlertMessage)
#define ENGINE_FPRINTF				(*g_engfuncs.pfnEngineFprintf)
#define ALLOC_PRIVATE				(*g_engfuncs.pfnPvAllocEntPrivateData)
//#define GET_PRIVATE					(*g_engfuncs.pfnPvEntPrivateData)// useless
inline void *GET_PRIVATE(edict_t *pent)
{
	if (pent == NULL)
		return NULL;

	return pent->pvPrivateData;
}
#define FREE_PRIVATE				(*g_engfuncs.pfnFreeEntPrivateData)
//#define STRING					(*g_engfuncs.pfnSzFromIndex)
#define ALLOC_STRING				(*g_engfuncs.pfnAllocString)
//#define VARS						(*g_engfuncs.pfnGetVarsOfEnt)// useless
inline entvars_t *VARS(edict_t *pent)
{
	if (pent == NULL)
		return NULL;

	return &pent->v;
}
//#define ENT						(*g_engfuncs.pfnPEntityOfEntOffset)
inline edict_t *ENT(EOFFSET eoffset)// for overloading
{
	return (*g_engfuncs.pfnPEntityOfEntOffset)(eoffset);
}
//#define OFFSET					(*g_engfuncs.pfnEntOffsetOfPEntity)
inline EOFFSET OFFSET(const edict_t *pent)// for overloading
{
#ifdef _DEBUG
	if (pent == NULL)
		ALERT(at_error, "Bad ent in OFFSET()!\n");
#endif
	return (*g_engfuncs.pfnEntOffsetOfPEntity)(pent);
}
#define ENTINDEX					(*g_engfuncs.pfnIndexOfEdict)
//inline int ENTINDEX(const edict_t *pEdict) { return (*g_engfuncs.pfnIndexOfEdict)(pEdict); }
#define INDEXENT					(*g_engfuncs.pfnPEntityOfEntIndex)
//inline edict_t *INDEXENT(int iEdictNum) { return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum); }
//#define ENT_BY_VARS				(*g_engfuncs.pfnFindEntityByVars)// DBG_EntOfVars
#define GET_MODEL_PTR				(*g_engfuncs.pfnGetModelPtr)
#define REG_USER_MSG				(*g_engfuncs.pfnRegUserMsg)
//#define ANIMATION_AUTOMOVE			(*g_engfuncs.pfnAnimationAutomove)// not used
#define GET_BONE_POSITION			(*g_engfuncs.pfnGetBonePosition)
#define FUNCTION_FROM_NAME			(*g_engfuncs.pfnFunctionFromName)
#define NAME_FOR_FUNCTION			(*g_engfuncs.pfnNameForFunction)
#define CLIENT_PRINTF				(*g_engfuncs.pfnClientPrintf)
#define SERVER_PRINT				(*g_engfuncs.pfnServerPrint)
#define CMD_ARGS					(*g_engfuncs.pfnCmd_Args)// does NOT include the command itself
#define CMD_ARGC					(*g_engfuncs.pfnCmd_Argc)
#define CMD_ARGV					(*g_engfuncs.pfnCmd_Argv)
#define GET_ATTACHMENT				(*g_engfuncs.pfnGetAttachment)
#define CRC32_INIT					(*g_engfuncs.pfnCRC32_Init)
#define CRC32_PROCESS_BUFFER		(*g_engfuncs.pfnCRC32_ProcessBuffer)
#define CRC32_PROCESS_BYTE			(*g_engfuncs.pfnCRC32_ProcessByte)
#define CRC32_FINAL					(*g_engfuncs.pfnCRC32_Final)
#define RANDOM_LONG					(*g_engfuncs.pfnRandomLong)
#define RANDOM_FLOAT				(*g_engfuncs.pfnRandomFloat)
#define SET_VIEW					(*g_engfuncs.pfnSetView)
//#define GETTIME						(*g_engfuncs.pfnTime)
#define SET_CROSSHAIRANGLE			(*g_engfuncs.pfnCrosshairAngle)
#define LOAD_FILE_FOR_ME			(*g_engfuncs.pfnLoadFileForMe)
#define FREE_FILE					(*g_engfuncs.pfnFreeFile)
#define END_SECTION					(*g_engfuncs.pfnEndSection)
#define COMPARE_FILE_TIME			(*g_engfuncs.pfnCompareFileTime)
#define GET_GAME_DIR				(*g_engfuncs.pfnGetGameDir)
//#define CVAR_REGISTER_VAR			(*g_engfuncs.pfnCvar_RegisterVariable)
//#define FADE_CLIENT_VOLUME			(*g_engfuncs.pfnFadeClientVolume)
//#define SET_CLIENT_MAXSPEED			(*g_engfuncs.pfnSetClientMaxspeed)
#define CREATE_FAKE_CLIENT			(*g_engfuncs.pfnCreateFakeClient)
//#define RUN_PLAYER_MOVE				(*g_engfuncs.pfnRunPlayerMove)
#define NUMBER_OF_ENTITIES			(*g_engfuncs.pfnNumberOfEntities)
#define GET_INFO_KEY_BUFFER			(*g_engfuncs.pfnGetInfoKeyBuffer)
#define GET_INFO_KEY_VALUE			(*g_engfuncs.pfnInfoKeyValue)
#define SET_INFO_KEY_VALUE			(*g_engfuncs.pfnSetKeyValue)
#define SET_CLIENT_KEY_VALUE		(*g_engfuncs.pfnSetClientKeyValue)
#define IS_MAP_VALID				(*g_engfuncs.pfnIsMapValid)
#define STATIC_DECAL				(*g_engfuncs.pfnStaticDecal)
#define PRECACHE_GENERIC			(*g_engfuncs.pfnPrecacheGeneric)
#define GETPLAYERUSERID				(*g_engfuncs.pfnGetPlayerUserId)
#define BUILD_SOUND_MSG				(*g_engfuncs.pfnBuildSoundMsg)
#define IS_DEDICATED_SERVER			(*g_engfuncs.pfnIsDedicatedServer)
#define CVAR_GET_POINTER			(*g_engfuncs.pfnCVarGetPointer)
#define GETPLAYERWONID				(*g_engfuncs.pfnGetPlayerWONId)

// YWB 8/1/99 TFF Physics additions
#define INFO_REMOVE_KEY				(*g_engfuncs.pfnInfo_RemoveKey)
#define ENGINE_GETPHYSKV			(*g_engfuncs.pfnGetPhysicsKeyValue)
#define ENGINE_SETPHYSKV			(*g_engfuncs.pfnSetPhysicsKeyValue)
#define ENGINE_GETPHYSINFO			(*g_engfuncs.pfnGetPhysicsInfoString)
#define PRECACHE_EVENT				(*g_engfuncs.pfnPrecacheEvent)
#define PLAYBACK_EVENT_FULL			(*g_engfuncs.pfnPlaybackEvent)

#define ENGINE_SET_PVS				(*g_engfuncs.pfnSetFatPVS)
#define ENGINE_SET_PAS				(*g_engfuncs.pfnSetFatPAS)

#define ENGINE_CHECK_VISIBILITY		(*g_engfuncs.pfnCheckVisibility)

#define DELTA_SET					(*g_engfuncs.pfnDeltaSetField)
#define DELTA_UNSET					(*g_engfuncs.pfnDeltaUnsetField)
#define DELTA_ADDENCODER			(*g_engfuncs.pfnDeltaAddEncoder)
#define ENGINE_CURRENT_PLAYER		(*g_engfuncs.pfnGetCurrentPlayer)
#define	ENGINE_CANSKIP				(*g_engfuncs.pfnCanSkipPlayer)
#define DELTA_FINDFIELD				(*g_engfuncs.pfnDeltaFindField)
#define DELTA_SETBYINDEX			(*g_engfuncs.pfnDeltaSetFieldByIndex)
#define DELTA_UNSETBYINDEX			(*g_engfuncs.pfnDeltaUnsetFieldByIndex)

#define ENGINE_SETGROUPMASK			(*g_engfuncs.pfnSetGroupMask)

#define ENGINE_INSTANCE_BASELINE	(*g_engfuncs.pfnCreateInstancedBaseline)
#define CVAR_DIRECT_SET				(*g_engfuncs.pfnCvar_DirectSet)

#define ENGINE_FORCE_UNMODIFIED		(*g_engfuncs.pfnForceUnmodified)
#define PLAYER_CNX_STATS			(*g_engfuncs.pfnGetPlayerStats)
#define ADD_SERVER_COMMAND			(*g_engfuncs.pfnAddServerCommand)

//pfnVoice_GetClientListening
//pfnVoice_SetClientListening

#define GETPLAYERAUTHID				(*g_engfuncs.pfnGetPlayerAuthId)
/*
#ifdef SVDLL_NEWFUNCTIONS

#define (*g_engfuncs.pfnSequenceGet)
#define (*g_engfuncs.pfnSequencePickSentence)
#define (*g_engfuncs.pfnGetFileSize)
#define (*g_engfuncs.pfnGetApproxWavePlayLen)
#define (*g_engfuncs.pfnIsCareerMatch)
#define (*g_engfuncs.pfnGetLocalizedStringLength)
#define (*g_engfuncs.pfnRegisterTutorMessageShown)
#define (*g_engfuncs.pfnGetTimesTutorMessageShown)
#define (*g_engfuncs.pfnProcessTutorMessageDecayBuffer)
#define (*g_engfuncs.pfnConstructTutorMessageDecayBuffer)
#define (*g_engfuncs.pfnResetTutorMessageDecayData)
#define (*g_engfuncs.pfnQueryClientCvarValue)
#define (*g_engfuncs.pfnQueryClientCvarValue2)

#endif // SVDLL_NEWFUNCTIONS
*/
#endif		//ENGINECALLBACK_H
