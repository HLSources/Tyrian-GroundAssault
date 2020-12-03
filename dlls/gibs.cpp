#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "pm_materials.h"
#include "func_break.h"
#include "weapons.h"
#include "sound.h"
#include "soundent.h"
#include "gamerules.h"
#include "globals.h"
#include "game.h"
#include "msg_fx.h"

#define GIB_MAX_VELOCITY		1600

LINK_ENTITY_TO_CLASS(gib, CGib);// XDM: world gibs

void CGib::KeyValue(KeyValueData *pkvd)// XDM
{
	if (FStrEq(pkvd->szKeyName, "bloodcolor"))
	{
		m_bloodColor = atoi(pkvd->szValue);// BLOOD_COLOR_RED
	}
	else if (FStrEq(pkvd->szKeyName, "blooddecals"))
	{
		m_cBloodDecals = atoi(pkvd->szValue);
	}
	else if (FStrEq(pkvd->szKeyName, "material"))
	{
		m_material = atoi(pkvd->szValue);
	}
	else if (FStrEq(pkvd->szKeyName, "life"))
	{
		m_lifeTime = atof(pkvd->szValue);
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

// XDM: world 'constant' gibs
void CGib::Spawn(void)
{
	if (!pev->model)
	{
		UTIL_Remove(this);
		return;
	}
	Precache();
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_NO;// XDM: don't die right after spawning (explosion), delay a bit
	pev->movetype = MOVETYPE_TOSS;
	pev->classname = MAKE_STRING("gib");

	int kvbody = pev->body;
	pev->body = 0;// temp hack to avoid crash
	SET_MODEL(ENT(pev), STRING(pev->model));
	if (kvbody < 0)
		pev->body = RANDOM_LONG(0, GetEntBodyCount(ENT(pev)) - 1);

//	ExtractBbox(GET_MODEL_PTR(edict()), 0, pev->mins, pev->maxs);
	UTIL_SetSize(pev, g_vecZero, g_vecZero);

	SetThinkNull();
	SetTouch(&CGib::BounceGibTouch);
	pev->nextthink = 0;
}

void CGib::SpawnGib(const char *szGibModel)
{
	pev->model = MAKE_STRING(szGibModel);
	Spawn();
	pev->movetype = MOVETYPE_BOUNCE;
	pev->friction = 0.75; // deading the bounce a bit
	// sometimes an entity inherits the edict from a former piece of glass, and will spawn using the same render FX or rendermode! bad!
	pev->renderamt = 255;// don't override in Spawn()!
	pev->rendermode = kRenderNormal;
	pev->renderfx = kRenderFxNone;
	m_material = matNone;
	m_cBloodDecals = 10;// how many blood decals this gib can place (1 per bounce until none remain). XDM: set in WaitTillLand()

	if (g_pGameRules->IsMultiplayer())// XDM
		m_lifeTime = 5.0f;
	else
		m_lifeTime = 20.0f;

	SetThink(&CGib::WaitTillLand);
//	SetTouch(&CGib::BounceGibTouch);
	pev->teleport_time = gpGlobals->time + m_lifeTime;
	pev->nextthink = gpGlobals->time + 0.4;// XDM
}

void CGib::Precache(void)// XDM
{
	PRECACHE_MODEL(STRINGV(pev->model));
}

int CGib::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if ((bitsDamageType & DMG_ALWAYSGIB) || (bitsDamageType & DMGM_BREAK))// XDM3033: a little hack to make gibs immune to teleporter grenade | UNHACKED
		Killed(pInflictor, pAttacker, (bitsDamageType & DMGM_GIB_CORPSE)?GIB_ALWAYS:GIB_NORMAL);

	return 1;
}

void CGib::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (bitsDamageType & DMGM_PUSH || bitsDamageType & DMG_BULLET)
		pev->velocity = pev->velocity + vecDir*flDamage;
}

