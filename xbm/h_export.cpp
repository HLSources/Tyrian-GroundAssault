#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"

#include "bot.h"
#include "waypoint.h"
#include "engine.h"
//#include <io.h>

#ifndef __linux__

HINSTANCE h_Library = NULL;
HGLOBAL h_global_argv = NULL;
void FreeNameFuncGlobals(void);
void LoadSymbols(const char *filename);

#else

void *h_Library = NULL;
char h_global_argv[1024];

#endif

enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
char *g_argv;

GETENTITYAPI other_GetEntityAPI = NULL;
GETENTITYAPI2 other_GetEntityAPI2 = NULL;// XBM: ???
GETNEWDLLFUNCTIONS other_GetNewDLLFunctions = NULL;
SERVER_GETBLENDINGINTERFACE other_Server_GetBlendingInterface = NULL;// HPB40
GIVEFNPTRSTODLL other_GiveFnptrsToDll = NULL;

XDM_ENTITY_RELATIONSHIP_FUNC XDM_EntityRelationship = NULL;
XDM_ENTITY_IS_FUNC XDM_EntityIs = NULL;
XDM_CAN_HAVE_ITEM_FUNC XDM_CanHaveItem = NULL;

extern DLL_FUNCTIONS other_gFunctionTable;
extern NEW_DLL_FUNCTIONS other_gNewFunctionTable;// XDM

//cvar_t g_Architecture = { "sv_dllarch", "i486", FCVAR_SERVER|FCVAR_SPONLY, 0.0f, NULL };// XDM3035
char g_Architecture[16];// XBM: server DLL postfix


#ifndef __linux__
// Required DLL entry point
int WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		memset(&other_gFunctionTable, 0, sizeof(DLL_FUNCTIONS));
		memset(&other_gNewFunctionTable, 0, sizeof(NEW_DLL_FUNCTIONS));
	}
	else if (fdwReason == DLL_PROCESS_DETACH)
	{
		WaypointInit();// Frees waypoint data

		FreeNameFuncGlobals();  // Free exported symbol table

		if (h_Library)
			FreeLibrary(h_Library);

		if (h_global_argv)
		{
			GlobalUnlock(h_global_argv);
			GlobalFree(h_global_argv);
		}
	}
	return TRUE;
}
#endif


