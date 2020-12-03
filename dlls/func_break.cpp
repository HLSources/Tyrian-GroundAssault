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

===== bmodels.cpp ========================================================

  spawn, think, and use functions for entities that use brush models

*/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "pm_materials.h"
#include "pm_shared.h"
#include "func_break.h"
#include "decals.h"
#include "weapons.h"// XDM
#include "explode.h"
#include "game.h"
#include "sound.h"
#include "soundent.h"
#include "gamerules.h"
#include "msg_fx.h"
#include "globals.h" 

extern DLL_GLOBAL Vector		g_vecAttackDir;

// =================== FUNC_Breakable ==============================================

// Just add more items to the bottom of this array and they will automagically be supported
// This is done instead of just a classname in the FGD so we can control which entities can
// be spawned, and still remain fairly flexible
const char *CBreakable::pSpawnObjects[] =
{
	NULL				
};

//
// func_breakable - bmodel that breaks into pieces after taking damage
//
LINK_ENTITY_TO_CLASS( func_breakable, CBreakable );

TYPEDESCRIPTION CBreakable::m_SaveData[] =
{
	DEFINE_FIELD( CBreakable, m_bServerGibs, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBreakable, m_iTrail, FIELD_INTEGER ),
	DEFINE_FIELD( CBreakable, m_iShards, FIELD_INTEGER ),
	DEFINE_FIELD( CBreakable, m_fShardSize, FIELD_FLOAT ),
	DEFINE_FIELD( CBreakable, m_Material, FIELD_INTEGER ),
	DEFINE_FIELD( CBreakable, m_iExplosion, FIELD_INTEGER ),
// Don't need to save/restore these because we precache after restore
//	DEFINE_FIELD( CBreakable, m_idShard, FIELD_INTEGER ),
	DEFINE_FIELD( CBreakable, m_angle, FIELD_FLOAT ),
	DEFINE_FIELD( CBreakable, m_iszGibModel, FIELD_STRING ),
//	DEFINE_FIELD( CBreakable, m_iszSpawnObject, FIELD_STRING ),
	DEFINE_FIELD( CBreakable, m_iSpawnObject, FIELD_INTEGER ),// XDM3035c: index in pSpawnObjects array
	// Explosion magnitude is stored in pev->impulse
};

IMPLEMENT_SAVERESTORE( CBreakable, CBaseEntity );

void CBreakable::KeyValue( KeyValueData* pkvd )
{
	// UNDONE_WC: explicitly ignoring these fields, but they shouldn't be in the map file!
	if (FStrEq(pkvd->szKeyName, "explosion"))
	{
		if (!stricmp(pkvd->szValue, "directed"))
			m_iExplosion = EXPLOSION_DIRECTED;
//		else if (!stricmp(pkvd->szValue, "random"))
//			m_iExplosion = EXPLOSION_RANDOM;
		else
			m_iExplosion = EXPLOSION_RANDOM;

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "material"))
	{
		int i = atoi( pkvd->szValue);

		// 0:glass, 1:metal, 2:flesh, 3:wood

		if ((i < 0) || (i >= matLastMaterial))
			m_Material = matWood;
		else
			m_Material = (Materials)i;

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "deadmodel"))
	{
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "shards"))
	{
			m_iShards = atoi(pkvd->szValue);
			pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "trail"))
	{
			m_iTrail = atoi(pkvd->szValue);
			pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "server_gibs"))
	{
		if (atoi(pkvd->szValue) > 0)
		{
			m_bServerGibs = TRUE;
			pkvd->fHandled = TRUE;
		}
		else
			pkvd->fHandled = FALSE;
	}
	
	else if (FStrEq(pkvd->szKeyName, "shardsize") )
	{
		m_fShardSize = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}

	else if (FStrEq(pkvd->szKeyName, "gibmodel"))
	{
		m_iszGibModel = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "spawnobject"))
	{
		int object = atoi(pkvd->szValue);
		if (object > 0 && object < ARRAYSIZE(pSpawnObjects))
			m_iSpawnObject = object;
//			m_iszSpawnObject = ALLOC_STRING(pSpawnObjects[object]);// XDM3035c
//		else
//	!!!		m_iszSpawnObject = ALLOC_STRING(pkvd->szValue);// XDM

		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "explodemagnitude"))
	{
		pev->impulse = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "lip"))
		pkvd->fHandled = TRUE;
	else
		CBaseDelay::KeyValue( pkvd );
}

