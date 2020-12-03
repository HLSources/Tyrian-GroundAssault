#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "skill.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "globals.h"
#include "msg_fx.h"
#include "projectiles.h"
#include "game.h"
#include "items.h"

class CLaserSpot : public CBaseEntity
{
	void Spawn( void );
	int ObjectCaps( void ) { return FCAP_DONT_SAVE; }
public:
	static CLaserSpot *CreateSpot( void );
};

LINK_ENTITY_TO_CLASS(laser_spot, CLaserSpot);

CLaserSpot *CLaserSpot::CreateSpot( void )
{
	CLaserSpot *pSpot = GetClassPtr( (CLaserSpot *)NULL );
	if (pSpot)
	{
		pSpot->Spawn();
		pSpot->pev->classname = MAKE_STRING("laser_spot");
	}
	return pSpot;
}

void CLaserSpot::Spawn( void )
{
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->flags |= FL_NOTARGET;
	pev->scale = 0.05;
	pev->frame = 0;
	pev->rendermode = kRenderGlow;
	pev->renderfx = kRenderFxNoDissipation;
	pev->renderamt = 255;
	SET_MODEL(ENT(pev), "sprites/particles_red.spr");
	UTIL_SetOrigin( pev, pev->origin );
};

class CTyrianGun : public CBasePlayerWeapon
{
public:
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual int GetItemInfo(ItemInfo *p);
	virtual void PrimaryAttack(void);
	virtual void SecondaryAttack(void);
	virtual void StartZoom(void);
	virtual void SwitchToSWeapon(void);
	virtual BOOL Deploy(void);
	virtual void ChooseWeapon(void);

	virtual void WeaponIdle(void);
	virtual BOOL ForceAutoaim(void); 
	virtual BOOL IsUseable(void);
	virtual void FireTyrianGun(void);
	virtual void FireSuperWeapon(void);
	virtual void FireBananaWeapon(bool PriFire);

	virtual void CheckForSWAmmo(void);
	virtual void CheckForSWEnergy(void);

	virtual void RestoreEnergy(void);
	virtual void UpdateEnergyStatus(void);
	virtual void OnFreePrivateData(void);

	void UpdateSpot(void);

	int m_iGunType;
	int m_iSuperWeapon;

	int m_iInSWMaxCharge;
	BOOL m_fInSuperWeapon;
	BOOL b_DeploySuperWeapon;
	BOOL b_UpdateRecharge;
	int m_iSWAmmoUse;

	float m_flEnergyUpdateTime;
	float m_fNextShoot;
	float m_fIdleAnimDelay;
	int m_iPowerUse;
	float m_fWpnDmg;

	Vector m_iWpnAccuracy;
	int m_iWpnProjAccuracy;

	int m_fSpotActive;
	int m_cActiveRockets;

	float m_fSide;

	int m_iZC;
	BOOL m_fInZoom;
	unsigned short m_usZoom;
	const char *szAnimExt;

protected:
	EHANDLE m_hLaserSpot;
};
LINK_ENTITY_TO_CLASS( weapon_tyriangun, CTyrianGun );

void CTyrianGun::Spawn(void)
{
	Precache();
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	m_iId = WEAPON_TYRIANGUN;
	m_fSpotActive = 0;
	m_hLaserSpot = NULL;

	m_iDefaultAmmo = AMMO_USE_CHRONOSCEPTRE;
	ChooseWeapon();
	Initialize();
}


void CTyrianGun::ChooseWeapon(void)
{
	int iWpnType = (int)mp_weapon_category.value;
	int iWpnMin, iWpnMax;

	if (iWpnType == STARTWPN_INSTAHIT)
	{
		iWpnMin = 0;
		iWpnMax = 19;
	}
	else if (iWpnType == STARTWPN_PROJECTILE)
	{
		iWpnMin = 20;
		iWpnMax = 45;
	}
	else if (iWpnType == STARTWPN_SMALL_DMG_RADIUS)
	{
		iWpnMin = 0;
		iWpnMax = 31;
	}
	else if (iWpnType == STARTWPN_ROCKETS_HP_ENERGY)
	{
		iWpnMin = 32;
		iWpnMax = 45;
	}
	else //all
	{
		iWpnMin = 0;
		iWpnMax = 45;
	}

	switch(RANDOM_LONG(iWpnMin,iWpnMax))
	{
		//insta-hit
		case 0: m_iGunType = TYRIANGUN_MACHINEGUN; break;
		case 1: m_iGunType = TYRIANGUN_VULCAN_MINIGUN; break;
		case 2: m_iGunType = TYRIANGUN_HEAVY_TURRET; break;
		case 3: m_iGunType = TYRIANGUN_GAUSS; break;
		case 4: m_iGunType = TYRIANGUN_LASER; break;
		case 5: m_iGunType = TYRIANGUN_RAILGUN; break;
		case 6: m_iGunType = TYRIANGUN_SUPER_RAILGUN; break;
		case 7: m_iGunType = TYRIANGUN_SHOCK_CANNON; break;
		case 8: m_iGunType = TYRIANGUN_LIGHTNING; break;
		case 9: m_iGunType = TYRIANGUN_TWIN_LASER; break;
		case 10: m_iGunType = TYRIANGUN_TAU_CANNON; break;
		case 11: m_iGunType = TYRIANGUN_TAU_CANNON; break;
		case 12: m_iGunType = TYRIANGUN_MEZON_CANNON; break;
		case 13: m_iGunType = TYRIANGUN_PHOTONGUN; break;
		case 14: m_iGunType = TYRIANGUN_BFG; break;
		case 15: m_iGunType = TYRIANGUN_CLUSTER_GUN; break;
		case 16: m_iGunType = TYRIANGUN_NEYTRON_GUN; break;
		case 17: m_iGunType = TYRIANGUN_FLAK_CANNON; break;
		case 18: m_iGunType = TYRIANGUN_BEAMSPLITTER; break;
		case 19: m_iGunType = TYRIANGUN_REPEATER; break;

		// low-power projectile
		case 20: m_iGunType = TYRIANGUN_NEEDLE_LASER; break;
		case 21: m_iGunType = TYRIANGUN_SHOCK_LASER; break;
		case 22: m_iGunType = TYRIANGUN_PLASMA; break;
		case 23: m_iGunType = TYRIANGUN_PULSE_CANNON; break;
		case 24: m_iGunType = TYRIANGUN_MULTI_CANNON; break;
		case 25: m_iGunType = TYRIANGUN_CHARGE_CANNON; break;
		case 26: m_iGunType = TYRIANGUN_RIPPER; break;
		case 27: m_iGunType = TYRIANGUN_BIORIFLE; break;
		case 28: m_iGunType = TYRIANGUN_FROSTER; break;
		case 29: m_iGunType = TYRIANGUN_SONIC_WAVE; break;
		case 30: m_iGunType = TYRIANGUN_M203; break;
		case 31: m_iGunType = TYRIANGUN_30MMGRENADE; break;

		// rockets
		case 32: m_iGunType = TYRIANGUN_BIOHAZARD_MISSILE; break;
		case 33: m_iGunType = TYRIANGUN_HELLFIRE; break;
		case 34: m_iGunType = TYRIANGUN_DEMOLITION_MISSILE; break;
		case 35: m_iGunType = TYRIANGUN_HVR_MISSILE; break;
		case 36: m_iGunType = TYRIANGUN_MICRO_MISSILE; break;
		case 37: m_iGunType = TYRIANGUN_GHOST_MISSILE; break;
		case 38: m_iGunType = TYRIANGUN_MINI_MISSILE; break;
		case 39: m_iGunType = TYRIANGUN_HELL_HOUNDER; break;

		// energy high-power projectile
		case 40: m_iGunType = TYRIANGUN_DISRUPTOR; break;
		case 41: m_iGunType = TYRIANGUN_STARBURST; break;
		case 42: m_iGunType = TYRIANGUN_GLUON; break;
		case 43: m_iGunType = TYRIANGUN_TRIDENT; break;
		case 44: m_iGunType = TYRIANGUN_PLASMA_STORM; break;
		case 45: m_iGunType = TYRIANGUN_SCORCHER; break;
	}
}


