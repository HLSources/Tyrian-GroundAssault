//====================================================================
//
// Purpose: trigger definitions
//
//====================================================================
#ifndef TRIGGERS_H
#define TRIGGERS_H
#ifdef _WIN32
#pragma once
#endif


class CFrictionModifier : public CBaseEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	void EXPORT	ChangeFriction(CBaseEntity *pOther);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int ObjectCaps(void) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	static TYPEDESCRIPTION m_SaveData[];
	float m_frictionFraction;// Sorry, couldn't resist this name :)
};



// This trigger will fire when the level spawns (or respawns if not fire once)
// It will check a global state before firing.  It supports delay and killtargets
#define SF_AUTO_FIREONCE		0x0001

class CTriggerAuto : public CBaseDelay
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Think(void);
	virtual int ObjectCaps(void) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

private:
	int m_globalstate;
	USE_TYPE triggerType;
};

class CFireAndDie : public CBaseDelay
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Think(void);
	virtual int ObjectCaps(void) { return CBaseDelay::ObjectCaps() | (FCAP_ACROSS_TRANSITION | FCAP_FORCE_TRANSITION); }	// Always go across transitions
};


#define SF_RELAY_FIREONCE		0x0001

class CTriggerRelay : public CBaseDelay
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int ObjectCaps(void) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

private:
	USE_TYPE triggerType;
};


// Flags to indicate masking off various render parameters that are normally copied to the targets
#define SF_RENDER_MASKFX	(1<<0)
#define SF_RENDER_MASKAMT	(1<<1)
#define SF_RENDER_MASKMODE	(1<<2)
#define SF_RENDER_MASKCOLOR	(1<<3)

class CRenderFxManager : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};


class CBaseTrigger : public CBaseToggle
{
public:
//	void EXPORT TeleportTouch ( CBaseEntity *pOther );
//	void EXPORT MultiTouch( CBaseEntity *pOther );
//	void EXPORT HurtTouch ( CBaseEntity *pOther );
//	void EXPORT CDAudioTouch ( CBaseEntity *pOther );
	virtual void KeyValue(KeyValueData *pkvd);
	virtual BOOL ShouldRespawn(void) { return FALSE; }// XDM3035
	virtual BOOL IsTrigger(void) { return TRUE; }// XDM3035a
	virtual int	ObjectCaps(void) { return CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	void ActivateMultiTrigger(CBaseEntity *pActivator);
	void InitTrigger(void);
	BOOL CanTouch(CBaseEntity *pToucher);// XDM
	void EXPORT MultiWaitOver(void);
	void EXPORT CounterUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT ToggleUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};


class CTriggerMonsterJump : public CBaseTrigger
{
public:
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);
	virtual void Think(void);
};


#define SF_TRIGGERCDAUDIO_LOOP		0x0001

class CTriggerCDAudio : public CBaseTrigger
{
public:
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual void Touch(CBaseEntity *pOther);
};


class CTargetCDAudio : public CPointEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual void Think(void);
};


#define SF_TRIGGER_HURT_TARGETONCE		1// Only fire hurt target once
#define	SF_TRIGGER_HURT_START_OFF		2
#define	SF_TRIGGER_HURT_NO_CLIENTS		8
#define SF_TRIGGER_HURT_CLIENTONLYFIRE	16// trigger hurt will only fire its target if it is hurting a client
#define SF_TRIGGER_HURT_CLIENTONLYTOUCH 32// only clients may touch this trigger.

class CTriggerHurt : public CBaseTrigger
{
public:
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);
	void EXPORT RadiationThink(void);
};

#define TRIGGER_ACTIVATE_VOLUME		128

class CTriggerMultiple : public CBaseTrigger
{
public:
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);// XDM
};

class CTriggerOnce : public CTriggerMultiple
{
public:
	virtual void Spawn(void);
};

class CTriggerCounter : public CBaseTrigger
{
public:
	virtual void Spawn(void);
};

