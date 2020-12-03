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
#include "monsters.h"
#include "customentity.h"
#include "effects.h"
#include "weapons.h"
#include "decals.h"
#include "pm_materials.h"
#include "func_break.h"
#include "shake.h"
#include "studio.h"
#include "player.h"
#include "gamerules.h"
#include "game.h"
#include "globals.h"
#include "sound.h"
#include "projectiles.h"

//LINK_ENTITY_TO_CLASS( info_target, CPointEntity );

class CBubbling : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void KeyValue( KeyValueData *pkvd );
	void EXPORT FizzThink(void);
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	virtual int ObjectCaps(void) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	static TYPEDESCRIPTION m_SaveData[];
	int m_density;
	int m_frequency;
//	int m_bubbleModel;
	int m_state;
};

LINK_ENTITY_TO_CLASS( env_bubbles, CBubbling );

TYPEDESCRIPTION	CBubbling::m_SaveData[] =
{
	DEFINE_FIELD( CBubbling, m_density, FIELD_INTEGER ),
	DEFINE_FIELD( CBubbling, m_frequency, FIELD_INTEGER ),
	DEFINE_FIELD( CBubbling, m_state, FIELD_INTEGER ),
	// Let spawn restore this!
	//DEFINE_FIELD( CBubbling, m_bubbleModel, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CBubbling, CBaseEntity );

#define SF_BUBBLES_START_OFF		0x0001

void CBubbling::Spawn(void)
{
	Precache();
	SET_MODEL( ENT(pev), STRING(pev->model) );		// Set size

	pev->solid = SOLID_NOT;							// Remove model & collisions
	pev->renderamt = 0;								// The engine won't draw this model if this is set to 0 and blending is on
	pev->rendermode = kRenderTransTexture;
	int speed = pev->speed > 0 ? pev->speed : -pev->speed;

	// HACKHACK!!! - Speed in rendercolor
	pev->rendercolor.x = speed >> 8;
	pev->rendercolor.y = speed & 255;
	pev->rendercolor.z = (pev->speed < 0) ? 1 : 0;

	if ( !(pev->spawnflags & SF_BUBBLES_START_OFF) )
	{
		SetThink(&CBubbling::FizzThink);
		pev->nextthink = gpGlobals->time + 2.0;
		m_state = 1;
	}
	else
		m_state = 0;
}

void CBubbling::Precache(void)
{
//	m_bubbleModel = PRECACHE_MODEL("sprites/bubble.spr");			// Precache bubble sprite
}

void CBubbling::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (g_pdeveloper->value <= 0.0f)
		return;

	if (ShouldToggle(useType, (m_state > 0)))
		m_state = !m_state;

	if (m_state)
	{
		SetThink(&CBubbling::FizzThink);
		pev->nextthink = gpGlobals->time + 0.1;
	}
	else
	{
		SetThinkNull();
		pev->nextthink = 0;
	}
}

