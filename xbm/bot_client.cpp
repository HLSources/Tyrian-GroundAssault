#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "bot.h"
#include "bot_func.h"
#include "bot_client.h"
#include "bot_cvar.h"
#include "bot_weapons.h"

// types of damage to ignore...
#define IGNORE_DAMAGE (DMG_CRUSH | DMG_BURN | DMG_FREEZE | \
						DMG_SHOCK | DMG_DROWN | DMG_NERVEGAS | DMG_RADIATION | \
						DMG_DROWNRECOVER | DMG_ACID | DMG_SLOWBURN | \
						DMG_SLOWFREEZE | 0xFF000000)

int g_iGameType = 0;
int g_iGameMode = 0;
int g_iSkillLevel = 0;

extern int num_logos;
extern int bot_taunt_count;
extern int recent_bot_taunt[];
extern bot_chat_t bot_taunt[];
extern bot_weapon_select_t *g_pWeaponSelectTable;// XDM3035

bot_weapon_t weapon_defs[MAX_WEAPONS]; // array of weapon definitions

// This message is sent when a client joins the game.  All of the weapons
// are sent with the weapon ID and information about what ammo is used.
// Called many times for each WRITE_BYTE()/etc.
void BotClient_Valve_WeaponList(void *p, int bot_index)
{
	static int state = 0;// current state machine state
	static bot_weapon_t bot_weapon;
	
	if (state == 0)
	{
		state++;
		strcpy(bot_weapon.szClassname, (char *)p);
	}
	else if (state == 1)
	{
		state++;
		bot_weapon.iAmmo1 = *(int *)p;  // ammo index 1
	}
	else if (state == 2)
	{
		state++;
		bot_weapon.iAmmo1Max = *(int *)p;  // max ammo1
	}
	else if (state == 3)
	{
		state++;
		bot_weapon.iAmmo2 = *(int *)p;  // ammo index 2
	}
	else if (state == 4)
	{
		state++;
		bot_weapon.iAmmo2Max = *(int *)p;  // max ammo2
	}
	else if (state == 5)
	{
		state++;
		bot_weapon.iSlot = *(int *)p;  // slot for this weapon
	}
	else if (state == 6)
	{
		state++;
		bot_weapon.iPosition = *(int *)p;  // position in slot
	}
	else if (state == 7)
	{
		state++;
		bot_weapon.iId = *(int *)p;  // weapon ID
	}
	else if (state == 8)
	{
		state = 0;
		bot_weapon.iFlags = *(int *)p;  // flags for weapon (WTF???)
		// store away this weapon with it's ammo information...
		weapon_defs[bot_weapon.iId] = bot_weapon;
	}
}

// This message is sent when a weapon is selected (either by the bot chosing
// a weapon or by the server auto assigning the bot a weapon).
void BotClient_Valve_CurrentWeapon(void *p, int bot_index)
{
	static int state = 0;// current state machine state
	static int iState;
	static int iId;
	static int iClip;

	if (state == 0)
	{
		state++;
		iState = *(int *)p;  // state of the current weapon
	}
	else if (state == 1)
	{
		state++;
		iId = *(int *)p;  // weapon ID of current weapon
	}
	else if (state == 2)
	{
		state = 0;
		iClip = *(int *)p;  // ammo currently in the clip for this weapon
		if (iId < MAX_WEAPONS)
		{
			bot_t *pBot = &bots[bot_index];
//			if (pBot && g_pWeaponSelectTable)
//				pBot->bot_weapons |= (1<<iId);  // set this weapon bit
			if (iState == 1)
			{
				pBot->current_weapon.iId = iId;
				pBot->current_weapon.iClip = iClip;
				// update the ammo counts for this weapon...
				pBot->current_weapon.iAmmo1 = pBot->m_rgAmmo[weapon_defs[iId].iAmmo1];
				pBot->current_weapon.iAmmo2 = pBot->m_rgAmmo[weapon_defs[iId].iAmmo2];
				pBot->current_weapon.weapon_select_table_index = GetWeaponSelectIndex(iId);
			}
		}
	}
}

