// HPB_bot - botman's High Ping Bastard bot
// Rewritten for Half-Life and X-Half-Life support.
// Many optimizations and extensions.
// ~X~
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "studio.h"
#include "bot.h"
#include "bot_cvar.h"
#include "bot_func.h"
#include "bot_weapons.h"
#include "waypoint.h"
#include "cdll_dll.h"

#include "player.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifndef __linux__
extern HINSTANCE h_Library;
#else
extern void *h_Library;
#endif

extern bot_chat_t bot_chat[];
extern bot_chat_t bot_whine[];
extern int bot_chat_count;
extern int bot_whine_count;
extern cvar_t 	*g_pmp_friendlyfire;

bool g_observer_mode = false;
int num_logos = 0;
char bot_logos[MAX_BOT_LOGOS][16];

// Bot array indexes DO NOT match entity/player indexes!
bot_t bots[MAX_PLAYERS];// max of 32 bots in a game

// how often (out of 1000 times) the bot will pause, based on bot skill
//float pause_frequency[5] = {4, 7, 10, 15, 20};
//float pause_time[5][2] = {{0.2, 0.5}, {0.5, 1.0}, {0.7, 1.3}, {1.0, 1.7}, {1.2, 2.0}};

extern "C" void player(entvars_t *pev);// use existing LINK_ENTITY_TO_FUNC for player


