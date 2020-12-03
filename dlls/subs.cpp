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

===== subs.cpp ========================================================

  frequently used global functions

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "doors.h"
#include "skill.h"
#include "gamerules.h"
#include "game.h"
#include "weapons.h"
#include "nodes.h"
#include "util_vector.h"
#include "pm_shared.h"
#include "msg_fx.h"

//-----------------------------------------------------------------------------
// Purpose: This updates global tables that need to know about entities being removed
//-----------------------------------------------------------------------------
void CBaseEntity::UpdateOnRemove(void)
{
	if (FBitSet(pev->flags, FL_GRAPHED))
	{
	// this entity was a LinkEnt in the world node graph, so we must remove it from
	// the graph since we are removing it from the world.
		for (int i = 0 ; i < WorldGraph.m_cLinks ; ++i)
		{
			if (WorldGraph.m_pLinkPool[i].m_pLinkEnt == pev)
			{
				// if this link has a link ent which is the same ent that is removing itself, remove it!
				WorldGraph.m_pLinkPool[i].m_pLinkEnt = NULL;
			}
		}
	}
	if (pev->globalname)
		gGlobalState.EntitySetState(pev->globalname, GLOBAL_DEAD);

#if !defined(NEW_DLL_FUNCTIONS)
	// XDM3035c: since nobody will call this externally in older engine versions
	OnFreePrivateData();
#endif
}

//-----------------------------------------------------------------------------
// Convenient way to delay removing oneself
//-----------------------------------------------------------------------------
void CBaseEntity::SUB_Remove(void)
{
	if (pev->health > 0)
	{
		// this situation can screw up monsters who can't tell their entity pointers are invalid.
		pev->health = 0;
/*#ifdef _DEBUG
		ALERT(at_aiconsole, "SUB_Remove called on entity %s (%s) with health > 0\n", STRING(pev->targetname), STRING(pev->classname));
#endif*/
	}

// fail =(	UTIL_Remove(this);
	if (ShouldRespawn())// XDM3035
	{
		SetThink(&CBaseEntity::SUB_Respawn);
		pev->nextthink = gpGlobals->time + mp_monsrespawntime.value;// XDM: TODO
	}
	else
	{
		UpdateOnRemove();
//		pev->flags |= FL_KILLME;
//		pev->targetname = 0;
		REMOVE_ENTITY(ENT(pev));
	}
}

