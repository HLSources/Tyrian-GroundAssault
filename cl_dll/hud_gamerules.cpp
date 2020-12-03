#include "hud.h"
#include "cl_util.h"
#include "vgui_XDMViewport.h"
#include "vgui_ScorePanel.h"
#include "pm_shared.h"


//-----------------------------------------------------------------------------
// Purpose: Is local player a spectator?
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHud::IsSpectator(void)
{
	if (g_iUser1 != OBS_NONE)// NO! We may check this during intermission too!- && g_iUser1 != OBS_INTERMISSION)
	{
		cl_entity_t *localplayer = gEngfuncs.GetLocalPlayer();
		if (localplayer)
			return (g_PlayerExtraInfo[localplayer->index].observer > 0);
//			return (g_IsSpectator[localplayer->index] > 0);
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: IntermissionStart (end of game) // XDM3035
//-----------------------------------------------------------------------------
void CHud::IntermissionStart(void)
{
	m_Ammo.UpdateCrosshair(0,0);// XDM

	if (gViewPort)
	{
		gViewPort->HideCommandMenu();
		gViewPort->HideVGUIMenu();
//ShowScoreBoard includes this		gViewPort->GetScoreBoard()->UpdateTitle();// XDM3035a: set the end-of-the-match title
		gViewPort->ShowScoreBoard();
		gViewPort->UpdateSpectatorPanel();
	}

	CenterPrint("");// hopefully this will clear any "sudden messages"

//	m_flTimeLeft = 0;
	m_iScoreLeft = 0;
	m_iTimeLeftLast = 0;
	m_iScoreLeftLast = 0;

	if (g_iUser1 == OBS_INTERMISSION)// XDM3035 TODO: revisit
		GameRulesEndGame();
}

//-----------------------------------------------------------------------------
// Purpose: Intermission ends (does not work as expected)
//-----------------------------------------------------------------------------
void CHud::IntermissionEnd(void)
{
//	if (g_pRenderManager)// XDM3035a
//		g_pRenderManager->DeleteAllSystems();

	if (gViewPort)
	{
		gViewPort->HideCommandMenu();
		gViewPort->HideScoreBoard();
		gViewPort->UpdateSpectatorPanel();
	}
	m_Ammo.UpdateCrosshair(1,0);// XDM
	m_flNextAnnounceTime = 0.0f;// XDM3035
}

//-----------------------------------------------------------------------------
// XDM3035a
//-----------------------------------------------------------------------------
void CHud::CheckRemainingScoreAnnouncements(void)
{
	if (m_iIntermission > 0)
		return;
	if (gEngfuncs.GetMaxClients() <= 1)
		return;

/*	if (m_pCvarScoreLeft)// a local game, not connected to a remote server
	{
		m_iScoreLeft = (int)m_pCvarScoreLeft->value;
	}*/

	if (m_iScoreLeft > 0 && m_iScoreLeft != m_iScoreLeftLast)
	{
//		CON_DPRINTF("CHud::CheckRemainingScoreAnnouncements(%d)\n", m_iScoreLeft);
		// GRInfo messages are already filtered, but there are other sources which can trigger this more often
		if (m_iScoreLeft == 30 || m_iScoreLeft == 20 || m_iScoreLeft == 10 || m_iScoreLeft == 5 || m_iScoreLeft == 3 || m_iScoreLeft == 1)
		{
//			if (m_flNextAnnounceTime <= m_flTime)// avoid to be triggered more than once per second
			{
				char *msgname = "MP_SCORELEFT";
				if (m_iGameType == GT_CTF || m_iGameType == GT_DOMINATION)
					msgname = "MP_SCORELEFT_PTS";
				else if (m_iGameType == GT_COOP)
				{
					if (m_iGameMode == COOP_MODE_LEVEL)
						msgname = "MP_SCORELEFT_TOUCH";
					else
						msgname = "MP_SCORELEFT_MONS";
				}

				client_textmessage_t *msg = TextMessageGet(msgname);
				if (msg)
				{
					m_MessageScoreLeft = *msg;// copy localized message
//					memcpy(&m_MessageScoreLeft, msg, sizeof(client_textmessage_t));// copy localized message
					//strncpy(m_szMessageScoreLeft, msg->pMessage, ANNOUNCEMENT_MSG_LENGTH);// store message TEXT
					sprintf(m_szMessageScoreLeft, msg->pMessage, m_iScoreLeft);// format the string
					m_szMessageScoreLeft[ANNOUNCEMENT_MSG_LENGTH-1] = 0;
					m_MessageScoreLeft.pMessage = m_szMessageScoreLeft;
					m_Message.MessageAdd(&m_MessageScoreLeft);
				}
//				else
					CON_PRINTF("- %d score points left\n", m_iScoreLeft);

//				m_flNextAnnounceTime = m_flTime + 3.0f;
//				m_iScoreLeftLast = m_iScoreLeft;
			}
		}
		m_iScoreLeftLast = m_iScoreLeft;
	}
}

//-----------------------------------------------------------------------------
// XDM3035a
//-----------------------------------------------------------------------------
void CHud::CheckRemainingTimeAnnouncements(void)
{
	if (m_iIntermission > 0)
		return;

	int timer = 0;

/*	if (g_pCvarTimeLeft)// a local game, not connected to a remote server
	{
		timer = (int)g_pCvarTimeLeft->value;
		m_iTimeLeft = timer;
//		CON_PRINTF("CHud::CheckRemainingTimeAnnouncements(%d) m_pCvarTimeLeft available\n", timer);
	}
	else*/
		timer = (int)m_flTimeLeft;

/*	cl_entity_t *pWorld = gEngfuncs.GetEntityByIndex(0);	// get world
	if (pWorld)
	{
		timer = pWorld->curstate.impacttime;
		CON_PRINTF("CHud::Think(%d) pWorld\n", timer);
	}
*/
	if (timer > 0 && timer != m_iTimeLeftLast)
	{
//		CON_DPRINTF("CHud::CheckRemainingTimeAnnouncements(%d)\n", timer);
		if (timer == 10 || timer == 30 || timer == 60 || timer == 180 || timer == 300 || timer == 600)
		{
			if (m_flNextAnnounceTime <= m_flTime)// avoid to be triggered more than once per second
			{
				char msgname[32];// message/sound name + NULLterm
				sprintf(msgname, "MP_TIMELEFT%d\0", timer);
				client_textmessage_t *msg = TextMessageGet(msgname);
				if (msg)
				{
					m_MessageTimeLeft = *msg;// copy localized message
//					memcpy(&m_MessageTimeLeft, msg, sizeof(client_textmessage_t));// copy localized message
					strncpy(m_szMessageTimeLeft, msg->pMessage, ANNOUNCEMENT_MSG_LENGTH);// store message TEXT
					m_szMessageTimeLeft[ANNOUNCEMENT_MSG_LENGTH-1] = 0;
					m_MessageTimeLeft.pMessage = m_szMessageTimeLeft;
					m_Message.MessageAdd(&m_MessageTimeLeft);
				}
//				else
					CON_PRINTF("- %d seconds left\n", timer);

				if (g_pCvarAnnouncer->value > 0.0f)
				{
//						sprintf(msgname, "announcer/timeleft%d.wav\0", timer);
					sprintf(msgname, "!CTR%d\0", timer);
					PlaySoundAnnouncer(msgname, 3.0f);
				}
			}
		}
		else if (timer <= 5)
		{
			if (g_pCvarAnnouncer->value > 0.0f)
			{
				char msgname[16];
				sprintf(msgname, "!CTR%d\0", timer);
				PlaySoundAnnouncer(msgname, 0.9f);
			}
		}
		m_iTimeLeftLast = timer;
	}
}


//-----------------------------------------------------------------------------
// Purpose: GameRulesEndGame
//-----------------------------------------------------------------------------
void CHud::GameRulesEndGame(void)
{
	if (g_iUser2 > 0)// winner
	{
		cl_entity_t *pWinner = gEngfuncs.GetEntityByIndex(g_iUser2);
		if (pWinner && pWinner->player)
		{
			char msgname[32];// + NULLterm
			cl_entity_t *localplayer = gEngfuncs.GetLocalPlayer();
			if (pWinner == localplayer || (IsTeamGame(gHUD.m_iGameType) && localplayer->curstate.team != TEAM_NONE && g_PlayerExtraInfo[g_iUser2].teamnumber == localplayer->curstate.team))
			{
				sprintf(msgname, "MP_WIN_LP\0");
				client_textmessage_t *msg = TextMessageGet(msgname);
				if (msg)
				{
					CON_PRINTF("* %s\n", msg->pMessage);
					if (g_pCvarTFX->value > 0.0f)
					{
						m_MessageAnnouncement = *msg;// copy localized message
//						memcpy(&m_MessageAnnouncement, msg, sizeof(client_textmessage_t));// copy localized message
						strncpy(m_szMessageAnnouncement, msg->pMessage, ANNOUNCEMENT_MSG_LENGTH);// store message TEXT
						m_szMessageAnnouncement[ANNOUNCEMENT_MSG_LENGTH-1] = 0;
						m_MessageAnnouncement.pMessage = m_szMessageAnnouncement;
						m_Message.MessageAdd(&m_MessageAnnouncement);
					}
				}
				else
					CON_PRINTF("* You are the winner!\n");

//				sprintf(msgname, "!MP_WINNER\0");
				PlaySoundAnnouncer("!MP_WINNER\0", 5);
			}
			else// winner is not me
			{
				if (IsTeamGame(gHUD.m_iGameType))
					sprintf(msgname, "MP_WIN_TEAM\0");
				else
					sprintf(msgname, "MP_WIN_PLAYER\0");

				client_textmessage_t *msg = TextMessageGet(msgname);
				if (msg)
				{
					// insert player/team name
					sprintf(m_szMessageAnnouncement, msg->pMessage, IsTeamGame(gHUD.m_iGameType)?gViewPort->GetTeamName(g_PlayerExtraInfo[g_iUser2].teamnumber):g_PlayerInfoList[g_iUser2].name);
					CON_PRINTF("* %s\n", m_szMessageAnnouncement);
					if (g_pCvarTFX->value > 0.0f)
					{
						m_MessageAnnouncement = *msg;// copy localized message
//						memcpy(&m_MessageAnnouncement, msg, sizeof(client_textmessage_t));// copy localized message
//						strncpy(m_szMessageAnnouncement, msg->pMessage, ANNOUNCEMENT_MSG_LENGTH);// store message TEXT
						m_szMessageAnnouncement[ANNOUNCEMENT_MSG_LENGTH-1] = 0;
						m_MessageAnnouncement.pMessage = m_szMessageAnnouncement;
						m_Message.MessageAdd(&m_MessageAnnouncement);
					}
				}
				else
					CON_PRINTF("* %s is the winner!\n", IsTeamGame(gHUD.m_iGameType)?gViewPort->GetTeamName(g_PlayerExtraInfo[g_iUser2].teamnumber):g_PlayerInfoList[g_iUser2].name);

				if (!gHUD.IsSpectator())//UTIL_IsSpectator(localplayer->index))// spectators can't loose :)
				{
//					sprintf(msgname, "!MP_LOST\0");
					PlaySoundAnnouncer("!MP_LOST\0", 5);

					// store this message in m_MessageAward/m_szMessageAward so it won't interfere
//					if (g_PlayerExtraInfo[g_iUser2].teamnumber != localplayer->curstate.team)// my team lost
					{
//							char msgname[32];// + NULLterm
						sprintf(msgname, "MP_LOST_LP\0");
						client_textmessage_t *msg = TextMessageGet(msgname);
						if (msg)
						{
							CON_PRINTF("* %s\n", msg->pMessage);
							if (g_pCvarTFX->value > 0.0f)
							{
								m_MessageAward = *msg;// copy localized message
//								memcpy(&m_MessageAward, msg, sizeof(client_textmessage_t));// copy localized message
//								ConsolePrint(msg->pMessage);
								strncpy(m_szMessageAward, msg->pMessage, DEATHNOTICE_MSG_LENGTH);// store message TEXT
								m_szMessageAward[DEATHNOTICE_MSG_LENGTH-1] = 0;
								m_MessageAward.pMessage = m_szMessageAward;
								m_Message.MessageAdd(&m_MessageAward);
							}
						}
						else
							CON_PRINTF("* You have lost the match!\n");

					}
				}
			}

			ASSERT(gViewPort->GetScoreBoard() != NULL);

			int bp = gViewPort->GetScoreBoard()->GetBestPlayer();
			int bt = gViewPort->GetScoreBoard()->GetBestTeam();
			if (g_iUser2 != bp || (IsTeamGame(gHUD.m_iGameType) && (g_PlayerExtraInfo[g_iUser2].teamnumber != bt)))
			{
				CON_DPRINTF(" ---> WINNER INFO MISMATCH!!! PL: sv %d cl %d, TM: sv %d cl %d <---\n", g_iUser2, bp, g_PlayerExtraInfo[g_iUser2].teamnumber, bt);
				DBG_FORCEBREAK
#ifdef _DEBUG// step into!
				bp = gViewPort->GetScoreBoard()->GetBestPlayer();
				bt = gViewPort->GetScoreBoard()->GetBestTeam();
#endif
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: GameRules Event handler
// Warning: LOTS of memory corruption possibilities! Also, mind the compiler...
// Input  : gameevent - GAME_EVENT_UNKNOWN
//			client1 - 
//			client2 - 
// Output : int
//-----------------------------------------------------------------------------
void CHud::GameRulesEvent(int gameevent, short data1, short data2)
{
	if (g_pCvarTFX->value <= 0.0f)
		return;

	// IMPORTANT: no checks done on data1/2 because for now they represent ONLY PLAYER indexes
	char astr[32];// + NULLterm
	client_textmessage_t *msg = NULL;
	astr[0] = 0;

	// switch (gameevent)
	if (gameevent == GAME_EVENT_AWARD)
//	case GAME_EVENT_AWARD:
	{
		// data1 - player
		// data2 - award
		if (g_PlayerInfoList[data1].thisplayer)// should always be true
		{
			if (data2 > 1)
			{
//					m_PlayerStats.AddAward(AWARD_KILL, data2);
				sprintf(astr, "KILL%d\0", data2);
				msg = TextMessageGet(astr);
				if (msg)
				{
					m_MessageAward = *msg;// copy localized message
//						memcpy(&m_MessageAward, msg, sizeof(client_textmessage_t));
					strcpy(m_szMessageAward, msg->pMessage);
					m_szMessageAward[DEATHNOTICE_MSG_LENGTH-1] = 0;
					m_MessageAward.pMessage = m_szMessageAward;
//							m_MessageAward.holdtime = SCORE_AWARD_TIME;
					gHUD.m_Message.MessageAdd(&m_MessageAward);
				}
				else
					sprintf(m_szMessageAward, "GAME_EVENT_AWARD %d %d\n", data1, data2);

				ConsolePrint(m_szMessageAward);// log to console too

				if (g_pCvarAnnouncer->value > 0.0f)
				{
					sprintf(astr, "announcer/score%d.wav\0", data2);
					PlaySoundAnnouncer(astr, 4);
				}
			}
		}
	}
//		break;
else if (gameevent == GAME_EVENT_COMBO)
//	case GAME_EVENT_COMBO:
	{
		// data1 - killer
		// data2 - victim
		if (g_PlayerInfoList[data1].thisplayer)
		{
//			CenterPrint("You did a combo!\n");
//TODO			if (m_pCvarDNEcho->value > 0.0f)
//TODO				ConsolePrint(szConsoleString);
			sprintf(astr, "COMBO_LOCAL%d\0", data2);
//			m_PlayerStats.AddAward(AWARD_COMBO, data2);
		}
		else
		{
//			CenterPrint("Someone did a combo!\n");
			sprintf(astr, "COMBO_OTHER%d\0", data2);
		}

		if (astr[0])
		{
			msg = TextMessageGet(astr);
			if (msg && msg->pMessage)
			{
				m_MessageCombo = *msg;// copy localized message
//				memcpy(&m_MessageCombo, msg, sizeof(client_textmessage_t));
				_snprintf(m_szMessageCombo, DEATHNOTICE_MSG_LENGTH, msg->pMessage, g_PlayerInfoList[data1].name);// add player name
//				strcpy(m_szMessageCombo, msg->pMessage);
				m_szMessageCombo[DEATHNOTICE_MSG_LENGTH-1] = 0;
				m_MessageCombo.pMessage = m_szMessageCombo;
				gHUD.m_Message.MessageAdd(&m_MessageCombo);
			}
			else
				sprintf(m_szMessageCombo, "GAME_EVENT_COMBO %d %d\n", data1, data2);

			ConsolePrint(m_szMessageCombo);// log to console too
		}
		if (g_PlayerInfoList[data1].thisplayer && g_pCvarAnnouncer->value > 0.0f)
		{
			if (data2 == 5 && stricmp(g_PlayerInfoList[data1].model, "XInuYasha") == 0)// J4L
			{
				ConsolePrint("Doglike!\n");
				sprintf(astr, "announcer/combo%dd.wav\0", data2);
			}
			else
				sprintf(astr, "announcer/combo%d.wav\0", data2);

			PlaySoundAnnouncer(astr, 3);
		}
	}
//		break;
else if (gameevent == GAME_EVENT_COMBO_BREAKER)
//	case GAME_EVENT_COMBO_BREAKER:
	{
		// data1 - killer
		// data2 - victim
		if (data1 == data2)
		{
//				CenterPrint("%s completed combo with a braincrushing suicide!\n");
			sprintf(astr, "COMBOBREAK_SELF\0");
			if (IsValidPlayerIndex(data1) && g_PlayerInfoList[data1].thisplayer)
				PlaySoundAnnouncer("announcer/combofail.wav\0", 2);// you fail
		}
		else if (IsValidPlayerIndex(data1) && g_PlayerInfoList[data1].thisplayer)
		{
//				CenterPrint("COMBO REAKER!\n");
			sprintf(astr, "COMBOBREAK_LOCAL\0");
//				m_PlayerStats.AddAward(AWARD_COMBOBREAK);
			PlaySoundAnnouncer("announcer/combo0.wav\0", 2);
		}
		else
		{
//			if (g_PlayerInfoList[data2].thisplayer)
//			{
//				CenterPrint("Your combo was broken!\n");
//				sprintf(astr, "COMBO_THISPLAYER0\0");
//			}
//			else
			{
//				CenterPrint("%s's precious combo was ended by %s!\n");
				sprintf(astr, "COMBOBREAK_OTHER\0");
			}
		}

		if (astr[0])
		{
			msg = TextMessageGet(astr);
			if (msg && msg->pMessage)
			{
				char entstring1[32];
				if (gHUD.m_iGameType == GT_COOP && !IsValidPlayerIndex(data1))
					sprintf(entstring1, "monster %d", data1);
				else
					GetEntityPrintableName(data1, entstring1, 32);

				m_MessageCombo = *msg;// copy localized message
//				memcpy(&m_MessageCombo, msg, sizeof(client_textmessage_t));
				_snprintf(m_szMessageCombo, DEATHNOTICE_MSG_LENGTH, msg->pMessage, g_PlayerInfoList[data2].name, entstring1);// replace all '%s' with names// for all??
				m_szMessageCombo[DEATHNOTICE_MSG_LENGTH-1] = 0;
//				StripEndNewlineFromString(m_szMessageCombo);
				m_MessageCombo.pMessage = m_szMessageCombo;
				gHUD.m_Message.MessageAdd(&m_MessageCombo);
			}
			else
				sprintf(m_szMessageCombo, "GAME_EVENT_COMBO_BREAKER %d %d\n", data1, data2);

			ConsolePrint(m_szMessageCombo);// log to console too
		}
	}
//		break;
else if (gameevent == GAME_EVENT_FIRST_SCORE)
//	case GAME_EVENT_FIRST_SCORE:
	{
		// data1 - player
		if (g_PlayerInfoList[data1].thisplayer)
		{
			sprintf(astr, "FIRSTSCORE_LOCAL\0");// CenterPrint("You made the first score!\n");
//			m_PlayerStats.AddAward(AWARD_FIRSTSCORE);
			PlaySoundAnnouncer("announcer/firstscore.wav\0", 2);
		}
		else
			sprintf(astr, "FIRSTSCORE_OTHER\0");// CenterPrint("%s made the first score!\n");

		if (astr[0])
		{
			msg = TextMessageGet(astr);
			if (msg && msg->pMessage)
			{
				m_MessageCombo = *msg;// copy localized message
				//memcpy(&m_MessageCombo, msg, sizeof(client_textmessage_t));// store in separate place?
				_snprintf(m_szMessageCombo, DEATHNOTICE_MSG_LENGTH, msg->pMessage, g_PlayerInfoList[data1].name);// replace all '%s' with names
				m_szMessageCombo[DEATHNOTICE_MSG_LENGTH-1] = 0;
				m_MessageCombo.pMessage = m_szMessageCombo;
				gHUD.m_Message.MessageAdd(&m_MessageCombo);
			}
			else
				sprintf(m_szMessageCombo, "GAME_EVENT_FIRST_SCORE %d %d\n", data1, data2);

			ConsolePrint(m_szMessageCombo);// log to console too
		}
	}
//		break;
else if (gameevent == GAME_EVENT_TAKES_LEAD)
//	case GAME_EVENT_TAKES_LEAD:
	{
		// data1 - player (not in teamplay)
		// data2 - team
		if (g_PlayerInfoList[data1].thisplayer)
		{
			sprintf(astr, "TAKENLEAD_LOCAL\0");// CenterPrint("You took the lead!\n");
//			m_PlayerStats.AddAward(AWARD_TAKENLEAD);
			PlaySoundAnnouncer("announcer/takenlead.wav\0", 2);
		}
		else
			sprintf(astr, "TAKENLEAD_OTHER\0");// CenterPrint("%s takes the lead!\n");

		if (astr[0])
		{
			msg = TextMessageGet(astr);
			if (msg && msg->pMessage)
			{
				m_MessageCombo = *msg;// copy localized message
				//memcpy(&m_MessageCombo, msg, sizeof(client_textmessage_t));// store in separate place?
				if (IsTeamGame(gHUD.m_iGameType))
					_snprintf(m_szMessageCombo, DEATHNOTICE_MSG_LENGTH, msg->pMessage, g_TeamInfo[data2].name);// replace '%s' with team name
				else
					_snprintf(m_szMessageCombo, DEATHNOTICE_MSG_LENGTH, msg->pMessage, g_PlayerInfoList[data1].name);// replace '%s' with name

				m_szMessageCombo[DEATHNOTICE_MSG_LENGTH-1] = 0;
				m_MessageCombo.pMessage = m_szMessageCombo;
				gHUD.m_Message.MessageAdd(&m_MessageCombo);
			}
			else
				sprintf(m_szMessageCombo, "GAME_EVENT_TAKES_LEAD %d %d\n", data1, data2);

			ConsolePrint(m_szMessageCombo);// log to console too
		}
	}
//		break;
else if (gameevent == GAME_EVENT_REVENGE)
//	case GAME_EVENT_REVENGE:
	{
		// data1 - killer
		// data2 - victim
		if (g_PlayerInfoList[data1].thisplayer)
		{
			sprintf(astr, "REVENGE_LOCAL\0");// CenterPrint("Revenge!\n");
//			m_PlayerStats.AddAward(AWARD_REVENGE_LOCAL);
			PlaySoundAnnouncer("announcer/revenge.wav\0", 2);
		}
		else if (g_PlayerInfoList[data2].thisplayer)
		{
			astr[0] = 0;
//			sprintf(astr, "REVENGE_THISPLAYER\0");// CenterPrint("%s took revenge on you!\n");
//			m_PlayerStats.AddAward(AWARD_REVENGE_THISPLAYER);
		}
		else
			sprintf(astr, "REVENGE_OTHER\0");// CenterPrint("%s tasted sweet revenge on %s!\n");

		if (astr[0])
		{
			msg = TextMessageGet(astr);
			if (msg && msg->pMessage)
			{
				m_MessageCombo = *msg;// copy localized message
//				memcpy(&m_MessageCombo, msg, sizeof(client_textmessage_t));// store in separate place?
				_snprintf(m_szMessageCombo, DEATHNOTICE_MSG_LENGTH, msg->pMessage, g_PlayerInfoList[data1].name, g_PlayerInfoList[data2].name);// replace all '%s' with names
				m_szMessageCombo[DEATHNOTICE_MSG_LENGTH-1] = 0;
				m_MessageCombo.pMessage = m_szMessageCombo;
				gHUD.m_Message.MessageAdd(&m_MessageCombo);
			}
			else
				sprintf(m_szMessageCombo, "GAME_EVENT_REVENGE %d %d\n", data1, data2);

			ConsolePrint(m_szMessageCombo);// log to console too
		}
	}
//		break;
else if (gameevent == GAME_EVENT_LOOSECOMBO)
//	case GAME_EVENT_LOOSECOMBO:
	{
		if (IsValidPlayerIndex(data2) && g_PlayerInfoList[data2].thisplayer)
		{
			sprintf(astr, "LOOSECOMBO_LOCAL\0");// CenterPrint("FFFUUUUUUUUU-\n");
//				m_PlayerStats.AddAward(AWARD_LOOSECOMBO_LOCAL);
			PlaySoundAnnouncer("announcer/loosecombo.wav\0", 4);
		}
		else if (IsValidPlayerIndex(data1) && g_PlayerInfoList[data1].thisplayer)
		{
			sprintf(astr, "LOOSECOMBO_THISPLAYER\0");// CenterPrint("You doublekilled %s!\n");
//				m_PlayerStats.AddAward(AWARD_LOOSECOMBO_THISPLAYER);
			PlaySoundAnnouncer("announcer/uncombo.wav\0", 4);
		}
		else if (IsValidPlayerIndex(data1))// don't allow "NULL doublekilled %s!"
		{
			if (data1 == data2)
				sprintf(astr, "LOOSECOMBO_SELF\0");// CenterPrint("%s committed another suicide!\n");
			else
				sprintf(astr, "LOOSECOMBO_OTHER\0");// CenterPrint("%s doublekilled %s!\n");
		}

		if (astr[0])
		{
			msg = TextMessageGet(astr);
			if (msg && msg->pMessage)
			{
				char entstring1[32];
				if (gHUD.m_iGameType == GT_COOP && !IsValidPlayerIndex(data1))
					sprintf(entstring1, "monster %d", data1);
				else
					GetEntityPrintableName(data1, entstring1, 32);

				char entstring2[32];
				if (gHUD.m_iGameType == GT_COOP && !IsValidPlayerIndex(data2))
					sprintf(entstring2, "monster %d", data2);
				else
					GetEntityPrintableName(data2, entstring2, 32);

				m_MessageCombo = *msg;// copy localized message
//				memcpy(&m_MessageCombo, msg, sizeof(client_textmessage_t));// store in separate place?
//				sprintf(m_szMessageCombo, msg->pMessage, g_PlayerInfoList[data1].name, g_PlayerInfoList[data2].name);// replace all '%s' with names
				_snprintf(m_szMessageCombo, DEATHNOTICE_MSG_LENGTH, msg->pMessage, entstring1, entstring2);// replace all '%s' with names
				m_szMessageCombo[DEATHNOTICE_MSG_LENGTH-1] = 0;
				m_MessageCombo.pMessage = m_szMessageCombo;
				gHUD.m_Message.MessageAdd(&m_MessageCombo);
			}
			else
				sprintf(m_szMessageCombo, "GAME_EVENT_LOOSECOMBO %d %d\n", data1, data2);

			ConsolePrint(m_szMessageCombo);// log to console too
		}
	}
//		break;
else if (gameevent == GAME_EVENT_COOP_PLAYER_FINISH)
//	case GAME_EVENT_COOP_PLAYER_FINISH:
	{
		if (g_PlayerInfoList[data1].thisplayer)
		{
			sprintf(astr, "COOP_PL_FINISH_LOCAL\0");
//			m_PlayerStats.AddAward(AWARD_FINISH_LOCAL);
			if (m_iGameMode == COOP_MODE_LEVEL)
				CenterPrint(BufferedLocaliseTextString("#COOP_TRIGGER_TOUCH"));
		}
		else
			sprintf(astr, "COOP_PL_FINISH_OTHER\0");

		if (astr[0])
		{
			msg = TextMessageGet(astr);
			if (msg && msg->pMessage)
			{
				m_MessageCombo = *msg;// copy localized message
//				memcpy(&m_MessageCombo, msg, sizeof(client_textmessage_t));// store in separate place?
				_snprintf(m_szMessageCombo, DEATHNOTICE_MSG_LENGTH, msg->pMessage, g_PlayerInfoList[data1].name);// replace all '%s' with names
				m_szMessageCombo[DEATHNOTICE_MSG_LENGTH-1] = 0;
				m_MessageCombo.pMessage = m_szMessageCombo;
				gHUD.m_Message.MessageAdd(&m_MessageCombo);
			}
			else
				sprintf(m_szMessageCombo, "GAME_EVENT_COOP_PLAYER_FINISH %d %d\n", data1, data2);

			ConsolePrint(m_szMessageCombo);// log to console too
		}
	}
//		break;
else if (gameevent == GAME_EVENT_PLAYER_READY)
//	case GAME_EVENT_PLAYER_READY:
	{
		if (IsValidPlayerIndex(data1))
		{
			g_PlayerExtraInfo[data1].ready = 1;
			if (gViewPort && gViewPort->GetScoreBoard())// && gViewPort->GetScoreBoard()->isVisible())
				gViewPort->GetScoreBoard()->FillGrid();// show changes now

			char szText[MAX_CHARS_PER_LINE];
//			LocaliseTextString("#CL_READY", szText, MAX_CHARS_PER_LINE);
			_snprintf(szText, MAX_CHARS_PER_LINE, BufferedLocaliseTextString(". #CL_READY"), g_PlayerInfoList[data1].name);
			gHUD.m_SayText.SayTextPrint(szText, 0, 0);
			ConsolePrint(szText);
		}
	}
//		break;
//	}
	else
	{
		CON_DPRINTF("CL: Unknown game event (%d %d %d)\n", gameevent, data1, data2);
	}

/*	if (strstr(m_szMessageCombo, "was ended by"))
	{
		DBG_FORCEBREAK
	}*/
}
