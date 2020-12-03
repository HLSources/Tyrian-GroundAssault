#include "extdll.h"
#include "util.h"
#include "engine.h"
#include "usercmd.h"
#include "bot.h"
#include "bot_cvar.h"
#include "bot_func.h"
#include "msg_fx.h"
#include <assert.h>

int gmsgTextMsg = 0;
int gmsgSayText = 0;
int gmsgShowMenu = 0;

void CmdStart(const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed);
void CmdEnd(const edict_t *player);

#ifdef _DEBUG
bool DBG_AssertFunction(bool fExpr, const char *szExpr, const char *szFile, int szLine, const char *szMessage)
{
	if (fExpr)
		return true;

	if (szMessage != NULL)
		ALERT(at_console, "XBM: ASSERT FAILED: %s (%s@%d)\n%s", szExpr, szFile, szLine, szMessage);
	else
		ALERT(at_console, "XBM: ASSERT FAILED: %s (%s@%d)\n", szExpr, szFile, szLine);

	if (g_pdeveloper && g_pdeveloper->value > 0.0f)// only bring up the "abort retry ignore" dialog if in debug mode!
		_assert((void *)szExpr, (void *)szFile, szLine);

	return fExpr;
}
#else// XDM: ASSERT function for release build
bool NDB_AssertFunction(bool fExpr, const char *szExpr, const char *szMessage)
{
	if (fExpr)
		return true;

	if (szMessage != NULL)
		ALERT(at_console, "XBM: ASSERT FAILED: %s %s\n", szExpr, szMessage);
	else
		ALERT(at_console, "XBM: ASSERT FAILED: %s\n", szExpr);

	return fExpr;
}
#endif	// DEBUG


Vector UTIL_VecToAngles(const Vector &vec)
{
	float rgflVecOut[3];
	VEC_TO_ANGLES(vec, rgflVecOut);
	return Vector(rgflVecOut);
}

// Overloaded to add IGNORE_GLASS
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE) | (ignoreGlass?0x100:0), pentIgnore, ptr );
}

void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr )
{
	TRACE_LINE( vecStart, vecEnd, (igmon == ignore_monsters ? TRUE : FALSE), pentIgnore, ptr );
}

edict_t *UTIL_FindEntityInSphere( edict_t *pentStart, const Vector &vecCenter, float flRadius )
{
	edict_t  *pentEntity = FIND_ENTITY_IN_SPHERE( pentStart, vecCenter, flRadius);
	if (!FNullEnt(pentEntity))
		return pentEntity;

	return NULL;
}

edict_t *UTIL_FindEntityByString( edict_t *pentStart, const char *szKeyword, const char *szValue )
{
	edict_t *pentEntity = FIND_ENTITY_BY_STRING( pentStart, szKeyword, szValue );
	if (!FNullEnt(pentEntity))
		return pentEntity;

	return NULL;
}

edict_t *UTIL_FindEntityByClassname( edict_t *pentStart, const char *szName )
{
	return UTIL_FindEntityByString( pentStart, "classname", szName );
}

edict_t *UTIL_FindEntityByTargetname( edict_t *pentStart, const char *szName )
{
	return UTIL_FindEntityByString( pentStart, "targetname", szName );
}

bool IsFacing(const Vector &origin, const Vector &v_angle, const Vector &reference)
{
	Vector forward, angle;
	Vector vecDir = reference - origin;
	vecDir.z = 0;
	vecDir = vecDir.Normalize();
	angle = v_angle;
	angle.x = 0;
	UTIL_MakeVectorsPrivate(angle, forward, NULL, NULL);
	// He's facing me, he meant it
	// TODO: use FOV!
	if (DotProduct(forward, vecDir) > 0.96)	// +/- 15 degrees or so
		return true;

	return false;
}

void ClientPrint( edict_t *pEntity, int msg_dest, const char *msg_name)
{
	if (gmsgTextMsg == 0)
		gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 );

	pfnMessageBegin( MSG_ONE, gmsgTextMsg, NULL, pEntity );
		pfnWriteByte( msg_dest );
		pfnWriteString( msg_name );
	pfnMessageEnd();
}

