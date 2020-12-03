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
//
// ========================== PATH_CORNER ===========================
//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "trains.h"
#include "saverestore.h"




LINK_ENTITY_TO_CLASS( path_corner, CPathCorner );

// Global Savedata for Delay
TYPEDESCRIPTION	CPathCorner::m_SaveData[] = 
{
	DEFINE_FIELD( CPathCorner, m_flWait, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CPathCorner, CPointEntity );

//
// Cache user-entity-field values until spawn is called.
//
void CPathCorner::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "wait"))
	{
		m_flWait = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "turnspeed")) // SHL
	{
		if (pkvd->szValue[0]) // if the field is blank, don't set the spawnflag.
		{
			pev->spawnflags |= SF_CORNER_AVELOCITY;
//			UTIL_StringToVector( (float*)pev->avelocity, pkvd->szValue);
			if (StringToVec(pkvd->szValue, pev->avelocity) == FALSE)
				ALERT(at_console, "Error: %s has bad value %s == \"%s\"!\n", pkvd->szClassName, pkvd->szKeyName, pkvd->szValue);
		}
		pkvd->fHandled = TRUE;
	}
	else 
		CPointEntity::KeyValue(pkvd);
}

void CPathCorner::Spawn(void)
{
//	ASSERTSZ(!FStringNull(pev->targetname), "path_corner without a targetname");
	if (FStringNull(pev->targetname))
	{
		ALERT(at_console, "Error: %s without a targetname!\n", STRING(pev->classname));
		UTIL_Remove(this);
	}
}




TYPEDESCRIPTION	CPathTrack::m_SaveData[] = 
{
	DEFINE_FIELD( CPathTrack, m_length, FIELD_FLOAT ),
	DEFINE_FIELD( CPathTrack, m_pnext, FIELD_CLASSPTR ),
	DEFINE_FIELD( CPathTrack, m_paltpath, FIELD_CLASSPTR ),
	DEFINE_FIELD( CPathTrack, m_pprevious, FIELD_CLASSPTR ),
	DEFINE_FIELD( CPathTrack, m_altName, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE( CPathTrack, CBaseEntity );

LINK_ENTITY_TO_CLASS( path_track, CPathTrack );

//
// Cache user-entity-field values until spawn is called.
//
void CPathTrack::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "altpath"))
	{
		m_altName = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "turnspeed"))// SHL
	{
		if (pkvd->szValue[0]) // if the field is blank, don't set the spawnflag.
		{
			pev->spawnflags |= SF_PATH_AVELOCITY;
//			UTIL_StringToVector( (float*)pev->avelocity, pkvd->szValue);
			if (StringToVec(pkvd->szValue, pev->avelocity) == FALSE)
				ALERT(at_console, "Error: %s has bad value %s == \"%s\"!\n", pkvd->szClassName, pkvd->szKeyName, pkvd->szValue);
		}
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

void CPathTrack::Spawn(void)
{
	pev->solid = SOLID_TRIGGER;
	UTIL_SetSize(pev, Vector(-8, -8, -8), Vector(8, 8, 8));

	m_pnext = NULL;
	m_pprevious = NULL;
// DEBUGGING CODE
#if PATH_SPARKLE_DEBUG
	SetThink(&CPathTrack::Sparkle);
	pev->nextthink = gpGlobals->time + 0.5;
#endif
}

void CPathTrack::Activate(void)
{
	if (!FStringNull(pev->targetname))		// Link to next, and back-link
		Link();
}

void CPathTrack::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	bool on;

	// Use toggles between two paths
	if ( m_paltpath )
	{
		on = !FBitSet( pev->spawnflags, SF_PATH_ALTERNATE );
		if ( ShouldToggle( useType, on ) )
		{
			if ( on )
				SetBits( pev->spawnflags, SF_PATH_ALTERNATE );
			else
				ClearBits( pev->spawnflags, SF_PATH_ALTERNATE );
		}
	}
	else	// Use toggles between enabled/disabled
	{
		on = !FBitSet( pev->spawnflags, SF_PATH_DISABLED );

		if ( ShouldToggle( useType, on ) )
		{
			if ( on )
				SetBits( pev->spawnflags, SF_PATH_DISABLED );
			else
				ClearBits( pev->spawnflags, SF_PATH_DISABLED );
		}
	}
}

