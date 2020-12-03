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
#include "game.h"
#include "skill.h"
#include "items.h"
#include "voice_gamemgr.h"
#include "hltv.h"
#include "globals.h"
#include "mapcycle.h"
#include "pm_shared.h"// observer modes
#include "shake.h"
#include "msg_fx.h"
#include "projectiles.h"
//#include "bot_main.h"// undone

CVoiceGameMgr	g_VoiceGameMgr;

class CMultiplayGameMgrHelper : public IVoiceGameMgrHelper
{
public:
	virtual bool CanPlayerHearPlayer(CBasePlayer *pListener, CBasePlayer *pTalker)
	{
		if (g_pGameRules->IsTeamplay() && g_pGameRules->PlayerRelationship(pListener, pTalker) != GR_TEAMMATE)
			return false;

		return true;
	}
};

static CMultiplayGameMgrHelper g_GameMgrHelper;


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CGameRulesMultiplay::CGameRulesMultiplay() : CGameRules()
{
	g_VoiceGameMgr.Init(&g_GameMgrHelper, gpGlobals->maxClients);

	m_pLastVictim = NULL;
	m_pIntermissionEntity1 = NULL;
	m_pIntermissionEntity2 = NULL;

	m_flIntermissionEndTime = 0.0f;
	m_flIntermissionStartTime = 0.0f;
	m_bReadyButtonsHit = false;

	m_iRemainingScore = 0;
	m_fRemainingTime = 0.0f;
	m_iRemainingScoreSent = 0;// XDM
	m_iRemainingTimeSent = 0;

	m_iFirstScoredPlayer = 0;// XDM3035
	m_iLeader = 0;// XDM3035
//	m_iAddItemsCount = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CGameRulesMultiplay::~CGameRulesMultiplay()
{
	m_flIntermissionEndTime = 0;
	m_flIntermissionStartTime = 0;
	m_pLastVictim = NULL;
	m_pIntermissionEntity1 = NULL;
	m_pIntermissionEntity2 = NULL;
	m_iRemainingScoreSent = 0;
	m_iRemainingTimeSent = 0;
}

//-----------------------------------------------------------------------------
// Purpose: World is spawning. Initialize all data, cvars, etc.
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::Initialize(void)
{
	if (IS_DEDICATED_SERVER())
	{
		char *servercfgfile = (char *)CVAR_GET_STRING("servercfgfile");
		if (servercfgfile && servercfgfile[0])
		{
			char szCommand[256];
			ALERT(at_console, "Executing dedicated server config file\n");
			sprintf(szCommand, "exec %s\n", servercfgfile );
			SERVER_COMMAND(szCommand);
		}
	}
	else
	{
		char *lservercfgfile = (char *)CVAR_GET_STRING("lservercfgfile");
		if (lservercfgfile && lservercfgfile[0])
		{
			char szCommand[256];
			ALERT(at_console, "Executing listen server config file\n");
			sprintf(szCommand, "exec %s\n", lservercfgfile);
			SERVER_COMMAND(szCommand);
		}
	}
	m_pLastVictim = NULL;
	m_pIntermissionEntity1 = NULL;
	m_pIntermissionEntity2 = NULL;
	m_flIntermissionEndTime = 0.0f;
	m_flIntermissionStartTime = 0.0f;
	m_bReadyButtonsHit = false;
	m_iRemainingScore = 0;
	m_fRemainingTime = 0.0f;
	m_iRemainingScoreSent = 0;
	m_iRemainingTimeSent = 0;
	m_iFirstScoredPlayer = 0;
	m_iLeader = 0;
//	m_iAddItemsCount = 0;

	CGameRules::Initialize();

	// XDM3035 crash recovery
	g_pServerAutoFile = LoadFile(g_szServerAutoFileName, "w+");// Opens an empty file for both reading and writing. If the given file exists, its contents are destroyed.
	if (g_pServerAutoFile)
	{
		fseek(g_pServerAutoFile, 0L, SEEK_SET);
		fprintf(g_pServerAutoFile, "// SERVER '%s' RUNNING '%s' ON MAP '%s'\n", CVAR_GET_STRING("hostname"), g_pGameRules->GetGameDescription(), STRING(gpGlobals->mapname));
		fprintf(g_pServerAutoFile, "echo \" *** Running server recovery config file.\"\n");
//		fprintf(g_pServerAutoFile, "maxplayers %d\nmap %s\n", maxplayers, szNextMap);
		fprintf(g_pServerAutoFile, "map %s\n", STRING(gpGlobals->mapname));
		fflush(g_pServerAutoFile);// IMPORTANT: write to disk
		fclose(g_pServerAutoFile);
		g_pServerAutoFile = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Client entered a console command
// Warning: Don't allow cheats
// Input  : *pPlayer - client
//			*pcmd - command line, use CMD_ARGC() and CMD_ARGV()
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::ClientCommand(CBasePlayer *pPlayer, const char *pcmd)
{
	if (g_VoiceGameMgr.ClientCommand(pPlayer, pcmd))
	{
	}
/*	else if (BotmatchCommand(pPlayer, pcmd))
	{
	}*/
	else if (FStrEq(pcmd, "whois"))
	{
		if (CMD_ARGC() > 1)
		{
			int i = atoi(CMD_ARGV(1));
			CBasePlayer *cl = UTIL_ClientByIndex(i);
			if (cl)
				ALERT(at_console, "Client %d: %s\n", i, STRING(cl->pev->netname));
		}
	}
	else if (FStrEq(pcmd, "follownext"))
	{
		if (pPlayer->IsObserver())
			pPlayer->Observer_FindNextPlayer(FALSE);
	}
	else if (FStrEq(pcmd, "followprev"))
	{
		if (pPlayer->IsObserver())
			pPlayer->Observer_FindNextPlayer(TRUE);
	}
	else if (FStrEq(pcmd, "spectate"))
	{
		if (!IsGameOver())
		{
			if (pPlayer->pev->flags & FL_PROXY)
			{
				pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles, OBS_CHASE_LOCKED, NULL);
			}
			else
			{
				if (g_pGameRules->FAllowSpectatorChange(pPlayer))
				{
					if (pPlayer->IsObserver())
						pPlayer->StopObserver();
					else
						pPlayer->StartObserver(pPlayer->pev->origin, pPlayer->pev->angles, OBS_ROAMING, NULL);
				}
			}
		}
	}
	else if (FStrEq(pcmd, "spec_mode"))
	{
		if (pPlayer->IsObserver())
			pPlayer->Observer_SetMode(atoi(CMD_ARGV(1)));
	}
	else if (FStrEq(pcmd, "joingame"))
	{
		if (!IsGameOver())
		{
			if (pPlayer->IsObserver())
				pPlayer->StopObserver();
	//		else
	//			g_engfuncs.pfnClientPrintf(pPlayer->edict(), print_center, "You're not a spectator!");
		}
	}
/*	else if (FStrEq(pcmd, "drop"))
	{
		if (g_pGameRules->GetGameType() == GT_CTF && pPlayer->m_pCarryingObject && CMD_ARGC() <= 1)
			pPlayer->m_pCarryingObject->Use(pPlayer, pPlayer, USE_TOGGLE, 0.0f);
		else// player is dropping an item.
			pPlayer->DropPlayerItem((char *)CMD_ARGV(1));
	}*/
	else if (g_psv_cheats->value > 0.0f && DeveloperCommand(pPlayer, pcmd))
	{
		// developer command was used
	}
#ifdef _DEBUG
/*	else if (DeveloperCommand(pPlayer, pcmd))
	{
	}*/
	else if (FStrEq(pcmd, "dumpplayers"))
	{
		ALERT(at_console, " ID (name) frags deaths team (team name)\n");
		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			CBasePlayer *plr = UTIL_ClientByIndex(i);
			if (plr)
				ALERT(at_console, " > %d (%s)\t fr %d dh %d\tteam %d (%s)\n", i, STRING(plr->pev->netname), (int)plr->pev->frags, plr->m_iDeaths, plr->pev->team, GetTeamName(plr->pev->team));
		}
	}
	else if (FStrEq(pcmd, "sv_cmd_gm"))
	{
		if (g_psv_cheats->value > 0 && g_pdeveloper->value > 0/* && pPlayer == UTIL_ClientByIndex(1)*/)
		{
			int i = atoi(CMD_ARGV(1));
			CBasePlayer *cl = UTIL_ClientByIndex(i);
			if (cl)
			{
				char *msg = NULL;
				cl->pev->flags = cl->pev->flags ^ FL_GODMODE;
				if (cl->pev->flags & FL_GODMODE)
					msg = "activated";
				else
					msg = "deactivated";

				ClientPrint(pPlayer->pev, HUD_PRINTCONSOLE, "God mode for %s %s\n", STRING(cl->pev->netname), msg);
//				ClientPrint(cl->pev, HUD_PRINTCENTER, "God mode %s\n", msg);
			}
			else
				ALERT(at_console, "Can't find client with index %d!\n", i);
		}
	}
#endif // _DEBUG
	else
		return false;//	return CGameRules::ClientCommand(pPlayer, pcmd);

	return true;
}


//-----------------------------------------------------------------------------
// Purpose: User info changed
// Input  : *pPlayer - 
//			*infobuffer - 
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer)
{
	CGameRules::ClientUserInfoChanged(pPlayer, infobuffer);
}


