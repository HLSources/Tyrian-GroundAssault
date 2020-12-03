//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "customentity.h"
#include "player.h"
#include "globals.h"
#include "effects.h"
#include "environment.h"
#include "game.h"
#include "gamerules.h"
#include "weapons.h"
#include "msg_fx.h"

//=================================================================
// CEnvFog
// fog effect
// TODO: fade in/out must be synchronized between all clients,
// even if someone has just connected
//=================================================================

LINK_ENTITY_TO_CLASS(env_fog, CEnvFog);

TYPEDESCRIPTION	CEnvFog::m_SaveData[] =
{
	DEFINE_FIELD(CEnvFog, m_iStartDist, FIELD_SHORT),
	DEFINE_FIELD(CEnvFog, m_iEndDist, FIELD_SHORT),
	DEFINE_FIELD(CEnvFog, m_iCurrentStartDist, FIELD_SHORT),
	DEFINE_FIELD(CEnvFog, m_iCurrentEndDist, FIELD_SHORT),
	DEFINE_FIELD(CEnvFog, m_fFadeTime, FIELD_FLOAT),
	DEFINE_FIELD(CEnvFog, m_fFadeStartTime, FIELD_TIME),
};

IMPLEMENT_SAVERESTORE(CEnvFog, CBaseEntity);

void CEnvFog::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "startdist"))
	{
		m_iStartDist = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "enddist"))
	{
		m_iEndDist = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
/*	else if (FStrEq(pkvd->szKeyName, "fadein"))
	{
		m_iFadeIn = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "fadeout"))
	{
		m_iFadeOut = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}*/
	else if (FStrEq(pkvd->szKeyName, "fadetime"))
	{
		m_fFadeTime = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
/*	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		m_fHoldTime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}*/
	else
		CBaseEntity::KeyValue(pkvd);
}

STATE CEnvFog::GetState(void)
{
	if (pev->impulse == 0)
		return STATE_OFF;

	return STATE_ON;
}

void CEnvFog::Spawn(void)
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->effects |= EF_NODRAW;

	Precache();

	if (pev->spawnflags & SF_FOG_ACTIVE)
	{
		pev->impulse = FOG_STATE_ON;
		m_iCurrentStartDist = m_iStartDist;
		m_iCurrentEndDist = m_iEndDist;
//		SendClientData(NULL, MSG_ALL);
	}
	else
	{
		pev->impulse = FOG_STATE_OFF;
		m_iCurrentStartDist = 0;
		m_iCurrentEndDist = 0;
	}

	SetThinkNull();
	SetNextThink(0);
}

void CEnvFog::Precache(void)
{
	if (m_iStartDist == 0) m_iStartDist = 1;
	if (m_iEndDist == 0) m_iEndDist = 1;
	if (m_fFadeTime <= 0.0f) m_fFadeTime = 2.0f;
	if (pev->targetname == 0)
		pev->spawnflags |= SF_FOG_ACTIVE;
}

// this code should be synchromized with client-side code
void CEnvFog::Think(void)
{
	if (pev->impulse == FOG_STATE_FADEIN)
	{
		float k = max(0.01f,gpGlobals->time - m_fFadeStartTime)/m_fFadeTime;
		m_iCurrentStartDist = (m_iStartDist - 0)*k;
		m_iCurrentEndDist = (m_iEndDist - 0)*k;
		SetNextThink(0.1);
	}
	else if (pev->impulse == FOG_STATE_FADEOUT)
	{
		float k = 1.0f-max(0.01f,gpGlobals->time - m_fFadeStartTime)/m_fFadeTime;
		m_iCurrentStartDist = (m_iStartDist - 0)*k;
		m_iCurrentEndDist = (m_iEndDist - 0)*k;
		SetNextThink(0.1);
	}
	else
		SetNextThink(0);
}

void CEnvFog::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	bool bOn = (pev->impulse == FOG_STATE_ON || pev->impulse == FOG_STATE_FADEIN);
	if (!ShouldToggle(useType, bOn))
		return;

	if (pev->impulse == FOG_STATE_OFF || pev->impulse == FOG_STATE_FADEOUT)
	{
		pev->impulse = FOG_STATE_ON;
//		pev->impulse == FOG_STATE_FADEIN;
	}
	else
	{
		pev->impulse = FOG_STATE_OFF;
//		pev->impulse == FOG_STATE_FADEOUT;
	}

//	m_fFadeStartTime = gpGlobals->time;
//	SetNextThink(0.1);
	SendClientData(NULL, MSG_ALL);
}

int CEnvFog::SendClientData(CBasePlayer *pClient, int msgtype)
{
	if (msgtype == MSG_ONE && (pev->impulse == 0 || pClient == NULL))
		return 0;

	MESSAGE_BEGIN(msgtype, gmsgSetFog, pev->origin, (pClient == NULL)?NULL : ENT(pClient->pev));
		WRITE_BYTE(pev->rendercolor.x);
		WRITE_BYTE(pev->rendercolor.y);
		WRITE_BYTE(pev->rendercolor.z);
		WRITE_SHORT(m_iStartDist);// current values
		WRITE_SHORT(m_iEndDist);
		WRITE_BYTE(pev->impulse);// 0-off, 1-on, 2-fade out, 3-dafe in
//		WRITE_SHORT(m_iFinalStartDist);// target values
//		WRITE_SHORT(m_iFinalEndDist);
//		WRITE_SHORT((int)(m_fFadeTime - (gpGlobals->time - m_fFadeStartTime)));// send remaining time
	MESSAGE_END();
	return 1;
}




