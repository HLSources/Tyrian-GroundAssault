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
#ifndef CBASE_H
#define CBASE_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* !__MINGW32__ */
#endif /* _WIN32 */

/*
Class Hierachy

CBaseEntity
	CBaseDelay
		CBaseAnimating
			CBaseToggle
				CBaseMonster
					CBasePlayer
*/

#define		MAX_PATH_SIZE	10 // max number of nodes available for a path.

// These are caps bits to indicate what an object's capabilities (currently used for save/restore and level transitions)
#define		FCAP_CUSTOMSAVE				0x00000001
#define		FCAP_ACROSS_TRANSITION		0x00000002		// should transfer between transitions
#define		FCAP_MUST_SPAWN				0x00000004		// Spawn after restore
#define		FCAP_DONT_SAVE				0x80000000		// Don't save this
#define		FCAP_IMPULSE_USE			0x00000008		// can be used by the player
#define		FCAP_CONTINUOUS_USE			0x00000010		// can be used by the player
#define		FCAP_ONOFF_USE				0x00000020		// can be used by the player
#define		FCAP_DIRECTIONAL_USE		0x00000040		// Player sends +/- 1 when using (currently only tracktrains)
#define		FCAP_MASTER					0x00000080		// Can be used to "master" other entities (like multisource)
#define		FCAP_ONLYDIRECT_USE			0x00000100		// XDM: this entity can be used only if it's visible
// UNDONE: This will ignore transition volumes (trigger_transition), but not the PVS!!!
#define		FCAP_FORCE_TRANSITION		0x00000200		// ALWAYS goes across transitions

// XDM: absolute values
#define MAX_ABS_ORIGIN		8192
//#define MAX_ABS_VELOCITY	65536

#define SKF_NOTEASY				(1<<0)
#define SKF_NOTMEDIUM			(1<<1)
#define SKF_NOTHARD				(1<<2)


#ifndef SAVERESTORE_H
#include "saverestore.h"
#endif

#ifndef SCHEDULE_H
#include "schedule.h"
#endif

#ifndef MONSTEREVENT_H
#include "monsterevent.h"
#endif

#include "damage.h"
#include "studio.h"


// C functions for external declarations that call the appropriate C++ methods
extern int DispatchSpawn(edict_t *pent);
extern void DispatchKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd);
extern void DispatchTouch(edict_t *pentTouched, edict_t *pentOther);
extern void DispatchUse(edict_t *pentUsed, edict_t *pentOther);
extern void DispatchThink(edict_t *pent);
extern void DispatchBlocked(edict_t *pentBlocked, edict_t *pentOther);
extern void DispatchSave(edict_t *pent, SAVERESTOREDATA *pSaveData);
extern int  DispatchRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity);
extern void	DispatchObjectCollisionBox(edict_t *pent);
extern void SaveWriteFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);
extern void SaveReadFields(SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount);
extern void SaveGlobalState(SAVERESTOREDATA *pSaveData);
extern void RestoreGlobalState(SAVERESTOREDATA *pSaveData);
extern void ResetGlobalState(void);

/*
typedef enum { USE_OFF = 0, USE_ON = 1, USE_SET = 2, USE_TOGGLE = 3 } USE_TYPE;
*/
typedef enum
{
	USE_OFF = 0,
	USE_ON = 1,
	USE_SET = 2,
	USE_TOGGLE = 3,
	USE_KILL = 4,
// special signals, never actually get sent:
	USE_SAME = 5,
	USE_NOT = 6,
} USE_TYPE;

// the values used for the new "global states" mechanism.
typedef enum
{
	STATE_OFF = 0,	// disabled, inactive, invisible, closed, or stateless. Or non-alert monster.
	STATE_TURN_ON,  // door opening, env_fade fading in, etc.
	STATE_ON,		// enabled, active, visisble, or open. Or alert monster.
	STATE_TURN_OFF, // door closing, monster dying (?).
	STATE_IN_USE,	// player is in control (train/tank/barney/scientist).
					// In_Use isn't very useful, I'll probably remove it.
} STATE;

