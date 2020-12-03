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
// Robin, 4-22-98: Moved set_suicide_frame() here from player.cpp to allow us to
//				   have one without a hardcoded player.mdl in tf_client.cpp

/*

===== client.cpp ========================================================

  client/server game specific stuff

*/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "spectator.h"
#include "client.h"
#include "soundent.h"
#include "maprules.h"
#include "gamerules.h"
#include "game.h"
#include "custom.h"
#include "customentity.h"
#include "weapons.h"
#include "globals.h"
#include "movewith.h"
#include "items.h"
#include "entconfig.h"
#include "pm_shared.h"
#include "pm_materials.h"
#include "usercmd.h"
#include "voice_gamemgr.h"

extern CVoiceGameMgr g_VoiceGameMgr;

DLL_GLOBAL bool g_ServerActive = false;
unsigned char g_ClientShouldInitizlize[MAX_CLIENTS+1];// XDM3035a: HACK to re-init after level change

void LinkUserMessages(void);

/*
===========
ClientConnect

called when a player connects to a server
============
*/
BOOL ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{
/*	try
	{*/
//	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);// XDM: wa cannot use this because players do not have private data yet
	if (g_pGameRules->ClientConnected(pEntity, pszName, pszAddress, szRejectReason))
	{
		g_ClientShouldInitizlize[ENTINDEX(pEntity)] = 1;// XDM3035c
		return TRUE;
	}
	return FALSE;
// a client connecting during an intermission can cause problems
//	if (intermission_running)
//		ExitIntermission ();
/*	}
	catch (...)
	{
		ALERT(at_console, "ERROR: ClientConnect() exception!\n");
		DBG_FORCEBREAK
	}*/
}

/*
===========
ClientDisconnect

 called when a player disconnects from a server
 GLOBALS ASSUMED SET:  g_fGameOver
============
*/
void ClientDisconnect(edict_t *pEntity)
{
/*	try
	{*/
//	if (g_pGameRules->IsGameOver())
//		return;

	CSound *pSound = CSoundEnt::SoundPointerForIndex(CSoundEnt::ClientSoundIndex(pEntity));
	{
		if (pSound != NULL)// since this client isn't around to think anymore, reset their sound.
			pSound->Reset();
	}

// since the edict doesn't get deleted, fix it so it doesn't interfere.
	pEntity->v.takedamage = DAMAGE_NO;// don't attract autoaim
	pEntity->v.solid = SOLID_NOT;// nonsolid
	pEntity->v.effects = EF_NOINTERP | EF_NODRAW;// XDM3035
	pEntity->v.euser2 = NULL;// XDM3037: this edict is no longer watching through any entity
// XDM3035b: testme	UTIL_SetOrigin(&pEntity->v, pEntity->v.origin);

	CBasePlayer *pPlayer = (CBasePlayer*)CBaseEntity::Instance(pEntity);// XDM
	if (pPlayer)
	{
		pPlayer->m_iSpawnState = -1;
		g_pGameRules->ClientDisconnected(pPlayer);
	}

	pEntity->v.netname = iStringNull;// XDM3037: so IsActivePlayer() will fail
/*	}
	catch (...)
	{
		ALERT(at_console, "ERROR: ClientDisconnect() exception!\n");
		DBG_FORCEBREAK
	}*/
}


/*
============
ClientKill

Player entered the suicide command
GLOBALS ASSUMED SET: g_ulModelIndexPlayer
============
*/
void ClientKill( edict_t *pEntity )
{
	CBasePlayer *pPlayer = (CBasePlayer*)CBasePlayer::Instance(pEntity);

	if (pPlayer->m_fNextSuicideTime > gpGlobals->time)
		return;// prevent suiciding too ofter

	pPlayer->m_fNextSuicideTime = gpGlobals->time + 5.0f; // don't let them suicide for 5 seconds after suiciding
	// have the player kill themself
	pPlayer->pev->health = 0.0f;
//	int lk = pPlayer->m_iLastKiller;
	pPlayer->Killed(pPlayer, pPlayer, GIB_ALWAYS);// XDM3035b just for fun :)
//	if (mp_revengemode.value > 1.0f)// XDM3035c: not needed here
//		pPlayer->m_iLastKiller = lk;
}

/*
===========
ClientPutInServer

called each time a player is spawned
============
*/
void ClientPutInServer( edict_t *pEntity )
{
/*	try
	{*/
	CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)VARS(pEntity));// XDM
	pPlayer->SetCustomDecalFrames(-1); // Assume none;
	pPlayer->m_iSpawnState = 0;// XDM

// bad idea
//	if (g_pGameRules && g_pGameRules->IsMultiplayer())
//		pPlayer->pev->flags |= FL_SPECTATOR;// XDM3035c: player is connecting for the first time with totally clean entvars, allow game rules to detect it

	// Allocate a CBasePlayer for pev, and call spawn
	pPlayer->Spawn(FALSE);// XDM3035
	// Reset interpolation during first frame
	pPlayer->pev->effects |= EF_NOINTERP;
/*	}
	catch (...)
	{
		ALERT(at_console, "ERROR: ClientPutInServer() exception!\n");
		DBG_FORCEBREAK
	}*/
}

//// HOST_SAY
// String comes in as
// say blah blah blah
// or as
// blah blah blah
// XDM3035: now that's a huge part of a bullshit
void Host_Say(edict_t *pEntity, const char *pText, bool teamonly)
{
	if (pEntity == NULL || pText == NULL)
		return;

	int textlen = strlen(pText);
	if (textlen < 1)
		return;

	CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)&pEntity->v);
	if (pPlayer->m_flNextChatTime > gpGlobals->time)
		return;

	const char *p = pText;// NEW

	// remove quotes if present
	if (*p == '"')
	{
		p++;
//		p[strlen(p)-1] = 0;
		if (pText[textlen-1] == '"')// closing quote
			--textlen;

		--textlen;
	}

	if (textlen < 1)// re-check after removing quotes, very useful
		return;

	// make sure the text has content
	const char *pc = NULL;
	for (pc = p; pc != NULL && *pc != 0; ++pc)
	{
		if (isprint(*pc) && !isspace(*pc))
		{
			pc = NULL;	// we've found an alphanumeric character,  so text is valid
			break;
		}
	}

	if (pc != NULL)
		return;  // no character found, so say nothing

	char text[128];
	text[0] = 2;// flag for client saytext parser
	text[1] = 0;// nullterm
	strncat(text, p, textlen);

	pPlayer->m_flNextChatTime = gpGlobals->time + CHAT_INTERVAL;

	// ignore this rule if not in team game
	if (!g_pGameRules->IsTeamplay())
		teamonly = false;

	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop
	CBasePlayer *client = NULL;
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		client = UTIL_ClientByIndex(i);

		if (client == NULL)
			continue;

		if (!(client->IsNetClient()))// Not a client ? (should never be true)
			continue;

		if (!g_pGameRules->IsGameOver())// XDM3035c: everyone talks during intermission
		{
			if (pPlayer->IsObserver() && !client->IsObserver())
				continue;
		}
		// can the receiver hear the sender? or has he muted him?
		if (g_VoiceGameMgr.PlayerHasBlockedPlayer(client, pPlayer))
			continue;

		if (teamonly && /*g_pGameRules->IsTeamplay() && */g_pGameRules->PlayerRelationship(client, CBaseEntity::Instance(pEntity)) != GR_TEAMMATE)
			continue;

		MESSAGE_BEGIN(MSG_ONE, gmsgSayText, NULL, client->edict());
			WRITE_BYTE(ENTINDEX(pEntity) | (teamonly?128:0));// XDM3035: last bit means team only. Enough for current MAX_PLAYERS
			WRITE_STRING(text);
		MESSAGE_END();
	}
	if (sv_lognotice.value > 0)// XDM
	{
		UTIL_LogPrintf( "\"%s (%d)<%d><%d>\" \"%s\" (teamonly:%d)\n",
			STRING(pEntity->v.netname),
			pEntity->v.team,
			GETPLAYERUSERID(pEntity),
			GETPLAYERAUTHID(pEntity),
			p,teamonly);
	}
}