void CBubbling::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "density"))
	{
		m_density = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "frequency"))
	{
		m_frequency = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "current"))
	{
		pev->speed = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CBubbling::FizzThink(void)
{
/*
	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, VecBModelOrigin(pev) );
		WRITE_BYTE( TE_FIZZ );
		WRITE_SHORT( (short)ENTINDEX( edict() ) );
		WRITE_SHORT( (short)m_bubbleModel );
		WRITE_BYTE( m_density );
	MESSAGE_END();
*/
	Vector vecBottomCenter((pev->absmax.x+pev->absmin.x)*0.5f, (pev->absmax.y+pev->absmin.y)*0.5f, pev->absmin.z+4.0f);// HACK: magic number??
//	Vector vecBottomCenter(Center());
//	UTIL_ShowBox(pev->origin, pev->mins, pev->maxs, 10, 0,255,0);
//	UTIL_ShowLine(vecBottomCenter, vecBottomCenter + Vector(0,0,16), 2.0, 0,255,0);

	Vector vecQVolume((pev->absmax.x-pev->absmin.x)*0.5f, (pev->absmax.y-pev->absmin.y)*0.5f, 0.5f);// resulting thickness is x2
	FX_BubblesBox(vecBottomCenter, vecQVolume, m_density*3);// XDM3035c: HL bubbles only spawn at bottom
//	UTIL_ShowBox(vecBottomCenter, -vecQVolume, vecQVolume, 2.0, 255,0,0);

	if ( m_frequency > 19 )
		pev->nextthink = gpGlobals->time + 0.5;
	else
		pev->nextthink = gpGlobals->time + 2.5 - (0.1 * m_frequency);
}









// --------------------------------------------------
//
// Beams
//
// --------------------------------------------------

LINK_ENTITY_TO_CLASS( beam, CBeam );

void CBeam::Spawn(void)
{
	pev->solid = SOLID_NOT; // Remove model & collisions
	Precache();
}

void CBeam::Precache(void)
{
	if ( pev->owner )
		SetStartEntity( ENTINDEX( pev->owner ) );
	if ( pev->aiment )
		SetEndEntity( ENTINDEX( pev->aiment ) );
}

int CBeam::ObjectCaps(void)
{
	int flags = 0;
	if (pev->spawnflags & SF_BEAM_TEMPORARY)
		flags = FCAP_DONT_SAVE;

	return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | flags;
}

void CBeam::SetStartEntity( int entityIndex )
{
	pev->sequence = (entityIndex & 0x0FFF) | ((pev->sequence&0xF000)<<12);
	pev->owner = INDEXENT(entityIndex);
}

void CBeam::SetEndEntity( int entityIndex )
{
	pev->skin = (entityIndex & 0x0FFF) | ((pev->skin&0xF000)<<12);
	pev->aiment = INDEXENT(entityIndex);
}

// These don't take attachments into account
const Vector &CBeam::GetStartPos(void)
{
	if ( GetType() == BEAM_ENTS )
	{
		edict_t *pent = INDEXENT(GetStartEntity());
		if (pent)
			return pent->v.origin;
	}
	return pev->origin;
}

const Vector &CBeam::GetEndPos(void)
{
	int type = GetType();
	if ( type == BEAM_POINTS || type == BEAM_HOSE )
	{
		return pev->angles;
	}

	edict_t *pent = INDEXENT(GetEndEntity());
	if ( pent )
		return pent->v.origin;
	return pev->angles;
}

CBeam *CBeam::BeamCreate( const char *pSpriteName, int width )
{
	// Create a new entity with CBeam private data
	CBeam *pBeam = GetClassPtr( (CBeam *)NULL );
	if (pBeam)
	{
		pBeam->pev->classname = MAKE_STRING("beam");
		pBeam->BeamInit( pSpriteName, width );
	}
	return pBeam;
}

void CBeam::BeamInit( const char *pSpriteName, int width )
{
	pev->flags |= FL_CUSTOMENTITY;
	SetColor( 255, 255, 255 );
	SetBrightness( 255 );
	SetNoise( 0 );
	SetFrame( 0 );
	SetScrollRate( 0 );
	pev->model = MAKE_STRING( pSpriteName );
	SetTexture( PRECACHE_MODEL( (char *)pSpriteName ) );
	SetWidth( width );
	pev->skin = 0;
	pev->sequence = 0;
	pev->rendermode = 0;
}

void CBeam::PointsInit( const Vector &start, const Vector &end )
{
	SetType( BEAM_POINTS );
	SetStartPos( start );
	SetEndPos( end );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::HoseInit( const Vector &start, const Vector &direction )
{
	SetType( BEAM_HOSE );
	SetStartPos( start );
	SetEndPos( direction );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::PointEntInit( const Vector &start, int endIndex )
{
	SetType( BEAM_ENTPOINT );
	SetStartPos( start );
	SetEndEntity( endIndex );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::EntPointInit( int endIndex, const Vector &end )// XDM: TODO: this does not seem to work
{
	SetType(BEAM_ENTPOINT);
	SetStartEntity( endIndex );
	SetEndPos( end );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::EntsInit( int startIndex, int endIndex )
{
	SetType( BEAM_ENTS );
	SetStartEntity( startIndex );
	SetEndEntity( endIndex );
	SetStartAttachment( 0 );
	SetEndAttachment( 0 );
	RelinkBeam();
}

void CBeam::RelinkBeam(void)
{
	const Vector &startPos = GetStartPos(), &endPos = GetEndPos();
	pev->mins.x = min( startPos.x, endPos.x );
	pev->mins.y = min( startPos.y, endPos.y );
	pev->mins.z = min( startPos.z, endPos.z );
	pev->maxs.x = max( startPos.x, endPos.x );
	pev->maxs.y = max( startPos.y, endPos.y );
	pev->maxs.z = max( startPos.z, endPos.z );
	pev->mins = pev->mins - pev->origin;
	pev->maxs = pev->maxs - pev->origin;
	UTIL_SetSize( pev, pev->mins, pev->maxs );
	UTIL_SetOrigin( pev, pev->origin );
}

#if 0
void CBeam::SetObjectCollisionBox(void)
{
	const Vector &startPos = GetStartPos(), &endPos = GetEndPos();

	pev->absmin.x = min( startPos.x, endPos.x );
	pev->absmin.y = min( startPos.y, endPos.y );
	pev->absmin.z = min( startPos.z, endPos.z );
	pev->absmax.x = max( startPos.x, endPos.x );
	pev->absmax.y = max( startPos.y, endPos.y );
	pev->absmax.z = max( startPos.z, endPos.z );
}
#endif

void CBeam::TriggerTouch( CBaseEntity *pOther )
{
	if ( pOther->pev->flags & (FL_CLIENT | FL_MONSTER) )
	{
		if ( pev->owner )
		{
			CBaseEntity *pOwner = CBaseEntity::Instance(pev->owner);
			pOwner->Use( pOther, this, USE_TOGGLE, 0 );
		}
		ALERT( at_console, "Firing targets!!!\n" );
	}
}

CBaseEntity *CBeam::RandomTargetname( const char *szName )
{
	int total = 0;
	CBaseEntity *pEntity = NULL;
	CBaseEntity *pNewEntity = NULL;
	while ((pNewEntity = UTIL_FindEntityByTargetname( pNewEntity, szName )) != NULL)
	{
		total++;
		if (RANDOM_LONG(0,total-1) < 1)
			pEntity = pNewEntity;
	}
	return pEntity;
}

void CBeam::DoSparks( const Vector &start, const Vector &end )
{
	if (g_pGameRules->FAllowEffects())// XDM3035
	{
		if (pev->spawnflags & (SF_BEAM_SPARKSTART|SF_BEAM_SPARKEND))
		{
			if ( pev->spawnflags & SF_BEAM_SPARKSTART )
			{
				UTIL_Sparks(start);
			}
			if ( pev->spawnflags & SF_BEAM_SPARKEND )
			{
				UTIL_Sparks(end);
			}
		}
	}
}

void CBeam::BeamDamage(TraceResult *ptr, CBaseEntity *pAttacker)// XDM3035
{
	RelinkBeam();
	if (ptr->flFraction != 1.0 && ptr->pHit != NULL)
	{
		CBaseEntity *pHit = CBaseEntity::Instance(ptr->pHit);
		if (pHit)
		{
			ClearMultiDamage();
			pHit->TraceAttack(pAttacker, pev->dmg * (gpGlobals->time - pev->dmgtime), (ptr->vecEndPos - pev->origin).Normalize(), ptr, DMG_ENERGYBEAM );
			ApplyMultiDamage(this, pAttacker);
			if (g_pGameRules->FAllowEffects() && (pev->spawnflags & SF_BEAM_DECALS))
			{
				if (pHit->IsBSPModel())
					UTIL_DecalTrace(ptr, DECAL_BIGSHOT1 + RANDOM_LONG(0,4));
			}
		}
	}
	pev->dmgtime = gpGlobals->time;
}

// XDM
void CBeam::Expand(float scaleSpeed, float fadeSpeed)
{
	pev->speed = scaleSpeed;
	pev->health = fadeSpeed;
	SetThink(&CBeam::ExpandThink);
	pev->nextthink = gpGlobals->time;
	pev->ltime = gpGlobals->time;
}

void CBeam::ExpandThink(void)
{
	float frametime = gpGlobals->time - pev->ltime;

//	if (pev->body < 255)
		pev->body ++;

	pev->scale += pev->speed * frametime;// width
	pev->renderamt -= pev->health * frametime;

	if (pev->renderamt <= 0)
	{
		pev->renderamt = 0;

		if (pev->flags & FL_GODMODE)// XDM3034
		{
			pev->effects |= EF_NODRAW;
			SetThinkNull();
		}
		else
			UTIL_Remove(this);
	}
	else
	{
		pev->nextthink = gpGlobals->time + 0.1;
		pev->ltime = gpGlobals->time;
	}
}
/*
CBaseEntity *CBeam::GetTripEntity(TraceResult *ptr)
{
	CBaseEntity *pTrip = NULL;

	if (ptr->flFraction == 1.0 || ptr->pHit == NULL)
		return NULL;

	pTrip = CBaseEntity::Instance(ptr->pHit);
	if (pTrip == NULL)
		return NULL;

	if (FStringNull(pev->netname))
	{
		if (pTrip->pev->flags & (FL_CLIENT | FL_MONSTER))
			return pTrip;
		else
			return NULL;
	}
	else if (FClassnameIs(pTrip->pev, STRING(pev->netname)))
		return pTrip;
	else if (FStrEq(STRING(pTrip->pev->targetname), STRING(pev->netname)))
		return pTrip;
	else
		return NULL;
}
*/



LINK_ENTITY_TO_CLASS( env_lightning, CLightning );
LINK_ENTITY_TO_CLASS( env_beam, CLightning );

TYPEDESCRIPTION	CLightning::m_SaveData[] =
{
	DEFINE_FIELD( CLightning, m_active, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_iszStartEntity, FIELD_STRING ),
	DEFINE_FIELD( CLightning, m_iszEndEntity, FIELD_STRING ),
	DEFINE_FIELD( CLightning, m_life, FIELD_FLOAT ),
	DEFINE_FIELD( CLightning, m_boltWidth, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_noiseAmplitude, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_brightness, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_speed, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_restrike, FIELD_FLOAT ),
	DEFINE_FIELD( CLightning, m_spriteTexture, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_iszSpriteName, FIELD_STRING ),
	DEFINE_FIELD( CLightning, m_frameStart, FIELD_INTEGER ),
	DEFINE_FIELD( CLightning, m_radius, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CLightning, CBeam );


void CLightning::Spawn(void)
{
	if ( FStringNull( m_iszSpriteName ) )
	{
		SetThink(&CLightning::SUB_Remove);
		return;
	}
	pev->solid = SOLID_NOT;							// Remove model & collisions
	Precache();
	pev->dmgtime = gpGlobals->time;

	if ( ServerSide() )
	{
		SetThinkNull();
		if ( pev->dmg > 0 )
		{
			SetThink(&CLightning::DamageThink );
			pev->nextthink = gpGlobals->time + 0.1;
		}
		if ( pev->targetname )
		{
			if ( !(pev->spawnflags & SF_BEAM_STARTON) )
			{
				pev->effects = EF_NODRAW;
				m_active = 0;
				pev->nextthink = 0;
			}
			else
				m_active = 1;

			SetUse(&CLightning::ToggleUse);
		}
	}
	else
	{
		m_active = 0;
		if ( !FStringNull(pev->targetname) )
		{
			SetUse(&CLightning::StrikeUse);
		}
		if ( FStringNull(pev->targetname) || FBitSet(pev->spawnflags, SF_BEAM_STARTON) )
		{
			SetThink(&CLightning::StrikeThink);
			pev->nextthink = gpGlobals->time + 1.0;
		}
	}
}

void CLightning::Precache(void)
{
	m_spriteTexture = PRECACHE_MODEL(STRINGV(m_iszSpriteName));
	CBeam::Precache();
}

void CLightning::Activate(void)
{
	if ( ServerSide() )
		BeamUpdateVars();
}

void CLightning::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "LightningStart"))
	{
		m_iszStartEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "LightningEnd"))
	{
		m_iszEndEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "life"))
	{
		m_life = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "BoltWidth"))
	{
		m_boltWidth = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "NoiseAmplitude"))
	{
		m_noiseAmplitude = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "TextureScroll"))
	{
		m_speed = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "StrikeTime"))
	{
		m_restrike = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "texture"))
	{
		m_iszSpriteName = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "framestart"))
	{
		m_frameStart = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "Radius"))
	{
		m_radius = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "damage"))
	{
		pev->dmg = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBeam::KeyValue( pkvd );
}

void CLightning::ToggleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (!ShouldToggle(useType, (m_active > 0)))
		return;

	if ( m_active )
	{
		m_active = 0;
		pev->effects |= EF_NODRAW;
		pev->nextthink = 0;
	}
	else
	{
		if (pActivator)// XDM3035
			pev->dmg_inflictor = pActivator->edict();
		else
			pev->dmg_inflictor = NULL;

		m_active = 1;
		pev->effects &= ~EF_NODRAW;
		DoSparks( GetStartPos(), GetEndPos() );
		if ( pev->dmg > 0 )
		{
			pev->nextthink = gpGlobals->time;
			pev->dmgtime = gpGlobals->time;
		}
	}
}

