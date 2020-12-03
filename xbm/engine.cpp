#include "extdll.h"
#include "util.h"
#include "bot.h"
#include "bot_client.h"
#include "bot_cvar.h"
#include "bot_func.h"
#include "engine.h"

extern enginefuncs_t g_engfuncs;

void (*botMsgFunction)(void *, int) = NULL;
void (*botMsgEndFunction)(void *, int) = NULL;

bool g_intermission = false;
int botMsgIndex;
int debug_engine = 0;
static FILE *fp;

// messages created in RegUserMsg which will be "caught"
int message_WeaponList = 0;
int message_CurWeapon = 0;
int message_AmmoX = 0;
int message_AmmoPickup = 0;
int message_WeapPickup = 0;
int message_ItemPickup = 0;
int message_Health = 0;
int message_Battery = 0;
int message_Damage = 0;
int message_DeathMsg = 0;
int message_ScreenFade = 0;
int message_GameMode = 0;
int message_UpdWeapons = 0;// XDM3035
int message_UpdAmmo = 0;
//int message_TeamNames = 0;
int message_HLTV = 0;// for Counter-Strike

int pfnPrecacheModel(char *s)
{
	return (*g_engfuncs.pfnPrecacheModel)(s);
}
int pfnPrecacheSound(char *s)
{
	return (*g_engfuncs.pfnPrecacheSound)(s);
}
void pfnSetModel(edict_t *e, const char *m)
{
	(*g_engfuncs.pfnSetModel)(e, m);
}
int pfnModelIndex(const char *m)
{
	return (*g_engfuncs.pfnModelIndex)(m);
}
int pfnModelFrames(int modelIndex)
{
	return (*g_engfuncs.pfnModelFrames)(modelIndex);
}
void pfnSetSize(edict_t *e, const float *rgflMin, const float *rgflMax)
{
	(*g_engfuncs.pfnSetSize)(e, rgflMin, rgflMax);
}
void pfnChangeLevel(char *s1, char *s2)
{
	// kick any bot off of the server after time/frag limit...
	for (int index = 0; index < MAX_PLAYERS; ++index)
	{
		if (bots[index].is_used)  // is this slot used?
		{
			char cmd[40];
//			sprintf(cmd, "kick \"%s\"\n", bots[index].name);
			sprintf(cmd, "kick # %d\n", ENTINDEX(bots[index].pEdict));
			bots[index].respawn_state = RESPAWN_NEED_TO_RESPAWN;
			SERVER_COMMAND(cmd);
		}
	}
	SERVER_EXECUTE();
	g_intermission = false;
	g_bot_use_flashlight.value = 0.0f;// XDM3037: reset to default for the next map
	(*g_engfuncs.pfnChangeLevel)(s1, s2);
}
void pfnGetSpawnParms(edict_t *ent)
{
	(*g_engfuncs.pfnGetSpawnParms)(ent);
}
void pfnSaveSpawnParms(edict_t *ent)
{
	(*g_engfuncs.pfnSaveSpawnParms)(ent);
}
float pfnVecToYaw(const float *rgflVector)
{
	return (*g_engfuncs.pfnVecToYaw)(rgflVector);
}
void pfnVecToAngles(const float *rgflVectorIn, float *rgflVectorOut)
{
	(*g_engfuncs.pfnVecToAngles)(rgflVectorIn, rgflVectorOut);
}
void pfnMoveToOrigin(edict_t *ent, const float *pflGoal, float dist, int iMoveType)
{
	(*g_engfuncs.pfnMoveToOrigin)(ent, pflGoal, dist, iMoveType);
}
void pfnChangeYaw(edict_t *ent)
{
	(*g_engfuncs.pfnChangeYaw)(ent);
}
void pfnChangePitch(edict_t *ent)
{
	(*g_engfuncs.pfnChangePitch)(ent);
}
edict_t *pfnFindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
	return (*g_engfuncs.pfnFindEntityByString)(pEdictStartSearchAfter, pszField, pszValue);
}
int pfnGetEntityIllum(edict_t *pEnt)
{
	return (*g_engfuncs.pfnGetEntityIllum)(pEnt);
}
edict_t *pfnFindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
	return (*g_engfuncs.pfnFindEntityInSphere)(pEdictStartSearchAfter, org, rad);
}
edict_t *pfnFindClientInPVS(edict_t *pEdict)
{
	return (*g_engfuncs.pfnFindClientInPVS)(pEdict);
}
edict_t *pfnEntitiesInPVS(edict_t *pplayer)
{
	return (*g_engfuncs.pfnEntitiesInPVS)(pplayer);
}
void pfnMakeVectors(const float *rgflVector)
{
	(*g_engfuncs.pfnMakeVectors)(rgflVector);
}
void pfnAngleVectors(const float *rgflVector, float *forward, float *right, float *up)
{
	(*g_engfuncs.pfnAngleVectors)(rgflVector, forward, right, up);
}
edict_t *pfnCreateEntity(void)
{
	return (*g_engfuncs.pfnCreateEntity)();
}
void pfnRemoveEntity(edict_t *e)
{
	(*g_engfuncs.pfnRemoveEntity)(e);
}
edict_t *pfnCreateNamedEntity(int className)
{
	return (*g_engfuncs.pfnCreateNamedEntity)(className);
}
void pfnMakeStatic(edict_t *ent)
{
	(*g_engfuncs.pfnMakeStatic)(ent);
}
int pfnEntIsOnFloor(edict_t *e)
{
	return (*g_engfuncs.pfnEntIsOnFloor)(e);
}
int pfnDropToFloor(edict_t *e)
{
	return (*g_engfuncs.pfnDropToFloor)(e);
}
int pfnWalkMove(edict_t *ent, float yaw, float dist, int iMode)
{
	return (*g_engfuncs.pfnWalkMove)(ent, yaw, dist, iMode);
}
void pfnSetOrigin(edict_t *e, const float *rgflOrigin)
{
	(*g_engfuncs.pfnSetOrigin)(e, rgflOrigin);
}
void pfnEmitSound(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch)
{
/* if (gpGlobals->deathmatch && mod_id == TFC_DLL)
	{
		if ((strcmp(sample, "speech/saveme1.wav") == 0) || (strcmp(sample, "speech/saveme2.wav") == 0))// is someone yelling for a medic?
		{
		}
	}*/
	(*g_engfuncs.pfnEmitSound)(entity, channel, sample, volume, attenuation, fFlags, pitch);
}
void pfnEmitAmbientSound(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
	(*g_engfuncs.pfnEmitAmbientSound)(entity, pos, samp, vol, attenuation, fFlags, pitch);
}
void pfnTraceLine(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	(*g_engfuncs.pfnTraceLine)(v1, v2, fNoMonsters, pentToSkip, ptr);
}
void pfnTraceToss(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr)
{
	(*g_engfuncs.pfnTraceToss)(pent, pentToIgnore, ptr);
}
int pfnTraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
	return (*g_engfuncs.pfnTraceMonsterHull)(pEdict, v1, v2, fNoMonsters, pentToSkip, ptr);
}
void pfnTraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
	(*g_engfuncs.pfnTraceHull)(v1, v2, fNoMonsters, hullNumber, pentToSkip, ptr);
}
void pfnTraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
	(*g_engfuncs.pfnTraceModel)(v1, v2, hullNumber, pent, ptr);
}
const char *pfnTraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2 )
{
	return (*g_engfuncs.pfnTraceTexture)(pTextureEntity, v1, v2);
}
void pfnTraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
	(*g_engfuncs.pfnTraceSphere)(v1, v2, fNoMonsters, radius, pentToSkip, ptr);
}
void pfnGetAimVector(edict_t *ent, float speed, float *rgflReturn)
{
	(*g_engfuncs.pfnGetAimVector)(ent, speed, rgflReturn);
}
void pfnServerCommand(char *str)
{
	(*g_engfuncs.pfnServerCommand)(str);
}
void pfnServerExecute(void)
{
	(*g_engfuncs.pfnServerExecute)();
}
void pfnClientCommand(edict_t *pEdict, char *szFmt, ...)
{
	if (!(pEdict->v.flags & FL_FAKECLIENT))
	{
		char tempFmt[256];
		va_list argp;
		va_start(argp, szFmt);
		vsprintf(tempFmt, szFmt, argp);
		(*g_engfuncs.pfnClientCommand)(pEdict, tempFmt);
		va_end(argp);
	}
	return;
}
void pfnParticleEffect(const float *org, const float *dir, float color, float count)
{
	(*g_engfuncs.pfnParticleEffect)(org, dir, color, count);
}
void pfnLightStyle(int style, char *val)
{
	(*g_engfuncs.pfnLightStyle)(style, val);
}
int pfnDecalIndex(const char *name)
{
	return (*g_engfuncs.pfnDecalIndex)(name);
}
int pfnPointContents(const float *rgflVector)
{
	return (*g_engfuncs.pfnPointContents)(rgflVector);
}
void pfnMessageBegin(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	if (msg_type == SVC_INTERMISSION)// XDM3035c
	{
		g_intermission = true;
		if (g_bot_chat_enable.value > 0.0f)
		{
			bot_t *pBot;
			int i = 0;
			for (i=0; i < MAX_PLAYERS; ++i)
			{
				if (bots[i].is_used == false)
					continue;

				pBot = &bots[i];
				pBot->f_move_speed = 0.0f;
				pBot->f_bot_chat_time += 10.0f;// delay all normal chatting
				pBot->pEdict->v.button |= (IN_ATTACK|IN_JUMP);// XDM3037: press ready buttons

				int winner = pBot->pEdict->v.iuser2;// should be same for all
				edict_t *pWinner = UTIL_ClientByIndex(winner);
				if (pWinner)
				{
					if (ENTINDEX(pBot->pEdict) == winner)// I am the winner
					{
						edict_t *pRandomEnemy = NULL;
						int enemy = RANDOM_LONG(1, gpGlobals->maxClients);
						for (int j=0; j < gpGlobals->maxClients; ++j)
						{
							pRandomEnemy = UTIL_ClientByIndex(enemy);
							if (pRandomEnemy && pRandomEnemy->v.team != pWinner->v.team)
								break;

							if (enemy < gpGlobals->maxClients)
								++enemy;
							else
								enemy = 1;// loop
						}
						BotSpeakTaunt(&bots[i], pRandomEnemy);// don't randomize, winner speaks always
						pBot->f_bot_say = gpGlobals->time + 1.0f;// override the timing
					}
					else if (is_team_play)// My team wins
					{
						if (pBot->pEdict->v.team == pWinner->v.team)
						{
							if ((RANDOM_LONG(1,100) <= pBot->taunt_percent))// don't ALWAYS speak so the chat won't get overflowed
							{
								edict_t *pLoser = NULL;
								int lid = RANDOM_LONG(1, gpGlobals->maxClients);
								for (int j=0; j < gpGlobals->maxClients; ++j)// don't loop more than once through all clients
								{
									pLoser = UTIL_ClientByIndex(lid);
									if (pLoser->v.team != pWinner->v.team)// found some player from non-winning team
										break;

									++lid;
									if (lid > gpGlobals->maxClients)
										lid = 1;
								}
								BotSpeakTaunt(pBot, pLoser);
								pBot->f_bot_say = gpGlobals->time + RANDOM_FLOAT(1,2);// override the timing
							}
						}
						else
						{
							if ((RANDOM_LONG(1,100) <= pBot->whine_percent))// don't ALWAYS speak so the chat won't get overflowed
								BotSpeakWhine(pBot, pWinner);
						}
						pBot->f_bot_say = gpGlobals->time + 1.0f + (float)i*0.5f;// let everyone get talkative, but not at the same time
					}
				}
			}
		}
		goto func_end;// skip everything else
	}
	if (gpGlobals->deathmatch)
	{
		int index = -1;
		if (ed)
		{
			index = UTIL_GetBotIndex(ed);
			// is this message for a bot?
			if (index != -1)
			{
				botMsgFunction = NULL;		// no msg function until known otherwise
				botMsgEndFunction = NULL;	// no msg end function until known otherwise
				botMsgIndex = index;		// index of bot receiving message

				if ((mod_id == VALVE_DLL) || (mod_id == TYRIAN_DLL))
				{
					if (msg_type == message_WeaponList)
					{
#ifndef SERVER_WEAPON_SLOTS
						if (mod_id == TYRIAN_DLL)
							botMsgFunction = BotClient_XDM_WeaponList;
						else
#endif
							botMsgFunction = BotClient_Valve_WeaponList;
					}
					else if (msg_type == message_CurWeapon)
						botMsgFunction = BotClient_Valve_CurrentWeapon;
					else if (msg_type == message_AmmoX)
						botMsgFunction = BotClient_Valve_AmmoX;
					else if (msg_type == message_AmmoPickup)
						botMsgFunction = BotClient_Valve_AmmoPickup;
					else if (msg_type == message_WeapPickup)
						botMsgFunction = BotClient_Valve_WeaponPickup;
					else if (msg_type == message_ItemPickup)
						botMsgFunction = BotClient_Valve_ItemPickup;
					else if (msg_type == message_Health)
						botMsgFunction = BotClient_Valve_Health;
					else if (msg_type == message_Battery)
						botMsgFunction = BotClient_Valve_Battery;
					else if (msg_type == message_Damage)
						botMsgFunction = BotClient_Valve_Damage;
					else if (msg_type == message_ScreenFade)
						botMsgFunction = BotClient_Valve_ScreenFade;
					else if (msg_type == message_UpdWeapons)// XDM3035
					{
						botMsgFunction = BotClient_XDM_UpdWeapons;
						botMsgEndFunction = BotClient_XDM_UpdWeaponsEnd;
					}
					else if (msg_type == message_UpdAmmo)
					{
						botMsgFunction = BotClient_XDM_UpdAmmo;
						botMsgEndFunction = BotClient_XDM_UpdAmmoEnd;
					}
					else if (msg_type == message_GameMode)// XDM3036
					{
						botMsgFunction = BotClient_XDM_GameMode;
						botMsgEndFunction = BotClient_XDM_GameModeEnd;
					}
//					else if (msg_type == message_TeamNames)
//						botMsgFunction = BotClient_TeamNames;
				}
			}
		}
		else if (msg_dest == MSG_ALL)
		{
			botMsgFunction = NULL;	// no msg function until known otherwise
			botMsgIndex = -1;		// index of bot receiving message (none)
			if (msg_type == message_DeathMsg)
				botMsgFunction = BotClient_Valve_DeathMsg;
		}
		else
		{
			// Steam makes the WeaponList message be sent differently
			botMsgFunction = NULL;	// no msg function until known otherwise
			botMsgIndex = -1;		// index of bot receiving message (none)

			if (mod_id == VALVE_DLL)
			{
				if (msg_type == message_WeaponList)
					botMsgFunction = BotClient_Valve_WeaponList;
			}
#ifndef SERVER_WEAPON_SLOTS
			else if (mod_id == TYRIAN_DLL)
			{
				if (msg_type == message_WeaponList)
					botMsgFunction = BotClient_XDM_WeaponList;
			}
#endif
			if (msg_type == message_HLTV)
				botMsgFunction = BotClient_CS_HLTV;
		}
	}
func_end:
	(*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ed);
}
void pfnMessageEnd(void)
{
	if (gpGlobals->deathmatch)
	{
		if (botMsgEndFunction)
			(*botMsgEndFunction)(NULL, botMsgIndex);  // NULL indicated msg end
		// clear out the bot message function pointers...
		botMsgFunction = NULL;
		botMsgEndFunction = NULL;
	}
	(*g_engfuncs.pfnMessageEnd)();
}
void pfnWriteByte(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);
	}
	(*g_engfuncs.pfnWriteByte)(iValue);
}
void pfnWriteChar(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);
	}
	(*g_engfuncs.pfnWriteChar)(iValue);
}
void pfnWriteShort(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);
	}
	(*g_engfuncs.pfnWriteShort)(iValue);
}
void pfnWriteLong(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);
	}
	(*g_engfuncs.pfnWriteLong)(iValue);
}
void pfnWriteAngle(float flValue)
{
	if (gpGlobals->deathmatch)
	{
		if (botMsgFunction)
			(*botMsgFunction)((void *)&flValue, botMsgIndex);
	}
	(*g_engfuncs.pfnWriteAngle)(flValue);
}
void pfnWriteCoord(float flValue)
{
	if (gpGlobals->deathmatch)
	{
		if (botMsgFunction)
			(*botMsgFunction)((void *)&flValue, botMsgIndex);
	}
	(*g_engfuncs.pfnWriteCoord)(flValue);
}
void pfnWriteString(const char *sz)
{
	if (gpGlobals->deathmatch)
	{
		if (botMsgFunction)
			(*botMsgFunction)((void *)sz, botMsgIndex);
	}
	(*g_engfuncs.pfnWriteString)(sz);
}
void pfnWriteEntity(int iValue)
{
	if (gpGlobals->deathmatch)
	{
		if (botMsgFunction)
			(*botMsgFunction)((void *)&iValue, botMsgIndex);
	}
	(*g_engfuncs.pfnWriteEntity)(iValue);
}
void pfnCVarRegister(cvar_t *pCvar)
{
	(*g_engfuncs.pfnCVarRegister)(pCvar);
}
float pfnCVarGetFloat(const char *szVarName)
{
	return (*g_engfuncs.pfnCVarGetFloat)(szVarName);
}
const char *pfnCVarGetString(const char *szVarName)
{
	return (*g_engfuncs.pfnCVarGetString)(szVarName);
}
void pfnCVarSetFloat(const char *szVarName, float flValue)
{
	(*g_engfuncs.pfnCVarSetFloat)(szVarName, flValue);
}
void pfnCVarSetString(const char *szVarName, const char *szValue)
{
	(*g_engfuncs.pfnCVarSetString)(szVarName, szValue);
}
void pfnAlertMessage(ALERT_TYPE atype, char *szFmt, ...)
{
//	if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnAlertMessage:\n"); fclose(fp); }
	va_list argptr;
	static char string[1024];
	va_start(argptr, szFmt);
	vsprintf(string, szFmt, argptr);
	va_end(argptr);
	(*g_engfuncs.pfnAlertMessage)(atype, string);
}
void pfnEngineFprintf(void *pfile, char *szFmt, ...)
{
//	if (debug_engine) { fp=fopen("bot.txt","a"); fprintf(fp,"pfnEngineFprintf:\n"); fclose(fp); }
	va_list argptr;
	static char string[1024];
	va_start(argptr, szFmt);
	vsprintf(string, szFmt, argptr);
	va_end(argptr);
	(*g_engfuncs.pfnEngineFprintf)(pfile, string);
}
void *pfnPvAllocEntPrivateData(edict_t *pEdict, int32 cb)
{
	return (*g_engfuncs.pfnPvAllocEntPrivateData)(pEdict, cb);
}
void *pfnPvEntPrivateData(edict_t *pEdict)
{
	return (*g_engfuncs.pfnPvEntPrivateData)(pEdict);
}
void pfnFreeEntPrivateData(edict_t *pEdict)
{
	(*g_engfuncs.pfnFreeEntPrivateData)(pEdict);
}
const char *pfnSzFromIndex(int iString)
{
	return (*g_engfuncs.pfnSzFromIndex)(iString);
}
int pfnAllocString(const char *szValue)
{
	return (*g_engfuncs.pfnAllocString)(szValue);
}
entvars_t *pfnGetVarsOfEnt(edict_t *pEdict)
{
	return (*g_engfuncs.pfnGetVarsOfEnt)(pEdict);
}
edict_t *pfnPEntityOfEntOffset(int iEntOffset)
{
	return (*g_engfuncs.pfnPEntityOfEntOffset)(iEntOffset);
}
int pfnEntOffsetOfPEntity(const edict_t *pEdict)
{
	return (*g_engfuncs.pfnEntOffsetOfPEntity)(pEdict);
}
int pfnIndexOfEdict(const edict_t *pEdict)
{
	return (*g_engfuncs.pfnIndexOfEdict)(pEdict);
}
edict_t *pfnPEntityOfEntIndex(int iEntIndex)
{
	return (*g_engfuncs.pfnPEntityOfEntIndex)(iEntIndex);
}
edict_t *pfnFindEntityByVars(entvars_t *pvars)
{
	return (*g_engfuncs.pfnFindEntityByVars)(pvars);
}
void *pfnGetModelPtr(edict_t *pEdict)
{
	return (*g_engfuncs.pfnGetModelPtr)(pEdict);
}
int pfnRegUserMsg(const char *pszName, int iSize)
{
	int msg = (*g_engfuncs.pfnRegUserMsg)(pszName, iSize);
//#ifdef _DEBUG
	ALERT(at_aiconsole, "XBM: pfnRegUserMsg(%s %d) = %d\n", pszName, iSize, msg);
//#endif
	if (gpGlobals->deathmatch)
	{
/*#ifdef _DEBUG
	fp=fopen("HPB_bot.txt","a"); fprintf(fp,"pfnRegUserMsg: pszName=%s msg=%d\n",pszName,msg); fclose(fp);
#endif*/
		
		if ((mod_id == VALVE_DLL) || (mod_id == TYRIAN_DLL))
		{
			if (strcmp(pszName, "WeaponList") == 0)
				message_WeaponList = msg;
			else if (strcmp(pszName, "CurWeapon") == 0)
				message_CurWeapon = msg;
			else if (strcmp(pszName, "AmmoX") == 0)
				message_AmmoX = msg;
			else if (strcmp(pszName, "AmmoPickup") == 0)
				message_AmmoPickup = msg;
			else if (strcmp(pszName, "WeapPickup") == 0)
				message_WeapPickup = msg;
			else if (strcmp(pszName, "ItemPickup") == 0)
				message_ItemPickup = msg;
			else if (strcmp(pszName, "Health") == 0)
				message_Health = msg;
			else if (strcmp(pszName, "Battery") == 0)
				message_Battery = msg;
			else if (strcmp(pszName, "Damage") == 0)
				message_Damage = msg;
			else if (strcmp(pszName, "DeathMsg") == 0)
				message_DeathMsg = msg;
			else if (strcmp(pszName, "ScreenFade") == 0)
				message_ScreenFade = msg;
			else if (strcmp(pszName, "GameMode") == 0)// XDM3036
				message_GameMode = msg;
			else if (strcmp(pszName, "UpdWeapons") == 0)// XDM3035
				message_UpdWeapons = msg;
			else if (strcmp(pszName, "UpdAmmo") == 0)
				message_UpdAmmo = msg;
			else if (strcmp(pszName, "HLTV") == 0)
				message_HLTV = msg;
//			else if (strcmp(pszName, "TeamNames") == 0)
//				message_TeamNames = msg;
		}
	}
	return msg;
}
void pfnAnimationAutomove(const edict_t *pEdict, float flTime)
{
	(*g_engfuncs.pfnAnimationAutomove)(pEdict, flTime);
}
void pfnGetBonePosition(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles)
{
	(*g_engfuncs.pfnGetBonePosition)(pEdict, iBone, rgflOrigin, rgflAngles);
}
uint32 pfnFunctionFromName(const char *pName)
{
#ifndef __linux__
	return FUNCTION_FROM_NAME(pName);
#else
	return (*g_engfuncs.pfnFunctionFromName)(pName);
#endif
}
const char *pfnNameForFunction(uint32 function)
{
#ifndef __linux__
	return NAME_FOR_FUNCTION(function);
#else
	return (*g_engfuncs.pfnNameForFunction)(function);
#endif
}
void pfnClientPrintf(edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg)
{
	if (!(pEdict->v.flags & FL_FAKECLIENT))
		(*g_engfuncs.pfnClientPrintf)(pEdict, ptype, szMsg);
}
void pfnServerPrint(const char *szMsg)
{
	(*g_engfuncs.pfnServerPrint)(szMsg);
}
void pfnGetAttachment(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles)
{
	(*g_engfuncs.pfnGetAttachment)(pEdict, iAttachment, rgflOrigin, rgflAngles);
}
void pfnCRC32_Init(CRC32_t *pulCRC)
{
	(*g_engfuncs.pfnCRC32_Init)(pulCRC);
}
void pfnCRC32_ProcessBuffer(CRC32_t *pulCRC, void *p, int len)
{
	(*g_engfuncs.pfnCRC32_ProcessBuffer)(pulCRC, p, len);
}
void pfnCRC32_ProcessByte(CRC32_t *pulCRC, unsigned char ch)
{
	(*g_engfuncs.pfnCRC32_ProcessByte)(pulCRC, ch);
}
CRC32_t pfnCRC32_Final(CRC32_t pulCRC)
{
	return (*g_engfuncs.pfnCRC32_Final)(pulCRC);
}
int32 pfnRandomLong(int32 lLow, int32 lHigh)
{
	return (*g_engfuncs.pfnRandomLong)(lLow, lHigh);
}
float pfnRandomFloat(float flLow, float flHigh)
{
	return (*g_engfuncs.pfnRandomFloat)(flLow, flHigh);
}
void pfnSetView(const edict_t *pClient, const edict_t *pViewent)
{
	(*g_engfuncs.pfnSetView)(pClient, pViewent);
}
float pfnTime(void)
{
	return (*g_engfuncs.pfnTime)();
}
void pfnCrosshairAngle(const edict_t *pClient, float pitch, float yaw)
{
	(*g_engfuncs.pfnCrosshairAngle)(pClient, pitch, yaw);
}
byte *pfnLoadFileForMe(char *filename, int *pLength)
{
	return (*g_engfuncs.pfnLoadFileForMe)(filename, pLength);
}
void pfnFreeFile(void *buffer)
{
	(*g_engfuncs.pfnFreeFile)(buffer);
}
void pfnEndSection(const char *pszSectionName)
{
	(*g_engfuncs.pfnEndSection)(pszSectionName);
}
int pfnCompareFileTime(char *filename1, char *filename2, int *iCompare)
{
	return (*g_engfuncs.pfnCompareFileTime)(filename1, filename2, iCompare);
}
void pfnGetGameDir(char *szGetGameDir)
{
	(*g_engfuncs.pfnGetGameDir)(szGetGameDir);
}
void pfnCvar_RegisterVariable(cvar_t *variable)
{
	(*g_engfuncs.pfnCvar_RegisterVariable)(variable);
}
void pfnFadeClientVolume(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
	(*g_engfuncs.pfnFadeClientVolume)(pEdict, fadePercent, fadeOutSeconds, holdTime, fadeInSeconds);
}
void pfnSetClientMaxspeed(const edict_t *pEdict, float fNewMaxspeed)
{
	int index = UTIL_GetBotIndex((edict_t *)pEdict);
	// is this message for a bot?
	if (index != -1)
		bots[index].f_max_speed = fNewMaxspeed;
	
	(*g_engfuncs.pfnSetClientMaxspeed)(pEdict, fNewMaxspeed);
}
edict_t *pfnCreateFakeClient(const char *netname)
{
	return (*g_engfuncs.pfnCreateFakeClient)(netname);
}
void pfnRunPlayerMove(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec )
{
	(*g_engfuncs.pfnRunPlayerMove)(fakeclient, viewangles, forwardmove, sidemove, upmove, buttons, impulse, msec);
}
int pfnNumberOfEntities(void)
{
	return (*g_engfuncs.pfnNumberOfEntities)();
}
char *pfnGetInfoKeyBuffer(edict_t *e)
{
	return (*g_engfuncs.pfnGetInfoKeyBuffer)(e);
}
char *pfnInfoKeyValue(char *infobuffer, char *key)
{
	return (*g_engfuncs.pfnInfoKeyValue)(infobuffer, key);
}
void pfnSetKeyValue(char *infobuffer, char *key, char *value)
{
	(*g_engfuncs.pfnSetKeyValue)(infobuffer, key, value);
}
void pfnSetClientKeyValue(int clientIndex, char *infobuffer, char *key, char *value)
{
	(*g_engfuncs.pfnSetClientKeyValue)(clientIndex, infobuffer, key, value);
}
int pfnIsMapValid(char *filename)
{
	return (*g_engfuncs.pfnIsMapValid)(filename);
}
void pfnStaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
	(*g_engfuncs.pfnStaticDecal)(origin, decalIndex, entityIndex, modelIndex);
}
int pfnPrecacheGeneric(char *s)
{
	return (*g_engfuncs.pfnPrecacheGeneric)(s);
}
int pfnGetPlayerUserId(edict_t *e)
{
	return (*g_engfuncs.pfnGetPlayerUserId)(e);
}
void pfnBuildSoundMsg(edict_t *entity, int channel, const char *sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
	(*g_engfuncs.pfnBuildSoundMsg)(entity, channel, sample, volume, attenuation, fFlags, pitch, msg_dest, msg_type, pOrigin, ed);
}
int pfnIsDedicatedServer(void)
{
	return (*g_engfuncs.pfnIsDedicatedServer)();
}
cvar_t *pfnCVarGetPointer(const char *szVarName)
{
	return (*g_engfuncs.pfnCVarGetPointer)(szVarName);
}
unsigned int pfnGetPlayerWONId(edict_t *e)
{
	return (*g_engfuncs.pfnGetPlayerWONId)(e);
}


