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

===== buttons.cpp ========================================================

  button-related code

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "doors.h"
#include "sound.h"
#include "pm_materials.h"
#include "effects.h"


#define SF_BUTTON_DONTMOVE		1
#define SF_ROTBUTTON_NOTSOLID	1
#define SF_BUTTON_ONLYDIRECT	16  // button can't be used through walls.
#define	SF_BUTTON_TOGGLE		32	// button stays pushed until reactivated
#define	SF_BUTTON_SPARK_IF_OFF	64	// button sparks in OFF state
#define SF_BUTTON_NOT_SOLID		128	// button isn't solid
#define SF_BUTTON_TOUCH_ONLY	256	// button only fires as a result of USE key.

//
// Generic Button
//
class CBaseButton : public CBaseToggle
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void KeyValue(KeyValueData *pkvd);
	// Buttons that don't take damage can be IMPULSE used
	virtual int	ObjectCaps(void);// XDM: moved to cpp { return (CBaseToggle:: ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | (pev->takedamage?0:FCAP_IMPULSE_USE); }
	virtual BOOL IsBSPModel(void) { return TRUE; }// XDM
	virtual BOOL IsMovingBSP(void) { return TRUE; }// XDM

	void ButtonActivate(void);
	void UpdateAllButtons(bool value);

	void EXPORT ButtonShot(void);
	void EXPORT ButtonTouch(CBaseEntity *pOther);
	void EXPORT ButtonSpark(void);
	void EXPORT TriggerAndWait(void);
	void EXPORT ButtonReturn(void);
	void EXPORT ButtonBackHome(void);
	void EXPORT ButtonUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	virtual void SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static CBaseButton *Instance(edict_t *pent) { return (CBaseButton *)GET_PRIVATE(pent);};

	enum BUTTON_CODE { BUTTON_NOTHING, BUTTON_ACTIVATE, BUTTON_RETURN };
	BUTTON_CODE	ButtonResponseToTouch(void);

	static	TYPEDESCRIPTION m_SaveData[];

	BOOL	m_fStayPushed;	// button stays pushed in until touched again?
	BOOL	m_fRotating;		// a rotating button?  default is a sliding button.

	string_t m_strChangeTarget;	// if this field is not null, this is an index into the engine string array.
							// when this button is touched, it's target entity's TARGET field will be set
							// to the button's ChangeTarget. This allows you to make a func_train switch paths, etc.

	locksound_t m_ls;			// door lock sounds

	byte	m_bLockedSound;		// ordinals from entity selection
	byte	m_bLockedSentence;
	byte	m_bUnlockedSound;
	byte	m_bUnlockedSentence;
	int		m_sounds;
	BOOL	m_Disable;// XDM3037: temporarily disable (used by new sync code)
};

/*QUAKED func_button (0 .5 .8) ?
When a button is touched, it moves some distance in the direction of it's angle,
triggers all of it's targets, waits some time, then returns to it's original position
where it can be triggered again.

"angle"		determines the opening direction
"target"	all entities with a matching targetname will be used
"speed"		override the default 40 speed
"wait"		override the default 1 second wait (-1 = never return)
"lip"		override the default 4 pixel lip remaining at end of move
"health"	if set, the button must be killed instead of touched
"sounds"
0) steam metal
1) wooden clunk
2) metallic click
3) in-out
*/

LINK_ENTITY_TO_CLASS( func_button, CBaseButton );