// Things that toggle (buttons/triggers/doors) need this
typedef enum
{
	TS_AT_TOP,
	TS_AT_BOTTOM,
	TS_GOING_UP,
	TS_GOING_DOWN
} TOGGLE_STATE;

// For Classify() IRelationship() g_iRelationshipTable
typedef enum
{
	CLASS_NONE = 0,
	CLASS_MACHINE,
	CLASS_PLAYER,
	CLASS_HUMAN_PASSIVE,
	CLASS_HUMAN_MILITARY,
	CLASS_ALIEN_MILITARY,
	CLASS_ALIEN_PASSIVE,
	CLASS_ALIEN_MONSTER,
	CLASS_ALIEN_PREY,
	CLASS_ALIEN_PREDATOR,
	CLASS_INSECT,
	CLASS_PLAYER_ALLY,
	CLASS_PLAYER_BIOWEAPON, // hornets and snarks.launched by players
	CLASS_ALIEN_BIOWEAPON, // hornets and snarks.launched by the alien menace
	CLASS_GRENADE, // XDM: dangerous
	CLASS_GIB, // XDM: om-nom-nom
	CLASS_BARNACLE// = 99 // special because no one pays attention to it, and it eats a wide cross-section of creatures.
} entity_class;

#define SF_NOTREAL		(1 << 29)// XDM3035 this entity does not have a world reference (a weapon that was not picked up but added by game rules, etc. or a monster created by trigger)
#define	SF_NORESPAWN	(1 << 30)// !!!set this bit on guns and stuff that should never respawn.

class CBaseEntity;
class CBaseMonster;
class CBasePlayer;
class CBasePlayerItem;
class CBaseAlias;// SHL
class CSquadMonster;

