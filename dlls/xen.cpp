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
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "animation.h"
#include "effects.h"
#include "weapons.h"
#include "player.h"
#include "shake.h"
#include "colors.h"// XDM

#define XEN_PLANT_GLOW_SPRITE "sprites/flare3.spr"
#define XEN_PLANT_HIDE_TIME 5

class CActAnimating : public CBaseAnimating
{
public:
	virtual void SetActivity( Activity act );
	inline Activity	GetActivity(void) { return m_Activity; }
	virtual int	ObjectCaps(void) { return CBaseAnimating :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	virtual void TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType );
	virtual int BloodColor(void) { return m_bloodColor; }
	static TYPEDESCRIPTION m_SaveData[];
	int m_bloodColor;

private:
	Activity m_Activity;
};

TYPEDESCRIPTION	CActAnimating::m_SaveData[] = 
{
	DEFINE_FIELD( CActAnimating, m_Activity, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CActAnimating, CBaseAnimating );

void CActAnimating::SetActivity( Activity act ) 
{ 
	int sequence = LookupActivity( act ); 
	if ( sequence != ACTIVITY_NOT_AVAILABLE )
	{
		pev->sequence = sequence;
		m_Activity = act; 
		pev->frame = 0;
		ResetSequenceInfo();
	}
}

void CActAnimating::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
	if(bitsDamageType & (DMG_GENERIC | DMG_CRUSH | DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_SONIC | DMG_ENERGYBEAM | DMG_ALWAYSGIB | DMG_MORTAR))
	{
		if(flDamage < 8)
		{
			flDamage = 8;
		}
		else if(flDamage > 48)
		{
			flDamage = 48;
		}
		UTIL_BloodDrips(ptr->vecEndPos, -vecDir, m_bloodColor, flDamage);
		TraceBleed( flDamage, vecDir, ptr, bitsDamageType );
	}
}

class CXenPLight : public CActAnimating
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Touch( CBaseEntity *pOther );
	virtual void Think(void);
	void LightOn(void);
	void LightOff(void);
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

private:
	CSprite *m_pGlow;
};

LINK_ENTITY_TO_CLASS( xen_plantlight, CXenPLight );

TYPEDESCRIPTION	CXenPLight::m_SaveData[] = 
{
	DEFINE_FIELD( CXenPLight, m_pGlow, FIELD_CLASSPTR ),
};

IMPLEMENT_SAVERESTORE( CXenPLight, CActAnimating );

void CXenPLight :: Spawn(void)
{
	Precache();
	SET_MODEL( ENT(pev), "models/light.mdl" );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_TRIGGER;
	UTIL_SetSize( pev, Vector(-80,-80,0), Vector(80,80,32));
	SetActivity( ACT_IDLE );
	pev->nextthink = gpGlobals->time + 0.1;
	pev->frame = RANDOM_FLOAT(0,255);
	m_pGlow = CSprite::SpriteCreate( XEN_PLANT_GLOW_SPRITE, pev->origin + Vector(0.0f,0.0f,(pev->mins.z+pev->maxs.z)*0.5f), FALSE );
	if (m_pGlow)
	{
		m_pGlow->SetTransparency( kRenderGlow, pev->rendercolor.x, pev->rendercolor.y, pev->rendercolor.z, pev->renderamt, pev->renderfx );
		m_pGlow->SetAttachment( edict(), 1 );
	}
}

void CXenPLight :: Precache(void)
{
	PRECACHE_MODEL("models/light.mdl");
	PRECACHE_MODEL( XEN_PLANT_GLOW_SPRITE );
}

void CXenPLight :: Think(void)
{
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;

	switch( GetActivity() )
	{
	case ACT_CROUCH:
		if ( m_fSequenceFinished )
		{
			SetActivity( ACT_CROUCHIDLE );
			LightOff();
		}
		break;

	case ACT_CROUCHIDLE:
		if ( gpGlobals->time > pev->dmgtime )
		{
			SetActivity( ACT_STAND );
			LightOn();
		}
		break;

	case ACT_STAND:
		if ( m_fSequenceFinished )
			SetActivity( ACT_IDLE );
		break;
	}
}

void CXenPLight :: Touch( CBaseEntity *pOther )
{
	if ( pOther->IsPlayer() )
	{
		pev->dmgtime = gpGlobals->time + XEN_PLANT_HIDE_TIME;
		if ( GetActivity() == ACT_IDLE || GetActivity() == ACT_STAND )
		{
			SetActivity( ACT_CROUCH );
		}
	}
}

