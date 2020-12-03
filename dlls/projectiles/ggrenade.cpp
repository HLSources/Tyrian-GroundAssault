#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "decals.h"
#include "player.h"
#include "effects.h"
#include "shake.h"
#include "game.h"// XDM
#include "gamerules.h"
#include "globals.h"
#include "msg_fx.h"
#include "projectiles.h"

LINK_ENTITY_TO_CLASS(grenade, CGrenade);

// XDM: spawnflags definitions moved to header "weapons.h"

void CGrenade::Spawn(void)
{
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_NO;
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
	pev->impulse = 0;// XDM
	m_bloodColor = DONT_BLEED;
	m_fRegisteredSound = FALSE;
}

//pre-explode effects
void CGrenade::Impact(CBaseEntity *pOther, bool StopCollide, float ScreenShakeAmplitude, float ScreenShakeRadius, bool PaintDecal, int decal, float ImpactDmg, int bitsImpactDamageType, float RadiusDmg, float RadiusDmgRadius, int bitsRadiusDamageType, float WallDist, int FX_Type)
{
	if ( POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		CGrenade::RemoveIfInSky();
		return;
	}
	TraceResult tr;
	Vector vecSpot = pev->origin - pev->velocity.Normalize() * 32;
	Vector vecEnd = pev->origin + pev->velocity.Normalize() * 64;
	UTIL_TraceLine( vecSpot, vecEnd, ignore_monsters, ENT(pev), &tr );
	pev->takedamage	= DAMAGE_NO;

	if (StopCollide)
		SetTouchNull();
	
	if (ImpactDmg > 0)
	{
		if (pOther->pev->takedamage)
		{
			TraceResult tr;
			Vector forward;
			ANGLE_VECTORS(pev->angles, forward, NULL, NULL);
			UTIL_TraceLine(pev->origin, pev->origin + gpGlobals->v_forward * 16.0f, dont_ignore_monsters, ENT(pev), &tr);

			CBaseEntity *pOwner = NULL;
			if (pev->owner)
				pOwner = CBaseEntity::Instance(pev->owner);
			else 
				pOwner = g_pWorld;

			ClearMultiDamage();
			pOther->TraceAttack(pOwner, ImpactDmg, pev->velocity.Normalize(), &tr, bitsImpactDamageType);
			ApplyMultiDamage(this, pOwner);
		}
	}

	if (ScreenShakeAmplitude > 0)
		UTIL_ScreenShake(pev->origin, ScreenShakeAmplitude, 0.75f, 1.0f, ScreenShakeRadius);

	if (PaintDecal)
		UTIL_DecalTrace(&tr, decal);

	if (RadiusDmg > 0)
	{
		CBaseEntity *pOwner = NULL;
		if (pev->owner)
			pOwner = CBaseEntity::Instance(pev->owner);
		else 
			pOwner = g_pWorld;
	
		::RadiusDamage(pev->origin, this, pOwner, RadiusDmg, RadiusDmgRadius, CLASS_NONE, bitsRadiusDamageType);
	}
	
	if (FX_Type)
		FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * WallDist), entindex(), FX_Type);
}

