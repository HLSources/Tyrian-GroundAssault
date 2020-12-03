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
/*

===== h_export.cpp ========================================================

  Entity classes exported by Halflife.

*/
/*
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOSERVICE
#define NOMCX
#define NOIME
#include "windef.h"
#endif
*/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "client.h"
#include "pm_shared.h"
#include "basemonster.h"
#include "game.h"
#include "gamerules.h"
//#include <io.h>
//#include <fcntl.h>
//#include <sys/stat.h>

// Holds engine functionality callbacks
enginefuncs_t g_engfuncs;// enginecallback.h
globalvars_t *gpGlobals;// util.h
DLL_GLOBAL int g_iProtocolVersion = 0;


#ifdef _WIN32

#define far
typedef unsigned long DWORD;
typedef void *PVOID;
typedef void far *LPVOID;

#ifdef STRICT
typedef void *HANDLE;
#define DECLARE_HANDLE(n) typedef struct n##__{int i;}*n
#else
typedef PVOID HANDLE;
#define DECLARE_HANDLE(n) typedef HANDLE n
#endif
typedef HANDLE *PHANDLE,*LPHANDLE;

DECLARE_HANDLE(HINSTANCE);

#ifndef APIENTRY
#define APIENTRY __stdcall
#endif

// Required DLL entry point
BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
#ifdef _DEBUG
	printf("XDM: DllMain(%d)\n", fdwReason);
#endif
	return TRUE;
}
#endif// _WIN32


// XDM3035c: stubs
sequenceEntry_s *pfnSequenceGet(const char *fileName, const char *entryName)
{
	return NULL;//(*g_engfuncs.pfnSequenceGet)(fileName, entryName);
}
sentenceEntry_s *pfnSequencePickSentence(const char *groupName, int pickMethod, int *picked)
{
	return NULL;//(*g_engfuncs.pfnSequencePickSentence)(groupName, pickMethod, picked);
}
int pfnGetFileSize(char *filename)
{
//	return (*g_engfuncs.pfnGetFileSize)(filename);
	int l = 0;
	byte *pFile = (*g_engfuncs.pfnLoadFileForMe)(filename, &l);
	if (pFile)
		(*g_engfuncs.pfnFreeFile)(pFile);
//	int fh = 0;
//	if ((fh = _open(filename, _O_RDWR | _O_CREAT, _S_IREAD | _S_IWRITE)) != -1)
//		l = _filelength(fh);
	return l;
}
unsigned int pfnGetApproxWavePlayLen(const char *filepath)
{
	return 10.0f;//(*g_engfuncs.pfnGetApproxWavePlayLen)(filepath);
}
int pfnIsCareerMatch(void)
{
	return 0;// (*g_engfuncs.pfnIsCareerMatch)();
}
int pfnGetLocalizedStringLength(const char *label)
{
	return strlen(label)*4;//(*g_engfuncs.pfnGetLocalizedStringLength)(label);
}
void pfnRegisterTutorMessageShown(int mid)
{
//	(*g_engfuncs.pfnRegisterTutorMessageShown)(mid);
}
int pfnGetTimesTutorMessageShown(int mid)
{
	return 0;//(*g_engfuncs.pfnGetTimesTutorMessageShown)(mid);
}
void pfnProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
//	(*g_engfuncs.pfnProcessTutorMessageDecayBuffer)(buffer, bufferLength);
}
void pfnConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
//	(*g_engfuncs.pfnConstructTutorMessageDecayBuffer)(buffer, bufferLength);
}
void pfnResetTutorMessageDecayData(void)
{
//	(*g_engfuncs.pfnResetTutorMessageDecayData)();
}
void pfnQueryClientCvarValue(const edict_t *player, const char *cvarName)
{
//	(*g_engfuncs.pfnQueryClientCvarValue)(player, cvarName);
}
void pfnQueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID)
{
//	(*g_engfuncs.pfnQueryClientCvarValue2)(player, cvarName, requestID);
}