void CGib::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)
{
	TraceResult tr;
	UTIL_TraceLine(pev->origin, pev->origin - Vector(0,0,8), ignore_monsters, ENT(pev), & tr);
	UTIL_BloodDecalTrace(&tr, m_bloodColor);
	if (g_pGameRules->FAllowEffects() && iGib == GIB_ALWAYS)
		UTIL_BloodDrips(pev->origin, UTIL_RandomBloodVector(), m_bloodColor, 8);

	MaterialSoundRandom(edict(), (Materials)m_material, 1.0);
	pev->health = 0;

//	if (pev->owner)// XDM3035c: UNDONE: what if a player is viewing through me?
//		donotremove?

	UTIL_Remove(this);
}

//-----------------------------------------------------------------------------
// Purpose: Override
// Output : float - final fall damage value
//-----------------------------------------------------------------------------
float CGib::FallDamage(const float &flFallVelocity)
{
	if (flFallVelocity > GIB_MAX_VELOCITY)
		return pev->health;// destroy

	return 0.0f;
}

// HACKHACK -- The gib velocity equations don't work
void CGib::LimitVelocity(void)
{
	float length = pev->velocity.Length();
	// The gib velocity equation is not bounded properly.  Rather than tune it
	// in 3 separate places again, I'll just limit it here.
	if (length > GIB_MAX_VELOCITY)
		pev->velocity = pev->velocity.Normalize() * GIB_MAX_VELOCITY;// This should really be sv_maxvelocity * 0.75 or something
}

//=========================================================
// WaitTillLand - in order to emit their meaty scent from
// the proper location, gibs should wait until they stop
// bouncing to emit their scent. That's what this function
// does.
//=========================================================
void CGib::WaitTillLand(void)
{
	if (!IsInWorld() || pev->teleport_time <= gpGlobals->time)
	{
		UTIL_Remove(this);
		return;
	}

	if (pev->velocity == g_vecZero)// may never be true
	{
		if (m_lifeTime >= 0.0f)
		{
			pev->solid = SOLID_NOT;
			pev->avelocity = g_vecZero;

			if (g_pGameRules->IsMultiplayer())
				SetThink(&CBaseEntity::SUB_Remove);
			else
				SetThink(&CBaseEntity::SUB_FadeOut);

			pev->nextthink = gpGlobals->time + m_lifeTime;
		}
		else
			SetThinkNull();

		// If you bleed, you stink!// ok, start stinkin!
		if (m_bloodColor != DONT_BLEED && !g_pGameRules->IsMultiplayer())// XDM: LAGLAG!
			CSoundEnt::InsertSound(bits_SOUND_MEAT, pev->origin, 384, 1.0);// XDM 25 );
	}
	else
	{
		// wait and check again in another half second.
		if (g_pGameRules->IsMultiplayer())
			pev->nextthink = gpGlobals->time + 0.5;
		else
			pev->nextthink = gpGlobals->time + 0.25;

		if (m_material == matFlesh || m_material == matWood)// XDM3034
		{
			if (pev->waterlevel >= 2)
			{
				pev->movetype = MOVETYPE_FLY;
				pev->velocity = pev->velocity * 0.5;
				pev->avelocity = pev->avelocity * 0.9;
				pev->gravity = 0.01;
				pev->velocity.z += 8.0;
			}
			else// if (pev->waterlevel == 0)
			{
				if (pev->movetype == MOVETYPE_FLY)// just exited water
				{
					pev->movetype = MOVETYPE_BOUNCE;// default
		//			pev->velocity.z *= 0.5;
					pev->velocity.z = 0;
					pev->gravity = 0.1;
				}
				else
					pev->gravity = 1.0;
			}
		}
	}
	// XDM: some post-initialization
	if (pev->takedamage == DAMAGE_NO)
		pev->takedamage = DAMAGE_YES;
}