//=================================================================
// CEnvFogZone
// fog effect like func_water
//=================================================================

LINK_ENTITY_TO_CLASS(env_fogzone, CEnvFogZone);

void CEnvFogZone::Spawn(void)
{
	Precache();
	SET_MODEL(ENT(pev), STRING(pev->model));// set size
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	if (showtriggers.value <= 0)
		pev->effects = EF_NODRAW;

	if (pev->spawnflags & SF_FOGZONE_ACTIVE)
		pev->skin = CONTENTS_FOG;
}

void CEnvFogZone::Precache(void)
{
	if (pev->targetname == 0)
		pev->spawnflags |= SF_FOGZONE_ACTIVE;
}

void CEnvFogZone::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (pev->skin == CONTENTS_FOG)
		pev->skin = 0;
	else
		pev->skin = CONTENTS_FOG;
}

// update fog params once when the player enters the zone
void CEnvFogZone::Touch(CBaseEntity *pOther)
{
	if (!pOther->IsPlayer())
		return;

	CBasePlayer *pPlayer = (CBasePlayer*)pOther;
	if (pPlayer)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgSetFog, NULL, ENT(pPlayer->pev));
			WRITE_BYTE(pev->rendercolor.x);
			WRITE_BYTE(pev->rendercolor.y);
			WRITE_BYTE(pev->rendercolor.z);
			WRITE_SHORT(m_iStartDist);
			WRITE_SHORT(m_iEndDist);
			WRITE_BYTE(0);
		MESSAGE_END();
	}
}




//=================================================================
// CEnvRain
// rain effect
//=================================================================

LINK_ENTITY_TO_CLASS(env_rain, CEnvRain);


void CEnvRain::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "maxdrips"))
	{
		pev->impulse = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "texture"))
	{
		pev->noise1 = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "splashsprite"))
	{
		pev->noise2 = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "splashspriteg"))
	{
		pev->noise3 = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "scalex"))
	{
		m_fScaleX = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "scaley"))
	{
		m_fScaleY = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CEnvRain::Spawn(void)
{
	Precache();
	SET_MODEL(ENT(pev), STRING(pev->model));// set size
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	if (showtriggers.value <= 0)
		pev->effects |= EF_NODRAW;

	UTIL_FixRenderColor(pev->rendermode, pev->rendercolor);// XDM3035a: IMPORTANT!

	if (pev->renderamt == 0)
		pev->renderamt = 255;

	if (pev->impulse < 1)
		pev->impulse = 1;

	if (pev->spawnflags & SF_RAIN_START_OFF)
		pev->bInDuck = FALSE;
	else
		pev->bInDuck = TRUE;

	if (m_fScaleX <= 0.0f)
		m_fScaleX = 1.0f;
	if (m_fScaleY <= 0.0f)
		m_fScaleY = 1.0f;
}

void CEnvRain::Precache(void)
{
	if (!FStringNull(pev->noise1))
		sprTexture = PRECACHE_MODEL(STRINGV(pev->noise1));
	if (!FStringNull(pev->noise2))
		sprHitWater = PRECACHE_MODEL(STRINGV(pev->noise2));
	if (!FStringNull(pev->noise3))
		sprHitGround = PRECACHE_MODEL(STRINGV(pev->noise3));
}

void CEnvRain::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType, pev->bInDuck > 0))
		return;
//ALERT(at_console, "SERVER RAIN TOGGLE!\n");

	if (pev->bInDuck > 0)
		pev->bInDuck = 0;
	else
		pev->bInDuck = 1;

	SendClientData(NULL, MSG_ALL);
}

int CEnvRain::SendClientData(CBasePlayer *pClient, int msgtype)
{
	unsigned short flags = 0;
	bool remove = false;

	if (pev->bInDuck > 0)
	{
		if (pev->flags & FL_DRAW_ALWAYS)
			flags |= RENDERSYSTEM_FLAG_DRAWALWAYS;

		if (pev->spawnflags & SF_RAIN_ZROTATION)
			flags |= RENDERSYSTEM_FLAG_ZROTATION;

//		if (pev->spawnflags & SF_RAIN_NOSPLASH)
		flags |= RENDERSYSTEM_FLAG_CLIPREMOVE;

		if (!(pev->spawnflags & SF_RAIN_IGNOREMODELS))// since the default behavior was to detect studio models, we need an opposite option here
			flags |= RENDERSYSTEM_FLAG_ADDPHYSICS;// XDM3035c: now it is required to hit players
	}
	else
	{
		if (msgtype == MSG_ONE)// a client has connected and needs an update
			return 0;

//		flags = RENDERSYSTEM_FLAG_REMOVE;
		remove = true;
	}

	if (msgtype == MSG_BROADCAST)
		msgtype = MSG_ALL;// we need this fix in case someone will try to put this update into unreliable message stream

	edict_t *pentTarget = FIND_ENTITY_BY_TARGETNAME(NULL, STRING(pev->target));

	MESSAGE_BEGIN(msgtype, gmsgSetRain, pev->origin, (pClient == NULL)?NULL : ENT(pClient->pev));
		WRITE_COORD(pev->movedir.x*pev->speed);// direction
		WRITE_COORD(pev->movedir.y*pev->speed);
		WRITE_COORD(pev->movedir.z*pev->speed);
	if (!FNullEnt(pentTarget))
		WRITE_SHORT(ENTINDEX(pentTarget));
	else
		WRITE_SHORT(entindex());

	if (remove)// XDM3035
	{
		WRITE_SHORT(0);
		WRITE_SHORT(0);
		WRITE_SHORT(0);
		WRITE_SHORT(0);// XDM3035c
		WRITE_SHORT(0);
	}
	else
	{
		WRITE_SHORT(pev->modelindex);
		WRITE_SHORT(sprTexture);
		if (pev->spawnflags & SF_RAIN_NOSPLASH)
		{
		WRITE_SHORT(0);// XDM3035c
		WRITE_SHORT(0);
		}
		else
		{
		WRITE_SHORT(sprHitWater);
		WRITE_SHORT(sprHitGround);// XDM3035c
		}
		WRITE_SHORT(pev->impulse);
	}
		WRITE_SHORT(0);// life*10
		WRITE_BYTE((int)(m_fScaleX*10.0f));// XDM3034: sprite proportions will also be used
		WRITE_BYTE((int)(m_fScaleY*10.0f));
		WRITE_BYTE(kRenderTransAdd);
		WRITE_BYTE(pev->rendercolor.x);   // byte,byte,byte (color)
		WRITE_BYTE(pev->rendercolor.y);
		WRITE_BYTE(pev->rendercolor.z);
		WRITE_BYTE(pev->renderamt);  // byte (brightness)
		WRITE_SHORT(flags);
	MESSAGE_END();

//	ALERT(at_console, "------------- CEnvRain sent %d\n", pev->impulse);
	return 1;
}


