#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "skill.h"
#include "weapons.h"
#include "player.h"
#include "game.h"
#include "gamerules.h"
#include "util_vector.h"
#include "msg_fx.h"
#include "projectiles.h"
#include "decals.h"

#define TRIPMINE_POWERUP_TIME		2.0f

//=========================================================
// DeactivateMines - removes all mines owned by
// the provided player. Should only be used upon death.
//
// Made this global on purpose.
//=========================================================
void DeactivateMines(CBasePlayer *pOwner)
{
	CBaseEntity *pEntity = NULL;
	while ((pEntity = UTIL_FindEntityByClassname(pEntity, "monster_tripmine")) != NULL)
	{
		CTripmineGrenade *pMine = (CTripmineGrenade *)pEntity;
		if (pMine)
		{
			if (pOwner == NULL || pMine->m_pRealOwner == pOwner->edict())// if specified pOwner == NULL, deactivate ALL mines
				pMine->Deactivate(TRUE);// UNDONE: delay this a bit so the charge may detonate
		}
	}
}

LINK_ENTITY_TO_CLASS(monster_tripmine, CTripmineGrenade);

TYPEDESCRIPTION	CTripmineGrenade::m_SaveData[] =
{
	DEFINE_FIELD( CTripmineGrenade, m_flPowerUp, FIELD_TIME ),
	DEFINE_FIELD( CTripmineGrenade, m_vecDir, FIELD_VECTOR ),
	DEFINE_FIELD( CTripmineGrenade, m_vecEnd, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CTripmineGrenade, m_flBeamLength, FIELD_FLOAT ),
	DEFINE_FIELD( CTripmineGrenade, m_hOwner, FIELD_EHANDLE ),
	DEFINE_FIELD( CTripmineGrenade, m_pBeam, FIELD_CLASSPTR ),
	DEFINE_FIELD( CTripmineGrenade, m_posOwner, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CTripmineGrenade, m_angleOwner, FIELD_VECTOR ),
	DEFINE_FIELD( CTripmineGrenade, m_pRealOwner, FIELD_EDICT ),
};

IMPLEMENT_SAVERESTORE(CTripmineGrenade,CGrenade);


void CTripmineGrenade::Spawn(void)
{
	SET_MODEL(ENT(pev), "models/w_tripmine.mdl");// XDM
	pev->sequence = 2;// XDM: world
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_NOT;

	UTIL_SetSize(pev, Vector(-8, -8, -4), Vector(8, 8, 4));
	UTIL_SetOrigin(pev, pev->origin);

	pev->button = 0;
	m_flPowerUp = gpGlobals->time + TRIPMINE_POWERUP_TIME;

	SetThink(&CTripmineGrenade::PowerupThink);
	pev->nextthink = gpGlobals->time + 0.2;

	pev->takedamage = DAMAGE_YES;
	pev->dmg = gSkillData.DmgTripMine;
	pev->health = 1; // don't let die normally

	if (pev->owner != NULL)
	{
		// play deploy sound
		EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/mine_deploy.wav", VOL_NORM, ATTN_NORM);
		EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, "weapons/mine_charge.wav", 0.2, ATTN_NORM, 0, pev->button?110:PITCH_NORM); // chargeup
		m_pRealOwner = pev->owner;// see CTripmineGrenade for why.
	}
	UTIL_MakeAimVectors(pev->angles);
	m_vecDir = gpGlobals->v_forward;
	m_vecEnd = pev->origin + m_vecDir * g_psv_zmax->value;// XDM: this should be farther than first hit point in case door opens or smth.
}

void CTripmineGrenade::PowerupThink(void)
{
	TraceResult tr;

	if (m_hOwner == NULL)
	{
		// find an owner
		edict_t *oldowner = pev->owner;
		pev->owner = NULL;
		UTIL_TraceLine( pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 32, dont_ignore_monsters, ENT( pev ), &tr );
		if (tr.fStartSolid || (oldowner && tr.pHit == oldowner))
		{
			pev->owner = oldowner;
			m_flPowerUp += 0.1;
			pev->nextthink = gpGlobals->time + 0.1;
			return;
		}
		if (tr.flFraction < 1.0)
		{
			pev->owner = tr.pHit;
			m_hOwner = CBaseEntity::Instance(pev->owner);
			m_posOwner = m_hOwner->pev->origin;
			m_angleOwner = m_hOwner->pev->angles;
		}
		else
		{
			Deactivate((oldowner != NULL)?TRUE:FALSE);
			ALERT(at_console, "Tripmine at %g %g %g removed\n", pev->origin.x, pev->origin.y, pev->origin.z);
			return;
		}
	}

	else if (m_posOwner != m_hOwner->pev->origin || m_angleOwner != m_hOwner->pev->angles)
	{
		// disable
		STOP_SOUND(ENT(pev), CHAN_VOICE, "weapons/mine_beep.wav");
		KillBeam();

		CBaseEntity *pMine = Create("item_tripmine", pev->origin + m_vecDir * 64, pev->angles);
		if (pMine)
			pMine->pev->spawnflags |= SF_NORESPAWN;

		pev->health = 0.0;
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time + 0.1;
		return;
	}
	// ALERT( at_console, "%d %.0f %.0f %0.f\n", pev->owner, m_pOwner->pev->origin.x, m_pOwner->pev->origin.y, m_pOwner->pev->origin.z );

	if (gpGlobals->time > m_flPowerUp)
	{
		// make solid
		pev->solid = SOLID_BBOX;
		UTIL_SetOrigin(pev, pev->origin);

		MakeBeam();
		SetThink(&CTripmineGrenade::BeamBreakThink);

		EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "weapons/mine_beep.wav", 0.5, ATTN_NORM, 0, PITCH_NORM);// XDM3035a: something wrong was here
	}
	pev->nextthink = gpGlobals->time + 0.1;
}

