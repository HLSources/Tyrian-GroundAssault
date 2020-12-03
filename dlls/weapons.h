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
#ifndef WEAPONS_H
#define WEAPONS_H

#include "effects.h"
#include "weapondef.h"
#include "basemonster.h"
#include "shared_resources.h"
//#include "entity_state.h"
//#include "usercmd.h"

typedef struct 
{
	CBaseEntity		*pEntity;
	float			amount;
	int				type;
} MULTIDAMAGE;

extern MULTIDAMAGE gMultiDamage;

void ClearMultiDamage(void);
void ApplyMultiDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker);
void AddMultiDamage(CBaseEntity *pInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType);


// XDM3035a
extern ItemInfo g_ItemInfoArray[MAX_WEAPONS];
extern AmmoInfo g_AmmoInfoArray[MAX_AMMO_SLOTS];
extern int giAmmoIndex;

void DecalGunshot(TraceResult *pTrace, const int &iBulletType);
int DamageDecal(CBaseEntity *pEntity, const int &bitsDamageType);
void RadiusDamage(const Vector &vecSrc, CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);
Vector FireBullets(ULONG cShots, const Vector &vecSrc, const Vector &vecDirShooting, const Vector &vecSpread, Vector *endpos, float flDistance, int iBulletType, int iDamage, int iDamageType, CBaseEntity *pInflictor = NULL, CBaseEntity *pAttacker = NULL, int shared_rand = 0);


bool CleanupTemporaryWeapons(void);// XDM3035c

//-----------------------------------------------------------------------------
#ifdef CLIENT_DLL
bool bIsMultiplayer(void);
void LoadVModel(char *szViewModel, CBasePlayer *m_pPlayer);
#endif

void W_Precache(void);

int AddAmmoToRegistry(const char *szAmmoName, int iMaxCarry);
int GetAmmoIndexFromRegistry(const char *szAmmoName);
int MaxAmmoCarry(int ammoID);
int MaxAmmoCarry(const char *szName);

//-----------------------------------------------------------------------------
class CBasePlayerWeapon;
class CBaseMonster;
class CBasePlayer;


// HasAmmo() FLAGS!
#define AMMO_ANYTYPE	0// same as (AMMO_PRIMARY|AMMO_SECONDARY)
#define AMMO_PRIMARY	1
#define AMMO_SECONDARY	2
#define AMMO_CLIP		4// check clip separately

// CBasePlayer::AddPlayerItem() result codes
/*
flags
#define ITEM_ADDRESULT_NONE			0
#define ITEM_ADDRESULT_PICKED		1
#define ITEM_ADDRESULT_EXTRACTED	2
#define ITEM_ADDRESULT_REMOVE		4
*/

enum
{
	ITEM_ADDRESULT_NONE = 0,// left alone, don't do anything
	ITEM_ADDRESULT_PICKED,// picked, modev into player's inventory, attach, don't destroy
	ITEM_ADDRESULT_EXTRACTED// extracted ammo, may be destroyed
};

//-----------------------------------------------------------------------------
// XDM: Base grenade class
// Hand grenades, ARgrenades, launcher grenades and most other projectiles
//-----------------------------------------------------------------------------
class CGrenade : public CBaseMonster
{
public:
	virtual void Spawn(void);
	virtual int	BloodColor(void) { return DONT_BLEED; }
	virtual int Classify(void) { return CLASS_GRENADE; }// XDM
	virtual BOOL FBecomeProne(void) {return TRUE;};// XDM: for barnacles 8)
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);

	static CGrenade *ShootTimed(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecVelocity, float time);
	static CGrenade *ShootContact(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecVelocity);

	void Explode(TraceResult *pTrace, int bitsDamageType);
	void DoDamageInit(const float &life, const float &updatetime, const float &damagedelta, const int &bitsDamageType);
	void EXPORT Impact(CBaseEntity *pOther, bool StopCollide, float ScreenShakeAmplitude, float ScreenShakeRadius, bool PaintDecal, int decal, float ImpactDmg, int bitsImpactDamageType, float RadiusDmg, float RadiusDmgRadius, int bitsRadiusDamageType, float WallDist, int FX_Type);
	void EXPORT Blast(float ScreenShakeAmplitude, float ScreenShakeRadius, float RadiusDmg, float RadiusDmgRadius, int bitsRadiusDamageType, int FX_Type);

	void EXPORT SeekTarget(float radius, float view_field);
	void EXPORT MovetoTarget(const Vector &vecTarget, float speed);
	void EXPORT SearchTarget(void);
	void EXPORT SpeedUp(void);
	void EXPORT RemoveIfInSky(void);
	void EXPORT DangerSoundThink(void);
	void EXPORT Detonate(void);
	void EXPORT TumbleThink(void);
	void EXPORT DoDamageThink(void);// XDM
	void EXPORT BounceTouch(CBaseEntity *pOther);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	virtual	BOOL IsMonster(void) { return FALSE; }// XDM
	virtual BOOL IsProjectile(void) {return TRUE;}
	virtual BOOL IsPushable(void) {return FALSE;}
	virtual BOOL ShouldRespawn(void) { return FALSE; }// XDM3035
	void EXPORT NuclearExplodeThink(void);
	virtual void BounceSound(void);

	BOOL m_fRegisteredSound;
};