void CXenPLight :: LightOn(void)
{
	SUB_UseTargets( this, USE_ON, 0 );
	if (m_pGlow)
		m_pGlow->pev->effects &= ~EF_NODRAW;
}

void CXenPLight :: LightOff(void)
{
	SUB_UseTargets( this, USE_OFF, 0 );
	if (m_pGlow)
		m_pGlow->pev->effects |= EF_NODRAW;
}




class CXenHair : public CActAnimating
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
//	void Think(void);
};

LINK_ENTITY_TO_CLASS( xen_hair, CXenHair );

#define SF_HAIR_SYNC		0x0001

void CXenHair::Spawn(void)
{
	Precache();
	SET_MODEL( edict(), "models/hair.mdl" );
	UTIL_SetSize( pev, Vector(-4,-4,0), Vector(4,4,32));
	pev->sequence = 0;
	if ( !(pev->spawnflags & SF_HAIR_SYNC) )
	{
		pev->frame = RANDOM_FLOAT(0,255);
		pev->framerate = RANDOM_FLOAT( 0.7, 1.4 );
	}
	ResetSequenceInfo();
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;// XDM: don't bleed
	m_bloodColor = DONT_BLEED;
//	pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0.1, 0.4 );	// Load balance these a bit
}
/* XDM3035a: don't overload the server
void CXenHair::Think(void)
{
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.5;
}
*/
void CXenHair::Precache(void)
{
	PRECACHE_MODEL("models/hair.mdl");
}






class CXenTreeTrigger : public CBaseEntity
{
public:
	virtual void Touch( CBaseEntity *pOther );
	static CXenTreeTrigger *TriggerCreate( edict_t *pOwner, const Vector &position );
};
LINK_ENTITY_TO_CLASS( xen_ttrigger, CXenTreeTrigger );

CXenTreeTrigger *CXenTreeTrigger :: TriggerCreate( edict_t *pOwner, const Vector &position )
{
	CXenTreeTrigger *pTrigger = GetClassPtr( (CXenTreeTrigger *)NULL );
	pTrigger->pev->origin = position;
	pTrigger->pev->classname = MAKE_STRING("xen_ttrigger");
	pTrigger->pev->solid = SOLID_TRIGGER;
	pTrigger->pev->movetype = MOVETYPE_NONE;
	pTrigger->pev->owner = pOwner;
	return pTrigger;
}

void CXenTreeTrigger::Touch( CBaseEntity *pOther )
{
	if ( pev->owner )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance(pev->owner);
		pEntity->Touch( pOther );
	}
}

#define TREE_AE_ATTACK		1

class CXenTree : public CActAnimating
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Touch(CBaseEntity *pOther);
	virtual void Think(void);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual void HandleAnimEvent(MonsterEvent_t *pEvent);
	void Attack(void);	
	virtual int Classify(void) { return CLASS_NONE; }
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];
	static const char *pAttackHitSounds[];
	static const char *pAttackMissSounds[];

private:
	CXenTreeTrigger	*m_pTrigger;
};

LINK_ENTITY_TO_CLASS( xen_tree, CXenTree );

TYPEDESCRIPTION	CXenTree::m_SaveData[] = 
{
	DEFINE_FIELD( CXenTree, m_pTrigger, FIELD_CLASSPTR ),
};

IMPLEMENT_SAVERESTORE( CXenTree, CActAnimating );

void CXenTree :: Spawn(void)
{
	Precache();
	SET_MODEL( ENT(pev), "models/tree.mdl" );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_YES;
	UTIL_SetSize( pev, Vector(-30,-30,0), Vector(30,30,188));
	SetActivity( ACT_IDLE );
	pev->nextthink = gpGlobals->time + 0.1;
	pev->frame = RANDOM_FLOAT(0,255);
	pev->framerate = RANDOM_FLOAT( 0.7, 1.4 );
	Vector triggerPosition;
	ANGLE_VECTORS( pev->angles, triggerPosition, NULL, NULL );
	triggerPosition = pev->origin + (triggerPosition * 64);
	m_pTrigger = CXenTreeTrigger::TriggerCreate( edict(), triggerPosition );
	UTIL_SetSize( m_pTrigger->pev, Vector( -24, -24, 0 ), Vector( 24, 24, 128 ) );
	m_bloodColor = BLOOD_COLOR_GREEN;
}

const char *CXenTree::pAttackHitSounds[] = 
{
	"tree/tree_strike1.wav",
	"tree/tree_strike2.wav",
};

const char *CXenTree::pAttackMissSounds[] = 
{
	"tree/tree_miss1.wav",
	"tree/tree_miss2.wav",
};

