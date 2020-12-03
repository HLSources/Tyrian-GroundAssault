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
/*

===== combat.cpp ========================================================

  functions dealing with damage infliction & death

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "sound.h"
#include "soundent.h"
#include "decals.h"
#include "animation.h"
#include "weapons.h"
#include "pm_materials.h"
#include "gamerules.h"// XDM
#include "globals.h"
#include "game.h"
#include "player.h"
#include "msg_fx.h"

MULTIDAMAGE gMultiDamage;

/*
==============================================================================

MULTI-DAMAGE

Collects multiple small damages into a single damage

==============================================================================
*/

// ClearMultiDamage - resets the global multi damage accumulator
void ClearMultiDamage(void)
{
	gMultiDamage.pEntity = NULL;
	gMultiDamage.amount	= 0;
	gMultiDamage.type = 0;
}

// ApplyMultiDamage - inflicts contents of global multi damage register on gMultiDamage.pEntity
void ApplyMultiDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker)
{
	if (!gMultiDamage.pEntity)
		return;

	gMultiDamage.pEntity->TakeDamage(pInflictor, pAttacker, gMultiDamage.amount, gMultiDamage.type );
}

void AddMultiDamage(CBaseEntity *pInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType)
{
	if (pEntity == NULL)
		return;

	gMultiDamage.type |= bitsDamageType;

	if (pEntity != gMultiDamage.pEntity)
	{
		ApplyMultiDamage(pInflictor, pInflictor); // UNDONE: wrong attacker!
		gMultiDamage.pEntity	= pEntity;
		gMultiDamage.amount		= 0;
	}
	gMultiDamage.amount += flDamage;
}


//-----------------------------------------------------------------------------
// Purpose: Certain entities may provide special decals to be painted on them
// Input  : *pEntity - 
//			&bitsDamageType - DMG_GENERIC
// Output : int server decal index DECAL_GUNSHOT1
//-----------------------------------------------------------------------------
int DamageDecal(CBaseEntity *pEntity, const int &bitsDamageType)
{
	if (pEntity)
		return pEntity->DamageDecal(bitsDamageType);

	if (bitsDamageType & DMG_BLAST)// XDM
		return (DECAL_LARGESHOT1 + RANDOM_LONG(0,4));

	if (bitsDamageType & DMG_SLASH)
		return (DECAL_BIGSHOT1 + RANDOM_LONG(0,4));

	return (DECAL_GUNSHOT1 + RANDOM_LONG(0,4));
}

