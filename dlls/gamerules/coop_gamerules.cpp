//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "gamerules.h"
#include "coop_gamerules.h"
#include "globals.h"
#include "game.h"
#include "monsters.h"

#define landmark euser4

// TODO: this code is raw and does not meet the beta version requirements!

//-----------------------------------------------------------------------------
// Purpose: constructor
//
// TODO: disable standard multiplayer level change mechanism, use triggers to
//  stop players. When all players reach trigger_changelevel, server changes
//  the map
//-----------------------------------------------------------------------------
CGameRulesCoOp::CGameRulesCoOp() : CGameRulesMultiplay()
{
	m_bPersistBetweenMaps = true;// XDM3035c: collect data between levels
	m_pFirstPlayer = NULL;
	m_pEntLandmark = NULL;
	m_iRegisteredTargets = 0;
	m_iChangeLevelTriggers = 0;
	m_iLastCheckedNumActivePlayers = 0;
	m_iLastCheckedNumFinishedPlayers = 0;

	memset(m_szLastMap, 0, MAX_MAPNAME);
	memset(m_szNextMap, 0, MAX_MAPNAME);

//	cvar_t m_cvEndOfLevelSpectate = {"mp_coop_eol_spectate",	"1",			FCVAR_SERVER | FCVAR_EXTDLL};
//	CVAR_REGISTER(&m_cvEndOfLevelSpectate);
/* BAD way to do things!
	// Prepare ALL entities for the game
	int entindex = 1;
	edict_t *pEdict = NULL;
	do
	{
		pEdict = INDEXENT(entindex);
		++entindex;
		if (pEdict)
		{
			if (UTIL_IsValidEntity(pEdict))
			{
				CBaseEntity *pEntity = NULL;
				pEntity = CBaseEntity::Instance(pEdict);
				if (pEntity)
				{
					if (pEntity->IsMonster())
					{
						if (mp_monstersrespawn.value > 0.0f)
							pEntity->pev->spawnflags &= ~SF_NORESPAWN;
						else
							pEntity->pev->spawnflags |= SF_NORESPAWN;
					}
					else if (pEntity->IsTrigger())
					{
						if (FClassnameIs(pEntity->pev, "trigger_changelevel"))
						{
							if (pEntity->pev->solid != SOLID_NOT)// is valid
							{
								++m_iChangeLevelTriggers;
								SERVER_PRINT(UTIL_VarArgs("* Registered %s %s\n", STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname)));
							}
						}
					}
				}
			}
		}
	} while (entindex < gpGlobals->maxEntities);
*/
//	CGameRulesMultiplay();

/*	too early! if (mp_monstersrespawn.value > 0.0f)
		m_iGameMode = COOP_MODE_MONSTERFRAGS;
	else if (m_iChangeLevelTriggers > 0)
		m_iGameMode = COOP_MODE_LEVEL;
	else
		m_iGameMode = COOP_MODE_SWEEP;*/
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CGameRulesCoOp::~CGameRulesCoOp()
{
// will be called after this.	CGameRulesMultiplay::~CGameRulesMultiplay();
	m_iChangeLevelTriggers = 0;
//	memset(m_szLastMap, 0, MAX_MAPNAME);
//	memset(m_szNextMap, 0, MAX_MAPNAME);
	m_RegisteredTargets.clear();
	m_iRegisteredTargets = 0;
	m_pEntLandmark = NULL;
	m_pFirstPlayer = NULL;
}


//-----------------------------------------------------------------------------
// Purpose: Sent initialization messages to client
// Input  : *pPlayer - client
//-----------------------------------------------------------------------------
void CGameRulesCoOp::InitHUD(CBasePlayer *pPlayer)
{
	CGameRulesMultiplay::InitHUD(pPlayer);
//	ALERT(at_console, " ---- WARNING!! CoOperative mode is NOT implemented! ----\n This is an ordinary DM with monsterfrags!\n");
/* CL
	if (m_iGameMode == COOP_MODE_MONSTERFRAGS)
		ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#COOP_INTRO_MON");//"--- CoOperative monster hunting! ---\n\nGet more monsterfrags!");
	else if (m_iGameMode == COOP_MODE_LEVEL)
		ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#COOP_INTRO_LVL");//"--- CoOperative level playing! ---\n\nAll players must reach the end of level!");
	else // if (m_iGameMode == COOP_MODE_SWEEP)
		ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#COOP_INTRO_SWP");//"--- CoOperative monster sweeping! ---\n\nClear this level off monsters!");
*/
}

//-----------------------------------------------------------------------------
// Purpose: Server is activated, all entities spawned
// Input  : *pEdictList - 
//			edictCount - 
//			clientMax - 
//-----------------------------------------------------------------------------
void CGameRulesCoOp::ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	// good place to choose mode based on specific entities
	if (mp_monstersrespawn.value > 0.0f)
		m_iGameMode = COOP_MODE_MONSTERFRAGS;
	else if (m_iChangeLevelTriggers > 0 && mp_coop_usemaptransition.value > 0.0f)
		m_iGameMode = COOP_MODE_LEVEL;
	else
		m_iGameMode = COOP_MODE_SWEEP;
}

