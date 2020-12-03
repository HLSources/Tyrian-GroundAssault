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

===== triggers.cpp ========================================================

  spawn and use functions for editor-placed triggers

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "saverestore.h"
#include "triggers.h"
#include "trains.h"// trigger_camera has train functionality
#include "gamerules.h"
#include "game.h"// XDM: cvar.value
#include "globals.h"
#include "shake.h"
#include "shared_resources.h"
#include "soundent.h"
#include "pm_shared.h"
#include "util_vector.h"

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( func_friction, CFrictionModifier );

// Global Savedata for changelevel friction modifier
TYPEDESCRIPTION	CFrictionModifier::m_SaveData[] =
{
	DEFINE_FIELD( CFrictionModifier, m_frictionFraction, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE(CFrictionModifier,CBaseEntity);

// Modify an entity's friction
void CFrictionModifier::Spawn(void)
{
	pev->solid = SOLID_TRIGGER;
	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	pev->movetype = MOVETYPE_NONE;
	SetTouch(&CFrictionModifier::ChangeFriction);
}

// Sets toucher's friction to m_frictionFraction (1.0 = normal friction)
void CFrictionModifier::ChangeFriction(CBaseEntity *pOther)
{
	if (pOther->pev->movetype != MOVETYPE_BOUNCEMISSILE && pOther->pev->movetype != MOVETYPE_BOUNCE)
		pOther->pev->friction = m_frictionFraction;
}

// Sets toucher's friction to m_frictionFraction (1.0 = normal friction)
void CFrictionModifier::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "modifier"))
	{
		m_frictionFraction = atof(pkvd->szValue) / 100.0;
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(trigger_auto, CTriggerAuto);

TYPEDESCRIPTION	CTriggerAuto::m_SaveData[] =
{
	DEFINE_FIELD(CTriggerAuto, m_globalstate, FIELD_STRING),
	DEFINE_FIELD(CTriggerAuto, triggerType, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CTriggerAuto,CBaseDelay);

void CTriggerAuto::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "globalstate"))
	{
		m_globalstate = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "triggerstate"))
	{
		int type = atoi(pkvd->szValue);
		if (type == 0)
			triggerType = USE_OFF;
		else if (type == 2)
			triggerType = USE_TOGGLE;
		else
			triggerType = USE_ON;

		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue(pkvd);
}

void CTriggerAuto::Spawn(void)
{
	Precache();
}

void CTriggerAuto::Precache(void)
{
	pev->nextthink = gpGlobals->time + 0.2;
}

void CTriggerAuto::Think(void)
{
	if (!m_globalstate || gGlobalState.EntityGetState(m_globalstate) == GLOBAL_ON)
	{
		SUB_UseTargets(this, triggerType, 0);
		if (pev->spawnflags & SF_AUTO_FIREONCE)
			UTIL_Remove(this);
	}
}


//-----------------------------------------------------------------------------
// Fires a target after level transition and then dies
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(fireanddie, CFireAndDie);

void CFireAndDie::Spawn(void)
{
	pev->classname = MAKE_STRING("fireanddie");
	// Don't call Precache() - it should be called on restore
}

void CFireAndDie::Precache(void)
{
	// This gets called on restore
	pev->nextthink = gpGlobals->time + m_flDelay;
}

void CFireAndDie::Think(void)
{
	SUB_UseTargets(this, USE_TOGGLE, 0);
	UTIL_Remove(this);
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_relay, CTriggerRelay );

TYPEDESCRIPTION	CTriggerRelay::m_SaveData[] =
{
	DEFINE_FIELD( CTriggerRelay, triggerType, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE(CTriggerRelay,CBaseDelay);

void CTriggerRelay::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "triggerstate"))
	{
		int type = atoi(pkvd->szValue);
		if (type == 0)
			triggerType = USE_OFF;
		else if (type == 2)
			triggerType = USE_TOGGLE;
		else
			triggerType = USE_ON;

		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue(pkvd);
}

void CTriggerRelay::Spawn(void)
{
//	Precache();
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
}

void CTriggerRelay::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	SUB_UseTargets(/* this*/pActivator, triggerType, 0);// XDM3035c: pActivator
	if (pev->spawnflags & SF_RELAY_FIREONCE)
		UTIL_Remove(this);
}


//-----------------------------------------------------------------------------
//
// Render parameters trigger
//
// This entity will copy its render parameters (renderfx, rendermode, rendercolor, renderamt)
// to its targets when triggered.
//
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( env_render, CRenderFxManager );

void CRenderFxManager::Spawn(void)
{
//	Precache();
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;// XDM3035a
}

void CRenderFxManager::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!FStringNull(pev->target))
	{
		edict_t *pentTarget = NULL;
		while ((pentTarget = FIND_ENTITY_BY_TARGETNAME(pentTarget, STRING(pev->target))) != NULL)
		{
			if (FNullEnt(pentTarget))
				break;

			entvars_t *pevTarget = VARS(pentTarget);
			if (!FBitSet(pev->spawnflags, SF_RENDER_MASKFX))
				pevTarget->renderfx = pev->renderfx;
			if (!FBitSet(pev->spawnflags, SF_RENDER_MASKAMT))
				pevTarget->renderamt = pev->renderamt;
			if (!FBitSet(pev->spawnflags, SF_RENDER_MASKMODE))
				pevTarget->rendermode = pev->rendermode;
			if (!FBitSet(pev->spawnflags, SF_RENDER_MASKCOLOR))
				pevTarget->rendercolor = pev->rendercolor;
		}
	}
}



//-----------------------------------------------------------------------------
// BASE TRIGGER
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger, CBaseTrigger );

// Cache user-entity-field values until spawn is called.
void CBaseTrigger::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "damage"))
	{
		pev->dmg = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "count"))
	{
		m_cTriggersLeft = (int) atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "damagetype"))
	{
		m_bitsDamageInflict = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "sounds"))
	{
		if (pkvd->szValue && strcmp(pkvd->szValue, "0"))// XDM: this prevents 'sound not precached' bug
			pev->noise = ALLOC_STRING(pkvd->szValue);// By the way, where does this sound gets precached?!

		pkvd->fHandled = TRUE;
	}
	else if(FStrEq(pkvd->szKeyName, "height"))// XDM
	{
		m_flHeight = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

BOOL CBaseTrigger::CanTouch(CBaseEntity *pToucher)
{
#if 0
		// if the trigger has an angles field, check player's facing direction
		if (!pev->movedir.IsZero())
		{
			Vector forward;
			ANGLE_VECTORS(pToucher->pev->angles, forward, NULL, NULL);
			if (DotProduct(forward, pev->movedir) < 0)
				return FALSE;         // not facing the right way
		}
#endif

	if (FStringNull(pev->netname))
	{
		// Only touch clients, monsters, or pushables (depending on flags)
		if (pToucher->IsPlayer())
			return !(pev->spawnflags & SF_TRIGGER_NOCLIENTS);
		else if (pToucher->IsMonster())
			return (pev->spawnflags & SF_TRIGGER_ALLOWMONSTERS);
		else if (pToucher->IsPushable())//(FClassnameIs(pToucher,"func_pushable"))
			return (pev->spawnflags & SF_TRIGGER_PUSHABLES);
		else
			return (pev->spawnflags & SF_TRIGGER_EVERYTHING);
	}
	else
	{
		// If netname is set, it's an entity-specific trigger; we ignore the spawnflags.
		if (!FClassnameIs(pToucher->pev, STRING(pev->netname)) &&
			(FStringNull(pToucher->pev->targetname) || !FStrEq(STRING(pToucher->pev->targetname), STRING(pev->netname))))
			return FALSE;
	}
	return TRUE;
}

void CBaseTrigger::InitTrigger(void)
{
	Precache();
	// trigger angles are used for one-way touches.  An angle of 0 is assumed
	// to mean no restrictions, so use a yaw of 360 instead.
	if (pev->angles != g_vecZero)
		SetMovedir(pev);

	if (pev->spawnflags & SF_TRIGGER_START_OFF)// XDM3035c: TESTME!
		pev->solid = SOLID_NOT;
	else
		pev->solid = SOLID_TRIGGER;

	pev->movetype = MOVETYPE_NONE;
	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world

	if (showtriggers.value <= 0)
		SetBits(pev->effects, EF_NODRAW);
}

// the trigger was just touched/killed/used
// self.enemy should be set to the activator so it can be held through a delay
// so wait for the delay time before firing
void CBaseTrigger::ActivateMultiTrigger(CBaseEntity *pActivator)
{
	if (pev->nextthink > gpGlobals->time)
		return;         // still waiting for reset time

	if (IsLockedByMaster())
		return;

/*XDM	if (FClassnameIs(pev, "trigger_secret"))
	{
		if ( pev->enemy == NULL || !FClassnameIs(pev->enemy, "player"))
			return;
		gpGlobals->found_secrets++;
	}*/

	if (!FStringNull(pev->noise))
		EMIT_SOUND(ENT(pev), CHAN_VOICE, STRINGV(pev->noise), VOL_NORM, ATTN_NORM);

// don't trigger again until reset
// pev->takedamage = DAMAGE_NO;

	m_hActivator = pActivator;
	SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );

	if ( pev->message && pActivator && pActivator->IsPlayer() )
	{
		UTIL_ShowMessage( STRING(pev->message), pActivator );
//		CLIENT_PRINTF( ENT( pActivator->pev ), print_center, STRING(pev->message) );
	}

	if (m_flWait > 0)
	{
		SetThink(&CBaseTrigger::MultiWaitOver);
		pev->nextthink = gpGlobals->time + m_flWait;
	}
	else
	{
		// we can't just remove (self) here, because this is a touch function
		// called while C code is looping through area links...
		SetTouchNull();
		pev->nextthink = gpGlobals->time + 0.1;
		SetThink(&CBaseEntity::SUB_Remove);
	}
}

