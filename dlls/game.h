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

#ifndef GAME_H
#define GAME_H

extern void GameDLLInit(void);
extern void GameDLLShutdown(void);// XDM

// registered console commands
void Cmd_GlobalFog(void);
void Cmd_SetKV(void);
void Cmd_ListEnts(void);
void Cmd_SpawnEnt(void);
void Cmd_ClientCmd(void);
void Cmd_EndGame(void);
void Cmd_ServerDir(void);
void Cmd_ServerInitialize(void);


#ifdef _DEBUG
extern cvar_t test1;
extern cvar_t test2;
extern cvar_t test3;
#endif
//extern cvar_t tmpcvar;

extern cvar_t bot_allow;
extern cvar_t bot_random_powerup_lvl;


// Game settings
extern cvar_t showtriggers;
extern cvar_t displaysoundlist;
extern cvar_t sv_nodegraphdisable;

// multiplayer server rules
extern cvar_t mp_bananamode;
extern cvar_t mp_wpnboxbrk;
extern cvar_t mp_scoreleft;
extern cvar_t mp_timeleft;
extern cvar_t mp_gamerules;
extern cvar_t mp_weapon_category;
extern cvar_t mp_maprules;
extern cvar_t mp_teamplay;
extern cvar_t sv_clientgibs;
extern cvar_t mp_allowmusicevents;
extern cvar_t mp_allowspectators;
extern cvar_t sv_lognotice;
extern cvar_t mp_scorelimit;
extern cvar_t mp_capturelimit;
extern cvar_t mp_fraglimit;
extern cvar_t mp_timelimit;
extern cvar_t mp_friendlyfire;
extern cvar_t mp_forcerespawn;
extern cvar_t mp_forcerespawntime;
extern cvar_t mp_teleport_allow;
extern cvar_t mp_respawntime;
extern cvar_t mp_spectoggle;
extern cvar_t mp_teamlist;
extern cvar_t mp_teamcolor1;
extern cvar_t mp_teamcolor2;
extern cvar_t mp_teamcolor3;
extern cvar_t mp_teamcolor4;
extern cvar_t mp_teambalance;
extern cvar_t mp_teamchange;
extern cvar_t mp_teamchangekill;
extern cvar_t mp_defaultteam;
extern cvar_t mp_monstersrespawn;
extern cvar_t mp_monsrespawntime;
extern cvar_t mp_itm_resp_time;
extern cvar_t mp_chattime;
extern cvar_t mp_nofriction;
extern cvar_t mp_flagstay;
extern cvar_t mp_domscoreperiod;
extern cvar_t mp_allowcamera;
extern cvar_t mp_laddershooting;
extern cvar_t mp_teammenu;
extern cvar_t mp_specteammates;
extern cvar_t mp_telegib;
extern cvar_t mp_spawnprotectiontime;
extern cvar_t mp_noshooting;
extern cvar_t mp_revengemode;
extern cvar_t mp_coop_eol_firstwin;
extern cvar_t mp_coop_eol_spectate;
extern cvar_t mp_coop_usemaptransition;
extern cvar_t mp_extra_nuke;
extern cvar_t sv_clientstaticents;
extern cvar_t sv_modelhitboxes;
extern cvar_t sv_generategamecfg;
extern cvar_t sv_loadentfile;
extern cvar_t sv_reliability;
extern cvar_t sv_decalfrequency;

extern cvar_t allowmonsters;

// Engine Cvars
extern cvar_t *g_pdeveloper;
extern cvar_t *g_psv_gravity;
extern cvar_t *g_psv_aim;
extern cvar_t *g_psv_cheats;
extern cvar_t *g_psv_maxspeed;
extern cvar_t *g_psv_zmax;

#endif		// GAME_H