void CLightning::StrikeUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (!ShouldToggle(useType, (m_active > 0)))
		return;

	if ( m_active )
	{
		m_active = 0;
		SetThinkNull();
	}
	else
	{
		if (pActivator)// XDM3035
			pev->dmg_inflictor = pActivator->edict();
		else
			pev->dmg_inflictor = NULL;

		SetThink(&CLightning::StrikeThink);
		pev->nextthink = gpGlobals->time + 0.1;
	}

	if ( !FBitSet( pev->spawnflags, SF_BEAM_TOGGLE ) )
		SetUseNull();
}

void CLightning::StrikeThink(void)
{
	if ( m_life != 0 )
	{
		if ( pev->spawnflags & SF_BEAM_RANDOM )
			pev->nextthink = gpGlobals->time + m_life + RANDOM_FLOAT( 0, m_restrike );
		else
			pev->nextthink = gpGlobals->time + m_life + m_restrike;
	}
	m_active = 1;

	if (FStringNull(m_iszEndEntity))
	{
		if (FStringNull(m_iszStartEntity))
		{
			RandomArea();
		}
		else
		{
			CBaseEntity *pStart = RandomTargetname( STRING(m_iszStartEntity) );
			if (pStart != NULL)
				RandomPoint( pStart->pev->origin );
			else
				ALERT(at_console, "%s %s: unknown entity \"%s\"\n", STRING(pev->classname), STRING(pev->targetname), STRING(m_iszStartEntity));
		}
		return;
	}

	CBaseEntity *pStart = RandomTargetname( STRING(m_iszStartEntity) );
	CBaseEntity *pEnd = RandomTargetname( STRING(m_iszEndEntity) );

	if ( pStart != NULL && pEnd != NULL )
	{
		if ( IsPointEntity( pStart ) || IsPointEntity( pEnd ) )
		{
			if ( pev->spawnflags & SF_BEAM_RING)
				return;// don't work
		}
//		ALERT(at_console, "DBG: CLightning::StrikeThink(TE_BEAM*)\n");// XDM
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			if ( IsPointEntity( pStart ) || IsPointEntity( pEnd ) )
			{
				if ( !IsPointEntity( pEnd ) )	// One point entity must be in pEnd
				{
					CBaseEntity *pTemp = pStart;
					pStart = pEnd;
					pEnd = pTemp;
				}
				if ( !IsPointEntity( pStart ) )	// One sided
				{
					WRITE_BYTE( TE_BEAMENTPOINT );
					WRITE_SHORT( pStart->entindex() );
					WRITE_COORD( pEnd->pev->origin.x);
					WRITE_COORD( pEnd->pev->origin.y);
					WRITE_COORD( pEnd->pev->origin.z);
				}
				else
				{
					WRITE_BYTE( TE_BEAMPOINTS);
					WRITE_COORD( pStart->pev->origin.x);
					WRITE_COORD( pStart->pev->origin.y);
					WRITE_COORD( pStart->pev->origin.z);
					WRITE_COORD( pEnd->pev->origin.x);
					WRITE_COORD( pEnd->pev->origin.y);
					WRITE_COORD( pEnd->pev->origin.z);
				}
			}
			else
			{
				if ( pev->spawnflags & SF_BEAM_RING)
					WRITE_BYTE( TE_BEAMRING );
				else
					WRITE_BYTE( TE_BEAMENTS );
				WRITE_SHORT( pStart->entindex() );
				WRITE_SHORT( pEnd->entindex() );
			}

			WRITE_SHORT( m_spriteTexture );
			WRITE_BYTE( m_frameStart );				// framestart
			WRITE_BYTE( (int)pev->framerate);		// framerate
			WRITE_BYTE( (int)(m_life*10.0) );		// life
			WRITE_BYTE( m_boltWidth );				// width
			WRITE_BYTE( m_noiseAmplitude );			// noise
			WRITE_BYTE( (int)pev->rendercolor.x );	// r, g, b
			WRITE_BYTE( (int)pev->rendercolor.y );	// r, g, b
			WRITE_BYTE( (int)pev->rendercolor.z );	// r, g, b
			WRITE_BYTE( pev->renderamt );			// brightness
			WRITE_BYTE( m_speed );					// speed
		MESSAGE_END();
		DoSparks( pStart->pev->origin, pEnd->pev->origin );
		if ( pev->dmg > 0)
		{
			TraceResult tr;
			UTIL_TraceLine( pStart->pev->origin, pEnd->pev->origin, dont_ignore_monsters, NULL, &tr );
			BeamDamageInstant( &tr, pev->dmg );
		}
	}
}

void CLightning::DamageThink(void)
{
	pev->nextthink = gpGlobals->time + 0.1;
	TraceResult tr;
	UTIL_TraceLine( GetStartPos(), GetEndPos(), dont_ignore_monsters, NULL, &tr );
	BeamDamage( &tr, this );
}

// This is called from RandomArea() and RandomPoint() only. Simple beam doesn't send any MSGs.
void CLightning::Zap( const Vector &vecSrc, const Vector &vecDest )
{
//	ALERT(at_console, "DBG: CLightning::Zap()\n");// XDM
#if 1
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BEAMPOINTS);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(vecDest.x);
		WRITE_COORD(vecDest.y);
		WRITE_COORD(vecDest.z);
		WRITE_SHORT(m_spriteTexture);
		WRITE_BYTE(m_frameStart); // framestart
		WRITE_BYTE((int)pev->framerate); // framerate
		WRITE_BYTE((int)(m_life*10.0f)); // life
		WRITE_BYTE(m_boltWidth);  // width
		WRITE_BYTE(m_noiseAmplitude);   // noise
		WRITE_BYTE((int)pev->rendercolor.x);	// r, g, b
		WRITE_BYTE((int)pev->rendercolor.y);	// r, g, b
		WRITE_BYTE((int)pev->rendercolor.z);	// r, g, b
		WRITE_BYTE(pev->renderamt);	// brightness
		WRITE_BYTE(m_speed);		// speed
	MESSAGE_END();
#else
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_LIGHTNING);
		WRITE_COORD(vecSrc.x);
		WRITE_COORD(vecSrc.y);
		WRITE_COORD(vecSrc.z);
		WRITE_COORD(vecDest.x);
		WRITE_COORD(vecDest.y);
		WRITE_COORD(vecDest.z);
		WRITE_BYTE((int)(m_life*10.0));
		WRITE_BYTE(m_boltWidth);
		WRITE_BYTE(m_noiseAmplitude);
		WRITE_SHORT(m_spriteTexture);
	MESSAGE_END();
#endif
	DoSparks( vecSrc, vecDest );
}

void CLightning::RandomArea(void)
{
	int iLoops = 0;
	Vector vecSrc = pev->origin;
	TraceResult tr1;
	for (iLoops = 0; iLoops < 10; iLoops++)
	{
		Vector vecDir1 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ),RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir1 = vecDir1.Normalize();
		UTIL_TraceLine( vecSrc, vecSrc + vecDir1 * m_radius, ignore_monsters, ENT(pev), &tr1 );

		if (tr1.flFraction == 1.0)
			continue;

		Vector vecDir2;
		do {
			vecDir2 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ),RANDOM_FLOAT( -1.0, 1.0 ) );
		} while (DotProduct(vecDir1, vecDir2 ) > 0);
		vecDir2 = vecDir2.Normalize();
		TraceResult		tr2;
		UTIL_TraceLine( vecSrc, vecSrc + vecDir2 * m_radius, ignore_monsters, ENT(pev), &tr2 );

		if (tr2.flFraction == 1.0)
			continue;

		if ((tr1.vecEndPos - tr2.vecEndPos).Length() < m_radius * 0.1)
			continue;

		UTIL_TraceLine( tr1.vecEndPos, tr2.vecEndPos, ignore_monsters, ENT(pev), &tr2 );

		if (tr2.flFraction != 1.0)
			continue;

		Zap( tr1.vecEndPos, tr2.vecEndPos );
		break;
	}
}

void CLightning::RandomPoint( Vector &vecSrc )
{
	int iLoops = 0;
	TraceResult tr1;
	for (iLoops = 0; iLoops < 10; iLoops++)
	{
		Vector vecDir1 = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ),RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir1 = vecDir1.Normalize();
		UTIL_TraceLine( vecSrc, vecSrc + vecDir1 * m_radius, ignore_monsters, ENT(pev), &tr1 );

		if ((tr1.vecEndPos - vecSrc).Length() < m_radius * 0.1)
			continue;

		if (tr1.flFraction == 1.0)
			continue;

		Zap( vecSrc, tr1.vecEndPos );
		break;
	}
}