// the wait time has passed, so set back up for another activation
void CBaseTrigger::MultiWaitOver(void)
{
//	if (pev->max_health)
//		{
//		pev->health		= pev->max_health;
//		pev->takedamage	= DAMAGE_YES;
//		pev->solid		= SOLID_BBOX;
//		}
	SetThinkNull();
}

void CBaseTrigger::CounterUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	m_cTriggersLeft--;
	m_hActivator = pActivator;

	if (m_cTriggersLeft < 0)
		return;
/*
	BOOL fTellActivator = (m_hActivator != 0) && pActivator->IsPlayer() && !FBitSet(pev->spawnflags, SPAWNFLAG_NOMESSAGE);

	if (m_cTriggersLeft != 0)
	{
		if (fTellActivator)
			ALERT(at_console, "Only %d more to go...\n", m_cTriggersLeft);

		return;
	}

	// !!!UNDONE: I don't think we want these Quakesque messages
	if (fTellActivator)
		ALERT(at_console, "Sequence completed!");
*/
	ActivateMultiTrigger(m_hActivator);
}

// ToggleUse - If this is the USE function for a trigger, its state will toggle every time it's fired
void CBaseTrigger::ToggleUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (IsLockedByMaster())// XDM3035c
		return;

	if (pev->solid == SOLID_NOT)
	{
		m_hActivator = pActivator;// XDM3035
		// if the trigger is off, turn it on
		pev->solid = SOLID_TRIGGER;
		// Force retouch
		gpGlobals->force_retouch++;
	}
	else
	{// turn the trigger off
		pev->solid = SOLID_NOT;
	}
	UTIL_SetOrigin( pev, pev->origin );
}


//-----------------------------------------------------------------------------
// trigger_monsterjump
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_monsterjump, CTriggerMonsterJump );

void CTriggerMonsterJump::Spawn (void)
{
	SetMovedir(pev);
	InitTrigger();

	pev->nextthink = 0;
	pev->speed = 200;
	if (m_flHeight == 0.0f)// XDM
		m_flHeight = 150;

	if (!FStringNull(pev->targetname))
	{// if targetted, spawn turned off
		pev->solid = SOLID_NOT;
		UTIL_SetOrigin( pev, pev->origin ); // Unlink from trigger list
		SetUse(&CBaseTrigger::ToggleUse);
	}
}

void CTriggerMonsterJump::Think(void)
{
	pev->solid = SOLID_NOT;// kill the trigger for now !!!UNDONE
	UTIL_SetOrigin( pev, pev->origin ); // Unlink from trigger list
	SetThinkNull();
}

void CTriggerMonsterJump::Touch(CBaseEntity *pOther)
{
	entvars_t *pevOther = pOther->pev;

	if (!FBitSet(pevOther->flags , FL_MONSTER))
	{// touched by a non-monster.
		return;
	}

	pevOther->origin.z += 1;

	if ( FBitSet ( pevOther->flags, FL_ONGROUND ) )
	{// clear the onground so physics don't bitch
		pevOther->flags &= ~FL_ONGROUND;
	}

	// toss the monster!
	pevOther->velocity = pev->movedir * pev->speed;
	pevOther->velocity.z += m_flHeight;
	pev->nextthink = gpGlobals->time;
}




//-----------------------------------------------------------------------------
// trigger_cdaudio - starts/stops cd audio tracks
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_cdaudio, CTriggerCDAudio );

// Changes tracks or stops CD when player touches
// !!!HACK - overloaded HEALTH to avoid adding new field
void CTriggerCDAudio::Touch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())// only clients may trigger these events
		return;

	Use(pOther, pOther, USE_ON, pev->health);
}

void CTriggerCDAudio::Spawn(void)
{
	InitTrigger();
}

void CTriggerCDAudio::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBaseEntity *pPlayer = NULL;// XDM

	if (pActivator && pActivator->IsPlayer())// only play this track for one player if that player activated this directly
		pPlayer = pActivator;

	if (FStringNull(pev->message))
		PlayCDTrack(pPlayer, (int)pev->health, (pev->spawnflags & SF_TRIGGERCDAUDIO_LOOP)?1:0);
	else
		PlayAudioTrack(pPlayer, STRING(pev->message), (pev->spawnflags & SF_TRIGGERCDAUDIO_LOOP)?1:0);

	UTIL_Remove(this);
}


//-----------------------------------------------------------------------------
// This plays a CD track when fired or when the player enters it's radius
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( target_cdaudio, CTargetCDAudio );

#define SF_TARGETCDAUDIO_LOOP		0x0001

void CTargetCDAudio::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "radius"))
	{
		pev->scale = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

void CTargetCDAudio::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;

	if (pev->scale > 0)
		pev->nextthink = gpGlobals->time + 1.0;
}

void CTargetCDAudio::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	CBaseEntity *pPlayer = NULL;// XDM

	if (pActivator && pActivator->IsPlayer())// only play this track for one player if that player activated this directly
		pPlayer = pActivator;

	if (FStringNull(pev->message))
		PlayCDTrack(pPlayer, (int)pev->health, (pev->spawnflags & SF_TARGETCDAUDIO_LOOP)?1:0);
	else
		PlayAudioTrack(pPlayer, STRING(pev->message), (pev->spawnflags & SF_TARGETCDAUDIO_LOOP)?1:0);

	UTIL_Remove(this);
}

// only plays for ONE client, so only use in single play!
void CTargetCDAudio::Think(void)
{
	pev->nextthink = gpGlobals->time + 0.5;
	edict_t *pClient = NULL;
	edict_t *pFirst = NULL;
	while ((pClient = FIND_CLIENT_IN_PVS(edict())) != NULL)// XDM3035c: tricky!
	{
		if (pFirst == NULL)
			pFirst = pClient;
		else if (pFirst == pClient)// reached first again
			break;

		if ((pClient->v.origin - pev->origin).Length() <= pev->scale)
		{
			pev->nextthink = 0;
			Use(CBaseEntity::Instance(pClient), this, USE_ON, pev->health);
			return;
		}
	}
}


//-----------------------------------------------------------------------------
// trigger_hurt - hurts anything that touches it. if the trigger has a targetname, firing it will toggle state
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(trigger_hurt, CTriggerHurt);
LINK_ENTITY_TO_CLASS(trigger_butthurt, CTriggerHurt);

void CTriggerHurt::Spawn(void)
{
	InitTrigger();
//	SetTouch(HurtTouch);

	if (!FStringNull(pev->targetname))
	{
		SetUse(&CBaseTrigger::ToggleUse);
	}
	else
	{
		SetUseNull();
	}

	if (m_bitsDamageInflict & DMG_RADIATION)
	{
		SetThink(&CTriggerHurt::RadiationThink);
		pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.0, 0.5);
	}

	if (FBitSet(pev->spawnflags, SF_TRIGGER_HURT_START_OFF))// if flagged to Start Turned Off, make trigger nonsolid.
		pev->solid = SOLID_NOT;

	UTIL_SetOrigin(pev, pev->origin);		// Link into the list
}