//-----------------------------------------------------------------------------
// Purpose: Paint a decal specific to a bullet type
// Input  : *pTrace - trace thet hits some surface
//			&iBulletType - Bullet enum (BULLET_NONE)
//-----------------------------------------------------------------------------
void DecalGunshot(TraceResult *pTrace, const int &iBulletType)
{
	if (!UTIL_IsValidEntity(pTrace->pHit))// Is the entity valid
		return;

	CBaseEntity *pEntity = CBaseEntity::Instance(pTrace->pHit);

//	if (VARS(pTrace->pHit)->solid == SOLID_BSP || VARS(pTrace->pHit)->movetype == MOVETYPE_PUSHSTEP)
	if (pEntity && pEntity->IsBSPModel())
	{
		if (iBulletType == BULLET_MINIGUN)
			UTIL_GunshotDecalTrace(pTrace, DamageDecal(pEntity, DMG_BLAST));// XDM: DMG_BLAST makes DECAL_LARGESHOT
		else if (iBulletType == BULLET_VULCAN)
			UTIL_DecalTrace(pTrace, DamageDecal(pEntity, DMG_SLASH));
		else
			UTIL_GunshotDecalTrace(pTrace, DamageDecal(pEntity, DMG_BULLET));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Inflict damage upon entities within a certain range (explosions, etc.)
// Input  : &vecSrc - center of the sphere
//			*pInflictor - attacker's gun
//			*pAttacker - the attacker
//			flDamage - 
//			flRadius - 
//			iClassIgnore - entity class to skip (CLASS_NONE)
//			bitsDamageType - damage bits (DMG_GENERIC)
//-----------------------------------------------------------------------------
void RadiusDamage(const Vector &vecSrc, CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType)
{
	TraceResult	tr;
	float flAdjustedDamage;//, falloff;
	CBaseEntity *pEntity = NULL;

	IGNORE_MONSTERS igm = dont_ignore_monsters;// XDM
	IGNORE_GLASS igg = dont_ignore_glass;
	if (bitsDamageType & (DMG_SONIC | DMG_ENERGYBEAM | DMG_RADIATION | DMG_WALLPIERCING))
	{
		igm = ignore_monsters;
		igg = ignore_glass;
	}
	else if (bitsDamageType & (DMG_BLAST | DMG_NERVEGAS))
	{
		igm = ignore_monsters;
	}

	// XDM: don't check my PointContents every time!
	byte bInWater = UTIL_LiquidContents(vecSrc);
	byte push = (bitsDamageType & DMGM_PUSH);

	if (!pAttacker)
		pAttacker = pInflictor;

	edict_t *pEntIgnore = NULL;
	if (pInflictor)
		pEntIgnore = ENT(pInflictor->pev);

	try
	{
	// iterate on all entities in the vicinity.
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, vecSrc, flRadius)) != NULL)
	{
		if (pEntity == pInflictor)// XDM: no self-destruction :) (endless loop!)
			continue;

		if (pEntity->pev->takedamage != DAMAGE_NO || (push && pEntity->IsPushable()))// do damage OR just push
		{
			// UNDONE: this should check a damage mask, not an ignore
			if (iClassIgnore != CLASS_NONE && pEntity->Classify() == iClassIgnore)
				continue;// houndeyes don't hurt other houndeyes with their attack

			UTIL_TraceLine(vecSrc, pEntity->BodyTarget(vecSrc), igm, igg, pEntIgnore, &tr);

			if (tr.flFraction == 1.0f || tr.pHit == pEntity->edict() || (bitsDamageType & DMG_WALLPIERCING))// XDM
			{
				if (tr.fStartSolid)// if we're stuck inside them, fixup the position and distance
				{
					tr.vecEndPos = vecSrc;
					tr.flFraction = 0.0f;
				}

				flAdjustedDamage = flDamage;
				Vector vecDelta = tr.vecEndPos - vecSrc;
				float len = vecDelta.Length();// = flRadius;
				if (len != 0.0f)
					vecDelta /= len;

				// decrease damage for an ent that's farther from the bomb.
				if (!FBitSet(bitsDamageType, DMG_RADIUS_MAX))// XDM
				{
					if (len > 2.0f)// HACK?!
						len -= 2.0f;

					flAdjustedDamage *= (1.0f - len/flRadius);

					if ((bitsDamageType & DMG_WALLPIERCING) && tr.flFraction < 1.0f)
						flAdjustedDamage *= tr.flFraction;
				}

				if (flAdjustedDamage <= 0.0f)
					continue;//flAdjustedDamage = 0;

				if (bInWater != (pEntity->pev->waterlevel > 2))// XDM: FIXED: blast's don't tavel into or out of water
					flAdjustedDamage *= 0.6f;//continue;

				float force = 0.0f;
				if (pEntity->IsPushable() && push && !FBitSet(pEntity->pev->flags, FL_FROZEN))
				{
					force = pEntity->DamageForce(flAdjustedDamage);
					pEntity->pev->velocity += vecDelta * force/3;//flAdjustedDamage * 3.0f;// XDM3035c: +=
					pEntity->pev->avelocity += (pEntity->Center() - tr.vecEndPos).Normalize() * force;// XDM3035c: +=
				}

				if (pEntity->pev->takedamage == DAMAGE_NO)
					continue;

				if (tr.flFraction != 1.0f)
				{
					ClearMultiDamage();
					pEntity->TraceAttack(pInflictor, flAdjustedDamage, vecDelta, &tr, bitsDamageType);
					ApplyMultiDamage(pInflictor, pAttacker);
				}
				else
					pEntity->TakeDamage(pInflictor, pAttacker, flAdjustedDamage, bitsDamageType);

				if (bitsDamageType & (DMG_FREEZE ) && !pEntity->pev->armorvalue)// XDM3035: moved here to make this global effect reusable
				{
					if (!pEntity->IsProjectile() && pEntity->Classify() != CLASS_NONE)
					{
						CBaseMonster *pVictim = pEntity->MyMonsterPointer();
						if (pVictim)
						{
							pVictim->FrozenStart(flAdjustedDamage*0.15f);
						}
					}
				}
			}
		}
	}
	}
	catch(char *str)
	{
		printf("RadiusDamage() exception: %s!\n", str);// at_error
		DBG_FORCEBREAK
	}
	catch(...)// GetExceptionCode()?
	{
		printf("RadiusDamage() unknown exception!\n");// at_error
		DBG_FORCEBREAK
	}
}


