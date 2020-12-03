//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "explode.h"
#include "trains.h"
#include "items.h"
#include "globals.h"

// NOTE: most of this code is completely obsolete

// UPD: discontinued project
LINK_ENTITY_TO_CLASS(func_wall_computer, CPointEntity);
LINK_ENTITY_TO_CLASS(env_teleporter, CPointEntity);

/*
enum wallcomp_skin_e
{
	SKIN_OFF = 0,
	SKIN_WORK,
	SKIN_READY,
	SKIN_ERROR,
};

enum wallcomp_e
{
	WALLCOMP_WALL = 0,
	WALLCOMP_WORK,
	WALLCOMP_TURN_ON,
	WALLCOMP_TURN_OFF,
};

class CWallComp : public CBaseToggle
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	void EXPORT TurnOff(void);
	void EXPORT TurnOn(void);
	void EXPORT CompThink(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual void TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType);
	virtual int TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType);
	virtual int	ObjectCaps(void) {return FCAP_ACROSS_TRANSITION | FCAP_IMPULSE_USE;}
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];
	int m_iOn;
	float m_flSoundTime;
	CBasePlayer *pPlayer;
};

TYPEDESCRIPTION CWallComp::m_SaveData[] =
{
	DEFINE_FIELD(CWallComp, m_iOn, FIELD_INTEGER),
	DEFINE_FIELD(CWallComp, m_flSoundTime, FIELD_TIME),
};

IMPLEMENT_SAVERESTORE(CWallComp, CBaseToggle);

LINK_ENTITY_TO_CLASS(func_wall_computer, CWallComp);

void CWallComp::Spawn(void)
{
	Precache();
	pev->solid		= SOLID_BBOX;
	pev->movetype	= MOVETYPE_NONE;
	UTIL_SetOrigin(pev, pev->origin);
	UTIL_SetSize(pev, Vector(-32,-24,-6), Vector(32,24,6));
	SET_MODEL(ENT(pev), "models/wallcomp.mdl");
	pev->skin = SKIN_OFF;
	pev->sequence = WALLCOMP_WALL;
	pev->frame = 0;
	pev->takedamage = DAMAGE_YES;
	pev->health = 160;
	m_iOn = 0;
	SetTouchNull();
	SetThink(CompThink);
}

void CWallComp::Precache(void)
{
	PRECACHE_MODEL("models/wallcomp.mdl");
	PRECACHE_SOUND("items/wcomp_off.wav");
	PRECACHE_SOUND("items/wcomp_on.wav");
	PRECACHE_SOUND("items/wcomp_work.wav");
	PRECACHE_SOUND("items/wcomp_no.wav");
}

void CWallComp::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!pActivator)
		return;

	if (!pActivator->IsPlayer())
		return;

	pPlayer = (CBasePlayer *)pActivator;
	if (pPlayer->m_rgItems[ITEM_SECURITY] > 0)
	{
		if (m_iOn)
			SetThink(TurnOff);
		else
			SetThink(TurnOn);
	}
	else if (m_flSoundTime <= gpGlobals->time)
	{
		m_flSoundTime = gpGlobals->time + 2.0;
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "items/wcomp_no.wav", 1.0, ATTN_NORM);
	}
	pev->nextthink = gpGlobals->time + 1.0;
}

void CWallComp::CompThink(void)
{
	if (m_iOn)
	{
		if (!pev->team)
		{
//			MESSAGE_BEGIN(MSG_ONE, gmsgVGUIMenu, NULL, pPlayer->pev);
//				WRITE_BYTE(MENU_MAPBRIEFING);// test
//			MESSAGE_END();
			EMIT_SOUND(ENT(pev), CHAN_STATIC, "items/wcomp_work.wav", 1.0, ATTN_NORM);
			pev->team = 1;
		}
		EntityLight(entindex(), pev->origin, 32, 80, 160, 255, 10, 256);
		pev->skin = SKIN_WORK;
		pev->sequence = WALLCOMP_WORK;
	}
	else
	{
		if (pev->sequence != WALLCOMP_WALL)
			pev->sequence = WALLCOMP_WALL;
	}
	pev->nextthink = gpGlobals->time + 1;
}

void CWallComp::TurnOn(void)
{
	EMIT_SOUND(ENT(pev), CHAN_STATIC, "items/wcomp_on.wav", 1.0, ATTN_NORM);
	pev->sequence = WALLCOMP_TURN_ON;
	m_iOn = 1;
	pev->team = 0;
	pev->skin = SKIN_READY;
	SetThink(CompThink);
	pev->nextthink = gpGlobals->time + 0.5;// sound length
}

void CWallComp::TurnOff(void)
{
	pev->skin = SKIN_READY;
	STOP_SOUND(ENT(pev), CHAN_STATIC, "items/wcomp_work.wav");
	EMIT_SOUND(ENT(pev), CHAN_STATIC, "items/wcomp_off.wav", 1.0, ATTN_NORM);
	pev->team = 0;
	pev->sequence = WALLCOMP_TURN_OFF;
	pev->skin = SKIN_OFF;
	m_iOn = 0;
	SetThink(CompThink);
	pev->nextthink = gpGlobals->time + 0.5;// sound length
}

void CWallComp::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (pev->dmgtime != gpGlobals->time || (RANDOM_LONG(0,10) < 3))
	{
		UTIL_Ricochet(ptr->vecEndPos, RANDOM_FLOAT(1, 2));
		pev->dmgtime = gpGlobals->time;
	}
}

int CWallComp::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	UTIL_Ricochet(pev->origin, 1);
	if ( m_iOn > 0)
	{
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "items/wcomp_no.wav", 1.0, ATTN_NORM );
		SetThink( TurnOff );
	}
	return pInflictor, pAttacker, flDamage, bitsDamageType;
}

*/





































