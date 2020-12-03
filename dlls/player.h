/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef PLAYER_H
#define PLAYER_H

#include "basemonster.h"

#define PLAYER_INVENTORY_SIZE	MAX_WEAPONS// not +1 because last possible ID is 31
#define MAX_CHECKPOINTS			16 // XDM3035c: per level

typedef enum
{
	PLAYER_IDLE,
	PLAYER_IDLE_FROZEN,
	PLAYER_WALK,
	PLAYER_JUMP,
	PLAYER_SUPERJUMP,
	PLAYER_DIE,
	PLAYER_ATTACK1,
	PLAYER_ARM,
	PLAYER_DISARM,
	PLAYER_RELOAD,
	PLAYER_CLIMB,
	PLAYER_FALL
} PLAYER_ANIM;

#define MAX_ID_RANGE		3200
#define SBAR_STRING_SIZE	128

enum sbar_data
{
//	SBAR_START = 0,
	SBAR_ID_TARGETNAME = 1,
	SBAR_ID_TARGETHEALTH,
	SBAR_ID_TARGETARMOR,
	SBAR_END,
};

#define CHAT_INTERVAL 1.0f

#define DOT_1DEGREE   0.9998476951564
#define DOT_2DEGREE   0.9993908270191
#define DOT_3DEGREE   0.9986295347546
#define DOT_4DEGREE   0.9975640502598
#define DOT_5DEGREE   0.9961946980917
#define DOT_6DEGREE   0.9945218953683
#define DOT_7DEGREE   0.9925461516413
#define DOT_8DEGREE   0.9902680687416
#define DOT_9DEGREE   0.9876883405951
#define DOT_10DEGREE  0.9848077530122
#define DOT_15DEGREE  0.9659258262891
#define DOT_20DEGREE  0.9396926207859
#define DOT_25DEGREE  0.9063077870367

#define AUTOAIM_1DEGREES  0.0124442437025
#define AUTOAIM_2DEGREES  0.0348994967025
#define AUTOAIM_5DEGREES  0.08715574274766
#define AUTOAIM_8DEGREES  0.1391731009601
#define AUTOAIM_10DEGREES 0.1736481776669

#define TRAIN_ACTIVE	0x80 
#define TRAIN_NEW		0xc0
#define TRAIN_OFF		0x00

#define ARMOR_RATIO			0.01 // Armor Takes 99% of the damage
#define ARMOR_BONUS			0.5// Each Point of Armor is work 1/x points of health

#define	PLAYER_USE_SEARCH_RADIUS	(float)64
#define PLAYER_AIR_TIME		12// lung full of air lasts this many seconds

// if in range of radiation source, ping geiger counter
#define GEIGERDELAY			0.25

#define PLAYER_MAX_WALK_SPEED	220// otherwise player is considered running

extern bool gInitHUD;


//-----------------------------------------------------
// This is Half-Life player entity
//-----------------------------------------------------
class CBasePlayer : public CBaseMonster
{
	friend class CBasePlayerItem;
	friend class CBasePlayerWeapon;
public:

	virtual int Classify(void);
	virtual void Spawn(byte restore);// XDM
	virtual void Spawn(void);
	virtual void Precache(void);
	// Player is moved across the transition by other means
	virtual int ObjectCaps(void) { return (CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION)| FCAP_IMPULSE_USE; }
	virtual void Touch(CBaseEntity *pOther);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	virtual void Jump(void);
	virtual void Duck(void);
	virtual BOOL IsPushable(void);
	virtual BOOL PlaceMine(void);

	virtual void PreThink(void);
	virtual void PostThink(void);
	virtual Vector GetGunPosition(void);
	virtual void TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);
	virtual Vector BodyTarget(const Vector &posSrc) { return Center() + pev->view_ofs * RANDOM_FLOAT(0.5f, 1.1f); };		// position to shoot at

	virtual BOOL IsAdministrator(void);// XDM3034: TODO: UNDONE: !!!
	virtual BOOL IsAlive(void)		{ return (pev->deadflag == DEAD_NO) && pev->health > 0; }
	virtual	BOOL IsPlayer(void)		{ return TRUE; }// Spectators should return FALSE for this, they aren't "players" as far as game logic is concerned
	virtual BOOL IsNetClient(void)	{ return TRUE; }// Bots should return FALSE for this, they can't receive NET messages Spectators should return TRUE for this
	virtual	BOOL IsMonster(void)	{ return FALSE; }// XDM
	virtual BOOL IsHuman(void)		{ return TRUE; }// XDM: modify if there will be non-human players