//-----------------------------------------------------------------------------
// Purpose: Go to the trouble of combining multiple pellets into a single damage call.
// Input  : cShots - number of bullets
//			&vecSrc - trace source
//			&vecDirShooting - 
//			&vecSpread - 
//			*endpos - output: hit position
//			flDistance - maximum distance
//			iBulletType - Bullet enum
//			iDamage - can override standard skill-based bullet damage
//			*pInflictor - attacker's gun
//			*pAttacker - the attacker
//			shared_rand - for players
// Output : Vector 
//-----------------------------------------------------------------------------
Vector FireBullets(ULONG cShots, const Vector &vecSrc, const Vector &vecDirShooting, const Vector &vecSpread, Vector *endpos, float flDistance, int iBulletType, int iDamage, int iDamageType, CBaseEntity *pInflictor, CBaseEntity *pAttacker, int shared_rand)
{
	if (pInflictor == NULL)
	{
#ifdef _DEBUG
		ALERT(at_console, "FireBullets() with no pInflictor!\n");
#endif
		return vecSpread;
	}

	if (pAttacker == NULL)
		pAttacker = pInflictor;// the default attacker is ourselves

	TraceResult tr;
	float x = 0.0f, y = 0.0f;
	edict_t *pentIgnore = NULL;
	if (pInflictor)
		pentIgnore = ENT(pInflictor->pev);

	ClearMultiDamage();

	for (ULONG iShot = 1; iShot <= cShots; ++iShot)
	{
		if (pAttacker->IsPlayer())//Use player's random seed. get circular gaussian spread
		{
			x = UTIL_SharedRandomFloat(shared_rand + iShot, -0.5f, 0.5f) + UTIL_SharedRandomFloat(shared_rand + (1 + iShot), -0.5f, 0.5f);
			y = UTIL_SharedRandomFloat(shared_rand + (2 + iShot), -0.5f, 0.5f) + UTIL_SharedRandomFloat(shared_rand + (3 + iShot), -0.5f, 0.5f);
		}
		else
		{
			x = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);
			y = RANDOM_FLOAT(-0.5,0.5) + RANDOM_FLOAT(-0.5,0.5);
		}

		Vector vecDir = vecDirShooting + x*vecSpread.x*gpGlobals->v_right + y*vecSpread.y*gpGlobals->v_up;
		Vector vecEnd = vecSrc + vecDir * flDistance;

		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, pentIgnore, &tr);
		vecEnd = tr.vecEndPos + vecDir;

		if (endpos)
		{
			endpos->x = tr.vecEndPos.x;
			endpos->y = tr.vecEndPos.y;
			endpos->z = tr.vecEndPos.z;
		}
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		int tex = (int)TEXTURETYPE_Trace(&tr, vecSrc, vecEnd);

		FX_FireBeam( vecSrc, tr.vecEndPos, tr.vecPlaneNormal, tex, iBulletType, pEntity->pev->takedamage?FALSE:TRUE);

		int pc = POINT_CONTENTS(tr.vecEndPos);
		if (pc < CONTENTS_SLIME)
			continue;// Don't draw any other fx if we're shooting at the sky or lava

		// do damage, paint decals
		if (tr.flFraction != 1.0f)
		{
			if (!pEntity)// XDM: ?
				continue;

			if (iBulletType == BULLET_BFG || iBulletType == BULLET_FLAK_CANNON)
			{
				::RadiusDamage(tr.vecEndPos, pInflictor, pAttacker, iDamage, iDamage*2.5, CLASS_NONE, iDamageType);
			}
			else if (iBulletType == BULLET_SHOCK || iBulletType == BULLET_HEAVY_TURRET || iBulletType == BULLET_BEAMSPLITTER)
			{
				::RadiusDamage(tr.vecEndPos, pInflictor, pAttacker, iDamage, iDamage*1.5, CLASS_NONE, iDamageType);
			}
			else if (iBulletType == BULLET_TAUCANNON || iBulletType == BULLET_LIGHTNING)
			{
				::RadiusDamage(tr.vecEndPos, pInflictor, pAttacker, iDamage, iDamage, CLASS_NONE, iDamageType);
			}
			else if (iBulletType == BULLET_CLUSTERGUN)
			{
				::RadiusDamage(tr.vecEndPos, pInflictor, pAttacker, iDamage, iDamage, CLASS_NONE, iDamageType);
				
				for (int i=0; i<12; ++i)
					FireBullets(1, tr.vecEndPos, tr.vecPlaneNormal, UTIL_RandomBloodVector(), NULL, RANDOM_FLOAT(512,1024), BULLET_FLAK_CANNON, gSkillData.DmgClusterGun*0.75, DMG_BLAST, pInflictor, pAttacker);
			}
			else
			{
				ClearMultiDamage();
				pEntity->TraceAttack(pAttacker, iDamage, vecDir, &tr, iDamageType);
				ApplyMultiDamage(pInflictor, pAttacker);
			}
		}

		// make bullet trails
		if ((pc <= CONTENTS_WATER && pc > CONTENTS_SKY))// XDM3035c: somehow fInWater is always 0
		{
			if (UTIL_LiquidContents(vecSrc))// water at start
				FX_BubblesLine(vecSrc, tr.vecEndPos, (flDistance * tr.flFraction) / 64.0f);
		}
	}

	ApplyMultiDamage(pInflictor, pAttacker);

	return Vector(x * vecSpread.x, y * vecSpread.y, 0.0f);
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &flDamage - 
//			&vecDir - 
//			*ptr - 
//			&bitsDamageType - 
//-----------------------------------------------------------------------------
void CBaseEntity::TraceBleed(const float &flDamage, const Vector &vecDir, TraceResult *ptr, const int &bitsDamageType)
{
	if (BloodColor() == DONT_BLEED)
		return;

	if (flDamage <= 0.0f)
		return;

	if (!(bitsDamageType & DMGM_BLEED))
		return;

// Already checked in TraceAttack()	if (bitsDamageType & DMG_DONT_BLEED)
//		return;

	TraceResult Bloodtr;
	Vector vecTraceDir;
	float flNoise;
	int cCount;
	int i;

	if (flDamage < 10.0f)
	{
		flNoise = 0.1f;
		cCount = 1;
	}
	else if (flDamage < 25.0f)
	{
		flNoise = 0.2f;
		cCount = 2;
	}
	else
	{
		flNoise = 0.3;
		cCount = 4;
/*
		if (ptr->iHitgroup == HITGROUP_HEAD)// XDM3035
		{
			Vector vecBlood = (ptr->vecEndPos - pev->origin).Normalize();
			UTIL_BloodStream(ptr->vecEndPos, vecBlood, BloodColor(), cCount + (int)(flDamage*0.5f));
		}*/
	}

	for (i = 0 ; i < cCount ; ++i)
	{
		vecTraceDir = -vecDir + UTIL_RandomVector()*flNoise;// trace in the opposite direction the shot came from (the direction the shot is going)
		UTIL_TraceLine(ptr->vecEndPos, ptr->vecEndPos + vecTraceDir * -172.0f, ignore_monsters, ENT(pev), &Bloodtr);
		if (Bloodtr.flFraction != 1.0f)
			UTIL_BloodDecalTrace(&Bloodtr, BloodColor());
	}
}



BOOL CBaseMonster::HasHumanGibs(void)
{
	if (IsHuman())
	{
		if (BloodColor() == BLOOD_COLOR_RED)
			return TRUE;
	}
	else if (Classify() == CLASS_GIB)// XDM3035c: dead monsters are classified like this
	{
		if (BloodColor() == BLOOD_COLOR_RED)
			return TRUE;
	}
	return FALSE;
}

