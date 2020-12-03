#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "bot.h"
#include "bot_func.h"
#include "bot_client.h"
#include "bot_weapons.h"

int BotUseCommand(bot_t *pBot, edict_t *pPlayer, int command)
{
	if (pBot == NULL || pPlayer == NULL)
		return 0;

	if (command >= BOT_USE_LASTTYPE)
		command = BOT_USE_NONE;

	int result = 0;
	char msg[BOT_NAME_LEN + 40];// should be enough
	if (pBot)
	{
		// check if user is a teammate...
		if (is_team_play == 0 || pBot->pEdict->v.team >= 0 && pBot->pEdict->v.team == pPlayer->v.team)
		{
			if (pBot->pBotUser == NULL || pBot->pBotUser == pPlayer)
			{
				switch(command)
				{
				default:
				case BOT_USE_NONE:
					pBot->use_type = BOT_USE_NONE;
					pBot->pBotUser = NULL;
					sprintf(msg, "%s is now free\n", pBot->name);
					break;
				case BOT_USE_FOLLOW:
					pBot->use_type = BOT_USE_FOLLOW;
					pBot->pBotUser = pPlayer;
					if (pBot->pBotEnemy == pPlayer)
						pBot->pBotEnemy = NULL;

					sprintf(msg, "%s will follow you\n", pBot->name);
					break;
				case BOT_USE_HOLD:
					pBot->use_type = BOT_USE_HOLD;
					pBot->pBotUser = pPlayer;
					pBot->v_hold_origin = pPlayer->v.origin;
					pBot->f_hold_ideal_yaw = pPlayer->v.angles.y;//.ideal_yaw;
					if (pBot->pBotEnemy == pPlayer)
						pBot->pBotEnemy = NULL;

					sprintf(msg, "%s will hold position\n", pBot->name);
					break;
				}
				result = 1;
			}
			else
				sprintf(msg, "%s is used by another player\n", pBot->name);
		}
		else
			sprintf(msg, "%s is not a teammate\n", pBot->name);

		ClientPrint(pPlayer, HUD_PRINTTALK, msg);
	}
	return result;
}

/*void BotUseHoldPosition(bot_t *pBot, edict_t *pPlayer)
{
	char msg[BOT_NAME_LEN + 40];
	if(pBot)
	{
		if(pBot->pBotUser == pPlayer)
		{
			pBot->pBotUser = NULL;
			pBot->v_hold_position = NULL;
			sprintf(msg, "%s stopped holding position\n", pBot->name);
		}
		else if(!pBot->pBotUser)
		{
			pBot->pBotUser = pPlayer;
			if(pBot->pBotEnemy == pPlayer)
				pBot->pBotEnemy = NULL;
			sprintf(msg, "%s will hold your position\n", pBot->name);
		}
		else
			sprintf(msg, "%s is holding another position\n", pBot->name);
	}
	else
		sprintf(msg, "ERROR 404: Bot not found\n");

	ClientPrint(pPlayer, HUD_PRINTTALK, msg);
}*/
