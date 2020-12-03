//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "decals.h"
#include "gamerules.h"
#include "game.h"
#include "msg_fx.h"
#include "projectiles.h"

#define AGRENADE_NORMAL_SCALE	1.0f
#define AGRENADE_SCALE_FACTOR	0.2f

LINK_ENTITY_TO_CLASS(AcidBlob, CAcidBlob);

// TODO: use vuser1, iuser2, iuser3 for these
TYPEDESCRIPTION	CAcidBlob::m_SaveData[] =
{
//	DEFINE_FIELD(CAcidBlob, m_pAiment, FIELD_CLASSPTR),
	DEFINE_FIELD(CAcidBlob, m_vecNormal, FIELD_VECTOR),
	DEFINE_FIELD(CAcidBlob, m_iCount, FIELD_INTEGER),
	DEFINE_FIELD(CAcidBlob, m_fTouched, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CAcidBlob, CGrenade);

CAcidBlob *CAcidBlob::ShootTimed(CBaseEntity *pOwner, const Vector &vecStart, float spread, float time, float dmg, int numSubgrenades, BOOL UseGravity)
{
	CAcidBlob *pNew = GetClassPtr((CAcidBlob *)NULL, "AcidBlob");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}
		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_ACID_BLOB + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmgtime = gpGlobals->time + time;

	//	This is HACK for Bot players
		if (UseGravity)
		{
			pNew->pev->movetype = MOVETYPE_TOSS;
			pNew->pev->gravity = 0.5;
		}
		else
		{
			pNew->pev->movetype = MOVETYPE_FLY;
			pNew->pev->gravity = 0;
		}

		if (numSubgrenades > 0)// XDM3035: was '1'
		{
			pNew->m_iCount = numSubgrenades;
			pNew->pev->scale = AGRENADE_NORMAL_SCALE + AGRENADE_SCALE_FACTOR * numSubgrenades;
			pNew->pev->dmg = (dmg * (1 + numSubgrenades) * 0.75);
			pNew->pev->dmg_save = dmg;
		}
		else
		{
			pNew->m_iCount = 0;
			pNew->pev->dmg = dmg;
		}
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_ACID_BLOB);
	}
	return pNew;
}

CAcidBlob *CAcidBlob::ShootTimedBaby(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecAng, const Vector &vecVel, float time, float dmg)
{
	CAcidBlob *pNew = GetClassPtr((CAcidBlob *)NULL, "AcidBlob");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}
		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->angles = vecAng;
		pNew->pev->velocity = vecVel;
		pNew->pev->dmgtime = gpGlobals->time + time;
		pNew->pev->movetype = MOVETYPE_TOSS;
		pNew->pev->gravity = 0.8;
		pNew->m_iCount = 0;
		pNew->pev->dmg = dmg;
	}
	return pNew;
}

void CAcidBlob::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/w_acidblob.mdl");
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_NO;
	UTIL_SetSize(this, 4.0f);
	UTIL_SetOrigin(pev, pev->origin);

	pev->renderfx = kRenderFxFullBright;// invisible in HL1111 kRenderFxFullBright;
	pev->rendermode = kRenderTransTexture;
	pev->rendercolor = Vector(0,255,0);
	pev->renderamt = 191;
	pev->scale = AGRENADE_NORMAL_SCALE;
	pev->flags |= FL_IMMUNE_SLIME;
	ResetSequenceInfo();

	m_fTouched = FALSE;
	m_vecNormal = g_vecZero;
//	bMovable = FALSE;
	m_pAiment = NULL;
	m_bloodColor = BLOOD_COLOR_GREEN;

	SetTouch(&CAcidBlob::AcidTouch);
	SetThink(&CAcidBlob::AcidThink);
	pev->animtime = gpGlobals->time;
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CAcidBlob::AcidThink(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	if (pev->dmgtime <= gpGlobals->time)
	{
		Detonate();
		return;
	}
	CGrenade::SearchTarget();

	if (pev->fuser4 == 0.0f)// XDM3035c: remember custom gravity
		pev->fuser4 = pev->gravity;

	if (pev->movetype == MOVETYPE_NONE)// XDM3034 touched a surface
	{
		if (pev->waterlevel >= 2)
		{
			pev->scale += 0.05f;
			if (pev->renderamt > 10)
				pev->renderamt -= 2;
		}
	}
	else// haven't touched any surface
	{
		if (pev->waterlevel >= 2)// float emulation
		{
			pev->flags |= FL_FLOAT;
//			pev->movetype = MOVETYPE_FLY;
			pev->velocity = pev->velocity * 0.4f;
			pev->avelocity = pev->avelocity * 0.5f;
			pev->gravity = 0.01f;
			pev->velocity.z += pev->waterlevel*2.0f;
		}
		else// if (pev->waterlevel == 0)
		{
			pev->flags &= ~FL_FLOAT;
/*			if (pev->movetype == MOVETYPE_FLY)// just exited water
			{
				pev->movetype = MOVETYPE_TOSS;// default
	//			pev->velocity.z *= 0.5f;
				pev->velocity.z = 0.0f;
				pev->gravity = 0.1f;
			}
			else*/
				pev->gravity = pev->fuser4;// XDM3035c: reset to default or custom value

		}
	}

//	if (!m_fTouched)
//		ALERT(at_aiconsole, "CAcidBlob pev->gravity = %g\n", pev->gravity);

	pev->nextthink = gpGlobals->time + 0.1f;

//	if (pev->waterlevel != 0)
//		pev->velocity = pev->velocity * 0.6f;

//	if (m_fTouched)// grenade is attached to an entity
//	{
// this ent may have died upon this monent and can cause crash!		if ((m_pAiment == NULL) || FNullEnt(m_pAiment->edict())/* || FNullEnt(pev->aiment)*/)
//			Detonate();
/*		if (bMovable)// entity can move, update origin
		{
			//vecSpot = (m_pAiment->pev->origin - m_pAiment->pev->angles) - vecDelta;
			// UNDONE: angular move? If the aiment is moving?
			vecSpot = m_pAiment->pev->origin - vecDelta;
			UTIL_SetOrigin(pev, vecSpot);
			pev->angles = m_pAiment->pev->angles;
		}*/
//	}

/* NO! recursion!	if (!m_fTouched && sv_overdrive.value > 0.0f)
	{
		if (pev->teleport_time <= gpGlobals->time)
		{
			CAcidBlob::ShootTimed(pev->origin + UTIL_RandomVector()*4.0f, g_vecZero, (RandomVector(VECTOR_CONE_45DEGREES)) * RANDOM_LONG(700, 900), Instance(pev->owner), pev->dmgtime - gpGlobals->time + RANDOM_FLOAT(1.0, 2.0), 0);
			pev->teleport_time = gpGlobals->time + 0.4f;
		}
	}*/
}