// XDM: target edict_t replaced with CBaseEntity
void CPathTrack::Link(void)
{
	CBaseEntity *pTarget;

	if (!FStringNull(pev->target))
	{
		pTarget = UTIL_FindEntityByTargetname(NULL, STRING(pev->target));

		if (pTarget == this)
		{
			ALERT(at_error, "%s (%s) refers to itself as a target!\n", STRING(pev->classname), STRING(pev->targetname));
		}
		else if (pTarget)
		{
			m_pnext = (CPathTrack*)pTarget;
			if (m_pnext)		// If no next pointer, this is the end of a path
				m_pnext->SetPrevious( this );
		}
		else
			ALERT( at_console, "Dead end link %s\n", STRING(pev->target) );
	}

	// Find "alternate" path
	if ( m_altName )
	{
		pTarget = UTIL_FindEntityByTargetname( NULL, STRING(m_altName) );
		if (pTarget)		// If no next pointer, this is the end of a path
		{
			m_paltpath = (CPathTrack*)pTarget;
			if (m_paltpath)	// If no next pointer, this is the end of a path
				m_paltpath->SetPrevious( this );
		}
	}
}

void CPathTrack::Project(CPathTrack *pStart, CPathTrack *pEnd, Vector *origin, float dist)
{
	if (pStart && pEnd)
	{
		Vector dir = (pEnd->pev->origin - pStart->pev->origin);// TODO: GetLocalOrigin()
		dir = dir.Normalize();
		*origin = pEnd->pev->origin + dir * dist;// TODO: GetLocalOrigin()
	}
}

CPathTrack *CPathTrack::ValidPath(CPathTrack *pPath, int testFlag)
{
	if (!pPath)
		return NULL;

	if (testFlag && FBitSet(pPath->pev->spawnflags, SF_PATH_DISABLED))
		return NULL;

	return pPath;
}

CPathTrack *CPathTrack::GetNext(void)
{
	if (m_paltpath && FBitSet(pev->spawnflags, SF_PATH_ALTERNATE) && !FBitSet(pev->spawnflags, SF_PATH_ALTREVERSE))
		return m_paltpath;
	
	return m_pnext;
}

CPathTrack *CPathTrack::GetPrevious(void)
{
	if (m_paltpath && FBitSet(pev->spawnflags, SF_PATH_ALTERNATE) && FBitSet(pev->spawnflags, SF_PATH_ALTREVERSE))
		return m_paltpath;
	
	return m_pprevious;
}

void CPathTrack::SetPrevious(CPathTrack *pprev)
{
	// Only set previous if this isn't my alternate path
	if ( pprev && !FStrEq( STRING(pprev->pev->targetname), STRING(m_altName) ) )
		m_pprevious = pprev;
}