typedef void (CBaseEntity::*BASEPTR)(void);
typedef void (CBaseEntity::*ENTITYFUNCPTR)(CBaseEntity *pOther);
typedef void (CBaseEntity::*USEPTR)(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
//typedef void (CBaseEntity::*MOVEDONEPTR)(int direction);


//
// Converts a entvars_t * to a class pointer
// It will allocate the class and entity if necessary
//
template <class T> T *GetClassPtr(T *a, const char *szName = NULL)// XDM
{
	entvars_t *pev = (entvars_t *)a;

	// allocate entity if necessary
	if (pev == NULL)
	{
		if (szName == NULL)
			pev = VARS(CREATE_ENTITY());
		else
			pev = VARS(CREATE_NAMED_ENTITY(MAKE_STRING(szName)));
	}

	pev->scale = 1.0f;// XDM3035b
	// don't UTIL_FixRenderColor now, render mode is not set yet.

	// get the private data
	a = (T *)GET_PRIVATE(ENT(pev));

	if (a == NULL)
	{
		// allocate private data
		a = new(pev) T;
		a->pev = pev;
	}
	return a;
}


//
// EHANDLE. Safe way to point to CBaseEntities who may die between frames
//
class EHANDLE
{
private:
	edict_t *m_pent;
	int		m_serialnumber;
public:
	EHANDLE();// XDM3035
	edict_t *Get(void);
	edict_t *Set(edict_t *pent);

	operator int();
	operator CBaseEntity *();

	CBaseEntity *operator = (CBaseEntity *pEntity);
	CBaseEntity *operator ->();
};


//
// Base Entity.  All entity types derive from this
//
void FX_Trail( Vector origin, int EntIndex, int type );
void FX_FireBeam( Vector origin, Vector angles, Vector normal, int Surface, int Type, BOOL IsWorldBrush);
void FX_FireGun( Vector origin, int EntIndex, int Animation, int Type, int AltFire = 0);
void FX_FireLgtng(Vector pos, Vector end, int EntIndex, int Type);
void FX_WpnIcon(edict_t *ent, int FBAmmo, int TripAmmo, int SpiderAmmo, int MortarAmmo, int SatAmmo, int AtomAmmo);

class CBaseEntity
{
public:
	// Constructor.  Set engine to use C/C++ callback functions
	CBaseEntity();// XDM3035c: the only way to pre-set variables before KeyValue() takes place
//	virtual ~CBaseEntity();

	// pointers to engine data
	entvars_t *pev;		// Don't need to save/restore this pointer, the engine resets it
	// path corners
	CBaseEntity *m_pGoalEnt;// path corner we are heading towards
	CBaseEntity *m_pLink;// used for temporary link-list operations.

#if defined(MOVEWITH)// if this bunch of hacks is desired
	CBaseEntity *m_pMoveWith;// XDM
	CBaseEntity *m_pChildMoveWith;// SHL: one of the entities that's moving with me
	CBaseEntity	*m_pSiblingMoveWith;// SHL: another entity that's Moving With the same ent as me. (linked list.)
	CBaseEntity *m_pAssistLink;// SHL: link to the next entity which needs to be Assisted before physics are applied
	string_t	m_iszMoveWith;
//	Vector		m_vecMoveOriginDelta;

	Vector		m_vecPostAssistVel;// SHL
	Vector		m_vecPostAssistAVel;// SHL
	Vector		m_vecPostAssistOrg;// SHL: child postorigin
	Vector		m_vecPostAssistAng;// SHL: child postangles

	Vector		m_vecOffsetOrigin;	// spawn offset origin
	Vector		m_vecOffsetAngles;	// spawn offset angles
	Vector		m_vecParentOrigin;	// temp container
	Vector		m_vecParentAngles;	// temp container

	float		m_fNextThink;// SHL: for SetNextThink and SetPhysThink. Marks the time when a think will be performed - not necessarily the same as pev->nextthink!
	float		m_fPevNextThink;// SHL: always set equal to pev->nextthink, so that we can tell when the latter gets changed by the @#$^¬! engine.
	Vector		m_vecSpawnOffset;// SHL: To fix things which (for example) MoveWith a door which Starts Open.
	BOOL		m_activated;// SHL:- moved here from func_train. Signifies that an entity has already been activated. (and hence doesn't need reactivating.)
#endif // MOVEWITH
	int			m_iLFlags;

	int			m_iSkill;// XDM
	string_t	m_iszGameRulesPolicy;// XDM3035c: a special set of game rules IDs at which this entity is allowed to spawn
	string_t	m_iszIcon;// XDM3035c: overview/minimap icon
	float		m_flBurnTime;// XDM
	Vector		m_vecSpawnSpot;// XDM3035: first map spawn spot (where put by map editor)

	// fundamental callbacks
	BASEPTR			m_pfnThink;
	ENTITYFUNCPTR	m_pfnTouch;
	USEPTR			m_pfnUse;
	ENTITYFUNCPTR	m_pfnBlocked;

	// initialization functions
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Spawn(byte restore);
	virtual void Precache(void);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual void OnFreePrivateData(void);// XDM3035
	virtual void PrepareForTransfer(void);// XDM3037
	virtual int ObjectCaps(void);
	virtual void Activate(void) {};
	virtual void PostSpawn(void) {};// SHL
//	virtual void InitMoveWith(void);
	virtual int ShouldCollide(CBaseEntity *pOther);// XDM3035
	virtual bool ShouldBeSentTo(CBasePlayer *pClient) { return true; };// XDM3035c
	// Setup the object->object collision box (pev->mins / pev->maxs is the object->world collision box)
	virtual void SetObjectCollisionBox(void);
	virtual int SendClientData(CBasePlayer *pClient, int msgtype) { return 0; };// XDM3035: return value 0 means no data were sent
	virtual void DesiredAction(void) {};// XDM3037

	virtual void UpdateOnRemove(void);// XDM3034 virtual, needed by things like satchels
	virtual void Disintegrate(void);// XDM3035
	virtual void AnnihilateProj(void);
	virtual STATE GetState(void) { return STATE_OFF; };
	virtual	int GetToggleState(void) { return TS_AT_TOP; };
	virtual void SetToggleState(int state) {};// This is ONLY used by the node graph to test movement through a door
	virtual int ShouldToggle(USE_TYPE useType, bool currentState);

	virtual int Classify(void) { return CLASS_NONE; };
	virtual void DeathNotice(CBaseEntity *pChild) {};// monster maker children use this to tell the monster maker that they have died.
// XDM3034: replaced old shitty functions with brand new
	virtual void TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual int TakeHealth(const float &flHealth, const int &bitsDamageType);
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);

	virtual int BloodColor(void) { return DONT_BLEED; };
	virtual void TraceBleed(const float &flDamage, const Vector &vecDir, TraceResult *ptr, const int &bitsDamageType);

	virtual float DamageForce(const float &damage);// XDM3035

	// Do the bounding boxes of these two intersect?
	virtual int Intersects(CBaseEntity *pOther);
	virtual void MakeDormant(void);

	virtual float GetDelay(void) { return 0; };
	virtual void OverrideReset(void) { };
	virtual int DamageDecal(const int &bitsDamageType);
	virtual BOOL OnControls(entvars_t *onpev) { return FALSE; };
	virtual BOOL IsTriggered(CBaseEntity *pActivator) { return TRUE; };
	virtual BOOL IsDormant(void);
	virtual BOOL IsLockedByMaster(void)	{ return FALSE; };
	virtual BOOL IsMoving(void)			{ return pev->velocity != g_vecZero; };
	virtual BOOL IsAlive(void)			{ return (pev->deadflag == DEAD_NO) && pev->health > 0.0f; };
	virtual BOOL IsBSPModel(void)		{ return pev->solid == SOLID_BSP || pev->movetype == MOVETYPE_PUSHSTEP; };
	virtual BOOL IsInWorld(void);
	virtual	BOOL IsPlayer(void);
	virtual BOOL IsNetClient(void)		{ return FALSE; };
	virtual	BOOL IsMonster(void)		{ return FALSE; };// XDM
	virtual BOOL IsMovingBSP(void)		{ return FALSE; };// XDM
	virtual BOOL IsProjectile(void)		{ return FALSE; };// XDM
	virtual BOOL IsPushable(void)		{ return FALSE; };// XDM
	virtual BOOL IsHuman(void)			{ return FALSE; };// XDM
	virtual BOOL IsBot(void)			{ return FALSE; };// XDM
	virtual BOOL IsBreakable(void)		{ return FALSE; };// XDM
	virtual BOOL IsPlayerItem(void)		{ return FALSE; };// XDM3035
	virtual BOOL IsPlayerWeapon(void)	{ return FALSE; };// XDM3035
	virtual BOOL IsTrigger(void)		{ return FALSE; };// XDM3035a
	virtual BOOL IsAlias(void)			{ return FALSE; };// SHL
	virtual BOOL IsGameGoal(void)		{ return FALSE; };// XDM3036
	virtual bool IsPickup(void)			{ return false; };// XDM3037

	virtual BOOL HasTarget(string_t targetname);
	virtual BOOL ShouldRespawn(void) { return FALSE; };// XDM3035
//	virtual void SetActivator(CBaseEntity *pActivator) { };
	virtual CBaseEntity *GetNextTarget(void);

	virtual CBaseEntity *Respawn(void);

	virtual BOOL AddPlayerItem(CBasePlayerItem *pItem) { return 0; };
	virtual BOOL RemovePlayerItem(CBasePlayerItem *pItem) { return 0; };
	virtual int GiveAmmo(const int &iAmount, const int &iIndex, const int &iMax) { return -1; };
	virtual int GiveAmmo(const int &iAmount, char *szName, const int &iMax) { return -1; };

	virtual BOOL FBecomeProne(void) { return FALSE; };
	virtual void SetModelCollisionBox(void);// XDM
	virtual void AlignToFloor(void);// XDM
	virtual void CheckEnvironment(void);// XDM3035b
	virtual int Illumination(void) { return GETENTITYILLUM(ENT(pev)); };

	virtual Vector Center(void) { return (pev->absmax + pev->absmin) * 0.5f; }; // center point of entity
	virtual Vector EyePosition(void) { return (pev->origin + pev->view_ofs); };	// position of eyes
	virtual Vector EarPosition(void) { return (pev->origin + pev->view_ofs); };	// position of ears
	// SHL stuff
	virtual Vector BodyTarget(const Vector &posSrc)		{ return Center(); };		// position to shoot at
	virtual Vector CalcPosition(CBaseEntity *pLocus)	{ return pev->origin; };
	virtual Vector CalcVelocity(CBaseEntity *pLocus)	{ return pev->velocity; };
	virtual float CalcRatio(CBaseEntity *pLocus)		{ return 0.0f; };

	virtual	bool FVisible(const Vector &vecOrigin);
	virtual	bool FVisible(CBaseEntity *pEntity);
	virtual bool FBoxVisible(CBaseEntity *pTarget, Vector &vecTargetOrigin, float flSize);// vecTargetOrigin is output

	virtual void LinearMove(const Vector &vecDest, const float &flSpeed) { };

	virtual void Think(void) { if (m_pfnThink) (this->*m_pfnThink)(); };
	virtual void Touch(CBaseEntity *pOther) { if (m_pfnTouch) (this->*m_pfnTouch)(pOther); };
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) { if (m_pfnUse) (this->*m_pfnUse)(pActivator, pCaller, useType, value); };
	virtual void Blocked(CBaseEntity *pOther) { if (m_pfnBlocked) (this->*m_pfnBlocked)(pOther); };

	virtual CBaseMonster *MyMonsterPointer(void) { return NULL; };
	virtual CSquadMonster *MySquadMonsterPointer(void) { return NULL; };

	// common member functions
	virtual void SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value);
	void EXPORT SUB_Remove(void);
	void EXPORT SUB_DoNothing(void);
	void EXPORT SUB_StartFadeOut(void);
	void EXPORT SUB_FadeOut(void);
	void EXPORT SUB_Disintegrate(void);// XDM3035: don't call or SetThink to this directly, use Disintegrate() instead
	void EXPORT SUB_MoveWith(void);// XDM
	void EXPORT SUB_CallUseToggle(void);// XDM3035c: don't export from headers
	void EXPORT SUB_Respawn(void);// XDM3035

	void DontThink(void);// XDM3037
	void SetNextThink(const float &delay);// XDM: for those, who like this

	CBaseMonster *GetMonsterPointer(edict_t *pentMonster)
	{
		CBaseEntity *pEntity = Instance(pentMonster);
		if (pEntity)
			return pEntity->MyMonsterPointer();
		return NULL;
	};

	edict_t *edict(void) { return ENT(pev); };
	EOFFSET eoffset(void) { return OFFSET(pev); };
	int entindex(void) { return ENTINDEX(edict()); };


	// allow engine to allocate instance data
    void *operator new(size_t stAllocateBlock, entvars_t *newpev)
	{
		return (void *)ALLOC_PRIVATE(ENT(newpev), stAllocateBlock);
	};

	// don't use this.