/*void UTIL_SayText( const char *pText, edict_t *pEdict )
{
	if (gmsgSayText == 0)
		gmsgSayText = REG_USER_MSG( "SayText", -1 );

	pfnMessageBegin( MSG_ONE, gmsgSayText, NULL, pEdict );
	pfnWriteByte(pEdict?ENTINDEX(pEdict):0);
		pfnWriteString( pText );
	pfnMessageEnd();
}*/


void UTIL_HostSay( edict_t *pEntity, int teamonly, char *message )
{
	int j;
	char  text[128];
	char *pc;
	int sender_team, player_team;
	edict_t *client;

	// make sure the text has content
	for ( pc = message; pc != NULL && *pc != 0; pc++ )
	{
		if ( isprint( *pc ) && !isspace( *pc ) )
		{
			pc = NULL;// we've found an alphanumeric character,  so text is valid
			break;
		}
	}

	if ( pc != NULL )
		return;  // no character found, so say nothing

	// turn on color set 2  (color on,  no sound)
	if (mod_id == TYRIAN_DLL)// XDM3035: XDM uses different mechanism
	{
		text[0] = 2;
		text[1] = 0;
	}
	else
	{
		if ( teamonly )
			sprintf( text, "%c(TEAM) %s: ", 2, STRING( pEntity->v.netname ) );
		else
			sprintf( text, "%c%s: ", 2, STRING( pEntity->v.netname ) );
		
		j = sizeof(text) - 2 - strlen(text);  // -2 for /n and null terminator
		if ( (int)strlen(message) > j )
			message[j] = 0;
	}

	strcat( text, message );
	strcat( text, "\n" );

	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop

	if (gmsgSayText == 0)
		gmsgSayText = REG_USER_MSG( "SayText", -1 );

	sender_team = pEntity->v.team;//UTIL_GetTeam(pEntity);

	client = NULL;
	while ( ((client = UTIL_FindEntityByClassname( client, "player" )) != NULL) && (!FNullEnt(client)) )
	{
		if ( client == pEntity )  // skip sender of message
			continue;

		player_team = client->v.team;//UTIL_GetTeam(client);

		if ( teamonly && (sender_team != player_team) )
			continue;

		pfnMessageBegin( MSG_ONE, gmsgSayText, NULL, client );
		pfnWriteByte(ENTINDEX(pEntity) | ((teamonly && mod_id==TYRIAN_DLL)?128:0));
		pfnWriteString( text );
		pfnMessageEnd();
	}

	// print to the sending client
	pfnMessageBegin( MSG_ONE, gmsgSayText, NULL, pEntity );
	pfnWriteByte( ENTINDEX(pEntity) );
	pfnWriteString( text );
	pfnMessageEnd();
	// echo to server console
	g_engfuncs.pfnServerPrint( text );
}


#ifdef _DEBUG
edict_t *DBG_EntOfVars( const entvars_t *pev )
{
	if (pev->pContainingEntity != NULL)
		return pev->pContainingEntity;

	ALERT(at_console, "entvars_t pContainingEntity is NULL, calling into engine\n");
	edict_t* pent = (*g_engfuncs.pfnFindEntityByVars)((entvars_t*)pev);

	if (pent == NULL)
		ALERT(at_console, "XBM: ERROR! Even the engine couldn't FindEntityByVars!\n");

	((entvars_t *)pev)->pContainingEntity = pent;
	return pent;
}
#endif //_DEBUG


