/***
*
*	Copyright(c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc.("Id Technology").  Id Technology(c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

/*
==========================
This file contains "stubs" of class member implementations so that we can predict certain
 weapons client side.  From time to time you might find that you need to implement part of the
 these functions.  If so, cut it from here, paste it in hl_weapons.cpp or somewhere else and
 add in the functionality you need.
==========================
*/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "skill.h"
#include "util_vector.h"
#include "cl_dll.h"
#include "event_api.h"
#include "r_efx.h"
#include "pm_defs.h"

// Globals used by game logic
//int gmsgWeapPickup = 0;
enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;
struct skilldata_t  gSkillData;

ItemInfo g_ItemInfoArray[MAX_WEAPONS];

#ifdef _DEBUG
edict_t *DBG_EntOfVars(const entvars_t *pev)
{
	if (pev->pContainingEntity != NULL)
		return pev->pContainingEntity;

	ALERT(at_console, "entvars_t pContainingEntity is NULL, calling into engine\n");
	edict_t *pent = (*g_engfuncs.pfnFindEntityByVars)((entvars_t*)pev);
	if (pent == NULL)
		ALERT(at_console, "WARNING! FindEntityByVars failed!\n");

	((entvars_t *)pev)->pContainingEntity = pent;
	return pent;
}
#endif // _DEBUG


// Global Stubs
void EMIT_SOUND_DYN(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int flags, int pitch)
{
	gEngfuncs.pEventAPI->EV_PlaySound(ENTINDEX(entity), entity->v.origin, channel, sample, volume, attenuation, flags, pitch);
}

void ClearMultiDamage(void) { }
void ApplyMultiDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker) { }
void AddMultiDamage(CBaseEntity *pInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType) { }
void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage) { }
int DamageDecal(CBaseEntity *pEntity, const int &bitsDamageType) { return 0; }
void DecalGunshot(TraceResult *pTrace, const int &iBulletType) { }
//void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype) { }
//int AddAmmoNameToAmmoRegistry(const char *szAmmoname) { return 0; }
//int GetAmmoIndex(const char *psz) { return -1; }

// UTIL_* Stubs
void UTIL_PrecacheOther(const char *szClassname) { }
void UTIL_BloodDrips(const Vector &origin, const Vector &direction, int color, int amount) { }
void UTIL_DecalTrace(TraceResult *pTrace, int decalNumber) { }
void UTIL_GunshotDecalTrace(TraceResult *pTrace, int decalNumber) { }

/*void UTIL_SetOrigin(entvars_t *pev, const Vector &vecOrigin)
{
	SET_ORIGIN(ENT(pev), vecOrigin);
	pev->origin = vecOrigin;
}*/

BOOL UTIL_GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon) { return TRUE; }
void UTIL_LogPrintf(char *fmt, ...)
{
//?	COM_Log(NULL, fmt, argptr);
}

void UTIL_ClientPrintAll(int,char const *,char const *,char const *,char const *,char const *) { }
void ClientPrint(entvars_t *client, int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4) { }

void StreakSplash(const Vector &origin, const Vector &direction, int color, int count, int speed, int velocityRange)
{
	gEngfuncs.Con_DPrintf("CL: ERROR! Obsolete StreakSplash() called!\n");
	gEngfuncs.pEfxAPI->R_StreakSplash(origin, direction, color, count, speed, -velocityRange, velocityRange);
}

BOOL IsColdDamage(int bitsDamageType)
{
	if((bitsDamageType & DMG_FREEZE) ||(bitsDamageType & DMG_SLOWFREEZE))
		return TRUE;
	else
		return FALSE;
}

BOOL IsFireDamage(int bitsDamageType)
{
	if((bitsDamageType & DMG_BURN) ||(bitsDamageType & DMG_SLOWBURN))
		return TRUE;
	else
		return FALSE;
}

void UTIL_Remove(CBaseEntity *pEntity)
{
	if (pEntity == NULL)
		return;

	if (pEntity->ShouldRespawn())// XDM3035
	{
//		pEntity->SetThink(&CBaseEntity::SUB_Respawn);
//		pEntity->pev->nextthink = gpGlobals->time + mp_monsrespawntime.value;// XDM: TODO
	}
	else
	{
		pEntity->UpdateOnRemove();
		pEntity->pev->flags |= FL_KILLME;
		pEntity->pev->targetname = 0;
//		ALERT(at_console, "UTIL_Remove(%s)\n", STRING(pEntity->pev->classname));
//crash		REMOVE_ENTITY(ENT(pEntity->pev));// XDM3035
	}
}