//-----------------------------------------------------------------------------
// Purpose: Get player with the best score   BUGBUG! Why is this called in CTF and DOM?!!
// Warning: Must be identical to client scoreboard code!
// Input  : team - may be TEAM_NONE
// Output : CBasePlayer *
//-----------------------------------------------------------------------------
CBasePlayer *CGameRulesMultiplay::GetBestPlayer(TEAM_ID team)
{
	if (g_pGameRules->IsTeamplay())// XDM3035: HACK!
	{
		if (team == TEAM_NONE)
			return NULL;
	}
	CBasePlayer *pBestPlayer = NULL;
	int score = 0;
	int bestscore = -65535;// XDM3037: in case there are only losers around
	int bestlooses = 65535;
	float bestlastscoretime = gpGlobals->time + SCORE_AWARD_TIME;// we use this to determine the first player to achieve his score. Allow + some seconds forward
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)
		{
			if (pPlayer->IsObserver())// XDM3035: don't award spectators!
				continue;
			if (team != TEAM_NONE && pPlayer->pev->team != team)
				continue;// skip if particular team specified and it didn't match

			score = (int)pPlayer->pev->frags;
			if (score > bestscore)
			{
				bestscore = score;
				bestlooses = pPlayer->m_iDeaths;
				bestlastscoretime = pPlayer->m_fNextScoreTime;
				pBestPlayer = pPlayer;
			}
			else if (score == bestscore)
			{
				if (pPlayer->m_iDeaths < bestlooses)
				{
//					bestscore = score;
					bestlooses = pPlayer->m_iDeaths;
					bestlastscoretime = pPlayer->m_fNextScoreTime;
					pBestPlayer = pPlayer;
				}
				else if (pPlayer->m_iDeaths == bestlooses)// TODO: the first one to achieve this score shoud win! lesser index/join time?
				{
					if (pPlayer->m_fNextScoreTime < bestlastscoretime)
					{
//						bestscore = score;
//						bestlooses = pPlayer->m_iDeaths;
						bestlastscoretime = pPlayer->m_fNextScoreTime;
						pBestPlayer = pPlayer;
					}
				}
			}
		}
	}

	if (!g_pGameRules->IsTeamplay())// XDM3037: in teamplay it matters not, just for the camera TODO: (!g_pGameRules->UseFrags)
		if (bestscore <= 0)// XDM3035c: nobody earned a single point, no winners. Can't be < 0.
			return NULL;

	return pBestPlayer;
}

//-----------------------------------------------------------------------------
// Purpose: Runs every server frame, should handle any timer tasks, periodic events, etc.
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::StartFrame(void)
{
//	ALERT(at_console, "CGameRulesMultiplay::StartFrame()\n");

	g_VoiceGameMgr.Update(gpGlobals->frametime);

	if (IsGameOver())// intermission in progress
	{
/*		int time = (int)mp_chattime.value;
		if (time < 1)// bounds check
			CVAR_DIRECT_SET(&mp_chattime, "1");
		else if (time > MAX_INTERMISSION_TIME)
			CVAR_DIRECT_SET(&mp_chattime, UTIL_dtos1(MAX_INTERMISSION_TIME));

		m_flIntermissionEndTime = m_flIntermissionStartTime + mp_chattime.value;*/

		if (m_bReadyButtonsHit == false)// XDM3036: now all players must press ready
			m_bReadyButtonsHit = CheckPlayersReady();

		// check to see if we should change levels now
		if (m_bReadyButtonsHit || m_flIntermissionEndTime != 0.0f && m_flIntermissionEndTime < gpGlobals->time)
		{
			m_flIntermissionEndTime = 0.0f;
			ChangeLevel();// intermission is over
		}
//		return;
	}
	else//	if (CheckLimits() == false)
	{
		// time limit check must be done every frame
		float flTimeLimit = mp_timelimit.value * 60.0f;
		if (flTimeLimit > 0.0f)
		{
			m_fRemainingTime = (flTimeLimit > gpGlobals->time ? (flTimeLimit - gpGlobals->time) : 0.0f);// warning! this relies on the fact that gpGlobals->time is 0 when every map gets loaded
			if (gpGlobals->time >= flTimeLimit)
			{
//				ALERT(at_console, "CGameRulesMultiplay::StartFrame(): TIME LIMIT\n");
				EndMultiplayerGame();// same as GoToIntermission(g_pGameRules->GetBestPlayer(g_pGameRules->GetBestTeam()), NULL);
				return;
			}
		}
		else
			m_fRemainingTime = 0.0f;

		SendLeftUpdates();
	}
}

//-----------------------------------------------------------------------------
// Purpose: This function sends rapid gamerules-related updates, optimize and pack as much as possible!
// Safe to be called from StartFrame (e.g. once per frame) only!
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::SendLeftUpdates(void)
{
	if (IsGameOver())
		return;// intermission has already been triggered, so ignore.

	int score_remaining = GetScoreRemaining();
	int time_remaining = (int)m_fRemainingTime;// no point virtualizing it // quantized by 1 second
//	int best_player_index = m_iLeader;
//	TEAM_ID best_team_index = GetBestTeam();

//	ALERT(at_console, "CGameRulesMultiplay::SendLeftUpdates(%d %d %d %d)\n", score_remaining, time_remaining, best_player_index, best_team_index);

	// Updates when remaining score change
	if (score_remaining != m_iRemainingScoreSent)
		CVAR_DIRECT_SET(&mp_scoreleft, UTIL_VarArgs("%d", score_remaining));

	// Updates once per second
	if (time_remaining != m_iRemainingTimeSent)
		CVAR_DIRECT_SET(&mp_timeleft, UTIL_VarArgs("%d", time_remaining));

	// prevent sending 200 messages/second and also track major changes
	byte send = 0;
	if (time_remaining != m_iRemainingTimeSent)
	{
		// HACK: since we don't have any OnCVarChange() callback from the engine, we can only assume when something changes
		if (abs(time_remaining - m_iRemainingTimeSent) > 2)// HACK: someone must've set the timer manually
			send = 2;// force
		else
			send = 1;
	}
	if (send < 2 && score_remaining != m_iRemainingScoreSent)// Don't 'else' because priority may rise
	{
		if (abs(score_remaining - m_iRemainingScoreSent) >= 10)// HACK: someone must've set the limit manually
			send = 2;// force
		else
			send = 1;
	}

//WTF? this function returns crap	if (IS_DEDICATED_SERVER())// XDM3035
	// Client can't read mp_timeleft cvar when connected to hlds

	// UNDONE: these numbers should be put into arrays unique to gamerules
	if (send > 0)
	{
		if (send > 1 ||
			(score_remaining > 0 && (score_remaining == 30 || score_remaining == 20 || score_remaining == 10 || score_remaining == 5 || score_remaining == 3 || score_remaining == 1)) ||
			(mp_timelimit.value > 0.0f && (time_remaining == 10 || time_remaining == 30 || time_remaining == 60 || time_remaining == 180 || time_remaining == 300 || time_remaining == 600)))// TODO: renice this code
		{
	//		ALERT(at_console, "CGameRulesMultiplay::SendLeftUpdates(%d %d %d %d) MSG\n", score_remaining, time_remaining, best_player_index, best_team_index);
			MESSAGE_BEGIN(MSG_ALL, gmsgGRInfo);
				WRITE_BYTE(m_iLeader);//best_player_index);
				WRITE_BYTE(GetBestTeam());
				WRITE_SHORT(score_remaining);
				WRITE_SHORT(time_remaining);
			MESSAGE_END();
		}
	}
	m_iRemainingScoreSent = score_remaining;
	m_iRemainingTimeSent  = time_remaining;
//fail (	g_pWorld->pev->frags = score_remaining;// XDM3035: attempt to send these to all clients through entity
//fail (	g_pWorld->pev->impacttime = time_remaining;
}

//-----------------------------------------------------------------------------
// Purpose: Should players respawn immediately?
// Output : Returns TRUE or FALSE
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::FForceRespawnPlayer(void)
{
	return (mp_forcerespawn.value > 0);
}

//-----------------------------------------------------------------------------
// Purpose: Check if all active players pressed ready (FIRE1/2/JUMP) buttons
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::CheckPlayersReady(void)
{
	byte iNumActivePlayers = 0;
	byte iNumReadyPlayers = 0;
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)
		{
			if (IsActivePlayer(pPlayer))
			{
				++iNumActivePlayers;
				if (pPlayer->m_bReadyPressed)
					++iNumReadyPlayers;
			}
		}
	}
	if (iNumReadyPlayers >= iNumActivePlayers)
		return true;
