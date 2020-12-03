//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#ifndef GAMERULES_H
#define GAMERULES_H
#ifdef _WIN32
#pragma once
#endif

#include "gamedefs.h"// XDM

class CBasePlayerItem;
class CBaseMonster;
class CBasePlayer;
class CItem;
class CBasePlayerAmmo;

// XDM3036:
/*class CBaseGameGoalEntity// : public CBaseEntity
{
public:
	virtual BOOL IsGameGoal(void) { return TRUE; };// TODO: mygamerules == currentgamerules
};*/



class CGameRules
{
public:
	CGameRules();
	virtual ~CGameRules();

	virtual short GetGameType(void);// { return m_iGameType; };// XDM3035x
	virtual short GetGameMode(void) { return m_iGameMode; };// XDM3036
	virtual void Initialize(void);// XDM: need this because g_pGameRules cannot be used from constructor
	virtual void RefreshSkillData(void);// fill skill data struct with proper values
	virtual void StartFrame(void) {};// GR_Think - runs every server frame, should handle any timer tasks, periodic events, etc.

	virtual bool IsAllowedToSpawn(CBaseEntity *pEntity) { return true; };  // Can this item spawn (eg monsters don't spawn in deathmatch).
	virtual bool FAllowFlashlight(void) { return true; };// Are players allowed to switch on their flashlight?
	virtual bool FAllowLevelChange(CBasePlayer *pActivator, char *szNextMap, edict_t *pEntLandmark) { return false; };
	virtual bool FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon) { return false; };// should the player switch to this weapon?
	virtual CBasePlayerItem *GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon);// Updated: get next-best weapon, but don't select it

// Functions to verify the single/multiplayer status of a game
	virtual bool IsMultiplayer(void) { return false; };// is this a multiplayer game? (either coop or deathmatch)
	virtual bool IsTeamplay(void) { return false; };// is this deathmatch game being played with team rules?
	virtual bool IsCoOp(void) { return false; };
	virtual bool IsGameOver(void);
	virtual const char *GetGameDescription(void) { return GAMETITLE_DEFAULT_STRING; };// this is the game name that gets seen in the server browser
//	virtual const char *GetDefaultSpawnEntity(void) { return "info_player_start"; }

	virtual void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax) {};// XDM3035c
// Client connection/disconnection
	virtual bool ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128] ) {return true;};// a client just connected to the server (player hasn't spawned yet)
	virtual void ClientDisconnected(CBasePlayer *pPlayer) {};	// a client just disconnected from the server
	virtual bool ClientCommand(CBasePlayer *pPlayer, const char *pcmd) { return false; };  // handles the user commands;  returns TRUE if command handled properly
	virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer);		// the player has changed userinfo;  can change it now
	virtual void InitHUD(CBasePlayer *pPlayer) {};				// the client dll is ready for updating
	virtual void UpdateGameMode(CBasePlayer *pPlayer);			// the client needs to be informed of the current game mode
	virtual bool CheckLimits(void) { return false; };			// has the game reached one of its limits?

// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer) { return 0.0f; };// this client just hit the ground after a fall. How much damage?
	virtual bool FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker) {return true;};// can this player take damage from this attacker?
	virtual bool ShouldAutoAim(CBasePlayer *pPlayer, CBaseEntity *pTarget);

// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer *pPlayer) {};// called by CBasePlayer::Spawn just before releasing player into the game
	virtual void PlayerThink(CBasePlayer *pPlayer) {};// called by CBasePlayer::PreThink every frame, before physics are run and after keys are accepted
	virtual bool FPlayerCanRespawn(CBasePlayer *pPlayer) { return false; };// is this player allowed to respawn now?
	virtual CBaseEntity *PlayerUseSpawnSpot(CBasePlayer *pPlayer);// Place this player on their spawnspot and face them the proper direction.
	virtual int GetPlayerMaxHealth(void) { return MAX_PLAYER_HEALTH; };// XDM3037: now game rules decide it