//-----------------------------------------------------------------------------
// XDM: Base item class
// Items that the player has in their inventory that they can use
//-----------------------------------------------------------------------------
class CBasePlayerItem : public CBaseAnimating
{
public:
//	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void SetObjectCollisionBox(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);// XDM3035c
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);// XDM3035c
	virtual int ShouldCollide(CBaseEntity *pOther);// XDM3035c

	virtual void Initialize(void);

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	virtual int AddToPlayer(CBasePlayer *pPlayer);	// return TRUE if the item you want the item added to the player inventory
	virtual int AddDuplicate(CBasePlayerItem *pOriginal);	// return TRUE if you want your duplicate removed from world
//	virtual void Drop(void);
	virtual void AttachToPlayer(CBasePlayer *pPlayer);
	virtual void DetachFromPlayer(void);

	virtual void ItemPreFrame(void)	{ return; }		// called each frame by the player PreThink
	virtual void ItemPostFrame(void) { return; }		// called each frame by the player PostThink
	virtual void ClientPostFrame(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, const double &time, const unsigned int &random_seed) { return; }// called by HUD_WeaponsPostThink (client-side only)
	virtual void ClientPackData(struct edict_s *player, struct weapon_data_s *weapondata) { return; }// called by GetWeaponData (server-side only)

	virtual CBaseEntity *StartRespawn(void);// copy a weapon
	virtual int GetItemInfo(ItemInfo *p) { return 0; };	// returns 0 if struct not filled out

	virtual BOOL CanDeploy(void) { return TRUE; };
	virtual BOOL CanHolster(void) { return TRUE; };// can this weapon be put away right now?
	virtual BOOL ForceAutoaim(void) { return FALSE; };// XDM
	virtual BOOL IsPlayerItem(void) { return TRUE; }// XDM3035
	virtual BOOL IsPlayerWeapon(void) { return FALSE; }// XDM3035
	virtual BOOL IsHolstered(void);// XDM3035
	virtual BOOL IsPushable(void);
	virtual bool IsPickup(void) { return true; };// XDM3037
	virtual BOOL IsCarried(void);
//	virtual BOOL HasAmmo(void);
	virtual BOOL HasAmmo(byte type);// primary/secondary

	virtual int PrimaryAmmoIndex(void);// const { return -1; };
	virtual int SecondaryAmmoIndex(void);// const { return -1; };

	virtual BOOL Deploy(void) { return TRUE; };// returns is deploy was successful
	virtual void Holster(int skiplocal = 0);
	virtual void PrintState(void) {};
//	virtual void OnOwnerDied(CBasePlayer *pOwner) {};// XDM3034

	virtual int UpdateClientData(char *pBuffer = NULL) { return 0; }
	virtual void UpdateItemInfo(void);

	virtual void OnFreePrivateData(void);

	virtual CBasePlayerWeapon *GetWeaponPtr(void) { return NULL; };

	void EXPORT DestroyItem(void);
	void EXPORT DefaultTouch(CBaseEntity *pOther);	// default weapon touch
	void EXPORT FallThink(void);// when an item is first spawned, this think is run to determine when the object has hit the ground.
	void EXPORT Materialize(void);// make a weapon visible and tangible
	void EXPORT AttemptToMaterialize(void);  // the weapon desires to become visible and tangible, if the game rules allow for it

	void FallInit(void);

	void SetOwner(CBasePlayer *pPlayer);
//	void SetID(const int &ID);

	CBasePlayer	*GetOwner(void) const;
	inline const int &GetID(void) const { return m_iId; };