// This message is sent whenever ammo ammounts are adjusted (up or down).
void BotClient_Valve_AmmoX(void *p, int bot_index)
{
	static int state = 0;// current state machine state
	static int index;
	static int ammount;
	int ammo_index;
	
	if (state == 0)
	{
		state++;
		index = *(int *)p;  // ammo index (for type of ammo)
	}
	else if (state == 1)
	{
		bot_t *pBot = &bots[bot_index];
		state = 0;
		ammount = *(int *)p;  // the ammount of ammo currently available
		pBot->m_rgAmmo[index] = ammount;  // store it away
		ammo_index = pBot->current_weapon.iId;
		
		// update the ammo counts for this weapon...
		pBot->current_weapon.iAmmo1 = pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
		pBot->current_weapon.iAmmo2 = pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
	}
}

// This message is sent when the bot picks up some ammo (AmmoX messages are
// also sent so this message is probably not really necessary except it
// allows the HUD to draw pictures of ammo that have been picked up.  The
// bots don't really need pictures since they don't have any eyes anyway.
void BotClient_Valve_AmmoPickup(void *p, int bot_index)
{
	static int state = 0;// current state machine state
	static int index;
	static int ammount;
	int ammo_index;
	
	if (state == 0)
	{
		state++;
		index = *(int *)p;
	}
	else if (state == 1)
	{
		bot_t *pBot = &bots[bot_index];
		state = 0;
		ammount = *(int *)p;
		pBot->m_rgAmmo[index] = ammount;
		ammo_index = pBot->current_weapon.iId;
		// update the ammo counts for this weapon...
		pBot->current_weapon.iAmmo1 = pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
		pBot->current_weapon.iAmmo2 = pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
	}
}

// This message gets sent when the bot picks up a weapon.
void BotClient_Valve_WeaponPickup(void *p, int bot_index)
{
//	int index = *(int *)p;// HPB40
	// set this weapon bit to indicate that we are carrying this weapon
//	bots[bot_index].bot_weapons |= (1<<index);// HPB40
}

// This message gets sent when the bot picks up an item (like a battery or a healthkit)
void BotClient_Valve_ItemPickup(void *p, int bot_index)
{
//	if (mod_id == TYRIAN_DLL)// XDM3035a: first byte indicates quantity
}

// This message gets sent when the bots health changes.
void BotClient_Valve_Health(void *p, int bot_index)
{
//	bots[bot_index].bot_health = *(int *)p;// HPB40
}

// This message gets sent when the bots armor changes.
void BotClient_Valve_Battery(void *p, int bot_index)
{
//	bots[bot_index].bot_armor = *(int *)p;// HPB40
}

// This message gets sent when the bots are getting damaged.
void BotClient_Valve_Damage(void *p, int bot_index)
{
	static int state = 0;// current state machine state
	static int damage_armor;
	static int damage_taken;
	static int damage_bits;  // type of damage being done
	static Vector damage_origin;
	bot_t *pBot = &bots[bot_index];

	if (state == 0)
	{
		state++;
		damage_armor = *(int *)p;
	}
	else if (state == 1)
	{
		state++;
		damage_taken = *(int *)p;
	}
	else if (state == 2)
	{
		state++;
		damage_bits = *(int *)p;
	}
	else if (state == 3)
	{
		state++;
		damage_origin.x = *(float *)p;
	}
	else if (state == 4)
	{
		state++;
		damage_origin.y = *(float *)p;
	}
	else if (state == 5)
	{
		state = 0;
		
		damage_origin.z = *(float *)p;
		
		if ((damage_armor > 0) || (damage_taken > 0))
		{
			if (damage_bits & DMG_DROWN)// XDM3035: drowning, swim up!
			{
				//BotUnderWater(pBot);
				if (pBot->pEdict->v.waterlevel == 3)
				{
					pBot->pEdict->v.idealpitch = -90;
					// swim up towards the surface
					pBot->pEdict->v.v_angle.x = -80;// look upwards
					// move forward (i.e. in the direction the bot is looking, up or down)
					pBot->pEdict->v.button |= (IN_FORWARD | IN_JUMP);
					return;
				}
			}

			// ignore certain types of damage...
			if (damage_bits & IGNORE_DAMAGE)
				return;
			
			// if the bot doesn't have an enemy and someone is shooting at it then
			// turn in the attacker's direction...
			if (pBot->pBotEnemy == NULL)
			{
				// face the attacker...
				Vector v_enemy = damage_origin - bots[bot_index].pEdict->v.origin;
				Vector bot_angles = UTIL_VecToAngles(v_enemy);
				pBot->pEdict->v.ideal_yaw = bot_angles.y;
				BotFixIdealYaw(pBot->pEdict);
				// stop using health or HEV stations...
				pBot->b_use_health_station = FALSE;
				pBot->b_use_HEV_station = FALSE;
			}
		}
	}
}