void CTriggerHurt::RadiationThink(void)
{
	Vector origin;
	Vector view_ofs;

	// check to see if a player is in pvs if not, continue
	// set origin to center of trigger so that this check works
	origin = pev->origin;
	view_ofs = pev->view_ofs;
	pev->origin = VecBModelOrigin(pev);//(pev->absmin + pev->absmax) * 0.5;
	pev->view_ofs = g_vecZero;
	pev->origin = origin;
	pev->view_ofs = view_ofs;

	pev->nextthink = gpGlobals->time + 0.25;
}

// When touched, a hurt trigger does DMG points of damage each half-second
void CTriggerHurt::Touch(CBaseEntity *pOther)// XDM
{
	if (!pOther->pev->takedamage)
		return;

	if ((pev->spawnflags & SF_TRIGGER_HURT_CLIENTONLYTOUCH) && !pOther->IsPlayer())
		return;// this trigger is only allowed to touch clients, and this ain't a client.

	if ((pev->spawnflags & SF_TRIGGER_HURT_NO_CLIENTS) && pOther->IsPlayer())
		return;

	if (pOther->IsPlayer() && FClassnameIs(pev, "trigger_butthurt"))
		ENGINE_SETPHYSKV(pOther->edict(), PHYSKEY_BUTTHURT, "1");

	// HACKHACK -- In multiplayer, players touch this based on packet receipt.
	// So the players who send packets later aren't always hurt.  Keep track of
	// how much time has passed and whether or not you've touched that player
	if (g_pGameRules->IsMultiplayer())
	{
		if (pev->dmgtime > gpGlobals->time)
		{
			if ( gpGlobals->time != pev->pain_finished )
			{// too early to hurt again, and not same frame with a different entity
				if ( pOther->IsPlayer() )
				{
					int playerMask = 1 << (pOther->entindex() - 1);
					// If I've already touched this player (this time), then bail out
					if (pev->impulse & playerMask)
						return;

					// Mark this player as touched
					// BUGBUG - There can be only 32 players!
					pev->impulse |= playerMask;
				}
				else
				{
					return;
				}
			}
		}
		else
		{
			// New clock, "un-touch" all players
			pev->impulse = 0;
			if ( pOther->IsPlayer() )
			{
				int playerMask = 1 << (pOther->entindex() - 1);
				// Mark this player as touched
				// BUGBUG - There can be only 32 players!
				pev->impulse |= playerMask;
			}
		}
		// XDM3034 HACK? Prevent overgibbage and HL crashes.
// more accurate but SLOW!		if (g_pGameRules->CountPlayers() > 8)
		if (gpGlobals->maxClients > 8 && (sv_clientgibs.value <= 0.0f))
			SetBits(m_bitsDamageInflict, DMG_NEVERGIB);
	}
	else	// Original code -- single player
	{
		if (pev->dmgtime > gpGlobals->time && gpGlobals->time != pev->pain_finished)
			return;// too early to hurt again, and not same frame with a different entity
	}
	// If this is time_based damage (poison, radiation), override the pev->dmg with a
	// default for the given damage type.  Monsters only take time-based damage
	// while touching the trigger.  Player continues taking damage for a while after
	// leaving the trigger
	float fldmg = pev->dmg * 0.5;	// 0.5 seconds worth of damage, pev->dmg is damage/second
	// JAY: Cut this because it wasn't fully realized.  Damage is simpler now.
#if 0
	switch (m_bitsDamageInflict)
	{
	default: break;
	case DMG_POISON:		fldmg = POISON_DAMAGE/4; break;
	case DMG_NERVEGAS:		fldmg = NERVEGAS_DAMAGE/4; break;
	case DMG_RADIATION:		fldmg = RADIATION_DAMAGE/4; break;
	case DMG_PARALYZE:		fldmg = PARALYZE_DAMAGE/4; break; // UNDONE: cut this? should slow movement to 50%
	case DMG_ACID:			fldmg = ACID_DAMAGE/4; break;
	case DMG_SLOWBURN:		fldmg = SLOWBURN_DAMAGE/4; break;
	case DMG_SLOWFREEZE:	fldmg = SLOWFREEZE_DAMAGE/4; break;
	}
#endif

	if ( fldmg < 0 )
		pOther->TakeHealth( -fldmg, m_bitsDamageInflict );
	else
		pOther->TakeDamage(this, m_hActivator?(CBaseEntity *)m_hActivator:this, fldmg, m_bitsDamageInflict);// XDM3035: m_hActivator

	// Store pain time so we can get all of the other entities on this frame
	pev->pain_finished = gpGlobals->time;
	// Apply damage every half second
	pev->dmgtime = gpGlobals->time + 0.5;// half second delay until this trigger can hurt toucher again

	if (pev->target)
	{
		// trigger has a target it wants to fire.
		if ( pev->spawnflags & SF_TRIGGER_HURT_CLIENTONLYFIRE )
		{
			// if the toucher isn't a client, don't fire the target!
			if ( !pOther->IsPlayer() )
				return;
		}

		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
		if ( pev->spawnflags & SF_TRIGGER_HURT_TARGETONCE )
			pev->target = 0;
	}
}


//-----------------------------------------------------------------------------
/*QUAKED trigger_multiple (.5 .5 .5) ? notouch
Variable sized repeatable trigger.  Must be targeted at one or more entities.
If "health" is set, the trigger must be killed to activate each time.
If "delay" is set, the trigger waits some time after activating before firing.
"wait" : Seconds between triggerings. (.2 default)
If notouch is set, the trigger is only fired by other entities, not by touching.
NOTOUCH has been obsoleted by trigger_relay!
sounds
1)      secret
2)      beep beep
3)      large switch
4)
NEW
if a trigger has a NETNAME, that NETNAME will become the TARGET of the triggered object.
*/
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_multiple, CTriggerMultiple );

void CTriggerMultiple::Spawn(void)
{
	if (m_flWait == 0)
		m_flWait = 0.2;

	InitTrigger();

// XDM wtf?	ASSERTSZ(pev->health == 0, "trigger_multiple with health");
//	UTIL_SetOrigin(pev, pev->origin);
//	SET_MODEL( ENT(pev), STRING(pev->model) );
//	if (pev->health > 0)
//	{
//		if (FBitSet(pev->spawnflags, SPAWNFLAG_NOTOUCH))
//			ALERT(at_error, "trigger_multiple spawn: health and notouch don't make sense");
//		pev->max_health = pev->health;
//UNDONE: where to get pfnDie from?
//		pev->pfnDie = multi_killed;
//		pev->takedamage = DAMAGE_YES;
//		pev->solid = SOLID_BBOX;
//		UTIL_SetOrigin(pev, pev->origin);  // make sure it links into the world
//		SetTouchNull();// XDM
//	}
//	else
/*	{
XDM			SetTouch( MultiTouch );
	}*/
}

void CTriggerMultiple::Touch(CBaseEntity *pOther)// XDM
{
	if (IsLockedByMaster())// XDM3035c
		return;

	bool bSoundActivate = false;
	if (pev->spawnflags & SF_TRIGGER_SOUNDACTIVATE)
	{
		CSound *pSound = CSoundEnt::SoundPointerForIndex(CSoundEnt::ClientSoundIndex(pOther->edict()));
		if (pSound)
			if (pSound->m_iVolume > TRIGGER_ACTIVATE_VOLUME)
				bSoundActivate = true;
	}
	// Only touch clients, monsters, or pushables (depending on flags)
/*	if (((pOther->IsPlayer()) && !(pev->spawnflags & SF_TRIGGER_NOCLIENTS)) ||
		((pOther->IsMonster()) && (pev->spawnflags & SF_TRIGGER_ALLOWMONSTERS)) ||
		((pSound != NULL && pSound->m_iVolume > TRIGGER_ACTIVATE_VOLUME) && (pev->spawnflags & SF_TRIGGER_SOUNDACTIVATE)) ||
		(pev->spawnflags & SF_TRIGGER_PUSHABLES) && FClassnameIs(pOther->pev, "func_pushable"))// not IsPushable() because almost EVERYTHING is pushable
		*/
	if (bSoundActivate || CanTouch(pOther))// XDM3035c
		ActivateMultiTrigger(pOther);
}