void CTripmineGrenade::KillBeam(void)
{
	if (m_pBeam)
	{
		UTIL_Remove(m_pBeam);
		m_pBeam = NULL;
	}
}

// XDM: no thinking here, just the beam code
void CTripmineGrenade::MakeBeam(void)
{
	TraceResult tr;
	UTIL_TraceLine(pev->origin, m_vecEnd, dont_ignore_monsters, ENT(pev), &tr);
	m_flBeamLength = tr.flFraction;
	m_pBeam = CBeam::BeamCreate("sprites/beams_all.spr", 10);
	if (m_pBeam)
	{
		m_pBeam->SetFrame(BLAST_SKIN_PLASMA);
		m_pBeam->PointEntInit(tr.vecEndPos, entindex());
		m_pBeam->SetScrollRate(255);
		m_pBeam->SetBrightness(48);// XDM3035: tweaked
		m_pBeam->pev->rendercolor = Vector(200,0,0);
	}
}

void CTripmineGrenade::BeamBreakThink(void)
{
	BOOL bBlowup = 0;
	TraceResult tr;
	CBaseEntity *pOther = NULL;
	// HACKHACK Set simple box using this really nice global!
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;
	UTIL_TraceLine(pev->origin, m_vecEnd, dont_ignore_monsters, ENT(pev), &tr);
	gpGlobals->trace_flags = 0;
	pOther = CBaseEntity::Instance(tr.pHit);

	// respawn detect.
	if (m_pBeam == NULL)
	{
		MakeBeam();
		if (tr.pHit)
			m_hOwner = pOther;	// reset owner too
	}

	if (fabs(m_flBeamLength - tr.flFraction) > 0.001)
	{
		bBlowup = 1;
	}
	else
	{
		if (m_hOwner == NULL)
			bBlowup = 1;
		else if (m_posOwner != m_hOwner->pev->origin)
			bBlowup = 1;
		else if (m_angleOwner != m_hOwner->pev->angles)
			bBlowup = 1;
	}

	pev->nextthink = gpGlobals->time + 0.1;

	if (bBlowup)
	{
		// a bit of a hack, but all CGrenade code passes pev->owner along to make sure the proper player gets credit for the kill
		// so we have to restore pev->owner from pRealOwner, because an entity's tracelines don't strike it's pev->owner which meant
		// that a player couldn't trigger his own tripmine. Now that the mine is exploding, it's safe the restore the owner so the
		// CGrenade code knows who the explosive really belongs to.
		pev->owner = m_pRealOwner;
		pev->health = 0;
		CBaseEntity *pOwner = FNullEnt(pev->owner)?NULL:Instance(pev->owner);// XDM3037: avoid ASSERT
		Killed(pOwner, pOwner, GIB_NORMAL);
	}
}

int CTripmineGrenade::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (gpGlobals->time < m_flPowerUp && flDamage < pev->health)
	{
		pev->health = 0.0f;
		KillBeam();
		SetThink(&CBaseEntity::SUB_Remove);
		pev->nextthink = gpGlobals->time + 0.1;
		return FALSE;
	}
	if (!(bitsDamageType & DMGM_BREAK))
		return FALSE;

	Killed(pAttacker, pInflictor, GIB_NORMAL);// XDM3037
	return 1;
}

void CTripmineGrenade::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)
{
	pev->takedamage = DAMAGE_NO;
	KillBeam();// XDM: call this first! Teleportation grenade will setthink to fade out!!

	STOP_SOUND(ENT(pev), CHAN_BODY, "weapons/mine_charge.wav");

	if (pAttacker && pAttacker->IsPlayer())
		pev->owner = pAttacker->edict();// some client has destroyed this mine, he'll get credit for any kills

	SetThink(&CTripmineGrenade::DelayDeathThink);
	pev->nextthink = gpGlobals->time + 0.2;
}

void CTripmineGrenade::DelayDeathThink(void)
{
	pev->takedamage = DAMAGE_NO;
	pev->solid = SOLID_NOT;
	TraceResult tr;
	gpGlobals->trace_flags = FTRACE_SIMPLEBOX;// XDM3035c: decorative, simplify
	UTIL_TraceLine(pev->origin + m_vecDir * 8, pev->origin - m_vecDir * 64,  dont_ignore_monsters, ENT(pev), &tr);
	gpGlobals->trace_flags = 0;
	pev->owner = m_pRealOwner;

	UTIL_DecalTrace(&tr, DECAL_SCORCH1 + RANDOM_LONG(0,2));
	FX_Trail( tr.vecEndPos + (tr.vecPlaneNormal * 25), entindex(), FX_TRIPMINE_DETONATE );
	::RadiusDamage(tr.vecEndPos, this, Instance(pev->owner), pev->dmg, pev->dmg*2, CLASS_NONE, DMG_BLAST);
	UTIL_Remove(this);
}

void CTripmineGrenade::Deactivate(BOOL disintegrate)// XDM3035
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->deadflag = DEAD_DYING;// XDM3035
	if (m_flPowerUp > gpGlobals->time)
	{
		STOP_SOUND(ENT(pev), CHAN_BODY, "weapons/mine_charge.wav");
	}
	KillBeam();

	pev->health = 0.0f;
	SetThinkNull();

	if (disintegrate)
		Disintegrate();
	else
		UTIL_Remove(this);
}