void CXenTree :: Precache(void)
{
	PRECACHE_MODEL("models/tree.mdl");
	PRECACHE_MODEL( XEN_PLANT_GLOW_SPRITE );
	PRECACHE_SOUND("tree/tree_hit.wav");
	PRECACHE_SOUND_ARRAY( pAttackMissSounds );
}

void CXenTree :: Touch( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() && FClassnameIs( pOther->pev, "monster_bigmomma" ) )
		return;

	Attack();
}

void CXenTree :: Attack(void)
{
	if ( GetActivity() == ACT_IDLE )
	{
		SetActivity( ACT_MELEE_ATTACK1 );
		pev->framerate = RANDOM_FLOAT( 1.0, 1.4 );
		EMIT_SOUND_ARRAY_DYN( CHAN_WEAPON, pAttackMissSounds );
	}
}

int CXenTree::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	EMIT_SOUND(ENT(pev), CHAN_BODY, "tree/tree_hit.wav", VOL_NORM, ATTN_NORM);
	Attack();
	return 0;
}

void CXenTree :: HandleAnimEvent( MonsterEvent_t *pEvent )
{
	switch( pEvent->event )
	{
		case TREE_AE_ATTACK:
		{
			CBaseEntity *pList[8];
			BOOL sound = FALSE;
			int count = UTIL_EntitiesInBox( pList, 8, m_pTrigger->pev->absmin, m_pTrigger->pev->absmax, FL_MONSTER|FL_CLIENT );
			Vector forward;

			ANGLE_VECTORS( pev->angles, forward, NULL, NULL );

			for ( int i = 0; i < count; i++ )
			{
				if ( pList[i] != this )
				{
					if ( pList[i]->pev->owner != edict() )
					{
						sound = TRUE;
						pList[i]->TakeDamage( this, this, 25, DMG_CRUSH | DMG_SLASH );
						pList[i]->pev->punchangle.x = 15;
						pList[i]->pev->velocity = pList[i]->pev->velocity + forward * 100;
					}
				}
			}
					
			if ( sound )
			{
				EMIT_SOUND_ARRAY_DYN( CHAN_WEAPON, pAttackHitSounds );
			}
		}
		return;
	}

	CActAnimating::HandleAnimEvent( pEvent );
}

void CXenTree :: Think(void)
{
	float flInterval = StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;
	DispatchAnimEvents( flInterval );

	switch( GetActivity() )
	{
	case ACT_MELEE_ATTACK1:
		if ( m_fSequenceFinished )
		{
			SetActivity( ACT_IDLE );
			pev->framerate = RANDOM_FLOAT( 0.6, 1.4 );
		}
		break;

	default:
	case ACT_IDLE:
		break;

	}
}



class CXenSpore : public CActAnimating
{
public:
	virtual int Classify(void) { return CLASS_ALIEN_BIOWEAPON; };// XDM: HACK to play hit sounds
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Touch( CBaseEntity *pOther );
	virtual void Think(void);
	virtual void TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType);// XDM
	static const char *pModelNames[];
};

class CXenSporeSmall : public CXenSpore
{
	void Spawn(void);
	void Touch( CBaseEntity *pOther ) {};// XDM: avoid throwing particles
};

class CXenSporeMed : public CXenSpore
{
	void Spawn(void);
};

class CXenSporeLarge : public CXenSpore
{
	void Spawn(void);

	static const Vector m_hullSizes[];
};

// Fake collision box for big spores
class CXenHull : public CPointEntity
{
public:
	int Classify(void) { return CLASS_ALIEN_BIOWEAPON; };// XDM: HACK to play hit sounds
	static CXenHull	*CreateHull( CBaseEntity *source, const Vector &mins, const Vector &maxs, const Vector &offset );
};

CXenHull *CXenHull :: CreateHull( CBaseEntity *source, const Vector &mins, const Vector &maxs, const Vector &offset )
{
	if (!source)// XDM3034
		return NULL;

	CXenHull *pHull = GetClassPtr((CXenHull *)NULL, "xen_hull");// XDM
	UTIL_SetOrigin( pHull->pev, source->pev->origin + offset );
	SET_MODEL( pHull->edict(), STRING(source->pev->model) );
	pHull->pev->solid = SOLID_BBOX;
	pHull->pev->movetype = MOVETYPE_NONE;
	pHull->pev->owner = source->edict();
	UTIL_SetSize( pHull->pev, mins, maxs );
	pHull->pev->renderamt = 0;
	pHull->pev->rendermode = kRenderTransTexture;
	//	pHull->pev->effects |= EF_NODRAW;
	return pHull;
}