#if defined(_MSC_VER) && _MSC_VER >= 1200 // only build this code if MSVC++ 6.0 or higher
	void operator delete(void *pMem, entvars_t *pev)
	{
		pev->flags |= FL_KILLME;
	};
#endif

/*	inline void SetThink(BASEPTR a)
	{
		m_pfnThink = reinterpret_cast <BASEPTR> (a);
	};
	inline void SetTouch(ENTITYFUNCPTR a)
	{
		m_pfnTouch = reinterpret_cast <ENTITYFUNCPTR> (a);
	};
	inline void SetUse(USEPTR a)
	{
		m_pfnUse = reinterpret_cast <USEPTR> (a);
	};
	inline void SetBlocked(ENTITYFUNCPTR a)
	{
		m_pfnBlocked = reinterpret_cast <ENTITYFUNCPTR> (a);
	};*/
	inline void SetThinkNull(void)// no effect on overloaded Think()
	{
		m_pfnThink = NULL;
	};
	inline void SetTouchNull(void)// no effect on overloaded Touch()
	{
		m_pfnTouch = NULL;
	};
	inline void SetUseNull(void)// no effect on overloaded Use()
	{
		m_pfnUse = NULL;
	};
	inline void SetBlockedNull(void)// no effect on overloaded Blocke()
	{
		m_pfnBlocked = NULL;
	};

	// Ugly code to lookup all functions to make sure they are exported when set.
