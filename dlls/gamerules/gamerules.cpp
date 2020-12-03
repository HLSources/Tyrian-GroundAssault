#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "skill.h"
#include "globals.h"
#include "monsters.h"
#include "game.h"
#include "gamerules.h"
#include "maprules.h"
#include "teamplay_gamerules.h"
#include "coop_gamerules.h"// XDM
#include "dom_gamerules.h"
#include "ctf_gamerules.h"
#include "round_gamerules.h"
#include "lms_gamerules.h"
#include "entconfig.h"
#include "msg_fx.h"

DLL_GLOBAL CGameRules *g_pGameRules = NULL;

CGameRules::CGameRules()
{
//	m_iGameType = 0;
	m_iGameMode = 0;
	m_fStartTime = gpGlobals->time;
	m_bGameOver = false;
	m_bPersistBetweenMaps = false;
}

CGameRules::~CGameRules()
{
//	m_iGameType = 255;// ?
}


//-----------------------------------------------------------------------------
// Purpose: 
// Output : short
//-----------------------------------------------------------------------------
short CGameRules::GetGameType(void)
{
	ALERT(at_error, "CGameRules: PURE VIRTUAL GAME RULES AND NO TYPE!\n");
	return 255;
}

//-----------------------------------------------------------------------------
// Purpose: // XDM: must be called by all derived classes!
//-----------------------------------------------------------------------------
void CGameRules::Initialize(void)
{
	if (m_bPersistBetweenMaps && m_bGameOver)// persist flag is ON, and game ove is ON (previous match is over)
		SERVER_PRINT("Game rules transferred between maps\n");

//	m_fStartTime = gpGlobals->time;// ?
	m_bGameOver = false;
	RefreshSkillData();// XDM: do this after loading CFG files
}

//-----------------------------------------------------------------------------
// Purpose: load the SkillData struct with the proper values based on the skill level
//-----------------------------------------------------------------------------
void CGameRules::RefreshSkillData(void)
{
	int	iSkill = (int)CVAR_GET_FLOAT("skill");

	if (iSkill < 1)
		iSkill = 1;
	else if (iSkill > 3)
		iSkill = 3; 

	g_iSkillLevel = iSkill;// XDM
	CVAR_SET_FLOAT("skill", (float)iSkill);

	ALERT(at_console, " Skill level: %d\n",iSkill);

	SkillUpdateData(iSkill);
}

//-----------------------------------------------------------------------------
// Purpose: Called when current weapon becomes unusable.
// Updated: Get method DOES NOT SELECT anything
// Input  : *pPlayer - inventory owner
//			*pCurrentWeapon - WARNING: may be active, already detached from player or NULL!
// Output : *pBest may be NULL if no weapons
//-----------------------------------------------------------------------------
CBasePlayerItem *CGameRules::GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon)
{
	CBasePlayerItem *pCheck = NULL;
	CBasePlayerItem *pBest = NULL;// this will be used in the event that we don't find a weapon in the same category.
	int iBestWeight = -1;
//	if (pCurrentWeapon)
//		iBestWeight = pCurrentWeapon->iWeight();
	for (int i = 0; i < MAX_WEAPONS; ++i)
	{
		pCheck = pPlayer->GetInventoryItem(i);
		if (pCheck)
		{
			if (pCheck->iWeight() > iBestWeight && pCheck != pCurrentWeapon && pCheck->CanDeploy())// don't reselect the weapon we're trying to get rid of
			{
				iBestWeight = pCheck->iWeight();// if this weapon is useable, flag it as the best
				pBest = pCheck;
			}
/*			else if (pCheck->iWeight() > -1 && pCheck->iWeight() == pCurrentWeapon->iWeight() && pCheck != pCurrentWeapon)// this weapon has the same priority
			{
				if (pCheck->CanDeploy())
				{
					pBest = pCheck;
				}
			}*/
		}
	}
	return pBest;
}