// new stuff for SDK 2.0

void pfnInfo_RemoveKey(char *s, const char *key)
{
	(*g_engfuncs.pfnInfo_RemoveKey)(s, key);
}
const char *pfnGetPhysicsKeyValue(const edict_t *pClient, const char *key)
{
	return (*g_engfuncs.pfnGetPhysicsKeyValue)(pClient, key);
}
void pfnSetPhysicsKeyValue(const edict_t *pClient, const char *key, const char *value)
{
	(*g_engfuncs.pfnSetPhysicsKeyValue)(pClient, key, value);
}
const char *pfnGetPhysicsInfoString(const edict_t *pClient)
{
	return (*g_engfuncs.pfnGetPhysicsInfoString)(pClient);
}
unsigned short pfnPrecacheEvent(int type, const char *psz)
{
	return (*g_engfuncs.pfnPrecacheEvent)(type, psz);
}
void pfnPlaybackEvent(int flags, const edict_t *pInvoker, unsigned short eventindex, float delay, float *origin, float *angles, float fparam1,float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
	(*g_engfuncs.pfnPlaybackEvent)(flags, pInvoker, eventindex, delay, origin, angles, fparam1, fparam2, iparam1, iparam2, bparam1, bparam2);
}
unsigned char *pfnSetFatPVS(float *org)
{
	return (*g_engfuncs.pfnSetFatPVS)(org);
}
unsigned char *pfnSetFatPAS(float *org)
{
	return (*g_engfuncs.pfnSetFatPAS)(org);
}
int pfnCheckVisibility(const edict_t *entity, unsigned char *pset)
{
	return (*g_engfuncs.pfnCheckVisibility)(entity, pset);
}
void pfnDeltaSetField(struct delta_s *pFields, const char *fieldname)
{
	(*g_engfuncs.pfnDeltaSetField)(pFields, fieldname);
}
void pfnDeltaUnsetField(struct delta_s *pFields, const char *fieldname)
{
	(*g_engfuncs.pfnDeltaUnsetField)(pFields, fieldname);
}
void pfnDeltaAddEncoder(char *name, void (*conditionalencode)(struct delta_s *pFields, const unsigned char *from, const unsigned char *to))
{
	(*g_engfuncs.pfnDeltaAddEncoder)(name, conditionalencode);
}
int pfnGetCurrentPlayer(void)
{
	return (*g_engfuncs.pfnGetCurrentPlayer)();
}
int pfnCanSkipPlayer(const edict_t *player)
{
	return (*g_engfuncs.pfnCanSkipPlayer)(player);
}
int pfnDeltaFindField(struct delta_s *pFields, const char *fieldname)
{
	return (*g_engfuncs.pfnDeltaFindField)(pFields, fieldname);
}
void pfnDeltaSetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	(*g_engfuncs.pfnDeltaSetFieldByIndex)(pFields, fieldNumber);
}
void pfnDeltaUnsetFieldByIndex(struct delta_s *pFields, int fieldNumber)
{
	(*g_engfuncs.pfnDeltaUnsetFieldByIndex)(pFields, fieldNumber);
}
void pfnSetGroupMask(int mask, int op)
{
	(*g_engfuncs.pfnSetGroupMask)(mask, op);
}
int pfnCreateInstancedBaseline(int classname, struct entity_state_s *baseline)
{
	return (*g_engfuncs.pfnCreateInstancedBaseline)(classname, baseline);
}
void pfnCvar_DirectSet(struct cvar_s *var, char *value)
{
	(*g_engfuncs.pfnCvar_DirectSet)(var, value);
}
void pfnForceUnmodified(FORCE_TYPE type, float *mins, float *maxs, const char *filename)
{
	(*g_engfuncs.pfnForceUnmodified)(type, mins, maxs, filename);
}
void pfnGetPlayerStats(const edict_t *pClient, int *ping, int *packet_loss)
{
	(*g_engfuncs.pfnGetPlayerStats)(pClient, ping, packet_loss);
}
void pfnAddServerCommand(char *cmd_name, void (*function)(void))
{
	(*g_engfuncs.pfnAddServerCommand)(cmd_name, function);
}

