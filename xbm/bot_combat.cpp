#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "gamedefs.h"// XDM
//#include "weapondef.h"// XDM3035c
#include "bot.h"
#include "bot_cvar.h"
#include "bot_func.h"
#include "bot_weapons.h"
#include "pm_shared.h"// XDM3035c: VEC_HULL_*

// ~X~
// This all code completely sucks. It lacks handling of critical weapon properties and abilities, switching and aiming
// The only way to fix this would be to rewrite everything in object-oriented CBaseEntity code in the server DLL.

bot_weapon_select_t *g_pWeaponSelectTable = NULL;// XDM3035
bot_fire_delay_t *g_pWeaponDelayTable = NULL;


float react_delay_min[BOT_REACTION_LEVELS][BOT_SKILL_LEVELS] = {
	{0.01, 0.02, 0.03, 0.04, 0.05},
	{0.07, 0.09, 0.12, 0.14, 0.17},
	{0.10, 0.12, 0.15, 0.18, 0.21}};
float react_delay_max[BOT_REACTION_LEVELS][BOT_SKILL_LEVELS] = {
	{0.04, 0.06, 0.08, 0.10, 0.12},
	{0.11, 0.14, 0.18, 0.21, 0.25},
	{0.15, 0.18, 0.22, 0.25, 0.30}};

float aim_tracking_x_scale[BOT_SKILL_LEVELS] = {4.5, 4.0, 3.2, 2.5, 2.0};
float aim_tracking_y_scale[BOT_SKILL_LEVELS] = {4.5, 4.0, 3.2, 2.5, 2.0};

// weapons are stored in priority order, most desired weapon should be at
// the start of the array and least desired should be at the end

// XDM3035b: finally got my hands on this mess
bot_weapon_select_t xdm_weapon_select[] =
{
	//	id						class name		skill	1MINdst	1MAXdst	2MINdst	2MAXdst	use%	UWater	pri%	minammo12	hold1	hold2	charge1	charge2	chrgtime1 2
	{WEAPON_TYRIANGUN,		"weapon_tyriangun",	5,		32,	8192,	32,	8192,	100,	true,	80,		0,		0,	false,	false,	false,	false,	0.0,	0.0},
	{WEAPON_NONE,			"",					0,		0,		0,		0,		0,		0,		false,	0,		1,		1,	false,	false,	false,	false,	0.0,	0.0}// terminator
};

bot_fire_delay_t xdm_fire_delay[] =
{
	{WEAPON_TYRIANGUN,
	0.05, {0.0, 0.1, 0.2, 0.4, 0.5}, {0.1, 0.3, 0.4, 0.6, 0.8},
	0.1, {0.2, 0.8, 1.4, 2.0, 2.8}, {0.6, 1.4, 2.0, 3.2, 4.0}},
		
	{WEAPON_NONE,
	0.0, {0.0, 0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0, 0.0},
	0.0, {0.0, 0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0, 0.0}}
};


bot_weapon_select_t valve_weapon_select[] = {
	{VALVE_WEAPON_NONE,			"",						0,	0.0,	0.0,	0.0,	0.0,	0,		true,	0,		1,		1,	false, false, false, false, 0.0, 0.0}/* terminator */
};

bot_fire_delay_t valve_fire_delay[] = {
	/* terminator */
	{0, 0.0, {0.0, 0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0, 0.0},
	0.0, {0.0, 0.0, 0.0, 0.0, 0.0}, {0.0, 0.0, 0.0, 0.0, 0.0}}
};