// return team number 0 through 3 based what MOD uses for team numbers
/*int UTIL_GetTeam(edict_t *pEntity)
{
	// must be HL or OpFor deathmatch...
	char *infobuffer;
	char model_name[32];
	
	if (team_names[0][0] == 0)
	{
		char *pName;
		char teamlist[MAX_TEAMS*MAX_TEAMNAME_LENGTH];
		int i;
		
		num_teams = 0;
		strcpy(teamlist, CVAR_GET_STRING("mp_teamlist"));
		pName = teamlist;
		pName = strtok(pName, ";");
		
		while (pName != NULL && *pName)
		{
			// check that team isn't defined twice
			for (i=0; i < num_teams; i++)
				if (stricmp(pName, team_names[i]) == 0)
					break;
				if (i == num_teams)
				{
					strcpy(team_names[num_teams], pName);
					num_teams++;
				}
				pName = strtok(NULL, ";");
		}
	}

	infobuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)( pEntity );
	strcpy(model_name, (g_engfuncs.pfnInfoKeyValue(infobuffer, "model")));

	for (int index=0; index < num_teams; index++)
	{
		if (stricmp(model_name, team_names[index]) == 0)
			return index;
	}

	return 0;
//	return pEntity->v.team;
}

// return class number 0 through N
int UTIL_GetClass(edict_t *pEntity)
{
	return 0;
}*/

int UTIL_CountBots(void)
{
	int count = 0;
	int i;
	for (i=0; i < MAX_PLAYERS; ++i)
	{
		if (bots[i].is_used)// count the number of bots in use
			++count;
	}
	return count;
}

int UTIL_GetBotIndex(edict_t *pEdict)
{
	int index;
	for (index=0; index < MAX_PLAYERS; ++index)
	{
		if (bots[index].pEdict == pEdict)
			return index;
	}
	return -1;  // return -1 if edict is not a bot
}

bot_t *UTIL_GetBotPointer(edict_t *pEdict)
{
	int index;
	for (index=0; index < MAX_PLAYERS; ++index)
	{
		if (bots[index].pEdict == pEdict)
			break;
	}

	if (index < MAX_PLAYERS)
		return (&bots[index]);

	return NULL;  // return NULL if edict is not a bot
}

bool IsAlive(edict_t *pEdict)
{
	return ((pEdict->v.deadflag == DEAD_NO) &&
		(pEdict->v.health > 0) &&
		!(pEdict->v.flags & FL_NOTARGET) &&
		(pEdict->v.takedamage != 0));
}

bool FInViewCone(const Vector &origin, edict_t *pEdict)
{
	Vector2D	vec2LOS;
	float		flDot;
	UTIL_MakeVectors(pEdict->v.angles);
	vec2LOS = (origin - pEdict->v.origin).Make2D();
	vec2LOS = vec2LOS.Normalize();
	flDot = DotProduct(vec2LOS, gpGlobals->v_forward.Make2D());

	if (flDot > 0.5)// 60 degree field of view //m_flFieldOfView)
		return true;

	return false;
}

bool FVisible(const Vector &vecOrigin, edict_t *pEdict)
{
	// look through caller's eyes
	Vector vecLookerOrigin = pEdict->v.origin + pEdict->v.view_ofs;

	bool bInWater = (POINT_CONTENTS(vecOrigin) == CONTENTS_WATER);
	bool bLookerInWater = (POINT_CONTENTS(vecLookerOrigin) == CONTENTS_WATER);

	// don't look through water
	if (bInWater != bLookerInWater)
		return false;

	TraceResult tr;
	UTIL_TraceLine(vecLookerOrigin, vecOrigin, ignore_monsters, ignore_glass, pEdict, &tr);

	if (tr.flFraction != 1.0)
		return false;  // Line of sight is not established
	else
		return true;  // line of sight is valid.
}

Vector GetGunPosition(edict_t *pEdict)
{
	return (pEdict->v.origin + pEdict->v.view_ofs);
}


void UTIL_SelectItem(edict_t *pEdict, char *item_name)
{
	FakeClientCommand(pEdict, item_name, NULL, NULL);
}

void UTIL_SelectItem(edict_t *pEdict, const int &iID)
{
	static char buf[8];
	FakeClientCommand(pEdict, "_sw", itoa(iID, buf, 10), NULL);
}

