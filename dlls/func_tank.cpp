#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "effects.h"
#include "weapons.h"
#include "explode.h"

#include "player.h"
#include "soundent.h"// XDM
#include "projectiles.h"
#include "skill.h"
#include "game.h"

#define SF_TANK_ACTIVE			0x0001
#define SF_TANK_NOTSOLID		0x0002
#define SF_TANK_LINEOFSIGHT		0x0010
#define SF_TANK_CANCONTROL		0x0020
#define SF_TANK_SOUNDON			0x8000

enum TANKBULLET
{
	TANK_BULLET_NONE = 0,
	TANK_BULLET_MACHINEGUN,
	TANK_BULLET_MINIGUN,
	TANK_BULLET_VULCAN,
	TANK_BULLET_HEAVY_TURRET,
	TANK_BULLET_GAUSS,
	TANK_BULLET_LASER,
	TANK_BULLET_RAILGUN,
	TANK_BULLET_SUPER_RAILGUN,
	TANK_BULLET_SHOCK_CANNON,
	TANK_BULLET_LIGHTNING,
	TANK_BULLET_TWIN_LASER,
	TANK_BULLET_TAU_CANNON,
	TANK_BULLET_MEZON_CANNON,
	TANK_BULLET_REPEATER,
	TANK_BULLET_PHOTONGUN,
	TANK_BULLET_BFG
};


enum TANKROCKET
{
	TANK_ROCKET_NONE = 0,
	TANK_ROCKET_M203,
	TANK_ROCKET_NEEDLE_LASER,
	TANK_ROCKET_SHOCK_LASER,
	TANK_ROCKET_HELL_HOUNDER,
	TANK_ROCKET_30MMGRENADE,
	TANK_ROCKET_MINI_MISSILE,
	TANK_ROCKET_BIOHAZARD_MISSILE,
	TANK_ROCKET_GLUON,
	TANK_ROCKET_HELL_FIRE,
	TANK_ROCKET_PLASMA,
	TANK_ROCKET_PLASMA_STORM,
	TANK_ROCKET_PULSE_CANNON,
	TANK_ROCKET_MULTI_CANNON,
	TANK_ROCKET_SCORCHER,
	TANK_ROCKET_FROSTER,
	TANK_ROCKET_SONIC_WAVE,
	TANK_ROCKET_CHARGE_CANNON,
	TANK_ROCKET_DEMOLITION_MISSILE,
	TANK_ROCKET_GHOST_MISSILE,
	TANK_ROCKET_HVR_MISSILE,
	TANK_ROCKET_MICRO_MISSILE,
	TANK_ROCKET_RIPPER,
	TANK_ROCKET_DISRUPTOR,
	TANK_ROCKET_PROTON_MISSILE,
	TANK_ROCKET_TELEPORT,
	TANK_ROCKET_SHOCK_WAVE,
	TANK_ROCKET_NUCLEAR_MISSILE,
	TANK_ROCKET_ANTIMATHERIAL_MISSILE,
	TANK_ROCKET_GUIDED_BOMB
};

class CFuncTank : public CBaseDelay// XDM3035: m_hActivator
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual void Think(void);
	virtual BOOL OnControls(entvars_t *pevTest);
	virtual BOOL IsBSPModel(void) { return TRUE; }// XDM
	virtual void Fire(const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker);
//	virtual void Fire2(const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker);
	Vector UpdateTargetPosition(CBaseEntity *pTarget)
	{
		return pTarget->BodyTarget(pev->origin);
	}
	void StartRotSound(void);
	void StopRotSound(void);
	void TrackTarget(void);
	// Bmodels don't go across transitions
	virtual int	ObjectCaps(void) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	inline bool IsActive(void) { return (pev->spawnflags & SF_TANK_ACTIVE); }
	inline void TankActivate(void) { pev->spawnflags |= SF_TANK_ACTIVE; pev->nextthink = pev->ltime + 0.1; m_fireLast = 0; }
	inline void TankDeactivate(void) { pev->spawnflags &= ~SF_TANK_ACTIVE; m_fireLast = 0; StopRotSound(); }
	BOOL CanFire(void);
	BOOL InRange( float range );
	// Acquire a target.  pPlayer is a player in the PVS
	edict_t *FindTarget( edict_t *pPlayer );
	void TankTrace( const Vector &vecStart, const Vector &vecForward, const Vector &vecSpread, TraceResult &tr );
	Vector BarrelPosition(void)
	{
		Vector forward, right, up;
		ANGLE_VECTORS(pev->angles, forward, right, up);
		return pev->origin + (forward * m_barrelPos.x) + (right * m_barrelPos.y) + (up * m_barrelPos.z);
	}
	void AdjustAnglesForBarrel(Vector &angles, float distance);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];
	BOOL StartControl(CBasePlayer* pController);
	void StopControl(void);
	void ControllerPostFrame(void);