class CTriggerVolume : public CBaseTrigger	// Derive from point entity so this doesn't move across levels
{
public:
	virtual void Spawn(void);
};



// We can only ever move 512 entities across a transition
#define TRANSITION_MAX_ENTS			512
#define SF_CHANGELEVEL_USEONLY		0x0002

class CChangeLevel : public CBaseTrigger
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual BOOL IsGameGoal(void);

	void EXPORT UseChangeLevel(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT TriggerChangeLevel(void);
	void EXPORT TouchChangeLevel(CBaseEntity *pOther);
	void ChangeLevelNow(CBaseEntity *pActivator);

	char m_szMapName[MAX_MAPNAME];		// trigger_changelevel only:  next map
	char m_szLandmarkName[MAX_MAPNAME];		// trigger_changelevel only:  landmark on next map
	int m_changeTarget;
	float m_changeTargetDelay;

	static edict_t *FindLandmark(const char *pLandmarkName);
	static int ChangeList(LEVELLIST *pLevelList, int maxList);
	static int AddTransitionToList(LEVELLIST *pLevelList, int listCount, const char *pMapName, const char *pLandmarkName, edict_t *pentLandmark);
//	static int InTransitionVolume(CBaseEntity *pEntity, char *pVolumeName );
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

};


class CLadder : public CBaseTrigger
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);// XDM3035a: toggle ladder functionality
};


#define	SF_TRIGGER_PUSH_START_OFF			2//spawnflag that makes trigger_push spawn turned OFF
#define	SF_TRIGGER_PUSH_ONLY_NOTONGROUND	4// XDM

class CTriggerPush : public CBaseTrigger
{
public:
//	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);
	// XDM
/*	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];
	int m_iszPushVel;
	int m_iszPushSpeed;*/
};


class CTriggerTeleport : public CBaseTrigger
{
public:
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);// XDM
};


class CTriggerSave : public CBaseTrigger
{
public:
//	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);// XDM
	virtual BOOL IsTriggered(CBaseEntity *pEntity);
//	void EXPORT SaveTouch( CBaseEntity *pOther );
};


#define SF_ENDSECTION_USEONLY		0x0001

class CTriggerEndSection : public CBaseTrigger
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	void EXPORT EndSectionTouch(CBaseEntity *pOther);
	void EXPORT EndSectionUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};


class CTriggerGravity : public CBaseTrigger
{
public:
	virtual void Spawn(void);
	void EXPORT GravityTouch(CBaseEntity *pOther);
};


class CTriggerPlayerFreeze : public CBaseDelay
{
public:
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int ObjectCaps(void) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};


class CTriggerChangeTarget : public CBaseDelay
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int ObjectCaps(void) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

private:
	int m_iszNewTarget;
};


#define SF_CAMERA_PLAYER_POSITION		1
#define SF_CAMERA_PLAYER_TARGET			2
#define SF_CAMERA_PLAYER_TAKECONTROL	4

class CTriggerCamera : public CBaseDelay
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual bool ShouldBeSentTo(CBasePlayer *pClient);// XDM3035c
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int	ObjectCaps(void) { return CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	void EXPORT FollowTarget(void);
	void Move(void);
	void Deactivate(void);

	EHANDLE m_hPlayer;
	EHANDLE m_hTarget;
	CBaseEntity *m_pentPath;
	int m_sPath;
	float m_flWait;
	float m_flReturnTime;
	float m_flStopTime;
	float m_moveDistance;
	float m_targetSpeed;
	float m_initialSpeed;
	float m_acceleration;
	float m_deceleration;
	int m_state;
	int	m_iszViewEntity;
	static TYPEDESCRIPTION m_SaveData[];
};


#define SF_BOUNCE_CUTOFF 16

class CTriggerBounce : public CBaseTrigger
{
public:
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);
};

#endif // TRIGGERS_H
