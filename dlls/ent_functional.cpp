#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "ent_functional.h"
#include "doors.h"
#include "sound.h"
#include "weapons.h"
#include "globals.h"
#include "effects.h"
#if defined(MOVEWITH)
#include "movewith.h"
#endif

LINK_ENTITY_TO_CLASS(info_teleport_destination, CPointEntity);
// Lightning target, just alias landmark
LINK_ENTITY_TO_CLASS(info_target, CPointEntity);


LINK_ENTITY_TO_CLASS(multisource, CMultiSource);

TYPEDESCRIPTION CMultiSource::m_SaveData[] =
{
	//!!!BUGBUG FIX
	DEFINE_ARRAY( CMultiSource, m_rgEntities, FIELD_EHANDLE, MS_MAX_TARGETS ),
	DEFINE_ARRAY( CMultiSource, m_rgTriggered, FIELD_INTEGER, MS_MAX_TARGETS ),
	DEFINE_FIELD( CMultiSource, m_iTotal, FIELD_INTEGER ),
	DEFINE_FIELD( CMultiSource, m_globalstate, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE(CMultiSource, CPointEntity);


void CMultiSource::KeyValue(KeyValueData *pkvd)
{
	CPointEntity::KeyValue(pkvd);
	if (pkvd->fHandled)
		return;

	if (FStrEq(pkvd->szKeyName, "style") ||
		FStrEq(pkvd->szKeyName, "height") ||
		FStrEq(pkvd->szKeyName, "killtarget") ||
		FStrEq(pkvd->szKeyName, "value1") ||
		FStrEq(pkvd->szKeyName, "value2") ||
		FStrEq(pkvd->szKeyName, "value3"))
	{
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "globalstate"))
	{
		m_globalstate = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

void CMultiSource::Spawn(void)
{
	CPointEntity::Spawn();
	// set up think for later registration
//	pev->solid = SOLID_NOT;
//	pev->movetype = MOVETYPE_NONE;
//old	pev->spawnflags |= SF_MULTISOURCE_INIT;	// Until it's initialized
	SetThink(&CMultiSource::Register);
	pev->nextthink = gpGlobals->time + 0.25;
}

// WARNING! pCaller may be CBaseDelay!!
void CMultiSource::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Still initializing?
//old	if (pev->spawnflags & SF_MULTISOURCE_INIT)
	if (pev->impulse == 0)
	{
		ALERT(at_console, "Warning: %s %s used by %s %s while initializing!\n", STRING(pev->classname), STRING(pev->targetname), STRING(pCaller->pev->classname), STRING(pCaller->pev->targetname));
//		return;
	}
	// Find the entity in our list
	if (FClassnameIs(pCaller->pev, "DelayedUse"))// XDM3035c: 20121121 HACK to accept "delayed use" from buttons, doors, etc.
	{
		if (UTIL_IsValidEntity(pCaller->m_pGoalEnt))
			pCaller = pCaller->m_pGoalEnt;// DELAY_ACTIVATOR_HACK
		else
			ALERT(at_console, "Warning: %s %s used by delayed invalid caller!\n", STRING(pev->classname), STRING(pev->targetname));
	}
	int i = 0;
	while (i < m_iTotal)
	{
		if ((CBaseEntity *)m_rgEntities[i] == pCaller)
			break;
		++i;
	}

	// if we didn't find it, report error and leave
	if (i >= m_iTotal)
	{
		ALERT(at_console, "Warning: %s %s used by non member %s %s!\n", STRING(pev->classname), STRING(pev->targetname), STRING(pCaller->pev->classname), STRING(pCaller->pev->targetname));
		return;
	}

	// CONSIDER: a Use input to the multisource always toggles.  Could check useType for ON/OFF/TOGGLE
//	if (ShouldToggle(useType, m_rgTriggered[i-1]))
//	m_rgTriggered[i-1] ^= 1;
	m_rgTriggered[i] ^= 1;

	if (IsTriggered(pActivator))
	{
		ALERT(at_aiconsole, "%s %s enabled (%d inputs)\n", STRING(pev->classname), STRING(pev->targetname), m_iTotal);
		USE_TYPE useType = USE_TOGGLE;
		if (m_globalstate)
			useType = USE_ON;

		SUB_UseTargets(/* NULL*/pActivator, useType, 0);// XDM3035a: TESTME!
	}
}

//-----------------------------------------------------------------------------
// Purpose: Did all my registered entities activate me?
// Input  : *pActivator - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CMultiSource::IsTriggered(CBaseEntity *pActivator)
{
	// Still initializing?
	//if (pev->spawnflags & SF_MULTISOURCE_INIT)
	if (pev->impulse == 0)
		return 0;

	// Is everything triggered?
	int i = 0;
	while (i < m_iTotal)
	{
		if (m_rgTriggered[i] == 0)// at least one untriggered entity is enough to stop
			break;
		++i;
	}

	if (i == m_iTotal)
	{
		if (!m_globalstate || gGlobalState.EntityGetState(m_globalstate) == GLOBAL_ON)
			return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Find and register entities that have me as their target
// Note: this code is MUCH cleaner, faster and more reliable than SoHL or anything else 8)
//-----------------------------------------------------------------------------
void CMultiSource::Register(void)
{
	m_iTotal = 0;
	memset(m_rgEntities, 0, MS_MAX_TARGETS * sizeof(EHANDLE));

	if (FStringNull(pev->targetname))
	{
		ALERT(at_warning, "%s with no targetname! Removing!\n", STRING(pev->classname));
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time;
		return;
	}

	SetThink(&CBaseEntity::SUB_DoNothing);
	pev->nextthink = 0;
//	SetThinkNull();
	ALERT(at_aiconsole, "%s %s registering sources\n", STRING(pev->classname), STRING(pev->targetname));

	// TODO: replace this all with global iteration with HasTarget() check
	CBaseEntity	*pEntity = NULL;
	CBaseEntity	*pTarget = NULL;
	edict_t		*pEdict = INDEXENT(MAX_PLAYERS+1);// skip players as they can't be multisource sources
	for (int i = MAX_PLAYERS+1; i < gpGlobals->maxEntities; ++i, ++pEdict)
	{
		if (!UTIL_IsValidEntity(pEdict))
			continue;
		pEntity = CBaseEntity::Instance(pEdict);
		if (pEntity == NULL)
			continue;
		if (pEntity == this)
			continue;

		pTarget = pEntity;
/* this is useless now because even DelayedUse's "caller" gets iterated in the process
		if (FClassnameIs(pEdict, "DelayedUse"))
		{
			if (!UTIL_IsValidEntity(pTarget->m_pGoalEnt))// DELAY_ACTIVATOR_HACK WTF? VITAL! This can NULL or a deleted trigger_auto! TODO: use special EHANDLE
				continue;

			ALERT(at_aiconsole, " %d: redirecting %s %s\n", m_iTotal, STRING(pTarget->pev->classname), STRING(pTarget->pev->targetname));
			pTarget = pTarget->m_pGoalEnt;// DELAY_ACTIVATOR_HACK: look for the real caller
		}*/

		if (pTarget->HasTarget(pev->targetname))
		{
			if (pev->spawnflags & SF_MULTISOURCE_ON)
				m_rgTriggered[m_iTotal] = 1;
			else
				m_rgTriggered[m_iTotal] = 0;

			ALERT(at_aiconsole, " %d: %s %s (state %d)\n", m_iTotal, STRING(pTarget->pev->classname), STRING(pTarget->pev->targetname), m_rgTriggered[m_iTotal]);
			m_rgEntities[m_iTotal] = pTarget;
			++m_iTotal;
		}
	}
	ALERT(at_aiconsole, "total: %d\n", m_iTotal);
//	pev->spawnflags &= ~SF_MULTISOURCE_INIT;
	pev->impulse = 1;
}





LINK_ENTITY_TO_CLASS( multi_manager, CMultiManager );

// Global Savedata for multi_manager
TYPEDESCRIPTION	CMultiManager::m_SaveData[] =
{
	DEFINE_FIELD( CMultiManager, m_cTargets, FIELD_INTEGER ),
	DEFINE_FIELD( CMultiManager, m_index, FIELD_INTEGER ),
	DEFINE_FIELD( CMultiManager, m_startTime, FIELD_TIME ),
	DEFINE_ARRAY( CMultiManager, m_iTargetName, FIELD_STRING, MAX_MULTI_TARGETS ),
	DEFINE_ARRAY( CMultiManager, m_flTargetDelay, FIELD_FLOAT, MAX_MULTI_TARGETS ),
	DEFINE_FIELD( CMultiManager, m_iszThreadName, FIELD_STRING ),// SHL
	DEFINE_FIELD( CMultiManager, m_iszLocusThread, FIELD_STRING ),// SHL
};

IMPLEMENT_SAVERESTORE(CMultiManager,CBaseToggle);

void CMultiManager::KeyValue(KeyValueData *pkvd)
{
	// DONE: Maybe this should do something like this:
	//CBaseToggle::KeyValue( pkvd );
	// if ( !pkvd->fHandled )
	// ... etc.
	CBaseToggle::KeyValue(pkvd);// XDM3036
	if (pkvd->fHandled)
		return;

	if (FStrEq(pkvd->szKeyName, "wait"))
	{
		m_flWait = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
/*	else if (FStrEq(pkvd->szKeyName, "master"))
	{
		m_sMaster = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}*/
	else if (FStrEq(pkvd->szKeyName, "m_iszThreadName"))// SHL
	{
		m_iszThreadName = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iszLocusThread"))// SHL
	{
		m_iszLocusThread = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else // add this field to the target list
	{
		// this assumes that additional fields are targetnames and their values are delay values.
		if (m_cTargets < MAX_MULTI_TARGETS)
		{
			char tmp[128];
			UTIL_StripToken(pkvd->szKeyName, tmp);
			m_iTargetName[m_cTargets] = ALLOC_STRING(tmp);
			m_flTargetDelay[m_cTargets] = atof(pkvd->szValue);
			pkvd->fHandled = TRUE;
		}
		++m_cTargets;
	}
}

// TESTNE on maps c1a0a, c2a1
void CMultiManager::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	SetState(STATE_OFF);

	if (m_cTargets > MAX_MULTI_TARGETS)
	{
		ALERT(at_aiconsole, "WARNING: %s \"%s\" has too many targets: %d (max %d)\n", STRING(pev->classname), STRING(pev->targetname), m_cTargets, MAX_MULTI_TARGETS);
		m_cTargets = MAX_MULTI_TARGETS;
	}

	SetUse(&CMultiManager::ManagerUse);
	SetThink(&CMultiManager::ManagerThink);

	// Sort targets
	// Quick and dirty bubble sort
	int swapped = 1;
	while (swapped)
	{
		swapped = 0;
		for (int i = 1; i < m_cTargets; ++i)
		{
			if (m_flTargetDelay[i] < m_flTargetDelay[i-1])
			{
				// Swap out of order elements
				int name = m_iTargetName[i];
				float delay = m_flTargetDelay[i];
				m_iTargetName[i] = m_iTargetName[i-1];
				m_flTargetDelay[i] = m_flTargetDelay[i-1];
				m_iTargetName[i-1] = name;
				m_flTargetDelay[i-1] = delay;
				swapped = 1;
			}
		}
	}

#ifdef _DEBUG
	ManagerReport();// XDM3035a
#endif
}

void CMultiManager::Activate(void)
{
	if (pev->spawnflags & SF_MULTIMAN_SPAWNFIRE)
	{
//		SetThink(&CMultiManager::UseThink);
//		SetUseNull();
		Use(g_pWorld, this, USE_ON, 1.0f);
#if defined(MOVEWITH)
		UTIL_DesiredThink(this);
#endif
	}
}

BOOL CMultiManager::HasTarget(string_t targetname)
{
	for (int i = 0; i < m_cTargets; ++i)
		if (FStrEq(STRING(targetname), STRING(m_iTargetName[i])))
			return TRUE;

	return FALSE;
}

// Designers were using this to fire targets that may or may not exist --
// so I changed it to use the standard target fire code, made it a little simpler.
void CMultiManager::ManagerThink(void)
{
	if (!IsActive())
	{
		SetNextThink(0.2);
		return;
	}

	float dtime = gpGlobals->time - m_startTime;
	while (m_index < m_cTargets && m_flTargetDelay[m_index] <= dtime)
	{
/*		if (FStrEq(STRING(m_iTargetName[m_index]), STRING(pev->targetname)))// Found self in list!!
		{
			ALERT(at_debug, "%s \"%s\": ManagerThink(): WARNING: Activating self!\n", STRING(pev->classname), STRING(pev->targetname));
			m_iTargetName[m_index] = 0;// don't hit that twice
			m_index = 0;// pretend it's been restarted
			m_startTime = gpGlobals->time;
			SetNextThink(0);
			return;//break;
		}*/

		FireTargets(STRING(m_iTargetName[m_index]), m_hActivator, this, USE_TOGGLE, 0);
		m_index++;
	}

	if (m_index >= m_cTargets)// have we fired all targets?
	{
		if (IsLooping())
		{
			ALERT(at_debug, "%s \"%s\": ManagerThink(): loop\n", STRING(pev->classname), STRING(pev->targetname));
			m_index = 0;
			m_startTime = gpGlobals->time;
			SetNextThink(0);
		}
		else
		{
			SetState(STATE_OFF);// finished
			ALERT(at_aiconsole, "%s \"%s\": ManagerThink(): finished\n", STRING(pev->classname), STRING(pev->targetname));
			if (IsClone() || pev->spawnflags & SF_MULTIMAN_ONLYONCE)
			{
				SetThink(&CMultiManager::SUB_Remove);
				SetNextThink(0.1);
//				UTIL_Remove(this);
//				return;
			}
			else
			{
				SetThinkNull();
				SetUse(&CMultiManager::ManagerUse);// allow manager re-use
			}
		}
	}
	else
		pev->nextthink = m_startTime + m_flTargetDelay[m_index];// don't think until next target needs to be fired
}

CMultiManager *CMultiManager::Clone(void)
{
	CMultiManager *pMulti = (CMultiManager *)CreateCopy(pev->classname, pev, pev->spawnflags|SF_MULTIMAN_CLONE, false);// XDM3035c: TESTED
	if (pMulti == NULL)
		return NULL;

	pMulti->m_cTargets = m_cTargets;
	memcpy(pMulti->m_iTargetName, m_iTargetName, sizeof(m_iTargetName));
	memcpy(pMulti->m_flTargetDelay, m_flTargetDelay, sizeof(m_flTargetDelay));
/*	// SHL
	if (m_iszThreadName) pMulti->pev->targetname = m_iszThreadName;
	pMulti->m_triggerType = m_triggerType;
	pMulti->m_iMode = m_iMode;
	pMulti->m_flWait = m_flWait;
	pMulti->m_flMaxWait = m_flMaxWait;*/
	return pMulti;
}

// The USE function builds the time table and starts the entity thinking.
void CMultiManager::ManagerUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (IsLooping())// SHL compatibility
	{
		if (IsActive())// if we're on, or turning on...
		{
			if (useType != USE_ON) // ...then turn it off if we're asked to.
			{
				SetState(STATE_OFF);
				if (IsClone() || (pev->spawnflags & SF_MULTIMAN_ONLYONCE))
				{
					SetUseNull();
					SetThink(&CMultiManager::SUB_Remove);
					SetNextThink(0.1);
					ALERT(at_debug, "%s \"%s\": Use(): loop halted (removing)\n", STRING(pev->classname), STRING(pev->targetname));
				}
				else
				{
					SetThinkNull();
					ALERT(at_debug, "%s \"%s\": Use(): loop halted\n", STRING(pev->classname), STRING(pev->targetname));
				}
			}
			// else we're already on and being told to turn on, so do nothing.
			else
				ALERT(at_debug, "%s \"%s\": Use(): loop already active.\n", STRING(pev->classname), STRING(pev->targetname));

			return;
		}
		else if (useType == USE_OFF) // it's already off
		{
			ALERT(at_debug, "%s \"%s\": Use(): loop already inactive.\n", STRING(pev->classname), STRING(pev->targetname));
			return;
		}
		// otherwise, start firing targets as normal.
	}

	if (IsLockedByMaster())
	{
		ALERT(at_debug, "%s \"%s\": Use(): locked by master \"%s\".\n", STRING(pev->classname), STRING(pev->targetname), STRING(m_iszMaster));
		return;
	}

	// In multiplayer games, clone the MM and execute in the clone (like a thread)
	// to allow multiple players to trigger the same multimanager
	if (ShouldClone())
	{
		CMultiManager *pClone = Clone();
		if (pClone)
		{
			pClone->ManagerUse(pActivator, pCaller, useType, value);
			if (m_iszLocusThread)
				FireTargets(STRING(m_iszLocusThread), pClone, this, USE_TOGGLE, 0);
		}
		return;
	}

	m_hActivator = pActivator;
	m_index = 0;
	m_startTime = gpGlobals->time;
	SetState(STATE_ON);

//	if (pev->spawnflags & SF_MULTIMAN_SAMETRIG)// SHL compatibility
//		m_triggerType = useType;

	if (IsLooping())// XDM3035c: toggle
		SetUse(&CMultiManager::ManagerUse);
	else
		SetUseNull();// disable use until all targets have fired

	SetThink(&CMultiManager::ManagerThink);
	SetNextThink(0);
	ManagerThink();
}

#if _DEBUG
void CMultiManager::ManagerReport(void)
{
	ALERT(at_console, "%s %s %s: state: %d targets: %d:\n", STRING(pev->classname), STRING(pev->targetname), STRING(pev->globalname), GetState(), m_cTargets);
	int	cIndex;
	for (cIndex = 0; cIndex < m_cTargets; cIndex++)
		ALERT(at_console, " %d: %s %g\n", cIndex, STRING(m_iTargetName[cIndex]), m_flTargetDelay[cIndex]);
}
#endif





TYPEDESCRIPTION CEnvGlobal::m_SaveData[] =
{
	DEFINE_FIELD( CEnvGlobal, m_globalstate, FIELD_STRING ),
	DEFINE_FIELD( CEnvGlobal, m_triggermode, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvGlobal, m_initialstate, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CEnvGlobal, CPointEntity );

LINK_ENTITY_TO_CLASS( env_global, CEnvGlobal );

void CEnvGlobal::KeyValue( KeyValueData *pkvd )
{
// XDM3035c?	pkvd->fHandled = TRUE;

	if ( FStrEq(pkvd->szKeyName, "globalstate") )		// State name
	{
		m_globalstate = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "triggermode") )
	{
		m_triggermode = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "initialstate") )
	{
		m_initialstate = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CEnvGlobal::Spawn(void)
{
	if ( !m_globalstate )
	{
		REMOVE_ENTITY( ENT(pev) );
		return;
	}
	if ( FBitSet( pev->spawnflags, SF_GLOBAL_SET ) )
	{
		if ( !gGlobalState.EntityInTable( m_globalstate ) )
			gGlobalState.EntityAdd( m_globalstate, gpGlobals->mapname, (GLOBALESTATE)m_initialstate );
	}
}

void CEnvGlobal::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	GLOBALESTATE oldState = gGlobalState.EntityGetState( m_globalstate );
	GLOBALESTATE newState;

	switch( m_triggermode )
	{
	case 0:
		newState = GLOBAL_OFF;
		break;
	case 1:
		newState = GLOBAL_ON;
		break;
	case 2:
		newState = GLOBAL_DEAD;
		break;
	default:
	case 3:
		if ( oldState == GLOBAL_ON )
			newState = GLOBAL_OFF;
		else if ( oldState == GLOBAL_OFF )
			newState = GLOBAL_ON;
		else
			newState = oldState;
	}

	if (gGlobalState.EntityInTable(m_globalstate))
		gGlobalState.EntitySetState(m_globalstate, newState);
	else
		gGlobalState.EntityAdd(m_globalstate, gpGlobals->mapname, newState);
}




TYPEDESCRIPTION CEnvState::m_SaveData[] =
{
	DEFINE_FIELD( CEnvState, m_fTurnOnTime, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvState, m_fTurnOffTime, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE(CEnvState, CBaseToggle);

LINK_ENTITY_TO_CLASS( env_state, CEnvState );

void CEnvState::KeyValue(KeyValueData *pkvd)
{
//	pkvd->fHandled = TRUE;

	if (FStrEq(pkvd->szKeyName, "turnontime"))
	{
		m_fTurnOnTime = atof(pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "turnofftime"))
	{
		m_fTurnOffTime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue(pkvd);
}

void CEnvState::Spawn(void)
{
	if (pev->spawnflags & SF_ENVSTATE_START_ON)
		SetState(STATE_ON);
	else
		SetState(STATE_OFF);
}

void CEnvState::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType, GetState() != STATE_OFF) || IsLockedByMaster())// XDM3037: STATE cast -- TESTME
	{
		if (pev->spawnflags & SF_ENVSTATE_DEBUG)
		{
			ALERT(at_aiconsole,"DEBUG: env_state \"%s\" ",STRING(pev->targetname));
			if (IsLockedByMaster())
				ALERT(at_aiconsole,"ignored trigger %s; locked by master \"%s\".\n",GetStringForUseType(useType),STRING(m_iszMaster));
			else if (useType == USE_ON)
				ALERT(at_aiconsole,"ignored trigger USE_ON; already on\n");
			else if (useType == USE_OFF)
				ALERT(at_aiconsole,"ignored trigger USE_OFF; already off\n");
			else
				ALERT(at_aiconsole,"ignored trigger %s.\n",GetStringForUseType(useType));
		}
		return;
	}

	switch (GetState())
	{
	case STATE_ON:
	case STATE_TURN_ON:
		if (m_fTurnOffTime)
		{
			SetState(STATE_TURN_OFF);
			if (pev->spawnflags & SF_ENVSTATE_DEBUG)
			{
				ALERT(at_aiconsole,"DEBUG: env_state \"%s\" triggered; will turn off in %f seconds.\n", STRING(pev->targetname), m_fTurnOffTime);
			}
			SetNextThink( m_fTurnOffTime );
		}
		else
		{
			SetState(STATE_OFF);
			if (pev->spawnflags & SF_ENVSTATE_DEBUG)
			{
				ALERT(at_aiconsole,"DEBUG: env_state \"%s\" triggered, turned off", STRING(pev->targetname));
				if (pev->target)
				{
					ALERT(at_aiconsole,": firing \"%s\"",STRING(pev->target));
					if (pev->noise2)
						ALERT(at_aiconsole," and \"%s\"",STRING(pev->noise2));
				}
				else if (pev->noise2)
					ALERT(at_aiconsole,": firing \"%s\"",STRING(pev->noise2));
				ALERT(at_aiconsole,".\n");
			}
			FireTargets(STRING(pev->target),pActivator,this,USE_OFF,0);
			FireTargets(STRING(pev->noise2),pActivator,this,USE_TOGGLE,0);
			SetNextThink(0);
		}
		break;
	case STATE_OFF:
	case STATE_TURN_OFF:
		if (m_fTurnOnTime)
		{
			SetState(STATE_TURN_ON);
			if (pev->spawnflags & SF_ENVSTATE_DEBUG)
			{
				ALERT(at_aiconsole,"DEBUG: env_state \"%s\" triggered; will turn on in %f seconds.\n", STRING(pev->targetname), m_fTurnOnTime);
			}
			SetNextThink( m_fTurnOnTime );
		}
		else
		{
			SetState(STATE_ON);
			if (pev->spawnflags & SF_ENVSTATE_DEBUG)
			{
				ALERT(at_aiconsole,"DEBUG: env_state \"%s\" triggered, turned on",STRING(pev->targetname));
				if (pev->target)
				{
					ALERT(at_aiconsole,": firing \"%s\"",STRING(pev->target));
					if (pev->noise1)
						ALERT(at_aiconsole," and \"%s\"",STRING(pev->noise1));
				}
				else if (pev->noise1)
					ALERT(at_aiconsole,": firing \"%s\"", STRING(pev->noise1));
				ALERT(at_aiconsole,".\n");
			}
			FireTargets(STRING(pev->target),pActivator,this,USE_ON,0);
			FireTargets(STRING(pev->noise1),pActivator,this,USE_TOGGLE,0);
			SetNextThink(0);
		}
		break;
	}
}

void CEnvState::Think(void)
{
	STATE iState = GetState();
	if (iState == STATE_TURN_ON)
	{
		SetState(STATE_ON);
		if (pev->spawnflags & SF_ENVSTATE_DEBUG)
		{
			ALERT(at_aiconsole,"DEBUG: env_state \"%s\" turned itself on",STRING(pev->targetname));
			if (pev->target)
			{
				ALERT(at_aiconsole,": firing %s",STRING(pev->target));
				if (pev->noise1)
					ALERT(at_aiconsole," and %s",STRING(pev->noise1));
			}
			else if (pev->noise1)
				ALERT(at_aiconsole,": firing %s",STRING(pev->noise1));
			ALERT(at_aiconsole,".\n");
		}
		FireTargets(STRING(pev->target),this,this,USE_ON,0);
		FireTargets(STRING(pev->noise1),this,this,USE_TOGGLE,0);
	}
	else if (iState == STATE_TURN_OFF)
	{
		SetState(STATE_OFF);
		if (pev->spawnflags & SF_ENVSTATE_DEBUG)
		{
			ALERT(at_aiconsole,"DEBUG: env_state \"%s\" turned itself off",STRING(pev->targetname));
			if (pev->target)
				ALERT(at_aiconsole,": firing %s",STRING(pev->target));
				if (pev->noise2)
					ALERT(at_aiconsole," and %s",STRING(pev->noise2));
			else if (pev->noise2)
				ALERT(at_aiconsole,": firing %s",STRING(pev->noise2));
			ALERT(at_aiconsole,".\n");
		}
		FireTargets(STRING(pev->target),this,this,USE_OFF,0);
		FireTargets(STRING(pev->noise2),this,this,USE_TOGGLE,0);
	}
}




LINK_ENTITY_TO_CLASS(env_beverage, CEnvBeverage);

void CEnvBeverage::Precache(void)
{
	UTIL_PrecacheOther("item_sodacan");// XDM
}

void CEnvBeverage::Spawn(void)
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->frags = 0;

	if (pev->health == 0)
		pev->health = 10;
}

void CEnvBeverage::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (pev->frags != 0 || pev->health == 0)// XDM3035c: CAN be -1 (infinite)
	{
		// no more cans while one is waiting in the dispenser, or if I'm out of cans.
		return;
	}

	CBaseEntity *pCan = CBaseEntity::Create("item_sodacan", pev->origin, pev->angles, edict());
	if (pCan)
	{
//		if ( pev->skin == 0)// XDM3035a: was 6. But I want some fun!
		{
			// random
			pCan->pev->skin = RANDOM_LONG(0, pCan->pev->impulse-1);
		}
/*		else
		{
			pCan->pev->skin = pev->skin;
		}*/

		pev->frags = 1;
		pev->health--;
	}
	//SetThink(&CBaseEntity::SUB_Remove);
	//pev->nextthink = gpGlobals->time;
}




LINK_ENTITY_TO_CLASS(env_ammodispenser, CFuncAmmoDispenser);

void CFuncAmmoDispenser::Spawn(void)
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;
	pev->modelindex = 0;
}

void CFuncAmmoDispenser::Precache(void)
{
//	UTIL_PrecacheOther(STRING(pev->message));// XDM
}

void CFuncAmmoDispenser::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (pActivator == NULL)
		return;

//	if (pActivator->IsPlayer())
	{
		int ammo_id = GetAmmoIndexFromRegistry(STRING(pev->message));
		if (ammo_id >= 0)
		{
			if (pActivator->GiveAmmo((int)pev->frags, ammo_id, MaxAmmoCarry(ammo_id)) <= 0)
				ALERT(at_console, "%s %s: unable to give ammo\n", STRING(pev->classname), STRING(pev->targetname));
		}
		else
			ALERT(at_console, "%s %s: invalid ammo type: %s!\n", STRING(pev->classname), STRING(pev->targetname), STRING(pev->message));
	}
}




LINK_ENTITY_TO_CLASS(env_cache, CEnvCache);

void CEnvCache::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "targetname"))
	{
		pev->targetname = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "graphic"))// avoid conflicts with entvars_t
	{
		if (m_iNumModels < ENVCACHE_MAX_SOUNDS)
		{
			strcpy(m_MdlNames[m_iNumModels], pkvd->szValue);
			m_iNumModels++;
			pkvd->fHandled = TRUE;
		}
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "sound"))
	{
		if (m_iNumSounds < ENVCACHE_MAX_SOUNDS)
		{
			strcpy(m_SndNames[m_iNumSounds], pkvd->szValue);
			m_iNumSounds++;
			pkvd->fHandled = TRUE;
		}
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "entity"))
	{
		if (m_iNumEnts < ENVCACHE_MAX_ENTITIES)
		{
			strcpy(m_EntNames[m_iNumEnts], pkvd->szValue);
			m_iNumEnts++;
			pkvd->fHandled = TRUE;
		}
	}
	else
		CBaseDelay::KeyValue(pkvd);// really useless
}