/*
===========
ClientCommand

called each time a player uses a "cmd" command
============
*/
//extern float g_flWeaponCheat;
// Use CMD_ARGV,  CMD_ARGV, and CMD_ARGC to get pointers the character string command.
void ClientCommand(edict_t *pEntity)
{
	// Is the client spawned yet?
	if (pEntity->pvPrivateData == NULL)
		return;

//	if (!ENTCONFIG_ValidateCommand())
//		return;

//	ALERT(at_console, "ClientCommand(%s %s)\n", pcmd, CMD_ARGS());
	const char *pcmd = CMD_ARGV(0);
	const char *arg1 = CMD_ARGV(1);
//	const char *arg2 = CMD_ARGV(2);
//	const char *arg3 = CMD_ARGV(3);
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = GetClassPtr((CBasePlayer *)pev);

	//frozen
	if (pPlayer->m_fFrozen)
		return;

	if (FStrnEq(pcmd, "_sw", 7))// XDM3035
	{
		pPlayer->SelectItem(atoi(arg1));
	}
	else if (FStrEq(pcmd, "lastinv"))
	{
		pPlayer->SelectLastItem();
	}
	else if (FStrnEq(pcmd, "weapon_", 7))// XDM3035: nobody should use this now
	{
		pPlayer->SelectItem(pcmd);
	}
	else if (FStrEq(pcmd, "drop"))
	{
		if (g_pGameRules->GetGameType() == GT_CTF && pPlayer->m_pCarryingObject && CMD_ARGC() <= 1)
			pPlayer->m_pCarryingObject->Use(pPlayer, pPlayer, USE_TOGGLE, 0.0f);
	}
	else if (FStrEq(pcmd, ".u") && CMD_ARGC() > 2)// Use entity by mouse click. TODO: integrate with CBasePlayer::PlayerUse()
	{
//		if (CMD_ARGC() > 1)
		{
			CBaseEntity *pObject = UTIL_EntityByIndex(atoi(CMD_ARGV(1)));
			if (pObject)// && pPlayer->FVisible(pObject->Center()))// prevent cheating??
			{
				if (pObject->ObjectCaps() & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE))
				{
				// TODO: check visibility here
				if ((pPlayer->EyePosition() - pObject->Center()).Length() <= PLAYER_USE_SEARCH_RADIUS)// prevent remote usage
				{
					int state = atoi(CMD_ARGV(2));
					float value = atof(CMD_ARGV(3));
					int caps = pObject->ObjectCaps();
					if ((state && (caps & FCAP_CONTINUOUS_USE)) || (state && (caps & (FCAP_IMPULSE_USE|FCAP_ONOFF_USE))))
					{
	//					if (caps & FCAP_CONTINUOUS_USE)
	//						pPlayer->m_afPhysicsFlags |= PFLAG_USING;

//						ClientPrint(pev, HUD_PRINTTALK, UTIL_VarArgs("Found %s, using (%g)\n", STRING(pObject->pev->classname), value));
						pObject->Use(pPlayer, pPlayer, USE_SET, value);
					}
					else if ((state == 0) && (caps & FCAP_ONOFF_USE))// BUGBUG This is an "off" use
					{
//						ClientPrint(pev, HUD_PRINTTALK, UTIL_VarArgs("Found %s, using (%g)\n", STRING(pObject->pev->classname), value));
						pObject->Use(pPlayer, pPlayer, USE_SET, value);
					}
				}
				}
			}
		}
	}
	else if (FStrEq(pcmd, "say"))
	{
		Host_Say(pEntity, CMD_ARGS(), false);
	}
	else if (FStrEq(pcmd, "say_team"))
	{
		Host_Say(pEntity, CMD_ARGS(), true);
	}
	else if (FStrEq(pcmd, "say_forward"))
	{
		if (pPlayer->IsObserver())
		{
			ClientPrint(pev, HUD_PRINTCONSOLE, "#MSG_NO_SPECTATORS");
			return;
		}
		CBaseEntity *cl = UTIL_FindEntityForward(pPlayer);
		if (arg1 == NULL || cl  == NULL || !cl->IsPlayer())
		{
			ClientPrint(pev, HUD_PRINTCONSOLE, "#MSG_NO_CLIENT");
			return;
		}
		MESSAGE_BEGIN(MSG_ONE, gmsgSayText, NULL, cl->edict());
			WRITE_BYTE(ENTINDEX(pEntity));
			WRITE_STRING(CMD_ARGS());
		MESSAGE_END();
		ClientPrint(pev, HUD_PRINTTALK, "#MSG_SENT_TO", STRING(cl->pev->netname));
	}
	else if (FStrEq(pcmd, "say_private"))
	{
		int i = atoi(arg1);
		CBasePlayer *cl = UTIL_ClientByIndex(i);
		if (cl)
		{
			if (pPlayer->IsObserver() && !cl->IsObserver())
			{
				ClientPrint(pev, HUD_PRINTTALK, "#MSG_NO_SPECTATORS");
			}
			else
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgSayText, NULL, cl->edict());
					WRITE_BYTE(ENTINDEX(pEntity));
					WRITE_STRING(CMD_ARGS());
				MESSAGE_END();
				ClientPrint(pev, HUD_PRINTTALK, "#MSG_SENT_TO", STRING(cl->pev->netname));
			}
		}
		else
			ClientPrint(pev, HUD_PRINTCONSOLE, "#MSG_NO_CLIENT");
	}
	else if (FStrEq(pcmd, "fullupdate"))
	{
		pPlayer->ForceClientDllUpdate();
	}
	else if (FStrEq(pcmd, "speaksound"))
	{
		if (arg1)
		{
		MESSAGE_BEGIN(MSG_BROADCAST, gmsgSpeakSnd, NULL);
			WRITE_BYTE(255);// vol
			WRITE_BYTE(PITCH_NORM);// pitch
			WRITE_STRING(arg1);
		MESSAGE_END();
		}
	}
	else if (g_pGameRules->ClientCommand(pPlayer, pcmd))
	{
	}
