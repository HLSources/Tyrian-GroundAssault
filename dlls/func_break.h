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
#ifndef FUNC_BREAK_H
#define FUNC_BREAK_H


// func_breakable
#define SF_BREAK_TRIGGER_ONLY	1	// may only be broken by trigger
#define	SF_BREAK_TOUCH			2	// can be 'crashed through' by running player (plate glass)
#define SF_BREAK_PRESSURE		4	// can be broken by a player standing on it
#define SF_BREAK_FADE_RESPAWN	8	// XDM: fades in gradually when respawned
#define SF_BREAK_CROWBAR		256	// instant break if hit with crowbar


#define EXPLOSION_RANDOM	0
#define EXPLOSION_DIRECTED	1

#define	NUM_SHARDS 6 // this many shards spawned when breakable objects break;

class CBreakable : public CBaseDelay
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual int ObjectCaps(void) { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION); }
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	// breakables use an overridden takedamage
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	// To spark when hit
	virtual void TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType);
	virtual BOOL IsBSPModel(void) { return TRUE; }// XDM
	virtual BOOL IsMovingBSP(void) { return FALSE; }// XDM
	virtual BOOL IsBreakable(void);
//	virtual BOOL SparkWhenHit(void);
	virtual BOOL Explodable(void);
	virtual int DamageDecal(const int &bitsDamageType);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	void DamageSound(void);

	void EXPORT BreakTouch(CBaseEntity *pOther);
	void EXPORT Die(void);
	void EXPORT ExistThink(void);

public:
	Materials	m_Material;
	int			m_iExplosion;
	int			m_idShard;
	float		m_angle;
	int			m_iszGibModel;
	int			m_iSpawnObject;// why hold string?

	int			m_iShards;
	int			m_iTrail;
	int			m_fShardSize;
	bool		m_bServerGibs;

	static	TYPEDESCRIPTION m_SaveData[];
	static const char *pSpawnObjects[];
};




// func_pushable (it's also func_breakable, so don't collide with those flags)
#define SF_PUSH_BREAKABLE		128
#define SF_PUSH_NOPULL			512// XDM

class CPushable : public CBreakable
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual int	ObjectCaps(void) { return (CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_CONTINUOUS_USE; }
	virtual void Touch(CBaseEntity *pOther);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual BOOL IsBSPModel(void) { return TRUE; }// XDM
	virtual BOOL IsMovingBSP(void) { return TRUE; }// XDM
	virtual BOOL IsPushable(void) { return TRUE; }// XDM

	inline float MaxSpeed(void) { return m_maxSpeed; }
	// breakables use an overridden takedamage
	void PlayMatPushSound(int m_lastSound);
	void StopMatPushSound(int m_lastSound);
	void Move(CBaseEntity *pMover, int push);

public:
	int m_lastSound;// no need to save/restore, just keeps the same sound from playing twice in a row
	float m_maxSpeed;
	float m_soundTime;
	static TYPEDESCRIPTION m_SaveData[];
};




// XDM: same as CBreakable, but allows studio model
class CBreakableModel : public CBreakable
{
public:
	virtual void Spawn(void);
};


#endif	// FUNC_BREAK_H
