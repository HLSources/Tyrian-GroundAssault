#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "gamerules.h"
#include "projectiles.h"
#include "globals.h"
#include "game.h"
#include "msg_fx.h"
#include "decals.h"

LINK_ENTITY_TO_CLASS(monster_spider_mine, CSpiderMine);

#define SMINE_DETONATE_DELAY	30.0

TYPEDESCRIPTION	CSpiderMine::m_SaveData[] =
{
	DEFINE_FIELD(CSpiderMine, m_flDie, FIELD_TIME),
	DEFINE_FIELD(CSpiderMine, m_vecTarget, FIELD_VECTOR),
	DEFINE_FIELD(CSpiderMine, m_flNextHunt, FIELD_TIME),
	DEFINE_FIELD(CSpiderMine, m_flNextHit, FIELD_TIME),
	DEFINE_FIELD(CSpiderMine, m_posPrev, FIELD_POSITION_VECTOR),
	DEFINE_FIELD(CSpiderMine, m_hOwner, FIELD_EHANDLE),
};

IMPLEMENT_SAVERESTORE(CSpiderMine, CBaseMonster);//CGrenade);

int CSpiderMine::Classify(void)
{
	if (m_iClass != CLASS_NONE)
		return m_iClass;

	if (m_hOwner != NULL && m_hOwner->IsPlayer())
		return CLASS_PLAYER_BIOWEAPON;
	else
		return CLASS_ALIEN_BIOWEAPON;
}

int CSpiderMine::IRelationship(CBaseEntity *pTarget)
{
	if (!pTarget->IsProjectile())
	{
		if (pTarget->IsMonster())
			return R_HT;// XDM3034
		else if (pTarget->IsPlayer())// XDM3034
		{
			if (m_hOwner != NULL)
			{
				if (mp_friendlyfire.value <= 0.0f)
				{
					if (m_hOwner == pTarget)
						return R_NO;

					if (g_pGameRules->IsTeamplay() && m_hOwner->pev->team == pTarget->pev->team)
						return R_NO;
				}
			}
			return R_HT;
		}
	}

	return CBaseMonster::IRelationship(pTarget);
}

void CSpiderMine::Spawn(void)
{
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), "models/w_spider_mine.mdl");

	UTIL_SetSize(pev, Vector(-12, -12, 0), Vector(12, 12, 24));
	UTIL_SetOrigin(pev, pev->origin);
	SetTouch(&CSpiderMine::SuperBounceTouch);
	SetThink(&CSpiderMine::HuntThink);
	pev->nextthink = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time + 1E6;
	pev->flags |= FL_MONSTER;
	pev->takedamage = DAMAGE_AIM;
	pev->health = 50;
	pev->gravity = 0.5;
	pev->friction = 0.5;
	pev->dmg = gSkillData.DmgSpiderMine;
	m_bloodColor = DONT_BLEED;
	m_flDie = gpGlobals->time + SMINE_DETONATE_DELAY;
	m_flFieldOfView = 0; // 180 degrees
	m_iClass = CLASS_PLAYER_BIOWEAPON;
	if (pev->owner)
	{
		m_hOwner = Instance( pev->owner );
	}
	ResetSequenceInfo();
	pev->sequence = LookupSequence("run");// XDM
}

int CSpiderMine::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	return CBaseMonster::TakeDamage(pInflictor, pAttacker, flDamage, bitsDamageType);
}

void CSpiderMine::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)
{
	pev->model = iStringNull;// make invisible
	SetThink(&CBaseEntity::SUB_Remove);
	SetTouchNull();
	pev->nextthink = gpGlobals->time + 0.1;
	pev->takedamage = DAMAGE_NO;

	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + Vector(0,0,-32), ignore_monsters, ENT(pev), &tr);
	UTIL_DecalTrace(&tr, DECAL_SCORCH1 + RANDOM_LONG(0,2));

	FX_Trail( pev->origin, entindex(), FX_SPIDERMINE_DETONATE);

	if (m_hOwner != NULL)
		::RadiusDamage(pev->origin, this, m_hOwner, pev->dmg, pev->dmg*1.5, CLASS_NONE, DMG_BLAST);
	else
		::RadiusDamage(pev->origin, this, this, pev->dmg, pev->dmg*1.5, CLASS_NONE, DMG_BLAST);

	// reset owner so death message happens
	if (m_hOwner != NULL)
		pev->owner = m_hOwner->edict();

	UTIL_Remove(this);
}