//projectile destroyed by something
void CGrenade::Blast(float ScreenShakeAmplitude, float ScreenShakeRadius, float RadiusDmg, float RadiusDmgRadius, int bitsRadiusDamageType, int FX_Type)
{
	pev->takedamage	= DAMAGE_NO;
	SetTouchNull();
	
	if (ScreenShakeAmplitude > 0 && ScreenShakeRadius > 0)
		UTIL_ScreenShake(pev->origin, ScreenShakeAmplitude, 0.75f, 1.0f, ScreenShakeRadius);

	if (RadiusDmg > 0)
	{
		CBaseEntity *pOwner = NULL;
		if (pev->owner)
			pOwner = CBaseEntity::Instance(pev->owner);
		else 
			pOwner = g_pWorld;
	
		::RadiusDamage(pev->origin, this, pOwner, RadiusDmg, RadiusDmgRadius, CLASS_NONE, bitsRadiusDamageType);
	}
	if (FX_Type)
		FX_Trail(pev->origin, entindex(), FX_Type);

	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

//
// Grenade Explode
//
void CGrenade::Explode(TraceResult *pTrace, int bitsDamageType)
{
	if (pTrace == NULL)
	{
		ALERT(at_aiconsole, "CGrenade::Explode(pTrace == NULL)!!\n");
		return;
	}

	if (pTrace->fAllSolid)// outside of the world
	{
		UTIL_Remove(this);
		return;
	}
	pev->model = iStringNull;//invisible
	pev->solid = SOLID_NOT;// intangible
	pev->takedamage	= DAMAGE_NO;// !!!


	int iDebrisSoundIndex = 0;
	float radius = pev->dmg * 2.5;

	CBaseEntity *pOwner = NULL;
	if (pev->owner)// XDM3035b
	{
		pOwner = CBaseEntity::Instance(pev->owner);
		pev->owner = NULL; // can't traceline attack owner if this is set
	}
	else if (m_hActivator)
	{
		pOwner = m_hActivator;
	}

	if (pTrace->flFraction != 1.0f)// Pull out of the wall a bit
		pev->origin = pTrace->vecEndPos + pTrace->vecPlaneNormal;

	BOOL bInWater = UTIL_LiquidContents(pev->origin);// are we in water now? XDM: don't use pev->waterlevel

	// fireball
	if (pev->spawnflags & SF_NOFIREBALL)
		FX_Trail(pev->origin, pev->dmg, FX_DEFAULT_EXP_NO_FIREBALL);
	else if (pev->spawnflags & SF_NOSPARKS)
		FX_Trail(pev->origin, pev->dmg, FX_DEFAULT_EXP_NO_SPARKS);
	else if (pev->spawnflags & SF_NOSMOKE)
		FX_Trail(pev->origin, pev->dmg, FX_DEFAULT_EXP_NO_SMOKE);
	else
		FX_Trail(pev->origin, pev->dmg, FX_DEFAULT_EXP);

	// sound
	if (!(pev->spawnflags & SF_NOSOUND))
	{
		if (bInWater)
			EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/explode_uw.wav", VOL_NORM, 0.6f, 0, RANDOM_LONG(95, 105));
		else
		{
			char sample[24];
			sprintf(sample, "weapons/explode%d.wav\0", RANDOM_LONG(3,5));// explode3.wav ...5
			EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, sample, VOL_NORM, 0.3f, 0, RANDOM_LONG(95, 105));
		}
		CSoundEnt::InsertSound(bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME + (int)pev->dmg, 1.0f);
	}
	//decal
	if (!(pev->spawnflags & SF_NODECAL))// XDM3035b: in event
		UTIL_DecalTrace(pTrace, DECAL_SCORCH1 + RANDOM_LONG(0,2));

	//damage
	if (!(pev->spawnflags & SF_NODAMAGE))
	{
		if (pev->flags & FL_ONGROUND)// XDM: for RadiusDamage
			pev->origin.z += 1.0f;

		if (bInWater)
		{
			::RadiusDamage(pev->origin, this, pOwner, pev->dmg, radius*0.75f, CLASS_NONE, bitsDamageType);
			UTIL_ScreenShake(pev->origin, pev->dmg*0.4f, 1.0f, 2.0f, radius*1.5f);
		}
		else
		{
			::RadiusDamage(pev->origin, this, pOwner, pev->dmg, radius, CLASS_NONE, bitsDamageType);
			UTIL_ScreenShake(pev->origin, pev->dmg*0.2f, 0.75f, 1.0f, radius*1.5f);
		}
		iDebrisSoundIndex = RANDOM_LONG(0,2);// must be same for all clients
	}

	pev->effects |= EF_NODRAW;
	pev->velocity = g_vecZero;
	pev->movetype = MOVETYPE_NONE;// XDM
	pev->impulse = 0;// XDM: for smoke

	if (pOwner)// XDM3034: IMPORTANT! Restore owner, satchels need this in UpdateOnRemove()
		pev->owner = pOwner->edict();

	if (!(pev->spawnflags & SF_NOFIREBALL))
	{
		if (pev->angles == g_vecZero)// for decals
			pev->angles = Vector(90,0,0);// face down

	}

	if (pev->spawnflags & SF_REPEATABLE)
	{
		SetThinkNull();
		pev->nextthink = 0.0f;
	}
	else
	{
		pev->health = 0.0f;// XDM3035a avoid stupid warning
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time + 0.1f;
	}
}

int CGrenade::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pev->takedamage != DAMAGE_NO)
	{
		if (bitsDamageType & DMGM_BREAK)
		{
			Killed(pAttacker, pInflictor, GIB_NORMAL);
			return 1;
		}
	}
	return 0;
}

void CGrenade::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)
{
	Detonate();
}