protected:
	CBasePlayer* m_pController;
	float		m_flNextAttack;
	Vector		m_vecControllerUsePos;
	float		m_yawCenter;	// "Center" yaw
	float		m_yawRate;		// Max turn rate to track targets
	float		m_yawRange;		// Range of turning motion (one-sided: 30 is +/- 30 degress from center)
	float		m_yawTolerance;	// Tolerance angle
	float		m_pitchCenter;	// "Center" pitch
	float		m_pitchRate;	// Max turn rate on pitch
	float		m_pitchRange;	// Range of pitch motion as above
	float		m_pitchTolerance;	// Tolerance angle
	float		m_fireLast;		// Last time I fired
	float		m_fireRate;		// How many rounds/second
	float		m_lastSightTime;// Last time I saw target
	float		m_persist;		// Persistence of firing (how long do I shoot when I can't see)
	float		m_minRange;		// Minimum range to aim/track
	float		m_maxRange;		// Max range to aim/track
	Vector		m_barrelPos;	// Length of the freakin barrel
	float		m_spriteScale;	// Scale of any sprites we shoot
	int			m_iszSpriteSmoke;
	int			m_iszSpriteFlash;
	TANKBULLET	m_bulletType;	// Bullet type
	int			m_iBulletDamage; // 0 means use Bullet type's default damage
	Vector		m_sightOrigin;	// Last sight of target
	int			m_spread;		// firing spread
	int			m_iBulletCount; // bullets per 1 shot
};

TYPEDESCRIPTION	CFuncTank::m_SaveData[] = 
{
	DEFINE_FIELD( CFuncTank, m_yawCenter, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_yawRate, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_yawRange, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_yawTolerance, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_pitchCenter, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_pitchRate, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_pitchRange, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_pitchTolerance, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_fireLast, FIELD_TIME ),
	DEFINE_FIELD( CFuncTank, m_fireRate, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_lastSightTime, FIELD_TIME ),
	DEFINE_FIELD( CFuncTank, m_persist, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_minRange, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_maxRange, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_barrelPos, FIELD_VECTOR ),
	DEFINE_FIELD( CFuncTank, m_spriteScale, FIELD_FLOAT ),
	DEFINE_FIELD( CFuncTank, m_iszSpriteSmoke, FIELD_STRING ),
	DEFINE_FIELD( CFuncTank, m_iszSpriteFlash, FIELD_STRING ),
	DEFINE_FIELD( CFuncTank, m_bulletType, FIELD_INTEGER ),
	DEFINE_FIELD( CFuncTank, m_sightOrigin, FIELD_VECTOR ),
	DEFINE_FIELD( CFuncTank, m_spread, FIELD_INTEGER ),
	DEFINE_FIELD( CFuncTank, m_pController, FIELD_CLASSPTR ),
	DEFINE_FIELD( CFuncTank, m_vecControllerUsePos, FIELD_VECTOR ),
	DEFINE_FIELD( CFuncTank, m_flNextAttack, FIELD_TIME ),
	DEFINE_FIELD( CFuncTank, m_iBulletDamage, FIELD_INTEGER ),
	DEFINE_FIELD( CFuncTank, m_iBulletCount, FIELD_INTEGER )
};

IMPLEMENT_SAVERESTORE( CFuncTank, CBaseEntity );

static Vector gTankSpread[] =
{
	Vector(0.00000, 0.00000, 0.00000),
	Vector(0.00436, 0.00436, 0.00436),
	Vector(0.00872, 0.00872, 0.00872),
	Vector(0.01309, 0.01309, 0.01309),
	Vector(0.01745, 0.01745, 0.01745),
	Vector(0.02181, 0.02181, 0.02181),
	Vector(0.02617, 0.02617, 0.02617),
	Vector(0.03052, 0.03052, 0.03052),
	Vector(0.03488, 0.03488, 0.03488),
	Vector(0.03923, 0.03923, 0.03923),
	Vector(0.04358, 0.04358, 0.04358), // 10 degrees
	Vector(0.04978, 0.04978, 0.04978), // 12 degrees
	Vector(0.06526, 0.06526, 0.06526), // 15 degrees
	Vector(0.08682, 0.08682, 0.08682)// 20 degrees
};

#define MAX_FIRING_SPREADS ARRAYSIZE(gTankSpread)

void CFuncTank :: Spawn(void)
{
	Precache();
	pev->movetype	= MOVETYPE_PUSH;  // so it doesn't get pushed by anything

	if (pev->spawnflags & SF_TANK_NOTSOLID)// XDM
		pev->solid = SOLID_NOT;
	else
		pev->solid = SOLID_BSP;

	SET_MODEL( ENT(pev), STRING(pev->model) );

	m_yawCenter = pev->angles.y;
	m_pitchCenter = pev->angles.x;

	if ( IsActive() )
		pev->nextthink = pev->ltime + 1.0;

	m_sightOrigin = BarrelPosition(); // Point at the end of the barrel

	if ( m_fireRate <= 0 )
		m_fireRate = 1;
	if ( m_spread > MAX_FIRING_SPREADS )
		m_spread = 0;

	pev->oldorigin = pev->origin;
}

void CFuncTank :: Precache(void)
{
	if ( m_iszSpriteSmoke )
		PRECACHE_MODEL(STRINGV(m_iszSpriteSmoke));
	if ( m_iszSpriteFlash )
		PRECACHE_MODEL(STRINGV(m_iszSpriteFlash));
	if ( pev->noise )
		PRECACHE_SOUND(STRINGV(pev->noise));
}