/*	else if (FStrEq(pcmd, "holster"))
	{
		if (pPlayer->m_pActiveItem)
		{
			if (pPlayer->m_pActiveItem->CanHolster())
			{
				pPlayer->m_pActiveItem->Holster();
				pPlayer->m_pActiveItem->pev->nextthink = 0;
				pPlayer->m_flNextAttack = 0;
			}
			else if (pPlayer->m_pActiveItem->CanDeploy())
			{
				pPlayer->m_pActiveItem->Deploy();
				pPlayer->m_pActiveItem->pev->nextthink = gpGlobals->time;
				pPlayer->m_flNextAttack = gpGlobals->time + 1.0;
			}
		}
	}*/
	else if (FStrEq(pcmd, "use"))// Who uses this?
	{
		pPlayer->SelectItem((char *)CMD_ARGV(1));
	}
	else if (FStrEq(pcmd, "profile_save"))
	{
		if (!arg1)
			ALERT(at_console, "Usage: %s <profile name>\n", pcmd);
		else
		{
			char cfgfile[MAX_PATH];
			sprintf(cfgfile, "%s.cfg", arg1);
			CONFIG_GenerateFromList("profile.lst", cfgfile);
		}
	}
	else
	{
		char command[128];
		strncpy(command, pcmd, 127);
		command[127] = '\0';
		ClientPrint(&pEntity->v, HUD_PRINTCONSOLE, "#CMD_UNKNOWN\n", command);//UTIL_VarArgs("Unknown command: %s\n", command));
	}
}

/*
========================
ClientUserInfoChanged

called after the player changes
userinfo - gives dll a chance to modify it before
it gets sent into the rest of the engine.
========================
*/
void ClientUserInfoChanged( edict_t *pEntity, char *infobuffer )
{
	// Is the client spawned yet?
	if ( !pEntity->pvPrivateData )
		return;

	char *pName = GET_INFO_KEY_VALUE(infobuffer, "name");
	// msg everyone if someone changes their name,  and it isn't the first time (changing no name to current name)
	if ( pEntity->v.netname && STRING(pEntity->v.netname)[0] != 0 && !FStrEq(STRING(pEntity->v.netname), pName))
	{
		char sName[256];
		strncpy( sName, pName, sizeof(sName) - 1 );
		sName[ sizeof(sName) - 1 ] = '\0';

		// First parse the name and remove any %'s
		for ( char *pApersand = sName; pApersand != NULL && *pApersand != 0; pApersand++ )
		{
			// Replace it with a space
			if (*pApersand == '%')
				*pApersand = ' ';
		}

		// Set the name
		g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity), infobuffer, "name", sName );

		UTIL_ClientPrintAll(HUD_PRINTTALK, "+ #CL_CHNAME\n", STRING(pEntity->v.netname), sName);
		if (sv_lognotice.value > 0)// XDM
		{
			// team match?
			UTIL_LogPrintf( "\"%s<%i><%s><%s>\" changed name to \"%s\"\n",
				STRING( pEntity->v.netname ),
				GETPLAYERUSERID( pEntity ),
				GETPLAYERAUTHID( pEntity ),
				GET_INFO_KEY_VALUE(infobuffer, "team"), // XDM
				GET_INFO_KEY_VALUE(infobuffer, "name"));
		}
		if (sv_modelhitboxes.value > 0.0f)// XDM3035: use custom hitboxes if not afraid of cheaters
		{
			char *pModelName = GET_INFO_KEY_VALUE(infobuffer, "model");
			if (pModelName)
				SET_MODEL(pEntity, pModelName);
		}
	}

	g_pGameRules->ClientUserInfoChanged( GetClassPtr((CBasePlayer *)&pEntity->v), infobuffer );
}

void ServerDeactivate(void)
{
	// It's possible that the engine will call this function more times than is necessary
	//  Therefore, only run it one time for each call to ServerActivate
	if (!g_ServerActive)
		return;

	g_ServerActive = false;
	// Peform any shutdown operations here...
	SERVER_PRINT("XDM: ServerDeactivate()\n");
}

void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
#ifdef _DEBUG
	SERVER_PRINT(UTIL_VarArgs("ServerActivate(%d, %d)\n", edictCount, clientMax));
#endif
//	try
//	{
	int				i;
	CBaseEntity		*pClass;
	// Every call to ServerActivate should be matched by a call to ServerDeactivate

	if (g_ServerActive == false)
	{
		g_ServerActive = true;
		// Link user messages here to make sure first client can get them...
		LinkUserMessages();

		// Clients have not been initialized yet
		for (i = 0; i < edictCount; ++i)
		{
			if ( pEdictList[i].free )
				continue;

			// Clients aren't necessarily initialized until ClientPutInServer()
			if (i <= clientMax)
			{
				g_ClientShouldInitizlize[i] = 1;// XDM3035a
				continue;
			}
			if (pEdictList[i].pvPrivateData == NULL)
				continue;

			pClass = CBaseEntity::Instance( &pEdictList[i] );
			// Activate this entity if it's got a class & isn't dormant
			if (pClass && !(pClass->pev->flags & FL_DORMANT))
				pClass->Activate();
			else
				ALERT( at_console, "Can't instance %s\n", STRING(pEdictList[i].v.classname) );
		}

		ENTCONFIG_ExecMapConfig();// XDM3035b: execute map cfg file AFTER game rules have been installed AND ALL entitieas have been spawned!
		SpawnPointInitialize();
		g_pGameRules->ServerActivate(pEdictList, edictCount, clientMax);
	}

	// XDM3035
/* NEVER!!!!!! CAUSES ENDLESS RELOADING LOOP!
	if (IS_DEDICATED_SERVER())// && STRING(gpGlobals->mapname) == "")
	{
		char szCommand[256];
		SERVER_PRINT("Executing server recovery config file\n");
		sprintf(szCommand, "exec %s\n", g_szServerAutoFileName);
		SERVER_COMMAND(szCommand);
	}*/
/*	}
	catch (...)
	{
		ALERT(at_console, "ERROR: ServerActivate() exception!\n");
		DBG_FORCEBREAK
	}*/
}

// a cached version of gpGlobals->frametime. The engine sets frametime to 0 if the player is frozen... so we just cache it in prethink,
// allowing it to be restored later and used by CheckDesiredList.
float g_flCachedFrametime = 0.0f;

/*
================
PlayerPreThink

Called every frame before physics are run
================
*/
void PlayerPreThink( edict_t *pEntity )
{
/*	try
	{*/
//	ALERT(at_console, "PlayerPreThink()\n");
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);
	if (pPlayer)
		pPlayer->PreThink();

/*	}
	catch (...)
	{
		SERVER_PRINT("XDM: PlayerPreThink() exception!\n");
		DBG_FORCEBREAK
	}*/
	g_flCachedFrametime = gpGlobals->frametime;
}

/*
================
PlayerPostThink

Called every frame after physics are run
================
*/
void PlayerPostThink( edict_t *pEntity )
{
//	try
//	{
/*		static lasttime;
		ALERT(at_console, "PlayerPostThink delta %g\n", gpGlobals->time - lasttime);
		lasttime = gpGlobals->time;
*/
//	ALERT(at_console, "PlayerPostThink()\n");
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);
	if (pPlayer)
		pPlayer->PostThink();
/*	}
	catch (...)
	{
		SERVER_PRINT("XDM: PlayerPreThink() exception!\n");
		DBG_FORCEBREAK
	}*/

#if defined(MOVEWITH)
	gpGlobals->frametime = g_flCachedFrametime;// restore valid frame time for SHL stuff
	CheckDesiredList();// SHL
#endif
}

void ParmsNewLevel(void)
{
#ifdef _DEBUG
	SERVER_PRINT("ParmsNewLevel()\n");
#endif
}


void ParmsChangeLevel(void)
{
#ifdef _DEBUG
	SERVER_PRINT("ParmsChangeLevel()\n");
#endif
	// retrieve the pointer to the save data
	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;

	if (pSaveData)
		pSaveData->connectionCount = BuildChangeList( pSaveData->levelList, MAX_LEVEL_CONNECTIONS );
}