//-----------------------------------------------------------------------------
// trigger_once
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_once, CTriggerOnce );

void CTriggerOnce::Spawn(void)
{
	m_flWait = -1;
	CTriggerMultiple::Spawn();
}


/*
QUAKED trigger_counter (.5 .5 .5) ? nomessage
Acts as an intermediary for an action that takes multiple inputs.
If nomessage is not set, it will print "1 more.. " etc when triggered and
"sequence complete" when finished.  After the counter has been triggered "cTriggersLeft"
times (default 2), it will fire all of it's targets and remove itself.
*/
//-----------------------------------------------------------------------------
// trigger_counter
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_counter, CTriggerCounter );

void CTriggerCounter::Spawn(void)
{
	// By making the flWait be -1, this counter-trigger will disappear after it's activated
	// (but of course it needs cTriggersLeft "uses" before that happens).
	m_flWait = -1;

	if (m_cTriggersLeft == 0)
		m_cTriggersLeft = 2;
	SetUse(&CBaseTrigger::CounterUse);
}


// ====================== TRIGGER_CHANGELEVEL ================================


//-----------------------------------------------------------------------------
// trigger_transition
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_transition, CTriggerVolume );

// Define space that travels across a level transition
void CTriggerVolume::Spawn(void)
{
	InitTrigger();// XDM3035c: use common mechanism for triggers
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
/*	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	pev->model = NULL;
	pev->modelindex = 0;*/
}


//-----------------------------------------------------------------------------
// trigger_changelevel
//-----------------------------------------------------------------------------
FILE_GLOBAL char st_szNextMap[MAX_MAPNAME];
FILE_GLOBAL char st_szNextSpot[MAX_MAPNAME];

LINK_ENTITY_TO_CLASS( trigger_changelevel, CChangeLevel );