//-----------------------------------------------------------------------------
// Convenient way to explicitly do nothing (passed to functions that require a method)
//-----------------------------------------------------------------------------
void CBaseEntity::SUB_DoNothing(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: XDM: obsolete: set think to parent 'move with' entity
//-----------------------------------------------------------------------------
void CBaseEntity::SUB_MoveWith(void)
{
/*	pev->origin = m_vecMoveOriginDelta + m_pMoveWith->pev->origin;
	pev->velocity = m_pMoveWith->pev->velocity;
	pev->nextthink = m_pMoveWith->pev->nextthink;*/
}

//-----------------------------------------------------------------------------
// Purpose: slowly fades a entity out, then removes it.
//
// DON'T USE ME FOR GIBS AND STUFF IN MULTIPLAYER!
// SET A FUTURE THINK AND A RENDERMODE!!
//-----------------------------------------------------------------------------
void CBaseEntity::SUB_StartFadeOut(void)
{
	if (pev->rendermode == kRenderNormal)
	{
		pev->renderamt = 255;
		pev->rendermode = kRenderTransTexture;
	}

	pev->solid = SOLID_NOT;
	pev->avelocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.1;
	SetThink(&CBaseEntity::SUB_FadeOut);
}

//-----------------------------------------------------------------------------
// Purpose: fade process
//-----------------------------------------------------------------------------
void CBaseEntity::SUB_FadeOut(void)
{
	if (pev->renderamt > 8)
	{
		pev->renderamt -= 8;
		pev->nextthink = gpGlobals->time + 0.1f;
	}
	else
	{
		pev->renderamt = 0.0f;
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time + 0.2f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Tried to make HL2-style disintegration effect
// TODO: move to client (remove real entity and create client-side copy)
// XDM3035
//-----------------------------------------------------------------------------
void CBaseEntity::Disintegrate(void)
{
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_NOCLIP;
	pev->solid = SOLID_NOT;
	pev->flags |= FL_FLY;
	pev->gravity = 0.0f;
	pev->velocity = Vector(0.0f,0.0f,4.0f);
	pev->origin.z += 0.5f;// HACK
	pev->angles.x += 2.0f;
	pev->angles.y += RANDOM_FLOAT(-2,2);
	pev->avelocity = UTIL_RandomVector()*6.0f;//Vector(RANDOM_FLOAT(-2,2)
//	pev->deadflag = DEAD_DYING;// XDM3035
	pev->framerate *= 0.25f;// slow down movements
	pev->health = 0;

	if (g_pGameRules->FAllowEffects())// Disintegration effect allowed?
	{
		pev->effects |= EF_MUZZLEFLASH;
		pev->rendermode = kRenderTransTexture;
		pev->renderfx = kRenderFxDisintegrate;
		pev->rendercolor = Vector(127,127,127);
		pev->renderamt = 160;
		SetThink(&CBaseEntity::SUB_Disintegrate);
		ParticleBurst(Center(), min(80, max(4, fabs((pev->maxs - pev->mins).Length()*0.5f))), 5, 10);
	}
	else
	{
//		pev->health = 0;
		UTIL_Remove(this);
	}
	pev->nextthink = gpGlobals->time;
}

//-----------------------------------------------------------------------------
// Purpose: XDM: don't use this directly! Call Disintegrate()!
//-----------------------------------------------------------------------------
void CBaseEntity::SUB_Disintegrate(void)
{
	if (pev->renderamt >= 8)
	{
		pev->origin.z += 0.2f;// 0.1f if renderamt -= 4
		pev->renderamt -= 8;
		pev->nextthink = gpGlobals->time + 0.1f;
		if (g_pGameRules->FAllowEffects())
		{
			pev->scale += 0.02f;
			if (RANDOM_LONG(0,3) == 0)
			{
				pev->effects |= EF_MUZZLEFLASH;
				UTIL_Sparks(pev->origin);
			}
		}
	}
	else
	{
		pev->angles.x = 0.0f;// don't really need this?
		pev->angles.y = 0.0f;
		pev->movetype = MOVETYPE_NONE;
		pev->effects |= EF_NODRAW;
		pev->flags &= ~FL_FLY;
		pev->scale = 1.0f;
		pev->renderamt = 0;
//		pev->rendermode = kRenderNormal;
		pev->rendercolor = g_vecZero;
//		pev->avelocity = g_vecZero;
		pev->framerate *= 4.0f;// restore
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time + 0.2f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: remove projectile with attached EFFECT
//-----------------------------------------------------------------------------
void CBaseEntity::AnnihilateProj(void)
{
	FX_Trail( pev->origin, entindex(), FX_REMOVE );
	UTIL_Remove(this);
	pev->nextthink = gpGlobals->time;
}

//-----------------------------------------------------------------------------
// Purpose: Delay call to Use() using pev->nextthink
//-----------------------------------------------------------------------------
void CBaseEntity::SUB_CallUseToggle(void)
{
	this->Use(this, this, USE_TOGGLE, 0);
}

//-----------------------------------------------------------------------------
// Purpose: Respawn. Used by monsters in multiplayer.
//-----------------------------------------------------------------------------
void CBaseEntity::SUB_Respawn(void)
{
	pev->effects |= EF_NODRAW;
	pev->effects |= EF_NOINTERP;
	SetThinkNull();
	pev->nextthink = 0;
	pev->avelocity = g_vecZero;
//	pev->angles.x = 0.0f;// prevent mosters from respawning upside down
//	pev->angles.y = 0.0f;
	pev->angles = g_vecZero;
	pev->punchangle = g_vecZero;// XDM3035b
	pev->idealpitch = 0.0f;
	pev->ideal_yaw = 0.0f;
//	pev->yaw_speed = 0.0f;
//	if (g_pGameRules->IsMultiplayer())// XDM3035
	{
//		pev->startpos = pev->origin;
		pev->origin = m_vecSpawnSpot;
		UTIL_SetOrigin(pev, pev->origin);
	}

	if (g_pGameRules->IsMultiplayer())// HACK
	{
		pev->rendermode = kRenderNormal;
		pev->renderamt = 255;
		pev->renderfx = kRenderFxNone;
	}
	pev->effects &= ~EF_NODRAW;
// we need to revisit the purpose of that function	Respawn();
//	GET_SPAWN_PARMS(ENT(pev));
	Spawn(FALSE);
	if (!(pev->flags & FL_KILLME))
	{
		pev->effects |= EF_MUZZLEFLASH;
		if (g_pGameRules->FAllowEffects())// do this AFTER restoring entity origin
		{
			ParticleBurst(pev->origin, 32, 128, 10);
			BeamEffect(TE_BEAMCYLINDER, pev->origin, pev->origin + Vector(0,0,56), g_iModelIndexBeamsAll, BLAST_SKIN_SHOCKWAVE,0, 10, 64,32, Vector(95,95,255), 255, 2);// small shockwave
		}
	}
//	DispatchSpawn(edict());
}

//-----------------------------------------------------------------------------
// Purpose: Make me respawn in 10 seconds
// Output : CBaseEntity
//-----------------------------------------------------------------------------
CBaseEntity *CBaseEntity::Respawn(void)
{
	ALERT(at_aiconsole, "CBaseEntity(%d)::Respawn() %s\n", entindex(), STRING(pev->classname));
	SetThink(&CBaseEntity::SUB_Respawn);
	pev->nextthink = gpGlobals->time + 10.0f;
	return this;
}

//-----------------------------------------------------------------------------
// Purpose: Simple, non-delayed version
// Input  : *pActivator - 
//			useType - 
//			value - 
//-----------------------------------------------------------------------------
void CBaseEntity::SUB_UseTargets( CBaseEntity *pActivator, USE_TYPE useType, float value )
{
	if (!FStringNull(pev->target))
		FireTargets( STRING(pev->target), pActivator, this, useType, value );
}







// Landmark class
void CPointEntity::Spawn(void)
{
	CBaseEntity::Spawn();//Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->frame = 0;
	pev->model = 0;
	pev->modelindex = 0;
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
}







LINK_ENTITY_TO_CLASS( DelayedUse, CBaseDelay );

// Global Savedata for Delay
TYPEDESCRIPTION	CBaseDelay::m_SaveData[] =
{
	DEFINE_FIELD( CBaseDelay, m_flDelay, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseDelay, m_iszKillTarget, FIELD_STRING ),
	DEFINE_FIELD( CBaseDelay, m_iszMaster, FIELD_STRING ),
	DEFINE_FIELD( CBaseDelay, m_hActivator, FIELD_EHANDLE ),// XDM3035
	DEFINE_FIELD( CBaseDelay, m_iState, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CBaseDelay, CBaseEntity );

// XDM3035c: the only way to pre-set variables before KeyValue() takes place
CBaseDelay::CBaseDelay() : CBaseEntity()
{
	m_iState = STATE_ON;// well, in HL entities are mostly ON unless _START_OFF flag is specified for them
}

/*CBaseDelay::~CBaseDelay()
{
}*/

void CBaseDelay::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "delay"))
	{
		m_flDelay = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "killtarget"))
	{
		m_iszKillTarget = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "master"))
	{
		m_iszMaster = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "state"))// XDM3035c
	{
		m_iState = (STATE)atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CBaseDelay::Spawn(void)
{
	CBaseEntity::Spawn();//Precache();
// just thoughts	if (m_iState != STATE_OFF) Activate();
}

/*
==============================
SUB_UseTargets

If self.delay is set, a DelayedUse entity will be created that will actually
do the SUB_UseTargets after that many seconds have passed.

Removes all entities with a targetname that match self.killtarget,
and removes them, so some events can remove other triggers.

Search for (string)targetname in all entities that
match (string)self.target and call their .use function (if they have one)

==============================
*/
void CBaseDelay::SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value)
{
	// exit immediatly if we don't have a target or kill target
	if (FStringNull(pev->target) && FStringNull(m_iszKillTarget))
		return;

	// check for a delay
	if (m_flDelay > 0.0f)
	{
		// create a temp object to fire at a later time
		CBaseDelay *pTemp = GetClassPtr((CBaseDelay *)NULL, "DelayedUse");// XDM
		pTemp->pev->nextthink = gpGlobals->time + m_flDelay;
		pTemp->SetThink(&CBaseDelay::DelayThink);
		// Save the useType
		pTemp->pev->button = (int)useType;
		pTemp->pev->armorvalue = value;// XDM3035c
		pTemp->m_iszKillTarget = m_iszKillTarget;
		pTemp->m_flDelay = 0; // prevent "recursion"
		pTemp->pev->target = pev->target;

		// HACKHACK
		// This wasn't in the release build of Half-Life.  We should have moved m_hActivator into this class
		// but changing member variable hierarchy would break save/restore without some ugly code.
		// This code is not as ugly as that code
		if (pActivator)// XDM
		{
			pTemp->m_hActivator = pActivator;
			pTemp->pev->owner = pActivator->edict();//old
		}
		else
		{
			pTemp->m_hActivator = NULL;
			pTemp->pev->owner = NULL;//old
		}
		pTemp->m_pGoalEnt = this;// XDM3035c: 20121121 HACK to remember the REAL caller (this). WARNING! This may actually be removed long before m_flDelay in which case m_pGoalEnt will become invalid!
		return;
	}

	//
	// kill the killtargets
	//
	if (m_iszKillTarget)
	{
		edict_t *pentKillTarget = NULL;
		ALERT(at_aiconsole, "%s %s: KillTarget: %s\n", STRING(pev->classname), STRING(pev->targetname), STRING(m_iszKillTarget));
		pentKillTarget = FIND_ENTITY_BY_TARGETNAME(NULL, STRING(m_iszKillTarget));
		while (!FNullEnt(pentKillTarget))
		{
			UTIL_Remove(CBaseEntity::Instance(pentKillTarget));
			ALERT(at_aiconsole, " killing %s\n", STRING(pentKillTarget->v.classname));
			pentKillTarget = FIND_ENTITY_BY_TARGETNAME(pentKillTarget, STRING(m_iszKillTarget));
		}
	}

	//
	// fire targets
	//
	CBaseEntity::SUB_UseTargets(pActivator, useType, value);// XDM3037: OOP design
}

void CBaseDelay::SetState(STATE newstate)
{
	if (m_iState != newstate)//&& CanChangeState(newstate))
	{
		STATE oldstate = m_iState;
		m_iState = newstate;
		this->OnStateChange(oldstate);
	}
}

void CBaseDelay::OnStateChange(STATE oldstate)
{
#ifdef _DEBUG
	if (pev)// may get called from the constructor!!!
		ALERT(at_aiconsole, "%s %s changed state from %s to %s\n", STRING(pev->classname), STRING(pev->targetname), GetStringForState(oldstate), GetStringForState(m_iState));
#endif
}

BOOL CBaseDelay::IsLockedByMaster(void)
{
	if (!FStringNull(m_iszMaster) && !UTIL_IsMasterTriggered(m_iszMaster, m_hActivator))
		return TRUE;
	else
		return FALSE;
}

void CBaseDelay::DelayThink(void)
{
	SUB_UseTargets(m_hActivator, (USE_TYPE)pev->button, pev->armorvalue);// XDM3035c: don't FireTargets() because we may have m_iszKillTarget and stuff
	REMOVE_ENTITY(ENT(pev));
}





// Global Savedata for Toggle
TYPEDESCRIPTION	CBaseToggle::m_SaveData[] =
{
	DEFINE_FIELD( CBaseToggle, m_toggle_state, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseToggle, m_flActivateFinished, FIELD_TIME ),
	DEFINE_FIELD( CBaseToggle, m_flMoveDistance, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseToggle, m_flWait, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseToggle, m_flLip, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseToggle, m_flTWidth, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseToggle, m_flTLength, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseToggle, m_vecPosition1, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CBaseToggle, m_vecPosition2, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CBaseToggle, m_vecAngle1, FIELD_VECTOR ),		// UNDONE: Position could go through transition, but also angle?
	DEFINE_FIELD( CBaseToggle, m_vecAngle2, FIELD_VECTOR ),		// UNDONE: Position could go through transition, but also angle?
	DEFINE_FIELD( CBaseToggle, m_cTriggersLeft, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseToggle, m_flHeight, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseToggle, m_pfnCallWhenMoveDone, FIELD_FUNCTION ),
	DEFINE_FIELD( CBaseToggle, m_vecFinalDest, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CBaseToggle, m_vecFinalAngle, FIELD_VECTOR ),
	DEFINE_FIELD( CBaseToggle, m_bitsDamageInflict, FIELD_INTEGER ),	// damage type inflicted
};

IMPLEMENT_SAVERESTORE( CBaseToggle, CBaseAnimating );

void CBaseToggle::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "lip"))
	{
		m_flLip = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "wait"))
	{
		m_flWait = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "distance"))
	{
		m_flMoveDistance = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue(pkvd);
}

