//====================================================================
//
// Purpose: client message handlers
//
//====================================================================

#ifndef BOT_CLIENT_H
#define BOT_CLIENT_H
#ifdef _WIN32
#pragma once
#endif


void BotClient_Valve_WeaponList(void *p, int bot_index);
void BotClient_Valve_CurrentWeapon(void *p, int bot_index);
void BotClient_Valve_AmmoX(void *p, int bot_index);
void BotClient_Valve_AmmoPickup(void *p, int bot_index);
void BotClient_Valve_WeaponPickup(void *p, int bot_index);
void BotClient_Valve_ItemPickup(void *p, int bot_index);
void BotClient_Valve_Health(void *p, int bot_index);
void BotClient_Valve_Battery(void *p, int bot_index);
void BotClient_Valve_Damage(void *p, int bot_index);
void BotClient_Valve_DeathMsg(void *p, int bot_index);
void BotClient_Valve_ScreenFade(void *p, int bot_index);
void BotClient_CS_HLTV(void *p, int bot_index);

void BotClient_XDM_GameMode(void *p, int bot_index);
void BotClient_XDM_GameModeEnd(void *p, int bot_index);// XDM3037: sizeless
void BotClient_XDM_WeaponList(void *p, int bot_index);// XDM3035b
void BotClient_XDM_UpdWeapons(void *p, int bot_index);// XDM3035
void BotClient_XDM_UpdWeaponsEnd(void *p, int bot_index);// XDM3035
void BotClient_XDM_UpdAmmo(void *p, int bot_index);
void BotClient_XDM_UpdAmmoEnd(void *p, int bot_index);
//void BotClient_XDM_TeamNames(void *p, int bot_index);// XDM3035c

#endif // BOT_CLIENT_H