#ifdef SERVER_WEAPON_SLOTS
	int			iItemSlot(void)		{ return g_ItemInfoArray[m_iId].iSlot + 1; }
	int			iItemPosition(void)	{ return g_ItemInfoArray[m_iId].iPosition; }
#endif
	const char	*pszAmmo1(void)		{ return g_ItemInfoArray[m_iId].pszAmmo1; }
	int			iMaxAmmo1(void)		{ return g_ItemInfoArray[m_iId].iMaxAmmo1; }
	const char	*pszAmmo2(void)		{ return g_ItemInfoArray[m_iId].pszAmmo2; }
	int			iMaxAmmo2(void)		{ return g_ItemInfoArray[m_iId].iMaxAmmo2; }
	const char	*pszName(void)		{ return g_ItemInfoArray[m_iId].pszName; }
	int			iMaxClip(void)		{ return g_ItemInfoArray[m_iId].iMaxClip; }
	int			iWeight(void)		{ return g_ItemInfoArray[m_iId].iWeight; }
	int			iFlags(void)		{ return g_ItemInfoArray[m_iId].iFlags; }

protected:
	virtual void Kill(void);

	CBasePlayer	*m_pPlayer;
	EHANDLE		m_hLastOwner;// never reset this
	int			m_iId;
	int			m_iModelIndexView;// v_
	int			m_iModelIndexWorld;// p_
//	valve preferred to hold item info in one common instance of array ItemInfo	m_ItemInfo;
};

//-----------------------------------------------------------------------------
// XDM: Base weapon class
// CBasePlayerItem virtual functions should go first!
//-----------------------------------------------------------------------------
enum weapon_e
{
	TYRIAN_WEAPON_ANIM_IDLE = 0,
	TYRIAN_WEAPON_ANIM_FIRE,
	TYRIAN_WEAPON_ANIM_FIRE_ALT,
	TYRIAN_WEAPON_ANIM_DRAW,
	TYRIAN_WEAPON_ANIM_HOLSTER
};

class CBasePlayerWeapon : public CBasePlayerItem
{
public:
	virtual void Spawn(void);
	virtual void Initialize(void);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	// generic weapon versions of CBasePlayerItem calls
	virtual int AddToPlayer(CBasePlayer *pPlayer);
	virtual int AddDuplicate(CBasePlayerItem *pOriginal);

	virtual void SwitchToSWeapon( void ) {return;}
	virtual void ItemPostFrame(void);
	virtual void ClientPostFrame(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, const double &time, const unsigned int &random_seed);
	virtual void ClientPackData(struct edict_s *player, struct weapon_data_s *weapondata);
	virtual int UpdateClientData(char *pBuffer);

	virtual BOOL CanDeploy(void);
	virtual BOOL ForceAutoaim(void) { return FALSE; };// XDM
	virtual BOOL IsPlayerWeapon(void) { return FALSE; }// XDM3035
//	virtual BOOL HasAmmo(void);// XDM
	virtual BOOL HasAmmo(byte type);

	virtual BOOL CanAttack(const float &attack_time);// XDM3035
	virtual BOOL IsUseable(void);
	virtual BOOL ShouldWeaponIdle(void) {return FALSE; };
	virtual BOOL UseDecrement(void);

	virtual int	PrimaryAmmoIndex(void);// const;
	virtual int	SecondaryAmmoIndex(void);// const;

	virtual int ExtractAmmo(CBasePlayerWeapon *pWeapon);
	virtual int ExtractClipAmmo(CBasePlayerWeapon *pWeapon);
	virtual BOOL PlayEmptySound(void);
	virtual void ResetEmptySound(void);
	virtual void SendWeaponAnim(const int &iAnim, const int &body = 0, bool skiplocal = true);
	virtual void PrintState(void);

	virtual void Holster(int skiplocal = 0);
	virtual void PrimaryAttack(void) { return; }				// do "+ATTACK"
	virtual void SecondaryAttack(void) { return; }				// do "+ATTACK2"
	virtual void Reload(void) { return; }						// do "+RELOAD"
	virtual void WeaponIdle(void) { return; }					// called when no buttons pressed
	virtual void RetireWeapon(void);
	virtual int UseAmmo(byte type, int count);// XDM3035c
	virtual float GetBarrelLength(void) const;// XDM3038

	int AddPrimaryAmmo(const int &iCount, const int &iAmmoIndex);//, const int &iMaxClip, const int &iMaxCarry);
	int AddSecondaryAmmo(const int &iCount, const int &iAmmoIndex);//, const int &iMaxCarry);