//
// GLOBALS ASSUMED SET:  g_ulFrameCount
//
/*#ifdef _DEBUG
#include <sys/timeb.h>
	struct _timeb timebuffer;
	double sv_time = 0.0;
	double sv_timeprev = 0.0;
	double sv_frametime = 0.0;
#endif*/
void StartFrame(void)
{
//	ALERT(at_console, "StartFrame()\n");
	if (g_pGameRules)
		g_pGameRules->StartFrame();
/*
	if (g_fGameOver)
		return;

#ifdef _DEBUG
	_ftime(&timebuffer);
	sv_time = (double)timebuffer.time + ((double)timebuffer.millitm/1000);// is this a right thing to do?
	sv_frametime = sv_time - sv_timeprev;
	sv_timeprev = sv_time;

	if (test1.value > 9000 && sv_frametime > 1.000)
	{
		SERVER_PRINT(UTIL_VarArgs(">>>>> WHAT THE FUCK?!!!!! FRAME TIME %f TOO LONG!!\n", sv_frametime));
 		DBG_FORCEBREAK
	}
#endif*/
//	g_ulFrameCount++;
#if defined(MOVEWITH)
	CheckAssistList();
#endif
}

void ClientPrecache(void)
{
	// setup precaches always needed
	PRECACHE_SOUND("player/sprayer.wav");		// spray paint sound for PreAlpha

	PRECACHE_SOUND("player/pl_jumpland2.wav");// UNDONE: play 2x step sound

	PRECACHE_SOUND("player/pl_jump1.wav");
	PRECACHE_SOUND("player/pl_jump2.wav");
	PRECACHE_SOUND("player/pl_jump_super.wav");

	PRECACHE_SOUND("player/pl_ringteleport.wav");
	//power ups
	PRECACHE_SOUND("items/haste.wav");
	PRECACHE_SOUND("items/health_aug.wav");
	PRECACHE_SOUND("items/invisibility.wav");
	PRECACHE_SOUND("items/invulnerability.wav");
	PRECACHE_SOUND("items/quaddamage.wav");
	PRECACHE_SOUND("items/rapidfire.wav");
	PRECACHE_SOUND("items/regeneration.wav");
	PRECACHE_SOUND("items/shield_aug.wav");
	PRECACHE_SOUND("items/lightning_field.wav");
	PRECACHE_SOUND("items/respawn.wav");
	PRECACHE_SOUND("items/plasma_shield.wav");
	PRECACHE_SOUND("items/banana.wav");

	PRECACHE_SOUND("items/energy_cube.wav");
	PRECACHE_SOUND("items/accuracy.wav");
	PRECACHE_SOUND("items/air_strike.wav");
	PRECACHE_SOUND("items/satellite_strike.wav");
	PRECACHE_SOUND("items/generator_aug.wav");
	PRECACHE_SOUND("items/longjump.wav");
	PRECACHE_SOUND("items/spidermine.wav");

	PRECACHE_SOUND("items/firesupressor.wav");
	PRECACHE_SOUND("items/weapon_aug.wav");
	PRECACHE_SOUND("items/shield_strength_aug.wav");
	PRECACHE_SOUND("items/battery.wav");
	PRECACHE_SOUND("items/healthkit.wav");

	PRECACHE_SOUND("player/pl_fallpain1.wav");// XDM
	PRECACHE_SOUND("player/pl_fallpain2.wav");
	PRECACHE_SOUND("player/pl_fallpain3.wav");

	PRECACHE_SOUND("player/pl_swim1.wav");		// breathe bubbles
	PRECACHE_SOUND("player/pl_swim2.wav");
	PRECACHE_SOUND("player/pl_swim3.wav");
	PRECACHE_SOUND("player/pl_swim4.wav");

	PRECACHE_SOUND("player/pl_burn.wav");

	PRECACHE_SOUND("plats/train_use1.wav");		// use a train
	PRECACHE_SOUND("items/flashlight1.wav");// XDM

	//weapon common sounds
	PRECACHE_SOUND("items/weapondrop1.wav");
	PRECACHE_SOUND("items/gunpickup2.wav");
	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/medshot5.wav");

	PRECACHE_SOUND("common/watersplash.wav");

	for (int m=0; m<=STEP_GRASS; ++m)// XDM3035a
	{
		for (int i=0; i<NUM_STEP_SOUNDS; ++i)
			PRECACHE_SOUND((char *)gStepSounds[m][i]);
	}

	//energy shield sounds
	PRECACHE_SOUND("player/pl_shield_impact1.wav");
	PRECACHE_SOUND("player/pl_shield_impact2.wav");
	PRECACHE_SOUND("player/pl_shield_impact3.wav");

	// geiger sounds
	PRECACHE_SOUND("player/geiger6.wav");
	PRECACHE_SOUND("player/geiger5.wav");
	PRECACHE_SOUND("player/geiger4.wav");
	PRECACHE_SOUND("player/geiger3.wav");
	PRECACHE_SOUND("player/geiger2.wav");
	PRECACHE_SOUND("player/geiger1.wav");

//	if (g_pGameRules->IsMultiplayer())
		PRECACHE_SOUND("player/respawn.wav");

	PRECACHE_MODEL("models/player.mdl");
	ENGINE_FORCE_UNMODIFIED(force_exactfile, VEC_HULL_MIN, VEC_HULL_MAX, "models/player.mdl");// XDM3035
}

/*
===============
GetGameDescription

Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
===============
*/
const char *GetGameDescription()
{
	if (g_pGameRules) // this function may be called before the world has spawned, and the game rules initialized
		return g_pGameRules->GetGameDescription();
	else
		return "Tyrian:Ground-Assault";
}

/*
================
Sys_Error

Engine is going to shut down, allows setting a breakpoint in game .dll to catch that occasion
================
*/
void Sys_Error( const char *error_string )
{
//	ALERT(at_logged, (char *)error_string);// XDM3035c
	DBG_FORCEBREAK
	// Default case, do nothing. MOD AUTHORS: Add code ( e.g., _asm { int 3 }; here to cause a breakpoint for debugging your game .dlls
//	SERVER_PRINT(error_string);
}
// "Error: lightmap for texture aaatrigger too large (3 x 192 = 576 luxels); cannot exceed 324

/*
================
PlayerCustomization

A new player customization has been registered on the server
UNDONE:  This only sets the # of frames of the spray can logo
animation right now.
================
*/
void PlayerCustomization( edict_t *pEntity, customization_t *pCust )
{
//	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (!pPlayer)
	{
		ALERT(at_console, "PlayerCustomization:  Couldn't get player!\n");
		return;
	}

	if (!pCust)
	{
		ALERT(at_console, "PlayerCustomization:  NULL customization!\n");
		return;
	}

	switch (pCust->resource.type)
	{
	case t_decal:
		pPlayer->SetCustomDecalFrames(pCust->nUserData2); // Second int is max # of frames.
		break;
	case t_sound:
	case t_skin:
	case t_model:
		// Ignore for now.
		break;
	default:
		ALERT(at_console, "PlayerCustomization:  Unknown customization type!\n");
		break;
	}
}

/*
================
SpectatorConnect
A spectator has joined the game
================
*/
void SpectatorConnect(edict_t *pEntity)
{
//	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorConnect();
}

