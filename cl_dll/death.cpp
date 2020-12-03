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
//
// death notice
//
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "vgui_XDMViewport.h"

DECLARE_MESSAGE(m_DeathNotice, DeathMsg);

//static int DEATHNOTICE_DISPLAY_TIME = 6;
const int DEATHNOTICE_TEXTSPACE = 5;

const color24 DN_COLOR_TEAMMATE = {255,63,0};
const color24 DN_COLOR_UNKNOWN = {127,127,127};
const color24 DN_COLOR_WORLD = {191,191,191};
const color24 DN_COLOR_W_MONSTER = {255,127,31};

char nullstring[] = "NULL\0";

CHudDeathNotice::CHudDeathNotice() : CHudBase()
{
	m_pDeathNoticeList = NULL;
	m_iNumDeathNotices = 0;
}

CHudDeathNotice::~CHudDeathNotice()
{
	FreeData();
// XDM3035	m_fLastKillAward = 0;
}

void CHudDeathNotice::FreeData(void)
{
	if (m_pDeathNoticeList)
	{
		delete [] m_pDeathNoticeList;
		m_pDeathNoticeList = NULL;
		m_iNumDeathNotices = 0;
	}
}

int CHudDeathNotice::Init(void)
{
	HOOK_MESSAGE(DeathMsg);

	m_pCvarDNNum = CVAR_CREATE("hud_deathnotice_num", "8", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_pCvarDNTime = CVAR_CREATE("hud_deathnotice_time", "6", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_pCvarDNEcho = CVAR_CREATE("hud_deathnotice_echo", "1", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_pCvarDNTop = CVAR_CREATE("hud_deathnotice_top", "0.075", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_iFlags |= HUD_DRAW_ALWAYS | HUD_INTERMISSION;// XDM

	FreeData();
	gHUD.AddHudElem(this);
	return 1;
}

void CHudDeathNotice::InitHUDData(void)
{
	memset(m_szMessage, 0, sizeof(m_szMessage));
	memset(m_szScoreMessage, 0, sizeof(m_szMessage));
	memset(&m_Message, 0, sizeof(client_textmessage_t));
	memset(&m_ScoreMessage, 0, sizeof(client_textmessage_t));

	if (m_pDeathNoticeList == NULL)
	{
		if (m_pCvarDNNum->value < 2)
			m_pCvarDNNum->value = 2;
		else if (m_pCvarDNNum->value > MAX_DEATHNOTICES)
			m_pCvarDNNum->value = MAX_DEATHNOTICES;

		m_iNumDeathNotices = (int)m_pCvarDNNum->value;
		m_pDeathNoticeList = new DeathNoticeItem[m_iNumDeathNotices];// XDM3035: this should really be a queue, not an array!
	}

	if (m_pDeathNoticeList)
		memset(m_pDeathNoticeList, 0, sizeof(DeathNoticeItem)*m_iNumDeathNotices);// XDM3036
}

int CHudDeathNotice::VidInit(void)
{
	FreeData();
	m_iDefaultSprite = gHUD.GetSpriteIndex("d_skull");
	InitHUDData();
	return 1;
}

int CHudDeathNotice::Draw(const float &flTime)
{
	if (m_pDeathNoticeList == NULL)
		return 0;

	int x, y;
	for (int i = 0; i < m_iNumDeathNotices; ++i)
	{
		if (m_pDeathNoticeList[i].iId == 0)
			break;  // we've gone through them all

		if (m_pDeathNoticeList[i].flDisplayTime < flTime)
		{ // display time has expired
			// remove the current item from the list and move all following items to its position
			memmove(&m_pDeathNoticeList[i], &m_pDeathNoticeList[i+1], sizeof(DeathNoticeItem) * (m_iNumDeathNotices-i));
			memset(&m_pDeathNoticeList[m_iNumDeathNotices-1], 0, sizeof(DeathNoticeItem));// XDM
			--i;  // continue on the next item;  stop the counter getting incremented
			continue;
		}
		m_pDeathNoticeList[i].flDisplayTime = min(m_pDeathNoticeList[i].flDisplayTime, gHUD.m_flTime + m_pCvarDNTime->value);

		// Only draw if the viewport will let me
		if (gViewPort && gViewPort->AllowedToPrintText())
		{
			// Draw the death notice
			y = (int)((float)ScreenHeight*m_pCvarDNTop->value) + 2 + (32 * i);  //!!! // default DNTop->value is YRES(32)

			int id = (m_pDeathNoticeList[i].iId == -1) ? m_iDefaultSprite : m_pDeathNoticeList[i].iId;
			x = ScreenWidth - ConsoleStringLen(m_pDeathNoticeList[i].szVictim) - (gHUD.GetSpriteRect(id).right - gHUD.GetSpriteRect(id).left);

			if (m_pDeathNoticeList[i].iKillType != KILL_SELF)
			{
				x -= (DEATHNOTICE_TEXTSPACE + ConsoleStringLen(m_pDeathNoticeList[i].szKiller));
				// Draw killers name
				DrawSetTextColor(m_pDeathNoticeList[i].KillerColor[0], m_pDeathNoticeList[i].KillerColor[1], m_pDeathNoticeList[i].KillerColor[2]);
				x = DEATHNOTICE_TEXTSPACE + DrawConsoleString(x, y, m_pDeathNoticeList[i].szKiller);
			}
			// Draw weapon
//			SPR_Set(gHUD.GetSprite(id), m_pDeathNoticeList[i].WeaponColor[0], m_pDeathNoticeList[i].WeaponColor[1], m_pDeathNoticeList[i].WeaponColor[2]);

			SPR_Set(gHUD.GetSprite(id), 255, 255, 255);
			SPR_DrawAdditive(0, x, y, &gHUD.GetSpriteRect(id));
			x += (gHUD.GetSpriteRect(id).right - gHUD.GetSpriteRect(id).left);

			// Draw victims name (if it was a player that was killed)
			DrawSetTextColor(m_pDeathNoticeList[i].VictimColor[0], m_pDeathNoticeList[i].VictimColor[1], m_pDeathNoticeList[i].VictimColor[2]);
			x = DrawConsoleString(x, y, m_pDeathNoticeList[i].szVictim);
		}
	}
	return 1;
}

//-----------------------------------------------------------------------------
// This message handler may be better off elsewhere
//-----------------------------------------------------------------------------
int CHudDeathNotice::MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf)
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ(pbuf, iSize);

	int killer_index = READ_SHORT();// WARNING: incompatibility: XDM3035a
	int victim_index = READ_SHORT();// WARNING! killer index is an ENTITY INDEX!
//	int flags = READ_BYTE();// UNDONE

	char killedwith[32];
	strcpy(killedwith, "d_");
	strncat(killedwith, READ_STRING(), 32);

	ASSERT(END_READ() == 0);
/*
#ifdef _DEBUG
	CON_DPRINTF("CL: DeathMsg(ki %d vi %d kw %s)\n", killer_index, victim_index, killedwith);
#endif
*/
	GetPlayerInfo(killer_index, &g_PlayerInfoList[killer_index]);// XDM3035c: TESTME
	GetPlayerInfo(victim_index, &g_PlayerInfoList[victim_index]);

// doesn't work anyway	gHUD.m_Spectator.DeathMessage(victim_index);

	if (gViewPort)
		gViewPort->DeathMsg(killer_index, victim_index);

	return AddNotice(killer_index, victim_index, killedwith);
}

//-----------------------------------------------------------------------------
// Purpose: special color for weapon icon
// Input  : *rgb - int[3] RGB
//			iKillType - KILLTYPE
//-----------------------------------------------------------------------------
void CHudDeathNotice::SetupWeaponColor(int *rgb, const byte &iKillType)
{
	int c = RGB_BLUE;
	switch (iKillType)
	{
	case KILL_NORMAL:	c = RGB_BLUE; break;
	case KILL_LOCAL:	c = RGB_CYAN; break;
	case KILL_SELF:		c = RGB_YELLOW; break;
	case KILL_TEAM:		c = RGB_RED; break;
	case KILL_MONSTER:
		{
			rgb[0] = DN_COLOR_W_MONSTER.r;
			rgb[1] = DN_COLOR_W_MONSTER.g;
			rgb[2] = DN_COLOR_W_MONSTER.b;
			return;// don't unpack
			break;
		}
	case KILL_THISPLAYER:	c = RGB_RED; break;
	case KILL_UNKNOWN:
	default:
		{
			rgb[0] = DN_COLOR_UNKNOWN.r;
			rgb[1] = DN_COLOR_UNKNOWN.g;
			rgb[2] = DN_COLOR_UNKNOWN.b;
			return;// don't unpack
			break;
		}
	}
	UnpackRGB(rgb[0], rgb[1], rgb[2], c);
}

//-----------------------------------------------------------------------------
// XDM3035: real code now goes here
// TODO: decide kill type/flags/hints on the server!
//-----------------------------------------------------------------------------
int CHudDeathNotice::AddNotice(int killer_index, int victim_index, char *killedwith)
{
	if (m_pDeathNoticeList == NULL)
		return 0;

	int i = 0;
	int prefix = 2;// "d_"
	bool killer_player = false;
	bool victim_player = false;
	char *killer_name = NULL;
	char *victim_name = NULL;

	for (i = 0; i < m_iNumDeathNotices; ++i)
	{
		if (m_pDeathNoticeList[i].iId == 0)
			break;
	}
	if (i == m_iNumDeathNotices)// move the rest of the list forward to make room for this item
	{
		// move everything from and after position 1 to position 0 to free the last position
		memmove(&m_pDeathNoticeList[0], &m_pDeathNoticeList[1], sizeof(DeathNoticeItem) * (m_iNumDeathNotices-1));
		i = m_iNumDeathNotices - 1;
	}
	memset(&m_pDeathNoticeList[i], 0, sizeof(DeathNoticeItem));// XDM
	m_pDeathNoticeList[i].iKillType = KILL_UNKNOWN;
	m_pDeathNoticeList[i].iId = -1;// sprite index


// -- Get the Victim's name
	// If victim is -1, the killer killed a specific, non-player object (like a sentrygun)
	if (IsValidPlayerIndex(victim_index))
	{
		victim_player = true;
		victim_name = g_PlayerInfoList[victim_index].name;
		GetTeamColor(g_PlayerExtraInfo[victim_index].teamnumber, m_pDeathNoticeList[i].VictimColor[0], m_pDeathNoticeList[i].VictimColor[1], m_pDeathNoticeList[i].VictimColor[2]);
//		m_pDeathNoticeList[i].VictimColor = GetClientColor(victim);

		if (victim_name)
			strncpy(m_pDeathNoticeList[i].szVictim, victim_name, MAX_PLAYER_NAME_LENGTH);
		else
			CON_DPRINTF("CL: AddNotice(ki %d vi %d kw %s) victim with no name!!\n", killer_index, victim_index, killedwith);

		m_pDeathNoticeList[i].szVictim[MAX_PLAYER_NAME_LENGTH-1] = 0;
	}
	else// victim is not a player (CoOp)
	{
		victim_player = false;
		sprintf(m_pDeathNoticeList[i].szVictim, "monster %d\0", victim_index);
		m_pDeathNoticeList[i].szVictim[MAX_PLAYER_NAME_LENGTH-1] = 0;
		victim_name = m_pDeathNoticeList[i].szVictim;
		m_pDeathNoticeList[i].VictimColor[0] = DN_COLOR_W_MONSTER.r;// Don't confuse with team color!
		m_pDeathNoticeList[i].VictimColor[1] = DN_COLOR_W_MONSTER.g;
		m_pDeathNoticeList[i].VictimColor[2] = DN_COLOR_W_MONSTER.b;
	}

// -- Get the Killer's name
	if (IsValidPlayerIndex(killer_index))// IMPORTANT: index fits into player info array bounds! (g_PlayerInfoList, etc.)
	{
		killer_player = true;
		killer_name = g_PlayerInfoList[killer_index].name;

		if (g_PlayerInfoList[killer_index].thisplayer && !IsTeamGame(gHUD.m_iGameType))// XDM3035
			Int2RGB(gHUD.m_iDrawColorCyan, m_pDeathNoticeList[i].KillerColor[0], m_pDeathNoticeList[i].KillerColor[1], m_pDeathNoticeList[i].KillerColor[2]);
		else// don't use the same cyan color for name highlighting in teamplay because it may match a team color
			GetTeamColor(g_PlayerExtraInfo[killer_index].teamnumber, m_pDeathNoticeList[i].KillerColor[0], m_pDeathNoticeList[i].KillerColor[1], m_pDeathNoticeList[i].KillerColor[2]);
		// we could probably use GetPlayerColor() here, but that's against the concept (colors have important significance here)

		if (killer_name)
			strncpy(m_pDeathNoticeList[i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH);

		m_pDeathNoticeList[i].szKiller[MAX_PLAYER_NAME_LENGTH-1] = 0;
		m_pDeathNoticeList[i].iKillType = KILL_NORMAL;// default, overridden later
	}
	else// killer is not a player
	{
		killer_player = false;
		if (strncmp(killedwith, "d_monster", 9) == 0)
		{
			m_pDeathNoticeList[i].iKillType = KILL_MONSTER;
			// color
			//UnpackRGB(m_pDeathNoticeList[i].KillerColor[0], m_pDeathNoticeList[i].KillerColor[1], m_pDeathNoticeList[i].KillerColor[2], RGB_YELLOW);
			m_pDeathNoticeList[i].KillerColor[0] = DN_COLOR_W_MONSTER.r;// Don't confuse with team color!
			m_pDeathNoticeList[i].KillerColor[1] = DN_COLOR_W_MONSTER.g;
			m_pDeathNoticeList[i].KillerColor[2] = DN_COLOR_W_MONSTER.b;
			// name
			killer_name = killedwith + 10;// skip "d_monster_"
			strncpy(m_pDeathNoticeList[i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH);
			m_pDeathNoticeList[i].szKiller[MAX_PLAYER_NAME_LENGTH-1] = 0;
			//*killedwith = NULL;
			killedwith[9] = 0;// "d_monster" for icon
			killer_index = -1;// ?
		}
		else if (strcmp(killedwith, "d_worldspawn") == 0)
		{
			killer_name = nullstring;
			m_pDeathNoticeList[i].szKiller[0] = 0;
			m_pDeathNoticeList[i].iKillType = KILL_SELF;// like in original HL
			m_pDeathNoticeList[i].KillerColor[0] = DN_COLOR_WORLD.r;// Don't confuse with team color!
			m_pDeathNoticeList[i].KillerColor[1] = DN_COLOR_WORLD.g;
			m_pDeathNoticeList[i].KillerColor[2] = DN_COLOR_WORLD.b;
		}
		else
		{
			killer_name = nullstring;
			m_pDeathNoticeList[i].szKiller[0] = 0;
			m_pDeathNoticeList[i].iKillType = KILL_UNKNOWN;
		}
	}


// -- determine which iKillType is it
//	if (victim_player)
//	{
		if (killer_index == victim_index)// suicide -- CHECK FIRST!
		{
			m_pDeathNoticeList[i].iKillType = KILL_SELF;
		}
		else if (killer_player && g_PlayerInfoList[killer_index].thisplayer)// local player scores
		{
			m_pDeathNoticeList[i].iKillType = KILL_LOCAL;
			client_textmessage_t *msg = TextMessageGet("KILL_LOCAL");
			if (msg)
			{
				sprintf(m_szScoreMessage, msg->pMessage, victim_name);// should be "you killed %s"
				memcpy(&m_ScoreMessage, msg, sizeof(client_textmessage_t));// copy all message parameters
//				m_ScoreMessage = *msg;// copy localized message
//				m_ScoreMessage.pName = "KILL_LOCAL";
				m_ScoreMessage.pMessage = m_szScoreMessage;// point to a real existing string
				gHUD.m_Message.MessageAdd(&m_ScoreMessage);
			}

			if (!strcmp(killedwith, "d_teammate"))// TEAMKILL!
			{
				m_pDeathNoticeList[i].iKillType = KILL_TEAM;
// XDM3035				m_fLastKillAward = 0;
				m_Message.r1 = DN_COLOR_TEAMMATE.r;// override color defined in titles.txt
				m_Message.g1 = DN_COLOR_TEAMMATE.g;
				m_Message.b1 = DN_COLOR_TEAMMATE.b;
			}
// XDM3035: MOVED to server!
			if (victim_player)// moved to server
			{
				if (m_iLastKilledBy == victim_index)
				{
/*					char mbuf[3];
					mbuf[0] = GAME_EVENT_REVENGE;
					mbuf[1] = killer_index;
					mbuf[2] = victim_index;
					gHUD.MsgFunc_GREvent(NULL, sizeof(mbuf), mbuf);
					*/
				}
				m_iLastKilledBy = 0;// reset anyway, late revenge doesn't count
			}
		}
		else if (g_PlayerInfoList[victim_index].thisplayer)// local player was killed
		{
			m_pDeathNoticeList[i].iKillType = KILL_THISPLAYER;
			if (killer_player)// don't print "you were killed by world" or by a monster
			{
				m_iLastKilledBy = killer_index;
				client_textmessage_t *msg = TextMessageGet("KILL_THISPLAYER");
				if (msg)
				{
					sprintf(m_szMessage, msg->pMessage, killer_name, killedwith+prefix);
//					m_Message = *msg;// copy localized message
					memcpy(&m_Message, msg, sizeof(client_textmessage_t));
//					m_Message.pName = "KILL_THISPLAYER";
					m_Message.pMessage = m_szMessage;
					gHUD.m_Message.MessageAdd(&m_Message);
				}
			}
			else
				m_iLastKilledBy = 0;
		}
		else if (!killer_player && !victim_player)
		{
			m_pDeathNoticeList[i].iKillType = KILL_UNKNOWN;
			if (m_pDeathNoticeList[i].szVictim[0] == 0)
				strcpy(m_pDeathNoticeList[i].szVictim, killedwith+prefix);
		}
//		else// some other player scored/died/etc,
//			CON_PRINTF("CL: non-local kill\n");
/*	}
	else// Is it a non-player object kill?
	{
		m_pDeathNoticeList[i].iKillType = KILL_UNKNOWN;
		strcpy(m_pDeathNoticeList[i].szVictim, killedwith+prefix);// Store the object's name in the Victim slot (skip the d_ bit)
	}*/

// -- Setup weapon icon color (pre-setup for faster drawing!)
	SetupWeaponColor(m_pDeathNoticeList[i].WeaponColor, m_pDeathNoticeList[i].iKillType);

// -- Print everything
	// Find the sprite in the list
	m_pDeathNoticeList[i].iId = gHUD.GetSpriteIndex(killedwith);
	m_pDeathNoticeList[i].flDisplayTime = gHUD.m_flTime + m_pCvarDNTime->value;// XDM3033

	// record the death notice in the console
	if (m_pCvarDNEcho->value > 0.0f)// 3033 XDM: SPAMSPAM!
	{
		char szConsoleString[80];

		if (m_pDeathNoticeList[i].iKillType == KILL_UNKNOWN)
		{
			if (killedwith)
				sprintf(szConsoleString, "* %s caused %s's death\n\0", killedwith+prefix, m_pDeathNoticeList[i].szVictim);
			else
				sprintf(szConsoleString, "* %s died mysteriously\n\0", m_pDeathNoticeList[i].szVictim);
		}
		else
		{
			if (m_pDeathNoticeList[i].iKillType == KILL_SELF)
			{
				if (strcmp(killedwith, "d_worldspawn") == 0)
					sprintf(szConsoleString, "* %s died", m_pDeathNoticeList[i].szVictim);
				else
					sprintf(szConsoleString, "* %s killed self", m_pDeathNoticeList[i].szVictim);
			}
			else if (m_pDeathNoticeList[i].iKillType == KILL_TEAM)
			{
				sprintf(szConsoleString, "* %s killed teammate %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);
			}
			//different weapons

			else
			{
				//"ELECTROCUTED"
				if ( 
					(strcmp(killedwith, "d_LightningField") == 0) ||
					(strcmp(killedwith, "d_LightningBlast") == 0) ||
					(strcmp(killedwith, "d_ShockWave") == 0) ||
					(strcmp(killedwith, "d_LightningGun") == 0) ||
					(strcmp(killedwith, "d_LightningBall") == 0) ||
					(strcmp(killedwith, "d_ShockCannon") == 0) || 
					(strcmp(killedwith, "d_ShockLaser") == 0)
					)
				sprintf(szConsoleString, "* %s ELECTROCUTED %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"DISPERSED INTO ATOMS"
				else if (
					(strcmp(killedwith, "d_StarBurst") == 0)||
					(strcmp(killedwith, "d_RingTeleport") == 0) ||
					( strcmp(killedwith, "d_Teleporter") == 0) ||
					( strcmp(killedwith, "d_strtarget") == 0)
					)
				sprintf(szConsoleString, "* %s DISPERSED INTO ATOMS %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"MELTED IN A PUDDLE"
				else if (
					(strcmp(killedwith, "d_PlasmaBall") == 0) ||
					( strcmp(killedwith, "d_BFG") == 0) ||
					( strcmp(killedwith, "d_PlasmaShieldBall") == 0) ||
					( strcmp(killedwith, "d_PlasmaStorm") == 0)
					)
				sprintf(szConsoleString, "* %s MELTED IN A PUDDLE %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"SHREDDED INTO PIECES"
				else if ( 
					(strcmp(killedwith, "d_Ripper") == 0) ||
					(strcmp(killedwith, "d_FlakCannon") == 0) ||
					(strcmp(killedwith, "d_Repeater") == 0) ||
					(strcmp(killedwith, "d_ClusterGun") == 0) )
				sprintf(szConsoleString, "* %s SHREDDED INTO PIECES %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				// "BLOWN UP INTO A THOUSAND PIECES"
				else if ( 
					( strcmp(killedwith, "d_spider_mine") == 0) || 
					(strcmp(killedwith, "d_tripmine") == 0) ||
					(strcmp(killedwith, "d_env_explosion") == 0)
					)
					sprintf(szConsoleString, "* %s BLOWN UP INTO A THOUSAND PIECES %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				// "BLOWN UP INTO A PILE OF JUNK"
				else if ( 
					(strcmp(killedwith, "d_ProtonMissile") == 0) ||
					(strcmp(killedwith, "d_MiniMissile") == 0) ||
					( strcmp(killedwith, "d_m203grenade") == 0) ||
					(strcmp(killedwith, "d_Grenade30mm") == 0) ||
					( strcmp(killedwith, "d_HellHounder") == 0) ||
					(strcmp(killedwith, "d_MicroMissile") == 0) ||
					( strcmp(killedwith, "d_DemolitionMissile") == 0) ||
					(strcmp(killedwith, "d_HVRMissile") == 0)
					)
				sprintf(szConsoleString, "* %s BLOWN UP INTO A PILE OF JUNK %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				// "BLASTED IN A CLOUD OF PARTS"
				else if ( 
					(strcmp(killedwith, "d_HeavyTurret") == 0) ||
					( strcmp(killedwith, "d_BeamSplitter") == 0) ||
					(strcmp(killedwith, "d_AmmunitionBlast") == 0)
					)
				sprintf(szConsoleString, "* %s BLASTED IN A CLOUD OF PARTS %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				// "MASHED IN A PUREE"
				else if ( 
					(strcmp(killedwith, "d_tracktrain") == 0) ||
					(strcmp(killedwith, "d_plat") == 0) ||
					(strcmp(killedwith, "d_door_rotating") == 0) ||
					(strcmp(killedwith, "d_door") == 0) ||
					(strcmp(killedwith, "d_Toilet") == 0)
					)
				sprintf(szConsoleString, "* %s MASHED IN A PUREE %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"WASTED INTO THE SMOKING GARBAGE"
				else if ( 
					(strcmp(killedwith, "d_env_laser") == 0) ||
					( strcmp(killedwith, "d_LaserCannon") == 0) ||
					( strcmp(killedwith, "d_TwinLaser") == 0) 
					)
				sprintf(szConsoleString, "* %s WASTED INTO THE SMOKING GARBAGE %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"ROASTED INTO CRISPY CHUNKS"
				else if ( 
					(strcmp(killedwith, "d_Scorcher") == 0) ||
					( strcmp(killedwith, "d_HellFire") == 0) 
					)
				sprintf(szConsoleString, "* %s ROASTED INTO CRISPY CHUNKS %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"TURNED INTO THE JUNK"
				else if ( 
					(strcmp(killedwith, "d_MezonCannon") == 0) ||
					(strcmp(killedwith, "d_GaussCannon") == 0) ||
					( strcmp(killedwith, "d_PulseCannon") == 0) ||
					( strcmp(killedwith, "d_MultiCannon") == 0) 
					)
				sprintf(szConsoleString, "* %s TURNED INTO THE JUNK %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"POISONED"
				else if ( 
					(strcmp(killedwith, "d_BioMissile") == 0) || 
					(strcmp(killedwith, "d_AcidBlob") == 0) || 
					(strcmp(killedwith, "d_Banana") == 0)
					)
				sprintf(szConsoleString, "* %s POISONED %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"DEMOLISHED"
				else if ( 
					(strcmp(killedwith, "d_mortar") == 0) ||
					(strcmp(killedwith, "d_SonicWave") == 0)
					)
				sprintf(szConsoleString, "* %s DEMOLISHED %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"TURNED INTO THE ASHES"
				else if ( 
					(strcmp(killedwith, "d_DisruptorBall") == 0) 
					|| (strcmp(killedwith, "d_TauCannon") == 0) ||
					(strcmp(killedwith, "d_GluonBall") == 0) || 
					(strcmp(killedwith, "d_PhotonGun") == 0)
					)
				sprintf(szConsoleString, "* %s TURNED INTO THE ASHES %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"DESTRUCTED"
				else if (
					(strcmp(killedwith, "d_ChargeCannon") == 0) ||
					(strcmp(killedwith, "d_Trident") == 0) || 
					( strcmp(killedwith, "d_DisruptorBall") == 0) 
					)
				sprintf(szConsoleString, "* %s DESTRUCTED %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"VAPOURIZED"
				else if (
					(strcmp(killedwith, "d_AtomBomb") == 0) || 
					(strcmp(killedwith, "d_NuclearMissile") == 0)
					)
				sprintf(szConsoleString, "* %s VAPOURIZED %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"DIZINTEGRATED"
				else if (
					(strcmp(killedwith, "d_SunOfGod") == 0) || 
					(strcmp(killedwith, "d_AntimatherialMissile") == 0)
					)
				sprintf(szConsoleString, "* %s DIZINTEGRATED %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				//"SHOT"
				else if ( 
					(strcmp(killedwith, "d_MachineGun") == 0) ||
					(strcmp(killedwith, "d_NeedleLaser") == 0) ||
					(strcmp(killedwith, "d_MiniGun") == 0) ||
					(strcmp(killedwith, "d_Vulcan") == 0) || 
					(strcmp(killedwith, "d_RailGun") == 0) ||
					(strcmp(killedwith, "d_SuperRailGun") == 0)
					)
				sprintf(szConsoleString, "* %s SHOT %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);

				else
					sprintf(szConsoleString, "* %s killed %s", m_pDeathNoticeList[i].szKiller, m_pDeathNoticeList[i].szVictim);
			}

			if (killedwith && *killedwith && (*killedwith > 13) && strcmp(killedwith, "d_worldspawn") != 0 && strcmp(killedwith, "d_monster") != 0)
			{
				strcat(szConsoleString, " with ");
				strcat(szConsoleString, killedwith+prefix);// skip over the "d_" part
			}
			strcat(szConsoleString, "\n");
		}
		ConsolePrint(szConsoleString);
	}
	return 1;
}
