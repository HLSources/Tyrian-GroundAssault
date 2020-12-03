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
// XDM: This system is obsolete and ineffective.
// There should really be 'reference' and 'instance' for each entity on a map...
// Unfortunately, it does depend on the HL engine and my spare time so much that it is almost impossible to redo.
//

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "decals.h"
#include "gamerules.h"
#include "teamplay_gamerules.h"
#include "game.h"
#include "skill.h"
#include "weapons.h"
#include "globals.h"

// XDM: all exports are now in h_export.cpp

//-----------------------------------------------------------------------------
// Purpose: Checks current game rules to match entity's m_iszGameRulesPolicy
// Input  : *pEntity - allowed string is "!* !1 m !6", everything is optional
// ! - NOT operator (disallow following value).
// * - mask for all. Example: "* !5" - allow everywhere except GT_CTF
// m - mask for multiplayer. Example: "!* m" or "m !0" - only in multiplayer
// All numbers (IDs) must be separated by space (acceptable by isspace())
//
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool VerifyGameRulesPolicy(CBaseEntity *pEntity)
{
	if (FStringNull(pEntity->m_iszGameRulesPolicy))
		return true;

	const char *pPolicyString = STRING(pEntity->m_iszGameRulesPolicy);
	char *pMask = strchr(pPolicyString, '*');//strstr(pPolicyString, "*");

	char PolicyDefault = POLICY_UNDEFINED;// global
	if (pMask)
	{
		// Make sure this is not the first character in policy string
		if ((pMask > pPolicyString) && (*(pMask - 1) == '!'))//char *pNegative = strchr(pPolicyString, '!');// reversed rule
			PolicyDefault = POLICY_DENY;
		else// mask is found and untouched
			PolicyDefault = POLICY_ALLOW;
	}

	char PolicyMP = POLICY_UNDEFINED;// multiplayer
	pMask = strchr(pPolicyString, 'm');
	if (pMask)
	{
		// Make sure this is not the first character in policy string
		if ((pMask > pPolicyString) && (*(pMask - 1) == '!'))// reversed rule
			PolicyMP = POLICY_DENY;
		else
			PolicyMP = POLICY_ALLOW;
	}

	if (g_pGameRules)// may be NULL during transition
	{
		char sGT[4];
		sprintf(sGT, "%d\0", g_pGameRules->GetGameType());
		char *pFound = NULL;
		char *pSearchStart = (char *)pPolicyString;
		do// make sure this is not a part of other decimal number (3 is not part of 13)
		{
			pFound = strstr(pSearchStart, sGT);
			if (pFound)
				pSearchStart = pFound + 1;// can this exceed the string length?
		}
		while (pFound && (pFound > pPolicyString) && isdigit(*(pFound - 1))/*!isspace(*(pFound - 1))*/);// && pSearchStart < end)

		if (pFound)// Highest priority: found current game rules ID
		{
			if ((pFound > pPolicyString) && (*(pFound - 1) == '!'))// reversed rule
				return false;// DENY
			else
				return true;// ALLOW
		}
	}

	// Next priority: multiplayer policy
	if (gpGlobals->deathmatch)// NOT g_pGameRules->IsMultiplayer()
	{
		if (PolicyMP == POLICY_ALLOW)
			return true;
		else if (PolicyMP == POLICY_DENY)
			return false;
		// else undefined
	}

	// Lowest priority: global policy
/*	if (PolicyDefault == POLICY_ALLOW)
		return true;
	else */if (PolicyDefault == POLICY_DENY)
		return false;

	return true;// there is no "completely undefined" case
}


//-----------------------------------------------------------------------------
// Purpose: Engine creates an entity.
// Note   : Not for players
// Input  : *pent - allocated entity
// Output : int -1 deny, 0 allow?
//-----------------------------------------------------------------------------
int DispatchSpawn(edict_t *pent)
{
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pent);
//	ASSERT(pEntity != NULL);
	if (pEntity)
	{
		if (pEntity->pev->scale <= 0.0f)
			pEntity->pev->scale = 1.0f;// XDM3035: engine is fine with 0 but we are not!

		// Initialize these or entities who don't link to the world won't have anything in here
		pEntity->pev->absmin = pEntity->pev->origin - Vector(1,1,1);
		pEntity->pev->absmax = pEntity->pev->origin + Vector(1,1,1);
		pEntity->m_vecSpawnSpot = pEntity->pev->origin;// XDM3035
		pEntity->Spawn(FALSE);

		// Try to get the pointer again, in case the spawn function deleted the entity.
		// UNDONE: Spawn() should really return a code to ask that the entity be deleted, but
		// that would touch too much code for me to do that right now.
		pEntity = (CBaseEntity *)GET_PRIVATE(pent);

		if (pEntity)
		{
//WTF is this?			SAVE_SPAWN_PARMS(pent);

			if (g_pGameRules)
			{
				if (!VerifyGameRulesPolicy(pEntity))
				{
					ALERT(at_aiconsole, "Entity %s (%s) (%s) rejected by its game rules policy.\n", STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname), STRING(pEntity->pev->globalname));
					return -1;
				}

				if (!g_pGameRules->IsAllowedToSpawn(pEntity))
					return -1;	// return that this entity should be deleted
			}
			if (pEntity->pev->flags & FL_KILLME)
				return -1;
			if (g_iSkillLevel == SKILL_EASY && pEntity->m_iSkill & SKF_NOTEASY)
				return -1;
			if (g_iSkillLevel == SKILL_MEDIUM && pEntity->m_iSkill & SKF_NOTMEDIUM)
				return -1;
			if (g_iSkillLevel == SKILL_HARD && pEntity->m_iSkill & SKF_NOTHARD)
				return -1;

			if (pEntity->pev->globalname)// Handle global stuff here
			{
				const globalentity_t *pGlobal = gGlobalState.EntityFromTable( pEntity->pev->globalname );
				if (pGlobal)
				{
					// Already dead? delete
					if ( pGlobal->state == GLOBAL_DEAD )
						return -1;
					else if ( !FStrEq( STRING(gpGlobals->mapname), pGlobal->levelName ) )
						pEntity->MakeDormant();	// Hasn't been moved to this level yet, wait but stay alive
					// In this level & not dead, continue on as normal
				}
				else
				{
					// Spawned entities default to 'On'
					gGlobalState.EntityAdd( pEntity->pev->globalname, gpGlobals->mapname, GLOBAL_ON );
	//				ALERT( at_console, "Added global entity %s (%s)\n", STRING(pEntity->pev->classname), STRING(pEntity->pev->globalname) );
				}
			}
			UTIL_FixRenderColor(pEntity->pev->rendermode, pEntity->pev->rendercolor);// XDM3035b: do it here and not in KeyValue, when rendermode is known 
		}
		return 0;// pent->serialnumber ? entindex() ?
	}
	ALERT(at_error, "ERROR IN DispatchSpawn!!! Invalid pointer!\n");
	DBG_FORCEBREAK
	return -1;
}