/*
================
SpectatorConnect

A spectator has left the game
================
*/
void SpectatorDisconnect(edict_t *pEntity)
{
//	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorDisconnect();
}

/*
================
SpectatorConnect

A spectator has sent a usercmd
================
*/
void SpectatorThink( edict_t *pEntity )
{
//	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorThink();
}


////////////////////////////////////////////////////////
// PAS and PVS routines for client messaging
//

/*
================
SetupVisibility

A client can have a separate "view entity" indicating that his/her view should depend on the origin of that
view entity.  If that's the case, then pViewEntity will be non-NULL and will be used. Otherwise, the current
entity's origin is used. Either is offset by the view_ofs to get the eye position.
From the eye position, we set up the PAS and PVS to use for filtering network messages to the client. At this point, we could
override the actual PAS or PVS values, or use a different origin.
NOTE:  Do not cache the values of pas and pvs, as they depend on reusable memory in the engine, they are only good for this one frame
================
*/
void SetupVisibility( edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas )
{
	edict_t *pView = pClient;

	// Find the client's PVS
	if (pViewEntity)
		pView = pViewEntity;

	if (pClient->v.flags & FL_PROXY)
	{
		*pvs = NULL;	// the spectator proxy sees
		*pas = NULL;	// and hears everything
		return;
	}

	if (pClient->v.iuser2 != 0)// XDM: use target's PVS
	{
		if (pClient->v.iuser1 == OBS_CHASE_LOCKED ||
			pClient->v.iuser1 == OBS_CHASE_FREE ||
			pClient->v.iuser1 == OBS_IN_EYE ||
			pClient->v.iuser1 == OBS_MAP_CHASE)
		{
			CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance(pClient);
			// Tracking Spectators use the visibility of their target
			if (pPlayer->m_hObserverTarget != NULL)
				pView = pPlayer->m_hObserverTarget->edict();
			else if (pClient->v.iuser2 > 0)
				pView = UTIL_ClientEdictByIndex(pClient->v.iuser2);
		}
	}

	Vector org(pView->v.origin);
/*	if (pView->v.effects & EF_MERGE_VISIBILITY)// XDM: g-cont techinque to use foreign PVS
	{
		org = pView->v.origin;
	}
	else*/
	{
		org += pView->v.view_ofs;

		if (pView->v.flags & FL_DUCKING)
			org += (VEC_HULL_MIN - VEC_DUCK_HULL_MIN);
	}
	*pvs = ENGINE_SET_PVS((float *)&org);
	*pas = ENGINE_SET_PAS((float *)&org);
}

#include "entity_state.h"

