#include "extdll.h"
#include "enginecallback.h"
#include "util.h"
#include "cbase.h"
#include "entity_state.h"
#include "usercmd.h"
#include "bot.h"
#include "bot_func.h"
#include "bot_cvar.h"
#include "waypoint.h"
//#include "pm_shared.h"

extern GETENTITYAPI other_GetEntityAPI;
extern GETENTITYAPI2 other_GetEntityAPI2;
extern GETNEWDLLFUNCTIONS other_GetNewDLLFunctions;
extern SERVER_GETBLENDINGINTERFACE other_Server_GetBlendingInterface;// HPB40

extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;
extern char *g_argv;

char cfgfile[] = "botmatch.cfg";

DLL_FUNCTIONS other_gFunctionTable;
NEW_DLL_FUNCTIONS other_gNewFunctionTable;// XDM
DLL_GLOBAL const Vector g_vecZero = Vector(0,0,0);

int mod_id = 0;

int g_iModelIndexAnimglow01 = 0;
int g_iModelIndexBeamsAll = 0;

#define FAKE_CMD_ARG_LEN	64

bool isFakeClientCommand = false;
int fake_arg_count = 0;
int IsDedicatedServer = 0;
float bot_check_time = 60.0;
int min_bots = -1;
int max_bots = -1;
int num_bots = 0;
int prev_num_bots = 0;
edict_t *clients[MAX_PLAYERS];
edict_t *listenserver_edict = NULL;
int bot_stop = 0;

bool g_GameRules = false;
bool is_team_play = false;
bool checked_teamplay = false;

bool need_to_open_cfg = true;
FILE *bot_cfg_fp = NULL;
float bot_cfg_pause_time = 0.0f;

bool spawn_time_reset = false;
float respawn_time = 0.0;

// TheFatal's method for calculating the msecval
int msecnum;
float msecdel;
float msecval;
// Now these are global


