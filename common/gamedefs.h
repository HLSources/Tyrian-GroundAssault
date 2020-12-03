//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2011
//-----------------------------------------------------------------------------
#ifndef GAMEDEFS_H
#define GAMEDEFS_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* !__MINGW32__ */
#endif

//-----------------------------------------------------------------------------
// Common types/constants
//-----------------------------------------------------------------------------

// Game types IMPORTANT: keep this in order shown below!
enum
{
	GT_SINGLE = 0,// auto-select in multiplayer
	GT_COOP,// multiplayer game types
	GT_DEATHMATCH,// deathmatch game types start from here
	GT_LMS,
	GT_TEAMPLAY,// only teamplay game types below
	GT_CTF,// - game types that use extra score below
	GT_DOMINATION,
	GT_ROUND,// round-based game types are derived from this
	GT_ASSAULT,
};

#define GAMETITLE_DEFAULT_STRING	"Tyrian: Ground Assault"
#define GAMETITLE_STRING_NAME		"GAMETITLE"

// Longest time the intermission can last, in seconds
#define MAX_INTERMISSION_TIME		120

// Respawn delay constants
#define ITEM_RESPAWN_TIME			45
#define WEAPON_RESPAWN_TIME			45
#define AMMO_RESPAWN_TIME			30

#define WEAPONBOX_DESTROY_TIME		60

// Number of items added by a server cvar
#define MAX_ADD_DEFAULT_ITEMS		32

enum
{
	EV_ITEMSPAWN_ITEM = 0,
	EV_ITEMSPAWN_WEAPON,
	EV_ITEMSPAWN_AMMO,
	EV_ITEMSPAWN_OTHER
};

// What may/should be dropped by players
enum
{
	GR_PLR_DROP_GUN_NO = 0,
	GR_PLR_DROP_GUN_ACTIVE,
	GR_PLR_DROP_GUN_ALL,
};

enum
{
	GR_PLR_DROP_AMMO_NO = 0,
	GR_PLR_DROP_AMMO_ACTIVE,
	GR_PLR_DROP_AMMO_ALL,
};

// Player relationship return codes
enum
{
	GR_NOTTEAMMATE = 0,
	GR_TEAMMATE,
	GR_ENEMY,
	GR_ALLY,
	GR_NEUTRAL,
};

// Game events
enum
{
	GAME_EVENT_UNKNOWN = 0,
	GAME_EVENT_AWARD,
	GAME_EVENT_COMBO,
	GAME_EVENT_COMBO_BREAKER,
	GAME_EVENT_FIRST_SCORE,
	GAME_EVENT_TAKES_LEAD,
	GAME_EVENT_REVENGE,
	GAME_EVENT_LOOSECOMBO,
	GAME_EVENT_ROUND_START,
	GAME_EVENT_ROUND_END,
	GAME_EVENT_COOP_PLAYER_FINISH,
	GAME_EVENT_PLAYER_READY,
//?	GAME_EVENT_ENDGAME
};

// GameMode message
#define GAME_FLAG_NOSHOOTING		1
#define GAME_FLAG_ALLOW_CAMERA		2
#define GAME_FLAG_ALLOW_SPECTATORS	4
#define GAME_FLAG_4					8
#define GAME_FLAG_5					16
#define GAME_FLAG_6					32
#define GAME_FLAG_8					128

// Tournament effects
#define SCORE_AWARD_TIME	5.0
#define SCORE_AWARD_COMBO	5// wins per combo
#define SCORE_AWARD_MAX		13// maximum fast-score award (n-kill)
#define SCORE_COMBO_MAX		5// max combos

// Kill flags
// These hold additional information to be sent to a client
// Clients cannot retrieve these details by themselves
#define KFLAG_TEAMKILL	1
#define KFLAG_MONSTER	2

//-----------------------------------------------------------------------------
// Teamplay
//-----------------------------------------------------------------------------

// NOTE
// In teamplay spectators and unassigned players have team == TEAM_NONE.
// But on client side we cannot rely on this value to determine if a player is spectating
// because in other game types ALL players may have team set to TEAM_NONE.
enum
{
	TEAM_NONE = 0,// unassigned
	TEAM_1,
	TEAM_2,
	TEAM_3,
	TEAM_4,
};// TEAMS;

// XDM3035: use TEAM_ID instead of just 'int' to make sure nobody use 0 for team 1
typedef int TEAM_ID;// Not an enum typedef, made on purpose

#define MAX_TEAMS				4// does not include virtual team for spectators or not connected players
#define MAX_TEAMNAME_LENGTH		16
#define MAX_TEAM_NAME			MAX_TEAMNAME_LENGTH// for compatibility

// Don't use less than 255 because pfnDrawSetTextColor doesn't recognize it
const unsigned char g_iTeamColors[MAX_TEAMS + 1][3] =
{
	{255,	255,	255},	// Default
	{0,		255,	0},		// Green
	{0,		0,		255},	// Blue
	{255,	0,		0},		// Red
	{255,	255,	0},	// XDM3035 Yellow
//	{0,		255,	255},	// Cyan
};// see TeamColormap() in server DLL

//-----------------------------------------------------------------------------
// Capture the flag
//-----------------------------------------------------------------------------

// CTF events
enum
{
	CTF_EV_INIT = 0,
	CTF_EV_DROP,
	CTF_EV_RETURN,
	CTF_EV_CAPTURED,
	CTF_EV_TAKEN,
};

#define MAX_CTF_TEAMS				2

#define CTF_LIGHT_RADIUS_DEFAULT	128
#define CTF_LIGHT_RADIUS_CARRIED	80

#define CTF_OBJ_TARGETNAME1			"ctf_cobj1"
#define CTF_OBJ_TARGETNAME2			"ctf_cobj2"

enum CAPTUREOBJECT_STATE
{
	CO_STAY = 0,
	CO_CARRIED,
	CO_DROPPED,
	CO_CAPTURED,
	CO_INIT,// client update
	CO_RESET,
};

enum flaganim_e
{
	FANIM_ON_GROUND = 0,// world
	FANIM_NOT_CARRIED,
	FANIM_CARRIED,// player
	FANIM_CARRIED_IDLE,
	FANIM_POSITION
};

enum captureobject_usetypes
{
	COU_NONE = 0,//USE_OFF,
	COU_CAPTURE,// = USE_ON,
	COU_TAKEN,// = USE_SET,
	COU_DROP,// = USE_TOGGLE,
	COU_RETURN,// = USE_KILL,
};


//-----------------------------------------------------------------------------
// Domination
//-----------------------------------------------------------------------------
#define DOM_LIGHT_RADIUS	128
#define DOM_MAX_POINTS		8


//-----------------------------------------------------------------------------
// Cooperative
//-----------------------------------------------------------------------------
enum
{
	COOP_MODE_SWEEP = 0,
	COOP_MODE_MONSTERFRAGS,
	COOP_MODE_LEVEL
};


#endif // GAMEDEFS_H
