//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "saverestore.h"
#include "trains.h"// trigger_camera has train functionality
#include "gamerules.h"
#include "game.h"


//=================================================================
//
// CTriggerSound
//
//=================================================================
class CTriggerSound : public CBaseToggle
{
public:
	virtual void KeyValue(KeyValueData* pkvd);
	virtual void Spawn(void);
	virtual void Touch(CBaseEntity *pOther);
	virtual int	ObjectCaps(void) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};

LINK_ENTITY_TO_CLASS(trigger_sound, CTriggerSound);

void CTriggerSound::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "roomtype"))
	{
		if (!pev->health)
			pev->health = atof(pkvd->szValue);
		else
			ALERT(at_aiconsole, "trigger_sound: room type already defined in 'health' keyvalue!\n");

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CTriggerSound::Touch(CBaseEntity *pOther)
{
	if (IsLockedByMaster())
		return;

	if (pOther->IsPlayer())
	{
		CBasePlayer *pPlayer = (CBasePlayer*)pOther;
		if (pPlayer->m_pentSndLast != this->edict())
		{
			pPlayer->m_pentSndLast = ENT(pev);
			pPlayer->m_flSndRoomtype = pev->health;
			pPlayer->m_flSndRange = 0;

			MESSAGE_BEGIN(MSG_ONE, SVC_ROOMTYPE, NULL, pPlayer->edict());// use the magic #1 for "one client"
				WRITE_SHORT((short)pev->health);
			MESSAGE_END();

			SUB_UseTargets(pPlayer, USE_TOGGLE, 0);
		}
	}
}

void CTriggerSound::Spawn(void)
{
	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL(ENT(pev), STRING(pev->model));
	SetBits(pev->effects, EF_NODRAW);
/* XDM3037:	0 means reset! if (!pev->health)
	{
		ALERT(at_aiconsole, "Removing trigger_sound without room type!\n");
		UTIL_Remove(this);
	}*/
}








//=================================================================
//
// CTriggerOnSight
// Fires target when something is visible
//
//=================================================================
#define SF_ONSIGHT_NOLOS   0x00001
#define SF_ONSIGHT_NOGLASS 0x00002
#define SF_ONSIGHT_ACTIVE  0x08000
#define SF_ONSIGHT_DEMAND  0x10000

class CTriggerOnSight : public CBaseDelay
{
public:
	virtual void Spawn(void);
	virtual void Think(void);
	virtual int	ObjectCaps(void) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual STATE GetState(void);

	BOOL VisionCheck(void);
	BOOL CanSee(CBaseEntity *pLooker, CBaseEntity *pSeen);
};

LINK_ENTITY_TO_CLASS( trigger_onsight, CTriggerOnSight );

void CTriggerOnSight::Spawn(void)
{
	if (pev->target || pev->noise)
		SetNextThink(1);// if we're going to have to trigger stuff, start thinking
	else// otherwise, just check whenever someone asks about our state.
		pev->spawnflags |= SF_ONSIGHT_DEMAND;

	if (pev->max_health > 0)
		pev->health = cos(pev->max_health/2 * M_PI/180.0);
}

STATE CTriggerOnSight::GetState(void)
{
	if (pev->spawnflags & SF_ONSIGHT_DEMAND)
		return VisionCheck()?STATE_ON:STATE_OFF;
	else
		return (pev->spawnflags & SF_ONSIGHT_ACTIVE)?STATE_ON:STATE_OFF;
}

void CTriggerOnSight::Think(void)
{
	// is this a sensible rate?
	SetNextThink(0.1);

//	if (!UTIL_IsMasterTriggered(m_sMaster, NULL))
//	{
//		pev->spawnflags &= ~SF_ONSIGHT_ACTIVE;
//		return;
//	}

	if (VisionCheck())
	{
		if (!FBitSet(pev->spawnflags, SF_ONSIGHT_ACTIVE))
		{
			FireTargets(STRING(pev->target), this, this, USE_TOGGLE, 0);
			FireTargets(STRING(pev->noise1), this, this, USE_ON, 0);
			pev->spawnflags |= SF_ONSIGHT_ACTIVE;
		}
	}
	else
	{
		if (pev->spawnflags & SF_ONSIGHT_ACTIVE)
		{
			FireTargets(STRING(pev->noise), this, this, USE_TOGGLE, 0);
			FireTargets(STRING(pev->noise1), this, this, USE_OFF, 0);
			pev->spawnflags &= ~SF_ONSIGHT_ACTIVE;
		}
	}
}

BOOL CTriggerOnSight::VisionCheck(void)
{
	CBaseEntity *pLooker;
	if (pev->netname)
	{
		pLooker = UTIL_FindEntityByTargetname(NULL, STRING(pev->netname));
		if (!pLooker)
			return FALSE; // if we can't find the eye entity, give up
	}
	else
	{
		pLooker = UTIL_FindEntityByClassname(NULL, "player");
		if (!pLooker)
		{
			ALERT(at_error, "trigger_onsight can't find player!?\n");
			return FALSE;
		}
	}

	CBaseEntity *pSeen;
	if (pev->message)
		pSeen = UTIL_FindEntityByTargetname(NULL, STRING(pev->message));
	else
		return CanSee(pLooker, this);

	if (!pSeen)
	{
		// must be a classname.
		pSeen = UTIL_FindEntityByClassname(pSeen, STRING(pev->message));
		while (pSeen != NULL)
		{
			if (CanSee(pLooker, pSeen))
				return TRUE;
			pSeen = UTIL_FindEntityByClassname(pSeen, STRING(pev->message));
		}
		return FALSE;
	}
	else
	{
		while (pSeen != NULL)
		{
			if (CanSee(pLooker, pSeen))
				return TRUE;
			pSeen = UTIL_FindEntityByTargetname(pSeen, STRING(pev->message));
		}
		return FALSE;
	}
}

// by the criteria we're using, can the Looker see the Seen entity?
BOOL CTriggerOnSight :: CanSee(CBaseEntity *pLooker, CBaseEntity *pSeen)
{
	// out of range?
	if (pev->frags && (pLooker->pev->origin - pSeen->pev->origin).Length() > pev->frags)
		return FALSE;

	// check FOV if appropriate
	if (pev->max_health < 360)
	{
		// copied from CBaseMonster's FInViewCone function
		Vector2D	vec2LOS;
		float	flDot;
		float flComp = pev->health;
		UTIL_MakeVectors ( pLooker->pev->angles );
		vec2LOS = ( pSeen->pev->origin - pLooker->pev->origin ).Make2D();
		vec2LOS = vec2LOS.Normalize();
		flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );

//		ALERT(at_debug, "flDot is %f\n", flDot);

		if ( pev->max_health == -1 )
		{
			CBaseMonster *pMonst = pLooker->MyMonsterPointer();
			if (pMonst)
				flComp = pMonst->m_flFieldOfView;
			else
				return FALSE; // not a monster, can't use M-M-M-MonsterVision
		}

		// outside field of view
		if (flDot <= flComp)
			return FALSE;
	}

	// check LOS if appropriate
	if (!FBitSet(pev->spawnflags, SF_ONSIGHT_NOLOS))
	{
		TraceResult tr;
		if (FBitSet(pev->spawnflags, SF_ONSIGHT_NOGLASS))
			UTIL_TraceLine(pLooker->EyePosition(), pSeen->pev->origin, ignore_monsters, ignore_glass, pLooker->edict(), &tr);
		else
			UTIL_TraceLine(pLooker->EyePosition(), pSeen->pev->origin, ignore_monsters, dont_ignore_glass, pLooker->edict(), &tr);
		if (tr.flFraction < 1.0 && tr.pHit != pSeen->edict())
			return FALSE;
	}

	return TRUE;
}




