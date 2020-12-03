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

===== world.cpp ========================================================

  precaches and defs for entities and other data that must always be available.

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "decals.h"
#include "skill.h"
#include "effects.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "globals.h"
#include "game.h"// XDM:
#include "maprules.h"// spawnflags
#include "client.h"// ClientPrecache
#include "entconfig.h"
#include "pm_materials.h"// Materials
#include "func_break.h"
#include "sound.h"
#include "soundent.h"
#include "nodes.h"
#include "shake.h"
#include "shared_resources.h"


DLL_GLOBAL CWorld *g_pWorld = NULL;// XDM


// Call this from Precache() to avoid error messages!
void LinkUserMessages(void)
{
	// Already taken care of?
	if (gmsgInitHUD)
		return;

	// WARNING! Message name must not be longer than 12 characters!
	gmsgInitHUD			= REG_USER_MSG("InitHUD", 0);// called every time a new player joins the server
	gmsgResetHUD		= REG_USER_MSG("ResetHUD", 0);// called every respawn // XDM3035c
	gmsgCurWeapon		= REG_USER_MSG("CurWeapon", 3);
	gmsgGeigerRange		= REG_USER_MSG("Geiger", 1);
	gmsgFlashBattery	= REG_USER_MSG("FlashBat", 3);
	gmsgDamage			= REG_USER_MSG("Damage", 10);
//	gmsgDamageFx		= REG_USER_MSG("DamageFx", 8);// short + long
	gmsgHudText			= REG_USER_MSG("HudText", -1);
	gmsgSayText			= REG_USER_MSG("SayText", -1);
	gmsgTextMsg			= REG_USER_MSG("TextMsg", -1);
	gmsgWeaponList		= REG_USER_MSG("WeaponList", -1);
	gmsgShowGameTitle	= REG_USER_MSG("GameTitle", 1);
	gmsgDeathMsg		= REG_USER_MSG("DeathMsg", -1);
	gmsgScoreInfo		= REG_USER_MSG("ScoreInfo", 5);// XDM: length changed from 9
	gmsgTeamInfo		= REG_USER_MSG("TeamInfo", 2);// XDM: length changed from -1. sets the name of a player's team
	gmsgTeamScore		= REG_USER_MSG("TeamScore", 3);// XDM: length changed from -1. sets the score of a team on the scoreboard
	gmsgDomInfo			= REG_USER_MSG("DomInfo", -1);
	gmsgFlagInfo		= REG_USER_MSG("FlagInfo", 3);
	gmsgGameMode		= REG_USER_MSG("GameMode", -1);// UPDATE: XDM3035c had 9
	gmsgMOTD			= REG_USER_MSG("MOTD", -1);
	gmsgServerName		= REG_USER_MSG("ServerName", -1);
	gmsgAmmoPickup		= REG_USER_MSG("AmmoPickup", 2);
	gmsgWeapPickup		= REG_USER_MSG("WeapPickup", 1);
	gmsgItemPickup		= REG_USER_MSG("ItemPickup", -1);
	gmsgHideWeapon		= REG_USER_MSG("HideWeapon", 1);
	gmsgSetFOV			= REG_USER_MSG("SetFOV", 1);
	gmsgViewMode		= REG_USER_MSG("ViewMode", 1);
	gmsgShowMenu		= REG_USER_MSG("ShowMenu", 2);
	gmsgShake			= REG_USER_MSG("ScreenShake", sizeof(ScreenShake));
	gmsgFade			= REG_USER_MSG("ScreenFade", sizeof(ScreenFade));
	gmsgAmmoX			= REG_USER_MSG("AmmoX", 2);
	gmsgTeamNames		= REG_USER_MSG("TeamNames", -1);
	gmsgStatusText		= REG_USER_MSG("StatusText", -1);
	gmsgStatusValue		= REG_USER_MSG("StatusValue", 3);
	gmsgStatusIcon		= REG_USER_MSG("StatusIcon", -1);
	gmsgViewModel		= REG_USER_MSG("ViewModel", 7);// XDM
	gmsgParticles		= REG_USER_MSG("Particles", 12);
	gmsgPartSys			= REG_USER_MSG("PartSys", 41);
	gmsgSpectator		= REG_USER_MSG("Spectator", 2);
//	gmsgAllowSpec		= REG_USER_MSG("AllowSpec", 1);
	gmsgSnow			= REG_USER_MSG("Snow", 8);
	gmsgSetFog			= REG_USER_MSG("SetFog", 8);
	gmsgSetSky			= REG_USER_MSG("SetSky", 7);
	gmsgSetRain			= REG_USER_MSG("SetRain", 29);// XDM3034 + 2 + 4 bytes // XDM3035c +2
	gmsgAudioTrack		= REG_USER_MSG("AudioTrack", 2);// XDM3035c
	gmsgSetStaticEnt	= REG_USER_MSG("StaticEnt", 27);// XDM3035
	gmsgStaticSprite	= REG_USER_MSG("StaticSpr", 22);// XDM3035a
	gmsgGRInfo			= REG_USER_MSG("GRInfo", 6);
	gmsgGREvent			= REG_USER_MSG("GREvent", 5);// XDM3035 1+2+2
	gmsgUpdateWeapons	= REG_USER_MSG("UpdWeapons", -1);
	gmsgUpdateAmmo		= REG_USER_MSG("UpdAmmo", -1);
	gmsgBubbles			= REG_USER_MSG("Bubbles", 14);// XDM3035c
	gmsgSpeakSnd		= REG_USER_MSG("SpeakSnd", -1);// XDM3035a
	gmsgPickedEnt		= REG_USER_MSG("PickedEnt", 8);// XDM3035a
#ifdef CLIENT_ITEM_SETTINGS
	gmsgSelBestItem		= REG_USER_MSG("SelBestItem", 1);// XDM3035c
#endif
	gmsgEnvParticle		= REG_USER_MSG("EnvParticle", -1);// XDM3035c: SHL compatibility
	gmsgPlayerStats		= REG_USER_MSG("PlayerStats", -1);// XDM3037

	gmsgTrail			= REG_USER_MSG("Trail", 9);
	gmsgFireBeam		= REG_USER_MSG("FireBeam", 21);
	gmsgFireGun			= REG_USER_MSG("FireGun", 11);
	gmsgFireLgtng		= REG_USER_MSG("FireLgtng", 15);
	gmsgMSGManager		= REG_USER_MSG("MSGManager", 3);
	gmsgWpnIcon			= REG_USER_MSG("WpnIcon", 6);
	gmsgItemSpawn		= REG_USER_MSG("ItemSpawn", 18);// XDM3035c: events do not always play
	gmsgThrowGib		= REG_USER_MSG("ThrowGib", 20);
}