extern "C" void DLLEXPORT STDCALL GiveFnptrsToDll(enginefuncs_t *pengfuncsFromEngine, globalvars_t *pGlobals)
{
	int pos;
	char game_dir[256];
	char mod_name[32];
	char game_dll_filename[256];

	// Patented X-Half-Life(tm) Half-Life server version check algorithm(c)!
	int protocol_ver = 46;// HL1110-, safest, default
	int exe_build = 0;
	const char *version_string = (*pengfuncsFromEngine->pfnCVarGetString)("sv_version");// "sv_version" is "0123456789abcdefghijklmnopqrstu,48,4554"
	if (version_string)
	{
//		char PatchVersion[32];// from XDM/steam.inf, max 32 chars
//fail		sscanf(version_string, "%s,%d,%d", PatchVersion, &protocol_ver, &exe_build);// == 3?
		char *protocol_ver_str = strchr(version_string, ',');// safe: HL is smart enough to filter all ',' from these values
		if (protocol_ver_str)
			sscanf(protocol_ver_str, ",%d,%d", &protocol_ver, &exe_build);// == 2?
	}

	// now use proper preset
	size_t engineFunctionsSize;
#ifdef SVDLL_NEWFUNCTIONS
	if (protocol_ver >= 48)// HL1121
		engineFunctionsSize = sizeof(enginefuncs_t);
	else if (protocol_ver == 47)// HL1120
		engineFunctionsSize = offsetof(enginefuncs_t, pfnQueryClientCvarValue);// TODO: find out
	else// HL1110
		engineFunctionsSize = offsetof(enginefuncs_t, pfnSequenceGet);
#else
		engineFunctionsSize = sizeof(enginefuncs_t);
#endif

/*
	(*pengfuncsFromEngine->pfnServerPrint)("!1");
	edict_t *test = (*pengfuncsFromEngine->pfnPEntityOfEntIndex)(1);
	(*pengfuncsFromEngine->pfnServerPrint)("!2");
	(*pengfuncsFromEngine->pfnQueryClientCvarValue)(test, "test");
*/
	char *str = UTIL_VarArgs("XBM: GiveFnptrsToDll(): using enginefuncs_t size %d (assuming protocol %d)\n", engineFunctionsSize, protocol_ver);
	(*pengfuncsFromEngine->pfnServerPrint)(str);
	printf(str);
	memcpy(&g_engfuncs, pengfuncsFromEngine, engineFunctionsSize);

	// get the engine functions from the engine...
	// test if we're running Steam or not and shorten the engine functions table if we aren't
/*
	if ((access("valve/steam.inf", 0) != -1) || (access("FileSystem_Steam.dll", 0) != -1))
		memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t)); // steam
	else
		memcpy(&g_engfuncs, pengfuncsFromEngine, 144 * sizeof(uint32)); // non-steam // offsetof?
*/

	gpGlobals = pGlobals;
	// find the directory name of the currently running MOD...
	GET_GAME_DIR(game_dir);

	pos = 0;
	if (strstr(game_dir, "/") != NULL)
	{
		pos = strlen(game_dir) - 1;
		// scan backwards till first directory separator...
		while ((pos) && (game_dir[pos] != '/'))
			--pos;
		
		if (pos == 0)// Error getting directory name!
			ALERT( at_warning, "TYRIAN: Error determining MOD directory name!");

		++pos;
	}
	strcpy(mod_name, &game_dir[pos]);
	game_dll_filename[0] = 0;

	memset(g_Architecture, 0, sizeof(g_Architecture));
	strcpy(g_Architecture, "i386");
	int len = 0;
	byte *pBuf = LOAD_FILE_FOR_ME("dlls/XBM.arc", &len);
	if (pBuf)
	{
		strncpy(g_Architecture, (char *)pBuf, min(len,sizeof(g_Architecture)));
		len = strlen(g_Architecture);// for later use
		FREE_FILE(pBuf);
		pBuf = NULL;
	}
	else
		SERVER_PRINT("TYRIAN: Unable to load XBM.arc!");

	if (strcmpi(mod_name, "valve") == 0)
	{
		mod_id = VALVE_DLL;
#ifndef __linux__
		if (len > 0)
			_snprintf(game_dll_filename, 255, "valve/dlls/hl_%s.dll", g_Architecture);
		else
			strcpy(game_dll_filename, "valve/dlls/hl.dll");
#else
		if (len > 0)
			_snprintf(game_dll_filename, 255, "valve/dlls/hl_%s.so", g_Architecture);
		else
			strcpy(game_dll_filename, "valve/dlls/hl_i386.so");
#endif
	}
	else if (strcmpi(mod_name, "TYRIAN") == 0)// XBM
	{
		mod_id = TYRIAN_DLL;
#ifndef __linux__
		if (len > 0)
			_snprintf(game_dll_filename, 255, "TYRIAN/dlls/TYRIAN_%s.dll", g_Architecture);
		else
			strcpy(game_dll_filename, "TYRIAN/dlls/XDM.dll");
#else
		if (len > 0)
			_snprintf(game_dll_filename, 255, "TYRIAN/dlls/TYRIAN_%s.so", g_Architecture);
		else
			strcpy(game_dll_filename, "TYRIAN/dlls/XDM.so");
#endif
	}

	if (game_dll_filename[0])
	{
//		(*g_engfuncs.pfnServerPrint)(game_dll_filename);
		ALERT(at_console, "XBM: loading '%s'\n\n", game_dll_filename);
#ifndef __linux__
		h_Library = LoadLibrary(game_dll_filename);
#else
		h_Library = dlopen(game_dll_filename, RTLD_NOW);
#endif
	}
	else
	{
		ALERT(at_error, "XBM: Mod dll not found or unsupported!");
		return;
	}

	if (h_Library == NULL)
	{
		ALERT(at_error, "XBM: Unable to load %s!\n", game_dll_filename);
		pGlobals->maxClients = 0;
		END_SECTION("void");//(*g_engfuncs.pfnEndSection)
		memset(pengfuncsFromEngine, 0, sizeof(enginefuncs_t));
		return;
	}

#ifndef __linux__
	h_global_argv = GlobalAlloc(GMEM_SHARE, 1024);
	g_argv = (char *)GlobalLock(h_global_argv);
#else
	g_argv = (char *)h_global_argv;
#endif

	other_GiveFnptrsToDll = (GIVEFNPTRSTODLL)GetProcAddress(h_Library, "GiveFnptrsToDll");
	if (other_GiveFnptrsToDll == NULL)// Can't find GiveFnptrsToDll!
		ALERT(at_error, "XBM: Can't get GiveFnptrsToDll!\n");

	other_GetEntityAPI = (GETENTITYAPI)GetProcAddress(h_Library, "GetEntityAPI");
	if (other_GetEntityAPI == NULL)// Can't find GetEntityAPI!
		ALERT(at_warning, "XBM: Can't get GetEntityAPI!\n");

	other_GetEntityAPI2 = (GETENTITYAPI2)GetProcAddress(h_Library, "GetEntityAPI2");
	if (other_GetEntityAPI2 == NULL)// Can't find GetEntityAPI2!
		ALERT(at_warning, "XBM: Can't get GetEntityAPI2!\n");

	other_GetNewDLLFunctions = (GETNEWDLLFUNCTIONS)GetProcAddress(h_Library, "GetNewDLLFunctions");
	if (other_GetNewDLLFunctions == NULL)// Can't find GetNewDLLFunctions!
		ALERT(at_warning, "XBM: Can't get GetNewDLLFunctions!\n");

	other_Server_GetBlendingInterface = (SERVER_GETBLENDINGINTERFACE)GetProcAddress(h_Library, "Server_GetBlendingInterface");
	if (other_Server_GetBlendingInterface == NULL)// purely optional
		ALERT(at_console, "XBM: Can't get GetBlendingInterface!\n");

	// XDM3036: TODO
	if (mod_id == TYRIAN_DLL)
	{
		XDM_EntityRelationship	= (XDM_ENTITY_RELATIONSHIP_FUNC)GetProcAddress(h_Library, "EntityRelationship");
		XDM_EntityIs			= (XDM_ENTITY_IS_FUNC)GetProcAddress(h_Library, "EntityIs");
		XDM_CanHaveItem			= (XDM_CAN_HAVE_ITEM_FUNC)GetProcAddress(h_Library, "CanHaveItem");
	}

#ifndef __linux__
	LoadSymbols(game_dll_filename);  // Load exported symbol table
#endif

	pengfuncsFromEngine->pfnCmd_Args = Cmd_Args;
	pengfuncsFromEngine->pfnCmd_Argv = Cmd_Argv;
	pengfuncsFromEngine->pfnCmd_Argc = Cmd_Argc;
	pengfuncsFromEngine->pfnPrecacheModel = pfnPrecacheModel;
	pengfuncsFromEngine->pfnPrecacheSound = pfnPrecacheSound;
	pengfuncsFromEngine->pfnSetModel = pfnSetModel;
	pengfuncsFromEngine->pfnModelIndex = pfnModelIndex;
	pengfuncsFromEngine->pfnModelFrames = pfnModelFrames;
	pengfuncsFromEngine->pfnSetSize = pfnSetSize;
	pengfuncsFromEngine->pfnChangeLevel = pfnChangeLevel;
	pengfuncsFromEngine->pfnGetSpawnParms = pfnGetSpawnParms;
	pengfuncsFromEngine->pfnSaveSpawnParms = pfnSaveSpawnParms;
	pengfuncsFromEngine->pfnVecToYaw = pfnVecToYaw;
	pengfuncsFromEngine->pfnVecToAngles = pfnVecToAngles;
	pengfuncsFromEngine->pfnMoveToOrigin = pfnMoveToOrigin;
	pengfuncsFromEngine->pfnChangeYaw = pfnChangeYaw;
	pengfuncsFromEngine->pfnChangePitch = pfnChangePitch;
	pengfuncsFromEngine->pfnFindEntityByString = pfnFindEntityByString;
	pengfuncsFromEngine->pfnGetEntityIllum = pfnGetEntityIllum;
	pengfuncsFromEngine->pfnFindEntityInSphere = pfnFindEntityInSphere;
	pengfuncsFromEngine->pfnFindClientInPVS = pfnFindClientInPVS;
	pengfuncsFromEngine->pfnEntitiesInPVS = pfnEntitiesInPVS;
	pengfuncsFromEngine->pfnMakeVectors = pfnMakeVectors;
	pengfuncsFromEngine->pfnAngleVectors = pfnAngleVectors;
	pengfuncsFromEngine->pfnCreateEntity = pfnCreateEntity;
	pengfuncsFromEngine->pfnRemoveEntity = pfnRemoveEntity;
	pengfuncsFromEngine->pfnCreateNamedEntity = pfnCreateNamedEntity;
	pengfuncsFromEngine->pfnMakeStatic = pfnMakeStatic;
	pengfuncsFromEngine->pfnEntIsOnFloor = pfnEntIsOnFloor;
	pengfuncsFromEngine->pfnDropToFloor = pfnDropToFloor;
	pengfuncsFromEngine->pfnWalkMove = pfnWalkMove;
	pengfuncsFromEngine->pfnSetOrigin = pfnSetOrigin;
	pengfuncsFromEngine->pfnEmitSound = pfnEmitSound;
	pengfuncsFromEngine->pfnEmitAmbientSound = pfnEmitAmbientSound;
	pengfuncsFromEngine->pfnTraceLine = pfnTraceLine;
	pengfuncsFromEngine->pfnTraceToss = pfnTraceToss;
	pengfuncsFromEngine->pfnTraceMonsterHull = pfnTraceMonsterHull;
	pengfuncsFromEngine->pfnTraceHull = pfnTraceHull;
	pengfuncsFromEngine->pfnTraceModel = pfnTraceModel;
	pengfuncsFromEngine->pfnTraceTexture = pfnTraceTexture;
	pengfuncsFromEngine->pfnTraceSphere = pfnTraceSphere;
	pengfuncsFromEngine->pfnGetAimVector = pfnGetAimVector;
	pengfuncsFromEngine->pfnServerCommand = pfnServerCommand;
	pengfuncsFromEngine->pfnServerExecute = pfnServerExecute;
	pengfuncsFromEngine->pfnClientCommand = pfnClientCommand;
	pengfuncsFromEngine->pfnParticleEffect = pfnParticleEffect;
	pengfuncsFromEngine->pfnLightStyle = pfnLightStyle;
	pengfuncsFromEngine->pfnDecalIndex = pfnDecalIndex;
	pengfuncsFromEngine->pfnPointContents = pfnPointContents;
	pengfuncsFromEngine->pfnMessageBegin = pfnMessageBegin;
	pengfuncsFromEngine->pfnMessageEnd = pfnMessageEnd;
	pengfuncsFromEngine->pfnWriteByte = pfnWriteByte;
	pengfuncsFromEngine->pfnWriteChar = pfnWriteChar;
	pengfuncsFromEngine->pfnWriteShort = pfnWriteShort;
	pengfuncsFromEngine->pfnWriteLong = pfnWriteLong;
	pengfuncsFromEngine->pfnWriteAngle = pfnWriteAngle;
	pengfuncsFromEngine->pfnWriteCoord = pfnWriteCoord;
	pengfuncsFromEngine->pfnWriteString = pfnWriteString;
	pengfuncsFromEngine->pfnWriteEntity = pfnWriteEntity;
	pengfuncsFromEngine->pfnCVarRegister = pfnCVarRegister;
	pengfuncsFromEngine->pfnCVarGetFloat = pfnCVarGetFloat;
	pengfuncsFromEngine->pfnCVarGetString = pfnCVarGetString;
	pengfuncsFromEngine->pfnCVarSetFloat = pfnCVarSetFloat;
	pengfuncsFromEngine->pfnCVarSetString = pfnCVarSetString;
	pengfuncsFromEngine->pfnPvAllocEntPrivateData = pfnPvAllocEntPrivateData;
	pengfuncsFromEngine->pfnPvEntPrivateData = pfnPvEntPrivateData;
	pengfuncsFromEngine->pfnFreeEntPrivateData = pfnFreeEntPrivateData;
	pengfuncsFromEngine->pfnSzFromIndex = pfnSzFromIndex;
	pengfuncsFromEngine->pfnAllocString = pfnAllocString;
	pengfuncsFromEngine->pfnGetVarsOfEnt = pfnGetVarsOfEnt;
	pengfuncsFromEngine->pfnPEntityOfEntOffset = pfnPEntityOfEntOffset;
	pengfuncsFromEngine->pfnEntOffsetOfPEntity = pfnEntOffsetOfPEntity;
	pengfuncsFromEngine->pfnIndexOfEdict = pfnIndexOfEdict;
	pengfuncsFromEngine->pfnPEntityOfEntIndex = pfnPEntityOfEntIndex;
	pengfuncsFromEngine->pfnFindEntityByVars = pfnFindEntityByVars;
	pengfuncsFromEngine->pfnGetModelPtr = pfnGetModelPtr;
	pengfuncsFromEngine->pfnRegUserMsg = pfnRegUserMsg;
	pengfuncsFromEngine->pfnAnimationAutomove = pfnAnimationAutomove;
	pengfuncsFromEngine->pfnGetBonePosition = pfnGetBonePosition;
	pengfuncsFromEngine->pfnFunctionFromName = pfnFunctionFromName;
	pengfuncsFromEngine->pfnNameForFunction = pfnNameForFunction;
	pengfuncsFromEngine->pfnClientPrintf = pfnClientPrintf;
	pengfuncsFromEngine->pfnServerPrint = pfnServerPrint;
	pengfuncsFromEngine->pfnCmd_Args = Cmd_Args;
	pengfuncsFromEngine->pfnCmd_Argv = Cmd_Argv;
	pengfuncsFromEngine->pfnCmd_Argc = Cmd_Argc;
	pengfuncsFromEngine->pfnGetAttachment = pfnGetAttachment;
	pengfuncsFromEngine->pfnCRC32_Init = pfnCRC32_Init;
	pengfuncsFromEngine->pfnCRC32_ProcessBuffer = pfnCRC32_ProcessBuffer;
	pengfuncsFromEngine->pfnCRC32_ProcessByte = pfnCRC32_ProcessByte;
	pengfuncsFromEngine->pfnCRC32_Final = pfnCRC32_Final;
	pengfuncsFromEngine->pfnRandomLong = pfnRandomLong;
	pengfuncsFromEngine->pfnRandomFloat = pfnRandomFloat;
	pengfuncsFromEngine->pfnSetView = pfnSetView;
	pengfuncsFromEngine->pfnTime = pfnTime;
	pengfuncsFromEngine->pfnCrosshairAngle = pfnCrosshairAngle;
	pengfuncsFromEngine->pfnLoadFileForMe = pfnLoadFileForMe;
	pengfuncsFromEngine->pfnFreeFile = pfnFreeFile;
	pengfuncsFromEngine->pfnEndSection = pfnEndSection;
	pengfuncsFromEngine->pfnCompareFileTime = pfnCompareFileTime;
	pengfuncsFromEngine->pfnGetGameDir = pfnGetGameDir;
	pengfuncsFromEngine->pfnCvar_RegisterVariable = pfnCvar_RegisterVariable;
	pengfuncsFromEngine->pfnFadeClientVolume = pfnFadeClientVolume;
	pengfuncsFromEngine->pfnSetClientMaxspeed = pfnSetClientMaxspeed;
	pengfuncsFromEngine->pfnCreateFakeClient = pfnCreateFakeClient;
	pengfuncsFromEngine->pfnRunPlayerMove = pfnRunPlayerMove;
	pengfuncsFromEngine->pfnNumberOfEntities = pfnNumberOfEntities;
	pengfuncsFromEngine->pfnGetInfoKeyBuffer = pfnGetInfoKeyBuffer;
	pengfuncsFromEngine->pfnInfoKeyValue = pfnInfoKeyValue;
	pengfuncsFromEngine->pfnSetKeyValue = pfnSetKeyValue;
	pengfuncsFromEngine->pfnSetClientKeyValue = pfnSetClientKeyValue;
	pengfuncsFromEngine->pfnIsMapValid = pfnIsMapValid;
	pengfuncsFromEngine->pfnStaticDecal = pfnStaticDecal;
	pengfuncsFromEngine->pfnPrecacheGeneric = pfnPrecacheGeneric;
	pengfuncsFromEngine->pfnGetPlayerUserId = pfnGetPlayerUserId;
	pengfuncsFromEngine->pfnBuildSoundMsg = pfnBuildSoundMsg;
	pengfuncsFromEngine->pfnIsDedicatedServer = pfnIsDedicatedServer;
	pengfuncsFromEngine->pfnCVarGetPointer = pfnCVarGetPointer;
	pengfuncsFromEngine->pfnGetPlayerWONId = pfnGetPlayerWONId;

	// SDK 2.0 additions...
	pengfuncsFromEngine->pfnInfo_RemoveKey = pfnInfo_RemoveKey;
	pengfuncsFromEngine->pfnGetPhysicsKeyValue = pfnGetPhysicsKeyValue;
	pengfuncsFromEngine->pfnSetPhysicsKeyValue = pfnSetPhysicsKeyValue;
	pengfuncsFromEngine->pfnGetPhysicsInfoString = pfnGetPhysicsInfoString;
	pengfuncsFromEngine->pfnPrecacheEvent = pfnPrecacheEvent;
	pengfuncsFromEngine->pfnPlaybackEvent = pfnPlaybackEvent;
	pengfuncsFromEngine->pfnSetFatPVS = pfnSetFatPVS;
	pengfuncsFromEngine->pfnSetFatPAS = pfnSetFatPAS;
	pengfuncsFromEngine->pfnCheckVisibility = pfnCheckVisibility;
	pengfuncsFromEngine->pfnDeltaSetField = pfnDeltaSetField;
	pengfuncsFromEngine->pfnDeltaUnsetField = pfnDeltaUnsetField;
	pengfuncsFromEngine->pfnDeltaAddEncoder = pfnDeltaAddEncoder;
	pengfuncsFromEngine->pfnGetCurrentPlayer = pfnGetCurrentPlayer;
	pengfuncsFromEngine->pfnCanSkipPlayer = pfnCanSkipPlayer;
	pengfuncsFromEngine->pfnDeltaFindField = pfnDeltaFindField;
	pengfuncsFromEngine->pfnDeltaSetFieldByIndex = pfnDeltaSetFieldByIndex;
	pengfuncsFromEngine->pfnDeltaUnsetFieldByIndex = pfnDeltaUnsetFieldByIndex;
	pengfuncsFromEngine->pfnSetGroupMask = pfnSetGroupMask;
	pengfuncsFromEngine->pfnCreateInstancedBaseline = pfnCreateInstancedBaseline;
	pengfuncsFromEngine->pfnCvar_DirectSet = pfnCvar_DirectSet;
	pengfuncsFromEngine->pfnForceUnmodified = pfnForceUnmodified;
	pengfuncsFromEngine->pfnGetPlayerStats = pfnGetPlayerStats;
	pengfuncsFromEngine->pfnAddServerCommand = pfnAddServerCommand;

	// SDK 2.3 additions...
	pengfuncsFromEngine->pfnVoice_GetClientListening = pfnVoice_GetClientListening;
	pengfuncsFromEngine->pfnVoice_SetClientListening = pfnVoice_SetClientListening;
	pengfuncsFromEngine->pfnGetPlayerAuthId = pfnGetPlayerAuthId;

#ifdef SVDLL_NEWFUNCTIONS
	pengfuncsFromEngine->pfnSequenceGet = pfnSequenceGet;
	pengfuncsFromEngine->pfnSequencePickSentence = pfnSequencePickSentence;
	// LH: Give access to filesize via filesystem
	pengfuncsFromEngine->pfnGetFileSize = pfnGetFileSize;
	pengfuncsFromEngine->pfnGetApproxWavePlayLen = pfnGetApproxWavePlayLen;
	// MDC: Added for CZ career-mode
	pengfuncsFromEngine->pfnIsCareerMatch = pfnIsCareerMatch;
	// BGC: return the number of characters of the localized string referenced by using "label"
	pengfuncsFromEngine->pfnGetLocalizedStringLength = pfnGetLocalizedStringLength;
	// BGC: added to facilitate persistent storage of tutor message decay values for
	// different career game profiles.  Also needs to persist regardless of mp.dll being
	// destroyed and recreated.
	pengfuncsFromEngine->pfnRegisterTutorMessageShown = pfnRegisterTutorMessageShown;
	pengfuncsFromEngine->pfnGetTimesTutorMessageShown = pfnGetTimesTutorMessageShown;
	pengfuncsFromEngine->pfnProcessTutorMessageDecayBuffer = pfnProcessTutorMessageDecayBuffer;
	pengfuncsFromEngine->pfnConstructTutorMessageDecayBuffer = pfnConstructTutorMessageDecayBuffer;
	pengfuncsFromEngine->pfnResetTutorMessageDecayData = pfnResetTutorMessageDecayData;
	pengfuncsFromEngine->pfnQueryClientCvarValue = pfnQueryClientCvarValue;
	pengfuncsFromEngine->pfnQueryClientCvarValue2 = pfnQueryClientCvarValue2;
#endif

	// give the engine functions to the other DLL...
	if (other_GiveFnptrsToDll)
		(*other_GiveFnptrsToDll)(pengfuncsFromEngine, pGlobals);
}