void CFuncTank :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "yawrate"))
	{
		m_yawRate = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "yawrange"))
	{
		m_yawRange = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "yawtolerance"))
	{
		m_yawTolerance = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "pitchrange"))
	{
		m_pitchRange = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "pitchrate"))
	{
		m_pitchRate = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "pitchtolerance"))
	{
		m_pitchTolerance = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "firerate"))
	{
		m_fireRate = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "barrel"))
	{
		m_barrelPos.x = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "barrely"))
	{
		m_barrelPos.y = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "barrelz"))
	{
		m_barrelPos.z = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "spritescale"))
	{
		m_spriteScale = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "spritesmoke"))
	{
		m_iszSpriteSmoke = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "spriteflash"))
	{
		m_iszSpriteFlash = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "rotatesound"))
	{
		pev->noise = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "persistence"))
	{
		m_persist = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "bullet"))
	{
		m_bulletType = (TANKBULLET)atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "bullet_damage" )) 
	{
		m_iBulletDamage = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
		else if ( FStrEq(pkvd->szKeyName, "bullet_count" )) 
	{
		m_iBulletCount = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "firespread"))
	{
		m_spread = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "minRange"))
	{
		m_minRange = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "maxRange"))
	{
		m_maxRange = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
/*	else if (FStrEq(pkvd->szKeyName, "master"))
	{
		m_iszMaster = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}*/
	else
		CBaseEntity::KeyValue( pkvd );
}

////////////// START NEW STUFF //////////////
//==================================================================================
// TANK CONTROLLING
BOOL CFuncTank :: OnControls( entvars_t *pevTest )
{
	if ( !(pev->spawnflags & SF_TANK_CANCONTROL) )
		return FALSE;

// WTF?	Vector offset = pevTest->origin - pev->origin;

	if ( (m_vecControllerUsePos - pevTest->origin).Length() < 30 )
		return TRUE;

	return FALSE;
}

BOOL CFuncTank :: StartControl( CBasePlayer *pController )
{
	if (m_pController != NULL)
		return FALSE;

	// Team only or disabled?
	m_hActivator = pController;
	if (IsLockedByMaster())// XDM3035c
		return FALSE;

//	ALERT( at_console, "using TANK!\n");

	m_pController = pController;
	if (m_pController->m_pActiveItem)
	{
		m_pController->m_pActiveItem->Holster();
//		m_pController->pev->weaponmodel = 0;
// XDM3035b		m_pController->pev->viewmodel = 0; 
	}

	m_pController->m_iHideHUD |= HIDEHUD_WEAPONS;
	m_vecControllerUsePos = m_pController->pev->origin;
	pev->nextthink = pev->ltime + 0.1;
	return TRUE;
}

void CFuncTank :: StopControl()
{
	// TODO: bring back the controllers current weapon
	if (!m_pController)
		return;

	if (m_pController->m_pActiveItem)
		m_pController->m_pActiveItem->Deploy();

//	ALERT( at_console, "stopped using TANK\n");

	m_pController->m_iHideHUD &= ~HIDEHUD_WEAPONS;
	pev->nextthink = 0;
	m_pController = NULL;

	if ( IsActive() )
		pev->nextthink = pev->ltime + 1.0;
}

// Called each frame by the player's ItemPostFrame
void CFuncTank :: ControllerPostFrame(void)
{
	if (m_pController == NULL)// XDM3035
		return;
//	ASSERT(m_pController != NULL);

	if (gpGlobals->time < m_flNextAttack)
		return;

	if (m_pController->pev->button & IN_ATTACK || m_pController->pev->button & IN_ATTACK2)// XDM
	{
		Vector vecForward;
		ANGLE_VECTORS(pev->angles, vecForward, NULL, NULL);
		m_fireLast = gpGlobals->time - (1/m_fireRate) - 0.01f;  // to make sure the gun doesn't fire too many bullets

//		if (m_pController->pev->button & IN_ATTACK)
			Fire(BarrelPosition(), vecForward, m_pController);
//		else
//			Fire2(BarrelPosition(), vecForward, m_pController);

		// HACKHACK -- make some noise (that the AI can hear)
		CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, LOUD_GUN_VOLUME, 1);// XDM

		if (m_pController && m_pController->IsPlayer())
			((CBasePlayer *)m_pController)->m_iWeaponVolume = LOUD_GUN_VOLUME;

		m_flNextAttack = gpGlobals->time + (1/m_fireRate);
	}
}
////////////// END NEW STUFF //////////////

void CFuncTank :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (mp_noshooting.value > 1)// XDM3037
		return;

	if ( pev->spawnflags & SF_TANK_CANCONTROL )
	{  // player controlled turret

//		if ( pActivator->Classify() != CLASS_PLAYER )
		if (!pActivator->IsPlayer())
			return;

		if ( value == 2 && useType == USE_SET )
		{
			ControllerPostFrame();
		}
		else if ( !m_pController && useType != USE_OFF )
		{
			((CBasePlayer*)pActivator)->m_pTank = this;
			StartControl( (CBasePlayer*)pActivator );
		}
		else
		{
			StopControl();
		}
	}
	else
	{
		if ( !ShouldToggle( useType, IsActive() ) )
			return;

		if ( IsActive() )
		{
			TankDeactivate();
			m_hActivator = NULL;// XDM3035
		}
		else
		{
			TankActivate();
			m_hActivator = pActivator;// XDM3035
		}
	}
}

edict_t *CFuncTank :: FindTarget( edict_t *pPlayer )
{
	if (FBitSet(pPlayer->v.flags, FL_NOTARGET))// XDM
		return NULL;

	return pPlayer;
}

BOOL CFuncTank::CanFire(void)// XDM3035c
{
	if ((gpGlobals->time - m_lastSightTime) < m_persist)
	{
		if (m_flNextAttack <= gpGlobals->time)
			return TRUE;
	}
	return FALSE;
}

BOOL CFuncTank :: InRange( float range )
{
	if ( range < m_minRange )
		return FALSE;
	if ( m_maxRange > 0 && range > m_maxRange )
		return FALSE;

	return TRUE;
}

