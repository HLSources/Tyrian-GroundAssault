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

===== doors.cpp ========================================================

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "doors.h"


const char *CBaseDoor::MoveSounds[] =
{
	"common/null.wav",
	"doors/doormove1.wav",
	"doors/doormove2.wav",
	"doors/doormove3.wav",
	"doors/doormove4.wav",
	"doors/doormove5.wav",
	"doors/doormove6.wav",
	"doors/doormove7.wav",
	"doors/doormove8.wav",
	"doors/doormove9.wav",
	"doors/doormove10.wav",
	"common/null.wav",
};

const char *CBaseDoor::StopSounds[] =
{
	"common/null.wav",
	"doors/doorstop1.wav",
	"doors/doorstop2.wav",
	"doors/doorstop3.wav",
	"doors/doorstop4.wav",
	"doors/doorstop5.wav",
	"doors/doorstop6.wav",
	"doors/doorstop7.wav",
	"doors/doorstop8.wav",
	"common/null.wav",
};


TYPEDESCRIPTION	CBaseDoor::m_SaveData[] =
{
	DEFINE_FIELD(CBaseDoor, m_bHealthValue, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseDoor, m_bMoveSnd, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseDoor, m_bStopSnd, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseDoor, m_bLockedSound, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseDoor, m_bLockedSentence, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseDoor, m_bUnlockedSound, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseDoor, m_bUnlockedSentence, FIELD_CHARACTER),
};

IMPLEMENT_SAVERESTORE(CBaseDoor, CBaseToggle);


/*QUAKED func_door (0 .5 .8) ? START_OPEN x DOOR_DONT_LINK TOGGLE
if two doors touch, they are assumed to be connected and operate as a unit.

TOGGLE causes the door to wait in both the start and end states for a trigger event.

START_OPEN causes the door to move to its destination when spawned, and operate in reverse.
It is used to temporarily or permanently close off an area when triggered (not usefull for
touch or takedamage doors).

"angle"         determines the opening direction
"targetname"	if set, no touch field will be spawned and a remote button or trigger
								field activates the door.
"health"        if set, door must be shot open
"speed"         movement speed (100 default)
"wait"          wait before returning (3 default, -1 = never return)
"lip"           lip remaining at end of move (8 default)
"dmg"           damage to inflict when blocked (2 default)
"sounds"
0)no sound
1)stone
2)base
3)stone chain
4)screechy metal
*/

LINK_ENTITY_TO_CLASS(func_door, CBaseDoor);
// func_water - same as a door.
LINK_ENTITY_TO_CLASS(func_water, CBaseDoor);

// Cache user-entity-field values until spawn is called.
void CBaseDoor::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "skin"))//skin is used for content type
	{
		pev->skin = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "movesnd"))
	{
		m_bMoveSnd = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "stopsnd"))
	{
		m_bStopSnd = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "healthvalue"))
	{
		m_bHealthValue = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "locked_sound"))
	{
		m_bLockedSound = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "locked_sentence"))
	{
		m_bLockedSentence = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "unlocked_sound"))
	{
		m_bUnlockedSound = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "unlocked_sentence"))
	{
		m_bUnlockedSentence = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "WaveHeight"))
	{
		pev->scale = (float)atof(pkvd->szValue) * (1.0f/8.0f);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "dontdraw"))
	{
		m_bDontDraw = atoi(pkvd->szValue);// 0-draw always, 1-dd opn, 2-dd cls
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue(pkvd);
}