bool CSpiderMine::GibMonster(void)
{
	return false;
}

void CSpiderMine::HuntThink(void)
{
	if (!IsInWorld())
	{
		SetTouchNull();
		UTIL_Remove( this );
		return;
	}
	pev->nextthink = gpGlobals->time + 0.1;

	if ( gpGlobals->time >= pev->frags )
	{
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/spidermine_hunt.wav", VOL_NORM, ATTN_NORM);
		pev->frags = gpGlobals->time + RANDOM_FLOAT(2,3);
	}

	// explode when ready
	if (gpGlobals->time >= m_flDie)
	{
		g_vecAttackDir = pev->velocity.Normalize();
		pev->health = -1;
		Killed(this, this, GIB_NEVER);// XDM3034
		return;
	}

	// return if not time to hunt
	if (m_flNextHunt > gpGlobals->time)
		return;

	m_flNextHunt = gpGlobals->time + 2.0;
	Vector vecDir;
	TraceResult tr;

	UTIL_MakeVectors( pev->angles );

	if (m_hEnemy == NULL || !m_hEnemy->IsAlive())
	{
		// find target, bounce a bit towards it.
		Look( 2048 );
		m_hEnemy = BestVisibleEnemy();
	}

	if (m_hEnemy != NULL)
	{
		if (FVisible(m_hEnemy))
		{
			vecDir = m_hEnemy->EyePosition() - pev->origin;
			m_vecTarget = vecDir.Normalize();
		}

		float flVel = pev->velocity.Length();
		float flAdj = 100.0 / (flVel + 10.0);

		if (flAdj > 1.2)
			flAdj = 1.2;
		pev->velocity = pev->velocity * flAdj + m_vecTarget * 450.0f;
	}

	if (FBitSet(pev->flags, FL_ONGROUND))
	{
		pev->avelocity.Clear();
	}
	else
	{
		if (pev->avelocity.IsZero())
		{
			pev->avelocity.x = RANDOM_FLOAT(-100, 100);
			pev->avelocity.z = RANDOM_FLOAT(-100, 100);
		}
	}

	if ((pev->origin - m_posPrev).Length() < 1.0f)
	{
		pev->velocity.x = RANDOM_FLOAT(-100, 100);
		pev->velocity.y = RANDOM_FLOAT(-100, 100);
	}
	m_posPrev = pev->origin;

	pev->angles = UTIL_VecToAngles( pev->velocity );
	pev->angles.z = 0;
	pev->angles.x = 0;
}

void CSpiderMine::SuperBounceTouch(CBaseEntity *pOther)
{
	TraceResult tr = UTIL_GetGlobalTrace();

	// don't hit the guy that launched this grenade
	if ( pev->owner && pOther->edict() == pev->owner )
		return;
	// at least until we've bounced once
//	pev->owner = NULL;
	pev->angles.x = 0;
	pev->angles.z = 0;

	// avoid bouncing too much
	if (m_flNextHit > gpGlobals->time)
		return;

	if ( pOther->pev->takedamage && m_flNextAttack < gpGlobals->time)
	{
		if (tr.pHit == pOther->edict())// make sure it's me who has touched them
		{
			if (tr.pHit->v.modelindex != pev->modelindex)// and it's not another squeakgrenade
			{
				g_vecAttackDir = pev->velocity.Normalize();
				pev->health = -1;
				Killed(this, this, GIB_NEVER);// XDM3034
				return;
			}
		}
//		else
//			ALERT( at_console, "been hit\n");
	}

	m_flNextHit = gpGlobals->time + 0.1;
	m_flNextHunt = gpGlobals->time;
}