//-----------------------------------------------------------------------------
// Purpose: Do NOT get into player's inventory here! Use as entity filter!
// Input  : *pPlayer - 
//			iAmmoIndex - 
//			iMaxCarry - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRules::CanHaveAmmo(CBasePlayer *pPlayer, const int &iAmmoIndex, const int &iMaxCarry)
{
//	if (pPlayer && iAmmoIndex > -1 && iAmmoIndex < MAX_AMMO_SLOTS)// pPlayer ???!!
			return true;// player has room for more of this type of ammo
//	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Move player to a proper spawn point
// Input  : *pPlayer - 
// Output : CBaseEntity
//-----------------------------------------------------------------------------
CBaseEntity *CGameRules::PlayerUseSpawnSpot(CBasePlayer *pPlayer)
{
	CBaseEntity *pSpawnSpot = NULL;
	try
	{
		pSpawnSpot = SpawnPointEntSelect(pPlayer);
	}
	catch(...)
	{
		SERVER_PRINT("SpawnPointEntSelect() exception!!\n");
		pSpawnSpot = NULL;//?! g_pWorld;
	}
	if (pSpawnSpot)
	{
		if (pSpawnSpot->IsBSPModel())// XDM3035c: spawn inside trigger
			pPlayer->pev->origin = RandomVectors(pSpawnSpot->pev->absmin, pSpawnSpot->pev->absmax);
		else
			pPlayer->pev->origin = pSpawnSpot->pev->origin;// + Vector(0,0,1);

		pPlayer->pev->origin.z += 1.0f;
		pPlayer->pev->v_angle  = g_vecZero;
		pPlayer->pev->velocity = pSpawnSpot->pev->velocity;// XDM3035c: TESTME! g_vecZero;
		pPlayer->pev->angles = pSpawnSpot->pev->angles;
		pPlayer->pev->punchangle = g_vecZero;
		pPlayer->pev->fixangle = TRUE;
//why not?		UTIL_SetOrigin(pPlayer, pPlayer->pev->origin);
	}
	else
		ALERT(at_console, "CGameRules::PlayerUseSpawnSpot(%d) failed!\n", pPlayer->entindex());

	return pSpawnSpot;
}

//-----------------------------------------------------------------------------
// Purpose: Can this player have specified item?
// Input  : *pPlayer - player instance
//			*pItem - item instance
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRules::CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pItem)
{
	if (!pPlayer->IsAlive())// only living players can have items
		return false;

	if (gEvilImpulse101 == 0 && pPlayer->m_fInitHUD)// XDM: HACK: prevent player from touching in-world weapons during HUD initialization
		return false;

	if (pPlayer->GetInventoryItem(pItem->GetID()))
	{
		// we can't carry anymore ammo for this gun. We can only have the gun if we aren't already carrying one of this type
		if (pItem->pszAmmo1() && CanHaveAmmo(pPlayer, GetAmmoIndexFromRegistry(pItem->pszAmmo1()), pItem->iMaxAmmo1()))// XDM: don't use pItem->PrimaryAmmoIndex()!!!
			return true;

		if (pItem->pszAmmo2() && CanHaveAmmo(pPlayer, GetAmmoIndexFromRegistry(pItem->pszAmmo2()), pItem->iMaxAmmo2()))
			return true;

		// weapon doesn't use ammo, don't take another if you already have it.
	}
	else
		return true;

	// note: will fall through to here if GetItemInfo doesn't fill the struct!
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Real number of active players on server, 
// Output : int
//-----------------------------------------------------------------------------
int CGameRules::CountPlayers(void)
{
	int c = 0;
	for(int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		if (UTIL_ClientByIndex(i) != NULL)
			++c;
	}
	return c;
}

//-----------------------------------------------------------------------------
// Purpose: Should autoaim track this target?
// Input  : *pPlayer - 
//			*pTarget - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRules::ShouldAutoAim(CBasePlayer *pPlayer, CBaseEntity *pTarget)
{
	if (pPlayer == pTarget)
		return false;

	return (pTarget->pev->takedamage == DAMAGE_AIM);
}

//-----------------------------------------------------------------------------
// Purpose: Sends current game rules to a client
// Input  : *pPlayer - dest
//-----------------------------------------------------------------------------
void CGameRules::UpdateGameMode(CBasePlayer *pPlayer)
{
	unsigned char gameflags = 0;// XDM3035a: some useful extensions

	if (mp_noshooting.value > 0.0f)
		gameflags |= GAME_FLAG_NOSHOOTING;

	if (mp_allowcamera.value > 0.0f)
		gameflags |= GAME_FLAG_ALLOW_CAMERA;

	if (mp_allowspectators.value > 0.0f)
		gameflags |= GAME_FLAG_ALLOW_SPECTATORS;

	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
		WRITE_BYTE(g_pGameRules->GetGameType());
		WRITE_BYTE(m_iGameMode);
		WRITE_BYTE(g_iSkillLevel);// XDM3035a
		WRITE_BYTE(gameflags);// XDM3035a is this a good place for these? (can be changed by the server on-the-fly)
		WRITE_BYTE((int)mp_revengemode.value);
		WRITE_SHORT((int)mp_fraglimit.value);
		WRITE_SHORT(g_pGameRules->GetScoreLimit());// XDM3035a: was (int)mp_scorelimit.value
		WRITE_BYTE(GetRoundsLimit());
		WRITE_BYTE(GetRoundsPlayed());
		WRITE_SHORT(MAX_PLAYER_HEALTH);
	MESSAGE_END();
}

//-----------------------------------------------------------------------------
// Purpose: Update player skin immediately (without restart). Warning! Should not interfere with teamplay!
// Input  : *pPlayer - 
//			*infobuffer - 
//-----------------------------------------------------------------------------
void CGameRules::ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer)
{
	char *strskin = GET_INFO_KEY_VALUE(infobuffer, "skin");
	if (strskin)
		pPlayer->pev->skin = atoi(strskin);
}