/*
AddToFullPack

Return 1 if the entity state has been filled in for the ent and the entity will be propagated to the client, 0 otherwise
state is the server maintained copy of the state info that is transmitted to the client
a MOD could alter values copied into state to send the "host" a different look for a particular entity update, etc.
e and ent are the entity that is being added to the update, if 1 is returned
host is the player's edict of the player whom we are sending the update to
player is 1 if the ent/e is a player and 0 otherwise
pSet is either the PAS or PVS that we previous set up.  We can use it to ask the engine to filter the entity against the PAS or PVS.
we could also use the pas/ pvs that we set in SetupVisibility, if we wanted to.  Caching the value is valid in that case, but still only for the current frame
*/
int AddToFullPack( struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet )
{
	// don't send if flagged for NODRAW and it's not the host getting the message
	if ((ent->v.effects == EF_NODRAW) && (ent != host))
		return 0;

	// Ignore ents without valid / visible models
	if (ent->v.modelindex == 0)// XDM3035c: flame clouds use this || FStringNull(ent->v.model))
		return 0;

	// Don't send spectators to other players
	if ((ent->v.flags & FL_SPECTATOR) && (ent != host))
		return 0;

	// Don't send entity to local client if the client says it's predicting the entity itself.
	if (ent->v.flags & FL_SKIPLOCALHOST)
	{
		if ((hostflags & 1) && (ent->v.owner == host))
			return 0;

		if (host->v.iuser1 == OBS_IN_EYE)// don't send the local host observers in eye mode either
			return 0;
	}

	CBaseEntity *pEntity = CBaseEntity::Instance(ent);
	CBasePlayer *pPlayer = (CBasePlayer *)CBasePlayer::Instance(host);

	if (ent->v.flags & FL_CUSTOMENTITY && ent->v.angles != g_vecZero)// entityType == ENTITY_BEAM
	{
		TraceResult tr;
		UTIL_TraceLine(ent->v.angles, host->v.origin, ignore_monsters, ignore_glass, host, &tr);// "angles" is actually "endpos" for beams
		if (tr.flFraction == 1.0f)
			goto accept;
	}

/*	if (pEntity && pPlayer)
	{
		if (pEntity->ShouldBeSentTo(pPlayer))
			goto accept;
	}*/

	// Ignore if not the host and not touching a PVS/PAS leaf
	// If pSet is NULL, then the test will always succeed and the entity will be added to the update
	if (ent != host)
	{
		// XDM: a little trick for 'sky' entities
		if (!(ent->v.flags & FL_DRAW_ALWAYS))
		{
			if (!ENGINE_CHECK_VISIBILITY(ent, pSet) ||
				(!(ent->v.flags & FL_WORLDBRUSH) && ((VecBModelOrigin(&ent->v) - host->v.origin).Length() > g_psv_zmax->value)))// XDM: PERFORMANCE zmax clip // XDM3034 TESTME
				return 0;
		}
	}

// why doesn't this work!?
/*	if (g_pGameRules->IsGameOver() && player)// XDM3035b: TESTME!!
	{
		CBaseEntity	*pActor = g_pGameRules->GetIntermissionActor1();
		if (ent->v.iuser1 == OBS_INTERMISSION && pActor)
		{
			if (pEntity != pActor)
			{
// walking players get caught too!				if (ent->v.health <= 0.0f && ent->v.deadflag == DEAD_DEAD)// ?
					if (pEntity->pev->origin == pActor->pev->origin)// nearby players somehow pass this!!
						return 0;// HACK to eliminate a bunch of players piling up during intermission
			}
//			if (!ENGINE_CHECK_VISIBILITY((const struct edict_s *)g_pGameRules->GetIntermissionActor1()->edict(), pSet))
//				return 0;

//no		if (ent->v.health <= 0.0f && ent->v.deadflag == DEAD_DEAD)
//			return 0;
		}
	}*/

	if (pEntity && pPlayer)
	{
		if (!pEntity->ShouldBeSentTo(pPlayer))// XDM3035c: individual entities
			return 0;
	}

	if (pEntity->IsPlayer())// XDM3035c: TESTME
		if (pEntity->pev->origin.IsZero())
			return 0;

	if ( host->v.groupinfo )
	{
		UTIL_SetGroupTrace( host->v.groupinfo, GROUP_OP_AND );
		// Should always be set, of course
		if ( ent->v.groupinfo )
		{
			if ( g_groupop == GROUP_OP_AND )
			{
				if ( !(ent->v.groupinfo & host->v.groupinfo ) )
					return 0;
			}
			else if ( g_groupop == GROUP_OP_NAND )
			{
				if ( ent->v.groupinfo & host->v.groupinfo )
					return 0;
			}
		}
		UTIL_UnsetGroupTrace();
	}

accept:
	memset(state, 0, sizeof(*state));

	// Flag custom entities.
/*	if (pEntity->IsPlayer())// XDM: this shit is useless because the ugly engine treats any etype as a fukin' beam!
		state->entityType = ENTITY_PLAYER;
	else if (pEntity->IsPlayerItem())
		state->entityType = ENTITY_ITEM;
	else */if (ent->v.flags & FL_CUSTOMENTITY)
		state->entityType = ENTITY_BEAM;
	else
		state->entityType = ENTITY_NORMAL;

	// Assign index so we can track this entity from frame to frame and delta from it.
	state->number = e;
//	state->msg_time = auto
//	state->messagenum = auto

	// Copy state data
	memcpy(state->origin, ent->v.origin, 3 * sizeof(float));
	memcpy(state->angles, ent->v.angles, 3 * sizeof(float));

	state->modelindex	= ent->v.modelindex;
	state->sequence		= ent->v.sequence;
	state->frame		= ent->v.frame;
	state->colormap		= ent->v.colormap;
	state->skin			= ent->v.skin;
	state->solid		= ent->v.solid;
	state->effects		= ent->v.effects;
	state->scale		= ent->v.scale;

	// This non-player entity is being moved by the game .dll and not the physics simulation system
	//  make sure that we interpolate it's position on the client if it moves
	if (!player && ent->v.animtime &&
		ent->v.velocity[0] == 0 &&
		ent->v.velocity[1] == 0 &&
		ent->v.velocity[2] == 0)
	{
		state->eflags |= EFLAG_SLERP;
	}
	if (ent->v.flags & FL_DRAW_ALWAYS)
		state->eflags |= EFLAG_DRAW_ALWAYS;

	if (pEntity->IsPickup())// XDM3037: wide range of objects
		state->eflags |= EFLAG_HIGHLIGHT;

	state->rendermode		= ent->v.rendermode;
	state->renderamt		= (int)ent->v.renderamt;
	state->rendercolor.r	= (int)ent->v.rendercolor.x;
	state->rendercolor.g	= (int)ent->v.rendercolor.y;
	state->rendercolor.b	= (int)ent->v.rendercolor.z;
	state->renderfx			= ent->v.renderfx;

	state->movetype			= ent->v.movetype;
	state->animtime			= (int)(1000.0f * ent->v.animtime)/1000.0f;// Round animtime to nearest millisecond
	state->framerate		= ent->v.framerate;
	state->body				= ent->v.body;
	state->controller[0]	= ent->v.controller[0];
	state->controller[1]	= ent->v.controller[1];
	state->controller[2]	= ent->v.controller[2];
	state->controller[3]	= ent->v.controller[3];
	state->blending[0]		= ent->v.blending[0];
	state->blending[1]		= ent->v.blending[1];
	state->blending[2]		= ent->v.blending[2];
	state->blending[3]		= ent->v.blending[3];
	memcpy(state->velocity, ent->v.velocity, 3 * sizeof(float));

	memcpy(state->mins, ent->v.mins, 3 * sizeof(float));
	memcpy(state->maxs, ent->v.maxs, 3 * sizeof(float));

	if (ent->v.aiment)
		state->aiment		= ENTINDEX(ent->v.aiment);

	if (ent->v.owner)// XDM
		state->owner		= ENTINDEX(ent->v.owner);

	state->friction			= ent->v.friction;// XDM3035b
	state->gravity			= ent->v.gravity;
	state->team				= ent->v.team;
	state->playerclass		= ent->v.playerclass;
	state->health			= (int)ent->v.health;
	state->spectator		= (ent->v.flags & FL_SPECTATOR);

	if (ent->v.weaponmodel)
		state->weaponmodel	= MODEL_INDEX(STRING(ent->v.weaponmodel));

	// Special stuff for players only
//	if (player)
//	{
	state->gaitsequence	= ent->v.gaitsequence;
	memcpy(state->basevelocity, ent->v.basevelocity, 3 * sizeof(float));
	state->usehull = (ent->v.flags & FL_DUCKING) ? 1 : 0;
//	}
	state->oldbuttons		= ent->v.oldbuttons;
	state->onground			= (ent->v.flags & FL_ONGROUND)?0:-1;// XDM3035c: does not cause jump problems
	state->iStepLeft		= ent->v.iStepLeft;
	state->flFallVelocity	= ent->v.flFallVelocity;
	state->fov				= ent->v.fov;
	state->weaponanim		= ent->v.weaponanim;
	state->startpos			= ent->v.startpos;
	state->endpos			= ent->v.endpos;
//	memcpy(state->startpos, ent->v.startpos, 3 * sizeof(float));
//	memcpy(state->endpos, ent->v.endpos, 3 * sizeof(float));
	state->impacttime		= ent->v.impacttime;
	state->starttime		= ent->v.starttime;

	state->iuser1 = ent->v.iuser1;
	state->iuser2 = ent->v.iuser2;
	state->iuser3 = ent->v.iuser3;
	state->iuser4 = ent->v.iuser4;
	state->fuser1 = ent->v.fuser1;
	state->fuser2 = ent->v.fuser2;
	state->fuser3 = ent->v.fuser3;
	state->fuser4 = ent->v.fuser4;
	state->vuser1 = ent->v.vuser1;
	state->vuser2 = ent->v.vuser2;
	state->vuser3 = ent->v.vuser3;
	state->vuser4 = ent->v.vuser4;
	return 1;
}


/*
===================
CreateBaseline
Creates baselines used for network encoding, especially for player data since players are not spawned until connect time.
===================
*/
void CreateBaseline( int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs )
{
	baseline->origin		= entity->v.origin;
	baseline->angles		= entity->v.angles;
	baseline->frame			= entity->v.frame;
	baseline->skin			= (short)entity->v.skin;
	baseline->body			= entity->v.body;// XDM
	if (entity->v.aiment)
		baseline->aiment		= ENTINDEX(entity->v.aiment);// XDM
	// render information
	baseline->rendermode	= (byte)entity->v.rendermode;
	baseline->renderamt		= (byte)entity->v.renderamt;
	baseline->rendercolor.r	= (byte)entity->v.rendercolor.x;
	baseline->rendercolor.g	= (byte)entity->v.rendercolor.y;
	baseline->rendercolor.b	= (byte)entity->v.rendercolor.z;
	baseline->renderfx		= (byte)entity->v.renderfx;

	if (player)
	{
		baseline->mins			= player_mins;
		baseline->maxs			= player_maxs;
		baseline->colormap		= 0;// XDM3037
		baseline->modelindex	= playermodelindex;
		baseline->friction		= 1.0;
		baseline->movetype		= MOVETYPE_WALK;
		baseline->scale			= 1.0;
		baseline->solid			= SOLID_SLIDEBOX;
		baseline->framerate		= 1.0;
		baseline->gravity		= 1.0;
	}
	else
	{
		baseline->mins			= entity->v.mins;
		baseline->maxs			= entity->v.maxs;
		baseline->colormap		= entity->v.colormap;// XDM
		baseline->modelindex	= entity->v.modelindex;//SV_ModelIndex(pr_strings + entity->v.model);
		baseline->movetype		= entity->v.movetype;
		baseline->scale			= entity->v.scale;
		baseline->solid			= entity->v.solid;
		baseline->framerate		= entity->v.framerate;
		baseline->gravity		= entity->v.gravity;
	}
	baseline->team			= entity->v.team;// XDM
	baseline->fov			= entity->v.fov;
	baseline->oldbuttons	= 0;// XDM3035c
//	baseline->impacttime	= entity->v.impacttime;
//	baseline->starttime		= entity->v.starttime;
}