// MUST be defined in definition file as number 1
extern "C" void DLLEXPORT STDCALL GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals)
{
	gpGlobals = pGlobals;

	// detect game protocol version
	g_iProtocolVersion = 46;// HL1110-, safest, default
	int exe_build = 0;
	const char *version_string = (*pengfuncsFromEngine->pfnCVarGetString)("sv_version");// "sv_version" is "0123456789abcdefghijklmnopqrstu,48,4554"
	if (version_string)
	{
//		char PatchVersion[32];// from XDM/steam.inf, max 32 chars
//fail		sscanf(version_string, "%s,%d,%d", PatchVersion, &protocol_ver, &exe_build);// == 3?
		char *protocol_ver_str = strchr(version_string, ',');// safe: HL is smart enough to filter all ',' from these values
		if (protocol_ver_str)
			sscanf(protocol_ver_str, ",%d,%d", &g_iProtocolVersion, &exe_build);// == 2?
	}

	// now use proper preset
	size_t engineFunctionsSize;
#ifdef SVDLL_NEWFUNCTIONS
	if (g_iProtocolVersion >= 48)// HL1121
	{
		engineFunctionsSize = sizeof(enginefuncs_t);
	}
	else if (g_iProtocolVersion == 47)// HL1120
	{
		engineFunctionsSize = offsetof(enginefuncs_t, pfnQueryClientCvarValue);// TODO: find out
		// stubs
		g_engfuncs.pfnQueryClientCvarValue = pfnQueryClientCvarValue;
		g_engfuncs.pfnQueryClientCvarValue2 = pfnQueryClientCvarValue2;
	}
	else// HL1110
	{
		engineFunctionsSize = offsetof(enginefuncs_t, pfnSequenceGet);
		// stubs
		g_engfuncs.pfnSequenceGet = pfnSequenceGet;
		g_engfuncs.pfnSequencePickSentence = pfnSequencePickSentence;
		g_engfuncs.pfnGetFileSize = pfnGetFileSize;
		g_engfuncs.pfnGetApproxWavePlayLen = pfnGetApproxWavePlayLen;
		g_engfuncs.pfnIsCareerMatch = pfnIsCareerMatch;
		g_engfuncs.pfnGetLocalizedStringLength = pfnGetLocalizedStringLength;
		g_engfuncs.pfnRegisterTutorMessageShown = pfnRegisterTutorMessageShown;
		g_engfuncs.pfnGetTimesTutorMessageShown = pfnGetTimesTutorMessageShown;
		g_engfuncs.pfnProcessTutorMessageDecayBuffer = pfnProcessTutorMessageDecayBuffer;
		g_engfuncs.pfnConstructTutorMessageDecayBuffer = pfnConstructTutorMessageDecayBuffer;
		g_engfuncs.pfnResetTutorMessageDecayData = pfnResetTutorMessageDecayData;
		g_engfuncs.pfnQueryClientCvarValue = pfnQueryClientCvarValue;
		g_engfuncs.pfnQueryClientCvarValue2 = pfnQueryClientCvarValue2;
	}
#else
		engineFunctionsSize = sizeof(enginefuncs_t);
#endif

/*
	(*pengfuncsFromEngine->pfnServerPrint)("!1");
	edict_t *te = (*pengfuncsFromEngine->pfnPEntityOfEntIndex)(1);
	(*pengfuncsFromEngine->pfnServerPrint)("!2");
	(*pengfuncsFromEngine->pfnQueryClientCvarValue)(te, "test");
*/
	char *str = UTIL_VarArgs("XDM: GiveFnptrsToDll(): using enginefuncs_t size %d (assuming protocol %d)\n", engineFunctionsSize, g_iProtocolVersion);
	(*pengfuncsFromEngine->pfnServerPrint)(str);
	printf(str);
	memcpy(&g_engfuncs, pengfuncsFromEngine, engineFunctionsSize);
}