void UTIL_SelectWeapon(edict_t *pEdict, const int &weapon_index)
{
	usercmd_t user;
	user.lerp_msec = 0;
	user.msec = 0;
	user.viewangles = pEdict->v.v_angle;
	user.forwardmove = 0;
	user.sidemove = 0;
	user.upmove = 0;
	user.lightlevel = 127;
	user.buttons = 0;
	user.impulse = 0;
	user.weaponselect = weapon_index;
	user.impact_index = 0;
	user.impact_position = Vector(0, 0, 0);
	CmdStart(pEdict, &user, 0);
	CmdEnd(pEdict);
}

Vector VecBModelOrigin(entvars_t *pevBModel)
{
	return (pevBModel->absmin + pevBModel->absmax) * 0.5f;// XDM: proper way
//	return pevBModel->absmin + (pevBModel->size * 0.5f);
}

bool UpdateSounds(edict_t *pEdict, edict_t *pPlayer)
{
	// check if this player is moving fast enough to make sounds...
	if (pPlayer->v.velocity.Length2D() > 220.0f)
	{
		float sensitivity = 1.0f;
		float volume = 500.0f;  // volume of sound being made (just pick something)
		Vector v_sound = pPlayer->v.origin - pEdict->v.origin;
		float distance = v_sound.Length();
		// is the bot close enough to hear this sound?
		if (distance < (volume * sensitivity))
		{
			Vector bot_angles = UTIL_VecToAngles(v_sound);
			pEdict->v.ideal_yaw = bot_angles.y;
			BotFixIdealYaw(pEdict);
			return TRUE;
		}
	}

	return FALSE;
}

void UTIL_ShowMenu( edict_t *pEdict, int slots, int displaytime, bool needmore, char *pText )
{
	if (gmsgShowMenu == 0)
		gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 );

	pfnMessageBegin( MSG_ONE, gmsgShowMenu, NULL, pEdict );
		pfnWriteShort( slots );
		pfnWriteChar( displaytime );
		pfnWriteByte( needmore );
		pfnWriteString( pText );
	pfnMessageEnd();
}

void UTIL_BuildFileName(char *filename, const char *arg1, const char *arg2)
{
	filename[0] = 0;
	
	if (mod_id == VALVE_DLL)
		strcpy(filename, "valve/");
	else if (mod_id == TYRIAN_DLL)// XBM
		strcpy(filename, "TYRIAN/");
	else
	{
		ALERT( at_warning, "Error in UTIL_BuildFileName (mod ID is unknown)!");
		filename[0] = 0;
		return;
	}
	
	if ((arg1) && (arg2))
	{
		if (*arg1 && *arg2)
		{
			strcat(filename, arg1);
			strcat(filename, "/");
			strcat(filename, arg2);
		}
		
		return;
	}

	if (arg1)
	{
		if (*arg1)
		{
			strcat(filename, arg1);
		}
	}

	// convert to MS-DOS or Linux format...
//	UTIL_Pathname_Convert(filename);
}

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf( char *fmt, ... )
{
	va_list argptr;
	static char string[1024];

	va_start(argptr, fmt);
	vsprintf(string, fmt, argptr);
	va_end(argptr);

	// Print to server console
	ALERT( at_logged, "%s", string );
}

void GetGameDir(char *game_dir)
{
	// This function fixes the erratic behaviour caused by the use of the GET_GAME_DIR engine
	// macro, which returns either an absolute directory path, or a relative one, depending on
	// whether the game server is run standalone or not. This one always return a RELATIVE path.
	unsigned char length, fieldstart, fieldstop;

	GET_GAME_DIR (game_dir); // call the engine macro and let it mallocate for the char pointer

	length = strlen (game_dir); // get the length of the returned string
	length--; // ignore the trailing string terminator

	// format the returned string to get the last directory name
	fieldstop = length;
	while (((game_dir[fieldstop] == '\\') || (game_dir[fieldstop] == '/')) && (fieldstop > 0))
		fieldstop--; // shift back any trailing separator

	fieldstart = fieldstop;
	while ((game_dir[fieldstart] != '\\') && (game_dir[fieldstart] != '/') && (fieldstart > 0))
		fieldstart--; // shift back to the start of the last subdirectory name

	if ((game_dir[fieldstart] == '\\') || (game_dir[fieldstart] == '/'))
		fieldstart++; // if we reached a separator, step over it

	// now copy the formatted string back onto itself character per character
	for (length = fieldstart; length <= fieldstop; length++)
		game_dir[length - fieldstart] = game_dir[length];
	game_dir[length - fieldstart] = 0; // terminate the string
}