LINK_ENTITY_TO_CLASS( xen_spore_small, CXenSporeSmall );
LINK_ENTITY_TO_CLASS( xen_spore_medium, CXenSporeMed );
LINK_ENTITY_TO_CLASS( xen_spore_large, CXenSporeLarge );
LINK_ENTITY_TO_CLASS( xen_hull, CXenHull );

void CXenSporeSmall::Spawn(void)
{
	pev->impulse = 0;
	CXenSpore::Spawn();
	UTIL_SetSize(pev, Vector(-16,-16,0), Vector(16,16,8));
	m_bloodColor = BLOOD_COLOR_GREEN;
}

void CXenSporeMed::Spawn(void)
{
	pev->impulse = 1;
	CXenSpore::Spawn();
	UTIL_SetSize(pev, Vector(-40,-40,0), Vector(40,40,120));
	m_bloodColor = BLOOD_COLOR_GRAY;
}

// I just eyeballed these -- fill in hulls for the legs
const Vector CXenSporeLarge::m_hullSizes[] = 
{
	Vector( 90, -25, 0 ),
	Vector( 25, 75, 0 ),
	Vector( -15, -100, 0 ),
	Vector( -90, -35, 0 ),
	Vector( -90, 60, 0 ),
};

void CXenSporeLarge::Spawn(void)
{
	pev->impulse = 2;
	CXenSpore::Spawn();
	UTIL_SetSize(pev, Vector(-48,-48,110), Vector(48,48,240));
	Vector forward, right;
	ANGLE_VECTORS(pev->angles, forward, right, NULL);
	m_bloodColor = BLOOD_COLOR_HUM_SKN;

	// Rotate the leg hulls into position
	for (int i = 0; i < ARRAYSIZE(m_hullSizes); i++)
		CXenHull::CreateHull(this, Vector(-12, -12, 0), Vector(12, 12, 120), (m_hullSizes[i].x * forward) + (m_hullSizes[i].y * right));
}

void CXenSpore :: Spawn(void)
{
	Precache();
	SET_MODEL(ENT(pev), pModelNames[pev->impulse]);
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_BBOX;
	pev->takedamage = DAMAGE_NO;// XDM: no
//	SetActivity( ACT_IDLE );
	pev->sequence = 0;
	pev->frame = RANDOM_FLOAT(0,255);
	pev->framerate = RANDOM_FLOAT(0.7, 1.4);
	ResetSequenceInfo();
	m_bloodColor = BLOOD_COLOR_GREEN;
	pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.1, 0.4);	// Load balance these a bit
	pev->dmgtime = gpGlobals->time + 0.1;
}

const char *CXenSpore::pModelNames[] = 
{
	"models/fungus(small).mdl",
	"models/fungus.mdl",
	"models/fungus(large).mdl",
};

void CXenSpore::Precache(void)
{
	PRECACHE_MODEL((char *)pModelNames[pev->impulse]);
}

void CXenSpore::Touch( CBaseEntity *pOther )
{
	if (pOther->IsPlayer() && pev->dmgtime <= gpGlobals->time)// XDM
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pOther;
		if(!pPlayer->m_fFrozen)
		{
			switch (m_bloodColor)
			{
			default:
			case BLOOD_COLOR_YELLOW:	UTIL_ScreenFade(pPlayer, Vector(200, 200, 50), 1.0, 1.0, 100, FFADE_IN); break;
			case BLOOD_COLOR_GREEN:		UTIL_ScreenFade(pPlayer, Vector(0, 200, 50), 1.0, 1.0, 100, FFADE_IN); break;
			case BLOOD_COLOR_HUM_SKN:	UTIL_ScreenFade(pPlayer, Vector(200, 160, 50), 1.0, 1.0, 100, FFADE_IN); break;
			}
		}
		ParticlesCustom(pev->origin + Vector(0.0f,0.0f,pev->maxs.z-4.0f), 80.0, 1.0, m_bloodColor, 32);
		pev->dmgtime = gpGlobals->time + 2.0;
	}
}

void CXenSpore::Think(void)
{
/*	float flInterval = */StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1;
}

void CXenSpore::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
	StreakSplash(ptr->vecEndPos, ptr->vecPlaneNormal, UTIL_BloodToStreak(m_bloodColor), 10, 30, 60);
	UTIL_BloodDrips(ptr->vecEndPos, ptr->vecPlaneNormal, m_bloodColor, flDamage);
}