//cvar_t sv_bot					= { "HPB_bot", "" };
cvar_t g_bot_skill				= { "bot_skill",				"2", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_strafe_percent		= { "bot_strafe_percent",		"20", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_chat_percent		= { "bot_chat_percent",			"10", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_taunt_percent		= { "bot_taunt_percent",		"20", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_whine_percent		= { "bot_whine_percent",		"10", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_grenade_time		= { "bot_grenade_time",			"15", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_logo_percent		= { "bot_logo_percent",			"40", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_logo_custom		= { "bot_logo_custom",			"1", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_chat_enable		= { "bot_chat_enable",			"1", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_chat_tag_percent	= { "bot_chat_tag_percent",		"80", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_chat_drop_percent	= { "bot_chat_drop_percent",	"10", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_chat_swap_percent	= { "bot_chat_swap_percent",	"10", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_chat_lower_percent	= { "bot_chat_lower_percent",	"50", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_reaction			= { "bot_reaction",				"2", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_dont_shoot			= { "bot_dont_shoot",			"0", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_check_lightlevel	= { "bot_check_lightlevel",		"0", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_use_flashlight		= { "bot_use_flashlight",		"0", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_bot_follow_actions		= { "bot_follow_actions",		"1", FCVAR_SERVER|FCVAR_EXTDLL };
cvar_t g_botmatch				= { "botmatch",					"0", FCVAR_SERVER };
//cvar_t *g_pBotmatch = &g_botmatch;
cvar_t	*g_pdeveloper = NULL;
cvar_t 	*g_pmp_friendlyfire = NULL;
cvar_t 	*g_pmp_noshooting = NULL;
cvar_t 	*g_pmp_revengemode = NULL;
cvar_t 	*g_psv_maxspeed = NULL;

void Cmd_KickBots(void)
{
	for (int bot_index = 0; bot_index < MAX_PLAYERS; ++bot_index)
	{
		if (bots[bot_index].is_used)
		{
			char cmd[40];
			sprintf(cmd, "kick # %d\n", ENTINDEX(bots[bot_index].pEdict));
			bots[bot_index].respawn_state = RESPAWN_IDLE;
			SERVER_COMMAND(cmd);  // kick the bot using "kick # id"
			--num_bots;
		}
	}
	SERVER_EXECUTE();
}

void Cmd_BotCreate(void)
{
	if (g_botmatch.value <= 0)
	{
		ALERT(at_console, "Botmatch is not allowed.\n");
		return;
	}

	const char *arg1 = Cmd_Argv(1);
	const char *arg2 = Cmd_Argv(2);
	if (!arg1 || !arg2)
	{
		ALERT(at_console, "Usage: %s <model> <name> [skill] [topcolor] [bottomcolor] [skin] [reaction time] [strafe %] [chat %] [taunt %] [whine %]\n", Cmd_Argv(0));
		return;
	}
	int skill = -1, topcolor = -1, bottomcolor = -1, skin = 0;
	int reaction_time = -1, strafe_prc = -1, chat_prc = -1, taunt_prc = -1, whine_prc = -1;
	const char *arg3 = Cmd_Argv(3);
	const char *arg4 = Cmd_Argv(4);
	const char *arg5 = Cmd_Argv(5);
	const char *arg6 = Cmd_Argv(6);
	const char *arg7 = Cmd_Argv(7);
	const char *arg8 = Cmd_Argv(8);
	const char *arg9 = Cmd_Argv(9);
	const char *arg10 = Cmd_Argv(10);
	const char *arg11 = Cmd_Argv(11);
	if (arg3 && *arg3) skill = atoi(arg3);
	if (arg4 && *arg4) topcolor = atoi(arg4);
	if (arg5 && *arg5) bottomcolor = atoi(arg5);
	if (arg6 && *arg6) skin = atoi(arg6);
	if (arg7 && *arg7) reaction_time = atoi(arg7);
	if (arg8 && *arg8) strafe_prc = atoi(arg8);
	if (arg9 && *arg9) chat_prc = atoi(arg9);
	if (arg10 && *arg10) taunt_prc = atoi(arg10);
	if (arg11 && *arg11) whine_prc = atoi(arg11);
	BOT_CREATE((char *)arg1, (char *)arg2, skill, topcolor, bottomcolor, skin, reaction_time, strafe_prc, chat_prc, taunt_prc, whine_prc, (char *)Cmd_Argv(12));
	bot_cfg_pause_time = gpGlobals->time + 0.25f;
}


void GameDLLInit(void)
{
//	cvar_t *pHostGameLoaded = CVAR_GET_POINTER("host_gameloaded");// Xash detection
	SERVER_PRINT(UTIL_VarArgs("Initializing XBM (based on HPB bot) server DLL (build %s)\n", __DATE__));
//	if (pHostGameLoaded)
//		SERVER_PRINT("Xash detected\n");

	SERVER_PRINT("nRegistering CVars...\n");
//	CVAR_REGISTER(&sv_bot);
	CVAR_REGISTER(&g_bot_skill);
	CVAR_REGISTER(&g_bot_strafe_percent);
	CVAR_REGISTER(&g_bot_chat_percent);
	CVAR_REGISTER(&g_bot_taunt_percent);
	CVAR_REGISTER(&g_bot_whine_percent);
	CVAR_REGISTER(&g_bot_grenade_time);
	CVAR_REGISTER(&g_bot_logo_percent);
	CVAR_REGISTER(&g_bot_logo_custom);
	CVAR_REGISTER(&g_bot_chat_enable);
	CVAR_REGISTER(&g_bot_chat_tag_percent);
	CVAR_REGISTER(&g_bot_chat_drop_percent);
	CVAR_REGISTER(&g_bot_chat_swap_percent);
	CVAR_REGISTER(&g_bot_chat_lower_percent);
	CVAR_REGISTER(&g_bot_reaction);
	CVAR_REGISTER(&g_bot_dont_shoot);
	CVAR_REGISTER(&g_bot_check_lightlevel);
	CVAR_REGISTER(&g_bot_use_flashlight);
	CVAR_REGISTER(&g_bot_follow_actions);
	CVAR_REGISTER(&g_botmatch);

	ADD_SERVER_COMMAND("bot_create", Cmd_BotCreate);
	ADD_SERVER_COMMAND("bot_kickall", Cmd_KickBots);

	IsDedicatedServer = IS_DEDICATED_SERVER();

	int i;
	for (i=0; i<MAX_PLAYERS; ++i)
		clients[i] = NULL;

	// initialize the bots array of structures...
	memset(bots, 0, sizeof(bot_t)*MAX_PLAYERS);
	BotLogoInit();
	BotWeaponInit();// XDM3035
	LoadBotChat();

	SERVER_PRINT("Done. XBM DLL initialized.\n");
	(*other_gFunctionTable.pfnGameInit)();

	g_pdeveloper = CVAR_GET_POINTER("developer");
	g_pmp_friendlyfire = CVAR_GET_POINTER("mp_friendlyfire");
	g_pmp_noshooting = CVAR_GET_POINTER("mp_noshooting");
	g_pmp_revengemode = CVAR_GET_POINTER("mp_revengemode");
	g_psv_maxspeed = CVAR_GET_POINTER("sv_maxspeed");
//	if (mod_id == XDM_DLL)// XDM3035
//		g_pBotmatch = CVAR_GET_POINTER("bot_allow");
//	else
//		g_pBotmatch = &g_botmatch;
}

int DispatchSpawn(edict_t *pent)
{
	if (gpGlobals->deathmatch)
	{
		char *pClassname = (char *)STRING(pent->v.classname);
		if (strcmp(pClassname, "worldspawn") == 0)
		{
			// do level initialization stuff here...
			SERVER_PRINT("XBM: Initializing waypoints...\n");
			WaypointInit();
			WaypointLoad(NULL);
			PRECACHE_SOUND("weapons/ric1.wav");		// waypoint add
			PRECACHE_SOUND("weapons/mine_activate.wav");  // waypoint delete
			PRECACHE_SOUND("common/wpn_hudoff.wav");		// path add/delete start
			PRECACHE_SOUND("common/wpn_hudon.wav");		 // path add/delete done
			PRECACHE_SOUND("common/wpn_moveselect.wav");  // path add/delete cancel
			PRECACHE_SOUND("common/wpn_denyselect.wav");  // path add/delete error
			PRECACHE_SOUND("player/sprayer.wav");			// logo spray sound
			g_iModelIndexAnimglow01 = PRECACHE_MODEL("sprites/animglow01.spr");
			g_iModelIndexBeamsAll = PRECACHE_MODEL("sprites/beams_all.spr");
			g_GameRules = true;
			is_team_play = false;
			checked_teamplay = false;
			bot_cfg_pause_time = 0.0;
			respawn_time = 0.0;
			spawn_time_reset = false;
			prev_num_bots = num_bots;
			num_bots = 0;
			bot_check_time = gpGlobals->time + 60.0f;
		}
	}
	return (*other_gFunctionTable.pfnSpawn)(pent);
}

void DispatchThink(edict_t *pent)
{
	(*other_gFunctionTable.pfnThink)(pent);
}

void DispatchUse(edict_t *pentUsed, edict_t *pentOther)
{
	(*other_gFunctionTable.pfnUse)(pentUsed, pentOther);
}

void DispatchTouch(edict_t *pentTouched, edict_t *pentOther)
{
	(*other_gFunctionTable.pfnTouch)(pentTouched, pentOther);
}

void DispatchBlocked(edict_t *pentBlocked, edict_t *pentOther)
{
	(*other_gFunctionTable.pfnBlocked)(pentBlocked, pentOther);
}

void DispatchKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd)
{
	(*other_gFunctionTable.pfnKeyValue)(pentKeyvalue, pkvd);
}

void DispatchSave(edict_t *pent, SAVERESTOREDATA *pSaveData)
{
	(*other_gFunctionTable.pfnSave)(pent, pSaveData);
}

int DispatchRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity)
{
	return (*other_gFunctionTable.pfnRestore)(pent, pSaveData, globalEntity);
}

void DispatchObjectCollisionBox(edict_t *pent)
{
	(*other_gFunctionTable.pfnSetAbsBox)(pent);
}

void SaveWriteFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount)
{
	(*other_gFunctionTable.pfnSaveWriteFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
}

void SaveReadFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount)
{
	(*other_gFunctionTable.pfnSaveReadFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
}

void SaveGlobalState(SAVERESTOREDATA *pSaveData)
{
	(*other_gFunctionTable.pfnSaveGlobalState)(pSaveData);
}

void RestoreGlobalState(SAVERESTOREDATA *pSaveData)
{
	(*other_gFunctionTable.pfnRestoreGlobalState)(pSaveData);
}

void ResetGlobalState(void)
{
	(*other_gFunctionTable.pfnResetGlobalState)();
}

BOOL ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{ 
	if (gpGlobals->deathmatch)
	{
		// check if this client is the listen server client
		if (strcmp(pszAddress, "loopback") == 0)
			listenserver_edict = pEntity;// save the edict of the listen server client...

		// check if this is NOT a bot joining the server...
/* XDM3037: makes no sence because server reports FULL and no connection is made
		if (strcmp(pszAddress, "127.0.0.1") != 0)// !(pEntity->v.flags & FL_FAKECLIENT)
		{
			// don't try to add bots for 60 seconds, give client time to get added
			bot_check_time = gpGlobals->time + 60.0f;

			int count = UTIL_CountBots();
			// if there are currently more than the minimum number of bots running then kick one of the bots off the server...
			if ((count > min_bots) && (min_bots != -1))
			{
				for (int i=0; i < MAX_PLAYERS; ++i)
				{
					if (bots[i].is_used)  // is this slot used?
					{
						char cmd[80];
						sprintf(cmd, "kick \"%s\"\n", bots[i].name);
						//sprintf(cmd, "kick # %d\n", i+1);
						SERVER_COMMAND(cmd);  // kick the bot using (kick "name")
						break;
					}
				}
			}
		}*/
	}
	return (*other_gFunctionTable.pfnClientConnect)(pEntity, pszName, pszAddress, szRejectReason);
}

void ClientDisconnect(edict_t *pEntity)
{
	if (gpGlobals->deathmatch)
	{
		int i = 0;
		for (i=0; i < MAX_PLAYERS; ++i)// bot indexes are 0-31
		{
			if (bots[i].pBotUser == pEntity)// XDM3035b: this bot was used by disconnected player
			{
				bots[i].pBotUser = NULL;
				bots[i].is_used = 0;
			}
			if (clients[i] == pEntity)
				clients[i] = NULL;//cli = i;
		}

		pEntity->v.flags &= ~FL_FAKECLIENT;// XDM3037: otherwise next joining player will have it

		bot_t *pBot = UTIL_GetBotPointer(pEntity);
		if (pBot)
		{
//			ALERT(at_console, "DEBUG: Bot %s kicked!\n", bots[i].name);
			// someone kicked this bot off of the server...
			BotSpawnInit(pBot);// XDM: this resets bot data to default values
			pBot->pEdict = NULL;// XDM
			pBot->is_used = false;// this slot is now free to use
			pBot->f_kick_time = gpGlobals->time;  // save the kicked time
		}
	}
	(*other_gFunctionTable.pfnClientDisconnect)(pEntity);
}

void ClientKill(edict_t *pEntity)
{
	(*other_gFunctionTable.pfnClientKill)(pEntity);
}

void ClientPutInServer(edict_t *pEntity)
{
	int i = 0;
	while ((i < MAX_PLAYERS) && (clients[i] != NULL))
		++i;

	if (i < MAX_PLAYERS)
		clients[i] = pEntity;  // store this clients edict in the clients array

	(*other_gFunctionTable.pfnClientPutInServer)(pEntity);
}

void ClientCommand(edict_t *pEntity)
{
	const char *pcmd = Cmd_Argv(0);
	const char *arg1 = Cmd_Argv(1);
	const char *arg2 = Cmd_Argv(2);
	// only allow custom commands if deathmatch mode and NOT dedicated server and client sending command is the listen server client...
	if ((gpGlobals->deathmatch) && (!IsDedicatedServer) && (pEntity == listenserver_edict))
	{
		if (FStrEq(pcmd, "addbot"))
		{
			ALERT(at_console, "Use 'bot_create' instead of '%s'\n", pcmd);
			return;
		}
		else if (FStrEq(pcmd, "bot_use"))
		{
			if (!arg1 || !arg2)
			{
				ALERT(at_notice, "Usage: %s <use type> <bot name>\n", pcmd);
				return;
			}

			int utype = atoi(arg1);
			for(int i=0; i<MAX_PLAYERS; ++i)
			{
				if (bots[i].is_used && stricmp(arg2, bots[i].name) == 0)
				{
					bot_t *pBot = &bots[i];
					if(pBot->use_type == utype)
						BotUseCommand(pBot, pEntity, BOT_USE_NONE);
					else
						BotUseCommand(pBot, pEntity, utype);
				}
			}
			return;
		}
		else if (FStrEq(pcmd, "bot_use_forward"))// kind of obsolete
		{
			if (!arg1)
			{
				ALERT(at_notice, "Usage: %s <use type>\n", pcmd);
				return;
			}
			TraceResult tr;
			UTIL_TraceLine(pEntity->v.origin + pEntity->v.view_ofs, pEntity->v.origin + pEntity->v.view_ofs + gpGlobals->v_forward * 96, dont_ignore_monsters, pEntity, &tr);
			if (tr.flFraction != 1.0 && !FNullEnt(tr.pHit))
			{
				bot_t *pBot = UTIL_GetBotPointer(tr.pHit);
				if (pBot)
				{
					int utype = atoi(arg1);
					if (pBot->use_type == utype)
						BotUseCommand(pBot, pEntity, BOT_USE_NONE);
					else
						BotUseCommand(pBot, pEntity, utype);
				}
			}
			return;
		}
		else if (FStrEq(pcmd, "debug_engine"))
		{
			if ((arg1 != NULL) && (*arg1 != 0))
			{
				int temp = atoi(arg1);
				if(temp)
					debug_engine = 1;
				else
					debug_engine = 0;
			}

			if (debug_engine)
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "debug_engine ENABLED!\n");
			else
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "debug_engine DISABLED!\n");

			return;
		}
		else if (FStrEq(pcmd, "waypoint"))
		{
			if (FStrEq(arg1, "on"))
			{
				g_waypoint_on = true;
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "waypoints are ON\n");
			}
			else if (FStrEq(arg1, "off"))
			{
				g_waypoint_on = false;
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "waypoints are OFF\n");
			}
			else if (FStrEq(arg1, "add"))
			{
				if (!g_waypoint_on)
					g_waypoint_on = true;  // turn waypoints on if off

				WaypointAdd(pEntity);
			}
			else if (FStrEq(arg1, "delete"))
			{
				if (!g_waypoint_on)
					g_waypoint_on = true;  // turn waypoints on if off

				WaypointDelete(pEntity);
			}
			else if (FStrEq(arg1, "save"))
			{
				WaypointSave();
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "waypoints saved\n");
			}
			else if (FStrEq(arg1, "load"))
			{
				if (WaypointLoad(pEntity))
					ClientPrint(pEntity, HUD_PRINTNOTIFY, "waypoints loaded\n");
			}
			else if (FStrEq(arg1, "info"))
			{
				WaypointPrintInfo(pEntity);
			}
			else if (FStrEq(arg1, "update"))
			{
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "updating waypoint tags...\n");
				WaypointUpdate(pEntity);
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "...update done!  (don't forget to save!)\n");
			}
			else
			{
				if (g_waypoint_on)
					ClientPrint(pEntity, HUD_PRINTNOTIFY, "waypoints are ON\n");
				else
					ClientPrint(pEntity, HUD_PRINTNOTIFY, "waypoints are OFF\n");
			}
			return;
		}
		else if (FStrEq(pcmd, "autowaypoint"))
		{
			if (FStrEq(arg1, "on"))
			{
				g_auto_waypoint = true;
				g_waypoint_on = true;  // turn this on just in case
			}
			else if (FStrEq(arg1, "off"))
			{
				g_auto_waypoint = false;
			}

			if (g_auto_waypoint)
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "autowaypoint is ON\n");
			else
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "autowaypoint is OFF\n");

			return;
		}
		else if (FStrEq(pcmd, "pathwaypoint"))
		{
			if (FStrEq(arg1, "on"))
			{
				g_path_waypoint = true;
				g_waypoint_on = true;  // turn this on just in case
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "pathwaypoint is ON\n");
			}
			else if (FStrEq(arg1, "off"))
			{
				g_path_waypoint = false;
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "pathwaypoint is OFF\n");
			}
			else if (FStrEq(arg1, "enable"))
			{
				g_path_waypoint_enable = true;
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "pathwaypoint is ENABLED\n");
			}
			else if (FStrEq(arg1, "disable"))
			{
				g_path_waypoint_enable = false;
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "pathwaypoint is DISABLED\n");
			}
			else if (FStrEq(arg1, "create1"))
			{
				WaypointCreatePath(pEntity, 1);
			}
			else if (FStrEq(arg1, "create2"))
			{
				WaypointCreatePath(pEntity, 2);
			}
			else if (FStrEq(arg1, "remove1"))
			{
				WaypointRemovePath(pEntity, 1);
			}
			else if (FStrEq(arg1, "remove2"))
			{
				WaypointRemovePath(pEntity, 2);
			}
			return;
		}
		else if (FStrEq(pcmd, "botobserver"))
		{
			if ((arg1 != NULL) && (*arg1 != 0))
			{
				int temp = atoi(arg1);
				if (temp)
					g_observer_mode = true;
				else
					g_observer_mode = false;
			}
			if (g_observer_mode)
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "observer mode ENABLED\n");
			else
				ClientPrint(pEntity, HUD_PRINTNOTIFY, "observer mode DISABLED\n");

			return;
		}
		else if (FStrEq(pcmd, "bot_gamemode"))
		{
			ALERT(at_console, "XBM: game mode %d dm %g svflags %g\n", g_iGameType, gpGlobals->deathmatch, gpGlobals->serverflags);
			return;
		}
#if _DEBUG
		else if (FStrEq(pcmd, "botstop"))
		{
			bot_stop = 1;
			return;
		}
		else if (FStrEq(pcmd, "botstart"))
		{
			bot_stop = 0;
			return;
		}
#endif
	}
	(*other_gFunctionTable.pfnClientCommand)(pEntity);
}

void ClientUserInfoChanged(edict_t *pEntity, char *infobuffer)
{
	(*other_gFunctionTable.pfnClientUserInfoChanged)(pEntity, infobuffer);
}

void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	g_intermission = false;
	(*other_gFunctionTable.pfnServerActivate)(pEdictList, edictCount, clientMax);
}