//-----------------------------------------------------------------------------
// Purpose: Edict version
// Input  : *pent - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool UTIL_IsValidEntity(edict_t *pent)
{
	if (pent == NULL || pent->free || (pent->v.flags & FL_KILLME))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Validate entity by all means! MUST BE BULLETPROOF!
// Input  : *pEntity - test subject
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool UTIL_IsValidEntity(CBaseEntity *pEntity)
{
	try
	{
		if (pEntity)
		{
			if (pEntity->pev == NULL)
				return false;
			if (pEntity->pev->flags & FL_KILLME)
				return false;
			if (pEntity->edict() == NULL)
				return false;
			if (pEntity->edict()->free)
				return false;

			return true;
		}
	}
	catch (...)
	{
		gEngfuncs.Con_Printf("ERROR: CL: UTIL_IsValidEntity() exception!\n");
		DBG_FORCEBREAK
	}
	return false;
}

void UTIL_SetSize(entvars_t *pev, const Vector &vecMin, const Vector &vecMax){ }
CBaseEntity *UTIL_FindEntityInSphere(CBaseEntity *pStartEntity, const Vector &vecCenter, float flRadius){ return 0;}
Vector UTIL_VecToAngles(const Vector &vec)
{
	Vector ang;
	VectorAngles(vec, ang);
	return ang;
}

float UTIL_GetWeaponWorldScale(void)
{
	if (gpGlobals->deathmatch > 0.0f)// stub
		return 1.25f;//, WEAPON_WORLD_SCALE);
	else
		return 1.0f;
}

int RANDOM_INT2(int a, int b)
{
	if (RANDOM_LONG(0,1) == 1)
		return b;

	return a;
}

int TrainSpeed(int iSpeed, int iMax) { 	return 0; }



EHANDLE::EHANDLE(void)// XDM3035
{
	m_pent = NULL;
	m_serialnumber = 0;
}



// CBaseEntity Stubs

CBaseEntity::CBaseEntity()
{
}

void CBaseEntity::KeyValue(struct KeyValueData_s *) { }

void CBaseEntity::Spawn(void)
{
	Precache();
}
void CBaseEntity::Spawn(byte restore)
{
	Spawn();
}
void CBaseEntity::Precache(void)
{
}

int CBaseEntity::Save(CSave &save) { return 1; }
int CBaseEntity::Restore(CRestore &restore) { return 1; }
void CBaseEntity::OnFreePrivateData(void) {};// XDM3035
void CBaseEntity::PrepareForTransfer(void)// XDM3037
{
#if defined(MOVEWITH)
	//m_pMoveWith ?
	m_pChildMoveWith = NULL;
	m_pSiblingMoveWith = NULL;
	m_pAssistLink = NULL;
#endif
}

int CBaseEntity::ObjectCaps(void) { return 0; }
int CBaseEntity::ShouldCollide(CBaseEntity *pOther) { return 1; }// XDM3035
void CBaseEntity::SetObjectCollisionBox(void) { }
int CBaseEntity::TakeHealth(const float &flHealth, const int &bitsDamageType) { return 1; }
int CBaseEntity::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType) { return 1; }
CBaseEntity *CBaseEntity::GetNextTarget(void) { return NULL; }

int	CBaseEntity::Intersects(CBaseEntity *pOther)
{
	if (pOther->pev->absmin.x > pev->absmax.x ||
		pOther->pev->absmin.y > pev->absmax.y ||
		pOther->pev->absmin.z > pev->absmax.z ||
		pOther->pev->absmax.x < pev->absmin.x ||
		pOther->pev->absmax.y < pev->absmin.y ||
		pOther->pev->absmax.z < pev->absmin.z )
		return 0;
	return 1;
}

void CBaseEntity::MakeDormant(void)
{
	SetBits(pev->flags, FL_DORMANT);
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	SetBits(pev->effects, EF_NODRAW);
	pev->nextthink = 0;
	UTIL_SetOrigin(pev, pev->origin);
}

int CBaseEntity::IsDormant(void)
{
	return FBitSet(pev->flags, FL_DORMANT);
}

BOOL CBaseEntity::IsInWorld(void) { return TRUE; }
BOOL CBaseEntity::IsPlayer(void)
{
	if (pev && (pev->flags & FL_CLIENT))
		return TRUE;

	return FALSE;
}

BOOL CBaseEntity::HasTarget(string_t targetname)
{
	return FStrEq(STRING(targetname), STRING(pev->targetname));
}

int CBaseEntity::ShouldToggle(USE_TYPE useType, bool currentState)
{
	if (useType != USE_TOGGLE && useType != USE_SET)
	{
		if ((currentState && useType == USE_ON) || (!currentState && useType == USE_OFF))
			return 0;
	}
	return 1;
}