// XDM: old stuff moved to effects_new.cpp

CGlobalState::CGlobalState(void)
{
	Reset();
}

void CGlobalState::Reset(void)
{
	m_pList = NULL;
	m_listCount = 0;
}

globalentity_t *CGlobalState::Find(string_t globalname)
{
	if (!globalname)
		return NULL;

	globalentity_t *pTest;
	const char *pEntityName = STRING(globalname);

	pTest = m_pList;
	while (pTest)
	{
		if (FStrEq(pEntityName, pTest->name))
			break;

		pTest = pTest->pNext;
	}

	return pTest;
}

// This is available all the time now on impulse 104, remove later
//#ifdef _DEBUG
void CGlobalState::DumpGlobals(void)
{
	static char *estates[] = { "Off", "On", "Dead" };
	globalentity_t *pTest;

	ALERT(at_console, "-- Globals --\n");
	pTest = m_pList;
	while (pTest)
	{
		ALERT(at_console, "%s: %s (%s)\n", pTest->name, pTest->levelName, estates[pTest->state]);
		pTest = pTest->pNext;
	}
}
//#endif

void CGlobalState::EntityAdd(string_t globalname, string_t mapName, GLOBALESTATE state)
{
	ASSERT(!Find(globalname));

	globalentity_t *pNewEntity = (globalentity_t *)calloc(sizeof(globalentity_t), 1);
	ASSERT(pNewEntity != NULL);
	pNewEntity->pNext = m_pList;
	m_pList = pNewEntity;
	strcpy(pNewEntity->name, STRING(globalname));
	strcpy(pNewEntity->levelName, STRING(mapName));
	pNewEntity->state = state;
	m_listCount++;
}