//-----------------------------------------------------------------------------
// Purpose: Variable arguments list for string formatting
// Input  : *format
// Output : char
//-----------------------------------------------------------------------------
char *UTIL_VarArgs(char *format, ...)
{
	va_list			argptr;
	static char		string[1024];
	va_start (argptr, format);
	vsprintf (string, format,argptr);
	va_end (argptr);
	return string;	
}

//-----------------------------------------------------------------------------
// Purpose: Is this a valid team ID?
// Input  : &team_id - TEAM_ID
// Output : Returns true if TEAM_NONE to TEAM_4
//-----------------------------------------------------------------------------
bool IsValidTeam(const TEAM_ID &team_id)
{
	return (team_id >= TEAM_NONE && team_id <= MAX_TEAMS);//gViewPort->GetNumberOfTeams());
}

int g_iNumTeams = MAX_TEAMS;// stub
//-----------------------------------------------------------------------------
// Purpose: Is this a real, playable team? // TEAM_NONE must be invalid here!
// Input  : &team_id - TEAM_ID
// Output : Returns true if TEAM_1 to TEAM_4 and active
//-----------------------------------------------------------------------------
bool IsActiveTeam(const TEAM_ID &team_id)
{
	return (team_id > TEAM_NONE && team_id <= g_iNumTeams);//gViewPort->GetNumberOfTeams());
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pent - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool UTIL_IsValidEntity(edict_t *pent)
{
	if (pent == NULL || pent->free || (pent->v.flags & FL_KILLME))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: returns players and bots as edict_t *pointer
// Input  : playerIndex - player/entity index
//-----------------------------------------------------------------------------
edict_t *UTIL_ClientByIndex(const int &playerIndex)
{
	if (playerIndex > 0 && playerIndex <= gpGlobals->maxClients)
	{
		edict_t *e = INDEXENT(playerIndex);
		if (UTIL_IsValidEntity(e))// && (e->v.flags & FL_CLIENT || e->v.flags & FL_FAKECLIENT))
			return e;
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Get entity name or at least something useful...
// Input  : *pEntity - name source
//			*output - print here
//			max_len - maximum length of output string
//-----------------------------------------------------------------------------
void GetEntityPrintableName(edict_t *pEntity, char *output, const size_t max_len)
{
	if (pEntity && output)
	{
		if (pEntity->v.netname)
		{
			strncpy(output, STRING(pEntity->v.netname), max_len);
			output[max_len] = 0;
		}
		else if (pEntity->v.targetname)// XDM3035: monsters?
		{
			strncpy(output, STRING(pEntity->v.targetname), max_len);
			output[max_len] = 0;
		}
		else if (pEntity->v.classname)// XDM3035: desperate
		{
			strncpy(output, STRING(pEntity->v.classname), max_len);
			output[max_len] = 0;
		}
		else
			*output = 0;
			//strcpy(output, "anonymous\0");// J4F
	}
}

// use TE_BEAMPOINTS, TE_BEAMTORUS, TE_BEAMDISK or TE_BEAMCYLINDER as types, Parameters are NOT converted (e.g. life*0.1, etc.)
void BeamEffect(int type, const Vector &vecPos, const Vector &vecAxis, int spriteindex, int startframe, int fps, int life, int width, int noise, const Vector &color, int brightness, int speed)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecPos);
		WRITE_BYTE(type);
		WRITE_COORD(vecPos.x);
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_COORD(vecAxis.x);
		WRITE_COORD(vecAxis.y);
		WRITE_COORD(vecAxis.z);
		WRITE_SHORT(spriteindex);
		WRITE_BYTE(min(startframe, 255));
		WRITE_BYTE(min(fps, 255));
		WRITE_BYTE(min(life, 255));
		WRITE_BYTE(min(width, 255));
		WRITE_BYTE(min(noise, 255));
		WRITE_BYTE(min(color.x, 255));
		WRITE_BYTE(min(color.y, 255));
		WRITE_BYTE(min(color.z, 255));
		WRITE_BYTE(min(brightness, 255));
		WRITE_BYTE(min(speed, 255));
	MESSAGE_END();
}

void GlowSprite(const Vector &vecPos, int mdl_idx, int life, int scale, int fade)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecPos);
		WRITE_BYTE(TE_GLOWSPRITE);
		WRITE_COORD(vecPos.x);
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_SHORT(mdl_idx);
		WRITE_BYTE(min(life, 255));
		WRITE_BYTE(min(scale, 255));
		WRITE_BYTE(min(fade, 255));
	MESSAGE_END();
}