//test	else if (iNumReadyPlayers == (iNumActivePlayers - 1)
//		print("waiting for %s", whoNotPressed)

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Called when player picks up a new weapon
// Input  : *pPlayer -
//			*pWeapon -
// Output : Returns TRUE or FALSE
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon)
{
	if (!pWeapon->CanDeploy())
		return false;// that weapon can't deploy anyway.

	if (pPlayer->m_pActiveItem == NULL)
		return true;// player doesn't have an active item!

	if (!pPlayer->m_pActiveItem->CanHolster())
		return false;// can't put away the active item.

	if (pWeapon->iWeight() > pPlayer->m_pActiveItem->iWeight())
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Sends current game rules to a client
// Input  : *pPlayer - dest
//-----------------------------------------------------------------------------
/*void CGameRulesMultiplay::UpdateGameMode(CBasePlayer *pPlayer)
{
	MESSAGE_BEGIN(MSG_ONE, gmsgGameMode, NULL, pPlayer->edict());
		WRITE_BYTE(m_iGameType);
		WRITE_BYTE(g_iSkillLevel);// XDM3035a
		WRITE_BYTE(sv_overdrive.value > 0.0f?1:0);// XDM3035a is this a good place for these? (can be changed by the server on-the-fly)
		WRITE_BYTE((int)mp_revengemode.value);
		WRITE_SHORT((int)fraglimit.value);
		WRITE_SHORT(g_pGameRules->GetScoreLimit());// XDM3035a: was (int)mp_scorelimit.value
	MESSAGE_END();
}*/

//-----------------------------------------------------------------------------
// Purpose: Initialize HUD (client data) for a client
// Input  : *pPlayer - dest
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::InitHUD(CBasePlayer *pPlayer)
{
	// notify other clients of player joining the game
	if (sv_lognotice.value > 0)// XDM
	{
		UTIL_LogPrintf("\"%s<%i><%s><%s>\" entered the game\n",
			STRING(pPlayer->pev->netname),
			GETPLAYERUSERID(pPlayer->edict()),
			GETPLAYERAUTHID(pPlayer->edict()),
			GET_INFO_KEY_VALUE(GET_INFO_KEY_BUFFER(pPlayer->edict()), "team"));// XDM
	}
	UpdateGameMode(pPlayer);

	// XDM3037: so client can start counting
	MESSAGE_BEGIN(((sv_reliability.value > 0)?MSG_ONE:MSG_ONE_UNRELIABLE), gmsgGRInfo, NULL, pPlayer->edict());
		WRITE_BYTE(m_iLeader);//best_player_index);
		WRITE_BYTE(GetBestTeam());
		WRITE_SHORT(GetScoreRemaining());
		WRITE_SHORT((int)m_fRemainingTime);
	MESSAGE_END();

	// loop through all active players and send their score info to the new client
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pClient = UTIL_ClientByIndex(i);
		if (pClient)
		{
			// Send this client's score info to me
			MESSAGE_BEGIN(MSG_ONE, gmsgScoreInfo, NULL, pPlayer->edict());
				WRITE_BYTE(i);// client number
				WRITE_SHORT((int)pClient->pev->frags);
				WRITE_SHORT(pClient->m_iDeaths);
			MESSAGE_END();
			// Send this client's spectator state to me
			MESSAGE_BEGIN(MSG_ONE, gmsgSpectator, NULL, pPlayer->edict());
				WRITE_BYTE(i);
				WRITE_BYTE(pClient->pev->iuser1);// XDM: was != 0
			MESSAGE_END();
		}
	}

	if (!pPlayer->IsBot() && !pPlayer->m_fGameHUDInitialized)// XDM: this may get called from spectator code
	{
		MESSAGE_BEGIN(((sv_reliability.value > 0)?MSG_ONE:MSG_ONE_UNRELIABLE), gmsgAudioTrack, NULL, pPlayer->edict());// XDM: begin playing soundtrack
			WRITE_BYTE(0);// HL track index
			WRITE_BYTE(1);// loop
		MESSAGE_END();
		SendMOTDToClient(pPlayer);
	}

	if (IsGameOver())
	{
		MESSAGE_BEGIN(MSG_ONE, SVC_INTERMISSION, NULL, pPlayer->edict());
		MESSAGE_END();
		// XDM3035c
		pPlayer->pev->framerate = 0;// stop!
		pPlayer->pev->velocity = g_vecZero;
		pPlayer->pev->speed = 0;
		pPlayer->pev->iuser1 = OBS_INTERMISSION;
		if (m_pIntermissionEntity1)
		{
			pPlayer->m_hObserverTarget = m_pIntermissionEntity1;// XDM3035c: use his PVS
			pPlayer->pev->iuser2 = m_pIntermissionEntity1->entindex();
		}
		else
			pPlayer->pev->iuser2 = 0;

		if (m_pIntermissionEntity2)
			pPlayer->pev->iuser3 = m_pIntermissionEntity2->entindex();
		else
			pPlayer->pev->iuser3 = 0;
	}
	else
		FireTargets("game_playerjoin", pPlayer, pPlayer, USE_TOGGLE, 0);// XDM
}

//-----------------------------------------------------------------------------
// Purpose: A network client is connecting. DO NOT CONVERT TO CBasePlayer!!!
// If ClientConnected returns FALSE, the connection is rejected and the user is provided the reason specified in svRejectReason
// Only the client's name and remote address are provided to the dll for verification.
// Input  : *pEntity -
//			*pszName -
//			*pszAddress -
//			szRejectReason -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{
	pEntity->v.effects |= EF_NODRAW;// XDM3035a
	pEntity->v.team = TEAM_NONE;// XDM: !!!!!!
	g_VoiceGameMgr.ClientConnected(pEntity);

	UTIL_ClientPrintAll(HUD_PRINTTALK, "+ #CL_JOIN\n", pszName);