//	virtual	BOOL IsBot(void)		{ return (/*GETPLAYERAUTHID(edict()) == NULL || */pev->flags & FL_FAKECLIENT); }
	virtual	BOOL IsBot(void)		{ return (pev->flags & FL_FAKECLIENT)?TRUE:FALSE; }
	virtual bool IsObserver(void);
	virtual BOOL ShouldRespawn(void) { return FALSE; }// XDM3035
	virtual BOOL ShouldFadeOnDeath(void) { return FALSE; }
	virtual float DamageForce(const float &damage);

	virtual BOOL FBecomeProne(void);
	virtual void BarnacleVictimBitten(CBaseEntity *pBarnacle);
	virtual void BarnacleVictimReleased(void);
	virtual int Illumination(void);

	virtual void FrozenStart(float freezetime);// XDM
	virtual void FrozenEnd(void);
	virtual void FrozenThink(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);// XDM3035: players can 'use' each other
	virtual void BotThink(void) {};// XDM

	float GetShootSpreadFactor(void);// XDM3037
	Vector GetAutoaimVector(const float &flDelta);
	Vector AutoaimDeflection(const Vector &vecSrc, const float &flDist, const float &flDelta);

	void TabulateAmmo(void);
	int AmmoInventory(const int &iAmmoIndex);
	virtual int GiveAmmo(const int &iAmount, const int &iIndex, const int &iMax);
	virtual int GiveAmmo(const int &iAmount, char *szName, const int &iMax);

	virtual float FallDamage(const float &flFallVelocity);// XDM3035c

	// JOHN: sends custom messages if player HUD data has changed  (eg health, ammo)
	void UpdateClientData(void);
	void SendAmmoUpdate(void);
	void SendWeaponsUpdate(void);

	CBaseEntity *GiveNamedItem(const char *szName);

	void PackDeadPlayerItems(void);
	void RemoveAllItems(bool removeSuit);
	void ResetAutoaim(void);

	CBasePlayerItem *GetInventoryItem(const int &iItemId);
	void DeployActiveItem(void);
	void QueueItem(CBasePlayerItem *pItem);
	BOOL AddPlayerItem(CBasePlayerItem *pItem);
	BOOL RemovePlayerItem(CBasePlayerItem *pItem);
	BOOL DropPlayerItem(CBasePlayerItem *pItem);
	BOOL DropPlayerItem(char *pszItemName);
	BOOL HasPlayerItem(CBasePlayerItem *pCheckItem);
	BOOL HasWeapons(void);// do I have ANY weapons?
	void SelectLastItem(void);
	BOOL SelectItem(CBasePlayerItem *pItem);// XDM3035
	BOOL SelectItem(const int &iID);// XDM3035
	BOOL SelectItem(const char *pstr);
	CBasePlayerItem *SelectNextBestItem(CBasePlayerItem *pItem);
	void ItemPreFrame(void);
	void ItemPostFrame(void);
	virtual BOOL CanAttack(void);

	BOOL FlashlightIsOn(void);
	void FlashlightTurnOn(void);
	void FlashlightTurnOff(void);

	void UpdatePlayerSound(void);
	virtual void DeathSound(void);

	void SetAnimation(PLAYER_ANIM playerAnim);
	void SetWeaponAnimType(const char *szExtention);

	// custom player functions
	virtual void ImpulseCommands(void);
	void CheatImpulseCommands(const int &iImpulse);

	// XDM
	void StartObserver(const Vector &vecPosition, const Vector &vecViewAngle, int mode, CBaseEntity *pTarget);
	void StopObserver(void);
	void Observer_FindNextPlayer(bool bReverse);
	void Observer_HandleButtons(void);
	void Observer_SetMode(int iMode);
	void Observer_SetTarget(CBaseEntity *pTarget);

	void EnableControl(bool fControl);
	BOOL IsOnLadder(void);
	void DisableLadder(const float &time);

	BOOL IsOnTrain(void);
	BOOL TrainAttach(CBaseEntity *pTrain);
	BOOL TrainDetach(void);
	CBaseDelay *GetControlledTrain(void);
	void TrainPreFrame(void);
	void WaterMove(void);
	void PlayerUse(void);

	void EXPORT PlayerDeathThink(void);
	void UpdateGeigerCounter(void);
	void CheckTimeBasedDamage(void);
	void ForceClientDllUpdate(void);  // Forces all client .dll specific data to be resent to client.

	void SetCustomDecalFrames(int nFrames);
	int GetCustomDecalFrames(void);

	//Player ID
	void InitStatusBar(void);
	void UpdateStatusBar(void);

	void OnCheckPoint(CBaseEntity *pCheckPoint);
	bool PassedCheckPoint(CBaseEntity *pCheckPoint);

	void PowerUpThink(void);