//=================================================================
//
// CTriggerSetPatrol
// Sets monster's state to patrol
//
//=================================================================
class CTriggerSetPatrol : public CBaseDelay
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int ObjectCaps(void) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	int		m_iszPath;
	static	TYPEDESCRIPTION m_SaveData[];
};

LINK_ENTITY_TO_CLASS( trigger_startpatrol, CTriggerSetPatrol );

TYPEDESCRIPTION	CTriggerSetPatrol::m_SaveData[] = 
{
	DEFINE_FIELD( CTriggerSetPatrol, m_iszPath, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE(CTriggerSetPatrol,CBaseDelay);

void CTriggerSetPatrol::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "m_iszPath"))
	{
		m_iszPath = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerSetPatrol::Spawn(void)
{
	SetBits(pev->effects, EF_NODRAW);
}

void CTriggerSetPatrol::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBaseEntity *pTarget = UTIL_FindEntityByTargetname( NULL, STRING( pev->target )/*, pActivator */);
	CBaseEntity *pPath = UTIL_FindEntityByTargetname( NULL, STRING( m_iszPath )/*, pActivator */);

	if (pTarget && pPath)
	{
		CBaseMonster *pMonster = pTarget->MyMonsterPointer();
		if (pMonster) pMonster->StartPatrol(pPath);
	}
}