// Assumes this is ALWAYS enabled
CPathTrack *CPathTrack::LookAhead( Vector *origin, float dist, int move )
{
	CPathTrack *pcurrent = this;
	float originalDist = dist;
	Vector currentPos = *origin;

	if (dist < 0)// Travelling backwards through path
	{
		dist = -dist;
		while ( dist > 0 )
		{
			Vector dir = pcurrent->pev->origin - currentPos;// TODO: GetLocalOrigin()
			float length = dir.Length();
			if ( !length )
			{
				if ( !ValidPath(pcurrent->GetPrevious(), move) ) 	// If there is no previous node, or it's disabled, return now.
				{
					if ( !move )
						Project( pcurrent->GetNext(), pcurrent, origin, dist );
					return NULL;
				}
				pcurrent = pcurrent->GetPrevious();
			}
			else if ( length > dist )	// enough left in this path to move
			{
				*origin = currentPos + (dir * (dist / length));
				return pcurrent;
			}
			else
			{
				dist -= length;
				currentPos = pcurrent->pev->origin;// TODO: GetLocalOrigin()
				*origin = currentPos;
				if ( !ValidPath(pcurrent->GetPrevious(), move) )	// If there is no previous node, or it's disabled, return now.
					return NULL;

				pcurrent = pcurrent->GetPrevious();
			}
		}
		*origin = currentPos;
		return pcurrent;
	}
	else 
	{
		while ( dist > 0 )
		{
			if ( !ValidPath(pcurrent->GetNext(), move) )	// If there is no next node, or it's disabled, return now.
			{
				if ( !move )
					Project( pcurrent->GetPrevious(), pcurrent, origin, dist );
				return NULL;
			}
			Vector dir = pcurrent->GetNext()->pev->origin - currentPos;// TODO: GetLocalOrigin()
			float length = dir.Length();
			if ( !length  && !ValidPath( pcurrent->GetNext()->GetNext(), move ) )
			{
				if ( dist == originalDist ) // HACK -- up against a dead end
					return NULL;
				return pcurrent;
			}
			if ( length > dist )	// enough left in this path to move
			{
				*origin = currentPos + (dir * (dist / length));
				return pcurrent;
			}
			else
			{
				dist -= length;
				currentPos = pcurrent->GetNext()->pev->origin;// TODO: GetLocalOrigin()
				pcurrent = pcurrent->GetNext();
				*origin = currentPos;
			}
		}
		*origin = currentPos;
	}

	return pcurrent;
}

// Assumes this is ALWAYS enabled
CPathTrack *CPathTrack::Nearest(const Vector &origin)
{
	int			deadCount;
	float		minDist, dist;
	Vector		delta;
	CPathTrack	*ppath, *pnearest;

	delta = origin - pev->origin;// TODO: GetLocalOrigin()
	delta.z = 0;
	minDist = delta.Length();
	pnearest = this;
	ppath = GetNext();

	// Hey, I could use the old 2 racing pointers solution to this, but I'm lazy :)
	deadCount = 0;
	while (ppath && ppath != this)
	{
		deadCount++;
		if (deadCount > 9999)
		{
			ALERT( at_error, "Bad sequence of path_tracks from %s", STRING(pev->targetname) );
			return NULL;
		}
		delta = origin - ppath->pev->origin;// TODO: GetLocalOrigin()
		delta.z = 0;
		dist = delta.Length();
		if (dist < minDist)
		{
			minDist = dist;
			pnearest = ppath;
		}
		ppath = ppath->GetNext();
	}
	return pnearest;
}


CPathTrack *CPathTrack::GetNextInDir(bool bForward)
{
	if (bForward)
		return GetNext();

	return GetPrevious();
}

Vector CPathTrack::GetOrientation(bool bForwardDir)
{
	CPathTrack *pPrev = this;
	CPathTrack *pNext = GetNextInDir(bForwardDir);

	if (!pNext)
	{	
		pPrev = GetNextInDir(!bForwardDir);
		pNext = this;
	}

	Vector vecDir = pNext->pev->origin - pPrev->pev->origin;// TODO: pNext->GetLocalOrigin() - pPrev->GetLocalOrigin();
	Vector angDir = UTIL_VecToAngles(vecDir);
	// The train actually points west
	angDir.y += 180;
	return angDir;
}

CPathTrack *CPathTrack::Instance(edict_t *pent)
{ 
	if (FClassnameIs(pent, "path_track"))
		return (CPathTrack *)GET_PRIVATE(pent);

	return NULL;
}

	// DEBUGGING CODE
#if PATH_SPARKLE_DEBUG
void CPathTrack::Sparkle(void)
{

	pev->nextthink = gpGlobals->time + 0.2;
	if ( FBitSet( pev->spawnflags, SF_PATH_DISABLED ) )
		UTIL_ParticleEffect(pev->origin, Vector(0,0,100), 210, 10);
	else
		UTIL_ParticleEffect(pev->origin, Vector(0,0,100), 84, 10);
}
#endif