BOOL CBaseMonster::HasAlienGibs(void)
{
	int myClass = Classify();
	if (myClass == CLASS_ALIEN_MILITARY ||
		myClass == CLASS_ALIEN_MONSTER	||
		myClass == CLASS_ALIEN_PASSIVE  ||
		myClass == CLASS_INSECT  ||
		myClass == CLASS_ALIEN_PREDATOR  ||
		myClass == CLASS_ALIEN_PREY)
	{
		if (BloodColor() != BLOOD_COLOR_RED)// XDM
			return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: create some gore and get rid of a monster's model
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBaseMonster::GibMonster(void)
{
	bool gibbed = false;

	// only humans throw skulls !!!UNDONE - eventually monsters will have their own sets of gibs
	if (HasHumanGibs())
	{
		{
			if (IsHuman() && !IsPlayer())// XDM3035c: players have their own special case
				CGib::SpawnHeadGib(this);

//			if (m_fFrozen)// XDM3035
//				FX_PlrGib( pev->origin, GIBBED_FROZEN);
//			else
//				FX_PlrGib( pev->origin, GIBBED_BODY);
		}
		gibbed = true;
	}
	else if (HasAlienGibs())
	{

		{
//			if (m_fFrozen)// Ghoul: Must add new client-sided gibs for aliens!!!
//				FX_PlrGib( pev->origin, GIBBED_FROZEN_ALIEN);
//			else
//				FX_PlrGib( pev->origin, GIBBED_BODY_ALIEN);
	  }
		gibbed = true;
	}

	if (HasCustomGibs())
	{
		int m = matNone;
		if (BloodColor() != DONT_BLEED)
			m = matFlesh;

		CGib::SpawnModelGibs(this, pev->origin, pev->mins, pev->maxs, g_vecAttackDir * -1, 10, m_iGibModelIndex, -1, 4.0, m, BloodColor(), 0);
	}

	if (!IsPlayer())// don't remove players!
	{
		if (gibbed)
		{
			SetThink(&CBaseEntity::SUB_Remove);
			pev->nextthink = gpGlobals->time;
		}
		else
			FadeMonster();
	}
	return gibbed;
}

//-----------------------------------------------------------------------------
// Purpose: Just fade out
//-----------------------------------------------------------------------------
void CBaseMonster::FadeMonster(void)
{
	StopAnimation();
	pev->velocity = g_vecZero;
	pev->movetype = MOVETYPE_NONE;
	pev->avelocity = g_vecZero;
	pev->animtime = gpGlobals->time;
	pev->effects |= EF_NOINTERP;
	SUB_StartFadeOut();
}

//-----------------------------------------------------------------------------
// Purpose: for everybody to hear
//-----------------------------------------------------------------------------
void CBaseMonster::DeathSound(void)
{
//STFU?	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/null.wav", VOL_NORM, ATTN_NORM);
}

//=========================================================
// GetDeathActivity - determines the best type of death
// anim to play.
//=========================================================
Activity CBaseMonster::GetDeathActivity(void)
{
	if (pev->deadflag != DEAD_NO)
	{
		// don't run this while dying.
		return m_IdealActivity;
	}

	Vector vecSrc = Center();
	bool fTriedDirection = false;
	Activity deathActivity = ACT_DIESIMPLE;// in case we can't find any special deaths to do.

	UTIL_MakeVectors ( pev->angles );
	float flDot = DotProduct ( gpGlobals->v_forward, g_vecAttackDir * -1 );
	TraceResult	tr;

	switch ( m_LastHitGroup )
	{
		// try to pick a region-specific death.
	case HITGROUP_HEAD:
		deathActivity = ACT_DIE_HEADSHOT;
		break;
	case HITGROUP_CHEST:
		{
			if (pev->health < (-0.2f*pev->max_health)/*-20*/)// XDM3034
				deathActivity = ACT_DIEVIOLENT;
			else
				deathActivity = ACT_DIE_CHESTSHOT;
		}
		break;
	case HITGROUP_STOMACH:
		deathActivity = ACT_DIE_GUTSHOT;
		break;

	default:
//	case HITGROUP_GENERIC:
		// try to pick a death based on attack direction
		fTriedDirection = true;

		if (flDot > 0.3)// shot from behind, fall forward
		{
			deathActivity = ACT_DIEFORWARD;
		}
		else if (flDot <= -0.3)
		{
			if (pev->health < (-0.15f*pev->max_health)/*-15*/ && (m_LastHitGroup == HITGROUP_GENERIC || m_LastHitGroup == HITGROUP_ARMOR))// XDM3034: ACT_DIEVIOLENT not for arms or legs
				deathActivity = ACT_DIEVIOLENT;
			else
				deathActivity = ACT_DIEBACKWARD;// don't use: ACT_DIE_BACKSHOT?
		}
		break;
	}

	// can we perform the prescribed death?
	if ( LookupActivity( deathActivity ) == ACTIVITY_NOT_AVAILABLE )
	{
		// no! did we fail to perform a directional death?
		if ( fTriedDirection )
		{
			// if yes, we're out of options. Go simple.
			deathActivity = ACT_DIESIMPLE;
		}
		else
		{
			// cannot perform the ideal region-specific death, so try a direction.
			if ( flDot > 0.3 )
			{
				deathActivity = ACT_DIEFORWARD;
			}
			else if ( flDot <= -0.3 )
			{
				deathActivity = ACT_DIEBACKWARD;
			}
		}
	}

	if ( LookupActivity ( deathActivity ) == ACTIVITY_NOT_AVAILABLE )
	{
		// if we're still invalid, simple is our only option.
		deathActivity = ACT_DIESIMPLE;
	}

	if ( deathActivity == ACT_DIEFORWARD )
	{
			// make sure there's room to fall forward
			UTIL_TraceHull ( vecSrc, vecSrc + gpGlobals->v_forward * 64, dont_ignore_monsters, head_hull, edict(), &tr );

			if ( tr.flFraction != 1.0 )
				deathActivity = ACT_DIESIMPLE;
	}
	else if ( deathActivity == ACT_DIEBACKWARD )
	{
			// make sure there's room to fall backward
			UTIL_TraceHull ( vecSrc, vecSrc - gpGlobals->v_forward * 64, dont_ignore_monsters, head_hull, edict(), &tr );

			if ( tr.flFraction != 1.0 )
				deathActivity = ACT_DIESIMPLE;
	}
	return deathActivity;
}

//=========================================================
// GetSmallFlinchActivity - determines the best type of flinch
// anim to play.
//=========================================================
Activity CBaseMonster::GetSmallFlinchActivity ( void )
{
	Activity	flinchActivity;

	switch (m_LastHitGroup)
	{
		// pick a region-specific flinch
	case HITGROUP_HEAD:
		flinchActivity = ACT_FLINCH_HEAD;
		break;
	case HITGROUP_STOMACH:
		flinchActivity = ACT_FLINCH_STOMACH;
		break;
	case HITGROUP_LEFTARM:
		flinchActivity = ACT_FLINCH_LEFTARM;
		break;
	case HITGROUP_RIGHTARM:
		flinchActivity = ACT_FLINCH_RIGHTARM;
		break;
	case HITGROUP_LEFTLEG:
		flinchActivity = ACT_FLINCH_LEFTLEG;
		break;
	case HITGROUP_RIGHTLEG:
		flinchActivity = ACT_FLINCH_RIGHTLEG;
		break;
	case HITGROUP_GENERIC:
	default:
		// just get a generic flinch.
		flinchActivity = ACT_SMALL_FLINCH;
		break;
	}
	// do we have a sequence for the ideal activity?
	if (LookupActivity(flinchActivity) == ACTIVITY_NOT_AVAILABLE)
		flinchActivity = ACT_SMALL_FLINCH;

	return flinchActivity;
}

void CBaseMonster::BecomeDead(void)
{
//	ALERT(at_console, "%s: BecomeDead()\n", STRING(pev->classname));
	pev->takedamage = DAMAGE_YES;// don't let autoaim aim at corpses.

	// give the corpse half of the monster's original maximum health.
//	pev->health = pev->max_health / 2;
//	pev->max_health = 5; // XDM3035: HELL NO! max_health now becomes a counter for how many blood decals the corpse can place.

	if (pev->movetype == MOVETYPE_WALK
		|| pev->movetype == MOVETYPE_STEP
		|| pev->movetype == MOVETYPE_TOSS)
	{
		AlignToFloor();// XDM
		// make the corpse fly away from the attack vector
		pev->movetype = MOVETYPE_TOSS;
		//pev->flags &= ~FL_ONGROUND;
		//pev->origin.z += 2;
		//pev->velocity = g_vecAttackDir * -1;
		//pev->velocity = pev->velocity * RANDOM_FLOAT( 300, 400 );

//	nope, this fails the animation and other stuff	SetThink(&CBaseMonster::CorpseFallThink);// XDM3035a
	}

/*	if (ShouldRespawn())// XDM3035
	{
//No! This disables AI and animations!		SetThink(&CBaseEntity::SUB_Respawn);
//		pev->nextthink = gpGlobals->time + mp_monsrespawntime.value;// XDM: TODO
		m_flRespawnTime = gpGlobals->time + mp_monsrespawntime.value;
	}*/
}

BOOL CBaseMonster::ShouldGibMonster(int iGib)
{
	if ((iGib == GIB_NORMAL && pev->health < (-0.5f*pev->max_health)) || iGib == GIB_ALWAYS)
		return TRUE;

	return FALSE;
}

void CBaseMonster::CallGibMonster(void)
{
	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_NOT;// do something with the body. while monster blows up
	pev->effects = EF_NODRAW; // make the model invisible.
	GibMonster();

		pev->deadflag = DEAD_DEAD;
	FCheckAITrigger();

	// don't let the status bar glitch for players.with <0 health.
	if (pev->health < -99)
		pev->health = 0;

	if (ShouldFadeOnDeath())// && !fade)
		UTIL_Remove(this);
}

BOOL CBaseMonster::ShouldRespawn(void)// XDM3035
{
	if (g_pGameRules->IsMultiplayer())
	{
		if (!IsMonster())
			return FALSE;
		if (IsProjectile())
			return FALSE;
		if (IsPlayer())
			return FALSE;
		if (pev->spawnflags & SF_NORESPAWN)
			return FALSE;
		if (g_pGameRules->IsGameOver())// XDM3035c
			return FALSE;

		int c = Classify();// BAD! HACK!
		if (c != CLASS_NONE && c != CLASS_PLAYER)// && c <= CLASS_PLAYER_ALLY XDM3035b: corpses has CLASS_GIB!
		{
			if (mp_monstersrespawn.value > 0.0f)
				return TRUE;
		}
	}
	return FALSE;
}

/*
============
Killed
============
*/
void CBaseMonster::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)// NEW
{
	if (HasMemory(bits_MEMORY_KILLED))
	{
		if (ShouldGibMonster(iGib))
			CallGibMonster();

		return;
	}

	if (m_fFrozen)// XDM
		FrozenEnd();

	Remember(bits_MEMORY_KILLED);

	// clear the deceased's sound channels.(may have been firing or reloading when killed)
	EMIT_SOUND(ENT(pev), CHAN_WEAPON, "common/null.wav", VOL_NORM, ATTN_NORM);

	m_IdealMonsterState = MONSTERSTATE_DEAD;

	// Make sure this condition is fired too (TakeDamage breaks out before this happens on death)
	SetConditions(bits_COND_LIGHT_DAMAGE);

	// XDM3035:
	g_pGameRules->MonsterKilled(this, pAttacker, pInflictor);

	CSoundEnt::InsertSound(bits_SOUND_DEATH, pev->origin, 256, 1.0f);// XDM

	// tell owner ( if any ) that we're dead.This is mostly for MonsterMaker functionality.
	if (!FNullEnt(pev->owner))
	{
		CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
		if (pOwner)
			pOwner->DeathNotice(this);
	}
	if (iGib == GIB_DISINTEGRATE)
	{
		pev->deadflag = DEAD_DEAD;
		FCheckAITrigger();// XDM3035c: may need to fire some conditional triggers
		Disintegrate();
		return;
	}
	else if (ShouldGibMonster(iGib))
	{
		CallGibMonster();
		return;
	}
	else if (pev->flags & FL_MONSTER)
	{
		SetTouchNull();
		BecomeDead();
//		pev->deadflag = DEAD_DYING;// XDM3035 no! :(
	}

	// don't let the status bar glitch for players.with <0 health.
	if (pev->health < -99)
		pev->health = 0;

	//pev->enemy = ENT( pAttacker );//why? (sjb)
	m_IdealMonsterState = MONSTERSTATE_DEAD;
}

// XDM: fadeout: moved to subs.cpp

// take health
int CBaseMonster::TakeHealth(const float &flHealth, const int &bitsDamageType)
{
	if (!pev->takedamage)
		return 0;

	// clear out any damage types we healed.
	// UNDONE: generic health should not heal any
	// UNDONE: time-based damage
	m_bitsDamageType &= ~(bitsDamageType & ~DMG_TIMEBASED);
	return CBaseEntity::TakeHealth(flHealth, bitsDamageType);
}

/*
============
TakeDamage

The damage is coming from inflictor, but get mad at attacker
This should be the only function that ever reduces health.
bitsDamageType indicates the type of damage sustained, ie: DMG_SHOCK

Time-based damage: only occurs while the monster is within the trigger_hurt.
When a monster is poisoned via an arrow etc it takes all the poison damage at once.
============
*/
int CBaseMonster::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (g_pGameRules->IsGameOver() && !IsPlayer())// XDM: players have g_pGameRules->FPlayerCanTakeDamage
		return 0;

	if (pev->takedamage == DAMAGE_NO)
		return 0;

//	if (bitsDamageType & DMG_BURN)// XDM
//		m_flBurnDamage = flDamage;
//	ALERT(at_console, "CBaseMonster(%d)::TakeDamage(%f), pev->deadflag = %d, health was %f of %f\n", entindex(), flDamage, pev->deadflag, pev->health, pev->max_health);

	if (pev->deadflag != DEAD_NO)// !IsAlive()) // XDM3035: DEAD_DYING too!
		return DeadTakeDamage(pInflictor, pAttacker, flDamage, bitsDamageType);

	if (pev->deadflag == DEAD_NO)// no pain sound during death animation.
		PainSound();// "Ouch!"

	//!!!LATER - make armor consideration here!
	float	flTake;
	Vector	vecDir;

	if (IsHuman() && bitsDamageType & DMG_DROWN)// XDM: this is called once per second
	{
		switch (RANDOM_LONG(0,3))
		{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_swim1.wav", 0.8, ATTN_STATIC); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_swim2.wav", 0.8, ATTN_STATIC); break;
			case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_swim3.wav", 0.8, ATTN_STATIC); break;
			case 3:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_swim4.wav", 0.8, ATTN_STATIC); break;
		}
		vecDir = EyePosition();// tmp
		FX_BubblesBox(vecDir, Vector(2,2,2), 20);//UTIL_Bubbles(vecDir - Vector(2,2,2), vecDir + Vector(2,2,2), 20);
	}

	if (pev->armorvalue > 0 && !(bitsDamageType & (DMG_NERVEGAS | DMG_DROWN | DMG_IGNOREARMOR)))// XDM: monsters now have armor!
	{
		int val = 0;
		if (bitsDamageType & DMG_BULLET)
			val = flDamage * 0.3f;

		if (bitsDamageType & DMG_SLASH)
			val = flDamage * 0.25f;

		if (pev->armorvalue < val)
			pev->armorvalue = 0.0f;
		else
			pev->armorvalue -= val;

		flTake = flDamage - val;
	}
	else
		flTake = flDamage;

