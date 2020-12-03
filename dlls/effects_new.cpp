//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
// Spirit compatibility mostly. Needs to be revisited.
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "customentity.h"
#include "effects.h"
#include "decals.h"
#include "player.h"
#include "globals.h"


// Body queue class here.... It's really just CBaseEntity
class CCorpse : public CBaseEntity
{
	virtual int ObjectCaps(void) { return FCAP_DONT_SAVE; }
};

LINK_ENTITY_TO_CLASS(bodyque, CCorpse);



//=================================================================
// env_model: like env_sprite, except you can specify a sequence.
//=================================================================
#define SF_ENVMODEL_OFF			1
#define SF_ENVMODEL_DROPTOFLOOR	2
#define SF_ENVMODEL_SOLID		4

class CEnvModel : public CBaseAnimating
{
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Think(void);
	virtual void KeyValue(KeyValueData *pkvd);
	virtual STATE GetState(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int	ObjectCaps(void) { return CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	void SetSequence(void);

	string_t m_iszSequence_On;
	string_t m_iszSequence_Off;
	int m_iAction_On;
	int m_iAction_Off;
};

TYPEDESCRIPTION CEnvModel::m_SaveData[] =
{
	DEFINE_FIELD(CEnvModel, m_iszSequence_On, FIELD_STRING),
	DEFINE_FIELD(CEnvModel, m_iszSequence_Off, FIELD_STRING),
	DEFINE_FIELD(CEnvModel, m_iAction_On, FIELD_INTEGER),
	DEFINE_FIELD(CEnvModel, m_iAction_Off, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CEnvModel, CBaseAnimating);

LINK_ENTITY_TO_CLASS(env_model, CEnvModel);

void CEnvModel::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "m_iszSequence_On"))
	{
		m_iszSequence_On = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iszSequence_Off"))
	{
		m_iszSequence_Off = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iAction_On"))
	{
		m_iAction_On = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "m_iAction_Off"))
	{
		m_iAction_Off = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseAnimating::KeyValue(pkvd);
}

void CEnvModel::Spawn(void)
{
	Precache();
	SET_MODEL(ENT(pev), STRING(pev->model));
	UTIL_SetOrigin(pev, pev->origin);

	if (pev->spawnflags & SF_ENVMODEL_SOLID)
	{
		pev->solid = SOLID_SLIDEBOX;
		UTIL_SetSize(pev, Vector(-10, -10, -10), Vector(10, 10, 10));// wtf??
	}

	if (pev->spawnflags & SF_ENVMODEL_DROPTOFLOOR)
	{
		pev->origin.z += 1;
		DROP_TO_FLOOR(ENT(pev));
	}

	SetBoneController(0, 0);
	SetBoneController(1, 0);
	SetSequence();
	SetNextThink(0.1);
}

void CEnvModel::Precache(void)
{
	PRECACHE_MODEL(STRINGV(pev->model));
}

STATE CEnvModel::GetState(void)
{
	if (pev->spawnflags & SF_ENVMODEL_OFF)
		return STATE_OFF;
	else
		return STATE_ON;
}

void CEnvModel::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (ShouldToggle(useType, !(pev->spawnflags & SF_ENVMODEL_OFF)))
	{
		if (pev->spawnflags & SF_ENVMODEL_OFF)
			pev->spawnflags &= ~SF_ENVMODEL_OFF;
		else
			pev->spawnflags |= SF_ENVMODEL_OFF;

		SetSequence();
		SetNextThink(0.1);
	}
}

void CEnvModel::Think(void)
{
	int iTemp;

//	ALERT(at_console, "env_model Think fr=%f\n", pev->framerate);

	StudioFrameAdvance(); // set m_fSequenceFinished if necessary

//	if (m_fSequenceLoops)
//	{
//		SetNextThink(1E6);
//		return; // our work here is done.
//	}
	if (m_fSequenceFinished && !m_fSequenceLoops)
	{
		if (pev->spawnflags & SF_ENVMODEL_OFF)
			iTemp = m_iAction_Off;
		else
			iTemp = m_iAction_On;

		switch (iTemp)
		{
//		case 1: // loop
//			pev->animtime = gpGlobals->time;
//			m_fSequenceFinished = FALSE;
//			m_flLastEventCheck = gpGlobals->time;
//			pev->frame = 0;
//			break;
		case 2: // change state
			if (pev->spawnflags & SF_ENVMODEL_OFF)
				pev->spawnflags &= ~SF_ENVMODEL_OFF;
			else
				pev->spawnflags |= SF_ENVMODEL_OFF;
			SetSequence();
			break;
		default: //remain frozen
			return;
		}
	}
	SetNextThink(0.1);
}

void CEnvModel :: SetSequence(void)
{
	int iszSeq;

	if (pev->spawnflags & SF_ENVMODEL_OFF)
		iszSeq = m_iszSequence_Off;
	else
		iszSeq = m_iszSequence_On;

	if (!iszSeq)
		return;
	pev->sequence = LookupSequence(STRING(iszSeq));

	if (pev->sequence == -1)
	{
		if (pev->targetname)
			ALERT(at_error, "env_model %s: unknown sequence \"%s\"\n", STRING(pev->targetname), STRING(iszSeq));
		else
			ALERT(at_error, "env_model: unknown sequence \"%s\"\n", STRING(iszSeq));
		pev->sequence = 0;
	}

	pev->frame = 0;
	ResetSequenceInfo();

	if (pev->spawnflags & SF_ENVMODEL_OFF)
	{
		if (m_iAction_Off == 1)
			m_fSequenceLoops = 1;
		else
			m_fSequenceLoops = 0;
	}
	else
	{
		if (m_iAction_On == 1)
			m_fSequenceLoops = 1;
		else
			m_fSequenceLoops = 0;
	}
}




//=========================================================
// SHL - Beam Trail effect
//=========================================================
#define SF_BEAMTRAIL_OFF 1
class CEnvBeamTrail : public CPointEntity
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual STATE GetState(void);
	void EXPORT StartTrailThink(void);
	void Affect(CBaseEntity *pTarget, USE_TYPE useType);