void CGrenade::Detonate(void)
{
	TraceResult tr;
//	Vector vecSpot;// trace starts here!
//	vecSpot = pev->origin + Vector(0,0,6);
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;// XDM3035c: decorative, simplify
	UTIL_TraceLine(pev->origin + Vector(0,0,6), pev->origin - Vector(0,0,42), ignore_monsters, ENT(pev), & tr);
	gpGlobals->trace_flags = 0;
	Explode(&tr, DMG_BLAST);
}

// Contact grenade, explode when it touches something
// Custom sprites here
void CGrenade::ExplodeTouch(CBaseEntity *pOther)
{
	TraceResult tr;
	Vector vecSpot;
	pev->enemy = pOther->edict();
	vecSpot = pev->origin - pev->velocity.Normalize() * 8.0f;
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;// XDM3035c: decorative, simplify
	UTIL_TraceLine(vecSpot, pev->origin + pev->velocity.Normalize() * 64.0f, ignore_monsters, ENT(pev), &tr);
	gpGlobals->trace_flags = 0;
	pev->spawnflags |= SF_NOSPARKS;
	Explode(&tr, DMG_BLAST);
}

void CGrenade::DangerSoundThink(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin + pev->velocity * 0.5f, pev->velocity.Length(), 0.2f);
	pev->nextthink = gpGlobals->time + 0.2f;

	if (pev->waterlevel != 0)
		pev->velocity = pev->velocity * 0.5f;
}

void CGrenade::SpeedUp(void)
{
	if (pev->velocity.Length() > 4096.0f)
	{
		pev->velocity = pev->velocity.Normalize()*4096.0f;
		return;
	}
	pev->velocity += pev->velocity.Normalize()*24.0f;
}

//Helps projectiles to hit an enemy (if fly too close to enemy)
void CGrenade::SearchTarget(void)
{
	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityInSphere(pEntity, pev->origin, PROJ_SEARCH_RADIUS)) != NULL)
	{
		if (!FBitSet(pEntity->pev->effects, EF_NODRAW) && pEntity->IsAlive() && !pEntity->IsProjectile() && pEntity != this && pEntity != Instance(pev->owner) && g_pGameRules->PlayerRelationship(Instance(pev->owner), pEntity) != GR_TEAMMATE)
		{
			if (pEntity->IsMonster() || pEntity->IsPlayer())
			{
				UTIL_SetOrigin(pev, pEntity->pev->origin);
				return;
			}
		}
	}
}

//For homing projectiles
void CGrenade::MovetoTarget(const Vector &vecTarget, float speed)
{
	if (pev->movedir.IsZero())
		pev->movedir = pev->velocity;

	float flSpeed = pev->movedir.Length();

	if (flSpeed > speed)
		pev->movedir = pev->movedir.Normalize() * speed;

	pev->movedir = pev->movedir + (vecTarget - pev->origin).Normalize() * speed;
	pev->velocity = pev->movedir;
}

//For homing projectiles
void CGrenade::SeekTarget(float radius, float view_field)
{
	if (m_hEnemy != NULL)
		return;

	CBaseEntity *pObject = NULL;
	CBaseEntity *pClosest = NULL;
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	Vector vecLOS;
	float flDot;
	TraceResult tr;

	UTIL_MakeVectors(pev->angles);

	while ((pObject = UTIL_FindEntityInSphere(pObject, pev->origin, radius)) != NULL)
	{
		if (pObject == this)
			continue;
		if (pObject->IsProjectile())
			continue;
		if (pObject->pev->effects & EF_NODRAW)
			continue;
		if (!pObject->IsAlive())
			continue;
		if (g_pGameRules->PlayerRelationship(pOwner, pObject) == GR_TEAMMATE)
			continue;
		if (pObject->pev->takedamage == DAMAGE_NO)
			continue;
		if (pObject->edict() == pev->owner)
			continue;

		vecLOS = (pObject->Center() - pev->origin);
		vecLOS = UTIL_ClampVectorToBox(vecLOS, pObject->pev->size * 0.5f);

		UTIL_TraceLine(pev->origin, pObject->Center(), ignore_monsters, dont_ignore_glass, edict(), &tr);
		if (tr.flFraction < 1.0f && tr.pHit != pObject->edict())
			continue;

		flDot = DotProduct(vecLOS, gpGlobals->v_forward);
		if (flDot > view_field)
		{
			pClosest = pObject;
			view_field = flDot;
		}
	}
	pObject = pClosest;

	if (pObject)
	{
		if (pObject->IsMonster() || pObject->IsPlayer())
		{
			m_hEnemy = pObject;
		}
	}
}