// Client kills/scoring
	virtual int IPointsForKill(CBaseEntity *pAttacker, CBaseEntity *pKilled) { return 0; };// how many points do I award whoever kills this player?
	virtual void PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor) {};// Called each time a player dies
	virtual void MonsterKilled(CBaseMonster *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor) {};// Called each time a monster dies
	virtual void DeathNotice(CBaseEntity *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor) {};// Call this from within a GameRules class to report an obituary.

// Weapon retrieval
	virtual bool CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pItem);// The player is touching an CBasePlayerItem, do I give it to him?
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon) {};// Called each time a player picks up a weapon from the ground

// Weapon spawn/respawn control
	virtual bool WeaponShouldRespawn(CBasePlayerItem *pWeapon) { return false; };// should this weapon respawn?
	virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon) { return 0.0f; };// when may this weapon respawn?
	virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon) { return 0.0f; }; // can i respawn now,  and if not, when should i try again?
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon) { return g_vecZero; };// where in the world should this weapon respawn?
	virtual float FlWeaponWorldScale(void);

// Item retrieval
	virtual bool CanHaveItem(CBasePlayer *pPlayer, CItem *pItem) { return true; };// is this player allowed to take this item?
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem) {};// call each time a player picks up an item (battery, healthkit, longjump)

// Item spawn/respawn control
	virtual bool ItemShouldRespawn(CItem *pItem) { return 0; };// Should this item respawn?
	virtual float FlItemRespawnTime(CItem *pItem) { return 0.0f; };// when may this item respawn?
	virtual Vector VecItemRespawnSpot(CItem *pItem) { return g_vecZero; };// where in the world should this item respawn?

// Ammo retrieval
	virtual bool CanHaveAmmo(CBasePlayer *pPlayer, const int &iAmmoIndex, const int &iMaxCarry);// can this player take more of this ammo?
	virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, const int &iCount) {};// called each time a player picks up some ammo in the world

// Ammo spawn/respawn control
	virtual bool AmmoShouldRespawn(CBasePlayerAmmo *pAmmo) { return false; };// should this ammo item respawn?
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo) { return 0.0f; };// when should this ammo item respawn?

// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime(void) { return 0.0f; };// how long until a depleted HealthCharger recharges itself?
	virtual float FlHEVChargerRechargeTime(void) { return 0.0f; };// how long until a depleted HealthCharger recharges itself?

// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer *pPlayer) { return GR_PLR_DROP_GUN_NO; };// what do I do with a player's weapons when he's killed?
// What happens to a dead player's ammo	
	virtual int DeadPlayerAmmo(CBasePlayer *pPlayer) { return GR_PLR_DROP_AMMO_NO; };// Do I drop ammo when the player dies? How much?