void ServerDeactivate(void)
{
	(*other_gFunctionTable.pfnServerDeactivate)();
}

void PlayerPreThink(edict_t *pEntity)
{
	(*other_gFunctionTable.pfnPlayerPreThink)(pEntity);
}

void PlayerPostThink(edict_t *pEntity)
{
	(*other_gFunctionTable.pfnPlayerPostThink)(pEntity);
}

void StartFrame(void)
{
	if (gpGlobals->deathmatch)
	{
		edict_t *pPlayer;
		static int i, index, player_index, bot_index;
		static float previous_time = -1.0f;
//		static float client_update_time = 0.0f;
//		clientdata_s cd;
		char msg[256];
		int count;

		// if a new map has started then (MUST BE FIRST IN StartFrame)...
		if ((gpGlobals->time + 0.1) < previous_time)
		{
			char filename[256];
			char mapname[64];

			msecnum = 0;
			msecdel = 0;
			msecval = 0;

			// check if mapname_bot.cfg file exists...
			strcpy(mapname, STRING(gpGlobals->mapname));
			strcat(mapname, "_");
			strcat(mapname, cfgfile);

			UTIL_BuildFileName(filename, "maps", mapname);

			if ((bot_cfg_fp = fopen(filename, "r")) != NULL)
			{
				sprintf(msg, "Executing %s\n", filename);
				ALERT(at_console, msg);

				for (index = 0; index < MAX_PLAYERS; ++index)
				{
					bots[index].is_used = false;
					bots[index].respawn_state = 0;
					bots[index].f_kick_time = 0.0;
				}

				if (IsDedicatedServer)
					bot_cfg_pause_time = gpGlobals->time + 5.0f;
				else
					bot_cfg_pause_time = gpGlobals->time + 20.0f;
			}
			else
			{
				count = 0;
				// mark the bots as needing to be respawned...
				for (index = 0; index < MAX_PLAYERS; ++index)
				{
					if (count >= prev_num_bots)
					{
						bots[index].is_used = false;
						bots[index].respawn_state = 0;
						bots[index].f_kick_time = 0.0;
					}
					if (bots[index].is_used)  // is this slot used?
					{
						bots[index].respawn_state = RESPAWN_NEED_TO_RESPAWN;
						count++;
					}
					// check for any bots that were very recently kicked...
					if ((bots[index].f_kick_time + 5.0) > previous_time)
					{
						bots[index].respawn_state = RESPAWN_NEED_TO_RESPAWN;
						count++;
					}
					else
						bots[index].f_kick_time = 0.0;  // reset to prevent false spawns later
				}

				// set the respawn time
				if (IsDedicatedServer)
					respawn_time = gpGlobals->time + 5.0f;
				else
					respawn_time = gpGlobals->time + 20.0f;
			}
//			client_update_time = gpGlobals->time + 10.0;  // start updating client data again
			bot_check_time = gpGlobals->time + 60.0f;
		}

		// adjust the millisecond delay based on the frame rate interval...
		if (msecdel <= gpGlobals->time)
		{
			msecdel = gpGlobals->time + 0.5f;
			if (msecnum > 0)
				msecval = 450.0f/msecnum;

			msecnum = 0;
		}
		else
			msecnum++;

		if (msecval < 1)// don't allow msec to be less than 1...
			msecval = 1;
		else if (msecval > 100)// ...or greater than 100
			msecval = 100;

		count = 0;
		if (bot_stop == 0)
		{
			for (bot_index = 0; bot_index < gpGlobals->maxClients; ++bot_index)
			{
				if ((bots[bot_index].is_used) && (bots[bot_index].respawn_state == RESPAWN_IDLE))// not respawning
				{
					if (bots[bot_index].pEdict->free)
					{
						bots[bot_index].is_used = false;
						ALERT(at_aiconsole, "XBM: Warning! Bot %d edict was freed!\n", bot_index);
						continue;
					}
// bots must think while spectating					if (bots[bot_index].need_to_initialize == false && bots[bot_index].not_started == false &&
//						bots[bot_index].pEdict->v.iuser1 != OBS_NONE)// spectator!
						BotThink(&bots[bot_index]);

					count++;
				}
			}
		}

		if (count > num_bots)
			num_bots = count;

		for (player_index = 1; player_index <= gpGlobals->maxClients; ++player_index)
		{
			pPlayer = INDEXENT(player_index);
			if (pPlayer && !pPlayer->free)
			{
				if ((g_waypoint_on) && FBitSet(pPlayer->v.flags, FL_CLIENT) && !FBitSet(pPlayer->v.flags, FL_FAKECLIENT))
					WaypointThink(pPlayer);
			}
		}

		if (g_botmatch.value > 0.0f)// XDM3035c: server may suddenly disallow bots
		{
			// are we currently respawning bots and is it time to spawn one yet?
			if ((respawn_time > 1.0f) && (respawn_time <= gpGlobals->time))
			{
				int index = 0;
				// find bot needing to be respawned...
				while ((index < MAX_PLAYERS) && (bots[index].respawn_state != RESPAWN_NEED_TO_RESPAWN))
					++index;

				if (index < MAX_PLAYERS)
				{
					if (bots[index].use_type != BOT_USE_NONE)// XDM: don't remember position for old map
						bots[index].use_type = BOT_USE_FOLLOW;
					
					bots[index].respawn_state = RESPAWN_IS_RESPAWNING;
					bots[index].is_used = false;		// free up this slot
					BOT_CREATE(bots[index].model, bots[index].name, bots[index].bot_skill, bots[index].top_color, bots[index].bottom_color, bots[index].model_skin, bots[index].reaction, bots[index].strafe_percent, bots[index].chat_percent, bots[index].taunt_percent, bots[index].whine_percent, "");
					respawn_time = gpGlobals->time + 1.0f;  // set next respawn time
					bot_check_time = gpGlobals->time + 5.0f;
				}
				else
					respawn_time = 0.0;
			}
		}

		if (g_GameRules)
		{
			if (need_to_open_cfg)  // have we open .cfg file yet?
			{
				char filename[256];
				need_to_open_cfg = false;  // only do this once!!!
				UTIL_BuildFileName(filename, (char *)cfgfile, NULL);
				sprintf(msg, "XBM: Executing %s\n", filename);
				ALERT(at_console, msg);
				bot_cfg_fp = fopen(filename, "r");
				if (bot_cfg_fp == NULL)
					ALERT(at_console, "%s file not found!\n", cfgfile);

				if (IsDedicatedServer)
					bot_cfg_pause_time = gpGlobals->time + 5.0f;
				else
					bot_cfg_pause_time = gpGlobals->time + 20.0f;
			}

			if (!IsDedicatedServer && !spawn_time_reset)
			{
				if (listenserver_edict != NULL)
				{
					if (IsAlive(listenserver_edict))
					{
						spawn_time_reset = true;
						if (respawn_time >= 1.0)
							respawn_time = min(respawn_time, gpGlobals->time + 1.0f);

						if (bot_cfg_pause_time >= 1.0)
							bot_cfg_pause_time = min(bot_cfg_pause_time, gpGlobals->time + 1.0f);
					}
				}
			}
			if ((bot_cfg_fp) && (bot_cfg_pause_time >= 1.0) && (bot_cfg_pause_time <= gpGlobals->time))
				ProcessBotCfgFile();// process .cfg file options...

			if (g_intermission)// XDM3037
			{
				for (bot_index = 0; bot_index < gpGlobals->maxClients; ++bot_index)
				{
					if (bots[bot_index].is_used)
						bots[bot_index].pEdict->v.button |= (IN_ATTACK|IN_JUMP);// XDM3037: press ready buttons
				}
			}
		}
		previous_time = gpGlobals->time;
	}
	(*other_gFunctionTable.pfnStartFrame)();
}