	int		m_iSprite;	// Don't save, precache
};

void CEnvBeamTrail::Precache(void)
{
//	if (pev->target)
//		PRECACHE_MODEL("sprites/null.spr");
	if (pev->netname)
		m_iSprite = PRECACHE_MODEL (STRINGV(pev->netname));
}

LINK_ENTITY_TO_CLASS(env_beamtrail, CEnvBeamTrail);

STATE CEnvBeamTrail :: GetState (void)
{
	if (pev->spawnflags & SF_BEAMTRAIL_OFF)
		return STATE_OFF;
	else
		return STATE_ON;
}

void CEnvBeamTrail :: StartTrailThink (void)
{
	pev->spawnflags |= SF_BEAMTRAIL_OFF; // fake turning off, so the Use turns it on properly
	Use(this, this, USE_ON, 0);
}

void CEnvBeamTrail::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (pev->target)
	{
		CBaseEntity *pTarget = UTIL_FindEntityByTargetname(NULL, STRING(pev->target)/*, pActivator */);
		while (pTarget)
		{
			Affect(pTarget, useType);
			pTarget = UTIL_FindEntityByTargetname(pTarget, STRING(pev->target)/*, pActivator */);
		}
	}
	else
	{
		if (!ShouldToggle(useType, GetState() != STATE_OFF))// XDM3037: cast from STATE to bool
			return;
		Affect(this, useType);
	}

	if (useType == USE_ON)
		pev->spawnflags &= ~SF_BEAMTRAIL_OFF;
	else if (useType == USE_OFF)
		pev->spawnflags |= SF_BEAMTRAIL_OFF;
	else if (useType == USE_TOGGLE)
	{
		if (pev->spawnflags & SF_BEAMTRAIL_OFF)
			pev->spawnflags &= ~SF_BEAMTRAIL_OFF;
		else
			pev->spawnflags |= SF_BEAMTRAIL_OFF;
	}
}

void CEnvBeamTrail::Affect(CBaseEntity *pTarget, USE_TYPE useType)
{
	if (useType == USE_ON || pev->spawnflags & SF_BEAMTRAIL_OFF)
	{
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_BEAMFOLLOW);
			WRITE_SHORT(pTarget->entindex());	// entity
			WRITE_SHORT(m_iSprite);	// model
			WRITE_BYTE(pev->health*10); // life
			WRITE_BYTE(pev->armorvalue);  // width
			WRITE_BYTE(pev->rendercolor.x);   // r, g, b
			WRITE_BYTE(pev->rendercolor.y);   // r, g, b
			WRITE_BYTE(pev->rendercolor.z);   // r, g, b
			WRITE_BYTE(pev->renderamt);	// brightness
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
			WRITE_BYTE(TE_KILLBEAM);
			WRITE_SHORT(pTarget->entindex());
		MESSAGE_END();
	}
}