void CBreakable::Spawn(void)
{
    Precache();

	if (FBitSet(pev->spawnflags, SF_BREAK_TRIGGER_ONLY))
		pev->takedamage	= DAMAGE_NO;
	else
	{
		pev->takedamage	= DAMAGE_YES;
		SetThink(&CBreakable::ExistThink);
		pev->nextthink = gpGlobals->time + 0.5f;// XDM3035c: need to think so CheckEnvironment() works.
	}

	pev->solid		= SOLID_BSP;
    pev->movetype	= MOVETYPE_PUSH;
    m_angle			= pev->angles.y;
	pev->angles.y	= 0;

	// HACK:  matGlass can receive decals, we need the client to know about this
	//  so use class to store the material flag
	if (m_Material == matGlass || m_Material == matUnbreakableGlass)// XDM: ?
	{
		pev->playerclass = 1;
	}

	SET_MODEL(ENT(pev), STRING(pev->model) );//set size and link into world.

	if ( FBitSet( pev->spawnflags, SF_BREAK_TRIGGER_ONLY ) )		// Only break on trigger
		SetTouchNull();
	else
		SetTouch(&CBreakable::BreakTouch);

	// Flag unbreakable glass as "worldbrush" so it will block ALL tracelines
	if ( !IsBreakable() && pev->rendermode == kRenderNormal )// XDM: I want to shoot through the glass!!!
		pev->flags |= FL_WORLDBRUSH;
}

void CBreakable::Precache( void )
{
	char *pGibName = NULL;

	if (m_bServerGibs == FALSE)
	{
		switch (m_Material)
		{
			case matCeilingTile:
			case matRocks:
			case matWood:
			case matFlesh:
			case matCinderBlock:
			case matUnbreakableGlass:
			case matGlass:
			case matGrate:
				pGibName = "models/effects/ef_gibs.mdl";
			break;

			case matVent:
			case matMetal:
			case matComputer:
				pGibName = "models/effects/ef_player_gibs.mdl";
			break;
		}
	}
	if (m_bServerGibs == TRUE)
	{
		switch (m_Material)
		{
			case matWood:
				pGibName = "models/woodgibs.mdl";
			break;

			case matFlesh:
				pGibName = "models/fleshgibs.mdl";
			break;

			case matComputer:
				pGibName = "models/computergibs.mdl";
			break;

			case matUnbreakableGlass:
			case matGlass:
				pGibName = "models/glassgibs.mdl";
			break;

			case matMetal:
				pGibName = "models/metalplategibs.mdl";
			break;

			case matCinderBlock:
				pGibName = "models/cindergibs.mdl";
			break;

			case matRocks:
				pGibName = "models/rockgibs.mdl";
			break;

			case matCeilingTile:
				pGibName = "models/ceilinggibs.mdl";
			break;

			case matGrate:
				pGibName = "models/metalgibs.mdl";
			break;

			case matVent:
				pGibName = "models/ventgibs.mdl";
			break;
		}
	}

	int i = 0;// XDM: don't precache all sounds!
	for(i = 0; i < NUM_BREAK_SOUNDS; ++i)
		PRECACHE_SOUND((char *)gBreakSounds[m_Material][i]);

	MaterialSoundPrecache(m_Material);

	if (m_iszGibModel)
		pGibName = STRINGV(m_iszGibModel);

	if (pGibName)
		m_idShard = PRECACHE_MODEL(pGibName);
	else
		m_idShard = 0;

	// Precache the spawn item's data
	if (m_iSpawnObject > 0)
		UTIL_PrecacheOther(pSpawnObjects[m_iSpawnObject]);
}

