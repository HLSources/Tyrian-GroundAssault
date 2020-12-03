//=========================================================
// skill.cpp - code for skill level concerns
// XDM: great resource savings: 1 cvar instead of 3!
//=========================================================
#include "extdll.h"
#include "util.h"
#include "skill.h"

static char SK_NULL[] = "0 0 0";

DLL_GLOBAL int g_iSkillLevel = SKILL_HARD;// >)
DLL_GLOBAL skilldata_t gSkillData;

cvar_t	sk_turret_health = {"sk_turret_health",SK_NULL, FCVAR_EXTDLL, 0, NULL};
cvar_t	sk_miniturret_health = {"sk_miniturret_health",SK_NULL, FCVAR_EXTDLL, 0, NULL};
cvar_t	sk_sentry_health = {"sk_sentry_health",SK_NULL, FCVAR_EXTDLL, 0, NULL};

// PLAYER WEAPONS
cvar_t	dmg_player_lightning_field	= {"dmg_player_lightning_field",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_spider_mine		= {"dmg_spider_mine",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_tripmine		= {"dmg_tripmine",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_airstrike		= {"dmg_airstrike",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_ripper			= {"dmg_ripper",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_m203_grenade	= {"dmg_m203_grenade",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_30mm_grenade	= {"dmg_30mm_grenade",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_machinegun		= {"dmg_machinegun",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_minigun			= {"dmg_minigun",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_vulcan			= {"dmg_vulcan",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_heavy_turret	= {"dmg_heavy_turret",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_laser			= {"dmg_laser",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_twin_laser		= {"dmg_twin_laser",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_gauss			= {"dmg_gauss",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_gluon_ball		= {"dmg_gluon_ball",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_trident			= {"dmg_trident",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_scorcher		= {"dmg_scorcher",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_rail_gun		= {"dmg_rail_gun",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_super_rail_gun	= {"dmg_super_rail_gun",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_needle_laser	= {"dmg_needle_laser",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_shock_laser		= {"dmg_shock_laser",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_shock_wave		= {"dmg_shock_wave",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_shock_rifle		= {"dmg_shock_rifle",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_lightning		= {"dmg_lightning",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_bio_missile		= {"dmg_bio_missile",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_demolition_missile	= {"dmg_demolition_missile",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_hvr_missile		= {"dmg_hvr_missile",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_mini_missile	= {"dmg_mini_missile",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_micro_missile	= {"dmg_micro_missile",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_hell_hounder	= {"dmg_hell_hounder",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_hell_fire		= {"dmg_hell_fire",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_plasma			= {"dmg_plasma",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_plasma_storm	= {"dmg_plasma_storm",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_pulse_cannon	= {"dmg_pulse_cannon",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_mezon_cannon	= {"dmg_mezon_cannon",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_multi_cannon	= {"dmg_multi_cannon",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_tau_cannon		= {"dmg_tau_cannon",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_proton_missile	= {"dmg_proton_missile",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_nuclear_missile	= {"dmg_nuclear_missile",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_antimatherial_missile	= {"dmg_antimatherial_missile",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_sun_of_god		= {"dmg_sun_of_god",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_teleporter		= {"dmg_teleporter",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_lightning_ball	= {"dmg_lightning_ball",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_lightning_beam	= {"dmg_lightning_beam",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_frostball		= {"dmg_frostball",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_sonicwave		= {"dmg_sonicwave",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_chargecannon	= {"dmg_chargecannon",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_ghost_missile	= {"dmg_ghost_missile",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_repeater		= {"dmg_repeater",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_photongun		= {"dmg_photongun",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_bfg				= {"dmg_bfg",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_disruptor		= {"dmg_disruptor",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_guided_bomb		= {"dmg_guided_bomb",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_toilet			= {"dmg_toilet",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_banana			= {"dmg_banana",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_clustergun		= {"dmg_clustergun",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_flak_cannon		= {"dmg_flak_cannon",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_player_explode	= {"dmg_player_explode",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_neytrongun		= {"dmg_neytrongun",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_atom_bomb		= {"dmg_atom_bomb",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_satellite_strike	= {"dmg_satellite_strike",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_star_burst		= {"dmg_star_burst",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_biorifle		= {"dmg_biorifle",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_beam_splitter	= {"dmg_beam_splitter",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	dmg_flash_bang	= {"dmg_flash_bang",SK_NULL, FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};


// Player damage adjusters
cvar_t	sk_player_head		= {"sk_player_head",	"3 3 3", FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	sk_player_chest		= {"sk_player_chest",	"1 1 1", FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	sk_player_stomach	= {"sk_player_stomach",	"1.5 1.5 1.5", FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	sk_player_arm		= {"sk_player_arm",		"0.9 0.9 0.9", FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	sk_player_leg		= {"sk_player_leg",		"0.8 0.8 0.8", FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};
cvar_t	sk_player_shield	= {"sk_player_shield",	"1 1 1", FCVAR_SPONLY|FCVAR_EXTDLL, 0, NULL};


void SkillRegisterCvars(void)
{
	CVAR_REGISTER(&sk_turret_health);
	CVAR_REGISTER(&sk_miniturret_health);
	CVAR_REGISTER(&sk_sentry_health);


	// PLAYER WEAPONS
	CVAR_REGISTER(&dmg_player_lightning_field);
	CVAR_REGISTER(&dmg_player_explode);
	CVAR_REGISTER(&dmg_spider_mine);
	CVAR_REGISTER(&dmg_tripmine);
	CVAR_REGISTER(&dmg_airstrike);
	CVAR_REGISTER(&dmg_toilet);
	CVAR_REGISTER(&dmg_ripper);
	CVAR_REGISTER(&dmg_hvr_missile);
	CVAR_REGISTER(&dmg_demolition_missile);
	CVAR_REGISTER(&dmg_chargecannon);
	CVAR_REGISTER(&dmg_sonicwave);
	CVAR_REGISTER(&dmg_m203_grenade);
	CVAR_REGISTER(&dmg_30mm_grenade);
	CVAR_REGISTER(&dmg_machinegun);
	CVAR_REGISTER(&dmg_minigun);
	CVAR_REGISTER(&dmg_vulcan);
	CVAR_REGISTER(&dmg_heavy_turret);
	CVAR_REGISTER(&dmg_laser);
	CVAR_REGISTER(&dmg_twin_laser);
	CVAR_REGISTER(&dmg_gauss);
	CVAR_REGISTER(&dmg_tau_cannon);
	CVAR_REGISTER(&dmg_rail_gun);
	CVAR_REGISTER(&dmg_super_rail_gun);
	CVAR_REGISTER(&dmg_trident);
	CVAR_REGISTER(&dmg_gluon_ball);
	CVAR_REGISTER(&dmg_scorcher);
	CVAR_REGISTER(&dmg_shock_laser);
	CVAR_REGISTER(&dmg_shock_wave);
	CVAR_REGISTER(&dmg_needle_laser);
	CVAR_REGISTER(&dmg_bio_missile);
	CVAR_REGISTER(&dmg_proton_missile);
	CVAR_REGISTER(&dmg_mini_missile);
	CVAR_REGISTER(&dmg_micro_missile);
	CVAR_REGISTER(&dmg_hell_hounder);
	CVAR_REGISTER(&dmg_shock_rifle);
	CVAR_REGISTER(&dmg_lightning);
	CVAR_REGISTER(&dmg_plasma);
	CVAR_REGISTER(&dmg_teleporter);
	CVAR_REGISTER(&dmg_plasma_storm);
	CVAR_REGISTER(&dmg_pulse_cannon);
	CVAR_REGISTER(&dmg_multi_cannon);
	CVAR_REGISTER(&dmg_mezon_cannon);
	CVAR_REGISTER(&dmg_hell_fire);
	CVAR_REGISTER(&dmg_nuclear_missile);
	CVAR_REGISTER(&dmg_sun_of_god);
	CVAR_REGISTER(&dmg_antimatherial_missile);
	CVAR_REGISTER(&dmg_ghost_missile);
	CVAR_REGISTER(&dmg_lightning_ball);
	CVAR_REGISTER(&dmg_lightning_beam);
	CVAR_REGISTER(&dmg_frostball);
	CVAR_REGISTER(&dmg_repeater);
	CVAR_REGISTER(&dmg_photongun);
	CVAR_REGISTER(&dmg_bfg);
	CVAR_REGISTER(&dmg_disruptor);
	CVAR_REGISTER(&dmg_guided_bomb);
	CVAR_REGISTER(&dmg_banana);
	CVAR_REGISTER(&dmg_clustergun);
	CVAR_REGISTER(&dmg_flak_cannon);
	CVAR_REGISTER(&dmg_neytrongun);
	CVAR_REGISTER(&dmg_atom_bomb);
	CVAR_REGISTER(&dmg_satellite_strike);
	CVAR_REGISTER(&dmg_star_burst);
	CVAR_REGISTER(&dmg_biorifle);
	CVAR_REGISTER(&dmg_beam_splitter);
	CVAR_REGISTER(&dmg_flash_bang);

	// Player damage adjusters
	CVAR_REGISTER(&sk_player_head);
	CVAR_REGISTER(&sk_player_chest);
	CVAR_REGISTER(&sk_player_stomach);
	CVAR_REGISTER(&sk_player_arm);
	CVAR_REGISTER(&sk_player_leg);
	CVAR_REGISTER(&sk_player_shield);
}

//=========================================================
// take the name of a cvar, tack a digit for the skill level
// on, and return the value.of that Cvar
//=========================================================
//float GetSkillCvar(char *pName)
float GetSkillCvar(const cvar_t *var)
{
	float v1 = 0.0f;
	float v2 = 0.0f;
	float v3 = 0.0f;

	if (sscanf(var->string, "%f %f %f", &v1, &v2, &v3) != 3)
		ALERT(at_console, "GetSkillCVar: error parsing %s!\n", var->name);
	else if (v1 == 0.0f && v2 == 0.0f && v3 == 0.0f)
		ALERT(at_console, "GetSkillCVar: WARNING! Got a zero for %s!\n", var->name);

	if (gSkillData.iSkillLevel == SKILL_EASY)
		return v1;
	else if (gSkillData.iSkillLevel == SKILL_MEDIUM)
		return v2;

	return v3;
}

void SkillUpdateData(int iSkill)
{
	SERVER_COMMAND("exec skill.cfg\n");

	gSkillData.iSkillLevel = iSkill;

	gSkillData.turretHealth = GetSkillCvar(&sk_turret_health);
	gSkillData.miniturretHealth = GetSkillCvar(&sk_miniturret_health);
	gSkillData.sentryHealth = GetSkillCvar(&sk_sentry_health);


	// PLAYER WEAPONS
	gSkillData.DmgPlrLightningField = GetSkillCvar(&dmg_player_lightning_field);
	gSkillData.DmgPlrExplode = GetSkillCvar(&dmg_player_explode);
	gSkillData.DmgNeytronGun = GetSkillCvar(&dmg_neytrongun);
	gSkillData.DmgSpiderMine = GetSkillCvar(&dmg_spider_mine);
	gSkillData.DmgTripMine = GetSkillCvar(&dmg_tripmine);
	gSkillData.DmgAirStrike = GetSkillCvar(&dmg_airstrike);
	gSkillData.DmgSatelliteStrike = GetSkillCvar(&dmg_satellite_strike);
	gSkillData.DmgBanana = GetSkillCvar(&dmg_banana);
	gSkillData.DmgGuidedBomb = GetSkillCvar(&dmg_guided_bomb);
	gSkillData.DmgDisruptor = GetSkillCvar(&dmg_disruptor);
	gSkillData.DmgRipper = GetSkillCvar(&dmg_ripper);
	gSkillData.DmgClusterGun = GetSkillCvar(&dmg_clustergun);
	gSkillData.DmgFlakCannon = GetSkillCvar(&dmg_flak_cannon);
	gSkillData.DmgChargeCannon = GetSkillCvar(&dmg_chargecannon);
	gSkillData.DmgSonicWave = GetSkillCvar(&dmg_sonicwave);
	gSkillData.DmgNeedleLaser = GetSkillCvar(&dmg_needle_laser);
	gSkillData.DmgShockLaser = GetSkillCvar(&dmg_shock_laser);
	gSkillData.DmgShockWave = GetSkillCvar(&dmg_shock_wave);
	gSkillData.DmgM203Grenade = GetSkillCvar(&dmg_m203_grenade);
	gSkillData.Dmg30mmGrenade = GetSkillCvar(&dmg_30mm_grenade);
	gSkillData.DmgGluonBall = GetSkillCvar(&dmg_gluon_ball);
	gSkillData.DmgTrident = GetSkillCvar(&dmg_trident);
	gSkillData.DmgScorcher = GetSkillCvar(&dmg_scorcher);
	gSkillData.DmgBioRifle = GetSkillCvar(&dmg_biorifle);
	gSkillData.DmgTeleporter = GetSkillCvar(&dmg_teleporter);
	gSkillData.DmgLightningBall = GetSkillCvar(&dmg_lightning_ball);
	gSkillData.DmgLightningBeam = GetSkillCvar(&dmg_lightning_beam);
	gSkillData.DmgGhostMissile = GetSkillCvar(&dmg_ghost_missile);
	gSkillData.DmgBioMissile = GetSkillCvar(&dmg_bio_missile);
	gSkillData.DmgToilet = GetSkillCvar(&dmg_toilet);
	gSkillData.DmgDemolitionMissile = GetSkillCvar(&dmg_demolition_missile);
	gSkillData.DmgMiniMissile	= GetSkillCvar(&dmg_mini_missile);
	gSkillData.DmgMicroMissile	= GetSkillCvar(&dmg_micro_missile);
	gSkillData.DmgHvrMissile	= GetSkillCvar(&dmg_hvr_missile);
	gSkillData.DmgHellHounder = GetSkillCvar(&dmg_hell_hounder);
	gSkillData.DmgHellFire = GetSkillCvar(&dmg_hell_fire);
	gSkillData.DmgProtonMissile	= GetSkillCvar(&dmg_proton_missile);
	gSkillData.DmgNuclearMissile	= GetSkillCvar(&dmg_nuclear_missile);
	gSkillData.DmgAntimatherialMissile	= GetSkillCvar(&dmg_antimatherial_missile);
	gSkillData.DmgStarBurst	= GetSkillCvar(&dmg_star_burst);
	gSkillData.DmgFlashBang	= GetSkillCvar(&dmg_flash_bang);
	gSkillData.DmgSunOfGod	= GetSkillCvar(&dmg_sun_of_god);
	gSkillData.DmgFrostBall	= GetSkillCvar(&dmg_frostball);
	gSkillData.DmgAtomBomb	= GetSkillCvar(&dmg_atom_bomb);
	gSkillData.DmgMachinegun = GetSkillCvar(&dmg_machinegun);
	gSkillData.DmgMinigun = GetSkillCvar(&dmg_minigun);
	gSkillData.DmgVulcan = GetSkillCvar(&dmg_vulcan);
	gSkillData.DmgHeavyTurret = GetSkillCvar(&dmg_heavy_turret);
	gSkillData.DmgRailGun = GetSkillCvar(&dmg_rail_gun);
	gSkillData.DmgSuperRailGun = GetSkillCvar(&dmg_rail_gun);
	gSkillData.DmgLaser = GetSkillCvar(&dmg_laser);
	gSkillData.DmgTwinLaser = GetSkillCvar(&dmg_twin_laser);
	gSkillData.DmgGauss = GetSkillCvar(&dmg_gauss);
	gSkillData.DmgLightning = GetSkillCvar(&dmg_lightning);
	gSkillData.DmgShockRifle = GetSkillCvar(&dmg_shock_rifle);
	gSkillData.DmgPlasma = GetSkillCvar(&dmg_plasma);
	gSkillData.DmgPlasmaStorm = GetSkillCvar(&dmg_plasma_storm);
	gSkillData.DmgPulseCannon = GetSkillCvar(&dmg_pulse_cannon);
	gSkillData.DmgMultiCannon = GetSkillCvar(&dmg_multi_cannon);
	gSkillData.DmgTauCannon = GetSkillCvar(&dmg_tau_cannon);
	gSkillData.DmgMezonCannon = GetSkillCvar(&dmg_mezon_cannon);
	gSkillData.DmgRepeater = GetSkillCvar(&dmg_repeater);
	gSkillData.DmgPhotongun = GetSkillCvar(&dmg_photongun);
	gSkillData.DmgBfg = GetSkillCvar(&dmg_bfg);
	gSkillData.DmgBeamSplitter = GetSkillCvar(&dmg_beam_splitter);

	// Player damage adj
	gSkillData.plrHead = GetSkillCvar(&sk_player_head);
	gSkillData.plrChest = GetSkillCvar(&sk_player_chest);
	gSkillData.plrStomach = GetSkillCvar(&sk_player_stomach);
	gSkillData.plrLeg = GetSkillCvar(&sk_player_leg);
	gSkillData.plrArm = GetSkillCvar(&sk_player_arm);
	gSkillData.plrArm = GetSkillCvar(&sk_player_shield);
	gSkillData.plrShield = GetSkillCvar(&sk_player_shield);
}
