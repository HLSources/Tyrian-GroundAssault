#define PROJ_SEARCH_RADIUS	32

class CTeleporter : public CGrenade
{
public:
	static CTeleporter *Fire(CBaseEntity *pOwner, CBaseEntity *pEnemy, const Vector &vecSrc, float dmg);
	static CTeleporter *FireTeleEnter(CBaseEntity *pOwner, const Vector &vecSrc, float dmg, const Vector &vecDestOrigin, const Vector &vecDestAngles);

	virtual void Spawn(void);
	virtual BOOL IsPushable(void) {return FALSE;}
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual int ShouldCollide(CBaseEntity *pOther);
	void EXPORT TeleportAndDetonate(void);
	void EXPORT Fly(void);
	void EXPORT Blast(void);
	void EXPORT TeleBallTouch(CBaseEntity *pOther);
	void EXPORT TeleEnterTouch(CBaseEntity *pOther);

	Vector m_vecDestOrigin;
	Vector m_vecDestAngles;
};

class CLightningBall : public CGrenade
{
public:
	static CLightningBall *Fire(CBaseEntity *pOwner, CBaseEntity *pEnemy, const Vector &vecSrc, float dmg, float dmgBeam);
	virtual void Spawn(void);
	virtual BOOL IsPushable(void) {return FALSE;}
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual int ShouldCollide(CBaseEntity *pOther);
	void EXPORT Fly(void);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
};