/*
//=================================================================
// CEnvSnow
// snow effect
//=================================================================

LINK_ENTITY_TO_CLASS(env_snow, CEnvSnow);

TYPEDESCRIPTION	CEnvSnow::m_SaveData[] =
{
	DEFINE_FIELD(CEnvSnow, m_fLife, FIELD_FLOAT),
	DEFINE_FIELD(CEnvSnow, m_fRamp, FIELD_FLOAT),
	DEFINE_FIELD(CEnvSnow, m_flUpdateTime, FIELD_FLOAT),
	DEFINE_FIELD(CEnvSnow, bState, FIELD_BOOLEAN),
};

IMPLEMENT_SAVERESTORE(CEnvSnow, CBaseEntity);

void CEnvSnow::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "ramp"))
	{
		m_fRamp = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "color"))
	{
		pev->colormap = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "life"))
	{
		m_fLife = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "type"))
	{
		pev->impulse = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "updatetime"))
	{
		m_flUpdateTime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

int	CEnvSnow::ObjectCaps(void)
{
	return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION;
}

void CEnvSnow::Spawn(void)
{
//	UTIL_Remove(this);
//	return;

	SET_MODEL(ENT(pev), STRING(pev->model));// Set size
	pev->solid = SOLID_NOT;
	if (showtriggers.value <= 0)
		pev->effects = EF_NODRAW;

	pev->impulse = clamp(pev->impulse, 0, 9);

	if (!m_fRamp)
		m_fRamp = 0.0;

	if (!m_fLife)
		m_fLife = 8.0;

	if (pev->health <= 0)
		pev->health = 256;

	if (pev->colormap > 255 || pev->colormap < 0)
		pev->colormap = 246;

	if (pev->spawnflags & SF_SNOW_START_OFF)
		bState = FALSE;
	else
	{
		bState = TRUE;
		SetNextThink(0.1);
	}
}

void CEnvSnow::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType, bState))
		return;

	if (bState == TRUE)
	{
		bState = FALSE;
		SetNextThink(0);
	}
	else
	{
		bState = TRUE;
		SetNextThink(0.1);
	}
}

void CEnvSnow::Think(void)
{
	if (!bState)
	{
		pev->nextthink = gpGlobals->time + 1.0;
		return;
	}
	MESSAGE_BEGIN(MSG_PVS, gmsgSnow, pev->origin);
		WRITE_SHORT(pev->modelindex);
		WRITE_SHORT(m_fLife*10);// life
		WRITE_SHORT(pev->health);// num
		WRITE_BYTE(pev->colormap);// color
		WRITE_BYTE(pev->impulse);// type
	MESSAGE_END();
	pev->nextthink = gpGlobals->time + 1.0;//m_flUpdateTime;
}
*/



//=================================================================
// CEnvWarpBall
// Teleportation effect
//=================================================================

LINK_ENTITY_TO_CLASS(env_warpball, CEnvWarpBall);

void CEnvWarpBall::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "mainsound"))
	{
		pev->noise2 = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "effectsound"))
	{
		pev->noise3 = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "beamcount"))
	{
		pev->oldbuttons = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "radius"))
	{
		pev->health = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CEnvWarpBall::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;
	if (pev->health <= WARPBALL_MIN_RADIUS)
		pev->health = WARPBALL_MIN_RADIUS;

	if (FStringNull(pev->noise2))
		pev->noise2 = ALLOC_STRING(DEFAULT_SND_MAIN);

	if (FStringNull(pev->noise3))
		pev->noise3 = ALLOC_STRING(DEFAULT_SND_EFFECT);

	Precache();
}

void CEnvWarpBall::Precache(void)
{
	PRECACHE_SOUND(STRINGV(pev->noise2));
	PRECACHE_SOUND(STRINGV(pev->noise3));
}