void CTyrianGun::Precache(void)
{
//	int iWpnType = (int)mp_weapon_category.value;

	//_p & _v models
//	if (iWpnType == STARTWPN_INSTAHIT)
//	{
		PRECACHE_MODEL("models/weapons/v_machinegun.mdl");
		PRECACHE_MODEL("models/weapons/p_machinegun.mdl");

		PRECACHE_MODEL("models/weapons/v_minigun.mdl");
		PRECACHE_MODEL("models/weapons/p_minigun.mdl");

		PRECACHE_MODEL("models/weapons/v_vulcan.mdl");
		PRECACHE_MODEL("models/weapons/p_vulcan.mdl");

		PRECACHE_MODEL("models/weapons/v_bfg.mdl");
		PRECACHE_MODEL("models/weapons/p_bfg.mdl");	

		PRECACHE_MODEL("models/weapons/v_beamsplitter.mdl");
		PRECACHE_MODEL("models/weapons/p_beamsplitter.mdl");

		PRECACHE_MODEL("models/weapons/v_clustergun.mdl");
		PRECACHE_MODEL("models/weapons/p_clustergun.mdl");

		PRECACHE_MODEL("models/weapons/v_neytrongun.mdl");
		PRECACHE_MODEL("models/weapons/p_neytrongun.mdl");

		PRECACHE_MODEL("models/weapons/v_hwturret.mdl");
		PRECACHE_MODEL("models/weapons/p_hwturret.mdl");

		PRECACHE_MODEL("models/weapons/v_photongun.mdl");
		PRECACHE_MODEL("models/weapons/p_photongun.mdl");

		PRECACHE_MODEL("models/weapons/v_twinlaser.mdl");
		PRECACHE_MODEL("models/weapons/p_twinlaser.mdl");

		PRECACHE_MODEL("models/weapons/v_shockcannon.mdl");
		PRECACHE_MODEL("models/weapons/p_shockcannon.mdl");

		PRECACHE_MODEL("models/weapons/v_mezoncannon.mdl");
		PRECACHE_MODEL("models/weapons/p_mezoncannon.mdl");

		PRECACHE_MODEL("models/weapons/v_railgun.mdl");
		PRECACHE_MODEL("models/weapons/p_railgun.mdl");

		PRECACHE_MODEL("models/weapons/v_superrailgun.mdl");
		PRECACHE_MODEL("models/weapons/p_superrailgun.mdl");

		PRECACHE_MODEL("models/weapons/v_lasergun.mdl");
		PRECACHE_MODEL("models/weapons/p_lasergun.mdl");

		PRECACHE_MODEL("models/weapons/v_gausscannon.mdl");
		PRECACHE_MODEL("models/weapons/p_gausscannon.mdl");

		PRECACHE_MODEL("models/weapons/v_lightninggun.mdl");
		PRECACHE_MODEL("models/weapons/p_lightninggun.mdl");

		PRECACHE_MODEL("models/weapons/v_flakcannon.mdl");
		PRECACHE_MODEL("models/weapons/p_flakcannon.mdl");

		PRECACHE_MODEL("models/weapons/v_taucannon.mdl");
		PRECACHE_MODEL("models/weapons/p_taucannon.mdl");
//	}

		PRECACHE_MODEL("models/weapons/v_demolition_missile.mdl");
		PRECACHE_MODEL("models/weapons/p_demolition_missile.mdl");

		PRECACHE_MODEL("models/weapons/v_biorifle.mdl");
		PRECACHE_MODEL("models/weapons/p_biorifle.mdl");

		PRECACHE_MODEL("models/weapons/v_scorcher.mdl");
		PRECACHE_MODEL("models/weapons/p_scorcher.mdl");

		PRECACHE_MODEL("models/weapons/v_sonicwave.mdl");
		PRECACHE_MODEL("models/weapons/p_sonicwave.mdl");

		PRECACHE_MODEL("models/weapons/v_gluongun.mdl");
		PRECACHE_MODEL("models/weapons/p_gluongun.mdl");

		PRECACHE_MODEL("models/weapons/v_trident.mdl");
		PRECACHE_MODEL("models/weapons/p_trident.mdl");

		PRECACHE_MODEL("models/weapons/v_starburst.mdl");
		PRECACHE_MODEL("models/weapons/p_starburst.mdl");

		PRECACHE_MODEL("models/weapons/v_repeater.mdl");
		PRECACHE_MODEL("models/weapons/p_repeater.mdl");

		PRECACHE_MODEL("models/weapons/v_ripper.mdl");
		PRECACHE_MODEL("models/weapons/p_ripper.mdl");

		PRECACHE_MODEL("models/weapons/v_minimissile.mdl");
		PRECACHE_MODEL("models/weapons/p_minimissile.mdl");

		PRECACHE_MODEL("models/weapons/v_plasmagun.mdl");
		PRECACHE_MODEL("models/weapons/p_plasmagun.mdl");

		PRECACHE_MODEL("models/weapons/v_disruptor.mdl");
		PRECACHE_MODEL("models/weapons/p_disruptor.mdl");

		PRECACHE_MODEL("models/weapons/v_micromissile.mdl");
		PRECACHE_MODEL("models/weapons/p_micromissile.mdl");

		PRECACHE_MODEL("models/weapons/v_pulsecannon.mdl");
		PRECACHE_MODEL("models/weapons/p_pulsecannon.mdl");

		PRECACHE_MODEL("models/weapons/v_shocklaser.mdl");
		PRECACHE_MODEL("models/weapons/p_shocklaser.mdl");

		PRECACHE_MODEL("models/weapons/v_needlelaser.mdl");
		PRECACHE_MODEL("models/weapons/p_needlelaser.mdl");

		PRECACHE_MODEL("models/weapons/v_multicannon.mdl");
		PRECACHE_MODEL("models/weapons/p_multicannon.mdl");

		PRECACHE_MODEL("models/weapons/v_chargecannon.mdl");
		PRECACHE_MODEL("models/weapons/p_chargecannon.mdl");

		PRECACHE_MODEL("models/weapons/v_hellhounder.mdl");
		PRECACHE_MODEL("models/weapons/p_hellhounder.mdl");

		PRECACHE_MODEL("models/weapons/v_hvrlauncher.mdl");
		PRECACHE_MODEL("models/weapons/p_hvrlauncher.mdl");

		PRECACHE_MODEL("models/weapons/v_30mmglauncher.mdl");
		PRECACHE_MODEL("models/weapons/p_30mmglauncher.mdl");

		PRECACHE_MODEL("models/weapons/v_m203glauncher.mdl");
		PRECACHE_MODEL("models/weapons/p_m203glauncher.mdl");

		PRECACHE_MODEL("models/weapons/v_bio_missile.mdl");
		PRECACHE_MODEL("models/weapons/p_bio_missile.mdl");

		PRECACHE_MODEL("models/weapons/v_froster.mdl");
		PRECACHE_MODEL("models/weapons/p_froster.mdl");

		PRECACHE_MODEL("models/weapons/v_ghostmissile.mdl");
		PRECACHE_MODEL("models/weapons/p_ghostmissile.mdl");

		PRECACHE_MODEL("models/weapons/v_bananagun.mdl");
		PRECACHE_MODEL("models/weapons/p_bananagun.mdl");

		PRECACHE_MODEL("models/weapons/v_plasmastorm.mdl");
		PRECACHE_MODEL("models/weapons/p_plasmastorm.mdl");

		//super weapons
		PRECACHE_MODEL("models/weapons/v_hellfire.mdl");
		PRECACHE_MODEL("models/weapons/p_hellfire.mdl");

		PRECACHE_MODEL("models/weapons/v_sw_guidedbomb.mdl");
		PRECACHE_MODEL("models/weapons/p_sw_guidedbomb.mdl");

		PRECACHE_MODEL("models/weapons/v_sw_protonmissile.mdl");
		PRECACHE_MODEL("models/weapons/p_sw_protonmissile.mdl");

		PRECACHE_MODEL("models/weapons/v_sw_teleport.mdl");
		PRECACHE_MODEL("models/weapons/p_sw_teleport.mdl");

		PRECACHE_MODEL("models/weapons/v_sw_shockwave.mdl");
		PRECACHE_MODEL("models/weapons/p_sw_shockwave.mdl");

		PRECACHE_MODEL("models/weapons/v_sw_nuke_launcher.mdl");
		PRECACHE_MODEL("models/weapons/p_sw_nuke_launcher.mdl");

		PRECACHE_MODEL("models/weapons/v_sw_chronosceptre.mdl");
		PRECACHE_MODEL("models/weapons/p_sw_chronosceptre.mdl");

		PRECACHE_MODEL("models/weapons/v_sw_sunofgod.mdl");
		PRECACHE_MODEL("models/weapons/p_sw_sunofgod.mdl");


	m_iZC = PRECACHE_MODEL("sprites/hud_zoom01.spr");

	//related models
	PRECACHE_MODEL("models/w_acidblob.mdl");
	PRECACHE_MODEL("models/projectiles.mdl");
	PRECACHE_MODEL("models/projectiles_alt.mdl");
	PRECACHE_MODEL("models/effects/ef_explosions.mdl");
	PRECACHE_MODEL("models/w_tripmine.mdl");

	//effects
	PRECACHE_MODEL("models/effects/ef_gun_shells.mdl");
	PRECACHE_MODEL("models/effects/ef_teleporter.mdl");
	PRECACHE_MODEL("models/effects/ef_atombomb_call.mdl");
	PRECACHE_MODEL("models/effects/ef_explosions.mdl");
	PRECACHE_MODEL("models/effects/ef_gibs.mdl");
	PRECACHE_MODEL("models/effects/ef_player_gibs.mdl");

	//sounds
	PRECACHE_SOUND("weapons/fire_neytrongun.wav");
	PRECACHE_SOUND("weapons/fire_clustergun.wav");
	PRECACHE_SOUND("weapons/fire_disruptor.wav");
	PRECACHE_SOUND("weapons/fire_particlebeam.wav");
	PRECACHE_SOUND("weapons/fire_photongun.wav");
	PRECACHE_SOUND("weapons/fire_repeater.wav");
	PRECACHE_SOUND("weapons/fire_machinegun.wav");
	PRECACHE_SOUND("weapons/fire_minigun.wav");
	PRECACHE_SOUND("weapons/fire_vulcan.wav");
	PRECACHE_SOUND("weapons/fire_heavy_turret.wav");
	PRECACHE_SOUND("weapons/fire_gauss.wav");
	PRECACHE_SOUND("weapons/fire_laser.wav");
	PRECACHE_SOUND("weapons/fire_shocklaser.wav");
	PRECACHE_SOUND("weapons/fire_needlelaser.wav");
	PRECACHE_SOUND("weapons/fire_minimissile.wav");
	PRECACHE_SOUND("weapons/fire_30mmgrenade.wav");
	PRECACHE_SOUND("weapons/fire_m203grenade.wav");
	PRECACHE_SOUND("weapons/fire_hellhounder.wav");
	PRECACHE_SOUND("weapons/fire_biohazardmissile.wav");
	PRECACHE_SOUND("weapons/fire_gluon.wav");
	PRECACHE_SOUND("weapons/fire_trident.wav");
	PRECACHE_SOUND("weapons/fire_hellfire.wav");
	PRECACHE_SOUND("weapons/fire_railgun.wav");
	PRECACHE_SOUND("weapons/fire_tau_cannon.wav");
	PRECACHE_SOUND("weapons/fire_superrailgun.wav");
	PRECACHE_SOUND("weapons/fire_scorcher.wav");
	PRECACHE_SOUND("weapons/fire_lightning.wav");
	PRECACHE_SOUND("weapons/fire_shock_cannon.wav");
	PRECACHE_SOUND("weapons/fire_multicannon.wav");
	PRECACHE_SOUND("weapons/fire_plasma.wav");
	PRECACHE_SOUND("weapons/fire_plasmastorm.wav");
	PRECACHE_SOUND("weapons/fire_pulsecannon.wav");
	PRECACHE_SOUND("weapons/fire_twin_laser.wav");
	PRECACHE_SOUND("weapons/fire_protonmissile.wav");
	PRECACHE_SOUND("weapons/fire_antimatherialmissile.wav");
	PRECACHE_SOUND("weapons/fire_sunofgod.wav");
	PRECACHE_SOUND("weapons/fire_sunofgod_container.wav");
	PRECACHE_SOUND("weapons/fire_nuclearmissile.wav");
	PRECACHE_SOUND("weapons/fire_hvrmissile.wav");
	PRECACHE_SOUND("weapons/fire_lightningball.wav");
	PRECACHE_SOUND("weapons/fire_teleport.wav");
	PRECACHE_SOUND("weapons/fire_froster.wav");
	PRECACHE_SOUND("weapons/fire_mezoncannon.wav");
	PRECACHE_SOUND("weapons/fire_sonicwave.wav");
	PRECACHE_SOUND("weapons/fire_micromissile.wav");
	PRECACHE_SOUND("weapons/fire_chargecannon.wav");
	PRECACHE_SOUND("weapons/fire_demolitionmissile.wav");
	PRECACHE_SOUND("weapons/fire_ghostmissile.wav");
	PRECACHE_SOUND("weapons/fire_ghostmissile_teleport.wav");
	PRECACHE_SOUND("weapons/fire_ripper.wav");
	PRECACHE_SOUND("weapons/fire_bfg.wav");
	PRECACHE_SOUND("weapons/fire_guidedbomb.wav");
	PRECACHE_SOUND("weapons/fire_toilet.wav");
	PRECACHE_SOUND("weapons/fire_banana.wav");
	PRECACHE_SOUND("weapons/fire_flak_cannon.wav");
	PRECACHE_SOUND("weapons/fire_starburst.wav");
	PRECACHE_SOUND("weapons/fire_biorifle.wav");
	PRECACHE_SOUND("weapons/fire_beamsplitter.wav");

	PRECACHE_SOUND("weapons/explode_demolitionmissile.wav");
	PRECACHE_SOUND("weapons/explode_trident.wav");
	PRECACHE_SOUND("weapons/hit_biorifle.wav");
	PRECACHE_SOUND("weapons/explode_biorifle.wav");
	PRECACHE_SOUND("weapons/explode_starburst.wav");
	PRECACHE_SOUND("weapons/explode_clustergun.wav");
	PRECACHE_SOUND("weapons/explode_disruptor.wav");
	PRECACHE_SOUND("weapons/explode_ripper.wav");
	PRECACHE_SOUND("weapons/explode_ripper01.wav");
	PRECACHE_SOUND("weapons/explode_ripper02.wav");
	PRECACHE_SOUND("weapons/explode_ghostmissile.wav");
	PRECACHE_SOUND("weapons/explode_sonicwave.wav");
	PRECACHE_SOUND("weapons/explode_antimatherial.wav");
	PRECACHE_SOUND("weapons/explode_teleport.wav");
	PRECACHE_SOUND("weapons/explode_lightning.wav");
	PRECACHE_SOUND("weapons/explode_lightningball.wav");
	PRECACHE_SOUND("weapons/explode_plasma.wav");
	PRECACHE_SOUND("weapons/explode_plasma_storm.wav");
	PRECACHE_SOUND("weapons/explode_plasma_shield_charge.wav");
	PRECACHE_SOUND("weapons/explode_m203.wav");
	PRECACHE_SOUND("weapons/explode_minimissile.wav");
	PRECACHE_SOUND("weapons/explode_biohazardmissile.wav");
	PRECACHE_SOUND("weapons/explode_shockwave.wav");
	PRECACHE_SOUND("weapons/explode_nuclearmissile.wav");
	PRECACHE_SOUND("weapons/explode_protonmissile.wav");
	PRECACHE_SOUND("weapons/explode_antimatherialmissile.wav");
	PRECACHE_SOUND("weapons/explode_blackhole.wav");
	PRECACHE_SOUND("weapons/explode_sunofgod.wav");
	PRECACHE_SOUND("weapons/explode_hellhounder.wav");
	PRECACHE_SOUND("weapons/explode_beam1.wav");
	PRECACHE_SOUND("weapons/explode_beam2.wav");
	PRECACHE_SOUND("weapons/explode_beam3.wav");
	PRECACHE_SOUND("weapons/explode_shocklaser.wav");
	PRECACHE_SOUND("weapons/explode_gluon.wav");
	PRECACHE_SOUND("weapons/explode_scorcher.wav");
	PRECACHE_SOUND("weapons/explode_flashbang.wav");
	PRECACHE_SOUND("weapons/explode_chargecannon.wav");

	PRECACHE_SOUND("weapons/explode1.wav");
	PRECACHE_SOUND("weapons/explode2.wav");
	PRECACHE_SOUND("weapons/explode3.wav");
	PRECACHE_SOUND("weapons/explode4.wav");
	PRECACHE_SOUND("weapons/explode5.wav");
	PRECACHE_SOUND("weapons/explode_uw.wav");
	PRECACHE_SOUND("weapons/electro4.wav");
	PRECACHE_SOUND("weapons/electro5.wav");
	PRECACHE_SOUND("weapons/electro6.wav");

	PRECACHE_SOUND("weapons/explode_small1.wav");
	PRECACHE_SOUND("weapons/explode_small2.wav");
	PRECACHE_SOUND("weapons/explode_small3.wav");

	PRECACHE_SOUND("weapons/fly_sunofgod.wav");
	PRECACHE_SOUND("weapons/fly_hvrmissile.wav");
	PRECACHE_SOUND("weapons/fly_minimissile.wav");
	PRECACHE_SOUND("weapons/fly_hellhounder.wav");
	PRECACHE_SOUND("weapons/fly_biohazardmissile.wav");
	PRECACHE_SOUND("weapons/fly_demolitionmissile.wav");
	PRECACHE_SOUND("weapons/fly_ghostmissile.wav");
	PRECACHE_SOUND("weapons/fly_guidedbomb.wav");
	PRECACHE_SOUND("weapons/fly_hellfire.wav");
	PRECACHE_SOUND("weapons/fly_toilet.wav");

	PRECACHE_SOUND("weapons/biohazardmissile_smoke.wav");
	PRECACHE_SOUND("weapons/projectile_bounce1.wav");
	PRECACHE_SOUND("weapons/projectile_bounce2.wav");
	PRECACHE_SOUND("weapons/projectile_bounce3.wav");

	PRECACHE_SOUND("weapons/ric1.wav");
	PRECACHE_SOUND("weapons/ric2.wav");
	PRECACHE_SOUND("weapons/ric3.wav");

	PRECACHE_SOUND("weapons/dryfire1.wav");
	PRECACHE_SOUND("weapons/zoom.wav");

	PRECACHE_SOUND("weapons/airstrike_mortar_whistle1.wav");
	PRECACHE_SOUND("weapons/airstrike_mortar_whistle2.wav");
	PRECACHE_SOUND("weapons/airstrike_mortar_whistle3.wav");
	PRECACHE_SOUND("weapons/airstrike_call.wav");
	PRECACHE_SOUND("weapons/airstrike_mortarhit.wav");

	PRECACHE_SOUND("weapons/mine_deploy.wav");
	PRECACHE_SOUND("weapons/mine_beep.wav");
	PRECACHE_SOUND("weapons/mine_charge.wav");
	PRECACHE_SOUND("weapons/mine_explode.wav");

	PRECACHE_SOUND("common/teleport.wav");

	//atom bomb
	PRECACHE_MODEL("models/w_atombomb.mdl");
	PRECACHE_SOUND("weapons/beep_atombomb.wav");
	PRECACHE_SOUND("weapons/atombomb_call.wav");

	//spider mine stuff
	PRECACHE_MODEL("models/w_spider_mine.mdl");
	PRECACHE_SOUND("weapons/spidermine_hunt.wav");

	//sentry
//	UTIL_PrecacheOther("monster_sentry");

	// lightning field
	PRECACHE_MODEL("models/effects/ef_lightning_field.mdl");

//satellite strike
	PRECACHE_SOUND("weapons/explode_satellite_strike.wav");
	PRECACHE_SOUND("weapons/satellite_strike_call.wav");

	UTIL_PrecacheOther("laser_spot");
	m_usZoom = PRECACHE_EVENT(1, "events/zoom.sc");
}