// CBaseButton
TYPEDESCRIPTION CBaseButton::m_SaveData[] =
{
	DEFINE_FIELD( CBaseButton, m_fStayPushed, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBaseButton, m_fRotating, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBaseButton, m_sounds, FIELD_INTEGER ),
	DEFINE_FIELD( CBaseButton, m_bLockedSound, FIELD_CHARACTER ),
	DEFINE_FIELD( CBaseButton, m_bLockedSentence, FIELD_CHARACTER ),
	DEFINE_FIELD( CBaseButton, m_bUnlockedSound, FIELD_CHARACTER ),
	DEFINE_FIELD( CBaseButton, m_bUnlockedSentence, FIELD_CHARACTER ),
	DEFINE_FIELD( CBaseButton, m_strChangeTarget, FIELD_STRING ),
	DEFINE_FIELD( CBaseButton, m_Disable, FIELD_BOOLEAN ),// XDM3037
//	DEFINE_FIELD( CBaseButton, m_ls, FIELD_??? ),   // This is restored in Precache()
};

IMPLEMENT_SAVERESTORE( CBaseButton, CBaseToggle );

int	CBaseButton::ObjectCaps(void)
{
	return (CBaseToggle::ObjectCaps()
			| ((pev->health > 0) ? 0:FCAP_IMPULSE_USE)
			| ((pev->spawnflags & SF_BUTTON_ONLYDIRECT) ? FCAP_ONLYDIRECT_USE:0));// XDM3037
}