/*
=============
LinearMove

calculate pev->velocity and pev->nextthink to reach vecDest from
pev->origin traveling at flSpeed
===============
*/
void CBaseToggle::LinearMove(const Vector &vecDest, const float &flSpeed)
{
//	ALERT(at_console, "LinearMove(%f)\n", flSpeed);

	ASSERTSZ(flSpeed != 0, "LinearMove:  no speed is defined!");
//	ASSERTSZ(m_pfnCallWhenMoveDone != NULL, "LinearMove: no post-move function defined");

	m_vecFinalDest = vecDest;

	// Already there?
	if (vecDest == pev->origin)
	{
		LinearMoveDone();
		return;
	}

	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDest - pev->origin;

	// divide vector length by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to LinearMoveDone when dest is reached
	pev->nextthink = pev->ltime + flTravelTime;
	SetThink(&CBaseToggle::LinearMoveDone);

	// scale the destdelta vector by the time spent traveling to get velocity
	pev->velocity = vecDestDelta / flTravelTime;
//	ALERT(at_console, "LinearMove(%f, %f, %f)\n", pev->velocity.x, pev->velocity.y, pev->velocity.z);
}


/*
============
After moving, set origin to exact final destination, call "move done" function
============
*/
void CBaseToggle::LinearMoveDone(void)
{
//	ALERT(at_console, "LinearMoveDone()\n");

	UTIL_SetOrigin(pev, m_vecFinalDest);
	pev->velocity = g_vecZero;
	pev->nextthink = -1;
	SetThinkNull();// XDM3035a: this prevents double call of LinearMoveDone() 

	if (m_pfnCallWhenMoveDone)
		(this->*m_pfnCallWhenMoveDone)();
}