	BOOL DefaultDeploy(const char *szViewModel, const char *szWeaponModel, int iViewAnim, const char *szAnimExt, int body);
	int DefaultReload(const int &iClipSize, const int &iAnim, const float &fDelay, const int &body = 0);

	virtual CBasePlayerWeapon *GetWeaponPtr(void) { return this; };

	float	m_flNextAmmoBurn;			// XDM: while charging, when to absorb another unit of player's ammo?
	float	m_flPumpTime;
	int		m_fInSpecialReload;			// Are we in the middle of a reload for the shotguns
	float	m_flNextPrimaryAttack;		// soonest time ItemPostFrame will call PrimaryAttack
	float	m_flNextSecondaryAttack;	// soonest time ItemPostFrame will call SecondaryAttack
	float	m_flTimeWeaponIdle;			// soonest time ItemPostFrame will call WeaponIdle
	int		m_iClip;					// number of shots left in the primary weapon clip, -1 it not used
	int		m_iClientClip;				// the last version of m_iClip sent to hud dll
	int		m_iClientWeaponState;		// the last version of the weapon state sent to hud dll (is current weapon, is on target)
	int		m_fInReload;				// Are we in the middle of a reload;
	int		m_iDefaultAmmo;// how much ammo you get when you pick up this weapon as placed by a level designer.

protected:
	int		m_iPrimaryAmmoType;			// "primary" ammo index into players m_rgAmmo[]
	int		m_iSecondaryAmmoType;		// "secondary" ammo index into players m_rgAmmo[]
	int m_iPlayEmptySound;
	int m_fFireOnEmpty;		// True when the gun is empty and the player is still holding down the attack key(s)
};


//-----------------------------------------------------------------------------
// XDM: Base ammo class
// Ammo mechanism changed since original HL
//-----------------------------------------------------------------------------
class CBasePlayerAmmo : public CBaseEntity
{
public:
	virtual int Save(CSave &save);// XDM
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];
	virtual void Spawn(void);
	virtual void Precache(void);// XDM
	virtual BOOL IsPushable(void);
	virtual bool IsPickup(void) { return true; };// XDM3037
	virtual CBaseEntity *Respawn(void);
	virtual BOOL AddAmmo(CBaseEntity *pOther);
	void EXPORT DefaultTouch(CBaseEntity *pOther); // default weapon touch
	void EXPORT Materialize(void);
	void InitAmmo(const int &ammo_give, const char *name, const int &ammo_max);
	inline int GetAmmoGive(void) { return m_iAmmoGive; };
	inline int GetAmmoMax(void) { return m_iAmmoMax; };
	inline const char *GetAmmoName(void) { return STRING(pev->message); }
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);// XDM
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);// XDM3035c

protected:// XDM
	int	m_iAmmoGive;
	int m_iAmmoMax;
//	char *szName;// XDM: custom name is now in pev->message
};

//-----------------------------------------------------------------------------
// XDM: Weapon Box
// A single entity that can store weapons and ammo
//-----------------------------------------------------------------------------
class CWeaponBox : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int	ObjectCaps(void) { return (CBaseEntity::ObjectCaps() | FCAP_IMPULSE_USE); }
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);// XDM
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);// XDM3035c
//	virtual void SetObjectCollisionBox(void);
	virtual void OnFreePrivateData(void);
	void EXPORT Kill(void);

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	static CWeaponBox *CreateBox(CBaseEntity *pOwner, const Vector &vecOrigin, const Vector &vecAngles);

	void Clear(void);
	bool IsEmpty(void);
	bool HasWeapon(const int &iID);
//	BOOL HasWeapon(CBasePlayerItem *pCheckItem);
	BOOL PackWeapon(CBasePlayerItem *pWeapon);
	bool PackAmmo(const char *szName, const int &iCount);
	int StoreAmmo(const int &iCount, const char *szName, const int &iMax, int *pIndex = NULL);

protected:
	string_t m_rgiszAmmo[MAX_AMMO_SLOTS];// ammo names
	int	m_rgAmmo[MAX_AMMO_SLOTS];// ammo quantities
	int m_cAmmoTypes;// how many ammo types packed into this box (if packed by a level designer)
	CBasePlayerItem	*m_rgpWeapons[MAX_WEAPONS];// one slot for each 
//	EHANDLE	m_rgpWeapons[MAX_WEAPONS];
};

#endif // WEAPONS_H
