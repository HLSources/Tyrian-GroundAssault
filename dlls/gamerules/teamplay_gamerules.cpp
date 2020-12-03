//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "maprules.h"
#include "game.h"
#include "colors.h"// XDM
#include "globals.h"
#include "util_vector.h"
#include "voice_gamemgr.h"


extern CVoiceGameMgr g_VoiceGameMgr;

//-----------------------------------------------------------------------------
// Purpose: Gets team color from CVars or predefined array. Slow! Do NOT use frequently!
// Input  : team - TEAM_ID
//			&r &g &b -
//-----------------------------------------------------------------------------
void GetTeamColor(TEAM_ID team, byte &r, byte &g, byte &b)
{
	if (g_pGameRules->IsTeamplay())
	{
		if (team <= TEAM_NONE || team > MAX_TEAMS)
		{
#ifdef _DEBUG
			if (team > TEAM_NONE)
				ALERT(at_console, "GetTeamColor() bad team ID %d!\n", team);
#endif
			team = 0;
		}

		cvar_t *pCvar = NULL;

		if (team == TEAM_1)
			pCvar = &mp_teamcolor1;
		else if (team == TEAM_2)
			pCvar = &mp_teamcolor2;
		else if (team == TEAM_3)
			pCvar = &mp_teamcolor3;
		else if (team == TEAM_4)
			pCvar = &mp_teamcolor4;

		if (pCvar == NULL || !StringToRGB(pCvar->string, r,g,b))
		{
#ifdef _DEBUG
			if (team > TEAM_NONE)
				ALERT(at_console, "GetTeamColor() is using predefined value!\n");
#endif
			r = g_iTeamColors[team][0];
			g = g_iTeamColors[team][1];
			b = g_iTeamColors[team][2];
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CGameRulesTeamplay::CGameRulesTeamplay() : CGameRulesMultiplay()
{
	m_DisableDeathMessages = false;
	m_DisableDeathPenalty = false;

	// clean up
	memset(m_Teams, 0, sizeof(m_Teams));
	m_iNumTeams = 0;
	m_teamLimit = false;
	m_LeadingTeam = TEAM_NONE;

	for (int i = 0; i <MAX_TEAMS+1; ++i)// XDM3035
		m_pBaseEntities[i] = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: World is spawning. Initialize all data, cvars, etc.
// XDM: need this because g_pGameRules cannot be used from constructor
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::Initialize(void)
{
	memset(m_Teams, 0, sizeof(m_Teams));// XDM3035 ?
	m_iNumTeams = 0;// ?

	CGameRulesMultiplay::Initialize();

	CreateNewTeam(" ");// XDM: always 0th team

	char szTeamList[MAX_TEAMS*MAX_TEAMNAME_LENGTH];
	memset(szTeamList, 0, MAX_TEAMS*MAX_TEAMNAME_LENGTH);
	strncpy(szTeamList, mp_teamlist.string, MAX_TEAMS*MAX_TEAMNAME_LENGTH-1);

	char *pName = szTeamList;
	pName = strtok(pName, ";");
	// don't use just this::MaxTeams() because it always returns value for CGameRulesTeamplay!
	while (pName != NULL && *pName && m_iNumTeams <= g_pGameRules->MaxTeams())// + 1
	{
		if (GetTeamIndex(pName) <= TEAM_NONE)
			CreateNewTeam(pName);

		pName = strtok(NULL, ";");
	}

	if (m_iNumTeams > 1)// 0th team already created
	{
		m_teamLimit = true;
		if (!IsValidTeam(mp_defaultteam.string))// check defaultteam to be valid
		{
			SERVER_PRINT(UTIL_VarArgs("%s invalid ('%s'), resetting.\n", mp_defaultteam.name, mp_defaultteam.string));
			CVAR_DIRECT_SET(&mp_defaultteam, "");
		}

		if (sv_lognotice.value > 0.0f)
			UTIL_LogPrintf("Teamplay initialized, %d teams total.\n Valid teams: %s\n Default team: %s\n", m_iNumTeams, szTeamList, mp_defaultteam.string);
	}

	RecountTeams(false);
}

//-----------------------------------------------------------------------------
// Purpose: Team base, e.g. flag return point or something
// Input  : team - ID
// Output : CBaseEntity
//-----------------------------------------------------------------------------
CBaseEntity *CGameRulesTeamplay::GetTeamBaseEntity(TEAM_ID team)
{
	if (IsValidTeam(team))
		return m_pBaseEntities[team];
/*	if (IsValidTeam(team))
	{
	}*/
	return NULL;// g_pWorld?
}

//-----------------------------------------------------------------------------
// Purpose: Set team base, e.g. flag return point or something
// Input  : team - ID
//			*pEntity -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::SetTeamBaseEntity(TEAM_ID team, CBaseEntity *pEntity)
{
	if (IsValidTeam(team) && UTIL_IsValidEntity(pEntity))
	{
		m_pBaseEntities[team] = pEntity;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Runs every server frame, should handle any timer tasks, periodic events, etc.
//-----------------------------------------------------------------------------
/*void CGameRulesTeamplay::StartFrame(void)
{
	CGameRulesMultiplay::StartFrame();// handle game over and time limit
}*/

//-----------------------------------------------------------------------------
// Purpose: instant end of the game with intermission
// WHAT THE FUCK?!! THIS DOES NOT WORK!
//-----------------------------------------------------------------------------
/*void CGameRulesTeamplay::EndMultiplayerGame(void)
{
	GoToIntermission(g_pGameRules->GetBestPlayer(g_pGameRules->GetBestTeam()), m_pLastVictim);
}*/

//-----------------------------------------------------------------------------
// Purpose: Initialize HUD (client data) for a client
// Input  : *pPlayer -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::InitHUD(CBasePlayer *pPlayer)
{
	// now we can use messages to send client info
	int i = 0;
	int clientIndex = pPlayer->entindex();

	if (!pPlayer->IsBot())// Send down team names BEFORE gmsgTeamInfo
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgTeamNames, NULL, ENT(pPlayer->pev));
			WRITE_BYTE(m_iNumTeams);// WARNING! +1 for 0th team!
		for (i = 0; i < m_iNumTeams; ++i)
		{
			WRITE_BYTE(m_Teams[i].color[0]);
			WRITE_BYTE(m_Teams[i].color[1]);
			WRITE_BYTE(m_Teams[i].color[2]);
// can be calculated on client side via (RGBtoHSV/360)*255			WRITE_BYTE(m_Teams[i].colormap & 0xFF);// 1 byte
			WRITE_STRING(m_Teams[i].name);
		}
		MESSAGE_END();
	}
	// loop through all active players and send their team info to the new client
	for (i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *plr = UTIL_ClientByIndex(i);
		if (plr)
		{
			if (i != clientIndex)// don't send MY info to me
			{
				// Send everyone's team info to this player
				MESSAGE_BEGIN(MSG_ONE, gmsgTeamInfo, NULL, ENT(pPlayer->pev));// dest: self
					WRITE_BYTE(plr->entindex());
					WRITE_BYTE(plr->pev->team);
				MESSAGE_END();
			}
			// sent MY team info to ALL players, even myself!
			MESSAGE_BEGIN(MSG_ONE, gmsgTeamInfo, NULL, ENT(plr->pev));// dest: all
				WRITE_BYTE(clientIndex);
				WRITE_BYTE(pPlayer->pev->team);
			MESSAGE_END();
		}
	}

	CGameRulesMultiplay::InitHUD(pPlayer);
}

//-----------------------------------------------------------------------------
// Purpose: A player is spawning
// Input  : *pPlayer -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::PlayerSpawn(CBasePlayer *pPlayer)
{
	if (pPlayer->m_iSpawnState == 0)// first time
	{
//		ALERT(at_console, "AssignPlayer(%s): first time\n", STRING(pPlayer->pev->netname));
		if (m_iNumTeams > 0 && mp_teammenu.value > 0.0f && !pPlayer->IsBot())// we have list to choose from // bots can't select team
		{
			pPlayer->pev->team = TEAM_NONE;
//called from RemoveAllItems			g_pGameRules->InitHUD(pPlayer);// send team list to this client (UpdateClientData() will not get called for spectator)
/*
// TODO UNDONE
			CBaseEntity *pSpawnSpot = SpawnPointEntSelectSpectator(NULL);
			if (pSpawnSpot)
				pPlayer->StartObserver(pSpawnSpot->pev->origin, pSpawnSpot->pev->angles);
			else
				pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles);
*/
//			CLIENT_COMMAND(pPlayer->edict(), "chooseteam\n");
			MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, NULL, ENT(pPlayer->pev));// dest: all
				WRITE_BYTE(MENU_TEAM);
				WRITE_BYTE(0);
			MESSAGE_END();
//			return;
		}
	}

	if (pPlayer->pev->team <= TEAM_NONE)// make sure to have set it in CGameRulesMultiplay::ClientConnected()
	{
		AssignPlayer(pPlayer, false);// assign player just on server
	}
/*	else
	{
		ClearBits(pPlayer->pev->effects, EF_NODRAW);
		pPlayer->pev->movetype = MOVETYPE_WALK;
		pPlayer->pev->takedamage = DAMAGE_AIM;
	}*/
	CGameRulesMultiplay::PlayerSpawn(pPlayer);
}

//-----------------------------------------------------------------------------
// Purpose: Assign player to ANY available team. TEAM JOIN LOGIC IS HERE!
// WARNING! neither pev->team, nor "team" kv gets saved during level change.
// Input  : *pPlayer -
//			bSend -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::AssignPlayer(CBasePlayer *pPlayer, bool bSend)
{
//	int clientIndex = pPlayer->entindex();
	char *plrteam = GetPlayerTeamName(pPlayer);
	int newteam = GetTeamIndex(plrteam);
//	bool balanceteams = (mp_teambalance.value > 0.0f);
//	ALERT(at_console, " * AP %s to team %s\n", STRING(pPlayer->pev->netname), plrteam);

	if (newteam > TEAM_NONE)// valid team
	{
		if (PlayerIsInTeam(pPlayer, newteam) <= TEAM_NONE)// already there?
			AddPlayerToTeam(pPlayer, newteam);// * joined team %s
	}
	else
	{
		newteam = CreateNewTeam(plrteam);// try to create a new team
		if (newteam <= TEAM_NONE)// failed
		{
			if (mp_teambalance.value <= 0.0f)
			{
				if (strlen(mp_defaultteam.string) > 0)
					newteam = GetTeamIndex(mp_defaultteam.string);
			}
			if (newteam <= TEAM_NONE)// defaultteam may fail too!
				newteam = TeamWithFewestPlayers();
//			ALERT(at_console, " * AP: unable to create team %s, using default: %d\n", plrteam, newteam);
		}
		else
		{
			if (sv_lognotice.value > 0.0f)
			{
				UTIL_LogPrintf("Created team \"%s\" for player \"%s<%i><%s>\"\n", plrteam,
					STRING(pPlayer->pev->netname),
					GETPLAYERUSERID(pPlayer->edict()),
					GETPLAYERAUTHID(pPlayer->edict()));
			}
		}
//		plrteam = (char *)GetTeamName(newteam);
		AddPlayerToTeam(pPlayer, newteam);//* assigned to team %s
	}

	RecountTeams(false);// don't send yet!
}

//-----------------------------------------------------------------------------
// Purpose: change player's team (by name)
// Input  : *pPlayer -
//			*pTeamName - desired team name, TeamWithFewestPlayers() if invalid
//			bKill - if nescessary
//			bGib -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, bool bKill, bool bGib)
{
	if (pPlayer == NULL)
		return;

	int teamindex = GetTeamIndex(pTeamName);
	if (teamindex == TEAM_NONE)// same as in IsValidTeam()
	{
		ClientPrint(pPlayer->pev, HUD_PRINTTALK, "#TEAM_NOT_FOUND", pTeamName);
		return;
//		teamindex = TeamWithFewestPlayers();
	}

	ChangePlayerTeam(pPlayer, teamindex, bKill, bGib);
}

//-----------------------------------------------------------------------------
// Purpose: change player's team (by TEAM ID)
// Input  : *pPlayer -
//			teamIndex - desired team ID, TeamWithFewestPlayers() if invalid
//			bKill - if nescessary
//			bGib -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::ChangePlayerTeam(CBasePlayer *pPlayer, TEAM_ID teamindex, bool bKill, bool bGib)
{
	if (pPlayer == NULL)
		return;

	if (!IsValidTeam(teamindex) || teamindex == TEAM_NONE)// XDM3035: TEAM_NONE is for spectators, but is technically a usual valid team
		teamindex = TeamWithFewestPlayers();

//no! this fails when all teams have equal players!	if (teamindex == TEAM_NONE)
//		return;

	int clientIndex = pPlayer->entindex();

	RemovePlayerFromTeam(pPlayer, pPlayer->pev->team);

	if (bKill)// kill the player, remove a death, and let them start on the new team
	{
		if (!pPlayer->IsObserver() && pPlayer->IsAlive())// XDM3034
		{
			m_DisableDeathMessages = true;
			m_DisableDeathPenalty = true;

	//		entvars_t *pevWorld = VARS(INDEXENT(0));
			pPlayer->Killed(g_pWorld, g_pWorld, bGib?GIB_ALWAYS:GIB_NEVER);// XDM3034

			m_DisableDeathMessages = false;
			m_DisableDeathPenalty = false;
		}
	}

	AddPlayerToTeam(pPlayer, teamindex);

	// notify everyone's HUD of the team change
	MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
		WRITE_BYTE(clientIndex);
		WRITE_BYTE(teamindex);
	MESSAGE_END();
	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
		WRITE_BYTE(clientIndex);
		WRITE_SHORT((int)pPlayer->pev->frags);
		WRITE_SHORT(pPlayer->m_iDeaths);
	MESSAGE_END();

	if (sv_lognotice.value > 0)
	{
		UTIL_LogPrintf("\"%s<%i><%s>\" joined team \"%s\"\n",
			STRING(pPlayer->pev->netname),
			GETPLAYERUSERID(pPlayer->edict()),
			GETPLAYERAUTHID(pPlayer->edict()),
			GetTeamByID(teamindex)->name);
	}

	if (pPlayer->IsObserver())
		pPlayer->StopObserver();// WARNING: this calls Spawn() and InitHUD()!
}

//-----------------------------------------------------------------------------
// Purpose: Client is about to connect
// Input  : *pEntity - 
//			*pszName - 
//			*pszAddress - 
//			szRejectReason[128] - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
/*bool CGameRulesTeamplay::ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{
	bool ret = CGameRulesMultiplay::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
	if (ret)// player was accepted
	{
		if (m_iNumTeams > 0 && mp_teammenu.value > 0.0f && !(pEntity->v.flags & FL_FAKECLIENT))// we have list to choose from // bots can't select team
		{
			pEntity->v.flags |= FL_SPECTATOR;// XDM3035a: mark to spawn as spectator
			pEntity->v.team = TEAM_NONE;
		}
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
void CGameRulesTeamplay::ClientDisconnected(CBasePlayer *pPlayer)
{
	if (pPlayer)
		RemovePlayerFromTeam(pPlayer, pPlayer->pev->team);

	CGameRulesMultiplay::ClientDisconnected(pPlayer);
}

//-----------------------------------------------------------------------------
// Purpose: Check if the game has reached one of its limits and go to intermission
//-----------------------------------------------------------------------------
bool CGameRulesTeamplay::CheckLimits(void)
{
	int iscorelimit = g_pGameRules->GetScoreLimit();//(int)mp_scorelimit.value;
	int ifraglimit = CGameRulesMultiplay::GetScoreLimit();//(int)fraglimit.value;
	if (iscorelimit > 0 || ifraglimit > 0)
	{
		int score_remaining = 0;
		TEAM_ID bestteam = g_pGameRules->GetBestTeam();// use with g_pGameRules-> only!
		team_t *pTeam = GetTeamByID(bestteam);
		if (pTeam)
		{
			if ((iscorelimit > 0 && pTeam->extrascore >= iscorelimit) ||
				(iscorelimit == 0 && ifraglimit > 0 && pTeam->score >= ifraglimit))// XDM3035a: don't count frag limit in special game rules (DOM/CTF)
			{
				ALERT(at_aiconsole, "CGameRulesTeamplay: team %s wins by reachnig score limit!\n", pTeam->name);
				GoToIntermission(g_pGameRules->GetBestPlayer(bestteam), g_pGameRules->GetTeamBaseEntity(bestteam));
				return true;
			}

			if (m_LeadingTeam != bestteam && (pTeam->score > 0 || pTeam->extrascore > 0))// XDM3035
			{
				m_LeadingTeam = bestteam;
				if (m_LeadingTeam != TEAM_NONE)
				{
				MESSAGE_BEGIN(MSG_BROADCAST, gmsgGREvent);
					WRITE_BYTE(GAME_EVENT_TAKES_LEAD);
					WRITE_SHORT(0);// don't send players in teamplay
					WRITE_SHORT(m_LeadingTeam);
				MESSAGE_END();
				}
			}

			CBasePlayer *pBestPlayer = g_pGameRules->GetBestPlayer(bestteam);
			if (pBestPlayer)
				m_iLeader = pBestPlayer->entindex();

			if (iscorelimit > 0)
				score_remaining = (iscorelimit > 0)?(iscorelimit - pTeam->extrascore):0;
			else if (ifraglimit > 0)
				score_remaining = (ifraglimit > 0)?(ifraglimit - pTeam->score):0;
		}
		else
		{
			if (iscorelimit > 0)
				score_remaining = iscorelimit;
			else if (ifraglimit > 0)
				score_remaining = ifraglimit;
		}
		m_iRemainingScore = score_remaining;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: disallow players to change vital user info (e.g. model color)
// Input  : *pPlayer -
//			*infobuffer -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer)
{
	CGameRulesMultiplay::ClientUserInfoChanged(pPlayer, infobuffer);// we may want this even after GameOver

	if (IsGameOver())
		return;

	if (pPlayer->IsObserver())// important: prevent auto-joining
		return;

/*#ifdef _DEBUG
	ALERT(at_aiconsole, "ClientUserInfoChanged(%s, %s)\n", STRING(pPlayer->pev->netname), infobuffer);
#endif*/
	int playerindex = pPlayer->entindex();

	char *plrteam = GET_INFO_KEY_VALUE(infobuffer, "team");
	if (IsValidTeam(plrteam))// player entered valid team name
	{
		if (GetTeamIndex(plrteam) != pPlayer->pev->team)// player entered a NEW team name
		{
			if (sv_lognotice.value > 0.0f)
			{
				UTIL_LogPrintf("\"%s<%i><%s>\" attempts to change team to \"%s\"\n",
					STRING(pPlayer->pev->netname),
					GETPLAYERUSERID(pPlayer->edict()),
					GETPLAYERAUTHID(pPlayer->edict()),
					plrteam);
			}
			if (mp_teamchange.value > 0.0f)
				ChangePlayerTeam(pPlayer, plrteam, (mp_teamchangekill.value > 0.0f), false);// recursion danger!
		}
	}
	else// just fix it
	{
		char *validteam = (char *)GetTeamName(pPlayer->pev->team);
		if (sv_lognotice.value > 0.0f)
		{
			UTIL_LogPrintf("\"%s<%i><%s>\" has invalid team \"%s\", resetting to \"%s\"\n",
				STRING(pPlayer->pev->netname),
				GETPLAYERUSERID(pPlayer->edict()),
				GETPLAYERAUTHID(pPlayer->edict()),
				plrteam, validteam);
		}
		g_engfuncs.pfnSetClientKeyValue(playerindex, infobuffer, "team", validteam);
	}

/*	if (mp_defaultteam.string || !IsValidTeam(plrteam))
	{
		sprintf( text, "* Can't change team to \'%s\'\n", plrteam );
		ClientPrint(pPlayer, HUD_PRINTTALK, text);
		sprintf( text, "* Server limits teams to \'%s\'\n", teamlist.string );
		ClientPrint(pPlayer, HUD_PRINTTALK, text);
		return;
	}
	ChangePlayerTeam(pPlayer, plrteam, true, true);
	RecountTeams(true);*/

	// validate player colors
	char value[4];
	byte topcolor = m_Teams[pPlayer->pev->team].colormap & 0xFF;
	byte bottomcolor = (m_Teams[pPlayer->pev->team].colormap & 0xFF00) >> 8;
//	memset(value, 0, 8);
	_snprintf(value, 7, "%d", topcolor);
	g_engfuncs.pfnSetClientKeyValue(playerindex, infobuffer, "topcolor", value);
	_snprintf(value, 7, "%d", bottomcolor);
	g_engfuncs.pfnSetClientKeyValue(playerindex, infobuffer, "bottomcolor", value);
	// reset to default skin?
//	g_engfuncs.pfnSetClientKeyValue(playerindex, infobuffer, "skin", "0");
}

//-----------------------------------------------------------------------------
// Purpose: send death notice to all clients
// Input  : *pVictim -
//			*pKiller -
//			*pInflictor - a weapon, object, world, etc.
//-----------------------------------------------------------------------------
/*void CGameRulesTeamplay::DeathNotice(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	if (m_DisableDeathMessages)
		return;

	if (pVictim && pKiller && pKiller->IsPlayer())
	{
		CBasePlayer *pk = (CBasePlayer*)pKiller;
		if (pk)
		{
			if ((pk != pVictim) && (PlayerRelationship(pVictim, pk) == GR_TEAMMATE))
			{
				MESSAGE_BEGIN(MSG_ALL, gmsgDeathMsg);
					WRITE_BYTE(ENTINDEX(pKiller->edict()));// the killer
					WRITE_BYTE(ENTINDEX(pVictim->edict()));// the victim
					WRITE_STRING("teammate");		// flag this as a teammate kill
				MESSAGE_END();
				return;
			}
		}
	}

	CGameRulesMultiplay::DeathNotice(pVictim, pKiller, pInflictor);
}*/

//-----------------------------------------------------------------------------
// Purpose: A player got killed
// Input  : *pVictim -
//			*pKiller -
//			*pInflictor -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	if (!m_DisableDeathPenalty)
	{
		CGameRulesMultiplay::PlayerKilled(pVictim, pKiller, pInflictor);
		RecountTeams(false);
	}
}

//-----------------------------------------------------------------------------
// Purpose: A monster got killed, add score to killer's team
// Input  : *pVictim -
//			*pKiller -
//			*pInflictor -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::MonsterKilled(CBaseMonster *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	if (pKiller && pKiller->IsPlayer())
		AddScoreToTeam(pKiller->pev->team, IPointsForKill(pKiller, pVictim));// TODO: different score for killing different monsters?

	CGameRulesMultiplay::MonsterKilled(pVictim, pKiller, pInflictor);
}


