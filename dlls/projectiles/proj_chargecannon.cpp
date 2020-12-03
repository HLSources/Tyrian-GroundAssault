#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "soundent.h"
#include "globals.h"
#include "game.h"
#include "msg_fx.h"
#include "projectiles.h"
#include "decals.h"
#include "customentity.h"

LINK_ENTITY_TO_CLASS(ChargeCannon, CChargeCannon);

void CChargeCannon::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	pev->body = PROJ_BODY_CHARGECANNON;
	pev->takedamage = DAMAGE_YES;
	pev->health = 2;
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_SLIDEBOX;
	UTIL_SetSize(this, 3.0f);
	FX_On();
	pev->nextthink = gpGlobals->time;

	SetTouch(&CChargeCannon::ExplodeTouch);
	SetThink(&CChargeCannon::Fly);
}

CChargeCannon *CChargeCannon::ShootChargeCannon(CBaseEntity *pOwner, const Vector &vecStart, float spread, float dmg)
{
	CChargeCannon *pNew = GetClassPtr((CChargeCannon *)NULL, "ChargeCannon");
	if (pNew)
	{
		if (pOwner)
		{
			pNew->pev->owner = pOwner->edict();
			pNew->pev->team = pOwner->pev->team;
		}

		pNew->Spawn();
		pNew->pev->origin = vecStart;
		pNew->pev->velocity = gpGlobals->v_forward * SPEED_CHARGECANNON + gpGlobals->v_right * RANDOM_FLOAT(-spread,spread) + gpGlobals->v_up * RANDOM_FLOAT(-spread,spread);
		pNew->pev->angles = UTIL_VecToAngles (pNew->pev->velocity);
		pNew->pev->dmg = dmg;
		FX_Trail(pNew->pev->origin, pNew->entindex(), FX_CHARGECANNON);
	}
	return pNew;
}

int CChargeCannon::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pAttacker->edict() == pev->owner)
		return 0;

	if (!(bitsDamageType & DMGM_BREAK))
		return 0;

	FX_Off();
	CGrenade::Blast(0, 0, 0, 0, 0, FX_CHARGECANNON_BLAST);
	return 1;
}

void CChargeCannon::ExplodeTouch(CBaseEntity *pOther)
{
	FX_Off();
	CGrenade::Impact(pOther, TRUE, 0, 0, TRUE, RANDOM_LONG(DECAL_OFSCORCH1,DECAL_OFSCORCH3), 0, 0, pev->dmg, pev->dmg, DMG_ENERGYBEAM | DMG_RADIUS_MAX, 16.0, FX_CHARGECANNON_DETONATE);
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time;
}

void CChargeCannon::Fly( void )
{
	if (POINT_CONTENTS(pev->origin) == CONTENTS_SKY )
	{
		FX_Off();
		CGrenade::RemoveIfInSky();
		return;
	}
	if (pev->waterlevel > 0)
	{
		FX_Off();
		CGrenade::Blast(0, 0, 0, 0, 0, FX_CHARGECANNON_BLAST);
		return;
	}
	CGrenade::SearchTarget();
	pev->nextthink = gpGlobals->time + 0.01;
}

void CChargeCannon::FX_On( void )
{
	Vector	posGun, angleGun;
	TraceResult trace;

	m_pBeam = CBeam::BeamCreate( "sprites/beams_all.spr", 30 );
	GetAttachment( 1, posGun, angleGun );
	GetAttachment( 2, posGun, angleGun );

	Vector vecEnd = (gpGlobals->v_forward * 60) + posGun;
	UTIL_TraceLine( posGun, vecEnd, dont_ignore_monsters, edict(), &trace );

	m_pBeam->EntsInit( entindex(), entindex() );
	m_pBeam->SetStartAttachment( 1 );
	m_pBeam->SetEndAttachment( 2 );
	m_pBeam->SetBrightness( 190 );
	m_pBeam->SetScrollRate( 20 );
	m_pBeam->SetNoise( 20 );
	m_pBeam->SetFlags( BEAM_FSHADEOUT );
	m_pBeam->SetColor( 35, 214, 177 );
	m_pBeam->SetFrame(BLAST_SKIN_LIGHTNING);

	m_pNoise = CBeam::BeamCreate( "sprites/beams_all.spr", 30 );

	GetAttachment( 1, posGun, angleGun );
	GetAttachment( 2, posGun, angleGun );

	UTIL_TraceLine( posGun, vecEnd, dont_ignore_monsters, edict(), &trace );

	m_pNoise->EntsInit( entindex(), entindex() );
	m_pNoise->SetStartAttachment( 1 );
	m_pNoise->SetEndAttachment( 2 );
	m_pNoise->SetBrightness( 190 );
	m_pNoise->SetScrollRate( 20 );
	m_pNoise->SetNoise( 65 );
	m_pNoise->SetFlags( BEAM_FSHADEOUT );
	m_pNoise->SetColor( 255, 255, 173 );
	m_pNoise->SetFrame(BLAST_SKIN_LIGHTNING);
	EXPORT RelinkBeam();
}

void CChargeCannon::FX_Off( void )
{
	UTIL_Remove( m_pBeam );
	m_pBeam = NULL;
	UTIL_Remove( m_pNoise );
	m_pNoise = NULL;
}