// Global Savedata for changelevel trigger
TYPEDESCRIPTION	CChangeLevel::m_SaveData[] =
{
	DEFINE_ARRAY( CChangeLevel, m_szMapName, FIELD_CHARACTER, MAX_MAPNAME ),
	DEFINE_ARRAY( CChangeLevel, m_szLandmarkName, FIELD_CHARACTER, MAX_MAPNAME ),
	DEFINE_FIELD( CChangeLevel, m_changeTarget, FIELD_STRING ),
	DEFINE_FIELD( CChangeLevel, m_changeTargetDelay, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE(CChangeLevel,CBaseTrigger);

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pLevelList - 
//			maxList - 
// Output : int
//-----------------------------------------------------------------------------
int BuildChangeList( LEVELLIST *pLevelList, int maxList )
{
	return CChangeLevel::ChangeList( pLevelList, maxList );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pEntity - 
//			*pVolumeName - 
// Output : int
//-----------------------------------------------------------------------------
int InTransitionVolume( CBaseEntity *pEntity, const char *pVolumeName )
{
	if (pEntity == NULL || pVolumeName == NULL)// XDM3035a
		return 0;// 1?

	if ( pEntity->ObjectCaps() & FCAP_FORCE_TRANSITION )
		return 1;

	// If you're following another entity, follow it through the transition (weapons follow the player)
	if ( pEntity->pev->movetype == MOVETYPE_FOLLOW )
	{
		if ( pEntity->pev->aiment != NULL )
			pEntity = CBaseEntity::Instance( pEntity->pev->aiment );
	}

	int inVolume = 1;	// Unless we find a trigger_transition, everything is in the volume
	CBaseEntity *pTarget = NULL;
	while ((pTarget = UTIL_FindEntityByTargetname(pTarget, pVolumeName)) != NULL)
	{
		if (pTarget && FClassnameIs(pTarget->pev, "trigger_transition"))
		{
			if (pTarget->Intersects(pEntity))// It touches one, it's in the volume
				return 1;
			else
				inVolume = 0;	// Found a trigger_transition, but I don't intersect it -- if I don't find another, don't go!
		}
	}

/*	pentVolume = FIND_ENTITY_BY_TARGETNAME( NULL, pVolumeName );
	while ( !FNullEnt( pentVolume ) )
	{
		CBaseEntity *pVolume = CBaseEntity::Instance( pentVolume );

		if ( pVolume && FClassnameIs( pVolume->pev, "trigger_transition" ) )
		{
			if ( pVolume->Intersects( pEntity ) )	// It touches one, it's in the volume
				return 1;
			else
				inVolume = 0;	// Found a trigger_transition, but I don't intersect it -- if I don't find another, don't go!
		}
		pentVolume = FIND_ENTITY_BY_TARGETNAME( pentVolume, pVolumeName );
	}*/

	return inVolume;
}

//-----------------------------------------------------------------------------
// Purpose: Cache user-entity-field values until spawn is called.
// Input  : *pkvd - 
// Output : void CChangeLevel ::
//-----------------------------------------------------------------------------
void CChangeLevel::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "map"))
	{
		if (strlen(pkvd->szValue) >= MAX_MAPNAME)
			ALERT( at_error, "Map name '%s' too long (%d chars max)\n", pkvd->szValue, MAX_MAPNAME);
		strcpy(m_szMapName, pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "landmark"))
	{
		if (strlen(pkvd->szValue) >= MAX_MAPNAME)
			ALERT( at_error, "Landmark name '%s' too long (%d chars max)\n", pkvd->szValue, MAX_MAPNAME);
		strcpy(m_szLandmarkName, pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "changetarget"))
	{
		m_changeTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "changedelay"))
	{
		m_changeTargetDelay = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseTrigger::KeyValue( pkvd );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CChangeLevel::Precache(void)
{
	if (g_pGameRules->IsCoOp())
		PRECACHE_SOUND("game/dom_touch.wav");
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CChangeLevel::IsGameGoal(void)
{
	if (g_pGameRules->IsCoOp() && g_pGameRules->GetGameMode() == COOP_MODE_LEVEL)
		return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: QUAKED trigger_changelevel (0.5 0.5 0.5) ? NO_INTERMISSION
// When the player touches this, he gets sent to the map listed in the "map" variable.  Unless the NO_INTERMISSION flag is set, the view will go to the info_intermission spot and display stats.
//-----------------------------------------------------------------------------
void CChangeLevel::Spawn(void)
{
	if (FStrEq(m_szMapName, ""))
	{
		ALERT(at_console, "WARNING: %s \"%s\" doesn't have a map!\n", STRING(pev->classname), STRING(pev->targetname));
		return;// stay disabled
	}

	if (FStrEq(m_szLandmarkName, ""))
	{
		ALERT(at_console, "WARNING: %s \"%s\" to %d doesn't have a landmark!\n", STRING(pev->classname), STRING(pev->targetname), m_szMapName);
//		return;// stay disabled
	}

	InitTrigger();

	if (!FStringNull(pev->targetname))
	{
		SetUse(&CChangeLevel::UseChangeLevel);
	}

	pev->nextthink = 0;

	if (g_pGameRules->IsCoOp())// XDM3035a: don't allow players to cross this trigger, but allow touching
	{
//		pev->solid = SOLID_BSP;
//		pev->movetype = MOVETYPE_PUSH;
//		SET_MODEL(ENT(pev), STRING(pev->model));// uncomment these 3 lines to make triggers solid which is bad (prevents big momma from reaching next level)
		ClearBits(pev->effects, EF_NODRAW);
//		pev->effects = 0;
		pev->rendermode = kRenderTransColor;
		pev->renderamt = 191;
		pev->rendercolor.y = 255;
		pev->renderfx = kRenderFxPulseFastWide;
//		pev->flags = FL_WORLDBRUSH;33554432
	}

	if (!(pev->spawnflags & SF_CHANGELEVEL_USEONLY))
		SetTouch(&CChangeLevel::TouchChangeLevel);

//	ALERT( at_console, "TRANSITION: %s (%s)\n", m_szMapName, m_szLandmarkName );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pLandmarkName - 
// Output : edict_t
//-----------------------------------------------------------------------------
edict_t *CChangeLevel::FindLandmark(const char *pLandmarkName)
{
	edict_t *pentLandmark = NULL;
	while ((pentLandmark = FIND_ENTITY_BY_TARGETNAME(pentLandmark, pLandmarkName)) != NULL)
	{
		if (FNullEnt(pentLandmark))
			break;
		if (FClassnameIs(pentLandmark, "info_landmark"))
			return pentLandmark;
	}
	ALERT( at_error, "Can't find landmark %s\n", pLandmarkName );
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Use - allows level transitions to be triggered by buttons, etc.
// Input  : *pActivator - 
//			*pCaller - 
//			useType - 
//			value - 
//-----------------------------------------------------------------------------
void CChangeLevel::UseChangeLevel(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// UNDONE: coop: trigger level change now! Don't wait for others!
	ChangeLevelNow(pActivator);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CChangeLevel::TouchChangeLevel(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
		return;

	ChangeLevelNow(pOther);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pActivator - 
//-----------------------------------------------------------------------------
void CChangeLevel::ChangeLevelNow(CBaseEntity *pActivator)
{
	// Some people are firing these multiple times in a frame, disable
	if (gpGlobals->time == pev->dmgtime)
		return;

	ASSERT(!FStrEq(m_szMapName, ""));

	pev->dmgtime = gpGlobals->time;

	CBasePlayer *pPlayer = NULL;
	if (pActivator && pActivator->IsPlayer())
	{
		pPlayer = (CBasePlayer *)pActivator;
	}
	else
	{
		if (g_pGameRules->IsMultiplayer())
			ALERT(at_console, "ERROR: ChangeLevelNow() called by non-player!\n");
		else
			pPlayer = (CBasePlayer *)CBasePlayer::Instance(INDEXENT(1));
	}

	if (pPlayer == NULL)
		return;

	if ( !InTransitionVolume( pPlayer, m_szLandmarkName ) )
	{
		ALERT( at_aiconsole, "ChangeLevelNow(): Player %s isn't in the transition volume %s, aborting\n", STRING(pPlayer->pev->netname), m_szLandmarkName );
		return;
	}

	edict_t	*pentLandmark = FindLandmark( m_szLandmarkName );

	if (!g_pGameRules->FAllowLevelChange(pPlayer, m_szMapName, pentLandmark))// XDM3035
		return;

	// Create an entity to fire the changetarget
	if (m_changeTarget)
	{
		CFireAndDie *pFireAndDie = GetClassPtr((CFireAndDie *)NULL);
		if (pFireAndDie)
		{
			// Set target and delay
			pFireAndDie->pev->target = m_changeTarget;
			pFireAndDie->m_flDelay = m_changeTargetDelay;
			pFireAndDie->pev->origin = pPlayer->pev->origin;
			// Call spawn
			DispatchSpawn(pFireAndDie->edict());
		}
	}
	// This object will get removed in the call to CHANGE_LEVEL, copy the params into "safe" memory
	strcpy(st_szNextMap, m_szMapName);

	m_hActivator = pActivator;
	SUB_UseTargets(pActivator, USE_TOGGLE, 0);
	st_szNextSpot[0] = 0;	// Init landmark to NULL

	// look for a landmark entity
	if (!FNullEnt(pentLandmark))
	{
		strcpy(st_szNextSpot, m_szLandmarkName);
		gpGlobals->vecLandmarkOffset = pentLandmark->v.origin;
	}

//	ALERT( at_console, "Level touches %d levels\n", ChangeList( levels, 16 ) );
	ALERT(at_console, "CHANGE LEVEL: %s %s\n", st_szNextMap, st_szNextSpot);

	g_pWorld = NULL;// XDM LAST ZOMFD!!!!!!!!!! crash prevention?
	CHANGE_LEVEL( st_szNextMap, st_szNextSpot );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Add a transition to the list, but ignore duplicates
// (a designer may have placed multiple trigger_changelevels with the same landmark)
// Input  : *pLevelList - 
//			listCount - 
//			*pMapName - 
//			*pLandmarkName - 
//			*pentLandmark - 
// Output : int
//-----------------------------------------------------------------------------
int CChangeLevel::AddTransitionToList( LEVELLIST *pLevelList, int listCount, const char *pMapName, const char *pLandmarkName, edict_t *pentLandmark )
{
	if (!pLevelList || !pMapName || !pLandmarkName || !pentLandmark)
		return 0;

	int i;
	for (i = 0; i < listCount; ++i)
	{
		if ( pLevelList[i].pentLandmark == pentLandmark && strcmp( pLevelList[i].mapName, pMapName ) == 0 )
			return 0;
	}
	strcpy( pLevelList[listCount].mapName, pMapName );
	strcpy( pLevelList[listCount].landmarkName, pLandmarkName );
	pLevelList[listCount].pentLandmark = pentLandmark;
	pLevelList[listCount].vecLandmarkOrigin = VARS(pentLandmark)->origin;
	return 1;
}


//-----------------------------------------------------------------------------
// Purpose: 
// This has grown into a complicated beast
// Can we make this more elegant?
// This builds the list of all transitions on this level and which entities are in their PVS's and can / should be moved across.
// Input  : *pLevelList - 
//			maxList - 
// Output : int
//-----------------------------------------------------------------------------
int CChangeLevel::ChangeList( LEVELLIST *pLevelList, int maxList )
{
	edict_t	*pentLandmark;
	int i = 0;
	int count = 0;

	// Find all of the possible level changes on this BSP
	CChangeLevel *pTrigger = NULL;
	while ((pTrigger = (CChangeLevel *)UTIL_FindEntityByClassname(pTrigger, "trigger_changelevel")) != NULL)
	{
		// Find the corresponding landmark
		pentLandmark = FindLandmark( pTrigger->m_szLandmarkName );
		if ( pentLandmark )
		{
			// Build a list of unique transitions
			if ( AddTransitionToList( pLevelList, count, pTrigger->m_szMapName, pTrigger->m_szLandmarkName, pentLandmark ) )
			{
				count++;
				if ( count >= maxList )		// FULL!!
					break;
			}
		}
	}

	if ( gpGlobals->pSaveData && ((SAVERESTOREDATA *)gpGlobals->pSaveData)->pTable )
	{
		CSave saveHelper( (SAVERESTOREDATA *)gpGlobals->pSaveData );

		for ( i = 0; i < count; i++ )
		{
			int j, entityCount = 0;
			CBaseEntity *pEntList[TRANSITION_MAX_ENTS];
			int			 entityFlags[TRANSITION_MAX_ENTS];
			// Follow the linked list of entities in the PVS of the transition landmark
			edict_t *pent = ENTITIES_IN_PVS( pLevelList[i].pentLandmark );
			// Build a list of valid entities in this linked list (we're going to use pent->v.chain again)
			while ( !FNullEnt( pent ) )
			{
				CBaseEntity *pEntity = CBaseEntity::Instance(pent);
				if ( pEntity )
				{
//					ALERT( at_console, "Trying %s\n", STRING(pEntity->pev->classname) );
					int caps = pEntity->ObjectCaps();
					if ( !(caps & FCAP_DONT_SAVE) )
					{
						int flags = 0;
						// If this entity can be moved or is global, mark it
						if ( caps & FCAP_ACROSS_TRANSITION )
							flags |= FENTTABLE_MOVEABLE;
						if ( pEntity->pev->globalname && !pEntity->IsDormant() )
							flags |= FENTTABLE_GLOBAL;
						if ( flags )
						{
							pEntList[ entityCount ] = pEntity;
							entityFlags[ entityCount ] = flags;
							entityCount++;
							if (entityCount > TRANSITION_MAX_ENTS)
								ALERT(at_error, "Too many entities across a transition!");
						}
//						else
//							ALERT( at_console, "Failed %s\n", STRING(pEntity->pev->classname) );
					}
//					else
//						ALERT( at_console, "DON'T SAVE %s\n", STRING(pEntity->pev->classname) );
				}
				pent = pent->v.chain;
			}

			for (j = 0; j < entityCount; ++j)
			{
				// Check to make sure the entity isn't screened out by a trigger_transition
				if ( entityFlags[j] && InTransitionVolume( pEntList[j], pLevelList[i].landmarkName ) )
				{
					// Mark entity table with 1<<i
					int index = saveHelper.EntityIndex( pEntList[j] );
					// Flag it with the level number
					saveHelper.EntityFlagsSet( index, entityFlags[j] | (1<<i) );
				}
//				else
//					ALERT( at_console, "Screened out %s\n", STRING(pEntList[j]->pev->classname) );
			}
		}
	}
	return count;
}


//-----------------------------------------------------------------------------
// func_ladder - makes an area vertically negotiable
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(func_ladder, CLadder);

void CLadder::KeyValue(KeyValueData *pkvd)
{
	CBaseTrigger::KeyValue(pkvd);
}

void CLadder::Precache(void)
{
	// Do all of this in here because we need to 'convert' old saved games
	pev->solid = SOLID_NOT;
	pev->skin = CONTENTS_LADDER;
	if (showtriggers.value <= 0.0f)
	{
		pev->rendermode = kRenderTransTexture;
		pev->renderamt = 0;
	}
	pev->effects &= ~EF_NODRAW;
}

void CLadder::Spawn(void)
{
	Precache();
	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	pev->movetype = MOVETYPE_PUSH;
}

void CLadder::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (IsLockedByMaster())// XDM3035c
		return;

	bool bActive = pev->skin == CONTENTS_LADDER;
	if (ShouldToggle(useType, bActive))
	{
		bActive = !bActive;
		if (bActive)
			pev->skin = CONTENTS_LADDER;
		else
			pev->skin = CONTENTS_EMPTY;
	}
}


//-----------------------------------------------------------------------------
// A TRIGGER THAT PUSHES YOU
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_push, CTriggerPush );
/*
TYPEDESCRIPTION	CTriggerPush::m_SaveData[] =
{
	DEFINE_FIELD( CTriggerPush, m_iszPushVel, FIELD_STRING ),
	DEFINE_FIELD( CTriggerPush, m_iszPushSpeed, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE(CTriggerPush,CBaseTrigger);

void CTriggerPush::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "m_iszPushSpeed"))
	{
		m_iszPushSpeed = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iszPushVel"))
	{
		m_iszPushVel = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseTrigger::KeyValue( pkvd );
}
*/
/*QUAKED trigger_push (.5 .5 .5) ? TRIG_PUSH_ONCE
Pushes the player
*/
void CTriggerPush::Spawn(void)
{
	if (pev->angles == g_vecZero)
		pev->angles.y = 360;

	InitTrigger();

	if (FBitSet(pev->spawnflags, SF_TRIGGER_PUSH_START_OFF))// if flagged to Start Turned Off, make trigger nonsolid.
		pev->solid = SOLID_NOT;

	SetUse(&CBaseTrigger::ToggleUse);
	UTIL_SetOrigin(pev, pev->origin);		// Link into the list
}

void CTriggerPush::Touch(CBaseEntity *pOther)
{
	entvars_t *pevToucher = pOther->pev;

	// UNDONE: Is there a better way than health to detect things that have physics? (clients/monsters)
	switch( pevToucher->movetype )
	{
	case MOVETYPE_NONE:
	case MOVETYPE_PUSH:
	case MOVETYPE_NOCLIP:
	case MOVETYPE_FOLLOW:
		return;
	}

	if (!pOther->IsPushable())// XDM
		return;

	if (FBitSet(pev->spawnflags, SF_TRIGGER_PUSH_ONLY_NOTONGROUND) && !FBitSet(pOther->pev->flags, FL_ONGROUND))
		return;// XDM

	Vector vecPush = pev->movedir;

	if (pev->speed)
		vecPush = vecPush * pev->speed;
	else
		vecPush = vecPush * 100;

	if (pevToucher->solid != SOLID_NOT && pevToucher->solid != SOLID_BSP)
	{
		// Instant trigger, just transfer velocity and remove
		if (FBitSet(pev->spawnflags, SF_TRIG_PUSH_ONCE))
		{
			pevToucher->velocity = pevToucher->velocity + vecPush;
			if (pevToucher->velocity.z > 0)
				pevToucher->flags &= ~FL_ONGROUND;

			UTIL_Remove(this);
		}
		else
		{	// Push field, transfer to base velocity
			if (pevToucher->flags & FL_BASEVELOCITY)
				vecPush = vecPush + pevToucher->basevelocity;

			pevToucher->basevelocity = vecPush;
			pevToucher->flags |= FL_BASEVELOCITY;
//			ALERT( at_console, "Vel %f, base %f\n", pevToucher->velocity.z, pevToucher->basevelocity.z );
		}
	}
}


//-----------------------------------------------------------------------------
// teleport trigger
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(trigger_teleport, CTriggerTeleport);

void CTriggerTeleport::Spawn(void)
{
	InitTrigger();
//	SetTouch( TeleportTouch );
}

void CTriggerTeleport::Touch(CBaseEntity *pOther)// XDM
{
	// Only teleport monsters or clients
	if (!FBitSet(pOther->pev->flags, FL_CLIENT|FL_MONSTER))
		return;

	if (IsLockedByMaster())
		return;

	if (!(pev->spawnflags & SF_TRIGGER_ALLOWMONSTERS))
	{// no monsters allowed!
		if (pOther->IsMonster()/* FBitSet(pOther->pev->flags, FL_MONSTER)*/)
			return;
	}

	if ((pev->spawnflags & SF_TRIGGER_NOCLIENTS))
	{// no clients allowed
		if (pOther->IsPlayer())
			return;
	}

	edict_t	*pentTarget = NULL;
	pentTarget = FIND_ENTITY_BY_TARGETNAME(pentTarget, STRING(pev->target));
	if (FNullEnt(pentTarget))
	   return;

	Vector vecSrc = pOther->pev->origin;// Center();?
//	Vector vecDelta = pOther->pev->origin - Center();// UNDONE XDM3034: complex teleporters may be touched in very weird ways. Forget it.
	Vector vecDest = VARS(pentTarget)->origin;

	if (pOther->IsPlayer())
		vecDest.z -= pOther->pev->mins.z;// make origin adjustments in case the teleportee is a player. (origin in center, not at feet)

	vecDest.z++;

	pOther->pev->flags &= ~FL_ONGROUND;

/*	if (pev->spawnflags & SF_TRIGGER_KEEPANGLES)// XDM: undone
		pOther->pev->angles = pOther->pev->angles + pentTarget->v.angles;
	else*/
		pOther->pev->angles = pentTarget->v.angles;

	pOther->pev->fixangle = 1;

	UTIL_SetOrigin(pOther->pev, vecDest/* + vecDelta*/);
//	if (pOther->IsPlayer())
//		pOther->pev->v_angle = pentTarget->v.angles;

	m_hActivator = pOther;// XDM3037

	if (g_pGameRules->IsMultiplayer())
	{
		if (mp_telegib.value > 0)
		{
			CBaseEntity *pEnt = NULL;
			while ((pEnt = UTIL_FindEntityInBox(pEnt, pOther->pev->absmin, pOther->pev->absmax)) != NULL)// TESTME: check real box instead of sphere (should be safer and faster)
			{
				if (pEnt->IsPlayer() && !(pEnt->edict() == ENT(pOther->pev)) || pEnt->IsMonster())
					pEnt->Killed(this, m_hActivator?(CBaseEntity *)m_hActivator:this, GIB_DISINTEGRATE);
			}
/*			while ((pEnt = UTIL_FindEntityInSphere(pEnt, vecDest, 70)) != NULL)// 72=HULL_MAX-HULL_MIN
			{
				if (pEnt->IsPlayer() && !(pEnt->edict() == ENT(pOther->pev)) || pEnt->IsMonster())
					pEnt->Killed(this, m_hActivator?(CBaseEntity *)m_hActivator:this, GIB_DISINTEGRATE);// overrides friendly fire checks, etc.
//					pEnt->TakeDamage(this, m_hActivator?(CBaseEntity *)m_hActivator:this, 300, DMG_GENERIC|DMG_DISINTEGRATING);// XDM3035: m_hActivator
//					pEnt->TakeDamage(VARS(INDEXENT(0)), VARS(INDEXENT(0)), 300, DMG_GENERIC);
			}*/
		}
		if (g_pGameRules->FAllowEffects())// XDM
			PLAYBACK_EVENT_FULL(FEV_RELIABLE, ENT(pOther->pev), g_usTeleport, 0.0, (float *)&vecDest, (float *)&vecSrc, 0.0, 0.0, pOther->pev->team, 0, 1, 0);
	}

	if (pev->spawnflags & SF_TRIGGER_CLEARVELOCITY)// XDM
	{
		pOther->pev->velocity = g_vecZero;
		pOther->pev->basevelocity = g_vecZero;
	}
	else
	{
		float k = pOther->pev->velocity.Length();
		ANGLE_VECTORS(pentTarget->v.angles, pOther->pev->velocity, NULL, NULL);
		pOther->pev->velocity = pOther->pev->velocity*k;
	}
}



//-----------------------------------------------------------------------------
// CTriggerSave - autosave in SP, checkpoint in MP
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_autosave, CTriggerSave );

void CTriggerSave::Spawn(void)
{
	InitTrigger();
//	SetTouch( SaveTouch );
}
/*
void CTriggerSave::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "checkpoint"))// XDM3035c: works like master
	{
		pev->noise = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseTrigger::KeyValue(pkvd);
}
*/

//-----------------------------------------------------------------------------
// Purpose: If player touches this trigger, remember it somehow
// Input  : *pOther - should be a player
//-----------------------------------------------------------------------------
void CTriggerSave::Touch(CBaseEntity *pOther)
{
	// Only save on clients
	if (!pOther->IsPlayer())// MUST be player!
		return;
	if (IsLockedByMaster())
		return;

	if (g_pGameRules->IsMultiplayer())// XDM3035c: in multiplayer (CoOp) these triggers fit very nice as checkpoints!
	{
		((CBasePlayer *)pOther)->OnCheckPoint(this);
	}
	else
	{
		SetTouchNull();
		UTIL_Remove(this);
		SERVER_COMMAND("autosave\n");
	}
}

//-----------------------------------------------------------------------------
// Purpose: Checks if this player already touched this trigger
// Input  : *pEntity - player
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CTriggerSave::IsTriggered(CBaseEntity *pEntity)
{
	if (g_pGameRules->IsMultiplayer())
	{
		if (pEntity->IsPlayer())
		{
			if (((CBasePlayer *)pEntity)->PassedCheckPoint(this) == false)
				return FALSE;
		}
	}
	return CBaseTrigger::IsTriggered(pEntity);
}

//-----------------------------------------------------------------------------
// CTriggerEndSection
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_endsection, CTriggerEndSection );

void CTriggerEndSection::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "section"))
	{
//		m_iszSectionName = ALLOC_STRING( pkvd->szValue );
		// Store this in message so we don't have to write save/restore for this ent
		pev->message = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseTrigger::KeyValue( pkvd );
}

void CTriggerEndSection::Spawn(void)
{
	if ( g_pGameRules->IsMultiplayer() )// XDM?
	{
		REMOVE_ENTITY( ENT(pev) );
		return;
	}

	InitTrigger();

	SetUse(&CTriggerEndSection::EndSectionUse);
	// If it is a "use only" trigger, then don't set the touch function.
	if ( ! (pev->spawnflags & SF_ENDSECTION_USEONLY) )
		SetTouch(&CTriggerEndSection::EndSectionTouch );
}

void CTriggerEndSection::EndSectionUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Only save on clients
	if ( pActivator && !pActivator->IsNetClient() )
		return;

	SetUseNull();

	if (pev->message)
		END_SECTION(STRING(pev->message));

	UTIL_Remove( this );
}

void CTriggerEndSection::EndSectionTouch(CBaseEntity *pOther)
{
	// Only save on clients
	if ( !pOther->IsNetClient() )
		return;

	SetTouchNull();

	if (pev->message)
		END_SECTION(STRING(pev->message));

	UTIL_Remove(this);
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(trigger_gravity, CTriggerGravity);

void CTriggerGravity::Spawn(void)
{
	InitTrigger();
	SetTouch(&CTriggerGravity::GravityTouch);
	SetUse(&CBaseTrigger::ToggleUse);// XDM3035c TESTME
}

void CTriggerGravity::GravityTouch(CBaseEntity *pOther)
{
	if (pev->solid == SOLID_NOT)// redundant?
		return;
	if (!CanTouch(pOther))// XDM3035a: how's this? >:)
		return;
//	if (IsLockedByMaster())// XDM3035c
//		return;

	pOther->pev->gravity = pev->gravity;
}


//-----------------------------------------------------------------------------
// XDM3035c: somebody may want this
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(trigger_playerfreeze, CTriggerPlayerFreeze);

void CTriggerPlayerFreeze::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!pActivator || !pActivator->IsPlayer())
		pActivator = CBaseEntity::Instance(INDEXENT(1));

	if (pActivator->pev->flags & FL_FROZEN)
		((CBasePlayer *)pActivator)->EnableControl(TRUE);
	else
		((CBasePlayer *)pActivator)->EnableControl(FALSE);
}