//-----------------------------------------------------------------------------
// Purpose: can this player take damage from this attacker?
// Input  : *pPlayer -
//			*pAttacker -
// Output : Returns TRUE if can
//-----------------------------------------------------------------------------
bool CGameRulesTeamplay::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (pAttacker && PlayerRelationship(pPlayer, pAttacker) == GR_TEAMMATE)
	{
		if (mp_friendlyfire.value <= 0 && pAttacker != pPlayer)
			return false;// my teammate hit me.
	}

	return CGameRulesMultiplay::FPlayerCanTakeDamage(pPlayer, pAttacker);
}

//-----------------------------------------------------------------------------
// Purpose: determines relationship between given player and entity
// Input  : *pPlayer -
//			*pTarget -
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesTeamplay::PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget)
{
	if (!pPlayer || !pTarget || !pTarget->IsPlayer())// TODO: team monsters?
		return GR_NOTTEAMMATE;

	if (!IsValidTeam(pTarget->pev->team))
		return GR_NOTTEAMMATE;

	if (pPlayer->pev->team == pTarget->pev->team)
		return GR_TEAMMATE;

	return CGameRulesMultiplay::PlayerRelationship(pPlayer, pTarget);
}

//-----------------------------------------------------------------------------
// Purpose: allow autoaim, unless target is a teammate
// Input  : *pPlayer -
//			*pTarget -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesTeamplay::ShouldAutoAim(CBasePlayer *pPlayer, CBaseEntity *pTarget)
{
	// we don't check for friendly fire here, because nobody would try to aim at a teammate anyway
	if (pTarget && pTarget->IsPlayer())
	{
		if (PlayerRelationship(pPlayer, pTarget) == GR_TEAMMATE)
			return false;// don't autoaim at teammates
	}

	return CGameRulesMultiplay::ShouldAutoAim(pPlayer, pTarget);
}