void CLightning::BeamUpdateVars(void)
{
	int beamType;
	int pointStart, pointEnd;

	CBaseEntity *pStart = UTIL_FindEntityByTargetname ( NULL, STRING(m_iszStartEntity) );
	CBaseEntity *pEnd   = UTIL_FindEntityByTargetname ( NULL, STRING(m_iszEndEntity) );
	if (!pStart || !pEnd) return;
	pointStart = IsPointEntity( pStart );
	pointEnd = IsPointEntity( pEnd );

	pev->skin = 0;
	pev->sequence = 0;
	pev->rendermode = 0;
	pev->flags |= FL_CUSTOMENTITY;
	pev->model = m_iszSpriteName;
	SetTexture( m_spriteTexture );

	beamType = BEAM_ENTS;
	if ( pointStart || pointEnd )
	{
		if ( !pointStart )	// One point entity must be in pStart
		{
			CBaseEntity *pTemp;
			// Swap start & end
			pTemp = pStart;
			pStart = pEnd;
			pEnd = pTemp;
			int swap = pointStart;
			pointStart = pointEnd;
			pointEnd = swap;
		}
		if ( !pointEnd )
			beamType = BEAM_ENTPOINT;
		else
			beamType = BEAM_POINTS;
	}

	SetType( beamType );
	if ( beamType == BEAM_POINTS || beamType == BEAM_ENTPOINT || beamType == BEAM_HOSE )
	{
		SetStartPos( pStart->pev->origin );
		if ( beamType == BEAM_POINTS || beamType == BEAM_HOSE )
			SetEndPos( pEnd->pev->origin );
		else
			SetEndEntity( ENTINDEX(ENT(pEnd->pev)) );
	}
	else
	{
		SetStartEntity( ENTINDEX(ENT(pStart->pev)) );
		SetEndEntity( ENTINDEX(ENT(pEnd->pev)) );
	}

	RelinkBeam();

	SetWidth( m_boltWidth );
	SetNoise( m_noiseAmplitude );
	SetFrame( m_frameStart );
	SetScrollRate( m_speed );
	if ( pev->spawnflags & SF_BEAM_SHADEIN )
		SetFlags( BEAM_FSHADEIN );
	else if ( pev->spawnflags & SF_BEAM_SHADEOUT )
		SetFlags( BEAM_FSHADEOUT );
	else if ( pev->spawnflags & SF_BEAM_SOLID )
		SetFlags( BEAM_FSOLID );
}












LINK_ENTITY_TO_CLASS( env_laser, CLaser );

TYPEDESCRIPTION	CLaser::m_SaveData[] =
{
	DEFINE_FIELD( CLaser, m_pSprite, FIELD_CLASSPTR ),
	DEFINE_FIELD( CLaser, m_iszSpriteName, FIELD_STRING ),
	DEFINE_FIELD( CLaser, m_firePosition, FIELD_POSITION_VECTOR ),
};

IMPLEMENT_SAVERESTORE( CLaser, CBeam );

void CLaser::Spawn(void)
{
	if (FStringNull(pev->model))
	{
		SetThink(&CLaser::SUB_Remove);
		return;
	}
	pev->solid = SOLID_NOT;							// Remove model & collisions
	Precache();

	SetThink(&CLaser::StrikeThink);
	pev->flags |= FL_CUSTOMENTITY;

	PointsInit( pev->origin, pev->origin );

	if (!m_pSprite && m_iszSpriteName)
		m_pSprite = CSprite::SpriteCreate( STRING(m_iszSpriteName), pev->origin, TRUE );
	else
		m_pSprite = NULL;

	if (m_pSprite)
	{
		m_pSprite->SetTransparency( kRenderGlow, pev->rendercolor.x, pev->rendercolor.y, pev->rendercolor.z, pev->renderamt, pev->renderfx );
		m_pSprite->pev->framerate = pev->framerate;// XDM3035
	}

	if ( pev->targetname && !(pev->spawnflags & SF_BEAM_STARTON) )
		TurnOff();
	else
		TurnOn();
}

void CLaser::Precache(void)
{
	pev->modelindex = PRECACHE_MODEL(STRINGV(pev->model));
	if (m_iszSpriteName)
		PRECACHE_MODEL(STRINGV(m_iszSpriteName));
}

void CLaser::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "LaserTarget"))
	{
		pev->message = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "width"))
	{
		SetWidth( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "NoiseAmplitude"))
	{
		SetNoise( atoi(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "TextureScroll"))
	{
		SetScrollRate( atoi(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "texture"))
	{
		pev->model = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "EndSprite"))
	{
		m_iszSpriteName = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "framestart"))
	{
		pev->frame = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "damage"))
	{
		pev->dmg = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBeam::KeyValue( pkvd );
}

void CLaser::UpdateOnRemove(void)// XDM3035a: remove child sprite along with the beam
{
	if (m_pSprite)
	{
		UTIL_Remove(m_pSprite);
		m_pSprite = NULL;
	}
}

int CLaser::IsOn(void)
{
	if (pev->effects & EF_NODRAW)
		return 0;
	return 1;
}

void CLaser::TurnOff(void)
{
	pev->effects |= EF_NODRAW;
	pev->nextthink = 0;

	if (m_pSprite)
		m_pSprite->TurnOff();
}

void CLaser::TurnOn(void)
{
	pev->effects &= ~EF_NODRAW;
	if (m_pSprite)
		m_pSprite->TurnOn();

	pev->dmgtime = gpGlobals->time;
	pev->nextthink = gpGlobals->time;
}

void CLaser::TurnOffThink(void)// XDM3035
{
	SetThinkNull();
	pev->nextthink = 0;

	if (IsOn())
		TurnOff();
}

void CLaser::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	int active = IsOn();
	if (!ShouldToggle(useType, (active > 0)))
		return;

	if (pActivator)// XDM3035
		pev->dmg_inflictor = pActivator->edict();
	else
		pev->dmg_inflictor = NULL;

	if (active)
		TurnOff();
	else
		TurnOn();
}

void CLaser::FireAtPoint(TraceResult &tr, CBaseEntity *pAttacker)
{
	SetEndPos(tr.vecEndPos);
	if (m_pSprite)
		UTIL_SetOrigin(m_pSprite->pev, tr.vecEndPos);

	BeamDamage(&tr, pAttacker);
	DoSparks(GetStartPos(), tr.vecEndPos);
}

void CLaser::StrikeThink(void)
{
	CBaseEntity *pEnd = RandomTargetname(STRING(pev->message));
	CBaseEntity *pAttacker = this;// XDM3035
	if (pev->dmg_inflictor)
		pAttacker = CBaseEntity::Instance(pev->dmg_inflictor);

	if (pEnd)
		m_firePosition = pEnd->pev->origin;

	TraceResult tr;
	UTIL_TraceLine(pev->origin, m_firePosition, dont_ignore_monsters, NULL, &tr);
	FireAtPoint(tr, pAttacker);
	pev->nextthink = gpGlobals->time + 0.1;
}




LINK_ENTITY_TO_CLASS( env_glow, CGlow );

TYPEDESCRIPTION	CGlow::m_SaveData[] =
{
	DEFINE_FIELD( CGlow, m_lastTime, FIELD_TIME ),
	DEFINE_FIELD( CGlow, m_maxFrame, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CGlow, CPointEntity );

void CGlow::Spawn(void)
{
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	if (pev->spawnflags & SF_GLOW_START_OFF)// XDM
		pev->effects |= EF_NODRAW;
	else
		pev->effects = 0;

	pev->frame			= 0;
	PRECACHE_MODEL(STRINGV(pev->model));
	SET_MODEL(ENT(pev), STRING(pev->model));
	m_maxFrame = (float)MODEL_FRAMES(pev->modelindex) - 1;
	if (m_maxFrame > 1.0 && pev->framerate != 0)
		pev->nextthink	= gpGlobals->time + 0.1;

	m_lastTime = gpGlobals->time;
}

void CGlow::Think(void)
{
	Animate(pev->framerate * (gpGlobals->time - m_lastTime));// If Think() would get called every frame, gpGlobals->frametime would be enough
	pev->nextthink		= gpGlobals->time + 0.1;
	m_lastTime			= gpGlobals->time;
}

void CGlow::Animate( float frames )
{
	if (m_maxFrame > 0)
		pev->frame = fmod( pev->frame + frames, m_maxFrame );
}

void CGlow::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)// XDM
{
	if (pev->effects & EF_NODRAW)
	{
		pev->effects &= ~EF_NODRAW;
		pev->nextthink = gpGlobals->time;
	}
	else
	{
		pev->effects |= EF_NODRAW;
		pev->nextthink = -1;
	}
}