void CGlobalState::EntitySetState(string_t globalname, GLOBALESTATE state)
{
	globalentity_t *pEnt = Find(globalname);

	if (pEnt)
		pEnt->state = state;
}


const globalentity_t *CGlobalState::EntityFromTable(string_t globalname)
{
	globalentity_t *pEnt = Find(globalname);

	return pEnt;
}

GLOBALESTATE CGlobalState::EntityGetState(string_t globalname)
{
	globalentity_t *pEnt = Find(globalname);
	if (pEnt)
		return pEnt->state;

	return GLOBAL_OFF;
}

// Global Savedata for Delay
TYPEDESCRIPTION	CGlobalState::m_SaveData[] =
{
	DEFINE_FIELD(CGlobalState, m_listCount, FIELD_INTEGER),
};

// Global Savedata for Delay
TYPEDESCRIPTION	gGlobalEntitySaveData[] =
{
	DEFINE_ARRAY(globalentity_t, name, FIELD_CHARACTER, 64),
	DEFINE_ARRAY(globalentity_t, levelName, FIELD_CHARACTER, 32),
	DEFINE_FIELD(globalentity_t, state, FIELD_INTEGER),
};

int CGlobalState::Save(CSave &save)
{
	int i;
	globalentity_t *pEntity;

	if (!save.WriteFields("GLOBAL", this, m_SaveData, ARRAYSIZE(m_SaveData)))
		return 0;

	pEntity = m_pList;
	for (i = 0; i < m_listCount && pEntity; ++i)
	{
		if (!save.WriteFields("GENT", pEntity, gGlobalEntitySaveData, ARRAYSIZE(gGlobalEntitySaveData)))
			return 0;

		pEntity = pEntity->pNext;
	}

	return 1;
}

int CGlobalState::Restore(CRestore &restore)
{
	int i, listCount;
	globalentity_t tmpEntity;

	ClearStates();
	if (!restore.ReadFields("GLOBAL", this, m_SaveData, ARRAYSIZE(m_SaveData)))
		return 0;

	listCount = m_listCount;	// Get new list count
	m_listCount = 0;				// Clear loaded data

	for (i = 0; i < listCount; ++i)
	{
		if (!restore.ReadFields("GENT", &tmpEntity, gGlobalEntitySaveData, ARRAYSIZE(gGlobalEntitySaveData)))
			return 0;
		EntityAdd(MAKE_STRING(tmpEntity.name), MAKE_STRING(tmpEntity.levelName), tmpEntity.state);
	}
	return 1;
}

void CGlobalState::EntityUpdate(string_t globalname, string_t mapname)
{
	globalentity_t *pEnt = Find(globalname);

	if (pEnt)
		strcpy(pEnt->levelName, STRING(mapname));
}

void CGlobalState::ClearStates(void)
{
	globalentity_t *pFree = m_pList;
	while (pFree)
	{
		globalentity_t *pNext = pFree->pNext;
		free(pFree);
		pFree = pNext;
	}
	Reset();
}

void SaveGlobalState(SAVERESTOREDATA *pSaveData)
{
	CSave saveHelper(pSaveData);
	gGlobalState.Save(saveHelper);
}

void RestoreGlobalState(SAVERESTOREDATA *pSaveData)
{
	CRestore restoreHelper(pSaveData);
	gGlobalState.Restore(restoreHelper);
}

void ResetGlobalState(void)
{
	gGlobalState.ClearStates();
	gInitHUD = TRUE;	// Init the HUD on a new game / load game
}



// moved CWorld class definition to cbase.h
//=======================
// CWorld
//
// This spawns first when each level begins.
//=======================
LINK_ENTITY_TO_CLASS(worldspawn, CWorld);

// XDM: world spawnflags moved to "maprules.h"!
void CWorld::Spawn(void)
{
	pev->angles = g_vecZero;// XDM3035a: or the world's gonna...

//	g_fGameOver = FALSE;
	Precache();

	g_iWeaponBoxCount = 0;// XDM3035

	// XDM: from CBasePlayer::Precache()
	if (sv_nodegraphdisable.value <= 0.0f)// XDM3035a
	{
		if (WorldGraph.m_fGraphPresent && !WorldGraph.m_fGraphPointersSet)
		{
			if (!WorldGraph.FSetGraphPointers())
				ALERT(at_console, "**Graph pointers were not set!\n");
			else
				ALERT(at_console, "**Graph Pointers Set!\n");
		}
	}
}