//	ALERT(at_console, "CBaseMonster::TakeDamage(%f %f)\n", flDamage, flTake);

	// set damage type sustained
	m_bitsDamageType |= bitsDamageType;

	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	vecDir = g_vecZero;
	if (pInflictor)
	{
		vecDir = (pInflictor->Center() - Vector(0,0,10) - Center()).Normalize();
		vecDir = g_vecAttackDir = vecDir.Normalize();
	}

	if (FBitSet(bitsDamageType, DMG_BURN))// XDM
	{
		if (pev->waterlevel < 2)// XDM3035a
		{
			if (m_flBurnTime <= gpGlobals->time)//m_flBurnTime >= 0.0f && - useless here in TakeDamage()
			{
				m_flBurnTime = gpGlobals->time + RANDOM_FLOAT(1.8f, 2.2f);// this sound is 1.0sec length
			}
		}
		if (m_fFrozen)
			FrozenEnd();

		if (!IsPlayer())
		{
			ClearConditions(bits_COND_SEE_ENEMY);
			SetConditions(bits_COND_HEAVY_DAMAGE);
		}
//		ChangeSchedule(slTakeCoverFromOrigin);
	}

	// add to the damage total for clients, which will be sent as a single message at the end of the frame
	// TODO: remove after combining shotgun blasts?
	if (IsPlayer())
	{
		if (pInflictor)// XDM: why only players?
			pev->dmg_inflictor = ENT(pInflictor->pev);

		pev->dmg_take += flTake;

		// check for godmode or invincibility
		if (pev->flags & FL_GODMODE)
			return 0;
	}

	if (FBitSet(pev->spawnflags, SF_MONSTER_INVULNERABLE))// monsters only
		flTake = 0;// XDM: don't return. Monsters must react to the damage

	// if this is a player, move him around! XDM3035: not just player
	if (bitsDamageType & DMGM_PUSH)// XDM3035
	{
		if (pInflictor && IsPushable() && (!pAttacker || pAttacker->pev->solid != SOLID_TRIGGER))
			pev->velocity = pev->velocity + vecDir * -DamageForce(flDamage);
	}