LINK_ENTITY_TO_CLASS( env_sprite, CSprite );

TYPEDESCRIPTION	CSprite::m_SaveData[] =
{
	DEFINE_FIELD( CSprite, m_lastTime, FIELD_TIME ),
	DEFINE_FIELD( CSprite, m_maxFrame, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CSprite, CPointEntity );

void CSprite::KeyValue(KeyValueData *pkvd)// XDM
{
	if (FStrEq(pkvd->szKeyName, "life"))
	{
		pev->dmgtime = gpGlobals->time + atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

// pev->button == 1 means a dynamically created sprite
void CSprite::Spawn(void)
{
	if (pev->button)
	{
		m_bClientOnly = FALSE;
	}
	else
	{
		const char *s = STRING(pev->model);// XDM3035a: some stupid mappers use this entity to place models! example: dm_industrial
		size_t l = strlen(s);
		if (strnicmp(s+l-4, ".mdl", 4) == 0)
		{
			ALERT(at_console, "ERROR: CSprite %s: %s is not a sprite!\n", STRING(pev->targetname), s);
			// if (pev->targetname)
//works			CBaseEntity::CreateCopy(MAKE_STRING("cycler"), pev, pev->spawnflags);
			CBaseEntity::CreateCopy(MAKE_STRING("env_static"), pev, pev->spawnflags);
			UTIL_Remove(this);
			return;
		}

		if (sv_clientstaticents.value < 1.0f)
		{
			m_bClientOnly = FALSE;
		}
		else if (pev->targetname)
		{
			m_bClientOnly = FALSE;
		}
		else
		{
			m_bClientOnly = TRUE;
		}
	}

	UTIL_FixRenderColor(pev->rendermode, pev->rendercolor);// XDM3035a: IMPORTANT!
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
//	pev->effects		= 0;
	pev->frame			= 0;
	if (m_bClientOnly)
		pev->effects |= EF_NODRAW;

	Precache();
	UTIL_SetModel( ENT(pev), STRING(pev->model) );
	m_maxFrame = (float) MODEL_FRAMES( pev->modelindex ) - 1;
	if ( pev->targetname && !(pev->spawnflags & SF_SPRITE_STARTON) )
		TurnOff();
	else
		TurnOn();

	// Worldcraft only sets y rotation, copy to Z
	if (pev->angles.y != 0 && pev->angles.z == 0)
	{
		pev->angles.z = pev->angles.y;
		pev->angles.y = 0;
	}
}

void CSprite::Precache(void)
{
	UTIL_PrecacheModel(STRINGV(pev->model));

	// Reset attachment after save/restore
	if (pev->aiment)
		SetAttachment(pev->aiment, pev->body);
	else
	{
		// Clear attachment
		pev->skin = 0;
		pev->body = 0;
	}
}

int CSprite::ObjectCaps(void)
{
	int flags = 0;
	if ( pev->spawnflags & SF_SPRITE_TEMPORARY )
		flags = FCAP_DONT_SAVE;

	return (CPointEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | flags;
}

CSprite *CSprite::SpriteCreate(const char *pSpriteName, const Vector &origin, BOOL animate)
{
	CSprite *pSprite = GetClassPtr((CSprite *)NULL, "env_sprite");// XDM
	if (pSprite)
	{
		pSprite->pev->button = 1;// XDM3035a: mark as dynamically created sprite
	//	pSprite->SpriteInit( pSpriteName, origin );
	//	pSprite->pev->classname = MAKE_STRING("env_sprite");
		pSprite->pev->model = MAKE_STRING(pSpriteName);
		pSprite->pev->origin = origin;
		pSprite->pev->solid = SOLID_NOT;
		pSprite->pev->movetype = MOVETYPE_NOCLIP;
		pSprite->Spawn();

		if (animate)
			pSprite->TurnOn();
	}
	return pSprite;
}

void CSprite::AnimateThink(void)
{
	Animate(pev->framerate * (gpGlobals->time - m_lastTime));

	if (pev->impulse > 0)// XDM3035a: TurnOff() may unset nextthink
		pev->nextthink = gpGlobals->time + 0.1;

	m_lastTime = gpGlobals->time;
}

void CSprite::AnimateUntilDead(void)
{
	if (gpGlobals->time > pev->dmgtime)
	{
		if (pev->flags & FL_GODMODE)// XDM3034
		{
			pev->effects |= EF_NODRAW;
			SetThinkNull();
		}
		else
			UTIL_Remove(this);
	}
	else
	{
		AnimateThink();
		pev->nextthink = gpGlobals->time;
	}
}

void CSprite::Expand( float scaleSpeed, float fadeSpeed )
{
	pev->speed = scaleSpeed;
	pev->health = fadeSpeed;
	SetThink(&CSprite::ExpandThink );

	pev->nextthink	= gpGlobals->time;
	m_lastTime		= gpGlobals->time;
}

void CSprite::ExpandThink(void)
{
	if (pev->movetype == MOVETYPE_FOLLOW && FNullEnt(pev->aiment))// XDM
	{
		pev->renderamt = 0;
		if (pev->flags & FL_GODMODE)// XDM3034
		{
			pev->effects |= EF_NODRAW;
			SetThinkNull();
		}
		else
			UTIL_Remove(this);
	}

	float frametime = gpGlobals->time - m_lastTime;
	pev->scale += pev->speed * frametime;
	pev->renderamt -= pev->health * frametime;
	if ( pev->renderamt <= 0 )
	{
		pev->renderamt = 0;
		if (pev->flags & FL_GODMODE)// XDM3034
		{
			pev->effects |= EF_NODRAW;
			SetThinkNull();
		}
		else
			UTIL_Remove( this );
	}
	else
	{
		pev->nextthink		= gpGlobals->time + 0.1;
		m_lastTime			= gpGlobals->time;
	}
}

void CSprite::Animate( float frames )
{
	pev->frame += frames;
	if (pev->frame > m_maxFrame)
	{
		if ( pev->spawnflags & SF_SPRITE_ONCE )
		{
			TurnOff();
		}
		else
		{
			if ( m_maxFrame > 0 )
				pev->frame = fmod( pev->frame, m_maxFrame );
		}
	}
}

void CSprite::SetAttachment( edict_t *pEntity, int attachment )
{
	if ( pEntity )
	{
		pev->skin = ENTINDEX(pEntity);
		pev->body = attachment;
		pev->aiment = pEntity;
		pev->movetype = MOVETYPE_FOLLOW;
	}
}

void CSprite::TurnOff(void)
{
	pev->impulse = 0;
	pev->effects |= EF_NODRAW;
	pev->nextthink = 0;
	SetThinkNull();// XDM3035a: TESTME
}

// XDM3035a: hacked entity
void CSprite::TurnOn(void)
{
	pev->impulse = 1;

	if (m_bClientOnly)// XDM3035a: client static entity
	{
		pev->effects |= EF_NODRAW;// keep invisible, don't send over network!
		return;
	}

	pev->effects &= ~EF_NODRAW;
	if ( (pev->framerate && m_maxFrame > 1.0) || (pev->spawnflags & SF_SPRITE_ONCE) )
	{
		SetThink(&CSprite::AnimateThink);
		pev->nextthink = gpGlobals->time;
		m_lastTime = gpGlobals->time;
	}
	pev->frame = 0;
}

void CSprite::AnimateAndDie(const float &framerate)
{
	SetThink(&CSprite::AnimateUntilDead);
	pev->framerate = framerate;
	pev->dmgtime = gpGlobals->time + (m_maxFrame / framerate);
	pev->nextthink = gpGlobals->time;
}

void CSprite::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	int on = pev->impulse;//!(pev->effects & EF_NODRAW);
	if (ShouldToggle(useType, (on > 0)))
	{
		if (on > 0)
			TurnOff();
		else
			TurnOn();

		SendClientData(NULL, MSG_ALL);
	}
}


// XDM3035a: awesome traffic economy on some maps!
// Called by clients connecting to the game
int CSprite::SendClientData(CBasePlayer *pClient, int msgtype)
{
	if (m_bClientOnly == FALSE)// server entity mode
		return 0;

#ifdef _DEBUG
	ALERT(at_aiconsole, "CSprite: Creating client sprite %s\n", STRING(pev->model));
#endif

	if (msgtype == MSG_ONE)// a client has connected and needs an update
	{
		if (pClient == NULL)
			return 0;

		if (pClient->IsBot())// bots don't need sprites =)
			return 0;
	}
	else if (msgtype == MSG_BROADCAST)
		msgtype = MSG_ALL;// we need this fix in case someone will try to put this update into unreliable message stream

	if (pev->impulse > 0)// sprite is turned off
	{
	MESSAGE_BEGIN(msgtype, gmsgStaticSprite, pev->origin, (pClient == NULL)?NULL : ENT(pClient->pev));
		WRITE_SHORT(entindex());// TODO: use more reliable pev->pContainingEntity->serialnumber
		WRITE_SHORT(pev->modelindex);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_ANGLE(pev->angles.x);
		WRITE_ANGLE(pev->angles.y);
		WRITE_ANGLE(pev->angles.z);
		WRITE_BYTE(pev->rendermode);
		WRITE_BYTE(pev->rendercolor.x);
		WRITE_BYTE(pev->rendercolor.y);
		WRITE_BYTE(pev->rendercolor.z);
		WRITE_BYTE(pev->renderamt);
		WRITE_BYTE(pev->effects);
		WRITE_COORD(pev->scale);
		WRITE_BYTE((int)pev->framerate);
//	if (pev->pContainingEntity->num_leafs > 0)// just send one because it's a point entity
//		WRITE_SHORT(pev->pContainingEntity->leafnums[0]);
//	else
//		WRITE_SHORT(0);
	MESSAGE_END();
	}
	return 1;
}


TYPEDESCRIPTION CGibShooter::m_SaveData[] =
{
	DEFINE_FIELD( CGibShooter, m_iGibs, FIELD_INTEGER ),
	DEFINE_FIELD( CGibShooter, m_iGibCapacity, FIELD_INTEGER ),
	DEFINE_FIELD( CGibShooter, m_iGibMaterial, FIELD_INTEGER ),
	DEFINE_FIELD( CGibShooter, m_iGibModelIndex, FIELD_INTEGER ),
	DEFINE_FIELD( CGibShooter, m_flGibVelocity, FIELD_FLOAT ),
	DEFINE_FIELD( CGibShooter, m_flVariance, FIELD_FLOAT ),
	DEFINE_FIELD( CGibShooter, m_flGibLife, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CGibShooter, CBaseDelay );

LINK_ENTITY_TO_CLASS( gibshooter, CGibShooter );
LINK_ENTITY_TO_CLASS( env_shooter, CGibShooter );// XDM3035a


void CGibShooter::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "m_iGibs"))
	{
		m_iGibs = m_iGibCapacity = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flVelocity"))
	{
		m_flGibVelocity = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flVariance"))
	{
		m_flVariance = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flGibLife"))
	{
		m_flGibLife = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "shootmodel"))// XDM: env_shooter
	{
		pev->model = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "shootsounds"))// XDM: env_shooter
	{
		int iNoise = atoi(pkvd->szValue);
		switch( iNoise )
		{
		case 0:
			m_iGibMaterial = matGlass;
			break;
		case 1:
			m_iGibMaterial = matWood;
			break;
		case 2:
			m_iGibMaterial = matMetal;
			break;
		case 3:
			m_iGibMaterial = matFlesh;
			break;
		case 4:
			m_iGibMaterial = matRocks;
			break;
		default:
//		case -1:
			m_iGibMaterial = matNone;
			break;
		}
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue(pkvd);
}

void CGibShooter::Precache(void)
{
	if (FStringNull(pev->model))
	{
		pev->model = ALLOC_STRING("models/effects/ef_player_gibs.mdl");
		m_iGibMaterial = matMetal;
	}

	m_iGibModelIndex = PRECACHE_MODEL(STRINGV(pev->model));

	MaterialSoundPrecache((Materials)m_iGibMaterial);

	if (pev->scale >= 50)// XDM: HACK! For c1a4* maps
	{
		ALERT(at_aiconsole, "%s %s: scale set too large! %f\n", STRING(pev->classname), STRING(pev->targetname), pev->scale);
		pev->scale = 1.0;
	}
}

void CGibShooter::Spawn(void)
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;

	if (m_flDelay == 0)
		m_flDelay = 0.1;

	if (m_flGibLife == 0)
		m_flGibLife = 25;

	SetMovedir ( pev );
	pev->body = RANDOM_LONG(36,56);
}

void CGibShooter::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink(&CGibShooter::ShootThink);
	pev->nextthink = gpGlobals->time;
}

CGib *CGibShooter :: CreateGib(void)
{
	CGib *pGib = GetClassPtr((CGib *)NULL, "gib");
	if (pGib)
	{
		pGib->SpawnGib(STRING(pev->model));
		int bodyPart = 0;
		if (pev->body > 1)
			bodyPart = RANDOM_LONG(0, pev->body-1);
	//	pGib->pev->body = RANDOM_LONG(0, pev->body - 1);// GTFO // avoid throwing random amounts of the 0th gib. (skull).

		pGib->pev->body = bodyPart;
		pGib->m_material = m_iGibMaterial;

		if (m_iGibMaterial == matFlesh)
			pGib->m_bloodColor = BLOOD_COLOR_RED;
		else
			pGib->m_bloodColor = DONT_BLEED;

		pGib->pev->rendermode = pev->rendermode;
		pGib->pev->renderamt = pev->renderamt;
		pGib->pev->rendercolor = pev->rendercolor;
		pGib->pev->renderfx = pev->renderfx;
	//	if (strstr(STRING(pev->model), ".spr") != NULL) XDM
		pGib->pev->scale = pev->scale;
		pGib->pev->skin = pev->skin;
	}
	return pGib;
}

void CGibShooter::ShootThink(void)
{
	Vector vecShootDir;
	vecShootDir = pev->movedir;
	vecShootDir = vecShootDir + gpGlobals->v_right * RANDOM_FLOAT( -1, 1) * m_flVariance;
	vecShootDir = vecShootDir + gpGlobals->v_forward * RANDOM_FLOAT( -1, 1) * m_flVariance;
	vecShootDir = vecShootDir + gpGlobals->v_up * RANDOM_FLOAT( -1, 1) * m_flVariance;
	vecShootDir = vecShootDir.Normalize();

	if (sv_clientgibs.value > 0.0f)// XDM3035
	{
		int flags = 0;

		const char *s = STRING(pev->model);// XDM3035a: some stupid mappers use this entity to place models! example: dm_industrial
		size_t l = strlen(s);
		if (strnicmp(s+l-4, ".mdl", 4) == 0)
		{
		// TODO: create replacement for this
		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_BREAKMODEL);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD(0);//size.x
			WRITE_COORD(0);//size.y
			WRITE_COORD(0);//size.z
			WRITE_COORD(vecShootDir.x*m_flGibVelocity);//velocity.x
			WRITE_COORD(vecShootDir.y*m_flGibVelocity);//velocity.y
			WRITE_COORD(vecShootDir.z*m_flGibVelocity);//velocity.z
			WRITE_BYTE((int)(m_flGibVelocity*0.1f));//random velocity 0.1
			WRITE_SHORT(m_iGibModelIndex);
			WRITE_BYTE(m_iGibCapacity);//count: Shoot all gibs in this version!
			WRITE_BYTE((int)(m_flGibLife * RANDOM_FLOAT(9.5, 10.5)));//life 0.1

			if (m_iGibMaterial == matGlass)
			{
				flags |= BREAK_GLASS;
				flags |= BREAK_TRANS;// kRenderTransTexture
			}
			else if (m_iGibMaterial == matMetal)
				flags |= BREAK_METAL;
			else if (m_iGibMaterial == matComputer)
				flags |= BREAK_METAL;
			else if (m_iGibMaterial == matFlesh)
				flags |= BREAK_FLESH;
			else if (m_iGibMaterial == matWood)
				flags |= BREAK_WOOD;
			else if (m_iGibMaterial == matCinderBlock)
				flags |= BREAK_CONCRETE;
			else if (m_iGibMaterial == matRocks)
				flags |= BREAK_CONCRETE;

			WRITE_BYTE(flags);//flags
		MESSAGE_END();
		}
		else// HACK: TE_BREAKMODEL does not specify render mode
		{
		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pev->origin);
			WRITE_BYTE(TE_SPRAY);
			WRITE_COORD(pev->origin.x);
			WRITE_COORD(pev->origin.y);
			WRITE_COORD(pev->origin.z);
			WRITE_COORD(vecShootDir.x);//direction.x
			WRITE_COORD(vecShootDir.y);//direction.y
			WRITE_COORD(vecShootDir.z);//direction.z
			WRITE_SHORT(m_iGibModelIndex);
			WRITE_BYTE(m_iGibCapacity);//count
			WRITE_BYTE((int)(m_flGibVelocity*0.1f));//speed
			WRITE_BYTE(80);// noise (client will divide by 100)
			WRITE_BYTE(pev->rendermode);// rendermode
		MESSAGE_END();
		}

		m_iGibs -= m_iGibCapacity;// use all gibs
	}
	else
	{
		CGib *pGib = CreateGib();
		if (pGib)
		{
			pGib->pev->origin = pev->origin;
			pGib->pev->velocity = vecShootDir * m_flGibVelocity;
			pGib->pev->avelocity.x = RANDOM_FLOAT(100, 200);
			pGib->pev->avelocity.y = RANDOM_FLOAT(100, 300);
			float thinkTime = pGib->pev->nextthink - gpGlobals->time;
			pGib->m_lifeTime = (m_flGibLife * RANDOM_FLOAT(0.95, 1.05));	// +/- 5%
			if (pGib->m_lifeTime < thinkTime)
			{
				pGib->pev->nextthink = gpGlobals->time + pGib->m_lifeTime;
				pGib->m_lifeTime = 0;
			}
		}
		--m_iGibs;
	}

	pev->nextthink = gpGlobals->time + m_flDelay;

	if (m_iGibs <= 0)
	{
		if (pev->spawnflags & SF_GIBSHOOTER_REPEATABLE)
		{
			m_iGibs = m_iGibCapacity;
			SetThinkNull();
			pev->nextthink = gpGlobals->time;
		}
		else
		{
			SetThink(&CGibShooter::SUB_Remove);
			pev->nextthink = gpGlobals->time;
		}
	}
}