//
// Cache user-entity-field values until spawn is called.
//
void CBaseButton::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "changetarget"))
	{
		m_strChangeTarget = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "locked_sound"))
	{
		m_bLockedSound = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "locked_sentence"))
	{
		m_bLockedSentence = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "unlocked_sound"))
	{
		m_bUnlockedSound = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "unlocked_sentence"))
	{
		m_bUnlockedSentence = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "sounds"))
	{
		m_sounds = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CBaseButton::Precache(void)
{
	char *pszSound;
	// get door button sounds, for doors which require buttons to open
	if (m_bLockedSound)
	{
		pszSound = ButtonSound( (int)m_bLockedSound );
		PRECACHE_SOUND(pszSound);
		m_ls.sLockedSound = ALLOC_STRING(pszSound);
	}

	if (m_bUnlockedSound)
	{
		pszSound = ButtonSound( (int)m_bUnlockedSound );
		PRECACHE_SOUND(pszSound);
		m_ls.sUnlockedSound = ALLOC_STRING(pszSound);
	}

	// get sentence group names, for doors which are directly 'touched' to open
	switch (m_bLockedSentence)
	{
		case 1: m_ls.sLockedSentence = MAKE_STRING("NA"); break; // access denied
		case 2: m_ls.sLockedSentence = MAKE_STRING("ND"); break; // security lockout
		case 3: m_ls.sLockedSentence = MAKE_STRING("NF"); break; // blast door
		case 4: m_ls.sLockedSentence = MAKE_STRING("NFIRE"); break; // fire door
		case 5: m_ls.sLockedSentence = MAKE_STRING("NCHEM"); break; // chemical door
		case 6: m_ls.sLockedSentence = MAKE_STRING("NRAD"); break; // radiation door
		case 7: m_ls.sLockedSentence = MAKE_STRING("NCON"); break; // gen containment
		case 8: m_ls.sLockedSentence = MAKE_STRING("NH"); break; // maintenance door
		case 9: m_ls.sLockedSentence = MAKE_STRING("NG"); break; // broken door
		default: m_ls.sLockedSentence = 0; break;
	}

	switch (m_bUnlockedSentence)
	{
		case 1: m_ls.sUnlockedSentence = MAKE_STRING("EA"); break; // access granted
		case 2: m_ls.sUnlockedSentence = MAKE_STRING("ED"); break; // security door
		case 3: m_ls.sUnlockedSentence = MAKE_STRING("EF"); break; // blast door
		case 4: m_ls.sUnlockedSentence = MAKE_STRING("EFIRE"); break; // fire door
		case 5: m_ls.sUnlockedSentence = MAKE_STRING("ECHEM"); break; // chemical door
		case 6: m_ls.sUnlockedSentence = MAKE_STRING("ERAD"); break; // radiation door
		case 7: m_ls.sUnlockedSentence = MAKE_STRING("ECON"); break; // gen containment
		case 8: m_ls.sUnlockedSentence = MAKE_STRING("EH"); break; // maintenance door
		default: m_ls.sUnlockedSentence = 0; break;
	}
}

void CBaseButton::Spawn(void)
{
	//----------------------------------------------------
	//determine sounds for buttons
	//a sound of 0 should not make a sound
	//----------------------------------------------------
	char  *pszSound = ButtonSound( m_sounds );
	PRECACHE_SOUND(pszSound);
	pev->noise = ALLOC_STRING(pszSound);

	Precache();

	if (FBitSet(pev->spawnflags, SF_BUTTON_SPARK_IF_OFF))// this button should spark in OFF state
	{
		SetThink(&CBaseButton::ButtonSpark);
		pev->nextthink = gpGlobals->time + 0.5;// no hurry, make sure everything else spawns
	}

	SetMovedir(pev);

	pev->movetype	= MOVETYPE_PUSH;
	if (pev->spawnflags & SF_BUTTON_NOT_SOLID)// XDM
		pev->solid		= SOLID_NOT;
	else
		pev->solid		= SOLID_BSP;

	SET_MODEL(ENT(pev), STRING(pev->model));

	if (pev->speed == 0)
		pev->speed = 40;

//	if (pev->health > 0)// Warning! Check ObjectCaps() formula!
		pev->takedamage = DAMAGE_YES;

	if (m_flWait == 0)
		m_flWait = 1;
	if (m_flLip == 0)
		m_flLip = 4;

	m_toggle_state = TS_AT_BOTTOM;
	m_vecPosition1 = pev->origin;
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2	= m_vecPosition1 + (pev->movedir * (fabs( pev->movedir.x * (pev->size.x-2) ) + fabs( pev->movedir.y * (pev->size.y-2) ) + fabs( pev->movedir.z * (pev->size.z-2) ) - m_flLip));

	// Is this a non-moving button?
	if ( ((m_vecPosition2 - m_vecPosition1).Length() < 1) || (pev->spawnflags & SF_BUTTON_DONTMOVE) )
		m_vecPosition2 = m_vecPosition1;

	m_fStayPushed = (m_flWait == -1 ? TRUE : FALSE);
	m_fRotating = FALSE;

	// if the button is flagged for USE button activation only, take away it's touch function and add a use function

	if ( FBitSet ( pev->spawnflags, SF_BUTTON_TOUCH_ONLY ) ) // touchable button
	{
		SetTouch(&CBaseButton::ButtonTouch);
	}
	else
	{
		SetTouchNull();
		SetUse(&CBaseButton::ButtonUse);
	}

	m_Disable = false;// XDM3037
}

//
// ButtonShot
//
int CBaseButton::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pev->health <= 0)// XDM3035c: allow pev->health to be dynamically changed
		return 0;
	if (m_Disable)// XDM3037
		return 0;

	BUTTON_CODE code = ButtonResponseToTouch();

	if ( code == BUTTON_NOTHING )
		return 0;
	// Temporarily disable the touch function, until movement is finished.
	SetTouchNull();

	m_hActivator = pAttacker;// XDM3037: TODO: check: do we really need to set m_hActivator even if it's null? 
	if (m_hActivator == NULL)
		return 0;

	if (code == BUTTON_RETURN)
	{
		if (pev->noise)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, STRINGV(pev->noise), VOL_NORM, ATTN_NORM);

		// Toggle buttons fire when they get back to their "home" position
		if ( !(pev->spawnflags & SF_BUTTON_TOGGLE) )
			SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );

		ButtonReturn();
	}
	else // code == BUTTON_ACTIVATE
		ButtonActivate();

	return 0;
}

//
// Makes flagged buttons spark when turned off
//
void CBaseButton::ButtonSpark (void)
{
	SetThink(&CBaseButton::ButtonSpark);
	pev->nextthink = gpGlobals->time + ( 0.1 + RANDOM_FLOAT ( 0, 1.5 ) );// spark again at random interval
	DoSpark( pev, pev->mins );
}