typedef struct
{
	char name[32];
	int	 field;
} entity_field_alias_t;

enum
{
	FIELD_ORIGIN0 = 0,
	FIELD_ORIGIN1,
	FIELD_ORIGIN2,
	FIELD_ANGLES0,
	FIELD_ANGLES1,
	FIELD_ANGLES2
};

static entity_field_alias_t entity_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
	{ "angles[0]",			0 },
	{ "angles[1]",			0 },
	{ "angles[2]",			0 },
};

void Entity_FieldInit( struct delta_s *pFields )
{
	entity_field_alias[ FIELD_ORIGIN0 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN0 ].name );
	entity_field_alias[ FIELD_ORIGIN1 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN1 ].name );
	entity_field_alias[ FIELD_ORIGIN2 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN2 ].name );
	entity_field_alias[ FIELD_ANGLES0 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES0 ].name );
	entity_field_alias[ FIELD_ANGLES1 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES1 ].name );
	entity_field_alias[ FIELD_ANGLES2 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES2 ].name );
}

/*
==================
Entity_Encode
Callback for sending entity_state_t info over network.
FIXME:  Move to script
==================
*/
void Entity_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	static bool initialized = 0;
	if ( !initialized )
	{
		Entity_FieldInit( pFields );
		initialized = 1;
	}

	entity_state_t *f, *t;
	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	bool localplayer =  ( t->number - 1 ) == ENGINE_CURRENT_PLAYER();
	if ( localplayer )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}

	// Parametric rockets: origin&angles are not used here
	if ( ( t->impacttime != 0 ) && ( t->starttime != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES2 ].field );
	}

	if ( ( t->movetype == MOVETYPE_FOLLOW ) && ( t->aiment != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
	else if ( t->aiment != f->aiment )
	{
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
}




static entity_field_alias_t player_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
	{ "angles[0]",			0 },
	{ "angles[1]",			0 },
	{ "angles[2]",			0 },
};

void Player_FieldInit( struct delta_s *pFields )
{
	player_field_alias[ FIELD_ORIGIN0 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN0 ].name );
	player_field_alias[ FIELD_ORIGIN1 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN1 ].name );
	player_field_alias[ FIELD_ORIGIN2 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN2 ].name );
	player_field_alias[ FIELD_ANGLES0 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ANGLES0 ].name );
	player_field_alias[ FIELD_ANGLES1 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ANGLES1 ].name );
	player_field_alias[ FIELD_ANGLES2 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ANGLES2 ].name );
}

/*
==================
Player_Encode
Callback for sending entity_state_t for players info over network.
==================
*/
void Player_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	static bool initialized = 0;
	if ( !initialized )
	{
		Player_FieldInit( pFields );
		initialized = 1;
	}

	entity_state_t *f, *t;
	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	bool localplayer =  ( t->number - 1 ) == ENGINE_CURRENT_PLAYER();
	if ( localplayer )
	{
		DELTA_UNSETBYINDEX( pFields, player_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, player_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, player_field_alias[ FIELD_ORIGIN2 ].field );
	}

	if ( ( t->movetype == MOVETYPE_FOLLOW ) && ( t->aiment != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, player_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, player_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, player_field_alias[ FIELD_ORIGIN2 ].field );
	}
	else if ( t->aiment != f->aiment )
	{
		DELTA_SETBYINDEX( pFields, player_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_SETBYINDEX( pFields, player_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_SETBYINDEX( pFields, player_field_alias[ FIELD_ORIGIN2 ].field );
	}
}

#define CUSTOMFIELD_ORIGIN0			0
#define CUSTOMFIELD_ORIGIN1			1
#define CUSTOMFIELD_ORIGIN2			2
#define CUSTOMFIELD_ANGLES0			3
#define CUSTOMFIELD_ANGLES1			4
#define CUSTOMFIELD_ANGLES2			5
#define CUSTOMFIELD_SKIN			6
#define CUSTOMFIELD_SEQUENCE		7
#define CUSTOMFIELD_ANIMTIME		8
//#define CUSTOMFIELD_PC				9

entity_field_alias_t custom_entity_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
	{ "angles[0]",			0 },
	{ "angles[1]",			0 },
	{ "angles[2]",			0 },
	{ "skin",				0 },
	{ "sequence",			0 },
	{ "animtime",			0 },
//	{ "playerclass",		0 },
};

void Custom_Entity_FieldInit( struct delta_s *pFields )
{
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_SKIN ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_SKIN ].name );
	custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].field= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].field= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].name );
//	custom_entity_field_alias[ CUSTOMFIELD_PC ].field = DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_PC ].name );
}

/*
==================
Custom_Encode
Callback for sending entity_state_t info ( for custom entities ) over network.
FIXME: Move to script
==================
*/
void Custom_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	static bool initialized = 0;
	if ( !initialized )
	{
		Custom_Entity_FieldInit( pFields );
		initialized = 1;
	}

	entity_state_t *f, *t;
	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	int beamType = t->rendermode & 0x0f;

	if ( beamType != BEAM_POINTS && beamType != BEAM_ENTPOINT )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].field );
	}

	if ( beamType != BEAM_POINTS )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].field );
	}

	if ( beamType != BEAM_ENTS && beamType != BEAM_ENTPOINT )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_SKIN ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].field );
	}

	// animtime is compared by rounding first
	// see if we really shouldn't actually send it
	if ( (int)f->animtime == (int)t->animtime )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].field );
	}
}

/*
=================
RegisterEncoders
Allows game .dll to override network encoding of certain types of entities and tweak values, etc.
=================
*/
void RegisterEncoders( void )
{
	DELTA_ADDENCODER( "Entity_Encode", Entity_Encode );
	DELTA_ADDENCODER( "Custom_Encode", Custom_Encode );
	DELTA_ADDENCODER( "Player_Encode", Player_Encode );
}

int GetWeaponData( struct edict_s *player, struct weapon_data_s *info )
{
	memset(info, 0, MAX_WEAPONS * sizeof(weapon_data_t));

#if defined(CLIENT_WEAPONS)
	entvars_t *pev = &player->v;
	CBasePlayer *pPlayer = (CBasePlayer *)CBasePlayer::Instance(pev);
	if (pPlayer == NULL)
		return 1;

	int i;
	CBasePlayerWeapon *pWeapon;
	for (i = 0; i < MAX_WEAPONS; ++i)
	{
		CBasePlayerItem *pPlayerItem = pPlayer->GetInventoryItem(i);
		if (pPlayerItem)
		{
			pWeapon = pPlayerItem->GetWeaponPtr();
			if (pWeapon && pWeapon->UseDecrement())
				pWeapon->ClientPackData(player, &info[pWeapon->GetID()]);
		}
	}
#else
//	weapon_data_t *item;
//	CBasePlayer *pPlayer = (CBasePlayer *)CBasePlayer::Instance(player);
//	if (pPlayer)
/*	{
		for (int i = 0; i < MAX_WEAPONS; ++i)
			(info[i]).m_iId = i;// lol
	}*/
#endif
	return 1;
}