//this fires rpojectiles

LINK_ENTITY_TO_CLASS( env_proj_shooter, CProjShooter );

void CProjShooter::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "m_iAmount"))
	{
		pev->health = pev->max_health = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "m_iProjType"))
	{
		pev->impulse = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flSpread"))
	{
		pev->frags = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flDmg"))
	{
		pev->dmg = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_flReloatTime"))
	{
		pev->animtime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue(pkvd);
}

void CProjShooter::Spawn(void)
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;

	if (m_flDelay == 0)
		m_flDelay = 0.1;

	SetThink(&CProjShooter::ShootThink);
	pev->nextthink = gpGlobals->time+10.0;
}

void CProjShooter::Precache(void)
{
	if (pev->impulse < PROJ_SHOOTER_30MMGRENADE)
	{
		UTIL_PrecacheOther("meteor");
	}
}

void CProjShooter::ShootThink(void)
{
	Vector vecAng = pev->angles;
	UTIL_MakeVectors(vecAng);
	Vector vecSrc = pev->origin + vecAng;
	
	switch(pev->impulse)
	{
		case PROJ_SHOOTER_METEOR_NORMAL:
			ProjDmg = (pev->dmg > 0)?(pev->dmg):(100);
			CMeteor::Shoot(this, vecSrc, gpGlobals->v_forward * RANDOM_FLOAT(600,1200), pev->frags, ProjDmg, 0, 1);
		break;

		case PROJ_SHOOTER_METEOR_FIRE:
			ProjDmg = (pev->dmg > 0)?(pev->dmg):(100);
			CMeteor::Shoot(this, vecSrc, gpGlobals->v_forward * RANDOM_FLOAT(750,1500), pev->frags, ProjDmg, 1, 1);
		break;

		case PROJ_SHOOTER_METEOR_FROZEN:
			ProjDmg = (pev->dmg > 0)?(pev->dmg):(100);
			CMeteor::Shoot(this, vecSrc, gpGlobals->v_forward * RANDOM_FLOAT(500,1100), pev->frags, ProjDmg, 2, 1);
		break;

		case PROJ_SHOOTER_30MMGRENADE:
			ProjDmg = (pev->dmg > 0)?(pev->dmg):(gSkillData.Dmg30mmGrenade);
			C30mmGrenade::ShootContact(this, vecSrc, pev->frags, ProjDmg, 1);
		break;

		case PROJ_SHOOTER_M203GRENADE:
			ProjDmg = (pev->dmg > 0)?(pev->dmg):(gSkillData.DmgM203Grenade);
			CM203Grenade::ShootM203Grenade(this, vecSrc, pev->frags, ProjDmg, 1);
		break;

		case PROJ_SHOOTER_NEEDLELASER:
			ProjDmg = (pev->dmg > 0)?(pev->dmg):(gSkillData.DmgNeedleLaser);
			CNeedleLaser::ShootNeedleLaser(this, vecSrc, pev->frags, ProjDmg);
		break;

		case PROJ_SHOOTER_SHOCKLASER:
			ProjDmg = (pev->dmg > 0)?(pev->dmg):(gSkillData.DmgShockLaser);
			CShockLaser::ShootShockLaser(this, vecSrc, pev->frags, ProjDmg);
		break;
	}
	pev->health--;
	pev->nextthink = gpGlobals->time + m_flDelay;

	if (pev->health <= 0)
	{
		if (pev->spawnflags & SF_PROJSHOOTER_REPEATABLE)
		{
			pev->health = pev->max_health;
			pev->nextthink = gpGlobals->time + (pev->animtime*RANDOM_FLOAT(0.75,1.5));
		}
		else
		{
			SetThink(&CProjShooter::SUB_Remove);
			pev->nextthink = gpGlobals->time;
		}
	}
}

#ifdef _DEBUG
class CTestEffect : public CBaseDelay
{
public:
	virtual void Spawn(void);
	void EXPORT TestThink(void);
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int m_iLoop;
	int m_iBeam;
	CBeam *m_pBeam[24];
	float m_flBeamTime[24];
	float m_flStartTime;
};

LINK_ENTITY_TO_CLASS( test_effect, CTestEffect );

void CTestEffect::Spawn(void)
{
}

void CTestEffect::TestThink(void)
{
	int i;
	float t = (gpGlobals->time - m_flStartTime);

	if (m_iBeam < 24)
	{
		CBeam *pbeam = CBeam::BeamCreate( "sprites/laserbeam.spr", 100 );

		TraceResult		tr;

		Vector vecSrc = pev->origin;
		Vector vecDir = Vector( RANDOM_FLOAT( -1.0, 1.0 ), RANDOM_FLOAT( -1.0, 1.0 ),RANDOM_FLOAT( -1.0, 1.0 ) );
		vecDir = vecDir.Normalize();
		UTIL_TraceLine( vecSrc, vecSrc + vecDir * 128, ignore_monsters, ENT(pev), &tr);

		pbeam->PointsInit( vecSrc, tr.vecEndPos );
		//pbeam->SetColor( 80, 100, 255 );
		pbeam->SetColor( 255, 180, 100 );
		pbeam->SetWidth( 100 );
		pbeam->SetScrollRate( 12 );
		m_flBeamTime[m_iBeam] = gpGlobals->time;
		m_pBeam[m_iBeam] = pbeam;
		m_iBeam++;

#if 0
		Vector vecMid = (vecSrc + tr.vecEndPos) * 0.5;
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE(TE_DLIGHT);
			WRITE_COORD(vecMid.x);	// X
			WRITE_COORD(vecMid.y);	// Y
			WRITE_COORD(vecMid.z);	// Z
			WRITE_BYTE( 20 );		// radius * 0.1
			WRITE_BYTE( 255 );		// r
			WRITE_BYTE( 180 );		// g
			WRITE_BYTE( 100 );		// b
			WRITE_BYTE( 20 );		// time * 10
			WRITE_BYTE( 0 );		// decay * 0.1
		MESSAGE_END();
#endif
	}

	if (t < 3.0)
	{
		for (i = 0; i < m_iBeam; i++)
		{
			t = (gpGlobals->time - m_flBeamTime[i]) / ( 3 + m_flStartTime - m_flBeamTime[i]);
			m_pBeam[i]->SetBrightness( 255 * t );
			// m_pBeam[i]->SetScrollRate( 20 * t );
		}
		pev->nextthink = gpGlobals->time + 0.1;
	}
	else
	{
		for (i = 0; i < m_iBeam; i++)
		{
			UTIL_Remove( m_pBeam[i] );
		}
		m_flStartTime = gpGlobals->time;
		m_iBeam = 0;
		// pev->nextthink = gpGlobals->time;
		SetThinkNull();
	}
}

void CTestEffect::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink(&CTestEffect::TestThink);
	pev->nextthink = gpGlobals->time + 0.1;
	m_flStartTime = gpGlobals->time;
}
#endif _DEBUG


// Screen shake
LINK_ENTITY_TO_CLASS( env_shake, CEnvShake );

// pev->scale is amplitude
// pev->dmg_save is frequency
// pev->dmg_take is duration
// pev->dmg is radius
// radius of 0 means all players
// NOTE: UTIL_ScreenShake() will only shake players who are on the ground
void CEnvShake::Spawn(void)
{
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	pev->effects		= 0;
	pev->frame			= 0;

	if ( pev->spawnflags & SF_SHAKE_EVERYONE )
		pev->dmg = 0;
}

void CEnvShake::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "amplitude"))
	{
		SetAmplitude( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "frequency"))
	{
		SetFrequency( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "duration"))
	{
		SetDuration( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "radius"))
	{
		SetRadius( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CEnvShake::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	UTIL_ScreenShake( pev->origin, Amplitude(), Frequency(), Duration(), Radius() );
}





LINK_ENTITY_TO_CLASS( env_fade, CEnvFade );

// pev->dmg_take is duration
// pev->dmg_save is hold duration
void CEnvFade::Spawn(void)
{
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;
	pev->effects		= 0;
	pev->frame			= 0;
}

void CEnvFade::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "duration"))
	{
		SetDuration( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		SetHoldTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "radius"))// XDM3035a
	{
		pev->armorvalue = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CEnvFade::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	int fadeFlags = 0;

	if ( !(pev->spawnflags & SF_FADE_IN) )
		fadeFlags |= FFADE_OUT;

	if ( pev->spawnflags & SF_FADE_MODULATE )
		fadeFlags |= FFADE_MODULATE;

	if ( pev->spawnflags & SF_FADE_STAYOUT )// XDM
		fadeFlags |= FFADE_STAYOUT;

	if ( pev->spawnflags & SF_FADE_ONLYONE )
	{
		if ( pActivator->IsNetClient() )
			UTIL_ScreenFade( pActivator, pev->rendercolor, Duration(), HoldTime(), pev->renderamt, fadeFlags );
	}
	else if (!(pev->spawnflags & (SF_FADE_DIRECTVISIBLE | SF_FADE_FACING)))
	{
		if (!g_pGameRules->IsMultiplayer())// XDM3036: spam
			UTIL_ScreenFadeAll( pev->rendercolor, Duration(), HoldTime(), pev->renderamt, fadeFlags );
	}
	else
	{
		// search through all clients
		for (int i = 1; i <= gpGlobals->maxClients; ++i)
		{
			CBasePlayer *pPlayer = UTIL_ClientByIndex(i);
			if (pPlayer)
			{
				if ((pev->spawnflags & SF_FADE_DIRECTVISIBLE) && !FVisible(pPlayer->EyePosition()))// XDM3035c
					continue;

				if ((pev->spawnflags & SF_FADE_FACING) && !IsFacing(pPlayer->EyePosition(), pPlayer->pev->v_angle, pev->origin))// XDM3035c
					continue;

				UTIL_ScreenFade(pPlayer, pev->rendercolor, Duration(), HoldTime(), pev->renderamt, fadeFlags);
			}
		}
	}
	SUB_UseTargets( this, USE_TOGGLE, 0 );
}