static DLL_FUNCTIONS gFunctionTable = 
{
	GameDLLInit,				//pfnGameInit
	DispatchSpawn,				//pfnSpawn
	DispatchThink,				//pfnThink
	DispatchUse,				//pfnUse
	DispatchTouch,				//pfnTouch
	DispatchBlocked,			//pfnBlocked
	DispatchKeyValue,			//pfnKeyValue
	DispatchSave,				//pfnSave
	DispatchRestore,			//pfnRestore
	DispatchObjectCollisionBox,	//pfnAbsBox

	SaveWriteFields,			//pfnSaveWriteFields
	SaveReadFields,				//pfnSaveReadFields

	SaveGlobalState,			//pfnSaveGlobalState
	RestoreGlobalState,			//pfnRestoreGlobalState
	ResetGlobalState,			//pfnResetGlobalState

	ClientConnect,				//pfnClientConnect
	ClientDisconnect,			//pfnClientDisconnect
	ClientKill,					//pfnClientKill
	ClientPutInServer,			//pfnClientPutInServer
	ClientCommand,				//pfnClientCommand
	ClientUserInfoChanged,		//pfnClientUserInfoChanged
	ServerActivate,				//pfnServerActivate
	ServerDeactivate,			//pfnServerDeactivate

	PlayerPreThink,				//pfnPlayerPreThink
	PlayerPostThink,			//pfnPlayerPostThink

	StartFrame,					//pfnStartFrame
	ParmsNewLevel,				//pfnParmsNewLevel
	ParmsChangeLevel,			//pfnParmsChangeLevel

	GetGameDescription,         //pfnGetGameDescription    Returns string describing current .dll game.
	PlayerCustomization,        //pfnPlayerCustomization   Notifies .dll of new customization for player.

	SpectatorConnect,			//pfnSpectatorConnect      Called when spectator joins server
	SpectatorDisconnect,        //pfnSpectatorDisconnect   Called when spectator leaves the server
	SpectatorThink,				//pfnSpectatorThink        Called when spectator sends a command packet (usercmd_t)

	Sys_Error,					//pfnSys_Error				Called when engine has encountered an error

	PM_Move,					//pfnPM_Move
	PM_Init,					//pfnPM_Init				Server version of player movement initialization
	PM_FindTextureType,			//pfnPM_FindTextureType
	
	SetupVisibility,			//pfnSetupVisibility        Set up PVS and PAS for networking for this client
	UpdateClientData,			//pfnUpdateClientData       Set up data sent only to specific client
	AddToFullPack,				//pfnAddToFullPack
	CreateBaseline,				//pfnCreateBaseline			Tweak entity baseline for network encoding, allows setup of player baselines, too.
	RegisterEncoders,			//pfnRegisterEncoders		Callbacks for network encoding
	GetWeaponData,				//pfnGetWeaponData
	CmdStart,					//pfnCmdStart
	CmdEnd,						//pfnCmdEnd
	ConnectionlessPacket,		//pfnConnectionlessPacket
	GetHullBounds,				//pfnGetHullBounds
	CreateInstancedBaselines,   //pfnCreateInstancedBaselines
	InconsistentFile,			//pfnInconsistentFile
	AllowLagCompensation,		//pfnAllowLagCompensation
};


extern "C" EXPORT int GetEntityAPI(DLL_FUNCTIONS *pFunctionTable, int interfaceVersion)
{
	// check if engine's pointer is valid and version is correct...
	if (pFunctionTable == NULL)
	{
		printf("XDM: GetEntityAPI(): bad arguments!\n");
		return 0;
	}
	if (interfaceVersion != INTERFACE_VERSION)
	{
		printf("XDM: GetEntityAPI(): incompatible interface version %d! (local %d)\n", interfaceVersion, INTERFACE_VERSION);
		return 0;
	}

	memcpy(pFunctionTable, &gFunctionTable, sizeof(gFunctionTable));
	printf("XDM: GetEntityAPI(): size = %d, version = %d\n", sizeof(gFunctionTable), interfaceVersion);
	return 1;
}