bool UTIL_LiquidContents(const Vector &vec)
{
	int pc = POINT_CONTENTS(vec);
	if (pc < CONTENTS_SOLID && pc > CONTENTS_SKY)
		return true;
	else
		return false;
}

void UTIL_ShowLine(const Vector &start, const Vector &end, float life, byte r, byte g, byte b)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, start);
		WRITE_BYTE(TE_LINE);
		WRITE_COORD(start.x);
		WRITE_COORD(start.y);
		WRITE_COORD(start.z);
		WRITE_COORD(end.x);
		WRITE_COORD(end.y);
		WRITE_COORD(end.z);
		WRITE_SHORT((int)(life*10.0f));
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
	MESSAGE_END();
}

// how to maket this work properly?
void UTIL_ShowBox(const Vector &origin, const Vector &mins, const Vector &maxs, float life, byte r, byte g, byte b)
{
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, origin);
		WRITE_BYTE(TE_BOX);
		WRITE_COORD(origin.x + mins.x);
		WRITE_COORD(origin.y + mins.y);
		WRITE_COORD(origin.z + mins.z);
		WRITE_COORD(origin.x + maxs.x);
		WRITE_COORD(origin.y + maxs.y);
		WRITE_COORD(origin.z + maxs.z);
		WRITE_SHORT((int)(life*10.0f));
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
	MESSAGE_END();
}



void UTIL_DebugBeam(const Vector &vecSrc, const Vector &vecEnd, float life, byte r, byte g, byte b)
{
#ifdef _DEBUG
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
		WRITE_BYTE(TE_BEAMPOINTS);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(vecEnd.x);
		WRITE_COORD(vecEnd.y);
		WRITE_COORD(vecEnd.z);
		WRITE_SHORT(g_iModelIndexBeamsAll);
		WRITE_BYTE(BLAST_SKIN_SHOCKWAVE);
		WRITE_BYTE(0);
		WRITE_BYTE((int)(life*10.0f));
		WRITE_BYTE(16);
		WRITE_BYTE(0);
		WRITE_BYTE(r);
		WRITE_BYTE(g);
		WRITE_BYTE(b);
		WRITE_BYTE(255);
		WRITE_BYTE(0);
	MESSAGE_END();
#endif
}

void UTIL_DebugPoint(const Vector &vecPos, float life, byte r, byte g, byte b)
{
	GlowSprite(vecPos, g_iModelIndexAnimglow01, (int)(life*10.0f), 10, 10);
}


void BotCheckTeamplay(void)
{
//	if ((int)CVAR_GET_FLOAT("mp_gamerules") >= GT_TEAMPLAY)// this can be -1
	g_iGameType = (int)gpGlobals->deathmatch;

	if (g_iGameType >= GT_TEAMPLAY)
		is_team_play = true;
	else
		is_team_play = false;

	checked_teamplay = true;
}


bool GameRulesHaveGoal(void)
{
	switch (g_iGameType)
	{
	case GT_COOP:
		{
			if (g_iGameMode == COOP_MODE_LEVEL)
				return true;
		}
		break;
	case GT_CTF:
	case GT_DOMINATION:
	case GT_ASSAULT:
		{
			return true;
		}
		break;
	}
	return false;
}
