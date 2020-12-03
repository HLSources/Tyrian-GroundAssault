#ifndef BOT_H
#define BOT_H

#include "studio.h"

// stuff for Win32 vs. Linux builds

#ifndef __linux__

#define WIN32_LEAN_AND_MEAN
#define NOWINRES
#define NOSERVICE
#define NOMCX
#define NOIME

#undef MAX_PATH
#include "windows.h"

typedef int (FAR *GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef int (FAR *GETENTITYAPI2)(DLL_FUNCTIONS *, int *);
typedef int (FAR *GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
typedef void (FAR *GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
typedef int (FAR *SERVER_GETBLENDINGINTERFACE)(int, struct sv_blending_interface_s **, struct engine_studio_api_s *, float (*)[3][4], float (*)[MAXSTUDIOBONES][3][4]);
typedef void (FAR *LINK_ENTITY_FUNC)(entvars_t *);
typedef void (FAR *LINK_ENTITY_FUNC)(entvars_t *);
typedef int (FAR *XDM_ENTITY_RELATIONSHIP_FUNC)(edict_t *, edict_t*);// XDM3035a
typedef int (FAR *XDM_ENTITY_IS_FUNC)(edict_t *);// XDM3035a
typedef int (FAR *XDM_CAN_HAVE_ITEM_FUNC)(edict_t *entity, edict_t *item);// XDM3035c

#else

#include <dlfcn.h>
#define GetProcAddress dlsym
#define Sleep sleep

//typedef int BOOL;

typedef int (*GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef int (*GETENTITYAPI2)(DLL_FUNCTIONS *, int *);
typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
typedef void (*GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
typedef int (*SERVER_GETBLENDINGINTERFACE)(int, struct sv_blending_interface_s **, struct engine_studio_api_s *, float (*)[3][4], float (*)[MAXSTUDIOBONES][3][4]);
typedef void (*LINK_ENTITY_FUNC)(entvars_t *);
typedef int (*XDM_ENTITY_RELATIONSHIP_FUNC)(edict_t *, edict_t*);// XDM3035a
typedef int (*XDM_ENTITY_IS_FUNC)(edict_t *);// XDM3035a
typedef int (*XDM_CAN_HAVE_ITEM_FUNC)(edict_t *entity, edict_t *item);// XDM3035c

#endif

// define constants used to identify the MOD we are playing...
#define VALVE_DLL				1
#define TYRIAN_DLL					2// XBM

#define LADDER_UNKNOWN			0
#define LADDER_UP				1
#define LADDER_DOWN				2

#define WANDER_LEFT				1
#define WANDER_RIGHT			2

#define RESPAWN_IDLE			1
#define RESPAWN_NEED_TO_RESPAWN	2
#define RESPAWN_IS_RESPAWNING	3

#define BOT_MODEL_LEN			32
#define BOT_NAME_LEN			32

#define BOT_SKILL_LEVELS		5
#define BOT_REACTION_LEVELS		3

#define MAX_BOT_CHAT			128

#define BOT_FOLLOW_DISTANCE		80.0f

#define PLAYER_SEARCH_RADIUS	40.0
#define MAX_BOT_LOGOS			100

#define BOT_SEARCH_MAX_DISTANCE	8192

#define BOT_CHAT_RECENT_ITEMS	8
#define BOT_CHAT_STRING_LEN		128
#define BOT_CHAT_TRY_INTERVAL	10// in XBM attempts are now made properly

enum
{
	BOT_USE_NONE = 0,
	BOT_USE_FOLLOW,
	BOT_USE_HOLD,
	BOT_USE_LASTTYPE
};

typedef struct
{
	bool can_modify;
	char text[BOT_CHAT_STRING_LEN];
} bot_chat_t;

typedef struct
{
	int iId;						// the weapon ID value
	char weapon_name[64];			// name of the weapon when selecting it
	int skill_level;				// bot skill must be less than or equal to this value
	float primary_min_distance;		// 0 = no minimum
	float primary_max_distance;		// 9999 = no maximum
	float secondary_min_distance;	// 0 = no minimum
	float secondary_max_distance;	// 9999 = no maximum
	int use_percent;				// times out of 100 to use this weapon when available
	bool can_use_underwater;		// can use this weapon underwater
	int primary_fire_percent;		// times out of 100 to use primary fire
	int min_primary_ammo;			// minimum ammout of primary ammo needed to fire
	int min_secondary_ammo;			// minimum ammout of seconday ammo needed to fire
	bool primary_fire_hold;			// hold down primary fire button to use?
	bool secondary_fire_hold;		// hold down secondary fire button to use?
	bool primary_fire_charge;		// charge weapon using primary fire?
	bool secondary_fire_charge;		// charge weapon using secondary fire?
	float primary_charge_delay;		// time to charge weapon
	float secondary_charge_delay;	// time to charge weapon
} bot_weapon_select_t;

typedef struct
{
	int iId;
	float primary_base_delay;
	float primary_min_delay[BOT_SKILL_LEVELS];
	float primary_max_delay[BOT_SKILL_LEVELS];
	float secondary_base_delay;
	float secondary_min_delay[BOT_SKILL_LEVELS];
	float secondary_max_delay[BOT_SKILL_LEVELS];
} bot_fire_delay_t;

typedef struct
{
	int iId;	// weapon ID
	int iClip;	// amount of ammo in the clip
	int iAmmo1;// amount of ammo in primary reserve
	int iAmmo2;// amount of ammo in secondary reserve
	int weapon_select_table_index;// XDM3035
} bot_current_weapon_t;


typedef struct bot_s
{
	bool is_used;
	byte respawn_state;
	edict_t *pEdict;
	bool need_to_initialize;
	char name[BOT_NAME_LEN+1];
	char model[BOT_MODEL_LEN+1];
	short model_skin;
	short bot_skill;
	bool not_started;
	float f_kick_time;
	float f_create_time;
	float f_frame_time;
	byte chat_percent;
	byte taunt_percent;
	byte whine_percent;
	byte chat_tag_percent;
	byte chat_drop_percent;
	byte chat_swap_percent;
	byte chat_lower_percent;
	float idle_angle;
	float idle_angle_time;
	float blinded_time;
	float f_max_speed;// from real bot, not template
	float f_prev_speed;
	float f_speed_check_time;
	Vector v_prev_origin;
	float f_find_item;
	edict_t *pBotPickupItem;

	byte ladder_dir;
	float f_start_use_ladder_time;
	float f_end_use_ladder_time;
	bool waypoint_top_of_ladder;

	float f_wall_check_time;
	float f_wall_on_right;
	float f_wall_on_left;
	float f_dont_avoid_wall_time;
	float f_look_for_waypoint_time;
	float f_jump_time;
	float f_drop_check_time;
	byte wander_dir;

	byte strafe_percent;
	float f_strafe_direction;  // 0 = none, negative = left, positive = right
	float f_strafe_time;

	float f_exit_water_time;

	Vector waypoint_origin;
	float f_waypoint_time;
	int curr_waypoint_index;
	int prev_waypoint_index[5];
	float f_random_waypoint_time;
	int waypoint_goal;
	float f_waypoint_goal_time;
	bool waypoint_near_flag;
	Vector waypoint_flag_origin;
	float prev_waypoint_distance;
	int weapon_points[6];  // five weapon locations + 1 null

//	float f_aim_tracking_time;
	float f_aim_x_angle_delta;
	float f_aim_y_angle_delta;

	edict_t *pBotEnemy;
	byte enemy_visible;
	float f_bot_see_enemy_time;
	float f_bot_find_enemy_time;
	Vector v_enemy_previous_origin;
//	int enemy_attack_count;

	float f_bot_spawn_time;
	edict_t *pBotKiller;

	edict_t *pBotUser;// XDM: a player who commands this bot
	byte use_type;// XDM: follow, hold position, etc.
	Vector v_hold_origin;// XDM: hold this position
	float f_hold_ideal_yaw;
	float f_bot_use_time;
//	bool b_follow_look;
//	float f_follow_look_time;

	bool b_bot_say;
	float f_bot_say;
	char bot_say_msg[256];
	float f_bot_chat_time;
	float f_duck_time;
	float f_sniper_aim_time;
	float f_shoot_time;
	float f_primary_charging;
	float f_secondary_charging;
	bool b_primary_holding;
	bool b_secondary_holding;
	int charging_weapon_id;
	int grenade_time;  // min time between grenade throws
	float f_gren_throw_time;
//	float f_gren_check_time;
	float f_grenade_search_time;
	float f_grenade_found_time;
//	float f_medic_check_time;
//	float f_medic_pause_time;
//	float f_medic_yell_time;
	float f_move_speed;
	float f_pause_time;
	float f_sound_update_time;
	bool bot_has_flag;
	bool b_see_tripmine;
	bool b_shoot_tripmine;
	Vector v_tripmine;
	bool b_use_health_station;
	float f_use_health_time;
	bool b_use_HEV_station;
	float f_use_HEV_time;
	bool b_use_button;
	float f_use_button_time;
	bool b_lift_moving;

	byte logo_percent;
	bool b_spray_logo;
	float f_spray_logo_time;
	char logo_name[16];

	byte top_color;
	byte bottom_color;

	int reaction;
	float f_reaction_target_time;  // time when enemy targeting starts

	bot_current_weapon_t current_weapon;  // one current weapon for each bot
	int m_rgAmmo[MAX_AMMO_SLOTS];  // total ammo amounts (1 array for each bot)
	bool need_to_avenge;// XDM3035c
} bot_t;

/*
#define MAX_TEAMS 32
#define MAX_FLAGS  5

typedef struct
{
	edict_t *edict;
	int  team_no;
} FLAG_S;
*/

typedef struct
{
	char identification[4];		// should be WAD2 (or 2DAW) or WAD3 (or 3DAW)
	int numlumps;
	int infotableofs;
} wadinfo_t;

typedef struct
{
	int filepos;
	int disksize;
	int size;					// uncompressed
	char type;
	char compression;
	char pad1, pad2;
	char name[16];				// must be null terminated
} lumpinfo_t;




// define some function prototypes...
BOOL ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
void ClientPutInServer(edict_t *pEntity);
void ClientCommand(edict_t *pEntity);
void UpdateClientData(const struct edict_s *ent, int sendweapons, struct clientdata_s *cd);
void FakeClientCommand(edict_t *pBot, char *arg1, char *arg2, char *arg3);

const char *Cmd_Args(void);
const char *Cmd_Argv(int argc);
int Cmd_Argc(void);

inline edict_t *CREATE_FAKE_CLIENT(const char *netname)
{
	return (*g_engfuncs.pfnCreateFakeClient)(netname);
}
inline char *GET_INFOKEYBUFFER(edict_t *e)
{
	return (*g_engfuncs.pfnGetInfoKeyBuffer)(e);
}
inline char *GET_INFO_KEYVALUE(char *infobuffer, char *key)
{
	return (g_engfuncs.pfnInfoKeyValue(infobuffer, key));
}
inline void SET_CLIENT_KEYVALUE(int clientIndex, char *infobuffer, char *key, char *value)
{
	(*g_engfuncs.pfnSetClientKeyValue)(clientIndex, infobuffer, key, value);
}


// new UTIL.CPP functions...
edict_t *UTIL_FindEntityInSphere(edict_t *pentStart, const Vector &vecCenter, float flRadius);
edict_t *UTIL_FindEntityByString(edict_t *pentStart, const char *szKeyword, const char *szValue);
edict_t *UTIL_FindEntityByClassname(edict_t *pentStart, const char *szName);
edict_t *UTIL_FindEntityByTargetname(edict_t *pentStart, const char *szName);
void ClientPrint(edict_t *pEdict, int msg_dest, const char *msg_name);
//void UTIL_SayText(const char *pText, edict_t *pEdict);
void UTIL_HostSay(edict_t *pEntity, int teamonly, char *message);
//int UTIL_GetTeam(edict_t *pEntity);
//int UTIL_GetClass(edict_t *pEntity);
int UTIL_CountBots(void);
int UTIL_GetBotIndex(edict_t *pEdict);
bot_t *UTIL_GetBotPointer(edict_t *pEdict);
bool IsAlive(edict_t *pEdict);
bool FInViewCone(const Vector &origin, edict_t *pEdict);
bool FVisible(const Vector &vecOrigin, edict_t *pEdict);
Vector Center(edict_t *pEdict);
Vector GetGunPosition(edict_t *pEdict);
void UTIL_SelectItem(edict_t *pEdict, char *item_name);
void UTIL_SelectItem(edict_t *pEdict, const int &iID);
void UTIL_SelectWeapon(edict_t *pEdict, const int &weapon_index);
bool UpdateSounds(edict_t *pEdict, edict_t *pPlayer);
void UTIL_ShowMenu(edict_t *pEdict, int slots, int displaytime, bool needmore, char *pText);
void UTIL_BuildFileName(char *filename, const char *arg1, const char *arg2);
//void UTIL_Pathname_Convert(char *pathname);
void GetGameDir(char *game_dir);
edict_t *UTIL_ClientByIndex(const int &playerIndex);// XDM3035
void GetEntityPrintableName(edict_t *pEntity, char *output, size_t max_len);



extern int mod_id;// XBM
extern int debug_engine;
extern bool g_waypoint_on;
extern bool g_auto_waypoint;
extern bool g_path_waypoint;
extern bool g_path_waypoint_enable;
extern bot_t bots[];
extern bool g_observer_mode;
extern bool g_intermission;
extern int g_iGameType;
extern int g_iGameMode;
extern int g_iSkillLevel;

extern bool is_team_play;
extern bool checked_teamplay;

extern int g_iModelIndexAnimglow01;
extern int g_iModelIndexBeamsAll;

// TheFatal's method for calculating the msecval
extern int msecnum;
extern float msecdel;
extern float msecval;

extern float react_delay_min[BOT_REACTION_LEVELS][BOT_SKILL_LEVELS];
extern float react_delay_max[BOT_REACTION_LEVELS][BOT_SKILL_LEVELS];

extern XDM_ENTITY_RELATIONSHIP_FUNC XDM_EntityRelationship;
extern XDM_ENTITY_IS_FUNC XDM_EntityIs;
extern XDM_CAN_HAVE_ITEM_FUNC XDM_CanHaveItem;

#endif // BOT_H