void CEnvBeamTrail::Spawn(void)
{
	Precache();
//	SET_MODEL(ENT(pev), "sprites/null.spr");
	UTIL_SetSize(pev, g_vecZero, g_vecZero);

	if (!(pev->spawnflags & SF_BEAMTRAIL_OFF))
	{
		SetThink(&CEnvBeamTrail::StartTrailThink);
		SetNextThink(0);
	}
}




//=========================================================
// SHL Decal effect
//=========================================================
class CEnvDecal : public CPointEntity
{
public:
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
};

LINK_ENTITY_TO_CLASS(env_decal, CEnvDecal);

void CEnvDecal::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	int iTexture = 0;
	switch(pev->impulse)
	{
		case 1: iTexture = DECAL_GUNSHOT1	+	RANDOM_LONG(0,4); break;
		case 2: iTexture = DECAL_BLOOD1		+	RANDOM_LONG(0,5); break;
		case 3: iTexture = DECAL_YBLOOD1	+	RANDOM_LONG(0,5); break;
		case 4: iTexture = DECAL_GLASSBREAK1+	RANDOM_LONG(0,2); break;
		case 5: iTexture = DECAL_BIGSHOT1	+	RANDOM_LONG(0,4); break;
		case 6: iTexture = DECAL_SCORCH1	+	RANDOM_LONG(0,2); break;
		case 7: iTexture = DECAL_SPIT1		+	RANDOM_LONG(0,1); break;
		case 8: iTexture = DECAL_LARGESHOT1	+	RANDOM_LONG(0,4); break;
	}

	if (pev->impulse)
		iTexture = g_Decals[iTexture].index;
	else
		iTexture = pev->skin; // custom texture

	Vector vecPos = pev->origin;

	Vector vecOffs;
	UTIL_MakeVectors(pev->angles);
	vecOffs = gpGlobals->v_forward;
	vecOffs = vecOffs.Normalize() * 4000;


	TraceResult trace;
	int			entityIndex;

	UTIL_TraceLine(vecPos, vecPos+vecOffs, ignore_monsters, NULL, &trace);

	if (trace.flFraction == 1.0)
		return; // didn't hit anything, oh well

	entityIndex = (short)ENTINDEX(trace.pHit);

	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE(TE_BSPDECAL);
		WRITE_COORD(trace.vecEndPos.x);
		WRITE_COORD(trace.vecEndPos.y);
		WRITE_COORD(trace.vecEndPos.z);
		WRITE_SHORT(iTexture);
		WRITE_SHORT(entityIndex);
		if (entityIndex)
			WRITE_SHORT((int)VARS(trace.pHit)->modelindex);
	MESSAGE_END();
}

void CEnvDecal::Spawn(void)
{
	if (pev->impulse == 0)
	{
		pev->skin = DECAL_INDEX(STRING(pev->noise));

		if (pev->skin == 0)
			ALERT(at_console, "env_decal \"%s\" can't find decal \"%s\"\n", STRING(pev->noise));
	}
}


//=========================================================
// Old HL decals
//=========================================================
#define SF_DECAL_NOTINDEATHMATCH 2048

class CDecal : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int SendClientData(CBasePlayer *pClient, int msgtype);
	void EXPORT StaticDecal(void);
	void EXPORT TriggerDecal(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	TraceResult trace;
};

LINK_ENTITY_TO_CLASS(infodecal, CDecal);