void CBaseDoor::Spawn(void)
{
	Precache();
	SetMovedir (pev);

	if (pev->skin == 0)
	{//normal door
		if (FBitSet(pev->spawnflags, SF_DOOR_PASSABLE))
			pev->solid = SOLID_NOT;
		else
			pev->solid = SOLID_BSP;
	}
	else
	{// special contents
		pev->solid		= SOLID_NOT;// XDM: we can't track down how wawter touches things (water physics disappear when any other solid property set)
		SetBits(pev->spawnflags, SF_DOOR_SILENT);	// water is silent for now
	}

	pev->movetype	= MOVETYPE_PUSH;
	UTIL_SetOrigin(pev, pev->origin);
	SET_MODEL(ENT(pev), STRING(pev->model));

	if (pev->speed == 0)
		pev->speed = 100;

	pev->frame = 0;// XDM3037

	m_vecPosition1	= pev->origin;
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2	= m_vecPosition1 + (pev->movedir * (fabs(pev->movedir.x * (pev->size.x-2)) + fabs(pev->movedir.y * (pev->size.y-2)) + fabs(pev->movedir.z * (pev->size.z-2)) - m_flLip));

	ASSERTSZ(m_vecPosition1 != m_vecPosition2, "CBaseDoor door start/end positions are equal");

	if (FBitSet(pev->spawnflags, SF_DOOR_START_OPEN))
	{	// swap pos1 and pos2, put door at pos2
		UTIL_SetOrigin(pev, m_vecPosition2);
		m_vecPosition2 = m_vecPosition1;
		m_vecPosition1 = pev->origin;

		if (m_bDontDraw == 1)// XDM
			pev->effects |= EF_NODRAW;

//		m_toggle_state = TS_AT_TOP;
	}
	else
	{
		if (m_bDontDraw == 2)// XDM
			pev->effects |= EF_NODRAW;

//		m_toggle_state = TS_AT_BOTTOM;
	}
	m_toggle_state = TS_AT_BOTTOM;

	// if the door is flagged for USE button activation only, use NULL touch function
	if (FBitSet(pev->spawnflags, SF_DOOR_USE_ONLY))
	{
		SetTouch(&CBaseDoor::DoorTouchDmgOnly);
	}
	else // touchable button
		SetTouch(&CBaseDoor::DoorTouch);
}

void CBaseDoor::SetToggleState(int state)
{
	if (state == TS_AT_TOP)
		UTIL_SetOrigin(pev, m_vecPosition2);
	else
		UTIL_SetOrigin(pev, m_vecPosition1);
}

void CBaseDoor::Precache(void)
{
// set the door's "in-motion" sound
	if (FStringNull(pev->noiseMoving))// XDM3035c: real custom sound has first priority
	{
		if (m_bMoveSnd > 0)
			pev->noiseMoving = MAKE_STRING(MoveSounds[m_bMoveSnd]);
	}
	if (!FStringNull(pev->noiseMoving))
		PRECACHE_SOUND(STRINGV(pev->noiseMoving));

// set the door's 'reached destination' stop sound
	if (FStringNull(pev->noiseArrived))// XDM3035c: real custom sound has first priority
	{
		if (m_bStopSnd > 0)
			pev->noiseArrived = MAKE_STRING(StopSounds[m_bStopSnd]);
	}
	if (!FStringNull(pev->noiseArrived))
		PRECACHE_SOUND(STRINGV(pev->noiseArrived));

	char *pszSound;
	// get door button sounds, for doors which are directly 'touched' to open
	if (m_bLockedSound)
	{
		pszSound = ButtonSound((int)m_bLockedSound);
		PRECACHE_SOUND(pszSound);
		m_ls.sLockedSound = ALLOC_STRING(pszSound);
	}

	if (m_bUnlockedSound)
	{
		pszSound = ButtonSound((int)m_bUnlockedSound);
		PRECACHE_SOUND(pszSound);
		m_ls.sUnlockedSound = ALLOC_STRING(pszSound);
	}

	// get sentence group names, for doors which are directly 'touched' to open
	switch (m_bLockedSentence)
	{
		case 1: m_ls.sLockedSentence = ALLOC_STRING("NA"); break; // access denied
		case 2: m_ls.sLockedSentence = ALLOC_STRING("ND"); break; // security lockout
		case 3: m_ls.sLockedSentence = ALLOC_STRING("NF"); break; // blast door
		case 4: m_ls.sLockedSentence = ALLOC_STRING("NFIRE"); break; // fire door
		case 5: m_ls.sLockedSentence = ALLOC_STRING("NCHEM"); break; // chemical door
		case 6: m_ls.sLockedSentence = ALLOC_STRING("NRAD"); break; // radiation door
		case 7: m_ls.sLockedSentence = ALLOC_STRING("NCON"); break; // gen containment
		case 8: m_ls.sLockedSentence = ALLOC_STRING("NH"); break; // maintenance door
		case 9: m_ls.sLockedSentence = ALLOC_STRING("NG"); break; // broken door
		default: m_ls.sLockedSentence = 0; break;
	}

	switch (m_bUnlockedSentence)
	{
		case 1: m_ls.sUnlockedSentence = ALLOC_STRING("EA"); break; // access granted
		case 2: m_ls.sUnlockedSentence = ALLOC_STRING("ED"); break; // security door
		case 3: m_ls.sUnlockedSentence = ALLOC_STRING("EF"); break; // blast door
		case 4: m_ls.sUnlockedSentence = ALLOC_STRING("EFIRE"); break; // fire door
		case 5: m_ls.sUnlockedSentence = ALLOC_STRING("ECHEM"); break; // chemical door
		case 6: m_ls.sUnlockedSentence = ALLOC_STRING("ERAD"); break; // radiation door
		case 7: m_ls.sUnlockedSentence = ALLOC_STRING("ECON"); break; // gen containment
		case 8: m_ls.sUnlockedSentence = ALLOC_STRING("EH"); break; // maintenance door
		default: m_ls.sUnlockedSentence = 0; break;
	}
}