void ParmsNewLevel(void)
{
	(*other_gFunctionTable.pfnParmsNewLevel)();
}

void ParmsChangeLevel(void)
{
	(*other_gFunctionTable.pfnParmsChangeLevel)();
}

const char *GetGameDescription(void)
{
	return (*other_gFunctionTable.pfnGetGameDescription)();
}

void PlayerCustomization(edict_t *pEntity, struct customization_s *pCust)
{
/* XDM3035c: TODO	if (pCust)
	{
		bot_t *pBot = UTIL_GetBotPointer(pEntity);
		if (pBot)
		{
			if (pCust->resource.type == t_decal)
				strcpy(pBot->logo_name, pCust->resource.szFileName);
				?pBot->logo_index = pCust->resource.nIndex;
				?pBot->logo_index = pCust->nUserData2; // Second int is max # of frames.
		}
	}*/
	(*other_gFunctionTable.pfnPlayerCustomization)(pEntity, pCust);
}

void SpectatorConnect(edict_t *pEntity)
{
	(*other_gFunctionTable.pfnSpectatorConnect)(pEntity);
}

void SpectatorDisconnect(edict_t *pEntity)
{
	(*other_gFunctionTable.pfnSpectatorDisconnect)(pEntity);
}

void SpectatorThink(edict_t *pEntity)
{
	(*other_gFunctionTable.pfnSpectatorThink)(pEntity);
}