//	int fTookDamage = flTake;
	// do the damage
	pev->health -= flTake;

	// HACKHACK Don't kill monsters in a script.  Let them break their scripts first
	if (m_MonsterState == MONSTERSTATE_SCRIPT)
	{
		SetConditions(bits_COND_LIGHT_DAMAGE);
		return 0;
	}

	if (pev->health <= 0)
	{
// XDM3034: obsolete		g_pevLastInflictor = pInflictor->pev;

		if (bitsDamageType & DMG_ALWAYSGIB )// XDM: instagib mode
			Killed(pInflictor, pAttacker, GIB_ALWAYS);
		else if (bitsDamageType & DMG_NEVERGIB)
			Killed(pInflictor, pAttacker, GIB_NEVER);
		else if (bitsDamageType & DMG_VAPOURIZING)// XDM3034
			Killed(pInflictor, pAttacker, GIB_VAPOURIZE);
		else if (bitsDamageType & DMG_DISINTEGRATING)// XDM3035
			Killed(pInflictor, pAttacker, GIB_DISINTEGRATE);
		else if (bitsDamageType & DMG_ANNIHILATION)
			Killed(pInflictor, pAttacker, GIB_REMOVE);
		else if (bitsDamageType & DMG_PLASMA)
			Killed(pInflictor, pAttacker, GIB_MELT);
		else if (bitsDamageType & DMG_SONIC)
			Killed(pInflictor, pAttacker, GIB_SONIC);
		else if (bitsDamageType & (DMG_BLAST | DMG_MORTAR))
			Killed(pInflictor, pAttacker, GIB_BLAST);
		else if (bitsDamageType & DMG_ENERGYBLAST)
			Killed(pInflictor, pAttacker, GIB_ENERGYBLAST);
		else if (bitsDamageType & DMG_ENERGYBEAM)
			Killed(pInflictor, pAttacker, GIB_ENERGYBEAM);
		else if (bitsDamageType & DMG_SHOCK)
			Killed(pInflictor, pAttacker, GIB_ELECTRO);
		else if (bitsDamageType & DMG_RADIATION)
			Killed(pInflictor, pAttacker, GIB_RADIATION);
		else if (bitsDamageType & (DMG_ACID | DMG_POISON))
			Killed(pInflictor, pAttacker, GIB_ACID);
		else if (bitsDamageType & (DMG_FREEZE | DMG_SLOWFREEZE))
			Killed(pInflictor, pAttacker, GIB_FROZEN);
		else if (bitsDamageType & (DMG_BURN | DMG_SLOWBURN))
			Killed(pInflictor, pAttacker, GIB_BURN);
		else if (bitsDamageType & DMG_NEVERGIB)
			Killed(pInflictor, pAttacker, GIB_NEVER);
		else
			Killed(pInflictor, pAttacker, GIB_NORMAL);

//		g_pevLastInflictor = NULL;
		return 0;// XDM3035c: revisit: WHY?!
	}