void CBaseDoor::DoorTouchDmgOnly(CBaseEntity *pOther)// XDM
{
/*	if (pev->skin < CONTENT_WATER && pOther->pev->takedamage != DAMAGE_NO && pev->dmgtime < gpGlobals->time)// XDM
	{
//		ALERT(at_console, "CBaseDoor::DoorTouchDmgOnly()\n");
		if (pev->skin == CONTENT_LAVA)
		{
			pOther->TakeDamage(this, m_hActivator?(CBaseEntity *)m_hActivator:this, 5 * pOther->pev->waterlevel, DMG_BURN | DMG_NEVERGIB);// XDM3035: m_hActivator
		}
		else if (pev->skin == CONTENT_SLIME)
		{
			pOther->TakeDamage(this, m_hActivator?(CBaseEntity *)m_hActivator:this, 4 * pOther->pev->waterlevel, DMG_ACID | DMG_NEVERGIB);// XDM3035: m_hActivator
		}
		pev->dmgtime = gpGlobals->time + 1.0;
	}*/
}

//
// Doors not tied to anything (e.g. button, another door) can be touched, to make them activate.
//
void CBaseDoor::DoorTouch(CBaseEntity *pOther)
{
	DoorTouchDmgOnly(pOther);// XDM
//	entvars_t*	pevToucher = pOther->pev;

	// Ignore touches by anything but players
	if (!pOther->IsPlayer())
		return;

	// If door has master, and it's not ready to trigger, play 'locked' sound
	if (IsLockedByMaster())
		PlayLockSounds(pev, &m_ls, TRUE, DOOR_SOUNDWAIT);

	// If door is somebody's target, then touching does nothing.
	// You have to activate the owner (e.g. button).
	if (!FStringNull(pev->targetname))
	{
		// play locked sound
		PlayLockSounds(pev, &m_ls, TRUE, DOOR_SOUNDWAIT);
		return;
	}

	m_hActivator = pOther;// remember who activated the door

	if (DoorActivate())
		SetTouch(&CBaseDoor::DoorTouchDmgOnly); // Temporarily disable the touch function, until movement is finished.
}

//
// Used by SUB_UseTargets, when a door is the target of a button.
//
void CBaseDoor::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
//	ALERT(at_console, "Door %s: m_toggle_state %d, m_flWait %f\n", STRING(pev->targetname), m_toggle_state, m_flWait);
	m_hActivator = pActivator;
	// if not ready to be used, ignore "use" command.
	if (m_toggle_state == TS_AT_BOTTOM || m_toggle_state == TS_AT_TOP && (FBitSet(pev->spawnflags, SF_DOOR_NO_AUTO_RETURN) || m_flWait < 0))// XDM: m_flWait
		DoorActivate();
}