//-----------------------------------------------------------------------------
// Purpose: center of gravity, useful for pushables (incl. monsters and players)
// 'gravity' - gravity scalar force
// 'health' - radius
//-----------------------------------------------------------------------------
// obsolete: use 'state' KV. #define SF_GRAVITY_START_ON			0x0001
#define SF_GRAVITY_GRADIENT			0x0002// clients as far as radius are almost not affected
#define SF_GRAVITY_NOCLIENTS		0x0004
#define SF_GRAVITY_NOMONSTERS		0x0008
#define SF_GRAVITY_CLEARVELOCITY	0x0010
#define SF_GRAVITY_IGNOREVIS		0x0020

class CEnvGravityPoint : public CBaseDelay
{
public:
	CEnvGravityPoint(void);
	virtual void Spawn(void);
	virtual void Think(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};

LINK_ENTITY_TO_CLASS(env_gravitypoint, CEnvGravityPoint);

CEnvGravityPoint::CEnvGravityPoint() : CBaseDelay()
{
	SetState(STATE_OFF);// this one starts off unless directly specified
}

void CEnvGravityPoint::Spawn(void)
{
	pev->effects = EF_NODRAW;
	CBaseDelay::Spawn();
}

void CEnvGravityPoint::Think(void)
{
	if (GetState() == STATE_ON)
	{
		CBaseEntity *pEntity = NULL;
		TraceResult	tr;
		Vector vecDelta, vecEntity;
		float dist, force;

		while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, pev->health)) != NULL)
		{
			if (pEntity == this)
				continue;
			if (!pEntity->IsPushable())
				continue;
			if (FBitSet(pEntity->pev->effects, EF_NODRAW))
				continue;
			if (pEntity->pev->movetype == MOVETYPE_NONE || pEntity->pev->movetype == MOVETYPE_NOCLIP)
				continue;
			if (pEntity->IsPlayer() && (pev->spawnflags & SF_GRAVITY_NOCLIENTS))
				continue;
			if (pEntity->IsMonster() && (pev->spawnflags & SF_GRAVITY_NOMONSTERS))
				continue;

			vecEntity = pEntity->Center();
			if (pev->spawnflags & SF_GRAVITY_IGNOREVIS)
				tr.flFraction = 1.0f;
			else
				UTIL_TraceLine(pev->origin, vecEntity, ignore_monsters, dont_ignore_glass, edict(), &tr);

			if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict())// the entity is blocking trace line itself
			{
				force = pev->gravity;
				vecDelta = pev->origin - vecEntity;
				dist = vecDelta.Length();
				if (dist != 0.0f)
					vecDelta /= dist;// 1-unit vector

				if (pev->spawnflags & SF_GRAVITY_GRADIENT)
					force *= (1.0f - dist/pev->health);

				if (pev->spawnflags & SF_GRAVITY_CLEARVELOCITY)
					pEntity->pev->velocity.Clear();

				if (dist < pEntity->pev->maxs.Length() * 1.5f)// otherwise entities will fly through and return...
				{
					pEntity->pev->velocity.Clear();
//					pEntity->pev->gravity = 0;
					UTIL_SetOrigin(pEntity->pev, pev->origin);
				}
				else
					pEntity->pev->velocity += vecDelta * force;
			}
		}
		SetNextThink(0.1);
	}
}

// This thing is so universal it should be moved to CBaseDelay()
void CEnvGravityPoint::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (IsLockedByMaster())
		return;

	if (!ShouldToggle(useType, (GetState() == STATE_ON)?1:0))
		return;

	if (GetState() == STATE_OFF)
	{
		SetState(STATE_ON);
		SetNextThink(0.0);
	}
	else
	{
		SetState(STATE_OFF);
		pev->nextthink = 0.0f;
	}
}