LINK_ENTITY_TO_CLASS( env_message, CEnvMessage );

void CEnvMessage::Spawn(void)
{
	Precache();
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_NONE;

	switch( pev->impulse )
	{
	case 1: // Medium radius
		pev->speed = ATTN_STATIC;
		break;

	case 2:	// Large radius
		pev->speed = ATTN_NORM;
		break;

	case 3:	//EVERYWHERE
		pev->speed = ATTN_NONE;
		break;

	default:
	case 0: // Small radius
		pev->speed = ATTN_IDLE;
		break;
	}
	pev->impulse = 0;

	// No volume, use normal
	if ( pev->scale <= 0 )
		pev->scale = 1.0;
}

void CEnvMessage::Precache(void)
{
	if (pev->noise)
		PRECACHE_SOUND(STRINGV(pev->noise));
}

void CEnvMessage::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "messagesound"))
	{
		pev->noise = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "messagevolume"))
	{
		pev->scale = atof(pkvd->szValue) * 0.1;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "messageattenuation"))
	{
		pev->impulse = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CEnvMessage::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pPlayer = NULL;

	if ( pev->spawnflags & SF_MESSAGE_ALL )
		UTIL_ShowMessageAll( STRING(pev->message) );
	else
	{
		if ( pActivator && pActivator->IsPlayer() )
			pPlayer = pActivator;
		else
			pPlayer = UTIL_ClientByIndex(1);//CBaseEntity::Instance( INDEXENT( 1 ) );

		if ( pPlayer )
			UTIL_ShowMessage( STRING(pev->message), pPlayer );
	}
	if ( pev->noise )
		EMIT_SOUND( edict(), CHAN_BODY, STRING(pev->noise), pev->scale, pev->speed );

	if ( pev->spawnflags & SF_MESSAGE_ONCE )
		UTIL_Remove( this );

	SUB_UseTargets( this, USE_TOGGLE, 0 );
}