/*
=============
AngularMove

calculate pev->velocity and pev->nextthink to reach vecDest from
pev->origin traveling at flSpeed
Just like LinearMove, but rotational.
===============
*/
void CBaseToggle::AngularMove(const Vector &vecDestAngles, const float &flSpeed)
{
//	ALERT(at_console, "AngularMove(%f)\n", flSpeed);

	ASSERTSZ(flSpeed != 0, "AngularMove:  no speed is defined!");
//	ASSERTSZ(m_pfnCallWhenMoveDone != NULL, "AngularMove: no post-move function defined");

	m_vecFinalAngle = vecDestAngles;

	// Already there?
	if (vecDestAngles == pev->angles)
	{
		AngularMoveDone();
		return;
	}

	// set destdelta to the vector needed to move
	Vector vecDestDelta = vecDestAngles - pev->angles;

	// divide by speed to get time to reach dest
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	// set nextthink to trigger a call to AngularMoveDone when dest is reached
	pev->nextthink = pev->ltime + flTravelTime;
	SetThink(&CBaseToggle::AngularMoveDone);

	// scale the destdelta vector by the time spent traveling to get velocity
	pev->avelocity = vecDestDelta / flTravelTime;
}


/*
============
After rotating, set angle to exact final angle, call "move done" function
============
*/
void CBaseToggle::AngularMoveDone(void)
{
//	ALERT(at_console, "AngularMoveDone()\n");

	pev->angles = m_vecFinalAngle;
	pev->avelocity = g_vecZero;
	pev->nextthink = -1;
	SetThinkNull();// XDM3035a

	if (m_pfnCallWhenMoveDone)
		(this->*m_pfnCallWhenMoveDone)();
}