// emit door moving and stop sounds on CHAN_STATIC so that the multicast doesn't
// filter them out and leave a client stuck with looping door sounds!
void CBaseDoor::StartSound(void)
{
	if (!FBitSet(pev->spawnflags, SF_DOOR_SILENT))
	{
		if (pev->noiseMoving)
			EMIT_SOUND(ENT(pev), DOOR_SNDCHAN_MOVING, STRINGV(pev->noiseMoving), VOL_NORM, ATTN_NORM);
	}
	pev->frame = 1;// XDM3037
}

void CBaseDoor::StopSound(void)
{
	if (!FBitSet(pev->spawnflags, SF_DOOR_SILENT))
	{
		if (pev->noiseMoving)
			STOP_SOUND(ENT(pev), DOOR_SNDCHAN_MOVING, STRINGV(pev->noiseMoving));
		if (pev->noiseArrived)
			EMIT_SOUND(ENT(pev), DOOR_SNDCHAN_ARRIVED, STRINGV(pev->noiseArrived), VOL_NORM, ATTN_NORM);
	}
	pev->frame = 0;// XDM3037
}

//
// Causes the door to "do its thing", i.e. start moving, and cascade activation.
//
int CBaseDoor::DoorActivate(void)
{
	if (IsLockedByMaster())
		return 0;

	if (m_toggle_state == TS_AT_TOP || m_toggle_state == TS_GOING_UP)// XDM3035c
	{// door should close
		if (FBitSet(pev->spawnflags, SF_DOOR_NO_AUTO_RETURN))
			DoorGoDown();
	}
	else if (m_toggle_state == TS_AT_BOTTOM || m_toggle_state == TS_GOING_DOWN)// door should open
	{
		if (m_hActivator != NULL && m_hActivator->IsPlayer())
		{// give health if player opened the door (medikit)
		// VARS(m_eoActivator)->health += m_bHealthValue;
			m_hActivator->TakeHealth(m_bHealthValue, DMG_GENERIC);
		}
		// play door unlock sounds
		PlayLockSounds(pev, &m_ls, FALSE, DOOR_SOUNDWAIT);
		DoorGoUp();
	}
	return 1;
}

//
// Starts the door going to its "up" position (simply ToggleData->vecPosition2).
//
void CBaseDoor::DoorGoUp(void)
{
	// It could be going-down, if blocked.
	ASSERT(m_toggle_state == TS_AT_BOTTOM || m_toggle_state == TS_GOING_DOWN);

	StartSound();

	if (pev->effects & EF_NODRAW)// XDM
		pev->effects &= ~EF_NODRAW;

	m_toggle_state = TS_GOING_UP;
	SetMoveDone(&CBaseDoor::DoorHitTop);

//	if (FClassnameIs(pev, "func_door_rotating"))// !!! BUGBUG Triggered doors don't work with this yet
	if (IsRotating())// XDM3035a
	{
		float	sign = 1.0;
		if (m_hActivator != NULL)
		{
			entvars_t *pevActivator = m_hActivator->pev;
			if (!FBitSet(pev->spawnflags, SF_DOOR_ONEWAY) && pev->movedir.y)// Y axis rotation, move away from the player
			{
				Vector vec = pevActivator->origin - pev->origin;
				Vector angles = pevActivator->angles;
				angles.x = 0;
				angles.z = 0;
				UTIL_MakeVectors(angles);
	//			Vector vnext = (pevToucher->origin + (pevToucher->velocity * 10)) - pev->origin;
				UTIL_MakeVectors(pevActivator->angles);
				Vector vnext = (pevActivator->origin + (gpGlobals->v_forward * 10)) - pev->origin;
				if ((vec.x*vnext.y - vec.y*vnext.x) < 0.0f)
					sign = -1.0;
			}
		}
		AngularMove(m_vecAngle2*sign, pev->speed);
	}
	else
		LinearMove(m_vecPosition2, pev->speed);
}