void CWorld::Precache(void)
{
	g_pLastSpawn = NULL;
#if defined(MOVEWITH)
	m_pAssistLink = NULL;
#endif
	m_pFirstAlias = NULL;

	if (entindex() == 0)// XDM3035a: in case some moron creates another "worldspawn"
		g_pWorld = this;// XDM3034: safer, Precache() gets called more often

#if 1
	CVAR_SET_STRING("sv_gravity", UTIL_dtos1(DEFAULT_GRAVITY));//"800"); // 67ft/sec
	CVAR_SET_STRING("sv_stepsize", "18");
#else
	CVAR_SET_STRING("sv_gravity", "384"); // 32ft/sec
	CVAR_SET_STRING("sv_stepsize", "24");
#endif

	CVAR_SET_STRING("room_type", "0");// clear DSP

	CVAR_SET_STRING("sv_maxvelocity", "65535");// XDM
	CVAR_SET_STRING("tracerspeed", "65535");

	g_Language = (int)CVAR_GET_FLOAT("sv_language");

	// Make sure any necessary user messages have been registered
	LinkUserMessages();

	mp_maprules.value = 0.0f;
	CVAR_DIRECT_SET(&mp_maprules, "0");

	if (sv_generategamecfg.value > 0.0f)
		CONFIG_GenerateFromTemplate("settings.scr", "game.cfg");

	SERVER_COMMAND("exec game.cfg\n");
	SERVER_EXECUTE();

	ENTCONFIG_ExecMapPreConfig();// XDM3035b: give user a chance to pre-define some variables for specific map

	// Decide which game type to use. May be affected by previous configs
	int iNewGameType = DetermineGameType();

	// Set up game rules after executing all configs
	if (g_pGameRules)
	{
		if (!g_pGameRules->FPersistBetweenMaps() || g_pGameRules->GetGameType() != iNewGameType)
		{
			SERVER_PRINT("Destroying old game rules\n");// XDM3036: don't keep even persistant rules if new map defines different game type
			delete g_pGameRules;
			g_pGameRules = NULL;
		}
	}

	if (pev->gravity == 0.0f)// XDM3035c: prevent default editor values from corrupting the game
		pev->gravity = 1.0f;
	if (pev->friction == 0.0f)
		pev->friction = 1.0f;

	if (g_pGameRules == NULL)
		g_pGameRules = InstallGameRules(iNewGameType);

	if (g_pGameRules)
		g_pGameRules->Initialize();// XDM
	else
		SERVER_PRINT("Game rules: Initialization failed!\n");

	// We need to have server cvars set (from server cfg) at this point!
	ENTCONFIG_Init();// XDM: load custom entities form <mapname>.ent file

	//!!!UNDONE why is there so much Spawn code in the Precache function? I'll just keep it here
	///!!!LATER - do we want a sound ent in deathmatch? (sjb)
	//pSoundEnt = CBaseEntity::Create("soundent", g_vecZero, g_vecZero, edict());
	if (g_pGameRules->FAllowMonsters())
	{
		pSoundEnt = GetClassPtr((CSoundEnt *)NULL, "soundent");
		pSoundEnt->Spawn();

		if (!pSoundEnt)
			ALERT(at_console, "**COULD NOT CREATE SOUNDENT**\n");
	}
//	InitBodyQue();
// init sentence group playback stuff from sentences.txt.
// ok to call this multiple times, calls after first are ignored.
	SENTENCEG_Init();
// init texture type array from materials.txt
//	TEXTURETYPE_Init();
// the area based ambient sounds MUST be the first precache_sounds
// player precaches
	SERVER_PRINT("Precaching common resources:\n");// XDM
	ClientPrecache();
	PrecacheSharedResources();
	W_Precache();// get weapon precaches

	SERVER_PRINT("Precaching materials...\n");
	// material/texture hit sounds
	MaterialSoundPrecache(matGlass);
	MaterialSoundPrecache(matWood);
	MaterialSoundPrecache(matMetal);
	MaterialSoundPrecache(matFlesh);
	MaterialSoundPrecache(matCinderBlock);
	MaterialSoundPrecache(matCeilingTile);

//	MaterialSoundPrecache(matComputer);			// same as matGlass
//	MaterialSoundPrecache(matUnbreakableGlass);	// same as matGlass
//	MaterialSoundPrecache(matRocks);			// same as matCinderBlock

	int i = 0;
	int c = NUM_BODYDROP_SOUNDS;//sizeof(gSoundsDropBody);// does not work!!!!

//	for(i = 0; i < c; ++i)
//		PRECACHE_SOUND((char *)gSoundsDropBody[i]);

	c = NUM_RICOCHET_SOUNDS;//ARRAYSIZE(gSoundsRicochet);
	for(i = 0; i < c; ++i)
		PRECACHE_SOUND((char *)gSoundsRicochet[i]);

	c = NUM_SPARK_SOUNDS;//ARRAYSIZE(gSoundsSparks);
	for(i = 0; i < c; ++i)
		PRECACHE_SOUND((char *)gSoundsSparks[i]);


//	c = NUM_MATERIALS;// error?! ARRAYSIZE(gMaterials);
//	for(i = 0; i < c; ++i)
//		UTIL_PrecacheMaterial(&gMaterials[i]);


// Ghoul: we precache frequently used models here
	PRECACHE_MODEL("models/w_items.mdl");

	PRECACHE_MODEL("sprites/anim_spr0.spr");
	PRECACHE_MODEL("sprites/anim_spr1.spr");
	PRECACHE_MODEL("sprites/anim_spr2.spr");
	PRECACHE_MODEL("sprites/anim_spr3.spr");
	PRECACHE_MODEL("sprites/anim_spr4.spr");
	PRECACHE_MODEL("sprites/anim_spr5.spr");
	PRECACHE_MODEL("sprites/anim_spr6.spr");
	PRECACHE_MODEL("sprites/anim_spr7.spr");
	PRECACHE_MODEL("sprites/anim_spr8.spr");
	PRECACHE_MODEL("sprites/anim_spr9.spr");
	PRECACHE_MODEL("sprites/anim_spr10.spr");
	PRECACHE_MODEL("sprites/anim_spr11.spr");
	PRECACHE_MODEL("sprites/anim_spr12.spr");

	PRECACHE_MODEL("sprites/explosion0.spr");
	PRECACHE_MODEL("sprites/explosion1.spr");
	PRECACHE_MODEL("sprites/explosion2.spr");
	PRECACHE_MODEL("sprites/explosion3.spr");
	PRECACHE_MODEL("sprites/explosion4.spr");
	PRECACHE_MODEL("sprites/explosion5.spr");

	PRECACHE_MODEL("sprites/particles_red.spr");
	PRECACHE_MODEL("sprites/particles_green.spr");
	PRECACHE_MODEL("sprites/particles_blue.spr");
	PRECACHE_MODEL("sprites/particles_violet.spr");
	PRECACHE_MODEL("sprites/particles_white.spr");
	PRECACHE_MODEL("sprites/particles_black.spr");
	PRECACHE_MODEL("sprites/particles_gibs.spr");
	PRECACHE_MODEL("sprites/beams_all.spr");

	g_usCaptureObject	= PRECACHE_EVENT(1, "events/game/captureobject.sc");
	g_usDomPoint		= PRECACHE_EVENT(1, "events/game/dompoint.sc");
	g_usTeleport		= PRECACHE_EVENT(1, "events/game/teleport.sc");
	g_usPM_Fall			= PRECACHE_EVENT(1, "events/pm/fall.sc");// XDM3035a

	SERVER_PRINT("Loading light styles and decals.\n");// XDM
//
// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.
//
	// 0 normal
	LIGHT_STYLE(0, "m");
	// 1 FLICKER (first variety)
	LIGHT_STYLE(1, "mmnmmommommnonmmonqnmmo");
	// 2 SLOW STRONG PULSE
	LIGHT_STYLE(2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");
	// 3 CANDLE (first variety)
	LIGHT_STYLE(3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");
	// 4 FAST STROBE
	LIGHT_STYLE(4, "mamamamamama");
	// 5 GENTLE PULSE 1
	LIGHT_STYLE(5,"jklmnopqrstuvwxyzyxwvutsrqponmlkj");
	// 6 FLICKER (second variety)
	LIGHT_STYLE(6, "nmonqnmomnmomomno");
	// 7 CANDLE (second variety)
	LIGHT_STYLE(7, "mmmaaaabcdefgmmmmaaaammmaamm");
	// 8 CANDLE (third variety)
	LIGHT_STYLE(8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");
	// 9 SLOW STROBE (fourth variety)
	LIGHT_STYLE(9, "aaaaaaaazzzzzzzz");
	// 10 FLUORESCENT FLICKER
	LIGHT_STYLE(10, "mmamammmmammamamaaamammma");
	// 11 SLOW PULSE NOT FADE TO BLACK
	LIGHT_STYLE(11, "abcdefghijklmnopqrrqponmlkjihgfedcba");
	// 12 UNDERWATER LIGHT MUTATION
	// this light only distorts the lightmap - no contribution
	// is made to the brightness of affected surfaces
	LIGHT_STYLE(12, "mmnnmmnnnmmnn");

// XDM: -------- DO NOT ADD ANYTHING HERE!!! --------
	// styles 32-62 are assigned by the light program for switchable lights
	// 63 testing
	LIGHT_STYLE(63, "a");

	for (int j = 0; j < DECAL_ENUM_SIZE; ++j)//ARRAYSIZE(g_Decals); ++i)
		g_Decals[j].index = DECAL_INDEX(g_Decals[j].name);

	SERVER_PRINT("Done precaching.\n");// XDM
// init the WorldGraph.
	if (sv_nodegraphdisable.value <= 0.0f)// XDM3035a
	{
		WorldGraph.InitGraph();

		if (g_pGameRules->FAllowMonsters())// XDM3035
		{
		// make sure the .NOD file is newer than the .BSP file.
			if (WorldGraph.CheckNODFile(STRINGV(gpGlobals->mapname)))
			{// Load the node graph for this level
				if (!WorldGraph.FLoadGraph(STRINGV(gpGlobals->mapname)))
				{// couldn't load, so alloc and prepare to build a graph.
					ALERT(at_console, "*Error opening .NOD file\n");
					WorldGraph.AllocNodes();
				}
				else
					ALERT(at_console, "\n*Graph Loaded!\n");
			}
			else
				WorldGraph.AllocNodes();// NOD file is not present, or is older than the BSP file.
		}
	}

	if (pev->speed > 0)
		CVAR_SET_FLOAT("sv_zmax", pev->speed);
	else
		CVAR_SET_FLOAT("sv_zmax", 8192);

	if (pev->netname)
	{
//		ALERT(at_aiconsole, "Chapter title: %s\n", STRING(pev->netname));
		CBaseEntity *pEntity = CBaseEntity::Create("env_message", g_vecZero, g_vecZero, NULL);
		if (pEntity)
		{
			pEntity->SetThink(&CBaseEntity::SUB_CallUseToggle);
			pEntity->pev->message = pev->netname;
			pEntity->pev->nextthink = gpGlobals->time + 0.3;
			pEntity->pev->spawnflags = SF_MESSAGE_ONCE;
			pev->netname = 0;
		}
	}

	if (pev->spawnflags & SF_WORLD_DARK)
		CVAR_SET_FLOAT("v_dark", 1.0);
	else
		CVAR_SET_FLOAT("v_dark", 0.0);

	if (pev->spawnflags & SF_WORLD_TITLE)
		g_DisplayTitle = true;// display the game title if this key is set
	else
		g_DisplayTitle = false;
}

//
// Just to ignore the "wad" field.
//
void CWorld::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "skyname"))
	{
		// Sent over net now.
		CVAR_SET_STRING("sv_skyname", pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "sounds"))
	{
		gpGlobals->cdAudioTrack = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "WaveHeight"))
	{
		// Sent over net now.
		pev->scale = atof(pkvd->szValue) * (1.0/8.0);
		pkvd->fHandled = TRUE;
		CVAR_SET_FLOAT("sv_wateramp", pev->scale);
	}
	else if (FStrEq(pkvd->szKeyName, "MaxRange"))
	{
		pev->speed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "chaptertitle"))
	{
		pev->netname = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "startdark"))
	{
		// UNDONE: This is a gross hack!!! The CVAR is NOT sent over the client/sever link
		// but it will work for single player
		int flag = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
		if (flag)
			pev->spawnflags |= SF_WORLD_DARK;
	}
	else if (FStrEq(pkvd->szKeyName, "newunit"))
	{
		// Single player only.  Clear save directory if set
		if (atoi(pkvd->szValue))
			CVAR_SET_FLOAT("sv_newunit", 1);

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "gametitle"))
	{
		if (atoi(pkvd->szValue))
			pev->spawnflags |= SF_WORLD_TITLE;

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "mapteams"))
	{
		pev->team = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "defaultteam"))
	{
		if (atoi(pkvd->szValue))
			pev->spawnflags |= SF_WORLD_FORCETEAM;

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "roomtype"))// XDM
	{
		pev->playerclass = atoi(pkvd->szValue);// XDM3035c: no problems with save/restore
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "startsuit"))
	{
		if (atoi(pkvd->szValue) > 0)
			pev->spawnflags |= SF_WORLD_STARTSUIT;

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "nomonsters"))
	{
		if (atoi(pkvd->szValue) > 0)
			pev->spawnflags |= SF_WORLD_NOMONSTERS;

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "wad"))
		pkvd->fHandled = TRUE;
	else if (FStrEq(pkvd->szKeyName, "worldtype"))
		pkvd->fHandled = TRUE;
	else if (FStrEq(pkvd->szKeyName, "mapversion"))
		pkvd->fHandled = TRUE;
	else if (FStrEq(pkvd->szKeyName, "version"))
		pkvd->fHandled = TRUE;
	else if (FStrEq(pkvd->szKeyName, "light"))
		pkvd->fHandled = TRUE;
	else if (FStrEq(pkvd->szKeyName, "copyright"))
		pkvd->fHandled = TRUE;
	else
		CBaseEntity::KeyValue(pkvd);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Note   : UNDONE!!!! What if a player is asking for data more than one time?!
