#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "trains.h"
#include "nodes.h"
#include "weapons.h"
#include "sound.h"
#include "soundent.h"
#include "monsters.h"
#include "shake.h"
#include "decals.h"
#include "maprules.h"
#include "gamerules.h"
#include "game.h"
#include "hltv.h"
#include "items.h"
#include "globals.h"
#include "explode.h"// XDM3034
#include "pm_shared.h"
#include "util_vector.h"
#include "msg_fx.h"
#include "projectiles.h"

int gEvilImpulse101 = 0;
bool gInitHUD = true;


// Global Savedata for player
TYPEDESCRIPTION	CBasePlayer::m_playerSaveData[] =
{
	DEFINE_FIELD( CBasePlayer, m_iFlashBattery, FIELD_INTEGER ),

	DEFINE_FIELD( CBasePlayer, m_afButtonLast, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_afButtonPressed, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_afButtonReleased, FIELD_INTEGER ),

	DEFINE_ARRAY( CBasePlayer, m_rgItems, FIELD_INTEGER, MAX_ITEMS ),
	DEFINE_FIELD( CBasePlayer, m_afPhysicsFlags, FIELD_INTEGER ),

	DEFINE_ARRAY( CBasePlayer, m_rgpWeapons, FIELD_CLASSPTR, PLAYER_INVENTORY_SIZE ),// XDM3035b: ID=32
	DEFINE_FIELD( CBasePlayer, m_pActiveItem, FIELD_CLASSPTR ),
	DEFINE_FIELD( CBasePlayer, m_pLastItem, FIELD_CLASSPTR ),
	DEFINE_FIELD( CBasePlayer, m_pNextItem, FIELD_CLASSPTR ),// XDM

	DEFINE_ARRAY( CBasePlayer, m_rgAmmo, FIELD_INTEGER, MAX_AMMO_SLOTS ),
	DEFINE_FIELD( CBasePlayer, m_idrowndmg, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_idrownrestored, FIELD_INTEGER ),

	DEFINE_FIELD( CBasePlayer, m_pTrain, FIELD_EHANDLE ),// XDM3035a
	DEFINE_FIELD( CBasePlayer, m_iTrain, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_bitsHUDDamage, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iTargetVolume, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iWeaponVolume, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iExtraSoundTypes, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iWeaponFlash, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_fLongJump, FIELD_BOOLEAN ),

	DEFINE_FIELD( CBasePlayer, m_iItemHaste, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iItemRapidFire, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iItemQuadDamage, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iItemInvisibility, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iItemInvulnerability, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iOldShieldAmount, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iItemHealthAug, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iItemShieldAug, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iItemShieldStrengthAug, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iItemWeaponPowerAug, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iItemShieldRegen, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iItemBanana, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iItemAccuracy, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iItemLightningField, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iItemRadShield, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iItemPlasmaShield, FIELD_INTEGER ),

	DEFINE_FIELD( CBasePlayer, m_iGeneratorPower, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iPowerUseShield, FIELD_INTEGER ),

	DEFINE_FIELD( CBasePlayer, m_iItemFireSupressor, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iItemAntidote, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iShield, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iGenerator, FIELD_INTEGER ),	
	DEFINE_FIELD( CBasePlayer, m_iWpnDestroyed, FIELD_BOOLEAN ),	
	
	DEFINE_FIELD( CBasePlayer, m_fInitHUD, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_tbdPrev, FIELD_TIME ),

	DEFINE_FIELD( CBasePlayer, m_pTank, FIELD_EHANDLE ),
	DEFINE_FIELD( CBasePlayer, m_iHideHUD, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iFOV, FIELD_INTEGER ),

	DEFINE_FIELD(CBasePlayer, m_flIgnoreLadderStopTime, FIELD_TIME),
	DEFINE_ARRAY( CBasePlayer, m_szAnimExtention, FIELD_CHARACTER, 32),
	DEFINE_FIELD(CBasePlayer, m_flNextDecalTime, FIELD_TIME),
};

LINK_ENTITY_TO_CLASS(player, CBasePlayer);


//-----------------------------------------------------------------------------
// Purpose: ID's player as such.
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayer::Classify(void)
{
	return m_iClass?m_iClass:CLASS_PLAYER;// XDM3037
}

//-----------------------------------------------------------------------------
// Purpose: Spawn. Called by ClientPutInServer() in mp.
// Input  : restore - true if restoring from a saved game (FCAP_MUST_SPAWN only)
//-----------------------------------------------------------------------------
void CBasePlayer::Spawn(byte restore)
{
	if (restore == FALSE)
	{
		int i = 0;
		for (i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
			m_rgpWeapons[i] = NULL;

		for (i = 0; i < MAX_ITEMS; ++i)// XDM3034
			m_rgItems[i] = 0;

		m_rgItems[ITEM_TYPE_AIRSTRIKE] = 0;
		m_rgItems[ITEM_TYPE_SATELLITE_STRIKE] = 0;
		m_rgItems[ITEM_TYPE_ANTIDOTE] = 1;
		m_rgItems[ITEM_TYPE_TRIPMINE] = 1;
		m_rgItems[ITEM_TYPE_SPIDERMINE] = 0;
		m_rgItems[ITEM_TYPE_FLASHBANG] = 2;
		m_rgItems[ITEM_TYPE_ENERGY_CUBE] = 25;
		m_rgItems[ITEM_TYPE_FIRE_SUPRESSOR] = 1;

		m_fFrozen = FALSE;// XDM3035
		m_bDisplayTitle = FALSE;
		m_voicePitch = PITCH_NORM;

		UTIL_SetView(edict(), edict());// XDM3035b: restore after possible SET_VIEW (like trigger_camera on previous level)
	}
	Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Spawn
//-----------------------------------------------------------------------------
void CBasePlayer::Spawn(void)
{
	pev->classname		= MAKE_STRING("player");
	pev->targetname		= pev->classname;// XDM3037: TESTME: is this ok?
	pev->takedamage		= DAMAGE_AIM;
	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_WALK;
	pev->flags			|= FL_CLIENT;
	pev->air_finished	= gpGlobals->time + PLAYER_AIR_TIME;
	pev->dmg			= 2;// initial water damage
	pev->deadflag		= DEAD_NO;
	pev->dmg_take		= 0;
	pev->dmg_save		= 0;

	m_fHitFxTime  = gpGlobals->time + 1.0;
	char *infobuffer = GET_INFO_KEY_BUFFER(edict());
	if (pev->skin == 0)
		pev->skin = atoi(GET_INFO_KEY_VALUE(infobuffer, "skin"));// XDM

	pev->gravity		= g_pWorld->pev->gravity;//1.0;
	pev->friction		= g_pWorld->pev->friction;//1.0; // XDM3035c: use map global multipliers

	pev->rendermode		= kRenderNormal;// XDM3035
	pev->renderfx		= kRenderFxNone;// XDM
	pev->rendercolor	= g_vecZero;// XDM
	pev->renderamt		= 0;// XDM3035

	pev->owner			= NULL;// XDM3035
	pev->maxspeed		= g_psv_maxspeed->value;// XDM3035c
	m_bitsHUDDamage		= -1;
	m_bitsDamageType	= 0;
	m_afPhysicsFlags	= 0;

	m_fBlindAmount		= 0;
	m_iItemHaste		= 0;
	m_iItemRapidFire	= 0;
	m_iItemQuadDamage	= 0;
	m_iItemInvisibility = 0;
	m_iItemInvulnerability = 0;
	m_iOldShieldAmount	= 0;
	m_iItemShieldRegen	= 0;
	m_iItemBanana		= 0;
	m_iItemAccuracy		= 0;
	m_iItemAntidote		= 0;
	m_iItemLightningField	= 0;
	m_iItemRadShield		= 0;
	m_iItemFireSupressor	= 0;
	m_iItemPlasmaShield		= 0;

	m_iTimePlasmaShieldUpdate = m_iTimeRadShieldUpdate = m_fTimeLightningFieldUpdate = m_fTimeAntidoteUpdate = m_fTimeFireSupressorUpdate = m_fTimeBananaUpdate = m_fTimeHasteUpdate = m_fTimeRapidUpdate = m_fTimeInvisUpdate	= m_fTimeQuadUpdate = m_flShieldRegenUpdate =	m_fTimeInvulUpdate = m_fTimeAccuracyUpdate = gpGlobals->time + 0.1;

	m_iShield			= TRUE;
	m_iGenerator		= TRUE;

	if (IsBot() && bot_random_powerup_lvl.value > 0.0f)
	{
		m_iItemWeaponPowerAug	= RANDOM_LONG(0, MAX_WEAPON_POWER_AUG);
		m_fLongJump				= RANDOM_LONG(0, 1);
		m_iItemHealthAug		= RANDOM_LONG(0, MAX_HEALTH_AUG);
		m_iItemShieldAug		= RANDOM_LONG(0, MAX_SHIELD_AUG);
		m_iItemShieldStrengthAug	= RANDOM_LONG(0, MAX_SHIELD_STRENGTH_AUG);
		pev->health			= MAX_PLAYER_HEALTH + (m_iItemHealthAug*EXTRA_HEALTH_HP_AUG);
		pev->max_health		= pev->health;
		pev->armorvalue		= MAX_NORMAL_BATTERY + (m_iItemShieldAug*EXTRA_BATTERY_SHIELD_AUG);
	}
	else
	{
		pev->health			= MAX_PLAYER_HEALTH;
		pev->max_health		= pev->health;
		pev->armorvalue		= MAX_NORMAL_BATTERY/2;
		m_iItemHealthAug		= 0;
		m_iItemShieldAug		= 0;
		m_fLongJump				= 0;
		m_iItemWeaponPowerAug	= 0;
		m_iItemShieldStrengthAug	= 0;
	}

	m_iGeneratorPower	= GENERATOR_PROVIDE_POWER;
	m_iPowerUseShield	= m_iGeneratorPower*2;

	m_iWpnDestroyed		= FALSE;
	m_fBurnFXTime = m_fFrozenFXTime = gpGlobals->time + 0.1;

	m_iKillGunType = TYRIANGUN_NONE;

	ENGINE_SETPHYSKV(edict(), PHYSKEY_DEFAULT, "1");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_LONGJUMP, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_IGNORELADDER, "0");// XDM3037: ignore ladder: off
	ENGINE_SETPHYSKV(edict(), PHYSKEY_HASTE, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_INVISIBILITY, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_INVULNERABILITY, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_QUADDAMAGE, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_ACCURACY, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_BANANA, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_ANTIDOTE, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_RAPIDFIRE, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_LIGHTNING_FIELD, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_SHIELD_REGEN, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_RADSHIELD, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_FIRE_SUPRESSOR, "0");
	ENGINE_SETPHYSKV(edict(), PHYSKEY_PLASMA_SHIELD, "0");

	pev->fov = m_iFOV	= 0.0f;// init field of view.
	m_iClientFOV		= -1; // make sure fov reset is sent
	m_flNextDecalTime	= 0;// let this player decal as soon as he spawns.
	m_flgeigerDelay = gpGlobals->time + 2.0; // wait a few seconds until user-defined message registrations

//	m_flTimeStepSound	= 0;
//	m_iStepLeft			= 0;
	m_flFieldOfView		= 0.5;// some monsters use this to determine whether or not the player is looking at them.
	m_bloodColor		= DONT_BLEED;
	m_afCapability		= bits_CAP_DUCK | bits_CAP_JUMP | bits_CAP_STRAFE | bits_CAP_SWIM | bits_CAP_CLIMB | bits_CAP_USE | bits_CAP_HEAR | bits_CAP_AUTO_DOORS | bits_CAP_OPEN_DOORS | bits_CAP_TURN_HEAD;
	m_iGibCount			= 16;// XDM
	m_flNextAttack		= UTIL_WeaponTimeBase();
//	StartSneaking();
	m_iFlashBattery		= POWER_MAX_CHARGE;

	// dont let uninitialized value here hurt the player
	m_flFallVelocity = 0;

	m_fDeadTime			= 0.0f;// XDM3035
	m_flIgnoreLadderStopTime = 0.0f;// XDM3037
	m_flLastSpawnTime	= gpGlobals->time;
//	m_fEnableTakeDamageTime = gpGlobals->time;// XDM3035a: spawn protection
	m_bReadyPressed		= false;// XDM3036

    SET_MODEL(ENT(pev), "models/player.mdl");
    g_ulModelIndexPlayer	= pev->modelindex;
	pev->sequence			= LookupActivity(ACT_IDLE);

	if (FBitSet(pev->flags, FL_DUCKING))
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

    pev->view_ofs = VEC_VIEW_OFFSET;
	Precache();
	m_HackedGunPos = Vector(0,32,0);

	m_fNoPlayerSound	= FALSE;// normal sound behavior.
	m_pLastItem			= NULL;
	m_fInitHUD			= TRUE;
	m_iClientHideHUD	= -1;  // force this to be recalculated
	m_fWeapon			= FALSE;
	m_pClientActiveItem	= NULL;
	m_iClientBattery	= -1;
	m_iClientWpnIcon	= -1;
	m_pActiveItem		= NULL;// XDM: TESTED
	m_pTank				= NULL;
//	m_pTrain			= NULL;// XDM3035
	m_pCarryingObject	= NULL;// XDM3033 TESTED
	m_iScoreCombo		= 0;// XDM3035
	m_iLastScoreAward	= 0;
	m_fNextScoreTime	= 0.0f;

	// reset all ammo values to 0
	for (int i = 0; i < MAX_AMMO_SLOTS; ++i)
	{
		m_rgAmmo[i] = 0;
		m_rgAmmoLast[i] = 0;  // client ammo values also have to be reset (the death hud clear messages does on the client side)
	}

	m_vecAutoAimPrev = g_vecZero;
	m_flNextChatTime = gpGlobals->time;
	g_pGameRules->PlayerSpawn(this);
	pev->oldorigin = pev->origin;// XDM3035: possibly reduces interpolation during respawn
	pev->effects		= 0;// stay invisible during respawn

	m_iSequenceDeepIdle = LookupSequence("deep_idle");// XDM3035b: fasterizer
	m_iSpawnState = 1;

	if (g_pGameRules->IsMultiplayer())
		UTIL_ScreenFade(this, Vector(0,0,255), 1.0, 0.2, 255, FFADE_IN);
}

//-----------------------------------------------------------------------------
// Purpose: Precache
//-----------------------------------------------------------------------------
void CBasePlayer::Precache(void)
{
// XDM: moved to CWorld
	// SOUNDS / MODELS ARE PRECACHED in ClientPrecache() (game specific)
	// because they need to precache before any clients have connected
	// init geiger counter vars during spawn and each time
	// we cross a level transition
	m_flgeigerRange = 1000;
	m_igeigerRangePrev = 1000;
	m_bitsDamageType = 0;
	m_bitsHUDDamage = -1;
	m_iClientBattery = -1;
	m_iClientWpnIcon	= -1;
	m_iTrain = TRAIN_NEW;

//	m_iUpdateTime = 5;  // won't update for 1/2 a second

	if (gInitHUD)
		m_fInitHUD = TRUE;

	if (IsOnTrain())// XDM3035c: restore
		m_iTrain |= TRAIN_NEW;

/*	else
		ALERT(at_console, "gInitHUD == FALSE\n");*/
}

//-----------------------------------------------------------------------------
// Purpose: Pushed by something
// Input  : *pOther - 
//-----------------------------------------------------------------------------
void CBasePlayer::Touch(CBaseEntity *pOther)
{
	if (FBitSet(pOther->pev->flags, FL_ONGROUND) && pOther->pev->groundentity && pOther->pev->groundentity == edict() && pev->waterlevel <= 0)
		return;

	if (pOther->IsPlayer() && pOther->IsAlive() && !FBitSet(pev->flags, FL_FROZEN))// XDM3035c: allow players to push each other
	{
		pev->velocity += pOther->pev->velocity*0.75f;// Not scientific, but works fine with HL
		pOther->pev->velocity *= 0.5f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Save
// Input  : &save - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayer::Save(CSave &save)
{
	if (!CBaseMonster::Save(save))
		return 0;

	return save.WriteFields("PLAYER", this, m_playerSaveData, ARRAYSIZE(m_playerSaveData));
}

//-----------------------------------------------------------------------------
// Purpose: Restore: special process for player // Called before Spawn(TRUE)
// Input  : &restore - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayer::Restore(CRestore &restore)
{
	if (!CBaseMonster::Restore(restore))
		return 0;

	int status = restore.ReadFields("PLAYER", this, m_playerSaveData, ARRAYSIZE(m_playerSaveData));

	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;
	// landmark isn't present.
	if (!pSaveData->fUseLandmark)
	{
		ALERT(at_console, "CBasePlayer::Restore(): No Landmark: %s\n", pSaveData->szLandmarkName);
		// default to normal spawn
		CBaseEntity *pSpawnSpot = SpawnPointEntSelect(this);
		pev->origin = pSpawnSpot->pev->origin + Vector(0,0,1);
		pev->angles = pSpawnSpot->pev->angles;
	}
	pev->v_angle.z = 0;	// Clear out roll
	pev->angles = pev->v_angle;
	pev->fixangle = TRUE;           // turn this way immediately

// still no help	m_fInitHUD = TRUE;// XDM3035b: CL_Precache needs this

// Copied from spawn() for now
//?	m_bloodColor	= BLOOD_COLOR_RED;
    g_ulModelIndexPlayer = pev->modelindex;

	if (FBitSet(pev->flags, FL_DUCKING))
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

	ENGINE_SETPHYSKV(edict(), PHYSKEY_DEFAULT, "1");

	if (m_fLongJump)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_LONGJUMP, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_LONGJUMP, "0");

	if (m_iItemHaste)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_HASTE, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_HASTE, "0");

	if (m_iItemInvisibility)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_INVISIBILITY, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_INVISIBILITY, "0");

	if (m_iItemInvulnerability)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_INVULNERABILITY, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_INVULNERABILITY, "0");

	if (m_iItemQuadDamage)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_QUADDAMAGE, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_QUADDAMAGE, "0");

	if (m_iItemAntidote)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_ANTIDOTE, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_ANTIDOTE, "0");

	if (m_iItemFireSupressor)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_FIRE_SUPRESSOR, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_FIRE_SUPRESSOR, "0");

	if (m_iItemBanana)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_BANANA, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_BANANA, "0");

	if (m_iItemRapidFire)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_RAPIDFIRE, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_RAPIDFIRE, "0");

	if (m_iItemLightningField)
	{
		ENGINE_SETPHYSKV(edict(), PHYSKEY_LIGHTNING_FIELD, "1");
		CLightningField::CreateNew(this, pev->origin, gSkillData.DmgPlrLightningField + (gSkillData.DmgPlrLightningField * (m_iItemWeaponPowerAug*0.1)), m_iItemLightningField);
	}
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_LIGHTNING_FIELD, "0");

	if (m_iItemRadShield)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_RADSHIELD, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_RADSHIELD, "0");

	if (m_iItemPlasmaShield)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_PLASMA_SHIELD, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_PLASMA_SHIELD, "0");

	if (m_iItemAccuracy)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_ACCURACY, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_ACCURACY, "0");

	if (m_iItemShieldRegen)
		ENGINE_SETPHYSKV(edict(), PHYSKEY_SHIELD_REGEN, "1");
	else
		ENGINE_SETPHYSKV(edict(), PHYSKEY_SHIELD_REGEN, "0");


//	SendWeaponsUpdate();// XDM: ?

#if defined( CLIENT_WEAPONS )
	// HACK:	This variable is saved/restored in CBaseMonster as a time variable, but we're using it
	//			as just a counter.  Ideally, this needs its own variable that's saved as a plain float.
	//			Barring that, we clear it out here instead of using the incorrect restored time value.
	m_flNextAttack = UTIL_WeaponTimeBase();
#endif

//	if (IsOnTrain())// XDM3035c: doesn't help - too early
//		m_iTrain |= TRAIN_NEW;

	return status;
}

//-----------------------------------------------------------------------------
// Purpose: players can 'use' each other. Works for bots and gets attention of players!
// Input  : *pActivator - 
//			*pCaller - 
//			useType - 
//			value - 
//-----------------------------------------------------------------------------
void CBasePlayer::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (pActivator->IsPlayer() && pActivator->pev != pev)// using self is really possible
	{
		if (IsBot())// Don't mess with real players entvars
		{
			pev->owner = pActivator->edict();// XBM: bot will react to this change
		}
		else
		{
			// Show direction from which the player was called
			MESSAGE_BEGIN(MSG_ONE, gmsgDamage, NULL, ENT(pev));
//				WRITE_BYTE(0);
//				WRITE_BYTE(1);
				WRITE_LONG(0);
				WRITE_COORD(pActivator->pev->origin.x);
				WRITE_COORD(pActivator->pev->origin.y);
				WRITE_COORD(pActivator->pev->origin.z);
			MESSAGE_END();
			ClientPrint(pev, HUD_PRINTCENTER, "%s\n", STRING(pActivator->pev->netname));// Print caller's name (or use some localized string maybe)
		}
	}
}

