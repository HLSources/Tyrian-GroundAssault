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
#ifndef DOORS_H
#define DOORS_H

#include "sound.h"

// Macros to make entvars more user-friendly
#define noiseMoving noise
#define noiseArrived noise1

// doors
#define SF_DOOR_ROTATE_Y			0
#define	SF_DOOR_START_OPEN			1
#define SF_DOOR_ROTATE_BACKWARDS	2
#define SF_DOOR_DONT_LINK			4// is this still used?
#define SF_DOOR_PASSABLE			8
#define SF_DOOR_ONEWAY				16
#define	SF_DOOR_NO_AUTO_RETURN		32// toggle
#define SF_DOOR_ROTATE_Z			64
#define SF_DOOR_ROTATE_X			128
#define SF_DOOR_USE_ONLY			256	// door must be opened by player's use button.
#define SF_DOOR_NOMONSTERS			512	// Monster can't open
#define SF_DOOR_SILENT				0x80000000

// emit door moving and stop sounds on CHAN_STATIC so that the multicast doesn't
// filter them out and leave a client stuck with looping door sounds!
#define DOOR_SNDCHAN_MOVING			CHAN_STATIC
#define DOOR_SNDCHAN_ARRIVED		CHAN_STATIC			

#define DOOR_SOUNDWAIT		3
#define BUTTON_SOUNDWAIT	0.5


class CBaseDoor : public CBaseToggle
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual void Blocked(CBaseEntity *pOther);
	virtual int	ObjectCaps(void)
	{
		if (pev->spawnflags & SF_ITEM_USE_ONLY)
			return (CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_IMPULSE_USE;
		else
			return (CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION);
	};
	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);

	static	TYPEDESCRIPTION m_SaveData[];

	virtual void SetToggleState(int state);
	virtual BOOL IsBSPModel(void) { return TRUE; }// XDM
	virtual BOOL IsMovingBSP(void) { return TRUE; }// XDM
	virtual BOOL IsRotating(void) { return FALSE; }// XDM

	virtual void StartSound(void);// XDM
	virtual void StopSound(void);// XDM

	// used to selectivly override defaults
	void EXPORT DoorTouch(CBaseEntity *pOther);
	void EXPORT DoorTouchDmgOnly(CBaseEntity *pOther);// XDM: used instead of NULL

	// local functions
	int DoorActivate(void);
	void EXPORT DoorGoUp(void);
	void EXPORT DoorGoDown(void);
	void EXPORT DoorHitTop(void);
	void EXPORT DoorHitBottom(void);

	static const char *MoveSounds[];// XDM
	static const char *StopSounds[];// XDM

	byte	m_bHealthValue;// some doors are medi-kit doors, they give players health
	byte	m_bMoveSnd;			// sound a door makes while moving
	byte	m_bStopSnd;			// sound a door makes when it stops
	locksound_t m_ls;			// door lock sounds
	byte m_bLockedSound;		// ordinals from entity selection
	byte m_bLockedSentence;
	byte m_bUnlockedSound;
	byte m_bUnlockedSentence;
	byte m_bDontDraw;// XDM // 0-draw always, 1-dd opn, 2-dd closed
};


class CRotDoor : public CBaseDoor
{
public:
	virtual void Spawn(void);
	virtual void SetToggleState(int state);
	virtual BOOL IsRotating(void) { return TRUE; }// XDM
};


class CMomentaryDoor : public CBaseDoor//CBaseToggle
{
public:
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
//	virtual int	ObjectCaps(void) { return CBaseToggle :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};


#endif		//DOORS_H