void CEnvCache::Spawn(void)
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;
	pev->modelindex = 0;
}

void CEnvCache::Precache(void)
{
	int m,s,e;
	for (m = 0; m < m_iNumModels; ++m)
		PRECACHE_MODEL(m_MdlNames[m]);

	for (s = 0; s < m_iNumSounds; ++s)
		PRECACHE_SOUND(m_SndNames[s]);

	for (e = 0; e < m_iNumEnts; ++e)
		UTIL_PrecacheOther(m_EntNames[e]);

	ALERT(at_aiconsole, "%s %s: precached %d models %d sounds %d entities\n", STRING(pev->classname), STRING(pev->targetname), m,s,e);
}



//-----------------------------------------------------------------------------
// CMultiMaster - multi_watcher
//-----------------------------------------------------------------------------
enum
{
	LOGIC_AND = 0,	// fire if all objects active
	LOGIC_OR,		// fire if any object active
	LOGIC_NAND,		// fire if not all objects active
	LOGIC_NOR,		// fire if all objects disable
	LOGIC_XOR,		// fire if only one (any) object active
	LOGIC_XNOR		// fire if active any number objects, but < then all
};


LINK_ENTITY_TO_CLASS( multi_watcher, CMultiMaster );

TYPEDESCRIPTION CMultiMaster::m_SaveData[] =
{
	DEFINE_FIELD( CMultiMaster, m_cTargets, FIELD_INTEGER ),
	DEFINE_FIELD( CMultiMaster, m_iLogicMode, FIELD_INTEGER ),
	DEFINE_FIELD( CMultiMaster, m_iSharedState, FIELD_INTEGER ),
	DEFINE_ARRAY( CMultiMaster, m_iTargetName, FIELD_STRING, MAX_MULTI_TARGETS ),
	DEFINE_ARRAY( CMultiMaster, m_iTargetState, FIELD_INTEGER, MAX_MULTI_TARGETS ),
};
IMPLEMENT_SAVERESTORE( CMultiMaster, CBaseDelay );

