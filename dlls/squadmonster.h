/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
#ifndef SQUADMONSTER_H
#define SQUADMONSTER_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include "basemonster.h"

//=========================================================
// CSquadMonster - all the extra data for monsters that
// form squads.
//=========================================================

#define	SF_SQUADMONSTER_LEADER	32


#define bits_NO_SLOT		0

// HUMAN GRUNT SLOTS
#define bits_SLOT_HGRUNT_ENGAGE1	( 1 << 0 )
#define bits_SLOT_HGRUNT_ENGAGE2	( 1 << 1 )
#define bits_SLOTS_HGRUNT_ENGAGE	( bits_SLOT_HGRUNT_ENGAGE1 | bits_SLOT_HGRUNT_ENGAGE2 )

#define bits_SLOT_HGRUNT_GRENADE1	( 1 << 2 ) 
#define bits_SLOT_HGRUNT_GRENADE2	( 1 << 3 ) 
#define bits_SLOTS_HGRUNT_GRENADE	( bits_SLOT_HGRUNT_GRENADE1 | bits_SLOT_HGRUNT_GRENADE2 )

// ALIEN GRUNT SLOTS
#define bits_SLOT_AGRUNT_HORNET1	( 1 << 4 )
#define bits_SLOT_AGRUNT_HORNET2	( 1 << 5 )
#define bits_SLOT_AGRUNT_CHASE		( 1 << 6 )
#define bits_SLOTS_AGRUNT_HORNET	( bits_SLOT_AGRUNT_HORNET1 | bits_SLOT_AGRUNT_HORNET2 )

// HOUNDEYE SLOTS
#define bits_SLOT_HOUND_ATTACK1		( 1 << 7 )
#define bits_SLOT_HOUND_ATTACK2		( 1 << 8 )
#define bits_SLOT_HOUND_ATTACK3		( 1 << 9 )
#define bits_SLOTS_HOUND_ATTACK		( bits_SLOT_HOUND_ATTACK1 | bits_SLOT_HOUND_ATTACK2 | bits_SLOT_HOUND_ATTACK3 )

// global slots
#define bits_SLOT_SQUAD_SPLIT		( 1 << 10 )// squad members don't all have the same enemy

#define NUM_SLOTS			11// update this every time you add/remove a slot.

#define	MAX_SQUAD_MEMBERS	5

//=========================================================
// CSquadMonster - for any monster that forms squads.
//=========================================================
class CSquadMonster : public CBaseMonster 
{
public:
	// squad leader info
	EHANDLE	m_hSquadLeader;		// who is my leader
	EHANDLE	m_hSquadMember[MAX_SQUAD_MEMBERS-1];	// valid only for leader
	int		m_afSquadSlots;
	float	m_flLastEnemySightTime; // last time anyone in the squad saw the enemy
	BOOL	m_fEnemyEluded;

	// squad member info
	int		m_iMySlot;// this is the behaviour slot that the monster currently holds in the squad. 

	virtual int  CheckEnemy ( CBaseEntity *pEnemy );
	virtual void StartMonster ( void );
	virtual void ScheduleChange( void );
	virtual void Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib);
	virtual BOOL NoFriendlyFire( void );
	virtual BOOL NoFriendlyFire( BOOL playerAlly );// XDM

	virtual BOOL OccupySlot(int iDesiredSlot);
	virtual void VacateSlot(void);

	// squad functions still left in base class
	CSquadMonster *MySquadLeader(void) ;
	CSquadMonster *MySquadMember(int i);

	virtual bool InSquad(void) { return m_hSquadLeader != NULL; }
	virtual bool IsLeader(void) { return m_hSquadLeader == this; }
//	virtual int SquadJoin ( int searchRadius );
	virtual int SquadRecruit ( int searchRadius, int maxMembers );
	virtual int	SquadCount( void );
	virtual void SquadRemove( CSquadMonster *pRemove );
	virtual void SquadUnlink(void);
	virtual BOOL SquadAdd( CSquadMonster *pAdd );
	virtual void SquadDisband( void );
//	virtual void SquadAddConditions ( int iConditions );
	virtual void SquadMakeEnemy ( CBaseEntity *pEnemy );
	virtual void SquadPasteEnemyInfo ( void );
	virtual void SquadCopyEnemyInfo ( void );
	virtual BOOL SquadEnemySplit ( void );
	virtual BOOL SquadMemberInRange( const Vector &vecLocation, float flDist );

	virtual CSquadMonster *MySquadMonsterPointer( void ) { return this; }

	static TYPEDESCRIPTION m_SaveData[];

	virtual int	Save( CSave &save ); 
	virtual int Restore( CRestore &restore );

	virtual BOOL FValidateCover ( const Vector &vecCoverLocation );

	virtual MONSTERSTATE GetIdealState ( void );
	virtual Schedule_t	*GetScheduleOfType ( int iType );
};


#endif // SQUADMONSTER_H