#ifdef _DEBUG
	void FunctionCheck(void *pFunction, char *name)
	{
		if (pFunction && !NAME_FOR_FUNCTION((unsigned long)(pFunction)))
			ALERT(at_error, "No EXPORT: %s:%s (%08lx)\n", STRING(pev->classname), name, (unsigned long)pFunction);
	};

	BASEPTR ThinkSet(BASEPTR func, char *name)
	{
		m_pfnThink = func;
		FunctionCheck((void *)*((int *)((char *)this + (offsetof(CBaseEntity,m_pfnThink)))), name);
		return func;
	};
	ENTITYFUNCPTR TouchSet(ENTITYFUNCPTR func, char *name)
	{
		m_pfnTouch = func;
		FunctionCheck((void *)*((int *)((char *)this + (offsetof(CBaseEntity,m_pfnTouch)))), name);
		return func;
	};
	USEPTR UseSet(USEPTR func, char *name)
	{
		m_pfnUse = func;
		FunctionCheck((void *)*((int *)((char *)this + (offsetof(CBaseEntity,m_pfnUse)))), name);
		return func;
	};
	ENTITYFUNCPTR BlockedSet(ENTITYFUNCPTR func, char *name)
	{
		m_pfnBlocked = func;
		FunctionCheck((void *)*((int *)((char *)this + (offsetof(CBaseEntity,m_pfnBlocked)))), name);
		return func;
	};