void CMultiMaster::KeyValue(KeyValueData *pkvd)
{
	CBaseDelay::KeyValue(pkvd);
	if (pkvd->fHandled)
		return;

	if (FStrEq(pkvd->szKeyName, "logic"))
	{
		m_iLogicMode = GetLogicModeForString(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "state"))
	{
		m_iSharedState = GetStateForString(pkvd->szValue);
		pkvd->fHandled = TRUE;
		globalstate = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "offtarget"))
	{
		pev->netname = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else // add this field to the target list
	{
		// this assumes that additional fields are targetnames and their values are delay values.
		if (m_cTargets < MAX_MULTI_TARGETS)
		{
			char tmp[128];
			UTIL_StripToken(pkvd->szKeyName, tmp);
			m_iTargetName[m_cTargets] = ALLOC_STRING(tmp);
			m_iTargetState[m_cTargets] = GetStateForString(pkvd->szValue);
			m_cTargets++;
			pkvd->fHandled = TRUE;
		}
	}
}

int CMultiMaster::GetLogicModeForString(const char *string)
{
	if (string == NULL)
		return -1;

	if (!stricmp(string, "AND"))
		return LOGIC_AND;
	else if (!stricmp(string, "OR"))
		return LOGIC_OR;
	else if (!stricmp(string, "NAND") || !stricmp(string, "!AND"))
		return LOGIC_NAND;
	else if (!stricmp(string, "NOR") || !stricmp(string, "!OR"))
		return LOGIC_NOR;
	else if (!stricmp(string, "XOR") || !stricmp(string, "^OR"))
		return LOGIC_XOR;
	else if (!stricmp(string, "XNOR") || !stricmp(string, "^!OR"))
		return LOGIC_XNOR;
	else if (isdigit(string[0]))
		return atoi(string);

	ALERT(at_error, "%s %s: unknown logic mode '%s' specified\n", STRING(pev->classname), STRING(pev->targetname), string);
	return -1;
}