// Gib bounces on the ground or wall, sponges some blood down, too!
void CGib::BounceGibTouch(CBaseEntity *pOther)
{
//	if ( RANDOM_LONG(0,1) )
//		return;// don't bleed everytime

	if (pev->flags & FL_ONGROUND)
	{
		if (pOther->IsPlayer())// XDM: player steps on the gib
			Killed(pOther, pOther, GIB_NORMAL);

		pev->velocity = pev->velocity * 0.95;
		pev->angles.x = 0;
//		pev->angles.z = 0;
		pev->avelocity.x = 0;
		pev->avelocity.z *= 0.5;
	}
	else
	{
		float vlen = pev->velocity.Length();
		if (vlen > 100)
		{
			TraceResult	tr;
			float volume = clamp(vlen * 0.002, 0.1, 1.0);
			MaterialSoundRandom(edict(), (Materials)m_material, volume);
			if (m_cBloodDecals > 0 && m_bloodColor != DONT_BLEED)
			{
				if (g_pGameRules->FAllowEffects())// XDM
				{
					UTIL_TraceLine(pev->origin + Vector(0,0,8), pev->origin + Vector(0, 0, -16), ignore_monsters, ENT(pev), & tr);
					UTIL_BloodDecalTrace(&tr, m_bloodColor);
					UTIL_BloodDrips(tr.vecEndPos, pev->velocity.Normalize() + tr.vecPlaneNormal, m_bloodColor, 2);
				}
				m_cBloodDecals--;
			}
			if (pOther->IsMonster() || pOther->IsPlayer())// XDM: only do damage if we're moving fairly fast
			{
				if (m_material != matNone && m_material != matFlesh && pev->dmgtime < gpGlobals->time )
				{
					TraceResult tr = UTIL_GetGlobalTrace();
					ClearMultiDamage();
					//ALERT(at_console, "GIB DAMAGE: %f\n", vlen * 0.005);
					pOther->TraceAttack(this, vlen * 0.005f, gpGlobals->v_forward, &tr, DMG_SLASH);
					ApplyMultiDamage(this, this);
					pev->dmgtime = gpGlobals->time + 1.0f; // debounce
				}
			}
		}
	}
}

// Sticky gib puts blood on the wall and stays put.
void CGib::StickyGibTouch(CBaseEntity *pOther)
{
	SetThink(&CBaseEntity::SUB_Remove);
	pev->nextthink = gpGlobals->time + 10.0f;

	if (pOther->IsBSPModel() && !pOther->IsMovingBSP())// XDM
	{
		TraceResult	tr;
		UTIL_TraceLine(pev->origin, pev->origin + pev->velocity * 32.0f,  ignore_monsters, ENT(pev), & tr);
		UTIL_BloodDecalTrace(&tr, m_bloodColor);

		pev->velocity = tr.vecPlaneNormal * -1.0f;
		pev->angles = UTIL_VecToAngles(pev->velocity);
		pev->velocity = g_vecZero;
		pev->avelocity = g_vecZero;
		pev->movetype = MOVETYPE_NONE;
	}
}