void CEnvWarpBall::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	TraceResult tr;
	CBeam *pBeam;
	for (int i = 0; i < pev->oldbuttons; ++i)
	{
		UTIL_TraceLine(pev->origin, pev->origin + pev->health * UTIL_RandomVector(), ignore_monsters, NULL, &tr);
		pBeam = CBeam::BeamCreate("sprites/beams_all.spr",200);
		pBeam->PointsInit(pev->origin, tr.vecEndPos);
		pBeam->SetWidth(16);
		pBeam->SetNoise(48);
		pBeam->SetFrame(BLAST_SKIN_SHOCKWAVE);
		pBeam->SetScrollRate(35);
		pBeam->SetColor(RANDOM_LONG(0,100), 255, RANDOM_LONG(0,100));
		pBeam->SetBrightness(RANDOM_LONG(100,150));
		pBeam->SetFlags(BEAM_FSHADEOUT);
		pBeam->SetThink(&CBaseEntity::SUB_FadeOut);
		pBeam->SetNextThink(RANDOM_FLOAT(0, 1.0));
	}
	EMIT_SOUND(ENT(pev), CHAN_BODY, STRINGV(pev->noise2), VOL_NORM, ATTN_NORM);
	SetNextThink(0.8);
	FX_Trail(pev->origin, entindex(), FX_WARPBALL_EFFECT );
}

void CEnvWarpBall::Think(void)
{
	EMIT_SOUND(edict(), CHAN_ITEM, STRINGV(pev->noise3), VOL_NORM, ATTN_NORM);
	SUB_UseTargets(this, USE_TOGGLE, 0);

	if (pev->spawnflags & SF_WARPBALL_ONCE)
		UTIL_Remove(this);
}




//=================================================================
// CEnvShockwave
// shock wave effect
//=================================================================

LINK_ENTITY_TO_CLASS(env_shockwave, CEnvShockwave);