void Sys_Error(const char *error_string)
{
	(*other_gFunctionTable.pfnSys_Error)(error_string);
}

void PM_Move(struct playermove_s *ppmove, int server)
{
	(*other_gFunctionTable.pfnPM_Move)(ppmove, server);
}

void PM_Init(struct playermove_s *ppmove)
{
	(*other_gFunctionTable.pfnPM_Init)(ppmove);
}

char PM_FindTextureType(char *name)
{
	return (*other_gFunctionTable.pfnPM_FindTextureType)(name);
}

void SetupVisibility(edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas)
{
	(*other_gFunctionTable.pfnSetupVisibility)(pViewEntity, pClient, pvs, pas);
}

void UpdateClientData(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd)
{
	(*other_gFunctionTable.pfnUpdateClientData)(ent, sendweapons, cd);
}

int AddToFullPack(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet)
{
	return (*other_gFunctionTable.pfnAddToFullPack)(state, e, ent, host, hostflags, player, pSet);
}

void CreateBaseline(int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs)
{
	(*other_gFunctionTable.pfnCreateBaseline)(player, eindex, baseline, entity, playermodelindex, player_mins, player_maxs);
}

void RegisterEncoders(void)
{
	(*other_gFunctionTable.pfnRegisterEncoders)();
}