//	if (g_DisplayTitle || mp_showgametitle.value > 0.0f)
//		pPlayer->m_bDisplayTitle = true;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: client has been disconnected
// Input  : *pPlayer -
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::ClientDisconnected(CBasePlayer *pPlayer)
{
	FireTargets("game_playerleave", pPlayer, pPlayer, USE_TOGGLE, 0);
	int iPlayer = pPlayer->entindex();
	// XDM: this may be called from StartObserver()
	if (!(pPlayer->pev->flags & FL_SPECTATOR))
	{
		if (sv_lognotice.value > 0)// XDM
		{
			UTIL_LogPrintf("\"%s<%i><%s><%s>\" disconnected\n",
				STRING(pPlayer->pev->netname),
				GETPLAYERUSERID(pPlayer->edict()),
				GETPLAYERAUTHID(pPlayer->edict()),
				GET_INFO_KEY_VALUE(GET_INFO_KEY_BUFFER(pPlayer->edict()), "team"));// XDM
		}
		pPlayer->RemoveAllItems(TRUE);// destroy all of the players weapons and items

		UTIL_ClientPrintAll(HUD_PRINTTALK, "- #CL_LEAVE\n", STRING(pPlayer->pev->netname));

		if (m_iFirstScoredPlayer == iPlayer)// XDM3035: ???
			m_iFirstScoredPlayer = 0;
	}

	CBasePlayer *pClient = NULL;
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		pClient = UTIL_ClientByIndex(i);
		if (pClient)
		{
			if (pClient == pPlayer)
				continue;

			// If a spectator was chasing this player, move him/her onto the next player
			if (pClient->IsObserver() && pClient->m_hObserverTarget == pPlayer)
				pClient->Observer_FindNextPlayer(FALSE);

			// Don't let this freed index be remembered anywhere!
			if (pClient->m_iLastKiller == iPlayer)
				pClient->m_iLastKiller = 0;
			if (pClient->m_iLastVictim == iPlayer)
				pClient->m_iLastVictim = 0;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Check if the game has reached one of its limits
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::CheckLimits(void)
{
	int ifraglimit = GetScoreLimit();
	if (ifraglimit > 0)
	{
		int score_remaining = 0;
		CBasePlayer *pBestPlayer = g_pGameRules->GetBestPlayer(g_pGameRules->GetBestTeam());
		if (pBestPlayer)
		{
			if ((int)pBestPlayer->pev->frags >= ifraglimit)
			{
//				ALERT(at_console, "CGameRulesMultiplay::CheckLimits(): FRAG LIMIT\n");
				GoToIntermission(pBestPlayer, NULL/*m_pLastVictim may contain random person*/);
				return true;
			}

			int best_player_index = pBestPlayer->entindex();
			if (m_iLeader > 0 && m_iLeader != best_player_index && pBestPlayer->pev->frags > 0)
			{
				m_iLeader = best_player_index;
				if (pBestPlayer->pev->frags > 1)// XDM3035a: don't interrupt "first score" announcement
				{
				MESSAGE_BEGIN(((sv_reliability.value > 1)?MSG_ALL:MSG_BROADCAST), gmsgGREvent);
					WRITE_BYTE(GAME_EVENT_TAKES_LEAD);
					WRITE_SHORT(m_iLeader);
					WRITE_SHORT(pBestPlayer->pev->team);
				MESSAGE_END();
				}
			}
			score_remaining = ifraglimit - (int)pBestPlayer->pev->frags;
		}
		else
			score_remaining = ifraglimit;

		m_iRemainingScore = score_remaining;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: How much damage should falling players take
// Input  : *pPlayer -
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesMultiplay::FlPlayerFallDamage(CBasePlayer *pPlayer)
{
	pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
		return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
}

//-----------------------------------------------------------------------------
// Purpose: Can this player take damage from this attacker?
// Input  : *pPlayer -
//			*pAttacker -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker)
{
	if (IsGameOver())// XDM3035: nobody wants to see score changed after the end!
		return false;

	if (pAttacker && !pAttacker->IsBSPModel())// world can inflict damage any time
	{
		if ((gpGlobals->time - pPlayer->m_flLastSpawnTime) <= max(0.0f, mp_spawnprotectiontime.value))
		{
			if (!(pPlayer->m_afButtonLast & (IN_ATTACK | IN_ATTACK2)) &&
				!(pPlayer->m_afButtonPressed & (IN_ATTACK | IN_ATTACK2)))// XDM3035c: testme
				return false;
		}

		if (pAttacker->IsPlayer())
		{
			if (mp_revengemode.value > 1.0f)// mode 2: players cannot hurt anyone until they get revenge!
			{
				CBasePlayer *pAttackerPlayer = (CBasePlayer *)pAttacker;
				if (pAttackerPlayer->m_iLastKiller != 0 &&
					pAttackerPlayer->m_iLastKiller != pAttackerPlayer->entindex() &&
					pAttackerPlayer->m_iLastKiller != pPlayer->entindex())
				{
					if (pAttackerPlayer->m_flNextChatTime <= gpGlobals->time)
					{
						ClientPrint(pAttackerPlayer->pev, HUD_PRINTCENTER, "#REVENGE_NEED");//"Can't hurt anyone! Get revenge!\n");
						pAttackerPlayer->m_flNextChatTime = gpGlobals->time + CHAT_INTERVAL;
					}
					return false;
				}
			}
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Individual function for each player
// Input  : *pPlayer -
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::PlayerThink(CBasePlayer *pPlayer)
{
	if (IsGameOver())
	{
		// check for button presses
		if (pPlayer->m_afButtonPressed & (IN_ATTACK | IN_ATTACK2 | IN_USE | IN_JUMP))
		{
			if (pPlayer->m_bReadyPressed == false)// XDM3036
			{
				pPlayer->m_bReadyPressed = true;
				MESSAGE_BEGIN(MSG_BROADCAST, gmsgGREvent);
					WRITE_BYTE(GAME_EVENT_PLAYER_READY);
					WRITE_SHORT(pPlayer->entindex());
					WRITE_SHORT(pPlayer->m_afButtonPressed);
				MESSAGE_END();
			}
		}

		// clear attack/use commands from player
		pPlayer->m_afButtonPressed = 0;
		pPlayer->pev->button = 0;
		pPlayer->m_afButtonReleased = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Player is spawning
// Input  : *pPlayer -
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::PlayerSpawn(CBasePlayer *pPlayer)
{
	try
	{
		if (!pPlayer->IsObserver())
		{
			g_pGameRules->PlayerUseSpawnSpot(pPlayer);// XDM
			g_pGameRules->UpdateGameMode(pPlayer);// XDM3035c: some of game values may change during the game.
			pPlayer->pev->weapons |= (1<<WEAPON_SUIT);

			gEvilImpulse101 = TRUE;// XDM: HACK
			pPlayer->GiveNamedItem("weapon_tyriangun");
			gEvilImpulse101 = FALSE;// XDM: HACK

			TraceResult tr1, tr2;
			UTIL_TraceLine( pPlayer->pev->origin, pPlayer->pev->origin + Vector(0,0,-96), ignore_monsters, pPlayer->edict(), &tr1 );
			UTIL_TraceLine( pPlayer->pev->origin, pPlayer->pev->origin + Vector(0,0,300), ignore_monsters, pPlayer->edict(), &tr2 );

			if (mp_teleport_allow.value > 0.0f && tr1.flFraction != 1.0 && tr2.flFraction == 1.0)// if we are standing on the ground and there is enough space (no floor)
			{
				FX_Trail(pPlayer->pev->origin, pPlayer->entindex(), FX_PLAYER_SPAWN_RINGTELEPORT_PART2);
				CRingTeleport::CreateNew(pPlayer, tr1.vecEndPos, FALSE);
			}
			else
				FX_Trail(pPlayer->pev->origin, pPlayer->entindex(), FX_PLAYER_SPAWN);
			
			FireTargets("game_playerspawn", pPlayer, pPlayer, USE_TOGGLE, 0);// XDM: moved here from CBasePlayer::UpdateClientData()
		}
	}
	catch (...)
	{
		ALERT(at_console, "ERROR: CGameRulesMultiplay::PlayerSpawn() exception!\n");
		DBG_FORCEBREAK
	}
}


//-----------------------------------------------------------------------------
// Purpose: Can the player respawn NOW?
// Input  : *pPlayer -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::FPlayerCanRespawn(CBasePlayer *pPlayer)
{
	if (IsGameOver())
		return false;

	if (mp_respawntime.value > 0.0f)// XDM3035
	{
		if ((gpGlobals->time - pPlayer->m_fDeadTime) < mp_respawntime.value)
			return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: How many points awarded to anyone that kills this entity
// Input  : *pAttacker -
//			*pKilled -
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesMultiplay::IPointsForKill(CBaseEntity *pAttacker, CBaseEntity *pKilled)
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Add score and awards UNDONE: monsters can score??
// Input  : *pWinner -
//			score -
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::AddScore(CBaseEntity *pWinner, int score)
{
	if (pWinner == NULL)
		return;

	pWinner->pev->frags += (float)score;
/* useful only for non-team games anyway
	int limit = g_pGameRules->GetScoreLimit();// XDM3037: IMPORTANT! g_pGameRules->GetScoreLimit() must return frags!
	if (limit > 0 && pWinner->pev->frags > limit)
		pWinner->pev->frags = limit;// clamp
*/
	if (pWinner->IsPlayer() && score > 0 && pWinner->IsAlive())// don't award dead players
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pWinner;

		// COMBOS (killing spree, etc.)
		pPlayer->m_iScoreCombo += score;// keep increasing this no matter what
		div_t dcombo = div(pPlayer->m_iScoreCombo, SCORE_AWARD_COMBO);// divide
		if (dcombo.quot > 0 && dcombo.rem == 0 && dcombo.quot <= SCORE_COMBO_MAX)
		{
			MESSAGE_BEGIN(MSG_BROADCAST, gmsgGREvent);
				WRITE_BYTE(GAME_EVENT_COMBO);
				WRITE_SHORT(pPlayer->entindex());
				WRITE_SHORT(dcombo.quot);
			MESSAGE_END();
/* is this important enough?
			MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
				WRITE_BYTE(9);// command length in bytes
				WRITE_BYTE(DRC_CMD_EVENT);
				WRITE_SHORT(pPlayer->entindex());// index number of primary entity
				WRITE_SHORT(0);// index number of secondary entity
				WRITE_LONG(7 | DRC_FLAG_FACEPLAYER);// eventflags (priority and flags)
			MESSAGE_END();
			*/
			if (sv_lognotice.value > 0)
			{
				UTIL_LogPrintf("\"%s<%i><%s><%s>\" got combo \"%d\"\n",
					STRING(pPlayer->pev->netname),
					GETPLAYERUSERID(pPlayer->edict()),
					GETPLAYERAUTHID(pPlayer->edict()),
					g_pGameRules->GetTeamName(pPlayer->pev->team),
					dcombo.quot);
			}
		}

		// AWARDS (double, triple, multi, etc.)
		if (pPlayer->m_fNextScoreTime >= gpGlobals->time)
			pPlayer->m_iLastScoreAward++;// increase
		else
			pPlayer->m_iLastScoreAward = 1;// start over

		if (pPlayer->m_iLastScoreAward > 1 && pPlayer->m_iLastScoreAward <= SCORE_AWARD_MAX)
		{
			MESSAGE_BEGIN(((sv_reliability.value > 0)?MSG_ONE:MSG_ONE_UNRELIABLE), gmsgGREvent, NULL, pPlayer->edict());
				WRITE_BYTE(GAME_EVENT_AWARD);
				WRITE_SHORT(pPlayer->entindex());
				WRITE_SHORT(pPlayer->m_iLastScoreAward);
			MESSAGE_END();
		}
		pPlayer->m_fNextScoreTime = gpGlobals->time + SCORE_AWARD_TIME;

		if (m_iFirstScoredPlayer == 0)// WARNING! This will be reset when client leaves the game! TODO: FIXME
		{
			MESSAGE_BEGIN(((sv_reliability.value > 1)?MSG_ALL:MSG_BROADCAST), gmsgGREvent);
				WRITE_BYTE(GAME_EVENT_FIRST_SCORE);
				WRITE_SHORT(pPlayer->entindex());
				WRITE_SHORT(0);
			MESSAGE_END();
			m_iFirstScoredPlayer = pPlayer->entindex();
		}
	}
	CheckLimits();
}

//-----------------------------------------------------------------------------
// Purpose: Score limit for this game type
//-----------------------------------------------------------------------------
int CGameRulesMultiplay::GetScoreLimit(void)
{
	return (int)mp_fraglimit.value;
}

//-----------------------------------------------------------------------------
// Purpose: How much score remaining until end of game
//-----------------------------------------------------------------------------
int CGameRulesMultiplay::GetScoreRemaining(void)
{
	return m_iRemainingScore;
}

//-----------------------------------------------------------------------------
// Purpose: A player got killed, run logic
// Input  : *pVictim - a player that was killed
//			*pKiller - a player, monster or whatever it can be
//			*pInflictor - the actual entity that did the damage (grenade, etc.)
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	ASSERT(pVictim != NULL);
//	ALERT(at_console, " +++ %s WAS KILLED +++\n", STRING(pVictim->pev->netname));

	m_pLastVictim = pVictim;// XDM3035
	pVictim->m_iDeaths++;

	DeathNotice(pVictim, pKiller, pInflictor);
	FireTargets("game_playerdie", pVictim, pVictim, USE_TOGGLE, 0);

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
				WRITE_SHORT(iKiller);
				WRITE_SHORT(iVictim);
			MESSAGE_END();
			}
// multiple?			pVictim->m_hLastKiller = NULL;
		}
		else
			pVictim->m_iLastKiller = iKiller;// remember new killer

		if (pMonsterKiller)// must be valid for monsters AND players
			pMonsterKiller->m_iLastVictim = iVictim;

		if (pPlayerKiller)
		{
			if (pVictim->m_iScoreCombo > SCORE_AWARD_COMBO)// at least one combo
			{
				MESSAGE_BEGIN(((sv_reliability.value > 1)?MSG_ALL:MSG_BROADCAST), gmsgGREvent);
					WRITE_BYTE(GAME_EVENT_COMBO_BREAKER);
					WRITE_SHORT(iKiller);
					WRITE_SHORT(iVictim);
				MESSAGE_END();
			}
		}

		if (pVictim->pev == pKiller->pev)// killed self
		{
			AddScore(pKiller, -IPointsForKill(pKiller, pVictim));// -1

			if (pPlayerKiller)
			{
// remember			pVictim->m_iLastKiller = 0;
				pPlayerKiller->m_iLastVictim = 0;
			}

			if (mp_revengemode.value > 1.0f)// XDM3037: if a player kills himself, he gets erased as anyone's killer
			{
				for (int i = 1; i <= gpGlobals->maxClients; ++i)
				{
					CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
					if (pPlayer)
					{
						if (pPlayer != pKiller && pPlayer->m_iLastKiller == iKiller)
						{
							ClientPrint(pPlayer->pev, HUD_PRINTCENTER, "#REVENGE_RESET", STRING(pKiller->pev->netname));
							pPlayer->m_iLastKiller = 0;// reset
						}
					}
				}
			}
		}
		else if (pPlayerKiller && !pPlayerKiller->IsObserver())// && pKiller->IsPlayer()) // XDM3035: don't award spectators!
		{
			// pKiller's killer is now a victim. Check BEFORE awarding!
			if (pPlayerKiller->m_iLastKiller == iVictim)
			{
				if (pPlayerKiller->m_iLastScoreAward == 0)// this is the FIRST victim after respawn
				{
					MESSAGE_BEGIN(((sv_reliability.value > 1)?MSG_ALL:MSG_BROADCAST), gmsgGREvent);// let everyone know
						WRITE_BYTE(GAME_EVENT_REVENGE);
						WRITE_SHORT(iKiller);
						WRITE_SHORT(iVictim);
					MESSAGE_END();
					if (mp_revengemode.value > 0.0f)
						AddScore(pKiller, 1);// extra score for killing your killer!
					// sats.revenges++
				}
				pPlayerKiller->m_iLastKiller = 0;// TODO TESTME!!!!! only once! (critical for mp_revengemode)
			}
			// if a player dies in a deathmatch game and the killer is a client, award the killer some points
			AddScore(pKiller, IPointsForKill(pPlayerKiller, pVictim));// +1
			FireTargets("game_playerkill", pKiller, pKiller, USE_TOGGLE, 0);
		}
		else if (/*pKiller && */pKiller->IsMonster())
		{
			if (g_pGameRules->IsCoOp())// XDM
				AddScore(pKiller, 1);
		}
		else// killed by the world
		{
			pVictim->m_iLastKiller = 0;
//			AddScore(pVictim, -1);
		}// not fair when killed by spawn spot
	}
	else
		pVictim->m_iLastKiller = 0;

	pVictim->m_iLastScoreAward = 0;// XDM3035: reset awards
	pVictim->m_iScoreCombo = 0;// XDM3035: reset combo

	// update the scores
	// killed scores
	MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
		WRITE_BYTE(iVictim);
		WRITE_SHORT((int)pVictim->pev->frags);
		WRITE_SHORT(pVictim->m_iDeaths);
	MESSAGE_END();

	// killers score, if it's a player
	if (pPlayerKiller)
	{
		MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
			WRITE_BYTE(iKiller);
			WRITE_SHORT((int)pPlayerKiller->pev->frags);
			WRITE_SHORT(pPlayerKiller->m_iDeaths);
		MESSAGE_END();
		// let the killer paint another decal as soon as he'd like.
		pPlayerKiller->m_flNextDecalTime = gpGlobals->time;
	}

	// UNDONE: in some cases this will help regaining trains, in other it will break the gameplay.
//	CBaseDelay *pTrain = pVictim->GetControlledTrain();
//	if (pTrain)
//		pTrain->Use(pVictim, pVictim, USE_OFF, 0.0f);// STOP the train
}

//-----------------------------------------------------------------------------
// Purpose: A monster got killed
// Input  : *pVictim -
//			*pKiller -
//			*pInflictor -
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::MonsterKilled(CBaseMonster *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
// Do nothing in this kind of game!
}

//-----------------------------------------------------------------------------
// Purpose: Work out what killed the player, and send a message to all clients about it
// Input  : *pVictim -
//			*pKiller -
//			*pInflictor - an entity that inflicted the damage, usually a weapon
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::DeathNotice(CBaseEntity *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	int killer_index = 0;
	CBasePlayer *pPlayerKiller = NULL;
	const char *killer_weapon_name = "world";// by default, the player is killed by the world

	ASSERT(pVictim != NULL);
	if (pKiller)
	{
		killer_index = pKiller->entindex();
		if (pKiller->IsPlayer())
		{
			pPlayerKiller = (CBasePlayer*)pKiller;
			// teamkill
			if (g_pGameRules->IsTeamplay() && (pPlayerKiller != pVictim) && (g_pGameRules->PlayerRelationship(pVictim, pPlayerKiller) == GR_TEAMMATE))
			{
				killer_weapon_name = "teammate";
			}
			else
			{
				if (pInflictor)
				{
					if (pInflictor->pev == pKiller->pev)// XDM is it safe?
					{
						// If the inflictor is the killer,  then it must be their current weapon doing the damage
						if (pPlayerKiller->m_pActiveItem)
							killer_weapon_name = pPlayerKiller->m_pActiveItem->pszName();
					}
					else
						killer_weapon_name = STRING(pInflictor->pev->classname);// it's just that easy
				}

				if (strncmp(killer_weapon_name, "monster_", 8) == 0)// !!! check for OWNED 'monster_'s here !!!
					killer_weapon_name += 8;// monster_snark, tripmine, etc.
			}
			//HACK for insta-hit (non projectile) to draw proper icons and names
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_MACHINEGUN)
			{
				killer_weapon_name = "MachineGun";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_MINIGUN)
			{
				killer_weapon_name = "MiniGun";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_VULCAN_MINIGUN)
			{
				killer_weapon_name = "Vulcan";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_HEAVY_TURRET)
			{
				killer_weapon_name = "HeavyTurret";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_CLUSTER_GUN)
			{
				killer_weapon_name = "ClusterGun";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_FLAK_CANNON)
			{
				killer_weapon_name = "FlakCannon";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_GAUSS)
			{
				killer_weapon_name = "GaussCannon";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_LASER)
			{
				killer_weapon_name = "LaserCannon";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_LIGHTNING)
			{
				killer_weapon_name = "LightningGun";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_MEZON_CANNON)
			{
				killer_weapon_name = "MezonCannon";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_NEYTRON_GUN)
			{
				killer_weapon_name = "NeytronGun";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_PHOTONGUN)
			{
				killer_weapon_name = "PhotonGun";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_RAILGUN)
			{
				killer_weapon_name = "RailGun";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_REPEATER)
			{
				killer_weapon_name = "Repeater";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_SHOCK_CANNON)
			{
				killer_weapon_name = "ShockCannon";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_SUPER_RAILGUN)
			{
				killer_weapon_name = "SuperRailGun";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_TAU_CANNON)
			{
				killer_weapon_name = "TauCannon";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_BEAMSPLITTER)
			{
				killer_weapon_name = "BeamSplitter";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_TWIN_LASER)
			{
				killer_weapon_name = "TwinLaser";
			}
			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_BFG)
			{
				killer_weapon_name = "BFG";
			}

			if (pPlayerKiller->m_iKillGunType == TYRIANGUN_PLAYER_BLAST)
			{
				killer_weapon_name = "AmmunitionBlast";
			}
		}
		else if (pInflictor)
		{
//			pPlayerKiller = NULL;
			if (pKiller->IsMonster())// XDM: HACK because we can't send monster classname and it's weapon name at once
				killer_weapon_name = STRING(pKiller->pev->classname);
			else
				killer_weapon_name = STRING(pInflictor->pev->classname);
		}
	}
	// strip the monster_* or weapon_* from the inflictor's classname
	if (strncmp(killer_weapon_name, "weapon_", 7) == 0)
		killer_weapon_name += 7;
	else if (strncmp(killer_weapon_name, "func_", 5) == 0)
		killer_weapon_name += 5;
	else if (strcmp(killer_weapon_name, "monster_tripmine") == 0)// HACKHACKHACK: this is not a monster!
		killer_weapon_name += 8;
	else if (strcmp(killer_weapon_name, "monster_spider_mine") == 0)
		killer_weapon_name += 8;

	// TODO: revenge! if the player kills his last killer before killing anyone else :)
	// TODO: merge gmsgGREvent into gmsgDeathMsg

	// WARNING: incompatibility: XDM3035a
	MESSAGE_BEGIN(MSG_ALL, gmsgDeathMsg);
		WRITE_SHORT(/*pPlayerKiller==NULL?0:*/killer_index);// the killer: client doesn't always know about non-player entindexes so just leave 0 (also there's only 1 byte)
		WRITE_SHORT(pVictim->entindex());// the victim
		WRITE_STRING(killer_weapon_name);// what they were killed by (should this be a string?)
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
		WRITE_BYTE(9);// command length in bytes
		WRITE_BYTE(DRC_CMD_EVENT);// player killed
		WRITE_SHORT(pVictim->entindex());// index number of primary entity
		if (pInflictor)
			WRITE_SHORT(pInflictor->entindex());// index number of secondary entity
		else
			WRITE_SHORT(killer_index);// index number of secondary entity

		WRITE_LONG(7 | DRC_FLAG_DRAMATIC);// eventflags (priority and flags)
	MESSAGE_END();

	if (sv_lognotice.value > 0)// XDM
	{
		if (pKiller && pVictim->pev == pKiller->pev)// killed self
		{
			UTIL_LogPrintf("\"%s<%i><%s><%s>\" committed suicide with \"%s\"\n",
				STRING(pVictim->pev->netname),
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_pGameRules->GetTeamName(pVictim->pev->team),
				killer_weapon_name);
		}
		else if (pKiller && pKiller->IsPlayer())
		{
			UTIL_LogPrintf("\"%s<%i><%s><%s>\" killed \"%s<%i><%s><%s>\" with \"%s\"\n",
				STRING(pKiller->pev->netname),
				GETPLAYERUSERID( pKiller->edict() ),
				GETPLAYERAUTHID( pKiller->edict() ),
				g_pGameRules->GetTeamName(pKiller->pev->team),
				STRING(pVictim->pev->netname),
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_pGameRules->GetTeamName(pVictim->pev->team),
				killer_weapon_name );
		}
		else if (pKiller && pKiller->IsMonster())
		{
			UTIL_LogPrintf("\"%s<%i><%s><%s>\" were killed by %s (%d)\n",
				STRING(pVictim->pev->netname),
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_pGameRules->GetTeamName(pVictim->pev->team),
				STRING(pKiller->pev->classname),
				killer_index);
		}
		else// killed by the world
		{
			UTIL_LogPrintf("\"%s<%i><%s><%s>\" was destoryed by \"%s\" (world)\n",
				STRING(pVictim->pev->netname),
				GETPLAYERUSERID( pVictim->edict() ),
				GETPLAYERAUTHID( pVictim->edict() ),
				g_pGameRules->GetTeamName(pVictim->pev->team),
				killer_weapon_name);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Player picked up a weapon
// Input  : *pPlayer -
//			*pWeapon -
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon)
{
}

//-----------------------------------------------------------------------------
// Purpose: What is the time in the future at which this weapon may spawn?
// Input  : *pWeapon -
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesMultiplay::FlWeaponRespawnTime(CBasePlayerItem *pWeapon)
{
	return gpGlobals->time + mp_itm_resp_time.value;
}

//-----------------------------------------------------------------------------
// Purpose: Weapon tries to respawn, calculate desired time for it
// Input  : *pWeapon -
// Output : float  the time at which it can try to spawn again (0 == now)
//-----------------------------------------------------------------------------
float CGameRulesMultiplay::FlWeaponTryRespawn(CBasePlayerItem *pWeapon)
{
	if (pWeapon && pWeapon->GetID() && (pWeapon->iFlags() & ITEM_FLAG_LIMITINWORLD))
	{
		if (NUMBER_OF_ENTITIES() < (gpGlobals->maxEntities - ENTITY_INTOLERANCE))
			return 0.0f;

		// we're past the entity tolerance level,  so delay the respawn
		return FlWeaponRespawnTime(pWeapon);
	}

	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: where should this weapon spawn?
// Some game variations may choose to randomize spawn locations
// Input  : *pWeapon -
// Output : Vector
//-----------------------------------------------------------------------------
Vector CGameRulesMultiplay::VecWeaponRespawnSpot(CBasePlayerItem *pWeapon)
{
	return pWeapon->pev->origin;
}

//-----------------------------------------------------------------------------
// Purpose: World models will be scaled by this factor. Don't use direcly!
//			Call UTIL_GetWeaponWorldScale() instead!
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesMultiplay::FlWeaponWorldScale(void)
{
	return 1.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Any conditions inhibiting the respawning of this weapon?
// Input  : *pWeapon -
// Output : int
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::WeaponShouldRespawn(CBasePlayerItem *pWeapon)
{
	return 1;// don't do item internal-only checks here!
}

//-----------------------------------------------------------------------------
// Purpose: Can this player have specified item?
// Input  : *pPlayer -
//			*pItem -
// Output : FALSE if the player is not allowed to pick up this weapon
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pItem)
{
	return CGameRules::CanHavePlayerItem(pPlayer, pItem);
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pPlayer -
//			*pItem -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::CanHaveItem(CBasePlayer *pPlayer, CItem *pItem)
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pPlayer -
//			*pItem -
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem)
{
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pItem -
// Output : int
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::ItemShouldRespawn(CItem *pItem)
{
	if (pItem->pev->spawnflags & SF_NORESPAWN)
		return 0;

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: At what time in the future may this Item respawn?
// Input  : *pItem -
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesMultiplay::FlItemRespawnTime(CItem *pItem)
{
	return gpGlobals->time + mp_itm_resp_time.value;// XDM
}

//-----------------------------------------------------------------------------
// Purpose: Some game variations may choose to randomize spawn locations
// Input  : *pItem -
// Output : Vector
//-----------------------------------------------------------------------------
Vector CGameRulesMultiplay::VecItemRespawnSpot(CItem *pItem)
{
	return pItem->pev->origin;
}

//-----------------------------------------------------------------------------
// Purpose: Player picked up some ammo
// Input  : *pPlayer -
//			*szName -
//			iCount -
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, const int &iCount)
{
}

//-----------------------------------------------------------------------------
// Purpose: Entity restrictions may apply here
// Input  : *pEntity -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::IsAllowedToSpawn(CBaseEntity *pEntity)
{
// players never get here	if (pEntity->IsPlayer())		return true;

	if (!FAllowMonsters() && pEntity->IsMonster())
		return false;

	if (mp_nofriction.value > 0 && FClassnameIs(pEntity->pev, "func_friction"))// XDM: thissux!
		return false;

	if (FClassnameIs(pEntity->pev, "env_fade"))// XDM: thissux!
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Any conditions inhibiting the respawning of this ammo?
// Input  : *pAmmo -
// Output : int 
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::AmmoShouldRespawn(CBasePlayerAmmo *pAmmo)
{
	return 1;// don't do item internal-only checks here!
}

//-----------------------------------------------------------------------------
// Purpose: Ammo respawn time (absolute value, gpGlobals->time)
// Input  : *pAmmo -
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesMultiplay::FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo)
{
	return gpGlobals->time + mp_itm_resp_time.value;
}

//-----------------------------------------------------------------------------
// Purpose: Wall-mounted charger renew time
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesMultiplay::FlHealthChargerRechargeTime(void)
{
	return mp_itm_resp_time.value;
}

//-----------------------------------------------------------------------------
// Purpose: Wall-mounted charger renew time
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesMultiplay::FlHEVChargerRechargeTime(void)
{
	return mp_itm_resp_time.value;
}

//-----------------------------------------------------------------------------
// Purpose: which weapons should be packed and dropped
// Input  : *pPlayer -
// Output : int 
//-----------------------------------------------------------------------------
int CGameRulesMultiplay::DeadPlayerWeapons(CBasePlayer *pPlayer)
{
	return GR_PLR_DROP_GUN_NO;
}

//-----------------------------------------------------------------------------
// Purpose: which ammo should be packed and dropped
// Input  : *pPlayer -
// Output : int 
//-----------------------------------------------------------------------------
int CGameRulesMultiplay::DeadPlayerAmmo(CBasePlayer *pPlayer)
{
	return GR_PLR_DROP_AMMO_NO;
}

//-----------------------------------------------------------------------------
// Purpose: returns next available player spawn spot
// Input  : *pPlayer -
// Output : CBaseEntity
//-----------------------------------------------------------------------------
CBaseEntity *CGameRulesMultiplay::PlayerUseSpawnSpot(CBasePlayer *pPlayer)
{
	CBaseEntity *pSpawnSpot = CGameRules::PlayerUseSpawnSpot(pPlayer);
	if (pSpawnSpot && pSpawnSpot->pev->target)
		FireTargets(STRING(pSpawnSpot->pev->target), pPlayer, pPlayer, USE_TOGGLE, 0);

	return pSpawnSpot;
}

//-----------------------------------------------------------------------------
// Purpose: XDM3037
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesMultiplay::GetPlayerMaxHealth(void)
{
	return MAX_PLAYER_HEALTH;// 200 HP
}

//-----------------------------------------------------------------------------
// Purpose: determines relationship between given player and entity
// Input  : *pPlayer -
//			*pTarget -
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesMultiplay::PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget)
{
	if (pTarget->IsBSPModel())// XDM3035: ?
		return GR_NEUTRAL;

	return GR_NOTTEAMMATE;
}

//-----------------------------------------------------------------------------
// Purpose: Should texture hit sounds be played?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::PlayTextureSounds(void)
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Should footstep sounds be played?
// Input  : *pPlayer -
//			fvol -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::PlayFootstepSounds(CBasePlayer *pPlayer, float fvol)
{
	if (pPlayer->IsOnLadder() || pPlayer->pev->velocity.Length2D() > 220.0f)
		return true;  // only make step sounds in multiplayer if the player is moving fast enough

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Allow players to use flashlight?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::FAllowFlashlight(void)
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Allow monsters in game?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::FAllowMonsters(void)
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Are effects allowed on this server?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::FAllowEffects(void)
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Is dynamic music (events, not playlist!) allowed on this server?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::FAllowMapMusic(void)
{
	if (IsGameOver())
		return false;
	else
		return (mp_allowmusicevents.value > 0.0f);
}

//-----------------------------------------------------------------------------
// Purpose: Are players allowed to switch to spectator mode in game? (cheat!)
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::FAllowSpectatorChange(CBasePlayer *pPlayer)
{
	if (IsGameOver())
		return false;
	else
		return (mp_allowspectators.value > 0.0f && mp_spectoggle.value > 0.0f);
}

//-----------------------------------------------------------------------------
// Purpose: EndOfGame. Show score board, camera shows the winner
// Input  : *pWinner -
//			*pInFrameEntity - some secondary entity to show in focus
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::GoToIntermission(CBasePlayer *pWinner, CBaseEntity *pInFrameEntity)
{
	if (IsGameOver())
		return;  // intermission has already been triggered, so ignore.

	ALERT(at_aiconsole, "CGameRulesMultiplay::GoToIntermission(%s, %s)\n", pWinner?STRING(pWinner->pev->netname):"NULL", pInFrameEntity?STRING(pInFrameEntity->pev->targetname):"NULL");

	m_pIntermissionEntity1 = pWinner;
	m_pIntermissionEntity2 = pInFrameEntity;
	int winner_index;

	if (pWinner)
	{
		winner_index = pWinner->entindex();
		pWinner->pev->flags |= FL_DRAW_ALWAYS;// XDM3035a: send even if outside of PVS
		pWinner->pev->effects = EF_BRIGHTLIGHT;// XDM3035c: forget all other effects
		m_iLeader = winner_index;// XDM3035a: update this!

//		TODO: find all clients outside winner's PVS and mark them with EF_NODRAW FNullEnt(FIND_CLIENT_IN_PVS(edict()))
	}
	else
		winner_index = 0;

	// spectator mode to move camera
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)
		{
			if (pPlayer->IsOnTrain())
			{
				CBaseDelay *pTrain = pPlayer->GetControlledTrain();
				if (pTrain)
					pTrain->Use(pPlayer, pPlayer, USE_OFF, 0.0f);// STOP the train itself
			}
			UTIL_SetOrigin(pPlayer->pev, pPlayer->pev->origin);
			pPlayer->pev->framerate = 0;// stop!
			pPlayer->pev->velocity = g_vecZero;
			pPlayer->pev->speed = 0;
			pPlayer->pev->iuser1 = OBS_INTERMISSION;
			if (pWinner)
			{
				pPlayer->m_hObserverTarget = pWinner;// XDM3035c: use his PVS
				pPlayer->pev->iuser2 = winner_index;
			}
			else
				pPlayer->pev->iuser2 = 0;

			if (pInFrameEntity)
				pPlayer->pev->iuser3 = pInFrameEntity->entindex();
			else
				pPlayer->pev->iuser3 = 0;

/*UNDONE			MESSAGE_BEGIN(((sv_reliability.value > 1)?MSG_ALL:MSG_BROADCAST), gmsgPlayerStats);
				WRITE_BYTE(i);
				for (int c = 0; c <= SCORE_COMBO_MAX; ++c)
				{
					WRITE_SHORT(pPlayer->m_NumCombos[c]);
				}
				WRITE_SHORT(pPlayer->m_iLastScoreAward);
//no need				WRITE_SHORT(pPlayer->m_iBestScoreAward);
				WRITE_SHORT(pPlayer->m_iComboBreakerCount);
				WRITE_SHORT(pPlayer->m_iRevengeCount);
				WRITE_SHORT(pPlayer->m_iFailCount);
//				WRITE_LONG(total_score);
			MESSAGE_END();*/
		}
	}

	FireTargets("game_roundend", g_pWorld, g_pWorld, USE_TOGGLE, 0);

	if (sv_lognotice.value > 0)// XDM
	{
		if (pWinner)
		{
			UTIL_LogPrintf("ENDGAME %s at %s Winner: \"%s<%i><%s><%s>\", team %d\n",
				g_pGameRules->GetGameDescription(),
				STRING(gpGlobals->mapname),
				STRING(pWinner->pev->netname),
				GETPLAYERUSERID(pWinner->edict()),
				GETPLAYERAUTHID(pWinner->edict()),
				g_pGameRules->GetTeamName(pWinner->pev->team),
				pWinner->pev->team);
		}
		else
		{
			UTIL_LogPrintf("ENDGAME %s at %s\n", g_pGameRules->GetGameDescription(), STRING(gpGlobals->mapname));
		}
	}

	MESSAGE_BEGIN(MSG_ALL, SVC_INTERMISSION);
	MESSAGE_END();

	// XDM3035 TESTME?
	MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
		WRITE_BYTE(9);// command length in bytes
		WRITE_BYTE(DRC_CMD_EVENT);
		WRITE_SHORT(winner_index);// index number of primary entity
		WRITE_SHORT(pInFrameEntity?pInFrameEntity->entindex():0);// index number of secondary entity
		WRITE_LONG(15 | DRC_FLAG_FINAL);// eventflags (priority and flags)
	MESSAGE_END();

	TEAM_ID winner_team = pWinner?(pWinner->pev->team):GetBestTeam();
	UTIL_ClientPrintAll(HUD_PRINTTALK, "* #MP_WIN_TEXT", pWinner?(g_pGameRules->IsTeamplay()?GetTeamName(winner_team):STRING(pWinner->pev->netname)):"#NOBODY");

	// bounds check
	int inttime = (int)mp_chattime.value;// XDM
	if (inttime < 1)
		CVAR_DIRECT_SET(&mp_chattime, "1");
	else if (inttime > MAX_INTERMISSION_TIME)
		CVAR_DIRECT_SET(&mp_chattime, UTIL_dtos1(MAX_INTERMISSION_TIME));

	inttime = (int)mp_chattime.value;
	// TODO: send to clients!
	MESSAGE_BEGIN(MSG_ALL, gmsgGRInfo);
		WRITE_BYTE(winner_index);
		WRITE_BYTE(winner_team);
		WRITE_SHORT(0);
		WRITE_SHORT(inttime);
	MESSAGE_END();

	m_flIntermissionEndTime = gpGlobals->time + inttime;
	m_flIntermissionStartTime = gpGlobals->time;

//	g_fGameOver = true;
	m_bGameOver = true;
	m_bReadyButtonsHit = false;

	UTIL_ScreenFadeAll(Vector(255,255,255), 1.0f, 0.25f, 200, FFADE_IN);// XDM3035: cool effect and also disables all other fade effects for everyone
}

//-----------------------------------------------------------------------------
// Purpose: instant end of the game with intermission
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::EndMultiplayerGame(void)
{
//	DBG_FORCEBREAK
	if (IsGameOver())
		return;

//	if (g_pGameRules->IsTeamplay())
	TEAM_ID bestteam = g_pGameRules->GetBestTeam();
	GoToIntermission(g_pGameRules->GetBestPlayer(bestteam), NULL/*m_pLastVictim may contain random person*/);
}

//-----------------------------------------------------------------------------
// Purpose: Server is changing to a new level, check mapcycle.txt for map name and setup info
//  UPDATE: XDM3034: search for the current map in list and continue from there!
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::ChangeLevel(void)
{
	static char szPreviousMapCycleFile[256];
	static mapcycle_t mapcycle;

	char szNextMap[32];
	char szFirstMapInList[32];
	char szCommands[1280];// XDM3035a: was 1500
	char szRules[1280];// here too
	int minplayers = 0, maxplayers = 0;
	int	curplayers;
	bool do_cycle = true;
	// find the map to change to
	char *mapcfile = NULL;

	if (mp_gamerules.value > 0)// XDM: otherwise, auto-select game type
	{
		switch (g_pGameRules->GetGameType())
		{
		default:
		case GT_DEATHMATCH:// LMS and TEAMPLAY too
		{
			mapcfile = "mapcycle.dm";
			strcpy(szFirstMapInList, "DM_000");// the absolute default level
		}
		break;
		case GT_COOP:
		{
			mapcfile = "mapcycle.cop";
			strcpy(szFirstMapInList, "COP_000");
		}
		break;
		case GT_CTF:
		{
			mapcfile = "mapcycle.ctf";
			strcpy(szFirstMapInList, "CTF_000");
		}
		break;
		case GT_DOMINATION:
		{
			mapcfile = "mapcycle.dom";
			strcpy(szFirstMapInList, "DOM_000");
		}
		break;
/*		case GT_ASSAULT:
		{
			mapcfile = "mapcycle.as";
			strcpy(szFirstMapInList, "AS_000");
		}
		break;*/
		}
	}
	if (mapcfile == NULL)
		mapcfile = (char *)CVAR_GET_STRING("mapcyclefile");

	if (!ASSERT(mapcfile != NULL))
		return;

	ALERT(at_logged, "SV: Using map cycle file '%s'\n", mapcfile);

	szCommands[0] = '\0';
	szRules[0] = '\0';

	curplayers = CountPlayers();

	// Has the map cycle filename changed?
	if (stricmp(mapcfile, szPreviousMapCycleFile))
	{
		strncpy(szPreviousMapCycleFile, mapcfile, 256);// XDM: buffer overrun protection
		DestroyMapCycle(&mapcycle);
		if (ReloadMapCycleFile(mapcfile, &mapcycle) <= 0 || mapcycle.items == NULL)
		{
			ALERT(at_console, "! Unable to load map cycle file %s\n", mapcfile);
			do_cycle = false;
		}
		// NOTE: we don't need to specially search for current map in freshly loaded list because it is already done by ReloadMapCycleFile()
	}
	else// XDM3035: we did not change the list file but played an out-of-order (but may be still in list) map, so try to find it and continue playing
	{
		mapcycle_item_t *found = Mapcycle_Find(&mapcycle, STRING(gpGlobals->mapname));
		if (found)
			mapcycle.next_item = found->next;
//		else
//			mapcycle.next_item = mapcycle.items;// restart the cycle?
	}

	if (do_cycle && mapcycle.items)
	{
		bool keeplooking = false;
		bool found = false;
		mapcycle_item_s *item = NULL;
		// Assume current map
		memset(szFirstMapInList, 0, sizeof(szFirstMapInList));
		memset(szNextMap, 0, sizeof(szNextMap));
		// Traverse list
		for (item = mapcycle.next_item; item->next != mapcycle.next_item; item = item->next)
		{
			keeplooking = false;
			ASSERT(item != NULL);

			if (item->minplayers != 0)
			{
				if (curplayers >= item->minplayers)
				{
					found = true;
					minplayers = item->minplayers;
				}
				else
					keeplooking = true;
			}
			if (item->maxplayers != 0)
			{
				if (curplayers <= item->maxplayers)
				{
					found = true;
					maxplayers = item->maxplayers;
				}
				else
					keeplooking = true;
			}

			if (keeplooking)
				continue;

			found = true;
			break;
		}

		if (!found)
			item = mapcycle.next_item;

		// Increment next item pointer
		mapcycle.next_item = item->next;
		// Perform logic on current item
		strcpy(szNextMap, item->mapname);
		ExtractCommandString(item->rulebuffer, szCommands);
		strcpy(szRules, item->rulebuffer);
	}

	if (!IS_MAP_VALID(szNextMap))
		strcpy(szNextMap, szFirstMapInList);

//	g_fGameOver = true;
	m_bGameOver = true;
	m_pIntermissionEntity1 = NULL;
	m_pIntermissionEntity2 = NULL;
	m_flIntermissionEndTime = 0.0f;

	ALERT(at_console, "CHANGE LEVEL: %s\n", szNextMap);
	if (minplayers > 0 || maxplayers > 0)
		ALERT(at_console, "PLAYER COUNT: min %i max %i current %i, RULES: %s\n", minplayers, maxplayers, curplayers, szRules);

	// XDM3035 crash recovery
//	g_pServerAutoFile ?

	CHANGE_LEVEL(szNextMap, NULL);
	if (strlen(szCommands) > 0)
		SERVER_COMMAND(szCommands);
}


#define MAX_MOTD_CHUNK	  60// chars per packet
//#define MAX_MOTD_LENGTH   1280// (MAX_MOTD_CHUNK * 4) XDM3035: reduced a little

//-----------------------------------------------------------------------------
// Purpose:Send MOTD to client in pieces
// Input  : *pClient -
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::SendMOTDToClient(CBasePlayer *pClient)
{
	// send the server name
	MESSAGE_BEGIN(MSG_ONE, gmsgServerName, NULL, pClient->edict());
		WRITE_STRING(CVAR_GET_STRING("hostname"));
	MESSAGE_END();

	// read from the MOTD.txt file
	int length = 0, char_count = 0;
	char *pFileList;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME((char *)CVAR_GET_STRING("motdfile"), &length);

	ASSERT(pFileList != NULL);

	// Send the message of the day
	// read it chunk-by-chunk,  and send it in parts
	while (pFileList && *pFileList && char_count < MAX_MOTD_LENGTH)
	{
		char chunk[MAX_MOTD_CHUNK+1];

		if (strlen(pFileList) < MAX_MOTD_CHUNK)
		{
			strcpy(chunk, pFileList);
		}
		else
		{
			strncpy(chunk, pFileList, MAX_MOTD_CHUNK);
			chunk[MAX_MOTD_CHUNK] = 0;		// strncpy doesn't always append the null terminator
		}
		char_count += strlen(chunk);

		if (char_count < MAX_MOTD_LENGTH)
			pFileList = aFileList + char_count;
		else
			*pFileList = 0;

		MESSAGE_BEGIN(MSG_ONE, gmsgMOTD, NULL, pClient->edict());
			WRITE_BYTE(*pFileList ? FALSE : TRUE);// FALSE means there is still more message to come
			WRITE_STRING(chunk);
		MESSAGE_END();
	}
	FREE_FILE(aFileList);
}

//-----------------------------------------------------------------------------
// Purpose: Is this server full?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesMultiplay::ServerIsFull(void)
{
	if (CountPlayers() >= gpGlobals->maxClients)
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Get primary intermission camera target, if available
//-----------------------------------------------------------------------------
CBaseEntity	*CGameRulesMultiplay::GetIntermissionActor1(void)
{
	return m_pIntermissionEntity1;
}

//-----------------------------------------------------------------------------
// Purpose: Get secondary intermission camera target, if available
//-----------------------------------------------------------------------------
CBaseEntity	*CGameRulesMultiplay::GetIntermissionActor2(void)
{
	return m_pIntermissionEntity2;
}

//-----------------------------------------------------------------------------
// Purpose: dump debug info to console
//-----------------------------------------------------------------------------
void CGameRulesMultiplay::DumpInfo(void)
{
	ALERT(at_console, "SV:pl id\tteam\tfrg\tdth\tcombo\tname\n");
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
		if (pPlayer)// XDM
		{
			ALERT(at_console, " #%d\t%d\t%d\t%d\t%d\t(%s)\n", i,
				pPlayer->pev->team,
				(int)pPlayer->pev->frags,
				pPlayer->m_iDeaths,
				pPlayer->m_iScoreCombo,
				STRING(pPlayer->pev->netname));
		}
	}
	CBasePlayer *pBestPlayer = g_pGameRules->GetBestPlayer(g_pGameRules->GetBestTeam());
	if (pBestPlayer)
		ALERT(at_console, " Best Player: %d (%s)\n", pBestPlayer->entindex(), STRING(pBestPlayer->pev->netname));
	else
		ALERT(at_console, " Best Player: none\n");
}