int	CBaseEntity::DamageDecal(const int &bitsDamageType) { return -1; }
CBaseEntity *CBaseEntity::Create(int iName, const Vector &vecOrigin, const Vector &vecAngles, const Vector &vecVeloity, edict_t *pentOwner, int spawnflags) { return NULL; }// XDM3035: we really NEED this!
CBaseEntity *CBaseEntity::Create(const char *szName, const Vector &vecOrigin, const Vector &vecAngles, const Vector &vecVelocity, edict_t *pentOwner) { return NULL; }// XDM
CBaseEntity *CBaseEntity::Create(const char *szName, const Vector &vecOrigin, const Vector &vecAngles, edict_t *pentOwner) { return NULL; }
CBaseEntity *CBaseEntity::Instance(edict_t *pent) { return (CBaseEntity *)GET_PRIVATE(pent); }
void CBaseEntity::SUB_Remove(void) { pev->health = 0; }
void CBaseEntity::SUB_FadeOut(void) { }
void CBaseEntity::SUB_Disintegrate(void) { }// XDM3035
void CBaseEntity::Disintegrate(void) { }// XDM3035
void CBaseEntity::AnnihilateProj(void) { }
void CBaseEntity::SetNextThink(const float &delay)
{
	pev->nextthink = gpGlobals->time + delay;
}
void CBaseEntity::UpdateOnRemove(void) {}// XDM3034
void CBaseEntity::SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value) {}// XDM3034
CBaseEntity *CBaseEntity::Respawn(void) { return this; }// XDM3035
float CBaseEntity::DamageForce(const float &damage) { return 0.0f; };// XDM3035
bool CBaseEntity::FVisible(CBaseEntity *pEntity) { return false; }
bool CBaseEntity::FVisible(const Vector &vecOrigin) { return false; }
bool CBaseEntity::FBoxVisible(CBaseEntity *pTarget, Vector &vecTargetOrigin, float flSize) { return false; }
void CBaseEntity::SetModelCollisionBox(void) {};// XDM3035b
void CBaseEntity::AlignToFloor(void) {};// XDM3035b
void CBaseEntity::CheckEnvironment(void) {};// XDM3035b

/*
=====================
CBaseEntity :: Killed

If weapons code "kills" an entity, just set its effects to EF_NODRAW
=====================
*/
void CBaseEntity::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)
{
	pev->effects |= EF_NODRAW;
}

/*
=====================
CBaseEntity::FireBulletsPlayer

Only produces random numbers to match the server ones.
=====================
*/
Vector FireBullets(ULONG cShots, const Vector &vecSrc, const Vector &vecDirShooting, const Vector &vecSpread, Vector *endpos, float flDistance, int iBulletType, int iDamage, CBaseEntity *pInflictor, CBaseEntity *pAttacker, int shared_rand)
{
	if (pInflictor == NULL)
		return vecSpread;

	if (pAttacker == NULL)
		pAttacker = pInflictor;// the default attacker is ourselves

	float x = 0, y = 0, z = 0;
	for (ULONG iShot = 1; iShot <= cShots; ++iShot)
	{
		if (pAttacker)// Use player's random seed
		{
			x = UTIL_SharedRandomFloat(shared_rand + /*0+*/iShot, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + (1 + iShot), -0.5, 0.5);
			y = UTIL_SharedRandomFloat(shared_rand + (2 + iShot), -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + (3 + iShot), -0.5, 0.5);
			z = x*x + y*y;
		}
		else// get circular gaussian spread
		{
			do
			{
				x = RANDOM_FLOAT(-0.5, 0.5) + RANDOM_FLOAT(-0.5, 0.5);
				y = RANDOM_FLOAT(-0.5, 0.5) + RANDOM_FLOAT(-0.5, 0.5);
				z = x*x+y*y;
			} while (z > 1);
		}
	}
	return Vector(x*vecSpread.x, y*vecSpread.y, 0.0f);
}
/*
Vector CBaseEntity::FireBulletsPlayer(ULONG cShots, const Vector &vecSrc, const Vector &vecDirShooting, const Vector &vecSpread, Vector *endpos, float flDistance, int iBulletType, int iDamage, CBaseEntity *pInflictor, CBaseEntity *pAttacker, int shared_rand)
{
	return CBaseEntity::FireBullets(cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, iDamage, pAttacker, shared_rand);
}
*/


// CBaseDelay Stubs
CBaseDelay::CBaseDelay() : CBaseEntity()
{
	m_iState = STATE_ON;// well, in HL entities are mostly ON unless _START_OFF flag is specified for them
}