int GetWeaponData(struct edict_s *player, struct weapon_data_s *info)
{
	return (*other_gFunctionTable.pfnGetWeaponData)(player, info);
}

void CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed)
{
	(*other_gFunctionTable.pfnCmdStart)(player, cmd, random_seed);
}

void CmdEnd(const edict_t *player)
{
	(*other_gFunctionTable.pfnCmdEnd)(player);
}

int ConnectionlessPacket(const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size)
{
	return (*other_gFunctionTable.pfnConnectionlessPacket)(net_from, args, response_buffer, response_buffer_size);
}

int GetHullBounds(int hullnumber, float *mins, float *maxs)
{
	return (*other_gFunctionTable.pfnGetHullBounds)(hullnumber, mins, maxs);
}

void CreateInstancedBaselines(void)
{
	(*other_gFunctionTable.pfnCreateInstancedBaselines)();
}

int InconsistentFile(const edict_t *player, const char *filename, char *disconnect_message)
{
	return (*other_gFunctionTable.pfnInconsistentFile)(player, filename, disconnect_message);
}

int AllowLagCompensation(void)
{
	return (*other_gFunctionTable.pfnAllowLagCompensation)();
}

DLL_FUNCTIONS gFunctionTable =
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

	GetGameDescription,			//pfnGetGameDescription	 Returns string describing current .dll game.
	PlayerCustomization,		//pfnPlayerCustomization	Notifies .dll of new customization for player.

	SpectatorConnect,			//pfnSpectatorConnect		Called when spectator joins server
	SpectatorDisconnect,		//pfnSpectatorDisconnect	Called when spectator leaves the server
	SpectatorThink,				//pfnSpectatorThink		  Called when spectator sends a command packet (usercmd_t)

	Sys_Error,					//pfnSys_Error			 Called when engine has encountered an error

	PM_Move,					//pfnPM_Move
	PM_Init,					//pfnPM_Init				Server version of player movement initialization
	PM_FindTextureType,			//pfnPM_FindTextureType

	SetupVisibility,			//pfnSetupVisibility		  Set up PVS and PAS for networking for this client
	UpdateClientData,			//pfnUpdateClientData		 Set up data sent only to specific client
	AddToFullPack,				//pfnAddToFullPack
	CreateBaseline,				//pfnCreateBaseline		  Tweak entity baseline for network encoding, allows setup of player baselines, too.
	RegisterEncoders,			//pfnRegisterEncoders		Callbacks for network encoding
	GetWeaponData,				//pfnGetWeaponData
	CmdStart,					//pfnCmdStart
	CmdEnd,						//pfnCmdEnd
	ConnectionlessPacket,		//pfnConnectionlessPacket
	GetHullBounds,				//pfnGetHullBounds
	CreateInstancedBaselines,	//pfnCreateInstancedBaselines
	InconsistentFile,			//pfnInconsistentFile
	AllowLagCompensation,		//pfnAllowLagCompensation
};

//-----------------------------------------------------------------------------
// Purpose: Old version, used only if GetEntityAPI2 is not available
// Input  : *pFunctionTable - empty function table to be filled by game DLL
//			interfaceVersion - engine interface version
// Output : int 0 = failure
//-----------------------------------------------------------------------------
extern "C" EXPORT int GetEntityAPI(DLL_FUNCTIONS *pFunctionTable, int interfaceVersion)
{
#ifdef _DEBUG
	if (pFunctionTable == NULL)
	{
		printf("XBM: GetEntityAPI(): pFunctionTable == NULL!\n");
		return 0;
	}
#endif
	if (interfaceVersion != INTERFACE_VERSION)
	{
		printf("XBM: GetEntityAPI(): incompatible interface version %d! (local %d)\n", interfaceVersion, INTERFACE_VERSION);
		return 0;
	}

	// pass engine callback function table to engine...
	memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));

	if (other_GetEntityAPI == NULL)
	{
		printf("XBM: game DLL does not have GetEntityAPI!\n");
		return 0;
	}
	// pass other DLLs engine callbacks to function table...
//	if (!(*other_GetEntityAPI)(&other_gFunctionTable, INTERFACE_VERSION))
//		return 0;  // error initializing function table!!!

	return (*other_GetEntityAPI)(&other_gFunctionTable, INTERFACE_VERSION);
}