/*
void CBaseToggle::InterpolatedMove(Vector vecDest, float flStartSpeed, float flEndSpeed)
{
	ALERT(at_console, "InterpolateMove(%f, %f)\n", flStartSpeed, flEndSpeed);
	m_vecFinalDest = vecDest;

	if (vecDest == pev->origin)
	{
		LinearMoveDone();
		return;
	}
	float midspeed = min(flEndSpeed, flStartSpeed) + (flEndSpeed - flStartSpeed)/2;
	float flTravelTime = m_flMoveDistance/midspeed;
	Vector vecDestDelta = vecDest - pev->origin;// XYZ delta
	m_flMoveDistance = vecDestDelta.Length();
	m_vecPosition2 = vecDestDelta.Normalize();

	// starting velocity with current speed
	pev->velocity = m_vecPosition2 * flStartSpeed;
//	Vector endvelocity = vecDestDelta / (m_flMoveDistance / flEndSpeed);
//	pev->basevelocity = (endvelocity - pev->velocity)/(flTravelTime / pev->framerate);// vector speed delta
//	pev->basevelocity = vecDestDelta*((flEndSpeed - flStartSpeed)/m_flMoveDistance)/(flTravelTime / pev->framerate);// vector speed delta

//	endvelocity = (flEndSpeed / flStartSpeed) * pev->velocity
// OR
//	pev->dmgtime = (flEndSpeed - flStartSpeed)/(flTravelTime / 0.05);// scalar speed increment
//	deltavelocity = vecDestDelta / (m_flMoveDistance / ds);

	float flSpeedDelta = flEndSpeed - flStartSpeed;
	pev->speed = (flSpeedDelta * flSpeedDelta) / (2 * m_flMoveDistance);
	pev->dmgtime = flStartSpeed;
	pev->starttime = gpGlobals->time;// ?

	if (flSpeedDelta < 0) pev->speed *= -1;

	SetThink(&CBaseToggle::InterMove);
	pev->nextthink = gpGlobals->time;//pev->ltime;
}

void CBaseToggle::InterMove(void)
{
	if (m_vecFinalDest == pev->origin)
	{
		LinearMoveDone();
		return;
	}

	ALERT(at_console, "InterMove: spd: %f, vel: %f %f %f\n", pev->speed, pev->velocity.x, pev->velocity.y, pev->velocity.z);
	pev->velocity = m_vecPosition2 * pev->speed;//pev->velocity + pev->basevelocity; or
	pev->speed += pev->dmgtime * gpGlobals->frametime;

	// move also all attached entities
/*	if (m_iszMoveTargetName != NULL)// XDM
	{
		CBaseEntity *pNewEntity = NULL;
		while ((pNewEntity = UTIL_FindEntityByTargetname(pNewEntity, STRING(m_iszMoveTargetName))) != NULL)
		{
			pNewEntity->m_pMoveWith = this;
			pNewEntity->SetThink(&CBaseEntity::SUB_MoveWith);
			pNewEntity->pev->velocity = vecDestDelta / flTravelTime;
			pNewEntity->pev->nextthink = pev->ltime;
		}
	}* /
	pev->velocity = ((pev->speed * (pev->ltime - pev->starttime)) + pev->dmgtime) * m_vecPosition2;
	pev->nextthink = gpGlobals->time + 0.05;
}
*/