void CFuncTank :: Think(void)
{
	pev->avelocity = g_vecZero;
	TrackTarget();

	if ( fabs(pev->avelocity.x) > 1 || fabs(pev->avelocity.y) > 1 )
		StartRotSound();
	else
		StopRotSound();
}

void CFuncTank::TrackTarget(void)
{
	TraceResult tr;
	edict_t *pPlayer = FIND_CLIENT_IN_PVS( edict() );
	BOOL updateTime = FALSE, lineOfSight;
	Vector angles, direction, targetPosition, barrelEnd;
	edict_t *pTarget = NULL;// XDM3035c
	// Get a position to aim for
	if (m_pController)
	{
		// Tanks attempt to mirror the player's angles
		angles = m_pController->pev->v_angle;
		angles[0] = 0 - angles[0];
		pev->nextthink = pev->ltime + 0.05;
	}
	else
	{
		if ( IsActive() )
			pev->nextthink = pev->ltime + 0.1;
		else
			return;

		if ( FNullEnt( pPlayer ) )
		{
			if ( IsActive() )
				pev->nextthink = pev->ltime + 2;	// Wait 2 secs
			return;
		}
		pTarget = FindTarget( pPlayer );
		if ( !pTarget )
			return;

		// Calculate angle needed to aim at target
		barrelEnd = BarrelPosition();
		targetPosition = pTarget->v.origin + pTarget->v.view_ofs;
		float range = (targetPosition - barrelEnd).Length();
		
		if ( !InRange( range ) )
			return;

		UTIL_TraceLine( barrelEnd, targetPosition, dont_ignore_monsters, edict(), &tr );
		
		lineOfSight = FALSE;
		// No line of sight, don't track
		if ( tr.flFraction == 1.0 || tr.pHit == pTarget )
		{
			lineOfSight = TRUE;

			CBaseEntity *pInstance = CBaseEntity::Instance(pTarget);
			if ( InRange( range ) && pInstance && pInstance->IsAlive() )
			{
				updateTime = TRUE;
				m_sightOrigin = UpdateTargetPosition( pInstance );
			}
		}

		// Track sight origin
		// !!! I'm not sure what i changed
		direction = m_sightOrigin - pev->origin;
//		direction = m_sightOrigin - barrelEnd;
		angles = UTIL_VecToAngles( direction );

		// Calculate the additional rotation to point the end of the barrel at the target (not the gun's center) 
		AdjustAnglesForBarrel( angles, direction.Length() );
	}

	angles.x = -angles.x;

	// Force the angles to be relative to the center position
	angles.y = m_yawCenter + UTIL_AngleDistance( angles.y, m_yawCenter );
	angles.x = m_pitchCenter + UTIL_AngleDistance( angles.x, m_pitchCenter );

	// Limit against range in y
	if ( angles.y > m_yawCenter + m_yawRange )
	{
		angles.y = m_yawCenter + m_yawRange;
		updateTime = FALSE;	// Don't update if you saw the player, but out of range
	}
	else if ( angles.y < (m_yawCenter - m_yawRange) )
	{
		angles.y = (m_yawCenter - m_yawRange);
		updateTime = FALSE; // Don't update if you saw the player, but out of range
	}

	if ( updateTime )
		m_lastSightTime = gpGlobals->time;

	// Move toward target at rate or less
	float distY = UTIL_AngleDistance( angles.y, pev->angles.y );
	pev->avelocity.y = distY * 10;
	if (pev->avelocity.y > m_yawRate)
		pev->avelocity.y = m_yawRate;
	else if ( pev->avelocity.y < -m_yawRate )
		pev->avelocity.y = -m_yawRate;

	// Limit against range in x
	if (angles.x > m_pitchCenter + m_pitchRange)
		angles.x = m_pitchCenter + m_pitchRange;
	else if (angles.x < m_pitchCenter - m_pitchRange)
		angles.x = m_pitchCenter - m_pitchRange;

	// Move toward target at rate or less
	float distX = UTIL_AngleDistance( angles.x, pev->angles.x );
	pev->avelocity.x = distX  * 10;

	if ( pev->avelocity.x > m_pitchRate )
		pev->avelocity.x = m_pitchRate;
	else if ( pev->avelocity.x < -m_pitchRate )
		pev->avelocity.x = -m_pitchRate;

	if ( m_pController )
		return;

	if ( CanFire() && ( (fabs(distX) < m_pitchTolerance && fabs(distY) < m_yawTolerance) || (pev->spawnflags & SF_TANK_LINEOFSIGHT) ) )
	{
		BOOL fire = FALSE;
		Vector forward;
		ANGLE_VECTORS(pev->angles, forward, NULL, NULL);

		if ( pev->spawnflags & SF_TANK_LINEOFSIGHT )
		{
			float length = direction.Length();
			UTIL_TraceLine( barrelEnd, barrelEnd + forward * length, dont_ignore_monsters, edict(), &tr );
			if (pTarget && tr.pHit == pTarget)// XDM3035c
				fire = TRUE;
		}
		else
			fire = TRUE;

		if (fire)
		{
			if (m_fireLast == 0.0f)
				m_fireLast = gpGlobals->time - (1/m_fireRate) - 0.01f;

			Fire(BarrelPosition(), forward, m_hActivator?(CBaseEntity *)m_hActivator:this);// XDM3035: m_hActivator
			m_flNextAttack = gpGlobals->time + (1/m_fireRate);
		}
		else
			m_fireLast = 0;
	}
	else
		m_fireLast = 0;
}
// If barrel is offset, add in additional rotation
void CFuncTank::AdjustAnglesForBarrel( Vector &angles, float distance )
{
	if ( m_barrelPos.y != 0 || m_barrelPos.z != 0 )
	{
		float r2, d2;
		distance -= m_barrelPos.z;
		d2 = distance * distance;
		if ( m_barrelPos.y )
		{
			r2 = m_barrelPos.y * m_barrelPos.y;
			angles.y += (180.0 / M_PI) * atan2( m_barrelPos.y, sqrt( d2 - r2 ) );
		}
		if ( m_barrelPos.z )
		{
			r2 = m_barrelPos.z * m_barrelPos.z;
			angles.x += (180.0 / M_PI) * atan2( -m_barrelPos.z, sqrt( d2 - r2 ) );
		}
	}
}
// Fire targets and spawn sprites
void CFuncTank::Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker )
{
//	if ( m_fireLast != 0 )
	{
		if ( m_iszSpriteSmoke )
		{
			CSprite *pSprite = CSprite::SpriteCreate( STRING(m_iszSpriteSmoke), barrelEnd, TRUE );
			if (pSprite)
			{
				pSprite->AnimateAndDie( RANDOM_FLOAT( 15.0, 20.0 ) );
				pSprite->SetTransparency( kRenderTransAlpha, pev->rendercolor.x, pev->rendercolor.y, pev->rendercolor.z, 255, kRenderFxNone );
				pSprite->pev->velocity.z = RANDOM_FLOAT(40, 80);
				pSprite->SetScale( m_spriteScale );
			}
		}
		if ( m_iszSpriteFlash )
		{
			CSprite *pSprite = CSprite::SpriteCreate( STRING(m_iszSpriteFlash), barrelEnd, TRUE );
			if (pSprite)
			{
				pSprite->AnimateAndDie( 60 );
				pSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 255, kRenderFxNoDissipation );
				pSprite->SetScale( m_spriteScale );
				// Hack Hack, make it stick around for at least 100 ms.
				pSprite->pev->nextthink += 0.1;
			}
		}
		SUB_UseTargets( this, USE_TOGGLE, 0 );

		if ((gpGlobals->time - m_fireLast) * m_fireRate > 0.0f)// XDM3035c
			m_fireLast = gpGlobals->time;
	}
}
/*
void CFuncTank::Fire2( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker )
{
    Fire(barrelEnd, forward, pAttacker);
}
*/
void CFuncTank::TankTrace( const Vector &vecStart, const Vector &vecForward, const Vector &vecSpread, TraceResult &tr )
{
	// get circular gaussian spread
	float x, y, z;
	do {
		x = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);
		y = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);
		z = x*x+y*y;
	} while (z > 1);
	Vector vecDir = vecForward +
		x * vecSpread.x * gpGlobals->v_right +
		y * vecSpread.y * gpGlobals->v_up;
	Vector vecEnd = vecStart + vecDir * 4096;
	UTIL_TraceLine( vecStart, vecEnd, dont_ignore_monsters, edict(), &tr );
}
	