//-----------------------------------------------------------------------------
// Purpose: Get weapon index in bot_weapon_select_t/bot_fire_delay_t array
// Input  : iId - WEAPON_NONE
// Output : int -1 == failure
//-----------------------------------------------------------------------------
int GetWeaponSelectIndex(const int &iId)
{
	int select_index = 0;// XDM3035: find this item's index in selection priority array
	while (g_pWeaponSelectTable[select_index].iId != WEAPON_NONE)
	{
		if (g_pWeaponSelectTable[select_index].iId == iId)
			return select_index;

		++select_index;
	}
	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: Does this bot have specified weapon?
// Input  : *pBot - 
//			&iId - WEAPON_NONE
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool BotHasWeapon(bot_t *pBot, const int &iId)
{
	if (pBot)
		return (pBot->pEdict->v.weapons & (1<<iId)) != 0;
// HPB40		return (pBot->bot_weapons & (1<<iId)) != 0;

	return false;
}

void BotWeaponInit(void)
{
	if (mod_id == VALVE_DLL)
	{
		g_pWeaponSelectTable = &valve_weapon_select[0];
		g_pWeaponDelayTable = &valve_fire_delay[0];
	}
	else if (mod_id == TYRIAN_DLL)// XDM
	{
		g_pWeaponSelectTable = &xdm_weapon_select[0];
		g_pWeaponDelayTable = &xdm_fire_delay[0];
	}
}

edict_t *BotFindEnemy(bot_t *pBot)
{
	if (g_bot_dont_shoot.value > 0.0f || (g_pmp_noshooting && g_pmp_noshooting->value > 0.0f))// XDM3035c
		return NULL;

	edict_t *pEdict = pBot->pEdict;
	if (pBot->pBotEnemy)// does the bot already have an enemy?
	{
		// if the enemy is dead?
		if (!IsAlive(pBot->pBotEnemy))  // is the enemy dead?, assume bot killed it
		{
			pBot->pBotEnemy = NULL;// don't have an enemy anymore so null out the pointer...
		}
		else  // enemy is still alive
		{
			if (!checked_teamplay)  // check for team play...
				BotCheckTeamplay();

			Vector vecEnd = pBot->pBotEnemy->v.origin + pBot->pBotEnemy->v.view_ofs;

			if (FInViewCone(vecEnd, pEdict) && FVisible(vecEnd, pEdict) && (g_bot_check_lightlevel.value == 0.0f || pBot->pBotEnemy->v.light_level >= 14))
			{
				// if enemy is still visible and in field of view, keep it face the enemy
				Vector v_enemy = pBot->pBotEnemy->v.origin - pEdict->v.origin;
				Vector bot_angles = UTIL_VecToAngles(v_enemy);
				pEdict->v.ideal_yaw = bot_angles.y;
				BotFixIdealYaw(pEdict);
				// keep track of when we last saw an enemy
				pBot->f_bot_see_enemy_time = gpGlobals->time;
				pBot->enemy_visible = 1;
				return pBot->pBotEnemy;
			}
			else  // enemy has gone out of bot's line of sight
				pBot->enemy_visible = 0;
		}
	}

	edict_t *pNewEnemy = NULL;
	float nearestdistance;
//	pBot->enemy_attack_count = 0;  // don't limit number of attacks
	if (pNewEnemy == NULL)
	{
		Vector vecEnd;
		nearestdistance = BOT_SEARCH_MAX_DISTANCE;
		pBot->enemy_visible = 0;

		if (g_iGameType != GT_COOP)
		{
			// search the world for players...
			for (int i = 1; i <= gpGlobals->maxClients; ++i)
			{
				edict_t *pPlayer = INDEXENT(i);
				// skip invalid players and skip self (i.e. this bot)
				if (pPlayer && !pPlayer->free && (pPlayer != pEdict))
				{
					if (pBot->need_to_avenge)// XDM3035c: in revenge-only mode
					{
						if (g_pmp_revengemode && g_pmp_revengemode->value > 1.0f)// XDM3035c: revenge-only mode!
						{
							if (pBot->pBotKiller && pPlayer != pBot->pBotKiller)// ignore everyone else for now
								continue;
						}
					}
					if (!IsAlive(pPlayer))// skip this player if not alive (i.e. dead or dying)
						continue;

					if (pPlayer->v.flags & FL_NOTARGET)
						continue; 					

//					if ((b_observer_mode) && !(pPlayer->v.flags & FL_FAKECLIENT) && !(pPlayer->v.flags & FL_THIRDPARTYBOT))
//						continue;

					if (pBot->pBotUser != NULL)
					{
						if (pPlayer == pBot->pBotUser)
							continue;// XDM: this 'player' is bot's 'user'

						bot_t *pNewEnemyBot = UTIL_GetBotPointer(pPlayer);
						if (pNewEnemyBot && (pNewEnemyBot->pBotUser == pBot->pBotUser))
							continue;// XDM: this 'player' is a bot and following the same 'user'
					}
					vecEnd = pPlayer->v.origin + pPlayer->v.view_ofs;
					// see if bot can't see the player...
					if (!FInViewCone(vecEnd, pEdict) || !FVisible(vecEnd, pEdict))
						continue;

					if (!checked_teamplay)  // check for team play...
						BotCheckTeamplay();

					// is team play enabled?
					if (is_team_play && pEdict->v.team > 0)
					{
						if (pEdict->v.team == pPlayer->v.team)// don't target your teammates...
							continue;
					}

					float distance = (pPlayer->v.origin - pEdict->v.origin).Length();
					if (distance < nearestdistance)
					{
						nearestdistance = distance;
						pNewEnemy = pPlayer;
	// XDM				pBot->pBotUser = NULL;  // don't follow user when enemy found
						pBot->enemy_visible = 1;
					}
				}
			}// for
		}// !COOP

		if (pNewEnemy == NULL)
		{
			edict_t *pent = NULL;
			char ent_name[40];
			nearestdistance = BOT_SEARCH_MAX_DISTANCE;
			// search the world for monsters...
			while ((pent = UTIL_FindEntityInSphere(pent, pEdict->v.origin, 960)) != NULL)
			{
				if (pent->v.effects & EF_NODRAW)// faster checks go first!
					continue;
				if (pent->v.takedamage == DAMAGE_NO || pent->v.health <= 0)
					continue;
				if (!(pent->v.flags & FL_MONSTER))
					continue; // discard anything that is not a monster
				if (!IsAlive(pent))
					continue; // discard dead or dying monsters

				if (pent->v.flags & FL_NOTARGET)
					continue; 

				vecEnd = pent->v.origin + pent->v.view_ofs;
				if (!FVisible(vecEnd, pEdict) || !FInViewCone(vecEnd, pEdict))
					continue;

				// NEW: XDM3035a: server DLL allows advanced and fast checks on entities
				if (mod_id == TYRIAN_DLL && XDM_EntityIs && XDM_EntityRelationship)
				{
					int entitytype = XDM_EntityIs(pent);
					if (entitytype == 4)
						continue;

					if (entitytype == 2 || entitytype == 3)// monsters
					{
						if (pent->v.framerate <= 0)// is this turret active?
							continue;
						if (pent->v.flags & FL_GODMODE)// XDM3035c: avoid invulnerable monsters
						{
							pBot->f_grenade_found_time = gpGlobals->time;
							pBot->f_move_speed = pBot->f_max_speed;// will be reversed later
							continue;
						}

						if (is_team_play)// don't shoot at your own turrets
						{
							if (pEdict->v.team > 0 && pEdict->v.team == pent->v.team)
								continue;
						}

						if (XDM_EntityRelationship(pent, pEdict) > 0)// R_DL, etc.
						{
							float distance = (vecEnd - pEdict->v.origin).Length();
							if (distance < nearestdistance)
							{
								nearestdistance = distance;
								pNewEnemy = pent;
							}
						}
					}
				}
				else// ordinary HL uses older and slower methods
				{
					strcpy(ent_name, STRING(pent->v.classname));
					if (FStrnEq("monster_", ent_name, 8))// check if entity is a monster
					{
						if (FStrEq("monster_furniture", ent_name) ||
							FStrEq("monster_generic", ent_name) ||
							FStrEq("monster_mortar", ent_name) ||
							FStrEq("monster_player", ent_name))
						{
							continue;
						}
						else if (FStrEq("monster_miniturret", ent_name) ||
								FStrEq("monster_sentry", ent_name) ||
								FStrEq("monster_turret", ent_name) ||
								FStrEq("monster_tripmine", ent_name))
						{
							if (pent->v.framerate <= 0)// is this turret active?
								continue;

							if (is_team_play)// don't shoot at your own turrets
							{
								if (pEdict->v.team > 0 && pEdict->v.team == pent->v.team)
									continue;
							}
						}
						else// already if (pent->v.flags & FL_MONSTER)
						{
							float distance = (vecEnd - pEdict->v.origin).Length();
							if (distance < nearestdistance)
							{
								nearestdistance = distance;
								pNewEnemy = pent;
							}
						}
					}// strings
				}// XDM

			}// while
		}// !pNewEnemy
	}
	if (pNewEnemy)
	{
		Vector v_enemy = pNewEnemy->v.origin - pEdict->v.origin;
		Vector bot_angles;
		VEC_TO_ANGLES(v_enemy, bot_angles);
//		Vector bot_angles = UTIL_VecToAngles(v_enemy);// face the enemy
		pEdict->v.ideal_yaw = bot_angles.y;
		BotFixIdealYaw(pEdict);
		// keep track of when we last saw an enemy
		pBot->f_bot_see_enemy_time = gpGlobals->time;
		if (pBot->reaction > 0 && pBot->f_reaction_target_time == 0.0f)// XDM3035c: don't constantly increase this delay!!
			pBot->f_reaction_target_time = gpGlobals->time + RANDOM_FLOAT(react_delay_min[pBot->reaction][pBot->bot_skill], react_delay_max[pBot->reaction][pBot->bot_skill]);
	}
	// has the bot NOT seen an ememy for at least 5 seconds (time to reload)?
	if ((pBot->f_bot_see_enemy_time > 0) && ((pBot->f_bot_see_enemy_time + 5.0f) <= gpGlobals->time))
	{
		pBot->f_bot_see_enemy_time = -1;  // so we won't keep reloading
		pEdict->v.button |= IN_RELOAD;  // press reload button
		// initialize aim tracking angles...
		pBot->f_aim_x_angle_delta = 5.0f;
		pBot->f_aim_y_angle_delta = 5.0f;
	}
	return (pNewEnemy);
}

// specifing a weapon_choice allows you to choose the weapon the bot will use (assuming enough ammo exists for that weapon)
// BotFireWeapon will return true if weapon was fired, false otherwise
bool BotFireWeapon(const Vector &v_enemy, bot_t *pBot, const int &weapon_choice)
{
	if (g_pWeaponSelectTable == NULL)
		return false;

	int select_index = -1;
	int iId;
	edict_t *pEdict = pBot->pEdict;

	// are we charging the primary fire?
	if (pBot->f_primary_charging > 0)
	{
		iId = pBot->current_weapon.iId;//pBot->charging_weapon_id;
		// is it time to fire the charged weapon?
		if (pBot->f_primary_charging <= gpGlobals->time)
		{
			// we DON'T set pEdict->v.button here to release the fire button which will fire the charged weapon
			pBot->f_primary_charging = -1;  // -1 means not charging
			// find the correct fire delay for this weapon
			select_index = GetWeaponSelectIndex(iId);
			if (select_index >= 0)
			{
				// set next time to shoot
				pBot->f_shoot_time = gpGlobals->time + g_pWeaponDelayTable[select_index].primary_base_delay +
					RANDOM_FLOAT(g_pWeaponDelayTable[select_index].primary_min_delay[pBot->bot_skill],
								g_pWeaponDelayTable[select_index].primary_max_delay[pBot->bot_skill]);
				return true;
			}
		}
		else
		{
			pEdict->v.button |= IN_ATTACK;// charge the weapon
			pBot->f_shoot_time = gpGlobals->time;// keep charging
			return true;
		}
	}
	else if (pBot->b_primary_holding)// no! (g_pWeaponSelectTable[pBot->current_weapon.weapon_select_table_index].primary_fire_hold)
	{
		if (pBot->m_rgAmmo[weapon_defs[pBot->current_weapon.iId].iAmmo1] >= g_pWeaponSelectTable[pBot->current_weapon.weapon_select_table_index].min_primary_ammo)
		{
			pEdict->v.button |= IN_ATTACK;// charge the weapon
			pBot->f_shoot_time = gpGlobals->time;// keep charging
			return true;		
		}
	}

	// are we charging the secondary fire?
	if (pBot->f_secondary_charging > 0)
	{
		iId = pBot->current_weapon.iId;//pBot->charging_weapon_id;
		// is it time to fire the charged weapon?
		if (pBot->f_secondary_charging <= gpGlobals->time)
		{
			// we DON'T set pEdict->v.button here to release the fire button which will fire the charged weapon
			pBot->f_secondary_charging = -1;// -1 means not charging
			// find the correct fire delay for this weapon
			select_index = GetWeaponSelectIndex(iId);
			if (select_index >= 0)
			{
				// set next time to shoot
				pBot->f_shoot_time = gpGlobals->time + g_pWeaponDelayTable[select_index].secondary_base_delay +
					RANDOM_FLOAT(g_pWeaponDelayTable[select_index].secondary_min_delay[pBot->bot_skill],
								g_pWeaponDelayTable[select_index].secondary_max_delay[pBot->bot_skill]);
				return true;
			}
		}
		else
		{
			pEdict->v.button |= IN_ATTACK2;  // charge the weapon
			pBot->f_shoot_time = gpGlobals->time;  // keep charging
			return true;
		}
	}
	else if (pBot->b_secondary_holding)
	{
		if (pBot->m_rgAmmo[weapon_defs[pBot->current_weapon.iId].iAmmo2] >= g_pWeaponSelectTable[pBot->current_weapon.weapon_select_table_index].min_secondary_ammo)
		{
			pEdict->v.button |= IN_ATTACK2;
			pBot->f_shoot_time = gpGlobals->time;
			return true;		
		}
	}

	int use_percent;
	int primary_percent;
	float distance = v_enemy.Length();  // how far away is the enemy?
	bool use_primary, use_secondary;
	bool primary_in_range, secondary_in_range;
	select_index = 0;
	// loop through all the weapons until terminator is found...
	while (g_pWeaponSelectTable[select_index].iId)
	{
		// was a weapon choice specified? (and if so do they NOT match?)
		if ((weapon_choice != 0) && (weapon_choice != g_pWeaponSelectTable[select_index].iId))
		{
			select_index++;  // skip to next weapon
			continue;
		}
		// is the bot NOT carrying this weapon?
		if (!BotHasWeapon(pBot, g_pWeaponSelectTable[select_index].iId))
		{
			select_index++;  // skip to next weapon
			continue;
		}
		// is the bot NOT skilled enough to use this weapon?
		if ((pBot->bot_skill+1) > g_pWeaponSelectTable[select_index].skill_level)
		{
			select_index++;  // skip to next weapon
			continue;
		}
		// is the bot underwater and does this weapon NOT work under water?
		if ((pEdict->v.waterlevel == 3) && !(g_pWeaponSelectTable[select_index].can_use_underwater))
		{
			select_index++;  // skip to next weapon
			continue;
		}
		use_percent = RANDOM_LONG(1, 100);
		// is use percent greater than weapon use percent?
		if (use_percent > g_pWeaponSelectTable[select_index].use_percent)
		{
			select_index++;  // skip to next weapon
			continue;
		}

		iId = g_pWeaponSelectTable[select_index].iId;
//		weapon_index = iId;
		use_primary = false;
		use_secondary = false;
		primary_percent = RANDOM_LONG(1, 100);

		primary_in_range = (distance >= g_pWeaponSelectTable[select_index].primary_min_distance) && (distance <= g_pWeaponSelectTable[select_index].primary_max_distance);
		secondary_in_range = (distance >= g_pWeaponSelectTable[select_index].secondary_min_distance) && (distance <= g_pWeaponSelectTable[select_index].secondary_max_distance);

		if (weapon_choice != 0)
		{
			primary_in_range = true;
			secondary_in_range = true;
		}

		// is primary percent less than weapon primary percent AND
		// no ammo required for this weapon OR
		// enough ammo available to fire AND
		// the bot is far enough away to use primary fire AND
		// the bot is close enough to the enemy to use primary fire
		if ((primary_percent <= g_pWeaponSelectTable[select_index].primary_fire_percent) &&
			((weapon_defs[iId].iAmmo1 == -1) ||
			(pBot->m_rgAmmo[weapon_defs[iId].iAmmo1] >= g_pWeaponSelectTable[select_index].min_primary_ammo)) &&
			(primary_in_range))
		{
			use_primary = true;
		}// otherwise see if there is enough secondary ammo AND the bot is far enough away to use secondary fire AND the bot is close enough to the enemy to use secondary fire
		else if (((weapon_defs[iId].iAmmo2 == -1) ||
			(pBot->m_rgAmmo[weapon_defs[iId].iAmmo2] >= g_pWeaponSelectTable[select_index].min_secondary_ammo)) &&
			(secondary_in_range))
		{
			use_secondary = true;
		}

		// see if there wasn't enough ammo to fire the weapon...
		if ((use_primary == false) && (use_secondary == false))
		{
			select_index++;  // skip to next weapon
			continue;
		}

		// select this weapon if it isn't already selected
		if (pBot->current_weapon.iId != iId)
		{
			if (mod_id == TYRIAN_DLL)// XDM3035: new fast protocol
				UTIL_SelectItem(pEdict, g_pWeaponSelectTable[select_index].iId);
			else
				UTIL_SelectItem(pEdict, g_pWeaponSelectTable[select_index].weapon_name);
		}

		if (g_pWeaponDelayTable[select_index].iId != iId)
		{
//				char msg[80];
//				sprintf(msg, "XBM: fire_delay mismatch for weapon id=%d\n",iId);
			ALERT(at_console, "XBM: fire_delay mismatch for weapon id=%d\n", iId);
			return false;
		}

		if ((use_primary && g_pWeaponSelectTable[select_index].primary_max_distance < 50) ||
			(use_secondary && g_pWeaponSelectTable[select_index].secondary_max_distance < 50))
		{
			// check if bot needs to duck down to hit enemy...
			if (pBot->pBotEnemy->v.origin.z < (pEdict->v.origin.z - 30))
				pBot->f_duck_time = gpGlobals->time + 1.0f;
		}

		pBot->b_primary_holding = false;
		pBot->b_secondary_holding = false;

		if (use_primary)// ATTACK!
		{
			pEdict->v.button |= IN_ATTACK;  // primary attack
			if (g_pWeaponSelectTable[select_index].primary_fire_charge)
			{
				pBot->charging_weapon_id = iId;
				// release primary fire after the appropriate delay...
				pBot->f_primary_charging = gpGlobals->time + g_pWeaponSelectTable[select_index].primary_charge_delay;
				pBot->f_shoot_time = gpGlobals->time;  // keep charging
			}
			else if (g_pWeaponSelectTable[select_index].primary_fire_hold)
			{
//				ALERT(at_console, "XBM: BotFireWeapon() id=%d primary_fire_hold\n", iId);
				pBot->b_primary_holding = true;
				pBot->f_shoot_time = gpGlobals->time;  // don't let button up
			}
			else// set next time to shoot
			{
				pBot->f_shoot_time = gpGlobals->time + g_pWeaponDelayTable[select_index].primary_base_delay +
					RANDOM_FLOAT(g_pWeaponDelayTable[select_index].primary_min_delay[pBot->bot_skill], g_pWeaponDelayTable[select_index].primary_max_delay[pBot->bot_skill]);
			}
		}
		else  // MUST be use_secondary...
		{
			pEdict->v.button |= IN_ATTACK2;  // secondary attack
			if (g_pWeaponSelectTable[select_index].secondary_fire_charge)
			{
				pBot->charging_weapon_id = iId;
				// release secondary fire after the appropriate delay...
				pBot->f_secondary_charging = gpGlobals->time + g_pWeaponSelectTable[select_index].secondary_charge_delay;
				pBot->f_shoot_time = gpGlobals->time;  // keep charging
			}
			else if (g_pWeaponSelectTable[select_index].secondary_fire_hold)
			{
//				ALERT(at_console, "XBM: BotFireWeapon() id=%d secondary_fire_hold\n", iId);
				pBot->b_secondary_holding = true;
				pBot->f_shoot_time = gpGlobals->time;  // don't let button up
			}
			else// set next time to shoot
			{
				pBot->f_shoot_time = gpGlobals->time + g_pWeaponDelayTable[select_index].secondary_base_delay +
					RANDOM_FLOAT(g_pWeaponDelayTable[select_index].secondary_min_delay[pBot->bot_skill], g_pWeaponDelayTable[select_index].secondary_max_delay[pBot->bot_skill]);
			}
		}
		return true;  // weapon was fired
	}
	return false;// didn't have any available weapons or ammo, return false
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pBot - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool BotShootAtEnemy(bot_t *pBot)
{
	if (pBot->pBotEnemy == NULL)
		return false;
	if (pBot->current_weapon.iId == WEAPON_NONE)
	{
/*#ifdef _DEBUG
		ALERT(at_aiconsole, "XBM: BotShootAtEnemy() bot %s has no weapon selected!\n", pBot->name);
#endif*/
		return false;
	}

	float f_distance;
	Vector v_enemy;
	edict_t *pEdict = pBot->pEdict;
/*
	// do we need to aim at the feet?
	if (pBot->current_weapon.iId == WEAPON_RPG)
	{
		TraceResult tr;
		Vector v_src = pEdict->v.origin + pEdict->v.view_ofs;  // bot's eyes
		Vector v_dest = pBot->pBotEnemy->v.origin - pBot->pBotEnemy->v.view_ofs;
		UTIL_TraceLine(v_src, v_dest, dont_ignore_monsters, pEdict->v.pContainingEntity, &tr);

		// can the bot see the enemies feet?
		if ((tr.flFraction >= 1.0f) || ((tr.flFraction >= 0.95f) && (tr.pHit->v.flags & (FL_CLIENT|FL_FAKECLIENT))))// XDM3035c (strcmp("player", STRING(tr.pHit->v.classname)) == 0)))
			v_enemy = (pBot->pBotEnemy->v.origin - pBot->pBotEnemy->v.view_ofs) - GetGunPosition(pEdict);// aim at the feet for RPG type weapons
		else
			v_enemy = (pBot->pBotEnemy->v.origin + pBot->pBotEnemy->v.view_ofs) - GetGunPosition(pEdict);
	}
	else// aim for the head...
*/
	v_enemy = (pBot->pBotEnemy->v.origin + Vector(0,0,12)/*+ pBot->pBotEnemy->v.view_ofs*/) - GetGunPosition(pEdict); //Tyrian bot shoot in center!!

//	Vector enemy_angle = UTIL_VecToAngles(v_enemy);
	Vector enemy_angle;
	VEC_TO_ANGLES(v_enemy, enemy_angle);// XDM3037: faster

	if (enemy_angle.x > 180)
		enemy_angle.x -=360;

	if (enemy_angle.y > 180)
		enemy_angle.y -=360;

	// adjust the view angle pitch to aim correctly
	enemy_angle.x = -enemy_angle.x;

	float d_x = (enemy_angle.x - pEdict->v.v_angle.x);
	if (d_x > 180.0f)
		d_x = 360.0f - d_x;
	if (d_x < -180.0f)
		d_x = 360.0f + d_x;

	float d_y = (enemy_angle.y - pEdict->v.v_angle.y);
	if (d_y > 180.0f)
		d_y = 360.0f - d_y;
	if (d_y < -180.0f)
		d_y = 360.0f + d_y;

	float delta_dist_x = fabs(d_x / pBot->f_frame_time);
	float delta_dist_y = fabs(d_y / pBot->f_frame_time);

	if ((delta_dist_x > 100.0f) && (RANDOM_LONG(1, 100) < 40))
		pBot->f_aim_x_angle_delta += aim_tracking_x_scale[pBot->bot_skill] * pBot->f_frame_time * 0.8f;
	else
		pBot->f_aim_x_angle_delta -= aim_tracking_x_scale[pBot->bot_skill] * pBot->f_frame_time;

	if (RANDOM_LONG(1, 100) < ((pBot->bot_skill+1) * 10))
		pBot->f_aim_x_angle_delta += aim_tracking_x_scale[pBot->bot_skill] * pBot->f_frame_time * 0.5f;

	if ((delta_dist_y > 100.0) && (RANDOM_LONG(1, 100) < 40))
		pBot->f_aim_y_angle_delta += aim_tracking_y_scale[pBot->bot_skill] * pBot->f_frame_time * 0.8f;
	else
		pBot->f_aim_y_angle_delta -= aim_tracking_y_scale[pBot->bot_skill] * pBot->f_frame_time;

	if (RANDOM_LONG(1, 100) < ((pBot->bot_skill+1) * 10))
		pBot->f_aim_y_angle_delta += aim_tracking_y_scale[pBot->bot_skill] * pBot->f_frame_time * 0.5f;

	if (pBot->f_aim_x_angle_delta > 5.0f)
		pBot->f_aim_x_angle_delta = 5.0f;
	else if (pBot->f_aim_x_angle_delta < 0.01f)
		pBot->f_aim_x_angle_delta = 0.01f;

	if (pBot->f_aim_y_angle_delta > 5.0f)
		pBot->f_aim_y_angle_delta = 5.0f;
	else if (pBot->f_aim_y_angle_delta < 0.01f)
		pBot->f_aim_y_angle_delta = 0.01f;

	if (d_x < 0.0)
		d_x = d_x - pBot->f_aim_x_angle_delta;
	else
		d_x = d_x + pBot->f_aim_x_angle_delta;

	if (d_y < 0.0)
		d_y = d_y - pBot->f_aim_y_angle_delta;
	else
		d_y = d_y + pBot->f_aim_y_angle_delta;

	pEdict->v.idealpitch = pEdict->v.v_angle.x + d_x;
	BotFixIdealPitch(pEdict);

	pEdict->v.ideal_yaw = pEdict->v.v_angle.y + d_y;
	BotFixIdealYaw(pEdict);

	v_enemy.z = 0;  // ignore z component (up & down)
	f_distance = v_enemy.Length();  // how far away is the enemy scum?

/* wtf is this all for?
	if (pBot->f_gren_check_time <= gpGlobals->time)
	{
		pBot->f_gren_check_time = gpGlobals->time + pBot->grenade_time;
		bool teammate = 0;
		
		if (!checked_teamplay)  // check for team play...
			BotCheckTeamplay();
		
		// is team play enabled?
		if (is_team_play)
		{
			// check if "enemy" is a teammate...
			if (pEdict->v.team >= TEAM_NONE && pEdict->v.team == pBot->pBotEnemy->v.team)// || (team_allies[bot_team] & (1<<player_team)))
				teammate = true;
			else
				teammate = false;
		}
	}
*/

#ifdef _DEBUG
	if (pEdict->v.button & IN_ATTACK)
		ALERT(at_aiconsole, "XBM: BotShootAtEnemy(): IN_ATTACK\n");
	if (pEdict->v.button & IN_ATTACK2)
		ALERT(at_aiconsole, "XBM: BotShootAtEnemy(): IN_ATTACK2\n");
#endif

	// is it time to shoot yet?
	if (pBot->f_shoot_time <= gpGlobals->time)
	{
		float maxd = max(g_pWeaponSelectTable[pBot->current_weapon.weapon_select_table_index].primary_max_distance,
						g_pWeaponSelectTable[pBot->current_weapon.weapon_select_table_index].secondary_max_distance);

		float mind = min(g_pWeaponSelectTable[pBot->current_weapon.weapon_select_table_index].primary_min_distance,
						g_pWeaponSelectTable[pBot->current_weapon.weapon_select_table_index].secondary_min_distance);

//does not work!	if (pBot->use_type != BOT_USE_HOLD)// XDM: don't run away from our spot
		{
			if (f_distance > maxd)// run if distance to enemy is far
			{
				pBot->f_move_speed = pBot->f_max_speed;
			}
			else// can fire
			{
				if (f_distance > (mind + HULL_RADIUS))// walk if distance is closer
					pBot->f_move_speed = pBot->f_max_speed * 0.5f;
				else
					pBot->f_move_speed = pBot->f_max_speed * 0.1f;// don't move if very close

				// select the best weapon to use at this distance and fire...
				if (!BotFireWeapon(v_enemy, pBot, 0))
				{
					pBot->pBotEnemy = NULL;
					pBot->f_bot_find_enemy_time = gpGlobals->time + 3.0f;
					pBot->f_move_speed = pBot->f_max_speed;
				}
			}
		}
	}
	else
		pBot->f_move_speed = pBot->f_max_speed;

	return true;
}

bool BotShootTripmine(bot_t *pBot)
{
	edict_t *pEdict = pBot->pEdict;

	if (!pBot->b_shoot_tripmine)
		return false;

	// aim at the tripmine and fire the glock...
	Vector v_enemy = pBot->v_tripmine - GetGunPosition(pEdict);
	pEdict->v.v_angle = UTIL_VecToAngles(v_enemy);
	if (pEdict->v.v_angle.y > 180)
		pEdict->v.v_angle.y -=360;

	// Paulo-La-Frite - START bot aiming bug fix
	if (pEdict->v.v_angle.x > 180)
		pEdict->v.v_angle.x -=360;

	// set the body angles to point the gun correctly
	pEdict->v.angles.x = pEdict->v.v_angle.x / 3.0f;
	pEdict->v.angles.y = pEdict->v.v_angle.y;
	pEdict->v.angles.z = 0.0f;
	// adjust the view angle pitch to aim correctly (MUST be after body v.angles stuff)
	pEdict->v.v_angle.x = -pEdict->v.v_angle.x;
	// Paulo-La-Frite - END
	pEdict->v.ideal_yaw = pEdict->v.v_angle.y;
	BotFixIdealYaw(pEdict);
	return (BotFireWeapon(v_enemy, pBot, WEAPON_TYRIANGUN));
}