public:
	static TYPEDESCRIPTION m_playerSaveData[];

	int				random_seed;// See that is shared between client & server for shared weapons code
	float 			m_fFrozenFXTime;
	float 			m_fBurnFXTime;
	float 			m_fAirStrikeTime; 
	float 			m_fTripmineTime; 
	float 			m_fSpiderMineTime; 
	float 			m_fNukeTime; 
	float			m_fSatelliteStrikeTime;
	float			m_fFlashBangTime;

	int				m_iTargetVolume;// ideal sound volume. 
	int				m_iWeaponVolume;// how loud the player's weapon is right now.
	int				m_iExtraSoundTypes;// additional classification for this weapon's sound
	int				m_iWeaponFlash;// brightness of the weapon flash
	float			m_flStopExtraSoundTime;

	int				m_iFlashBattery;// Flashlight Battery Draw

	int				m_afButtonLast;
	int				m_afButtonPressed;
	int				m_afButtonReleased;

	edict_t		   *m_pentSndLast;// last sound entity to modify player room type
/*	float*/	short	m_flSndRoomtype;// last roomtype set by sound entity // XDM: short!
	float			m_flSndRange;// dist from player to sound entity


	int				m_rgItems[MAX_ITEMS];
	int				m_fKnownItem;// True when a new item needs to be added

	float			m_fNextSuicideTime;// the time after which the player can next use the suicide command
	float		m_tbdPrev;// Time-based damage timer

	float		m_flgeigerRange;// range to nearest radiation source
	float		m_flgeigerDelay;// delay per update of range msg to client
	int			m_igeigerRangePrev;
//	int			m_iStepLeft;// alternate left/right foot stepping sound
//	char		m_szTextureName[CBTEXTURENAMEMAX];// current texture name we're standing on
//	char		m_chTextureType;// current texture type

	int			m_idrowndmg;// track drowning damage taken
	int			m_idrownrestored;// track drowning damage restored

	int			m_bitsHUDDamage;// Damage bits for the current fame. These get sent to the hud via the DAMAGE message
	BOOL		m_fInitHUD;// True when deferred HUD restart msg needs to be sent
	BOOL		m_fGameHUDInitialized;
	int			m_iTrain;// Train control position
	EHANDLE		m_pTrain;// XDM3035a
	BOOL		m_fWeapon;// Set this to FALSE to force a reset of the current weapon HUD info
	BOOL		m_fInitEntities;// XDM3035: do we need to ask entities to send client data? 
	int			m_iInitEntity;// XDM3035: next entity to search for
	float		m_flInitEntitiesTime;// XDM3035: slow down updates to avoid datagram overflows

	EHANDLE		m_pTank;// the tank which the player is currently controlling,  NULL if no tank
	float		m_fDeadTime;// the time at which the player died  (used in PlayerDeathThink())

	BOOL		m_fNoPlayerSound;// a debugging feature. Player makes no sound if this is true. 
	BOOL		m_fLongJump;// does this player have the longjump module?

	float		m_fBlindAmount;

	int			m_iItemFireSupressor;
	int			m_iItemAntidote;
	int			m_iItemAccuracy;
	int			m_iItemBanana;
	int			m_iItemHaste;
	int			m_iItemRapidFire;
	int			m_iItemQuadDamage;
	int			m_iItemInvisibility;
	int			m_iItemInvulnerability;
	int			m_iOldShieldAmount;
	int			m_iItemShieldRegen;
	int			m_iItemLightningField;
	int			m_iItemRadShield;
	int			m_iItemPlasmaShield;

	int			m_iItemHealthAug;
	int			m_iItemShieldAug;
	int			m_iItemShieldStrengthAug;
	int			m_iItemWeaponPowerAug;

	int 			m_iKillGunType;