void CBreakable::ExistThink(void)
{
	if (pev->takedamage != DAMAGE_NO)
		pev->nextthink = pev->ltime + 0.5f;
//		pev->nextthink = gpGlobals->time + 0.5f;
}

// play shard sound when func_breakable takes damage.
// the more damage, the louder the shard sound.
void CBreakable::DamageSound( void )
{
	int material = m_Material;
	int pitch = RANDOM_LONG(95,110);// XDM
	float fvol = RANDOM_FLOAT(0.75, 1.0);

	if (material == matComputer && RANDOM_LONG(0,1))
		material = matMetal;

	switch (material)
	{
	case matComputer:
	case matGlass:
	case matUnbreakableGlass:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gSoundsGlass[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
	case matWood:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gSoundsWood[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
	case matMetal:
	case matGrate:
	case matVent:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gSoundsMetal[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
	case matFlesh:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gSoundsFlesh[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
	case matRocks:
	case matCinderBlock:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gSoundsConcrete[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
	case matCeilingTile:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gSoundsCeiling[RANDOM_LONG(0,NUM_SHARD_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
	}
}

void CBreakable::BreakTouch( CBaseEntity *pOther )
{
	float flDamage;

	// only players can break these right now
	if ( !pOther->IsPlayer() || !IsBreakable() )
	{
        return;
	}

	if ( FBitSet ( pev->spawnflags, SF_BREAK_TOUCH ) )
	{// can be broken when run into
		flDamage = pOther->pev->velocity.Length() * 0.01;

		if (flDamage >= pev->health)
		{
			SetTouchNull();
			TakeDamage(pOther, pOther, flDamage, DMG_CRUSH);
			// do a little damage to player if we broke glass or computer
			pOther->TakeDamage(this, m_hActivator?(CBaseEntity *)m_hActivator:this, flDamage/4, DMG_SLASH);// XDM3035: m_hActivator
		}
	}

	if ( FBitSet ( pev->spawnflags, SF_BREAK_PRESSURE ) && pOther->pev->absmin.z >= pev->maxs.z - 2 )
	{// can be broken when stood upon

		// play creaking sound here.
		DamageSound();
		SetThink(&CBreakable::Die);
		SetTouchNull();

		if ( m_flDelay == 0 )
		{// !!!BUGBUG - why doesn't zero delay work?
			m_flDelay = 0.1;
		}

		pev->nextthink = pev->ltime + m_flDelay;
	}
}

//
// Smash the our breakable object
//

// Break when triggered
void CBreakable::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (IsBreakable())
	{
		pev->angles.y = m_angle;
		UTIL_MakeVectors(pev->angles);
		g_vecAttackDir = gpGlobals->v_forward;
		Die();
	}
}

void CBreakable::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
	// random spark if this is a 'computer' object
	if (m_Material == matComputer)// XDM: optimization
	{
		if (RANDOM_LONG(0,1))
		{
			UTIL_Sparks(ptr->vecEndPos);
//			char spark_snd[20];// XDM
//			sprintf(spark_snd, "buttons/spark%d.wav", RANDOM_LONG(1, 6));
//			EMIT_SOUND(ENT(pev), CHAN_VOICE, spark_snd, RANDOM_FLOAT(0.7, 1.0), ATTN_NORM);
			EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gSoundsSparks[RANDOM_LONG(0,NUM_SPARK_SOUNDS-1)], RANDOM_FLOAT(0.7, 1.0), ATTN_NORM, 0, RANDOM_LONG(95,105));
		}
	}
	else if (m_Material == matGlass)
	{
		UTIL_DecalTrace(ptr, DECAL_GLASSBREAK1 + RANDOM_LONG(0, 2));
	}
	else if (m_Material == matUnbreakableGlass)
	{
		if (bitsDamageType & (DMG_CRUSH | DMG_BULLET | DMG_SLASH))
			UTIL_Ricochet(ptr->vecEndPos, RANDOM_FLOAT(0.5, 1.5));
// client UTIL_DecalTrace(ptr, DECAL_BPROOF1);
	}

	CBaseDelay::TraceAttack( pAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

//=========================================================
// Special takedamage for func_breakable. Allows us to make
// exceptions that are breakable-specific
// bitsDamageType indicates the type of damage sustained ie: DMG_CRUSH
//=========================================================
int CBreakable::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	BOOL makenoise = TRUE;
	if (FBitSet(bitsDamageType, DMG_BURN) || FBitSet(bitsDamageType, DMG_SLOWBURN))// XDM
	{
		if (m_Material == matWood || m_Material == matComputer)
		{
			if (m_flBurnTime > 0 && m_flBurnTime <= gpGlobals->time)
			{
				EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/flame_burn.wav", VOL_NORM, ATTN_IDLE, 0, RANDOM_LONG(95, 105));
				makenoise = FALSE;
				m_flBurnTime = gpGlobals->time + 1.0f;
			}
			FX_Trail(pev->origin, RANDOM_LONG(15,25), FX_BLACKSMOKE);
		}
	}

	Vector	vecTemp;

	// if Attacker == Inflictor, the attack was a melee or other instant-hit attack.
	// (that is, no actual entity projectile was involved in the attack so use the shooter's origin).
	if (pInflictor)
	{
	if ( pAttacker == pInflictor )
	{
		vecTemp = pInflictor->pev->origin - ( pev->absmin + ( pev->size * 0.5 ) );

		// if a client hit the breakable with a crowbar, and breakable is crowbar-sensitive, break it now.
		if ( pAttacker && FBitSet ( pAttacker->pev->flags, FL_CLIENT ) &&
				 FBitSet ( pev->spawnflags, SF_BREAK_CROWBAR ) && (bitsDamageType & DMG_SLASH))
			flDamage = pev->health;
	}
	else
	// an actual missile was involved.
	{
		vecTemp = pInflictor->pev->origin - ( pev->absmin + ( pev->size * 0.5 ) );
	}
	}

	if (!IsBreakable())
		return 0;

	// Breakables take double damage from the crowbar
	if ( bitsDamageType & DMG_SLASH )
		flDamage *= 1.5;// XDM: here was 2

	// Boxes / glass / etc. don't take much poison damage, just the impact of the dart - consider that 10%
	if (!(bitsDamageType & DMGM_BREAK))// XDM: ignore some damage types
	{
		if (bitsDamageType & DMG_RADIATION || bitsDamageType & DMG_FREEZE || bitsDamageType & DMG_NERVEGAS)
		{
			if (m_Material == matFlesh)
				flDamage = 1.0;
			else
				return 0;
		}
		else if (bitsDamageType & DMG_ACID || bitsDamageType & DMG_POISON)
		{
			if (m_Material == matGlass || m_Material == matUnbreakableGlass)
				return 0;
			else
				flDamage *= 0.1;
		}
		else if (bitsDamageType & DMG_FREEZE)
		{
			if (m_Material != matGlass && m_Material != matFlesh)
				return 0;
		}
		else if (bitsDamageType & DMG_BURN)
		{
			if (m_Material == matCinderBlock ||
				m_Material == matUnbreakableGlass ||
				m_Material == matRocks)
			{
				return 0;
			}
			else if (m_Material == matMetal)
			{
				if (Explodable())
					flDamage *= 0.1;
				else
					return 0;
			}
		}
	}
	else
	{
		if (bitsDamageType & DMG_BULLET || bitsDamageType & DMG_MORTAR || bitsDamageType & DMG_ENERGYBEAM)
		{
			if (m_Material == matVent || m_Material == matGrate)
				flDamage *= 0.1;
			else
				flDamage *= 0.25;
		}
		else if (bitsDamageType & DMG_SHOCK)
		{
			if (m_Material != matFlesh && m_Material != matComputer)
				return 0.1;
		}
	}
	// this global is still used for glass and other non-monster killables, along with decals.
	g_vecAttackDir = vecTemp.Normalize();

//	pev->dmg = flDamage;// XDM: remember gere, after ignoring

	// do the damage
	pev->health -= flDamage;
	if (pev->health <= 0)
	{
		m_hActivator = pAttacker;// XDM3035c: TESTME
		Killed(pInflictor, pAttacker, GIB_NORMAL);// IMPORTANT: order
		Die();
//		return 0;
	}
	else
	{
		// Make a shard noise each time func breakable is hit.
		// Don't play shard noise if cbreakable actually died.
		if (makenoise)// XDM
			DamageSound();
	}
	return 1;
}

// TODO: replace with normal Killed()
void CBreakable::Die( void )
{
//	CBaseEntity *pEntity = NULL;
	char cFlag = 0;
	float fvol;
	int soundbits = NULL;// XDM
	int pitch = 95 + RANDOM_LONG(0,29);

	if (pitch > 97 && pitch < 103)
		pitch = 100;

	// The more negative pev->health, the louder
	// the sound should be.
	fvol = RANDOM_FLOAT(0.85, 1.0) + (abs(pev->health) / 100.0);

	if (fvol > 1.0)
		fvol = 1.0;

	switch (m_Material)
	{
	case matGlass:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gBreakSoundsGlass[RANDOM_LONG(0,NUM_BREAK_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
		cFlag = BREAK_GLASS;
		soundbits = bits_SOUND_WORLD;
		break;

	case matWood:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gBreakSoundsWood[RANDOM_LONG(0,NUM_BREAK_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
		cFlag = BREAK_WOOD;
		soundbits = bits_SOUND_WORLD;
		break;

	case matComputer:
	case matMetal:
	case matGrate:
	case matVent:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gBreakSoundsMetal[RANDOM_LONG(0,NUM_BREAK_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
		cFlag = BREAK_METAL;
		soundbits = bits_SOUND_WORLD;
		break;

	case matFlesh:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gBreakSoundsFlesh[RANDOM_LONG(0,NUM_BREAK_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
		cFlag = BREAK_FLESH;
		soundbits = bits_SOUND_MEAT;
		break;

	case matRocks:
	case matCinderBlock:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gBreakSoundsConcrete[RANDOM_LONG(0,NUM_BREAK_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
		cFlag = BREAK_CONCRETE;
		soundbits = bits_SOUND_WORLD;
		break;

	case matCeilingTile:
		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gBreakSoundsCeiling[RANDOM_LONG(0,NUM_BREAK_SOUNDS-1)], fvol, ATTN_NORM, 0, pitch); break;
		cFlag = BREAK_CONCRETE;
		soundbits = bits_SOUND_GARBAGE;
		break;
	}

	if (m_idShard > 0)
	{
		Vector vecSpot;// shard origin
		Vector vecVelocity;// shard velocity

		if (m_iExplosion == EXPLOSION_DIRECTED)
			vecVelocity = g_vecAttackDir * clamp(pev->dmg * -10, -1024, 1024);// XDM 200;
		else
			vecVelocity = UTIL_RandomVector() * clamp(pev->dmg * 10, 1, 240);// XDM: really nice randomization!

		if (m_Material == matWood)// XDM
			vecVelocity = vecVelocity * 2;

		vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5;

		if (m_iShards <= 0) //based on explodemagnitude value)
		{
			if (pev->impulse <= 0)
				m_iShards = 8; //default
			else
				m_iShards = (pev->impulse/20); //based on explodemagnitude value
		}
		else
			m_iShards = m_iShards;

		if (m_fShardSize <= 0)
			m_fShardSize = 10;

		if (m_bServerGibs == FALSE)
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgThrowGib);
			WRITE_COORD( pev->maxs.x );
			WRITE_COORD( pev->maxs.y );
			WRITE_COORD( pev->maxs.z );
			WRITE_COORD( pev->mins.x );
			WRITE_COORD( pev->mins.y );
			WRITE_COORD( pev->mins.z );

			if (m_iszGibModel)			
				WRITE_SHORT( m_idShard ); //use custom model
			else
				WRITE_SHORT( NULL ); //use default gib model (with different bodygroups)

			WRITE_SHORT( m_fShardSize );
			WRITE_BYTE( m_iShards );
			WRITE_BYTE( m_Material );
			WRITE_BYTE( m_iTrail ); // trail effects
			WRITE_BYTE( pev->renderamt ); // brightness
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_BREAKMODEL);
			// position
			WRITE_COORD( vecSpot.x );
			WRITE_COORD( vecSpot.y );
			WRITE_COORD( vecSpot.z );
			// size
			WRITE_COORD( pev->size.x);
			WRITE_COORD( pev->size.y);
			WRITE_COORD( pev->size.z);
			// velocity
			WRITE_COORD( vecVelocity.x );
			WRITE_COORD( vecVelocity.y );
			WRITE_COORD( vecVelocity.z );
			// randomization
			WRITE_BYTE( clamp(pev->dmg, 10, 50) );
			// Model
			WRITE_SHORT( m_idShard );//model id#
			// # of shards
			WRITE_BYTE( m_iShards ); // let client decide
			// duration
			WRITE_BYTE( 25 );// 2.5 seconds
			// flags
			WRITE_BYTE( cFlag );
			MESSAGE_END();
		}
	}

	if (soundbits)
		CSoundEnt::InsertSound(soundbits, pev->origin, 128, 0.5);// XDM

	float size = pev->size.x;
	if (size < pev->size.y)
		size = pev->size.y;
	if (size < pev->size.z)
		size = pev->size.z;

	// !!! HACK  This should work!
	// Build a box above the entity that looks like an 8 pixel high sheet
	Vector mins = pev->absmin;
	Vector maxs = pev->absmax;
	mins.z = pev->absmax.z;
	maxs.z += 8;

	// BUGBUG -- can only find 256 entities on a breakable -- should be enough
	CBaseEntity *pList[256];
	int count = UTIL_EntitiesInBox( pList, 256, mins, maxs, FL_ONGROUND );
	if ( count )
	{
		for ( int i = 0; i < count; ++i )
		{
			ClearBits( pList[i]->pev->flags, FL_ONGROUND );
			pList[i]->pev->groundentity = NULL;
		}
	}

	// Don't fire something that could fire myself
	pev->targetname = 0;
	pev->solid = SOLID_NOT;
	pev->health = 0;// XDM
	pev->takedamage = DAMAGE_NO;// IMPORTANT! Prevents self-destructive recursion!
	// Fire targets on break
	SUB_UseTargets(m_hActivator, USE_TOGGLE, 0);// XDM3035c: TESTME

	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = pev->ltime + 0.1;
	if (m_iSpawnObject > 0)
		CBaseEntity::Create(pSpawnObjects[m_iSpawnObject], VecBModelOrigin(pev), pev->angles, edict() );

	if (Explodable())
	{
//		pev->takedamage = DAMAGE_NO;// IMPORTANT! Prevents self-destructive recursion!
		pev->deadflag = DEAD_DEAD;
		ExplosionCreate(Center(), pev->angles, this, pev->impulse, 0, 0.0f);
	}
}

BOOL CBreakable::IsBreakable(void)
{
	return m_Material != matUnbreakableGlass;
}

BOOL CBreakable::Explodable(void)
{
	return pev->impulse > 0;
}

int	CBreakable::DamageDecal(const int &bitsDamageType)
{
	if ( m_Material == matGlass  )
		return DECAL_GLASSBREAK1 + RANDOM_LONG(0,2);

	if ( m_Material == matUnbreakableGlass )
		return DECAL_BPROOF1;

	if ( m_Material == matWood )// XDM
		return DECAL_WOODBREAK1 + RANDOM_LONG(0,2);

	if ( m_Material == matComputer )// XDM
		return DECAL_LARGESHOT1 + RANDOM_LONG(0,4);

	return CBaseEntity::DamageDecal( bitsDamageType );
}





TYPEDESCRIPTION	CPushable::m_SaveData[] =
{
	DEFINE_FIELD( CPushable, m_maxSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CPushable, m_soundTime, FIELD_TIME ),
};

IMPLEMENT_SAVERESTORE( CPushable, CBreakable );

LINK_ENTITY_TO_CLASS( func_pushable, CPushable );

void CPushable :: Spawn( void )
{
	if (pev->spawnflags & SF_PUSH_BREAKABLE)
		CBreakable::Spawn();
	else
		Precache();

	pev->movetype	= MOVETYPE_PUSHSTEP;
	pev->solid		= SOLID_BBOX;
	SET_MODEL(ENT(pev), STRING(pev->model));

	if (pev->friction > 399)
		pev->friction = 399;

	m_maxSpeed = 400 - pev->friction;
	SetBits(pev->flags, FL_FLOAT);
	pev->friction = 0;
	pev->origin.z += 1;	// Pick up off of the floor
	UTIL_SetOrigin( pev, pev->origin );
	// Multiply by area of the box's cross-section (assume 1000 units^3 standard volume)
	pev->skin = ( pev->skin * (pev->maxs.x - pev->mins.x) * (pev->maxs.y - pev->mins.y) ) * 0.0005;
	m_soundTime = 0;
}

void CPushable :: Precache(void)
{
	if (m_Material == matNone || m_Material == matLastMaterial)
		return;

	int i = 0;// XDM: don't precache all sounds!
	for(i = 0; i < 3; i++)
		PRECACHE_SOUND((char *)gPushSounds[m_Material][i]);

	if (pev->spawnflags & SF_PUSH_BREAKABLE)
		CBreakable::Precache();
}

void CPushable::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq(pkvd->szKeyName, "size") )
	{
		int bbox = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;

		switch( bbox )
		{
		case 0:	// Point
			UTIL_SetSize(pev, Vector(-8, -8, -8), Vector(8, 8, 8));
			break;

		case 2: // Big Hull!?!? !!!BUGBUG Figure out what this hull really is
			UTIL_SetSize(pev, VEC_DUCK_HULL_MIN*2, VEC_DUCK_HULL_MAX*2);
			break;

		case 3: // Player duck
			UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
			break;

		default:
		case 1: // Player
			UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
			break;
		}

	}
	else if ( FStrEq(pkvd->szKeyName, "buoyancy") )
	{
		pev->skin = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBreakable::KeyValue( pkvd );
}

// Pull the func_pushable
void CPushable::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (!pActivator || !pActivator->IsPlayer())
	{
		if (pev->spawnflags & SF_PUSH_BREAKABLE)
			this->CBreakable::Use( pActivator, pCaller, useType, value );
		return;
	}

	if (pActivator->pev->velocity != g_vecZero)
		Move( pActivator, 0 );
}

void CPushable :: Touch( CBaseEntity *pOther )
{
	if (pOther->IsBSPModel())// XDM: worldspawn is BSP model too
	{
		if ((pev->spawnflags & SF_PUSH_BREAKABLE) && (pev->velocity.z <= -1024))
		{
			// Didn't do 'was in air' check. Let's allow breaking when sliding too fast?
//			ALERT(at_console, "velocity = %f %f %f\n", pev->velocity.x, pev->velocity.y, pev->velocity.z);
			this->CBreakable::Use(pOther, pOther, USE_ON, 1);
			return;
		}

		if (!pOther->IsMovingBSP())// but not moving...
			return;
	}

	if (pOther->IsProjectile())// XDM
		return;

	Move( pOther, 1 );
}

void CPushable :: Move( CBaseEntity *pOther, int push )
{
	entvars_t*	pevToucher = pOther->pev;
	int playerTouch = 0;

	// Is entity standing on this pushable ?
	if ( FBitSet(pevToucher->flags,FL_ONGROUND) && pevToucher->groundentity && VARS(pevToucher->groundentity) == pev )
	{
		// Only push if floating
		if ( pev->waterlevel > 0 )
			pev->velocity.z += pevToucher->velocity.z * 0.1;

		return;
	}


	if ( pOther->IsPlayer() )
	{
		if ( push && !(pevToucher->button & (IN_FORWARD|IN_USE)) )	// Don't push unless the player is pushing forward and NOT use (pull)
			return;
		playerTouch = 1;
	}

	float factor;

	if ( playerTouch )
	{
		if ( !(pevToucher->flags & FL_ONGROUND) )	// Don't push away from jumping/falling players unless in water
		{
			if ( pev->waterlevel < 1 )
				return;
			else
				factor = 0.1;
		}
		else
			factor = 1;
	}
	else
		factor = 0.25;

	pev->velocity.x += pevToucher->velocity.x * factor;
	pev->velocity.y += pevToucher->velocity.y * factor;

	float length = sqrt( pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y );
	if ( push && (length > MaxSpeed()) )
	{
		pev->velocity.x = (pev->velocity.x * MaxSpeed() / length );
		pev->velocity.y = (pev->velocity.y * MaxSpeed() / length );
	}
	if ( playerTouch )
	{
		pevToucher->velocity.x = pev->velocity.x;
		pevToucher->velocity.y = pev->velocity.y;
		if ( (gpGlobals->time - m_soundTime) > 0.7 )
		{
			m_soundTime = gpGlobals->time;
			if ( length > 0 && FBitSet(pev->flags,FL_ONGROUND) )
			{
				m_lastSound = RANDOM_LONG(0,2);
				PlayMatPushSound( m_lastSound );// XDM
			}
			else
				StopMatPushSound( m_lastSound );// XDM
		}
	}
}

#if 0
void CPushable::StopSound( void )
{
	Vector dist = pev->oldorigin - pev->origin;
	if ( dist.Length() <= 0 )
		STOP_SOUND( ENT(pev), CHAN_WEAPON, m_soundNames[m_lastSound] );
}
#endif

int CPushable::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if ( pev->spawnflags & SF_PUSH_BREAKABLE )
		return CBreakable::TakeDamage( pInflictor, pAttacker, flDamage, bitsDamageType );

	return 1;
}

void CPushable::PlayMatPushSound( int m_lastSound )
{
	if (m_Material == matNone/* || m_Material == matLastMaterial*/)
		return;

	EMIT_SOUND(ENT(pev), CHAN_BODY, gPushSounds[m_Material][m_lastSound], 0.5f, ATTN_NORM);
}

void CPushable::StopMatPushSound( int m_lastSound )
{
	if (m_Material == matNone/* || m_Material == matLastMaterial*/)
		return;

	STOP_SOUND(ENT(pev), CHAN_BODY, gPushSounds[m_Material][m_lastSound]);
}




LINK_ENTITY_TO_CLASS(func_breakable_model, CBreakableModel);

void CBreakableModel::Spawn(void)
{
	PRECACHE_MODEL(STRINGV(pev->model));
	CBreakable::Precache();

	pev->movetype = MOVETYPE_TOSS;
	pev->solid = SOLID_BBOX;
	SET_MODEL(ENT(pev), STRING(pev->model));
/*
	if (FBitSet(pev->spawnflags, SF_BREAK_TRIGGER_ONLY))
		pev->takedamage	= DAMAGE_NO;
	else
		pev->takedamage	= DAMAGE_YES;

	if (FBitSet(pev->spawnflags, SF_BREAK_TRIGGER_ONLY))
		SetTouchNull();
	else
		SetTouch(&CBreakable::BreakTouch);
*/
	if (pev->health <= 0)
		pev->health = 20;

	m_angle = pev->angles.y;
}