// Teamplay stuff
//	virtual const char *GetTeamID(CBaseEntity *pEntity) = 0;// what team is this entity on?
	virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget) { return GR_NEUTRAL; };// What is the player's relationship with this entity?
	virtual int GetTeamIndex(const char *pTeamName) { return -1; };
	virtual const char *GetTeamName(int teamIndex) { return ""; };
	virtual bool IsValidTeam(const char *pTeamName) { return true; };
	virtual bool IsValidTeam(int team) { return true; };
	virtual bool IsRealTeam(int team) { return false; };
	virtual void ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, bool bKill, bool bGib) {};
	virtual void ChangePlayerTeam(CBasePlayer *pPlayer, TEAM_ID teamindex, bool bKill, bool bGib) {};
	virtual void AddScoreToTeam(TEAM_ID teamIndex, int score) {};
	virtual void AddScore(CBaseEntity *pWinner, int score) {};
	virtual int GetScoreLimit(void) { return 0; };
	virtual int GetScoreRemaining(void) { return 0; };
	virtual int NumPlayersInTeam(TEAM_ID teamIndex) { return 0; };
	virtual int MaxTeams(void) { return 1; };// maximum allowed number for this game type
	virtual int GetNumberOfTeams(void) { return 0; }// number of currently active teams
	virtual short GetRoundsLimit(void) { return 0; };
	virtual short GetRoundsPlayed(void) { return 0; };

	virtual int CountPlayers(void);
	virtual bool CheckPlayersReady(void) { return true; };

	virtual CBasePlayer *GetBestPlayer(TEAM_ID team) { return NULL; };
	virtual TEAM_ID GetBestTeam(void) { return TEAM_NONE; };
	virtual CBaseEntity *GetTeamBaseEntity(TEAM_ID team) { return NULL; };
	virtual void SetTeamBaseEntity(TEAM_ID team, CBaseEntity *pEntity) {};

	virtual bool PlayTextureSounds(void) { return true; };
	virtual bool PlayFootstepSounds(CBasePlayer *pPlayer, float fvol) { return true; };
	virtual bool FAllowMonsters(void)  { return true; };//are monsters allowed
	virtual bool FAllowEffects(void)  { return true; };// XDM: are effects allowed
	virtual bool FAllowMapMusic(void)  { return true; };// XDM: dynamic map music
	virtual bool FAllowSpectatorChange(CBasePlayer *pPlayer)  { return false; };// XDM
	virtual bool FForceRespawnPlayer(void) { return false; };
	virtual bool FPersistBetweenMaps(void) { return m_bPersistBetweenMaps; };

// Immediately end a multiplayer game
	virtual void EndMultiplayerGame(void) {};
	virtual bool ServerIsFull(void) { return true; };
	virtual void ChangeLevel(void) {};
	virtual void OnChangeLevel(CBasePlayer *pActivator, char *szNextMap, edict_t *pEntLandmark) {};// XDM3035: trigger
	virtual CBaseEntity	*GetIntermissionActor1(void) { return NULL; };
	virtual CBaseEntity	*GetIntermissionActor2(void) { return NULL; };
	virtual void DumpInfo(void) {};

//	void SetGameType(short gametype);// XDM3035c
protected:
//	short m_iGameType;// XDM: enum
	short m_iGameMode;// XDM3035a: additional game mode
	float m_fStartTime;// absolute game start time on current map
	bool m_bGameOver;
	bool m_bPersistBetweenMaps;
};


class CGameRulesSinglePlay : public CGameRules
{
public:
	CGameRulesSinglePlay(void);
	virtual short GetGameType(void) { return GT_SINGLE; };// XDM3035
	virtual void StartFrame(void);

	virtual bool IsAllowedToSpawn(CBaseEntity *pEntity);
	virtual bool FAllowFlashlight(void) { return true; };
	virtual bool FAllowLevelChange(CBasePlayer *pActivator, char *szNextMap, edict_t *pEntLandmark) { return true; };
	virtual bool FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);

// Functions to verify the single/multiplayer status of a game
	virtual bool IsMultiplayer(void) { return false; };
	virtual bool IsTeamplay(void) { return false; };
	virtual const char *GetGameDescription(void) { return "XHL Single"; }

// Client connection/disconnection
	virtual bool ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
	virtual void ClientDisconnected(CBasePlayer *pPlayer);
	virtual bool ClientCommand(CBasePlayer *pPlayer, const char *pcmd);// XDM
	virtual void InitHUD(CBasePlayer *pPlayer);		// the client dll is ready for updating
	virtual bool CheckLimits(void) { return false; };

// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer);

// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual void PlayerThink(CBasePlayer *pPlayer);
	virtual bool FPlayerCanRespawn(CBasePlayer *pPlayer);

// Client kills/scoring
	virtual int IPointsForKill(CBaseEntity *pAttacker, CBaseEntity *pKilled);
	virtual void PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);
	virtual void MonsterKilled(CBaseMonster *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);// XDM3035
	virtual void DeathNotice(CBaseEntity *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);