/*
=================
UpdateClientData
Data sent to current client only
engine sets cd to 0 before calling.
=================
*/
void UpdateClientData ( const struct edict_s *ent, int sendweapons, struct clientdata_s *cd )
{
	cd->origin			= ent->v.origin;
	cd->velocity		= ent->v.velocity;
	cd->viewmodel		= MODEL_INDEX(STRING(ent->v.viewmodel));
	cd->punchangle		= ent->v.punchangle;
	cd->flags			= ent->v.flags;
	cd->waterlevel		= ent->v.waterlevel;
	cd->watertype		= ent->v.watertype;
	cd->view_ofs		= ent->v.view_ofs;
	cd->health			= ent->v.health;
	cd->bInDuck			= ent->v.bInDuck;
	cd->weapons			= ent->v.weapons;
	cd->flTimeStepSound = ent->v.flTimeStepSound;
	cd->flDuckTime		= ent->v.flDuckTime;
	cd->flSwimTime		= ent->v.flSwimTime;
	cd->waterjumptime	= (int)ent->v.teleport_time;
	cd->maxspeed		= ent->v.maxspeed;
	cd->fov				= ent->v.fov;
	cd->weaponanim		= ent->v.weaponanim;
//	cd->m_iId			=;
//	cd->m_flNextAttack	=;
	cd->tfstate			= ent->v.team;
	cd->pushmsec		= ent->v.pushmsec;
	cd->deadflag		= ent->v.deadflag;
	strcpy(cd->physinfo, ENGINE_GETPHYSINFO(ent));
	cd->iuser1			= ent->v.iuser1;
	cd->iuser2			= ent->v.iuser2;
	cd->iuser3			= ent->v.iuser3;
	cd->iuser4			= ent->v.iuser4;
	cd->fuser1			= ent->v.fuser1;
	cd->fuser2			= ent->v.fuser2;
	cd->fuser3			= ent->v.fuser3;
	cd->fuser4			= ent->v.fuser4;
	cd->vuser1			= ent->v.vuser1;
	cd->vuser2			= ent->v.vuser2;
	cd->vuser3			= ent->v.vuser3;
	cd->vuser4			= ent->v.vuser4;

	cd->ammo_shells		= 0;
	cd->ammo_nails		= 0;
	cd->ammo_cells		= 0;
	cd->ammo_rockets	= 0;
	if (sendweapons)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance((edict_t *)ent);
		if (pEntity)// XDM3035b: MAY BE NULL!
		{
			CBaseMonster *pMonster = pEntity->MyMonsterPointer();
			if (pMonster)
			{
				cd->m_flNextAttack	= pMonster->m_flNextAttack;
			}
			if (pEntity->IsPlayer())
			{
				CBasePlayer *pPlayer = (CBasePlayer *)pEntity;
				if (pPlayer->m_pActiveItem)
					cd->m_iId = pPlayer->m_pActiveItem->GetID();
				else
					cd->m_iId = WEAPON_NONE;
			}
		}
	}
}

/*
=================
CmdStart

We're about to run this usercmd for the specified player.  We can set up groupinfo and masking here, etc.
This is the time to examine the usercmd for anything extra.  This call happens even if think does not.
=================
*/
void CmdStart( const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed )
{
	entvars_t *pev = (entvars_t *)&player->v;
	CBasePlayer *pl = (CBasePlayer *)CBasePlayer::Instance(pev);

	if ( !pl )
		return;

	if ( pl->pev->groupinfo != 0 )
		UTIL_SetGroupTrace( pl->pev->groupinfo, GROUP_OP_AND );

	pl->random_seed = random_seed;

//	ALERT(at_console, "cmd->impulse = %d\n", cmd->impulse);// WHY THE FUCK DOES _THIS_ WORK?!!

/* da fuck iz diz shit?!!?!?!?
	if (cmd->weaponselect)// WHY THE FUCK DOES THIS _NOT_ WORK?!!
	{
		ALERT(at_console, "cmd->weaponselect = %d\n", cmd->weaponselect);
		if (pl->m_pActiveItem == NULL || (pl->m_pActiveItem->GetID() != cmd->weaponselect))
			pl->SelectItem(cmd->weaponselect);
	}*/
}

/*
=================
CmdEnd

Each cmdstart is exactly matched with a cmd end, clean up any group trace flags, etc. here
=================
*/
void CmdEnd ( const edict_t *player )
{
	entvars_t *pev = (entvars_t *)&player->v;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );

	if ( !pl )
		return;

	if ( pl->pev->groupinfo != 0 )
		UTIL_UnsetGroupTrace();
}

/*
================================
ConnectionlessPacket

 Return 1 if the packet is valid.  Set response_buffer_size if you want to send a response packet.  Incoming, it holds the max
  size of the response_buffer, so you must zero it out if you choose not to respond.
================================
*/
int	ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
	// Parse stuff from args
//	int max_buffer_size = *response_buffer_size;

	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;

	// Since we don't listen for anything here, just respond that it's a bogus message
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

/*
================================
GetHullBounds

  Engine calls this to enumerate player collision hulls, for prediction.  Return 0 if the hullnumber doesn't exist.
================================
*/
int GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	int iret = 0;

	switch ( hullnumber )
	{
	case 0:				// Normal player
		mins = VEC_HULL_MIN;
		maxs = VEC_HULL_MAX;
		iret = 1;
		break;
	case 1:				// Crouched player
		mins = VEC_DUCK_HULL_MIN;
		maxs = VEC_DUCK_HULL_MAX;
		iret = 1;
		break;
	case 2:				// Point based hull
		mins = g_vecZero;
		maxs = g_vecZero;
		iret = 1;
		break;
	}

	return iret;
}

/*
================================
CreateInstancedBaselines

Create pseudo-baselines for items that aren't placed in the map at spawn time, but which are likely
to be created during play ( e.g., grenades, ammo packs, projectiles, corpses, etc. )
================================
*/
void CreateInstancedBaselines ( void )
{
/*	int iret = 0;
	entity_state_t state;

	memset( &state, 0, sizeof( state ) );
*/
	// Create any additional baselines here for things like grendates, etc.
	// iret = ENGINE_INSTANCE_BASELINE( pc->pev->classname, &state );

	// Destroy objects.
	//UTIL_Remove( pc );
}

/*
================================
InconsistentFile

One of the ENGINE_FORCE_UNMODIFIED files failed the consistency check for the specified player
 Return 0 to allow the client to continue, 1 to force immediate disconnection ( with an optional disconnect message of up to 256 characters )
================================
*/
int	InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message )
{
	// Default behavior is to kick the player
	_snprintf( disconnect_message, 256, "WARNING: client %d file %s differs from server!\n", ENTINDEX(player), filename);// XDM3035: is sprintf safe here?

	// Server doesn't care?
	if (CVAR_GET_FLOAT("mp_consistency") <= 0.0f)
	{
		SERVER_PRINT(disconnect_message);// XDM3035b: we don't care, but aware
		return 0;
	}
	// Kick now with specified disconnect message.
	return 1;
}

/*
================================
AllowLagCompensation

 The game .dll should return 1 if lag compensation should be allowed ( could also just set
  the sv_unlag cvar.
 Most games right now should return 0, until client-side weapon prediction code is written
  and tested for them ( note you can predict weapons, but not do lag compensation, too,
  if you want.
================================
*/
int AllowLagCompensation( void )
{
#if defined(CLIENT_WEAPONS)
	return 1;
#endif
	return 0;// TODO: XDM3035
}