/*
enum teleporter_e
{
	TELE_CLOSE = 0,
	TELE_CLOSED,
	TELE_OPEN,
	TELE_OPENED,
	TELE_WORK
};

class CRingTeleporter : public CBaseAnimating
{
public:
	void Spawn(void);
	void Precache(void);
	void EXPORT CloseThink(void);
	void EXPORT ClosedThink(void);
	void EXPORT OpenThink(void);
	void EXPORT OpenedThink(void);
	void EXPORT WorkThink(void);
	void TeleportEntity(CBaseEntity *pEnt);
	void CatchEntity(void);
	void Touch(CBaseEntity *pOther);
	void SetSequence(const char *seq);
	virtual int	ObjectCaps( void ) { return CBaseAnimating::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];
	BOOL m_fSequenceFinished;// flag set when StudioAdvanceFrame moves across a frame boundry
	int state;
	CSprite *m_pGlow[5];
};

LINK_ENTITY_TO_CLASS( env_teleporter, CRingTeleporter );

TYPEDESCRIPTION	CRingTeleporter::m_SaveData[] =
{
	DEFINE_FIELD( CRingTeleporter, state, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE(CRingTeleporter, CBaseAnimating);

void CRingTeleporter::Spawn(void)
{
	Precache();
	SET_MODEL( ENT(pev), "models/teleporter.mdl" );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_TRIGGER;
	UTIL_SetSize(pev, Vector(-100,-100,0), Vector(100,100,32));
	SetThink(ClosedThink);
	pev->nextthink = gpGlobals->time + 0.1;
	pev->frame = 0;
	for(int i = 0; i<6; i++)
	{
		m_pGlow[i] = CSprite::SpriteCreate("sprites/teleporter.spr", pev->origin, TRUE);
		m_pGlow[i]->SetTransparency(kRenderGlow, 255, 255, 255, 0, kRenderFxNoDissipation);
		m_pGlow[i]->SetAttachment(edict(), i);
	}
}

void CRingTeleporter::Precache(void)
{
	PRECACHE_MODEL("models/teleporter.mdl");
	PRECACHE_MODEL("sprites/teleporter.spr");
}

void CRingTeleporter::CloseThink(void)
{
	if (state != TELE_CLOSE)
	{
		state = TELE_CLOSE;
		SetSequence("close");
	}

	if (m_fSequenceFinished)
		SetThink(ClosedThink);
}

void CRingTeleporter::ClosedThink(void)
{
	if (state != TELE_CLOSED)
	{
		state = TELE_CLOSED;
		SetSequence("closed");
	}
}

void CRingTeleporter::OpenThink(void)
{
	if (state != TELE_OPEN)
	{
		state = TELE_OPEN;
		SetSequence("open");
	}

	if (m_fSequenceFinished)
		SetThink(OpenedThink);
}

void CRingTeleporter::OpenedThink(void)
{
	if (state != TELE_OPENED)
	{
		state = TELE_OPENED;
		SetSequence("opened");
	}
}

void CRingTeleporter::WorkThink(void)
{
	if (state != TELE_WORK)
	{
		state = TELE_WORK;
		SetSequence("work");
	}

	if ((pev->frame > 8)||(pev->frame < 19))
	{
		for(int i = 0; i<6; i++)
		{
			m_pGlow[i]->SetTransparency(kRenderGlow, 255, 255, 255, 255, kRenderFxNoDissipation);
			m_pGlow[i]->SetAttachment(edict(), i);
			m_pGlow[i]->AnimateAndDie(10);
		}
	}
//9, 10, 11, 12 go down
//13, 14 idle
//15, 16, 17, 18 go up
	if (m_fSequenceFinished)
		SetThink(CloseThink);
}

void CRingTeleporter::Touch(CBaseEntity *pOther)
{
	if (state == TELE_CLOSED)
	{
		SetThink(OpenThink);
	}
	else if (state == TELE_OPENED)
	{
		SetThink(WorkThink);
	}
}

void CRingTeleporter::TeleportEntity(CBaseEntity *pEnt)
{
}

void CRingTeleporter::CatchEntity(void)
{
}

void CRingTeleporter::SetSequence(const char *seq)
{
	int sequence = LookupSequence(seq);
	if (sequence != -1)
	{
		pev->sequence = sequence;
		pev->frame = 0;
		ResetSequenceInfo();
	}
}

*/