void CGrenade::RemoveIfInSky(void)
{
	pev->takedamage = DAMAGE_NO;
	pev->velocity = g_vecZero;
	SetTouchNull();
	SetThinkNull();
	FX_Trail( pev->origin, entindex(), FX_REMOVE );
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CGrenade::BounceTouch(CBaseEntity *pOther)
{
	// don't hit the guy that launched this grenade
	if (pOther->edict() == pev->owner)
		return;

	// only do damage if we're moving fairly fast
	if (pev->impacttime < gpGlobals->time && pev->velocity.Length() > 100)
	{
//		entvars_t *pevOwner = VARS(pev->owner);
		CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
		if (pOwner)
		{
			TraceResult tr = UTIL_GetGlobalTrace();
			ClearMultiDamage();
			pOther->TraceAttack(pOwner, 1, gpGlobals->v_forward, &tr, DMG_SLASH | DMG_NEVERGIB);
			ApplyMultiDamage(this, pOwner);
		}
		pev->impacttime = gpGlobals->time + 1.0f; // debounce
	}

	Vector vecTestVelocity(pev->velocity);
	// pev->avelocity = Vector (300, 300, 300);

	// this is my heuristic for modulating the grenade velocity because grenades dropped purely vertical
	// or thrown very far tend to slow down too quickly for me to always catch just by testing velocity.
	// trimming the Z velocity a bit seems to help quite a bit.
//	vecTestVelocity = pev->velocity;
	vecTestVelocity.z *= 0.45f;

	if (!m_fRegisteredSound && vecTestVelocity.Length() <= 60)
	{
		//ALERT(at_console, "Grenade Registered!: %f\n", vecTestVelocity.Length());
		// grenade is moving really slow. It's probably very close to where it will ultimately stop moving.
		// go ahead and emit the danger sound.
		// register a radius louder than the explosion, so we make sure everyone gets out of the way
		CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin, pev->dmg*2.5f, 0.3f);
		m_fRegisteredSound = TRUE;
	}

	if (pev->flags & FL_ONGROUND)
	{
		// add a bit of static friction
		pev->velocity = pev->velocity * 0.8f;
		pev->sequence = LookupActivity(ACT_WALK);//RANDOM_LONG(1, 2);
	}
	else
		BounceSound();

	pev->framerate = pev->velocity.Length() / 200.0f;

	if (pev->framerate > 1.0f)
		pev->framerate = 1.0f;
	else if (pev->framerate < 0.5f)
		pev->framerate = 0.0f;
}

void CGrenade::BounceSound(void)
{
	switch (RANDOM_LONG(0, 2))
	{
		case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/projectile_bounce1.wav", RANDOM_FLOAT(0.4, 0.6), ATTN_STATIC); break;
		case 1:	EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/projectile_bounce2.wav", RANDOM_FLOAT(0.4, 0.6), ATTN_STATIC); break;
		case 2:	EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/projectile_bounce3.wav", RANDOM_FLOAT(0.4, 0.6), ATTN_STATIC); break;
	}
}