// TODO: any value may contain some %macro%
// For example: %cvar sv_gravity% or %random_long 0 10%
//static char g_szKeyValueMacroExpanded[256];

void DispatchKeyValue(edict_t *pentKeyvalue, KeyValueData *pkvd)
{
	if (!pkvd || !pentKeyvalue)
		return;

//	EntvarsKeyvalue(VARS(pentKeyvalue), pkvd);

	// If the key was an entity variable, or there's no class set yet, don't look for the object, it may not exist yet.
	if (pkvd->fHandled || pkvd->szClassName == NULL)
	{
		// This should be valid for classname only!!
//		ASSERT(strcmp(pkvd->szKeyName, "classname") == 0);
		EntvarsKeyvalue(VARS(pentKeyvalue), pkvd);// XDM3035c: now EntvarsKeyvalue() is the last thing to get called by CBaseEntity
		return;
	}

	// Get the actualy entity object
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pentKeyvalue);
	if (pEntity)
	{
/*TODO		static char *pRealValue;
		if (*pkvd->szValue && *pkvd->szValue == '%')
		{
			pRealValue = pkvd->szValue;
			pkvd->szValue = ExpandMacro(pkvd->szValue);
		}*/
		pEntity->KeyValue(pkvd);
//		pkvd->szValue = pRealValue;// always restore original memory pointer!
		if (pkvd->fHandled == FALSE)
			ALERT(at_console, "Entity '%s' has unknown key '%s' (%s)!\n", STRING(pEntity->pev->classname), pkvd->szKeyName, pkvd->szValue);
	}
}


// HACKHACK -- this is a hack to keep the node graph entity from "touching" things (like triggers) while it builds the graph
BOOL gTouchDisabled = FALSE;
void DispatchTouch(edict_t *pentTouched, edict_t *pentOther)
{
	if (gTouchDisabled)
		return;
//	try
//	{

	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pentTouched);
	CBaseEntity *pOther = (CBaseEntity *)GET_PRIVATE(pentOther);

	if (pEntity && pOther)
	{
/*		if (pOther == g_pWorld)
		{
			if ((pEntity->pev->flags & FL_ONGROUND) && Length(pEntity->pev->velocity) > 0.0f)
				pEntity->Bounce(pOther);
		}*/

//		ALERT(at_aiconsole, "DispatchTouch(): %s (%s) (%s): %s\n", STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname), STRING(pEntity->pev->globalname),
//			NAME_FOR_FUNCTION((unsigned long)((void *)*((int *)((char *)pEntity + (offsetof(CBaseEntity,m_pfnTouch)))))));

		if (!((pEntity->pev->flags | pOther->pev->flags) & FL_KILLME))
			pEntity->Touch(pOther);
	}

/*	}
	catch (...)
	{
		DBG_FORCEBREAK
		printf("*** DispatchTouch(%s, %s) exception!\n", STRING(pEntity->pev->classname), STRING(pOther->pev->classname));
	}*/
}

void DispatchUse(edict_t *pentUsed, edict_t *pentOther)
{
//?	UTIL_IsValidEntity(pentUsed);
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pentUsed);
	if (pEntity && !(pEntity->pev->flags & FL_KILLME))
	{
//		ALERT(at_aiconsole, "DispatchUse(): %s (%s) (%s): %s\n", STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname), STRING(pEntity->pev->globalname),
//			NAME_FOR_FUNCTION((unsigned long)((void *)*((int *)((char *)pEntity + (offsetof(CBaseEntity,m_pfnUse)))))));
		CBaseEntity *pOther = (CBaseEntity *)GET_PRIVATE(pentOther);
		pEntity->Use(pOther, pOther, USE_TOGGLE, 0);
	}
}

#include <float.h>

// For all entities except players
void DispatchThink(edict_t *pent)
{
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pent);
//	try
//	{
	if (pEntity)
	{
		if (FBitSet(pEntity->pev->flags, FL_DORMANT))
			ALERT(at_console, "WARNING! Dormant entity %s is thinking!!\n", STRING(pEntity->pev->classname));// XDM: not an ERROR!
//testme
//		if (test2.value > 0)// TEST DEBUG TODO UNDONE
//			ALERT(at_aiconsole, "DispatchThink(): %s (%s) (%s): %s\n", STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname), STRING(pEntity->pev->globalname),
//			NAME_FOR_FUNCTION((unsigned long)((void *)*((int *)((char *)pEntity + (offsetof(CBaseEntity,m_pfnThink)))))));
//short		ALERT(at_console, "DispatchThink(): %s (%s)\n", STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname));
/*			if (_isnan(pEntity->pev->angles.y))
			{
				printf("*** DispatchThink(%s) NaN!\n", STRING(pEntity->pev->classname));
				DBG_FORCEBREAK
			}*/
			pEntity->CheckEnvironment();// XDM: affects everyone!
//		try
//		{
			pEntity->Think();
/*		}
		catch (...)
		{
			printf("*** DispatchThink(%s) exception Think()!\n", STRING(pEntity->pev->classname));
			DBG_FORCEBREAK
		}*/
	}
/*	}
	catch (...)
	{
		printf("*** DispatchThink(%s) exception!\n", STRING(pEntity->pev->classname));
		DBG_FORCEBREAK
	}*/
}

void DispatchBlocked(edict_t *pentBlocked, edict_t *pentOther)
{
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pentBlocked);
	if (pEntity)
	{
		CBaseEntity *pOther = (CBaseEntity *)GET_PRIVATE(pentOther);
		pEntity->Blocked(pOther);
	}
}

