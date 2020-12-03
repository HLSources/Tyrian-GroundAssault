//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "maprules.h"
#include "game.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "round_gamerules.h"
#include "lms_gamerules.h"
#include "globals.h"
#include "pm_shared.h"// observer modes


//-----------------------------------------------------------------------------
// Purpose: Initialize HUD (client data) for a client
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CGameRulesLMS::InitHUD(CBasePlayer *pPlayer)
{
	CGameRulesRoundBased::InitHUD(pPlayer);
}

//-----------------------------------------------------------------------------
// Purpose: A player got killed, run logic
// Input  : *pVictim - 
//			*pKiller - 
//			*pInflictor - 
//-----------------------------------------------------------------------------
void CGameRulesLMS::PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	CGameRulesMultiplay::PlayerKilled(pVictim, pKiller, pInflictor);

	int limit = (int)mp_fraglimit.value;
	if (pVictim->m_iDeaths >= limit)
		pVictim->StartObserver(pVictim->pev->origin, pVictim->pev->angles, OBS_CHASE_FREE, NULL);
}

//-----------------------------------------------------------------------------
// Purpose: Get player with the best score (least deaths)
// Input  : team - 
// Output : CBasePlayer
//-----------------------------------------------------------------------------
CBasePlayer *CGameRulesLMS::GetBestPlayer(TEAM_ID team)
{
	int mindeaths = 65535;
	int limit = GetScoreLimit();//(int)mp_fraglimit.value;
	int best_player_index = 0;
	CBasePlayer *pPlayer = NULL;
	CBasePlayer *pBestPlayer = NULL;
	m_iLastCheckedNumActivePlayers = 0;
	m_iLastCheckedNumFinishedPlayers = 0;
	m_iRemainingScore = 0;
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)
		{
			if (pPlayer->IsObserver())// XDM3036
				continue;// this must be a non-active player

			if (pPlayer->m_iDeaths < limit)
			{
				++m_iLastCheckedNumActivePlayers;
				m_iRemainingScore += (limit - pPlayer->m_iDeaths);// XDM3036
				if (pPlayer->m_iDeaths < mindeaths)
				{
					mindeaths = pPlayer->m_iDeaths;
					pBestPlayer = pPlayer;
					best_player_index = pBestPlayer->entindex();
				}
			}
			else
				++m_iLastCheckedNumFinishedPlayers;
		}
	}
	if (mindeaths == 65535)// nobody earned a single point, no winners.
		return NULL;

	return pBestPlayer;
}

//-----------------------------------------------------------------------------
// Purpose: Check if the game has reached one of its limits
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CGameRulesLMS::CheckLimits(void)
{
	CBasePlayer *pBestPlayer = g_pGameRules->GetBestPlayer(g_pGameRules->GetBestTeam());
	if (pBestPlayer == NULL)
		return false;

	if (m_iLastCheckedNumActivePlayers == 1)// winner
		return true;

	int best_player_index = pBestPlayer->entindex();
	if (m_iLeader > 0 && m_iLeader != best_player_index && pBestPlayer->pev->frags > 0)
	{
		m_iLeader = best_player_index;
		if (pBestPlayer->pev->frags > 1)// XDM3035a: don't interrupt "first score" announcement
		{
		MESSAGE_BEGIN(MSG_BROADCAST, gmsgGREvent);
			WRITE_BYTE(GAME_EVENT_TAKES_LEAD);
			WRITE_SHORT(m_iLeader);
			WRITE_SHORT(pBestPlayer->pev->team);
		MESSAGE_END();
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Score limit for this game type
//-----------------------------------------------------------------------------
/*int CGameRulesLMS::GetScoreLimit(void)
{
	return CGameRulesMultiplay::GetScoreLimit();// use fraglimit value
}*/