TYPEDESCRIPTION	CEnvShockwave::m_SaveData[] =
{
	DEFINE_FIELD( CEnvShockwave, m_iHeight, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvShockwave, m_iTime, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvShockwave, m_iRadius, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvShockwave, m_iScrollRate, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvShockwave, m_iNoise, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvShockwave, m_iFrameRate, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvShockwave, m_iStartFrame, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvShockwave, m_iSpriteTexture, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvShockwave, m_cType, FIELD_CHARACTER ),
	DEFINE_FIELD( CEnvShockwave, m_iszPosition, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE(CEnvShockwave, CBaseEntity);

void CEnvShockwave::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "m_iTime"))
	{
		m_iTime = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iRadius"))
	{
		m_iRadius = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iHeight"))
	{
		m_iHeight = atoi(pkvd->szValue)/2;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iScrollRate"))
	{
		m_iScrollRate = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iNoise"))
	{
		m_iNoise = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iFrameRate"))
	{
		m_iFrameRate = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iStartFrame"))
	{
		m_iStartFrame = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iszPosition"))
	{
		m_iszPosition = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_cType"))
	{
		m_cType = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CEnvShockwave::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;
	Precache();
}

void CEnvShockwave::Precache(void)
{
	m_iSpriteTexture = PRECACHE_MODEL(STRINGV(pev->netname));
}

void CEnvShockwave::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	Vector vecPos = pev->origin;

	if (!(pev->spawnflags & SF_SHOCKWAVE_CENTERED))
		vecPos.z += m_iHeight;

	// blast circle
	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, pev->origin);
	if (m_cType)
		WRITE_BYTE(m_cType);
	else
		WRITE_BYTE(TE_BEAMCYLINDER);
		WRITE_COORD(vecPos.x);// coord coord coord (center position)
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z);
		WRITE_COORD(vecPos.x);// coord coord coord (axis and radius)
		WRITE_COORD(vecPos.y);
		WRITE_COORD(vecPos.z + m_iRadius);
		WRITE_SHORT(m_iSpriteTexture); // short (sprite index)
		WRITE_BYTE(m_iStartFrame); // byte (starting frame)
		WRITE_BYTE(m_iFrameRate); // byte (frame rate in 0.1's)
		WRITE_BYTE(m_iTime); // byte (life in 0.1's)
		WRITE_BYTE(m_iHeight);  // byte (line width in 0.1's)
		WRITE_BYTE(m_iNoise);   // byte (noise amplitude in 0.01's)
		WRITE_BYTE(pev->rendercolor.x);   // byte,byte,byte (color)
		WRITE_BYTE(pev->rendercolor.y);
		WRITE_BYTE(pev->rendercolor.z);
		WRITE_BYTE(pev->renderamt);  // byte (brightness)
		WRITE_BYTE(m_iScrollRate);	// byte (scroll speed in 0.1's)
	MESSAGE_END();

	if (!(pev->spawnflags & SF_SHOCKWAVE_REPEATABLE))
	{
		SetThink(&CBaseEntity::SUB_Remove);
		SetNextThink(0);
	}
}




//=================================================================
// CEnvDLight
// dynamic light effect
// OBSOLETE: update this to use RenderSystem!
//=================================================================

LINK_ENTITY_TO_CLASS(env_dlight, CEnvDLight);
LINK_ENTITY_TO_CLASS(env_elight, CEnvDLight);

TYPEDESCRIPTION	CEnvDLight::m_SaveData[] =
{
	DEFINE_FIELD(CEnvDLight, m_vecPos, FIELD_VECTOR),
	DEFINE_FIELD(CEnvDLight, m_hAttach, FIELD_EHANDLE),
};

IMPLEMENT_SAVERESTORE(CEnvDLight, CPointEntity);

void CEnvDLight::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;
	if (FClassnameIs(ENT(pev), "env_elight"))
		elight = TRUE;
	else
		elight = FALSE;
}

void CEnvDLight::PostSpawn(void)
{
	if (FStringNull(pev->targetname) || pev->spawnflags & SF_DLIGHT_STARTON)
		DesiredAction();
}

void CEnvDLight::DesiredAction(void)
{
	Use(this, this, USE_ON, 0);
}

void CEnvDLight::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (elight)
	{
		if (pev->target)
		{
			m_hAttach = UTIL_FindEntityByTargetname(NULL, STRING(pev->target)/*, pActivator*/);
			if (m_hAttach == NULL)
			{
				ALERT(at_console, "Entity env_elight '%s' can't find target '%s'!\n", STRING(pev->targetname), STRING(pev->target));
				m_hAttach = this;
			}
		}
		else
			m_hAttach = this;
	}

	if (!ShouldToggle(useType, pev->nextthink > 0?TRUE:FALSE))
		return;

	if (pev->health == 0 && pev->nextthink > 0) // if we're thinking, and in switchable mode, then we're on
	{
		// turn off
		SetNextThink(0);
		return;
	}

	int iTime;
	m_vecPos = pev->origin;

	if (pev->health == 0)
	{
		iTime = 10;// 1 second
		SetNextThink(1);
	}
	else if (pev->health > 25)
	{
		iTime = 250;
		pev->takedamage = 25;
		SetNextThink(25);
	}
	else
		iTime = pev->health*10;

	MakeLight(iTime);

	if (pev->spawnflags & SF_DLIGHT_ONLYONCE)
	{
		SetThink(&CBaseEntity::SUB_Remove);
		SetNextThink(0);
	}
}

void CEnvDLight::Think(void)
{
	int iTime;
	if (pev->health == 0)
	{
		iTime = 10;
		SetNextThink(1);
	}
	else
	{
		pev->takedamage += 25;
		if (pev->health > pev->takedamage)
		{
			iTime = 25;
			SetNextThink(25);
		}
		else
		{
			// finished, just do the leftover bit
			iTime = (pev->health - pev->takedamage)*10;
			pev->takedamage = 0;
		}
	}

	MakeLight(iTime);
}

void CEnvDLight::MakeLight(int iTime)
{
	if (elight)
	{
		if (m_hAttach == NULL)
		{
			SetNextThink(0);
			pev->takedamage = 0;
			return;
		}
	}

	MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, m_vecPos);
	if (elight)
	{
		WRITE_BYTE(TE_ELIGHT);
		WRITE_SHORT(m_hAttach->entindex() + 0x1000 * pev->impulse);// entity, attachment
	}
	else
		WRITE_BYTE(TE_DLIGHT);

		WRITE_COORD(m_vecPos.x);		// X
		WRITE_COORD(m_vecPos.y);		// Y
		WRITE_COORD(m_vecPos.z);		// Z
	if (elight)							// radius * 0.1
		WRITE_COORD(pev->renderamt);
	else
		WRITE_BYTE(pev->renderamt);

		WRITE_BYTE(pev->rendercolor.x);	// r
		WRITE_BYTE(pev->rendercolor.y);	// g
		WRITE_BYTE(pev->rendercolor.z);	// b
		WRITE_BYTE(iTime);				// time * 10
	if (elight)							// decay * 0.1
		WRITE_COORD(pev->frags);
	else
		WRITE_BYTE(pev->frags);

	MESSAGE_END();
}


//=================================================================
// CEnvFountain
// Particle system effect
//=================================================================

LINK_ENTITY_TO_CLASS(env_fountain, CEnvFountain);
/*
TYPEDESCRIPTION	CEnvFountain::m_SaveData[] =
{
	DEFINE_FIELD(CEnvFountain, bRandVec, FIELD_BOOLEAN),
//	DEFINE_FIELD(CEnvFountain, bState, FIELD_BOOLEAN),
//	DEFINE_FIELD(CEnvFountain, bEntFound, FIELD_BOOLEAN),
//	DEFINE_FIELD(CEnvFountain, pTargetEnt, FIELD_CLASSPTR),
};

IMPLEMENT_SAVERESTORE(CEnvFountain, CBaseEntity);
*/
void CEnvFountain::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "amount"))
	{
		pev->dmg  = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "direction"))
	{
		if (StringToVec(pkvd->szValue, pev->movedir))
		{
//			pev->gamestate = 0;// random direction
			pkvd->fHandled = TRUE;
		}
/*		else
		{
// we're not sure if origin was already set			pev->movedir = pev->origin;
			pev->gamestate = 1;
			pkvd->fHandled = FALSE;
		}*/
	}
	else if (FStrEq(pkvd->szKeyName, "spread"))
	{
		if (StringToVec(pkvd->szValue, pev->view_ofs))
			pkvd->fHandled = TRUE;
		else
			pkvd->fHandled = FALSE;
	}
	else if (FStrEq(pkvd->szKeyName, "life"))
	{
		pev->health = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "scale_delta"))
	{
		pev->max_health = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "renderamt_delta"))
	{
		pev->frags = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "animate_spr"))
	{
		pev->animtime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "start_frame"))
	{
		pev->frame = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "velocity"))
	{
		pev->yaw_speed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "sprite"))
	{
		pev->model = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue(pkvd);
}

void CEnvFountain::Spawn(void)
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;

	UTIL_FixRenderColor(pev->rendermode, pev->rendercolor);// XDM3035a: IMPORTANT!
//	bRandVec = FALSE;
//	bEntFound = FALSE;

	if (FBitSet(pev->spawnflags, SF_FOUNTAIN_START_OFF))
		pev->impulse = FALSE;
	else
		pev->impulse = TRUE;
}