//-----------------------------------------------------------------------------
// this is a really bad idea.
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_changetarget, CTriggerChangeTarget );

TYPEDESCRIPTION	CTriggerChangeTarget::m_SaveData[] =
{
	DEFINE_FIELD( CTriggerChangeTarget, m_iszNewTarget, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE(CTriggerChangeTarget,CBaseDelay);

void CTriggerChangeTarget::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "m_iszNewTarget"))
	{
		m_iszNewTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerChangeTarget::Spawn(void)
{
}

void CTriggerChangeTarget::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pTarget = UTIL_FindEntityByTargetname(NULL, STRING(pev->target));
	if (pTarget)
	{
		pTarget->pev->target = m_iszNewTarget;
		CBaseMonster *pMonster = pTarget->MyMonsterPointer();
		if (pMonster)
		{
			pMonster->m_pGoalEnt = NULL;
		}
		else if (FClassnameIs(pTarget->pev, "trigger_camera"))// XDM3035c: else (faster)
		{
			pTarget->Use(this, this, USE_SET, 1.0);
		}
	}
}


//-----------------------------------------------------------------------------
// CTriggerCamera
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS( trigger_camera, CTriggerCamera );

// Global Savedata for changelevel friction modifier
TYPEDESCRIPTION	CTriggerCamera::m_SaveData[] =
{
	DEFINE_FIELD( CTriggerCamera, m_hPlayer, FIELD_EHANDLE ),
	DEFINE_FIELD( CTriggerCamera, m_hTarget, FIELD_EHANDLE ),
	DEFINE_FIELD( CTriggerCamera, m_pentPath, FIELD_CLASSPTR ),
	DEFINE_FIELD( CTriggerCamera, m_sPath, FIELD_STRING ),
	DEFINE_FIELD( CTriggerCamera, m_flWait, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_flReturnTime, FIELD_TIME ),
	DEFINE_FIELD( CTriggerCamera, m_flStopTime, FIELD_TIME ),
	DEFINE_FIELD( CTriggerCamera, m_moveDistance, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_targetSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_initialSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_acceleration, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_deceleration, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_state, FIELD_INTEGER ),
	DEFINE_FIELD( CTriggerCamera, m_iszViewEntity, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE(CTriggerCamera,CBaseDelay);

void CTriggerCamera::Spawn(void)
{
	pev->movetype = MOVETYPE_NOCLIP;
	pev->solid = SOLID_NOT;							// Remove model & collisions
	pev->effects = EF_NODRAW;// only in disabled state!
/*
#ifdef _DEBUG
	pev->modelindex = g_iModelIndexAnimglow01;
	pev->rendermode = kRenderTransAdd;
	pev->renderamt = 255;
	pev->effects = EF_BRIGHTFIELD;
#else
	pev->effects = EF_NODRAW;
#endif
*/
	m_initialSpeed = pev->speed;
	if (m_acceleration == 0)
		m_acceleration = 500;
	if (m_deceleration == 0)
		m_deceleration = 500;
}

void CTriggerCamera::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "wait"))
	{
		m_flWait = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "moveto"))
	{
		m_sPath = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "acceleration"))
	{
		m_acceleration = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "deceleration"))
	{
		m_deceleration = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "viewentity"))
	{
		m_iszViewEntity = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerCamera::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	ALERT(at_aiconsole, "CTriggerCamera(%s)::Use(%s %s %d %g)\n", STRING(pev->targetname), pActivator?STRING(pActivator->pev->classname):"NULL", pCaller?STRING(pCaller->pev->classname):"NULL", useType, value);
	if (!ShouldToggle(useType, m_state > 0))
		return;

//	if (IsLockedByMaster())// XDM3035c: should this ever be locked?
//		return;

	// Toggle state
	m_state = !m_state;
	ALERT(at_aiconsole, "CTriggerCamera(%s)::Use(): state set to %d\n", STRING(pev->targetname), m_state);
	if (m_state == 0)
	{
		m_flReturnTime = gpGlobals->time;
		return;
	}

	if (!pActivator || !pActivator->IsPlayer())
	{
		pActivator = CBaseEntity::Instance(INDEXENT(1));
		ALERT(at_console, "CTriggerCamera(%s)::Use(): has no activator, picked default player\n", STRING(pev->targetname));
	}

	m_hPlayer = pActivator;

	if (m_flWait > 0)// XDM
		m_flReturnTime = gpGlobals->time + m_flWait;
	else
		m_flReturnTime = 0;

	pev->speed = m_initialSpeed;
	m_targetSpeed = m_initialSpeed;

	if (FBitSet(pev->spawnflags, SF_CAMERA_PLAYER_TARGET))
		m_hTarget = m_hPlayer;
	else
		m_hTarget = GetNextTarget();

	// Nothing to look at!
	if (m_hTarget == NULL)
	{
		ALERT(at_console, "CTriggerCamera(%s)::Use(): error: no target!\n", STRING(pev->targetname));
		return;
	}

	if (FBitSet(pev->spawnflags, SF_CAMERA_PLAYER_TAKECONTROL))
		((CBasePlayer *)pActivator)->EnableControl(FALSE);

	if (m_sPath)
		m_pentPath = UTIL_FindEntityByTargetname(NULL, STRING(m_sPath));
	else
		m_pentPath = NULL;

	m_flStopTime = gpGlobals->time;
	if (m_pentPath)
	{
		if (m_pentPath->pev->speed != 0.0f)
			m_targetSpeed = m_pentPath->pev->speed;

		m_flStopTime += m_pentPath->GetDelay();
	}

	pev->flags |= FL_DRAW_ALWAYS;// XDM3035c: ignore PVS restrictions. We use ShouldBeSentTo() to prevent sending to unneeded players and flooding the network.
	pev->effects &= ~EF_NODRAW;
	pev->modelindex = g_iModelIndexAnimglow01;
	pev->renderamt = 0;								// The engine won't draw this model if this is set to 0 and blending is on
	pev->rendermode = kRenderTransAdd;				// But the entity will be sent to clients!
	pev->framerate = 0.0f;
//	m_hPlayer->pev->iuser1 = OBS_IN_EYE;
//	m_hPlayer->pev->iuser2 = entindex();
//	m_hPlayer->m_hObserverTarget = this;

	// copy over player information
	if (FBitSet(pev->spawnflags, SF_CAMERA_PLAYER_POSITION))
	{
		UTIL_SetOrigin(pev, pActivator->pev->origin + pActivator->pev->view_ofs);
		pev->angles.x = -pActivator->pev->angles.x;
		pev->angles.y = pActivator->pev->angles.y;
		pev->angles.z = 0;
		pev->velocity = pActivator->pev->velocity;
	}
	else
	{
		UTIL_SetOrigin(pev, pev->origin);
		pev->velocity = g_vecZero;
	}

	if (m_iszViewEntity)//LRC
	{
		CBaseEntity *pEntity = UTIL_FindEntityByTargetname(NULL, STRING(m_iszViewEntity));
		if (pEntity)
			UTIL_SetView(pActivator->edict(), pEntity->edict());
		else
			ALERT(at_console, "CTriggerCamera: unable to find view entity: '%s'!\n", STRING(m_iszViewEntity));
	}
	else
		UTIL_SetView(pActivator->edict(), edict());

	// follow the player down
	SetThink(&CTriggerCamera::FollowTarget);
	pev->nextthink = gpGlobals->time;
	m_moveDistance = 0;
	Move();
}