void DispatchSave(edict_t *pent, SAVERESTOREDATA *pSaveData)
{
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pent);
	
	if (pEntity && pSaveData)
	{
		ENTITYTABLE *pTable = &pSaveData->pTable[ pSaveData->currentIndex ];

		if (pTable->pent != pent)
			ALERT(at_error, "DispatchSave: ENTITY TABLE OR INDEX IS WRONG!!!!\n");

		if (pEntity->ObjectCaps() & FCAP_DONT_SAVE)
			return;

		// These don't use ltime & nextthink as times really, but we'll fudge around it.
		if ( pEntity->pev->movetype == MOVETYPE_PUSH )
		{
			float delta = pEntity->pev->nextthink - pEntity->pev->ltime;
			pEntity->pev->ltime = gpGlobals->time;
			pEntity->pev->nextthink = pEntity->pev->ltime + delta;
		}

		pTable->location = pSaveData->size;		// Remember entity position for file I/O
		pTable->classname = pEntity->pev->classname;	// Remember entity class for respawn

		CSave saveHelper( pSaveData );
		pEntity->Save( saveHelper );

		pTable->size = pSaveData->size - pTable->location;	// Size of entity block is data size written to block
	}
}


// Find the matching global entity.  Spit out an error if the designer made entities of
// different classes with the same global name
CBaseEntity *FindGlobalEntity(string_t classname, string_t globalname)
{
	CBaseEntity *pReturn = UTIL_FindEntityByString(NULL, "globalname", STRING(globalname));
	if (pReturn)
	{
		if (!FClassnameIs(pReturn->pev, STRING(classname)))
		{
			ALERT( at_console, "Global entity found %s, wrong class %s\n", STRING(globalname), STRING(pReturn->pev->classname) );
			pReturn = NULL;
		}
	}
	return pReturn;
}


int DispatchRestore(edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity)
{
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pent);

	if (pEntity && pSaveData)
	{
		entvars_t tmpVars;
		Vector oldOffset;
		CRestore restoreHelper( pSaveData );
		if ( globalEntity )
		{
			CRestore tmpRestore( pSaveData );
			tmpRestore.PrecacheMode( 0 );
			tmpRestore.ReadEntVars( "ENTVARS", &tmpVars );

			// HACKHACK - reset the save pointers, we're going to restore for real this time
			pSaveData->size = pSaveData->pTable[pSaveData->currentIndex].location;
			pSaveData->pCurrentData = pSaveData->pBaseData + pSaveData->size;
			// -------------------

			const globalentity_t *pGlobal = gGlobalState.EntityFromTable( tmpVars.globalname );
			// Don't overlay any instance of the global that isn't the latest
			// pSaveData->szCurrentMapName is the level this entity is coming from
			// pGlobla->levelName is the last level the global entity was active in.
			// If they aren't the same, then this global update is out of date.
			if ( !FStrEq( pSaveData->szCurrentMapName, pGlobal->levelName ) )
				return 0;

			// Compute the new global offset
			oldOffset = pSaveData->vecLandmarkOffset;
			CBaseEntity *pNewEntity = FindGlobalEntity( tmpVars.classname, tmpVars.globalname );
			if ( pNewEntity )
			{
//				ALERT( at_console, "Overlay %s with %s\n", STRING(pNewEntity->pev->classname), STRING(tmpVars.classname) );
				// Tell the restore code we're overlaying a global entity from another level
				restoreHelper.SetGlobalMode( 1 );	// Don't overwrite global fields
				pSaveData->vecLandmarkOffset = (pSaveData->vecLandmarkOffset - pNewEntity->pev->mins) + tmpVars.mins;
				pEntity = pNewEntity;// we're going to restore this data OVER the old entity
				pent = ENT( pEntity->pev );
				// Update the global table to say that the global definition of this entity should come from this level
				gGlobalState.EntityUpdate( pEntity->pev->globalname, gpGlobals->mapname );
			}
			else
			{
				// This entity will be freed automatically by the engine.  If we don't do a restore on a matching entity (below)
				// or call EntityUpdate() to move it to this level, we haven't changed global state at all.
				return 0;
			}
		}

		if ( pEntity->ObjectCaps() & FCAP_MUST_SPAWN )
		{
			pEntity->Restore( restoreHelper );
			pEntity->Spawn(TRUE);
		}
		else
		{
			pEntity->Restore( restoreHelper );
			pEntity->Precache();
		}

		// Again, could be deleted, get the pointer again.
		pEntity = (CBaseEntity *)GET_PRIVATE(pent);
#if 0
		if ( pEntity && pEntity->pev->globalname && globalEntity ) 
		{
			ALERT( at_console, "Global %s is %s\n", STRING(pEntity->pev->globalname), STRING(pEntity->pev->model) );
		}
#endif
		// Is this an overriding global entity (coming over the transition), or one restoring in a level
		if ( globalEntity )
		{
//			ALERT( at_console, "After: %f %f %f %s\n", pEntity->pev->origin.x, pEntity->pev->origin.y, pEntity->pev->origin.z, STRING(pEntity->pev->model) );
			pSaveData->vecLandmarkOffset = oldOffset;
			if ( pEntity )
			{
				UTIL_SetOrigin( pEntity->pev, pEntity->pev->origin );
				pEntity->OverrideReset();
			}
		}
		else if ( pEntity && pEntity->pev->globalname ) 
		{
			const globalentity_t *pGlobal = gGlobalState.EntityFromTable( pEntity->pev->globalname );
			if ( pGlobal )
			{
				// Already dead? delete
				if ( pGlobal->state == GLOBAL_DEAD )
					return -1;
				else if ( !FStrEq( STRING(gpGlobals->mapname), pGlobal->levelName ) )
				{
					pEntity->MakeDormant();	// Hasn't been moved to this level yet, wait but stay alive
				}
				// In this level & not dead, continue on as normal
			}
			else
			{
				ALERT( at_error, "Global Entity %s (%s) not in table!!!\n", STRING(pEntity->pev->globalname), STRING(pEntity->pev->classname) );
				// Spawned entities default to 'On'
				gGlobalState.EntityAdd( pEntity->pev->globalname, gpGlobals->mapname, GLOBAL_ON );
			}
		}
	}
	return 0;
}


void DispatchObjectCollisionBox(edict_t *pent)
{
	CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pent);
	if (pEntity)
	{
		pEntity->SetObjectCollisionBox();
	}
	else
		SetObjectCollisionBox(&pent->v);
}


void SaveWriteFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount )
{
	CSave saveHelper( pSaveData );
	saveHelper.WriteFields( pname, pBaseData, pFields, fieldCount );
}


void SaveReadFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount )
{
	CRestore restoreHelper( pSaveData );
	restoreHelper.ReadFields( pname, pBaseData, pFields, fieldCount );
}




EHANDLE::EHANDLE(void)// XDM3035
{
	m_pent = NULL;
	m_serialnumber = 0;
}

edict_t *EHANDLE::Get(void)
{
//	try
//	{
	if (m_pent)
	{
		if (m_pent->serialnumber == m_serialnumber) 
			return m_pent; 
		else
			return NULL;
	}
/*	}
	catch(...)
	{
		fprintf(stderr, " *** EHANDLE::Get() exception! ***\n");
	}*/
	return NULL; 
}

edict_t *EHANDLE::Set(edict_t *pent)
{ 
	m_pent = pent;  
	if (m_pent)
		m_serialnumber = m_pent->serialnumber; 
	else
		m_serialnumber = 0;

	return m_pent; 
}

EHANDLE::operator CBaseEntity *()
{ 
	return (CBaseEntity *)GET_PRIVATE(Get());
}

CBaseEntity *EHANDLE::operator = (CBaseEntity *pEntity)
{
	if (pEntity)
	{
		m_pent = ENT(pEntity->pev);
		if (m_pent)
			m_serialnumber = m_pent->serialnumber;
	}
	else
	{
		m_pent = NULL;
		m_serialnumber = 0;
	}
	return pEntity;
}

EHANDLE::operator int()
{
	return Get() != NULL;
}

CBaseEntity *EHANDLE::operator ->()
{
	return (CBaseEntity *)GET_PRIVATE(Get());
}




// Global Savedata for Delay
TYPEDESCRIPTION	CBaseEntity::m_SaveData[] = 
{
	DEFINE_FIELD(CBaseEntity, m_pGoalEnt, FIELD_CLASSPTR),
//NO	DEFINE_FIELD(CBaseEntity, m_pLink, FIELD_CLASSPTR),
#if defined(MOVEWITH)
	DEFINE_FIELD(CBaseEntity, m_iszMoveWith, FIELD_STRING),
#endif
	DEFINE_FIELD(CBaseEntity, m_pfnThink, FIELD_FUNCTION),		// UNDONE: Build table of these!!!
	DEFINE_FIELD(CBaseEntity, m_pfnTouch, FIELD_FUNCTION),
	DEFINE_FIELD(CBaseEntity, m_pfnUse, FIELD_FUNCTION),
	DEFINE_FIELD(CBaseEntity, m_pfnBlocked, FIELD_FUNCTION),
	DEFINE_FIELD(CBaseEntity, m_iSkill, FIELD_INTEGER),// XDM
	DEFINE_FIELD(CBaseEntity, m_iszGameRulesPolicy, FIELD_STRING),
	DEFINE_FIELD(CBaseEntity, m_iszIcon, FIELD_STRING),
	DEFINE_FIELD(CBaseEntity, m_flBurnTime, FIELD_TIME),
//	DEFINE_FIELD(CBaseEntity, m_iszMoveTargetName, FIELD_STRING),
//	DEFINE_FIELD(CBaseEntity, m_vecMoveOriginDelta, FIELD_VECTOR),
	DEFINE_FIELD(CBaseEntity, m_vecSpawnSpot, FIELD_POSITION_VECTOR),
};

// XDM3035c: the only way to pre-set variables before KeyValue() takes place
CBaseEntity::CBaseEntity()
{
	pev = NULL;
	m_pGoalEnt = NULL;
	m_pLink = NULL;
#if defined(MOVEWITH)
	m_pMoveWith = NULL;
	m_pChildMoveWith = NULL;
	m_pAssistLink = NULL;
	m_iszMoveWith = iStringNull;
#endif
}

/*CBaseEntity::~CBaseEntity()
{
}*/

//-----------------------------------------------------------------------------
// Purpose: Called before spawn() for each KV pair set for ent in world editor
// Input  : *pkvd - 
//-----------------------------------------------------------------------------
void CBaseEntity::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "rendercolor"))
	{
		if (isalpha(*pkvd->szValue))
		{
			if (FStrnEq(pkvd->szValue, "teamcolor", 9) && isdigit(pkvd->szValue[9]))
			{
				byte r,g,b;
				GetTeamColor(atoi(pkvd->szValue + 9), r,g,b);
				pev->rendercolor.x = r;
				pev->rendercolor.y = g;
				pev->rendercolor.z = b;
				pkvd->fHandled = TRUE;
			}
		}
		else if (StringToVec(pkvd->szValue, pev->rendercolor))
		{
			pkvd->fHandled = TRUE;
		}

		if (pkvd->fHandled == FALSE)// XDM3036: oops!
		{
			ALERT(at_console, "WARNING: fixing bad %s in entity %s: \"%s\"\n", pkvd->szKeyName, pkvd->szClassName, pkvd->szValue);
			UTIL_FixRenderColor(pev->rendermode, pev->rendercolor);
			pkvd->fHandled = TRUE;// mark as known
		}
	}
	else if (FStrnEq(pkvd->szKeyName, "_minlight", 9))// Hammer/Zoner's Compile Tools
		pkvd->fHandled = TRUE;
	else if (FStrnEq(pkvd->szKeyName, "_light", 6))
		pkvd->fHandled = TRUE;
	else if (FStrnEq(pkvd->szKeyName, "_fade", 5))
		pkvd->fHandled = TRUE;
	else if (FStrEq(pkvd->szKeyName, "style"))
		pkvd->fHandled = TRUE;
	else if (FStrEq(pkvd->szKeyName, "delay"))
		pkvd->fHandled = TRUE;