//
// Button's Use function
//
void CBaseButton::ButtonUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Ignore touches if button is moving, or pushed-in and waiting to auto-come-out.
	// UNDONE: Should this use ButtonResponseToTouch() too?
	if (m_toggle_state == TS_GOING_UP || m_toggle_state == TS_GOING_DOWN)
		return;

	m_hActivator = pActivator;
	if (m_toggle_state == TS_AT_TOP)
	{
		if (!m_fStayPushed && FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE))
		{
			if (pev->noise)
				EMIT_SOUND(ENT(pev), CHAN_VOICE, STRINGV(pev->noise), VOL_NORM, ATTN_NORM);

			//SUB_UseTargets( m_eoActivator );
			ButtonReturn();
		}
	}
	else
		ButtonActivate();
}

CBaseButton::BUTTON_CODE CBaseButton::ButtonResponseToTouch(void)
{
	// Ignore touches if button is moving, or pushed-in and waiting to auto-come-out.
	if (m_toggle_state == TS_GOING_UP || m_toggle_state == TS_GOING_DOWN ||
		(m_toggle_state == TS_AT_TOP && !m_fStayPushed && !FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE)))
		return BUTTON_NOTHING;

	if (m_toggle_state == TS_AT_TOP)
	{
		if ((FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE)) && !m_fStayPushed)
			return BUTTON_RETURN;
	}
	else
		return BUTTON_ACTIVATE;

	return BUTTON_NOTHING;
}


//
// Touching a button simply "activates" it.
//
void CBaseButton::ButtonTouch(CBaseEntity *pOther)
{
	// Ignore touches by anything but players
	if (!pOther->IsPlayer())
		return;
	if (m_Disable)// XDM3037
		return;

	m_hActivator = pOther;

	BUTTON_CODE code = ButtonResponseToTouch();

	if (code == BUTTON_NOTHING)
		return;

	if (IsLockedByMaster())
	{
		// play button locked sound
		PlayLockSounds(pev, &m_ls, TRUE, BUTTON_SOUNDWAIT);
		return;
	}

	// Temporarily disable the touch function, until movement is finished.
	SetTouchNull();

	if (code == BUTTON_RETURN)
	{
		if (pev->noise)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, STRINGV(pev->noise), VOL_NORM, ATTN_NORM);

		SUB_UseTargets(m_hActivator, USE_TOGGLE, 0);
		ButtonReturn();
	}
	else	// code == BUTTON_ACTIVATE
		ButtonActivate();
}

//
// Starts the button moving "in/up".
//
void CBaseButton::ButtonActivate(void)
{
	if (m_Disable)// XDM3037
		return;

	if (IsLockedByMaster())
	{
		// button is locked, play locked sound
		PlayLockSounds(pev, &m_ls, TRUE, BUTTON_SOUNDWAIT);
		return;
	}
	else
	{
		// button is unlocked, play unlocked sound
		PlayLockSounds(pev, &m_ls, FALSE, BUTTON_SOUNDWAIT);
	}

	if (pev->noise)
		EMIT_SOUND(ENT(pev), CHAN_VOICE, STRINGV(pev->noise), VOL_NORM, ATTN_NORM);

	ASSERT(m_toggle_state == TS_AT_BOTTOM);
	m_toggle_state = TS_GOING_UP;
	SetMoveDone(&CBaseButton::TriggerAndWait);
	if (!m_fRotating)
		LinearMove(m_vecPosition2, pev->speed);
	else
		AngularMove(m_vecAngle2, pev->speed);

	UpdateAllButtons(true);// XDM3037: disable all buttons that use my target!
	m_Disable = false;
}