// Weapon retrieval
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);

// Weapon spawn/respawn control
	virtual bool WeaponShouldRespawn(CBasePlayerItem *pWeapon);
	virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon);
	virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon);
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon);

// Item retrieval
	virtual bool CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem);

// Item spawn/respawn control
	virtual bool ItemShouldRespawn(CItem *pItem);
	virtual float FlItemRespawnTime(CItem *pItem);
	virtual Vector VecItemRespawnSpot(CItem *pItem);

// Ammo retrieval
	virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, const int &iCount);

// Ammo spawn/respawn control
	virtual bool AmmoShouldRespawn(CBasePlayerAmmo *pAmmo);
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo);

// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime(void);

// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer *pPlayer);

// What happens to a dead player's ammo	
	virtual int DeadPlayerAmmo(CBasePlayer *pPlayer);

	virtual bool FAllowEffects(void);
	virtual bool FAllowMapMusic(void)  { return true; };

	virtual CBasePlayer *GetBestPlayer(TEAM_ID team);
	virtual TEAM_ID GetBestTeam(void) { return TEAM_NONE; };

// Teamplay stuff	
	virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget);
};


// frag-based multiplayer game
class CGameRulesMultiplay : public CGameRules
{
public:
	CGameRulesMultiplay();
	virtual ~CGameRulesMultiplay();

	virtual short GetGameType(void) { return GT_DEATHMATCH; };// XDM3035
	virtual void Initialize(void);// XDM
	virtual void StartFrame(void);

	virtual bool IsAllowedToSpawn(CBaseEntity *pEntity);
	virtual bool FAllowFlashlight(void);
	virtual bool FAllowLevelChange(CBasePlayer *pActivator, char *szNextMap, edict_t *pEntLandmark) { return false; };
	virtual bool FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);

// Functions to verify the single/multiplayer status of a game
	virtual bool IsMultiplayer(void) { return true; };
	virtual const char *GetGameDescription(void) { return "XHL Deathmatch"; }// this is the game name that gets seen in the server browser
//	virtual const char *GetDefaultSpawnEntity(void) { return "info_player_deathmatch"; }

// Client connection/disconnection
	virtual bool ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
	virtual void ClientDisconnected(CBasePlayer *pPlayer);
	virtual bool ClientCommand(CBasePlayer *pPlayer, const char *pcmd);
	virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer);// XDM3035
	virtual void InitHUD(CBasePlayer *pPlayer);		// the client dll is ready for updating
//	virtual void UpdateGameMode(CBasePlayer *pPlayer);  // the client needs to be informed of the current game mode
	virtual bool CheckLimits(void);

// Client damage rules
	virtual float FlPlayerFallDamage(CBasePlayer *pPlayer);
	virtual bool  FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker);

// Client spawn/respawn control
	virtual void PlayerSpawn(CBasePlayer *pPlayer);
	virtual void PlayerThink(CBasePlayer *pPlayer);
	virtual bool FPlayerCanRespawn(CBasePlayer *pPlayer);
	virtual CBaseEntity *PlayerUseSpawnSpot(CBasePlayer *pPlayer);
	virtual int GetPlayerMaxHealth(void);// XDM3037

// Client kills/scoring
	virtual int IPointsForKill(CBaseEntity *pAttacker, CBaseEntity *pKilled);
	virtual void PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);
	virtual void MonsterKilled(CBaseMonster *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);// XDM3035
	virtual void DeathNotice(CBaseEntity *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor);

// Weapon retrieval
	virtual bool CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pItem);// The player is touching an CBasePlayerItem, do I give it to him?
	virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);

// Weapon spawn/respawn control
	virtual bool WeaponShouldRespawn(CBasePlayerItem *pWeapon);
	virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon);
	virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon);
	virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon);
	virtual float FlWeaponWorldScale(void);

// Item retrieval
	virtual bool CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
	virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem);