/*
	if ((bitsDamageType & DMGM_VISIBLE) && g_pGameRules->FAllowEffects() && flTake > 2.0f)
	{
		MESSAGE_BEGIN(MSG_PVS, gmsgDamageFx, pev->origin);// XDM3035c
			WRITE_SHORT(entindex());
			WRITE_LONG(bitsDamageType);
			WRITE_COORD(flDamage);
		MESSAGE_END();
	}
*/
	// react to the damage (get mad)
	if ((pev->flags & FL_MONSTER) && pAttacker)
	{
		if (pAttacker->IsMonster() || pAttacker->IsPlayer())// only if the attack was a monster or client!
		{
			// enemy's last known position is somewhere down the vector that the attack came from.
			if (pInflictor)
			{
				if (m_hEnemy == NULL || pInflictor == m_hEnemy || !HasConditions(bits_COND_SEE_ENEMY))
					m_vecEnemyLKP = pInflictor->pev->origin;
			}
			else
				m_vecEnemyLKP = pev->origin + (g_vecAttackDir * 64.0f);

			MakeIdealYaw(m_vecEnemyLKP);

			if (IsHeavyDamage(flDamage, bitsDamageType))// XDM
				SetConditions(bits_COND_HEAVY_DAMAGE);
			else
				SetConditions(bits_COND_LIGHT_DAMAGE);
		}
	}
	return 1;
}

//=========================================================
// DeadTakeDamage - takedamage function called when a monster's
// corpse is damaged.
//=========================================================
int CBaseMonster::DeadTakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	// grab the vector of the incoming attack. ( pretend that the inflictor is a little lower than it really is, so the body will tend to fly upward a bit).
	Vector vecDir = g_vecZero;

	if (pInflictor)
	{
		vecDir = (pInflictor->Center() - Vector(0, 0, 10) - Center()).Normalize();
//		vecDir = g_vecAttackDir = vecDir.Normalize();
		g_vecAttackDir = vecDir;
	}

	if (bitsDamageType & DMG_NEVERGIB)
		return 1;