//////////////////////////// see explode.h //////////////////////////////////////
// !!!! OBSOLETE
/*
LINK_ENTITY_TO_CLASS( explosion_sphere, CExplosionSphere );

// Don't forget to Init(); !!!!!!
CExplosionSphere *CExplosionSphere::CreateSphere(Vector vecOrigin, Vector vecAngles, const char *model)
{
	CExplosionSphere *pSphere = GetClassPtr((CExplosionSphere *)NULL, "explosion_sphere");
	pSphere->pev->origin = vecOrigin;
	pSphere->pev->angles = vecAngles;
	pSphere->Spawn();
	SET_MODEL(pSphere->edict(), model);
	UTIL_SetOrigin(pSphere->pev, vecOrigin);
	return pSphere;
}

CExplosionSphere *CExplosionSphere::CreateSphere(Vector vecOrigin, Vector vecAngles, int modelindex)
{
	CExplosionSphere *pSphere = GetClassPtr((CExplosionSphere *)NULL, "explosion_sphere");
	pSphere->pev->origin = vecOrigin;
	pSphere->pev->angles = vecAngles;
	pSphere->Spawn();
	pSphere->pev->modelindex = modelindex;
	UTIL_SetOrigin(pSphere->pev, vecOrigin);
	return pSphere;
}

// Call Init() after this!
void CExplosionSphere::Spawn(void)
{
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->gravity = 0;
	pev->speed = 0;
//	pev->ltime = gpGlobals->time;
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
}

void CExplosionSphere::Precache(void)
{
	PRECACHE_MODEL("models/explodeball.mdl");
}*/
/* CExplosionSphere::Init();
*	int rendermode;			// will be used as pev->rendermode
*	int startrenderamt;		// will be used as pev->renderamt at the beginning
*	int endrenderamt;		// will be used as pev->renderamt at the end
*	float startscale;		// start scale factor
*	float endscale;			// end scale factor
*	float life;				// life time
*	float delay;			// delay between frames (nextthink)
*	Vector startcolor;		// will be used as pev->rendercolor at the beginning
*	Vector endcolor;		// will be used as pev->rendercolor at the end*/
/*void CExplosionSphere::Init(float life, float delay, int rendermode, int renderfx,
							int startrenderamt, int endrenderamt,
							float startscale, float endscale,
							Vector startcolor, Vector endcolor)
{
//	pev->impulse = life / delay;// number of frames (how many times ExplodeThink() is called)
	pev->animtime = delay;
	pev->scale = startscale;
	pev->rendermode = rendermode;
	pev->renderfx = renderfx;
	pev->renderamt = startrenderamt;
	pev->rendercolor = startcolor;
	dRenderAmt = (endrenderamt - startrenderamt)/(life/delay);
	dScale = (endscale - startscale)/(life/delay);
	dColor = (endcolor - startcolor)/(life/delay);
	pev->dmgtime = gpGlobals->time + life;
	pev->nextthink = gpGlobals->time + delay;
	SetThink(ExplodeThink);
}

void CExplosionSphere::ExplodeThink(void)
{*/
/*	pev->scale += (fB - fA)/pev->impulse;
*	pev->renderamt += (iB - iA)/pev->impulse;
*	pev->rendercolor.x += (vB.x - vA.x)/pev->impulse;
*	pev->rendercolor.y += (vB.y - vA.y)/pev->impulse;
*	pev->rendercolor.z += (vB.z - vA.z)/pev->impulse;*/
/*	pev->renderamt += dRenderAmt;
	pev->rendercolor = pev->rendercolor + dColor;
	pev->scale += dScale;

	if (pev->dmgtime < gpGlobals->time)
		UTIL_Remove(this);

	pev->nextthink = gpGlobals->time + pev->animtime;
}
*/