int CTyrianGun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "energy_core";
	p->iMaxAmmo1 = MAX_AMMO_SUPERWEAPON;
	p->pszAmmo2 = "generator_power";
	p->iMaxAmmo2 = MAX_GENERATOR_AUG;
	p->iMaxClip = -1;
#ifdef SERVER_WEAPON_SLOTS
	p->iSlot = 0;
	p->iPosition = 0;
#endif
	p->iId = m_iId = WEAPON_TYRIANGUN;
	p->iFlags = ITEM_FLAG_CANNOTDROP;
	return 1;
}

BOOL CTyrianGun::IsUseable(void)
{
	if (m_pPlayer->m_iWpnDestroyed == TRUE)
		return FALSE;

	return TRUE;
}

BOOL CTyrianGun::Deploy(void)
{
	m_cActiveRockets = 0;
	m_fInZoom = FALSE;
	pev->pushmsec = 0;

	if (m_fInSuperWeapon)
	{
		MESSAGE_BEGIN(MSG_ONE, gmsgFlashBattery, NULL, ENT(m_pPlayer->pev));
		WRITE_SHORT(m_pPlayer->m_iFlashBattery);
		WRITE_BYTE(m_iGunType);
		MESSAGE_END();

		b_UpdateRecharge = FALSE;
	}
	else
		b_UpdateRecharge = TRUE;

	UpdateEnergyStatus();

	m_flTimeWeaponIdle = m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.25;
	m_flEnergyUpdateTime = UTIL_WeaponTimeBase() + 0.1;

	if (m_pPlayer->m_iItemBanana || mp_bananamode.value > 0.0f)
	{
		m_fInSuperWeapon = FALSE;
		pev->viewmodel = MAKE_STRING("models/weapons/v_bananagun.mdl");
		pev->weaponmodel= MAKE_STRING("models/weapons/p_bananagun.mdl");
		szAnimExt = "gauss";
	}
	else if (m_fInSuperWeapon)
	{
		CheckForSWEnergy();
		b_DeploySuperWeapon = FALSE;

		switch(m_iSuperWeapon)
		{
			case TYRIANGUN_SWEAPON_GUIDED_BOMB:
				m_iInSWMaxCharge = POWER_MAX_CHARGE_GUIDED_BOMB;
				pev->viewmodel = MAKE_STRING("models/weapons/v_sw_guidedbomb.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_sw_guidedbomb.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_SWEAPON_PROTON_MISSILE: 
				m_iInSWMaxCharge = POWER_MAX_CHARGE_PROTON_MISSILE;
				pev->viewmodel = MAKE_STRING("models/weapons/v_sw_protonmissile.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_sw_protonmissile.mdl");
				szAnimExt = "rpg";
			break;

			case TYRIANGUN_SWEAPON_DISPLACER: 
				m_iInSWMaxCharge = POWER_MAX_CHARGE_DISPLACER;
				pev->viewmodel = MAKE_STRING("models/weapons/v_sw_teleport.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_sw_teleport.mdl");
				szAnimExt = "egon";
			break;

			case TYRIANGUN_SWEAPON_SHOCK_WAVE: 
				m_iInSWMaxCharge = POWER_MAX_CHARGE_SHOCK_WAVE;
				pev->viewmodel = MAKE_STRING("models/weapons/v_sw_shockwave.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_sw_shockwave.mdl");
				szAnimExt = "egon";
			break;

			case TYRIANGUN_SWEAPON_NUCLEAR_LAUNCHER: 
				m_iInSWMaxCharge = POWER_MAX_CHARGE_NUCLEAR_MISSILE;
				pev->viewmodel = MAKE_STRING("models/weapons/v_sw_nuke_launcher.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_sw_nuke_launcher.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_SWEAPON_CHRONOSCEPTRE:
				m_iInSWMaxCharge = POWER_MAX_CHARGE_CHRONOSCEPTRE;
				pev->viewmodel = MAKE_STRING("models/weapons/v_sw_chronosceptre.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_sw_chronosceptre.mdl");
				szAnimExt = "egon";
			break;

			case TYRIANGUN_SWEAPON_SUN_OF_GOD: 
				m_iInSWMaxCharge = POWER_MAX_CHARGE_SUN_OF_GOD;
				pev->viewmodel = MAKE_STRING("models/weapons/v_sw_sunofgod.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_sw_sunofgod.mdl");
				szAnimExt = "minigun";
			break;
		}
	}
	else //normal gun
	{
		switch(m_iGunType)
		{
			case TYRIANGUN_MACHINEGUN:
				pev->viewmodel = MAKE_STRING("models/weapons/v_machinegun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_machinegun.mdl");
				szAnimExt = "egon";
			break;

			case TYRIANGUN_MINIGUN:
				pev->viewmodel = MAKE_STRING("models/weapons/v_minigun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_minigun.mdl");
				szAnimExt = "minigun";
			break;

			case TYRIANGUN_VULCAN_MINIGUN:
				pev->viewmodel = MAKE_STRING("models/weapons/v_vulcan.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_vulcan.mdl");
				szAnimExt = "egon";
			break;

			case TYRIANGUN_HEAVY_TURRET:
				pev->viewmodel = MAKE_STRING("models/weapons/v_hwturret.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_hwturret.mdl");
				szAnimExt = "saw";
			break;

			case TYRIANGUN_REPEATER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_repeater.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_repeater.mdl");
				szAnimExt = "gauss";
			break;

			case TYRIANGUN_SHOCK_CANNON:
				pev->viewmodel = MAKE_STRING("models/weapons/v_shockcannon.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_shockcannon.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_RAILGUN:
				pev->viewmodel = MAKE_STRING("models/weapons/v_railgun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_railgun.mdl");
				szAnimExt = "sniper";
			break;		

			case TYRIANGUN_MEZON_CANNON:
				pev->viewmodel = MAKE_STRING("models/weapons/v_mezoncannon.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_mezoncannon.mdl");
				szAnimExt = "gauss";
			break;

			case TYRIANGUN_GAUSS:
				pev->viewmodel = MAKE_STRING("models/weapons/v_gausscannon.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_gausscannon.mdl");
				szAnimExt = "egon";
			break;

			case TYRIANGUN_LASER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_lasergun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_lasergun.mdl");
				szAnimExt = "sniper";
			break;

			case TYRIANGUN_SUPER_RAILGUN:
				pev->viewmodel = MAKE_STRING("models/weapons/v_superrailgun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_superrailgun.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_LIGHTNING:
				pev->viewmodel = MAKE_STRING("models/weapons/v_lightninggun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_lightninggun.mdl");
				szAnimExt = "saw";
			break;

			case TYRIANGUN_NEYTRON_GUN:
				pev->viewmodel = MAKE_STRING("models/weapons/v_neytrongun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_neytrongun.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_TAU_CANNON:
				pev->viewmodel = MAKE_STRING("models/weapons/v_taucannon.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_taucannon.mdl");
				szAnimExt = "mp5";
			break;

			case TYRIANGUN_PHOTONGUN:
				pev->viewmodel = MAKE_STRING("models/weapons/v_photongun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_photongun.mdl");
				szAnimExt = "mp5";
			break;

			case TYRIANGUN_CLUSTER_GUN:
				pev->viewmodel = MAKE_STRING("models/weapons/v_clustergun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_clustergun.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_M203:
				pev->viewmodel = MAKE_STRING("models/weapons/v_m203glauncher.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_m203glauncher.mdl");
				szAnimExt = "gauss";
			break;

			case TYRIANGUN_30MMGRENADE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_30mmglauncher.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_30mmglauncher.mdl");
				szAnimExt = "mp5";
			break;

			case TYRIANGUN_SHOCK_LASER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_shocklaser.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_shocklaser.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_NEEDLE_LASER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_needlelaser.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_needlelaser.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_HELL_HOUNDER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_hellhounder.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_hellhounder.mdl");
				szAnimExt = "sniper";
			break;

			case TYRIANGUN_FLAK_CANNON:
				pev->viewmodel = MAKE_STRING("models/weapons/v_flakcannon.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_flakcannon.mdl");
				szAnimExt = "gauss";
			break;

			case TYRIANGUN_BIOHAZARD_MISSILE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_bio_missile.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_bio_missile.mdl");
				szAnimExt = "rpg";
			break;

			case TYRIANGUN_MICRO_MISSILE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_micromissile.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_micromissile.mdl");
				szAnimExt = "mp5";
			break;

			case TYRIANGUN_DISRUPTOR:
				pev->viewmodel = MAKE_STRING("models/weapons/v_disruptor.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_disruptor.mdl");
				szAnimExt = "mp5";
			break;

			case TYRIANGUN_PLASMA:
				pev->viewmodel = MAKE_STRING("models/weapons/v_plasmagun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_plasmagun.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_HVR_MISSILE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_hvrlauncher.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_hvrlauncher.mdl");
				szAnimExt = "rpg";
			break;

			case TYRIANGUN_PULSE_CANNON:
				pev->viewmodel = MAKE_STRING("models/weapons/v_pulsecannon.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_pulsecannon.mdl");
				szAnimExt = "minigun";
			break;

			case TYRIANGUN_MULTI_CANNON:
				pev->viewmodel = MAKE_STRING("models/weapons/v_multicannon.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_multicannon.mdl");
				szAnimExt = "minigun";
			break;

			case TYRIANGUN_SCORCHER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_scorcher.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_scorcher.mdl");
				szAnimExt = "saw";
			break;

			case TYRIANGUN_FROSTER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_froster.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_froster.mdl");
				szAnimExt = "saw";
			break;

			case TYRIANGUN_SONIC_WAVE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_sonicwave.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_sonicwave.mdl");
				szAnimExt = "rpg";
			break;

			case TYRIANGUN_CHARGE_CANNON:
				pev->viewmodel = MAKE_STRING("models/weapons/v_chargecannon.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_chargecannon.mdl");
				szAnimExt = "saw";
			break;

			case TYRIANGUN_TWIN_LASER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_twinlaser.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_twinlaser.mdl");
				m_fSide = 1;
				szAnimExt = "uzis";
			break;

			case TYRIANGUN_RIPPER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_ripper.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_ripper.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_PLASMA_STORM:
				pev->viewmodel = MAKE_STRING("models/weapons/v_plasmastorm.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_plasmastorm.mdl");
				szAnimExt = "gauss";
			break;

			case TYRIANGUN_MINI_MISSILE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_minimissile.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_minimissile.mdl");
				szAnimExt = "uzis";
			break;

			case TYRIANGUN_DEMOLITION_MISSILE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_demolition_missile.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_demolition_missile.mdl");
				szAnimExt = "rpg";
			break;

			case TYRIANGUN_STARBURST:
				pev->viewmodel = MAKE_STRING("models/weapons/v_starburst.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_starburst.mdl");
				szAnimExt = "gauss";
			break;

			case TYRIANGUN_GLUON:
				pev->viewmodel = MAKE_STRING("models/weapons/v_gluongun.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_gluongun.mdl");
				szAnimExt = "mp5";
			break;

			case TYRIANGUN_TRIDENT:
				pev->viewmodel = MAKE_STRING("models/weapons/v_trident.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_trident.mdl");
				szAnimExt = "rifle";
			break;

			case TYRIANGUN_BFG:
				pev->viewmodel = MAKE_STRING("models/weapons/v_bfg.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_bfg.mdl");
				szAnimExt = "saw";
			break;

			case TYRIANGUN_BIORIFLE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_biorifle.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_biorifle.mdl");
				szAnimExt = "saw";
			break;

			case TYRIANGUN_HELLFIRE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_hellfire.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_hellfire.mdl");
				szAnimExt = "rpg";
			break;

			case TYRIANGUN_BEAMSPLITTER:
				pev->viewmodel = MAKE_STRING("models/weapons/v_beamsplitter.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_beamsplitter.mdl");
				szAnimExt = "saw";
			break;		

			case TYRIANGUN_GHOST_MISSILE:
				pev->viewmodel = MAKE_STRING("models/weapons/v_ghostmissile.mdl");
				pev->weaponmodel= MAKE_STRING("models/weapons/p_ghostmissile.mdl");
				szAnimExt = "rifle";
			break;
		}
	}
	return DefaultDeploy(STRING(pev->viewmodel), STRING(pev->weaponmodel), TYRIAN_WEAPON_ANIM_DRAW, szAnimExt, 0);
}

void CTyrianGun::PrimaryAttack(void)
{
	if (m_fInZoom)
		pev->pushmsec = 0;// XDM: stop zooming

	if (m_pPlayer->m_iFlashBattery < m_iPowerUse || !m_iGunType)
	{
		EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/dryfire1.wav", VOL_NORM, ATTN_NORM);
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase()+0.5;
		return;
	}

	if (m_fInSuperWeapon)
	{
		if (b_DeploySuperWeapon || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < AMMO_USE_GUIDED_BOMB || m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE_GUIDED_BOMB)
		{
			EMIT_SOUND(ENT(pev), CHAN_BODY, "weapons/dryfire1.wav", VOL_NORM, ATTN_NORM);
			m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase()+1.0;
			SwitchToSWeapon();
			b_DeploySuperWeapon = FALSE;
			return;
		}
	}

	if (!b_UpdateRecharge)
		b_UpdateRecharge = TRUE;

	if (mp_bananamode.value > 0.0f || m_pPlayer->m_iItemBanana)
	{
		FireBananaWeapon(true);
		return;
	}
	
	else if (m_fInSuperWeapon)
		FireSuperWeapon();
	else
		FireTyrianGun();

	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase()+0.1;
}

void CTyrianGun::SecondaryAttack(void)
{
	if (!m_iGunType)
		return;

	if (mp_bananamode.value > 0.0f || m_pPlayer->m_iItemBanana)
	{
		FireBananaWeapon(false);
		return;
	}
	StartZoom();
}

void CTyrianGun::FireTyrianGun(void)
{
	Vector vecAng = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	Vector vecSrc = gpGlobals->v_forward * 8 + gpGlobals->v_right * 2.5f + gpGlobals->v_up * -1.0;
	UTIL_MakeVectors(vecAng);

	switch(m_iGunType)
	{
	case TYRIANGUN_MACHINEGUN:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_3DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_5DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_MACHINEGUN;
		m_fWpnDmg = gSkillData.DmgMachinegun + (gSkillData.DmgMachinegun * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_MACHINEGUN, m_fWpnDmg, DMG_BULLET, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_MACHINEGUN;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.25;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_MINIGUN: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_2DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_4DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_6DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_MINIGUN;
		m_fWpnDmg = gSkillData.DmgMinigun + (gSkillData.DmgMinigun * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(2, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_MINIGUN, m_fWpnDmg, DMG_BULLET, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_MINIGUN;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*0.75):(m_fWpnDmg*1.5);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.25;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;
	
	case TYRIANGUN_PHOTONGUN:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_2DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_PHOTONGUN;
		m_fWpnDmg = gSkillData.DmgPhotongun + (gSkillData.DmgPhotongun * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 12 + gpGlobals->v_right * 2.5f + gpGlobals->v_up * -0.5, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_PHOTONGUN, m_fWpnDmg, DMG_ENERGYBEAM, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_PHOTONGUN;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.0;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_VULCAN_MINIGUN:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_3DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_5DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_7DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_VULCAN_MINIGUN;
		m_fWpnDmg = gSkillData.DmgVulcan + (gSkillData.DmgVulcan * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(3, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_VULCAN, m_fWpnDmg, DMG_BULLET, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_VULCAN_MINIGUN;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*0.75):(m_fWpnDmg*1.5);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.1;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_HEAVY_TURRET: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_2DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_3DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_4DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_HEAVY_TURRET;
		m_fWpnDmg = gSkillData.DmgHeavyTurret + (gSkillData.DmgHeavyTurret * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_HEAVY_TURRET, m_fWpnDmg, DMG_BLAST, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_30MMTURRET;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;
	
	case TYRIANGUN_GAUSS: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_2DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_GAUSS;
		m_fWpnDmg = gSkillData.DmgGauss + (gSkillData.DmgGauss * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + gpGlobals->v_forward*8 + gpGlobals->v_right * 2.5f + gpGlobals->v_up * -2.0f, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_GAUSS, m_fWpnDmg, DMG_ENERGYBEAM, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_GAUSS;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.1;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_RAILGUN: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_RAILGUN;
		m_fWpnDmg = gSkillData.DmgRailGun + (gSkillData.DmgRailGun * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_RAILGUN, m_fWpnDmg, DMG_BULLET, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_RAILGUN;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_SUPER_RAILGUN: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_SUPER_RAILGUN;
		m_fWpnDmg = gSkillData.DmgSuperRailGun + (gSkillData.DmgSuperRailGun * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_SUPERRAILGUN, m_fWpnDmg, DMG_BULLET, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_SUPER_RAILGUN;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.5;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_SHOCK_CANNON: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_2DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_SHOCK_CANNON;
		m_fWpnDmg = gSkillData.DmgShockRifle + (gSkillData.DmgShockRifle * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_SHOCK, m_fWpnDmg, DMG_SHOCK, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_SHOCK_CANNON;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.2;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_LIGHTNING: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_3DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_LIGHTNING;
		m_fWpnDmg = gSkillData.DmgLightning + (gSkillData.DmgLightning * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_LIGHTNING, m_fWpnDmg, DMG_SHOCK | DMG_RADIUS_MAX, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_LIGHTNING;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.8;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_LASER: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_2DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_LASER;
		m_fWpnDmg = gSkillData.DmgLaser + (gSkillData.DmgLaser * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 8 + gpGlobals->v_right * 1.5f + gpGlobals->v_up * -1.0, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_LASER, m_fWpnDmg, DMG_ENERGYBEAM, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_LASER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_TWIN_LASER: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_2DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_3DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_TWIN_LASER;
		m_fWpnDmg = gSkillData.DmgTwinLaser + (gSkillData.DmgTwinLaser * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 8.0f + (gpGlobals->v_right * 2.0f * m_fSide) + gpGlobals->v_up * -1.0, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_TWINLASER, m_fWpnDmg, DMG_ENERGYBEAM, this, m_pPlayer, m_pPlayer->random_seed);

		if (m_fSide != 1)
			FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), TYRIAN_WEAPON_ANIM_FIRE_ALT, m_iGunType, 1);
		else
			FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), TYRIAN_WEAPON_ANIM_FIRE, m_iGunType, 0);

		m_fNextShoot = FIRERATE_TWIN_LASER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.8;
		m_fSide = -m_fSide;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_TAU_CANNON: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_TAU_CANNON;
		m_fWpnDmg = gSkillData.DmgTauCannon + (gSkillData.DmgTauCannon * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 12 + gpGlobals->v_right * 2.0f + gpGlobals->v_up * -1.0, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_TAUCANNON, m_fWpnDmg, DMG_ENERGYBLAST, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_TAU_CANNON;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.0;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_BEAMSPLITTER: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_0DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_3DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_BEAMSPLITTER;
		m_fWpnDmg = gSkillData.DmgBeamSplitter + (gSkillData.DmgBeamSplitter * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_BEAMSPLITTER, m_fWpnDmg, DMG_VAPOURIZING, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_BEAMSPLITTER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.0;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_MEZON_CANNON: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_2DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_3DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_MEZON_CANNON;
		m_fWpnDmg = gSkillData.DmgMezonCannon + (gSkillData.DmgMezonCannon * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16.0f + gpGlobals->v_right * 6.0f + gpGlobals->v_up * -2.0, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_MEZONCANNON, m_fWpnDmg, DMG_ENERGYBEAM, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_MEZON_CANNON;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
		break;

	case TYRIANGUN_REPEATER: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_4DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_6DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_8DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_REPEATER;
		m_fWpnDmg = gSkillData.DmgRepeater + (gSkillData.DmgRepeater * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(8, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_BUCKSHOT, m_fWpnDmg, DMG_BULLET, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_REPEATER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*4.0):(m_fWpnDmg*8.0);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 2.0;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_BFG: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_6DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_8DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_12DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_BFG;
		m_fWpnDmg = gSkillData.DmgBfg + (gSkillData.DmgBfg * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(5, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_BFG, m_fWpnDmg, DMG_PLASMA, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_BFG;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*2.0):(m_fWpnDmg*4.0);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_CLUSTER_GUN:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_3DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_4DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_CLUSTER_GUN;	
		m_fWpnDmg = gSkillData.DmgClusterGun + (gSkillData.DmgClusterGun * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 8 + gpGlobals->v_right * 1.5f + gpGlobals->v_up * -1.5, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_CLUSTERGUN, m_fWpnDmg, DMG_MORTAR | DMG_RADIUS_MAX, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_CLUSTER_GUN;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_NEYTRON_GUN: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_1DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_2DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_3DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_NEYTRON_GUN;
		m_fWpnDmg = gSkillData.DmgNeytronGun + (gSkillData.DmgNeytronGun * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(1, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_NEYTRONGUN, m_fWpnDmg, DMG_ENERGYBEAM, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_NEYTRON_GUN;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	case TYRIANGUN_FLAK_CANNON:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_4DEGREES;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnAccuracy = VECTOR_CONE_6DEGREES;
		else
			m_iWpnAccuracy = VECTOR_CONE_10DEGREES;

		m_pPlayer->m_iKillGunType = TYRIANGUN_FLAK_CANNON;
		m_fWpnDmg = gSkillData.DmgFlakCannon + (gSkillData.DmgFlakCannon * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		FireBullets(8, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 8 + gpGlobals->v_right * 1.5f + gpGlobals->v_up * -1.2, m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES), m_iWpnAccuracy, NULL, g_psv_zmax->value, BULLET_FLAK_CANNON, m_fWpnDmg, DMG_MORTAR, this, m_pPlayer, m_pPlayer->random_seed);
		m_fNextShoot = FIRERATE_FLAK_CANNON;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*4.0):(m_fWpnDmg*8.0);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.25;
		m_pPlayer->m_iKillGunType = TYRIANGUN_NONE;
	break;

	//projectile weapons

	case TYRIANGUN_M203: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_M203GRENADE*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_M203GRENADE*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_M203GRENADE;

		m_fWpnDmg = gSkillData.DmgM203Grenade + (gSkillData.DmgM203Grenade * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CM203Grenade::ShootM203Grenade(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 12 + gpGlobals->v_right * 6.0f + gpGlobals->v_up * -2.5, m_iWpnProjAccuracy, m_fWpnDmg, m_pPlayer->IsBot()?0:1);
		m_fNextShoot = FIRERATE_M203;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.9;
	break;

	case TYRIANGUN_30MMGRENADE: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_30MMGRENADE*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_30MMGRENADE*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_30MMGRENADE;

		m_fWpnDmg = gSkillData.Dmg30mmGrenade + (gSkillData.Dmg30mmGrenade * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		C30mmGrenade::ShootContact(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg, m_pPlayer->IsBot()?0:1);
		m_fNextShoot = FIRERATE_30MMGRENADE;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.8;
	break;

	case TYRIANGUN_NEEDLE_LASER:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_NEEDLE_LASER*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_NEEDLE_LASER*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_NEEDLE_LASER;

		m_fWpnDmg = gSkillData.DmgNeedleLaser + (gSkillData.DmgNeedleLaser * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CNeedleLaser::ShootNeedleLaser(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 12 + gpGlobals->v_right * 6.0f + gpGlobals->v_up * -2.5, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_NEEDLE_LASER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
	break;

	case TYRIANGUN_SHOCK_LASER: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_SHOCK_LASER*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_SHOCK_LASER*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_SHOCK_LASER;

		m_fWpnDmg = gSkillData.DmgShockLaser + (gSkillData.DmgShockLaser * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CShockLaser::ShootShockLaser(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_SHOCK_LASER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
	break;

	case TYRIANGUN_BIORIFLE:
		{
			if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
				m_iWpnProjAccuracy = FIRESPREAD_BIORIFLE*ZOOM_ACCURACY_MODYFIER;
			else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
				m_iWpnProjAccuracy = FIRESPREAD_BIORIFLE*ACCURACY_MODYFIER;
			else
				m_iWpnProjAccuracy = FIRESPREAD_BIORIFLE;

			int BabyesAmount = 0;
			int iRand = RANDOM_LONG(0,0x7FFF); //20% chance to launch huge blob
			if (iRand < (0x7fff/5))
				BabyesAmount = 4;

			m_fWpnDmg = gSkillData.DmgBioRifle + (gSkillData.DmgBioRifle * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
			CAcidBlob::ShootTimed(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, RANDOM_FLOAT(4,5), m_fWpnDmg, BabyesAmount, m_pPlayer->IsBot()?0:1);
			m_fNextShoot = FIRERATE_BIORIFLE;
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
			m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.95;
		}
	break;

	case TYRIANGUN_HELL_HOUNDER: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_HELL_HOUNDER*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_HELL_HOUNDER*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_HELL_HOUNDER;

		m_fWpnDmg = gSkillData.DmgHellHounder + (gSkillData.DmgHellHounder * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CHellHounder::ShootHellHounder(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_HELL_HOUNDER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
	break;

	case TYRIANGUN_MINI_MISSILE: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_MINI_MISSILE*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_MINI_MISSILE*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_MINI_MISSILE;

		m_fWpnDmg = gSkillData.DmgMiniMissile + (gSkillData.DmgMiniMissile * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CMiniMissile::ShootMiniMissile(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_MINI_MISSILE;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_BIOHAZARD_MISSILE: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_BIOHAZARD_MISSILE*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_BIOHAZARD_MISSILE*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_BIOHAZARD_MISSILE;

		m_fWpnDmg = gSkillData.DmgBioMissile + (gSkillData.DmgBioMissile * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CBioMissile::ShootBioMissile(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_BIOHAZARD_MISSILE;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_GLUON:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_GLUONGUN*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_GLUONGUN*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_GLUONGUN;

		m_fWpnDmg = gSkillData.DmgGluonBall + (gSkillData.DmgGluonBall * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CGluonBall::ShootGluonBall(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_GLUON;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_TRIDENT:
		{
			if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
				m_iWpnProjAccuracy = FIRESPREAD_TRIDENT*ZOOM_ACCURACY_MODYFIER;
			else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
				m_iWpnProjAccuracy = FIRESPREAD_TRIDENT*ACCURACY_MODYFIER;
			else
				m_iWpnProjAccuracy = FIRESPREAD_TRIDENT;

			m_fWpnDmg = gSkillData.DmgTrident + (gSkillData.DmgTrident * (m_pPlayer->m_iItemWeaponPowerAug*0.1));

			float angle = -M_PI/2;
			for (int k = 0; k < 3; k++)
			{
				CTrident::ShootTrident(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 16 + gpGlobals->v_right * 2.5f + gpGlobals->v_up * -1.0 + gpGlobals->v_up*(4*sin(angle)) + gpGlobals->v_right*(4*cos(angle)), gpGlobals->v_forward*SPEED_TRIDENT + gpGlobals->v_up*(m_iWpnProjAccuracy*sin(angle))+gpGlobals->v_right*(m_iWpnProjAccuracy*cos(angle)), m_fWpnDmg);
				angle += 2*M_PI/3;
			}		
			m_fNextShoot = FIRERATE_TRIDENT;
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?((m_fWpnDmg*3)*AMMOUSE_MODYFIER):(m_fWpnDmg*1.5);
			m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
		}
	break;

	case TYRIANGUN_PLASMA_STORM:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_PLASMASTORM*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_PLASMASTORM*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_PLASMASTORM;

		m_fWpnDmg = gSkillData.DmgPlasmaStorm + (gSkillData.DmgPlasmaStorm * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CPlasmaStorm::ShootPlasmaStorm(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_PLASMA_STORM;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_PLASMA: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_PLASMA*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_PLASMA*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_PLASMA;

		m_fWpnDmg = gSkillData.DmgPlasma + (gSkillData.DmgPlasma * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CPlasmaBall::ShootPlasmaBall(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_PLASMA;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.5;
	break;

	case TYRIANGUN_HELLFIRE: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_HELLFIRE*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_HELLFIRE*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_HELLFIRE;

		m_fWpnDmg = gSkillData.DmgHellFire + (gSkillData.DmgHellFire * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CHellFire::ShootHellFire(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_HELL_FIRE;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_PULSE_CANNON:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_PULSE_CANNON*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_PULSE_CANNON*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_PULSE_CANNON;

		m_fWpnDmg = gSkillData.DmgPulseCannon + (gSkillData.DmgPulseCannon * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CPulseCannon::ShootPulseCannon(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_PULSE_CANNON;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.6;
	break;

	case TYRIANGUN_MULTI_CANNON: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_MULTI_CANNON*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_MULTI_CANNON*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_MULTI_CANNON;

		m_fWpnDmg = gSkillData.DmgMultiCannon + (gSkillData.DmgMultiCannon * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CMultiCannon::ShootMultiCannon(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_MULTI_CANNON;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_SCORCHER: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_SCORCHER*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_SCORCHER*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_SCORCHER;

		m_fWpnDmg = gSkillData.DmgScorcher + (gSkillData.DmgScorcher * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CScorcher::ShootScorcher(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_SCORCHER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_FROSTER:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_FROSTER*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_FROSTER*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_FROSTER;

		m_fWpnDmg = gSkillData.DmgFrostBall + (gSkillData.DmgFrostBall * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CFrostBall::ShootFrostBall(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward*12 + gpGlobals->v_right * 3.0f + gpGlobals->v_up * -1.5f, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_FROSTER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.0;
	break;

	case TYRIANGUN_SONIC_WAVE: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_SONIC_WAVE*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_SONIC_WAVE*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_SONIC_WAVE;

		m_fWpnDmg = gSkillData.DmgSonicWave + (gSkillData.DmgSonicWave * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CSonicWave::ShootSonicWave(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_SONIC_WAVE;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_CHARGE_CANNON: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_CHARGE_CANNON*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_CHARGE_CANNON*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_CHARGE_CANNON;

		m_fWpnDmg = gSkillData.DmgChargeCannon + (gSkillData.DmgChargeCannon * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CChargeCannon::ShootChargeCannon(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_CHARGE_CANNON;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
	break;

	case TYRIANGUN_DEMOLITION_MISSILE:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_DEMOLITION_MISSILE*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_DEMOLITION_MISSILE*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_DEMOLITION_MISSILE;

		m_fWpnDmg = gSkillData.DmgDemolitionMissile + (gSkillData.DmgDemolitionMissile * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CDemolitionMissile::ShootDemolitionMissile(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_DEMOLITION_MISSILE;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_GHOST_MISSILE: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_GHOST_MISSILE*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_GHOST_MISSILE*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_GHOST_MISSILE;

		m_fWpnDmg = gSkillData.DmgGhostMissile + (gSkillData.DmgGhostMissile * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CGhostMissile::ShootGhostMissile(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 12 + gpGlobals->v_right * 3.5f + gpGlobals->v_up * 1.0f, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_GHOST_MISSILE;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;

	case TYRIANGUN_HVR_MISSILE: 
		CHVRMissile *pHVRMissile;
		m_fWpnDmg = gSkillData.DmgHvrMissile + (gSkillData.DmgHvrMissile * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		pHVRMissile = CHVRMissile::ShootHVRMissile(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_fWpnDmg);

		if (m_fSpotActive && m_hLaserSpot.Get())
			pHVRMissile->pev->enemy = m_hLaserSpot->edict();

		m_fNextShoot = FIRERATE_HVR_MISSILE;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
		UpdateSpot();
	break;

	case TYRIANGUN_RIPPER: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_RIPPER*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_RIPPER*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_RIPPER;

		m_fWpnDmg = gSkillData.DmgRipper + (gSkillData.DmgRipper * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CRipper::ShootRipper(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 8 + gpGlobals->v_right * 8.0f + gpGlobals->v_up * -2.0, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_RIPPER;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.6;
	break;

	case TYRIANGUN_MICRO_MISSILE: 

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_MICRO_MISSILE*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_MICRO_MISSILE*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_MICRO_MISSILE;

		m_fWpnDmg = gSkillData.DmgMicroMissile + (gSkillData.DmgMicroMissile * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CMicroMissile::ShootMicroMissile(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward*GetBarrelLength() + gpGlobals->v_right * 3.0f + gpGlobals->v_up * -1.5f, m_pPlayer->m_hAutoaimTarget, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_MICRO_MISSILE;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
	break;

	case TYRIANGUN_DISRUPTOR: 
		{
			m_fWpnDmg = gSkillData.DmgDisruptor + (gSkillData.DmgDisruptor * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
			CDisruptorBall::ShootDisruptorBall(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 15 + gpGlobals->v_right * 2.5f + gpGlobals->v_up * -1.0, 0, m_fWpnDmg);

			float angle = -M_PI/2;
			for (int k = 1; k < 3; k++)
			{
				CDisruptorBall::ShootDisruptorBall(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward * 15 + gpGlobals->v_right * 2.5f + gpGlobals->v_up * -1.0, k, m_fWpnDmg);
				angle += 2*M_PI/3;
			}
			
			m_fNextShoot = FIRERATE_DISRUPTOR;
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg):(m_fWpnDmg*2);
			m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
		}
	break;

	case TYRIANGUN_STARBURST:

		if (m_fInZoom && m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_STARBURST*ZOOM_ACCURACY_MODYFIER;
		else if (m_fInZoom || m_pPlayer->m_iItemAccuracy)
			m_iWpnProjAccuracy = FIRESPREAD_STARBURST*ACCURACY_MODYFIER;
		else
			m_iWpnProjAccuracy = FIRESPREAD_STARBURST;

		m_fWpnDmg = gSkillData.DmgStarBurst + (gSkillData.DmgStarBurst * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CStarBurst::ShootStarBurst(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_iWpnProjAccuracy, m_fWpnDmg);
		m_fNextShoot = FIRERATE_STARBURST;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.85;
	break;
	}

	if (m_iGunType != TYRIANGUN_TWIN_LASER)
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), TYRIAN_WEAPON_ANIM_FIRE, m_iGunType, 0);

	if (m_pPlayer->IsBot() && bot_random_powerup_lvl.value > 0.0f)
		m_iPowerUse = AMMO_USE_BOT_WEAPON;
	else
		m_pPlayer->m_iFlashBattery -= m_iPowerUse;

	UpdateEnergyStatus();

	if (!m_pPlayer->m_iItemRapidFire)
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + m_fNextShoot;
	else
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + (m_fNextShoot * RAPIDFIRE_MODYFIER);
}

void CTyrianGun::FireSuperWeapon(void)
{
	Vector vecAng = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	Vector vecSrc = gpGlobals->v_forward*GetBarrelLength() + gpGlobals->v_right * 5.0f + gpGlobals->v_up * -3.0;
	UTIL_MakeVectors(vecAng);

	switch(m_iSuperWeapon)
	{
		case TYRIANGUN_SWEAPON_GUIDED_BOMB: 
			CGuidedBomb::ShootGuidedBomb(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, (m_pPlayer->m_iItemAccuracy)?(FIRESPREAD_GUIDED_BOMB*ACCURACY_MODYFIER):FIRESPREAD_GUIDED_BOMB, 4, gSkillData.DmgGuidedBomb + (gSkillData.DmgGuidedBomb * (m_pPlayer->m_iItemWeaponPowerAug*0.1)));
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?(POWER_MAX_CHARGE_GUIDED_BOMB - POWER_MAX_CHARGE):((POWER_MAX_CHARGE_GUIDED_BOMB - POWER_MAX_CHARGE)*2.0);
			m_iSWAmmoUse = m_pPlayer->m_iItemAccuracy?(AMMO_USE_GUIDED_BOMB*AMMOUSE_MODYFIER):(AMMO_USE_GUIDED_BOMB) ;
		break;

		case TYRIANGUN_SWEAPON_PROTON_MISSILE: 
			CProtonMissile::ShootProtonMissile(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->m_hAutoaimTarget, gSkillData.DmgProtonMissile + (gSkillData.DmgProtonMissile * (m_pPlayer->m_iItemWeaponPowerAug*0.1)));
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?(POWER_MAX_CHARGE_PROTON_MISSILE - POWER_MAX_CHARGE):((POWER_MAX_CHARGE_PROTON_MISSILE - POWER_MAX_CHARGE)*2.0);
			m_iSWAmmoUse = m_pPlayer->m_iItemAccuracy?(AMMO_USE_PROTON_MISSILE*AMMOUSE_MODYFIER):(AMMO_USE_PROTON_MISSILE) ;
		break;

		case TYRIANGUN_SWEAPON_DISPLACER: 
			CTeleporter::Fire(m_pPlayer, m_pPlayer->m_hAutoaimTarget, m_pPlayer->GetGunPosition() + vecSrc, gSkillData.DmgTeleporter + (gSkillData.DmgTeleporter * (m_pPlayer->m_iItemWeaponPowerAug*0.1)));
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?(POWER_MAX_CHARGE_DISPLACER - POWER_MAX_CHARGE):((POWER_MAX_CHARGE_DISPLACER - POWER_MAX_CHARGE)*2.0);
			m_iSWAmmoUse = m_pPlayer->m_iItemAccuracy?(AMMO_USE_DISPLACER*AMMOUSE_MODYFIER):(AMMO_USE_DISPLACER) ;
		break;

		case TYRIANGUN_SWEAPON_SHOCK_WAVE: 
			CLightningBall::Fire(m_pPlayer, m_pPlayer->m_hAutoaimTarget, m_pPlayer->GetGunPosition() + vecSrc, gSkillData.DmgLightningBall + (gSkillData.DmgLightningBall * (m_pPlayer->m_iItemWeaponPowerAug*0.1)), gSkillData.DmgLightningBeam + (gSkillData.DmgLightningBeam * (m_pPlayer->m_iItemWeaponPowerAug*0.1)));
			CShockWave::ShootShockWave(m_pPlayer, m_pPlayer->GetGunPosition(), gSkillData.DmgShockWave + (gSkillData.DmgShockWave * (m_pPlayer->m_iItemWeaponPowerAug*0.1)));
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?(POWER_MAX_CHARGE_SHOCK_WAVE - POWER_MAX_CHARGE):((POWER_MAX_CHARGE_SHOCK_WAVE - POWER_MAX_CHARGE)*2.0);
			m_iSWAmmoUse = m_pPlayer->m_iItemAccuracy?(AMMO_USE_SHOCK_WAVE*AMMOUSE_MODYFIER):(AMMO_USE_SHOCK_WAVE) ;
		break;

		case TYRIANGUN_SWEAPON_NUCLEAR_LAUNCHER: 
			CNuclearMissile::ShootNuclearMissile(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->m_hAutoaimTarget, gSkillData.DmgNuclearMissile + (gSkillData.DmgNuclearMissile * (m_pPlayer->m_iItemWeaponPowerAug*0.1)));
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?(POWER_MAX_CHARGE_NUCLEAR_MISSILE - POWER_MAX_CHARGE):((POWER_MAX_CHARGE_NUCLEAR_MISSILE - POWER_MAX_CHARGE)*2.0);
			m_iSWAmmoUse = m_pPlayer->m_iItemAccuracy?(AMMO_USE_NUCLEAR_MISSILE*AMMOUSE_MODYFIER):(AMMO_USE_NUCLEAR_MISSILE) ;
		break;

		case TYRIANGUN_SWEAPON_CHRONOSCEPTRE: 
			CAntimatherialMissile::ShootAntimatherialMissile(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, m_pPlayer->m_hAutoaimTarget, gSkillData.DmgAntimatherialMissile + (gSkillData.DmgAntimatherialMissile * (m_pPlayer->m_iItemWeaponPowerAug*0.1)));
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?(POWER_MAX_CHARGE_CHRONOSCEPTRE - POWER_MAX_CHARGE):((POWER_MAX_CHARGE_CHRONOSCEPTRE - POWER_MAX_CHARGE)*2.0);
			m_iSWAmmoUse = m_pPlayer->m_iItemAccuracy?(AMMO_USE_CHRONOSCEPTRE*AMMOUSE_MODYFIER):(AMMO_USE_CHRONOSCEPTRE) ;
		break;

		case TYRIANGUN_SWEAPON_SUN_OF_GOD: 
			CSunOfGod::ShootSunOfGod(m_pPlayer, m_pPlayer->GetGunPosition() + vecSrc, gSkillData.DmgSunOfGod + (gSkillData.DmgSunOfGod * (m_pPlayer->m_iItemWeaponPowerAug*0.1)));
			m_iPowerUse = m_pPlayer->m_iItemAccuracy?(POWER_MAX_CHARGE_SUN_OF_GOD - POWER_MAX_CHARGE):((POWER_MAX_CHARGE_SUN_OF_GOD - POWER_MAX_CHARGE)*2.0);
			m_iSWAmmoUse = m_pPlayer->m_iItemAccuracy?(AMMO_USE_SUN_OF_GOD*AMMOUSE_MODYFIER):(AMMO_USE_SUN_OF_GOD) ;
		break;
	}
	FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), TYRIAN_WEAPON_ANIM_FIRE, m_iSuperWeapon, 0);

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= m_iSWAmmoUse;
	m_pPlayer->m_iFlashBattery -= m_iPowerUse;
	UpdateEnergyStatus();
	CheckForSWAmmo();
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0;
	m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.25;
}

void CTyrianGun::FireBananaWeapon(bool PriFire)
{
	Vector vecAng = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors(vecAng);

	if (PriFire)
	{
		m_fWpnDmg = gSkillData.DmgBanana + (gSkillData.DmgBanana * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CBanana::ShootBanana(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward*16 + gpGlobals->v_right * 0.75f + gpGlobals->v_up * -1.0, (m_pPlayer->m_iItemAccuracy)?(FIRESPREAD_BANANA*ACCURACY_MODYFIER):FIRESPREAD_BANANA, m_fWpnDmg);
		m_fNextShoot = FIRERATE_BANANA;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), TYRIAN_WEAPON_ANIM_FIRE, TYRIANGUN_BANANA);
	}
	else
	{
		m_fWpnDmg = gSkillData.DmgToilet + (gSkillData.DmgToilet * (m_pPlayer->m_iItemWeaponPowerAug*0.1));
		CToilet::ShootToilet(m_pPlayer, m_pPlayer->GetGunPosition() + gpGlobals->v_forward*16 + gpGlobals->v_right * 2.5f + gpGlobals->v_up * -1.5, (m_pPlayer->m_iItemAccuracy)?(FIRESPREAD_TOILET*ACCURACY_MODYFIER):FIRESPREAD_TOILET, m_fWpnDmg, m_pPlayer->IsBot()?0:1);
		m_fNextShoot = FIRERATE_TOILET;
		m_iPowerUse = m_pPlayer->m_iItemAccuracy?(m_fWpnDmg*AMMOUSE_MODYFIER):(m_fWpnDmg);
		FX_FireGun(m_pPlayer->pev->v_angle, m_pPlayer->entindex(), TYRIAN_WEAPON_ANIM_FIRE, TYRIANGUN_TOILET);
	}

	m_pPlayer->m_iFlashBattery -= m_iPowerUse;
	UpdateEnergyStatus();

	if (!m_pPlayer->m_iItemRapidFire)
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + m_fNextShoot;
	else
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + (m_fNextShoot * RAPIDFIRE_MODYFIER);

	m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 0.75;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase()+0.1;
}

void CTyrianGun::StartZoom(void)
{
	int flags = 0;
#if defined(CLIENT_WEAPONS)
	flags = FEV_CLIENT;
#else
	flags = FEV_HOSTONLY;
#endif
	if (m_fInZoom)
	{
		if (pev->pushmsec == 0)
		{
			PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usZoom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 0, 0, 0, 0, 0, 0);
			m_fInZoom = FALSE;
		}
	}
	else
	{
		PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usZoom, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, 4.0, 0, m_iZC, 0, 1, kRenderTransAdd);
		m_pPlayer->ResetAutoaim();
		m_fInZoom = TRUE;
		pev->pushmsec = 1;
	}
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.5f;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5f;
}

void CTyrianGun::SwitchToSWeapon(void)
{
	b_UpdateRecharge = FALSE;
	if (m_fInSuperWeapon && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < AMMO_USE_GUIDED_BOMB)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Not enough power for super weapon!");
		m_fInSuperWeapon = FALSE;

		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.5f;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.2;

		if (m_pPlayer->m_pActiveItem)
		{
			m_pPlayer->m_pActiveItem->Holster();
			m_pPlayer->m_pNextItem = m_pPlayer->m_pActiveItem;
			m_pPlayer->m_pActiveItem = NULL;
		}		
		return;
	}

	if (m_fInSuperWeapon)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Super Weapon Deactivated!");
		m_fInSuperWeapon = FALSE;
		b_UpdateRecharge = TRUE;
	}
	else
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Super Weapon Enabled!");
		m_fInSuperWeapon = TRUE;
		b_UpdateRecharge = FALSE;
	}

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.5f;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 2.5f;
	m_fIdleAnimDelay = UTIL_WeaponTimeBase() + 1.2;

	if (m_pPlayer->m_pActiveItem)
	{
		m_pPlayer->m_pActiveItem->Holster();
		m_pPlayer->m_pNextItem = m_pPlayer->m_pActiveItem;
		m_pPlayer->m_pActiveItem = NULL;
	}
}

BOOL CTyrianGun::ForceAutoaim(void)
{
	if (m_fInSuperWeapon && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= AMMO_USE_GUIDED_BOMB)
		return TRUE;

	if (m_iGunType == TYRIANGUN_MICRO_MISSILE)
		return TRUE;

	return FALSE;
}

void CTyrianGun::WeaponIdle( void )
{
	if (m_fInZoom)
		pev->pushmsec = 0;

	// if we have a critical health level, the weapon will blast
	if (!m_pPlayer->m_iWpnDestroyed && m_pPlayer->pev->health <= MAX_PLAYER_HEALTH*0.1 && !m_pPlayer->m_iItemBanana && !m_pPlayer->m_iItemInvulnerability && !m_pPlayer->m_iItemPlasmaShield)
	{
		if (m_fInZoom)
			StartZoom();

		MESSAGE_BEGIN(MSG_ONE, gmsgFlashBattery, NULL, ENT(m_pPlayer->pev));
		WRITE_SHORT(m_pPlayer->m_iFlashBattery);
		WRITE_BYTE(TYRIANGUN_NONE);
		MESSAGE_END();

		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "The Gun Destroyed!");
		m_pPlayer->pev->viewmodel = iStringNull;
		m_pPlayer->pev->weaponmodel = iStringNull;
		FX_Trail(pev->origin, m_pPlayer->entindex(), FX_PLAYER_WEAPON_BLAST);
		m_pPlayer->m_iWpnDestroyed = TRUE;
		m_pPlayer->m_iItemWeaponPowerAug = WEAPON_IS_DEAD;

		MESSAGE_BEGIN(MSG_ONE, gmsgMSGManager, NULL, ENT(m_pPlayer->pev));
		WRITE_BYTE(MSG_ITEM_WEAPON_POWER);
		WRITE_SHORT(m_pPlayer->m_iItemWeaponPowerAug);
		MESSAGE_END();
		return;
	}

	if (m_fInSuperWeapon && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] && (m_pPlayer->m_iFlashBattery >= POWER_MAX_CHARGE_PROTON_MISSILE && m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE_SUN_OF_GOD))
		m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	else if (m_iGunType == TYRIANGUN_MICRO_MISSILE)
		m_pPlayer->GetAutoaimVector(AUTOAIM_1DEGREES);
	else
		m_pPlayer->ResetAutoaim();


	if (m_iGunType == TYRIANGUN_HVR_MISSILE && !m_fInSuperWeapon && !m_pPlayer->m_iItemBanana)
	{
		m_fSpotActive = 1;
		UpdateSpot();
	}
	else
	{
		if (m_hLaserSpot.Get())
		{
			m_fSpotActive = 0;
			m_cActiveRockets = 0;
			m_hLaserSpot->Killed(NULL, NULL, GIB_NEVER);
			m_hLaserSpot = NULL;
		}
	}

	if (m_flEnergyUpdateTime < gpGlobals->time)
	{
		UpdateEnergyStatus();
		RestoreEnergy();
		m_flEnergyUpdateTime = UTIL_WeaponTimeBase() + 0.25;
	}
	if (m_iGunType && m_fIdleAnimDelay < gpGlobals->time && !FBitSet(m_pPlayer->pev->flags, FL_FROZEN))
	{
		SendWeaponAnim(TYRIAN_WEAPON_ANIM_IDLE);
		m_fIdleAnimDelay = UTIL_WeaponTimeBase() + RANDOM_FLOAT(12, 16);
	}
}

//===============PURPOSE===============
//Restores server-side energy level
//=====================================
void CTyrianGun::RestoreEnergy(void)
{
	if (!m_pPlayer->m_iGenerator)
	return;

	if (m_pPlayer->m_fFrozen)
	return;

	//if Super weapon is activated, don't charge more, than current weapon requires!
	if (m_fInSuperWeapon && m_iSuperWeapon)
	{
		m_pPlayer->m_iFlashBattery += m_pPlayer->m_iGeneratorPower;
		if (m_pPlayer->m_iFlashBattery > m_iInSWMaxCharge)
		{
			m_pPlayer->m_iFlashBattery = m_iInSWMaxCharge;
			b_UpdateRecharge = FALSE;
		}
		return;
	}

	// regular weapon mode
	int SWAmmo = m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType];

	if (SWAmmo < AMMO_USE_GUIDED_BOMB && m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE || m_pPlayer->m_iItemBanana || mp_bananamode.value > 0.0f)
	{
		m_pPlayer->m_iFlashBattery += m_pPlayer->m_iGeneratorPower;

		if (m_pPlayer->m_iFlashBattery > POWER_MAX_CHARGE)
		{
			m_pPlayer->m_iFlashBattery = POWER_MAX_CHARGE;
			b_UpdateRecharge = FALSE;
		}
	}
	else if (SWAmmo >= AMMO_USE_GUIDED_BOMB && SWAmmo < AMMO_USE_PROTON_MISSILE && m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE_GUIDED_BOMB)
	{
		m_pPlayer->m_iFlashBattery += m_pPlayer->m_iGeneratorPower;
		if (m_pPlayer->m_iFlashBattery > POWER_MAX_CHARGE_GUIDED_BOMB)
		{
			m_pPlayer->m_iFlashBattery = POWER_MAX_CHARGE_GUIDED_BOMB;
			b_UpdateRecharge = FALSE;
		}
	}
	else if (SWAmmo >= AMMO_USE_PROTON_MISSILE && SWAmmo < AMMO_USE_DISPLACER && m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE_PROTON_MISSILE)
	{
		m_pPlayer->m_iFlashBattery += m_pPlayer->m_iGeneratorPower;
		if (m_pPlayer->m_iFlashBattery > POWER_MAX_CHARGE_PROTON_MISSILE)
		{
			m_pPlayer->m_iFlashBattery = POWER_MAX_CHARGE_PROTON_MISSILE;
			b_UpdateRecharge = FALSE;
		}
	}
	else if (SWAmmo >= AMMO_USE_DISPLACER && SWAmmo < AMMO_USE_SHOCK_WAVE && m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE_DISPLACER)
	{
		m_pPlayer->m_iFlashBattery += m_pPlayer->m_iGeneratorPower;
		if (m_pPlayer->m_iFlashBattery > POWER_MAX_CHARGE_DISPLACER)
		{
			m_pPlayer->m_iFlashBattery = POWER_MAX_CHARGE_DISPLACER;
			b_UpdateRecharge = FALSE;
		}
	}
	else if (SWAmmo >= AMMO_USE_SHOCK_WAVE && SWAmmo < AMMO_USE_NUCLEAR_MISSILE && m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE_SHOCK_WAVE)
	{
		m_pPlayer->m_iFlashBattery += m_pPlayer->m_iGeneratorPower;
		if (m_pPlayer->m_iFlashBattery > POWER_MAX_CHARGE_SHOCK_WAVE)
		{
			m_pPlayer->m_iFlashBattery = POWER_MAX_CHARGE_SHOCK_WAVE;
			b_UpdateRecharge = FALSE;
		}
	}
	else if (SWAmmo >= AMMO_USE_NUCLEAR_MISSILE && SWAmmo < AMMO_USE_CHRONOSCEPTRE && m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE_NUCLEAR_MISSILE)
	{
		m_pPlayer->m_iFlashBattery += m_pPlayer->m_iGeneratorPower;
		if (m_pPlayer->m_iFlashBattery > POWER_MAX_CHARGE_NUCLEAR_MISSILE)
		{
			m_pPlayer->m_iFlashBattery = POWER_MAX_CHARGE_NUCLEAR_MISSILE;
			b_UpdateRecharge = FALSE;
		}
	}
	else if (SWAmmo >= AMMO_USE_CHRONOSCEPTRE && SWAmmo < AMMO_USE_SUN_OF_GOD && m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE_CHRONOSCEPTRE)
	{
		m_pPlayer->m_iFlashBattery += m_pPlayer->m_iGeneratorPower;
		if (m_pPlayer->m_iFlashBattery > POWER_MAX_CHARGE_CHRONOSCEPTRE)
		{
			m_pPlayer->m_iFlashBattery = POWER_MAX_CHARGE_CHRONOSCEPTRE;
			b_UpdateRecharge = FALSE;
		}
	}
	else if (SWAmmo >= AMMO_USE_SUN_OF_GOD && m_pPlayer->m_iFlashBattery < POWER_MAX_CHARGE_SUN_OF_GOD)
	{
		m_pPlayer->m_iFlashBattery += m_pPlayer->m_iGeneratorPower;
		if (m_pPlayer->m_iFlashBattery > POWER_MAX_CHARGE_SUN_OF_GOD)
		{
			m_pPlayer->m_iFlashBattery = POWER_MAX_CHARGE_SUN_OF_GOD;
			b_UpdateRecharge = FALSE;
		}
	}
}

//===============PURPOSE===============
//Updates client-side HUD data of energy level
//=====================================
void CTyrianGun::UpdateEnergyStatus(void)
{
	if (!b_UpdateRecharge)
		return;

	m_pPlayer->m_iGeneratorPower = GENERATOR_PROVIDE_POWER + (GENERATOR_PROVIDE_POWER * m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType]);

	if (m_pPlayer->IsBot())
		return;

	if (m_pPlayer->m_iFlashBattery < 0)
		m_pPlayer->m_iFlashBattery = 0;

	MESSAGE_BEGIN(MSG_ONE, gmsgFlashBattery, NULL, ENT(m_pPlayer->pev));
		WRITE_SHORT(m_pPlayer->m_iFlashBattery);
		if (mp_bananamode.value > 0.0f || m_pPlayer->m_iItemBanana)
			WRITE_BYTE(TYRIANGUN_BANANA);
		else
			WRITE_BYTE(m_iGunType);
	MESSAGE_END();
}

//===============PURPOSE===============
//Chooses super weapon, depends on current energy level
//=====================================
void CTyrianGun::CheckForSWEnergy(void)
{
	int m_iBat = m_pPlayer->m_iFlashBattery;

	if (m_iBat >= POWER_MAX_CHARGE_SUN_OF_GOD)
		m_iSuperWeapon = TYRIANGUN_SWEAPON_SUN_OF_GOD;
	else if (m_iBat >= POWER_MAX_CHARGE_CHRONOSCEPTRE)
		m_iSuperWeapon = TYRIANGUN_SWEAPON_CHRONOSCEPTRE;
	else if (m_iBat >= POWER_MAX_CHARGE_NUCLEAR_MISSILE)
		m_iSuperWeapon = TYRIANGUN_SWEAPON_NUCLEAR_LAUNCHER;
	else if (m_iBat >= POWER_MAX_CHARGE_SHOCK_WAVE)
		m_iSuperWeapon = TYRIANGUN_SWEAPON_SHOCK_WAVE;
	else if (m_iBat >= POWER_MAX_CHARGE_DISPLACER)
		m_iSuperWeapon = TYRIANGUN_SWEAPON_DISPLACER;
	else if (m_iBat >= POWER_MAX_CHARGE_PROTON_MISSILE)
		m_iSuperWeapon = TYRIANGUN_SWEAPON_PROTON_MISSILE;
	else if (m_iBat >= POWER_MAX_CHARGE_GUIDED_BOMB)
		m_iSuperWeapon = TYRIANGUN_SWEAPON_GUIDED_BOMB;
}

//===============PURPOSE===============
//Is it enough Ammo for the second shot from the Super Weapon?
//=====================================
void CTyrianGun::CheckForSWAmmo(void)
{
	int iSWAmmo = m_pPlayer->m_rgAmmo[GetAmmoIndexFromRegistry("energy_core")];

	if (iSWAmmo < m_iSWAmmoUse)
	{
		m_iInSWMaxCharge = POWER_MAX_CHARGE;
		b_DeploySuperWeapon = TRUE;
	}
}

void CTyrianGun::UpdateSpot(void)
{
	if (m_fSpotActive)
	{
		if (m_hLaserSpot.Get() == NULL)
		{
			m_hLaserSpot = CLaserSpot::CreateSpot();
			pev->skin = 1;
		}
		ASSERT(m_hLaserSpot.Get() != NULL);

		Vector v_forward;
		ANGLE_VECTORS(m_pPlayer->pev->v_angle, v_forward, NULL, NULL);
		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecEnd = vecSrc + v_forward * 8192;
		TraceResult tr;
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ignore_glass, ENT(m_pPlayer->pev), &tr);
		UTIL_SetOrigin(m_hLaserSpot->pev, tr.vecEndPos + tr.vecPlaneNormal * 0.1f);

		if (!(m_hLaserSpot->pev->effects & EF_NODRAW))
		{
			tr.vecPlaneNormal.x *= -1.0f;
			tr.vecPlaneNormal.y *= -1.0f;
			m_hLaserSpot->pev->angles = UTIL_VecToAngles(tr.vecPlaneNormal);
		}
	}
	else
	{
		if (m_hLaserSpot.Get())
		{
			m_hLaserSpot->Killed(NULL, NULL, 0);
			m_hLaserSpot = NULL;
			pev->skin = 0;
		}
	}
}

void CTyrianGun::OnFreePrivateData(void)// XDM3037
{
	if (m_hLaserSpot.Get())
	{
		CBaseEntity *pSpotEnt = (CBaseEntity *)m_hLaserSpot;
		if (pSpotEnt)
			m_hLaserSpot->Killed(NULL, NULL, GIB_NEVER);
	}
	m_hLaserSpot = NULL;
	CBasePlayerWeapon::OnFreePrivateData();
}