void CBasePlayer::DeathSound( void )
{
	switch (RANDOM_LONG(0,3))
	{
		case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/death1.wav", 1, ATTN_NORM); break;
		case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/death2.wav", 1, ATTN_NORM); break;
		case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/death3.wav", 1, ATTN_NORM); break;
		case 3:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/death4.wav", 1, ATTN_NORM); break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: GetGunPosition
// Output : Vector
//-----------------------------------------------------------------------------
Vector CBasePlayer::GetGunPosition(void)
{
//	Vector org, ang;
//	GetAttachment(0, org, ang);
//	UTIL_MakeVectors(pev->v_angle);
//	m_HackedGunPos = pev->view_ofs;
	return pev->origin + pev->view_ofs;
}

//-----------------------------------------------------------------------------
// Purpose: TraceAttack
// Input  : *pAttacker - 
//			flDamage - 
//			&vecDir - 
//			*ptr - 
//			bitsDamageType - 
//-----------------------------------------------------------------------------
void CBasePlayer::TraceAttack(CBaseEntity *pAttacker, float flDamage, const Vector &vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (pev->takedamage)
	{
		m_LastHitGroup = ptr->iHitgroup;
		if (pev->armorvalue)
			flDamage *= gSkillData.plrShield;
		else
		switch (ptr->iHitgroup)
		{
		case HITGROUP_HEAD:
			{
				flDamage *= gSkillData.plrHead;
				if (!m_fFrozen && !m_fBlindAmount)// XDM: undone: move to client
					UTIL_ScreenFade(this, Vector(255,20,0), 1.0f, 0.1f, clamp((int)flDamage, 10, 255), FFADE_IN);
			}
			break;
		case HITGROUP_CHEST:
			{
				flDamage *= gSkillData.plrChest;
			}
			break;
		case HITGROUP_STOMACH:
			{
				flDamage *= gSkillData.plrStomach;

				if (!m_iItemInvulnerability)
					pev->punchangle.x = 2.0f;// XDM
			}
			break;
		case HITGROUP_LEFTARM:// flDamage *= gSkillData.plrArm; break;
		case HITGROUP_RIGHTARM:
			{
				flDamage *= gSkillData.plrArm;

				if (!m_iItemInvulnerability)
					pev->punchangle.y = clamp(flDamage*0.5f, 0.0f, 10.0f);

				if (ptr->iHitgroup == HITGROUP_RIGHTARM && !m_iItemInvulnerability)
					pev->punchangle.y *= -0.5f;
			}
			break;
		case HITGROUP_RIGHTLEG:
		case HITGROUP_LEFTLEG:
			{
				flDamage *= gSkillData.plrLeg;

				if (!m_iItemInvulnerability)
				{
					pev->punchangle.x = 2.0f;// XDM
					pev->punchangle.z = clamp(flDamage*0.5f, 0.0f, 10.0f);
				}
				if (ptr->iHitgroup == HITGROUP_LEFTLEG && !m_iItemInvulnerability)
					pev->punchangle.z *= -0.5f;
			}
			break;
		case HITGROUP_ARMOR:
			{
				flDamage *= ARMOR_BONUS;// XDM3035
			}
			break;
		}

		if (ptr->iHitgroup == HITGROUP_ARMOR)// XDM3035
		{
			if (g_pGameRules->FAllowEffects())
				UTIL_Ricochet(ptr->vecEndPos, flDamage);
		}
		else if (BloodColor() != DONT_BLEED && !(bitsDamageType & DMG_DONT_BLEED))// XDM
		{
			UTIL_BloodDrips(ptr->vecEndPos, -vecDir, BloodColor(), (int)flDamage);// a little surface blood.
			TraceBleed(flDamage, vecDir, ptr, bitsDamageType);
		}
		AddMultiDamage(pAttacker, this, flDamage, bitsDamageType);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
/*
	Take some damage.
	NOTE: each call to TakeDamage with bitsDamageType set to a time-based damage
	type will cause the damage time countdown to be reset.  Thus the ongoing effects of poison, radiation
	etc are implemented with subsequent calls to TakeDamage using DMG_GENERIC.
*/
// Input  : *pInflictor - 
//			*pAttacker - 
//			flDamage - 
//			bitsDamageType - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayer::TakeDamage(CBaseEntity *pInflictor, CBaseEntity *pAttacker, float flDamage, int bitsDamageType)
{
	if (pev->movetype == MOVETYPE_NOCLIP && pev->solid == SOLID_NOT)// XDM3035: disintegration
		return 0;

	if (pev->effects & EF_NODRAW)// XDM3035: gibbed or respawning or smth
		return 0;

	// Already dead
	if (!IsAlive())
	{
		return DeadTakeDamage(pInflictor, pAttacker, flDamage, bitsDamageType);// XDM3035
	}

	if (pAttacker==this && bitsDamageType & DMG_NOSELF)
		return 0;

	if (bitsDamageType & DMG_NOSELF && g_pGameRules->PlayerRelationship(this, pInflictor) == GR_TEAMMATE)
		return 0;

	// go take the damage first
	if (!g_pGameRules->FPlayerCanTakeDamage(this, pAttacker))
		return 0;// Refuse the damage

	if (bitsDamageType & DMG_BLIND)
	{
		Vector ToFlash = UTIL_VecToAngles (pInflictor->pev->origin - pev->origin);
		int alpha = (cos(UTIL_AngleDistance( ToFlash.y, pev->v_angle.y ) * 3.1415926 / 180) + 1) * 255 / 2;

		if (alpha > m_fBlindAmount)
		{
			UTIL_ScreenFade( this, Vector(255,255,255), flDamage/2, flDamage/20, (alpha>215)?255:alpha, FFADE_IN);
			m_fBlindAmount = alpha;
		}
		flDamage = 0;
	}

	//recharge armor and battery
	if (m_iItemRadShield && (bitsDamageType & (DMG_RADIATION)))
	{
		float MaxShield = (m_iItemInvulnerability?MAX_INVULNER_BATTERY:MAX_NORMAL_BATTERY) + (m_iItemShieldAug*EXTRA_BATTERY_SHIELD_AUG);

		if (pev->armorvalue < MaxShield)
		{
			pev->armorvalue += flDamage*0.5;

			if (pev->armorvalue > MaxShield)
				pev->armorvalue = MaxShield;
		}
		if (m_iFlashBattery < (POWER_MAX_CHARGE-1))
		{
			m_iFlashBattery += flDamage*0.5;

			if (m_iFlashBattery > (POWER_MAX_CHARGE-1))
				m_iFlashBattery = (POWER_MAX_CHARGE-1);
		}
		return 0;
	}

	// check for quad damage powerup on the attacker
	if (pAttacker->IsPlayer())
	{
		if ( ((CBasePlayer*)pAttacker)->m_iItemQuadDamage)
		{
			flDamage *= 4;
		}
	}

	if (m_fFrozen)
		flDamage = flDamage*2;

	if (!pev->armorvalue && m_iItemAntidote && (bitsDamageType & (DMG_PARALYZE | DMG_POISON |DMG_NERVEGAS | DMG_ACID)))
		flDamage = flDamage*0.25;

	if (!pev->armorvalue && m_iItemFireSupressor && (bitsDamageType & (DMG_BURN | DMG_SLOWBURN)))
		flDamage = flDamage*0.5;

	// have suit diagnose the problem - ie: report damage type
	int bitsDamage = bitsDamageType;
	int fTookDamage;
	float flRatio = ARMOR_RATIO;
	float flBonus = ARMOR_BONUS - (m_iItemShieldStrengthAug*0.075);

	// keep track of amount of damage last sustained
	m_lastDamageAmount = (int)flDamage;

	// Armor.
	if (pev->armorvalue > 0 && !(bitsDamageType & (DMG_NERVEGAS | DMG_DROWN | DMG_IGNOREARMOR)))// armor doesn't protect against fall or drown damage!
	{
		float flNew = flDamage * flRatio;
		float flArmor = (flDamage - flNew) * flBonus;

		// Does this use more armor than we have?
		if (flArmor > pev->armorvalue)
		{
			flArmor = pev->armorvalue;
			flArmor *= (1/flBonus);
			flNew = flDamage - flArmor;
			pev->armorvalue = 0;
		}
		else
			pev->armorvalue -= flArmor;

		flDamage = flNew;
	}

	if (pev->armorvalue > 0 && !m_iItemPlasmaShield && !(bitsDamageType & (DMG_PARALYZE | DMG_BULLET | DMG_IGNOREARMOR | DMG_NEVERGIB)))
	{
		if(gpGlobals->time >= m_fShieldFxTime)
		{
			if (m_iItemInvulnerability)
				FX_Trail(pev->origin, entindex(), FX_SHIELDEFFECT_INVUL);
			else 
				FX_Trail(pev->origin, entindex(), FX_SHIELDEFFECT);

			m_fShieldFxTime = gpGlobals->time + 0.1f;
		}
	}
// plasma shield uses kinetic energy of impacting projectiles to restore itself and generate plasma projectiles to harm an attacker
	if (m_iShield && m_iItemPlasmaShield && !(bitsDamageType & DMG_IGNOREARMOR))
	{
		if(gpGlobals->time >= m_fPlasmaShieldTime)
		{
			if (pAttacker && pAttacker != this && (pAttacker->IsPlayer() || pAttacker->IsMonster()))
			{
				float MaxShield = (m_iItemInvulnerability?MAX_INVULNER_BATTERY:MAX_NORMAL_BATTERY) + (m_iItemShieldAug*EXTRA_BATTERY_SHIELD_AUG);

				if (m_lastDamageAmount > 200)
					m_lastDamageAmount = 200;

				if (pev->armorvalue < MaxShield)
				{
					pev->armorvalue += m_lastDamageAmount*0.75;

					if (pev->armorvalue > MaxShield)
						pev->armorvalue = MaxShield;
				}
				Vector vecAng;
				Vector vecDir(pAttacker->BodyTarget(pev->origin));
				vecDir -= pev->origin; vecDir.NormalizeSelf();
				Vector vecStart(pev->origin); vecStart += vecDir * HULL_RADIUS;
				VectorAngles(vecDir, vecAng);
				CPlasmaShieldCharge::CreateNew(vecStart, vecAng, vecDir, this, m_lastDamageAmount*2.5, (m_iItemAccuracy)?(FIRESPREAD_PLASMA_SHIELD_CHARGE*ACCURACY_MODYFIER):FIRESPREAD_PLASMA_SHIELD_CHARGE);
			}
		}
		FX_Trail(pev->origin, entindex(), FX_SHIELDEFFECT_PLASMA);
		m_fPlasmaShieldTime = gpGlobals->time + 0.075f;
	}

	//destroy shield emitter, if health is too low
	if (!m_iItemInvulnerability && !m_iItemPlasmaShield && !m_iItemBanana && m_iShield && pev->health <= MAX_PLAYER_HEALTH*0.3)
	{
		m_iShield = FALSE;
		m_iItemShieldAug = 0;
		m_iItemShieldStrengthAug = 0;
		FX_Trail(pev->origin, entindex(), FX_PLAYER_SHIELD_BLAST);
		flDamage *= 0.25;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE( MSG_ARMOR );
			WRITE_SHORT(SHIELD_IS_DEAD);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_SHIELD_STRENGTH);
			WRITE_SHORT(m_iItemShieldStrengthAug);
			MESSAGE_END();	
		}
	}
	//destroy power generator, if health is too low
	if (!m_iItemInvulnerability && m_iItemPlasmaShield && !m_iItemBanana && m_iGenerator && pev->health <= MAX_PLAYER_HEALTH*0.2)
	{
		m_rgAmmo[GetAmmoIndexFromRegistry("generator_power")] = GENERATOR_IS_DEAD;//remove all generator augmentations

		m_iGenerator = FALSE;
		FX_Trail(pev->origin, entindex(), FX_PLAYER_GENERATOR_BLAST);
		flDamage *= 0.25;
	}

	// this cast to INT is critical!!! If a player ends up with 0.5 health, the engine will get that
	// as an int (zero) and think the player is dead! (this will incite a clientside screentilt, etc)
	fTookDamage = CBaseMonster::TakeDamage(pInflictor, pAttacker, (int)flDamage, bitsDamageType);

	// reset damage time countdown for each type of time based damage player just sustained
	{
		for (int i = 0; i < CDMG_TIMEBASED; ++i)
			if (bitsDamageType & (DMG_PARALYZE << i))
				m_rgbTimeBasedDamage[i] = 0;
	}

	// tell director about it
	if (pInflictor)
	{
		if (g_pGameRules->IsMultiplayer())// XDM3035a: causes buffer overflow in SP
		{
		MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
			WRITE_BYTE(9);	// command length in bytes
			WRITE_BYTE(DRC_CMD_EVENT);	// take damage event
			WRITE_SHORT(entindex());	// index number of primary entity
			WRITE_SHORT(pInflictor->entindex());	// index number of secondary entity
			WRITE_LONG(5);   // eventflags (priority and flags)
		MESSAGE_END();
		}
	}
	else
	{
		pInflictor = NULL;
	}

	m_bitsDamageType |= bitsDamage; // Save this so we can report it to the client
	m_bitsHUDDamage = -1;  // make sure the damage bits get resent

	if (m_fFrozen || (bitsDamageType & (DMG_CRUSH | DMG_FREEZE | DMG_BLAST | DMG_ENERGYBLAST | DMG_PARALYZE)) && (flDamage > 25) && IsOnLadder())// && fTookDamage)// XDM3037: make the player fall from ladder
		DisableLadder(1.0f);

	if (!m_iItemInvulnerability)
	{
		if (pev->punchangle.x > -2.0f)
			pev->punchangle.x -= 1.5f;// XDM3035
	}

	return fTookDamage;
}

//-----------------------------------------------------------------------------
// Purpose: Killed
// Input  : *pInflictor - 
//			*pAttacker - 
//			iGib - 
//-----------------------------------------------------------------------------
void CBasePlayer::Killed(CBaseEntity *pInflictor, CBaseEntity *pAttacker, int iGib)
{
	if (pev->deadflag == DEAD_NO)// XDM3035: DeadTakeDamage(): player may get 'killed' second time like dead monsters
	{
		pev->deadflag = DEAD_DYING;// set here so weapons may detect it

		// Holster weapon immediately, to allow it to cleanup
		if (m_pActiveItem)// && m_pActiveItem->GetOwner())// XDM3035a: somehow glock with m_pPlayer == NULL got here
		{
			m_pActiveItem->Holster();
			m_pActiveItem->SetThinkNull();// XDM3035b
			m_pActiveItem->pev->nextthink = 0;
			m_pLastItem = NULL;// XDM: clear if no active item
		}
		m_pNextItem = NULL;// XDM

		// MUST be called BEORE removing any items!
		g_pGameRules->PlayerKilled(this, pAttacker, pInflictor);

		if (m_pTank != NULL)
		{
			m_pTank->Use(this, this, USE_OFF, 0);
			m_pTank = NULL;
		}
	//	if (IsOnTrain())
		{
	//		m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
			TrainDetach();
		}

/* ORLY?		if (m_fFrozen)
		{
			FrozenEnd();
			m_fFrozen = TRUE;// XDM3035: leave this for gibbage
		}*/

		// this client isn't going to be thinking for a while, so reset the sound until they respawn
		CSound *pSound = CSoundEnt::SoundPointerForIndex(CSoundEnt::ClientSoundIndex(edict()));
		{
			if (pSound)
				pSound->Reset();
		}


		pev->modelindex = g_ulModelIndexPlayer;    // don't use eyes
		pev->movetype = MOVETYPE_TOSS;
		ClearBits(pev->flags, FL_ONGROUND);
		if (pev->velocity.z < 10)
			pev->velocity.z += RANDOM_FLOAT(0,300);

		
		// send "health" update message to zero
		m_iClientHealth = 0;
		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
		WRITE_BYTE(MSG_HEALTH);
		WRITE_SHORT(m_iClientHealth);
		MESSAGE_END();

		// Tell Ammo Hud that the player is dead
		MESSAGE_BEGIN(MSG_ONE, gmsgCurWeapon, NULL, ENT(pev));
			WRITE_BYTE(0);
			WRITE_BYTE(0XFF);
			WRITE_BYTE(0xFF);
		MESSAGE_END();
		// reset FOV
		
		pev->fov = m_iFOV = m_iClientFOV = 0.0f;
		MESSAGE_BEGIN(MSG_ONE, gmsgSetFOV, NULL, ENT(pev));
			WRITE_BYTE(0);
		MESSAGE_END();

		EnableControl(true);// XDM3037: FIX: players died with controls disabled can not respawn!
	}//pev->deadflag == DEAD_NO

	if (pev->effects & EF_NODRAW)// XDM3035: gibbed or respawning or smth (should never get here)
		return;

	// *** ONLY EFFECTS AFTER THIS LINE! No logic and deadflags! ***

	if (g_pGameRules->FAllowMonsters())
		CSoundEnt::InsertSound(bits_SOUND_DEATH, pev->origin, 256, 1.0f);// XDM3035c

	if (iGib == GIB_DISINTEGRATE)
	{
		if (g_pGameRules->FAllowEffects())// Disintegration effect allowed?
		{
			ParticleBurst(pev->origin, 20, 5, 10);// Center() is not nescessary for players
			UTIL_ScreenShakeOne(this, pev->origin, 10.0f, 0.5f, 3.0f);
		}

		UTIL_ScreenFade(this, g_vecZero, 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);

		pev->movetype = MOVETYPE_NOCLIP;
		pev->flags |= FL_FLY;
		pev->velocity = Vector(0.0f,0.0f,4.0f);
		pev->origin.z += 0.5f;// HACK
		pev->avelocity = UTIL_RandomVector()*2.0f;//Vector(RANDOM_FLOAT(-2,2)
		pev->effects |= EF_MUZZLEFLASH;
		pev->rendermode = kRenderTransTexture;
		pev->renderfx	= kRenderFxNone;
		pev->rendercolor = Vector(127,127,127);
		pev->renderamt = 160;
		pev->framerate *= 0.25f;
		pev->gravity = 0.0f;
		pev->takedamage = DAMAGE_NO;

		// DON'T Disintegrate(); players!!
	}
	if (iGib == GIB_RADIATION)
	{
		UTIL_ScreenFade(this, Vector(128,128,128), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_GIB_RADIATION);

		pev->velocity = g_vecZero;
		pev->effects |= EF_LIGHT;
		pev->takedamage = DAMAGE_NO;

		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = Vector(255,80,0);
		pev->renderamt = 12;
	}
	else if (iGib == GIB_FROZEN || pev->flags & FL_FROZEN)
	{
		UTIL_ScreenFade(this, Vector(0,0,128), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		pev->effects |= (EF_NODRAW|EF_NOINTERP);

		m_iKillGunType = TYRIANGUN_PLAYER_BLAST;
		::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrExplode*0.75, gSkillData.DmgPlrExplode*0.75, CLASS_NONE, DMG_SLOWFREEZE);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_GIB_FROZEN_DETONATE);
		m_iKillGunType = TYRIANGUN_NONE;
	}

	else if (iGib == GIB_REMOVE)
	{
		pev->effects |= (EF_NODRAW|EF_NOINTERP);
		UTIL_ScreenFade( this, Vector(0,0,0), 6, mp_respawntime.value + 1.0f, 255, FFADE_IN | FFADE_MODULATE );
	}
	else if (iGib == GIB_VAPOURIZE)
	{
		pev->effects |= (EF_NODRAW|EF_NOINTERP);
		FX_Trail(pev->origin, entindex(), FX_PLAYER_GIB_VAPOURIZE);
		UTIL_ScreenFade(this, Vector(128,128,128), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
	}
	else if (iGib == GIB_ELECTRO)
	{
		pev->effects |= (EF_NODRAW|EF_NOINTERP);

		pev->angles.x = pev->angles.x * 0;
		pev->angles.y = pev->angles.y;
		pev->angles.z = pev->angles.z * 0;

		FX_FireBeam(pev->origin, pev->angles, pev->v_angle, CHAR_TEX_CONCRETE, FX_PLAYER_GIB_ELECTROCUTED, FALSE);
		UTIL_ScreenFade(this, Vector(0,192,255), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		CLightningField::CreateLgtngGib(this, pev->origin, RANDOM_FLOAT(1,2));
	}
	else if (iGib == GIB_MELT)
	{
		pev->effects |= (EF_NODRAW|EF_NOINTERP);
		UTIL_ScreenFade(this, Vector(255,100,0), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		BOOL bInWater = UTIL_LiquidContents(pev->origin);

		if (!bInWater)
		{
			::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrExplode*0.75, gSkillData.DmgPlrExplode*0.75, CLASS_NONE, DMG_BURN);
			FX_Trail(pev->origin, entindex(), FX_PLAYER_GIB_MELT);
		}
		else
			FX_Trail(pev->origin, entindex(), FX_PLAYER_GIB_NORMAL);
	}
	else if (iGib == GIB_BLAST)
	{
		UTIL_ScreenFade(this, Vector(128,0,0), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		pev->effects |= (EF_NODRAW|EF_NOINTERP);
		m_iKillGunType = TYRIANGUN_PLAYER_BLAST;
		::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrExplode, gSkillData.DmgPlrExplode, CLASS_NONE, DMG_BLAST);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_GIB_BLAST);
		m_iKillGunType = TYRIANGUN_NONE;
	}
	else if (iGib == GIB_SONIC)
	{
		UTIL_ScreenFade(this, Vector(255,255,8), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		pev->effects |= (EF_NODRAW|EF_NOINTERP);
		m_iKillGunType = TYRIANGUN_PLAYER_BLAST;
		::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrExplode, gSkillData.DmgPlrExplode, CLASS_NONE, DMG_BLAST);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_GIB_SONIC);
		m_iKillGunType = TYRIANGUN_NONE;
	}

	else if (iGib == GIB_BURN)
	{
		UTIL_ScreenFade(this, Vector(255,0,0), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		pev->effects |= (EF_NODRAW|EF_NOINTERP);
		m_iKillGunType = TYRIANGUN_PLAYER_BLAST;
		::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrExplode, gSkillData.DmgPlrExplode, CLASS_NONE, DMG_BURN);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_GIB_BURN);
		m_iKillGunType = TYRIANGUN_NONE;
	}
	else if (iGib == GIB_ENERGYBEAM)
	{
		UTIL_ScreenFade(this, Vector(128,0,128), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		pev->effects |= (EF_NODRAW|EF_NOINTERP);

		m_iKillGunType = TYRIANGUN_PLAYER_BLAST;
		::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrExplode, gSkillData.DmgPlrExplode, CLASS_NONE, DMG_BLAST);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_GIB_ENERGYBEAM);
		m_iKillGunType = TYRIANGUN_NONE;
	}
	else if (iGib == GIB_ENERGYBLAST)
	{
		UTIL_ScreenFade(this, Vector(128,0,128), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		pev->effects |= (EF_NODRAW|EF_NOINTERP);

		m_iKillGunType = TYRIANGUN_PLAYER_BLAST;
		::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrExplode, gSkillData.DmgPlrExplode, CLASS_NONE, DMG_BLAST);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_GIB_ENERGYBLAST);
		m_iKillGunType = TYRIANGUN_NONE;
	}
	else if (iGib == GIB_ACID)
	{
		UTIL_ScreenFade(this, Vector(0,128,0), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);
		pev->effects |= (EF_NODRAW|EF_NOINTERP);

		m_iKillGunType = TYRIANGUN_PLAYER_BLAST;
		::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrExplode, gSkillData.DmgPlrExplode, CLASS_NONE, DMG_ACID);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_GIB_ACID_BLAST);
		m_iKillGunType = TYRIANGUN_NONE;
	}
	
	else 
	{
		UTIL_ScreenFade(this, Vector(128,0,0), 1.5f, mp_respawntime.value + 1.0f, 255, FFADE_OUT | FFADE_MODULATE | FFADE_STAYOUT);// XDM3035 :3
		pev->avelocity = RandomVector(RANDOM_FLOAT(-320,320), RANDOM_FLOAT(-320,320), RANDOM_FLOAT(-160,160));// XDM

		if( RANDOM_LONG(0, 32) <= 16 ) // 25% chance to throw out dead head
		{
			CGib *pHeadGib = CGib::SpawnHeadGib(this);
			if (pHeadGib)
			{
			//	UTIL_SetView(edict(), pHeadGib->edict());// - setview is a boolshit because of wall-viev
				pHeadGib->pev->owner = edict();
			}
		}
		pev->effects |= (EF_NODRAW|EF_NOINTERP);
		m_iKillGunType = TYRIANGUN_PLAYER_BLAST;
		::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrExplode, gSkillData.DmgPlrExplode, CLASS_NONE, DMG_BLAST);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_GIB_NORMAL);
		m_iKillGunType = TYRIANGUN_NONE;

//		DeathSound();
	}

	SetAnimation(PLAYER_DIE);
	m_iRespawnFrames = 0;
	pev->solid = SOLID_NOT;
	SetThink(&CBasePlayer::PlayerDeathThink);
	//drop random item from the dead player
	CBaseEntity *pEntity = NULL;
	CBaseEntity *pEntity2 = NULL;
	const char *szName = NULL;

	int iRand = RANDOM_LONG(0,0x7FFF); //75% chance
	if (iRand < (0x7fff/1.5))
	{
		switch (RANDOM_LONG(0,26))
		{
			case 0:	szName = "item_healthkit"; break;
			case 1:	szName = "item_health_aug"; break;
			case 2:	szName = "item_battery"; break;
			case 3:	szName = "item_shield_aug"; break;
			case 4:	szName = "item_shield_regeneration"; break;
			case 5:	szName = "item_antidote"; break;
			case 6:	szName = "item_airstrike"; break;
			case 7:	szName = "item_longjump"; break;
			case 8:	szName = "item_haste"; break;
			case 9:	szName = "item_rapidfire"; break;
			case 10:	szName = "item_quaddamage"; break;
			case 11:	szName = "item_invisibility"; break;
			case 12:	szName = "item_invulnerability"; break;
			case 13:	szName = "item_banana"; break;
			case 14:	szName = "item_generator_aug"; break;
			case 15:	szName = "item_accuracy"; break;
			case 16:	szName = "item_superweapon"; break;
			case 17:	szName = "item_tripmine"; break;
			case 18:	szName = "item_spidermine"; break;
			case 19:	szName = "item_shield_strength_aug"; break;
			case 20:	szName = "item_lightning_field"; break;
			case 21:	szName = "item_energy_cube"; break;
			case 22:	szName = "item_satellite_strike"; break;
			case 23:	szName = "item_flashbang"; break;
			case 24:	szName = "item_fire_supressor"; break;
			case 25:	szName = "item_plasma_shield"; break;
			case 26:	szName = "item_weapon_power_aug"; break;
		}
		pEntity = CBaseEntity::Create( MAKE_STRING(szName), pev->origin + Vector(0,0,48), g_vecZero, UTIL_RandomBloodVector()*256.0f, NULL, SF_NOTREAL|SF_NORESPAWN);
	}
	//gives an extra ammo pack for nuke
	if (mp_extra_nuke.value > 0.0f)
	{
		if (iRand < (0x7fff/1.5))
			pEntity2 = CBaseEntity::Create( MAKE_STRING("item_energy_cube"), pev->origin + Vector(0,0,32), g_vecZero, UTIL_RandomBloodVector()*300.0f, NULL, SF_NOTREAL|SF_NORESPAWN);
	}
	pev->nextthink = gpGlobals->time + 0.1f;
}

//-----------------------------------------------------------------------------
// Purpose: Set the activity based on an event or current state
// Undone : 
// Input  : playerAnim - 
//-----------------------------------------------------------------------------

