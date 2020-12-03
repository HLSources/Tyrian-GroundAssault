//=========================================================
// skill.h - skill level concerns
//=========================================================
#ifndef SKILL_H
#define SKILL_H

#define SKILL_EASY		1
#define SKILL_MEDIUM	2
#define SKILL_HARD		3

struct skilldata_t
{
	int iSkillLevel; // game skill level

	float turretHealth;
	float miniturretHealth;
	float sentryHealth;

	float DmgSatelliteStrike;
	float DmgPlrLightningField;
	float DmgPlrExplode;
	float DmgTripMine;
	float DmgSpiderMine;
	float DmgAirStrike;
	float DmgGuidedBomb;
	float DmgDisruptor;
	float DmgRipper;
	float DmgHvrMissile;
	float DmgGhostMissile;
	float DmgChargeCannon;
	float DmgSonicWave;
	float DmgMicroMissile;
	float DmgM203Grenade;
	float Dmg30mmGrenade;
	float DmgNuclearMissile;
	float DmgShockWave;
	float DmgFlashBang;
	float DmgShockLaser;
	float DmgNeedleLaser;
	float DmgBioMissile;
	float DmgBioRifle;
	float DmgToilet;
	float DmgMiniMissile;
	float DmgHellHounder;
	float DmgHellFire;
	float DmgGluonBall;
	float DmgTrident;
	float DmgScorcher;
	float DmgLightning;
	float DmgShockRifle;
	float DmgPlasma;
	float DmgTeleporter;
	float DmgLightningBall;
	float DmgLightningBeam;
	float DmgMultiCannon;
	float DmgPlasmaStorm;
	float DmgPulseCannon;
	float DmgProtonMissile;
	float DmgDemolitionMissile;
	float DmgAntimatherialMissile;
	float DmgSunOfGod;
	float DmgFrostBall;
	float DmgAtomBomb;
	float DmgStarBurst;
	float DmgMachinegun;
	float DmgMinigun;
	float DmgVulcan;
	float DmgHeavyTurret;
	float DmgGauss;
	float DmgLaser;
	float DmgTwinLaser;
	float DmgRailGun;
	float DmgSuperRailGun;
	float DmgTauCannon;
	float DmgMezonCannon;
	float DmgRepeater;
	float DmgPhotongun;
	float DmgBfg;
	float DmgBanana;
	float DmgClusterGun;
	float DmgFlakCannon;
	float DmgNeytronGun;
	float DmgBeamSplitter;

// player damage adj
	float plrHead;
	float plrChest;
	float plrStomach;
	float plrLeg;
	float plrArm;
	float plrShield;
};

extern DLL_GLOBAL skilldata_t gSkillData;
extern DLL_GLOBAL int g_iSkillLevel;

void SkillRegisterCvars(void);
float GetSkillCvar(const cvar_t *var);
void SkillUpdateData(int iSkill);

#endif // SKILL_H