#endif

	static CBaseEntity *Create(const char *szName, const Vector &vecOrigin, const Vector &vecAngles, edict_t *pentOwner = NULL);// XDM
	static CBaseEntity *Create(const char *szName, const Vector &vecOrigin, const Vector &vecAngles, const Vector &vecVeloity, edict_t *pentOwner = NULL);
	static CBaseEntity *Create(int iName, const Vector &vecOrigin, const Vector &vecAngles, const Vector &vecVeloity, edict_t *pentOwner = NULL, int spawnflags = 0);// XDM3035: we really NEED this!
	static CBaseEntity *CreateCopy(int iName, entvars_t *pSourceVars, int spawnflags, bool spawn = true);// XDM3035a: provide a source entvars to be copied to a new entity
	static CBaseEntity *Instance(edict_t *pent);
	static CBaseEntity *Instance(entvars_t *instpev) { return Instance(ENT(instpev)); };

	static TYPEDESCRIPTION m_SaveData[];
};

/*
template <typename To, typename From>

inline To unsafe_union_cast(From from)
{
	struct union_cast_holder
	{
		union
		{
			From from;
			To to;
		};
	};
	ASSERT(sizeof(From) == sizeof(To) && sizeof(From) == sizeof(union_cast_holder));
	union_cast_holder holder;
	holder.from = from;
	return holder.to;
}
*/

// Ugly technique to override base member functions
// Normally it's illegal to cast a pointer to a member function of a derived class to a pointer to a
// member function of a base class.  static_cast is a sleezy way around that problem.
#if _MSC_VER >= 1200 // only build this code if MSVC++ 6.0 or higher

#ifdef _DEBUG

