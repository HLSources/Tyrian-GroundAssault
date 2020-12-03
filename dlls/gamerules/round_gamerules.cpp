//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "game.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "round_gamerules.h"
#include "maprules.h"
#include "globals.h"
#include "pm_shared.h"// observer modes
#include "voice_gamemgr.h"

extern CVoiceGameMgr	g_VoiceGameMgr;


// UNDONE

// can't make these private :(
cvar_t mp_roundminplayers	= {"mp_roundminplayers","2",	FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t mp_rounds			= {"mp_rounds",			"2",	FCVAR_SERVER | FCVAR_EXTDLL};
cvar_t mp_roundtime			= {"mp_roundtime",		"15",	FCVAR_SERVER | FCVAR_EXTDLL};


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CGameRulesRoundBased::CGameRulesRoundBased() : CGameRulesTeamplay()
{
// possible way to clearly reset all entities is to reload the map using "m_bPersistBetweenMaps" flag to keep this game rules instance
//	m_bPersistBetweenMaps = false;

	m_fRoundStartTime = 0.0f;
	m_iRoundsCompleted = 0;
	m_iRoundState = ROUND_STATE_WAITING;
/*	m_cvRoundTime.name = "mp_roundtime";
	m_cvRoundTime.string = "10";
	m_cvRoundTime.flags = FCVAR_SERVER | FCVAR_EXTDLL;
	m_cvRoundTime = {"mp_roundtime",	"1",			FCVAR_SERVER | FCVAR_EXTDLL};
*/
	CVAR_REGISTER(&mp_roundtime);
	CVAR_REGISTER(&mp_roundminplayers);
}

//-----------------------------------------------------------------------------
// Purpose: First thing called after constructor. Initialize all data, cvars, etc.
//-----------------------------------------------------------------------------
void CGameRulesRoundBased::Initialize(void)
{
	m_fRoundStartTime = 0.0f;
	m_flIntermissionEndTime = 0.0f;
	m_iRoundState = ROUND_STATE_WAITING;

	if (m_bPersistBetweenMaps)// this instance was transferred from previous round on the same map
	{
/*		if (m_iRoundsCompleted > 0)
		{
			m_bSwitchTeams = true;// NO! because it'll make players shift team every time they spawn!
		}*/
	}
	CGameRulesTeamplay::Initialize();
}

//-----------------------------------------------------------------------------
// Purpose: Useless, because entvars will be cleared after this
// Input  : *pEntity - 
//			*pszName - 
//			*pszAddress - 
//			szRejectReason[128] - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
/*bool CGameRulesRoundBased::ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{
	bool ret = CGameRulesTeamplay::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
	if (ret)// don't allow players to spawn
	{
		pEntity->v.flags |= FL_SPECTATOR;// XDM3035a: mark to spawn as spectator
// Entity data does not exist yet!
// NO!		CBaseEntity *pSpawnSpot = EntSelectSpectatorPoint(NULL);
//		pPlayer->StartObserver(pSpawnSpot->pev->origin, pSpawnSpot->pev->angles);
	}
	return ret;
}*/

//-----------------------------------------------------------------------------
// Purpose: client has been disconnected
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
/*void CGameRulesRoundBased::ClientDisconnected(CBasePlayer *pPlayer)
{
	CGameRulesTeamplay::ClientDisconnected(pPlayer);
}*/

//-----------------------------------------------------------------------------
// Purpose: Initialize client HUD
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
/*void CGameRulesRoundBased::InitHUD(CBasePlayer *pPlayer)
{
	CGameRulesTeamplay::InitHUD(pPlayer);
}*/

//-----------------------------------------------------------------------------
// Purpose: Runs every server frame, should handle any timer tasks, periodic events, etc.
//-----------------------------------------------------------------------------
void CGameRulesRoundBased::StartFrame(void)
{
	if (m_iRoundState == ROUND_STATE_WAITING)
	{
		int c = CountPlayers();
//		if (c >= gpGlobals->maxClients)
//			m_bReadyButtonsHit = true;// game starts when the server becomes full

		if (c > 1)// don't start if there's only one client
		{
			if (m_bReadyButtonsHit == false)// XDM3036: now all players must press ready
				m_bReadyButtonsHit = CheckPlayersReady();

			if (m_bReadyButtonsHit == false)
				if (m_flIntermissionEndTime != 0.0f && m_flIntermissionEndTime < gpGlobals->time)
					m_bReadyButtonsHit = true;
		}
		if (m_bReadyButtonsHit)
		{
			m_bReadyButtonsHit = false;
			m_flIntermissionEndTime = 0.0f;
			RoundStart();
		}
		else
			g_VoiceGameMgr.Update(gpGlobals->frametime);// don't run parent::StartFrame()
	}
	else if (m_iRoundState == ROUND_STATE_ACTIVE)
	{
		if ((m_fRoundStartTime != 0.0f) && gpGlobals->time >= (m_fRoundStartTime + mp_roundtime.value))
			EndMultiplayerGame();//RoundEnd();
		else
			CGameRulesTeamplay::StartFrame();// CheckLimits() is there
	}
	else
		CGameRulesTeamplay::StartFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Disallow damage during round change, intermission, etc.
// Input  : *pPlayer -
//			*pAttacker -
// Output : Returns TRUE if can
//-----------------------------------------------------------------------------
bool CGameRulesRoundBased::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (m_iRoundState == ROUND_STATE_ACTIVE)
		return CGameRulesTeamplay::FPlayerCanTakeDamage(pPlayer, pAttacker);

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: A player is spawning
// Input  : *pPlayer -
//-----------------------------------------------------------------------------
void CGameRulesRoundBased::PlayerSpawn(CBasePlayer *pPlayer)
{
	if (pPlayer->m_iSpawnState == 0)// first time
	{
		if (m_flIntermissionEndTime == 0.0f)// Somebody joined first. Start counting time to round start.
			m_flIntermissionEndTime = gpGlobals->time + mp_chattime.value;// TODO: check limits?

		if (m_iRoundState == ROUND_STATE_WAITING)// we have list to choose from // bots can't select team
		{
			pPlayer->pev->team = TEAM_NONE;
//called from RemoveAllItems			g_pGameRules->InitHUD(pPlayer);// send team list to this client (UpdateClientData() will not get called for spectator)
			CBaseEntity *pSpawnSpot = SpawnPointEntSelectSpectator(NULL);
			if (pSpawnSpot)
				pPlayer->StartObserver(pSpawnSpot->pev->origin, pSpawnSpot->pev->angles, OBS_ROAMING, NULL);
			else
				pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles, OBS_ROAMING, NULL);

			if (!pPlayer->IsBot())
			{
			MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, NULL, ENT(pPlayer->pev));// dest: all
				WRITE_BYTE(MENU_TEAM);
				WRITE_BYTE(0);
			MESSAGE_END();
//			CLIENT_COMMAND(pPlayer->edict(), "chooseteam\n");
			}
		}
		else
			CGameRulesTeamplay::PlayerSpawn(pPlayer);
	}
	else// not first time
	{
		if (m_iRoundState == ROUND_STATE_ACTIVE)// spawn once
		{
			CGameRulesMultiplay::PlayerSpawn(pPlayer);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: RoundStart
//-----------------------------------------------------------------------------
void CGameRulesRoundBased::RoundStart(void)
{
	if (m_iRoundState == ROUND_STATE_ACTIVE)
		return;

	m_iRoundState = ROUND_STATE_SPAWNING;

	// TODO: use save/restore mechanism to reset all entities to their initial state each round
/*	if (m_iRoundsCompleted == 0) well, we'd better just keep instance of this rules class and just restart the map ;)
		Game_Save();
	else
		Game_Restore();*/

	FireTargets("game_roundstart", g_pWorld, g_pWorld, USE_TOGGLE, 0);

	CBasePlayer *pPlayer = NULL;
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)
		{
			if (m_iRoundsCompleted > 0)// this map was played at least once
			{
				// shift team indexes for all players or entities?
				// Players and teams must have all their scores saved!
				pPlayer->pev->team++;
				if (pPlayer->pev->team > GetNumberOfTeams())
					pPlayer->pev->team = TEAM_1;
			}
			pPlayer->StopObserver();// Spawn() is here
		}
	}
	m_fRoundStartTime = gpGlobals->time;
	m_iRoundState = ROUND_STATE_ACTIVE;

	MESSAGE_BEGIN(MSG_BROADCAST, gmsgGREvent);
		WRITE_BYTE(GAME_EVENT_ROUND_START);
		WRITE_SHORT(m_iRoundsCompleted);
		WRITE_SHORT(0);// round time?
	MESSAGE_END();

	if (sv_lognotice.value > 0)// XDM
	{
		UTIL_LogPrintf("Round %d start\n", m_iRoundsCompleted);
	}
}

//-----------------------------------------------------------------------------
// Purpose: RoundEnd (local intermission)
//-----------------------------------------------------------------------------
void CGameRulesRoundBased::RoundEnd(void)
{
	if (m_iRoundState == ROUND_STATE_FINISHED)
		return;

	m_iRoundState = ROUND_STATE_FINISHED;

	// TODO: stop, disable damage, show scoreboard

	MESSAGE_BEGIN(MSG_BROADCAST, gmsgGREvent);
		WRITE_BYTE(GAME_EVENT_ROUND_END);
		WRITE_SHORT(m_iRoundsCompleted);
		WRITE_SHORT(0);// round time?
	MESSAGE_END();

	FireTargets("game_roundend", g_pWorld, g_pWorld, USE_TOGGLE, 0);

	if (sv_lognotice.value > 0)// XDM
	{
		UTIL_LogPrintf("Round %d end\n", m_iRoundsCompleted);
	}
	m_iRoundsCompleted++;


}

//-----------------------------------------------------------------------------
// Purpose: User-defined number of rounds to play on this map.
// Can be based on team switching, number of objectives, etc.
// Output : int 0 == unlimited
//-----------------------------------------------------------------------------
short CGameRulesRoundBased::GetRoundsLimit(void)
{
	return (int)(mp_rounds.value);
//	return MaxTeams();
}


//-----------------------------------------------------------------------------
// Purpose: Overridden. Called when limits are reached.
//-----------------------------------------------------------------------------
void CGameRulesRoundBased::EndMultiplayerGame(void)
{
	RoundEnd();
	CGameRulesTeamplay::EndMultiplayerGame();// GoToIntermission() and stuff
}

//-----------------------------------------------------------------------------
// Purpose: Overridden. Parent CGameRules will call this after intermission
// and we must catch it and decide if it's end of game, or just end of round.
//-----------------------------------------------------------------------------
void CGameRulesRoundBased::ChangeLevel(void)
{
	// TODO: reload current map without destroying this instance of game rules class
	if (m_iRoundsCompleted < GetRoundsLimit())
	{
		m_bPersistBetweenMaps = true;// keep instance of this
		CHANGE_LEVEL(STRINGV(gpGlobals->mapname), NULL);// restart level
	}
	else
	{
		m_bPersistBetweenMaps = false;
//		endless loop g_pGameRules->EndMultiplayerGame();
		TEAM_ID bestteam = g_pGameRules->GetBestTeam();
		GoToIntermission(g_pGameRules->GetBestPlayer(bestteam), NULL/*m_pLastVictim may contain random person*/);
	}
}