// XDM3037
void CBaseButton::SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value)
{
	if (m_Disable == false)
		CBaseToggle::SUB_UseTargets(pActivator, useType, value);
	else
		ALERT(at_aiconsole, "CBaseButton::SUB_UseTargets() disabled\n");
}

//-----------------------------------------------------------------------------
// Purpose: Attempt to disable semi-simultaneous use of buttons that share
//			same target. It could be possible BUT! many buttons may target a
//			single multisource! The only solution I came up with is to rely on
//			mapper and search buttons by THEIR name rather than their target.
//			Especially useful for co-op.
// Note   : Buttons must have identical names.
// Input  : value - 
//-----------------------------------------------------------------------------
void CBaseButton::UpdateAllButtons(bool value)
{
	if (FStringNull(pev->targetname))
		return;

	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByTargetname(pEntity, STRING(pev->targetname))) != NULL)
	{
		if (pEntity == this)
			continue;
// custom Instance() does the trick		if (FClassnameIs(pentTarget, STRING(pev->classname)))
		CBaseButton *pButton = CBaseButton::Instance(pEntity->edict());
		if (pButton)
		{
			pButton->m_Disable = value;
			ALERT(at_aiconsole, "CBaseButton::UpdateAllButtons(%d): updating %d %s %s\n", value, pButton->entindex(), STRING(pButton->pev->classname), STRING(pButton->pev->targetname));
		}
	}
}

//
// Button has reached the "in/up" position.  Activate its "targets", and pause before "popping out".
//
void CBaseButton::TriggerAndWait(void)
{
	ASSERT(m_toggle_state == TS_GOING_UP);

	if (IsLockedByMaster())
		return;

	m_toggle_state = TS_AT_TOP;

	// If button automatically comes back out, start it moving out.
	// Else re-instate touch method
	if (m_fStayPushed || FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE))
	{
		if (!FBitSet(pev->spawnflags, SF_BUTTON_TOUCH_ONLY)) // this button only works if USED, not touched!
			SetTouchNull();// ALL buttons are now use only
		else
			SetTouch(&CBaseButton::ButtonTouch);

		UpdateAllButtons(false);// XDM3037
	}
	else
	{
		pev->nextthink = pev->ltime + m_flWait;
		SetThink(&CBaseButton::ButtonReturn);
	}

	pev->frame = 1;			// use alternate textures
	SUB_UseTargets(m_hActivator, USE_TOGGLE, 0);
}

//
// Starts the button moving "out/down".
//
void CBaseButton::ButtonReturn(void)
{
	ASSERT(m_toggle_state == TS_AT_TOP);
	m_toggle_state = TS_GOING_DOWN;

	SetMoveDone(&CBaseButton::ButtonBackHome);
	if (!m_fRotating)
		LinearMove(m_vecPosition1, pev->speed);
	else
		AngularMove(m_vecAngle1, pev->speed);

	pev->frame = 0;			// use normal textures
}


//
// Button has returned to start state.  Quiesce it.
//
void CBaseButton::ButtonBackHome(void)
{
	ASSERT(m_toggle_state == TS_GOING_DOWN);
	m_toggle_state = TS_AT_BOTTOM;

	if (FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE))
	{
		//EMIT_SOUND(ENT(pev), CHAN_VOICE, STRINGV(pev->noise), VOL_NORM, ATTN_NORM);
		SUB_UseTargets(m_hActivator, USE_TOGGLE, 0);
	}

	if (!FStringNull(pev->target))
	{
		edict_t* pentTarget	= NULL;
		for (;;)
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME(pentTarget, STRING(pev->target));

			if (FNullEnt(pentTarget))
				break;

			if (!FClassnameIs(pentTarget, "multisource"))
				continue;

			CBaseEntity *pTarget = CBaseEntity::Instance(pentTarget);
			if (pTarget)
				pTarget->Use(m_hActivator, this, USE_TOGGLE, 0);
		}
	}

	UpdateAllButtons(false);// XDM3037

	// Re-instate touch method, movement cycle is complete.
	if (!FBitSet(pev->spawnflags, SF_BUTTON_TOUCH_ONLY)) // this button only works if USED, not touched!
		SetTouchNull();// All buttons are now use only
	else
		SetTouch(&CBaseButton::ButtonTouch);

	// reset think for a sparking button
	if (FBitSet(pev->spawnflags, SF_BUTTON_SPARK_IF_OFF))
	{
		SetThink(&CBaseButton::ButtonSpark);
		pev->nextthink = gpGlobals->time + 0.5;// no hurry.
	}
}