void CFuncTank::StartRotSound(void)
{
	if ( !pev->noise || (pev->spawnflags & SF_TANK_SOUNDON) )
		return;
	pev->spawnflags |= SF_TANK_SOUNDON;
	EMIT_SOUND( edict(), CHAN_STATIC, STRINGV(pev->noise), 0.85, ATTN_NORM);
}

void CFuncTank::StopRotSound(void)
{
	if ( pev->spawnflags & SF_TANK_SOUNDON )
		STOP_SOUND( edict(), CHAN_STATIC, STRINGV(pev->noise) );
	pev->spawnflags &= ~SF_TANK_SOUNDON;
}


class CFuncTankGun : public CFuncTank
{
public:
	virtual void Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker );
};
LINK_ENTITY_TO_CLASS( func_tank, CFuncTankGun );

void CFuncTankGun::Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker )
{
	UTIL_MakeAimVectors(pev->angles);

	if (m_iBulletCount <= 0)
		m_iBulletCount = 1;

		switch( m_bulletType )
		{
			case TANK_BULLET_MACHINEGUN:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_MACHINEGUN, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgMachinegun, DMG_BULLET, this, pAttacker);
			break;

			case TANK_BULLET_MINIGUN:
			FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_MINIGUN, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgMinigun, DMG_BULLET, this, pAttacker);
			break;

			case TANK_BULLET_VULCAN:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_VULCAN, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgVulcan, DMG_BULLET, this, pAttacker);
			break;

			case TANK_BULLET_HEAVY_TURRET:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_HEAVY_TURRET, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgHeavyTurret, DMG_BULLET, this, pAttacker);
			break;

			case TANK_BULLET_GAUSS:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_GAUSS, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgGauss, DMG_ENERGYBEAM, this, pAttacker);
			break;

			case TANK_BULLET_RAILGUN:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_RAILGUN, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgRailGun, DMG_BULLET, this, pAttacker);
			break;

			case TANK_BULLET_SUPER_RAILGUN:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_SUPERRAILGUN, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgSuperRailGun, DMG_BULLET, this, pAttacker);
			break;

			case TANK_BULLET_SHOCK_CANNON:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_SHOCK, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgShockRifle, DMG_SHOCK, this, pAttacker);
			break;

			case TANK_BULLET_LIGHTNING:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_LIGHTNING, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgLightning, DMG_SHOCK, this, pAttacker);
			break;

			case TANK_BULLET_LASER:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_LASER, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgLaser, DMG_ENERGYBEAM, this, pAttacker);
			break;

			case TANK_BULLET_TWIN_LASER:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_TWINLASER, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgTwinLaser, DMG_ENERGYBEAM, this, pAttacker);
			break;

			case TANK_BULLET_TAU_CANNON:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_TAUCANNON, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgTauCannon, DMG_ENERGYBLAST, this, pAttacker);
			break;

			case TANK_BULLET_MEZON_CANNON:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_MEZONCANNON, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgMezonCannon, DMG_ENERGYBEAM, this, pAttacker);
			break;

			case TANK_BULLET_REPEATER:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_BUCKSHOT, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgRepeater, DMG_BULLET, this, pAttacker);
			break;

			case TANK_BULLET_BFG:
				FireBullets(m_iBulletCount, barrelEnd, forward, gTankSpread[m_spread], NULL, 8192, BULLET_BFG, (m_iBulletDamage > 0)? m_iBulletDamage : gSkillData.DmgBfg, DMG_BLAST, this, pAttacker);
			break;

			default:
				case TANK_BULLET_NONE:
			break;
		}
		CFuncTank::Fire( barrelEnd, forward, pAttacker );
}