void CAcidBlob::AcidTouch(CBaseEntity *pOther)
{
	if (pev->impulse && pOther->pev == m_pAiment->pev)
		return;

	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}

// why?	if (pOther->pev == VARS(pev->owner))
//		return;

	BOOL bMakeLight = TRUE;

	if (pOther->pev->takedamage > DAMAGE_NO)// XDM3035: half of damage is applied directly to living/destructable things
		CGrenade::Impact(pOther, FALSE, 0, 0, FALSE, 0, pev->dmg, DMG_ACID, 0, 0, 0, 0.0, 0);

	if (pOther->IsMonster() || pOther->IsPlayer() || pOther->IsProjectile())
	{
// XDM3035		if (pOther->IsAlive())// a little hack here
// XDM3035			pev->dmg *= 0.8f;
//		else
//			pev->velocity = pev->velocity.Normalize();

		Detonate();
		return;
	}
	else if (pOther->IsBSPModel())// worldspawn, func_wall, breakable, etc.
	{
		pev->movetype = MOVETYPE_NONE;
		if (!FClassnameIs(pOther->pev, "worldspawn"))// because world is 0th entity
		{
			m_fTouched = TRUE;
			m_pAiment = pOther;
//			pev->aiment = pOther->edict();
		}
		if (pOther->IsMovingBSP() || pOther->IsBreakable())// pushable, door, button, etc. XDM3034: breakables may be destroyed!
		{
/*			vecDelta = pOther->pev->origin - pev->origin;
			pev->solid = SOLID_NOT;
			pev->aiment = pOther->edict();// ?
			pev->movetype = MOVETYPE_FOLLOW;
			bMovable = TRUE;*/
			if (pOther->IsMoving())
				bMakeLight = FALSE;// don't make light, because it will not follow me

			Detonate();
			return;
		}
		else
		{
//			pev->solid = SOLID_BBOX;
			pev->takedamage = DAMAGE_YES;
			pev->solid = SOLID_TRIGGER;
// XDM3035			UTIL_SetOrigin(pev, pev->origin);
//			UTIL_SetSize(pev, Vector(-8, -8, -8), Vector(8, 8, 8));
// XDM3035			UTIL_SetSize(this, 8.0f);
		}
	}
	else return;

	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin + pev->velocity.Normalize() * 8.0f, dont_ignore_monsters, ENT(pev), &tr);
	m_vecNormal = tr.vecPlaneNormal;
	pev->origin = pev->origin + m_vecNormal*2.0f;// XDM3035: pull back 2 units to prevent hurting people behind thin glass
	UTIL_SetOrigin(pev, pev->origin);
	UTIL_SetSize(this, 2.0f);// XDM3035: so it won't be triggered through a thin wall
	UTIL_DecalTrace(&tr, DECAL_SPIT1 + RANDOM_LONG(0,1));

	FX_Trail(pev->origin, entindex(), FX_ACID_BLOB_TOUCH );

	if (pev->renderfx == kRenderFxNone)
		pev->renderfx = kRenderFxPulseFast;

	if (m_iCount > 0)
		SetThink(&CAcidBlob::DissociateThink);
}

void CAcidBlob::DissociateThink(void)
{
	if (m_iCount <= 0)// 1 because we already have one!
	{
		pev->renderfx = kRenderFxFullBright;// reset back
		SetThink(&CAcidBlob::AcidThink);// continue thinkin'
		pev->nextthink = gpGlobals->time;
		return;
	}
	CAcidBlob::ShootTimedBaby(Instance(pev->owner), pev->origin + m_vecNormal * RANDOM_FLOAT(5.0, 8.0) + UTIL_RandomBloodVector(), g_vecZero, (m_vecNormal + RandomVector(VECTOR_CONE_40DEGREES)) * RANDOM_FLOAT(275, 350), RANDOM_FLOAT(1.5, 2.5), pev->dmg_save);
	pev->scale -= AGRENADE_SCALE_FACTOR;
	pev->dmg -= pev->dmg_save*0.75;

	--m_iCount;
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.1f, 0.18f);
}

int CAcidBlob::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	Detonate();
	return 1;
}

void CAcidBlob::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)
{
	Detonate();
}

void CAcidBlob::Detonate(void)
{
	if (m_fTouched)// fixed
		pev->origin = pev->origin + m_vecNormal*2.0f;

	CGrenade::Blast(0, 0, pev->dmg, 32+pev->dmg*1.5, DMG_ACID, FX_ACID_BLOB_DETONATE);
}