void CDecal::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "texture"))
	{
		pev->skin = DECAL_INDEX(pkvd->szValue);
		if (pev->skin < 0)
			ALERT(at_console, "Can't find decal %s\n", pkvd->szValue);

		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

// UNDONE:  These won't get sent to joining players in multi-player
// XDM3035: used new update mechanism on these
void CDecal::Spawn(void)
{
	if (pev->skin < 0 || (gpGlobals->deathmatch && FBitSet(pev->spawnflags, SF_DECAL_NOTINDEATHMATCH)))
	{
		REMOVE_ENTITY(ENT(pev));
		return;
	}

	memset(&trace, NULL, sizeof(TraceResult));

	if (FStringNull(pev->targetname))
	{
		pev->impulse = 1;
		SetThink(&CDecal::StaticDecal);
		// if there's no targetname, the decal will spray itself on as soon as the world is done spawning.
		pev->nextthink = gpGlobals->time;
	}
	else
	{
		pev->impulse = 0;
		// if there IS a targetname, the decal sprays itself on when it is triggered.
		//SetThink(&CBaseEntity::SUB_DoNothing);
		SetThinkNull();// XDM: TESTME
		SetUse(&CDecal::TriggerDecal);
	}
}

// this is set up as a USE function for infodecals that have targetnames, so that the decal doesn't get applied until it is fired. (usually by a scripted sequence)
void CDecal::TriggerDecal(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	UTIL_TraceLine(pev->origin - Vector(5,5,5), pev->origin + Vector(5,5,5),  ignore_monsters, ENT(pev), &trace);
	pev->impulse = 1;
	SendClientData(NULL, MSG_BROADCAST);
	SetThinkNull();// XDM: TESTME
//	SetThink(&CBaseEntity::SUB_Remove);
//	pev->nextthink = gpGlobals->time + 0.1;
}

// XDM: does this affect joining player too?
void CDecal::StaticDecal(void)
{
	int entityIndex, modelIndex;
	UTIL_TraceLine(pev->origin - Vector(5,5,5), pev->origin + Vector(5,5,5),  ignore_monsters, ENT(pev), &trace);
	entityIndex = (short)ENTINDEX(trace.pHit);

	if (entityIndex)
		modelIndex = (int)VARS(trace.pHit)->modelindex;
	else
		modelIndex = 0;

	STATIC_DECAL(pev->origin, (int)pev->skin, entityIndex, modelIndex);
	SUB_Remove();
}

int CDecal::SendClientData(CBasePlayer *pClient, int msgtype)
{
	if (msgtype == MSG_ONE && (pev->impulse == 0 || pClient == NULL))
		return 0;

//	MESSAGE_BEGIN(MSG_BROADCAST, SVC_TEMPENTITY);
	MESSAGE_BEGIN(msgtype, SVC_TEMPENTITY, pev->origin, (pClient == NULL)?NULL : ENT(pClient->pev));
		WRITE_BYTE(TE_BSPDECAL);
		WRITE_COORD(pev->origin.x);
		WRITE_COORD(pev->origin.y);
		WRITE_COORD(pev->origin.z);
		WRITE_SHORT((int)pev->skin);
		int entityIndex = (short)ENTINDEX(trace.pHit);
		WRITE_SHORT(entityIndex);
		if (entityIndex)
			WRITE_SHORT((int)VARS(trace.pHit)->modelindex);
	MESSAGE_END();
	return 1;
}



//-----------------------------------------------------------------------------
// Purpose: SHL compatibility layer
//-----------------------------------------------------------------------------
#define SF_PARTICLE_START_ON		0x0001
#define SF_PARTICLE_PORTIONAL		0x0002

class CEnvParticleSpawner : public CPointEntity
{
public:
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int SendClientData(CBasePlayer *pClient, int msgtype);
};

LINK_ENTITY_TO_CLASS(env_particle, CEnvParticleSpawner);

void CEnvParticleSpawner::Spawn(void)
{
	CPointEntity::Spawn();

	UTIL_FixRenderColor(pev->rendermode, pev->rendercolor);// XDM3035a: IMPORTANT!

	if (FBitSet(pev->spawnflags, SF_PARTICLE_START_ON))
	{
		pev->impulse = 1;
//		SendClientData(NULL, MSG_ALL);
	}
	else
	{
		pev->impulse = 0;
	}
}

void CEnvParticleSpawner::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (pev->spawnflags & SF_PARTICLE_PORTIONAL)
	{
		SendClientData(NULL, MSG_BROADCAST);//MSG_ALL); ?
		pev->impulse++;// flag as enabled, also count
		return;
	}

	if (!ShouldToggle(useType, pev->impulse > 0))
		return;

	pev->impulse = !pev->impulse;

	if (pev->impulse)
		SendClientData(NULL, MSG_PVS);// MSG_ALL?
	else
		SendClientData(NULL, MSG_ALL);
}

int CEnvParticleSpawner::SendClientData(CBasePlayer *pClient, int msgtype)
{
	if (!pClient && msgtype == MSG_ONE)// a client has connected and needs an update
		return 0;

	unsigned short flags = 0;// temporary for now

	if (pev->impulse)// (was) enabled
	{
		if (pev->flags & FL_DRAW_ALWAYS)
		{
			flags |= RENDERSYSTEM_FLAG_DRAWALWAYS;
			if (msgtype == MSG_PVS)
				msgtype = MSG_ALL;
		}
	}
	else// not enabled
	{
		if (msgtype == MSG_ONE)// a client has connected and needs an update
			return 0;

		flags |= RENDERSYSTEM_FLAG_NODRAW;// equals destroy
	}

	MESSAGE_BEGIN(msgtype, gmsgEnvParticle, pev->origin, (pClient == NULL)?NULL : ENT(pClient->pev));
		WRITE_SHORT(entindex());// follow entity
		WRITE_SHORT(flags);
		WRITE_STRING(STRING(pev->message));
	MESSAGE_END();
	return 1;
}