// XDM3035c: works perfectly without flooding network channels!
bool CTriggerCamera::ShouldBeSentTo(CBasePlayer *pClient)
{
	if (m_hPlayer.Get() && m_hPlayer == pClient)
		return true;

	return false;// must return false for all clients to override FL_DRAW_ALWAYS
}

void CTriggerCamera::FollowTarget(void)
{
	if (m_hPlayer == NULL)
		return;

	if (m_hTarget == NULL || (m_flReturnTime > 0 && m_flReturnTime < gpGlobals->time))// XDM
	{
		Deactivate();
		return;
	}

	Vector vecGoal = UTIL_VecToAngles( m_hTarget->EyePosition() - pev->origin );// XDM: EyePosition
	vecGoal.x = -vecGoal.x;

	if (pev->angles.y > 360)
		pev->angles.y -= 360;

	if (pev->angles.y < 0)
		pev->angles.y += 360;

	float dx = vecGoal.x - pev->angles.x;
	float dy = vecGoal.y - pev->angles.y;

	NormalizeAngle(&dx);// XDM3035c
	NormalizeAngle(&dy);
/*	if (dx < -180)
		dx += 360;
	if (dx > 180)
		dx -= 360;

	if (dy < -180)
		dy += 360;
	if (dy > 180)
		dy -= 360;
*/
	pev->avelocity.x = dx * 40.0f * gpGlobals->frametime;
	pev->avelocity.y = dy * 40.0f * gpGlobals->frametime;

	if (!(FBitSet(pev->spawnflags, SF_CAMERA_PLAYER_TAKECONTROL)))
	{
		pev->velocity = pev->velocity * 0.8f;
		if (pev->velocity.Length() < 10.0f)
			pev->velocity = g_vecZero;
	}
	pev->nextthink = gpGlobals->time;
	Move();
}