void CBasePlayer::SetAnimation(PLAYER_ANIM playerAnim)
{
//FIXME: weapon and animations in single
	int animDesired;
	char szAnim[64];
	float speed = pev->velocity.Length2D();

	if (pev->flags & FL_FROZEN)
	{
		speed = 0;
		playerAnim = PLAYER_IDLE_FROZEN;
	}

	switch (playerAnim)
	{
	case PLAYER_IDLE_FROZEN:
		m_IdealActivity = ACT_SLEEP;
		break;

	case PLAYER_JUMP:
		m_IdealActivity = ACT_HOP;
		break;

	case PLAYER_SUPERJUMP:
		m_IdealActivity = ACT_LEAP;
		break;

	case PLAYER_DIE:

		if (pev->effects & EF_LIGHT)
		{
			m_IdealActivity = ACT_DIEVIOLENT;
		}
		else
		{
			m_IdealActivity = ACT_DIESIMPLE;
			m_IdealActivity = GetDeathActivity();
		}
		break;

	case PLAYER_ATTACK1:
		{
		switch( m_Activity )
		{
		case ACT_HOVER:
		case ACT_SWIM:
		case ACT_HOP:
		case ACT_LEAP:
		case ACT_DIESIMPLE:
			m_IdealActivity = m_Activity;
			break;
		default:
			m_IdealActivity = ACT_RANGE_ATTACK1;
			break;
		}
		}
		break;
	case PLAYER_IDLE:
	case PLAYER_WALK:
		{
		if (!FBitSet(pev->flags, FL_ONGROUND) && (m_Activity == ACT_HOP || m_Activity == ACT_LEAP))// Still jumping
		{
			m_IdealActivity = m_Activity;
		}
		else if (pev->waterlevel > 1)
		{
			if (speed == 0)
				m_IdealActivity = ACT_HOVER;
			else
				m_IdealActivity = ACT_SWIM;
		}
		else
		{
			m_IdealActivity = ACT_WALK;
		}
		}
		break;
	case PLAYER_ARM:
		m_IdealActivity = ACT_ARM;// XDM: draw animations
		break;
	case PLAYER_DISARM:
		m_IdealActivity = ACT_DISARM;// XDM: holster animations
		break;
	case PLAYER_RELOAD:
		m_IdealActivity = ACT_RELOAD;// XDM: reload animations
		break;
	case PLAYER_CLIMB:
		m_IdealActivity = ACT_WALK;// XDM: ladder climb animations replacement
		break;
	case PLAYER_FALL:
		m_IdealActivity = ACT_FALL;// XDM: fall animations
		break;
	}

	switch (m_IdealActivity)
	{
	case ACT_HOVER:
	case ACT_LEAP:
	case ACT_SWIM:
	case ACT_HOP:
	case ACT_DIESIMPLE:
	default:
		{
			if (m_Activity == m_IdealActivity)
				return;

			m_Activity = m_IdealActivity;
			animDesired = LookupActivity(m_Activity);
			// Already using the desired animation?
			if (pev->sequence == animDesired)
				return;

			pev->gaitsequence = 0;
			pev->sequence = animDesired;
			pev->frame = 0;
			ResetSequenceInfo();
			return;
		}
		break;

	case ACT_RANGE_ATTACK1:
		{
			if (FBitSet(pev->flags, FL_DUCKING))
				strcpy(szAnim, "crouch_shoot_");
			else
				strcpy(szAnim, "ref_shoot_");

			strcat(szAnim, m_szAnimExtention);
			animDesired = LookupSequence(szAnim);
			if (animDesired == -1)
				animDesired = 0;

			if (pev->sequence != animDesired || !m_fSequenceLoops)
				pev->frame = 0;

			if (!m_fSequenceLoops)
				pev->effects |= EF_NOINTERP;

			m_Activity = m_IdealActivity;
			pev->sequence = animDesired;
			ResetSequenceInfo();
		}
		break;

	case ACT_WALK:
		{
			if (m_Activity != ACT_RANGE_ATTACK1 || m_fSequenceFinished)
			{
				if (FBitSet(pev->flags, FL_DUCKING))
					strcpy(szAnim, "crouch_aim_");
				else
					strcpy(szAnim, "ref_aim_");

				strcat(szAnim, m_szAnimExtention);
				animDesired = LookupSequence(szAnim);
				if (animDesired == -1)
					animDesired = 0;

				m_Activity = ACT_WALK;
			}
			else
				animDesired = pev->sequence;
		}
		break;

	case ACT_ARM:
		{
			if (FBitSet(pev->flags, FL_DUCKING))// XDM: draw animations
				strcpy(szAnim, "crouch_draw_");
			else
				strcpy(szAnim, "ref_draw_");

			strcat(szAnim, m_szAnimExtention);
			animDesired = LookupSequence(szAnim);
			if (animDesired == -1)
				animDesired = 0;

			if (pev->sequence != animDesired || !m_fSequenceLoops)
				pev->frame = 0;

			if (!m_fSequenceLoops)
				pev->effects |= EF_NOINTERP;

			m_Activity = m_IdealActivity;
			pev->sequence = animDesired;
			ResetSequenceInfo();
		}
		break;

	case ACT_RELOAD:
		{
			if (FBitSet(pev->flags, FL_DUCKING))// XDM: reload animations
				strcpy(szAnim, "crouch_reload_");
			else
				strcpy(szAnim, "ref_reload_");

			strcat(szAnim, m_szAnimExtention);
			animDesired = LookupSequence(szAnim);
			if (animDesired == -1)
				animDesired = 0;

			if (pev->sequence != animDesired || !m_fSequenceLoops)
				pev->frame = 0;

			if (!m_fSequenceLoops)
				pev->effects |= EF_NOINTERP;

			m_Activity = m_IdealActivity;
			pev->sequence = animDesired;
			ResetSequenceInfo();
		}
		break;
	}// end switch

	if (FBitSet(pev->flags, FL_DUCKING))
	{
		if (speed == 0)
			pev->gaitsequence	= LookupActivity( ACT_CROUCHIDLE );
		else
			pev->gaitsequence	= LookupActivity( ACT_CROUCH );
	}
	else if (speed > PLAYER_MAX_WALK_SPEED)
	{
		pev->gaitsequence	= LookupActivity( ACT_RUN );
	}
	else if (speed > 0)
	{
		pev->gaitsequence	= LookupActivity( ACT_WALK );
	}
	else
	{
		pev->gaitsequence	= m_iSequenceDeepIdle;//LookupSequence( "deep_idle" );
	}

	// Already using the desired animation?
	if (pev->sequence == animDesired)
		return;

	//ALERT( at_console, "Set animation to %d\n", animDesired );
	// Reset to first frame of desired animation
	pev->sequence		= animDesired;
	pev->frame			= 0;
	ResetSequenceInfo();
}


//-----------------------------------------------------------------------------
// Purpose: Safe method to set animation extnsion
// Input  : *szExtention - 
//-----------------------------------------------------------------------------
void CBasePlayer::SetWeaponAnimType(const char *szExtention)
{
	if (szExtention)
		strcpy(m_szAnimExtention, szExtention);
}