void CGrenade::TumbleThink(void)
{
	if (!IsInWorld())
	{
		UTIL_Remove(this);
		return;
	}

	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1f;

	if (pev->dmgtime - 1 < gpGlobals->time)
		CSoundEnt::InsertSound(bits_SOUND_DANGER, pev->origin + pev->velocity * (pev->dmgtime - gpGlobals->time), 400, 0.1);

	if (pev->dmgtime <= gpGlobals->time)
	{
		SetThink(&CGrenade::Detonate);
	}
	if (pev->waterlevel != 0)
	{
		pev->velocity = pev->velocity * 0.5f;
		pev->framerate = 0.2f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Do some damage after the projectile has exploded (e.g. gas, acid, radiation)
// Input  : &life - life in seconds
//			&updatetime - RadiusDamage() intervals
//			&damagedelta - this value is added to pev->dmg after each interval
//			&bitsDamageType - DMG_GENERIC
//-----------------------------------------------------------------------------
void CGrenade::DoDamageInit(const float &life, const float &updatetime, const float &damagedelta, const int &bitsDamageType)
{
	pev->effects |= EF_NODRAW;
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;// !!
	pev->dmgtime = gpGlobals->time + life;
	pev->dmg_save = damagedelta;
	pev->ltime = updatetime;
	pev->button = bitsDamageType;
	SetThink(&CGrenade::DoDamageThink);
	pev->nextthink = gpGlobals->time;
}

void CGrenade::DoDamageThink(void)
{
	if (pev->dmgtime <= gpGlobals->time)
		UTIL_Remove(this);

	if (pev->waterlevel > 0 && pev->dmg > 0.5f && g_pGameRules->FAllowEffects() && RANDOM_LONG(0,1)==0)
		FX_BubblesPoint(pev->origin, VECTOR_CONE_20DEGREES, max(2, min((int)(pev->dmg*0.5f), 64)));
//		UTIL_Bubbles(pev->origin - Vector(8,8,8), pev->origin + Vector(8,8,8), max(2, min((int)(pev->dmg*0.5f), 64)));

	float radius = pev->dmg*2.0f;// XDM3035

	::RadiusDamage(pev->origin, this, Instance(pev->owner), pev->dmg * 0.1f, radius, CLASS_NONE, pev->button);

	pev->dmg += pev->dmg_save;// XDM3035: this will do for now
	if (pev->dmg_save < 0 && pev->dmg < 0)
	{
		pev->dmg = 0;
		pev->dmg_save = 0;
	}

	pev->nextthink = gpGlobals->time + pev->ltime;
}



CGrenade *CGrenade::ShootContact(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecVelocity)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)NULL, "grenade");
	pGrenade->pev->spawnflags |= SF_NORESPAWN;// XDM3035b
	pGrenade->Spawn();
	SET_MODEL(ENT(pGrenade->pev), "models/projectiles.mdl");
	pGrenade->pev->body = PROJ_BODY_M203;

	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles (pGrenade->pev->velocity);
	if (pOwner)
		pGrenade->pev->owner = pOwner->edict();
	// make monsters afaid of it while in the air
	pGrenade->SetThink(&CGrenade::DangerSoundThink);
	pGrenade->pev->nextthink = gpGlobals->time;
	pGrenade->SetTouch(&CGrenade::ExplodeTouch);
	pGrenade->pev->dmg = gSkillData.DmgM203Grenade;
	return pGrenade;
}

CGrenade *CGrenade::ShootTimed(CBaseEntity *pOwner, const Vector &vecStart, const Vector &vecVelocity, float time)
{
	CGrenade *pGrenade = GetClassPtr((CGrenade *)NULL, "grenade");
	pGrenade->pev->spawnflags |= SF_NORESPAWN;// XDM3035b
	pGrenade->Spawn();
	SET_MODEL(ENT(pGrenade->pev), "models/projectiles.mdl");
	pGrenade->pev->body = PROJ_BODY_M203;
	UTIL_SetOrigin(pGrenade->pev, vecStart);
	pGrenade->pev->velocity = vecVelocity;
	pGrenade->pev->angles = UTIL_VecToAngles(pGrenade->pev->velocity);
	if (pOwner)
		pGrenade->pev->owner = pOwner->edict();


	pGrenade->SetTouch(&CGrenade::BounceTouch);	// Bounce if touched
	pGrenade->pev->dmgtime = gpGlobals->time + time;
	pGrenade->pev->nextthink = gpGlobals->time + 0.1f;
	pGrenade->SetBodygroup(1,1);// XDM: without ring

	pGrenade->SetThink(&CGrenade::TumbleThink); 
	pGrenade->pev->takedamage = DAMAGE_YES;

	if (time < 0.1f)
	{
		pGrenade->pev->nextthink = gpGlobals->time;
		pGrenade->pev->velocity = g_vecZero;
	}
	pGrenade->pev->sequence = pGrenade->LookupActivity(ACT_FALL);

	pGrenade->pev->framerate = 1.0f;
	pGrenade->pev->gravity = 0.5f;
	pGrenade->pev->friction = 0.8f;
	pGrenade->pev->dmg = gSkillData.Dmg30mmGrenade;// XDM
	return pGrenade;
}