class CFuncTankLaser : public CFuncTank
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Activate(void);
	virtual void Think(void);
	virtual void Fire(const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker);
//	virtual void Fire2(const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker);
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	CLaser *GetLaser(void);
	static TYPEDESCRIPTION m_SaveData[];

private:
	CLaser *m_pLaser;
	float m_laserTime;
};

LINK_ENTITY_TO_CLASS( func_tanklaser, CFuncTankLaser );

TYPEDESCRIPTION	CFuncTankLaser::m_SaveData[] = 
{
	DEFINE_FIELD( CFuncTankLaser, m_pLaser, FIELD_CLASSPTR ),
	DEFINE_FIELD( CFuncTankLaser, m_laserTime, FIELD_TIME ),
};

IMPLEMENT_SAVERESTORE( CFuncTankLaser, CFuncTank );

void CFuncTankLaser::Activate(void)
{
	if ( !GetLaser() )
	{
		ALERT(at_error, "Laser tank %s with no env_laser!\n", STRING(pev->targetname));
		UTIL_Remove(this);
	}
	else
	{
		m_pLaser->TurnOff();
	}
}

void CFuncTankLaser::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "laserentity"))
	{
		pev->message = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CFuncTank::KeyValue( pkvd );
}

CLaser *CFuncTankLaser::GetLaser(void)
{
	if ( m_pLaser )
		return m_pLaser;

	edict_t	*pentLaser;

	pentLaser = FIND_ENTITY_BY_TARGETNAME( NULL, STRING(pev->message) );
	while ( !FNullEnt( pentLaser ) )
	{
		// Found the landmark
		if ( FClassnameIs( pentLaser, "env_laser" ) )
		{
			m_pLaser = (CLaser *)CBaseEntity::Instance(pentLaser);
			break;
		}
		else
			pentLaser = FIND_ENTITY_BY_TARGETNAME( pentLaser, STRING(pev->message) );
	}

	return m_pLaser;
}

void CFuncTankLaser::Think(void)
{
	if ( m_pLaser && m_pLaser->IsOn() && (gpGlobals->time > m_laserTime) )// XDM3035: this code is obsolete, but needs to be tested to be removed completely
		m_pLaser->TurnOff();

	CFuncTank::Think();
}

void CFuncTankLaser::Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker )
{
	if (/* m_fireLast != 0 && */GetLaser())
	{
		// TankTrace needs gpGlobals->v_up, etc.
		UTIL_MakeAimVectors(pev->angles);
		int bulletCount = (gpGlobals->time - m_fireLast) * m_fireRate;
		if (bulletCount)
		{
			TraceResult tr;
			for (int i = 0; i < bulletCount; ++i)
			{
				m_pLaser->pev->origin = barrelEnd;
				TankTrace( barrelEnd, forward, gTankSpread[m_spread], tr );
				m_laserTime = gpGlobals->time;
				m_pLaser->TurnOn();
				m_pLaser->pev->dmgtime = gpGlobals->time - 1.0f;
//				m_pLaser->m_hOwner = pAttacker;// XDM3037
//				m_pLaser->FireAtPoint(tr);
				m_pLaser->FireAtPoint(tr, pAttacker);
				m_pLaser->SetThink(&CLaser::TurnOffThink);
				m_pLaser->pev->nextthink = gpGlobals->time + gpGlobals->frametime * 3.0f;// TESTME 0.2f;// XDM3035: TUNE
			}
			CFuncTank::Fire( barrelEnd, forward, pAttacker );
		}
	}
	else
	{
		CFuncTank::Fire( barrelEnd, forward, pAttacker );
	}
}
/*
void CFuncTankLaser::Fire2( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker )
{
	Fire( barrelEnd, forward, pev );
}
*/
class CFuncTankRocket : public CFuncTank
{
public:
	virtual void Precache(void);
	virtual void Fire(const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker);
	virtual void KeyValue(KeyValueData *pkvd);
	TANKROCKET	m_projectileType;
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];
};

LINK_ENTITY_TO_CLASS( func_tankrocket, CFuncTankRocket );

void CFuncTankRocket::Precache(void)
{
	CFuncTank::Precache();
}

void CFuncTankRocket::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "projectile"))
	{
		m_projectileType = (TANKROCKET)atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CFuncTank::KeyValue( pkvd );
}