//-----------------------------------------------------------------------------
// Purpose: First thing called after constructor. Initialize all data, cvars, etc.
//-----------------------------------------------------------------------------
void CGameRulesCoOp::Initialize(void)
{
	m_iChangeLevelTriggers = 0;
	memset(m_szLastMap, 0, MAX_MAPNAME);
	memset(m_szNextMap, 0, MAX_MAPNAME);
	m_iLastCheckedNumActivePlayers = 0;
	m_iLastCheckedNumFinishedPlayers = 0;
	m_RegisteredTargets.clear();
	m_iRegisteredTargets = 0;
	m_pEntLandmark = NULL;
	m_pFirstPlayer = NULL;
	CGameRulesMultiplay::Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: Runs every server frame, should handle any timer tasks, periodic events, etc.
//-----------------------------------------------------------------------------
void CGameRulesCoOp::StartFrame(void)
{
	// frag/time limit/etc
	CGameRulesMultiplay::StartFrame();

	if (!IsGameOver())
	{
		if ((m_iGameMode == COOP_MODE_LEVEL && CheckPlayersTouchedTriggers(true)) ||// checks if all players touched trigger
			(m_iGameMode == COOP_MODE_SWEEP && m_iRegisteredTargets > 0 && m_RegisteredTargets.size() <= 0))
		{
			GoToIntermission(g_pGameRules->GetBestPlayer(TEAM_NONE), m_pLastVictim);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: How many points awarded to anyone that kills this entity
// Input  : *pAttacker - player/monster
//			*pKilled - player/monster
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesCoOp::IPointsForKill(CBaseEntity *pAttacker, CBaseEntity *pKilled)
{
	if (pAttacker->IsPlayer() && pKilled->IsPlayer())
		return -1;

	if (pKilled->IsMonster())
	{
//		if (pKilled->MyMonsterPointer()->IRelationship(pAttacker) > R_NO)// may change dynamically, which is bad...
//		int r = g_iRelationshipTable[pKilled->Classify()][pAttacker->Classify()];// XDM3035b: use base relationship, not dynamic
		int r = g_iRelationshipTable[pAttacker->Classify()][pKilled->Classify()];// works better for barnacles
		if (r >= R_DL)// enemy
			return 1;
		else if (r == R_AL)// ally
			return -1;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: A player got killed
// Input  : *pVictim - 
//			*pKiller - 
//			*pInflictor - 
//-----------------------------------------------------------------------------
void CGameRulesCoOp::PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	CGameRulesMultiplay::PlayerKilled(pVictim, pKiller, pInflictor);
}

//-----------------------------------------------------------------------------
// Purpose: A monster got killed
// Input  : *pVictim - 
//			*pKiller - 
//			*pInflictor - 
//-----------------------------------------------------------------------------
void CGameRulesCoOp::MonsterKilled(CBaseMonster *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	ASSERT(pVictim != NULL);
//	ALERT(at_console, " +++ %s WAS KILLED +++\n", STRING(pVictim->pev->classname));

//?	m_pLastVictim = pVictim;
//	pVictim->m_iDeaths++;

	DeathNotice(pVictim, pKiller, pInflictor);
	FireTargets("game_monsterdie", pVictim, pVictim, USE_TOGGLE, 0);

	CBasePlayer *pPlayerKiller = NULL;
	CBaseMonster *pMonsterKiller = NULL;
	int iKiller = 0;
	int iVictim = pVictim->entindex();

	if (pKiller)
	{
		iKiller = pKiller->entindex();
		if (pKiller->IsPlayer())
			pPlayerKiller = (CBasePlayer *)pKiller;
// both can be true		else
			pMonsterKiller = pKiller->MyMonsterPointer();

		// pVictim was killed by the same entity twice!
		if (pVictim->m_iLastKiller == iKiller)// is not NULL already so don't need to check
		{
//no			if (pVictim->m_iLastScoreAward == 0)// pVictim did not kill anyone after respawning
			{
//			MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, gmsgGREvent, NULL, pVictim->edict());// send to victim
			MESSAGE_BEGIN(MSG_BROADCAST, gmsgGREvent);// let everyone laugh?
				WRITE_BYTE(GAME_EVENT_LOOSECOMBO);
				WRITE_SHORT(iKiller);//hard to use this for monsters because of indexes and lack of information on client side
				WRITE_SHORT(iVictim);
			MESSAGE_END();
			}
// multiple?			pVictim->m_hLastKiller = NULL;
		}
		else
			pVictim->m_iLastKiller = iKiller;// remember new killer

		if (pMonsterKiller)// must be valid for monsters AND players
			pMonsterKiller->m_iLastVictim = iVictim;

		if (pVictim->pev == pKiller->pev)// killed self
		{
			AddScore(pKiller, -1);
		}
		else if (pPlayerKiller && !pPlayerKiller->IsObserver())// && pKiller->IsPlayer()) // XDM3035: don't award spectators!
		{
			// if a player dies in a deathmatch game and the killer is a client, award the killer some points
			AddScore(pKiller, IPointsForKill(pKiller, pVictim));// +1
			FireTargets("game_monsterkill", pKiller, pKiller, USE_TOGGLE, 0);
		}
		else  if (pKiller->IsMonster())// monster kills another monster
		{
			AddScore(pKiller, IPointsForKill(pKiller, pVictim));
		}
		else// killed by the world
		{
			pVictim->m_iLastKiller = 0;
			AddScore(pVictim, -1);
		}
	}
//	AddScore(pKiller, IPointsForKill(pKiller, pVictim));
//	CGameRulesMultiplay::MonsterKilled(pVictim, pKiller, pInflictor);

	if (m_iGameMode == COOP_MODE_SWEEP && m_iRegisteredTargets > 0)
	{
		CBaseEntity *pSearch = NULL;
		for (m_TargetIterator = m_RegisteredTargets.begin(); m_TargetIterator != m_RegisteredTargets.end(); ++m_TargetIterator)
		{
			pSearch = *m_TargetIterator;
			if (pSearch == pVictim)
			{
				m_RegisteredTargets.erase(m_TargetIterator);
//	^			if (m_iGameMode == COOP_MODE_SWEEP)
				{
					m_iRemainingScore = m_RegisteredTargets.size();
//					ALERT(at_aiconsole, "CGameRulesCoOp: remaining monsters: %d of %d\n", m_iRemainingScore, m_iRegisteredTargets);
				}
				break;
			}
		}
	}
//	if (pSearch != pVictim) {}// not found

	// update the scores
	// killed scores
/* monsters aren't shown on score board
	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
		WRITE_BYTE(ENTINDEX(pVictim->edict()));
		WRITE_SHORT((int)pVictim->pev->frags);
		WRITE_SHORT(pVictim->m_iDeaths);
	MESSAGE_END();*/

	// killers score, if it's a player
	if (pPlayerKiller)
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
			WRITE_BYTE(ENTINDEX(pPlayerKiller->edict()));
			WRITE_SHORT((int)pPlayerKiller->pev->frags);
			WRITE_SHORT(pPlayerKiller->m_iDeaths);
		MESSAGE_END();
		// let the killer paint another decal as soon as he'd like.
		pPlayerKiller->m_flNextDecalTime = gpGlobals->time;
	}

	CheckLimits();
}

//-----------------------------------------------------------------------------
// Purpose: Entity restrictions may apply here, register monsters
// Input  : *pEntity - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesCoOp::IsAllowedToSpawn(CBaseEntity *pEntity)
{
	if (pEntity->IsMonster() && !pEntity->IsProjectile())
	{
		if (mp_monstersrespawn.value > 0.0f)
			pEntity->pev->spawnflags &= ~SF_NORESPAWN;
		else
			pEntity->pev->spawnflags |= SF_NORESPAWN;

		// CONSIDER REVISIT: use of IRelationship is ureliable beccause it is dynamic
		if (g_iRelationshipTable[pEntity->Classify()][CLASS_PLAYER] > R_NO)// dislike or hate players
		{
			if (!(pEntity->pev->flags & FL_GODMODE) && !(pEntity->pev->spawnflags & SF_MONSTER_INVULNERABLE))// don't register those who we cannot kill
			{
				m_RegisteredTargets.push_back(pEntity);
				ALERT(at_aiconsole, "CGameRulesCoOp: registered hostile monster: %s %s (#%d)\n", STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname), m_iRegisteredTargets);
				++m_iRegisteredTargets;
			}
		}
	}
	else if (pEntity->IsTrigger())
	{
		if (FClassnameIs(pEntity->pev, "trigger_changelevel"))
		{
			if (pEntity->pev->solid != SOLID_NOT)// is valid
			{
				SERVER_PRINT(UTIL_VarArgs("* Registered %s %s (#%d)\n", STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname), m_iChangeLevelTriggers));
//				strcpy(m_szNextMap, szNextMap);
				++m_iChangeLevelTriggers;
			}
		}
	}

	return CGameRulesMultiplay::IsAllowedToSpawn(pEntity);
}

//-----------------------------------------------------------------------------
// Purpose: Are players allowed to switch to spectator mode in game? (cheat!)
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesCoOp::FAllowSpectatorChange(CBasePlayer *pPlayer)
{
	if (m_iGameMode == COOP_MODE_LEVEL && mp_coop_eol_spectate.value > 0.0f &&
		pPlayer->pev->landmark == m_pEntLandmark && !pPlayer->IsObserver())// this player finished the level, AND is not trying to join the game again
		return true;
	else
		return CGameRulesMultiplay::FAllowSpectatorChange(pPlayer);
}

//-----------------------------------------------------------------------------
// Purpose: Always transfer my instance to the next map if playing campaign
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CGameRulesCoOp::FPersistBetweenMaps(void)
{
	if (m_iGameMode == COOP_MODE_LEVEL)
		return true;

	return CGameRulesMultiplay::FPersistBetweenMaps();
}

//-----------------------------------------------------------------------------
// Purpose: Can this player take damage from this attacker?
// Input  : *pPlayer - 
//			*pAttacker - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesCoOp::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (pAttacker && pAttacker->IsPlayer())
	{
		if ((mp_friendlyfire.value <= 0.0f) && (pAttacker != pPlayer))
			return FALSE;
	}
	return CGameRulesMultiplay::FPlayerCanTakeDamage(pPlayer, pAttacker);
}

//-----------------------------------------------------------------------------
// Purpose: Relationship determines score assignment and other important stuff
// Input  : *pPlayer - 
//			*pTarget - 
// Output : int GR_NEUTRAL
//-----------------------------------------------------------------------------
int CGameRulesCoOp::PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget)
{
	if (pTarget->IsPlayer())
		return GR_ALLY;
	else if (pTarget->IsMonster())
		return GR_ENEMY;
	else
		return CGameRulesMultiplay::PlayerRelationship(pPlayer, pTarget);
}