void CGib::SpawnStickyGibs(CBaseEntity *pVictim, const Vector &vecOrigin, int cGibs)
{
	int i;
	for (i = 0 ; i < cGibs ; i++)
	{
		CGib *pGib = GetClassPtr((CGib *)NULL, "gib");

		pGib->SpawnGib("models/effects/ef_player_gibs.mdl");
		pGib->pev->body = RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08);

		if (pVictim)
		{
			pGib->pev->origin.x = vecOrigin.x + RANDOM_FLOAT(-3, 3);
			pGib->pev->origin.y = vecOrigin.y + RANDOM_FLOAT(-3, 3);
			pGib->pev->origin.z = vecOrigin.z + RANDOM_FLOAT(-3, 3);
			// make the gib fly away from the attack vector
			pGib->pev->velocity = g_vecAttackDir * -1.0f;
			// mix in some noise
			pGib->pev->velocity.x += RANDOM_FLOAT(-0.15, 0.15);
			pGib->pev->velocity.y += RANDOM_FLOAT(-0.15, 0.15);
			pGib->pev->velocity.z += RANDOM_FLOAT(-0.15, 0.15);
			pGib->pev->velocity = pGib->pev->velocity * 900.0f;
			pGib->pev->avelocity.x = RANDOM_FLOAT(250.0f, 400.0f);
			pGib->pev->avelocity.y = RANDOM_FLOAT(250.0f, 400.0f);
			pGib->pev->avelocity.z = RANDOM_FLOAT(0, 100);
			// copy owner's blood color
			pGib->m_bloodColor = pVictim->BloodColor();

			if (pVictim->pev->health > -50.0f)
				pGib->pev->velocity = pGib->pev->velocity * 0.7f;
			else if ( pVictim->pev->health > -200)
				pGib->pev->velocity = pGib->pev->velocity * 2.0f;
			else
				pGib->pev->velocity = pGib->pev->velocity * 4.0f;

			pGib->pev->movetype = MOVETYPE_TOSS;
			pGib->pev->solid = SOLID_BBOX;
			pGib->SetTouch(&CGib::StickyGibTouch);
			pGib->SetThinkNull();
		}
		pGib->LimitVelocity();
	}
}

CGib *CGib::SpawnHeadGib(CBaseEntity *pVictim)
{
	CGib *pGib = GetClassPtr((CGib *)NULL, "gib");

	pGib->SpawnGib("models/effects/ef_player_gibs.mdl");
	pGib->pev->body = PLR_GIB_HEAD;
	pGib->m_material = matMetal;// XDM

	if (pVictim)
	{
		pGib->pev->origin = pVictim->pev->origin + pVictim->pev->view_ofs;
/*		edict_t *pentPlayer = FIND_CLIENT_IN_PVS( pGib->edict() );
		if ( RANDOM_LONG(0, 100) <= 5 && pentPlayer )
		{
			// 5% chance head will be thrown at player's face.
			entvars_t *pevPlayer = VARS( pentPlayer );
			pGib->pev->velocity = ((pevPlayer->origin + pevPlayer->view_ofs) - pGib->pev->origin).Normalize() * 300.0f;
			pGib->pev->velocity.z += 100.0f;
		}
		else*/
			pGib->pev->velocity = Vector (RANDOM_FLOAT(-100,100), RANDOM_FLOAT(-100,100), RANDOM_FLOAT(200,300));

		pGib->pev->avelocity.x = RANDOM_FLOAT(100, 200);
		pGib->pev->avelocity.y = RANDOM_FLOAT(100, 300);
		pGib->pev->avelocity.z = RANDOM_FLOAT(0, 100);

		pGib->m_bloodColor = DONT_BLEED;

		if (pVictim->pev->health > -50)
			pGib->pev->velocity = pGib->pev->velocity * 0.7f;
		else if (pVictim->pev->health > -200)
			pGib->pev->velocity = pGib->pev->velocity * 2.0f;
		else
			pGib->pev->velocity = pGib->pev->velocity * 4.0f;
	}
	pGib->LimitVelocity();
	return pGib;
}

