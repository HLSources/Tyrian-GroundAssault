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
#include "extdll.h"
#include <time.h>
#include "eiface.h"
#include "util.h"
#include "game.h"
#include "gamerules.h"
#include "skill.h"
#include "cbase.h"
#include "globals.h"
#include "environment.h"
#include "basemonster.h"
#include "entconfig.h"

DLL_GLOBAL const char g_szServerAutoFileName[] = "server_run.cfg";
DLL_GLOBAL FILE *g_pServerAutoFile = NULL;// XDM3035: holds current map information for restoring the dedicated server

#ifdef _DEBUG
cvar_t	test1		= {"test1",		"0",	FCVAR_UNLOGGED, 0.0f, NULL};
cvar_t	test2		= {"test2",		"0",	FCVAR_UNLOGGED, 0.0f, NULL};
cvar_t	test3		= {"test3",		"0",	FCVAR_UNLOGGED, 0.0f, NULL};
#endif

cvar_t	bot_allow				= {"bot_allow",					"1",				FCVAR_SERVER};
cvar_t	bot_random_powerup_lvl	= {"bot_random_powerup_lvl",	"0",		FCVAR_SERVER | FCVAR_EXTDLL};

cvar_t	showtriggers			= {"sv_showtriggers",		"0",			FCVAR_SERVER | FCVAR_EXTDLL | FCVAR_CHEAT};
cvar_t	displaysoundlist		= {"sv_displaysoundlist",	"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	sv_nodegraphdisable		= {"sv_nodegraphdisable",	"0",			FCVAR_SERVER | FCVAR_EXTDLL};

// multiplayer server rules
cvar_t	mp_scoreleft			= {"mp_scoreleft",			"0",			FCVAR_SERVER | FCVAR_EXTDLL | FCVAR_UNLOGGED};// Don't spam console/log files/users with this changing
cvar_t	mp_timeleft				= {"mp_timeleft",			"0",			FCVAR_SERVER | FCVAR_EXTDLL | FCVAR_UNLOGGED};
cvar_t	mp_gamerules			= {"mp_gamerules",			"0",			FCVAR_SERVER/* | FCVAR_SPONLY*/};
cvar_t	mp_weapon_category		= {"mp_weapon_category",	"0",			FCVAR_SERVER};
cvar_t	mp_maprules				= {"mp_maprules",			"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_teamplay				= {"mp_teamplay",			"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	sv_clientgibs			= {"sv_clientgibs",			"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_allowmusicevents		= {"mp_allowmusicevents",	"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t  mp_allowspectators		= {"mp_allowspectators",	"1",			FCVAR_SERVER | FCVAR_EXTDLL | FCVAR_SPONLY};
cvar_t	sv_lognotice			= {"sv_lognotice",			"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_scorelimit			= {"mp_scorelimit",			"100",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_capturelimit			= {"mp_capturelimit",		"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_fraglimit			= {"mp_fraglimit",			"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_timelimit			= {"mp_timelimit",			"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_friendlyfire			= {"mp_friendlyfire",		"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_forcerespawn			= {"mp_forcerespawn",		"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_forcerespawntime		= {"mp_forcerespawntime",	"5.0",			FCVAR_SERVER | FCVAR_EXTDLL,	5.0f};
cvar_t	mp_respawntime			= {"mp_respawntime",		"1.0",			FCVAR_SERVER | FCVAR_EXTDLL,	1.0f};
cvar_t	mp_teleport_allow		= {"mp_teleport_allow",		"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_specteammates		= {"mp_specteammates",		"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_spectoggle			= {"mp_spectoggle",			"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_teamlist				= {"mp_teamlist",			"Team1;Team2;Team3;Team4",	FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_teamcolor1			= {"mp_teamcolor1",			"0 255 0",		FCVAR_SERVER | FCVAR_EXTDLL};// XDM3035
cvar_t	mp_teamcolor2			= {"mp_teamcolor2",			"0 0 255",		FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_teamcolor3			= {"mp_teamcolor3",			"255 0 0",		FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_teamcolor4			= {"mp_teamcolor4",			"255 255 0",	FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_teambalance			= {"mp_teambalance",		"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_teamchange			= {"mp_teamchange",			"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_teamchangekill		= {"mp_teamchangekill",		"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_defaultteam			= {"mp_defaultteam",		"",				FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_monstersrespawn		= {"mp_monstersrespawn",	"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_monsrespawntime		= {"mp_monrespawntime",		"10",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_itm_resp_time		= {"mp_itm_resp_time",		"30",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_chattime				= {"mp_chattime",			"15",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_nofriction			= {"mp_nofriction",			"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_flagstay				= {"mp_flagstay",			"20",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_domscoreperiod		= {"mp_domscoreperiod",		"3",			FCVAR_SERVER | FCVAR_EXTDLL,	3.0f};
cvar_t	mp_allowcamera			= {"mp_allowcamera",		"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_laddershooting		= {"mp_laddershooting",		"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_teammenu				= {"mp_teammenu",			"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_telegib				= {"mp_telegib",			"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_spawnprotectiontime	= {"mp_spawnprotectiontime","0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_noshooting			= {"mp_noshooting",			"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_revengemode			= {"mp_revengemode",		"0",			FCVAR_SERVER | FCVAR_EXTDLL};// 0 = normal game, 1 = extra score, 2 = hunt mode (ability to kill)
cvar_t	mp_coop_eol_firstwin	= {"mp_coop_eol_firstwin",	"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_coop_eol_spectate	= {"mp_coop_eol_spectate",	"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_coop_usemaptransition= {"mp_coop_usemaptransition","0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_extra_nuke			= {"mp_extra_nuke",			"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	sv_clientstaticents		= {"sv_clientstaticents",	"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	sv_modelhitboxes		= {"sv_modelhitboxes",		"0",			FCVAR_SERVER | FCVAR_EXTDLL | FCVAR_CHEAT};
cvar_t	sv_generategamecfg		= {"sv_generategamecfg",	"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	sv_loadentfile			= {"sv_loadentfile",		"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	sv_reliability			= {"sv_reliability",		"0",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	sv_decalfrequency		= {"decalfrequency",		"30",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_wpnboxbrk			= {"mp_wpnboxbrk",			"1",			FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t	mp_bananamode			= {"mp_bananamode",			"0",			FCVAR_SERVER | FCVAR_EXTDLL};


// Engine Cvars
cvar_t	*g_pdeveloper = NULL;
cvar_t 	*g_psv_gravity = NULL;
cvar_t	*g_psv_aim = NULL;
cvar_t	*g_psv_cheats = NULL;
cvar_t 	*g_psv_maxspeed = NULL;
cvar_t	*g_psv_zmax = NULL;

// HACK? indicates that all following commands in config file are rejected in non-multiplayer mode
void Cmd_MultiplayerOnly(void)
{
	if (g_MapConfigCommands)// valid only in map.cfg
	{
		if (CMD_ARGC() > 1)// == 2) no arguments, just enable
			g_MultiplayerOnlyCommands = (atoi(CMD_ARGV(1)) > 0);
		else
			g_MultiplayerOnlyCommands = true;
	}
}

// HACK? indicates that all following commands in config file are rejected in other game rules mode
// Provides basic comparsion and bitwise operators
void Cmd_GamerulesOnly(void)
{
	if (g_MapConfigCommands)// valid only in map.cfg
	{
		if (CMD_ARGC() > 2)
		{
			if (CMD_ARGV(1))
				strncpy(g_iGamerulesSpecificCommandsOp, CMD_ARGV(1), 4);

			g_iGamerulesSpecificCommands = atoi(CMD_ARGV(2));
		}
		else if (CMD_ARGC() > 1)// GT_TEAMPLAY, etc
		{
			if (strcmp(CMD_ARGV(1), "end") == 0)
				g_iGamerulesSpecificCommands = -1;// to disable
		}
	}
}

void Cmd_GlobalFog(void)
{
	if (!ENTCONFIG_ValidateCommand())
		return;

	if (CMD_ARGC() == 6)
	{
		CEnvFog *pFog = NULL;// initialize first!
		pFog = (CEnvFog *)::UTIL_FindEntityByClassname(pFog, "env_fog");// search for existing fog
		if (pFog == NULL)
		{
			pFog = (CEnvFog *)CBaseEntity::Create("env_fog", g_vecZero, g_vecZero, NULL);
			SERVER_PRINT("Created new env_fog\n");
		}

		if (pFog)
		{
			pFog->pev->impulse = 1;
			pFog->pev->rendercolor.x = atoi(CMD_ARGV(1));
			pFog->pev->rendercolor.y = atoi(CMD_ARGV(2));
			pFog->pev->rendercolor.z = atoi(CMD_ARGV(3));
			pFog->m_iStartDist = atoi(CMD_ARGV(4));
			pFog->m_iEndDist = atoi(CMD_ARGV(5));
			SERVER_PRINT(UTIL_VarArgs("Existing fog set to: %f %f %f  %d %d\n", pFog->pev->rendercolor.x, pFog->pev->rendercolor.y, pFog->pev->rendercolor.z, pFog->m_iStartDist, pFog->m_iEndDist));

			if (g_pGameRules)// XDM3035a: update now
				pFog->SendClientData(NULL, MSG_ALL);
		}
	}
	else
		SERVER_PRINT(UTIL_VarArgs("Error creating fog: invalid parameters!\nusage: %s <r g b> <mindist> <maxdist>\n", CMD_ARGV(0)));
}


void Cmd_SetKV(void)// moved here to be used in map configs
{
	if (!ENTCONFIG_ValidateCommand())
		return;

	if (CMD_ARGC() > 2)// XDM3035b: allow empty argument
	{
		edict_t	*pEntity = NULL;
		while ((pEntity = FIND_ENTITY_BY_TARGETNAME(pEntity, CMD_ARGV(1))) != NULL)
		{
//wrong!			if (pEntity == NULL)
			if (FNullEnt(pEntity))
				break;
			KeyValueData kvd;
			kvd.szKeyName = (char *)CMD_ARGV(2);
			kvd.szValue = (char *)CMD_ARGV(3);
			kvd.szClassName = STRINGV(pEntity->v.classname);
			kvd.fHandled = FALSE;// IMPORTANT!!
			DispatchKeyValue(pEntity, &kvd);
			SERVER_PRINT(UTIL_VarArgs("%s: found %s\n", CMD_ARGV(0), STRING(pEntity->v.classname)));
		}
	}
	else
		SERVER_PRINT(UTIL_VarArgs("usage: %s <targetname> <key> <\"value\">\n", CMD_ARGV(0)));
}


EHANDLE g_WriteEntityToFile;// since console command functions cannot return anything, we need this global

void Cmd_SpawnEnt(void)
{
	if (!ENTCONFIG_ValidateCommand())
		return;

	g_WriteEntityToFile = NULL;// mark new attempt anyway

	if (g_pdeveloper->value <= 0.0f)
		return;

	if (CMD_ARGC() > 1)
	{
		Vector origin;
		if (StringToVec(CMD_ARGV(2), origin))
		{
			Vector angles;
			if (!StringToVec(CMD_ARGV(3), angles))
				angles = g_vecZero;

			CBaseEntity *pEntity = CBaseEntity::Create((char *)CMD_ARGV(1), origin, angles, NULL);
			if (pEntity)
			{
				pEntity->pev->classname = ALLOC_STRING(CMD_ARGV(1));// very important thing!
				if (CMD_ARGC() > 4)
					pEntity->pev->targetname = ALLOC_STRING(CMD_ARGV(4));

				g_WriteEntityToFile = pEntity;
				ALERT(at_aiconsole, "Added %s\n", CMD_ARGV(1));
			}
			else
				ALERT(at_aiconsole, "Failed to create %s!\n", CMD_ARGV(1));
		}
		else
			ALERT(at_console, "Unable to create %s without origin!\n", CMD_ARGV(1));
	}
	else
		ALERT(at_console, "usage: %s <classname> <\"x y z\" origin> [\"x y z\" angles] [targetname]\n", CMD_ARGV(0));
}

void Cmd_ListEnts(void)
{
	if (g_pdeveloper->value <= 0.0f)
		return;

	if (CMD_ARGC() > 1)
	{
		CBaseEntity *pEntity = NULL;
		while ((pEntity = UTIL_FindEntityByTargetname(pEntity, CMD_ARGV(1))) != NULL)
		{
			ALERT(at_console, " - %s at (%f %f %f)\n", STRING(pEntity->pev->classname), pEntity->pev->origin.x, pEntity->pev->origin.y, pEntity->pev->origin.z);
			if (CMD_ARGC() > 2 && atoi(CMD_ARGV(2)) > 0)
			{
				ALERT(at_console, "removed\n");
				UTIL_Remove(pEntity);
			}
		}
	}
	else
		ALERT(at_console, "usage: %s <targetname> [1/0 - remove]\n", CMD_ARGV(0));
}

void Cmd_ClientCmd(void)
{
	if (CMD_ARGC() > 2)
	{
		edict_t	*pEdict = UTIL_ClientEdictByIndex(atoi(CMD_ARGV(1)));
		if (pEdict)
		{
			SERVER_PRINT("Executing client command\n");
			CLIENT_COMMAND(pEdict, (char *)CMD_ARGV(2));
		}
		else
			SERVER_PRINT("Client not found.\n");
	}
	else
		ALERT(at_console, "usage: %s <index> <\"command\">\n", CMD_ARGV(0));
}

void Cmd_BotCreate(void)
{
	if (bot_allow.value > 0.0)
	{
		if (CMD_ARGC() > 1)
		{
//			BotCreate(CMD_ARGV(1));
		}
		else
			ALERT(at_console, "usage: %s <name>\n", CMD_ARGV(0));
	}
	else
		SERVER_PRINT("Botmatch is not allowed.\n");
}

void Cmd_EndGame(void)
{
	if (g_pGameRules)
	{
		SERVER_PRINT("End multiplayer game\n");
		if (g_pGameRules->IsGameOver())// XDM3037: someone wants to forcibly skip intermission
			g_pGameRules->ChangeLevel();
		else
			g_pGameRules->EndMultiplayerGame();
	}
}

void Cmd_ServerDir(void)
{
	UTIL_ListFiles(CMD_ARGV(1));
}

/* BAD BAD BAD! VERY VERY BAD!!
void Cmd_ServerInitialize(void)
{
	if (g_pGameRules)
	{
		SERVER_PRINT("GameRules force Initialize()\n");
		g_pGameRules->Initialize();
	}
}*/

/*
void Cmd_NextMap(void)
{
	if (g_pGameRules)
	{
		SERVER_PRINT("Instant changelevel\n");
		g_pGameRules->ChangeLevel(); - move to public and stuff...
	}
}
*/
// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit(void)
{
	SERVER_PRINT(UTIL_VarArgs("Initializing X-Half-Life server DLL (build %s)\nRegistering variables...\n", __DATE__));
	// Register cvars here:
	g_pdeveloper = CVAR_GET_POINTER("developer");
	g_psv_gravity = CVAR_GET_POINTER("sv_gravity");
	g_psv_aim = CVAR_GET_POINTER("sv_aim");
	g_psv_cheats = CVAR_GET_POINTER("sv_cheats");
	g_psv_maxspeed = CVAR_GET_POINTER("sv_maxspeed");
	g_psv_zmax = CVAR_GET_POINTER("sv_zmax");

	cvar_t *pHostGameLoaded = CVAR_GET_POINTER("host_gameloaded");// Xash detection
	if (pHostGameLoaded)
		sv_loadentfile.value = 0.0f;// Xash has its own internal patching mechanism

#ifdef _DEBUG
	SERVER_PRINT("XDM: DEBUG MODE\n");
	CVAR_REGISTER(&test1);
	CVAR_REGISTER(&test2);
	CVAR_REGISTER(&test3);
#endif

	CVAR_REGISTER(&bot_allow);
	CVAR_REGISTER(&bot_random_powerup_lvl);
	CVAR_REGISTER(&showtriggers);
	CVAR_REGISTER(&displaysoundlist);
	CVAR_REGISTER(&sv_nodegraphdisable);
	CVAR_REGISTER(&mp_bananamode);

	CVAR_REGISTER(&mp_wpnboxbrk);
	CVAR_REGISTER(&mp_scoreleft);
	CVAR_REGISTER(&mp_timeleft);
	CVAR_REGISTER(&mp_gamerules);
	CVAR_REGISTER(&mp_weapon_category);
	CVAR_REGISTER(&mp_maprules);
	CVAR_REGISTER(&mp_teamplay);
	CVAR_REGISTER(&sv_clientgibs);
	CVAR_REGISTER(&mp_allowmusicevents);
	CVAR_REGISTER(&mp_allowspectators);
	CVAR_REGISTER(&sv_lognotice);
	CVAR_REGISTER(&mp_scorelimit);
	CVAR_REGISTER(&mp_capturelimit);
	CVAR_REGISTER(&mp_fraglimit);
	CVAR_REGISTER(&mp_timelimit);
	CVAR_REGISTER(&mp_friendlyfire);
	CVAR_REGISTER(&mp_forcerespawn);
	CVAR_REGISTER(&mp_forcerespawntime);
	CVAR_REGISTER(&mp_teleport_allow);
	CVAR_REGISTER(&mp_respawntime);
	CVAR_REGISTER(&mp_specteammates);
	CVAR_REGISTER(&mp_spectoggle);
	CVAR_REGISTER(&mp_teamlist);
	CVAR_REGISTER(&mp_teamcolor1);
	CVAR_REGISTER(&mp_teamcolor2);
	CVAR_REGISTER(&mp_teamcolor3);
	CVAR_REGISTER(&mp_teamcolor4);
	CVAR_REGISTER(&mp_teambalance);
	CVAR_REGISTER(&mp_teamchange);
	CVAR_REGISTER(&mp_teamchangekill);
	CVAR_REGISTER(&mp_defaultteam);
	CVAR_REGISTER(&mp_monstersrespawn);
	CVAR_REGISTER(&mp_monsrespawntime);
	CVAR_REGISTER(&mp_itm_resp_time);
	CVAR_REGISTER(&mp_chattime);
	CVAR_REGISTER(&mp_nofriction);
	CVAR_REGISTER(&mp_flagstay);
	CVAR_REGISTER(&mp_domscoreperiod);
	CVAR_REGISTER(&mp_allowcamera);
	CVAR_REGISTER(&mp_laddershooting);
	CVAR_REGISTER(&mp_teammenu);
	CVAR_REGISTER(&mp_telegib);
	CVAR_REGISTER(&mp_spawnprotectiontime);
	CVAR_REGISTER(&mp_noshooting);
	CVAR_REGISTER(&mp_revengemode);
	CVAR_REGISTER(&mp_coop_eol_firstwin);
	CVAR_REGISTER(&mp_coop_eol_spectate);
	CVAR_REGISTER(&mp_coop_usemaptransition);
	CVAR_REGISTER(&mp_extra_nuke);
	CVAR_REGISTER(&sv_clientstaticents);
	CVAR_REGISTER(&sv_modelhitboxes);
	if (g_iProtocolVersion <= 46)// Old HL does this!
	{
		sv_generategamecfg.string = "0";
		sv_generategamecfg.value = 0.0f;
	}
	CVAR_REGISTER(&sv_generategamecfg);
	CVAR_REGISTER(&sv_loadentfile);
	CVAR_REGISTER(&sv_reliability);
	CVAR_REGISTER(&sv_decalfrequency);

	SkillRegisterCvars();

	ADD_SERVER_COMMAND("multiplayer_only", Cmd_MultiplayerOnly);
	ADD_SERVER_COMMAND("gamerules_only", Cmd_GamerulesOnly);
	ADD_SERVER_COMMAND("map_globalfog", Cmd_GlobalFog);
	ADD_SERVER_COMMAND("setkvbytn", Cmd_SetKV);
	ADD_SERVER_COMMAND("listents", Cmd_ListEnts);
	ADD_SERVER_COMMAND("spawn_ent", Cmd_SpawnEnt);
	ADD_SERVER_COMMAND("clcmd", Cmd_ClientCmd);
	ADD_SERVER_COMMAND("endgame", Cmd_EndGame);
	ADD_SERVER_COMMAND("sv_dir", Cmd_ServerDir);
//	ADD_SERVER_COMMAND("sv_init", Cmd_ServerInitialize);
//	ADD_SERVER_COMMAND("nextmap", Cmd_NextMap);
//	ADD_SERVER_COMMAND("bot_create", Cmd_BotCreate);


	SERVER_COMMAND("exec skill.cfg\n");
/*
	g_pServerAutoFile = LoadFile(g_szServerAutoFileName, "rwt");
	if (g_pServerAutoFile == NULL)
	{
		SERVER_PRINT("Error creating/opening server autofile!\n");
	}*/

	SERVER_PRINT("Done. Server DLL initialized.\n");

	// XDM3035: causes infinite server reloads. This file must be used externally (from init.d script)
/*	if (IS_DEDICATED_SERVER() && STRING(gpGlobals->mapname) == "")
	{
		SERVER_PRINT("Executing server recovery config file\n");
		char szCommand[256];
		sprintf(szCommand, "exec %s\n", g_szServerAutoFileName);
		SERVER_COMMAND(szCommand);
	}*/
}

void GameDLLShutdown(void)// XDM
{
	SERVER_PRINT("XDM: Server shutting down\n");
	if (g_pGameRules)
	{
		delete g_pGameRules;
		g_pGameRules = NULL;
	}
/*
	if (g_pServerAutoFile)
	{
		fclose(g_pServerAutoFile);
		g_pServerAutoFile = NULL;
	}*/
	// XDM3035 crash recovery
	g_pServerAutoFile = LoadFile(g_szServerAutoFileName, "w");// Opens an empty file for writing. If the given file exists, its contents are destroyed.
	if (g_pServerAutoFile)
	{
/*		fseek(g_pServerAutoFile, 0L, SEEK_SET);

		time_t ltime;
		// Get UNIX-style time
		time(&ltime);
		fprintf(g_pServerAutoFile, "// SERVER WAS SHUT DOWN PROPERLY AT %s\n", ctime(&ltime));
		fflush(g_pServerAutoFile);// IMPORTANT: write to disk
		*/
		fclose(g_pServerAutoFile);
		g_pServerAutoFile = NULL;
		remove(g_szServerAutoFileName);// XDM3035 201101
		SERVER_PRINT("XDM: Server recovery config erased.\n");
	}
}