class CFrostBall : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CFrostBall *ShootFrostBall(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CTrident : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CTrident *ShootTrident(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecVel, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CGluonBall : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CGluonBall *ShootGluonBall(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CDisruptorBall : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CDisruptorBall *ShootDisruptorBall(CBaseEntity *pOwner, const Vector &vecStart, int spin, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CScorcher : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CScorcher *ShootScorcher(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CSunOfGod : public CGrenade
{
public:
	virtual void Spawn(void);
	static CSunOfGod *ShootSunOfGod(CBaseEntity *pOwner, const Vector &vecStart, float dmg);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	void EXPORT BurnThink(void);
	void EXPORT RadiationThink(void);
	void EXPORT DestroyContainer(void);
	void EXPORT BounceTouch(CBaseEntity *pOther);
};

class CShockWave : public CGrenade
{
public:
	virtual void Spawn(void);
	static CShockWave *ShootShockWave(CBaseEntity *pOwner, const Vector &vecStart, float dmg);
	void EXPORT Detonate(void);
};

class CPulseCannon : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CPulseCannon *ShootPulseCannon(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CAntimatherialMissile : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CAntimatherialMissile *ShootAntimatherialMissile(CBaseEntity *pOwner, const Vector &vecStart, CBaseEntity *pEnemy, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT BlackHole(void);
	void EXPORT Fly(void);
};

class CAirStrikeTarget : public CGrenade
{
public:
	static CAirStrikeTarget *CreateNew(CBaseEntity *pOwner, const Vector &vecStart, float dmg, int type);

	virtual void Spawn(void);
	virtual int ObjectCaps(void) {return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE;}
	void EXPORT ATThink(void);
	void EXPORT ATTouch(CBaseEntity *pOther);
	void Ping(void);
	BOOL CheckSatellite(void);
	void EXPORT RadiationThink(void);
};

class CTeleportTarget : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int ObjectCaps(void) {return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE;}
	void EXPORT TTThink(void);
	void EXPORT TTTouch(CBaseEntity *pOther);
	void Ping(void);
	BOOL CheckCeiling(void);
};

class CM203Grenade : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CM203Grenade *ShootM203Grenade(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg, BOOL UseGravity);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CNuclearMissile : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CNuclearMissile *ShootNuclearMissile(CBaseEntity *pOwner, const Vector &vecStart, CBaseEntity *pEnemy, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT BlastRing(void);
	void EXPORT RadiationThink(void);
	void EXPORT Fly(void);
};

class CMeteor : public CGrenade
{
public:
	virtual void Precache(void);
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CMeteor *Shoot(CBaseEntity *pOwner, const Vector &vecSrc, const Vector &vecVel, float spread, float dmg, int type, BOOL UseGravity);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
	void EXPORT Blast(void);
};

class C30mmGrenade : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static C30mmGrenade *ShootContact(CBaseEntity *pOwner, const Vector &vecSrc, float spread, float dmg, BOOL UseGravity);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CPlasmaStorm : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CPlasmaStorm *ShootPlasmaStorm(CBaseEntity *pOwner, const Vector &vecSrc, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
	void EXPORT RadiationThink(void);
};

class CPlasmaBall : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CPlasmaBall *ShootPlasmaBall(CBaseEntity *pOwner, const Vector &vecSrc, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CRipper : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CRipper *ShootRipper(CBaseEntity *pOwner, const Vector &vecSrc, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CNeedleLaser : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CNeedleLaser *ShootNeedleLaser(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CSonicWave : public CGrenade
{
public:
	virtual void Spawn(void);
	static CSonicWave *ShootSonicWave(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CMultiCannon : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CMultiCannon *ShootMultiCannon(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CChargeCannon : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CChargeCannon *ShootChargeCannon(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT FX_On(void);
	void EXPORT FX_Off(void);
	void EXPORT Fly(void);
private:
	CBeam *m_pBeam;
	CBeam *m_pNoise;
};

class CShockLaser : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CShockLaser *ShootShockLaser(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CBanana : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CBanana *ShootBanana(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CMicroMissile : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CMicroMissile *ShootMicroMissile(CBaseEntity *pOwner, const Vector &vecStart, CBaseEntity *pEnemy, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CHellHounder : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CHellHounder *ShootHellHounder(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CFlashBang : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CFlashBang *ShootFlashBang(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT BounceTouch(CBaseEntity *pOther);
	void EXPORT Detonate(void);
	void EXPORT TimeThink(void);
};

class CHVRMissile : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CHVRMissile *ShootHVRMissile(CBaseEntity *pOwner, const Vector &vecStart, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CDemolitionMissile : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CDemolitionMissile *ShootDemolitionMissile(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CGhostMissile : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CGhostMissile *ShootGhostMissile(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};


class CMiniMissile : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CMiniMissile *ShootMiniMissile(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CBioMissile : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CBioMissile *ShootBioMissile(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT BounceTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
	void EXPORT PoisonThink(void);
	void EXPORT PoisonThinkEnd(void);
};

class CToilet : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CToilet *ShootToilet(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg, BOOL LowGravity);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};

class CHellFire : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CHellFire *ShootHellFire(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
	void EXPORT BurnThink(void);
};

class CProtonMissile : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CProtonMissile *ShootProtonMissile(CBaseEntity *pOwner, const Vector &vecStart, CBaseEntity *pEnemy, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
	void EXPORT ShootShards( void );
};

class CGuidedBomb : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CGuidedBomb *ShootGuidedBomb(CBaseEntity *pOwner, const Vector &vecStart, float spread, int numShards, float dmg);
	static CGuidedBomb *ShootGuidedShard(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecVel, float time, float dmg);
	void EXPORT DissociateThink(void);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
	int m_iCount;
	Vector m_vecNormal;
};

class CTripmineGrenade : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);
	void MakeBeam(void);
	void KillBeam(void);
	void EXPORT PowerupThink(void);
	void EXPORT BeamBreakThink(void);
	void EXPORT DelayDeathThink(void);
	virtual void Deactivate(BOOL disintegrate);// XDM3035
	virtual int ObjectCaps(void) {return FCAP_ACROSS_TRANSITION;}// XDM
//	virtual BOOL IsProjectile(void) {return FALSE;}// XDM3034 TESTME
	virtual BOOL IsPushable(void) {return FALSE;}

	static TYPEDESCRIPTION m_SaveData[];

	edict_t *m_pRealOwner;// tracelines don't hit PEV->OWNER, which means a player couldn't detonate his own trip mine, so we store the owner here.
protected:
	float m_flPowerUp;
	float m_flBeamLength;
	Vector m_vecDir;
	Vector m_vecEnd;
	Vector m_posOwner;
	Vector m_angleOwner;
	EHANDLE m_hOwner;
	CBeam *m_pBeam;
};

class CSpiderMine : public CBaseMonster//CGrenade
{
public:
	virtual void Spawn(void);
	virtual int Classify(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);
	virtual bool GibMonster(void);
	virtual BOOL IsProjectile(void) {return TRUE;}
	virtual BOOL IsPushable(void) {return TRUE;}
	virtual BOOL ShouldRespawn(void) { return FALSE; }// XDM3035c: always a temporary monster
	virtual int IRelationship(CBaseEntity *pTarget);

	void EXPORT SuperBounceTouch(CBaseEntity *pOther);
	void EXPORT HuntThink(void);

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];

	float m_flDie;
	Vector m_vecTarget;
	float m_flNextHunt;
	float m_flNextHit;
	Vector m_posPrev;
	EHANDLE m_hOwner;
};

class CAtomBomb : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int ObjectCaps(void) {return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE;}
	virtual void Explode(void);
	void EXPORT BombThink(void);// don't use just Think() - this cannot be disabled
	void EXPORT BombTouch(CBaseEntity *pOther);// same thing
};

class CFrozenCube : public CGrenade
{
public:
	static CFrozenCube *CreateNew(CBaseEntity *pOwner, const Vector &vecStart, float life);
	virtual void Spawn(void);
	void EXPORT OnUpdate(void);
	void EXPORT Destroy(void);
};

class CLightningField : public CGrenade
{
public:
	static CLightningField *CreateNew(CBaseEntity *pOwner, const Vector &vecStart, float dmg, float life);
	static CLightningField *CreateLgtngGib(CBaseEntity *pOwner, const Vector &vecStart, float life);
	virtual void Spawn(void);
	void EXPORT LightningFieldThink(void);
	void EXPORT PlayerElecroDeathThink(void);
};

class CRingTeleport : public CGrenade
{
public:
	static CRingTeleport *CreateNew(CBaseEntity *pOwner, const Vector &vecStart, BOOL SummonBomb);
	virtual void Spawn(void);
	void EXPORT StartFx(void);
	void EXPORT TeleportFx(void);
	void EXPORT EndFx(void);
	void EXPORT Destroy(void);
	void EXPORT DoDmg(void);
};

class CStarBurst : public CGrenade
{
public:
	virtual void Spawn(void);
	static CStarBurst *ShootStarBurst(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Blast(void);
	void EXPORT CheckTarget(void);
	void EXPORT Follow(void);
	void EXPORT CallStrike(void);
};

class CAcidBlob : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);// NEW
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static CAcidBlob *ShootTimed(CBaseEntity *pOwner, const Vector &vecStart, float spread, float time, float dmg, int numSubgrenades, BOOL UseGravity);
	static CAcidBlob *ShootTimedBaby(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecAng, const Vector &vecVel, float time, float dmg);

	void EXPORT AcidTouch(CBaseEntity *pOther);
	void EXPORT AcidThink(void);
	void EXPORT DissociateThink(void);
	void EXPORT Detonate(void);// virtual ?

	static TYPEDESCRIPTION m_SaveData[];

protected:
	CBaseEntity *m_pAiment;
	Vector m_vecNormal;
	int m_iCount;
	BOOL m_fTouched;
};

class CPlasmaShieldCharge : public CGrenade
{
public:
	virtual void Spawn(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	static CPlasmaShieldCharge *CreateNew(const Vector &vecSrc, const Vector &vecAng, const Vector &vecDir, CBaseEntity *pOwner, float dmg, float spread);
	void EXPORT ExplodeTouch(CBaseEntity *pOther);
	void EXPORT Fly(void);
};