// Item spawn/respawn control
	virtual bool ItemShouldRespawn(CItem *pItem);
	virtual float FlItemRespawnTime(CItem *pItem);
	virtual Vector VecItemRespawnSpot(CItem *pItem);

// Ammo retrieval
	virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, const int &iCount);

// Ammo spawn/respawn control
	virtual bool AmmoShouldRespawn(CBasePlayerAmmo *pAmmo);
	virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo);

// Healthcharger respawn control
	virtual float FlHealthChargerRechargeTime(void);
	virtual float FlHEVChargerRechargeTime(void);

// What happens to a dead player's weapons
	virtual int DeadPlayerWeapons(CBasePlayer *pPlayer);

// What happens to a dead player's ammo	
	virtual int DeadPlayerAmmo(CBasePlayer *pPlayer);

// Teamplay stuff	
//	virtual const char *GetTeamID(CBaseEntity *pEntity) {return "";}
	virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget);
	virtual void AddScore(CBaseEntity *pWinner, int score);
	virtual int GetScoreLimit(void);
	virtual int GetScoreRemaining(void);

	virtual bool PlayTextureSounds(void);
	virtual bool PlayFootstepSounds(CBasePlayer *pPlayer, float fvol);

	virtual bool FAllowMonsters(void);
	virtual bool FAllowEffects(void);
	virtual bool FAllowMapMusic(void);
	virtual bool FAllowSpectatorChange(CBasePlayer *pPlayer);
	virtual bool FForceRespawnPlayer(void);

	virtual bool CheckPlayersReady(void);

	virtual CBasePlayer *GetBestPlayer(TEAM_ID team);
	virtual TEAM_ID GetBestTeam(void) { return TEAM_NONE; };

	// Immediately end a multiplayer game
	virtual void EndMultiplayerGame(void);

	virtual bool ServerIsFull(void);
	virtual void ChangeLevel(void);
	virtual CBaseEntity	*GetIntermissionActor1(void);
	virtual CBaseEntity	*GetIntermissionActor2(void);
	virtual void DumpInfo(void);

protected:
	virtual void GoToIntermission(CBasePlayer *pWinner, CBaseEntity *pInFrameEntity);
	virtual void SendLeftUpdates(void);// XDM3036: former parameters are useless

	void SendMOTDToClient(CBasePlayer *pClient);

	CBaseEntity	*m_pLastVictim;
	CBaseEntity	*m_pIntermissionEntity1;// primary, can be NULL
	CBaseEntity	*m_pIntermissionEntity2;// in-frame entity, can be NULL

	float m_flIntermissionEndTime;
	float m_flIntermissionStartTime;// XDM ?
	bool m_bReadyButtonsHit;

	int m_iRemainingScore;
	float m_fRemainingTime;

	int m_iRemainingScoreSent;
	int m_iRemainingTimeSent;// float is bad for comparsion
	int last_frags;
	int last_time;

	int m_iFirstScoredPlayer;// index of the first player to score (for GAME_EVENT_FIRST_SCORE)
	int m_iLeader;// index of current best player

//	string_t m_pAddItems[MAX_ADD_DEFAULT_ITEMS];
//	int m_iAddItemsCount;
//	mapcycle_t mapcycle;
};

int DetermineGameType(void);
CGameRules *InstallGameRules(int game_type);
bool IsActiveTeam(const TEAM_ID &team_id);
bool IsActivePlayer(CBaseEntity *pPlayerEntity);
bool IsActivePlayer(const int &idx);
bool IsValidPlayerIndex(const int &idx);

bool DeveloperCommand(CBasePlayer *pPlayer, const char *pcmd);// XDM: special version of ClientCommand() for development/administrative purposes

extern DLL_GLOBAL CGameRules *g_pGameRules;
extern DLL_GLOBAL FILE *g_pServerAutoFile;
extern DLL_GLOBAL const char g_szServerAutoFileName[];

#endif // GAMERULES_H