#define SF_SMOKE_START_OFF		0x0001

class CSmoke : public CBaseAnimating
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
//	void EXPORT SmokeThink(void);
	void EXPORT SmokeUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};

LINK_ENTITY_TO_CLASS(env_smoke, CSmoke);

void CSmoke::Precache(void)
{
//	UTIL_PrecacheOther("smoke_cloud");
}

void CSmoke::Spawn(void)
{
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_TRIGGER;
	pev->takedamage = DAMAGE_NO;
	SET_MODEL(ENT(pev), "sprites/smoke_loop.spr");
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
	if (pev->spawnflags & SF_SMOKE_START_OFF)
		pev->impulse = 0;

	ALERT(at_console, "%s: %s is an obsolete entity not supported by this code, removing.\n", STRING(pev->classname), STRING(pev->targetname));
	SetThink(&CBaseEntity::SUB_Remove);
//	SetThink(&CSmoke::SmokeThink);
//	SetUse(&CSmoke::SmokeUse);
	pev->nextthink = gpGlobals->time + 0.05;
}

void CSmoke::SmokeUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (pev->impulse)
		pev->impulse = 0;
	else
		pev->impulse = 1;
}
/*
void CSmoke::SmokeThink(void)
{
	if (pev->impulse)
		CSmokeCloud::CreateCloud(pev->origin, gpGlobals->v_up * 32, pev->renderamt, pev->rendermode, pev->rendercolor, pev->scale);

	pev->nextthink = gpGlobals->time + 0.05;
}
*/

// Still used on old XHL maps
/////////////////////////////////////////
#define SF_TB_START_OFF		0x0001

class CTeleporterBall : public CBasePlatTrain//CBaseToggle
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
//	void EXPORT BallUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
//	void EXPORT DestReached(void);
	virtual int ObjectCaps(void) { return FCAP_ACROSS_TRANSITION; }
//	CSprite *pGlow;
};

void CTeleporterBall::Spawn(void)
{
	Precache();
	pev->movetype		= MOVETYPE_NOCLIP;
	pev->solid			= SOLID_NOT;
	pev->takedamage		= DAMAGE_NO;
//	pev->effects		= EF_BRIGHTLIGHT;
	pev->rendermode		= kRenderTransAdd;// XDM3035c: kRenderTransTexture;
//	pev->renderfx		= kRenderFxFullBright;
	if (pev->renderamt <= 0)
		pev->renderamt = 255;

	SET_MODEL(ENT(pev), "models/tele_ball.mdl");
	UTIL_SetSize(pev, g_vecZero, g_vecZero);
	UTIL_SetOrigin(pev, pev->origin);

	if (pev->spawnflags & SF_TB_START_OFF)
	{
		pev->effects |= EF_NODRAW;
		pev->framerate = 0.0;
		pev->animtime = -1.0;
	}
	else
	{
		pev->framerate = 1.0;
		CBaseAnimating::Spawn();// starts animation
	}
	pev->spawnflags = 0;
	pev->sequence = 0;
//	m_activated = FALSE;
	if (pev->speed == 0)
		pev->speed = 100;

//	if (pev->dmg == 0)
//		pev->dmg = 2;

	if (m_volume == 0)
		m_volume = 0.85;
}

void CTeleporterBall::Precache(void)
{
	PRECACHE_MODEL("models/tele_ball.mdl");
}

void CTeleporterBall::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType, (pev->effects & EF_NODRAW)))
		return;

//	if (FStringNull(pev->target))
//	{
		if (pev->effects & EF_NODRAW)
		{
			pev->effects &= ~EF_NODRAW;
			pev->framerate = 1.0;
			pev->animtime = gpGlobals->time;
		}
		else
		{
			pev->effects |= EF_NODRAW;
			pev->framerate = 0.0;
			pev->animtime = -1.0;
		}
//	}
//	else
//		CFuncTrain::Use(pActivator, pCaller, useType, value);
}

LINK_ENTITY_TO_CLASS(tele_ball, CTeleporterBall);