//-----------------------------------------------------------------------------
// Purpose: This function is used to find and store all the ammo we have into the ammo vars.
//-----------------------------------------------------------------------------
void CBasePlayer::TabulateAmmo(void)
{
/*
#if defined( CLIENT_WEAPONS )// big valve hack
	ammo_9mm = AmmoInventory( GetAmmoIndexFromRegistry( "9mm" ) );
	ammo_357 = AmmoInventory( GetAmmoIndexFromRegistry( "357" ) );
	ammo_argrens = AmmoInventory( GetAmmoIndexFromRegistry( "ARgrenades" ) );
	ammo_bolts = AmmoInventory( GetAmmoIndexFromRegistry( "bolts" ) );
	ammo_buckshot = AmmoInventory( GetAmmoIndexFromRegistry( "buckshot" ) );
	ammo_rockets = AmmoInventory( GetAmmoIndexFromRegistry( "rockets" ) );
	ammo_uranium = AmmoInventory( GetAmmoIndexFromRegistry( "uranium" ) );
	ammo_hornets = AmmoInventory( GetAmmoIndexFromRegistry( "Hornets" ) );
#endif
*/
	if (m_pActiveItem && m_pActiveItem->GetWeaponPtr())
	{
		if (m_pActiveItem->GetWeaponPtr()->m_fInReload > 0)
			SetAnimation(PLAYER_RELOAD);
			//m_IdealActivity = ACT_RELOAD;// XDM: reload animations
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::WaterMove(void)
{
	if (pev->movetype == MOVETYPE_NOCLIP)
		return;

	if (pev->health < 0)
		return;

	if (pev->waterlevel == 2)// water circles
	{
		if (g_pGameRules->FAllowEffects() && m_fWaterCircleTime < gpGlobals->time)
		{
			BeamEffect(TE_BEAMDISK, pev->origin + Vector(0,0,4), pev->origin + Vector(0,0,56), g_iModelIndexPartWhite, 12, 0, 10, 1, 64/*noise*/, Vector(95,95,95), 64, 0);
			m_fWaterCircleTime = gpGlobals->time + 1.0f;
		}
	}

	// waterlevel 0 - not in water
	// waterlevel 1 - feet in water
	// waterlevel 2 - waist in water
	// waterlevel 3 - head in water
	if (pev->waterlevel != 3)
	{
		// not underwater
		// play 'up for air' sound
		if (pev->air_finished < gpGlobals->time)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade1.wav", VOL_NORM, ATTN_NORM);
		else if (pev->air_finished < gpGlobals->time + 9)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade2.wav", VOL_NORM, ATTN_NORM);

		pev->air_finished = gpGlobals->time + PLAYER_AIR_TIME;
		pev->dmg = 2.0f;

		// if we took drowning damage, give it back slowly
		if (m_idrowndmg > m_idrownrestored)
		{
			// set drowning damage bit.  hack - dmg_drownrecover actually
			// makes the time based damage code 'give back' health over time.
			// make sure counter is cleared so we start count correctly.
			// NOTE: this actually causes the count to continue restarting
			// until all drowning damage is healed.
			m_bitsDamageType |= DMG_DROWNRECOVER;
			m_bitsDamageType &= ~DMG_DROWN;
			m_rgbTimeBasedDamage[itbd_DrownRecover] = 0;
		}
	}
	else
	{	// fully under water
		// stop restoring damage while underwater
		m_bitsDamageType &= ~DMG_DROWNRECOVER;
		m_rgbTimeBasedDamage[itbd_DrownRecover] = 0;

		if (pev->air_finished < gpGlobals->time)		// drown!
		{
			if (pev->pain_finished < gpGlobals->time)
			{
				// take drowning damage
				pev->dmg += 1;
				if (pev->dmg > 5)
					pev->dmg = 5;

				TakeDamage(g_pWorld, g_pWorld, pev->dmg, DMG_DROWN);// XDM3034 VARS(eoNullEntity)
				pev->pain_finished = gpGlobals->time + 1;

				// track drowning damage, give it back when
				// player finally takes a breath
				m_idrowndmg += pev->dmg;
			}
		}
		else
			m_bitsDamageType &= ~DMG_DROWN;
	}

	if (pev->waterlevel == 0)
	{
		if (FBitSet(pev->flags, FL_INWATER))
			ClearBits(pev->flags, FL_INWATER);

		return;
	}

	// make bubbles
	int air = (int)(pev->air_finished - gpGlobals->time);
	if (pev->waterlevel > 2 /*XDM*/ && !RANDOM_LONG(0,0x1f) && RANDOM_LONG(0,PLAYER_AIR_TIME-1) >= air)
	{
		switch (RANDOM_LONG(0,3))
		{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim1.wav", 0.8, ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim2.wav", 0.8, ATTN_NORM); break;
			case 2:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim3.wav", 0.8, ATTN_NORM); break;
			case 3:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim4.wav", 0.8, ATTN_NORM); break;
		}
	}

	if (!FBitSet(pev->flags, FL_INWATER))
	{
		m_flBurnTime = 0;// XDM3035c: stop burning right now
		SetBits(pev->flags, FL_INWATER);
//		pev->dmgtime = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: PlayerDeathThink
//-----------------------------------------------------------------------------
void CBasePlayer::PlayerDeathThink(void)
{
	if (pev->movetype != MOVETYPE_NOCLIP)
	{
		if (FBitSet(pev->flags, FL_ONGROUND))
		{
			float flForward = pev->velocity.Length() - 20.0f;
			if (flForward <= 0)
				pev->velocity = g_vecZero;
			else
				pev->velocity = flForward * pev->velocity.Normalize();
		}
	}

	if (HasWeapons())
	{
		// we drop the guns here because weapons that have an area effect and can kill their user
		// will sometimes crash coming back from CBasePlayer::Killed() if they kill their owner because the
		// player class sometimes is freed. It's safer to manipulate the weapons once we know
		// we aren't calling into any of their code anymore through the player pointer.
		try
		{
			PackDeadPlayerItems();
		}
		catch(...)
		{
			ALERT(at_console, "ERROR: PlayerDeathThink: PackDeadPlayerItems() exception!\n");
		}
	}

	if (pev->movetype == MOVETYPE_NOCLIP)// XDM3035 Disintegrate
	{
		ClearBits(pev->flags, FL_ONGROUND);
		if (pev->renderamt > 1)
		{
			pev->renderamt -= 1;
			pev->origin.z += 0.5f;// HACK

			if (g_pGameRules->FAllowEffects())
			{
				if (RANDOM_LONG(0,5) == 0)
				{
					pev->effects |= EF_MUZZLEFLASH;
					UTIL_Sparks(pev->origin);
				}
			}
		}
		else
			pev->renderamt = 0;// add EF_NODRAW?
	}

	if (pev->modelindex && (!m_fSequenceFinished) && (pev->deadflag == DEAD_DYING))
	{
		StudioFrameAdvance();
		m_iRespawnFrames++;				// Note, these aren't necessarily real "frames", so behavior is dependent on # of client movement commands
		if (m_iRespawnFrames < 180)   // Animations should be no longer than this
			return;
	}

	// once we're done animating our death and we're on the ground, we want to set movetype to None so our dead body won't do collisions and stuff anymore
	// this prevents a bug where the dead body would go to a player's head if he walked over it while the dead player was clicking their button to respawn
	if (pev->movetype != MOVETYPE_NONE && FBitSet(pev->flags, FL_ONGROUND))
		if (pev->movetype != MOVETYPE_NOCLIP)/// XDM3035
			pev->movetype = MOVETYPE_NONE;

	if (pev->deadflag == DEAD_DYING)
		pev->deadflag = DEAD_DEAD;

	StopAnimation();

	pev->effects |= EF_NOINTERP;
	pev->framerate = 0.0;

	BOOL fAnyButtonDown = (pev->button & ~IN_SCORE);

	// wait for all buttons released
	if (pev->deadflag == DEAD_DYING)// XDM3034 TESTME
	{
		if (fAnyButtonDown)
			return;
	}
	else if (pev->deadflag == DEAD_DEAD)// XDM3034 TESTME
	{
		m_fDeadTime = gpGlobals->time;
		pev->deadflag = DEAD_RESPAWNABLE;
	}

// if the player has been dead for one second longer than allowed by forcerespawn,
// forcerespawn isn't on. Send the player off to an intermission camera until they
// choose to respawn.
//	if ( g_pGameRules->IsMultiplayer() && ( gpGlobals->time > (m_fDeadTime + 5.0) ) && !(m_afPhysicsFlags & PFLAG_OBSERVER) )
// XDM: moved to client		StartDeathCam();// go to dead camera.

// wait for any button down, or mp_forcerespawn is set and the respawn time is up
	if (fAnyButtonDown || (g_pGameRules->FForceRespawnPlayer() && m_fDeadTime > 0.0f && (gpGlobals->time > (m_fDeadTime + mp_forcerespawntime.value))))// XDM3034 TESTME
	{
		pev->button = 0;
		m_iRespawnFrames = 0;
		//ALERT(at_console, "Respawn\n");
	//	respawn(pev, !(m_afPhysicsFlags & PFLAG_OBSERVER) );// don't copy a corpse if we're in deathcam.

		// XDM3035
		pev->nextthink = -1;

		if (g_pGameRules->FPlayerCanRespawn(this))// XDM
		{
			pev->impulse = 0;// XDM3035a: clear all pending impulse commands (flashlight, flares, etc.)
			pev->effects |= EF_NODRAW;// XDM
	//		if (!m_afPhysicsFlags & PFLAG_OBSERVER)
	//			CopyToBodyQue(pev);// make a copy of the dead body for appearances sake
			// respawn player
			pev->effects |= EF_NOINTERP;
			// TODO: FIXME: HACK: UNDONE: TESTME: just this->Spawn(FALSE)?
			GetClassPtr((CBasePlayer *)pev)->Spawn(FALSE);// XDM3035
		}
		else if (!g_pGameRules->IsMultiplayer())// restart the entire server
			SERVER_COMMAND("reload\n");

		// XDM3035
//		pev->nextthink = -1;
	}
}

//-----------------------------------------------------------------------------
// Purpose: handles +USE keypress
//-----------------------------------------------------------------------------
void CBasePlayer::PlayerUse(void)
{
	// Was use pressed or released?
	if (!((pev->button | m_afButtonPressed | m_afButtonReleased) & IN_USE))
		return;

	// Hit Use
	if (m_afButtonPressed & IN_USE)
	{
		if (m_pTank != NULL)
		{
			// Stop controlling the tank
			// TODO: Send HUD Update
			m_pTank->Use(this, this, USE_OFF, 0);
			m_pTank = NULL;
			return;
		}
		else
		{
			// Hit Use on a train?
// XDM3035			if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
			if (IsOnTrain())// XDM3035b
			{
// XDM3035				m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
// XDM3035				m_iTrain = TRAIN_NEW|TRAIN_OFF;
				TrainDetach();// don't stop the train, just disable controls
				return;
			}
			else
			{	// Start controlling the train!
				if (!(pev->button & IN_JUMP) && FBitSet(pev->flags, FL_ONGROUND))
				{
					CBaseEntity *pTrain = CBaseEntity::Instance( pev->groundentity );
					if (pTrain && (pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) && pTrain->OnControls(pev))
					{
// XDM3035						m_afPhysicsFlags |= PFLAG_ONTRAIN;
// XDM3035						m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
// XDM3035						m_iTrain |= TRAIN_NEW;
// XDM3035						pTrain->pev->euser1 = edict();// become attacker in multiplayer
						if (TrainAttach(pTrain))// XDM3035: HACKHACKHACK!!!!!!!!
							EMIT_SOUND(ENT(pev), CHAN_ITEM, "plats/train_use1.wav", 0.8, ATTN_NORM);

						return;// we're done here
					}
				}
			}
		}
	}

	CBaseEntity *pObject = NULL;
	CBaseEntity *pClosest = NULL;
	Vector vecLOS;
	Vector vecSrc = GetGunPosition();
	float flMaxDot = VIEW_FIELD_NARROW;
	float flDot;
	int caps = 0;

	UTIL_MakeVectors(pev->v_angle);// so we know which way we are facing

	while ((pObject = UTIL_FindEntityInSphere(pObject, pev->origin, PLAYER_USE_SEARCH_RADIUS)) != NULL)
	{
		if (pObject == this)// XDM3035
			continue;
//		if (pObject->IsProjectile())// XDM3037: satchels and mines are usable!
//			continue;
		if (pObject->IsPlayerItem() && (pObject->pev->effects & EF_NODRAW))// XDM3037
			continue;

		caps = pObject->ObjectCaps();
		if (caps & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE | FCAP_DIRECTIONAL_USE))
		{
			// !!!PERFORMANCE- should this check be done on a per case basis AFTER we've determined that
			// this object is actually usable? This dot is being done for every object within PLAYER_USE_SEARCH_RADIUS
			// when player hits the use key. How many objects can be in that area, anyway? (sjb)
			vecLOS = (pObject->Center() - vecSrc);// old: VecBModelOrigin

			// This essentially moves the origin of the target to the corner nearest the player to test to see
			// if it's "hull" is in the view cone
			vecLOS = UTIL_ClampVectorToBox(vecLOS, pObject->pev->size * 0.5f);

// can't see wtf is this vector anyway			UTIL_DebugBeam(pObject->Center(), pObject->Center() + vecLOS, 5.0);

			// XDM3037: traceline here to prevent USEing buttons through walls
			if (caps & FCAP_ONLYDIRECT_USE)
			{
				TraceResult tr;
				UTIL_TraceLine(vecSrc, pObject->Center(), ignore_monsters, dont_ignore_glass, edict(), &tr);// UNDONE: trace to object's closest plane, not center
				if (tr.flFraction < 1.0f && tr.pHit != pObject->edict())
					continue;
			}

			flDot = DotProduct(vecLOS, gpGlobals->v_forward);
			if (flDot > flMaxDot)
			{// only if the item is in front of the user
				pClosest = pObject;
				flMaxDot = flDot;
//				ALERT( at_console, "%s : %f\n", STRING( pObject->pev->classname ), flDot );
			}
//			ALERT( at_console, "%s : %f\n", STRING( pObject->pev->classname ), flDot );
		}
	}
	pObject = pClosest;

	// Found an object
	if (pObject)
	{
		if (m_afButtonPressed & IN_USE)
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/wpn_moveselect.wav", 0.4, ATTN_IDLE);

		caps = pObject->ObjectCaps();// !
		if (((pev->button & IN_USE) && (caps & FCAP_CONTINUOUS_USE)) ||
			((m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE|FCAP_ONOFF_USE))))
		{
			if (caps & FCAP_CONTINUOUS_USE)
				m_afPhysicsFlags |= PFLAG_USING;

			pObject->Use(this, this, USE_SET, 1);
		}
		// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
		else if ((m_afButtonReleased & IN_USE) && (pObject->ObjectCaps() & FCAP_ONOFF_USE))	// BUGBUG This is an "off" use
		{
			pObject->Use(this, this, USE_SET, (pObject->GetState() == STATE_OFF)?1:0);// XDM3037
		}
	}
	else
	{
		if (m_afButtonPressed & IN_USE)
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/wpn_denyselect.wav", 0.4, ATTN_IDLE);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Jump
//-----------------------------------------------------------------------------
void CBasePlayer::Jump(void)
{
	Vector		vecWallCheckDir;// direction we're tracing a line to find a wall when walljumping
	Vector		vecAdjustedVelocity;
	Vector		vecSpot;
	TraceResult	tr;

	if (FBitSet(pev->flags, FL_WATERJUMP))
		return;

	if (pev->waterlevel >= 2)
		return;

	// jump velocity is sqrt( height * gravity * 2)
	// If this isn't the first frame pressing the jump button, break out.
	if (!FBitSet(m_afButtonPressed, IN_JUMP))
		return;         // don't pogo stick

	if (!(pev->flags & FL_ONGROUND) || !pev->groundentity)
		return;

// many features in this function use v_forward, so makevectors now.
	UTIL_MakeVectors (pev->angles);
	// ClearBits(pev->flags, FL_ONGROUND);			// don't stairwalk
//	SetAnimation(PLAYER_JUMP);

//	pev->punchangle.x -= 5.0f;

	if (m_fLongJump && pev->velocity.Length() > 50)
	{
		SetAnimation(PLAYER_SUPERJUMP);
		FX_Trail(pev->origin, entindex(), FX_PLAYER_LONGJUMP);
	}
	else
		SetAnimation(PLAYER_JUMP);

	// If you're standing on a conveyor, add it's velocity to yours (for momentum)
	entvars_t *pevGround = VARS(pev->groundentity);
	if (pevGround && (pevGround->flags & FL_CONVEYOR))
		pev->velocity = pev->velocity + pev->basevelocity;
}

//-----------------------------------------------------------------------------
// Purpose: Duck
//-----------------------------------------------------------------------------
void CBasePlayer::Duck(void)
{
	if (m_IdealActivity != ACT_LEAP)
		SetAnimation(PLAYER_WALK);
}

//-----------------------------------------------------------------------------
// Purpose: Player ID
//-----------------------------------------------------------------------------
void CBasePlayer::InitStatusBar(void)
{
	m_flStatusBarDisappearDelay = 0;
	m_SbarString1[0] = 0;//m_SbarString0[0] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: StatusBar - small text label shown when player is facing someone
//-----------------------------------------------------------------------------
void CBasePlayer::UpdateStatusBar(void)
{
	int newSBarState[ SBAR_END ];
	char sbuf1[ SBAR_STRING_SIZE ];
	memset( newSBarState, 0, sizeof(newSBarState) );
	strcpy( sbuf1, m_SbarString1 );

	// Find an ID Target
	TraceResult tr;
	UTIL_MakeVectors(pev->v_angle + pev->punchangle);
	Vector vecSrc = EyePosition();
	Vector vecEnd = vecSrc + (gpGlobals->v_forward * MAX_ID_RANGE);
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, edict(), &tr);

	if (tr.flFraction != 1.0)
	{
		if (!FNullEnt(tr.pHit))
		{
			CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
			if (pEntity->IsPlayer() || pEntity->IsMonster() )// XDM
			{
				newSBarState[SBAR_ID_TARGETNAME] = ENTINDEX(pEntity->edict());

				int rs = g_pGameRules->PlayerRelationship(this, pEntity);
				if (rs == GR_TEAMMATE || rs == GR_ALLY)
					strcpy(sbuf1, "#SB_AL");// XDM3035a //1 #SB_TM %p1\n2 #SB_H: %i2\n3 #SB_A: %i3
				else
					strcpy(sbuf1, "#SB_EN");// XDM3035a

				newSBarState[SBAR_ID_TARGETHEALTH] = pEntity->pev->health;// XDM
				newSBarState[SBAR_ID_TARGETARMOR] = pEntity->pev->armorvalue;

				m_flStatusBarDisappearDelay = gpGlobals->time + 1.0f;
			}
		}
		else if (m_flStatusBarDisappearDelay > gpGlobals->time)
		{
			// hold the values for a short amount of time after viewing the object
			newSBarState[SBAR_ID_TARGETNAME] = m_izSBarState[SBAR_ID_TARGETNAME];
			newSBarState[SBAR_ID_TARGETHEALTH] = m_izSBarState[SBAR_ID_TARGETHEALTH];
			newSBarState[SBAR_ID_TARGETARMOR] = m_izSBarState[SBAR_ID_TARGETARMOR];
		}
	}

	BOOL bForceResend = FALSE;
	if ( strcmp( sbuf1, m_SbarString1 ) )
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgStatusText, NULL, ENT(pev));
			WRITE_BYTE(1);
			WRITE_STRING( sbuf1 );
		MESSAGE_END();
		strcpy( m_SbarString1, sbuf1 );
		// make sure everything's resent
		bForceResend = TRUE;
	}
	// Check values and send if they don't match
	for (int i = 1; i < SBAR_END; ++i)
	{
		if ( newSBarState[i] != m_izSBarState[i] || bForceResend )
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgStatusValue, NULL, ENT(pev));
				WRITE_BYTE(i);
				WRITE_SHORT( newSBarState[i] );
			MESSAGE_END();
			m_izSBarState[i] = newSBarState[i];
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: PlayerPreThink TODO: keep as clean as possible!!!
//-----------------------------------------------------------------------------
void CBasePlayer::PreThink(void)
{
	int buttonsChanged = (m_afButtonLast ^ pev->button);	// These buttons have changed this frame

	if (pev->flags & FL_FROZEN)// XDM3035b: disable everyting
		buttonsChanged = 0;

	// Debounced button codes for pressed/released
	// UNDONE: Do we need auto-repeat?
	m_afButtonPressed =  buttonsChanged & pev->button;		// The changed ones still down are "pressed"
	m_afButtonReleased = buttonsChanged & (~pev->button);	// The ones not down are "released"

	g_pGameRules->PlayerThink(this);

//test	if (m_pCarryingObject)
//		m_pCarryingObject->pev->owner = edict();

	if (g_pGameRules->IsGameOver())
		return;// intermission or finale

	if (IsObserver())// XDM: spectator
	{
//		if (m_afButtonReleased + m_afButtonPressed > 0)
//			ALERT(at_console, "PreThink() P = %d, R = %d\n", m_afButtonReleased, m_afButtonPressed);
		Observer_HandleButtons();
		pev->impulse = 0;
		return;
	}

	CheckEnvironment();// XDM3035b: this may cause death

	UTIL_MakeVectors(pev->v_angle);             // is this still used?

//	if (!IsObserver())// XDM: BUGBUG: this prevents crash when entering spectator mode without crowbar
	try
	{
		ItemPreFrame();
	}
	catch(...)
	{
		printf("*** CBasePlayer(%d)::PreThink() ItemPreFrame() exception!\n", entindex());
		DBG_FORCEBREAK
	}

	WaterMove();
	// JOHN: checks if new client data (for HUD and view control) needs to be sent to the client
	UpdateClientData();

	// BUGBUG access violations, priveleged instructions always appear here. WTF!?

	if (IsAlive())// XDM3034: don't send useless data
	{
		CheckTimeBasedDamage();
	}
	if (pev->deadflag >= DEAD_DYING)
	{
		PlayerDeathThink();
		return;
	}

	if (m_fFrozen)
		FrozenThink();

	if (!(m_afPhysicsFlags & PFLAG_ONBARNACLE))// XDM3034
	{
		// If trying to duck, already ducked, or in the process of ducking
		if ((pev->button & IN_DUCK) || FBitSet(pev->flags,FL_DUCKING) || (m_afPhysicsFlags & PFLAG_DUCKING) )
			Duck();

		if (pev->button & IN_JUMP)
			Jump();

		if (!FBitSet(pev->flags, FL_ONGROUND))
			m_flFallVelocity = -pev->velocity.z;
	}

	if (m_afPhysicsFlags & PFLAG_ONBARNACLE)
	{
		pev->velocity = g_vecZero;
		TrainDetach();// XDM3035b: don't stop the train!
//		ClearBits(m_afPhysicsFlags, PFLAG_ONTRAIN);
	}

	TrainPreFrame();

	// StudioFrameAdvance();//!!!HACKHACK!!! Can't be hit by traceline when not animating?

	// Clear out ladder pointer
//	m_hEnemy = NULL;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::CheckTimeBasedDamage(void)
{
	int i;
	byte bDuration = 0;

	if (m_fBlindAmount > 0)
		m_fBlindAmount -= 0.2;

	if (!(m_bitsDamageType & DMG_TIMEBASED))
		return;

	// only check for time based damage approx. every 2 seconds
	if (abs(gpGlobals->time - m_tbdPrev) < 2.0)
		return;

	m_tbdPrev = gpGlobals->time;

	for (i = 0; i < CDMG_TIMEBASED; ++i)
	{
		// make sure bit is set for damage type
		if (m_bitsDamageType & (DMG_PARALYZE << i))
		{
			switch (i)
			{
			case itbd_Paralyze:
				// UNDONE - flag movement as half-speed
				if (!pev->armorvalue && !m_iItemAntidote)
				{
					TakeDamage(this, g_pWorld, PARALYZE_DAMAGE, DMG_GENERIC);// XDM3035: world
					bDuration = PARALYZE_DURATION;	
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);
				break;
			case itbd_NerveGas:
				if (!pev->armorvalue && !m_iItemAntidote)
				{
					TakeDamage(this, g_pWorld, NERVEGAS_DAMAGE, DMG_GENERIC);// XDM3035: world
					bDuration = NERVEGAS_DURATION;	
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);
			break;
			case itbd_Poison:
				if (!pev->armorvalue && !m_iItemAntidote)
				{
					TakeDamage(this, g_pWorld, POISON_DAMAGE, DMG_GENERIC);// XDM3035: world
					bDuration = POISON_DURATION;	
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);
				break;
			case itbd_Radiation:
				if (!pev->armorvalue)
				{
					TakeDamage(this, g_pWorld, RADIATION_DAMAGE, DMG_GENERIC);
					bDuration = RADIATION_DURATION;	
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);
				break;
			case itbd_DrownRecover:
				// NOTE: this hack is actually used to RESTORE health
				// after the player has been drowning and finally takes a breath
				if (m_idrowndmg > m_idrownrestored)
				{
					int idif = min(m_idrowndmg - m_idrownrestored, DROWNRESTORE_REGEN);

					TakeHealth(idif, DMG_GENERIC);
					m_idrownrestored += idif;
				}
				bDuration = DROWNRESTORE_DURATION;	// get up to 5*10 = 50 points back
				break;
			case itbd_Acid:
				if (!pev->armorvalue && !m_iItemAntidote)
				{
					TakeDamage(this, g_pWorld, ACID_DAMAGE, DMG_IGNOREARMOR);
					bDuration = ACID_DURATION;	
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
				break;

				break;
			case itbd_SlowBurn:
				if (!pev->armorvalue && !m_iItemFireSupressor)
				{
					TakeDamage(this, g_pWorld, SLOWBURN_DAMAGE, DMG_GENERIC);
					bDuration = SLOWBURN_DURATION;

					if (m_fFrozen)
						FrozenEnd();

					if (gpGlobals->time >= m_fBurnFXTime)
					{
						FX_Trail( pev->origin, entindex(), FX_PLAYER_BURN_EFFECT);
						EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_burn.wav", 0.8, ATTN_NORM);
						m_fBurnFXTime = gpGlobals->time + 2;
					}
				}
				else
				{
					FX_Trail( pev->origin, entindex(), FX_REMOVE);
					STOP_SOUND(ENT(pev), CHAN_VOICE, "player/pl_burn.wav" );
					m_bitsDamageType &= ~(DMG_PARALYZE << i);
				}
				break;
			case itbd_SlowFreeze:
				if (!pev->armorvalue)
				{
					TakeDamage(this, g_pWorld, SLOWFREEZE_DAMAGE, DMG_GENERIC);
					bDuration = SLOWFREEZE_DURATION;	
				}
				else
					m_bitsDamageType &= ~(DMG_PARALYZE << i);
				break;
			default:
				bDuration = 0;
			}

			if (m_rgbTimeBasedDamage[i])
			{
				// use up an antitoxin on poison or nervegas after a few seconds of damage
				if (m_rgItems[ITEM_TYPE_ANTIDOTE] && !m_iItemAntidote)
				{
					if (((i == itbd_NerveGas) && (m_rgbTimeBasedDamage[i] < NERVEGAS_DURATION)) || ((i == itbd_Poison) && (m_rgbTimeBasedDamage[i] < POISON_DURATION)) || ((i == itbd_Acid) && (m_rgbTimeBasedDamage[i] < ACID_DURATION)) || ((i == itbd_Paralyze) && (m_rgbTimeBasedDamage[i] < PARALYZE_DURATION)))
					{
						m_rgbTimeBasedDamage[i] = 0;
						m_rgItems[ITEM_TYPE_ANTIDOTE]--;
						m_iItemAntidote = TIME_ANTIDOTE;
						ENGINE_SETPHYSKV(edict(), PHYSKEY_ANTIDOTE, "1");
						ClientPrint(pev, HUD_PRINTTALK, UTIL_VarArgs("Antidotes left: %d\n", m_rgItems[ITEM_TYPE_ANTIDOTE]));
						EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "items/medshot5.wav", VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(96, 104));
					}
				}

				if (m_rgItems[ITEM_TYPE_FIRE_SUPRESSOR] && !m_iItemFireSupressor)
				{
					if (((i == itbd_SlowBurn) && (m_rgbTimeBasedDamage[i] < SLOWBURN_DURATION)))
					{
						m_rgbTimeBasedDamage[i] = 0;
						m_rgItems[ITEM_TYPE_FIRE_SUPRESSOR]--;
						m_iItemFireSupressor = TIME_FIRE_SUPRESSOR;
						ENGINE_SETPHYSKV(edict(), PHYSKEY_FIRE_SUPRESSOR, "1");
						ClientPrint(pev, HUD_PRINTTALK, UTIL_VarArgs("Fire Supressors left: %d\n", m_rgItems[ITEM_TYPE_FIRE_SUPRESSOR]));
						FX_Trail(pev->origin, entindex(), FX_FIRE_SUPRESSOR);
					}
				}

				// decrement damage duration, detect when done.
				if (!m_rgbTimeBasedDamage[i] || --m_rgbTimeBasedDamage[i] == 0)
				{
					m_rgbTimeBasedDamage[i] = 0;
					// if we're done, clear damage bits
					m_bitsDamageType &= ~(DMG_PARALYZE << i);
				}
			}
			else
				// first time taking this damage type - init damage duration
				m_rgbTimeBasedDamage[i] = bDuration;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: UpdateGeigerCounter
//-----------------------------------------------------------------------------
void CBasePlayer::UpdateGeigerCounter(void)
{
	// delay per update ie: don't flood net with these msgs
	if (gpGlobals->time < m_flgeigerDelay)
		return;

	m_flgeigerDelay = gpGlobals->time + GEIGERDELAY;

	// send range to radition source to client
	byte range = (byte)(m_flgeigerRange / 4.0f);

	if (range != m_igeigerRangePrev)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgGeigerRange, NULL, ENT(pev));
			WRITE_BYTE( range );
		MESSAGE_END();
		m_igeigerRangePrev = range;
	}

	// reset counter and semaphore
	if (RANDOM_LONG(0,3) == 0)
		m_flgeigerRange = 1000;
}

//-----------------------------------------------------------------------------
// Purpose: updates the position of the player's reserved sound slot in the sound list.
//-----------------------------------------------------------------------------
void CBasePlayer::UpdatePlayerSound(void)
{
	int iBodyVolume = 0;
	int iVolume = 0;
	CSound *pSound = CSoundEnt::SoundPointerForIndex(CSoundEnt::ClientSoundIndex(edict()));
	// XDM: may return null if monsters are not allowed

	if (pSound)
		pSound->m_iType = bits_SOUND_NONE;

	// now calculate the best target volume for the sound. If the player's weapon
	// is louder than his body/movement, use the weapon volume, else, use the body volume.
	if (FBitSet(pev->flags, FL_ONGROUND))
	{
		iBodyVolume = pev->velocity.Length();

		// clamp the noise that can be made by the body, in case a push trigger,
		// weapon recoil, or anything shoves the player abnormally fast.
		if (iBodyVolume > 512)
			iBodyVolume = 512;
	}
	else
		iBodyVolume = 0;

	if (pev->button & IN_JUMP)
		iBodyVolume += 100;

// convert player move speed and actions into sound audible by monsters.
	if (m_iWeaponVolume > iBodyVolume)
	{
		m_iTargetVolume = m_iWeaponVolume;
		// OR in the bits for COMBAT sound if the weapon is being louder than the player.
		if (pSound)
			pSound->m_iType |= bits_SOUND_COMBAT;
	}
	else
	{
		m_iTargetVolume = iBodyVolume;
	}

	// decay weapon volume over time so bits_SOUND_COMBAT stays set for a while
	m_iWeaponVolume -= (int)(250.0f * gpGlobals->frametime);
	if (m_iWeaponVolume < 0)
		iVolume = 0;

	if (m_fNoPlayerSound)
	{
		// debugging flag, lets players move around and shoot without monsters hearing.
		iVolume = 0;
	}
	else
	{
		// if target volume is greater than the player sound's current volume, we paste the new volume in
		// immediately. If target is less than the current volume, current volume is not set immediately to the
		// lower volume, rather works itself towards target volume over time. This gives monsters a much better chance
		// to hear a sound, especially if they don't listen every frame.
		if (pSound)
			iVolume = pSound->m_iVolume;

		if (m_iTargetVolume > iVolume)
		{
			iVolume = m_iTargetVolume;
		}
		else if (iVolume > m_iTargetVolume)
		{
			iVolume -= (int)(250.0f * gpGlobals->frametime);

			if (iVolume < m_iTargetVolume)
				iVolume = 0;
		}
	}

	if (gpGlobals->time > m_flStopExtraSoundTime)
	{
		// since the extra sound that a weapon emits only lasts for one client frame, we keep that sound around for a server frame or two
		// after actual emission to make sure it gets heard.
		m_iExtraSoundTypes = 0;
	}

	if (pSound)
	{
		pSound->m_vecOrigin = pev->origin;
		pSound->m_iType |= ( bits_SOUND_PLAYER | m_iExtraSoundTypes );
		pSound->m_iVolume = iVolume;
	}

	// keep track of virtual muzzle flash
	m_iWeaponFlash -= 256 * (int)gpGlobals->frametime;
	if (m_iWeaponFlash < 0)
		m_iWeaponFlash = 0;

	//UTIL_MakeVectors ( pev->angles );
	//gpGlobals->v_forward.z = 0;

	// Below are a couple of useful little bits that make it easier to determine just how much noise the
	// player is making.
//	UTIL_ParticleEffect ( pev->origin + gpGlobals->v_forward * iVolume, g_vecZero, 255, 25 );
//	ALERT ( at_console, "%d/%d\n", iVolume, m_iTargetVolume );
}


//-----------------------------------------------------------------------------
// Purpose: Override
// Output : float - final fall damage value
//-----------------------------------------------------------------------------
float CBasePlayer::FallDamage(const float &flFallVelocity)
{
	if (m_flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED)
		return g_pGameRules->FlPlayerFallDamage(this);

	return 0.0f;
}


//-----------------------------------------------------------------------------
// Purpose: PlayerPostThink
//-----------------------------------------------------------------------------
void CBasePlayer::PostThink(void)
{
	if (!IsAlive())
		goto pt_end;

	if (g_pGameRules->IsGameOver())
		goto pt_end;         // intermission or finale

	PowerUpThink();

	if (m_fFrozen && gpGlobals->time >= m_fFrozenFXTime)
	{
		TakeDamage(this, g_pWorld, SLOWFREEZE_DAMAGE, DMG_GENERIC);
		FX_Trail( pev->origin, entindex(), FX_PLAYER_FREEZE_EFFECT);
		m_fFrozenFXTime = gpGlobals->time + 2;
	}

	if (m_flIgnoreLadderStopTime != 0.0f && m_flIgnoreLadderStopTime < gpGlobals->time)
	{
		ENGINE_SETPHYSKV(edict(), PHYSKEY_IGNORELADDER, "0");
		m_flIgnoreLadderStopTime = 0.0f;
	}

	// Handle Tank controlling
	if (m_pTank != NULL)
	{ // if they've moved too far from the gun,  or selected a weapon, unuse the gun
		if (m_pTank->OnControls(pev) && !pev->weaponmodel)
		{
			m_pTank->Use(this, this, USE_SET, 2);	// try fire the gun
		}
		else// they've moved off the platform
		{
			m_pTank->Use(this, this, USE_OFF, 0);
			m_pTank = NULL;
		}
	}

// do weapon stuff
	try
	{
		ItemPostFrame();
	}
	catch(...)
	{
		ALERT(at_console, "ERROR: PostThink: ItemPostFrame() exception!\n");
	}

// check to see if player landed hard enough to make a sound
// falling farther than half of the maximum safe distance, but not as far a max safe distance will
// play a bootscrape sound, and no damage will be inflicted. Fallling a distance shorter than half
// of maximum safe distance will make no sound. Falling farther than max safe distance will play a
// fallpain sound, and damage will be inflicted based on how far the player fell

	if ((FBitSet(pev->flags, FL_ONGROUND) || IsOnLadder()) && (pev->health > 0) && m_flFallVelocity >= PLAYER_FALL_PUNCH_THRESHHOLD)
	{
		// ALERT ( at_console, "%f\n", m_flFallVelocity );
		if (pev->watertype == CONTENTS_WATER && pev->waterlevel >= 2)// XDM3035a: player still get hit on shallow areas
		{
			// Did he hit the world or a non-moving entity?
			// BUG - this happens all the time in water, especially when
			// water has current force
			// if ( !pev->groundentity || VARS(pev->groundentity)->velocity.z == 0 )
			// EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade1.wav", VOL_NORM, ATTN_NORM);

			// see PM_CheckFalling g_usPM_Fall
//			EMIT_SOUND(ENT(pev), CHAN_STATIC, "common/watersplash.wav", VOL_NORM, ATTN_NORM);
//			StreakSplash(pev->origin+Vector(0,0,6), Vector(0,0,2), 7, 40, 20, floor(m_flFallVelocity*0.4f));
			m_fWaterCircleTime = gpGlobals->time;
			if (g_pGameRules->FAllowEffects())
			{
				UTIL_Bubbles(pev->origin + pev->mins - Vector(0.0f,0.0f,m_flFallVelocity*0.04f), pev->origin + pev->maxs, floor(m_flFallVelocity*0.1f));
			}
		}
		else if (m_flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED)// after this point, we start doing damage
		{
			float flFallDamage = FallDamage(m_flFallVelocity);//g_pGameRules->FlPlayerFallDamage(this);
			if (flFallDamage > 0.0f)
			{
				if (pev->groundentity)// XDM3035b: TESTME! ooops! We've landed on someone's head!
				{
					CBaseEntity *pLandedOn = CBaseEntity::Instance(pev->groundentity);
					if (pLandedOn && pLandedOn->pev->takedamage != DAMAGE_NO)
					{
						if (pLandedOn->IsPlayer() || pLandedOn->IsMonster())
							pLandedOn->TakeDamage(/*this confuses DeathNotice()*/g_pWorld, this, flFallDamage, DMG_CRUSH/* | DMG_NEVERGIB*/);
					}
				}
				if (flFallDamage > pev->health)// XDM3035// NOTE: play on item/voice channel because we play footstep landing on body channel
				{
					if (m_fFrozen)
						EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, gBreakSoundsGlass[RANDOM_LONG(0,NUM_BREAK_SOUNDS-1)], VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(96, 104));
					else
						EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "player/pl_fallpain2.wav", VOL_NORM, ATTN_NORM, 0, RANDOM_LONG(96, 104));

//					UTIL_DecalPoints(pev->origin, pev->origin-Vector(0.0f,0.0f,m_flFallVelocity), ENT(pev), DECAL_BLOODSMEARR1);// the biggest spot // XDM3035a
					pev->punchangle.x = 0.0f;
				}
//				else if (flFallDamage > pev->health/2)
//				{
//					UTIL_DecalPoints(pev->origin, pev->origin-Vector(0.0f,0.0f,m_flFallVelocity), ENT(pev), DECAL_BLOODSMEARR2 + RANDOM_LONG(0,1));// XDM3035a
//				}
				TakeDamage(g_pWorld, g_pWorld, flFallDamage, DMG_CRUSH | DMG_NEVERGIB);// XDM: never gib! XDM3034 VARS(eoNullEntity)
			}
		}

		if (IsAlive())
			SetAnimation(PLAYER_WALK);

		if (m_flFallVelocity >= PLAYER_MIN_BOUNCE_SPEED)// XDM && !g_pGameRules->IsMultiplayer())
		{
			CSoundEnt::InsertSound(bits_SOUND_PLAYER, pev->origin, (int)m_flFallVelocity, 0.2);
		}
		m_flFallVelocity = 0;
	}

	// select the proper animation for the player character
// already checked	if (IsAlive())
	{
		if (!pev->velocity.x && !pev->velocity.y)
			SetAnimation( PLAYER_IDLE );
		else if ((pev->velocity.x || pev->velocity.y) && (FBitSet(pev->flags, FL_ONGROUND)))
			SetAnimation( PLAYER_WALK );
		else if (pev->waterlevel > 1)
			SetAnimation( PLAYER_WALK );
	}

	StudioFrameAdvance();

	pev->modelindex = g_ulModelIndexPlayer;// XDM: ?

	UpdatePlayerSound();

pt_end:
	// Track button info so we can detect 'pressed' and 'released' buttons next frame
	m_afButtonLast = pev->button;

#if defined( CLIENT_WEAPONS )
		// Decay timers on weapons
	// go through all of the weapons and make a list of the ones to pack
	for (int i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
	{
		if (m_rgpWeapons[i])
		{
			CBasePlayerWeapon *gun = m_rgpWeapons[i]->GetWeaponPtr();

			if (gun && gun->UseDecrement())
			{
				gun->m_flNextPrimaryAttack		= max( gun->m_flNextPrimaryAttack - gpGlobals->frametime, -1.0 );
				gun->m_flNextSecondaryAttack	= max( gun->m_flNextSecondaryAttack - gpGlobals->frametime, -0.001 );

				if (gun->m_flTimeWeaponIdle != 1000)
					gun->m_flTimeWeaponIdle = max( gun->m_flTimeWeaponIdle - gpGlobals->frametime, -0.001 );

				if (gun->pev->fuser1 != 1000)
					gun->pev->fuser1 = max( gun->pev->fuser1 - gpGlobals->frametime, -0.001 );

				// Only decrement if not flagged as NO_DECREMENT
				if (gun->m_flPumpTime != 1000)
					gun->m_flPumpTime = max( gun->m_flPumpTime - gpGlobals->frametime, -0.001 );

				if (gun->m_flNextAmmoBurn != 1000)// XDM: unhack
					gun->m_flNextAmmoBurn = max( gun->m_flNextAmmoBurn - gpGlobals->frametime, -0.001 );
			}
		}
	}

	m_flNextAttack -= gpGlobals->frametime;
	if (m_flNextAttack < -0.001)
		m_flNextAttack = -0.001;
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::FlashlightIsOn(void)
{
	return FBitSet(pev->effects, EF_DIMLIGHT);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::FlashlightTurnOn(void)
{
	if (!g_pGameRules->FAllowFlashlight())
		return;

	if ((pev->weapons & (1<<WEAPON_SUIT)))
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "items/flashlight1.wav", VOL_NORM, ATTN_IDLE, 0, PITCH_NORM);
		SetBits(pev->effects, EF_DIMLIGHT);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::FlashlightTurnOff(void)
{
	if (IsAlive() && (pev->weapons & (1<<WEAPON_SUIT)))
		EMIT_SOUND_DYN(ENT(pev), CHAN_ITEM, "items/flashlight1.wav", VOL_NORM, ATTN_IDLE, 0, PITCH_NORM);

    ClearBits(pev->effects, EF_DIMLIGHT);
}

//-----------------------------------------------------------------------------
// Purpose: When recording a demo, we need to have the server tell us the entire client state
// so that the client side .dll can behave correctly.
// Reset stuff so that the state is transmitted.
//-----------------------------------------------------------------------------
void CBasePlayer::ForceClientDllUpdate(void)
{
	m_iClientHealth  = -1;
	m_iClientBattery = -1;
	m_iClientWpnIcon	= -1;
	m_iTrain |= TRAIN_NEW;  // Force new train message.
	m_fWeapon = FALSE;          // Force weapon send
	m_fKnownItem = FALSE;    // Force weaponinit messages.
	m_fInitHUD = TRUE;		// Force HUD gmsgResetHUD message
	// Now force all the necessary messages to be sent.
	UpdateClientData();
}

//-----------------------------------------------------------------------------
// Purpose: "impulse %d"
//-----------------------------------------------------------------------------
void CBasePlayer::ImpulseCommands(void)
{
	if (m_fFrozen)// XDM: fgrenade
	{
		pev->impulse = 0;
		return;
	}

	int iImpulse = (int)pev->impulse;
	switch (iImpulse)
	{
	case 98:
		{
			if (m_pActiveItem)
			{
				CBasePlayerItem *pGun = m_pActiveItem->GetWeaponPtr();
				if (pGun)
					((CBasePlayerWeapon*)pGun)->SwitchToSWeapon();
			}				
			break;
		}
	case 99:
		{
			static byte iOn = 0;
			if (iOn == 0)
				iOn = 1;
			else
				iOn = 0;
			// XDM: old stuff
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_LOGO);
			WRITE_SHORT(iOn);
			MESSAGE_END();	
			
		break;
		}
	case 100:
		{
			if (FlashlightIsOn())
				FlashlightTurnOff();
			else
				FlashlightTurnOn();
		}
		break;

		// air strike (mortar)
	case 120:
		{
			if (m_rgItems[ITEM_TYPE_AIRSTRIKE] > 0 && (gpGlobals->time >= m_fAirStrikeTime) )
			{
				UTIL_MakeVectors(pev->v_angle);
				CAirStrikeTarget::CreateNew(this, GetGunPosition() + gpGlobals->v_forward*24.0f, gSkillData.DmgAirStrike + (gSkillData.DmgAirStrike * (m_iItemWeaponPowerAug*0.1)), AIRSTRIKE_MORTAR);

				m_rgItems[ITEM_TYPE_AIRSTRIKE]--;
				m_fAirStrikeTime = gpGlobals->time + 5;

				if (!IsBot())
					FX_WpnIcon(ENT(pev), m_rgItems[ITEM_TYPE_FLASHBANG], m_rgItems[ITEM_TYPE_TRIPMINE], m_rgItems[ITEM_TYPE_SPIDERMINE], m_rgItems[ITEM_TYPE_AIRSTRIKE], m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], m_rgItems[ITEM_TYPE_ENERGY_CUBE]);
			}
		}
	break;
	//place trip mine
	case 121:
		{
			PlaceMine();
		}
	break;

	//throw spider mine
	case 122:
		{
			if (m_rgItems[ITEM_TYPE_SPIDERMINE] > 0 && (gpGlobals->time >= m_fSpiderMineTime) )
			{
				UTIL_MakeVectors(pev->v_angle);
#ifndef CLIENT_DLL
				CBaseEntity *pSpider = Create("monster_spider_mine", GetGunPosition() + gpGlobals->v_forward * 24.0f, pev->angles, pev->velocity + gpGlobals->v_forward * 300, ENT(pev));
				if (pSpider)
				{
					pSpider->pev->spawnflags |= SF_NORESPAWN;
					pSpider->pev->dmg = gSkillData.DmgSpiderMine + (gSkillData.DmgSpiderMine * (m_iItemWeaponPowerAug*0.1));
				}
#endif
				m_rgItems[ITEM_TYPE_SPIDERMINE]--;
				m_fSpiderMineTime = gpGlobals->time + 3.5;

				if (!IsBot())
					FX_WpnIcon(ENT(pev), m_rgItems[ITEM_TYPE_FLASHBANG], m_rgItems[ITEM_TYPE_TRIPMINE], m_rgItems[ITEM_TYPE_SPIDERMINE], m_rgItems[ITEM_TYPE_AIRSTRIKE], m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], m_rgItems[ITEM_TYPE_ENERGY_CUBE]);
			}
		}
	break;	
//summon atom bomb
	case 123:
		{
			if (m_rgItems[ITEM_TYPE_ENERGY_CUBE] >= MAX_ENERGY_CUBE_CARRY && (gpGlobals->time >= m_fNukeTime))
			{
				UTIL_MakeVectors(pev->v_angle);
#ifndef CLIENT_DLL
				CBaseEntity *pTeleport = Create("teleporttarget", GetGunPosition() + gpGlobals->v_forward * 24.0f, pev->angles, pev->velocity + gpGlobals->v_forward * 300, ENT(pev));
				if (pTeleport)
					pTeleport->pev->spawnflags |= SF_NORESPAWN;
#endif
				m_rgItems[ITEM_TYPE_ENERGY_CUBE] = 0;

				if (!IsBot())
					FX_WpnIcon(ENT(pev), m_rgItems[ITEM_TYPE_FLASHBANG], m_rgItems[ITEM_TYPE_TRIPMINE], m_rgItems[ITEM_TYPE_SPIDERMINE], m_rgItems[ITEM_TYPE_AIRSTRIKE], m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], m_rgItems[ITEM_TYPE_ENERGY_CUBE]);
			}
			else
				ClientPrint(pev, HUD_PRINTTALK, UTIL_VarArgs("Not enough power to summon Atom Bomb!\n"));
				m_fNukeTime = gpGlobals->time + 5.0;
		}
	break;
// call satellite strike
	case 124:
		{
			if (m_rgItems[ITEM_TYPE_SATELLITE_STRIKE] > 0 && (gpGlobals->time >= m_fSatelliteStrikeTime) )
			{
				UTIL_MakeVectors(pev->v_angle);
				CAirStrikeTarget::CreateNew(this, GetGunPosition() + gpGlobals->v_forward*24.0f, gSkillData.DmgSatelliteStrike + (gSkillData.DmgSatelliteStrike * (m_iItemWeaponPowerAug*0.1)), AIRSTRIKE_SATELLITE);

				m_rgItems[ITEM_TYPE_SATELLITE_STRIKE]--;
				m_fSatelliteStrikeTime = gpGlobals->time + 5;

				if (!IsBot())
					FX_WpnIcon(ENT(pev), m_rgItems[ITEM_TYPE_FLASHBANG], m_rgItems[ITEM_TYPE_TRIPMINE], m_rgItems[ITEM_TYPE_SPIDERMINE], m_rgItems[ITEM_TYPE_AIRSTRIKE], m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], m_rgItems[ITEM_TYPE_ENERGY_CUBE]);
			}
		}
	break;

	case 125:
		{
			if (m_rgItems[ITEM_TYPE_FLASHBANG] > 0 && (gpGlobals->time >= m_fFlashBangTime) )
			{
				UTIL_MakeVectors(pev->v_angle);
				CFlashBang::ShootFlashBang(this, GetGunPosition() + gpGlobals->v_forward*24.0f, FIRESPREAD_FLASHBANG, gSkillData.DmgFlashBang + (gSkillData.DmgFlashBang * (m_iItemWeaponPowerAug*0.1)));

				m_rgItems[ITEM_TYPE_FLASHBANG]--;
				m_fFlashBangTime = gpGlobals->time + 2.5;

				if (!IsBot())
					FX_WpnIcon(ENT(pev), m_rgItems[ITEM_TYPE_FLASHBANG], m_rgItems[ITEM_TYPE_TRIPMINE], m_rgItems[ITEM_TYPE_SPIDERMINE], m_rgItems[ITEM_TYPE_AIRSTRIKE], m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], m_rgItems[ITEM_TYPE_ENERGY_CUBE]);
			}
		}
	break;

	case 201:// paint decal
		if (m_flNextDecalTime <= gpGlobals->time)
		{
			TraceResult	tr;
			UTIL_MakeVectors(pev->v_angle);
			UTIL_TraceLine(GetGunPosition(), GetGunPosition() + gpGlobals->v_forward * 128.0f, ignore_monsters, ENT(pev), & tr);
			if (tr.flFraction != 1.0)// line hit something, so paint a decal
			{
				int nFrames = GetCustomDecalFrames();
				if (nFrames == -1)// No customization present.
					UTIL_DecalTrace(&tr, DECAL_BIOHAZ);
				else
					UTIL_PlayerDecalTrace(&tr, entindex(), nFrames, TRUE);

				EMIT_SOUND_DYN2(ENT(pev), CHAN_STATIC, "player/sprayer.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);// XDM: CHAN_VOICE
				m_flNextDecalTime = gpGlobals->time + sv_decalfrequency.value;
			}
		}
		break;
	case 204:  //  Demo recording, update client dll specific data again.
		ForceClientDllUpdate();
		break;
	default:
		// check all of the cheat impulse commands now
		CheatImpulseCommands(iImpulse);
		break;
	}
	pev->impulse = 0;
}