float CBaseToggle::AxisValue(int flags, const Vector &angles)
{
	if (FBitSet(flags, SF_DOOR_ROTATE_Z))
		return angles.z;
	if (FBitSet(flags, SF_DOOR_ROTATE_X))
		return angles.x;

	return angles.y;
}

void CBaseToggle::AxisDir(void)
{
	if (FBitSet(pev->spawnflags, SF_DOOR_ROTATE_Z))
		pev->movedir = Vector(0, 0, 1);	// around z-axis
	else if (FBitSet(pev->spawnflags, SF_DOOR_ROTATE_X))
		pev->movedir = Vector(1, 0, 0);	// around x-axis
	else
		pev->movedir = Vector(0, 1, 0);	// around y-axis
}

float CBaseToggle::AxisDelta(int flags, const Vector &angle1, const Vector &angle2)
{
	if (FBitSet(flags, SF_DOOR_ROTATE_Z))
		return angle1.z - angle2.z;

	if (FBitSet(flags, SF_DOOR_ROTATE_X))
		return angle1.x - angle2.x;

	return angle1.y - angle2.y;
}

/*
STATE CBaseToggle::GetState(void)// XDM
{
	switch (m_toggle_state)
	{
		case TS_AT_TOP:		return STATE_ON;
		case TS_AT_BOTTOM:	return STATE_OFF;
		case TS_GOING_UP:	return STATE_TURN_ON;
		case TS_GOING_DOWN:	return STATE_TURN_OFF;
		default:			return STATE_OFF; // This should never happen.
	}
};
*/