#define SetThink(a) ThinkSet(static_cast <BASEPTR>(a), #a)
#define SetTouch(a) TouchSet(static_cast <ENTITYFUNCPTR>(a), #a)
#define SetUse(a) UseSet(static_cast <USEPTR> (a), #a)
#define SetBlocked(a) BlockedSet(static_cast <ENTITYFUNCPTR>(a), #a)
#define SetMoveDone(a) m_pfnCallWhenMoveDone = static_cast <BASEPTR>(a)

#else

#define SetThink(a) m_pfnThink = static_cast <BASEPTR>(a)
#define SetTouch(a) m_pfnTouch = static_cast <ENTITYFUNCPTR>(a)
#define SetUse(a) m_pfnUse = static_cast <USEPTR>(a)
#define SetBlocked(a) m_pfnBlocked = static_cast <ENTITYFUNCPTR>(a)
#define SetMoveDone(a) m_pfnCallWhenMoveDone = static_cast <BASEPTR>(a)

#endif// _DEBUG

#else// _MSC_VER

#define SetThink(a) m_pfnThink = static_cast <BASEPTR>(a)
#define SetTouch(a) m_pfnTouch = static_cast <ENTITYFUNCPTR>(a)
#define SetUse(a) m_pfnUse = static_cast <USEPTR>(a)
#define SetBlocked(a) m_pfnBlocked = static_cast <ENTITYFUNCPTR>(a)
#define SetMoveDone(a) m_pfnCallWhenMoveDone = static_cast <BASEPTR>(a)

#endif// _MSC_VER

/* the original
#define SetThink(a) m_pfnThink = static_cast <void (CBaseEntity::*)(void)> (a)
#define SetTouch(a) m_pfnTouch = static_cast <void (CBaseEntity::*)(CBaseEntity *)> (a)
#define SetUse(a) m_pfnUse = static_cast <void (CBaseEntity::*)(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)> (a)
#define SetBlocked(a) m_pfnBlocked = static_cast <void (CBaseEntity::*)(CBaseEntity *)> (a)
*/





class CBaseDelay : public CBaseEntity
{
public:
	CBaseDelay();// XDM3035c: the only way to pre-set variables before KeyValue() takes place
//	virtual ~CBaseDelay();

	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);// XDM3035c
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual void SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value);
	virtual STATE GetState(void) { return m_iState; };
	virtual void SetState(STATE newstate);
	virtual void OnStateChange(STATE oldstate);
	virtual BOOL IsLockedByMaster(void);

	void EXPORT DelayThink(void);

	float		m_flDelay;
	int			m_iszKillTarget;
	string_t	m_iszMaster;// If this button has a master switch, this is the targetname.
							// A master switch must be of the multisource type. If all
							// of the switches in the multisource have been triggered, then
							// the button will be allowed to operate. Otherwise, it will be
							// deactivated.
	EHANDLE		m_hActivator;// XDM3035

private:
	STATE		m_iState;// XDM3035c: SHL

	static TYPEDESCRIPTION m_SaveData[];
};




class CBaseAnimating : public CBaseDelay
{
public:
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual void Spawn(void);// XDM
	virtual void HandleAnimEvent(MonsterEvent_t *pEvent);

	// Basic Monster Animation functions
	float StudioFrameAdvance(float flInterval = 0.0f); // accumulate animation frame time from last time called until now
	int	GetSequenceFlags(void);
	int LookupActivity(int activity);
	int LookupActivityHeaviest(int activity);
	int LookupSequence(const char *label);
	void ResetSequenceInfo(void);
	void DispatchAnimEvents(float flFutureInterval = 0.1); // Handle events that have happend since last time called up until X seconds into the future
	float SetBoneController(int iController, float flValue);
	void InitBoneControllers(void);
	float SetBlending(byte iBlender, float flValue);
	void GetBonePosition(int iBone, Vector &origin, Vector &angles);
	void GetAutomovement(Vector &origin, Vector &angles, float flInterval = 0.1);
	int  FindTransition(int iEndingSequence, int iGoalSequence, int *piDir);
	void GetAttachment(int iAttachment, Vector &origin, Vector &angles);
	void SetBodygroup(int iGroup, int iValue);
	int GetBodygroup(int iGroup);
	int ExtractBbox(int sequence, Vector &mins, Vector &maxs);
	void SetSequenceBox(void);
	void UpdateFrame(void);// XDM