//-----------------------------------------------------------------------------
// Purpose: ImpulseCommands considered as cheats
// Input  : iImpulse - 
//-----------------------------------------------------------------------------
void CBasePlayer::CheatImpulseCommands(const int &iImpulse)
{
	if (g_psv_cheats->value <= 0.0f)
		return;

	switch (iImpulse)
	{
	case 101:
		gEvilImpulse101 = TRUE;
			m_iFlashBattery		= POWER_MAX_CHARGE_SUN_OF_GOD-1;
			pev->armorvalue		= 999;
			pev->health			= MAX_PLAYER_HEALTH;
		gEvilImpulse101 = FALSE;
		break;

	case 103:
		{
			// What the hell are you doing?
			CBaseEntity *pEntity = UTIL_FindEntityForward(this);
			if (pEntity)
			{
				ALERT(at_console, "%s: report AI state\n", STRING(pEntity->pev->classname));
				CBaseMonster *pMonster = pEntity->MyMonsterPointer();
				if (pMonster)
					pMonster->ReportAIState();
			}
		}
		break;
	case 104:
		// Dump all of the global state varaibles (and global entity names)
		gGlobalState.DumpGlobals();
		break;
	case 105:// player makes no sound for monsters to hear.
		{
			if (m_fNoPlayerSound)
			{
				ALERT(at_console, "Player is audible\n");
				m_fNoPlayerSound = FALSE;
			}
			else
			{
				ALERT(at_console, "Player is silent\n");
				m_fNoPlayerSound = TRUE;
			}
		}
		break;
	case 106:
		{
			// Give me the classname and targetname of this entity.
			CBaseEntity *pEntity = UTIL_FindEntityForward(this);
			if (pEntity)
				UTIL_PrintEntInfo(pEntity);
		}
		break;
	case 107:
		{
			TraceResult tr;
			Vector start = GetGunPosition();
			Vector end = start + gpGlobals->v_forward * 1024.0f;
			UTIL_TraceLine(start, end, ignore_monsters, edict(), &tr);
			if (tr.pHit)
			{
				const char *pTextureName = TRACE_TEXTURE(tr.pHit, start, end);
				if (pTextureName)
					ALERT(at_console, "Texture: %s\n", pTextureName);
			}
		}
		break;
	case 195:
		{
			ALERT(at_console, "Show shortest paths for entire level to nearest node\nnode_viewer_fly\n");
			Create("node_viewer_fly", pev->origin, pev->angles);
		}
		break;
	case 196:
		{
			ALERT(at_console, "Show shortest paths for entire level to nearest node\nnode_viewer_large\n");
			Create("node_viewer_large", pev->origin, pev->angles);
		}
		break;
	case 197:
		{
			ALERT(at_console, "Show shortest paths for entire level to nearest node\nnode_viewer_human\n");
			Create("node_viewer_human", pev->origin, pev->angles);
		}
		break;
	case 199:// show nearest node and all connections
		{
			int node = WorldGraph.FindNearestNode(pev->origin, bits_NODE_GROUP_REALM);
			ALERT(at_console, "Show nearest node and all connections\n%d\n", node);
			WorldGraph.ShowNodeConnections(node);
		}
		break;
	case 202:// Random blood splatter
		{
			ALERT(at_console, "Random blood splatter\n");
			UTIL_MakeVectors(pev->v_angle);
			TraceResult tr;
			UTIL_TraceLine(GetGunPosition(), GetGunPosition() + gpGlobals->v_forward * 128.0f, ignore_monsters, ENT(pev), & tr);

			if (tr.flFraction != 1.0)// line hit something, so paint a decal
				UTIL_BloodDecalTrace(&tr, BLOOD_COLOR_RED);
		}
		break;
	case 203:// remove creature.
		{
			CBaseEntity *pEntity = UTIL_FindEntityForward(this);
			if (pEntity && pEntity->pev->takedamage)
			{
				ALERT(at_console, "Removing entity %s\n", STRING(pEntity->pev->classname));// XDM
				pEntity->SetThink(&CBaseEntity::SUB_Remove);
			}
		}
		break;
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
// WARNING: do not use CBaseMonster version as it uses m_flNextAttack in a different way!
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::CanAttack(void)
{
	if (m_fFrozen)
		return FALSE;

	if (IsOnLadder() && (mp_laddershooting.value <= 0.0f))// XDM3035: TESTME
		return FALSE;

	if (m_flNextAttack > UTIL_WeaponTimeBase())// this prevents player from shooting while changing weapons
		return FALSE;

	return TRUE;
}
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &iAmmoIndex - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayer::AmmoInventory(const int &iAmmoIndex)
{
	if (iAmmoIndex < 0)
		return -1;

	return m_rgAmmo[iAmmoIndex];
}

//-----------------------------------------------------------------------------
// Purpose: Give Ammo by ID
// Input  : iCount - 
//			&iIndex - 
//			iMax - 
// Output : Returns the amount of ammo actually added
//-----------------------------------------------------------------------------
int CBasePlayer::GiveAmmo(const int &iAmount, const int &iIndex, const int &iMax)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayer(%d)::GiveAmmo(%d, %d, %d)\n", entindex(), iAmount, iIndex, iMax);
#endif

	if (iIndex < 0 || iIndex >= MAX_AMMO_SLOTS)
		return 0;

	if (AmmoInventory(iIndex) >= iMax)// XDM3035b: check here!
		return 0;

	if (!g_pGameRules->CanHaveAmmo(this, iIndex, iMax))
		return 0;// game rules say I can't have any more of this ammo type.

	int iAdd = min(iMax - AmmoInventory(iIndex), iAmount);// fill inventory to MAX or take what iCount can offer
	if (iAdd <= 0)
		return 0;

//	ALERT(at_console, "CBasePlayer::GiveAmmo(%d %d %d max)\n", iAmount, iIndex, iMax);

	m_rgAmmo[iIndex] += iAdd;

	if (gmsgAmmoPickup)// make sure the ammo messages have been linked first
	{
//		ALERT(at_console, "gmsgAmmoPickup: %s\n", szName);// XDM
		// Send the message that ammo has been picked up
		MESSAGE_BEGIN(MSG_ONE, gmsgAmmoPickup, NULL, ENT(pev));
			WRITE_BYTE(iIndex);		// ammo ID
			WRITE_BYTE(iAdd);		// amount
		MESSAGE_END();
	}
	TabulateAmmo();
	return iAdd;
}

//-----------------------------------------------------------------------------
// Purpose: GiveAmmo by name
// Input  : iAmount - 
//			*szName - 
//			iMax - 
// Output : Returns the amount of ammo actually added
//-----------------------------------------------------------------------------
int CBasePlayer::GiveAmmo(const int &iAmount, char *szName, const int &iMax)
{
	return GiveAmmo(iAmount, GetAmmoIndexFromRegistry(szName), iMax);
}

//-----------------------------------------------------------------------------
// Purpose: Add a weapon to the player (Item == Weapon == Selectable Object)
// XDM3035a: NOTE: add lots of checks here because we may get corrupted pointers/objects
// Input  : *pItem - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayer::AddPlayerItem(CBasePlayerItem *pItem)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d)(id %d) START\n", entindex(), pItem->entindex(), pItem->GetID());
#endif
	if (pItem == NULL)
		return ITEM_ADDRESULT_NONE;
	if (pItem->pev == NULL)
	{
		ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d) corrupted item!\n", entindex(), pItem->GetID());
		return ITEM_ADDRESULT_NONE;
	}
	if (pItem->GetID() == WEAPON_NONE)
	{
		ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d) tried to add WEAPON_NONE!\n", entindex(), pItem->GetID());
		return ITEM_ADDRESULT_NONE;
	}

//	something's not right here! recheck step-by-step!
//	somehow after removing pItem it appears in our inventory!
	int id = pItem->GetID();
	CBasePlayerItem *pExisting = GetInventoryItem(id);// XDM: !!! we may already have item of this kind!
	if (pExisting)// XDM3035b
	{
		if (pExisting == pItem)// Why? Why does this keep happening?
		{
			ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d)(id %d): pExisting == pItem!\n", entindex(), pItem->entindex(), pItem->GetID());
//			DBG_FORCEBREAK
			pItem->AttachToPlayer(this);// prevent further touching!!
			return ITEM_ADDRESULT_NONE;// pretend we did not touch it, so DefaultTouch() won't kill it! right?
		}

		if (pItem->AddDuplicate(pExisting))
		{
			if (HasPlayerItem(pItem))
			{
				ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d)(id %d) pItem got into inventory!! 1\n", entindex(), pItem->entindex(), pItem->GetID());
				DBG_FORCEBREAK
			}
			g_pGameRules->PlayerGotWeapon(this, pExisting);
//			pItem->CheckRespawn();

			// ugly hack to update clip w/o an update clip message
/*			pExisting->UpdateItemInfo();
			if (m_pActiveItem && m_pActiveItem != pExisting)// XDM3035: ??? we don't want two identical messages
				m_pActiveItem->UpdateItemInfo();

			if (HasPlayerItem(pItem))
			{
				ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d)(id %d) pItem got into inventory!! 2\n", entindex(), pItem->entindex(), pItem->GetID());
				DBG_FORCEBREAK
			}*/
			return ITEM_ADDRESULT_EXTRACTED;// EXTRACTED AMMO
		}
		if (GetInventoryItem(id) == NULL)
		{
			ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d)(id %d) KERNEL PANIC!!!!!!!!!!\n", entindex(), pItem->entindex(), pItem->GetID());
			DBG_FORCEBREAK
		}
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d)(id %d) 1 FALSE\n", entindex(), pItem->entindex(), pItem->GetID());
#endif
		return ITEM_ADDRESULT_NONE;// NOT EXTRACTED
	}

	if (pItem->AddToPlayer(this))
	{
		pev->weapons |= (1<<pItem->GetID());// XDM3035a
		m_rgpWeapons[pItem->GetID()] = pItem;
		g_pGameRules->PlayerGotWeapon(this, pItem);
//		pItem->CheckRespawn();
		MESSAGE_BEGIN(MSG_ONE, gmsgWeapPickup, NULL, ENT(pev));
			WRITE_BYTE(pItem->GetID());
		MESSAGE_END();
		// Switch only if we're not attacking or have no weapon equipped at all
		if (!(pev->button & (IN_ATTACK|IN_ATTACK2)) || m_pActiveItem == NULL)
		{
			if (g_pGameRules->FShouldSwitchWeapon(this, pItem))// should we switch to this item?
				SelectItem(pItem);
		}
#ifdef _DEBUG_ITEMS
		ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d)(id %d) 2 TRUE\n", entindex(), pItem->entindex(), pItem->GetID());
#endif
		return ITEM_ADDRESULT_PICKED;// PICKED UP
	}
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayer(%d)::AddPlayerItem(%d)(id %d) 3 FALSE!\n", entindex(), pItem->entindex(), pItem->GetID());
#endif
	return ITEM_ADDRESULT_NONE;// NOT PICKED
}

//-----------------------------------------------------------------------------
// Purpose: clears item from player, but does not destroy it!
// Input  : *pItem - 
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayer::RemovePlayerItem(CBasePlayerItem *pItem)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayer(%d)::RemovePlayerItem(%d)(id %d)\n", entindex(), pItem->entindex(), pItem->GetID());
#endif

	if (pItem == NULL)
		return FALSE;
	if (pItem->GetID() == WEAPON_NONE)
	{
//#ifdef _DEBUG
		ALERT(at_aiconsole, "! RemovePlayerItem(%d) tried to remove WEAPON_NONE!\n", pItem->GetID());
		DBG_FORCEBREAK
//#endif
		return FALSE;
	}

	if (m_pActiveItem == pItem)
	{
		ResetAutoaim();
		if (AmmoInventory(pItem->PrimaryAmmoIndex()) != 0)// CAN be -1!! Prevent recursion when removing item
			pItem->Holster();

		m_pActiveItem = NULL;
		pev->viewmodel = 0;
		pev->weaponmodel = 0;
	}

	if (m_pLastItem == pItem)
		m_pLastItem = NULL;

	if (m_pNextItem == pItem)// XDM
		m_pNextItem = NULL;

	if (m_rgpWeapons[pItem->GetID()] == pItem || m_rgpWeapons[pItem->GetID()] == NULL)// EXACTLY the same pointer! Not just same weapon ID XDM3035b: wtf?!?!
	{
		m_rgpWeapons[pItem->GetID()] = NULL;
		pev->weapons &= ~(1 << pItem->GetID());// XDM: moved from weapons like CHandGrenade // take item off hud
		pItem->DetachFromPlayer();
		return TRUE;
	}
	else
	{
		DBG_FORCEBREAK
		ALERT(at_console, "CBasePlayer(%d)::RemovePlayerItem(%d)(id %d) MISMATCH!! ei %d\n", entindex(), pItem->entindex(), pItem->GetID(), m_rgpWeapons[pItem->GetID()]->entindex());
		pItem->DetachFromPlayer();
//		pItem->DestroyItem();
		return TRUE;// HACK: to act as normal
	}