//-----------------------------------------------------------------------------
// Purpose: Score limit for this game type
//-----------------------------------------------------------------------------
int CGameRulesCoOp::GetScoreLimit(void)
{
	if (m_iGameMode == COOP_MODE_SWEEP)// && m_iRegisteredTargets > 0)
		return m_iRegisteredTargets;
	else if (m_iGameMode == COOP_MODE_LEVEL)
		return m_iLastCheckedNumActivePlayers;
	else
		return CGameRulesMultiplay::GetScoreLimit();
}

//-----------------------------------------------------------------------------
// Purpose: How much score remaining until end of game
//-----------------------------------------------------------------------------
int CGameRulesCoOp::GetScoreRemaining(void)
{
	if (m_iGameMode == COOP_MODE_LEVEL)
		return (m_iLastCheckedNumActivePlayers - m_iLastCheckedNumFinishedPlayers);// XDM3036
	else
		return CGameRulesMultiplay::GetScoreRemaining();// returns m_iRemainingScore which holds right values for COOP_MODE_SWEEP and COOP_MODE_MONSTERFRAGS
}

//-----------------------------------------------------------------------------
// Purpose: Get player with the best score (or first got to the end of level)
// Input  : team - may be TEAM_NONE
// Output : CBasePlayer *
//-----------------------------------------------------------------------------
CBasePlayer *CGameRulesCoOp::GetBestPlayer(TEAM_ID team)
{
	if (m_iGameMode == COOP_MODE_LEVEL && mp_coop_eol_firstwin.value > 0.0f)
	{
		if (m_pFirstPlayer)
			return m_pFirstPlayer;
	}
	return CGameRulesMultiplay::GetBestPlayer(team);
}