// XDM3035: The engine DOES NOT call GetEntityAPI if it finds GetEntityAPI2 first!
extern "C" EXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
	if (pFunctionTable == NULL || interfaceVersion == NULL)
	{
		printf("XDM: GetEntityAPI2(): bad arguments!\n");
		return 0;
	}
	if (*interfaceVersion != INTERFACE_VERSION)
	{
		printf("XDM: GetEntityAPI2(): incompatible interface version %d! (local %d)\n", *interfaceVersion, INTERFACE_VERSION);
		// Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return 0;// should we?
	}

	memcpy(pFunctionTable, &gFunctionTable, sizeof(gFunctionTable));
	printf("XDM: GetEntityAPI2(): size = %d, version = %d\n", sizeof(gFunctionTable), *interfaceVersion);
	return 1;
}






//NEW_DLL_FUNCTIONS gNewFunctionTableEngine;// original pointers are all NULLs 8(

// XDM
void OnFreeEntPrivateData(edict_t *pEnt)
{
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pEnt);
	if (pEntity)
	{
		pEntity->OnFreePrivateData();
// DON'T!		FREE_PRIVATE(pEnt);
	}
// this is null	gNewFunctionTableEngine.pfnOnFreeEntPrivateData(pEnt);
}

// XDM: return 1 to restore default behaviour
int ShouldCollide(edict_t *pentTouched, edict_t *pentOther)
{
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pentTouched);
	if (pEntity)
	{
		CBaseEntity *pOther = (CBaseEntity *)GET_PRIVATE(pentOther);
		if (pOther)
			return pEntity->ShouldCollide(pOther);
	}
	return 1;
}

#ifdef HL1120
void CvarValue(const edict_t *pEnt, const char *value)
{
}

void CvarValue2(const edict_t *pEnt, int requestID, const char *cvarName, const char *value)
{
}
#endif

static NEW_DLL_FUNCTIONS gNewFunctionTable = 
{
	OnFreeEntPrivateData,	//pfnOnFreeEntPrivateData
	GameDLLShutdown,		//pfnGameShutdown
	ShouldCollide,			//pfnShouldCollide
#ifdef HL1120// XDM3035: these causes API glitches in earlier HL engine such as ClientCommand will be never called
	CvarValue,				//pfnCvarValue
	CvarValue2,				//pfnCvarValue2
#endif
};


// Make sure this function is added to .def file as well!
extern "C" EXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)// XDM: I need this
{
	if (pFunctionTable == NULL || interfaceVersion == NULL)
	{
		printf("XDM: GetNewDLLFunctions(): bad arguments!\n");
		return 0;
	}
	if (*interfaceVersion != NEW_DLL_FUNCTIONS_VERSION)
	{
		printf("XDM: GetNewDLLFunctions(): incompatible interface version %d! (local %d)\n", *interfaceVersion, NEW_DLL_FUNCTIONS_VERSION);
		// Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		return 0;
	}

/*
TODO:
	size_t newFunctionsSize;

	if (g_iProtocolVersion >= 47)
		newFunctionsSize = sizeof(gNewFunctionTable);
	else
		newFunctionsSize = offsetof(NEW_DLL_FUNCTIONS, pfnCvarValue);

	memcpy(pFunctionTable, &gNewFunctionTable, newFunctionsSize);
*/
	memcpy(pFunctionTable, &gNewFunctionTable, sizeof(gNewFunctionTable));
	printf("XDM: GetNewDLLFunctions(): size = %d, version = %d\n", sizeof(gNewFunctionTable), *interfaceVersion);
	return 1;
}

/* TODO
extern "C" EXPORT int Server_GetBlendingInterface(int version, struct sv_blending_interface_s **ppinterface, struct engine_studio_api_s *pstudio, float (*rotationmatrix)[3][4], float (*bonetransform)[MAXSTUDIOBONES][3][4])
{
	return 0;
}
*/

// XDM3035b: useful for external DLLs like XBM

//-----------------------------------------------------------------------------
// Purpose: Classify entity
// Input  : *entity - 
// Output : extern "C" int EXPORT
//-----------------------------------------------------------------------------
extern "C" int EXPORT Classify(edict_t *entity)
{
	CBaseEntity *pEntity = CBaseEntity::Instance(entity);
	if (pEntity)
		return pEntity->Classify();

	return CLASS_NONE;
}

