#ifndef BOT_FUNC_H
#define BOT_FUNC_H

void BOT_CREATE(char *model, const char *name, int skill,
				int topcolor, int bottomcolor, int skin, int reaction,
				int strafe_prc, int chat_prc, int taunt_prc, int whine_prc, /*const*/ char *team);

void BotSpawnInit(bot_t *pBot);
int BotInFieldOfView(bot_t *pBot, const Vector &dest);
bool BotEntityIsVisible(bot_t *pBot, const Vector &dest);
void BotPickLogo(bot_t *pBot);
void BotTrySprayLogo(bot_t *pBot);
void BotSprayLogo(edict_t *pEntity, const char *logo_name);
void BotFindItem(bot_t *pBot);
//bool BotLookForMedic(bot_t *pBot);

bool BotLookForGrenades(bot_t *pBot);
void BotThink(bot_t *pBot);

void BotFixIdealPitch(edict_t *pEdict);
float BotChangePitch(bot_t *pBot, float speed);// no const & because this function needs a copy of speed anyway
void BotFixIdealYaw(edict_t *pEdict);
float BotChangeYaw(bot_t *pBot, float speed);
void BotFixBodyAngles(edict_t *pEdict);
void BotFixViewAngles(edict_t *pEdict);
bool BotFindWaypoint(bot_t *pBot);
bool BotHeadTowardWaypoint(bot_t *pBot);
void BotOnLadder(bot_t *pBot, const float &moved_distance);
void BotUnderWater(bot_t *pBot);
void BotUseLift(bot_t *pBot, const float &moved_distance);
bool BotStuckInCorner(bot_t *pBot);
void BotTurnAtWall(bot_t *pBot, TraceResult *tr, bool negative);
bool BotCantMoveForward(bot_t *pBot, TraceResult *tr);
bool BotCanJumpUp(bot_t *pBot, bool *bDuckJump);
bool BotCanDuckUnder(bot_t *pBot);
void BotRandomTurn(bot_t *pBot);
bool BotFollowUser(bot_t *pBot);
void BotHandleWalls(bot_t *pBot, float &moved_distance);
bool BotCheckWallOnLeft(bot_t *pBot);
bool BotCheckWallOnRight(bot_t *pBot);
bool BotLookForDrop(bot_t *pBot);

int GetWeaponSelectIndex(const int &iId);
bool BotHasWeapon(bot_t *pBot, const int &iId);
void BotWeaponInit(void);
void BotCheckTeamplay(void);
edict_t *BotFindEnemy(bot_t *pBot);
Vector BotBodyTarget(edict_t *pBotEnemy, bot_t *pBot);
bool BotFireWeapon(const Vector &v_enemy, bot_t *pBot, const int &weapon_choice);
bool BotShootAtEnemy(bot_t *pBot);
bool BotShootTripmine(bot_t *pBot);

int BotUseCommand(bot_t *pBot, edict_t *pPlayer, int command);
bool BotHoldPosition(bot_t *pBot);


void BotLogoInit(void);
void ProcessBotCfgFile(void);
void LoadBotChat(void);
void BotTrimBlanks(const char *in_string, char *out_string, const size_t max_len);
bool BotChatTrimTag(const char *original_name, char *out_name, const size_t max_len);
void BotChatName(const char *original_name, char *out_name, const size_t max_len);
void BotChatText(const char *in_text, char *out_text, const size_t max_len);
void BotChatFillInName(bot_t *pBot, char *bot_say_msg, const char *chat_text, const char *chat_name);
void BotSpeakDirect(bot_t *pBot, const char *pText, float delay);
void BotSpeakChat(bot_t *pBot);
void BotSpeakTaunt(bot_t *pBot, edict_t *pVictim);
void BotSpeakWhine(bot_t *pBot, edict_t *pKiller);

#endif // BOT_FUNC_H