void CBaseDelay::KeyValue(struct KeyValueData_s *) { }
void CBaseDelay::Spawn(void) { }// XDM3035c
int CBaseDelay::Save(class CSave &save) { return 1; }
int CBaseDelay::Restore(class CRestore &restore) { return 1; }
void CBaseDelay::SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value) {}// XDM3034
void CBaseDelay::SetState(STATE newstate)// XDM3035c
{
	if (m_iState != newstate)//&& CanChangeState(newstate))
	{
		STATE oldstate = m_iState;
		m_iState = newstate;
		this->OnStateChange(oldstate);
	}
}
void CBaseDelay::OnStateChange(STATE oldstate) { }
BOOL CBaseDelay::IsLockedByMaster(void) { return FALSE; }// XDM3035c
void CBaseDelay::DelayThink(void) { }// XDM3034

// CBaseToggle Stubs
int CBaseToggle::Restore(class CRestore &restore) { return 1; }
int CBaseToggle::Save(class CSave &save) { return 1; }
void CBaseToggle::KeyValue(struct KeyValueData_s *) { }
void CBaseToggle::LinearMove(const Vector &vecDest, const float &flSpeed) { }
void CBaseToggle::AngularMove(const Vector &vecDestAngles, const float &flSpeed) { }

// CBaseAnimating Stubs
void CBaseAnimating::Spawn(void) { }// XDM
int CBaseAnimating::Restore(class CRestore &) { return 1; }
int CBaseAnimating::Save(class CSave &) { return 1; }
void CBaseAnimating::HandleAnimEvent(MonsterEvent_t *pEvent) { };// XDM3035b
int CBaseAnimating::LookupActivity(int activity) { return 0; }
int CBaseAnimating::LookupActivityHeaviest(int activity) { return 0; }
int CBaseAnimating::LookupSequence(const char *label) { return 0; }
void CBaseAnimating::ResetSequenceInfo(void) { }
BOOL CBaseAnimating::GetSequenceFlags(void) { return FALSE; }
void CBaseAnimating::DispatchAnimEvents(float flInterval) { }
float CBaseAnimating::SetBoneController(int iController, float flValue) { return 0.0; }
void CBaseAnimating::InitBoneControllers(void) { }
float CBaseAnimating::SetBlending(byte iBlender, float flValue) { return 0; }
void CBaseAnimating::GetBonePosition(int iBone, Vector &origin, Vector &angles) { }
void CBaseAnimating::GetAttachment(int iAttachment, Vector &origin, Vector &angles) { }
int CBaseAnimating::FindTransition(int iEndingSequence, int iGoalSequence, int *piDir) { return -1; }
void CBaseAnimating::GetAutomovement(Vector &origin, Vector &angles, float flInterval) { }
void CBaseAnimating::SetBodygroup(int iGroup, int iValue) { }
int CBaseAnimating::GetBodygroup(int iGroup) { return 0; }
float CBaseAnimating::StudioFrameAdvance(float flInterval) { return 0.0; }