void BotClient_Valve_DeathMsg(void *p, int bot_index)// This message gets sent when the bots get killed
{
	static int state = 0;// current state machine state
	static int killer_index;
	static int victim_index;
	static edict_t *killer_edict;
	static edict_t *victim_edict;
	static short victimbotindex;// index in bot array
	static short killerbotindex;

	if (state == 0)
	{
		state++;
		killer_index = *(int *)p;  // ENTINDEX() of killer
	}
	else if (state == 1)
	{
		state++;
		victim_index = *(int *)p;  // ENTINDEX() of victim
	}
	else if (state == 2)
	{
		state = 0;
		killer_edict = INDEXENT(killer_index);
		victim_edict = INDEXENT(victim_index);
		// get the bot index of the killer...
		killerbotindex = UTIL_GetBotIndex(killer_edict);
		// is this message about a bot killing someone?
		if (killerbotindex != -1)
		{
			if (killer_index != victim_index)  // didn't kill self...
			{
				if ((RANDOM_LONG(1, 100) <= bots[killerbotindex].logo_percent) && (num_logos))
				{
					bots[killerbotindex].b_spray_logo = TRUE;  // this bot should spray logo now
					bots[killerbotindex].f_spray_logo_time = gpGlobals->time;
				}
			}

			if (victim_edict != NULL)
			{
				if (bots[killerbotindex].pBotKiller == victim_edict)// I killed my killer
					bots[killerbotindex].need_to_avenge = false;

				if ((g_bot_chat_enable.value > 0.0f) && (RANDOM_LONG(1,100) <= bots[killerbotindex].taunt_percent))
					BotSpeakTaunt(&bots[killerbotindex], victim_edict);
			}
		}
		// get the bot index of the victim...
		victimbotindex = UTIL_GetBotIndex(victim_edict);
		// is this message about a bot being killed?
		if (victimbotindex != -1)
		{
			if ((killer_index == 0) || (killer_index == victim_index))
			{
				bots[victimbotindex].pBotKiller = NULL;// bot killed by world (worldspawn) or bot killed self...
				bots[victimbotindex].need_to_avenge = false;
			}
			else
			{
				bots[victimbotindex].pBotKiller = killer_edict;//INDEXENT(killer_index);// store edict of player that killed this bot...
				bots[victimbotindex].need_to_avenge = true;
			}
		}
	}
}

void BotClient_Valve_ScreenFade(void *p, int bot_index)
{
	static int state = 0;// current state machine state
	static int duration;
	static int hold_time;
	static int fade_flags;
	//  int length;
	
	if (state == 0)
	{
		state++;
		duration = *(int *)p;
	}
	else if (state == 1)
	{
		state++;
		hold_time = *(int *)p;
	}
	else if (state == 2)
	{
		state++;
		fade_flags = *(int *)p;
	}
	else if (state == 6)
	{
		state = 0;
		
		float length = (float)(duration + hold_time) / 4096.0f;
		bots[bot_index].blinded_time = gpGlobals->time + length - 2.0f;
	}
	else
		state++;
}

void BotClient_CS_HLTV(void *p, int bot_index)// HPB40
{
	static int state = 0;// current state machine state
	static int players;
	int index;

	if (state == 0)
		players = *(int *) p;
	else if (state == 1)
	{
		// new round in CS 1.6
		if ((players == 0) && (*(int *) p == 0))
		{
			for (index = 0; index < 32; ++index)
			{
				if (bots[index].is_used)
					BotSpawnInit(&bots[index]); // reset bots for new round
			}
		}
	}
}

static int g_iGameModeArgument = 0;