//	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: drop specified item
// Input  : *pItem - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::DropPlayerItem(CBasePlayerItem *pItem)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayer(%d)::DropPlayerItem(%d)(id %d)\n", entindex(), pItem->entindex(), pItem->GetID());
#endif

	if (pItem)// && !(pItem->iFlags() & ITEM_FLAG_CANNOTDROP))
	{
		CBasePlayerItem *pDroppedItem = pItem;

		if (pDroppedItem == m_pActiveItem)
		{
			if (SelectNextBestItem(pDroppedItem) == NULL)
				m_fWeapon = FALSE;
		}
		// m_pActiveItem changed after this!

		int	iAmmoIndex = GetAmmoIndexFromRegistry(pItem->pszAmmo1());
		if (RemovePlayerItem(pItem))// m_pActiveItem == NULL after this!
		{
			if (iAmmoIndex >= 0)// this weapon weapon uses ammo, so pack an appropriate amount.
			{
				CBasePlayerWeapon *pDroppedWeapon = pDroppedItem->GetWeaponPtr();
				if (pDroppedWeapon)
				{
					pDroppedWeapon->m_iDefaultAmmo = 0;// important
					if (pDroppedWeapon->iFlags() & ITEM_FLAG_EXHAUSTIBLE)// pack up all the ammo, this weapon is its own ammo type
					{
						pDroppedWeapon->m_iClip = m_rgAmmo[iAmmoIndex];
						m_rgAmmo[iAmmoIndex] = 0;
					}
					else// pack half of the ammo
					{
						m_rgAmmo[iAmmoIndex] /= 2;// half remains
						pDroppedWeapon->m_iClip += m_rgAmmo[iAmmoIndex];// other half packed, keeping the clip, if any
					}
					UTIL_MakeVectors(pev->v_angle);//ANGLE_VECTORS
					pDroppedItem->pev->impulse = 1;// XDM3035: "dropped by a dead player"
					pDroppedItem->pev->spawnflags |= SF_NORESPAWN;// should be already set
					pDroppedItem->pev->origin = GetGunPosition() + gpGlobals->v_forward * 36.0f;
					pDroppedItem->FallInit();
					pDroppedItem->pev->angles.y = pev->angles.y;
					pDroppedItem->pev->angles.z = 0.0f;
					pDroppedItem->pev->velocity = pev->velocity + gpGlobals->v_forward * 30.0f;// weaponbox has player's velocity, then some.
				}
			}
			return TRUE;// since RemovePlayerItem() worked
		}
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: drop the named item, or if no name, the active item.
// Input  : *pszItemName - 
//-----------------------------------------------------------------------------
BOOL CBasePlayer::DropPlayerItem(char *pszItemName)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayer(%d)::DropPlayerItem(%s)\n", entindex(), pszItemName);
#endif

	if (strlen(pszItemName) == 0)
	{
		// if this string has no length, the client didn't type a name!
		// assume player wants to drop the active item.
		// make the string null to make future operations in this function easier
		pszItemName = NULL;
	}

	CBasePlayerItem *pWeapon = NULL;
	if (pszItemName)
	{
		ItemInfo II;
		CBasePlayerItem *pItem = NULL;
		for (int i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
		{
			pItem = GetInventoryItem(i);
			if (pItem)
			{
				if (pItem->GetItemInfo(&II))// XDM3035c: use item name, not class name
				{
					if (strcmp(II.pszName, pszItemName) == 0)
					{
						pWeapon = pItem;
						break;
					}
				}
			}
		}
	}
	else
		pWeapon = m_pActiveItem;// trying to drop active item

	return DropPlayerItem(pWeapon);// pWeapon may still be NULL
}

//-----------------------------------------------------------------------------
// Purpose: 
// PackDeadPlayerItems - call this when a player dies to
// pack up the appropriate weapons and ammo items, and to
// destroy anything that shouldn't be packed.
//
// This is pretty brute force :(
//
// BUGBUG: satchels are packed AS AMMO when dropped through GR_PLR_DROP_AMMO_ALL (when not packed as weapon) so player who picks up the box won't get satchel weapon!
//-----------------------------------------------------------------------------
void CBasePlayer::PackDeadPlayerItems(void)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayer(%d)::PackDeadPlayerItems()\n", entindex());
#endif
	// get the game rules
	int iWeaponRules = g_pGameRules->DeadPlayerWeapons(this);
 	int iAmmoRules = g_pGameRules->DeadPlayerAmmo(this);

	// nothing to pack. Remove the weapons and return. Don't call create on the box!
	if (iWeaponRules == GR_PLR_DROP_GUN_NO && iAmmoRules == GR_PLR_DROP_AMMO_NO)
		goto finish;

//	CBasePlayerItem *pItemToDrop; TODO: make a cvar to decide which weapon to drop: current or best
	if (m_pActiveItem == NULL)// player was killed while changing weapons
		m_pActiveItem = m_pLastItem;

	// XDM3035b: drop real weapon, pack ammo into its clip. TODO: Move to DropPlayerItem()?
	if (iWeaponRules == GR_PLR_DROP_GUN_ACTIVE && iAmmoRules == GR_PLR_DROP_AMMO_ACTIVE)
	{
		if (m_pActiveItem)
		{
			CBasePlayerItem *pDroppedItem = m_pActiveItem;
			if (pDroppedItem->iFlags() & ITEM_FLAG_CANNOTDROP)// player is holding a weapon he cannot drop
			{
				CBasePlayerItem *pBestItem = g_pGameRules->GetNextBestWeapon(this, pDroppedItem);// try dropping some other weapon
				if (pBestItem && pBestItem != pDroppedItem)
					pDroppedItem = pBestItem;
			}
			if (pDroppedItem->iFlags() & ITEM_FLAG_CANNOTDROP)
			{
				goto finish;// still annot drop
			}
			int	iAmmoIndex = GetAmmoIndexFromRegistry(pDroppedItem->pszAmmo1());
			if (RemovePlayerItem(pDroppedItem))// m_pActiveItem == NULL after this!
			{
				if (iAmmoIndex >= 0)// this weapon weapon uses ammo, so pack an appropriate amount.
				{
					CleanupTemporaryWeapons();
					CBasePlayerWeapon *pDroppedWeapon = pDroppedItem->GetWeaponPtr();
					if (pDroppedWeapon)
					{
//						if (pDroppedWeapon->iFlags() & ITEM_FLAG_EXHAUSTIBLE)// pack up all the ammo, this weapon is its own ammo type
//						{
							pDroppedWeapon->m_iDefaultAmmo = 0;// important
							pDroppedWeapon->m_iClip += m_rgAmmo[iAmmoIndex];// don't loose the clip itself
							m_rgAmmo[iAmmoIndex] = 0;

/*						}
						else// pack half of the ammo
						{
							pDroppedWeapon->m_iClip = m_rgAmmo[iAmmoIndex]/2;
							m_rgAmmo[iAmmoIndex] /= 2;
						}*/
						pDroppedItem->pev->impulse = 1;// XDM3035: "dropped by a dead player"
						pDroppedItem->pev->spawnflags |= SF_NORESPAWN;
						pDroppedItem->FallInit();
						pDroppedItem->pev->angles.y = pev->angles.y;
						pDroppedItem->pev->angles.z = 0.0f;
						pDroppedItem->pev->velocity = pev->velocity * 1.4f;// weaponbox has player's velocity, then some.
						++g_iWeaponBoxCount;
					}
				}
				goto finish;// since RemovePlayerItem() worked, we can't go back
			}
		}
	}

	{// <- prevent Compiler Error C2362: initialization of 'identifierr' is skipped by 'goto label'
	int i = 0;
	CBasePlayerWeapon *rgpPackWeapons[PLAYER_INVENTORY_SIZE];// XDM3034: 32 hardcoded for now. How to determine exactly how many weapons we have?
	int iPackAmmo[MAX_AMMO_SLOTS + 1];
	int iPW = 0;// number of weapons packed
	int iPA = 0;// number of ammo packed

	memset(rgpPackWeapons, NULL, sizeof(rgpPackWeapons));
	memset(iPackAmmo, -1, sizeof(iPackAmmo));

	// go through all of the weapons and make a list of the ones to pack
	if (iWeaponRules != GR_PLR_DROP_GUN_NO && !(iWeaponRules == GR_PLR_DROP_GUN_ACTIVE && m_pActiveItem == NULL))
	{
		for (i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
		{
			if (GetInventoryItem(i))// there's a weapon here. Should I pack it?
			{
				CBasePlayerWeapon *pItem = m_rgpWeapons[i]->GetWeaponPtr();// XDM: ->GetWeaponPtr()
				if (pItem == NULL)
					continue;
				if (pItem->GetID() == WEAPON_NONE)// XDM3035
					continue;
//				if ((pItem->iFlags() & ITEM_FLAG_EXHAUSTIBLE) && AmmoInventory(pItem->PrimaryAmmoIndex()) == 0)// XDM UPD: don't pack exhausted items, "== 0" is critical
//					continue;// TESTME
				// XDM: check if we're trying to pack exhausted weapon
				if (pItem->iFlags() & ITEM_FLAG_EXHAUSTIBLE && !pItem->IsUseable())// DeactivateSatchels() must be called before this point!
					continue;

				if (iWeaponRules == GR_PLR_DROP_GUN_ACTIVE)
				{
					if (pItem != m_pActiveItem)
					{
						if (pItem->iFlags() & ITEM_FLAG_EXHAUSTIBLE)// unusable items were already filtered out
							if (iAmmoRules != GR_PLR_DROP_AMMO_ALL)// pack exhaustible items as ammo!
								continue;
					}
				}
				rgpPackWeapons[iPW] = pItem;
				++iPW;
			}
		}
	}

	// go through ammo and make a list of which types to pack.
	if (iAmmoRules == GR_PLR_DROP_AMMO_ACTIVE)
	{
		if (m_pActiveItem)
		{
			if (m_pActiveItem->PrimaryAmmoIndex() >= 0) // this is the primary ammo type for the active weapon
				iPackAmmo[iPA++] = m_pActiveItem->PrimaryAmmoIndex();

			if (m_pActiveItem->SecondaryAmmoIndex() >= 0)// this is the secondary ammo type for the active weapon
				iPackAmmo[iPA++] = m_pActiveItem->SecondaryAmmoIndex();
		}
	}
	else if (iAmmoRules == GR_PLR_DROP_AMMO_ALL)// we still don't check if this "ammo" is useless (satchels, grenades, mines, etc.)
	{
		for (i = 0; i < MAX_AMMO_SLOTS; ++i)
		{
			if (m_rgAmmo[i] > 0)// player has some ammo of this type.
			{
				iPackAmmo[iPA] = i;
				++iPA;
			}
		}
	}

	if (iPW > 0 || iPA > 0)// TODO: UNDONE: XDM3035: I don't really want to flood world with ammo-only weapon boxes! But others may want.
	{
		// create a box to pack the stuff into.
		CWeaponBox *pWeaponBox = CWeaponBox::CreateBox(this, pev->origin, g_vecZero);
//useless		CWeaponBox *pWeaponBox = (CWeaponBox *)CBaseEntity::Create(MAKE_STRING("weaponbox"), pev->origin + gpGlobals->v_forward * 10.0f, pev->angles, pev->velocity, edict());
		if (pWeaponBox)
		{
			int i;
			for (i=0; i<iPA; ++i)// pack the ammo
			{
				if (iPackAmmo[i] != -1)
				{
					pWeaponBox->PackAmmo(g_AmmoInfoArray[iPackAmmo[i]].name, AmmoInventory(iPackAmmo[i]));
				}
			}
			for (i=0; i<iPW; ++i)// now pack all of the items in the lists
			{
				if (rgpPackWeapons[i])
				{
					if (RemovePlayerItem(rgpPackWeapons[i]))// clear out from player's inventory, but don't destroy it!
					{
						// weapon unhooked from the player. Pack it into box.
						if (pWeaponBox->PackWeapon(rgpPackWeapons[i]) == FALSE)
						{
#ifdef _DEBUG
	ALERT(at_console, "CBasePlayer(%d)::PackDeadPlayerItems() failed to pack %d id %d into box\n", entindex(), rgpPackWeapons[i]->entindex(), rgpPackWeapons[i]->GetID());
#endif
							rgpPackWeapons[i]->DestroyItem();// XDM3035b: WARNING! Box may fail to store some items!
							rgpPackWeapons[i] = NULL;
						}
					}
				}
			}
	//		pWeaponBox->pev->angles.x = 0.0f;// don't let weaponbox tilt.
			pWeaponBox->pev->angles.y = pev->angles.y;
	//		pWeaponBox->pev->angles.z = 0.0f;
			pWeaponBox->pev->velocity = pev->velocity * 1.4f;// weaponbox has player's velocity, then some.
		}
	}
	}
finish:
	m_pActiveItem = NULL;// don't holster, just remove
	RemoveAllItems(TRUE);// now strip off everything that wasn't handled by the code above.
}

//-----------------------------------------------------------------------------
// Purpose: RemoveAllItems - clear m_rgpWeapons and other containers
// Input  : removeSuit - 
//-----------------------------------------------------------------------------
void CBasePlayer::RemoveAllItems(bool removeSuit)
{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayer(%d)::RemoveAllItems(%d)\n", entindex(), removeSuit);
#endif
	m_pLastItem = NULL;
	m_pNextItem = NULL;
	if (m_pActiveItem)
	{
		ResetAutoaim();
//		if (m_pActiveItem->GetOwner())// XDM3035a: WTF?!?!
//			m_pActiveItem->Holster();

		m_pActiveItem = NULL;
	}

	int i = 0;
	for (i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
	{
		CBasePlayerItem *pItem = GetInventoryItem(i);
		if (pItem)// XDM3035
		{
			pItem->DetachFromPlayer();// testme
			pItem->DestroyItem();// XDM: changed from 'Drop()'
			m_rgpWeapons[i] = NULL;
		}
	}

	pev->viewmodel = 0;
	pev->weaponmodel = 0;

	if (removeSuit)
		pev->weapons = 0;
	else
		pev->weapons &= ~WEAPON_ALLWEAPONS;

	for (i = 0; i < MAX_AMMO_SLOTS; ++i)
		m_rgAmmo[i] = 0;

	m_fWeapon = FALSE;// call gmsgCurWeapon
	UpdateClientData();
}

//-----------------------------------------------------------------------------
// Purpose: Called every frame by the player PreThink
//-----------------------------------------------------------------------------
void CBasePlayer::ItemPreFrame(void)
{
    if (m_flNextAttack > UTIL_WeaponTimeBase())
		return;

	if (m_pActiveItem == NULL)// XDM
	{
		if (m_pNextItem)
		{
			if (m_pNextItem->GetID() > WEAPON_NONE)// XDM3035a: somehow bad items get here
			{
				m_pActiveItem = m_pNextItem;
				DeployActiveItem();// XDM
				m_pNextItem = NULL;
			}
			else
				ALERT(at_console, "CBasePlayer::ItemPreFrame(): Bad next item!\n");
		}
	}
	else// still no active weapon?! //	if (m_pActiveItem)
		m_pActiveItem->ItemPreFrame();
}

//-----------------------------------------------------------------------------
// Purpose: Called every frame by the player PostThink
//-----------------------------------------------------------------------------
void CBasePlayer::ItemPostFrame(void)
{
//	static int fInSelect = FALSE;
	// check if the player is using a tank
	if (m_pTank != NULL)
		return;

// XDM3035: TESTME	if (m_flNextAttack > UTIL_WeaponTimeBase())// this prevents player from shooting while changing weapons
//		return;

	// Handle use events
	PlayerUse();

	ImpulseCommands();

	if (IsObserver())// XDM3035
		return;

	if (m_pActiveItem)
	{
		if (m_pActiveItem->GetID() > 0)// --TEST--
		{
			if (mp_laddershooting.value <= 0.0f)// XDM3035: TESTME
			{
				if (IsOnLadder())
				{
					if (!m_pActiveItem->IsHolstered())
					{
	//					pev->weaponmodel = 0;
						m_pActiveItem->Holster();
					}
					m_flNextAttack = gpGlobals->time+0.4f;
				}
				else
				{
					if (m_pTank == NULL && m_pTrain == NULL)
					{
						if (m_pActiveItem->IsHolstered())
							if (!m_pActiveItem->Deploy())
								ALERT(at_aiconsole, "Client %d: %s fails to Deploy()!\n", entindex(), STRING(m_pActiveItem->pev->classname));
					}
				}
			}
		}
		else
		{
#ifdef _DEBUG_ITEMS
	ALERT(at_console, "CBasePlayer(%d)::ItemPostFrame() Error! m_pActiveItem(ei %d %s) bad!\n", entindex(), m_pActiveItem->entindex(), STRING(m_pActiveItem->pev->classname));
#endif
			DBG_FORCEBREAK
		}
/*#ifdef _DEBUG
		try
		{
#endif*/
		m_pActiveItem->ItemPostFrame();
/*#ifdef _DEBUG
		}
		catch (...)
		{
			printf("*** m_pActiveItem->ItemPostFrame() exception!\n");
			DBG_FORCEBREAK
		}
#endif*/
	}
}

//-----------------------------------------------------------------------------
// XDM3035: combine into one packet/message
// Called from UpdateClientData
// makes sure the client has all the necessary ammo info,  if values have changed
//-----------------------------------------------------------------------------
void CBasePlayer::SendAmmoUpdate(void)
{
	try
	{
		int i=0;
//		int nummessages = 0;
		char pBuffer[MAX_AMMO_SLOTS*2 + 1];
		memset(pBuffer, NULL, sizeof(pBuffer));
		int iSize = 0;
		byte value;

		for (i=0; i < MAX_AMMO_SLOTS; ++i)
		{
			if (m_rgAmmo[i] != m_rgAmmoLast[i])
			{
				ASSERT(m_rgAmmo[i] >= 0);// && m_rgAmmo[i] < 255);
	//			if (m_rgAmmo[i] < 0)
	//				ALERT(at_console, "* error: Player %s has %d ammo of type %s!\n", STRING(pev->netname), m_rgAmmo[i], g_AmmoInfoArray[i].pszName);

				// send "Ammo" update message
#ifdef SERVER_OLD_AMMO_UPDATE
				MESSAGE_BEGIN(MSG_ONE, gmsgAmmoX, NULL, pev);
					WRITE_BYTE(i);
					WRITE_BYTE(max(min(m_rgAmmo[i], 254), 0));  // clamp the value to one byte
				MESSAGE_END();
#else
				value = i;
				memcpy(pBuffer+iSize, &value, sizeof(byte)); ++iSize;
				value = max(min(m_rgAmmo[i], 254), 0);
				memcpy(pBuffer+iSize, &value, sizeof(byte)); ++iSize;
#endif
				m_rgAmmoLast[i] = m_rgAmmo[i];
//				++nummessages;
			}
		}
#ifndef SERVER_OLD_AMMO_UPDATE
		if (iSize > 0)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgUpdateAmmo, NULL, ENT(pev));
				//WRITE_STRING(pBuffer);
			for (i = 0; i < iSize; ++i)
				WRITE_BYTE(pBuffer[i]);

			MESSAGE_END();
//			ALERT(at_console, "CBasePlayer::SendAmmoUpdate(): sent %d bytes\n", iSize);
		}
#endif
//			ALERT(at_aiconsole, "CBasePlayer::SendAmmoUpdate(): generated %d messages\n", nummessages);
	}
	catch (...)
	{
		printf("*** CBasePlayer::SendAmmoUpdate() exception!\n");
		DBG_FORCEBREAK
	}
}

//-----------------------------------------------------------------------------
// Purpose: send base weapon information (name, ammo type, etc.)
//-----------------------------------------------------------------------------
void CBasePlayer::SendWeaponsUpdate(void)
{
	try
	{
	// New Weapon?
	if (!m_fKnownItem)
	{
		m_fKnownItem = TRUE;
		int nummessages = 0;
		for (int i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
		{
			ItemInfo &II = g_ItemInfoArray[i];
			if (II.iId == WEAPON_NONE)
				continue;

			const char *pszName;
			if (II.pszName)
				pszName = II.pszName;
			else
				pszName = "Empty";

			// XDM: BUGBUG? Too early? PF_MessageEnd_I:  Unknown User Msg 77
			MESSAGE_BEGIN(MSG_ONE, gmsgWeaponList, NULL, ENT(pev));
				WRITE_STRING(pszName);			// string	weapon name
				WRITE_BYTE(GetAmmoIndexFromRegistry(II.pszAmmo1));	// byte		Ammo Type
				WRITE_BYTE(II.iMaxAmmo1);				// byte     Max Ammo 1
				WRITE_BYTE(GetAmmoIndexFromRegistry(II.pszAmmo2));	// byte		Ammo2 Type
				WRITE_BYTE(II.iMaxAmmo2);				// byte     Max Ammo 2
#ifdef SERVER_WEAPON_SLOTS
				WRITE_BYTE(II.iSlot);					// byte		bucket
				WRITE_BYTE(II.iPosition);				// byte		bucket pos
#endif
				WRITE_BYTE(II.iId);						// byte		id (bit index into pev->weapons)
				WRITE_BYTE(II.iFlags);					// byte		Flags
			MESSAGE_END();
			++nummessages;
		}
//#ifdef _DEBUG
//		if (nummessages > 0)
//			ALERT(at_aiconsole, "CBasePlayer::SendWeaponsUpdate(): generated %d messages\n", nummessages);
//#endif
	}// !m_fKnownItem
	}
	catch (...)
	{
		printf("*** CBasePlayer::SendWeaponsUpdate() exception 1!\n");
		DBG_FORCEBREAK
	}

	// XDM3035c: this part was moved here
	int i = 0;
	int bufferlen = 0;
	try
	{
		// Update all the items
//		int sent = 0;
		char pBuffer[PLAYER_INVENTORY_SIZE*3 + 1];
		memset(pBuffer, NULL, sizeof(pBuffer));
//		int bufferlen = 0;
		CBasePlayerItem *pItem;
		for (i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
		{
			pItem = GetInventoryItem(i);
			if (pItem)
				bufferlen += pItem->UpdateClientData(pBuffer + bufferlen);// XDM3035: combined multiple mesages into single
		}
		if (bufferlen > 0)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgUpdateWeapons, NULL, ENT(pev));// XDM3035: pack all updates into a single packet!
				//WRITE_STRING(pBuffer);// this probably detects zeros or does some nasty things with \r\n
			for (i = 0; i < bufferlen; ++i)
			{
				WRITE_BYTE(pBuffer[i]);
			}
			MESSAGE_END();
//			ALERT(at_console, "UpdateClientData(): sent %d bytes\n", bufferlen);
		}
	}
	catch (...)
	{
//#ifdef _DEBUG
//		FILE *fp=fopen("logs/XDMexcept.txt","a"); fprintf(fp,"*** CBasePlayer::UpdateClientData() exception 2c! (i = %d, bufferlen = %d)\n", i, bufferlen); fclose(fp);
//#endif
		printf("*** CBasePlayer::SendWeaponsUpdate() exception 2! (i = %d, bufferlen = %d)\n", i, bufferlen);
		DBG_FORCEBREAK
	}

	if (m_fWeapon == FALSE)// XDM:
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgCurWeapon, NULL, ENT(pev));
			WRITE_BYTE(0);
			WRITE_BYTE(0);
			WRITE_BYTE(0);
		MESSAGE_END();
		m_fWeapon = TRUE;
	}
}

extern unsigned char g_ClientShouldInitizlize[];

//-----------------------------------------------------------------------------
/*
	UpdateClientData
resends any changed player HUD info to the client.
Called every frame by PlayerPreThink
Also called at start of demo recording and playback by
ForceClientDllUpdate to ensure the demo gets messages
reflecting all of the HUD state info.
*/
//-----------------------------------------------------------------------------
void CBasePlayer::UpdateClientData(void)
{
	try
	{
	int myindex = entindex();
	if (g_ClientShouldInitizlize[myindex] > 0)// XDM3035a
	{
		m_iInitEntity = 0;// XDM3035c: include world
		m_fInitEntities = TRUE;// this should NEVER happen twice per map!
		g_ClientShouldInitizlize[myindex] = 0;
	}

	if (m_fInitHUD)
	{
		m_fInitHUD = FALSE;
		gInitHUD = FALSE;

		MESSAGE_BEGIN(MSG_ONE, gmsgResetHUD, NULL, ENT(pev));
		MESSAGE_END();

		if (!m_fGameHUDInitialized)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgInitHUD, NULL, ENT(pev));// XDM: this also clears client fog
			MESSAGE_END();

			g_pGameRules->InitHUD(this);

//			ALERT(at_aiconsole, "Client %d: EntsSendClientData()\n", entindex());
//			EntsSendClientData(this);// XDM: only when not m_fGameHUDInitialized
			m_iInitEntity = 1;
			m_fInitEntities = TRUE;
			m_flInitEntitiesTime = gpGlobals->time + 0.1f;// XDM: IMPORTANT!!! HL will crash if we send too much in one frame

			if (g_DisplayTitle)
				m_bDisplayTitle = TRUE;

//			if (g_pGameRules->IsMultiplayer())
//				FireTargets("game_playerjoin", this, this, USE_TOGGLE, 0);
			m_fGameHUDInitialized = TRUE;

			// ONCE per level!! Even in multiplayer!
/*			if (g_pWorld && g_pWorld->pev)
			{
				if ((gpGlobals->cdAudioTrack > 1) || !FStringNull(g_pWorld->pev->noise))
				{
					char string[32];
					if (CVAR_GET_FLOAT("mp3player") > 1)
					{
						if (!FStringNull(g_pWorld->pev->noise))
							sprintf(string, "bgm_play %s 0\n", STRING(g_pWorld->pev->noise));
						else
							sprintf(string, "bgm_play %d 0\n", gpGlobals->cdAudioTrack);
					}
					else
						sprintf(string, "cd mp3track %d\n", gpGlobals->cdAudioTrack);

	//				ALERT(at_aiconsole, "Playing map audio track: %d\n", gpGlobals->cdAudioTrack);
					CLIENT_COMMAND(edict(), string);
				}
			}*/
		}
		InitStatusBar();

		if (g_pWorld && g_pWorld->pev)// XDM: Fire this only once, after spawning!
		{
			if (m_flSndRoomtype != g_pWorld->pev->playerclass)// XDM
			{
				m_flSndRoomtype = g_pWorld->pev->playerclass;
				m_flSndRange = 0;
				m_pentSndLast = ENT(g_pWorld->pev);
				MESSAGE_BEGIN(MSG_ONE, SVC_ROOMTYPE, NULL, ENT(pev));
					WRITE_SHORT(m_flSndRoomtype);
				MESSAGE_END();
			}
		}
	}

	if (m_iHideHUD != m_iClientHideHUD)
	{
		if (g_pGameRules && g_pGameRules->FAllowFlashlight())
			m_iHideHUD &= ~HIDEHUD_FLASHLIGHT;
		else
			m_iHideHUD |= HIDEHUD_FLASHLIGHT;

		MESSAGE_BEGIN(MSG_ONE, gmsgHideWeapon, NULL, ENT(pev));
			WRITE_BYTE(m_iHideHUD);
		MESSAGE_END();
		m_iClientHideHUD = m_iHideHUD;
	}

	if (m_iFOV != m_iClientFOV)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgSetFOV, NULL, ENT(pev));
			WRITE_BYTE(m_iFOV);
		MESSAGE_END();
		// cache FOV change at end of function, so weapon updates can see that FOV has changed
	}

	// HACKHACK -- send the message to display the game title
	if (m_bDisplayTitle)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgShowGameTitle, NULL, ENT(pev));
			WRITE_BYTE(0);
		MESSAGE_END();
		m_bDisplayTitle = FALSE;
	}

	if (pev->health != m_iClientHealth)// update health
	{
		if (!IsBot())
		{
			int iHealth = max(pev->health, 0);// make sure that no negative health values are sent
			// send "health" update message

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_HEALTH);
			WRITE_SHORT(iHealth);
			MESSAGE_END();		
			
			m_iClientHealth = pev->health;
		}
	}

	if (pev->armorvalue != m_iClientBattery)// update armor
	{
		if (!IsBot())
		{
			m_iClientBattery = pev->armorvalue;
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE( MSG_ARMOR );
			WRITE_SHORT((int)pev->armorvalue);
			MESSAGE_END();

			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_SHIELD_STRENGTH);
			WRITE_SHORT(m_iItemShieldStrengthAug);
			MESSAGE_END();	
		}
	}

	if (m_rgItems[ITEM_TYPE_FLASHBANG] != m_iClientWpnIcon)// update flashbangs
	{
		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_WEAPON_POWER);
			WRITE_SHORT(m_iItemWeaponPowerAug);
			MESSAGE_END();	

			FX_WpnIcon(ENT(pev), m_rgItems[ITEM_TYPE_FLASHBANG], m_rgItems[ITEM_TYPE_TRIPMINE], m_rgItems[ITEM_TYPE_SPIDERMINE], m_rgItems[ITEM_TYPE_AIRSTRIKE], m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], m_rgItems[ITEM_TYPE_ENERGY_CUBE]);
			m_iClientWpnIcon = m_rgItems[ITEM_TYPE_FLASHBANG];
		}
	}

	}
	catch (...)
	{
		printf("*** CBasePlayer::UpdateClientData() exception 1!\n");
		DBG_FORCEBREAK
	}


	if (!IsObserver())// XDM
	{
	try
	{
		// update damage
		if (pev->dmg_take || pev->dmg_save || m_bitsHUDDamage != m_bitsDamageType)
		{
			// Comes from inside me if not set
			Vector damageOrigin = pev->origin;
			// send "damage" message
			// causes screen to flash, and pain compass to show direction of damage
			if (pev->dmg_inflictor)
			{
				CBaseEntity *pEntity = CBaseEntity::Instance(pev->dmg_inflictor);
				if (pEntity)
					damageOrigin = pEntity->Center();
			}

			// only send down damage type that have hud art
			int visibleDamageBits = m_bitsDamageType & (DMGM_SHOWNHUD | DMGM_VISIBLE);

			if (pev->takedamage != DAMAGE_NO && !IsBot())
			{
				if ( gpGlobals->time >= m_fHitFxTime)
				{
					MESSAGE_BEGIN(MSG_ONE, gmsgDamage, NULL, ENT(pev));
					WRITE_LONG(visibleDamageBits);
					WRITE_COORD(damageOrigin.x);
					WRITE_COORD(damageOrigin.y);
					WRITE_COORD(damageOrigin.z);
					// XDM: TODO: hitgroup
					MESSAGE_END();
				}
			}
			pev->dmg_take = 0;
			pev->dmg_save = 0;
			m_bitsHUDDamage = m_bitsDamageType;

			// Clear off non-time-based damage indicators
			m_bitsDamageType &= DMG_TIMEBASED;
		}
	}
	catch (...)
	{
		printf("*** CBasePlayer::UpdateClientData() exception 2a!\n");
		DBG_FORCEBREAK
	}

	try
	{
		if (m_iTrain & TRAIN_NEW)
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_TRAIN);
			WRITE_SHORT(m_iTrain & 0xF);
			MESSAGE_END();		

			m_iTrain &= ~TRAIN_NEW;
		}
	}
	catch (...)
	{
#ifdef	DEBUG
		FILE *fp=fopen("logs/XDMexcept.txt","a"); fprintf(fp,"*** CBasePlayer::UpdateClientData() exception 2b!\n"); fclose(fp);
#endif
		printf("*** CBasePlayer::UpdateClientData() exception 2b!\n");
//		DBG_FORCEBREAK
	}

	// XDM3035c: combined
	SendAmmoUpdate();

	// Send ALL the weapon info now
	SendWeaponsUpdate();

	}// !IsObserver()

	try
	{
	// XDM3035: cycle through all entities that need to SendClientData()
	// WARNING! This may cause datagram overflow, so don't send all data in one frame!
	// m_flInitEntitiesTime slows down data transmission
	// NOTE: this system does not consider PVS so all players recieve packets from all entities,
	// but this happens rarely and keeps data well synchronized.
/*	if (IsBot())// bots probably don't need this?
	{
		m_fInitEntities = FALSE;
	}
	else
	{*/
	if (m_fInitEntities && m_flInitEntitiesTime <= gpGlobals->time)
	{
		edict_t *pEdict = NULL;//INDEXENT(m_iInitEntityLast);
		do
		{
			pEdict = INDEXENT(m_iInitEntity);
			++m_iInitEntity;// we don't need this anywhere else, so increase immediately
			if (UTIL_IsValidEntity(pEdict))
			{
				CBaseEntity *pEntity = NULL;
				pEntity = CBaseEntity::Instance(pEdict);
				if (pEntity && !pEntity->IsPlayer())// XDM3035c: not players
				{
					if (pEntity->SendClientData(this, MSG_ONE) > 0)
					{
//						ALERT(at_debug, "SendClientData(%d) %s %s sent\n", entindex(), STRING(pEntity->pev->classname), STRING(pEntity->pev->targetname));
						break;// data was sent, exit and wait for next UpdateClientData()
					}
				}
			}
		} while (m_iInitEntity < gpGlobals->maxEntities);

		if (m_iInitEntity >= gpGlobals->maxEntities)
		{
			m_fInitEntities = FALSE;
			m_iInitEntity = 1;
		}
		m_flInitEntitiesTime = gpGlobals->time + 0.15f;
	}
//	}

	// Cache and client weapon change
	m_pClientActiveItem = m_pActiveItem;
	m_iClientFOV = m_iFOV;

	// Update Status Bar
	if (!IsBot() && g_pGameRules->IsMultiplayer())// XDM3035: SAVE CPU! Not for bots
	{
		if (m_flNextSBarUpdateTime <= gpGlobals->time)
		{
			UpdateStatusBar();
			m_flNextSBarUpdateTime = gpGlobals->time + 0.25;
		}
	}

	}
	catch (...)
	{
		printf("CBasePlayer::UpdateClientData() exception 3!\n");
		DBG_FORCEBREAK
	}
}

