/***
*
*	Copyright(c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc.("Id Technology").  Id Technology(c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/

#ifndef BASEMONSTER_H
#define BASEMONSTER_H

typedef enum
{
	SCRIPT_PLAYING = 0,		// Playing the sequence
	SCRIPT_WAIT,			// Waiting on everyone in the script to be ready
	SCRIPT_CLEANUP,			// Cancelling the script / cleaning up
	SCRIPT_WALK_TO_MARK,
	SCRIPT_RUN_TO_MARK,
} SCRIPTSTATE;

typedef enum
{
	MONSTERSTATE_NONE = 0,
	MONSTERSTATE_IDLE,
	MONSTERSTATE_COMBAT,
	MONSTERSTATE_ALERT,
	MONSTERSTATE_HUNT,
	MONSTERSTATE_PRONE,
	MONSTERSTATE_SCRIPT,
	MONSTERSTATE_PLAYDEAD,
	MONSTERSTATE_DEAD
} MONSTERSTATE;

#define	ROUTE_SIZE			8 // how many waypoints a monster can store at one time
#define MAX_OLD_ENEMIES		4 // how many old enemies to remember

#define	bits_CAP_DUCK			( 1 << 0 )// crouch
#define	bits_CAP_JUMP			( 1 << 1 )// jump/leap
#define bits_CAP_STRAFE			( 1 << 2 )// strafe ( walk/run sideways)
#define bits_CAP_SQUAD			( 1 << 3 )// can form squads
#define	bits_CAP_SWIM			( 1 << 4 )// proficiently navigate in water
#define bits_CAP_CLIMB			( 1 << 5 )// climb ladders/ropes
#define bits_CAP_USE			( 1 << 6 )// open doors/push buttons/pull levers
#define bits_CAP_HEAR			( 1 << 7 )// can hear forced sounds
#define bits_CAP_AUTO_DOORS		( 1 << 8 )// can trigger auto doors
#define bits_CAP_OPEN_DOORS		( 1 << 9 )// can open manual doors
#define bits_CAP_TURN_HEAD		( 1 << 10)// can turn head, always bone controller 0
#define bits_CAP_RANGE_ATTACK1	( 1 << 11)// can do a range attack 1
#define bits_CAP_RANGE_ATTACK2	( 1 << 12)// can do a range attack 2
#define bits_CAP_MELEE_ATTACK1	( 1 << 13)// can do a melee attack 1
#define bits_CAP_MELEE_ATTACK2	( 1 << 14)// can do a melee attack 2
#define bits_CAP_FLY			( 1 << 15)// can fly, move all around

#define bits_CAP_DOORS_GROUP    (bits_CAP_USE | bits_CAP_AUTO_DOORS | bits_CAP_OPEN_DOORS)


class CCineMonster;
class CSound;

//
// generic Monster
//
class CBaseMonster : public CBaseToggle
{
public:
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];
	virtual void KeyValue(KeyValueData *pkvd);

// monster use function
	void EXPORT MonsterUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
//	void EXPORT CorpseUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

// overrideable Monster member functions
	virtual int Classify(void) { return m_iClass ? m_iClass:CLASS_NONE; }
	virtual int BloodColor(void) { return m_bloodColor; }

	virtual CBaseMonster *MyMonsterPointer(void) { return this; }
	virtual void Look(int iDistance);// basic sight function for monsters
	virtual void RunAI(void);// core ai function!
	void Listen(void);

	virtual BOOL IsAlive(void) { return(pev->deadflag != DEAD_DEAD); }
	virtual BOOL ShouldFadeOnDeath(void);

// Basic Monster AI functions
	virtual void SetYawSpeed(void) { };// allows different yaw_speeds for each activity
	virtual float ChangeYaw(float yawSpeed);
	float VecToYaw(const Vector &vecDir);
	float FlYawDiff(void);
//	virtual float DamageForce(float damage);

// stuff written for new state machine
	virtual void MonsterThink(void);
	void EXPORT	CallMonsterThink(void);
	virtual int IRelationship(CBaseEntity *pTarget);
	virtual void MonsterInit(void);
	virtual void MonsterInitDead(void);	// Call after animation/pose is set up
	virtual void BecomeDead(void);

	void EXPORT CorpseFallThink(void);
	void EXPORT MonsterInitThink(void);

	virtual void StartMonster(void);
	virtual CBaseEntity* BestVisibleEnemy(void);// finds best visible enemy for attack
	virtual BOOL FInViewCone(CBaseEntity *pEntity);// see if pEntity is in monster's view cone
	virtual BOOL FInViewCone(const Vector &origin);// see if given location is in monster's view cone
	virtual void HandleAnimEvent(MonsterEvent_t *pEvent);

	virtual int CheckLocalMove(const Vector &vecStart, const Vector &vecEnd, CBaseEntity *pTarget, float *pflDist);// check validity of a straight move through space
	virtual void Move(float flInterval = 0.1);
	virtual void MoveExecute(CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval);
	virtual BOOL ShouldAdvanceRoute(float flWaypointDist);

	virtual Activity GetStoppedActivity(void) { return ACT_IDLE; }
	virtual void Stop(void) { m_IdealActivity = GetStoppedActivity(); }

	// This will stop animation until you call ResetSequenceInfo() at some point in the future
	inline void StopAnimation(void) { pev->framerate = 0; }

	// these functions will survey conditions and set appropriate conditions bits for attack types.
	virtual BOOL CheckRangeAttack1(float flDot, float flDist);
	virtual BOOL CheckRangeAttack2(float flDot, float flDist);
	virtual BOOL CheckMeleeAttack1(float flDot, float flDist);
	virtual BOOL CheckMeleeAttack2(float flDot, float flDist);

	virtual void StartTask(Task_t *pTask);
	virtual void RunTask(Task_t *pTask);
	virtual Schedule_t *GetScheduleOfType(int Type);
	virtual Schedule_t *GetSchedule(void);
	virtual void ScheduleChange(void) {}
	// virtual int CanPlaySequence(void) { return((m_pCine == NULL) &&(m_MonsterState == MONSTERSTATE_NONE || m_MonsterState == MONSTERSTATE_IDLE || m_IdealMonsterState == MONSTERSTATE_IDLE)); }
	virtual int CanPlaySequence(BOOL fDisregardState, int interruptLevel);
	virtual int CanPlaySentence(BOOL fDisregardState) { return IsAlive(); }
	virtual void PlaySentence(const char *pszSentence, float duration, float volume, float attenuation);
	virtual void PlayScriptedSentence(const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener);
	virtual void SentenceStop(void);

	BOOL FHaveSchedule(void);
	BOOL FScheduleValid(void);
	void ClearSchedule(void);
	BOOL FScheduleDone(void);
	void ChangeSchedule(Schedule_t *pNewSchedule);
	void NextScheduledTask(void);
	Schedule_t *ScheduleInList(const char *pName, Schedule_t **pList, int listCount);

	virtual Schedule_t *ScheduleFromName(const char *pName);
	static Schedule_t *m_scheduleList[];

	void MaintainSchedule(void);
	Task_t *GetTask(void);
	virtual MONSTERSTATE GetIdealState(void);
	virtual void SetActivity(Activity NewActivity);
	virtual void ReportAIState(void);

	void SetSequenceByName(char *szSequence);
	void SetMonsterState(MONSTERSTATE State);// XDM3035c: name confusion
	void CheckAttacks(CBaseEntity *pTarget, const float &flDist);

	virtual int CheckEnemy(CBaseEntity *pEnemy);

	void PushEnemy(CBaseEntity *pEnemy, Vector &vecLastKnownPos);
	BOOL PopEnemy(void);
	BOOL FGetNodeRoute(const Vector &vecDest);

	void MovementComplete(void);
	int TaskIsRunning(void);
	inline void TaskComplete(void) { if(!HasConditions(bits_COND_TASK_FAILED)) m_iTaskStatus = TASKSTATUS_COMPLETE; }
	inline void TaskFail(void) { SetConditions(bits_COND_TASK_FAILED); }
	inline void TaskBegin(void) { m_iTaskStatus = TASKSTATUS_RUNNING; }
	inline int TaskIsComplete(void) { return(m_iTaskStatus == TASKSTATUS_COMPLETE); }
	inline int MovementIsComplete(void) { return(m_movementGoal == MOVEGOAL_NONE); }

	int IScheduleFlags(void);
	BOOL FRefreshRoute(void);
	BOOL FRouteClear(void);
	void RouteSimplify(CBaseEntity *pTargetEnt);
	void AdvanceRoute(float distance);
	virtual BOOL FTriangulate(const Vector &vecStart, const Vector &vecEnd, float flDist, CBaseEntity *pTargetEnt, Vector *pApex);
	void MakeIdealYaw(const Vector &vecTarget);
	BOOL BuildRoute(const Vector &vecGoal, int iMoveFlag, CBaseEntity *pTarget);
	virtual BOOL BuildNearestRoute(const Vector &vecThreat, const Vector &vecViewOffset, float flMinDist, float flMaxDist);
	int RouteClassify(int iMoveFlag);
	void InsertWaypoint(const Vector &vecLocation, const int &afMoveFlags);

	BOOL FindLateralCover(const Vector &vecThreat, const Vector &vecViewOffset);
	virtual BOOL FindCover(const Vector &vecThreat, const Vector &vecViewOffset, float flMinDist, float flMaxDist);
	virtual BOOL FValidateCover(const Vector &vecCoverLocation) { return TRUE; };
	virtual float CoverRadius(void) { return 784; } // Default cover radius

	virtual BOOL FCanCheckAttacks(void);
	virtual void CheckAmmo(void) { };
	virtual int IgnoreConditions(void);

	inline void	SetConditions(int iConditions) { m_afConditions |= iConditions; }
	inline void	ClearConditions(int iConditions) { m_afConditions &= ~iConditions; }
	inline BOOL HasConditions(int iConditions) { if(m_afConditions & iConditions) return TRUE; return FALSE; }
	inline BOOL HasAllConditions(int iConditions) { if((m_afConditions & iConditions) == iConditions) return TRUE; return FALSE; }

	virtual BOOL FValidateHintType(short sHint);
	int FindHintNode(void);
	virtual BOOL FCanActiveIdle(void);
	void SetTurnActivity(void);
	float FLSoundVolume(CSound *pSound);

	BOOL MoveToNode(Activity movementAct, float waitTime, const Vector &goal);
	BOOL MoveToTarget(Activity movementAct, float waitTime);
	BOOL MoveToLocation(Activity movementAct, float waitTime, const Vector &goal);
	BOOL MoveToEnemy(Activity movementAct, float waitTime);

	// Returns the time when the door will be open
	float OpenDoorAndWait(CBaseEntity *pDoor);

	virtual int ISoundMask(void);
	virtual CSound *PBestSound(void);
	virtual CSound *PBestScent(void);
	virtual float HearingSensitivity(void) { return 1.0f; };

	virtual float FallDamage(const float &flFallVelocity);// XDM3035c

	BOOL FBecomeProne(void);
	virtual void BarnacleVictimBitten(CBaseEntity *pBarnacle);
	virtual void BarnacleVictimReleased(void);

	void SetEyePosition(void);

	BOOL FShouldEat(void);// see if a monster is 'hungry'
	void Eat(float flFullDuration);// make the monster 'full' for a while.

	CBaseEntity *CheckTraceHullAttack(const float &flDist, const int &iDamage, const int &iDmgType);
	BOOL FacingIdeal(void);
	BOOL FCheckAITrigger(void);// checks and, if necessary, fires the monster's trigger target.
	virtual BOOL NoFriendlyFire(void) { return TRUE; };
	BOOL BBoxFlat(void);

	virtual void PrescheduleThink(void) { };

	// combat functions
	BOOL GetEnemy(void);

	virtual Activity GetDeathActivity(void);
	Activity GetSmallFlinchActivity(void);
	BOOL ExitScriptedSequence();
	BOOL CineCleanup();

	Vector ShootAtEnemy(const Vector &shootOrigin);
	virtual Vector BodyTarget(const Vector &posSrc);
	virtual	Vector GetGunPosition(void);

	virtual void TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int TakeHealth(const float &flHealth, const int &bitsDamageType);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);

	int DeadTakeDamage(CBaseEntity* pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);

	void RadiusDamage(CBaseEntity* pInflictor, CBaseEntity *pAttacker, float flDamage, int iClassIgnore, int bitsDamageType);
	void RadiusDamage(const Vector &vecSrc, CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int iClassIgnore, int bitsDamageType);

	void RouteClear(void);
	void RouteNew(void);

	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);// NEW
	virtual bool GibMonster(void);
	virtual BOOL ShouldGibMonster(int iGib);
	virtual void CallGibMonster(void);
	virtual BOOL HasHumanGibs(void);
	virtual BOOL HasAlienGibs(void);
	virtual void FadeMonster(void);	// Called instead of GibMonster() when gibs are disabled

	virtual void DeathSound(void);
	virtual void AlertSound(void) { };
	virtual void IdleSound(void) { };
	virtual void PainSound(void) { };
	virtual void StopFollowing(BOOL clearSchedule) {}

	inline void	Remember(int iMemory) { m_afMemory |= iMemory; }
	inline void	Forget(int iMemory) { m_afMemory &= ~iMemory; }
	inline BOOL HasMemory(int iMemory) { if(m_afMemory & iMemory) return TRUE; return FALSE; }
	inline BOOL HasAllMemories(int iMemory) { if((m_afMemory & iMemory) == iMemory) return TRUE; return FALSE; }

	// XDM
	void StartPatrol(CBaseEntity *path);
	virtual int GetVoicePitch(void) { return m_voicePitch; }
	virtual float GetSoundVolue(void) { return VOL_NORM; }

	virtual BOOL IsMoving(void) { return m_movementGoal != MOVEGOAL_NONE; }
	virtual	BOOL IsMonster(void) { return TRUE; }
	virtual BOOL IsBSPModel(void) { return FALSE; }
	virtual BOOL IsPushable(void) { return TRUE; }
	virtual BOOL HasCustomGibs(void) { return m_iszGibModel != iStringNull; }
	virtual BOOL IsHeavyDamage(float flDamage, int bitsDamageType);// FALSE means light damage
	virtual BOOL IsHuman(void);
	virtual BOOL HasTarget(string_t targetname);// XDM3035c
	virtual BOOL ShouldRespawn(void);// XDM3035

	virtual float GetDamageAmount(void) { return 8; }// XDM: default value. Don't return pev->dmg!
	virtual void FrozenStart(float freezetime);
	virtual void FrozenEnd(void);
	virtual void FrozenThink(void);
	virtual BOOL CanAttack(void);

	CBaseEntity *DropItem(const char *pszItemName, const Vector &vecPos, const Vector &vecAng);// drop an item.

	Vector		m_vFrozenViewAngles;	// XDM: can't look around
	float		m_flUnfreezeTime;		// XDM: for a long time?
	char		*m_iszMusicEventTrack;
	string_t	m_iszGibModel;
	int			m_iGibModelIndex;
	int			m_iGibCount;
	BOOL		m_fFrozen;				// XDM: is the player frozen by fgrenade?
	BOOL		m_fFreezeEffect;		// XDM: used by glowshell effect.
	BOOL		m_bPlayMusicEvent;
	int			m_voicePitch;
	int			m_iClass;// XDM: custom class,
//	float		m_flRespawnTime;// XDM3035

	// these fields have been added in the process of reworking the state machine.(sjb)
	EHANDLE				m_hEnemy;		 // the entity that the monster is fighting.
	EHANDLE				m_hTargetEnt;	 // the entity that the monster is trying to reach
	EHANDLE				m_hOldEnemy[MAX_OLD_ENEMIES];
	Vector				m_vecOldEnemy[MAX_OLD_ENEMIES];

	float				m_flFieldOfView;// width of monster's field of view(dot product)
	float				m_flWaitFinished;// if we're told to wait, this is the time that the wait will be over.
	float				m_flMoveWaitFinished;

	Activity			m_Activity;// what the monster is doing(animation)
	Activity			m_IdealActivity;// monster should switch to this activity

	int					m_LastHitGroup; // the last body region that took damage

	MONSTERSTATE		m_MonsterState;// monster's current state
	MONSTERSTATE		m_IdealMonsterState;// monster should change to this state

	int					m_iTaskStatus;
	Schedule_t			*m_pSchedule;
	int					m_iScheduleIndex;

	WayPoint_t			m_Route[ROUTE_SIZE];	// Positions of movement
	int					m_movementGoal;			// Goal that defines route
	int					m_iRouteIndex;			// index into m_Route[]
	float				m_moveWaitTime;			// How long I should wait for something to move

	Vector				m_vecMoveGoal; // kept around for node graph moves, so we know our ultimate goal
	Activity			m_movementActivity;	// When moving, set this activity

	int					m_iAudibleList; // first index of a linked list of sounds that the monster can hear.
	int					m_afSoundTypes;

	Vector				m_vecLastPosition;// monster sometimes wants to return to where it started after an operation.

	int					m_iHintNode; // this is the hint node that the monster is moving towards or performing active idle on.
	int					m_afMemory;
	int					m_iMaxHealth;// keeps track of monster's maximum health value(for re-healing, etc)
	Vector				m_vecEnemyLKP;// last known position of enemy.(enemy's origin)
	int					m_cAmmoLoaded;		// how much ammo is in the weapon(used to trigger reload anim sequences)
	int					m_afCapability;// tells us what a monster can/can't do.
	float				m_flNextAttack;		// cannot attack again until this time
	int					m_bitsDamageType;	// what types of damage has monster(player) taken
	byte				m_rgbTimeBasedDamage[CDMG_TIMEBASED];
	int					m_lastDamageAmount;// how much damage did monster(player) last take
											// time based damage counters, decr. 1 per 2 seconds
	int					m_bloodColor;		// color of blood particless
	int					m_failSchedule;				// Schedule type to choose if current schedule fails
	float				m_flHungryTime;// set this is a future time to stop the monster from eating for a while.
	float				m_flDistTooFar;	// if enemy farther away than this, bits_COND_ENEMY_TOOFAR set in CheckEnemy
	float				m_flDistLook;	// distance monster sees(Default 2048)
	int					m_iTriggerCondition;// for scripted AI, this is the condition that will cause the activation of the monster's TriggerTarget
	string_t			m_iszTriggerTarget;// name of target that should be fired.
	Vector				m_HackedGunPos;	// HACK until we can query end of gun
// Scripted sequence Info
	SCRIPTSTATE			m_scriptState;		// internal cinematic state
	CCineMonster		*m_pCine;

	float				m_flFallVelocity;// XDM3035c: from player

	int		m_iLastKiller;
	int		m_iLastVictim;

protected:
	int m_afConditions;
};


#endif // BASEMONSTER_H