//
// Rotating button (aka "lever")
//
class CRotButton : public CBaseButton
{
public:
	virtual void Spawn(void);
};

LINK_ENTITY_TO_CLASS( func_rot_button, CRotButton );

void CRotButton::Spawn(void)
{
	char *pszSound;
	//----------------------------------------------------
	//determine sounds for buttons
	//a sound of 0 should not make a sound
	//----------------------------------------------------
	pszSound = ButtonSound(m_sounds);
	PRECACHE_SOUND(pszSound);
	pev->noise = ALLOC_STRING(pszSound);

	// set the axis of rotation
	CBaseToggle::AxisDir();

	// check for clockwise rotation
	if (pev->spawnflags & SF_DOOR_ROTATE_BACKWARDS)
		pev->movedir = pev->movedir * -1;

	pev->movetype	= MOVETYPE_PUSH;

	if (pev->spawnflags & SF_ROTBUTTON_NOTSOLID)
		pev->solid		= SOLID_NOT;
	else
		pev->solid		= SOLID_BSP;

	SET_MODEL(ENT(pev), STRING(pev->model));

	if (pev->speed == 0)
		pev->speed = 40;

	if (m_flWait == 0)
		m_flWait = 1;

//	if (pev->health > 0)
		pev->takedamage = DAMAGE_YES;

	m_toggle_state = TS_AT_BOTTOM;
	m_vecAngle1	= pev->angles;
	m_vecAngle2	= pev->angles + pev->movedir * m_flMoveDistance;
	ASSERTSZ(m_vecAngle1 != m_vecAngle2, "rotating button start/end positions are equal");

	m_fStayPushed = (m_flWait == -1 ? TRUE : FALSE);
	m_fRotating = TRUE;

	// if the button is flagged for USE button activation only, take away it's touch function and add a use function
	if (!FBitSet(pev->spawnflags, SF_BUTTON_TOUCH_ONLY))
	{
		SetTouchNull();
		SetUse(&CBaseButton::ButtonUse);
	}
	else // touchable button
		SetTouch(&CBaseButton::ButtonTouch);

	//SetTouch(&CBaseButton::ButtonTouch);
}


// Make this button behave like a door (HACKHACK)
// This will disable use and make the button solid
// rotating buttons were made SOLID_NOT by default since their were some
// collision problems with them...
#define SF_MOMENTARY_DOOR		0x0001

#define SF_MOMROTBTN_AUTO_RETURN		16

class CMomentaryRotButton : public CBaseToggle
{
public:
	virtual void Spawn(void);
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int	ObjectCaps(void)
	{
		int flags = CBaseToggle::ObjectCaps() & (~FCAP_ACROSS_TRANSITION);
		if ( pev->spawnflags & SF_MOMENTARY_DOOR )
			return flags;
		return flags | FCAP_CONTINUOUS_USE;
	}
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	void	EXPORT Off(void);
	void	EXPORT Return(void);

	void	UpdateSelf(float &value);
	void	UpdateSelfReturn(float &value);
	void	UpdateAllButtons(float &value, int start);

	void	PlaySound(void);
	void	UpdateTarget(float &value);