//-----------------------------------------------------------------------------
// FBecomeProne - Overridden for the player to set the proper
// physics flags when a barnacle grabs player.
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::FBecomeProne(void)
{
	m_afPhysicsFlags |= PFLAG_ONBARNACLE;
	TrainDetach();// XDM3035
	return TRUE;
}

//-----------------------------------------------------------------------------
// BarnacleVictimBitten - bad name for a function that is called
// by Barnacle victims when the barnacle pulls their head
// into its mouth. For the player, just die.
// Input  : *pBarnacle - 
//-----------------------------------------------------------------------------
void CBasePlayer::BarnacleVictimBitten(CBaseEntity *pBarnacle)
{
	if (!IsAlive())// Or the player will stay frozen and unable to respawn/load game!
	{
		Killed(pBarnacle, pBarnacle, GIB_ALWAYS);
		return;
	}

	if (FBitSet(pev->flags, FL_DUCKING))
		ClearBits(pev->flags, FL_DUCKING);

	TrainDetach();// XDM3035: Double check. Just in case.
	UTIL_ScreenFade(this, Vector(200,0,0), 1.0, 0.1, 160, FFADE_IN);
}

//-----------------------------------------------------------------------------
// Purpose: overridden for player who has physics flags concerns.
//-----------------------------------------------------------------------------
void CBasePlayer::BarnacleVictimReleased(void)
{
//	UTIL_ScreenFade(this, Vector(100,0,0), 1.0, 0.5, 80, FFADE_IN);// XDM
	m_afPhysicsFlags &= ~PFLAG_ONBARNACLE;
}

//-----------------------------------------------------------------------------
// Purpose: return player light level plus virtual muzzle flash
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayer::Illumination(void)
{
	int iIllum = CBaseMonster::Illumination() + m_iWeaponFlash;// XDM3035c: parent class
	if (iIllum > 255)
		return 255;
	return iIllum;
}

//-----------------------------------------------------------------------------
// Purpose: XDM3037: dynamic shooting spread based on various factors
// Output : float spread multiplier
//-----------------------------------------------------------------------------
float CBasePlayer::GetShootSpreadFactor(void)
{
	if (g_iSkillLevel == SKILL_EASY)
		return 1.0f;

	float k = 1.0f;
	float speed = pev->velocity.Length();
	if (FBitSet(pev->flags, FL_DUCKING))
	{
		if (speed == 0)
			k = 0.1f;
		else
			k = 0.5f;
	}
	else if (speed > PLAYER_MAX_WALK_SPEED)// running
	{
		k = 1.5f;
	}
	else if (speed > 0)// walking
	{
		k = 1.0f;
	}
	else// standing
	{
		k = 0.25f;
	}
	return k;
}

//-----------------------------------------------------------------------------
// Purpose: set crosshair position to point to enemey
// Input  : flDelta - 
// Output : Vector
//-----------------------------------------------------------------------------
Vector CBasePlayer::GetAutoaimVector(const float &flDelta)
{
	if (!m_pActiveItem || !m_pActiveItem->ForceAutoaim())// XDM: always check m_pActiveItem!!
	{
		UTIL_MakeVectors(pev->v_angle + pev->punchangle);
		return gpGlobals->v_forward;
	}

	Vector vecSrc = GetGunPosition();
	float flDist = g_psv_zmax->value;//8192;

	BOOL m_fOldTargeting = m_fOnTarget;
	Vector angles = AutoaimDeflection(vecSrc, flDist, flDelta);

	if (m_fOldTargeting != m_fOnTarget)
		m_pActiveItem->UpdateItemInfo();

	NormalizeAngle(&angles.x);// XDM3035c
	NormalizeAngle(&angles.y);
/*
	if (angles.x > 180)
		angles.x -= 360;
	if (angles.x < -180)
		angles.x += 360;
	if (angles.y > 180)
		angles.y -= 360;
	if (angles.y < -180)
		angles.y += 360;
*/
	if (angles.x > 25)
		angles.x = 25;
	if (angles.x < -25)
		angles.x = -25;
	if (angles.y > 12)
		angles.y = 12;
	if (angles.y < -12)
		angles.y = -12;

	m_vecAutoAim = angles * 0.9f;

	// Don't send across network if sv_aim is 0
//	if (m_pActiveItem->ForceAutoaim() /*|| g_psv_aim->value > 0 */)
//	if (m_vecAutoAim.x != m_lastx || m_vecAutoAim.y != m_lasty)
	if (m_vecAutoAim != m_vecAutoAimPrev)
	{
		SET_CROSSHAIRANGLE(ENT(pev), -m_vecAutoAim.x, m_vecAutoAim.y);
		m_vecAutoAimPrev = m_vecAutoAim;
//		m_lastx = m_vecAutoAim.x;
//		m_lasty = m_vecAutoAim.y;
	}
	// ALERT( at_console, "%f %f\n", angles.x, angles.y );
	UTIL_MakeVectors(pev->v_angle + pev->punchangle + m_vecAutoAim);
	return gpGlobals->v_forward;
}