void CTriggerCamera::Move(void)
{
	// Not moving on a path, return
	if (!m_pentPath)
		return;

	// Subtract movement from the previous frame
	m_moveDistance -= pev->speed * gpGlobals->frametime;

	// Have we moved enough to reach the target?
	if ( m_moveDistance <= 0 )
	{
		// Fire the passtarget if there is one
		if ( m_pentPath->pev->message )
		{
			FireTargets( STRING(m_pentPath->pev->message), this, this, USE_TOGGLE, 0 );
			if ( FBitSet( m_pentPath->pev->spawnflags, SF_CORNER_FIREONCE ) )
				m_pentPath->pev->message = 0;
		}
		// Time to go to the next target
		m_pentPath = m_pentPath->GetNextTarget();

		// Set up next corner
		if ( !m_pentPath )
		{
			pev->velocity = g_vecZero;
		}
		else
		{
			if ( m_pentPath->pev->speed != 0 )
				m_targetSpeed = m_pentPath->pev->speed;

			Vector delta = m_pentPath->pev->origin - pev->origin;
			m_moveDistance = delta.Length();
			pev->movedir = delta.Normalize();
			m_flStopTime = gpGlobals->time + m_pentPath->GetDelay();
		}
	}

	if ( m_flStopTime > gpGlobals->time )
		pev->speed = UTIL_Approach( 0, pev->speed, m_deceleration * gpGlobals->frametime );
	else
		pev->speed = UTIL_Approach( m_targetSpeed, pev->speed, m_acceleration * gpGlobals->frametime );

	float fraction = 2 * gpGlobals->frametime;
	pev->velocity = ((pev->movedir * pev->speed) * fraction) + (pev->velocity * (1-fraction));
}

void CTriggerCamera::Deactivate(void)
{
	ALERT(at_aiconsole, "CTriggerCamera(%s)::Deactivate()\n", STRING(pev->targetname));
	ASSERT(m_hPlayer.Get() != NULL);
	UTIL_SetView(m_hPlayer->edict(), m_hPlayer->edict());// XDM3035b: even if dead
	if (m_hPlayer->IsAlive())
	{
		((CBasePlayer *)((CBaseEntity *)m_hPlayer))->EnableControl(TRUE);
	}
	SUB_UseTargets(this, USE_TOGGLE, 0);
	pev->avelocity = g_vecZero;
	pev->flags &= ~FL_DRAW_ALWAYS;
	pev->effects |= EF_NODRAW;
	pev->modelindex = 0;
	pev->renderamt = 0;
//	pev->rendermode = kRenderTransAdd;
	m_state = 0;
}


//-----------------------------------------------------------------------------
// CTriggerBounce
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(trigger_bounce, CTriggerBounce);

void CTriggerBounce::Spawn(void)
{
	SetMovedir(pev);
	InitTrigger();
}

void CTriggerBounce::Touch(CBaseEntity *pOther)
{
	if (!CanTouch(pOther))
		return;
	if (IsLockedByMaster())
		return;

	float dot = DotProduct(pev->movedir, pOther->pev->velocity);
	if (dot < -pev->armorvalue)
	{
		if (pev->spawnflags & SF_BOUNCE_CUTOFF)
			pOther->pev->velocity = pOther->pev->velocity - (dot + pev->frags*(dot+pev->armorvalue))*pev->movedir;
		else
			pOther->pev->velocity = pOther->pev->velocity - (dot + pev->frags*dot)*pev->movedir;

		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
	}
}