//-----------------------------------------------------------------------------
// Purpose: World models will be scaled by this factor. Don't use direcly!
//			Call UTIL_GetWeaponWorldScale() instead!
// Output : float
//-----------------------------------------------------------------------------
float CGameRules::FlWeaponWorldScale(void)
{
	return 1.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Is game over?
// Output : Returns TRUE on success, false on failure.
//-----------------------------------------------------------------------------
bool CGameRules::IsGameOver(void)
{
	return m_bGameOver;
}

//-----------------------------------------------------------------------------
// Purpose: the one and only way
//-----------------------------------------------------------------------------
//void CGameRules::SetGameType(short gametype)
//{
//		m_iGameType = gametype;
//}






//-----------------------------------------------------------------------------
// Purpose: Search map for player enemies
// Output : int
//-----------------------------------------------------------------------------
/* useless because monsters are not added yet
int CheckMapForMonsters(void)
{
	// Prepare ALL entities for the game
	int entindex = 1;
	edict_t *pEdict = NULL;
	do
	{
		pEdict = INDEXENT(entindex);
		++entindex;
		if (pEdict)
		{
			if (!pEdict->free)
			{
				CBaseEntity *pEntity = NULL;
				pEntity = CBaseEntity::Instance(pEdict);
				if (pEntity)
				{
					if (pEntity->IsMonster())
					{
						if (g_iRelationshipTable[pEntity->Classify()][CLASS_PLAYER] > R_NO)// dislike or hate players
							return 1;// don't count, just return
					}
				}
			}
		}
	} while (entindex < gpGlobals->maxEntities);
	return 0;
}*/

//-----------------------------------------------------------------------------
// Purpose: DetermineGameType based on 'deathmatch, mp_maprules' and map name
// Output : int GT_SINGLE
//-----------------------------------------------------------------------------
int DetermineGameType(void)
{
	// Execute this beforehand, but not from here.
//	SERVER_COMMAND("exec game.cfg\n");
//	SERVER_EXECUTE();

	int iGT = (int)mp_gamerules.value;

	if (gpGlobals->deathmatch <= 0)
	{
		iGT = GT_SINGLE;
	}
	else
	{
		if (iGT <= GT_SINGLE)// auto-select
		{
			if (mp_maprules.value > GT_SINGLE)// this var should be aded to maps/<mapname>_pre.cfg for every map
				iGT = (int)mp_maprules.value;// overrides everything else
			else if (strnicmp(STRING(gpGlobals->mapname), "CTF_", 4) == 0)// TODO: move these prefixes to a file in "<prefix> <gamerules#>\n" format
				iGT = GT_CTF;
			else if (strnicmp(STRING(gpGlobals->mapname), "DOM_", 4) == 0)
				iGT = GT_DOMINATION;
//			else if (strnicmp(STRING(gpGlobals->mapname), "AS_", 3) == 0)
//				iGT = GT_ASSAULT;
//			else if (CheckMapForMonsters())// detect CoOp if there are monsters
//				iGT = GT_COOP;
			else if (strnicmp(STRING(gpGlobals->mapname), "OP4CTF_", 7) == 0)// TODO: move these prefixes to a file in "<prefix> <gamerules#>\n" format
				iGT = GT_CTF;
			else// no need to check "DM_"
			{
				if (mp_teamplay.value > 0.0f)// teamplay selected by default
					iGT = GT_TEAMPLAY;
				else
					iGT = GT_DEATHMATCH;
			}
		}
	}
	return iGT;
}

//-----------------------------------------------------------------------------
// Purpose: Decide wich game rules to use this time
// Input  : game_type - GT_SINGLE, etc.
// Output : CGameRules
//-----------------------------------------------------------------------------
CGameRules *InstallGameRules(int game_type)
{
//	int game_type = DetermineGameType();
	CGameRules *pNewGameRules = NULL;// XDM

	switch (game_type)
	{
	case GT_SINGLE:
		{
			pNewGameRules = new CGameRulesSinglePlay;
		}
		break;
	case GT_COOP:
		{
			pNewGameRules = new CGameRulesCoOp;
			gpGlobals->coop = 1.0;
		}
		break;
	case GT_DEATHMATCH:
		{
			pNewGameRules = new CGameRulesMultiplay;
		}
		break;
	case GT_LMS:
		{
			pNewGameRules = new CGameRulesLMS;
		}
		break;
	case GT_TEAMPLAY:
		{
			pNewGameRules = new CGameRulesTeamplay;
		}
		break;
	case GT_CTF:
		{
			pNewGameRules = new CGameRulesCTF;
		}
		break;
	case GT_DOMINATION:
		{
			pNewGameRules = new CGameRulesDomination;
		}
		break;
	case GT_ROUND:
		{
			pNewGameRules = new CGameRulesRoundBased;
		}
		break;
/*	case GT_ASSAULT:
		{
			pNewGameRules = new CGameRulesAssault;
		}
		break;*/
	default:
		{
			ALERT(at_console, "Unknown game type!\n");
//			iGT = GT_DEATHMATCH;
			pNewGameRules = new CGameRulesMultiplay;
		}
		break;
	}

	ALERT(at_console, "\n ** GAME TYPE: %s (flags %f) **\n", pNewGameRules->GetGameDescription(), gpGlobals->serverflags);
//	pNewGameRules->m_iGameType = game_type;
	gpGlobals->deathmatch = (float)game_type;// XDM: for external DLLs

	if (pNewGameRules->IsTeamplay())
		gpGlobals->teamplay = 1.0f;
	else
		gpGlobals->teamplay = 0.0f;

	return pNewGameRules;
}

/*
const char *GetGameDescription(const int &gamerules)
{
	char stringname[16];
	sprintf(stringname, "%s%d\0", GAMETITLE_STRING_NAME, gamerules);
	const char *str = BufferedLocaliseTextString(stringname);
	if (str)
		return str;

	return GAMETITLE_DEFAULT_STRING;
}
*/


//-----------------------------------------------------------------------------
// Purpose: Is this a real, playable team? // TEAM_NONE must be invalid here!
// Input  : &team_id - TEAM_ID
// Output : Returns true if TEAM_1 to TEAM_4 and active
//-----------------------------------------------------------------------------
bool IsActiveTeam(const TEAM_ID &team_id)
{
	if (g_pGameRules)
		return (team_id > TEAM_NONE && team_id <= g_pGameRules->GetNumberOfTeams());

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Real player, connected and not a spectator?
// Input  : *ent - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool IsActivePlayer(CBaseEntity *pPlayerEntity)
{
	if (pPlayerEntity && pPlayerEntity->IsPlayer() && !FStringNull(pPlayerEntity->pev->netname))
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pPlayerEntity;
		if (!pPlayer->IsObserver())// UNDONE: intermission!
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Real player, connected and not a spectator?
// Input  : *ent - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool IsActivePlayer(const int &idx)
{
	return IsActivePlayer(UTIL_ClientByIndex(idx));
}

//-----------------------------------------------------------------------------
// Purpose: Is this a possible index for a player?
// Input  : idx - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool IsValidPlayerIndex(const int &idx)
{
	if (idx >= 1 && idx <= gpGlobals->maxClients)
		return true;

	return false;
}




//CBaseEntity *pPrepareEntity;// XDM
static Vector mem_origin;// coords
static Vector mem_angles;
static bool remembered;
TraceResult g_PickTrace;
CBaseEntity *g_pPickEntity = NULL;
extern EHANDLE g_WriteEntityToFile;

//-----------------------------------------------------------------------------
// Purpose: Developer commands. May be considered cheats. Sort by usability!
// Input  : *pPlayer - 
//			*pcmd - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool DeveloperCommand(CBasePlayer *pPlayer, const char *pcmd)
{
	if (g_pdeveloper->value <= 0.0f)
		return false;

	if (FStrEq(pcmd, ".p"))// pick
	{
		if (CMD_ARGC() >= 7)
		{
			Vector vecSrc, vecEnd;
			vecSrc.x = atof(CMD_ARGV(1));
			vecSrc.y = atof(CMD_ARGV(2));
			vecSrc.z = atof(CMD_ARGV(3));
			vecEnd.x = atof(CMD_ARGV(4));
			vecEnd.y = atof(CMD_ARGV(5));
			vecEnd.z = atof(CMD_ARGV(6));
			UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(pPlayer->pev), &g_PickTrace);
			if (g_PickTrace.flFraction != 1.0f && !FNullEnt(g_PickTrace.pHit))
			{
				g_pPickEntity = CBaseEntity::Instance(g_PickTrace.pHit);
				if (g_pPickEntity)
				{
					MESSAGE_BEGIN(MSG_ONE, gmsgPickedEnt, NULL, pPlayer->edict());
						WRITE_SHORT(g_pPickEntity->entindex());
						WRITE_COORD(g_PickTrace.vecEndPos.x);
						WRITE_COORD(g_PickTrace.vecEndPos.y);
						WRITE_COORD(g_PickTrace.vecEndPos.z);
					MESSAGE_END();
				}
			}
		}
		else
			memset(&g_PickTrace, NULL, sizeof(TraceResult));
	}
	else if (FStrEq(pcmd, ".c") && CMD_ARGC() >= 3)// create
	{
//		if (CMD_ARGC() >= 3)// <classname> <\"x y z\" origin> <\"x y z\" angles> [targetname]
		{
			Cmd_SpawnEnt();// reuse this command code
			if (g_WriteEntityToFile.Get())// make sure the entity was created
			{
	//			if (sv_loadentfile.value > 0.0f)
	//			{
				ENTCONFIG_WriteEntity(g_WriteEntityToFile->pev);
/*				char mapentname[80];
				sprintf(mapentname, "maps/%s.ent", STRING(gpGlobals->mapname));
				FILE *pMapConfig = LoadFile(mapentname, "a+");// Opens an empty file for both reading and writing. If the given file exists, its contents are destroyed.
				if (pMapConfig)
				{
					fseek(pMapConfig, 0L, SEEK_END);
					if (CMD_ARGV(4))
						fprintf(pMapConfig, "{\n\"classname\" \"%s\"\n\"origin\" \"%s\"\n\"angles\" \"%s\"\n\"targetname\" \"%s\"\n}\n", CMD_ARGV(1), CMD_ARGV(2), CMD_ARGV(3), CMD_ARGV(4));
					else
						fprintf(pMapConfig, "{\n\"classname\" \"%s\"\n\"origin\" \"%s\"\n\"angles\" \"%s\"\n}\n", CMD_ARGV(1), CMD_ARGV(2), CMD_ARGV(3));

					fclose(pMapConfig);
				}*/
	//			}else{
	/* OLD way: through map config commands
				char mapconfigname[80];
				sprintf(mapconfigname, "maps/%s.cfg", STRING(gpGlobals->mapname));
				FILE *pMapConfig = LoadFile(mapconfigname, "a+");// Opens an empty file for both reading and writing. If the given file exists, its contents are destroyed.
				if (pMapConfig)
				{
	//				fseek(pMapConfig, 0L, SEEK_END);
					fprintf(pMapConfig, "spawn_ent \"%s\" \"%s\" \"%s\" \"%s\"\n", CMD_ARGV(1), CMD_ARGV(2), CMD_ARGV(3), CMD_ARGV(4));
					fclose(pMapConfig);
				}*/
				g_WriteEntityToFile = NULL;
			}
		}
	}
	else if (FStrEq(pcmd, "useent"))
	{
		if (CMD_ARGC() < 2)
		{
			ALERT(at_console, "usage: %s <targetname> <int usetype 0-4>\n", pcmd);
		}
		else
		{
			USE_TYPE ut = USE_SET;
			switch(atoi(CMD_ARGV(2)))
			{
			default: ALERT(at_console, "Invalid use type! Using USE_SET (%d).\n", USE_SET); break;
			case 0: ut = USE_OFF; break;
			case 1: ut = USE_ON; break;
			case 2: ut = USE_SET; break;
			case 3: ut = USE_TOGGLE; break;
			case 4: ut = USE_KILL; break;
			}
			FireTargets(CMD_ARGV(1), pPlayer, pPlayer, ut, 1.0);
		}
	}
	else if (FStrEq(pcmd, "getcoord"))
	{
		char str[128];
		sprintf(str, "Origin: %f %f %f\nAngles: %f %f %f\n",
			pPlayer->pev->origin.x, pPlayer->pev->origin.y, pPlayer->pev->origin.z,
			pPlayer->pev->angles.x, pPlayer->pev->angles.y, pPlayer->pev->angles.z);
		ClientPrint(pPlayer->pev, HUD_PRINTCENTER, str);
		if (CMD_ARGV(1))
		{
			mem_origin = pPlayer->pev->origin;
			mem_angles = pPlayer->pev->angles;
			remembered = true;
		}
		if (CMD_ARGV(2))
			ALERT(at_logged, str);
	}
	else if (FStrEq(pcmd, "mycoordshow"))
	{
		ALERT(at_console, "Origin: %f %f %f\tAngles: %f %f %f\n", mem_origin.x, mem_origin.y, mem_origin.z, mem_angles.x, mem_angles.y, mem_angles.z);
	}
	else if (FStrEq(pcmd, "mycoordedit"))
	{
		if (CMD_ARGC() < 4)
			ALERT(at_console, "usage: %s <origin x y z> [angles x y z]\n", pcmd);
		else
		{
			mem_origin.x = atof(CMD_ARGV(1));
			mem_origin.y = atof(CMD_ARGV(2));
			mem_origin.z = atof(CMD_ARGV(3));
			if (CMD_ARGC() == 7)
			{
				mem_angles.x = atof(CMD_ARGV(4));
				mem_angles.y = atof(CMD_ARGV(5));
				mem_angles.z = atof(CMD_ARGV(6));
				ALERT(at_console, "Coordinates saved: origin, angles\n");
			}
			else
				ALERT(at_console, "Coordinates saved: origin\n");
		}
	}
	else if (FStrEq(pcmd, "entsetcoord"))
	{
		if (remembered)
		{
			CBaseEntity *pEntity = NULL;
			if (g_pPickEntity)
			{
				pEntity = g_pPickEntity;
				ALERT(at_console, "%s: using picked entity: %s\n", pcmd, STRING(pEntity->pev->classname));
			}
			else
			{
				pEntity = UTIL_FindEntityForward(pPlayer);
				if (pEntity)
					ALERT(at_console, "%s: using entity in front: %s\n", pcmd, STRING(pEntity->pev->classname));
			}
			if (pEntity)
			{
				if (pEntity->IsPushable())
				{
					char str[128];
					sprintf(str, "Moving entity to origin: %f %f %f\nAngles: %f %f %f\n",
						mem_origin.x, mem_origin.y, mem_origin.z,
						mem_angles.x, mem_angles.y, mem_angles.z);
					ClientPrint(pPlayer->pev, HUD_PRINTCENTER, str);
					UTIL_SetOrigin(pEntity->pev, mem_origin);
					pEntity->pev->angles = mem_angles;
					ClearBits(pEntity->pev->flags, FL_ONGROUND);
//					remembered = false;
				}
				else
					ALERT(at_console, "Entiy is not pushable!\n");
			}
			else
				ALERT(at_console, "No entity picked and no entities in front of you.\n");
		}
		else
			ALERT(at_console, "Remember coordinates first.\n");

	}
	else if (FStrEq(pcmd, "dbg_dumpinfo"))
	{
		ALERT(at_console, "-------- Game: %s DumpInfo --------\n", g_pGameRules->GetGameDescription());
		g_pGameRules->DumpInfo();
		ALERT(at_console, "-------- Game::DumpInfo end --------\n");
	}
	else if (FStrEq(pcmd, "dbg_sendevent"))
	{
		PLAYBACK_EVENT_FULL(FEV_HOSTONLY, ENT(pPlayer->pev), PRECACHE_EVENT(1, CMD_ARGV(1)), 0.0,
			(float *)&pPlayer->pev->origin, (float *)&pPlayer->pev->angles,
			atof(CMD_ARGV(2)), atof(CMD_ARGV(3)),
			atoi(CMD_ARGV(4)), atoi(CMD_ARGV(5)), atoi(CMD_ARGV(6)), atoi(CMD_ARGV(7)));
	}
	else if (FStrEq(pcmd, "dbg_showll"))
	{
		ALERT(at_console, " pev->light_level = %d, GETENTITYILLUM = %d\n", pPlayer->pev->light_level, GETENTITYILLUM(ENT(pPlayer->pev)));
	}
	else if (FStrEq(pcmd, "give"))
	{
		if (g_psv_cheats->value > 0.0f)
		{
			int iszItem = ALLOC_STRING(CMD_ARGV(1));// Make a copy of the classname
			pPlayer->GiveNamedItem(STRING(iszItem));
		}
	}
	else if (FStrEq(pcmd, "allammo"))
	{
		if (g_psv_cheats->value > 0.0f)
		{
			for(int i = 0; i < MAX_AMMO_SLOTS; ++i)
				pPlayer->GiveAmmo(254, i, 254);
		}
	}
	else if (FStrEq(pcmd, "summon"))
	{
		int a = ALLOC_STRING(CMD_ARGV(1));
		CBaseEntity *pEnt = CBaseEntity::Create(STRINGV(a), pPlayer->pev->origin + gpGlobals->v_forward * 48, pPlayer->pev->angles, NULL);
		if (pEnt)
			pEnt->pev->spawnflags |= SF_NORESPAWN;
	}
	else if (FStrEq(pcmd, "spawnmass"))
	{
		int a = ALLOC_STRING(CMD_ARGV(1));
		for(int i = 0; i < atoi(CMD_ARGV(2)); ++i)
		{
			CBaseEntity *pEnt = CBaseEntity::Create(STRINGV(a), pPlayer->pev->origin + gpGlobals->v_forward * 48 *(i+1), pPlayer->pev->angles, NULL);
			if (pEnt)
				pEnt->pev->spawnflags |= SF_NORESPAWN;
		}
	}
	else if (FStrEq(pcmd, "throw"))
	{
		int a = ALLOC_STRING(CMD_ARGV(1));
		CBaseEntity *pEnt = CBaseEntity::Create(STRINGV(a), pPlayer->pev->origin + gpGlobals->v_forward * 48, pPlayer->pev->angles, NULL);
		if (pEnt)
		{
			pEnt->pev->spawnflags |= SF_NORESPAWN;
			pEnt->pev->velocity = gpGlobals->v_forward * 300.0f;
		}
	}
	else if (FStrEq(pcmd, "setkeyvalue"))
	{
		CBaseEntity *pEntity = NULL;
		if (g_pPickEntity)
		{
			pEntity = g_pPickEntity;
			ALERT(at_console, "%s: using picked entity: %s\n", pcmd, STRING(pEntity->pev->classname));
		}
		else
		{
			pEntity = UTIL_FindEntityForward(pPlayer);
			if (pEntity)
				ALERT(at_console, "%s: using entity in front: %s\n", pcmd, STRING(pEntity->pev->classname));
		}
		if (pEntity)
		{
			KeyValueData kvd;
			kvd.szClassName = STRINGV(pEntity->pev->classname);
			kvd.szKeyName = (char *)CMD_ARGV(1);
			kvd.szValue = (char *)CMD_ARGV(2);
			kvd.fHandled = FALSE;
			DispatchKeyValue(pEntity->edict(), &kvd);
		}
	}
	else if (FStrEq(pcmd, "setkvbyindex"))
	{
		CBaseEntity *pEntity = UTIL_EntityByIndex(atoi(CMD_ARGV(1)));
		if (pEntity)
		{
			KeyValueData kvd;
			kvd.szClassName = STRINGV(pEntity->pev->classname);
			kvd.szKeyName = (char *)CMD_ARGV(2);
			kvd.szValue = (char *)CMD_ARGV(3);
			kvd.fHandled = FALSE;
			DispatchKeyValue(pEntity->edict(), &kvd);
		}
	}
/*	else if (FStrEq(pcmd, "getcolormap"))
	{
		ALERT(at_console, "pev->colormap = %d\n", pPlayer->pev->colormap);
	}
	else if (FStrEq(pcmd, "showedicts"))// try using 'stats' and 'entities'
	{
		edict_t *e = NULL;
		for (int i =1; i <= gpGlobals->maxClients; ++i)
		{
			e = UTIL_ClientEdictByIndex(i);
			if (e != NULL)
				ALERT(at_console, "Found: #%d %s.\n", i, STRING(e->v.netname));
		}
	}*/
	else if (FStrEq(pcmd, "fov"))
	{
		if (CMD_ARGC() > 1)
			pPlayer->m_iFOV = atoi(CMD_ARGV(1));
		else
			CLIENT_PRINTF(ENT(pPlayer->pev), print_console, UTIL_VarArgs("server: FOV = %d\n", (int)pPlayer->m_iFOV));
	}
	else if (FStrEq(pcmd, "voicespeak"))
	{
		EMIT_SOUND(ENT(pPlayer->pev), CHAN_VOICE, CMD_ARGV(1), VOL_NORM, ATTN_NORM);
	}
	else if (FStrEq(pcmd, "hideitems"))// for overview screenshots mostly
	{
		edict_t *pEdict = INDEXENT(1);
		for (int i = 1; i < gpGlobals->maxEntities; ++i, pEdict++)
		{
			if (!UTIL_IsValidEntity(pEdict))
				continue;

			if (!strnicmp(STRING(pEdict->v.classname), "weapon_", 7) ||
				!strnicmp(STRING(pEdict->v.classname), "ammo_", 5) ||
				!strnicmp(STRING(pEdict->v.classname), "item_", 5))
			{
				if (pEdict->v.effects & EF_NODRAW)
					pEdict->v.effects &= ~EF_NODRAW;
				else
					pEdict->v.effects |= EF_NODRAW;
			}
			
		}
	}
	else if (FStrEq(pcmd, "showweapons"))
	{
		for (int i = 0; i < MAX_WEAPONS; ++i)
		{
			CBasePlayerItem *pItem = pPlayer->GetInventoryItem(i);
			if (pItem)
				pItem->PrintState();
		}
	}
	else if (FStrEq(pcmd, "showammoslots"))
	{
		ALERT(at_console, "Registered ammo types: %d\n", giAmmoIndex);
		for (int i = 0; i < MAX_AMMO_SLOTS; ++i)
		{
			if (g_AmmoInfoArray[i].name[0] == 0)// 1st char is empty
				continue;

			ALERT(at_console, "%d: %s\n", i, g_AmmoInfoArray[i].name);
		}
	}
	else if (FStrEq(pcmd, "searchents"))
	{
		if (CMD_ARGC() > 2)
		{
			int show = 0;
			if (CMD_ARGC() > 3)
				show = atoi(CMD_ARGV(3));

			CBaseEntity *pEntity = NULL;
			unsigned short count = 0;
			while ((pEntity = UTIL_FindEntityByString(pEntity, CMD_ARGV(1), CMD_ARGV(2))) != NULL)
			{
				ALERT(at_console, "Found: %d: %s %s (%s)\n", pEntity->entindex(), STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname), STRING(pEntity->pev->model));
				if (show)
				{
				Vector c = pEntity->Center();
				MESSAGE_BEGIN(MSG_ONE_UNRELIABLE/*prevent overflow*/, SVC_TEMPENTITY, pPlayer->pev->origin, ENT(pPlayer->pev));
					WRITE_BYTE(TE_BEAMENTPOINT);// BEAMENTS doesn't recognize brushes
					WRITE_SHORT(pPlayer->entindex());
					WRITE_COORD(c.x);
					WRITE_COORD(c.y);
					WRITE_COORD(c.z);
					WRITE_SHORT(g_iModelIndexBeamsAll);
					WRITE_BYTE(BLAST_SKIN_SHOCKWAVE);	// framestart
					WRITE_BYTE(0);	// framerate
					WRITE_BYTE(100);// life
					WRITE_BYTE(10);	// width
					WRITE_BYTE(0);	// noise
					WRITE_BYTE(255);// r
					WRITE_BYTE(255);// g
					WRITE_BYTE(255);// b
					WRITE_BYTE(200);// brightness
					WRITE_BYTE(10);	// speed
				MESSAGE_END();
				}
				const char *act = CMD_ARGV(4);
				if (strcmp(act, "kill") == 0)
				{
					UTIL_Remove(pEntity);
					ALERT(at_console, " - and removed;\n");
				}
				else if (strcmp(act, "show") == 0)
				{
					if (atoi(CMD_ARGV(5)) > 0)
					{
						//pEntity->pev->effects &= ~EF_NODRAW;
						pEntity->pev->effects |= EF_BRIGHTFIELD;
						ALERT(at_console, " - and highlighted;\n");
					}
					else
					{
						pEntity->pev->effects &= ~EF_BRIGHTFIELD;
						ALERT(at_console, " - and unhighlighted;\n");
					}
				}
				else if (strcmp(act, "info") == 0)
				{
					UTIL_PrintEntInfo(pEntity);
				}
				else if (strcmp(act, "use") == 0)
				{
					pEntity->Use(pPlayer, pPlayer, (USE_TYPE)atoi(CMD_ARGV(5)), 1.0);// USE_TOGGLE?
					ALERT(at_console, " - and used;\n");
				}
				else if (strcmp(act, "set") == 0)
				{
					KeyValueData kvd;
					kvd.szClassName = STRINGV(pEntity->pev->classname);
					kvd.szKeyName = (char *)CMD_ARGV(5);
					kvd.szValue = (char *)CMD_ARGV(6);
					kvd.fHandled = FALSE;
					DispatchKeyValue(pEntity->edict(), &kvd);
				}
			}
			ALERT(at_console, " %u entities total\n");
		}
		else
			ALERT(at_console, "usage: %s <entity string> <string value> [show 1/0] [action [options]]\nExample: %s classname func_button 1 use 3\n", pcmd, pcmd);
	}
	else if (FStrEq(pcmd, "searchradius"))
	{
		if (CMD_ARGC() > 1)
		{
			float radius = atof(CMD_ARGV(1));
			int show = 0;
			if (CMD_ARGC() > 2)
				show = atoi(CMD_ARGV(2));

			CBaseEntity *pEntity = NULL;
			unsigned short count = 0;
			while ((pEntity = UTIL_FindEntityInSphere(pEntity, pPlayer->Center(), radius)) != NULL)
			{
				ALERT(at_console, "Found: %d: %s %s (%s)\n", pEntity->entindex(), STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname), STRING(pEntity->pev->model));
				if (show)
				{
				Vector c = pEntity->Center();
				MESSAGE_BEGIN(MSG_ONE_UNRELIABLE/*prevent overflow*/, SVC_TEMPENTITY, pPlayer->pev->origin, ENT(pPlayer->pev));
					WRITE_BYTE(TE_BEAMENTPOINT);// BEAMENTS doesn't recognize brushes
					WRITE_SHORT(pPlayer->entindex());
					WRITE_COORD(c.x);
					WRITE_COORD(c.y);
					WRITE_COORD(c.z);
					WRITE_SHORT(g_iModelIndexBeamsAll);
					WRITE_BYTE(BLAST_SKIN_SHOCKWAVE);	// framestart
					WRITE_BYTE(0);	// framerate
					WRITE_BYTE(100);// life
					WRITE_BYTE(10);	// width
					WRITE_BYTE(0);	// noise
					WRITE_BYTE(255);// r
					WRITE_BYTE(255);// g
					WRITE_BYTE(255);// b
					WRITE_BYTE(200);// brightness
					WRITE_BYTE(10);	// speed
				MESSAGE_END();
				}
			}
			ALERT(at_console, " %u entities total\n");
		}
		else
			ALERT(at_console, "usage: %s <radius> [show 1/0]\n", pcmd);
	}
	else if (FStrEq(pcmd, "createplayerstart"))
	{
		if (/*g_psv_cheats->value > 0.0f && */pPlayer->IsAdministrator())// XDM3034: TODO: UNDONE: !!!
		{
			Vector org, ang;
			CBaseEntity *pStart = NULL;
			if (CMD_ARGC() > 1 && StringToVec(CMD_ARGV(1), org))
			{
				if (CMD_ARGC() < 2 || !StringToVec(CMD_ARGV(2), ang))
					ang = g_vecZero;
//				pStart = CBaseEntity::Create("info_player_deathmatch", org, ang);
//					ALERT(at_console, "DM start created at %g %g %g\n", org.x, org.y, org.z);
			}
			else
			{
				org = pPlayer->pev->origin;
				ang = pPlayer->pev->angles;
			}
			pStart = CBaseEntity::Create("info_player_deathmatch", org, ang);// g_pSpotList[g_usSpawnPointType].classname
			if (pStart)
			{
				ALERT(at_console, "DM start saved at:\n %g %g %g %g %g %g\n", org.x, org.y, org.z, ang.x, ang.y, ang.z);
				ENTCONFIG_WriteEntity(pStart->pev);
			}
		}
		else
			ALERT(at_console, "%s not allowed\n", pcmd);
	}
	else
		return false;

	return true;
}