void CBaseMonster::SetEyePosition(void) { }
BOOL CBaseMonster::ShouldGibMonster(int iGib) { return FALSE; };
void CBaseMonster::CallGibMonster(void) {};
CBaseEntity *CBaseMonster::CheckTraceHullAttack(const float &flDist, const int &iDamage, const int &iDmgType) { return NULL; }
void CBaseMonster::Eat(float flFullDuration) { }
BOOL CBaseMonster::FShouldEat(void) { return TRUE; }
void CBaseMonster::BarnacleVictimBitten(CBaseEntity *pBarnacle) { }
void CBaseMonster::BarnacleVictimReleased(void) { }
void CBaseMonster::Listen(void) { }
float CBaseMonster::FLSoundVolume(CSound *pSound) { return 0.0; }
BOOL CBaseMonster::FValidateHintType(short sHint) { return FALSE; }
void CBaseMonster::Look(int iDistance) { }
int CBaseMonster::ISoundMask(void) { return 0; }
CSound *CBaseMonster::PBestSound(void) { return NULL; }
CSound *CBaseMonster::PBestScent(void) { return NULL; } 
float CBaseMonster::FallDamage(const float &flFallVelocity) { return 1.0f; };// XDM3035c
void CBaseMonster::MonsterThink(void) { }
void CBaseMonster::MonsterUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) { }
int CBaseMonster::IgnoreConditions(void) { return 0; }
void CBaseMonster::RouteClear(void) { }
void CBaseMonster::RouteNew(void) { }
BOOL CBaseMonster::FRouteClear(void) { return FALSE; }
BOOL CBaseMonster::FRefreshRoute(void) { return 0; }
BOOL CBaseMonster::MoveToEnemy(Activity movementAct, float waitTime) { return FALSE; }
BOOL CBaseMonster::MoveToLocation(Activity movementAct, float waitTime, const Vector &goal) { return FALSE; }
BOOL CBaseMonster::MoveToTarget(Activity movementAct, float waitTime) { return FALSE; }
BOOL CBaseMonster::MoveToNode(Activity movementAct, float waitTime, const Vector &goal) { return FALSE; }
int ShouldSimplify(int routeType) { return TRUE; }
void CBaseMonster::RouteSimplify(CBaseEntity *pTargetEnt) { }
BOOL CBaseMonster::FBecomeProne(void) { return TRUE; }
BOOL CBaseMonster::CheckRangeAttack1(float flDot, float flDist) { return FALSE; }
BOOL CBaseMonster::CheckRangeAttack2(float flDot, float flDist) { return FALSE; }
BOOL CBaseMonster::CheckMeleeAttack1(float flDot, float flDist) { return FALSE; }
BOOL CBaseMonster::CheckMeleeAttack2(float flDot, float flDist) { return FALSE; }
void CBaseMonster::CheckAttacks(CBaseEntity *pTarget, const float &flDist) { }
BOOL CBaseMonster::FCanCheckAttacks(void) { return FALSE; }
int CBaseMonster::CheckEnemy(CBaseEntity *pEnemy) { return 0; }
void CBaseMonster::PushEnemy(CBaseEntity *pEnemy, Vector &vecLastKnownPos) { }
BOOL CBaseMonster::PopEnemy(void) { return FALSE; }
void CBaseMonster::SetActivity(Activity NewActivity) { }
void CBaseMonster::SetSequenceByName(char *szSequence) { }
int CBaseMonster::CheckLocalMove(const Vector &vecStart, const Vector &vecEnd, CBaseEntity *pTarget, float *pflDist) { return 0; }
float CBaseMonster::OpenDoorAndWait(CBaseEntity *pDoor) { return 0.0; }
void CBaseMonster::AdvanceRoute(float distance) { }
int CBaseMonster::RouteClassify(int iMoveFlag) { return 0; }
BOOL CBaseMonster::BuildRoute(const Vector &vecGoal, int iMoveFlag, CBaseEntity *pTarget) { return FALSE; }
void CBaseMonster::InsertWaypoint(const Vector &vecLocation, const int &afMoveFlags) { }
BOOL CBaseMonster::FTriangulate(const Vector &vecStart , const Vector &vecEnd, float flDist, CBaseEntity *pTargetEnt, Vector *pApex) { return FALSE; }
void CBaseMonster::Move(float flInterval) { }
BOOL CBaseMonster::ShouldAdvanceRoute(float flWaypointDist) { return FALSE; }
void CBaseMonster::MoveExecute(CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval) { }
void CBaseMonster::MonsterInit(void) { }
void CBaseMonster::MonsterInitThink(void) { }
void CBaseMonster::StartMonster(void) { }
void CBaseMonster::MovementComplete(void) { }
int CBaseMonster::TaskIsRunning(void) { return 0; }
int CBaseMonster::IRelationship(CBaseEntity *pTarget) { return 0; }
BOOL CBaseMonster::FindCover(const Vector &vecThreat, const Vector &vecViewOffset, float flMinDist, float flMaxDist) { return FALSE; }
BOOL CBaseMonster::BuildNearestRoute(const Vector &vecThreat, const Vector &vecViewOffset, float flMinDist, float flMaxDist) { return FALSE; }
CBaseEntity *CBaseMonster::BestVisibleEnemy(void) { return NULL; }
BOOL CBaseMonster::FInViewCone(CBaseEntity *pEntity) { return FALSE; }
BOOL CBaseMonster::FInViewCone(const Vector &origin) { return FALSE; }
void CBaseMonster::MakeIdealYaw(const Vector &vecTarget) { }
float CBaseMonster::FlYawDiff(void) { return 0.0; }
float CBaseMonster::ChangeYaw(float yawSpeed) { return 0; }
float CBaseMonster::VecToYaw(const Vector &vecDir) { return 0.0f; }
void CBaseMonster::HandleAnimEvent(MonsterEvent_t *pEvent) { }
Vector CBaseMonster::ShootAtEnemy(const Vector &shootOrigin) { return g_vecZero; }
Vector CBaseMonster::BodyTarget(const Vector &posSrc) { return Center(); }
Vector CBaseMonster::GetGunPosition(void) { return pev->origin; }
void CBaseEntity::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType) { }
void CBaseEntity::TraceBleed(const float &flDamage, const Vector &vecDir, TraceResult *ptr, const int &bitsDamageType) { }
BOOL CBaseMonster::FGetNodeRoute(const Vector &vecDest) { return TRUE; }
int CBaseMonster::FindHintNode(void) { return -1; }
void CBaseMonster::ReportAIState(void) { }
void CBaseMonster::KeyValue(KeyValueData *pkvd) { }
BOOL CBaseMonster::FCheckAITrigger(void) { return FALSE; }
int CBaseMonster::CanPlaySequence(BOOL fDisregardMonsterState, int interruptLevel) { return FALSE; }
BOOL CBaseMonster::FindLateralCover(const Vector &vecThreat, const Vector &vecViewOffset) { return FALSE; }
BOOL CBaseMonster::FacingIdeal(void) { return FALSE; }
BOOL CBaseMonster::FCanActiveIdle(void) { return FALSE; }
void CBaseMonster::PlaySentence(const char *pszSentence, float duration, float volume, float attenuation) { }
void CBaseMonster::PlayScriptedSentence(const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener) { }
void CBaseMonster::SentenceStop(void) { }
void CBaseMonster::CorpseFallThink(void) { }
void CBaseMonster::MonsterInitDead(void) { }
BOOL CBaseMonster::BBoxFlat(void) { return TRUE; }
BOOL CBaseMonster::GetEnemy(void) { return FALSE; }
void CBaseMonster::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType) { }
CBaseEntity *CBaseMonster::DropItem(const char *pszItemName, const Vector &vecPos, const Vector &vecAng) { return NULL; }
BOOL CBaseMonster::ShouldFadeOnDeath(void) { return FALSE; }
void CBaseMonster::RadiusDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int iClassIgnore, int bitsDamageType) { }
void CBaseMonster::RadiusDamage(const Vector &vecSrc, CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int iClassIgnore, int bitsDamageType) { }
void CBaseMonster::FadeMonster(void) { }
void CBaseMonster::DeathSound(void) { }
bool CBaseMonster::GibMonster(void) { return false; }
BOOL CBaseMonster::HasHumanGibs(void) { return FALSE; }
BOOL CBaseMonster::HasAlienGibs(void) { return FALSE; }
Activity CBaseMonster::GetDeathActivity(void) { return ACT_DIE_HEADSHOT; }
MONSTERSTATE CBaseMonster::GetIdealState(void) { return MONSTERSTATE_ALERT; }
Schedule_t *CBaseMonster::GetScheduleOfType(int Type) { return NULL; }
Schedule_t *CBaseMonster::GetSchedule(void) { return NULL; }
void CBaseMonster::RunTask(Task_t *pTask) { }
void CBaseMonster::StartTask(Task_t *pTask) { }
Schedule_t *CBaseMonster::ScheduleFromName(const char *pName) { return NULL;}
void CBaseMonster::BecomeDead(void) {}
void CBaseMonster::RunAI(void) {}
void CBaseMonster::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib) {}
BOOL CBaseMonster::IsHeavyDamage(float flDamage, int bitsDamageType)// XDM: same as in server.DLL
{
	if (flDamage <= 0)
		return FALSE;

	if (bitsDamageType & DMG_DROWNRECOVER)
		return FALSE;

	if (pev->max_health / flDamage <= 2.8)// ~36% and more!
		return TRUE;
	else
		return FALSE;
}
BOOL CBaseMonster::IsHuman(void) { return FALSE; };// XDM3035b
BOOL CBaseMonster::ShouldRespawn(void) { return FALSE; };// XDM3035
int CBaseMonster::TakeHealth(const float &flHealth, const int &bitsDamageType) { return 0; }
int CBaseMonster::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType) { return 0; }
int CBaseMonster::Restore(class CRestore &) { return 1; }
int CBaseMonster::Save(class CSave &) { return 1; }
void CBaseMonster::FrozenThink(void) { }// XDM
void CBaseMonster::FrozenEnd(void) { }
void CBaseMonster::FrozenStart(float freezetime) { }
BOOL CBaseMonster::CanAttack(void)
{
	if (m_fFrozen)
		return FALSE;

	if (m_flNextAttack > gpGlobals->time)
		return FALSE;

	return TRUE;
}
// XDM3035c: special
BOOL CBaseMonster::HasTarget(string_t targetname)
{
	if (FStrEq(STRING(targetname), STRING(m_iszTriggerTarget)))
		return true;

	return CBaseToggle::HasTarget(targetname);
}