void CEnvFountain::Precache(void)
{
	pev->modelindex = PRECACHE_MODEL(STRINGV(pev->model));
}

void CEnvFountain::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType, pev->impulse > 0))
		return;

	if (pev->impulse > 0)
		pev->impulse = 0;
	else
		pev->impulse = 1;

	if (pev->impulse > 0)
		SendClientData(NULL, MSG_PVS);// MSG_ALL?
	else
		SendClientData(NULL, MSG_ALL);
}

int CEnvFountain::SendClientData(CBasePlayer *pClient, int msgtype)
{
	if (pClient == NULL && msgtype == MSG_ONE)// can't be
		return 0;

	unsigned short flags = RENDERSYSTEM_FLAG_DONTFOLLOW;// temporary for now
	bool remove = false;

	if (pev->impulse > 0)
	{
		if (pev->flags & FL_DRAW_ALWAYS)
			flags |= RENDERSYSTEM_FLAG_DRAWALWAYS;

		if (pev->spawnflags & SF_FOUNTAIN_CLIPREMOVE)
			flags |= RENDERSYSTEM_FLAG_CLIPREMOVE;

		if (pev->spawnflags & SF_FOUNTAIN_RANDOMFRAME)
			flags |= RENDERSYSTEM_FLAG_RANDOMFRAME;

		if (pev->spawnflags & SF_FOUNTAIN_ADDGRAVITY)
			flags |= RENDERSYSTEM_FLAG_ADDGRAVITY;

		if (pev->spawnflags & SF_FOUNTAIN_ZROTATION)
			flags |= RENDERSYSTEM_FLAG_ZROTATION;
	
		if (pev->spawnflags & SF_FOUNTAIN_SIMULTANEOUS)
			flags |= RENDERSYSTEM_FLAG_SIMULTANEOUS;	
		
		if (pev->spawnflags & SF_FOUNTAIN_INCONTENTSONLY)
			flags |= RENDERSYSTEM_FLAG_INCONTENTSONLY;	

		if (pev->spawnflags & SF_FOUNTAIN_ADDPHYSICS)
			flags |= RENDERSYSTEM_FLAG_ADDPHYSICS;	
	}
	else
	{
		if (msgtype == MSG_ONE)// a client has connected and needs an update
			return 0;

//		flags |= RENDERSYSTEM_FLAG_REMOVE;
		remove = true;
	}

//	ALERT(at_console, "pev->origin = %f %f %f\n", pev->origin.x, pev->origin.y, pev->origin.z);
	MESSAGE_BEGIN(msgtype, gmsgPartSys, pev->origin, (pClient == NULL)?NULL : ENT(pClient->pev));
	WRITE_COORD(pev->origin.x);	
	WRITE_COORD(pev->origin.y);
	WRITE_COORD(pev->origin.z);

	if (FBitSet(pev->spawnflags, SF_FOUNTAIN_RANDOMDIR))
	{
		WRITE_COORD(pev->origin.x);	
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
	}
	else
	{
		WRITE_COORD(pev->movedir.x);//direction
		WRITE_COORD(pev->movedir.y);
		WRITE_COORD(pev->movedir.z);
	}

	WRITE_COORD(pev->view_ofs.x);// spread
	WRITE_COORD(pev->view_ofs.y);
	WRITE_COORD(pev->view_ofs.z);		
	WRITE_SHORT(pev->yaw_speed); //velocity
	WRITE_SHORT(pev->modelindex);// sprite name
	WRITE_BYTE(pev->rendermode);// render mode
	WRITE_SHORT(pev->dmg);// max particles

	if (FBitSet(pev->spawnflags, SF_FOUNTAIN_ONCE))
		WRITE_SHORT(pev->health * 10);// life
	else
		WRITE_SHORT(0);// toggle

	WRITE_SHORT(entindex());// follow entity
	WRITE_BYTE(pev->scale);// scale
	WRITE_BYTE(pev->max_health);// scale delta
	WRITE_BYTE(pev->rendercolor.x); //color r
	WRITE_BYTE(pev->rendercolor.y);//color g 
	WRITE_BYTE(pev->rendercolor.z);//color b
	WRITE_BYTE(pev->renderamt); //brightness
	WRITE_BYTE(pev->frags); //brightness delta
	WRITE_BYTE(pev->animtime); //Animated?
	WRITE_BYTE(pev->frame); //start frame
	WRITE_SHORT(flags);

	if (remove)
		WRITE_BYTE(PARTSYSTEM_TYPE_REMOVEANY);
	else
		WRITE_BYTE(PARTSYSTEM_TYPE_BLASTCONE);
	
	MESSAGE_END();

/*	if (FBitSet(pev->spawnflags, SF_FOUNTAIN_REMOVE))
	{
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time + pev->health;
	}*/
	return 1;
}

//=================================================================
// CEnvSun
// not yet
//=================================================================
/*
LINK_ENTITY_TO_CLASS(env_sun, CEnvSun);

void CEnvSun::Precache(void)
{
}

void CEnvSun::Spawn(void)
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;
	pev->effects = 0;
}

void CEnvSun::Think(void)
{
}
*/

//=================================================================
// CLavaBall
// lava balls
//=================================================================
LINK_ENTITY_TO_CLASS(lava_ball, CLavaBall);