#if defined(MOVEWITH)
	else if (FStrEq(pkvd->szKeyName, "movewith"))
	{
//		m_pMoveWith = UTIL_FindEntityByTargetname(this, pkvd->szValue);// NO! All ents must be spawned first!
		m_iszMoveWith = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "lflags"))
	{
		m_iLFlags = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
#endif // MOVEWITH
	else if (FStrEq(pkvd->szKeyName, "skill"))
	{
		m_iSkill = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "gamerules"))
	{
		m_iszGameRulesPolicy = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "origin"))// entvars set in DispatchKeyValue()
	{
		if (StringToVec(pkvd->szValue, pev->origin))
		{
			UTIL_SetOrigin(pev, pev->origin);
			pkvd->fHandled = TRUE;
		}
		else
			pkvd->fHandled = FALSE;
	}
	else if (FStrEq(pkvd->szKeyName, "angles"))
	{
		if (StringToVec(pkvd->szValue, pev->angles))
			pkvd->fHandled = TRUE;
		else
			pkvd->fHandled = FALSE;
	}
	else if (FStrEq(pkvd->szKeyName, "drawalways"))
	{
		if (atoi(pkvd->szValue) > 0)
		{
			pev->flags |= FL_DRAW_ALWAYS;
			pkvd->fHandled = TRUE;
		}
		else
			pkvd->fHandled = FALSE;
	}
	else if (FStrEq(pkvd->szKeyName, "icon"))// XDM3035c
	{
		m_iszIcon = ALLOC_STRING(pkvd->szValue);
//		if (g_ServerActive)//IsRunTime())
//hang			pev->iuser4 = MODEL_INDEX(STRINGV(m_iszIcon));

		pkvd->fHandled = TRUE;
	}