// CBasePlayer Stubs
int  CBasePlayer::Classify(void) { return 0; }
void CBasePlayer::Precache(void) { }
void CBasePlayer::Touch(CBaseEntity *pOther) { };
int CBasePlayer::Save(CSave &save) { return 0; }
int CBasePlayer::Restore(CRestore &restore) { return 0; }
void CBasePlayer::Jump(void) { }
void CBasePlayer::Duck(void) { }
BOOL CBasePlayer::IsPushable(void) { return TRUE; }
BOOL CBasePlayer::PlaceMine(void) { return TRUE; }
void CBasePlayer::PreThink(void) { }
void CBasePlayer::PostThink(void) { }
void CBasePlayer::DeathSound(void) { }
void CBasePlayer::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType) { }
int CBasePlayer::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType) { return 0; }
void CBasePlayer::PackDeadPlayerItems(void) { }
void CBasePlayer::RemoveAllItems(bool removeSuit) { }
void CBasePlayer::SetAnimation(PLAYER_ANIM playerAnim) { }
void CBasePlayer::SetWeaponAnimType(const char *szExtention)
{
	if (szExtention)
		strcpy(m_szAnimExtention, szExtention);
}
void CBasePlayer::WaterMove(void) { }
BOOL CBasePlayer::IsOnLadder(void) { return FALSE; }
bool CBasePlayer::IsObserver(void) { return false; }//g_PlayerInfoList[index].doesnotwork; }
void CBasePlayer::PlayerDeathThink(void) { }
//void CBasePlayer::StartDeathCam(void) { }
//void CBasePlayer::DeathCamObserver(Vector vecPosition, Vector vecViewAngle) { }