//does player have a shield?
	BOOL		 m_iShield;
	BOOL		 m_iGenerator;
	BOOL		 m_iWpnDestroyed;

	int			m_iGeneratorPower;
	int			m_iPowerUseShield;

	//item time update
	float		m_fTimeQuadUpdate;
	float		m_fTimeInvulUpdate;
	float		m_fTimeInvisUpdate;
	float		m_fTimeRapidUpdate;
	float		m_fTimeHasteUpdate;
	float		m_fTimeBananaUpdate;
	float		m_flShieldRegenUpdate;
	float		m_fTimeAccuracyUpdate;
	float		m_fTimeAntidoteUpdate;
	float		m_fTimeFireSupressorUpdate;

	float		m_fTimeLightningFieldUpdate;
	float		m_iTimeRadShieldUpdate;
	float		m_iTimePlasmaShieldUpdate;

	float		m_fShieldFxTime;
	float		m_fPlasmaShieldTime;

	float		m_fHitFxTime;

	float		m_fWaterCircleTime;
//	int			m_iUpdateTime;// stores the number of frame ticks before sending HUD update messages
	int			m_iClientHealth;// the health currently known by the client.  If this changes, send a new
	int			m_iClientBattery;// the Battery currently known by the client.  If this changes, send a new
	int			m_iClientWpnIcon;
	int			m_iHideHUD;// the players hud weapon info is to be hidden
	int			m_iClientHideHUD;
	int			m_iFOV;// field of view
	int			m_iClientFOV;// client's known FOV
	// usable player items 
	CBasePlayerItem *m_pActiveItem;
	CBasePlayerItem *m_pLastItem;
	CBasePlayerItem *m_pNextItem;// XDM: select this one
	// shared ammo slots
	int			m_rgAmmo[MAX_AMMO_SLOTS];
	int			m_rgAmmoLast[MAX_AMMO_SLOTS];

	Vector		m_vecAutoAim;
	Vector		m_vecAutoAimPrev;// These are the previous update's crosshair angles, DON"T SAVE/RESTORE
	BOOL		m_fOnTarget;
	int			m_iDeaths;
	float		m_iRespawnFrames;// used in PlayerDeathThink() to make sure players can always respawn

	float		m_flNextObserverInput;
	EHANDLE		m_hObserverTarget;// XDM
	EHANDLE		m_hAutoaimTarget;
	CBaseEntity	*m_pCarryingObject;// not EHANDLE, because don't need to save
//	EHANDLE invalidates after each respawn!

	float m_flIgnoreLadderStopTime;// XDM3037: time to enable ladder detection
	float m_flNextDecalTime;// next time this player can spray a decal
	float m_flNextChatTime;
	float m_flLastSpawnTime;
	int m_iSpawnState;
	bool m_bReadyPressed;

	int m_iScoreCombo;// XDM3035: accumulated score
	int m_iLastScoreAward;// XDM3035: last type of award (or number of times scored)
	float m_fNextScoreTime;// XDM3035: after this time passed the player won't get award increased
	BOOL m_bDisplayTitle;// XDM3035
//	/*TEAM_ID*/int m_iLastTeamID;// XDM3035a
	int m_iszCheckPoints[MAX_CHECKPOINTS];// XDM3035c

protected:
	unsigned int	m_afPhysicsFlags;// physics flags - set when 'normal' physics should be revisited or overriden
	int			m_nCustomSprayFrames;// Custom clan logo frames for this player

	CBasePlayerItem	*m_rgpWeapons[PLAYER_INVENTORY_SIZE];// XDM: that original weird system caused too much crashes!
	CBasePlayerItem *m_pClientActiveItem;// client version of the active item

	int m_izSBarState[SBAR_END];
	float m_flNextSBarUpdateTime;
	float m_flStatusBarDisappearDelay;
//	char m_SbarString0[SBAR_STRING_SIZE];
	char m_SbarString1[SBAR_STRING_SIZE];

	char m_szAnimExtention[32];
	int m_iSequenceDeepIdle;//LookupSequence( "deep_idle" );
};

#endif // PLAYER_H