	static CMomentaryRotButton *Instance(edict_t *pent) { return (CMomentaryRotButton *)GET_PRIVATE(pent);};
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	int		m_lastUsed;
	int		m_direction;
	float	m_returnSpeed;
	vec3_t	m_start;
	vec3_t	m_end;
	int		m_sounds;
};
TYPEDESCRIPTION CMomentaryRotButton::m_SaveData[] =
{
	DEFINE_FIELD( CMomentaryRotButton, m_lastUsed, FIELD_INTEGER ),
	DEFINE_FIELD( CMomentaryRotButton, m_direction, FIELD_INTEGER ),
	DEFINE_FIELD( CMomentaryRotButton, m_returnSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CMomentaryRotButton, m_start, FIELD_VECTOR ),
	DEFINE_FIELD( CMomentaryRotButton, m_end, FIELD_VECTOR ),
	DEFINE_FIELD( CMomentaryRotButton, m_sounds, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CMomentaryRotButton, CBaseToggle );

LINK_ENTITY_TO_CLASS( momentary_rot_button, CMomentaryRotButton );

void CMomentaryRotButton::Spawn(void)
{
	CBaseToggle::AxisDir();

	if ( pev->speed == 0 )
		pev->speed = 100;

	if ( m_flMoveDistance < 0 )
	{
		m_start = pev->angles + pev->movedir * m_flMoveDistance;
		m_end = pev->angles;
		m_direction = 1;		// This will toggle to -1 on the first use()
		m_flMoveDistance = -m_flMoveDistance;
	}
	else
	{
		m_start = pev->angles;
		m_end = pev->angles + pev->movedir * m_flMoveDistance;
		m_direction = -1;		// This will toggle to +1 on the first use()
	}

	if ( pev->spawnflags & SF_MOMENTARY_DOOR )
		pev->solid		= SOLID_BSP;
	else
		pev->solid		= SOLID_NOT;

	pev->movetype	= MOVETYPE_PUSH;
	UTIL_SetOrigin(pev, pev->origin);
	SET_MODEL(ENT(pev), STRING(pev->model) );

	char *pszSound = ButtonSound( m_sounds );
	PRECACHE_SOUND(pszSound);
	pev->noise = ALLOC_STRING(pszSound);
	m_lastUsed = 0;
}

void CMomentaryRotButton::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "returnspeed"))
	{
		m_returnSpeed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "sounds"))
	{
		m_sounds = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CMomentaryRotButton::PlaySound(void)
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, STRINGV(pev->noise), VOL_NORM, ATTN_NORM);
}

// BUGBUG: This design causes a latentcy.  When the button is retriggered, the first impulse
// will send the target in the wrong direction because the parameter is calculated based on the
// current, not future position.
void CMomentaryRotButton::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	pev->ideal_yaw = CBaseToggle::AxisDelta( pev->spawnflags, pev->angles, m_start ) / m_flMoveDistance;

	UpdateAllButtons( pev->ideal_yaw, 1 );
	UpdateTarget( pev->ideal_yaw );
}

void CMomentaryRotButton::UpdateAllButtons(float &value, int start)
{
	// Update all rot buttons attached to the same target
	edict_t *pentTarget = NULL;
	for (;;)
	{
		pentTarget = FIND_ENTITY_BY_TARGET(pentTarget, STRING(pev->target));
		if (FNullEnt(pentTarget))
			break;

		if (FClassnameIs(pentTarget, STRING(pev->classname)))
		{
			CMomentaryRotButton *pEntity = CMomentaryRotButton::Instance(pentTarget);
			if (pEntity)
			{
				if (start)
					pEntity->UpdateSelf( value );
				else
					pEntity->UpdateSelfReturn( value );
			}
		}
	}
}