// new stuff for SDK 2.3

qboolean pfnVoice_GetClientListening(int iReceiver, int iSender)
{
	return (*g_engfuncs.pfnVoice_GetClientListening)(iReceiver, iSender);
}
qboolean pfnVoice_SetClientListening(int iReceiver, int iSender, qboolean bListen)
{
	return (*g_engfuncs.pfnVoice_SetClientListening)(iReceiver, iSender, bListen);
}
const char *pfnGetPlayerAuthId(edict_t *e)
{
	return (*g_engfuncs.pfnGetPlayerAuthId)(e);
}


// XDM3035
sequenceEntry_s *pfnSequenceGet(const char *fileName, const char *entryName)
{
	return (*g_engfuncs.pfnSequenceGet)(fileName, entryName);
}
sentenceEntry_s *pfnSequencePickSentence(const char *groupName, int pickMethod, int *picked)
{
	return (*g_engfuncs.pfnSequencePickSentence)(groupName, pickMethod, picked);
}
int pfnGetFileSize(char *filename)
{
	return (*g_engfuncs.pfnGetFileSize)(filename);
}
unsigned int pfnGetApproxWavePlayLen(const char *filepath)
{
	return (*g_engfuncs.pfnGetApproxWavePlayLen)(filepath);
}
int pfnIsCareerMatch(void)
{
	return (*g_engfuncs.pfnIsCareerMatch)();
}
int pfnGetLocalizedStringLength(const char *label)
{
	return (*g_engfuncs.pfnGetLocalizedStringLength)(label);
}
void pfnRegisterTutorMessageShown(int mid)
{
	(*g_engfuncs.pfnRegisterTutorMessageShown)(mid);
}
int pfnGetTimesTutorMessageShown(int mid)
{
	return (*g_engfuncs.pfnGetTimesTutorMessageShown)(mid);
}
void pfnProcessTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	(*g_engfuncs.pfnProcessTutorMessageDecayBuffer)(buffer, bufferLength);
}
void pfnConstructTutorMessageDecayBuffer(int *buffer, int bufferLength)
{
	(*g_engfuncs.pfnConstructTutorMessageDecayBuffer)(buffer, bufferLength);
}
void pfnResetTutorMessageDecayData(void)
{
	(*g_engfuncs.pfnResetTutorMessageDecayData)();
}
void pfnQueryClientCvarValue(const edict_t *player, const char *cvarName)
{
	(*g_engfuncs.pfnQueryClientCvarValue)(player, cvarName);
}
void pfnQueryClientCvarValue2(const edict_t *player, const char *cvarName, int requestID)
{
	(*g_engfuncs.pfnQueryClientCvarValue2)(player, cvarName, requestID);
}

/*
mm compatiblity is not implemented
const char *pfnGetPlayerAuthId(edict_t *e)
{
	if ((e->v.flags & FL_FAKECLIENT) || (e->v.flags & FL_THIRDPARTYBOT))
		RETURN_META_VALUE(MRES_SUPERCEDE, "0");

	RETURN_META_VALUE (MRES_IGNORED, NULL);
}*/