void CMultiMaster::Spawn(void)
{
	// use local states instead
	if (!globalstate)
		m_iSharedState = (STATE)-1;

	SetNextThink(0.1);
}

bool CMultiMaster::CheckState(STATE state, int targetnum)
{
	// global state for all targets
	if (m_iSharedState != -1)
	{
		if (m_iSharedState == state)
			return TRUE;
		return FALSE;
	}

	if ((STATE)m_iTargetState[targetnum] == state)
		return TRUE;

	return FALSE;
}

void CMultiMaster::Think(void)
{
	if (EvalLogic(NULL)) 
	{
		if (GetState() == STATE_OFF)
		{
			SetState(STATE_ON);
			/*UTIL_*/FireTargets(STRING(pev->target), this, this, USE_ON, 1.0f);
		}
	}
	else 
	{
		if (GetState() == STATE_ON)
		{
			SetState(STATE_OFF);
			/*UTIL_*/FireTargets(STRING(pev->netname), this, this, USE_OFF, 0.0f);
		}
	}
 	SetNextThink(0.01);
}

BOOL CMultiMaster::EvalLogic(CBaseEntity *pActivator)
{
	BOOL xorgot = FALSE;
	CBaseEntity *pEntity;
	for (int i = 0; i < m_cTargets; ++i)
	{
		pEntity = UTIL_FindEntityByTargetname(NULL, STRING(m_iTargetName[i]), pActivator);
		if (!pEntity) continue;

		// handle the states for this logic mode
		if (CheckState(pEntity->GetState(), i))
		{
			switch (m_iLogicMode)
			{
			case LOGIC_OR:
				return TRUE;
				break;
			case LOGIC_NOR:
				return FALSE;
				break;
			case LOGIC_XOR: 
				if (xorgot)
					return FALSE;
				xorgot = TRUE;
				break;
			case LOGIC_XNOR:
				if (xorgot)
					return TRUE;
				xorgot = TRUE;
				break;
			}
		}
		else // state is false
		{
			switch (m_iLogicMode)
			{
	        case LOGIC_AND:
	         	return FALSE;
				break;
			case LOGIC_NAND:
				return TRUE;
				break;
			}
		}
	}

	// handle the default cases for each logic mode
	switch (m_iLogicMode)
	{
	case LOGIC_AND:
	case LOGIC_NOR:
		return TRUE;
		break;
	case LOGIC_XOR:
		return xorgot;
		break;
	case LOGIC_XNOR:
		return !xorgot;
		break;
	default:
		return FALSE;
		break;
	}
}