//	else if (FStrEq(pkvd->szKeyName, "{"))// XDM3036 TEST
//	{
//	}
	else
	{
		EntvarsKeyvalue(pev, pkvd);
		// no alerts here
//		pkvd->fHandled = FALSE;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Entity is being created (simple version)
//-----------------------------------------------------------------------------
void CBaseEntity::Spawn(void)
{
	Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Entity is being created (restore-aware version)
// Input  : restore - 1 if spawning after Restore (loading saved game) FCAP_MUST_SPAWN
//-----------------------------------------------------------------------------
void CBaseEntity::Spawn(byte restore)
{
	Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache resources. Called either from Spawn() or directly.
//-----------------------------------------------------------------------------
void CBaseEntity::Precache(void)
{
//	if (!FStringNull(m_iszIcon))
//hang		pev->iuser4 = PRECACHE_MODEL(STRINGV(m_iszIcon));// XDM3035c
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &save - 
// Output : int
//-----------------------------------------------------------------------------
int CBaseEntity::Save(CSave &save)
{
	if (save.WriteEntVars("ENTVARS", pev))
		return save.WriteFields("BASE", this, m_SaveData, ARRAYSIZE(m_SaveData));

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Restore class data after loading, called before Spawn(TRUE)
// Input  : &restore - 
// Output : int
//-----------------------------------------------------------------------------
int CBaseEntity::Restore(CRestore &restore)
{
	int status = restore.ReadEntVars("ENTVARS", pev);
	if (status)
		status = restore.ReadFields("BASE", this, m_SaveData, ARRAYSIZE(m_SaveData));

    if (pev->modelindex != 0 && !FStringNull(pev->model))
	{
		Vector mins, maxs;
		mins = pev->mins;	// Set model is about to destroy these
		maxs = pev->maxs;
		PRECACHE_MODEL(STRINGV(pev->model));
		SET_MODEL(ENT(pev), STRING(pev->model));
		UTIL_SetSize(pev, mins, maxs);	// Reset them
	}
	return status;
}

//-----------------------------------------------------------------------------
// Purpose: XDM3035: called before deleting[] this entity... probably
// MAY be more reliable than UpdateOnRemove()?
// Warning: ONLY available with NEW_DLL_FUNCTIONS
//-----------------------------------------------------------------------------
void CBaseEntity::OnFreePrivateData(void)
{
	edict_t *pClientEd = NULL;
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		pClientEd = UTIL_ClientEdictByIndex(i);
		if (pClientEd && pClientEd->pvPrivateData && pClientEd->free == 0 && !FStringNull(pClientEd->v.netname))// engine may be shutting down or the client might got disconnected
		{
			if (pClientEd->v.euser2 == edict())// XDM3035c: if a player was watching through this entity
			{
				UTIL_SetView(pClientEd, pClientEd);
				pClientEd->v.euser2 = NULL;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: XDM3037: entity is about to be transferred to another map
// Clear links and other sensitive data pointers here
//-----------------------------------------------------------------------------
void CBaseEntity::PrepareForTransfer(void)
{
#if defined(MOVEWITH)
	m_pMoveWith = NULL;// XDM-only: TESTME!
	m_pChildMoveWith = NULL;
	m_pSiblingMoveWith = NULL;
	m_pAssistLink = NULL;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Entity capabilities flags. XDM: global safety
// Output : int
//-----------------------------------------------------------------------------
int CBaseEntity::ObjectCaps(void)
{
	if (IsBSPModel())
		return 0;

	return FCAP_ACROSS_TRANSITION;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
// Warning: ONLY available with NEW_DLL_FUNCTIONS
//-----------------------------------------------------------------------------
int CBaseEntity::ShouldCollide(CBaseEntity *pOther)
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Initialize absmin & absmax to the appropriate box
// Input  : *pev - 
//-----------------------------------------------------------------------------
void SetObjectCollisionBox(entvars_t *pev)
{
	if ((pev->solid == SOLID_BSP) && (pev->angles.x || pev->angles.y|| pev->angles.z))
	{	// expand for rotation
		float		max, v;
		int			i;

		max = 0;
		for (i=0 ; i<3 ; i++)
		{
			v = fabs(((float *)pev->mins)[i]);
			if (v > max)
				max = v;
			v = fabs(((float *)pev->maxs)[i]);
			if (v > max)
				max = v;
		}
		for (i=0 ; i<3 ; i++)
		{
			((float *)pev->absmin)[i] = ((float *)pev->origin)[i] - max;
			((float *)pev->absmax)[i] = ((float *)pev->origin)[i] + max;
		}
	}
	else
	{
		pev->absmin = pev->origin + pev->mins;
		pev->absmax = pev->origin + pev->maxs;
	}

	pev->absmin.x -= 1;
	pev->absmin.y -= 1;
	pev->absmin.z -= 1;
	pev->absmax.x += 1;
	pev->absmax.y += 1;
	pev->absmax.z += 1;
}

void CBaseEntity::SetObjectCollisionBox(void)
{
	::SetObjectCollisionBox(pev);
}

int	CBaseEntity::Intersects(CBaseEntity *pOther)
{
	if (pOther->pev->absmin.x > pev->absmax.x ||
		pOther->pev->absmin.y > pev->absmax.y ||
		pOther->pev->absmin.z > pev->absmax.z ||
		pOther->pev->absmax.x < pev->absmin.x ||
		pOther->pev->absmax.y < pev->absmin.y ||
		pOther->pev->absmax.z < pev->absmin.z )
		return 0;
	return 1;
}

void CBaseEntity::MakeDormant(void)
{
	SetBits(pev->flags, FL_DORMANT);
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	SetBits(pev->effects, EF_NODRAW);
	pev->nextthink = 0;
	UTIL_SetOrigin(pev, pev->origin);
}

BOOL CBaseEntity::IsDormant(void)
{
	return FBitSet(pev->flags, FL_DORMANT);
}

BOOL CBaseEntity::IsInWorld(void)
{
	// position 
	if (pev->origin.x > MAX_ABS_ORIGIN) return FALSE;// XDM
	if (pev->origin.y > MAX_ABS_ORIGIN) return FALSE;
	if (pev->origin.z > MAX_ABS_ORIGIN) return FALSE;
	if (pev->origin.x < -MAX_ABS_ORIGIN) return FALSE;
	if (pev->origin.y < -MAX_ABS_ORIGIN) return FALSE;
	if (pev->origin.z < -MAX_ABS_ORIGIN) return FALSE;
/*
	// speed
	if (pev->velocity.x > MAX_ABS_VELOCITY) return FALSE;
	if (pev->velocity.y > MAX_ABS_VELOCITY) return FALSE;
	if (pev->velocity.z > MAX_ABS_VELOCITY) return FALSE;
	if (pev->velocity.x < -MAX_ABS_VELOCITY) return FALSE;
	if (pev->velocity.y < -MAX_ABS_VELOCITY) return FALSE;
	if (pev->velocity.z < -MAX_ABS_VELOCITY) return FALSE;*/
	return TRUE;
}

BOOL CBaseEntity::IsPlayer(void)
{
	if (pev && (pev->flags & FL_CLIENT))
		return TRUE;

	return FALSE;
}

int CBaseEntity::ShouldToggle(USE_TYPE useType, bool currentState)
{
	if (useType != USE_TOGGLE && useType != USE_SET)
	{
		if ((currentState && useType == USE_ON) || (!currentState && useType == USE_OFF))
			return 0;
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Which decal should be drawn on this entity for specified damage type
// Input  : &bitsDamageType - 
// Output : server decal index to draw
//-----------------------------------------------------------------------------
int	CBaseEntity::DamageDecal(const int &bitsDamageType)
{
	if (pev->rendermode == kRenderTransAlpha)
		return -1;

	if (pev->rendermode != kRenderNormal)
		return DECAL_BPROOF1;

	return DECAL_GUNSHOT1 + RANDOM_LONG(0,4);
}

//-----------------------------------------------------------------------------
// Purpose: Simplified version
// Input  : *szName - must be a pointer to constant memory, e.g. "monster_class" because the entity will keep a pointer to it after this call.
// Output : CBaseEntity *
//-----------------------------------------------------------------------------
CBaseEntity *CBaseEntity::Create(const char *szName, const Vector &vecOrigin, const Vector &vecAngles, edict_t *pentOwner)
{
	return Create(szName, vecOrigin, vecAngles, g_vecZero, pentOwner);
}

//-----------------------------------------------------------------------------
// Purpose: Simplified version
// Input  : *szName - must be a pointer to constant memory, e.g. "monster_class" because the entity will keep a pointer to it after this call.
//			&vecVelocity - 
// Output : CBaseEntity *
//-----------------------------------------------------------------------------
CBaseEntity *CBaseEntity::Create(const char *szName, const Vector &vecOrigin, const Vector &vecAngles, const Vector &vecVelocity, edict_t *pentOwner)
{
	if (szName)
		return Create(MAKE_STRING(szName), vecOrigin, vecAngles, vecVelocity, pentOwner);

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: The one and only way to create an entity! Spawn() will be called.
// Input  : iName - existing string_t
//			&vecOrigin - 
//			&vecAngles - 
//			&vecVelocity - 
//			*pentOwner - 
//			spawnflags - 
// Output : CBaseEntity *
//-----------------------------------------------------------------------------
CBaseEntity *CBaseEntity::Create(int iName, const Vector &vecOrigin, const Vector &vecAngles, const Vector &vecVelocity, edict_t *pentOwner, int spawnflags)
{
	if (iName == 0)
		return NULL;

	edict_t	*pent = NULL;
	try
	{
		pent = CREATE_NAMED_ENTITY(iName);
	}
	catch(...)
	{
		fprintf(stderr, " *** CREATE_NAMED_ENTITY exception! ***\n");
		return NULL;
	}

	if (FNullEnt(pent))
	{
		ALERT(at_console, "CBaseEntity::Create: Unable to create entity '%s'!\n", STRING(iName));
		return NULL;
	}

	CBaseEntity *pEntity = Instance(pent);
	if (pEntity)
	{
		pEntity->pev->owner = pentOwner;
		pEntity->pev->origin = vecOrigin;
		pEntity->pev->angles = vecAngles;
		pEntity->pev->velocity = vecVelocity;
		pEntity->pev->spawnflags = spawnflags;
		DispatchSpawn(pent);
#ifdef _DEBUG
		if (pEntity->entindex() >= 2048)// MAX_EDICTS is purely theoretical :(
		{
			SERVER_PRINT("WARNING! ENTITY INDEX >= 2048!\n");
			ALERT(at_console, "WARNING: CBaseEntity::Create(%s): index >= 2048!\n", STRING(iName));
		}
#endif
	}
	return pEntity;
}

//-----------------------------------------------------------------------------
// Purpose: A safe way to duplicate an entity! Use this!
// Input  : iName - existing string_t
//			*pSourceVars - pev to copy
//			spawnflags - will be set prior to spawning
//			spawn - Spawn() will be called if true
// Output : CBaseEntity *
//-----------------------------------------------------------------------------
CBaseEntity *CBaseEntity::CreateCopy(int iName, entvars_t *pSourceVars, int spawnflags, bool spawn)
{
	if (iName == 0 || pSourceVars == NULL)
		return NULL;

	edict_t	*pent = NULL;
	try
	{
		pent = CREATE_NAMED_ENTITY(iName);
	}
	catch(...)
	{
		fprintf(stderr, " *** CREATE_NAMED_ENTITY exception! ***\n");
		return NULL;
	}

	if (FNullEnt(pent))
	{
		ALERT(at_console, "CBaseEntity::CreateCopy: Unable to create entity '%s'!\n", STRING(iName));
		return NULL;
	}

	CBaseEntity *pEntity = Instance(pent);
	if (pEntity)// && pSourceVars)
	{
		string_t classname = pEntity->pev->classname;
		string_t globalname = pEntity->pev->globalname;
		edict_t *pNewContainingEntity = pEntity->pev->pContainingEntity;
		memcpy(pEntity->pev, pSourceVars, sizeof(entvars_t));
		pEntity->pev->classname = classname;
		pEntity->pev->globalname = globalname;
		pEntity->pev->pContainingEntity = pNewContainingEntity;
		pEntity->pev->spawnflags = spawnflags;// allow override
		if (spawn)
			DispatchSpawn(pEntity->edict());
	}
	return pEntity;
}

//-----------------------------------------------------------------------------
// Purpose: Add some sanity to this (c/t)rash code. Get CBaseEntity of edict_t
// Input  : *pent - 
// Output : CBaseEntity
//-----------------------------------------------------------------------------
CBaseEntity *CBaseEntity::Instance(edict_t *pent)
{
	ASSERTSZ(pent != NULL, "CBaseEntity::Instance(NULL)!\n");
//	if (pent == NULL)// XDM3035c: TESTME! DANGER!
//		pent = ENT(0);// WTF?!! This is the world!
	return (CBaseEntity *)GET_PRIVATE(pent);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBaseEntity::DontThink(void)
{
#if defined(MOVEWITH)
	m_fNextThink = 0;
	if (m_pMoveWith == NULL && m_pChildMoveWith == NULL)
	{
		pev->nextthink = 0;
		m_fPevNextThink = 0;
	}
#else
	pev->nextthink = 0;
#endif
//	ALERT(at_console, "DontThink for %s\n", STRING(pev->targetname));
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &delay - 
//-----------------------------------------------------------------------------
void CBaseEntity::SetNextThink(const float &delay)
{
	pev->nextthink = gpGlobals->time + delay;
}

//-----------------------------------------------------------------------------
// Purpose: Set size from bbmin/bbmax of a model's sequence
//-----------------------------------------------------------------------------
void CBaseEntity::SetModelCollisionBox(void)
{
	studiohdr_t *pstudiohdr = (studiohdr_t *)GET_MODEL_PTR(ENT(pev));
	if (pstudiohdr == NULL)
	{
		ALERT(at_console,"Entity %s was unable to fetch model pointer (%s)!\n", STRING(pev->classname), STRING(pev->model));
		UTIL_SetSize(pev, pev->mins, pev->maxs);
	}
	else
	{
		mstudioseqdesc_t *pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);
		UTIL_SetSize(pev, pseqdesc[pev->sequence].bbmin, pseqdesc[pev->sequence].bbmax);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Align entity to underlying surface (sets pev->angles)
//-----------------------------------------------------------------------------
void CBaseEntity::AlignToFloor(void)
{
	TraceResult tr;
//	UTIL_TraceLine(pev->origin, pev->origin - Vector(0,0,pev->size.z), ignore_monsters, ENT(pev), &tr);
	UTIL_TraceLine(pev->origin, pev->origin - gpGlobals->v_up*(pev->size.z), ignore_monsters, ENT(pev), &tr);
	if (tr.flFraction < 1.0f)
	{
//		ALERT(at_console, "AlignToFloor: tr.flFraction < 1.0\n");
		Vector n = UTIL_VecToAngles(tr.vecPlaneNormal);
//		n.x += 270.0f;
//		pev->angles = n;
		pev->angles.x = n.x + 270.0f;// XDM: ignore rotation .y (against vertical axis)
		pev->angles.z = n.z;
//		UTIL_SetOrigin(pev, pev->origin);// should not be changed
	}
}

//-----------------------------------------------------------------------------
// Purpose: Check if in lava/slime/other stuff and take desired actions
//-----------------------------------------------------------------------------
void CBaseEntity::CheckEnvironment(void)
{
//	ALERT(at_console, "CBaseEntity::CheckEnvironment() %s\n", STRING(pev->classname));
	if (pev->takedamage != DAMAGE_NO && (pev->waterlevel > 0))
	{
		if (pev->radsuit_finished < gpGlobals->time)// HACK: this variable is not used anyway
		{
			if (/*!FBitSet(pev->flags, FL_IMMUNE_LAVA) && */pev->watertype == CONTENTS_LAVA)		// do damage
			{
				TakeDamage(g_pWorld, g_pWorld, 10 * pev->waterlevel, DMG_BURN | DMG_SLOWBURN);
			}
			else if (!FBitSet(pev->flags, FL_IMMUNE_SLIME) && pev->watertype == CONTENTS_SLIME)		// do damage
			{
				TakeDamage(g_pWorld, g_pWorld, 4 * pev->waterlevel, DMG_ACID);
			}
			pev->radsuit_finished = gpGlobals->time + 0.5f;// don't check too fast
//			ALERT(at_console, "CBaseEntity::CheckEnvironment() 2\n");
		}
	}
}


//=========================================================
// FVisible - returns true if a line can be traced from
// the caller's eyes to the target
//=========================================================
bool CBaseEntity::FVisible(CBaseEntity *pEntity)
{
	if (FBitSet(pEntity->pev->flags, FL_NOTARGET))
		return false;

	// don't look through water
	if ((pev->waterlevel != 3 && pEntity->pev->waterlevel == 3) || (pev->waterlevel == 3 && pEntity->pev->waterlevel == 0))
		return false;

	TraceResult tr;
	UTIL_TraceLine(EyePosition(), pEntity->EyePosition(), ignore_monsters, ignore_glass, ENT(pev)/*pentIgnore*/, &tr);

	if (tr.flFraction != 1.0f)
		return false;// Line of sight is not established

	return true;// line of sight is valid.
}

//=========================================================
// FVisible - returns true if a line can be traced from
// the caller's eyes to the target vector
//=========================================================
bool CBaseEntity::FVisible(const Vector &vecOrigin)
{
	if (pev->waterlevel != 3 && (POINT_CONTENTS(vecOrigin) <= CONTENTS_WATER))// XDM3035c: don't look through water
		return false;

	TraceResult tr;
	UTIL_TraceLine(EyePosition(), vecOrigin, ignore_monsters, ignore_glass, ENT(pev)/*pentIgnore*/, &tr);

	if (tr.flFraction != 1.0f)
		return false;// Line of sight is not established

	return true;// line of sight is valid.
}

//=========================================================
// FBoxVisible - a more accurate ( and slower ) version
// of FVisible.
// vecTargetOrigin is output
//=========================================================
bool CBaseEntity::FBoxVisible(CBaseEntity *pTarget, Vector &vecTargetOrigin, float flSize)
{
	// don't look through water
	if ((pev->waterlevel != 3 && pTarget->pev->waterlevel == 3) || (pev->waterlevel == 3 && pTarget->pev->waterlevel == 0))
		return false;

	TraceResult tr;
	for (int i = 0; i < 5; ++i)
	{
		Vector vecTarget = pTarget->pev->origin;
		vecTarget.x += RANDOM_FLOAT(pTarget->pev->mins.x + flSize, pTarget->pev->maxs.x - flSize);
		vecTarget.y += RANDOM_FLOAT(pTarget->pev->mins.y + flSize, pTarget->pev->maxs.y - flSize);
		vecTarget.z += RANDOM_FLOAT(pTarget->pev->mins.z + flSize, pTarget->pev->maxs.z - flSize);

		UTIL_TraceLine(EyePosition(), vecTarget, ignore_monsters, ignore_glass, ENT(pev)/*pentIgnore*/, &tr);

		if (tr.flFraction == 1.0f)
		{
			vecTargetOrigin = vecTarget;
			return true;// line of sight is valid.
		}
	}
	return false;// Line of sight is not established
}

// XDM3035: now not for monsters only
float CBaseEntity::DamageForce(const float &damage)
{
	if (!IsPushable())
		return 0.0f;

	if (pev->movetype == MOVETYPE_NONE)
		return 0.0f;

	//HULL_RADIUS*HULL_RADIUS*4 * (HULL_MAX - HULL_MIN)
//	float force = damage * ((32 * 32 * 72.0) / (pev->size.x * pev->size.y * pev->size.z)) * 5.0f;
	float force = 2.0f * damage * (73728.0f / (pev->size.x * pev->size.y * pev->size.z));// XDM3035c: tuned

	if (force > 2048.0f)
		force = 2048.0f;

	return force;
}

void CBaseEntity::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (pev->takedamage)
	{
		AddMultiDamage(pAttacker, this, flDamage, bitsDamageType);
		int blood = BloodColor();
		if (blood != DONT_BLEED && !((bitsDamageType & DMG_DONT_BLEED) || (bitsDamageType & DMG_NEVERGIB)))// XDM
		{
			Vector vecOrigin = ptr->vecEndPos - vecDir * 4.0f;
			UTIL_BloodDrips(vecOrigin, vecDir, blood, flDamage);// a little surface blood.
			TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
		}
	}
}

// give health
int CBaseEntity::TakeHealth(const float &flHealth, const int &bitsDamageType)
{
	if (!pev->takedamage)
		return 0;

	if (pev->health >= pev->max_health)
		return 0;

	pev->health += flHealth;

	float added;// the health points actually added

	if (pev->health > pev->max_health)
	{
		added = flHealth - (pev->health - pev->max_health);
		pev->health = pev->max_health;
	}
	else
		added = flHealth;

	return (int)added;
}

// inflict damage on this entity.  bitsDamageType indicates type of damage inflicted, ie: DMG_CRUSH
int CBaseEntity::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pev->takedamage == DAMAGE_NO)
		return 0;

	// UNDONE: some entity types may be immune or resistant to some bitsDamageType
/*	int bitsDamage = (bitsDamageType & ~m_bitsDamageImmune)// only those bits must remain which are not in m_bitsDamageImmune
	if (bitsDamage == 0)// no suitable damage type
		return 0;*/

	Vector vecTemp;
	// if Attacker == Inflictor, the attack was a melee or other instant-hit attack.
	// (that is, no actual entity projectile was involved in the attack so use the shooter's origin). 
	if (pInflictor)// an actual missile was involved.
	{
		vecTemp = pInflictor->pev->origin - (VecBModelOrigin(pev));
	}
	else if (pAttacker)
	{
		vecTemp = pAttacker->Center() - (VecBModelOrigin(pev));
	}

// this global is still used for glass and other non-monster killables, along with decals.
	g_vecAttackDir = vecTemp.Normalize();
		
// save damage based on the target's armor level

// figure momentum add (don't let hurt brushes or other triggers move player)
	if (pInflictor && pAttacker && (pAttacker->pev->solid != SOLID_TRIGGER) && IsPushable())
	{
		Vector vecDir = (pev->origin - pInflictor->Center()).Normalize();
//		float v = (VEC_HULL_MAX - VEC_HULL_MIN).Volume();// (32 * 32 * 72.0f)
		float flForce = flDamage * ((32 * 32 * 72.0f) / (pev->size.x * pev->size.y * pev->size.z)) * 5.0f;
		if (flForce > 1000.0f) 
			flForce = 1000.0f;
		pev->velocity = pev->velocity + vecDir * flForce;
	}

	// do the damage
	pev->health -= flDamage;
	if (pev->health <= 0)
	{
		int iGib = GIB_NORMAL;// XDM
		if (FBitSet(bitsDamageType, DMG_NEVERGIB))
			iGib = GIB_NEVER;
		else if (FBitSet(bitsDamageType, DMG_ALWAYSGIB))
			iGib = GIB_ALWAYS;

		Killed(pInflictor, pAttacker, iGib);
		return 0;
	}
	return 1;
}

void CBaseEntity::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)// new
{
	pev->takedamage = DAMAGE_NO;
	pev->deadflag = DEAD_DEAD;
	UTIL_Remove(this);
}

BOOL CBaseEntity::HasTarget(string_t targetname)
{
	return FStrEq(STRING(targetname), STRING(pev->target));// XDM3035c: target! Not targetname!!!
}

CBaseEntity *CBaseEntity::GetNextTarget(void)
{
	if (FStringNull(pev->target))
		return NULL;

	return UTIL_FindEntityByTargetname(NULL, STRING(pev->target));// XDM3035c
}