//-----------------------------------------------------------------------------
// Purpose: how many points to add after each kill
// Input  : *pAttacker -
//			*pKilled -
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesTeamplay::IPointsForKill(CBaseEntity *pAttacker, CBaseEntity *pKilled)
{
	if (!pKilled)
		return 0;

	if (!pAttacker)
		return CGameRulesMultiplay::IPointsForKill(pAttacker, pKilled);

	if (pAttacker != pKilled && PlayerRelationship(pAttacker, pKilled) == GR_TEAMMATE)
		return -1;

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Find team with least players
// Output : TEAM_ID team index
//-----------------------------------------------------------------------------
TEAM_ID CGameRulesTeamplay::TeamWithFewestPlayers(void)
{
	int minPlayers = gpGlobals->maxClients;// 65535;// MAX_INT or something
	TEAM_ID idx = TEAM_NONE;
	for (int i = 1; i < m_iNumTeams; ++i)// start from 1
	{
//		ALERT(at_console, " TFP: passed team %d (%d players)\n", i, m_Teams[i].playercount);
		if (m_Teams[i].playercount < minPlayers)
		{
			minPlayers = m_Teams[i].playercount;
			idx = i;
		}
	}
//	ALERT(at_console, " TFP: returning team %d (%d players)\n", idx, m_Teams[idx].playercount);
	return idx;// should never return TEAM_NONE
}

//-----------------------------------------------------------------------------
// Purpose: loop through all teams, recounting everything
// Input  : bResendInfo - Someone's info changed, send the team info to everyone
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::RecountTeams(bool bResendInfo)
{
/*#ifdef _DEBUG
	int m_LastFrags[MAX_TEAMS+1];
#endif*/
	int i = 0;
	// clear dynamic data which is acquired elsewhere (ex. from pev->frags)
	for (i = 0; i < m_iNumTeams; ++i)
	{
/*#ifdef _DEBUG
		m_LastFrags[i] = m_Teams[i].score;
#endif*/
		m_Teams[i].score = 0;// XDM3035: TODO TESTME REVISIT
		m_Teams[i].looses = 0;
//no!		m_Teams[i].extrascore = 0;
		m_Teams[i].playercount = 0;
	}
	// reassign all clients
	for (i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)
		{
			m_Teams[pPlayer->pev->team].score += (int)pPlayer->pev->frags;
			m_Teams[pPlayer->pev->team].looses += pPlayer->m_iDeaths;
			m_Teams[pPlayer->pev->team].playercount ++;

			if (bResendInfo)// Someone's info changed, let's send the team info again.
			{
				MESSAGE_BEGIN(MSG_ALL, gmsgTeamInfo);
					WRITE_BYTE(pPlayer->entindex());
					WRITE_BYTE(pPlayer->pev->team);
				MESSAGE_END();
			}
//			pPlayer->pev->colormap = m_Teams[pPlayer->pev->team].colormap;// XDM3034: TMP: players may change teir look using console!
		}
	}
/*#ifdef _DEBUG
	for (i = 0; i < m_iNumTeams; ++i)
	{
		if (m_LastFrags[i] != m_Teams[i].score)
			ALERT(at_aiconsole, "RecountTeams(): updated frags for team %d - from %d to %d\n", i, m_LastFrags[i], m_Teams[i].score);
	}
#endif*/
}