//-----------------------------------------------------------------------------
// Purpose: Called first in new engine versions, allows DLL to return version
// Input  : *pFunctionTable - empty function table to be filled by game DLL
//			*interfaceVersion - in: engine interface version for check,
//					out: must be set after that to game DLL inverface version
// Output : int 0 = failure
//-----------------------------------------------------------------------------
extern "C" EXPORT int GetEntityAPI2(DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
#ifdef _DEBUG
	if (pFunctionTable == NULL || interfaceVersion == NULL)
	{
		printf("XBM: GetEntityAPI2(): bad arguments!\n");
		return 0;
	}
#endif
	if (*interfaceVersion != INTERFACE_VERSION)
	{
		printf("XBM: GetEntityAPI2(): incompatible interface version %d! (local %d)\n", *interfaceVersion, INTERFACE_VERSION);
		// Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = INTERFACE_VERSION;
		return 0;// should we?
	}

	// pass engine callback function table to engine...
	memcpy(pFunctionTable, &gFunctionTable, sizeof(DLL_FUNCTIONS));// XDM3035 this was a stupid FAIL, because engine DOES NOT call GetEntityAPI if it finds GetEntityAPI2 first!

	if (other_GetEntityAPI2 == NULL)
	{
		printf("XBM: game DLL does not have GetEntityAPI2!\n");
		return 0;// should we?
	}

	// This part is tricky: we overwrite function pointer array, but the game dll will try to overwrite it again, so supply custom array instead
	return (*other_GetEntityAPI2)(&other_gFunctionTable, interfaceVersion);
}

/*
// These functions are optional, so check if they are available
void OnFreeEntPrivateData(edict_t *pEnt)
{
	if (other_gNewFunctionTable.pfnOnFreeEntPrivateData)
		(*other_gNewFunctionTable.pfnOnFreeEntPrivateData)(pEnt);
}

void GameDLLShutdown(void)
{
	if (other_gNewFunctionTable.pfnGameShutdown)
		(*other_gNewFunctionTable.pfnGameShutdown)();
}

int ShouldCollide(edict_t *pentTouched, edict_t *pentOther)
{
	if (other_gNewFunctionTable.pfnShouldCollide)
		return (*other_gNewFunctionTable.pfnShouldCollide)(pentTouched, pentOther);
}

#ifdef HL1120
void CvarValue(const edict_t *pEnt, const char *value)
{
	if (other_gNewFunctionTable.pfnCvarValue)
		(*other_gNewFunctionTable.pfnCvarValue)(pEnt, value);
}

void CvarValue2(const edict_t *pEnt, int requestID, const char *cvarName, const char *value)
{
	if (other_gNewFunctionTable.pfnCvarValue2)
		(*other_gNewFunctionTable.pfnCvarValue2)(pEnt, requestID, cvarName, value);
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
*/

//-----------------------------------------------------------------------------
// Purpose: This API is optional, so don't crash if game DLL does not support it
// Input  : *pFunctionTable - empty function table to be filled by game DLL
//			*interfaceVersion - in: engine interface version for check,
//					out: must be set after that to game DLL inverface version
// Output : int 0 = failure
//-----------------------------------------------------------------------------
extern "C" EXPORT int GetNewDLLFunctions(NEW_DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion)
{
#ifdef _DEBUG
	if (pFunctionTable == NULL || interfaceVersion == NULL)
	{
		printf("XBM: GetNewDLLFunctions(): bad arguments!\n");
		return 0;
	}
#endif
	if (*interfaceVersion != NEW_DLL_FUNCTIONS_VERSION)
	{
		printf("XBM: GetNewDLLFunctions(): incompatible interface version %d! (local %d)\n", *interfaceVersion, NEW_DLL_FUNCTIONS_VERSION);
		// Tell engine what version we had, so it can figure out who is out of date.
		*interfaceVersion = NEW_DLL_FUNCTIONS_VERSION;
		return 0;
	}

//	if (other_GetNewDLLFunctions == NULL)
//		other_GetNewDLLFunctions = (GETNEWDLLFUNCTIONS)GetProcAddress(h_Library, "GetNewDLLFunctions");
//	if (other_GetNewDLLFunctions == NULL)// Can't find GetNewDLLFunctions!
//		ALERT(at_warning, "XBM: Can't get GetNewDLLFunctions!\n" );
/*
	printf("XBM: GetNewDLLFunctions(): size = %d, version = %d\n", sizeof(gNewFunctionTable), *interfaceVersion);

	memcpy(pFunctionTable, &gNewFunctionTable, sizeof(gNewFunctionTable));
*/
	if (other_GetNewDLLFunctions == NULL)// child mod server DLL does not have GetNewDLLFunctions API
	{
		printf("XBM: game DLL does not require GetNewDLLFunctions.\n");
		return 0;
	}

	// This part is tricky: we overwrite function pointer array, but the game dll will try to overwrite it again, so supply custom array instead
	return (*other_GetNewDLLFunctions)(/*other_gNewFunctionTable*/pFunctionTable, interfaceVersion);
}


//-----------------------------------------------------------------------------
// Purpose: This API is optional, so don't crash if game DLL does not support it
// Input  : version - 
//			**ppinterface - 
//			*pstudio - 
//			(*rotationmatrix - 
// Output : int 0 = failure
//-----------------------------------------------------------------------------
int EXPORT STDCALL Server_GetBlendingInterface(int version, struct sv_blending_interface_s **ppinterface, struct engine_studio_api_s *pstudio, float (*rotationmatrix)[3][4], float (*bonetransform)[MAXSTUDIOBONES][3][4])
{
//	static SERVER_GETBLENDINGINTERFACE other_Server_GetBlendingInterface = NULL;
	static bool missing = false;

	// if the blending interface has been formerly reported as missing, give up
	if (missing)
		return 0;

	// do we NOT know if the blending interface is provided? if so, look for its address
//	if (other_Server_GetBlendingInterface == NULL)
//		other_Server_GetBlendingInterface = (SERVER_GETBLENDINGINTERFACE)GetProcAddress(h_Library, "Server_GetBlendingInterface");

	// have we NOT found it ?
	if (other_Server_GetBlendingInterface == NULL)
	{
//		ALERT(at_console, "XBM: Can't get GetBlendingInterface!\n" );
		printf("XBM: game DLL does not require GetBlendingInterface.\n");
		missing = true;// then mark it as missing, no use to look for it again in the future
		return 0;// and give up
	}

	// else call the function that provides the blending interface on request
	return (*other_Server_GetBlendingInterface)(version, ppinterface, pstudio, rotationmatrix, bonetransform);
}