TYPEDESCRIPTION	CFuncTankRocket::m_SaveData[] = 
{
	DEFINE_FIELD( CFuncTankRocket, m_projectileType, FIELD_INTEGER )
};

IMPLEMENT_SAVERESTORE( CFuncTankRocket, CBaseEntity );

void CFuncTankRocket::Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker )
{
	UTIL_MakeVectors(pev->angles);

	switch(m_projectileType)
	{
		case TANK_ROCKET_M203:
			CM203Grenade::ShootM203Grenade(pAttacker, barrelEnd, FIRESPREAD_M203GRENADE, gSkillData.DmgM203Grenade, 1);
		break;

		case TANK_ROCKET_30MMGRENADE:
			C30mmGrenade::ShootContact(pAttacker, barrelEnd, FIRESPREAD_30MMGRENADE, gSkillData.Dmg30mmGrenade, 1);
		break;

		case TANK_ROCKET_NEEDLE_LASER:
			CNeedleLaser::ShootNeedleLaser(pAttacker, barrelEnd, FIRESPREAD_NEEDLE_LASER, gSkillData.DmgNeedleLaser);
		break;

		case TANK_ROCKET_SHOCK_LASER:
			CShockLaser::ShootShockLaser(pAttacker, barrelEnd, FIRESPREAD_SHOCK_LASER, gSkillData.DmgShockLaser);
		break;

		case TANK_ROCKET_HELL_HOUNDER:
			CHellHounder::ShootHellHounder(pAttacker, barrelEnd, FIRESPREAD_HELL_HOUNDER, gSkillData.DmgHellHounder);
		break;

		case TANK_ROCKET_MINI_MISSILE:
			CMiniMissile::ShootMiniMissile(pAttacker, barrelEnd, FIRESPREAD_MINI_MISSILE, gSkillData.DmgMiniMissile);
		break;

		case TANK_ROCKET_BIOHAZARD_MISSILE:
			CBioMissile::ShootBioMissile(pAttacker, barrelEnd, FIRESPREAD_BIOHAZARD_MISSILE, gSkillData.DmgBioMissile);
		break;

		case TANK_ROCKET_GLUON:
			CGluonBall::ShootGluonBall(pAttacker, barrelEnd, FIRESPREAD_GLUONGUN, gSkillData.DmgGluonBall);
		break;

		case TANK_ROCKET_PLASMA_STORM:
			CPlasmaStorm::ShootPlasmaStorm(pAttacker, barrelEnd, FIRESPREAD_PLASMASTORM, gSkillData.DmgPlasmaStorm);
		break;

		case TANK_ROCKET_PLASMA:
			CPlasmaBall::ShootPlasmaBall(pAttacker, barrelEnd, FIRESPREAD_PLASMA, gSkillData.DmgPlasma);
		break;

		case TANK_ROCKET_HELL_FIRE:
			CHellFire::ShootHellFire(pAttacker, barrelEnd, FIRESPREAD_HELLFIRE, gSkillData.DmgHellFire);
		break;

		case TANK_ROCKET_PULSE_CANNON:
			CPulseCannon::ShootPulseCannon(pAttacker, barrelEnd, FIRESPREAD_PULSE_CANNON, gSkillData.DmgPulseCannon);
		break;

		case TANK_ROCKET_MULTI_CANNON:
			CMultiCannon::ShootMultiCannon(pAttacker, barrelEnd, FIRESPREAD_MULTI_CANNON, gSkillData.DmgMultiCannon);
		break;

		case TANK_ROCKET_SCORCHER:
			CScorcher::ShootScorcher(pAttacker, barrelEnd, FIRESPREAD_SCORCHER, gSkillData.DmgScorcher);
		break;

		case TANK_ROCKET_FROSTER:
			CFrostBall::ShootFrostBall(pAttacker, barrelEnd, FIRESPREAD_FROSTER, gSkillData.DmgFrostBall);
		break;

		case TANK_ROCKET_SONIC_WAVE:
			CSonicWave::ShootSonicWave(pAttacker, barrelEnd, FIRESPREAD_SONIC_WAVE, gSkillData.DmgSonicWave);
		break;

		case TANK_ROCKET_CHARGE_CANNON:
			CChargeCannon::ShootChargeCannon(pAttacker, barrelEnd, FIRESPREAD_CHARGE_CANNON, gSkillData.DmgChargeCannon);
		break;

		case TANK_ROCKET_GHOST_MISSILE:
			CGhostMissile::ShootGhostMissile(pAttacker, barrelEnd, FIRESPREAD_GHOST_MISSILE, gSkillData.DmgGhostMissile);
		break;

		case TANK_ROCKET_HVR_MISSILE:
			CHVRMissile::ShootHVRMissile(pAttacker, barrelEnd, gSkillData.DmgHvrMissile);
		break;

		case TANK_ROCKET_RIPPER:
			CRipper::ShootRipper(pAttacker, barrelEnd, FIRESPREAD_RIPPER, gSkillData.DmgRipper);
		break;

		case TANK_ROCKET_DEMOLITION_MISSILE:
			CDemolitionMissile::ShootDemolitionMissile(pAttacker, barrelEnd, FIRESPREAD_DEMOLITION_MISSILE, gSkillData.DmgDemolitionMissile);
		break;

		case TANK_ROCKET_MICRO_MISSILE:
			CMicroMissile::ShootMicroMissile(pAttacker, barrelEnd, 0, FIRESPREAD_MICRO_MISSILE, gSkillData.DmgMicroMissile);
		break;

		case TANK_ROCKET_DISRUPTOR:
			CDisruptorBall::ShootDisruptorBall(pAttacker, barrelEnd, 0, gSkillData.DmgDisruptor);
			CDisruptorBall::ShootDisruptorBall(pAttacker, barrelEnd, 1, gSkillData.DmgDisruptor);
			CDisruptorBall::ShootDisruptorBall(pAttacker, barrelEnd, 2, gSkillData.DmgDisruptor);
		break;

		case TANK_ROCKET_PROTON_MISSILE:
			CProtonMissile::ShootProtonMissile(pAttacker, barrelEnd, 0, gSkillData.DmgProtonMissile);
		break;

		case TANK_ROCKET_TELEPORT:
			CTeleporter::Fire(pAttacker, 0, barrelEnd, gSkillData.DmgTeleporter);
		break;

		case TANK_ROCKET_SHOCK_WAVE:
			CLightningBall::Fire(pAttacker, 0, barrelEnd, gSkillData.DmgLightningBall, gSkillData.DmgLightningBeam);
		break;

		case TANK_ROCKET_NUCLEAR_MISSILE:
			CNuclearMissile::ShootNuclearMissile(pAttacker, barrelEnd, 0, gSkillData.DmgNuclearMissile);
		break;

		case TANK_ROCKET_ANTIMATHERIAL_MISSILE:
			CAntimatherialMissile::ShootAntimatherialMissile(pAttacker, barrelEnd, 0, gSkillData.DmgAntimatherialMissile);
		break;

		case TANK_ROCKET_GUIDED_BOMB:
			CGuidedBomb::ShootGuidedBomb(pAttacker, barrelEnd, FIRESPREAD_GUIDED_BOMB, 4, gSkillData.DmgGuidedBomb);
		break;


		default:
			case TANK_ROCKET_NONE:
		break;
	}

	CFuncTank::Fire( barrelEnd, forward, pAttacker );// XDM3034: pAttacker was this
}