//-----------------------------------------------------------------------------
// Purpose: trigger_changelevel was touched
//-----------------------------------------------------------------------------
bool CGameRulesCoOp::FAllowLevelChange(CBasePlayer *pActivator, char *szNextMap, edict_t *pEntLandmark)
{
	if (m_iGameMode != COOP_MODE_LEVEL)
		return false;

	if (m_szNextMap[0] == 0)
	{
		strcpy(m_szNextMap, szNextMap);// TODO: invent a better mechanism
		UTIL_ClientPrintAll(HUD_PRINTCENTER, "#COOP_SETNEXTMAP", m_szNextMap);
		m_pEntLandmark = pEntLandmark;// must be set before spectating
	}

	if (pActivator->pev->landmark != pEntLandmark)
	{
		pActivator->pev->landmark = pEntLandmark;
//		EMIT_SOUND(ENT(pev), CHAN_STATIC, "game/dom_touch.wav", VOL_NORM, ATTN_NORM);
		UTIL_EmitAmbientSound(ENT(pActivator->pev), pEntLandmark->v.origin, "game/dom_touch.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
//cl		ClientPrint(pActivator->pev, HUD_PRINTCENTER, "#COOP_TRIGGER_TOUCH");//"- Congratulations, end of level! -\n\nAll players must touch it to complete this map\n");
		MESSAGE_BEGIN(MSG_BROADCAST, gmsgGREvent);
			WRITE_BYTE(GAME_EVENT_COOP_PLAYER_FINISH);
			WRITE_SHORT(pActivator->entindex());
			WRITE_SHORT(pActivator->pev->team);
		MESSAGE_END();
		if (g_pGameRules->FAllowEffects())// XDM
			PLAYBACK_EVENT_FULL(FEV_RELIABLE, pActivator->edict(), g_usTeleport, 0.0, pActivator->pev->origin.As3f(), pActivator->pev->origin.As3f(), 0.0, 0.0, pActivator->pev->team, 0, 0, 1);

		if (mp_coop_eol_spectate.value > 0.0f && g_pGameRules->FAllowSpectatorChange(pActivator))
			pActivator->StartObserver(pActivator->pev->origin, pActivator->pev->angles, OBS_ROAMING, NULL);
	}
	//else// "wait for other players!"

/*	if (m_szLastMap[0])
	{
		if (stricmp(m_szLastMap, szNextMap) == 0)
		{
			// ALERT(at_aiconsole, "CGameRulesCoOp::FAllowLevelChange: tried to set previously played map as next\n");
		}
	}*/

//	bool allinplace = CheckPlayersTouchedTriggers(true);

/*
	We don't allow the trigger to chenge level itself, instead we just remember sufficent parameters
	and start intermission when ready (the map will be changed normally after that).
*/

	{
		if (m_pFirstPlayer == NULL)
			m_pFirstPlayer = pActivator;

//		pActivator->NotifyGoal();
//		pActivator->Spectate(); ?

		if (CheckPlayersTouchedTriggers(true))
		{
			for (int i = 1; i <= gpGlobals->maxClients; ++i)// move everyone to landmark before changing level
			{
				CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
				if (pPlayer)
					pPlayer->pev->origin = m_pEntLandmark->v.origin;
			}
			GoToIntermission(g_pGameRules->GetBestPlayer(TEAM_NONE), m_pLastVictim);
		}
	}
	return false;// ?
}

//-----------------------------------------------------------------------------
// Purpose: Server is changing to a new level, check mapcycle.txt for map name and setup info
// In CoOp all players touch trigger_changelevel to start intermission and THEN
// this function gets called.
//-----------------------------------------------------------------------------
void CGameRulesCoOp::ChangeLevel(void)
{
	if (m_iChangeLevelTriggers > 0 && m_iGameMode == COOP_MODE_LEVEL && IsGameOver() && IS_MAP_VALID(m_szNextMap))
	{
		m_pIntermissionEntity1 = NULL;
		m_pIntermissionEntity2 = NULL;
		strcpy(m_szLastMap, STRING(gpGlobals->mapname));// remember current map
		ALERT(at_console, "CHANGE LEVEL: %s (from trigger)\n", m_szNextMap);
		CHANGE_LEVEL(m_szNextMap, NULL);//m_pEntLandmark?STRINGV(m_pEntLandmark->v.targetname):NULL);
		// TODO: implement local save/restore mechanism (m_bPersistBetweenMaps allows to)
		// NOTE: global entities are somehow already saved!
	}
	else
	{
		CGameRulesMultiplay::ChangeLevel();
	}
}

//-----------------------------------------------------------------------------
// Check if all players touched the same one trigger!
// Purpose: Can the server change levels? (used by triggers)
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesCoOp::CheckPlayersTouchedTriggers(bool bCheckTransitionVolume)
{
	if (m_iChangeLevelTriggers > 0 && m_pEntLandmark != NULL)
	{
		m_iLastCheckedNumActivePlayers = 0;
		m_iLastCheckedNumFinishedPlayers = 0;
//		int iNotCompletedPlayers = 0;
		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
			if (pPlayer)
			{
				if (pPlayer->pev->landmark == m_pEntLandmark)// finished
				{
					++m_iLastCheckedNumFinishedPlayers;
					++m_iLastCheckedNumActivePlayers;
					continue;
				}

				if (pPlayer->IsObserver())// XDM3035c: those who finished level become spectators too, but that was checked already
					continue;// so this must be a non-active player

				++m_iLastCheckedNumActivePlayers;

//				if (pPlayer->pev->landmark != m_pEntLandmark)
//					++iNotCompletedPlayers;

/*				if (pPlayer->m_pGoalEnt == NULL)// UNDONE: check for REGISTERED triggers!
					++iNotCompletedPlayers;
				else
				{
					if (bCheckTransitionVolume)
					{
						if (!InTransitionVolume(pPlayer, STRING(m_pEntLandmark->v.targetname)))
							++iNotCompletedPlayers;
					}
				}*/
			}
		}
		if (m_iLastCheckedNumActivePlayers == 0 || m_iLastCheckedNumFinishedPlayers == 0)// during level change
			return false;

//		if (m_iLastCheckedNumActivePlayers > 0 && iNotCompletedPlayers == 0)//AllPlayersTouchedTrigger)
		if (m_iLastCheckedNumActivePlayers == m_iLastCheckedNumFinishedPlayers)
			return true;
	}
	return false;
}