void CBasePlayer::StartObserver(const Vector &vecPosition, const Vector &vecViewAngle, int mode, CBaseEntity *pTarget) { }// XDM
void CBasePlayer::StopObserver(void) { }
void CBasePlayer::Observer_FindNextPlayer(bool bReverse) { }
void CBasePlayer::Observer_HandleButtons(void) { }
void CBasePlayer::Observer_SetMode(int iMode) { }
void CBasePlayer::FrozenThink(void) { }
void CBasePlayer::FrozenEnd(void) { }
void CBasePlayer::FrozenStart(float freezetime) { }
void CBasePlayer::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value) { }

void CBasePlayer::PlayerUse(void) { }
void CBasePlayer::CheckTimeBasedDamage(void)  { }
void CBasePlayer::UpdateGeigerCounter(void) { }
void CBasePlayer::UpdatePlayerSound(void) { }
BOOL CBasePlayer::HasWeapons(void) { return FALSE; }
CBaseEntity *FindEntityForward(CBaseEntity *pMe) { return NULL; }
BOOL CBasePlayer::FlashlightIsOn(void) { return FALSE; }
void CBasePlayer::FlashlightTurnOn(void) { }
void CBasePlayer::FlashlightTurnOff(void) { }
void CBasePlayer::ForceClientDllUpdate(void) { }
void CBasePlayer::ImpulseCommands(void) { }
void CBasePlayer::CheatImpulseCommands(const int &iImpulse) { }
int CBasePlayer::AddPlayerItem(CBasePlayerItem *pItem) { return FALSE; }
int CBasePlayer::RemovePlayerItem(CBasePlayerItem *pItem) { return FALSE; }
void CBasePlayer::ItemPreFrame(void) { }
void CBasePlayer::ItemPostFrame(void) { }
int CBasePlayer::AmmoInventory(const int &iAmmoIndex)
{
	if (iAmmoIndex < 0)
		return -1;

	return m_rgAmmo[iAmmoIndex];
}
void CBasePlayer::TabulateAmmo(void) { }
void CBasePlayer::SendAmmoUpdate(void) { }
void CBasePlayer::SendWeaponsUpdate(void) { }// XDM
void CBasePlayer::UpdateClientData(void) { }
BOOL CBasePlayer::FBecomeProne(void) { return TRUE; }
void CBasePlayer::BarnacleVictimBitten(CBaseEntity *pBarnacle) { }
void CBasePlayer::BarnacleVictimReleased(void) { }
int CBasePlayer::Illumination(void) { return 0; }
void CBasePlayer::EnableControl(bool fControl) { }
float CBasePlayer::GetShootSpreadFactor(void) { return 1.0f; };// XDM3037
Vector CBasePlayer::GetAutoaimVector(const float &flDelta) { return g_vecZero; }
Vector CBasePlayer::AutoaimDeflection(const Vector &vecSrc, const float &flDist, const float &flDelta) { return g_vecZero; }
void CBasePlayer::ResetAutoaim(void) { }
void CBasePlayer::SetCustomDecalFrames(int nFrames) { }
int CBasePlayer::GetCustomDecalFrames(void) { return -1; }
BOOL CBasePlayer::DropPlayerItem(CBasePlayerItem *pItem) { return FALSE; }
BOOL CBasePlayer::DropPlayerItem(char *pszItemName) { return FALSE; }
Vector CBasePlayer::GetGunPosition(void) { return pev->origin + pev->view_ofs; }
int CBasePlayer::GiveAmmo(const int &iAmount, const int &iIndex, const int &iMax) { return 0; }
int CBasePlayer::GiveAmmo(const int &iAmount, char *szName, const int &iMax) { return 0; }
float CBasePlayer::FallDamage(const float &flFallVelocity) { return 10.0f; };// XDM3035c
float CBasePlayer::DamageForce(const float &damage)// XDM3035
{
	return damage*3.0f;
}
BOOL CBasePlayer::IsAdministrator(void)  { return FALSE; }// XDM3035
BOOL CBasePlayer::CanAttack(void)// XDM3035
{
	if (m_fFrozen)
		return FALSE;

//	if (IsOnLadder() && (mp_laddershooting.value <= 0.0f))// XDM3035: TESTME
//		return FALSE;

	if (m_flNextAttack > UTIL_WeaponTimeBase())// this prevents player from shooting while changing weapons
		return FALSE;

	return TRUE;
}