void CLavaBall::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_BOUNCE;
	pev->takedamage = DAMAGE_NO;
	pev->flags = 0;// obsolete FL_IMMUNE_LAVA;
	pev->health = 1.0;
	pev->scale = 0.5;
	pev->gravity = 0.8;
	pev->dmg = 2.0;
	SET_MODEL(ENT(pev), "sprites/anim_spr13.spr");
	UTIL_SetSize(pev, g_vecZero, g_vecZero);

	FX_Trail(pev->origin, entindex(), FX_LAVABALL );
	
	pev->velocity = Vector(0.0f,0.0f,RANDOM_FLOAT(100,150));
	pev->renderamt = 255;
	pev->rendermode = kRenderTransAdd;
	pev->renderfx = kRenderFxPulseFast;
	pev->impulse = MODEL_FRAMES(pev->modelindex);
	pev->nextthink = gpGlobals->time + 0.5;
}

void CLavaBall::Think(void)
{
	if (pev->renderamt <= 5)
		UTIL_Remove(this);

	pev->frame = (int)(pev->frame + 1) % pev->impulse;
	pev->renderamt -= 5;
	pev->nextthink = gpGlobals->time + 0.05;
}


//=================================================================
// CBaseLava
// lava balls spawner
//=================================================================
LINK_ENTITY_TO_CLASS(func_lava, CBaseLava);

void CBaseLava::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "updatetime"))
	{
		pev->animtime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CBaseLava::Spawn(void)
{
	Precache();
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;
	if (showtriggers.value <= 0)
		pev->effects = EF_NODRAW;

	SET_MODEL(ENT(pev), STRING(pev->model));
	UTIL_SetSize(pev, pev->mins, pev->maxs);
	SetThink(&CBaseLava::LavaThink);
	SetUse(&CBaseLava::LavaUse);

	if (!pev->animtime)
		pev->animtime = 1.0;

	if (FBitSet(pev->spawnflags, SF_LAVA_START_OFF))
	{
		pev->impulse = FALSE;
	}
	else
	{
		pev->impulse = TRUE;
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CBaseLava::Precache(void)
{
	UTIL_PrecacheOther("lava_ball");
}

void CBaseLava::LavaThink(void)
{
	if (pev->impulse <= 0)
		return;

	Vector vecSrc;
	vecSrc.x = pev->mins.x + RANDOM_FLOAT(0, pev->size.x);
	vecSrc.y = pev->mins.y + RANDOM_FLOAT(0, pev->size.y);
	vecSrc.z = pev->mins.z + RANDOM_FLOAT(0, pev->size.z);
	CBaseEntity::Create("lava_ball", vecSrc, g_vecZero, Vector(0,0,1) + UTIL_RandomBloodVector() * RANDOM_LONG(50,100), edict());
	pev->nextthink = gpGlobals->time + pev->animtime;
}

void CBaseLava::LavaUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType, pev->impulse > 0))
		return;

	if (pev->impulse > 0)
	{
		pev->impulse = 0;
		SetNextThink(0);
	}
	else
	{
		pev->impulse = 1;
		SetNextThink(0.1);
	}
}


//=================================================================
// CEnvLightAttachment
// dlight attacment
//=================================================================

LINK_ENTITY_TO_CLASS(env_lightatt, CEnvLightAttachment);

void CEnvLightAttachment::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;
	pev->effects = EF_NODRAW | EF_DIMLIGHT;
	pev->nextthink = gpGlobals->time + 0.1;
}

void CEnvLightAttachment::Think(void)
{
	if (!pev->aiment)
	{
		if (!FStringNull(pev->target))
		{
			CBaseEntity *pTargetEnt = UTIL_FindEntityByTargetname(NULL, STRING(pev->target));
			if (pTargetEnt)
			{
				pev->aiment = pTargetEnt->edict();
				pev->movetype = MOVETYPE_FOLLOW;
			}
			else
				pev->movetype = MOVETYPE_NONE;
		}
	}
	pev->nextthink = gpGlobals->time + 0.1;
}




//=================================================================
// CEnvRotParticles
// health - radius
// skin - color
// impulse - count
// XDM3035: UPDATE do not use this!
// OBSOLETE: kept for backwards compatibility
//=================================================================

LINK_ENTITY_TO_CLASS(env_rotparticles, CEnvRotParticles);

void CEnvRotParticles::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->takedamage = DAMAGE_NO;
	pev->effects = EF_NODRAW;
	if (pev->spawnflags & SF_ROTPART_START_OFF)
		pev->impulse = 0;
	else
		pev->impulse = 1;

	pev->nextthink = gpGlobals->time + 0.1;
}

void CEnvRotParticles::Think(void)
{
	pev->nextthink = gpGlobals->time + 0.1;
	if (pev->impulse <= 0)
		return;

	pev->angles.y += gpGlobals->frametime*4.0f;
	Vector forward, right;
	ANGLE_VECTORS(pev->angles, forward, right, NULL);
//	UTIL_ParticleEffect(pev->origin, g_vecZero, 255, 25);
	UTIL_ParticleEffect(pev->origin + forward * pev->health, g_vecZero, pev->skin, pev->impulse);
	UTIL_ParticleEffect(pev->origin - forward * pev->health, g_vecZero, pev->skin, pev->impulse);
	UTIL_ParticleEffect(pev->origin + right * pev->health, g_vecZero, pev->skin, pev->impulse);
	UTIL_ParticleEffect(pev->origin - right * pev->health, g_vecZero, pev->skin, pev->impulse);
}

void CEnvRotParticles::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (pev->impulse == 1)
		pev->impulse = 0;
	else
		pev->impulse = 1;
}