//=========================================================
// FunnelEffect
//=========================================================
LINK_ENTITY_TO_CLASS( env_funnel, CEnvFunnel );

void CEnvFunnel::Precache(void)
{
	if (pev->noise)// XDM: custom model
		m_iSprite = PRECACHE_MODEL(STRINGV(pev->netname));
	else
		m_iSprite = PRECACHE_MODEL("sprites/flare6.spr");
}

void CEnvFunnel::Spawn(void)
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
}

void CEnvFunnel::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_LARGEFUNNEL);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT(m_iSprite);
	if (pev->spawnflags & SF_FUNNEL_REVERSE)// funnel flows in reverse?
		WRITE_SHORT(1);
	else
		WRITE_SHORT(0);

	MESSAGE_END();

	if (!(pev->spawnflags & SF_FUNNEL_REPEATABLE))
	{
		SetThink(&CEnvFunnel::SUB_Remove);
		pev->nextthink = gpGlobals->time;
	}
}






//----------------------------------------------------------------
// Spark
//----------------------------------------------------------------
void DoSpark(entvars_t *pev, const Vector &location)
{
	Vector tmp = location + pev->size * 0.5f;
	UTIL_Sparks( tmp );
	EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gSoundsSparks[RANDOM_LONG(0,NUM_SPARK_SOUNDS-1)], RANDOM_FLOAT(0.05, 0.5), ATTN_STATIC, 0, RANDOM_LONG(95,105));
}

TYPEDESCRIPTION CEnvSpark::m_SaveData[] =
{
	DEFINE_FIELD( CEnvSpark, m_flDelay, FIELD_FLOAT),
};

IMPLEMENT_SAVERESTORE( CEnvSpark, CBaseEntity );

LINK_ENTITY_TO_CLASS(env_spark, CEnvSpark);
LINK_ENTITY_TO_CLASS(env_debris, CEnvSpark);

void CEnvSpark::Spawn(void)
{
	SetThinkNull();
	SetUseNull();

	if (FBitSet(pev->spawnflags, 32)) // Use for on/off
	{
		if (FBitSet(pev->spawnflags, 64)) // Start on
		{
			SetThink(&CEnvSpark::SparkThink);	// start sparking
			SetUse(&CEnvSpark::SparkStop);		// set up +USE to stop sparking
		}
		else
			SetUse(&CEnvSpark::SparkStart);
	}
	else
		SetThink(&CEnvSpark::SparkThink);

	pev->nextthink = gpGlobals->time + ( 0.1 + RANDOM_FLOAT ( 0, 1.5 ) );

	if (m_flDelay <= 0)
		m_flDelay = 1.5;

	Precache();
}

void CEnvSpark::Precache(void)
{
}

void CEnvSpark::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "MaxDelay"))
	{
		m_flDelay = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (	FStrEq(pkvd->szKeyName, "style") ||
				FStrEq(pkvd->szKeyName, "height") ||
				FStrEq(pkvd->szKeyName, "killtarget") ||
				FStrEq(pkvd->szKeyName, "value1") ||
				FStrEq(pkvd->szKeyName, "value2") ||
				FStrEq(pkvd->szKeyName, "value3"))
		pkvd->fHandled = TRUE;
	else
		CBaseEntity::KeyValue( pkvd );
}

void CEnvSpark::SparkThink(void)
{
	pev->nextthink = gpGlobals->time + 0.1 + RANDOM_FLOAT (0, m_flDelay);
	DoSpark( pev, pev->origin );
}

void CEnvSpark::SparkStart(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetUse(&CEnvSpark::SparkStop);
	SetThink(&CEnvSpark::SparkThink);
	pev->nextthink = gpGlobals->time + (0.1 + RANDOM_FLOAT ( 0, m_flDelay));
}

void CEnvSpark::SparkStop(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetUse(&CEnvSpark::SparkStart);
	SetThinkNull();
}