/*
#if 0// turn this back on when the bounding box issues are resolved.

	pev->flags &= ~FL_ONGROUND;
	pev->origin.z += 1;

	// let the damage scoot the corpse around a bit.
	if ( !FNullEnt(pInflictor) && (pAttacker->pev->solid != SOLID_TRIGGER) )
		pev->velocity = pev->velocity + vecDir * -DamageForce( flDamage );

#endif
*/
	if (flDamage > 0.0f)
	{
		// Accumulate corpse gibbing damage, so you can gib with multiple hits
		pev->health -= flDamage * 0.2f;// XDM3035: DMG_NEVERGIB was already discarded

		// kill the corpse if enough damage was done to destroy the corpse and the damage is of a type that is allowed to destroy the corpse.
		if (bitsDamageType & DMGM_GIB_CORPSE)
		{
//			if (flDamage >= pev->health)// XDM: FIXME
//			if (pev->health < -0.75f*pev->max_health)// XDM
			if (ShouldGibMonster(GIB_NORMAL))// XDM3035
			{
				pev->health = -50.0f;// WTF?
				Killed(pInflictor, pAttacker, GIB_ALWAYS);
				return 0;
			}
		}
		else if (bitsDamageType & DMG_DISINTEGRATING)// XDM3035
		{
			if (flDamage >= pev->health)
			{
				pev->health = 0.0f;
				Killed(pInflictor, pAttacker, GIB_DISINTEGRATE);
			}
		}
			else if (bitsDamageType & DMG_ANNIHILATION)
		{
			if (flDamage >= pev->health)
			{
				pev->health = 0.0f;
				Killed(pInflictor, pAttacker, GIB_REMOVE);
			}
		}
	}
	return 1;
}

void CBaseMonster::RadiusDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int iClassIgnore, int bitsDamageType)
{
	::RadiusDamage(pev->origin, pInflictor, pAttacker, flDamage, flDamage * 2.5f, iClassIgnore, bitsDamageType);
}

void CBaseMonster::RadiusDamage(const Vector &vecSrc, CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int iClassIgnore, int bitsDamageType)
{
	::RadiusDamage(vecSrc, pInflictor, pAttacker, flDamage, flDamage * 2.5f, iClassIgnore, bitsDamageType);
}

//=========================================================
// CheckTraceHullAttack - expects a length to trace, amount
// of damage to do, and damage type. Returns a pointer to
// the damaged entity in case the monster wishes to do
// other stuff to the victim (punchangle, etc)
//
// Used for many contact-range melee attacks. Bites, claws, etc.
//=========================================================
CBaseEntity *CBaseMonster::CheckTraceHullAttack(const float &flDist, const int &iDamage, const int &iDmgType)
{
	TraceResult tr;
	if (IsPlayer())
		UTIL_MakeVectors(pev->angles);
	else
		UTIL_MakeAimVectors(pev->angles);

	Vector vecStart = pev->origin;
	vecStart.z += pev->size.z * 0.5f;
	Vector vecEnd = vecStart + (gpGlobals->v_forward * flDist);

	UTIL_TraceHull(vecStart, vecEnd, dont_ignore_monsters, head_hull, ENT(pev), &tr);

	if (tr.pHit)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

		if (iDamage > 0)
			pEntity->TakeDamage(this, this, iDamage, iDmgType);

		return pEntity;
	}
	return NULL;
}

//=========================================================
// FInViewCone - returns true is the passed ent is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall.
//=========================================================
BOOL CBaseMonster::FInViewCone(CBaseEntity *pEntity)
{
	return FInViewCone(pEntity->pev->origin);// XDM3035c
}

//=========================================================
// FInViewCone - returns true is the passed vector is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall.
//=========================================================
BOOL CBaseMonster::FInViewCone(const Vector &origin)
{
	Vector2D	vec2LOS;
	float		flDot;
	UTIL_MakeVectors(pev->angles);
	vec2LOS = (origin - pev->origin).Make2D();
	vec2LOS = vec2LOS.Normalize();
	flDot = DotProduct(vec2LOS, gpGlobals->v_forward.Make2D());

	if (flDot > m_flFieldOfView)
		return TRUE;

	return FALSE;
}

//=========================================================
// TraceAttack
//=========================================================
void CBaseMonster::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
//	ALERT(at_console, "TraceAttack: %s %f", STRING(pev->classname), flDamage);
	if (pev->takedamage)
	{
		m_LastHitGroup = ptr->iHitgroup;
		if (pev->armorvalue)
			flDamage *= gSkillData.plrShield;
		else
		switch (ptr->iHitgroup)
		{
		case HITGROUP_GENERIC:
			break;
		case HITGROUP_HEAD:
			flDamage *= gSkillData.plrHead;
			break;
		case HITGROUP_CHEST:
			flDamage *= gSkillData.plrChest;
			break;
		case HITGROUP_STOMACH:
			flDamage *= gSkillData.plrStomach;
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			flDamage *= gSkillData.plrArm;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			flDamage *= gSkillData.plrLeg;
			break;
/*		default:
			break;*/
		}
//		ALERT(at_console, "appdmg %f\n", flDamage);

		if (ptr->iHitgroup != HITGROUP_ARMOR && BloodColor() != DONT_BLEED && !(bitsDamageType & DMG_DONT_BLEED))// XDM3033: armor check
		{
			UTIL_BloodDrips(ptr->vecEndPos, -vecDir, BloodColor(), flDamage);// a little surface blood.
			TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
		}
		AddMultiDamage(pAttacker, this, flDamage, bitsDamageType);
	}
}