//-----------------------------------------------------------------------------
// Purpose: main aiming code
// Input  : &vecSrc - 
//			flDist - 
//			flDelta - 
// Output : Vector
//-----------------------------------------------------------------------------
Vector CBasePlayer::AutoaimDeflection(const Vector &vecSrc, const float &flDist, const float &flDelta)
{
	CBaseEntity	*pEntity = NULL;
	CBaseEntity	*pBestent = NULL;
	edict_t		*pEdict = INDEXENT(1);
	float		bestdot = flDelta; // +- 10 degrees
	Vector		bestdir = gpGlobals->v_forward;
	TraceResult tr;

	UTIL_MakeVectors(pev->v_angle + pev->punchangle + m_vecAutoAim);

//	ALERT(at_console, "AutoaimDeflection\n");
	// try all possible entities
	m_fOnTarget = FALSE;

	UTIL_TraceLine(vecSrc, vecSrc + bestdir * flDist, dont_ignore_monsters, edict(), &tr);

	if (tr.pHit && tr.pHit->v.takedamage != DAMAGE_NO)
	{
		// don't look through water
		if (!((pev->waterlevel != 3 && tr.pHit->v.waterlevel == 3) || (pev->waterlevel == 3 && tr.pHit->v.waterlevel == 0)))
		{
			if (tr.pHit->v.takedamage == DAMAGE_AIM)
				m_fOnTarget = TRUE;

			return m_vecAutoAim;
		}
	}
	else// XDM: reset?
		m_hAutoaimTarget = NULL;

	// TODO: ENTITIES_IN_PVS()
	for (int i = 1; i < gpGlobals->maxEntities; ++i, ++pEdict)
	{
		if (!UTIL_IsValidEntity(pEdict))
			continue;
/*		if (pEdict->v.takedamage != DAMAGE_AIM)
			continue;
		if (pEdict == edict())
			continue;
*/
		pEntity = CBaseEntity::Instance(pEdict);
		if (pEntity == NULL)
			continue;

		if (!pEntity->IsAlive())
			continue;

		if (!g_pGameRules->ShouldAutoAim(this, pEntity))
			continue;

		// don't look through water
		if ((pev->waterlevel != 3 && pEntity->pev->waterlevel == 3) || (pev->waterlevel == 3 && pEntity->pev->waterlevel == 0))
			continue;

		Vector center = pEntity->BodyTarget(vecSrc);
		Vector dir = (center - vecSrc).Normalize();

		// make sure it's in front of the player
		if (DotProduct(dir, gpGlobals->v_forward) < 0)
			continue;

		float dot = fabs(DotProduct(dir, gpGlobals->v_right)) + fabs(DotProduct(dir, gpGlobals->v_up)) * 0.5f;

		// tweek for distance
		dot *= 1.0f + 0.2f * ((center - vecSrc).Length() / flDist);

		if (dot >= bestdot)
			continue;// too far to turn

		UTIL_TraceLine(vecSrc, center, dont_ignore_monsters, edict(), &tr);

		if (tr.flFraction != 1.0f && tr.pHit != pEdict)
			continue;// ALERT( at_console, "hit %s, can't see %s\n", STRING( tr.pHit->v.classname ), STRING( pEdict->v.classname ) );

		// don't shoot at friends
		if (IRelationship(pEntity) < R_NO)
			if (!pEntity->IsPlayer() && !g_pGameRules->IsMultiplayer())
				continue;// ALERT( at_console, "friend\n");

		// can shoot at this one
		bestdot = dot;
		bestdir = dir;
		pBestent = pEntity;
	}

	if (pBestent)
	{
		bestdir = UTIL_VecToAngles(bestdir);
		bestdir.x = -bestdir.x;
		bestdir = bestdir - pev->v_angle - pev->punchangle;

		if (pBestent->pev->takedamage == DAMAGE_AIM)
			m_fOnTarget = TRUE;

		m_hAutoaimTarget = pBestent;// XDM
		return bestdir;
	}
//	ALERT(at_console, "AutoaimDeflection: NO BEST ENT!\n");
	return g_vecZero;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::ResetAutoaim(void)
{
//	ALERT(at_console, "ResetAutoaim()\n");
	if (m_vecAutoAim.x != 0.0f || m_vecAutoAim.y != 0.0f)
	{
		m_vecAutoAim = g_vecZero;
		SET_CROSSHAIRANGLE(ENT(pev), 0, 0);
	}
	m_fOnTarget = FALSE;
	m_hAutoaimTarget = NULL;// XDM
}

//-----------------------------------------------------------------------------
// Purpose: UNDONE:  Determine real frame limit, 8 is a placeholder.
// Note: -1 means no custom frames present.
// Input  : nFrames - 
//-----------------------------------------------------------------------------
void CBasePlayer::SetCustomDecalFrames(int nFrames)
{
	if (nFrames > 0 && nFrames < 8)
		m_nCustomSprayFrames = nFrames;
	else
		m_nCustomSprayFrames = -1;
}

//-----------------------------------------------------------------------------
// Purpose: Returns the # of custom frames this player's custom clan logo contains.
// Output : int
//-----------------------------------------------------------------------------
int CBasePlayer::GetCustomDecalFrames(void)
{
	return m_nCustomSprayFrames;
}



//-----------------------------------------------------------------------------
// Purpose: Does the player already have EXACTLY THIS INSTANCE of item?
// Input  : *pCheckItem - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::HasPlayerItem(CBasePlayerItem *pCheckItem)
{
	if (pCheckItem == NULL)
		return FALSE;

	if (m_rgpWeapons[pCheckItem->GetID()] == pCheckItem)
		return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Safe way to access player's inventory
// Input  : &iID - 
// Output : CBasePlayerItem
//-----------------------------------------------------------------------------
CBasePlayerItem *CBasePlayer::GetInventoryItem(const int &iItemId)
{
	if (iItemId >= 0 && iItemId < PLAYER_INVENTORY_SIZE)
	{
		if (m_rgpWeapons[iItemId])
		{
			if (m_rgpWeapons[iItemId]->GetOwner() != this)
			{
				ALERT(at_console, "CBasePlayer(%d)::GetInventoryItem(%d)(ei %d id %d) bad item owner!\n", entindex(), iItemId, m_rgpWeapons[iItemId]->entindex(), m_rgpWeapons[iItemId]->GetID());
#if defined(_DEBUG_ITEMS)
				DBG_FORCEBREAK
#endif
				m_rgpWeapons[iItemId]->SetOwner(this);
			}

#ifdef _DEBUG
			if (m_rgpWeapons[iItemId]->pev && m_rgpWeapons[iItemId]->GetID() > WEAPON_NONE)
#endif
				return m_rgpWeapons[iItemId];
#if defined(_DEBUG) && defined(_DEBUG_ITEMS)
			else
				ALERT(at_console, "CBasePlayer(%d)::GetInventoryItem(%d)(ei %d id %d) got bad item!\n", entindex(), iItemId, m_rgpWeapons[iItemId]->entindex(), m_rgpWeapons[iItemId]->GetID());
#endif
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: DeployActiveItem
//-----------------------------------------------------------------------------
void CBasePlayer::DeployActiveItem(void)// XDM
{
	if (m_pActiveItem)
	{
#ifdef _DEBUG
		if (m_pActiveItem->GetOwner() != this)
		{
			ALERT(at_console, "CBasePlayer(%d)::DeployActiveItem(ei %d id %d) bad item owner!\n", entindex(), m_pActiveItem->entindex(), m_pActiveItem->GetID());
			DBG_FORCEBREAK
		}
#endif
//		m_pActiveItem->SetOwner(this);// WTF? safety
		m_pActiveItem->Deploy();
		m_pActiveItem->UpdateItemInfo();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Used by SelectItem to delay weapon selection
// Input  : *pItem - 
//-----------------------------------------------------------------------------
void CBasePlayer::QueueItem(CBasePlayerItem *pItem)// XDM
{
	if (pItem == NULL || pItem->GetID() == WEAPON_NONE)// XDM3035: wtf?
		return;

#ifdef _DEBUG
	if (pItem->GetOwner() != this)
	{
		ALERT(at_console, "CBasePlayer(%d)::QueueItem(ei %d id %d) bad item owner!\n", entindex(), pItem->entindex(), pItem->GetID());
		DBG_FORCEBREAK
	}
#endif
	if (m_pActiveItem == NULL)// no active weapon
	{
		m_pActiveItem = pItem;
//		return;// just set this item as active
	}
    else// remember active item
	{
		m_pLastItem = m_pActiveItem;
		m_pActiveItem = NULL;// clear current
		m_pNextItem = pItem;// add item to queue
	}
}

//-----------------------------------------------------------------------------
// Purpose: Normal method to switch to specified item
// Input  : *pWeapon - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
/*BOOL CBasePlayer::SwitchWeapon(CBasePlayerItem *pWeapon)
{
	if (pWeapon == NULL)// XDM3035
		return FALSE;
	if (pWeapon->GetID() == WEAPON_NONE)// XDM3035
	{
//		DBG_FORCEBREAK;
		return FALSE;
	}

	if (!pWeapon->CanDeploy())
		return FALSE;

	ResetAutoaim();

	if (m_pActiveItem)
		m_pActiveItem->Holster();

	QueueItem(pWeapon);// XDM
	DeployActiveItem();// XDM: QueueItem() sets m_pActiveItem if we have no current weapon
	return TRUE;
}*/

//-----------------------------------------------------------------------------
// Purpose: Normal method to switch to specified item
// Input  : *pItem - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::SelectItem(CBasePlayerItem *pItem)
{
	if (pItem == NULL)
		return FALSE;
	if (pItem->GetID() == WEAPON_NONE)
	{
//		DBG_FORCEBREAK;
		return FALSE;
	}
#ifdef _DEBUG
	if (pItem->GetOwner() != this)
	{
		ALERT(at_console, "CBasePlayer(%d)::SelectItem(ei %d id %d) bad item owner!\n", entindex(), pItem->entindex(), pItem->GetID());
		DBG_FORCEBREAK
	}
#endif
	if (!pItem->CanDeploy())// XDM: TESTME!!
		return FALSE;

	if (m_pActiveItem)
	{
		m_pActiveItem->SetOwner(this);// HACKFIX

		if (pItem == m_pActiveItem)
			return TRUE;

		if (!m_pActiveItem->CanHolster())// XDM
			return FALSE;
	}

	ResetAutoaim();

	if (m_pActiveItem)
		m_pActiveItem->Holster();

	QueueItem(pItem);// XDM
	DeployActiveItem();// XDM: QueueItem() sets m_pActiveItem if we have no current weapon
	return TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: Select item by ID
// Input  : iID - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::SelectItem(const int &iID)
{
	if (iID < 0 || iID >= PLAYER_INVENTORY_SIZE)
		return FALSE;

	if (GetInventoryItem(iID))
		return SelectItem(m_rgpWeapons[iID]);

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Select item by name
// Input  : *pstr - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::SelectItem(const char *pstr)
{
	if (pstr == NULL)
		return FALSE;

	CBasePlayerItem *pItem = NULL;
	ItemInfo II;
	for (int i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
	{
		pItem = GetInventoryItem(i);
		if (pItem)
		{
			if (pItem->GetItemInfo(&II))// XDM3035c: use item name, not class name!
			{
				if (strcmp(II.pszName, pstr) == 0)
				{
					return SelectItem(pItem);
					break;
				}
			}
		}
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: Select Next Best Item
// TODO: move to client, every player must be able to define personal priority list
// Input  : *pItem - 
// Output : CBasePlayerItem
//-----------------------------------------------------------------------------
CBasePlayerItem *CBasePlayer::SelectNextBestItem(CBasePlayerItem *pItem)
{
	if (!m_pActiveItem->CanHolster())
		return NULL;// can't put this gun away right now, so can't switch.

#ifdef CLIENT_ITEM_SETTINGS// TODO: use client-side logic, wait for special command
	MESSAGE_BEGIN(MSG_ONE, gmsgSelBestItem, NULL, ENT(pev));
		WRITE_BYTE(pItem?pItem->GetID():WEAPON_NONE);
	MESSAGE_END();
	return pItem;//NULL;?
#else
//	if (pItem == NULL)
	CBasePlayerItem *pBestItem = g_pGameRules->GetNextBestWeapon(this, pItem);
	if (pBestItem && pBestItem != pItem)
		SelectItem(pBestItem);
#endif
	return pBestItem;
}

//-----------------------------------------------------------------------------
// Purpose: Select previously selected item
//-----------------------------------------------------------------------------
void CBasePlayer::SelectLastItem(void)
{
	if (m_pLastItem == NULL || !m_pLastItem->CanDeploy())// XDM
		return;

	if (m_pActiveItem && !m_pActiveItem->CanHolster())
		return;

	if (m_pActiveItem == m_pLastItem)
		return;

	ResetAutoaim();

	if (m_pActiveItem)
		m_pActiveItem->Holster();

	QueueItem(m_pLastItem);// XDM
	DeployActiveItem();// XDM
}

//-----------------------------------------------------------------------------
// Purpose: HasWeapons - do I have any weapons at all?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::HasWeapons(void)
{
	for (int i = 0; i < PLAYER_INVENTORY_SIZE; ++i)
	{
		if (GetInventoryItem(i))
			return TRUE;
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: GiveNamedItem
// XDM3035: modified to return a pointer
// NEW: SF_NOTREAL protects the game from being spammed with items added by game rules
// Input  : *pszName - must reside in memory! Temporary variables are not allowed!
// Output : CBaseEntity
//-----------------------------------------------------------------------------
CBaseEntity *CBasePlayer::GiveNamedItem(const char *pszName)
{
	CBaseEntity *pEnt = CBaseEntity::Create(pszName, pev->origin, pev->angles, NULL/* edict()*/);
	if (pEnt)
	{
		pEnt->pev->spawnflags |= (SF_NOTREAL | SF_NORESPAWN);
		if (gEvilImpulse101)// XDM
			pEnt->pev->effects |= EF_NODRAW;

		pEnt->Touch(this);
	}
	return pEnt;
}


//-----------------------------------------------------------------------------
// Purpose: freeze grenade
// Input  : freezetime - 
//-----------------------------------------------------------------------------
void CBasePlayer::FrozenStart(float freezetime)
{
	if (!IsAlive())
		return;

	if (pev->armorvalue)
		return;

#ifndef CLIENT_DLL
	if (!FBitSet(pev->flags, FL_FROZEN))
		CFrozenCube::CreateNew(this, pev->origin, freezetime);
#endif

	pev->movetype = MOVETYPE_TOSS;
//	m_afPhysicsFlags |= PFLAG_ONBARNACLE;
	pev->fixangle = 1;
	pev->flags |= FL_FROZEN;

	if (m_iItemLightningField)
		m_iItemLightningField = 1;

	CBaseMonster::FrozenStart(freezetime);// this sets rendercolor

//	if (!m_fFrozen)// UNDONE: if already frozen
		UTIL_ScreenFade(this, pev->rendercolor, 1.0, 1.0, 127, FFADE_OUT | FFADE_STAYOUT);

	m_flNextAttack = UTIL_WeaponTimeBase() + freezetime;// override for player
	if (m_pActiveItem)// XDM: crash prevention
	{
		CBasePlayerWeapon *pWeapon = m_pActiveItem->GetWeaponPtr();
		if (pWeapon)
		{
			ASSERT(pWeapon->GetOwner() != NULL);
			pWeapon->m_flNextPrimaryAttack = pWeapon->m_flNextSecondaryAttack = m_flNextAttack;
			pWeapon->m_flTimeWeaponIdle = UTIL_WeaponTimeBase();
			pWeapon->WeaponIdle();// Force TESTME!!
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::FrozenEnd(void)
{
	pev->movetype = MOVETYPE_WALK;
//	m_afPhysicsFlags &= ~PFLAG_ONBARNACLE;
	pev->fixangle = FALSE;
	UTIL_ScreenFade(this, pev->rendercolor, 4, 0, 128, FFADE_IN);
	CBaseMonster::FrozenEnd();
	m_flNextAttack = UTIL_WeaponTimeBase();
	pev->flags &= ~FL_FROZEN;

	if (m_pActiveItem)// XDM: crash prevention
	{
		CBasePlayerWeapon *pWeapon = m_pActiveItem->GetWeaponPtr();
		if (pWeapon != NULL)
			pWeapon->m_flNextPrimaryAttack = pWeapon->m_flNextSecondaryAttack = m_flNextAttack;
	}
//restore effects for quaddamage and(or) invisibility
	if (m_iItemInvisibility)
	{
		pev->renderfx = kRenderFxGlowShell;
		pev->rendermode = kRenderTransColor;
		pev->renderamt = INVISIBILITY_RENDERAMT;
		pev->rendercolor = Vector(1,1,1);
	}
	if (m_iItemQuadDamage)
	{
		pev->renderfx = kRenderFxGlowShell;
		pev->rendercolor = Vector(200,0,0);
		if (m_iItemInvisibility)
			pev->renderamt = INVISIBILITY_RENDERAMT;
		else
			pev->renderamt = 15;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CBasePlayer::FrozenThink(void)
{
	if (!m_fFrozen)
		return;

	if (m_flUnfreezeTime > 0.0f)
	{
		if (m_flUnfreezeTime <= gpGlobals->time)
			FrozenEnd();// call this before CBaseMonster
		else
			CBaseMonster::FrozenThink();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : damage - 
// Output : float
//-----------------------------------------------------------------------------
float CBasePlayer::DamageForce(const float &damage)
{
	return 3.0f*CBaseEntity::DamageForce(damage);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : fControl - 
//-----------------------------------------------------------------------------
void CBasePlayer::EnableControl(bool fControl)
{
	if (!fControl)
		pev->flags |= FL_FROZEN;
	else
		pev->flags &= ~FL_FROZEN;
}


//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns TRUE if the player is attached to a ladder
//-----------------------------------------------------------------------------
BOOL CBasePlayer::IsOnLadder(void)
{
	return (pev->movetype == MOVETYPE_FLY);
}

//-----------------------------------------------------------------------------
// Purpose: disable use of ladders (just fall)
// Input  : time - for this amount of time
//-----------------------------------------------------------------------------
void CBasePlayer::DisableLadder(const float &time)
{
	pev->movetype = MOVETYPE_WALK;
	ENGINE_SETPHYSKV(edict(), PHYSKEY_IGNORELADDER, "1");
	m_flIgnoreLadderStopTime = gpGlobals->time + time;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::IsOnTrain(void)// XDM3035
{
	if (m_afPhysicsFlags & PFLAG_ONTRAIN)
		return TRUE;
//	if (m_pTrain)// OR or AND?
//		return TRUE;
//	if (m_iTrain & TRAIN_ACTIVE)
//		return TRUE;
// THIS DESTROYS LOGIC!	if (pev->flags & FL_ONTRAIN)
//		return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: attach player to a train controls
// Input  : *pTrain - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::TrainAttach(CBaseEntity *pTrain)
{
	if (pTrain == NULL)
		return FALSE;

	CBaseDelay *pRealTrain = (CBaseDelay *)pTrain;// XDM3035: HACKHACKHACK!!!!!!

	if (pRealTrain->IsLockedByMaster())// XDM3037: disabled train
		return FALSE;

	if ((pTrain->pev->euser1 != NULL) || (pRealTrain->m_hActivator != NULL))
	{
		if (g_pGameRules->IsMultiplayer())
		{
			ClientPrint(pev, HUD_PRINTCENTER, "#HIJACK");
//			ALERT(at_console, "HIJACK!\n");
		}
//		ALERT(at_console, "Train is occupied\n");
//		return FALSE;
	}

	if (IsOnTrain())// already using
	{
//		if (pTrain->pev != m_pTrain->pev)// ANOTHER train
//			if (!TrainDetach())// which I failed to detach from
				return FALSE;// so don't bother
	}

	m_afPhysicsFlags |= PFLAG_ONTRAIN;
	m_iTrain = TrainSpeed((int)pTrain->pev->speed, pTrain->pev->impulse);
	m_iTrain |= TRAIN_NEW;
	pTrain->pev->euser1 = edict();// become attacker in multiplayer
	pRealTrain->m_hActivator = this;// XDM3035: HACKHACKHACK!!!!!!
	m_pTrain = pTrain;
//	pev->flags |= FL_ONTRAIN;
#ifdef _DEBUG
	ALERT(at_aiconsole, "TrainAttach() successful\n");
#endif
	return TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: detach player from train controls
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::TrainDetach(void)
{
	if (m_pTrain)
	{
		CBaseDelay *pRealTrain = GetControlledTrain();// get this FIRST, while it is valid!
		if (pRealTrain)// May get NULL!
		{
			if (pRealTrain->m_hActivator == this)// new
				pRealTrain->m_hActivator = NULL;
		}
		if (m_pTrain->pev->euser1 == edict())
			m_pTrain->pev->euser1 = NULL;

		m_pTrain = NULL;
	}
	m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
	m_iTrain = TRAIN_NEW|TRAIN_OFF;
//	ALERT(at_aiconsole, "TrainDetach() successful\n");
	return TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: For external use
// Output : CBaseDelay
//-----------------------------------------------------------------------------
CBaseDelay *CBasePlayer::GetControlledTrain(void)
{
	if (m_pTrain)
	{
		if (m_pTrain->pev->euser1 == edict())// do we need to check this here?
		{
			return (CBaseDelay *)(CBaseEntity *)m_pTrain;// XDM3035: HACKHACKHACK!!!!!!
		}
	}
	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: PreThink OnTrain part
//-----------------------------------------------------------------------------
void CBasePlayer::TrainPreFrame(void)
{
	// Train speed control
	if (IsOnTrain())//m_afPhysicsFlags & PFLAG_ONTRAIN)
	{
	// So the correct flags get sent to client asap.
		pev->flags |= FL_ONTRAIN;
// XDM3035		CBaseEntity *pTrain = CBaseEntity::Instance( pev->groundentity );
		CBaseEntity *pTrain = (CBaseEntity *)m_pTrain;// XDM3035b
		if (pTrain == NULL)
		{
			ALERT(at_aiconsole, "Client %d: Acquiring train from ground entity.\n", entindex());
			pTrain = CBaseEntity::Instance(pev->groundentity);
			TrainAttach(pTrain);// somehow it works
		}

		if (pTrain == NULL)
		{
			TraceResult trainTrace;
			// Maybe this is on the other side of a level transition
			UTIL_TraceLine( pev->origin, pev->origin + Vector(0,0,-38), ignore_monsters, ENT(pev), &trainTrace);

			// HACKHACK - Just look for the func_tracktrain classname
			if (trainTrace.flFraction != 1.0 && trainTrace.pHit)
				pTrain = CBaseEntity::Instance(trainTrace.pHit);

			if (pTrain == NULL || !(pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) || !pTrain->OnControls(pev))
			{
				ALERT(at_aiconsole, "Client %d: In train mode with no train!\n", entindex());
// XDM3035				m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
// XDM3035				m_iTrain = TRAIN_NEW|TRAIN_OFF;
				TrainDetach();// XDM3035b
//FIXME not return!				return;
			}
		}
		else if (!FBitSet(pev->flags, FL_ONGROUND) || FBitSet(pTrain->pev->spawnflags, SF_TRACKTRAIN_NOCONTROL) || (pev->button & (IN_MOVELEFT|IN_MOVERIGHT)))
		{
			// Turn off the train if you jump, strafe, or the train controls go dead
// XDM3035			m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
// XDM3035			m_iTrain = TRAIN_NEW|TRAIN_OFF;
			TrainDetach();// XDM3035b
//FIXME not return!			return;
		}

//		if (IsOnTrain())// still on train?
		if ((m_afPhysicsFlags & PFLAG_ONTRAIN) && pTrain)// XDM3035: removed returns;
		{
			float vel = 0.0f;
			pev->velocity = g_vecZero;
			if ( m_afButtonPressed & IN_FORWARD )
			{
				vel = 1.0f;
				pTrain->Use(this, this, USE_SET, vel);
			}
			else if ( m_afButtonPressed & IN_BACK )
			{
				vel = -1.0f;
				pTrain->Use(this, this, USE_SET, vel);
			}

			if (vel != 0.0f)
			{
				m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
				m_iTrain |= TRAIN_ACTIVE|TRAIN_NEW;
			}
		}
	}
	else
	{
		pev->flags &= ~FL_ONTRAIN;
		if (m_iTrain & TRAIN_ACTIVE)
			m_iTrain = TRAIN_NEW; // turn off train
	}
}

//-----------------------------------------------------------------------------
// Purpose: TODO: UNDONE! A client that is allowed to enter administrative commands
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
BOOL CBasePlayer::IsAdministrator(void)
{
	if (IS_DEDICATED_SERVER())
		return FALSE;

	if (entindex() == 1)
		return TRUE;

	return FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: CoOp: player reached and touched some checkpoint (trigger_autosave)
// Input  : *pCheckPoint - 
//-----------------------------------------------------------------------------
void CBasePlayer::OnCheckPoint(CBaseEntity *pCheckPoint)
{
	ASSERT(pCheckPoint != NULL);
//	if (m_iLastCheckPointIndex != pCheckPoint->entindex())
	if (!PassedCheckPoint(pCheckPoint))
	{
//		m_iLastCheckPointIndex = pCheckPoint->entindex();
		int i;
		for (i=0; i<MAX_CHECKPOINTS; ++i)
		{
			if (m_iszCheckPoints[i] == 0)// empty slot
			{
				m_iszCheckPoints[i] = pCheckPoint->entindex();
				ClientPrint(pev, HUD_PRINTCENTER, "#CHECKPOINT\n", UTIL_dtos1(m_iszCheckPoints[i]), STRING(pCheckPoint->pev->targetname));
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: CoOp: has this player touched this checkpoint? (trigger_autosave)
// Input  : *pCheckPoint - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CBasePlayer::PassedCheckPoint(CBaseEntity *pCheckPoint)
{
	ASSERT(pCheckPoint != NULL);
	for (int i=0; i<MAX_CHECKPOINTS; ++i)// m_iszCheckPoints MUST BE CLEARED OUT before using this!
	{
		if (m_iszCheckPoints[i] != 0 && m_iszCheckPoints[i] == pCheckPoint->entindex())
			return true;
	}
	return false;
}


//-----------------------------------------------------------------------------
// Purpose: power up stuff
//-----------------------------------------------------------------------------
void CBasePlayer::PowerUpThink(void)
{	
	//quad damage
	if (m_iItemQuadDamage > 0 && gpGlobals->time >= m_fTimeQuadUpdate)
	{
		m_iItemQuadDamage--;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_QUADDAMAGE);
			WRITE_SHORT(m_iItemQuadDamage);
			MESSAGE_END();			
		}

		m_fTimeQuadUpdate = gpGlobals->time + 1.0f;

		if (m_iItemQuadDamage <=0)
		{
			pev->renderamt = 0;			
			pev->renderfx = 0;
			pev->rendercolor = g_vecZero;
			pev->rendermode = kRenderNormal;
			m_iItemQuadDamage = 0;
			ClientPrint(pev, HUD_PRINTTALK, "QUAD DAMAGE FINISHED!");
			ENGINE_SETPHYSKV(edict(), PHYSKEY_QUADDAMAGE, "0");

			if (m_iItemInvisibility)
			{
				pev->renderfx = kRenderFxGlowShell;
				pev->rendermode = kRenderTransColor;
				pev->renderamt = INVISIBILITY_RENDERAMT;
				pev->rendercolor = Vector(1,1,1);
			}
		}
	}

	//invulnerability
	if (m_iItemInvulnerability > 0 && gpGlobals->time >= m_fTimeInvulUpdate)
	{
		m_iItemInvulnerability--;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_INVULNERABILITY);
			WRITE_SHORT(m_iItemInvulnerability);
			MESSAGE_END();			
		}

		m_fTimeInvulUpdate = gpGlobals->time + 1.0f;

		if (m_iItemInvulnerability <=0)
		{
			if (pev->armorvalue > m_iOldShieldAmount)
				pev->armorvalue = m_iOldShieldAmount; 

			m_iItemInvulnerability = 0;
			m_iOldShieldAmount = 0;
			ENGINE_SETPHYSKV(edict(), PHYSKEY_INVULNERABILITY, "0");
			ClientPrint(pev, HUD_PRINTTALK, "INVULNERABILITY FINISHED!");
		}
	}

	//invisibility
	if (m_iItemInvisibility > 0 && gpGlobals->time >= m_fTimeInvisUpdate)
	{
		m_iItemInvisibility--;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_INVISIBILITY);
			WRITE_SHORT(m_iItemInvisibility);
			MESSAGE_END();			
		}

		m_fTimeInvisUpdate = gpGlobals->time + 1.0f;

		if (m_iItemInvisibility <=0)
		{
			pev->renderamt = 0;
			pev->renderfx = kRenderFxNone;
			pev->rendercolor = g_vecZero;
			pev->rendermode = kRenderNormal;
			m_iItemInvisibility = 0;
			pev->flags	&= ~FL_NOTARGET;
			ENGINE_SETPHYSKV(edict(), PHYSKEY_INVISIBILITY, "0");
			ClientPrint(pev, HUD_PRINTTALK, "INVISIBILITY FINISHED!");

			if (m_iItemQuadDamage)
			{
				pev->renderfx = kRenderFxGlowShell;
				pev->rendercolor = Vector(200, 0, 0);
				pev->renderamt = 15;
			}
		}
	}
	//rapid fire
	if (m_iItemRapidFire > 0 && gpGlobals->time >= m_fTimeRapidUpdate)
	{
		m_iItemRapidFire--;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_RAPIDFIRE);
			WRITE_SHORT(m_iItemRapidFire);
			MESSAGE_END();			
		}

		m_fTimeRapidUpdate = gpGlobals->time + 1.0f;

		if (m_iItemRapidFire <=0)
		{
			m_iItemRapidFire = 0;
			ClientPrint(pev, HUD_PRINTTALK, "RAPID FIRE FINISHED!");
			ENGINE_SETPHYSKV(edict(), PHYSKEY_RAPIDFIRE, "0");
		}
	}
	//haste
	if (m_iItemHaste > 0 && gpGlobals->time >= m_fTimeHasteUpdate)
	{
		m_iItemHaste--;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_HASTE);
			WRITE_SHORT(m_iItemHaste);
			MESSAGE_END();			
		}

		m_fTimeHasteUpdate = gpGlobals->time + 1.0f;

		if (m_iItemHaste <=0)
		{
			m_iItemHaste = 0;
			ENGINE_SETPHYSKV(edict(), PHYSKEY_HASTE, "0");
			ClientPrint(pev, HUD_PRINTTALK, "HASTE FINISHED!");
		}
	}
	//banana mode
	if (m_iItemBanana > 0 && gpGlobals->time >= m_fTimeBananaUpdate)
	{
		m_iItemBanana--;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_BANANA);
			WRITE_SHORT(m_iItemBanana);
			MESSAGE_END();			
		}

		m_fTimeBananaUpdate = gpGlobals->time + 1.0f;

		if (!m_fFrozen && m_iGenerator && m_iFlashBattery > (POWER_MAX_CHARGE/4))
		{
			float MaxHealth = MAX_PLAYER_HEALTH + (m_iItemHealthAug*EXTRA_HEALTH_HP_AUG);
			pev->health += m_iGeneratorPower/5;
				
			if (pev->health > MaxHealth)
				pev->health = MaxHealth;
		}

		if (m_iItemBanana <=0)
		{
			m_iItemBanana = 0;
			ClientPrint(pev, HUD_PRINTTALK, "BANANA MODE FINISHED!");
			ENGINE_SETPHYSKV(edict(), PHYSKEY_BANANA, "0");

			if (m_pActiveItem)
				m_pActiveItem->Deploy();
		}
	}

	//shield recharger
	if (m_iItemShieldRegen > 0 && gpGlobals->time >= m_flShieldRegenUpdate)
	{
		m_iItemShieldRegen--;

		if (!m_fFrozen && m_iShield && m_iGenerator && m_iFlashBattery > (POWER_MAX_CHARGE/4) && m_iFlashBattery > m_iPowerUseShield)
		{
			float MaxShield = (m_iItemInvulnerability)?MAX_INVULNER_BATTERY:MAX_NORMAL_BATTERY  + (m_iItemShieldAug*EXTRA_BATTERY_SHIELD_AUG);

			if (pev->armorvalue < MaxShield)
			{
				m_iFlashBattery -= m_iPowerUseShield;
				pev->armorvalue += m_iGeneratorPower/5;
			}
			if (pev->armorvalue > MaxShield && !m_iItemInvulnerability)
				pev->armorvalue = MaxShield;

			if (!IsBot())
			{
				MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
				WRITE_BYTE(MSG_ITEM_SHIELDREGEN);
				WRITE_SHORT(m_iItemShieldRegen);
				MESSAGE_END();			
			}
		}
		m_flShieldRegenUpdate = gpGlobals->time + 1.0f;

		if (m_iItemShieldRegen <=0)
		{
			m_iItemShieldRegen = 0;
			ClientPrint(pev, HUD_PRINTTALK, "SHIELD REGENERATION FINISHED!");
			ENGINE_SETPHYSKV(edict(), PHYSKEY_SHIELD_REGEN, "0");
		}
	}
		//accuracy modifyer
	if (m_iItemAccuracy > 0 && gpGlobals->time >= m_fTimeAccuracyUpdate)
	{
		m_iItemAccuracy--;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_ACCURACY);
			WRITE_SHORT(m_iItemAccuracy);
			MESSAGE_END();			
		}
		m_fTimeAccuracyUpdate = gpGlobals->time + 1.0f;

		if (m_iItemAccuracy <=0)
		{
			m_iItemAccuracy = 0;
			ENGINE_SETPHYSKV(edict(), PHYSKEY_ACCURACY, "0");
			ClientPrint(pev, HUD_PRINTTALK, "SUPER ACCURACY FINISHED!");
		}
	}

	//radiation shield
	if (m_iItemRadShield > 0 && gpGlobals->time >= m_iTimeRadShieldUpdate)
	{
		m_iItemRadShield--;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_RADSHIELD);
			WRITE_SHORT(m_iItemRadShield);
			MESSAGE_END();			
		}
		m_iTimeRadShieldUpdate = gpGlobals->time + 1.0f;

		if (m_iItemRadShield <=0)
		{
			m_iItemRadShield = 0;
			ENGINE_SETPHYSKV(edict(), PHYSKEY_RADSHIELD, "0");
			ClientPrint(pev, HUD_PRINTTALK, "RADIATION SHIELD FINISHED!");
		}
	}

	//plasma shield
	if (m_iItemPlasmaShield > 0 && gpGlobals->time >= m_iTimePlasmaShieldUpdate)
	{
		m_iItemPlasmaShield--;

		float MaxShield = MAX_NORMAL_BATTERY + (m_iItemShieldAug*EXTRA_BATTERY_SHIELD_AUG);

		if (pev->armorvalue < MIN_PLASMA_SHIELD_AMOUNT) 
		{
			pev->armorvalue = MIN_PLASMA_SHIELD_AMOUNT;
		}
		if (pev->armorvalue >= MIN_PLASMA_SHIELD_AMOUNT && pev->armorvalue < MaxShield)
		{
			pev->armorvalue += m_iGeneratorPower/10;

			if (pev->armorvalue > MaxShield)
				pev->armorvalue = MaxShield;
		}

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_PLASMA_SHIELD);
			WRITE_SHORT(m_iItemPlasmaShield);
			MESSAGE_END();			
		}
		m_iTimePlasmaShieldUpdate = gpGlobals->time + 1.0f;

		if (m_iItemPlasmaShield <=0)
		{
			m_iItemPlasmaShield = 0;
			ENGINE_SETPHYSKV(edict(), PHYSKEY_PLASMA_SHIELD, "0");
			ClientPrint(pev, HUD_PRINTTALK, "PLASMA SHIELD FINISHED!");
		}
	}

	//Lightning Field 
	if (m_iItemLightningField > 0 && gpGlobals->time >= m_fTimeLightningFieldUpdate)
	{
		if (pev->waterlevel > 0)
		{
			m_iItemLightningField = 0;
			ENGINE_SETPHYSKV(edict(), PHYSKEY_LIGHTNING_FIELD, "0");
			::RadiusDamage(pev->origin, this, this, gSkillData.DmgPlrLightningField*10, 1024, CLASS_NONE, DMG_SHOCK | DMG_RADIUS_MAX);

			switch (RANDOM_LONG(0,2))
			{
				case 0:	EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/electro4.wav", 1, ATTN_NORM); break;
				case 1:	EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/electro5.wav", 1, ATTN_NORM); break;
				case 2:	EMIT_SOUND(ENT(pev), CHAN_ITEM, "weapons/electro6.wav", 1, ATTN_NORM); break;
			}
		}
		else
			m_iItemLightningField--;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_LIGHTNING_FIELD);
			WRITE_SHORT(m_iItemLightningField);
			MESSAGE_END();			
		}
		m_fTimeLightningFieldUpdate = gpGlobals->time + 1.0f;

		if (m_iItemLightningField <=0)
		{
			ENGINE_SETPHYSKV(edict(), PHYSKEY_LIGHTNING_FIELD, "0");
			m_iItemLightningField = 0;
			ClientPrint(pev, HUD_PRINTTALK, "LIGHTNING FIELD DEPLETED!");
		}
	}
		//antidote 
	if (m_iItemAntidote > 0 && gpGlobals->time >= m_fTimeAntidoteUpdate)
	{
		m_iItemAntidote--;

		float MaxHealth = MAX_PLAYER_HEALTH + (m_iItemHealthAug*EXTRA_HEALTH_HP_AUG);
		pev->health++;
			
		if (pev->health > MaxHealth)
				pev->health = MaxHealth;

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_ANTIDOTE);
			WRITE_SHORT(m_iItemAntidote);
			MESSAGE_END();			
		}
		m_fTimeAntidoteUpdate = gpGlobals->time + 1.0f;

		if (m_iItemAntidote <=0)
		{
			m_iItemAntidote = 0;
			ENGINE_SETPHYSKV(edict(), PHYSKEY_ANTIDOTE, "0");
			ClientPrint(pev, HUD_PRINTTALK, "ANTIDOTE FINISHED!");
		}
	}
	//fire supressor 
	if (m_iItemFireSupressor > 0 && gpGlobals->time >= m_fTimeFireSupressorUpdate)
	{
		m_iItemFireSupressor--;
		FX_Trail(pev->origin, entindex(), FX_FIRE_SUPRESSOR_BUBBLES);

		if (!IsBot())
		{
			MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(pev));
			WRITE_BYTE(MSG_ITEM_FIRE_SUPRESSOR);
			WRITE_SHORT(m_iItemFireSupressor);
			MESSAGE_END();			
		}
		m_fTimeFireSupressorUpdate = gpGlobals->time + 1.0f;

		if (m_iItemFireSupressor <=0)
		{
			m_iItemFireSupressor = 0;
			ENGINE_SETPHYSKV(edict(), PHYSKEY_FIRE_SUPRESSOR, "0");
			ClientPrint(pev, HUD_PRINTTALK, "FIRE SUPRESSOR FINISHED!");
		}
	}
}

BOOL CBasePlayer::IsPushable(void)
{
	if (m_iItemInvulnerability)
		return FALSE;

	return TRUE;
}

BOOL CBasePlayer::PlaceMine(void)
{
	if (m_rgItems[ITEM_TYPE_TRIPMINE] <= 0 || (gpGlobals->time <= m_fTripmineTime) )
		return false;

	UTIL_MakeVectors(pev->v_angle + pev->punchangle);
	Vector vecSrc = GetGunPosition();
	Vector vecAiming = gpGlobals->v_forward;
	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecSrc + vecAiming * 128.0f, dont_ignore_monsters, ENT(pev), &tr);
	bool success = false;
	if (tr.flFraction < 1.0f)
	{
		CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
		if (pEntity && !(pEntity->pev->flags & FL_CONVEYOR))
		{
			Vector angles = UTIL_VecToAngles(tr.vecPlaneNormal);
			CBaseEntity *pMine = Create("monster_tripmine", tr.vecEndPos + tr.vecPlaneNormal * 6, angles, g_vecZero, ENT(pev));
			if (pMine)
			{
				pMine->pev->team = pev->team;
				pMine->pev->spawnflags |= SF_NORESPAWN;
				pMine->pev->dmg = gSkillData.DmgTripMine + (gSkillData.DmgTripMine * (m_iItemWeaponPowerAug*0.1));

				m_rgItems[ITEM_TYPE_TRIPMINE]--;
				m_fTripmineTime = gpGlobals->time + 2.5;

				if (!IsBot())
					FX_WpnIcon(ENT(pev), m_rgItems[ITEM_TYPE_FLASHBANG], m_rgItems[ITEM_TYPE_TRIPMINE], m_rgItems[ITEM_TYPE_SPIDERMINE], m_rgItems[ITEM_TYPE_AIRSTRIKE], m_rgItems[ITEM_TYPE_SATELLITE_STRIKE], m_rgItems[ITEM_TYPE_ENERGY_CUBE]);

				success = true;
			}
		}
	}
	return success;
}