//-----------------------------------------------------------------------------
// Purpose: This is one of the fundamental things in HL. This method is used
// by all entities that can activate ("fire") something by targetname.
// Input  : *targetName - other pev->targetname to search by
//			*pActivator - person who started the chain (for CBaseEntity::Use())
//			*pCaller - exact entity that fires the target
//			useType - USE_TYPE
//			value -
//-----------------------------------------------------------------------------
void FireTargets(const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (targetName == NULL)
		return;

	// SHL: force some use types
	if (targetName[0] == '+')
	{
		targetName++;
		useType = USE_ON;
	}
	else if (targetName[0] == '-')
	{
		targetName++;
		useType = USE_OFF;
	}
 	else if (targetName[0] == '<')
	{
		targetName++;
		useType = USE_SET;
	}
/*	else if (targetName[0] == '>')
	{
		targetName++;
		useType = USE_RESET;
	}
	else if (targetName[0] == '!')
	{
		targetName++;
		useType = USE_REMOVE;
	}*/	

#ifdef _DEBUG
	ALERT(at_aiconsole, "FireTargets(\"%s\", activator %s(%d), caller %s(%d), t %d, v %g)\n", targetName, pActivator?STRING(pActivator->pev->classname):"NULL", pActivator?pActivator->entindex():0, pCaller?STRING(pCaller->pev->classname):"NULL", pCaller?pCaller->entindex():0, useType, value);
#endif
	CBaseEntity *pTarget = NULL;
	while ((pTarget = UTIL_FindEntityByTargetname(pTarget, targetName)) != NULL)
	{
		if (pTarget == pCaller)
		{
			ALERT(at_aiconsole, " FireTargets: WARNING! found self in activation list!\n");
//				continue;// XDM3037: WARNING! NOTE: this may cause some things to not to work
		}
		ALERT(at_aiconsole, " Found: %s (%s), firing\n", STRING(pTarget->pev->classname), targetName);
		pTarget->Use(pActivator, pCaller, useType, value);
	}
/*	edict_t *pentTarget = NULL;
	for (;;)
	{
		pentTarget = FIND_ENTITY_BY_TARGETNAME(pentTarget, targetName);
		if (FNullEnt(pentTarget))
			break;
		if (!UTIL_IsValidEntity(pentTarget))
			continue;

		CBaseEntity *pTarget = CBaseEntity::Instance(pentTarget);
		if (pTarget)// UTIL_IsValidEntity takes care of this// && !(pTarget->pev->flags & FL_KILLME))	// Don't use dying ents
		{
			if (pTarget == pCaller)
			{
				ALERT(at_aiconsole, " FireTargets: WARNING! found self in activation list!\n");
				continue;
			}
			ALERT(at_aiconsole, " Found: %s (%s), firing\n", STRING(pTarget->pev->classname), targetName);
			pTarget->Use(pActivator, pCaller, useType, value);
		}
	}*/
}