//-----------------------------------------------------------------------------
// Purpose: find team by name
// Input  : *pTeamName -
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesTeamplay::GetTeamIndex(const char *pTeamName)
{
	if (pTeamName && *pTeamName != 0 && strlen(pTeamName) > 0)
	{
		for (int tm = 0; tm < MAX_TEAMS; tm++)
		{
			if (!stricmp(m_Teams[tm].name, pTeamName))
				return tm;
		}
	}
	return TEAM_NONE;
}

//-----------------------------------------------------------------------------
// Purpose: get team name by index
// Input  : teamIndex -
// Output : const char
//-----------------------------------------------------------------------------
const char *CGameRulesTeamplay::GetTeamName(int teamIndex)
{
	if (teamIndex <= TEAM_NONE || teamIndex >= m_iNumTeams)// g_pGameRules->MaxTeams() ?
		return "";

	return m_Teams[teamIndex].name;
}

//-----------------------------------------------------------------------------
// Purpose: validate team name
// Input  : *pTeamName -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesTeamplay::IsValidTeam(const char *pTeamName)
{
	if (!m_teamLimit)// Any team is valid if the teamlist isn't set
		return true;

	return (GetTeamIndex(pTeamName) != TEAM_NONE) ? true : false;
}

//-----------------------------------------------------------------------------
// Purpose: validate team index, accepts TEAM_NONE
// Input  : team -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesTeamplay::IsValidTeam(int team)
{
	if (team < TEAM_NONE || team >= m_iNumTeams)
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: validate team index, ignores TEAM_NONE
// Input  : team -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesTeamplay::IsRealTeam(int team)
{
	if (team < TEAM_1 || team >= m_iNumTeams)
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: special team score - flags/points/rounds/etc.
// Input  : teamIndex -
//			score -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::AddScoreToTeam(TEAM_ID teamIndex, int score)
{
//	ALERT(at_console, "AddScoreToTeam(%d, %d)\n", teamIndex, score);
	if (!IsGameOver() && IsValidTeam(teamIndex))
	{
		m_Teams[teamIndex].extrascore += score;

		// XDM3037: can't figure which score is being added
/*		int limit = g_pGameRules->GetScoreLimit();// XDM3037: IMPORTANT! g_pGameRules->GetScoreLimit() must return frags!
		if (limit > 0 && m_Teams[teamIndex].extrascore > limit)
			m_Teams[teamIndex].extrascore = limit;// clamp
*/
		// update
		MESSAGE_BEGIN(MSG_ALL, gmsgTeamScore, NULL);
			WRITE_BYTE(teamIndex);
			WRITE_SHORT(m_Teams[teamIndex].extrascore);
		MESSAGE_END();

		CheckLimits();
	}
}

//-----------------------------------------------------------------------------
// Purpose: number of players in a team
// Input  : teamIndex
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesTeamplay::NumPlayersInTeam(TEAM_ID teamIndex)
{
	if (IsValidTeam(teamIndex))
		return m_Teams[teamIndex].playercount;

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: if a player joined server with a NEW UNIQUE team name specified,
//          server tries to create this new team if possible.
// Input  : *pTeamName -
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesTeamplay::CreateNewTeam(const char *pTeamName)
{
	ASSERT(g_pGameRules != NULL);
	TEAM_ID tm = TEAM_NONE;

	if (g_pGameRules && m_iNumTeams < g_pGameRules->MaxTeams()+1/* 0th == unassigned/spectators */ && !m_teamLimit && pTeamName && (*pTeamName != 0))
	{
		tm = m_iNumTeams;
		++m_iNumTeams;
		m_Teams[tm].playercount = 0;
		GetTeamColor(tm, m_Teams[tm].color[0], m_Teams[tm].color[1], m_Teams[tm].color[2]);
		float h = 1.0f, s=0.0f, l=0.0f;
		RGB2HSL(m_Teams[tm].color[0], m_Teams[tm].color[1], m_Teams[tm].color[2], h,s,l);
		m_Teams[tm].colormap = ((int)(h*255.0f/360.0f));// XDM3035//teamcolormap[tm];
		m_Teams[tm].colormap += ((int)(NormalizeAngle(h+15.0f)*255.0f/360.0f)) << 8;// shift 1 byte to the left
		strncpy(m_Teams[tm].name, pTeamName, MAX_TEAMNAME_LENGTH);
		ALERT(at_logged, "Created new team: %s (color: %d %d %d, colormap: %X)\n", m_Teams[tm].name, m_Teams[tm].color[0], m_Teams[tm].color[1], m_Teams[tm].color[2], m_Teams[tm].colormap);
	}
	return tm;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pPlayer -
//			teamIndex -
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesTeamplay::PlayerIsInTeam(CBasePlayer *pPlayer, TEAM_ID teamIndex)
{
	if (teamIndex > TEAM_NONE && pPlayer->pev->team == teamIndex)
		return 1;

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Recalculate player's and team score, assign proper color map values
// Input  : *pPlayer -
//			teamIndex -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesTeamplay::AddPlayerToTeam(CBasePlayer *pPlayer, TEAM_ID teamIndex)
{
	if (teamIndex <= TEAM_NONE)
		return false;

	if (PlayerIsInTeam(pPlayer, teamIndex) > TEAM_NONE)// already in that team
		return false;

//	ALERT(at_console, " ADDed %s to team %d (%s)\n", STRING(pPlayer->pev->netname), teamIndex, m_Teams[teamIndex].name);
	m_Teams[teamIndex].score += (int)pPlayer->pev->frags;
	m_Teams[teamIndex].playercount ++;
	pPlayer->pev->team = teamIndex;
	pPlayer->pev->colormap = m_Teams[teamIndex].colormap;
	SetPlayerTeamParams(pPlayer);
//	PlayerUseSpawnSpot(pPlayer);
//	PlayerSpawn(pPlayer);
	if (sv_lognotice.value > 0)
	{
		UTIL_LogPrintf("\"%s<%i><%s>\" added to team %d\n",
			STRING(pPlayer->pev->netname),
			GETPLAYERUSERID(pPlayer->edict()),
			GETPLAYERAUTHID(pPlayer->edict()),
			teamIndex);
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: RemovePlayerFromTeam - removes player from specified team if he is there
// Input  : *pPlayer -
//			teamIndex -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesTeamplay::RemovePlayerFromTeam(CBasePlayer *pPlayer, TEAM_ID teamIndex)
{
	if (teamIndex <= TEAM_NONE)
		return true;// the player IS on TEAM_NONE anyway

	if (PlayerIsInTeam(pPlayer, teamIndex) == 0)// player has different team set
		return false;

	m_Teams[teamIndex].score -= (int)pPlayer->pev->frags;
	m_Teams[teamIndex].playercount --;
	pPlayer->pev->team = TEAM_NONE;
	pPlayer->pev->colormap = 0;
// keep player's team name for next level? // SetPlayerTeamParams(pPlayer);
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: client entered a console command
// Input  : *pPlayer - client
//			*pcmd - command line, use CMD_ARGC() and CMD_ARGV()
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesTeamplay::ClientCommand(CBasePlayer *pPlayer, const char *pcmd)
{
	if (FStrEq(pcmd, "jointeam"))// index TODO: UNDONE: shouldn't be a command..?
	{
		// OBSOLETE?
		if (CMD_ARGC() > 1)
		{
			if (!IsGameOver() && mp_teamchange.value > 0.0f)
				ChangePlayerTeam(pPlayer, atoi(CMD_ARGV(1)), (mp_teamchangekill.value > 0.0f), false);
//				ChangePlayerTeam(pPlayer, GetTeamName(atoi(CMD_ARGV(1))), (mp_teamchangekill.value > 0.0f), false);
		}
		else
			ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "Usage: \"<%s> <team number 1-4>\" or \"<team> <team name>\"\n", CMD_ARGV(0));
	}
/*	else if (FStrEq(pcmd, "joingame"))// overridden for teamplay
	{
		if (!IsGameOver())
		{
			if (pPlayer->IsObserver())
			{
				pPlayer->StopObserver();
				ChangePlayerTeam(pPlayer, GetTeamName(pPlayer->pev->team), false, false);
			}
		}
	}*/
#ifdef _DEBUG
	else if (FStrEq(pcmd, "dumpteams"))
	{
		ALERT(at_console, "m_iNumTeams: %d\n", m_iNumTeams);
		for (int i = 0; i < m_iNumTeams; ++i)
			ALERT(at_console, " > %d %s\tscore %d\textra %d\tplayers %d\tcmap %d\n", i, m_Teams[i].name, m_Teams[i].score, m_Teams[i].extrascore, m_Teams[i].playercount, m_Teams[i].colormap);
	}
#endif
	else return CGameRulesMultiplay::ClientCommand(pPlayer, pcmd);

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Retrieve the team name entered by this player
// Input  : *pPlayer -
// Output : char *name
//-----------------------------------------------------------------------------
char *CGameRulesTeamplay::GetPlayerTeamName(CBasePlayer *pPlayer)
{
//	ALERT(at_console, " -- GetPlayerTeamName() %s %d\n", STRING(pPlayer->pev->netname), pPlayer->pev->team);
	return GET_INFO_KEY_VALUE(GET_INFO_KEY_BUFFER(pPlayer->edict()), "team");
}

//-----------------------------------------------------------------------------
// Purpose: update client user info
// Input  : *pPlayer -
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::SetPlayerTeamParams(CBasePlayer *pPlayer)
{
	char *infobuffer = GET_INFO_KEY_BUFFER(pPlayer->edict());
	if (!infobuffer)
		return;

//	ALERT(at_console, " -- SetPlayerTeamParams() %s %s\n", STRING(pPlayer->pev->netname), GetTeamName(pPlayer->pev->team));
	g_engfuncs.pfnSetClientKeyValue(pPlayer->entindex(), infobuffer, "team", (char *)GetTeamName(pPlayer->pev->team));

	ClientUserInfoChanged(pPlayer, infobuffer);
}

//-----------------------------------------------------------------------------
// Purpose: Overridden for teamplay
// Input  : team - may be TEAM_NONE
// Output : CBasePlayer *
//-----------------------------------------------------------------------------
CBasePlayer *CGameRulesTeamplay::GetBestPlayer(TEAM_ID team)
{
	if (team == TEAM_NONE)
		return NULL;

	return CGameRulesMultiplay::GetBestPlayer(team);
}

//-----------------------------------------------------------------------------
// Purpose: Get team with the best score (FRAG VERSION)
// Output : TEAM_ID
//-----------------------------------------------------------------------------
TEAM_ID CGameRulesTeamplay::GetBestTeam(void)
{
	TEAM_ID bestteam = TEAM_NONE;
	int score;
	int bestscore = 0;// Right now, we get TEAM_NONE if all teams have 0 // -65535;
	int bestlooses = 65535;
	int fewestplayercount = gpGlobals->maxClients;//MAX_CLIENTS;
//	bool use_score = (mp_scorelimit.value > 0.0f)?true:false;// user team score instead of frags
	for (TEAM_ID i = TEAM_1/* check this */; i < m_iNumTeams; ++i)
	{
		score = m_Teams[i].score;

		if (score > bestscore)
		{
			bestscore = score;
			bestlooses = m_Teams[i].looses;
			fewestplayercount = m_Teams[i].playercount;
			bestteam = i;
		}
		else if (score == bestscore)
		{
			if (m_Teams[i].looses < bestlooses)
			{
//				bestscore = score;
				bestlooses = m_Teams[i].looses;
				fewestplayercount = m_Teams[i].playercount;
				bestteam = i;
			}
			else if (m_Teams[i].looses == bestlooses)
			{
				if (m_Teams[i].playercount < fewestplayercount)
				{
//					bestscore = score;
//					bestlooses = m_Teams[i].looses;
					fewestplayercount = m_Teams[i].playercount;
					bestteam = i;
				}
			}
		}
	}
//	ALERT(at_console, "CGameRulesTeamplay::GetBestTeam() %d\n", bestteam);
	return bestteam;
}

//-----------------------------------------------------------------------------
// Purpose: Retrieve pointer to a structure, NULL if the index is invalid
// Input  : team - ID
// Output : team_t
//-----------------------------------------------------------------------------
team_t *CGameRulesTeamplay::GetTeamByID(TEAM_ID team)
{
	if (IsValidTeam(team))
		return &m_Teams[team];

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Score limit for this game type
//-----------------------------------------------------------------------------
int CGameRulesTeamplay::GetScoreLimit(void)
{
	if (g_pGameRules->GetGameType() == GT_TEAMPLAY)
		return 0;// tell the logic to use frag limit ONLY

	return (int)mp_scorelimit.value;// try to use EXTRA SCORE limit
}

//-----------------------------------------------------------------------------
// Purpose: dump debug info to console
//-----------------------------------------------------------------------------
void CGameRulesTeamplay::DumpInfo(void)
{
	CGameRulesMultiplay::DumpInfo();
	ALERT(at_console, " teams (SV): %d\n num\tfrg\tdth\tpng\tpls\tplr\t\tovr\tnam--\n", m_iNumTeams);
	for (TEAM_ID i = TEAM_NONE; i < m_iNumTeams; ++i)// m_iNumTeams includes 0th!
	{
		ALERT(at_console, " #%d  %d %d %d %d %d %d (%s)\n", i, 
		m_Teams[i].score,
		m_Teams[i].looses,
		0,//m_Teams[i].ping,
		0,//m_Teams[i].packetloss,
		m_Teams[i].playercount,
		m_Teams[i].extrascore,
		m_Teams[i].name);
	}
	TEAM_ID bestteam = g_pGameRules->GetBestTeam();
	ALERT(at_console, " Best Team: %d (%s)\n", bestteam, GetTeamName(bestteam));
}