// Acts as engine for the Mod dll.
void FakeClientCommand(edict_t *pBot, char *arg1, char *arg2, char *arg3)
{
	int length;
	memset(g_argv, 0, 1024);
	isFakeClientCommand = true;

	if ((arg1 == NULL) || (*arg1 == 0))
		return;

	if ((arg2 == NULL) || (*arg2 == 0))
	{
		length = sprintf(&g_argv[0], "%s", arg1);
		fake_arg_count = 1;
	}
	else if ((arg3 == NULL) || (*arg3 == 0))
	{
		length = sprintf(&g_argv[0], "%s %s", arg1, arg2);
		fake_arg_count = 2;
	}
	else
	{
		length = sprintf(&g_argv[0], "%s %s %s", arg1, arg2, arg3);
		fake_arg_count = 3;
	}

	g_argv[length] = 0;  // null terminate just in case
	strcpy(&g_argv[FAKE_CMD_ARG_LEN], arg1);

	if (arg2)
		strcpy(&g_argv[FAKE_CMD_ARG_LEN*2], arg2);

	if (arg3)
		strcpy(&g_argv[FAKE_CMD_ARG_LEN*3], arg3);

	// allow the MOD DLL to execute the ClientCommand...
	ClientCommand(pBot);
	isFakeClientCommand = false;
}

const char *Cmd_Args(void)
{
	if (isFakeClientCommand)
		return &g_argv[0];
	else
		return (*g_engfuncs.pfnCmd_Args)();
}

const char *Cmd_Argv(int argc)
{
	if (isFakeClientCommand)
	{
		if (argc == 0)
			return &g_argv[FAKE_CMD_ARG_LEN];
		else if (argc == 1)
			return &g_argv[FAKE_CMD_ARG_LEN*2];
		else if (argc == 2)
			return &g_argv[FAKE_CMD_ARG_LEN*3];
		else
			return NULL;
	}
	else
		return (*g_engfuncs.pfnCmd_Argv)(argc);
}

int Cmd_Argc(void)
{
	if (isFakeClientCommand)
		return fake_arg_count;
	else
		return (*g_engfuncs.pfnCmd_Argc)();
}

//-----------------------------------------------------------------------------
// Purpose: Special config processing function for maps/mapname_botmatch.cfg (hack?)
//-----------------------------------------------------------------------------
void ProcessBotCfgFile(void)
{
	int ch;
	char cmd_line[256];
	int cmd_index;
	static char server_cmd[80];
	char *cmd, *arg1, *arg2, *arg3, *arg4, *arg5, *arg6;
	char msg[80];

	if (bot_cfg_pause_time > gpGlobals->time)
		return;

	if (bot_cfg_fp == NULL)
		return;

	cmd_index = 0;
	cmd_line[cmd_index] = 0;
	ch = fgetc(bot_cfg_fp);
	// skip any leading blanks
	while (ch == ' ')
		ch = fgetc(bot_cfg_fp);
	
	while ((ch != EOF) && (ch != '\r') && (ch != '\n'))
	{
		if(ch == '\t')  // convert tabs to spaces
			ch = ' ';
		
		cmd_line[cmd_index] = ch;
		ch = fgetc(bot_cfg_fp);
		// skip multiple spaces in input file
		while ((cmd_line[cmd_index] == ' ') && (ch == ' '))
			ch = fgetc(bot_cfg_fp);

		cmd_index++;
	}

	if (ch == '\r')  // is it a carriage return?
		ch = fgetc(bot_cfg_fp);  // skip the linefeed

	// if reached end of file, then close it
	if (ch == EOF)
	{
		fclose(bot_cfg_fp);
		bot_cfg_fp = NULL;
		bot_cfg_pause_time = 0.0;
	}
	cmd_line[cmd_index] = 0;  // terminate the command line
	// copy the command line to a server command buffer...
	strcpy(server_cmd, cmd_line);
	strcat(server_cmd, "\n");
	cmd_index = 0;
	cmd = cmd_line;
	arg1 = arg2 = arg3 = arg4 = arg5 = arg6 = NULL;
	// skip to blank or end of string...
	while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
		cmd_index++;
	
	if (cmd_line[cmd_index] == ' ')
	{
		cmd_line[cmd_index++] = 0;
		arg1 = &cmd_line[cmd_index];
		
		// skip to blank or end of string...
		while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
			cmd_index++;
		
		if (cmd_line[cmd_index] == ' ')
		{
			cmd_line[cmd_index++] = 0;
			arg2 = &cmd_line[cmd_index];
			
			// skip to blank or end of string...
			while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
				cmd_index++;
			
			if (cmd_line[cmd_index] == ' ')
			{
				cmd_line[cmd_index++] = 0;
				arg3 = &cmd_line[cmd_index];
				
				// skip to blank or end of string...
				while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
					cmd_index++;
				
				if (cmd_line[cmd_index] == ' ')
				{
					cmd_line[cmd_index++] = 0;
					arg4 = &cmd_line[cmd_index];
					
					// skip to blank or end of string...
					while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
						cmd_index++;
					
					if (cmd_line[cmd_index] == ' ')
					{
						cmd_line[cmd_index++] = 0;
						arg5 = &cmd_line[cmd_index];
						
						// skip to blank or end of string...
						while ((cmd_line[cmd_index] != ' ') && (cmd_line[cmd_index] != 0))
							cmd_index++;
						
						if (cmd_line[cmd_index] == ' ')
						{
							cmd_line[cmd_index++] = 0;
							arg6 = &cmd_line[cmd_index];
						}
					}
				}
			}
		}
	}

	if (cmd_line[0] == '#' || cmd_line[0] == 0)
		return;  // return if comment or blank line

	if (cmd_line[0] == '/' && cmd_line[1] == '/')
		return;// XDM

	if (strcmp(cmd, "min_bots") == 0)
	{
		min_bots = atoi(arg1);
		if ((min_bots < 0) || (min_bots > (gpGlobals->maxClients-1)))
			min_bots = 1;

		if (IsDedicatedServer)
		{
			sprintf(msg, "min_bots set to %d\n", min_bots);
			printf(msg);
		}
	}
	else if (strcmp(cmd, "max_bots") == 0)
	{
		max_bots = atoi(arg1);
		if ((max_bots < 0) || (max_bots > gpGlobals->maxClients)) 
			max_bots = 1;

		if (IsDedicatedServer)
		{
			sprintf(msg, "max_bots set to %d\n", max_bots);
			printf(msg);
		}
	}
	else if (strcmp(cmd, "pause") == 0)
	{
		bot_cfg_pause_time = gpGlobals->time + atof(arg1);
	}
	else
	{
		if (debug_engine)
		{
			sprintf(msg, "XBM: CFG server command: %s\n", server_cmd);
			if (IsDedicatedServer)
				printf(msg);
			else
				ALERT(at_console, msg);
		}
		SERVER_COMMAND(server_cmd);
		SERVER_EXECUTE();
	}
}