//-----------------------------------------------------------------------------
// CSwitcher - multi_switcher
//-----------------------------------------------------------------------------
enum
{
	MODE_INCREMENT = 0,
	MODE_DECREMENT,
	MODE_RANDOM_VALUE
};

LINK_ENTITY_TO_CLASS( multi_switcher, CSwitcher );

// Global Savedata for switcher
TYPEDESCRIPTION CSwitcher::m_SaveData[] = 
{
	DEFINE_FIELD( CSwitcher, m_index, FIELD_INTEGER ),
	DEFINE_FIELD( CSwitcher, m_cTargets, FIELD_INTEGER ),
	DEFINE_ARRAY( CSwitcher, m_iTargetName, FIELD_STRING, MAX_MULTI_TARGETS ),
};
IMPLEMENT_SAVERESTORE( CSwitcher, CBaseDelay );

void CSwitcher::KeyValue(KeyValueData *pkvd)
{
	CBaseDelay::KeyValue(pkvd);
	if (pkvd->fHandled)
		return;

	if (FStrEq(pkvd->szKeyName, "mode"))
	{
		pev->button = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (m_cTargets < MAX_MULTI_TARGETS)
	{
		// add this field to the target list
		// this assumes that additional fields are targetnames and their values are delay values.
		char tmp[128];
		UTIL_StripToken(pkvd->szKeyName, tmp);
		m_iTargetName[m_cTargets] = ALLOC_STRING(tmp);
		m_cTargets++;
		pkvd->fHandled = TRUE;
	}
}

void CSwitcher::Spawn(void)
{
	int r_index = 0;
	int w_index = m_cTargets - 1;
	while (r_index < w_index)
	{
		// we store target with right index in tempname
		int name = m_iTargetName[r_index];
		// target with right name is free, record new value from wrong name
		m_iTargetName[r_index] = m_iTargetName[w_index];
		// ok, we can swap targets
		m_iTargetName[w_index] = name;
		r_index++;
		w_index--;
	}
	
	m_index = 0;
	if (FBitSet(pev->spawnflags, SF_SWITCHER_START_ON))
	{
 		SetNextThink(m_flDelay);
		SetState(STATE_ON);
	}
	else
		SetState(STATE_OFF);
}

void CSwitcher::Think(void)
{
	if (pev->button == MODE_INCREMENT)
	{
		// increase target number
		m_index++;
		if (m_index >= m_cTargets)
			m_index = 0;
	}
	else if (pev->button == MODE_DECREMENT)
	{
		m_index--;
		if (m_index < 0)
			m_index = m_cTargets - 1;
	}
	else if (pev->button == MODE_RANDOM_VALUE)
	{
		m_index = RANDOM_LONG(0, m_cTargets - 1);
	}

	SetNextThink(m_flDelay);
}

void CSwitcher::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (IsLockedByMaster())
		return;

	m_hActivator = pActivator;// XDM3035c: moved AFTER master check

	if (useType == USE_SET)
	{
		// set new target for activate (direct choose or increment\decrement)
		if (FBitSet(pev->spawnflags, SF_SWITCHER_START_ON))
		{
			SetState(STATE_ON);
			SetNextThink(m_flDelay);
			return;
		}

		// set maximum priority for direct choose
		if (value) 
		{
			m_index = (value - 1);
			if (m_index >= m_cTargets || m_index < -1)
				m_index = -1;
			return;
		}

		if (pev->button == MODE_INCREMENT)
		{
			m_index++;
			if (m_index >= m_cTargets)
				m_index = 0; 	
		}
		else if (pev->button == MODE_DECREMENT)
		{
			m_index--;
			if (m_index < 0)
				m_index = m_cTargets - 1;
		}
		else if (pev->button == MODE_RANDOM_VALUE)
		{
			m_index = RANDOM_LONG(0, m_cTargets - 1);
		}
	}
/*	else if (useType == USE_RESET)
	{
		// reset switcher
		m_iState = STATE_OFF;
		SetThinkNull();//DontThink();
		m_index = 0;
		return;
	}*/
	else if (m_index != -1) // fire any other USE_TYPE and right index
	{
		/*UTIL_*/FireTargets(STRING(m_iTargetName[m_index]), m_hActivator, this, useType, value );
	}
}