void CGrenade::NuclearExplodeThink(void)
{
	CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
	float Damage =gSkillData.DmgAtomBomb;

	switch(pev->oldbuttons)
	{
	default:
	case 0:
		{
			pev->movetype = MOVETYPE_NONE;
			pev->solid = SOLID_NOT;
			pev->effects |= EF_NODRAW;
			pev->takedamage = DAMAGE_NO;
			::RadiusDamage(pev->origin, this, pOwner, Damage, Damage*2, CLASS_NONE, DMG_VAPOURIZING | DMG_RADIATION | DMG_WALLPIERCING);
			FX_Trail(pev->origin, entindex(), FX_ATOMBOMB_DETONATE_PART1);
			pev->nextthink = gpGlobals->time + 0.2f;
		}
		break;
	case 1:
		{
			pev->nextthink = gpGlobals->time + 0.3f;
		}
		break;
	case 2:
		{
			FX_Trail(pev->origin, entindex(), FX_ATOMBOMB_DETONATE_PART2);
#ifndef CLIENT_DLL
			if (g_pGameRules->FAllowEffects())
				UTIL_ScreenShake(pev->origin, 32.0f, 4.0f, 3.0f, Damage*2);
#endif
			pev->nextthink = gpGlobals->time + 0.4f;
		}
		break;
	case 3:
		{
			pev->nextthink = gpGlobals->time + 0.5f;
		}
		break;
	case 4:
		{
			pev->nextthink = gpGlobals->time + 0.2f;
		}
		break;
	case 5:
		{
			FX_Trail(pev->origin, RANDOM_LONG(350,400), FX_ATOMBOMB_DETONATE_PART3);
			::RadiusDamage(pev->origin, this, pOwner, Damage*0.25, Damage, CLASS_NONE, DMG_VAPOURIZING | DMG_RADIATION | DMG_WALLPIERCING);
			pev->nextthink = gpGlobals->time + 0.2f;
		}
		break;
	case 6:
		{
			FX_Trail(pev->origin, RANDOM_LONG(350,400), FX_ATOMBOMB_DETONATE_PART4);
			::RadiusDamage(pev->origin, this, pOwner, Damage*0.5, Damage*1.25, CLASS_NONE, DMG_VAPOURIZING | DMG_RADIATION | DMG_WALLPIERCING);
			pev->nextthink = gpGlobals->time + 0.25f;
		}
		break;
	case 7:
		{
			FX_Trail(pev->origin, entindex(), FX_ATOMBOMB_DETONATE_PART5 );
			::RadiusDamage(pev->origin, this, pOwner, Damage*0.75, Damage*1.5, CLASS_NONE, DMG_VAPOURIZING | DMG_RADIATION | DMG_WALLPIERCING);
			pev->nextthink = gpGlobals->time + 0.25f;
		}
		break;
	case 8:
		{
			FX_Trail(pev->origin, entindex(), FX_ATOMBOMB_DETONATE_PART6);
			::RadiusDamage(pev->origin, this, pOwner, Damage, Damage*1.75, CLASS_NONE, DMG_VAPOURIZING | DMG_RADIATION | DMG_WALLPIERCING);
			pev->nextthink = gpGlobals->time + 0.20f;
		}
		break;
	case 9:
		{
			FX_Trail(pev->origin, entindex(), FX_ATOMBOMB_DETONATE_PART7 );
			::RadiusDamage(pev->origin, this, pOwner, Damage, Damage*2, CLASS_NONE, DMG_VAPOURIZING | DMG_RADIATION | DMG_WALLPIERCING);
			pev->nextthink = gpGlobals->time + 0.3f;
		}
		break;
	case 10:
		{
//	Doesn't stop!	EMIT_SOUND(ENT(pev), CHAN_ITEM, "ambience/quake02.wav", VOL_NORM, 0.6);
#ifndef CLIENT_DLL
			UTIL_ScreenShakeAll(pev->origin, 15.0f, 6.0f, 4.0f);
			UTIL_ScreenFadeAll(Vector(255, 207, 127), 1, 0.5f, 128, FFADE_IN);
			UTIL_DecalPoints(pev->origin, pev->origin - Vector(0,0,256), ENT(pev), DECAL_NUCBLOW1 + RANDOM_LONG(0,2));
#endif
			pev->nextthink = gpGlobals->time + 0.1f;
		}
		break;
	case 11:
		{
			pev->nextthink = gpGlobals->time + 0.5f;
		}
		break;
	case 12:
		{
			FX_Trail(pev->origin, entindex(), FX_ATOMBOMB_DETONATE_PART7 );
			pev->nextthink = gpGlobals->time + 0.3f;
		break;
		}
	case 13:
		{
			if (!FStringNull(pev->target))
				FireTargets(STRING(pev->target), this, this, USE_TOGGLE, 0);

			if (pev->spawnflags & SF_REPEATABLE)
			{
				SetThink(&CBaseEntity::SUB_DoNothing);
			}
			else
			{
				DoDamageInit(10.0f, 1.0f, 0.0f, DMG_RADIATION | DMG_NEVERGIB);// XDM3035
//				UTIL_Remove(this);
				return;
			}
			pev->nextthink = gpGlobals->time;
		}
		break;
	}
	++pev->oldbuttons;
}


//======================end grenade