//-----------------------------------------------------------------------------
// Purpose: Reliably determine entity class in XDM
// Input  : entity
// Output : Returns 1 for player, 2-3 for monster
// and 0 for other entities.
//-----------------------------------------------------------------------------
extern "C" int EXPORT EntityIs(edict_t *entity)
{
	if (UTIL_IsValidEntity(entity) == false)
		return ENTIS_INVALID;

	CBaseEntity *pEntity = CBaseEntity::Instance(entity);

	if (UTIL_IsValidEntity(pEntity) == false)
		return ENTIS_INVALID;

	if (pEntity->IsPlayer())// check first!
	{
		return ENTIS_PLAYER;
	}
	else if (pEntity->IsMonster())
	{
		if (pEntity->IsHuman())
			return ENTIS_HUMAN;
		else
			return ENTIS_MONSTER;
	}
	else if (pEntity->IsGameGoal())
		return ENTIS_GAMEGOAL;
	else if (pEntity->IsProjectile())
		return ENTIS_PROJECTILE;
	else if (pEntity->IsPlayerWeapon())
		return ENTIS_PLAYERWEAPON;
	else if (pEntity->IsPlayerItem())
		return ENTIS_PLAYERITEM;
	else if (pEntity->IsPushable())
		return ENTIS_PUSHABLE;
	else if (pEntity->IsBreakable())
		return ENTIS_BREAKABLE;
	else if (pEntity->IsTrigger())
		return ENTIS_TRIGGER;

	return ENTIS_OTHER;
}

//-----------------------------------------------------------------------------
// Purpose: Reliably determine relationship between two entities in XDM
// Input  : entity1, entity2
// Output : Returns PlayerRelationship(ent2) e.g. GR_NOTTEAMMATE for player,
//			IRelationship(ent2) for monster and 0 for other entities.
//-----------------------------------------------------------------------------
extern "C" int EXPORT EntityRelationship(edict_t *entity1, edict_t *entity2)
{
	CBaseEntity *pEntity1 = CBaseEntity::Instance(entity1);
	CBaseEntity *pEntity2 = CBaseEntity::Instance(entity2);

	if (!ASSERT(pEntity1 != NULL))
		return 0;
	if (!ASSERT(pEntity2 != NULL))
		return 0;

	if (pEntity1->IsPlayer())
		return g_pGameRules->PlayerRelationship(pEntity1, pEntity2);
	else if (pEntity1->IsMonster())
		return pEntity1->MyMonsterPointer()->IRelationship(pEntity2);

	return 0;//R_NO;
}

//-----------------------------------------------------------------------------
// Purpose: CanHavePlayerItem
// Input  : *entity - 
//			*item - CBasePlayerItem only!
// Output : int R_NO
//-----------------------------------------------------------------------------
extern "C" int EXPORT CanHaveItem(edict_t *entity, edict_t *item)
{
	CBaseEntity *pEntity1 = CBaseEntity::Instance(entity);
	CBaseEntity *pEntity2 = CBaseEntity::Instance(item);

	if (ASSERT(pEntity1 != NULL) && ASSERT(pEntity2 != NULL) && pEntity1->IsPlayer() && pEntity2->IsPlayerItem())// luckily these allow us to cast directly to desired types
	{
		return g_pGameRules->CanHavePlayerItem((CBasePlayer *)pEntity1, (CBasePlayerItem *)pEntity2);
	}
//	else if (pEntity1->IsMonster())// right now monsters don't pickup items

	return 0;//R_NO;
}

//-----------------------------------------------------------------------------
// Purpose: Reliably get game rules
// Input  : *type - pointer to be filled
//			*mode - 
// Output : byte game rules state: 0 - not installed, 1 - normal, 2 - game over
//-----------------------------------------------------------------------------
extern "C" byte EXPORT GetActiveGameRules(short *type, short *mode)
{
	if (g_pGameRules)
	{
		if (type)
			*type = g_pGameRules->GetGameType();

		if (mode)
			*mode = g_pGameRules->GetGameMode();

		if (g_pGameRules->IsGameOver())
			return 2;

		return 1;
	}
	return 0;
}