void CBasePlayerAmmo::Spawn(void) { }
void CBasePlayerAmmo::Precache(void) { }// XDM
CBaseEntity *CBasePlayerAmmo::Respawn(void) { return this; }
void CBasePlayerAmmo::Materialize(void) { }
void CBasePlayerAmmo::DefaultTouch(CBaseEntity *pOther) { }
BOOL CBasePlayerAmmo::AddAmmo(CBaseEntity *pOther) { return TRUE; }// XDM
int CBasePlayerAmmo::Restore(class CRestore &) { return 1; }// XDM
int CBasePlayerAmmo::Save(class CSave &) { return 1; }// XDM



int GetSequenceCount(void *pmodel)
{
	studiohdr_t *pstudiohdr = (studiohdr_t *)pmodel;
	if (pstudiohdr == NULL)
	{
		return 0;
	}
	return pstudiohdr->numseq;
}







/*
=====================
UTIL_TraceLine

Don't actually trace, but act like the trace didn't hit anything.
=====================
*/
void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr)
{
	memset(ptr, 0, sizeof(*ptr));
	ptr->flFraction = 1.0f;
}

void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, IGNORE_MONSTERS igmon, IGNORE_GLASS ignoreGlass, edict_t *pentIgnore, TraceResult *ptr)
{
	memset(ptr, 0, sizeof(*ptr));
	ptr->flFraction = 1.0f;
}

/*
=====================
UTIL_ParticleBox

For debugging, draw a box around a player made out of particles
=====================
*/
void UTIL_ParticleBox( CBasePlayer *player, float *mins, float *maxs, float life, unsigned char r, unsigned char g, unsigned char b )
{
	int i;
	vec3_t mmin, mmax;

	for ( i = 0; i < 3; i++ )
	{
		mmin[ i ] = player->pev->origin[ i ] + mins[ i ];
		mmax[ i ] = player->pev->origin[ i ] + maxs[ i ];
	}

	gEngfuncs.pEfxAPI->R_ParticleBox( (float *)&mmin, (float *)&mmax, r,g,b, life);
}

/*
=====================
UTIL_ParticleBoxes

For debugging, draw boxes for other collidable players
=====================
*/
void UTIL_ParticleBoxes( void )
{
	int idx;
	physent_t *pe;
	cl_entity_t *player;
	vec3_t mins, maxs;
	
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();

	player = gEngfuncs.GetLocalPlayer();
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( player->index - 1 );	

	for ( idx = 1; idx < 100; idx++ )
	{
		pe = gEngfuncs.pEventAPI->EV_GetPhysent( idx );
		if ( !pe )
			break;

		if ( pe->info >= 1 && pe->info <= gEngfuncs.GetMaxClients() )
		{
			mins = pe->origin + pe->mins;
			maxs = pe->origin + pe->maxs;

			gEngfuncs.pEfxAPI->R_ParticleBox( (float *)&mins, (float *)&maxs, 0, 0, 255, 2.0 );
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

/*
=====================
UTIL_ParticleLine

For debugging, draw a line made out of particles
=====================
*/
void UTIL_ParticleLine( CBasePlayer *player, float *start, float *end, float life, unsigned char r, unsigned char g, unsigned char b )
{
	gEngfuncs.pEfxAPI->R_ParticleLine( start, end, r, g, b, life );
}