	// animation needs
	float		m_flFrameRate;		// computed FPS for current sequence
	float		m_flGroundSpeed;	// computed linear movement rate for current sequence
	float		m_flLastEventCheck;	// last time the event list was checked
	BOOL		m_fSequenceFinished;// flag set when StudioAdvanceFrame moves across a frame boundry
	BOOL		m_fSequenceLoops;	// true if the sequence loops
	int			m_nFrames;// XDM: sprites

	static TYPEDESCRIPTION m_SaveData[];
};




//
// generic Toggle entity.
//
#define	SF_ITEM_USE_ONLY	256 //  ITEM_USE_ONLY = BUTTON_USE_ONLY = DOOR_USE_ONLY!!!

class CBaseToggle : public CBaseAnimating
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int GetToggleState(void) { return m_toggle_state; };
	virtual float GetDelay(void) { return m_flWait; };

	// common member functions
	virtual void LinearMove(const Vector &vecDest, const float &flSpeed);
	virtual void AngularMove(const Vector &vecDestAngles, const float &flSpeed);
//	virtual void InterpolatedMove(Vector vecDest, float flStartSpeed, float flEndSpeed);// XDM

	void EXPORT LinearMoveDone(void);
	void EXPORT AngularMoveDone(void);
//	void EXPORT InterMove(void);// XDM

	void AxisDir(void);

	static float AxisValue(int flags, const Vector &angles);
	static float AxisDelta(int flags, const Vector &angle1, const Vector &angle2);


	TOGGLE_STATE		m_toggle_state;
	float				m_flActivateFinished;//like attack_finished, but for doors
	float				m_flMoveDistance;// how far a door should slide or rotate
	float				m_flWait;
	float				m_flLip;
	float				m_flTWidth;// for plats
	float				m_flTLength;// for plats

	Vector				m_vecPosition1;
	Vector				m_vecPosition2;
	Vector				m_vecAngle1;
	Vector				m_vecAngle2;

	int					m_cTriggersLeft;		// trigger_counter only, # of activations remaining
	float				m_flHeight;
	BASEPTR				m_pfnCallWhenMoveDone;// XDM3035
	Vector				m_vecFinalDest;
	Vector				m_vecFinalAngle;

	int					m_bitsDamageInflict;	// DMG_ damage type that the door or tigger does

	static TYPEDESCRIPTION m_SaveData[];
};




class CPointEntity : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual int	ObjectCaps(void) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; };
};


// this moved here from world.cpp, to allow classes to be derived from it
//=======================
// CWorld
//
// This spawns first when each level begins.
//=======================
class CWorld : public CBaseEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual int ObjectCaps(void) { return FCAP_MUST_SPAWN; };// XDM3034: Calls CWorld::Spawn() after loading the game, making g_pWorld != NULL
	virtual int SendClientData(CBasePlayer *pClient, int msgtype);// XDM3035c
	virtual void OnFreePrivateData(void);// XDM3036
	virtual BOOL IsBSPModel(void) { return TRUE; };// XDM
	virtual BOOL IsMovingBSP(void) { return FALSE; };

	CBaseAlias *m_pFirstAlias;// SHL
};


void FireTargets(const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
void SetObjectCollisionBox(entvars_t *pev);// XDM

// XDM: can't move these to util.h becuase of types used
char *GetStringForUseType(USE_TYPE useType);
const char *GetStringForState(STATE state);// XDM3035c
STATE GetStateForString(const char *string);// XDM3035c


#endif // CBASE_H