void CGib::SpawnRandomGibs(CBaseEntity *pVictim, int cGibs, int human, Vector velocity)
{
	if (NUMBER_OF_ENTITIES() > (gpGlobals->maxEntities - ENTITY_INTOLERANCE))// XDM3035a: in case server uses 'real' gibs, preserve its stability
		return;

	if (!pVictim)
		return;

	float vk = 80.0f;// XDM3034
	if (UTIL_LiquidContents(pVictim->pev->origin))
		vk = 40.0f;

	if (sv_clientgibs.value > 0.0f)// XDM3035
	{
		int modelindex;
		modelindex = MODEL_INDEX("models/effects/ef_player_gibs.mdl");

		velocity *= vk;
		// TODO: create replacement for this
		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pVictim->pev->origin);
			WRITE_BYTE(TE_BREAKMODEL);
			WRITE_COORD(pVictim->pev->origin.x);
			WRITE_COORD(pVictim->pev->origin.y);
			WRITE_COORD(pVictim->pev->origin.z);
			WRITE_COORD(pVictim->pev->size.x);//size.x
			WRITE_COORD(pVictim->pev->size.y);//size.y
			WRITE_COORD(pVictim->pev->size.z);//size.z
			WRITE_COORD(velocity.x);//velocity.x
			WRITE_COORD(velocity.y);//velocity.y
			WRITE_COORD(velocity.z);//velocity.z
			WRITE_BYTE(10);//random velocity 0.1
			WRITE_SHORT(modelindex);
			WRITE_BYTE(cGibs);//count

			if (g_pGameRules->IsMultiplayer())// XDM
				WRITE_BYTE(50);//life 0.1
			else
				WRITE_BYTE(200);//life 0.1

			WRITE_BYTE(BREAK_FLESH);//flags
		MESSAGE_END();
	}
	else
	{

	for (int i = 0 ; i < cGibs; ++i)
	{
		CGib *pGib = GetClassPtr((CGib *)NULL, "gib");

		pGib->SpawnGib("models/effects/ef_player_gibs.mdl");
		pGib->pev->body = RANDOM_LONG(PLR_GIB_LEG01, PLR_GIB_METALL08);		

		// spawn the gib somewhere in the monster's bounding volume
		pGib->pev->origin.x = pVictim->pev->absmin.x + pVictim->pev->size.x * (RANDOM_FLOAT(0.0f, 1.0f));
		pGib->pev->origin.y = pVictim->pev->absmin.y + pVictim->pev->size.y * (RANDOM_FLOAT(0.0f, 1.0f));
		pGib->pev->origin.z = pVictim->pev->absmin.z + pVictim->pev->size.z * (RANDOM_FLOAT(0.0f, 1.0f)) + 1.0f;// absmin.z is in the floor because the engine subtracts 1 to enlarge the box

		// make the gib fly away from the attack vector
		// mix in some noise
		pGib->pev->velocity = (g_vecAttackDir*-4.0f + UTIL_RandomVector())*vk;
		pGib->pev->avelocity = UTIL_RandomVector()*vk*2.0f;

		// copy owner's blood color
		pGib->m_bloodColor = (CBaseEntity::Instance(pVictim->pev))->BloodColor();

		if (pVictim->pev->health < 0.0f)
			pGib->pev->velocity = pGib->pev->velocity + pGib->pev->velocity*pVictim->pev->health*-0.01f;

/*		if (pVictim->pev->health > -50)
			pGib->pev->velocity = pGib->pev->velocity * 0.7;
		else if ( pVictim->pev->health > -200)
			pGib->pev->velocity = pGib->pev->velocity * 2;
		else
			pGib->pev->velocity = pGib->pev->velocity * 4;*/

		pGib->m_material = matMetal;// XDM
		pGib->pev->solid = SOLID_BBOX;
		pGib->LimitVelocity();
	}
	}
}

