#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "globals.h"
#include "game.h"
#include "msg_fx.h"
#include "projectiles.h"
#include "decals.h"

#define AM_MISSILE_GRAVITY_RADIUS	1536
#define AM_MISSILE_ANNIHILATE_RADIUS 300

LINK_ENTITY_TO_CLASS(AntimatherialMissile, CAntimatherialMissile);

void CAntimatherialMissile::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_ANTIMATHERIAL;
	pev->takedamage = DAMAGE_YES;
	pev->health = 20;
	pev->impulse = AM_MISSILE_ANNIHILATE_RADIUS;
	pev->frags = 0;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 3.0f);
	pev->nextthink = gpGlobals->time+0.3;

	SetTouch(&CAntimatherialMissile::ExplodeTouch);
	SetThink (&CAntimatherialMissile::Fly);
}

CAntimatherialMissile *CAntimatherialMissile::ShootAntimatherialMissile(CBaseEntity *pOwner, const Vector &vecStart, CBaseEntity *pEnemy, float dmg)
{
	CAntimatherialMissile *pNew = GetClassPtr((CAntimatherialMissile *)NULL, "AntimatherialMissile");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_ANTIMATHERIAL_MISSILE;
		pNew->m_hEnemy = pEnemy;
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		pNew->pev->dmg_save = dmg;
	}
	return pNew;
}

void CAntimatherialMissile::Fly( void)
{
	if (m_hEnemy != NULL)
	{
		if (m_hEnemy->IsAlive())
			CGrenade::MovetoTarget(m_hEnemy->Center(), SPEED_ANTIMATHERIAL_MISSILE);
		else
			m_hEnemy = NULL;
	}
	
	if (pev->frags == 0)
	{
		FX_Trail(pev->origin, entindex(), FX_ANTIMATHERIALMISSILE);
		pev->frags = 1;
	}
	pev->angles = UTIL_VecToAngles(pev->velocity);
	pev->nextthink = gpGlobals->time + 0.05;
}

int CAntimatherialMissile::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();

		CGrenade::Blast(pev->dmg*0.8, pev->dmg*3.5, pev->dmg, pev->dmg*1.25, DMG_ENERGYBLAST, FX_ANTIMATHERIALMISSILE_BLAST);
	return 1;
}

void CAntimatherialMissile::ExplodeTouch(CBaseEntity *pOther)
{
	CGrenade::Impact(pOther, TRUE, pev->dmg*0.5, pev->dmg*6.0, TRUE, RANDOM_LONG(DECAL_SCORCH1,DECAL_SCORCH3), 0, 0, 0, 0, 0, 24.0, FX_ANTIMATHERIALMISSILE_DETONATE);
	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	SetThink(BlackHole);
	pev->nextthink = gpGlobals->time;
}

void CAntimatherialMissile::BlackHole(void)
{
	if (!IsInWorld() || POINT_CONTENTS(pev->origin) == CONTENTS_WATER)
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}
	CBaseEntity *pEnt = NULL;
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	TraceResult tr;
	bool fractionpassed = false;

	while ((pEnt = UTIL_FindEntityInSphere(pEnt, pev->origin, AM_MISSILE_GRAVITY_RADIUS)) != NULL)
	{
		UTIL_TraceLine(pev->origin, pEnt->Center(), ignore_monsters, dont_ignore_glass, ENT(pev), &tr);

		if (tr.flFraction == 1.0f)
			fractionpassed = true;
		else if (tr.pHit == pEnt->edict())
			fractionpassed = true;// the entity is blocking trace line itself
		else
			fractionpassed = false;

		if (fractionpassed && !FBitSet(pEnt->pev->effects, EF_NODRAW) && pEnt != this)
		{
			Vector delta = pev->origin - pEnt->pev->origin;
			float distance = delta.Length();

			if (pEnt->pev->team == pOwner->pev->team) // don't hit my teammates
				continue;
			
			if (pEnt->IsProjectile())// String comparsion is MUCH slower...
			{
				if (FClassnameIs(pEnt->pev, "AntimatherialMissile"))
				{
					continue;// Immune
				}
				else
				{
					float k = distance*4.0f;
					pEnt->pev->velocity = pEnt->pev->velocity*0.5f + delta.Normalize()*k;
					pEnt->pev->movetype = MOVETYPE_TOSS;
					
					if (distance < pev->impulse)
						pEnt->AnnihilateProj();
				}
			}
			else if (pEnt->IsBSPModel())
			{
				delta = pev->origin - pEnt->Center();
			}
			if (pEnt->IsPushable())
			{
				float k = distance*4.0f;
				pEnt->pev->velocity = pEnt->pev->velocity*0.5f + delta.Normalize()*k;

				if (pEnt->IsMonster() && !(pEnt->pev->flags & FL_GODMODE))// XDM3035: invincible entities (scripted/multiplayer)
				{
					pEnt->pev->movetype = MOVETYPE_TOSS;// TODO: set for all ents, not just monsters? But this disables noclip for players =)
					if (pEnt->IsAlive() && pEnt->pev->takedamage != DAMAGE_NO && distance < pev->impulse)
					{
						pEnt->MyMonsterPointer()->SUB_Remove();
					}
				}
			}
			if (pEnt->pev->takedamage != DAMAGE_NO)
			{
				CGrenade::Impact(pEnt, FALSE, 0, 0, FALSE, 0, pev->dmg*0.01, DMG_PARALYZE, 0, 0, 0, 0.0, 0);
	
				if (distance < pev->impulse)
					CGrenade::Impact(pEnt, FALSE, 0, 0, FALSE, 0, pev->dmg, DMG_ANNIHILATION, 0, 0, 0, 0.0, 0);
			}
		}
	}

	if (pev->impulse <= 0)
	{
		CGrenade::Blast(0, 0, pev->dmg_save*0.5, pev->dmg_save, DMG_SONIC | DMG_WALLPIERCING, FX_ANTIMATHERIALMISSILE_BLACKHOLE_DETONATE);
	}
	else
	{
		pev->impulse-=6;
		pev->dmg *= 0.92;
		pev->nextthink = gpGlobals->time + 0.1f;
	}
}