/*
		WRITE_BYTE(g_pGameRules->GetGameType());
		WRITE_BYTE(m_iGameMode);
		WRITE_BYTE(g_iSkillLevel);// XDM3035a
		WRITE_BYTE(gameflags);// XDM3035a is this a good place for these? (can be changed by the server on-the-fly)
		WRITE_BYTE((int)mp_revengemode.value);
		WRITE_SHORT((int)mp_fraglimit.value);
		WRITE_SHORT(g_pGameRules->GetScoreLimit());// XDM3035a: was (int)mp_scorelimit.value
		WRITE_BYTE(GetRoundsLimit());
		WRITE_BYTE(GetRoundsPlayed());
*/
void BotClient_XDM_GameMode(void *p, int bot_index)
{
	if (g_iGameModeArgument == 0)
		g_iGameType = *(byte *)p;
	else if (g_iGameModeArgument == 1)
		g_iGameMode = *(byte *)p;
	else if (g_iGameModeArgument == 2)
		g_iSkillLevel = *(byte *)p;

	++g_iGameModeArgument;
}

// XDM3037: sizeless, reliable method
void BotClient_XDM_GameModeEnd(void *p, int bot_index)
{
	g_iGameModeArgument = 0;
}

// XDM3035b
void BotClient_XDM_WeaponList(void *p, int bot_index)
{
	static int state = 0;// current state machine state
	static int startafterskip = 0;
	static bot_weapon_t bot_weapon;
	
	if (state == 0)
	{
		state++;
		strcpy(bot_weapon.szClassname, (char *)p);
		bot_weapon.iSlot = 0;
		bot_weapon.iPosition = 0;
	}
	else if (state == 1)
	{
		state++;
		bot_weapon.iAmmo1 = *(int *)p;  // ammo index 1
	}
	else if (state == 2)
	{
		state++;
		bot_weapon.iAmmo1Max = *(int *)p;  // max ammo1
	}
	else if (state == 3)
	{
		state++;
		bot_weapon.iAmmo2 = *(int *)p;  // ammo index 2
	}
	else if (state == 4)
	{
		state++;
		bot_weapon.iAmmo2Max = *(int *)p;  // max ammo2
		startafterskip = state;
	}
#ifdef SERVER_WEAPON_SLOTS
	else if (state == 5)
	{
		state++;
		bot_weapon.iSlot = *(int *)p;  // slot for this weapon
	}
	else if (state == 6)
	{
		state++;
		bot_weapon.iPosition = *(int *)p;  // position in slot
		startafterskip = state;
	}
#endif
	else if (state == startafterskip)// 7 or 5
	{
		state++;
		bot_weapon.iId = *(int *)p;  // weapon ID
	}
	else if (state == startafterskip+1)// 8 or 6
	{
		state = 0;
		startafterskip = 0;
		bot_weapon.iFlags = *(int *)p;  // flags for weapon (WTF???)
		// store away this weapon with it's ammo information...
		weapon_defs[bot_weapon.iId] = bot_weapon;
	}
}

// XDM3035 multiple updates combined into one packet
static int g_iByteIndexUpdWeapons = 0;

void BotClient_XDM_UpdWeapons(void *p, int bot_index)
{
	BotClient_Valve_CurrentWeapon(p, bot_index);
	++g_iByteIndexUpdWeapons;
}

void BotClient_XDM_UpdWeaponsEnd(void *p, int bot_index)
{
	g_iByteIndexUpdWeapons = 0;
}

// XDM3035 multiple updates combined into one packet
static int g_iByteIndexUpdAmmo = 0;

void BotClient_XDM_UpdAmmo(void *p, int bot_index)
{
/*	static BYTE aindex;
	static BYTE aamount;
	int ammo_index;

	if (g_iByteIndexUpdAmmo & 1)// every second byte
	{
		aamount = *(BYTE *)p;  // the ammount of ammo currently available

		bot_t *pBot = &bots[bot_index];
		pBot->m_rgAmmo[aindex] = aamount;  // store it away
		ammo_index = pBot->current_weapon.iId;
		// update the ammo counts for this weapon...
		pBot->current_weapon.iAmmo1 = pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo1];
		pBot->current_weapon.iAmmo2 = pBot->m_rgAmmo[weapon_defs[ammo_index].iAmmo2];
	}
	else// we get here first!
	{
		aindex = *(BYTE *)p;  // ammo index (for type of ammo)
	}*/
	BotClient_Valve_AmmoX(p, bot_index);
	++g_iByteIndexUpdAmmo;
}

void BotClient_XDM_UpdAmmoEnd(void *p, int bot_index)
{
	g_iByteIndexUpdAmmo = 0;
}

/*
void BotClient_XDM_TeamNames(void *p, int bot_index)
{
}
*/