void BOT_CREATE(char *model, const char *name, int skill,
				int topcolor, int bottomcolor, int skin, int reaction,
				int strafe_prc, int chat_prc, int taunt_prc, int whine_prc, char *team)
{
	int clientIndex = 1;
	while (clientIndex <= gpGlobals->maxClients)
	{
		edict_t *pPlayerEdict = INDEXENT(clientIndex);
		if (!pPlayerEdict || pPlayerEdict->free || strlen(STRING(pPlayerEdict->v.netname)) <= 0)// kicked bot IS an edict but it has no netname!
			break;
		++clientIndex;
	}
	if (clientIndex > gpGlobals->maxClients)
	{
		ALERT(at_aiconsole, "XBM: Can't create a bot! No free player slots.\n");
		return;
	}

	if ((name == NULL) && (*name == 0))
	{
		ALERT(at_console, "XBM: Cannot create a bot without name!\n");
		return;
	}

	char c_name[BOT_NAME_LEN+1];
	char c_model[BOT_MODEL_LEN+1];
	char dir_name[32];
	char filename[128];
	struct stat stat_str;
	GET_GAME_DIR(dir_name);

	strncpy(c_name, name, BOT_NAME_LEN-1);
	c_name[BOT_NAME_LEN] = 0;  // make sure c_name is null-terminated

	// XDM: TODO: should we really check this or leave upon the real server?
	strncpy(c_model, model, BOT_MODEL_LEN-1);
	c_model[BOT_MODEL_LEN] = 0;  // make sure c_model is null-terminated

#ifdef _WIN32// windows is not case-sensitive
	strlwr(c_model);// XBM convert to all lowercase?
#endif

	sprintf(filename, "%s/models/player/%s", dir_name, c_model);
	if (stat(filename, &stat_str) != 0)
	{
		sprintf(filename, "valve/models/player/%s", c_model);
		if (stat(filename, &stat_str) != 0)
		{
			ALERT(at_console, "XBM: model '%s' not found for bot %s!\n", c_model, c_name);
			return;
		}
	}

	int index = 0;
	while ((bots[index].is_used) && (index < MAX_PLAYERS))
		++index;

	if (index >= MAX_PLAYERS)
	{
		ALERT(at_console, "XBM: Can't create a bot - no free slots.\n");
		return;
	}
	edict_t *BotEnt = CREATE_FAKE_CLIENT(c_name);
	if (FNullEnt(BotEnt))
	{
		ALERT(at_aiconsole, "XBM: Unable to create a bot client!\n");// don't spam the console
		return;
	}
	BotEnt->v.flags |= FL_FAKECLIENT;
	ALERT(at_aiconsole, "XBM: Creating bot %s (%s,%d)...\n", c_name, c_model, skin);
	// create the player entity by calling MOD's player function
	// (from LINK_ENTITY_TO_CLASS for player object)
	player(VARS(BotEnt));
	char *infobuffer = GET_INFOKEYBUFFER(BotEnt);
	clientIndex = ENTINDEX(BotEnt);

	if (!checked_teamplay)// check for team play...
		BotCheckTeamplay();

	SET_CLIENT_KEYVALUE(clientIndex, infobuffer, "model", c_model);

	if (skin < 0)
		skin = 0;
	else if (skin > MAXSTUDIOSKINS)
		skin = MAXSTUDIOSKINS;

	char buf[8];
	if (skin > 0)
	{
		itoa(skin, buf, 10);
//		ALERT(at_console, ">>> SKIN: %s\n", buf);
		SET_CLIENT_KEYVALUE(clientIndex, infobuffer, "skin", buf);
	}

	SET_CLIENT_KEYVALUE(clientIndex, infobuffer, "team", team);

	if (!is_team_play)
	{
		if (topcolor < 0)
			topcolor = RANDOM_LONG(0,255);
		if (bottomcolor < 0)
			bottomcolor = RANDOM_LONG(0,255);
		itoa(topcolor, buf, 10);
		SET_CLIENT_KEYVALUE(clientIndex, infobuffer, "topcolor", buf);
		itoa(bottomcolor, buf, 10);
		SET_CLIENT_KEYVALUE(clientIndex, infobuffer, "bottomcolor", buf);
	}

	char ptr[128];// allocate space for message from ClientConnect
	ClientConnect(BotEnt, c_name, "127.0.0.1", ptr);

	ClientPutInServer(BotEnt);

	BotEnt->v.flags |= FL_THIRDPARTYBOT;

	if (skill < 1 || skill > BOT_SKILL_LEVELS)
		skill = (int)g_bot_skill.value;

	if (skill < 1)
		skill = 1;
	else if (skill > BOT_SKILL_LEVELS)
		skill = BOT_SKILL_LEVELS;

	if (reaction < 0)
		reaction = (int)g_bot_reaction.value;

	if (reaction < 0)
		reaction = 0;
	else if (reaction > BOT_REACTION_LEVELS)
		reaction = BOT_REACTION_LEVELS;

	if (strafe_prc < 0)
		strafe_prc = (int)g_bot_strafe_percent.value;

	if (chat_prc < 0)
		chat_prc = (int)g_bot_chat_percent.value;

	if (taunt_prc < 0)
		taunt_prc = (int)g_bot_taunt_percent.value;

	if (whine_prc < 0)
		whine_prc = (int)g_bot_whine_percent.value;

	// initialize all the variables for this bot...
	bot_t *pBot = &bots[index];
	pBot->pEdict = BotEnt;
	pBot->is_used = true;
	pBot->respawn_state = RESPAWN_IDLE;
	pBot->f_create_time = gpGlobals->time;
	pBot->name[0] = 0;// name not set by server yet
	strcpy(pBot->model, c_model);
	pBot->pEdict->v.skin = pBot->model_skin = skin;
// no need to	if (!is_team_play)
//	{
		// XBM: remember user-defined values to be able to restore them after level change
		pBot->top_color = topcolor;
		pBot->bottom_color = bottomcolor;
//	}
	pBot->use_type = BOT_USE_NONE;
	pBot->v_hold_origin = Vector(0,0,0);
	pBot->f_exit_water_time = 0.0f;
	BotPickLogo(pBot);
	pBot->not_started = 1;// hasn't joined game yet
	BotSpawnInit(pBot);
	pBot->need_to_initialize = false;// don't need to initialize yet
	BotEnt->v.idealpitch = BotEnt->v.v_angle.x;
	BotEnt->v.ideal_yaw = BotEnt->v.v_angle.y;
	// these should REALLY be MOD dependant...
	BotEnt->v.pitch_speed = 240;// slightly faster than HLDM of 225
	BotEnt->v.yaw_speed = 240;// slightly faster than HLDM of 210
	pBot->idle_angle = 0.0;
	pBot->idle_angle_time = 0.0f;
	// some individuality
	pBot->bot_skill = skill - 1;// 0 based for array indexes
	pBot->chat_percent = chat_prc;
	pBot->taunt_percent = taunt_prc;
	pBot->whine_percent = whine_prc;
	pBot->strafe_percent = strafe_prc;
	pBot->reaction = reaction - 1;// 0 based for array indexes
	pBot->f_bot_chat_time = gpGlobals->time + RANDOM_FLOAT(5, 20);// XDM3035c: set only once!
	// these values are still same for all bots
	pBot->chat_tag_percent = (int)g_bot_chat_tag_percent.value;
	pBot->chat_drop_percent = (int)g_bot_chat_drop_percent.value;
	pBot->chat_swap_percent = (int)g_bot_chat_swap_percent.value;
	pBot->chat_lower_percent = (int)g_bot_chat_lower_percent.value;
	pBot->logo_percent = (int)g_bot_logo_percent.value;
	pBot->f_strafe_direction = 0.0f;// not strafing
	pBot->f_strafe_time = 0.0f;
	pBot->need_to_avenge = false;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn, not only creation!
// Input  : *pBot - 
//-----------------------------------------------------------------------------
void BotSpawnInit(bot_t *pBot)
{
//	pBot->model_skin = 0;
	pBot->v_prev_origin = Vector(9999.0, 9999.0, 9999.0);
	pBot->f_speed_check_time = gpGlobals->time;

	pBot->waypoint_origin = Vector(0, 0, 0);
	pBot->f_waypoint_time = 0.0;
	pBot->curr_waypoint_index = -1;
	pBot->prev_waypoint_index[0] = -1;
	pBot->prev_waypoint_index[1] = -1;
	pBot->prev_waypoint_index[2] = -1;
	pBot->prev_waypoint_index[3] = -1;
	pBot->prev_waypoint_index[4] = -1;

	pBot->f_random_waypoint_time = gpGlobals->time;
	pBot->f_waypoint_goal_time = 0.0;
	pBot->waypoint_goal = -1;
	pBot->waypoint_near_flag = false;
	pBot->waypoint_flag_origin = Vector(0, 0, 0);
	pBot->prev_waypoint_distance = 0.0;

	pBot->weapon_points[0] = 0;
	pBot->weapon_points[1] = 0;
	pBot->weapon_points[2] = 0;
	pBot->weapon_points[3] = 0;
	pBot->weapon_points[4] = 0;
	pBot->weapon_points[5] = 0;
//	pBot->msecnum = 0;
//	pBot->msecdel = 0.0;
//	pBot->msecval = 0.0;
//	pBot->bot_health = 0;
//	pBot->bot_armor = 0;
//	pBot->bot_weapons = 0;
	pBot->blinded_time = 0.0;
	pBot->f_max_speed = g_psv_maxspeed->value;//CVAR_GET_FLOAT("sv_maxspeed");
	pBot->f_prev_speed = 0.0;  // fake "paused" since bot is NOT stuck
	pBot->f_find_item = 0.0;
	pBot->ladder_dir = LADDER_UNKNOWN;
	pBot->f_start_use_ladder_time = 0.0;
	pBot->f_end_use_ladder_time = 0.0;
	pBot->waypoint_top_of_ladder = false;

	pBot->f_wall_check_time = 0.0;
	pBot->f_wall_on_right = 0.0;
	pBot->f_wall_on_left = 0.0;
	pBot->f_dont_avoid_wall_time = 0.0;
	pBot->f_look_for_waypoint_time = 0.0;
	pBot->f_jump_time = 0.0;
	pBot->f_drop_check_time = 0.0;
	// pick a wander direction (50% of the time to the left, 50% to the right)
	if (RANDOM_LONG(1, 100) <= 50)
		pBot->wander_dir = WANDER_LEFT;
	else
		pBot->wander_dir = WANDER_RIGHT;

//	pBot->use_type = 0;
	pBot->v_hold_origin = Vector(0,0,0);
	pBot->f_exit_water_time = 0.0;
	pBot->pBotEnemy = NULL;
//	pBot->pBotUser = NULL;
	pBot->f_bot_see_enemy_time = gpGlobals->time;
	pBot->f_bot_find_enemy_time = gpGlobals->time;
//	pBot->f_aim_tracking_time = 0.0;
	pBot->f_aim_x_angle_delta = 0.0;
	pBot->f_aim_y_angle_delta = 0.0;
	pBot->f_bot_use_time = 0.0;
	pBot->b_bot_say = false;
	pBot->f_bot_say = 0.0;
	pBot->bot_say_msg[0] = 0;
//	pBot->f_bot_chat_time = gpGlobals->time + RANDOM_FLOAT(10, 40);// XDM3035c: Shut up! don't reset this!
//	pBot->enemy_attack_count = 0;
	pBot->f_duck_time = 0.0;
	pBot->f_sniper_aim_time = 0.0;
	pBot->f_shoot_time = gpGlobals->time+0.5f;
	pBot->f_primary_charging = -1.0;
	pBot->f_secondary_charging = -1.0;
	pBot->b_primary_holding = false;
	pBot->b_secondary_holding = false;
	pBot->charging_weapon_id = 0;
	pBot->f_gren_throw_time = -1.0;
//	pBot->f_gren_check_time = 0.0;
	pBot->f_grenade_search_time = 0.0;
	pBot->f_grenade_found_time = 0.0;
//	pBot->f_medic_check_time = 0.0;
//	pBot->f_medic_pause_time = 0.0;
//	pBot->f_medic_yell_time = 0.0;
	pBot->f_pause_time = 0.0;
	pBot->f_sound_update_time = 0.0;
	pBot->bot_has_flag = false;
	pBot->b_see_tripmine = false;
	pBot->b_shoot_tripmine = false;
	pBot->v_tripmine = Vector(0,0,0);
	pBot->b_use_health_station = false;
	pBot->f_use_health_time = 0.0;
	pBot->b_use_HEV_station = false;
	pBot->f_use_HEV_time = 0.0;
	pBot->b_use_button = false;
	pBot->f_use_button_time = 0;
	pBot->b_lift_moving = false;
	pBot->b_spray_logo = false;
	pBot->f_reaction_target_time = 0.0;
	memset(&(pBot->current_weapon), 0, sizeof(pBot->current_weapon));
	memset(&(pBot->m_rgAmmo), 0, sizeof(pBot->m_rgAmmo));
// keep this remembered!	pBot->need_to_avenge = false;
}

int BotInFieldOfView(bot_t *pBot, const Vector &dest)
{
	// find angles from source to destination...
	Vector entity_angles = UTIL_VecToAngles(dest);
	// make yaw angle 0 to 360 degrees if negative...
	if (entity_angles.y < 0)
		entity_angles.y += 360;

	// get bot's current view angle...
	float view_angle = pBot->pEdict->v.v_angle.y;
	// make view angle 0 to 360 degrees if negative...
	if (view_angle < 0)
		view_angle += 360;

	// return the absolute value of angle to destination entity
	// zero degrees means straight ahead,  45 degrees to the left or
	// 45 degrees to the right is the limit of the normal view angle
	// rsm - START angle bug fix
	int angle = abs((int)view_angle - (int)entity_angles.y);
	if (angle > 180)
		angle = 360 - angle;

	return angle;
	// rsm - END
}

bool BotEntityIsVisible(bot_t *pBot, const Vector &dest)
{
	TraceResult tr;
	// trace a line from bot's eyes to destination...
	UTIL_TraceLine(pBot->pEdict->v.origin + pBot->pEdict->v.view_ofs, dest, ignore_monsters, pBot->pEdict->v.pContainingEntity, &tr);
	if (tr.flFraction >= 1.0)// check if line of sight to object is not blocked (i.e. visible)
		return true;
	else
		return false;
}

void BotLogoInit(void)
{
	FILE *bot_logo_fp;
	char bot_logo_filename[256];
	char logo_buffer[80];
	int length, index;

#ifndef __linux__
	HANDLE h_logo;
	char dir_name[32];
	char decal_filename[256];
	DWORD dwDummy;

	struct stat stat_str;
	wadinfo_t wad_header;
	lumpinfo_t lump_info;
	GET_GAME_DIR(dir_name);
	sprintf(decal_filename, "%s/decals.wad", dir_name);

	if (stat(decal_filename, &stat_str) != 0)
	{
		strcpy(decal_filename, "valve/decals.wad");
//		UTIL_Pathname_Convert(decal_filename);
		if (stat(decal_filename, &stat_str) != 0)
			return;  // file not found
	}

	h_logo = CreateFile(decal_filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (h_logo == INVALID_HANDLE_VALUE)
		return;  // can't open file

	if (!ReadFile(h_logo, &wad_header, sizeof(wadinfo_t), &dwDummy, NULL))
	{
		CloseHandle(h_logo);  // can't read wad header
		return;
	}

	if (SetFilePointer(h_logo, wad_header.infotableofs, NULL, FILE_BEGIN) == 0)
	{
		CloseHandle(h_logo);
		return;  // can't seek to lump info table
	}

	for (index=0; index < wad_header.numlumps; ++index)
	{
		if (!ReadFile(h_logo, &lump_info, sizeof(lumpinfo_t), &dwDummy, NULL))
		{
			CloseHandle(h_logo);  // can't read lump info
			return;
		}

		if (strncmp(lump_info.name, "{__", 3) == 0)
		{
			strcpy(bot_logos[num_logos], lump_info.name);
			++num_logos;
		}

		if (strncmp(lump_info.name, "__", 2) == 0)
		{
			strcpy(bot_logos[num_logos], lump_info.name);
			++num_logos;
		}
	}

	CloseHandle(h_logo);

#endif
	char *logocfgfile;// XDM
	if (mod_id == TYRIAN_DLL)// XDM
		logocfgfile = "botlogo.lst";
	else
		logocfgfile = "HPB_bot_logo.cfg";

	UTIL_BuildFileName(bot_logo_filename, logocfgfile, NULL);
	bot_logo_fp = fopen(bot_logo_filename, "r");
	if (bot_logo_fp != NULL)
	{
		while ((num_logos < MAX_BOT_LOGOS) && (fgets(logo_buffer, 80, bot_logo_fp) != NULL))
		{
			length = strlen(logo_buffer);

			if (logo_buffer[length-1] == '\n')
			{
				logo_buffer[length-1] = 0;  // remove '\n'
				--length;
			}

			if (logo_buffer[0] != 0)
			{
				strncpy(bot_logos[num_logos], logo_buffer, 16);
				++num_logos;
			}
		}
		fclose(bot_logo_fp);
	}
}

void BotPickLogo(bot_t *pBot)
{
	if (num_logos == 0)
		return;

	pBot->logo_name[0] = 0;
	int logo_index = RANDOM_LONG(1, num_logos) - 1;  // zero based
	// check make sure this name isn't used
	bool used = true;
	int check_count = 0;
	int index;
	while ((used) && (check_count < MAX_BOT_LOGOS))
	{
		used = false;
		for (index=0; index < MAX_PLAYERS; ++index)
		{
			if (bots[index].is_used)
			{
				if (strcmp(bots[index].logo_name, bot_logos[logo_index]) == 0)
					used = true;
			}
		}
		if (used)
			++logo_index;

		if (logo_index == MAX_BOT_LOGOS)
			logo_index = 0;

		++check_count;
	}
	strcpy(pBot->logo_name, bot_logos[logo_index]);
}

void BotTrySprayLogo(bot_t *pBot)
{
	edict_t *pEdict = pBot->pEdict;
	if (pBot->b_spray_logo && ((pBot->f_spray_logo_time + 3.0) < gpGlobals->time))// took too long trying to spray logo?...
	{
		pBot->b_spray_logo = false;
		pEdict->v.idealpitch = 0.0f;
	}
	if (pBot->b_spray_logo)  // trying to spray a logo?
	{
		Vector v_src, v_dest, angle;
		TraceResult tr;
		// find the nearest wall to spray logo on (or floor)...
		angle = pEdict->v.v_angle;
		angle.x = 0;  // pitch is level horizontally
		UTIL_MakeVectors( angle );
		v_src = pEdict->v.origin + pEdict->v.view_ofs;
		v_dest = v_src + gpGlobals->v_forward * 100.0f;
		UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters, pEdict->v.pContainingEntity, &tr);
		if (tr.flFraction < 1.0)
		{
			// already facing the correct yaw, just set pitch...
			pEdict->v.idealpitch = RANDOM_FLOAT(0.0f, 30.0f) - 15.0f;
		}
		else
		{
			v_dest = v_src + gpGlobals->v_right * 100.0f;  // to the right
			UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters, pEdict->v.pContainingEntity, &tr);
			if (tr.flFraction < 1.0)
			{
				// set the ideal yaw and pitch...
				Vector bot_angles = UTIL_VecToAngles(v_dest - v_src);
				pEdict->v.ideal_yaw = bot_angles.y;
				BotFixIdealYaw(pEdict);
				pEdict->v.idealpitch = RANDOM_FLOAT(0.0, 30.0) - 15.0f;
			}
			else
			{
				v_dest = v_src + gpGlobals->v_right * -100;  // to the left
				UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters, pEdict->v.pContainingEntity, &tr);
				if (tr.flFraction < 1.0)
				{
					// set the ideal yaw and pitch...
					Vector bot_angles = UTIL_VecToAngles(v_dest - v_src);
					pEdict->v.ideal_yaw = bot_angles.y;
					BotFixIdealYaw(pEdict);
					pEdict->v.idealpitch = RANDOM_FLOAT(0.0, 30.0) - 15.0f;
				}
				else
				{
					v_dest = v_src + gpGlobals->v_forward * -100;  // behind
					UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters, pEdict->v.pContainingEntity, &tr);
					if (tr.flFraction < 1.0)
					{
						// set the ideal yaw and pitch...
						Vector bot_angles = UTIL_VecToAngles(v_dest - v_src);
						pEdict->v.ideal_yaw = bot_angles.y;
						BotFixIdealYaw(pEdict);
						pEdict->v.idealpitch = RANDOM_FLOAT(0.0, 30.0) - 15.0f;
					}
					else
					{
						// on the ground...
						angle = pEdict->v.v_angle;
						angle.x = 85.0f;  // 85 degrees is downward
						UTIL_MakeVectors( angle );
						v_src = pEdict->v.origin + pEdict->v.view_ofs;
						v_dest = v_src + gpGlobals->v_forward * 80;
						UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters, pEdict->v.pContainingEntity, &tr);

						if (tr.flFraction < 1.0)
						{
							// set the pitch...
							pEdict->v.idealpitch = 85.0f;
							BotFixIdealPitch(pEdict);
						}
					}
				}
			}
		}
		pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0f;
		// is there a wall close to us?
		UTIL_MakeVectors( pEdict->v.v_angle );
		v_src = pEdict->v.origin + pEdict->v.view_ofs;
		v_dest = v_src + gpGlobals->v_forward * 80;
		UTIL_TraceLine( v_src, v_dest, dont_ignore_monsters, pEdict->v.pContainingEntity, &tr);
		if (tr.flFraction < 1.0)
		{
			BotSprayLogo(pEdict, pBot->logo_name);

			pBot->b_spray_logo = false;
			pEdict->v.idealpitch = 0.0f;
		}
	}
}