//
// The door has reached the "up" position.  Either go back down, or wait for another activation.
//
void CBaseDoor::DoorHitTop(void)
{
	StopSound();

	ASSERT(m_toggle_state == TS_GOING_UP);
	m_toggle_state = TS_AT_TOP;

	if (m_bDontDraw == 2)// XDM
		pev->effects |= EF_NODRAW;

	// toggle-doors don't come down automatically, they wait for refire.
	if (FBitSet(pev->spawnflags, SF_DOOR_NO_AUTO_RETURN))
	{
		// Re-instate touch method, movement is complete
		if (!FBitSet(pev->spawnflags, SF_DOOR_USE_ONLY))
			SetTouch(&CBaseDoor::DoorTouch);
	}
	else
	{
		// In flWait seconds, DoorGoDown will fire, unless wait is -1, then door stays open
		SetThink(&CBaseDoor::DoorGoDown);

		if (m_flWait == -1)
			pev->nextthink = -1;
		else
			pev->nextthink = pev->ltime + m_flWait;
	}

	SUB_UseTargets(m_hActivator, USE_TOGGLE, 0); // this isn't finished

	// Fire the close target (if startopen is set, then "top" is closed) - netname is the close target
	if (pev->netname && (pev->spawnflags & SF_DOOR_START_OPEN))
		FireTargets(STRING(pev->netname), m_hActivator, this, USE_TOGGLE, 0);
}

//
// Starts the door going to its "down" position (simply ToggleData->vecPosition1).
//
void CBaseDoor::DoorGoDown(void)
{
	if (pev->effects & EF_NODRAW)// XDM
		pev->effects &= ~EF_NODRAW;

	StartSound();

#ifdef DOOR_ASSERT
	ASSERT(m_toggle_state == TS_AT_TOP);
#endif // DOOR_ASSERT
	m_toggle_state = TS_GOING_DOWN;

	SetMoveDone(&CBaseDoor::DoorHitBottom);

//	if (FClassnameIs(pev, "func_door_rotating"))//rotating door
	if (IsRotating())// XDM3035a
		AngularMove(m_vecAngle1, pev->speed);
	else
		LinearMove(m_vecPosition1, pev->speed);
}

//
// The door has reached the "down" position.  Back to quiescence.
//
void CBaseDoor::DoorHitBottom(void)
{
	StopSound();

	ASSERT(m_toggle_state == TS_GOING_DOWN);
	m_toggle_state = TS_AT_BOTTOM;

	if (m_bDontDraw == 1)// XDM
		pev->effects |= EF_NODRAW;

	// Re-instate touch method, cycle is complete
	if (FBitSet(pev->spawnflags, SF_DOOR_USE_ONLY))// use only door
		SetTouch(&CBaseDoor::DoorTouchDmgOnly);
	else // touchable door
		SetTouch(&CBaseDoor::DoorTouch);

	SUB_UseTargets(m_hActivator, USE_TOGGLE, 0); // this isn't finished

	// Fire the close target (if startopen is set, then "top" is closed) - netname is the close target
	if (pev->netname && !(pev->spawnflags & SF_DOOR_START_OPEN))
		FireTargets(STRING(pev->netname), m_hActivator, this, USE_TOGGLE, 0);
}