class CFuncTankMortar : public CFuncTank
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Fire(const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker);
//	virtual void Fire2(const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker);
};
LINK_ENTITY_TO_CLASS( func_tankmortar, CFuncTankMortar );

void CFuncTankMortar::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "iMagnitude"))
	{
		pev->impulse = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CFuncTank::KeyValue( pkvd );
}

void CFuncTankMortar::Fire( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker )
{
//	if ( m_fireLast != 0 )
	{
		int bulletCount = (gpGlobals->time - m_fireLast) * m_fireRate;
		// Only create 1 explosion
		if ( bulletCount > 0 )
		{
			TraceResult tr;
			// TankTrace needs gpGlobals->v_up, etc.
			UTIL_MakeAimVectors(pev->angles);
			TankTrace( barrelEnd, forward, gTankSpread[m_spread], tr );
//			ExplosionCreate( tr.vecEndPos, pev->angles, pAttacker!=NULL?pAttacker:this, this, pev->impulse, 0, 0.0f);// XDM
			ExplosionCreate( tr.vecEndPos, pev->angles, pAttacker!=NULL?pAttacker:this, pev->impulse, 0, 0.0f);// XDM

			CFuncTank::Fire( barrelEnd, forward, pAttacker );
		}
	}
//	else
//		CFuncTank::Fire( barrelEnd, forward, pAttacker );
}
/*
void CFuncTankMortar::Fire2( const Vector &barrelEnd, const Vector &forward, CBaseEntity *pAttacker )
{
	Fire( barrelEnd, forward, pev );
}
*/
//============================================================================	
// FUNC TANK CONTROLS
//============================================================================
class CFuncTankControls : public CBaseEntity
{
public:
	virtual int	ObjectCaps(void);
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual void Think(void);
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	CFuncTank *m_pTank;
	static TYPEDESCRIPTION m_SaveData[];
};

LINK_ENTITY_TO_CLASS( func_tankcontrols, CFuncTankControls );

TYPEDESCRIPTION	CFuncTankControls::m_SaveData[] = 
{
	DEFINE_FIELD( CFuncTankControls, m_pTank, FIELD_CLASSPTR ),
};

IMPLEMENT_SAVERESTORE( CFuncTankControls, CBaseEntity );

int	CFuncTankControls :: ObjectCaps(void) 
{ 
	return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_IMPULSE_USE; 
}

void CFuncTankControls :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ // pass the Use command onto the controls
	if ( m_pTank )
		m_pTank->Use( pActivator, pCaller, useType, value );

	ASSERT( m_pTank != NULL );	// if this fails,  most likely means save/restore hasn't worked properly
}

void CFuncTankControls :: Think(void)
{
	edict_t *pTarget = NULL;

	do 
	{
		pTarget = FIND_ENTITY_BY_TARGETNAME( pTarget, STRING(pev->target) );
	} while ( !FNullEnt(pTarget) && strncmp( STRING(pTarget->v.classname), "func_tank", 9 ) );

	if ( FNullEnt( pTarget ) )
	{
		ALERT( at_console, "No tank %s\n", STRING(pev->target) );
		return;
	}

	m_pTank = (CFuncTank*)Instance(pTarget);
}

void CFuncTankControls::Spawn(void)
{
	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;
	pev->effects |= EF_NODRAW;
	SET_MODEL( ENT(pev), STRING(pev->model) );
	UTIL_SetSize( pev, pev->mins, pev->maxs );
	UTIL_SetOrigin( pev, pev->origin );
	pev->nextthink = gpGlobals->time + 0.3;// After all the func_tank's have spawned
	CBaseEntity::Spawn();
}