void CMomentaryRotButton::UpdateSelf(float &value)
{
	BOOL fplaysound = FALSE;

	if ( !m_lastUsed )
	{
		fplaysound = TRUE;
		m_direction = -m_direction;
	}
	m_lastUsed = 1;

	pev->nextthink = pev->ltime + 0.1;
	if ( m_direction > 0 && value >= 1.0 )
	{
		pev->avelocity = g_vecZero;
		pev->angles = m_end;
		return;
	}
	else if ( m_direction < 0 && value <= 0 )
	{
		pev->avelocity = g_vecZero;
		pev->angles = m_start;
		return;
	}

	if (fplaysound)
		PlaySound();

	// HACKHACK -- If we're going slow, we'll get multiple player packets per frame, bump nexthink on each one to avoid stalling
	if ( pev->nextthink < pev->ltime )
		pev->nextthink = pev->ltime + 0.1;
	else
		pev->nextthink += 0.1;

	pev->avelocity = (m_direction * pev->speed) * pev->movedir;
	SetThink(&CMomentaryRotButton::Off);
}

void CMomentaryRotButton::UpdateTarget(float &value)
{
//	ALERT(at_console, "CMomentaryRotButton::UpdateTarget(%f)\n", value);

	if (!FStringNull(pev->target))
	{
		edict_t* pentTarget	= NULL;
		for (;;)// SLOW!
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME(pentTarget, STRING(pev->target));
			if (FNullEnt(pentTarget))
				break;

			CBaseEntity *pEntity = CBaseEntity::Instance(pentTarget);
			if (pEntity)
				pEntity->Use(this, this, USE_SET, value);
		}
	}
}

void CMomentaryRotButton::Off(void)
{
	pev->avelocity = g_vecZero;
	m_lastUsed = 0;
	if ( FBitSet( pev->spawnflags, SF_MOMROTBTN_AUTO_RETURN ) && m_returnSpeed > 0 )
	{
		SetThink(&CMomentaryRotButton::Return);
		pev->nextthink = pev->ltime + 0.1;
		m_direction = -1;
	}
	else
		SetThinkNull();
}

void CMomentaryRotButton::Return(void)
{
	float value = CBaseToggle::AxisDelta( pev->spawnflags, pev->angles, m_start ) / m_flMoveDistance;

	UpdateAllButtons( value, 0 );	// This will end up calling UpdateSelfReturn() n times, but it still works right
	if ( value > 0 )
		UpdateTarget( value );
}

void CMomentaryRotButton::UpdateSelfReturn(float &value)
{
	if ( value <= 0 )
	{
		pev->avelocity = g_vecZero;
		pev->angles = m_start;
		pev->nextthink = -1;
		SetThinkNull();
	}
	else
	{
		pev->avelocity = -m_returnSpeed * pev->movedir;
		pev->nextthink = pev->ltime + 0.1;
	}
}


#define SF_BTARGET_USE		0x0001
#define SF_BTARGET_ON		0x0002

class CButtonTarget : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual int ObjectCaps(void);
};

LINK_ENTITY_TO_CLASS( button_target, CButtonTarget );

void CButtonTarget::Spawn(void)
{
	pev->movetype	= MOVETYPE_PUSH;
	pev->solid		= SOLID_BSP;
	SET_MODEL(ENT(pev), STRING(pev->model));
	pev->takedamage = DAMAGE_YES;

	if ( FBitSet( pev->spawnflags, SF_BTARGET_ON ) )
		pev->frame = 1;
}

void CButtonTarget::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (!ShouldToggle(useType, ((int)pev->frame > 0)))// XDM3037: type
		return;

	pev->frame = 1-pev->frame;
	if ( pev->frame )
		SUB_UseTargets( pActivator, USE_ON, 0 );
	else
		SUB_UseTargets( pActivator, USE_OFF, 0 );
}

int	CButtonTarget::ObjectCaps(void)
{
	int caps = CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION;

	if ( FBitSet(pev->spawnflags, SF_BTARGET_USE) )
		return caps | FCAP_IMPULSE_USE;
	else
		return caps;
}

int CButtonTarget::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	Use(pAttacker, this, USE_TOGGLE, 0);
	return 1;
}
