#ifndef CDLL_DLL_H
#define CDLL_DLL_H

#define MAX_PLAYERS			32// XDM3036: must be same as MAX_CLIENTS (or replace)

#define MAX_WEAPON_SLOTS	8
#define MAX_ITEMS			32

#define MAX_AMMO_SLOTS		32

#define DEFAULT_FOV			90

#define	HIDEHUD_WEAPONS		(1<<0)
#define	HIDEHUD_FLASHLIGHT	(1<<1)
#define	HIDEHUD_ALL			(1<<2)
#define HIDEHUD_HEALTH		(1<<3)

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

#define MAX_NORMAL_BATTERY	200
#define MAX_INVULNER_BATTERY	5000
#define MAX_PLAYER_HEALTH	200
#define MIN_PLASMA_SHIELD_AMOUNT 50

#define EXTRA_BATTERY_SHIELD_AUG	50
#define EXTRA_HEALTH_HP_AUG			50

#define MAX_MAPNAME				32// XDM3035c: that's the lowest value found in the valve's code
#define MAX_PLAYER_NAME_LENGTH	32
#define	MAX_MOTD_LENGTH			1536

#define MENU_DEFAULT				1
#define MENU_TEAM 					2
#define MENU_CLASS 					3
#define MENU_MAPBRIEFING			4
#define MENU_INTRO 					5
#define MENU_CLASSHELP				6
#define MENU_CLASSHELP2 			7
#define MENU_REPEATHELP 			8
#define MENU_SPECHELP				9
#define MENU_10						10
#define MENU_11						11
#define MENU_SPY					12
#define MENU_SPY_SKIN				13
#define MENU_SPY_COLOR				14
#define MENU_ENGINEER				15
#define MENU_ENGINEER_FIX_DISPENSER	16
#define MENU_ENGINEER_FIX_SENTRYGUN	17
#define MENU_ENGINEER_FIX_MORTAR	18
#define MENU_DISPENSER				19
#define MENU_CLASS_CHANGE			20
#define MENU_TEAM_CHANGE			21
#define MENU_INVENTORY				22
#define MENU_MAPINFO				23
#define MENU_CONSOLE				24
#define MENU_REFRESH_RATE 			25
#define MENU_ENTITYCREATE			26

#define MENU_VOICETWEAK				50

#endif // CDLL_DLL_H