//=================================================================
// CEnvSky
// env_sky, 3D skybox view point
//=================================================================
LINK_ENTITY_TO_CLASS(env_sky, CEnvSky);

void CEnvSky::Spawn(void)
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;
	pev->impulse = 1;
	pev->nextthink = 0;
}

int CEnvSky::SendClientData(CBasePlayer *pClient, int msgtype)
{
//	ALERT(at_console, "CEnvSky::SendClientData(%d)\n", msgtype);
	MESSAGE_BEGIN(msgtype, gmsgSetSky, pev->origin, (pClient == NULL)?NULL : ENT(pClient->pev));
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_BYTE(pev->impulse);
	MESSAGE_END();
	return 1;
}

//=================================================================
// CEnvStatic - static mesh
// pev->impulse 1 - animating, def. - 0
//=================================================================
LINK_ENTITY_TO_CLASS(env_static, CEnvStatic);

void CEnvStatic::Precache(void)
{
	pev->modelindex = UTIL_PrecacheModel(STRINGV(pev->model));
}

void CEnvStatic::Spawn(void)
{
	Precache();
	pev->deadflag		= DEAD_NO;

	if (pev->spawnflags & SF_ENVSTATIC_NOTSOLID)// XDM
		pev->solid		= SOLID_NOT;
	else
		pev->solid		= SOLID_SLIDEBOX;

	pev->movetype		= MOVETYPE_NONE;
	pev->takedamage		= DAMAGE_NO;
	pev->effects		= EF_NOINTERP;
	pev->health			= 65535.0;

	UTIL_FixRenderColor(pev->rendermode, pev->rendercolor);// XDM3035a: IMPORTANT!

	UTIL_SetModel(ENT(pev),	STRINGV(pev->model));

// this thing does not work	MAKE_STATIC(ENT(pev));

	m_flFrameRate = 0.0f;
	m_flGroundSpeed = 0.0f;

	if (pev->spawnflags & SF_ENVSTATIC_START_INVISIBLE)
		pev->impulse = 0;
	else
		pev->impulse = 1;// visible

	if (sv_clientstaticents.value < 1.0f)
	{
		if (pev->impulse == 0)
			pev->effects |= EF_NODRAW;
		else
			pev->effects &= ~EF_NODRAW;
	}
	else
	{
		pev->effects |= EF_NODRAW;// XDM3035: use impulse to determine visibility, this flag is to NOT to send to client only
	}
	pev->sequence = 0;
	pev->frame = 0;
	pev->framerate = 0;
	SetThinkNull();
	pev->nextthink = 0.0f;
	pev->animtime = gpGlobals->time + 0.1f;
}

void CEnvStatic::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
//	if (!ShouldToggle(useType, pev->impulse > 0))
	if (!ShouldToggle(useType, (pev->effects & EF_NODRAW)?FALSE:TRUE))
		return;

//	if (pev->effects & EF_NODRAW)
	if (pev->impulse == 0)
	{
		if (sv_clientstaticents.value < 1.0f)
			pev->effects &= ~EF_NODRAW;

		pev->impulse = 1;
		pev->animtime = gpGlobals->time;
		pev->framerate = 1.0;
		SendClientData(NULL, MSG_ALL);
		// UPDATE ONCE AND FOR ALL
	}
/*	else
	{
		if (sv_clientstaticents.value < 1.0f)
			pev->effects |= EF_NODRAW;

//		pev->effects |= EF_NODRAW;
		pev->impulse = 0;
		pev->animtime = 0.0;
		pev->framerate = 0.0;
	}*/
}

// TODO: make non-updating persistant entities on a client side
int CEnvStatic::SendClientData(CBasePlayer *pClient, int msgtype)
{
	if (sv_clientstaticents.value < 1.0f)
		return 0;

	if (msgtype == MSG_ONE)// a client has connected and needs an update
	{
		if (pClient == NULL)
			return 0;

		if (pClient->IsBot())// bots don't need trees =)
			return 0;// game dll-integrated bots will recognize these
	}
	else if (msgtype == MSG_BROADCAST)
		msgtype = MSG_ALL;// we need this fix in case someone will try to put this update into unreliable message stream

	if (pev->impulse == 1)
	{
	MESSAGE_BEGIN(msgtype, gmsgSetStaticEnt, pev->origin, (pClient == NULL)?NULL : ENT(pClient->pev));
		WRITE_SHORT(entindex());// TODO: use more reliable pev->pContainingEntity->serialnumber
		WRITE_SHORT(pev->modelindex);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_ANGLE(pev->angles.x);
		WRITE_ANGLE(pev->angles.y);
		WRITE_ANGLE(pev->angles.z);
		WRITE_BYTE(pev->rendermode);
		WRITE_BYTE(pev->renderfx);
		WRITE_BYTE(pev->rendercolor.x);
		WRITE_BYTE(pev->rendercolor.y);
		WRITE_BYTE(pev->rendercolor.z);
		WRITE_BYTE(pev->renderamt);
//	if (pev->impulse == 1)
//		WRITE_BYTE(pev->effects & ~EF_NODRAW);
//	else
//		WRITE_BYTE(pev->effects & EF_NODRAW);

		WRITE_BYTE(pev->body);
		WRITE_BYTE(pev->skin);
		WRITE_COORD(pev->scale);
		WRITE_SHORT(pev->sequence);
	if (pev->pContainingEntity->num_leafs > 0)// just send one because it's a point entity
		WRITE_SHORT(pev->pContainingEntity->leafnums[0]);
	else
		WRITE_SHORT(0);
	MESSAGE_END();
	}
	return 1;
}