void CBaseDoor::Blocked(CBaseEntity *pOther)
{
	edict_t	*pentTarget = NULL;
	CBaseDoor *pDoor = NULL;

	// Hurt the blocker a little.
	if (pev->dmg)
		pOther->TakeDamage(this, m_hActivator?(CBaseEntity *)m_hActivator:this, pev->dmg, DMG_CRUSH);// XDM3035: m_hActivator

	// if a door has a negative wait, it would never come back if blocked,
	// so let it just squash the object to death real fast
	if (m_flWait >= 0)
	{
		if (!FBitSet(pev->spawnflags, SF_DOOR_SILENT))// XDM
			if (pev->noiseMoving)
				STOP_SOUND(ENT(pev), DOOR_SNDCHAN_MOVING, STRINGV(pev->noiseMoving));

		if (m_toggle_state == TS_GOING_DOWN)
			DoorGoUp();
		else
			DoorGoDown();
	}

	// Block all door pieces with the same targetname here.
	if (!FStringNull(pev->targetname))
	{
		for (;;)
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME(pentTarget, STRING(pev->targetname));

			if (VARS(pentTarget) != pev)
			{
				if (FNullEnt(pentTarget))
					break;

				//if (FClassnameIs (pentTarget, "func_door") || FClassnameIs (pentTarget, "func_door_rotating"))
				if (strncmp(STRING(pentTarget->v.classname), "func_door", 9) == 0)// XDM3035a: faster hack
				{
					pDoor = GetClassPtr((CBaseDoor *) VARS(pentTarget));
					if (pDoor->m_flWait >= 0)
					{
						if (pDoor->pev->velocity == pev->velocity && pDoor->pev->avelocity == pev->velocity)
						{
							// this is the most hacked, evil, bastardized thing I've ever seen. kjb
							//if (FClassnameIs (pentTarget, "func_door"))
							if (pDoor->IsRotating())// XDM3035a
							{// set angles to realign rotating doors
								pDoor->pev->angles = pev->angles;
								pDoor->pev->avelocity = g_vecZero;
							}
							else
							{// set origin to realign normal doors
								pDoor->pev->origin = pev->origin;
								pDoor->pev->velocity = g_vecZero;// stop!
							}
						}

						if (!FBitSet(pDoor->pev->spawnflags, SF_DOOR_SILENT))// XDM
							if (pDoor->pev->noiseMoving)
								STOP_SOUND(ENT(pDoor->pev), DOOR_SNDCHAN_MOVING, STRINGV(pDoor->pev->noiseMoving));

						if (pDoor->m_toggle_state == TS_GOING_DOWN)
							pDoor->DoorGoUp();
						else
							pDoor->DoorGoDown();
					}
				}
			}
		}
	}
}


/*QUAKED FuncRotDoorSpawn (0 .5 .8) ? START_OPEN REVERSE
DOOR_DONT_LINK TOGGLE X_AXIS Y_AXIS
if two doors touch, they are assumed to be connected and operate as
a unit.

TOGGLE causes the door to wait in both the start and end states for
a trigger event.

START_OPEN causes the door to move to its destination when spawned,
and operate in reverse.  It is used to temporarily or permanently
close off an area when triggered (not usefull for touch or
takedamage doors).

You need to have an origin brush as part of this entity.  The
center of that brush will be
the point around which it is rotated. It will rotate around the Z
axis by default.  You can
check either the X_AXIS or Y_AXIS box to change that.

"distance" is how many degrees the door will be rotated.
"speed" determines how fast the door moves; default value is 100.

REVERSE will cause the door to rotate in the opposite direction.

"angle"		determines the opening direction
"targetname" if set, no touch field will be spawned and a remote
button or trigger field activates the door.
"health"	if set, door must be shot open
"speed"		movement speed (100 default)
"wait"		wait before returning (3 default, -1 = never return)
"dmg"		damage to inflict when blocked (2 default)
"sounds"
0)	no sound
1)	stone
2)	base
3)	stone chain
4)	screechy metal
*/

LINK_ENTITY_TO_CLASS(func_door_rotating, CRotDoor);


void CRotDoor::Spawn(void)
{
	Precache();
	// set the axis of rotation
	AxisDir();

	// check for clockwise rotation
	if (FBitSet(pev->spawnflags, SF_DOOR_ROTATE_BACKWARDS))
		pev->movedir = pev->movedir * -1;

	//m_flWait			= 2; who the hell did this? (sjb)
	m_vecAngle1	= pev->angles;
	m_vecAngle2	= pev->angles + pev->movedir * m_flMoveDistance;

	ASSERTSZ(m_vecAngle1 != m_vecAngle2, "rotating door start/end positions are equal");

	if (FBitSet(pev->spawnflags, SF_DOOR_PASSABLE))
		pev->solid = SOLID_NOT;
	else
		pev->solid = SOLID_BSP;

	pev->movetype	= MOVETYPE_PUSH;
	UTIL_SetOrigin(pev, pev->origin);
	SET_MODEL(ENT(pev), STRING(pev->model));

	if (pev->speed == 0)
		pev->speed = 100;

// DOOR_START_OPEN is to allow an entity to be lighted in the closed position
// but spawn in the open position
	if (FBitSet(pev->spawnflags, SF_DOOR_START_OPEN))
	{	// swap pos1 and pos2, put door at pos2, invert movement direction
		pev->angles = m_vecAngle2;
		Vector vecSav = m_vecAngle1;
		m_vecAngle2 = m_vecAngle1;
		m_vecAngle1 = vecSav;
		pev->movedir = pev->movedir * -1;
	}

	m_toggle_state = TS_AT_BOTTOM;

	if (FBitSet(pev->spawnflags, SF_DOOR_USE_ONLY))
		SetTouch(&CBaseDoor::DoorTouchDmgOnly);
	else // touchable button
		SetTouch(&CBaseDoor::DoorTouch);
}