// Input  : *pClient - 
//			msgtype - 
// Output : int
//-----------------------------------------------------------------------------
int CWorld::SendClientData(CBasePlayer *pClient, int msgtype)
{
	if (msgtype == MSG_ONE && pClient)// a single client asking for update
	{
		if (!FStringNull(pev->noise))
		{
			PlayAudioTrack(pClient, STRING(pev->noise), 0);
			return 1;
		}
		else if (gpGlobals->cdAudioTrack > 1)
		{
			PlayCDTrack(pClient, gpGlobals->cdAudioTrack, 0);
			return 1;
		}
	}
	return 0;
}

extern unsigned char g_ClientShouldInitizlize[];

//-----------------------------------------------------------------------------
// Purpose: called before destructor
//-----------------------------------------------------------------------------
void CWorld::OnFreePrivateData(void)
{
	if (g_pGameRules && g_pGameRules->IsMultiplayer())// Fix all temporary users in mp
	{
		CBaseEntity *pEntity = NULL;
		while ((pEntity = UTIL_FindEntityByClassname(pEntity, "DelayedUse")) != NULL)
		{
			pEntity->m_pGoalEnt = NULL;
//			UTIL_Remove(pEntity);
		}
	}
	//memset(g_ClientShouldInitizlize, 0, sizeof(
	int i;
	for (i=0; i<=MAX_CLIENTS; ++i)//gpGlobals->maxClients; ++i)
		g_ClientShouldInitizlize[i] = 1;

	// SHL, G-Cont
	CBaseEntity *pEntity = NULL;
	for (i=1; i <= gpGlobals->maxEntities; ++i)
	{
		edict_t *pEntityEdict = INDEXENT(i);
		if (UTIL_IsValidEntity(pEntityEdict) && !FStringNull(pEntityEdict->v.globalname))
		{
			pEntity = CBaseEntity::Instance(pEntityEdict);
			if (pEntity)
				pEntity->PrepareForTransfer();
		}
	}
}