void BotSprayLogo(edict_t *pEntity, const char *logo_name)
{
	int index = DECAL_INDEX(logo_name);
	if (index < 0)
	  return;

	TraceResult pTrace;
	Vector v_src, v_dest;
	UTIL_MakeVectors(pEntity->v.v_angle);
	v_src = pEntity->v.origin + pEntity->v.view_ofs;
	v_dest = v_src + gpGlobals->v_forward * 80;
	UTIL_TraceLine(v_src, v_dest, ignore_monsters, pEntity->v.pContainingEntity, &pTrace);

	if (pTrace.pHit && (pTrace.flFraction < 1.0))
	{
		if (pTrace.pHit->v.solid != SOLID_BSP)
			return;

		if (g_bot_logo_custom.value > 0)
		{
			// XDM3035: I dont' really understand why we can't use this because it works
			MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
				WRITE_BYTE(TE_PLAYERDECAL);
				WRITE_BYTE(ENTINDEX(pEntity));
				WRITE_COORD(pTrace.vecEndPos.x);
				WRITE_COORD(pTrace.vecEndPos.y);
				WRITE_COORD(pTrace.vecEndPos.z);
				WRITE_SHORT((short)ENTINDEX(pTrace.pHit));
				WRITE_BYTE(index);
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			if (index > 255)
			{
				WRITE_BYTE(TE_WORLDDECALHIGH);
				index -= 256;
			}
			else
			{
				WRITE_BYTE(TE_WORLDDECAL);
			}
				WRITE_COORD(pTrace.vecEndPos.x);
				WRITE_COORD(pTrace.vecEndPos.y);
				WRITE_COORD(pTrace.vecEndPos.z);
				WRITE_BYTE(index);
			MESSAGE_END();
		}
		EMIT_SOUND_DYN2(pEntity, CHAN_STATIC, "player/sprayer.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);// XDM: was CHAN_VOICE
	}
}

void BotFindItem(bot_t *pBot)
{
	edict_t *pent = NULL;
	edict_t *pPickupEntity = NULL;
	Vector pickup_origin;
	Vector entity_origin;
	float radius = 500;
	bool can_pickup;
	float min_distance;
	char item_name[40];
	TraceResult tr;
	Vector vecStart;
	Vector vecEnd;
	int angle_to_entity;
	edict_t *pEdict = pBot->pEdict;
	pBot->pBotPickupItem = NULL;
	// use a MUCH smaller search radius when waypoints are available
	if ((num_waypoints > 0) && (pBot->curr_waypoint_index != -1))
		radius = 100.0;
	else
		radius = 500.0;

	min_distance = radius + 1.0f;
	while ((pent = UTIL_FindEntityInSphere(pent, pEdict->v.origin, radius)) != NULL)
	{
		can_pickup = false;  // assume can't use it until known otherwise
		strcpy(item_name, STRING(pent->v.classname));
		// see if this is a "func_" type of entity (func_button, etc.)...
		if (strncmp("func_", item_name, 5) == 0)
		{
			// BModels have 0,0,0 for origin so must use VecBModelOrigin...
			entity_origin = VecBModelOrigin(VARS(pent));
			vecStart = pEdict->v.origin + pEdict->v.view_ofs;
			vecEnd = entity_origin;
			angle_to_entity = BotInFieldOfView( pBot, vecEnd - vecStart );
			// check if entity is outside field of view (+/- 45 degrees)
			if (angle_to_entity > 45)
				continue;  // skip this item if bot can't "see" it

			// check if entity is a ladder (ladders are a special case)
			// DON'T search for ladders if there are waypoints in this level...
			if ((strcmp("func_ladder", item_name) == 0) && (num_waypoints == 0))
			{
				// force ladder origin to same z coordinate as bot since
				// the VecBModelOrigin is the center of the ladder.  For
				// LONG ladders, the center MAY be hundreds of units above
				// the bot.  Fake an origin at the same level as the bot...
				entity_origin.z = pEdict->v.origin.z;
				vecEnd = entity_origin;
				// trace a line from bot's eyes to func_ladder entity...
				UTIL_TraceLine( vecStart, vecEnd, dont_ignore_monsters, pEdict->v.pContainingEntity, &tr);
				// check if traced all the way up to the entity (didn't hit wall)
				if (tr.flFraction >= 1.0)
				{
					// find distance to item for later use...
					float distance = (vecEnd - vecStart).Length();
					// use the ladder about 100% of the time, if haven't used a ladder in at least 5 seconds...
					if ((RANDOM_LONG(1, 100) <= 100) && ((pBot->f_end_use_ladder_time + 5.0) < gpGlobals->time))
					{
						// if close to ladder...
						if (distance < 100)// don't avoid walls for a while
							pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0f;

						can_pickup = true;
					}
				}
			}
			else
			{
				// trace a line from bot's eyes to func_ entity...
				UTIL_TraceLine(vecStart, vecEnd, dont_ignore_monsters, pEdict->v.pContainingEntity, &tr);
				if (tr.pHit)// XDM3035c: can be NULL!
				{
				// check if traced all the way up to the entity (didn't hit wall)
				if (strcmp(item_name, STRING(tr.pHit->v.classname)) == 0)
				{
					// find distance to item for later use...
					float distance = (vecEnd - vecStart).Length();
					// check if entity is wall mounted health charger...
					if (strcmp("func_healthcharger", item_name) == 0)
					{
						// check if the bot can use this item and
						// check if the recharger is ready to use (has power left)...
						if ((pEdict->v.health < MAX_PLAYER_HEALTH) && (pent->v.frame == 0))
						{
							// check if flag not set...
							if (!pBot->b_use_health_station)
							{
								// check if close enough and facing it directly...
								if ((distance < PLAYER_SEARCH_RADIUS) && (angle_to_entity <= 10))
								{
									pBot->b_use_health_station = true;
									pBot->f_use_health_time = gpGlobals->time;
								}
								// if close to health station...
								if (distance < 100)// don't avoid walls for a while
									pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0f;

								can_pickup = true;
							}
						}
						else// don't need or can't use this item...
							pBot->b_use_health_station = false;
					}
					else if (strcmp("func_recharge", item_name) == 0)// check if entity is wall mounted HEV charger...
					{
						// check if the bot can use this item and
						// check if the recharger is ready to use (has power left)...
						if ((pEdict->v.armorvalue < MAX_NORMAL_BATTERY) && (pent->v.frame == 0))
						{
							// check if flag not set and facing it...
							if (!pBot->b_use_HEV_station)
							{
								// check if close enough and facing it directly...
								if ((distance < PLAYER_SEARCH_RADIUS) && (angle_to_entity <= 10))
								{
									pBot->b_use_HEV_station = true;
									pBot->f_use_HEV_time = gpGlobals->time;
								}
								// if close to HEV recharger...
								if (distance < 100)// don't avoid walls for a while
									pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0f;

								can_pickup = true;
							}
						}
						else// don't need or can't use this item...
							pBot->b_use_HEV_station = false;
					}
					else if (strcmp("func_button", item_name) == 0)// check if entity is a button...
					{
						// use the button about 100% of the time, if haven't
						// used a button in at least 5 seconds...
						if ((RANDOM_LONG(1, 100) <= 100) && ((pBot->f_use_button_time + 5) < gpGlobals->time))
						{
							// check if flag not set and facing it...
							if (!pBot->b_use_button)
							{
								// check if close enough and facing it directly...
								if ((distance < PLAYER_SEARCH_RADIUS) && (angle_to_entity <= 10))
								{
									pBot->b_use_button = true;
									pBot->b_lift_moving = false;
									pBot->f_use_button_time = gpGlobals->time;
								}
								// if close to button...
								if (distance < 100)// don't avoid walls for a while
									pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0f;

								can_pickup = true;
							}
						}
						else// don't need or can't use this item...
							pBot->b_use_button = false;
					}
				}// hit classname
				}// tr.pHit
			}
		}
		else if (strncmp("trigger_", item_name, 8) == 0)// see if this is a "trigger_" type of entity (func_button, etc.)...
		{
			entity_origin = (pent->v.absmax + pent->v.absmin) * 0.5f;
			vecStart = pEdict->v.origin + pEdict->v.view_ofs;
			vecEnd = entity_origin;
			// find angles from bot origin to entity...
			angle_to_entity = BotInFieldOfView(pBot, vecEnd - vecStart);
			// check if entity is outside field of view (+/- 45 degrees)
			if (angle_to_entity > 45)
				continue;  // skip this item if bot can't "see" it
			// check if line of sight to object is not blocked (i.e. visible)
			if (BotEntityIsVisible(pBot, vecEnd) && pent->v.solid != SOLID_NOT)// XDM3035c
			{
				if (strcmp("hurt", item_name+8) == 0 && !(pent->v.spawnflags & 8) && (pent->v.solid != SOLID_NOT) && (pent->v.dmg > 0.0f))// & SF_TRIGGER_HURT_NO_CLIENTS))// treat dangerous trigger as a grenade
				{
					pBot->f_grenade_found_time = gpGlobals->time;
					pBot->f_move_speed = pBot->f_max_speed;// will be reversed later
					pBot->f_dont_avoid_wall_time += 4.0f;
					pBot->f_wall_check_time += 4.0f;
				}
				else if (is_team_play)// these are valid in teamplay only
				{
					if (strcmp("dom_point", item_name+8) == 0)// XDM: hold domination point
					{
						if (pent->v.team != pEdict->v.team)
							can_pickup = true;
					}
				}
			}
		}
		else if (is_team_play && strcmp("info_capture_obj", item_name) == 0)// XDM: take the flag
		{
			// We don't check if this flag is carried by someone, pretend we're gonna follow the flag carrier
			if (pent->v.team != pEdict->v.team)
				can_pickup = true;// UNDONE: take the flag to the base
		}
		else // everything else... (!func_ && !trigger_)
		{
			if (pent->v.effects & EF_NODRAW)// XDM3035c: TESTME
				continue;

			entity_origin = pent->v.origin;
			vecStart = pEdict->v.origin + pEdict->v.view_ofs;
			vecEnd = entity_origin;
			// find angles from bot origin to entity...
			angle_to_entity = BotInFieldOfView(pBot, vecEnd - vecStart);
			// check if entity is outside field of view (+/- 45 degrees)
			if (angle_to_entity > 45)
				continue;  // skip this item if bot can't "see" it
			// check if line of sight to object is not blocked (i.e. visible)
			if (BotEntityIsVisible(pBot, vecEnd))
			{
				if (strncmp("weapon_", item_name, 7) == 0)// check if entity is a weapon...
				{
					if (mod_id == TYRIAN_DLL && XDM_CanHaveItem)
					{
						if (XDM_CanHaveItem(pEdict, pent) == 0)// XDM3035c: real inventory check
							continue;
					}
					can_pickup = true;
				}
				else if (strncmp("ammo_", item_name, 5) == 0)// check if entity is ammo...
				{
					can_pickup = true;
				}
				else if (strcmp("weaponbox", item_name) == 0)// check if entity is a packed up weapons box...
				{
					can_pickup = true;
				}
			}  // end if object is visible
		}  // end else not "func_" entity

		if (can_pickup) // if the bot found something it can pickup...
		{
			float distance = (entity_origin - pEdict->v.origin).Length();
			// see if it's the closest item so far...
			if (distance < min_distance)
			{
				min_distance = distance;	// update the minimum distance
				pPickupEntity = pent;		// remember this entity
				pickup_origin = entity_origin;	// remember location of entity
			}
		}
	}  // end while loop

	if (pPickupEntity != NULL)
	{
		// let's head off toward that item...
		Vector v_item = pickup_origin - pEdict->v.origin;
		Vector bot_angles = UTIL_VecToAngles( v_item );
		pEdict->v.ideal_yaw = bot_angles.y;
		BotFixIdealYaw(pEdict);
		pBot->pBotPickupItem = pPickupEntity;  // save the item bot is trying to get
	}
}

/*bool BotLookForMedic(bot_t *pBot)
{
	return false;
}*/

#define MINE_SAFE_RADIUS 400

bool BotLookForGrenades(bot_t *pBot)
{
	char classname[40];
	Vector entity_origin;
	float radius = 500;
	edict_t *pEdict = pBot->pEdict;
	edict_t *pent= NULL;
	while ((pent = UTIL_FindEntityInSphere( pent, pEdict->v.origin, radius )) != NULL)
	{
		strcpy(classname, STRING(pent->v.classname));
		entity_origin = pent->v.origin;
		if (FInViewCone(entity_origin, pEdict) && FVisible(entity_origin, pEdict))
		{
			if (mod_id == TYRIAN_DLL)// XDM
			{
				if (strcmp("BioMissile", classname) == 0)// XDM3035
				{
					if (is_team_play && g_pmp_friendlyfire && g_pmp_friendlyfire->value <= 0.0f)
					{
						if (pent->v.team > 0 && pent->v.team == pEdict->v.team && pent->v.owner != pBot->pEdict)
							return false;
					}
					return true;
				}
				else if (strcmp("squeakbox", classname) == 0)
				{
					if (is_team_play)// XDM3035
					{
						if (pent->v.team > 0 && pent->v.team == pEdict->v.team)
							return false;
					}
					return true;
				}
			}

			if (strcmp("grenade", classname) == 0)
			{
				if (is_team_play && g_pmp_friendlyfire && g_pmp_friendlyfire->value <= 0.0f)
				{
					if (pent->v.team > 0 && pent->v.team == pEdict->v.team && pent->v.owner != pBot->pEdict)
						return false;
				}

				if (!(pent->v.effects & EF_NODRAW))
					return true;
			}
			else if (strcmp("monster_snark", classname) == 0)
			{
				if (is_team_play)// XDM3035
				{
					if (pent->v.team > 0 && pent->v.team == pEdict->v.team)
						return false;
				}
				return true;
			}
			else if (strcmp("monster_tripmine", classname) == 0)// check if entity is an armed tripmine
			{
				if (is_team_play)
				{
					if (pent->v.team > 0 && pent->v.team == pEdict->v.team)
						continue;
				}
				float distance = (pent->v.origin - pEdict->v.origin).Length();
				if (pBot->b_see_tripmine)
				{
					// see if this tripmine is closer to bot...
					if (distance < (pBot->v_tripmine - pEdict->v.origin).Length())
					{
						pBot->v_tripmine = pent->v.origin;
						pBot->b_shoot_tripmine = false;
						// see if bot is far enough to shoot the tripmine...
						if (distance >= 512)
							pBot->b_shoot_tripmine = true;
					}
				}
				else
				{
					pBot->b_see_tripmine = true;
					pBot->v_tripmine = pent->v.origin;
					pBot->b_shoot_tripmine = false;
					// see if bot is far enough to shoot the tripmine...
					if (distance >= 512)  // 375 is damage radius
						pBot->b_shoot_tripmine = true;
				}
			}

		}
	}
	return false;  // no grenades were found
}

void BotThink(bot_t *pBot)
{
//	int index = 0;
	Vector v_diff;				// vector from previous to current location
	float pitch_degrees;
	float yaw_degrees;
	float moved_distance;		// length of v_diff vector (distance bot moved)
	TraceResult tr;
	bool found_waypoint;
	bool is_idle;
	float f_strafe_speed;
	bool bCrouchJump;

	edict_t *pEdict = pBot->pEdict;
	pEdict->v.flags |= FL_FAKECLIENT;

	if (pBot->name[0] == 0)  // name filled in yet?
		strcpy(pBot->name, STRING(pBot->pEdict->v.netname));

	pBot->f_frame_time = msecval / 1000;  // calculate frame time

	pBot->f_max_speed = g_psv_maxspeed->value;

	pEdict->v.button = 0;
	pBot->f_move_speed = 0.0f;

	// if the bot hasn't selected stuff to start the game yet, go do that...
	if (pBot->not_started)
	{
//		BotStartGame(pBot);
		// XDM: do bot start code here
		BotFixIdealPitch(pEdict);
		BotFixIdealYaw(pEdict);
		BotFixBodyAngles(pEdict);
		BotFixViewAngles(pEdict);
		g_engfuncs.pfnRunPlayerMove(pEdict, pEdict->v.v_angle, pBot->f_move_speed, 0, 0, pEdict->v.button, 0, msecval);
		pBot->not_started = false;
		return;
	}

	// does bot need to say a message and time to say a message?
	if (pBot->b_bot_say && pBot->f_bot_say < gpGlobals->time)
	{
		pBot->b_bot_say = false;
		UTIL_HostSay(pEdict, 0, pBot->bot_say_msg);
	}
	// if the bot is dead, randomly press fire to respawn...
	if ((pEdict->v.health < 1) || (pEdict->v.deadflag != DEAD_NO))
	{
		if (pBot->need_to_initialize)
		{
			BotSpawnInit(pBot);
			pBot->need_to_initialize = false;
			// did another player kill this bot AND bot whine messages loaded AND
			// has the bot been alive for at least 15 seconds AND
			if ((g_bot_chat_enable.value > 0.0f) && (pBot->pBotKiller != NULL) && ((pBot->f_bot_spawn_time + 15.0) <= gpGlobals->time) && (pBot->pBotKiller->v.flags & (FL_CLIENT|FL_MONSTER|FL_FAKECLIENT)))// XDM3035c: TESTME: don't talk to triggers!
			{
				if ((RANDOM_LONG(1,100) <= pBot->whine_percent))
					BotSpeakWhine(pBot, pBot->pBotKiller);
			}
		}

		if (RANDOM_LONG(1, 100) > 60)
			pEdict->v.button = IN_ATTACK;

		BotFixIdealPitch(pEdict);
		BotFixIdealYaw(pEdict);
		BotFixBodyAngles(pEdict);
		BotFixViewAngles(pEdict);
		g_engfuncs.pfnRunPlayerMove(pEdict, pEdict->v.v_angle, pBot->f_move_speed, 0, 0, pEdict->v.button, 0, msecval);
		return;
	}

	if (pBot->pEdict->v.iuser1 != 0)//OBS_NONE)// XDM3035c: TESTME! Spectators don't move!
		return;

	// XDM3035: used by a player (invoked by XDM code)
	if (pBot->pEdict->v.owner)
	{
		if (pBot->pEdict->v.owner->v.flags & FL_CLIENT)
		{
			// fixed some bad exploits here
			int accepted = BotUseCommand(pBot, pBot->pEdict->v.owner, pBot->use_type+1);
			// bot will notice use even if the command is rejected
			if (!accepted || pBot->use_type == BOT_USE_FOLLOW)// but don't change angles when not nescessary
			{
				// don't put this code inside BotUseCommand()!
	//			Vector bot_angles = UTIL_VecToAngles(pEdict->v.owner->v.origin - pEdict->v.origin);
				Vector bot_angles;
				VEC_TO_ANGLES(pEdict->v.owner->v.origin - pEdict->v.origin, bot_angles);// XDM3037: faster
				pEdict->v.ideal_yaw = bot_angles.y;
				BotFixIdealYaw(pEdict);
			}
		}
		pBot->pEdict->v.owner = NULL;// reset owner so this won't get called twice
	}

	if (g_bot_chat_enable.value > 0.0f && pBot->f_bot_chat_time < gpGlobals->time)
	{
		if (RANDOM_LONG(1,100) <= pBot->chat_percent)
			BotSpeakChat(pBot);

		pBot->f_bot_chat_time = gpGlobals->time + BOT_CHAT_TRY_INTERVAL;
	}

	// set this for the next time the bot dies so it will initialize stuff
	if (pBot->need_to_initialize == false)
	{
		pBot->need_to_initialize = true;
		pBot->f_bot_spawn_time = gpGlobals->time;
	}
	is_idle = false;

	if (pBot->pEdict->v.flags & (FL_FROZEN))
		is_idle = true;  // don't do anything while frozen

	if (pBot->blinded_time > gpGlobals->time)
		is_idle = true;  //TODO: don't shoot while blinded

	if (is_idle)
	{
		if (pBot->idle_angle_time <= gpGlobals->time)
		{
			pBot->idle_angle_time = gpGlobals->time + RANDOM_FLOAT(0.5, 2.0);
			pEdict->v.ideal_yaw = pBot->idle_angle + RANDOM_FLOAT(0.0, 60.0) - 30.0f;
			BotFixIdealYaw(pEdict);
		}

		// turn towards ideal_yaw by yaw_speed degrees (slower than normal)
		BotChangeYaw( pBot, pEdict->v.yaw_speed / 2 );

		BotFixIdealPitch(pEdict);
		BotFixIdealYaw(pEdict);
		BotFixBodyAngles(pEdict);
		BotFixViewAngles(pEdict);
		g_engfuncs.pfnRunPlayerMove(pEdict, pEdict->v.v_angle, pBot->f_move_speed, 0, 0, pEdict->v.button, 0, msecval);
		return;
	}
	else
		pBot->idle_angle = pEdict->v.v_angle.y;

	// check if time to check for player sounds (if don't already have enemy)
	if ((pBot->f_sound_update_time <= gpGlobals->time) && (pBot->pBotEnemy == NULL))
	{
		int ind;
		edict_t *pPlayer;
		pBot->f_sound_update_time = gpGlobals->time + 1.0f;

		for (ind = 1; ind <= gpGlobals->maxClients; ++ind)
		{
			pPlayer = INDEXENT(ind);
			// is this player slot is valid and it's not this bot...
			if ((pPlayer) && (!pPlayer->free) && (pPlayer != pEdict))
			{
				if (IsAlive(pPlayer) && (FBitSet(pPlayer->v.flags, FL_CLIENT) || FBitSet(pPlayer->v.flags, FL_FAKECLIENT)))
				{
					// check for sounds being made by other players...
					if (UpdateSounds(pEdict, pPlayer))
					{
						// don't check for sounds for another 30 seconds
						pBot->f_sound_update_time = gpGlobals->time + 30.0f;
					}
				}
			}
		}
	}

	if (pBot->use_type == BOT_USE_NONE || (g_bot_follow_actions.value == 0.0f))// XDM3037: don't decide this on my own if following
	{
		if (((pEdict->v.effects & EF_DIMLIGHT) != 0) != (g_bot_use_flashlight.value > 0.0f))
			pEdict->v.impulse = 100;// toggle flashlight
	}

//XDM	pBot->f_move_speed = pBot->f_max_speed;  // set to max speed

	if (pBot->f_speed_check_time <= gpGlobals->time)
	{
		// see how far bot has moved since the previous position...
		v_diff = pBot->v_prev_origin - pEdict->v.origin;
		moved_distance = v_diff.Length();
		// save current position as previous
		pBot->v_prev_origin = pEdict->v.origin;
		pBot->f_speed_check_time = gpGlobals->time + 0.2f;
	}
	else
		moved_distance = 2.0;

	// if the bot is not underwater AND not in the air (or on ladder),
	// check if the bot is about to fall off high ledge or into water...
	if (pEdict->v.waterlevel != 3 && pEdict->v.flags & FL_ONGROUND && pEdict->v.movetype != MOVETYPE_FLY && (pBot->f_drop_check_time < gpGlobals->time))
	{
		pBot->f_drop_check_time = gpGlobals->time + 0.05f;
		if (BotLookForDrop(pBot))
			pBot->f_move_speed *= 0.1f;// XDM3037
	}

	// turn towards ideal_pitch by pitch_speed degrees
	pitch_degrees = BotChangePitch(pBot, pEdict->v.pitch_speed);
	// turn towards ideal_yaw by yaw_speed degrees
	yaw_degrees = BotChangeYaw(pBot, pEdict->v.yaw_speed);

	if ((pitch_degrees >= 45) || (yaw_degrees >= 45))
	{
		pBot->f_move_speed = 0.0f;  // don't move while turning a lot
	}
	else if ((pitch_degrees >= 30) || (yaw_degrees >= 30))
	{
		pBot->f_move_speed *= 0.1f;// slow down while turning
	}
	else if ((pitch_degrees >= 20) || (yaw_degrees >= 20))
	{
		pBot->f_move_speed *= 0.25f;// slow down while turning
	}
	//else  // else handle movement related actions...
	{
		bool do_movement_processing = false;// XDM3035a: check for walls/jumping/crouching/ladders?

		if (GameRulesHaveGoal())// TODO: reaching goal should be 1st priority
		{
			pBot->waypoint_goal = WaypointFindNearestGoal(pEdict, pBot->curr_waypoint_index, pBot->pEdict->v.team, W_FL_FLAG_GOAL);
		}

//			if (pBot->waypoint_goal == -1)// XDM3036
		if (pBot->pEdict->v.weapons && pBot->current_weapon.iId != WEAPON_NONE)// XDM3037: useless to search for enemy with empty hands
			pBot->pBotEnemy = BotFindEnemy(pBot);

		if (pBot->pBotEnemy)// does an enemy exist?
		{
			if (pBot->f_reaction_target_time <= gpGlobals->time)// XDM3035c
			{
				if (BotShootAtEnemy(pBot) == false)// shoot at the enemy
				{
					pBot->f_move_speed = pBot->f_max_speed;
					do_movement_processing = true;
				}
				pBot->f_reaction_target_time = 0.0f;
			}
			pBot->f_pause_time = 0;// dont't pause if enemy exists
		}
		else if (pBot->f_pause_time > gpGlobals->time)// is bot "paused"?
		{
			// you could make the bot look left then right, or look up
			// and down, to make it appear that the bot is hunting for
			// something (don't do anything right now)
		}
		else if (pBot->pBotUser != NULL && pBot->use_type == BOT_USE_FOLLOW && BotFollowUser(pBot))
		{// is bot being "used" and can still follow "user"?
			pBot->f_strafe_direction = 0.0f;
			do_movement_processing = true;
		}
		else if (pBot->pBotUser != NULL && pBot->use_type == BOT_USE_HOLD && BotHoldPosition(pBot))
		{
			pBot->f_strafe_direction = 0.0f;
		}
		else// no enemy, let's just wander around...
		{
			do_movement_processing = true;
			pBot->f_move_speed = pBot->f_max_speed;

			BotTrySprayLogo(pBot);

			// is bot NOT under water AND not trying to spray a logo
			if (pEdict->v.waterlevel != 2 && pEdict->v.waterlevel != 3 && !pBot->b_spray_logo)
			{
				// reset pitch to 0 (level horizontally)
				pEdict->v.idealpitch = 0;
				pEdict->v.v_angle.x = 0;
			}

			// check if bot should look for items now or not...
			if (pBot->f_find_item <= gpGlobals->time)
			{
				pBot->f_find_item = gpGlobals->time + 0.1f;
				BotFindItem( pBot );  // see if there are any visible items
			}

			// check if bot sees a tripmine...
			if (pBot->b_see_tripmine)
			{
				// check if bot can shoot the tripmine...
				if (pBot->b_shoot_tripmine && BotShootTripmine(pBot))
				{
					// shot at tripmine, may or may not have hit it, clear
					// flags anyway, next BotFindItem will see it again if
					// it is still there...
					pBot->b_shoot_tripmine = false;
					pBot->b_see_tripmine = false;
					// pause for a while to allow tripmine to explode...
					pBot->f_pause_time = gpGlobals->time + 0.5f;
				}
				else  // run away!!!
				{
					Vector tripmine_angles;
					tripmine_angles = UTIL_VecToAngles( pBot->v_tripmine - pEdict->v.origin );
					// face away from the tripmine
					pEdict->v.ideal_yaw += 180;  // rotate 180 degrees
					BotFixIdealYaw(pEdict);
					pBot->b_see_tripmine = false;
					pBot->f_move_speed = 0;  // don't run while turning
				}
			}
			else if (pBot->b_use_health_station)// check if should use wall mounted health station...
			{
				if ((pBot->f_use_health_time + 10.0) > gpGlobals->time)
				{
					pBot->f_move_speed = 0;  // don't move while using health station
					pEdict->v.button = IN_USE;
				}
				else// bot is stuck trying to "use" a health station...
				{
					pBot->b_use_health_station = false;
					// don't look for items for a while since the bot
					// could be stuck trying to get to an item
					pBot->f_find_item = gpGlobals->time + 0.5f;
				}
			}
			else if (pBot->b_use_HEV_station)// check if should use wall mounted HEV station...
			{
				if ((pBot->f_use_HEV_time + 10.0) > gpGlobals->time)
				{
					pBot->f_move_speed = 0;  // don't move while using HEV station
					pEdict->v.button = IN_USE;
				}
				else
				{
					// bot is stuck trying to "use" a HEV station...
					pBot->b_use_HEV_station = false;
					// don't look for items for a while since the bot
					// could be stuck trying to get to an item
					pBot->f_find_item = gpGlobals->time + 0.5f;
				}
			}
			else if (pBot->b_use_button)
			{
				pBot->f_move_speed = 0;  // don't move while using elevator
				BotUseLift(pBot, moved_distance);
			}
		//	else
		//  XDM3035a: world movement processing goes below
		}
			if (do_movement_processing)// XDM3035a: this code should be executed when bot is following user too!
			{
				if (pEdict->v.waterlevel == 3)  // check if the bot is underwater...
					BotUnderWater(pBot);

				found_waypoint = false;
				// if the bot is not trying to get to something AND
				// it is time to look for a waypoint AND
				// there are waypoints in this level...
				if (pBot->use_type == BOT_USE_NONE)// XDM3035a: don't!
				{
					if ((pBot->pBotPickupItem == NULL) && (pBot->f_look_for_waypoint_time <= gpGlobals->time) && (num_waypoints != 0))
						found_waypoint = BotHeadTowardWaypoint(pBot);
				}

				// check if the bot is on a ladder...
				if (pEdict->v.movetype == MOVETYPE_FLY)
				{
					// check if bot JUST got on the ladder...
					if ((pBot->f_end_use_ladder_time + 1.0) < gpGlobals->time)
						pBot->f_start_use_ladder_time = gpGlobals->time;
					// go handle the ladder movement
					BotOnLadder(pBot, moved_distance);
					pBot->f_dont_avoid_wall_time = gpGlobals->time + 2.0f;
					pBot->f_end_use_ladder_time = gpGlobals->time;
				}
				else
				{
					// check if the bot JUST got off the ladder...
					if ((pBot->f_end_use_ladder_time + 1.0) > gpGlobals->time)
						pBot->ladder_dir = LADDER_UNKNOWN;
				}

				// if the bot isn't headed toward a waypoint...
				if (found_waypoint == false && pBot->f_move_speed > 0.0f)// XDM3035a: speed > 0
					BotHandleWalls(pBot, moved_distance);

				// check if bot is on a ladder and has been on a ladder for more than 5 seconds...
				if ((pEdict->v.movetype == MOVETYPE_FLY) && (pBot->f_start_use_ladder_time > 0.0) && ((pBot->f_start_use_ladder_time + 5.0) <= gpGlobals->time))
				{
					// bot is stuck on a ladder...
					BotRandomTurn(pBot);
					// don't look for items for 2 seconds
					pBot->f_find_item = gpGlobals->time + 2.0f;
					pBot->f_start_use_ladder_time = 0.0;  // reset start ladder time
				}

				// check if the bot hasn't moved much since the last location
				// (and NOT on a ladder since ladder stuck handled elsewhere)
				if ((moved_distance <= 1.0) && (pBot->f_prev_speed >= 1.0f) && (pEdict->v.movetype != MOVETYPE_FLY))
				{
					// the bot must be stuck!
					pBot->f_dont_avoid_wall_time = gpGlobals->time + 1.0f;
					pBot->f_look_for_waypoint_time = gpGlobals->time + 1.0f;

					if (BotCanJumpUp(pBot, &bCrouchJump))  // can the bot jump onto something?
					{
						if ((pBot->f_jump_time + 2.0) <= gpGlobals->time)
						{
							pBot->f_jump_time = gpGlobals->time;
							pEdict->v.button |= IN_JUMP;  // jump up and move forward

							if (bCrouchJump)
								pEdict->v.button |= IN_DUCK;  // also need to duck
						}
						else// bot already tried jumping less than two seconds ago, just turn
							BotRandomTurn(pBot);
					}
					else if (BotCanDuckUnder(pBot))  // can the bot duck under something?
					{
						pEdict->v.button |= IN_DUCK;  // duck down and move forward
					}
					else
					{
						BotRandomTurn(pBot);
						// is the bot trying to get to an item?...
						if (pBot->pBotPickupItem != NULL)
						{
							// don't look for items for a while since the bot
							// could be stuck trying to get to an item
							pBot->f_find_item = gpGlobals->time + 0.5f;
						}
					}
				}
				// should the bot pause for a while here? (don't pause on ladders or while being "used"...
				/*  XDM if ((RANDOM_LONG(1, 1000) <= pause_frequency[pBot->bot_skill]) && (pEdict->v.movetype != MOVETYPE_FLY) && (pBot->pBotUser == NULL))
				{
				// set the time that the bot will stop "pausing"
				pBot->f_pause_time = gpGlobals->time + RANDOM_FLOAT(pause_time[pBot->bot_skill][0], pause_time[pBot->bot_skill][1]);
				}*/
			}// do_movement_processing
//		}
	}

	if (pBot->curr_waypoint_index != -1)  // does the bot have a waypoint?
	{
		// check if the next waypoint is a door waypoint...
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_DOOR)
			pBot->f_move_speed = pBot->f_max_speed / 3.0f;  // slow down for doors

		// check if the next waypoint is a ladder waypoint...
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_LADDER)
		{
			// check if the waypoint is at the top of a ladder AND
			// the bot isn't currenly on a ladder...
			if ((pBot->waypoint_top_of_ladder) && (pEdict->v.movetype != MOVETYPE_FLY))
			{
				// is the bot on "ground" above the ladder?
				if (pEdict->v.flags & FL_ONGROUND)
				{
					float waypoint_distance = (pEdict->v.origin - pBot->waypoint_origin).Length();

					if (waypoint_distance <= 20.0)  // if VERY close...
						pBot->f_move_speed = 20.0;  // go VERY slow
					else if (waypoint_distance <= 100.0)  // if fairly close...
						pBot->f_move_speed = 50.0;  // go fairly slow

					pBot->ladder_dir = LADDER_DOWN;
				}
				else  // bot must be in mid-air, go BACKWARDS to touch ladder...
					pBot->f_move_speed = -pBot->f_max_speed;
			}
			else
			{
				// don't avoid walls for a while
				pBot->f_dont_avoid_wall_time = gpGlobals->time + 5.0f;
				pBot->waypoint_top_of_ladder = false;
			}
		}

		// check if the next waypoint is a crouch waypoint...
		if (waypoints[pBot->curr_waypoint_index].flags & W_FL_CROUCH)
			pEdict->v.button |= IN_DUCK;  // duck down while moving forward

		// check if the waypoint is a sniper waypoint AND
		// bot isn't currently aiming at an ememy...
		if ((waypoints[pBot->curr_waypoint_index].flags & W_FL_SNIPER) && (pBot->pBotEnemy == NULL))
		{
			// check if the bot need to move back closer to the waypoint...
			float distance = (pEdict->v.origin - waypoints[pBot->curr_waypoint_index].origin).Length();
			if (distance > 40)
			{
				// turn towards the sniper waypoint and move there...
				Vector v_direction = waypoints[pBot->curr_waypoint_index].origin - pEdict->v.origin;
				Vector bot_angles = UTIL_VecToAngles( v_direction );
				pEdict->v.ideal_yaw = bot_angles.y;
				BotFixIdealYaw(pEdict);
				// go slow to prevent the "loop the loop" problem...
				pBot->f_move_speed = pBot->f_max_speed / 3;
				pBot->f_sniper_aim_time = 0.0;  // reset aiming time
				pEdict->v.v_angle.z = 0;  // reset roll to 0 (straight up and down)
				// set the body angles the same way the bot is looking/aiming
				pEdict->v.angles.x = -pEdict->v.v_angle.x / 3;
				pEdict->v.angles.y = pEdict->v.v_angle.y;
				pEdict->v.angles.z = pEdict->v.v_angle.z;
				// save the previous speed (for checking if stuck)
				pBot->f_prev_speed = pBot->f_move_speed;
				f_strafe_speed = 0.0f;

				BotFixIdealPitch(pEdict);
				BotFixIdealYaw(pEdict);
				BotFixBodyAngles(pEdict);
				BotFixViewAngles(pEdict);
				g_engfuncs.pfnRunPlayerMove(pEdict, pEdict->v.v_angle, pBot->f_move_speed, f_strafe_speed, 0, pEdict->v.button, 0, msecval);
				return;
			}
			// check if it's time to adjust aim yet...
			if (pBot->f_sniper_aim_time <= gpGlobals->time)
			{
				int aim_index = WaypointFindNearestAiming(waypoints[pBot->curr_waypoint_index].origin);
				if (aim_index != -1)
				{
					Vector v_aim = waypoints[aim_index].origin - waypoints[pBot->curr_waypoint_index].origin;
					Vector aim_angles = UTIL_VecToAngles( v_aim );
					aim_angles.y += RANDOM_LONG(0, 30) - 15;
					pEdict->v.ideal_yaw = aim_angles.y;
					BotFixIdealYaw(pEdict);
				}
				// don't adjust aim again until after a few seconds...
				pBot->f_sniper_aim_time = gpGlobals->time + RANDOM_FLOAT(3.0, 5.0);
			}
		}
		// check if the waypoint is a sentry gun waypoint and this waypoint
		// is the bot's goal...
/*		if ((waypoints[pBot->curr_waypoint_index].flags & W_FL_SENTRYGUN) && (pBot->waypoint_goal == pBot->curr_waypoint_index))
		pBot->f_move_speed = pBot->f_max_speed / 3;// go slowly when approaching sentry gun waypoints
		// check if the waypoint is a dispenser waypoint and this waypoint
		// is the bot's goal...
		if ((waypoints[pBot->curr_waypoint_index].flags & W_FL_DISPENSER) && (pBot->waypoint_goal == pBot->curr_waypoint_index))
		pBot->f_move_speed = pBot->f_max_speed / 3;// go slowly when approaching dispenser waypoints
*/
	}

	if (pBot->f_pause_time > gpGlobals->time)// is the bot "paused"?
		pBot->f_move_speed = 0;  // don't move while pausing

	// XDM3035a: this code is now awesome! Bots dodge attacks!
	if (pBot->f_strafe_time < gpGlobals->time)// time to strafe yet?
	{
		if (pBot->pBotEnemy)// XDM: strafe only when the enemy is visible!
		{
			Vector vecEnemy = pBot->pBotEnemy->v.origin + pBot->pBotEnemy->v.view_ofs;
			if (FInViewCone(vecEnemy, pBot->pEdict) && pBot->enemy_visible)
			{
				// XDM3035a: enemy is shooting while facing me! Strafe now!
				if (IsFacing(pBot->pBotEnemy->v.origin, pBot->pBotEnemy->v.v_angle, pBot->pEdict->v.origin))
				{
					if (pBot->pBotEnemy->v.button & (IN_ATTACK | IN_ATTACK2))// shooting
					{
						if (pBot->f_strafe_direction == 0.0f)// pick new direction only if we haven't
						{
							if (RANDOM_LONG(0,1) == 0)// TODO: check for wall/ledge and enemy move direction
								pBot->f_strafe_direction = 4.0f;
							else
								pBot->f_strafe_direction = -4.0f;

							//pBot->f_jump_time = gpGlobals->time;
							//pEdict->v.button |= IN_JUMP;  // jump up and move forward
						}
						pBot->f_strafe_time = gpGlobals->time;// check every frame? + 0.1f;
					}
					else// stopped shooting, change direction next time!
					{
						pBot->f_strafe_direction = 0.0f;
						// keep checking
						if (pBot->reaction > 0)// use reaction setting
							pBot->f_strafe_time = gpGlobals->time + 2.0f * RANDOM_FLOAT(react_delay_min[pBot->reaction][pBot->bot_skill], react_delay_max[pBot->reaction][pBot->bot_skill]);
						else
							pBot->f_strafe_time = gpGlobals->time + RANDOM_FLOAT(0.1f, 0.5f);
					}
				}
				else// just use random behaviour
				{
					if (RANDOM_LONG(1, 100) <= pBot->strafe_percent)
					{
						if (RANDOM_LONG(0, 1) == 1)
							pBot->f_strafe_direction = -1.0f * RANDOM_FLOAT(0.5, 1.0);
						else
							pBot->f_strafe_direction = RANDOM_FLOAT(0.5, 1.0);

						pBot->f_strafe_time = gpGlobals->time + RANDOM_FLOAT(0.5f, 1.0f);
					}
					else
					{
						pBot->f_strafe_direction = 0.0f;
						pBot->f_strafe_time = gpGlobals->time + RANDOM_FLOAT(1.0f, 2.0f);
					}
				}
			}
		}
	}

	if (pBot->f_duck_time > gpGlobals->time)
		pEdict->v.button |= IN_DUCK;  // need to duck (crowbar attack)

	if (pBot->f_grenade_search_time <= gpGlobals->time)
	{
		pBot->f_grenade_search_time = gpGlobals->time + 0.1f;
		// does the bot see any grenades laying about?
		if (!(pBot->pEdict->v.flags & FL_GODMODE))
		{
			if (BotLookForGrenades(pBot))
			{
				pBot->f_grenade_found_time = gpGlobals->time;
				pBot->f_move_speed = pBot->f_max_speed;// will be reversed later
			}
		}
	}

	// do we have a grenade primed and ready to throw..
	if (pBot->f_gren_throw_time > gpGlobals->time)
	{
		pBot->f_move_speed = 0.0;  // don't move while priming
	}
	else if (pBot->f_gren_throw_time + 5.0f > gpGlobals->time)
	{
		// move backwards for 5.0 seconds after throwing grenade...
		pBot->f_move_speed = -1.0f * pBot->f_move_speed;
	}
	else if (pBot->f_grenade_found_time + 1.0 > gpGlobals->time)
	{
		// move backwards for 1.0 second after seeing a grenade...
		pBot->f_move_speed = -1.0f * pBot->f_move_speed;
	}

	pEdict->v.v_angle.z = 0;  // reset roll to 0 (straight up and down)
	// set the body angles the same way the bot is looking/aiming
	pEdict->v.angles.x = -pEdict->v.v_angle.x / 3;
	pEdict->v.angles.y = pEdict->v.v_angle.y;
	pEdict->v.angles.z = pEdict->v.v_angle.z;
	// save the previous speed (for checking if stuck)
	pBot->f_prev_speed = pBot->f_move_speed;

	f_strafe_speed = pBot->f_strafe_direction * (pBot->f_move_speed / 2.0f);

	BotFixIdealPitch(pEdict);
	BotFixIdealYaw(pEdict);
	BotFixBodyAngles(pEdict);
	BotFixViewAngles(pEdict);
	g_engfuncs.pfnRunPlayerMove(pEdict, pEdict->v.v_angle, pBot->f_move_speed, f_strafe_speed, 0, pEdict->v.button, 0, msecval);
}

