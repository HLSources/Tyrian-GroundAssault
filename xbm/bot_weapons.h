#ifndef BOT_WEAPONS_H
#define BOT_WEAPONS_H

#include "weapondef.h"
#include "protocol.h"

#define VALVE_WEAPON_NONE			WEAPON_NONE
#define VALVE_WEAPON_CROWBAR		WEAPON_CROWBAR
#define VALVE_WEAPON_GLOCK			WEAPON_GLOCK
#define VALVE_WEAPON_PYTHON			WEAPON_PYTHON
#define VALVE_WEAPON_MP5			WEAPON_MP5
#define VALVE_WEAPON_CROSSBOW		WEAPON_CROSSBOW
#define VALVE_WEAPON_SHOTGUN		WEAPON_SHOTGUN
#define VALVE_WEAPON_RPG			WEAPON_RPG
#define VALVE_WEAPON_GAUSS			WEAPON_GAUSS
#define VALVE_WEAPON_EGON			WEAPON_EGON
#define VALVE_WEAPON_HORNETGUN		WEAPON_HORNETGUN
#define VALVE_WEAPON_HANDGRENADE	WEAPON_HANDGRENADE
#define VALVE_WEAPON_TRIPMINE		WEAPON_TRIPMINE
#define VALVE_WEAPON_SATCHEL		WEAPON_SATCHEL
#define VALVE_WEAPON_SNARK			WEAPON_SNARK

typedef struct
{
	char szClassname[64];
	int  iAmmo1;	// ammo index for primary ammo
	int  iAmmo1Max;	// max primary ammo
	int  iAmmo2;	// ammo index for secondary ammo
	int  iAmmo2Max;	// max secondary ammo
	int  iSlot;		// HUD slot (0 based)
	int  iPosition;	// slot position
	int  iId;		// weapon ID
	int  iFlags;	// flags???
} bot_weapon_t;

extern bot_weapon_t weapon_defs[MAX_WEAPONS];

#endif // BOT_WEAPONS_H