// XDM: full customization (if count is <= 0, this function uses model body count)
int CGib::SpawnModelGibs(CBaseEntity *pVictim, const Vector &origin, const Vector &mins, const Vector &maxs, const Vector &velocity,
						int rndVel, int modelIndex, int count, float life, int material, int bloodcolor, int flags/* = 0*/)
{
	if (NUMBER_OF_ENTITIES() > (gpGlobals->maxEntities - ENTITY_INTOLERANCE))// XDM3035a: in case server uses 'real' gibs, preserve its stability
		return 0;

	int bodynum = 1;
	int i = 0;
	BOOL getcount = FALSE;
	if (count <= 0)
	{
		count = 1;// just to enter 'for'
		getcount = TRUE;
	}
	float vk = 80.0f;// XDM3034
	if (UTIL_LiquidContents(pVictim->pev->origin))
		vk = 40.0f;

//	velocity *= vk;

	if (sv_clientgibs.value > 0.0f)// XDM3035
	{
		// TODO: create replacement for this
		MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pVictim->pev->origin);
			WRITE_BYTE(TE_BREAKMODEL);
			WRITE_COORD(origin.x);
			WRITE_COORD(origin.y);
			WRITE_COORD(origin.z);
			WRITE_COORD(maxs.x - mins.x);//size.x
			WRITE_COORD(maxs.y - mins.y);//size.y
			WRITE_COORD(maxs.z - mins.z);//size.z
			WRITE_COORD(velocity.x * vk);//velocity.x
			WRITE_COORD(velocity.y * vk);//velocity.y
			WRITE_COORD(velocity.z * vk);//velocity.z
			WRITE_BYTE(rndVel);//random velocity 0.1
			WRITE_SHORT(modelIndex);
			WRITE_BYTE(count);//count
			WRITE_BYTE((int)(life*10.0f));//life 0.1
			WRITE_BYTE(flags);//flags BREAK_FLESH
		MESSAGE_END();

	}
	else
	{
	//count = (pVictim->pev->size.x + pVictim->pev->size.y + pVictim->pev->size.z) * 0.3;
	for (i = 0 ; i < count ; i++)
	{
		CGib *pGib = GetClassPtr((CGib *)NULL, "gib");// GetClassPtr(NULL) creates a new entity
		if (!pGib)
			continue;

		pGib->pev->solid = SOLID_SLIDEBOX;
		pGib->pev->takedamage = DAMAGE_NO;// XDM
		pGib->pev->movetype = MOVETYPE_BOUNCE;
//		pGib->pev->classname = MAKE_STRING("gib");
		pGib->pev->modelindex = modelIndex;
		pGib->pev->friction = 0.55;
		bodynum = GetEntBodyCount(pGib->edict());
		if (getcount)
		{
			count = bodynum;
			getcount = FALSE;
		}
		pGib->pev->nextthink = gpGlobals->time + 4;
		pGib->m_lifeTime = life;
		pGib->m_cBloodDecals = 0;
		pGib->m_bloodColor = bloodcolor;
		if (pVictim)
		{
			pGib->pev->rendermode = pVictim->pev->rendermode;
			pGib->pev->renderfx = pVictim->pev->renderfx;
			pGib->pev->renderamt = pVictim->pev->renderamt;
		}
		else
		{
			pGib->pev->rendermode = kRenderNormal;
			pGib->pev->renderfx = kRenderFxNone;
			pGib->pev->renderamt = 0;
		}
		if (material < 0)
		{
			if (FBitSet(flags, BREAK_GLASS))
				material = matGlass;
			else if (FBitSet(flags, BREAK_METAL))
				material = matMetal;
			else if (FBitSet(flags, BREAK_FLESH))
			{
				if (g_pGameRules->IsMultiplayer())
					pGib->m_cBloodDecals = 1;
				else
					pGib->m_cBloodDecals = 4;

				material = matFlesh;
			}
			else if (FBitSet(flags, BREAK_WOOD))
				material = matWood;
			else if (FBitSet(flags, BREAK_CONCRETE))
				material = matCinderBlock;

		}
		pGib->m_material = material;
		pGib->pev->body = RANDOM_LONG(0, bodynum-1);
		pGib->pev->origin = origin + RandomVectors(mins, maxs);
		pGib->pev->velocity = velocity + UTIL_RandomVector()*rndVel;
		pGib->pev->avelocity = UTIL_RandomVector()*200;
		pGib->SetThink(&CGib::WaitTillLand);
		pGib->SetTouch(&CGib::BounceGibTouch);

		if (pGib->m_bloodColor != DONT_BLEED)
			pGib->pev->takedamage = DAMAGE_YES;// XDM

		pGib->LimitVelocity();
	}
	}
	return i;
}