void CRotDoor::SetToggleState(int state)
{
	if (state == TS_AT_TOP)
		pev->angles = m_vecAngle2;
	else
		pev->angles = m_vecAngle1;

	UTIL_SetOrigin(pev, pev->origin);
}



//-----------------------------------------------------------------------------
// CMomentaryDoor: moves while control is pressed
//-----------------------------------------------------------------------------
LINK_ENTITY_TO_CLASS(momentary_door, CMomentaryDoor);

void CMomentaryDoor::Spawn(void)
{
	SetMovedir (pev);

	pev->solid		= SOLID_BSP;
	pev->movetype	= MOVETYPE_PUSH;

	Precache();

	UTIL_SetOrigin(pev, pev->origin);
	SET_MODEL(ENT(pev), STRING(pev->model));

	if (pev->speed == 0)
		pev->speed = 100;
	if (pev->dmg == 0)
		pev->dmg = 2;

	m_vecPosition1	= pev->origin;
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2	= m_vecPosition1 + (pev->movedir * (fabs(pev->movedir.x * (pev->size.x-2)) + fabs(pev->movedir.y * (pev->size.y-2)) + fabs(pev->movedir.z * (pev->size.z-2)) - m_flLip));

	ASSERTSZ(m_vecPosition1 != m_vecPosition2, "CMomentaryDoor start/end positions are equal!");

	if (FBitSet(pev->spawnflags, SF_DOOR_START_OPEN))
	{	// swap pos1 and pos2, put door at pos2
		UTIL_SetOrigin(pev, m_vecPosition2);
		m_vecPosition2 = m_vecPosition1;
		m_vecPosition1 = pev->origin;
		m_toggle_state = TS_AT_TOP;
	}
	else
		m_toggle_state = TS_AT_BOTTOM;

	SetTouchNull();
}

void CMomentaryDoor::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (useType != USE_SET)		// Momentary buttons will pass down a float in here
		return;

//	ALERT(at_console, "CMomentaryDoor::Use(%f)\n", value);

	if (value > 1.0f)
		value = 1.0f;

	Vector move = m_vecPosition1 + (value * (m_vecPosition2 - m_vecPosition1));
	Vector delta = move - pev->origin;
	float speed = delta.Length() * 10.0f;

	if (speed != 0.0f)
	{
		// This entity only thinks when it moves, so if it's thinking, it's in the process of moving play the sound when it starts moving
		if (pev->nextthink < pev->ltime || pev->nextthink == 0)
			StartSound();
//			EMIT_SOUND(ENT(pev), DOOR_SNDCHAN_MOVING, STRINGV(pev->noiseMoving), VOL_NORM, ATTN_NORM);

		LinearMove(move, speed);

		if (m_toggle_state == TS_AT_TOP)// XDM3035a: since CMomentaryDoor is now derived from CBaseDoor, we can reuse really neat code here
		{
			m_toggle_state = TS_GOING_DOWN;
			SetMoveDone(&CBaseDoor::DoorHitBottom);
		}
		else
		{
			m_toggle_state = TS_GOING_UP;
			SetMoveDone(&CBaseDoor::DoorHitTop);
		}
	}

}
/*
class CBaseWater : public CBaseDoor
{
public:
	void Spawn(void);
	void Precache(void);
};

//
// func_water - same as a door.
//
LINK_ENTITY_TO_CLASS(func_water, CBaseWater);


void CBaseWater::Spawn(void)
{
	CBaseDoor::Spawn();
}

void CBaseWater::Precache(void)
{
	CBaseDoor::Precache();
}
*/
