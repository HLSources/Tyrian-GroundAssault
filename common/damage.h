//====================================================================
//
// Purpose: damage constants
//
//====================================================================

#ifndef DAMAGE_H
#define DAMAGE_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* !__MINGW32__ */
#endif

// Damage types (bits, can be combined)

#define DMG_GENERIC			0			// generic damage was done
#define DMG_CRUSH			(1 << 0)	// crushed by falling or moving object
#define DMG_BULLET			(1 << 1)	// shot
#define DMG_SLASH			(1 << 2)	// cut, clawed, stabbed
#define DMG_BURN			(1 << 3)	// heat burned
#define DMG_FREEZE			(1 << 4)	// frozen
#define DMG_ENERGYBLAST		(1 << 5)	// energy explosions
#define DMG_BLAST			(1 << 6)	// explosive blast damage
#define DMG_ANNIHILATION	(1 << 7)	// annihilating everything
#define DMG_SHOCK			(1 << 8)	// electric shock
#define DMG_SONIC			(1 << 9)	// sound pulse shockwave
#define DMG_ENERGYBEAM		(1 << 10)	// laser or other high energy beam
#define DMG_BLIND			(1 << 11)	//blinds player (flashbangs)
#define DMG_NEVERGIB		(1 << 12)	// with this bit OR'd in, no damage type will be able to gib victims upon death
#define DMG_ALWAYSGIB		(1 << 13)	// with this bit OR'd in, any damage type can be made to gib victims upon death.
#define DMG_DROWN			(1 << 14)	// drowning
#define DMG_PARALYZE		(1 << 15)	// slows affected creature down
#define DMG_NERVEGAS		(1 << 16)	// nerve toxins, very bad
#define DMG_POISON			(1 << 17)	// blood poisioning
#define DMG_RADIATION		(1 << 18)	// radiation exposure
#define DMG_DROWNRECOVER	(1 << 19)	// drowning recovery
#define DMG_ACID			(1 << 20)	// toxic chemicals or acid burns
#define DMG_SLOWBURN		(1 << 21)	// in an oven
#define DMG_SLOWFREEZE		(1 << 22)	// in a subzero freezer
#define DMG_MORTAR			(1 << 23)	// Hit by air raid (done to distinguish grenade from mortar)
#define DMG_PLASMA			(1 << 24)	// Extremally hot plasma
#define DMG_RADIUS_MAX		(1 << 25)	// Radius damage with this flag doesn't decrease over distance
#define DMG_DONT_BLEED		(1 << 26)	// Don't spawn blood (radiation, etc.)
#define DMG_IGNOREARMOR		(1 << 27)	// Damage ignores target's armor
#define DMG_VAPOURIZING		(1 << 28)	// XDM3034: DMG_ALWAYSGIB with no gibs
#define DMG_WALLPIERCING	(1 << 29)	// Blast Damages ents through walls
#define DMG_DISINTEGRATING	(1 << 30)	// XDM3035: makes entities Disintegrate()
#define DMG_NOSELF			(1 << 31)	// no self damage with this

// time-based damage
#define DMG_TIMEBASED		(~(0x3fff))	// mask for time-based damage

// Damage masks

// these are the damage types that are allowed to gib corpses
#define DMGM_GIB_CORPSE		(DMG_CRUSH | DMG_BLAST | DMG_SONIC)
// these are the damage types that have client hud art
#define DMGM_SHOWNHUD		(DMG_IGNOREARMOR | DMG_CRUSH | DMG_BURN | DMG_FREEZE | DMG_SHOCK | DMG_SONIC | DMG_MORTAR | DMG_DROWN | DMG_NERVEGAS | DMG_POISON | DMG_RADIATION | DMG_ACID | DMG_BULLET | DMG_BLAST | DMG_ENERGYBLAST | DMG_ENERGYBEAM | DMG_PARALYZE | DMG_SLASH | DMG_DISINTEGRATING | DMG_VAPOURIZING)
// these can break objects (like crates)
#define DMGM_BREAK			(DMG_GENERIC | DMG_CRUSH | DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_ENERGYBLAST | DMG_SONIC | DMG_ENERGYBEAM | DMG_ALWAYSGIB | DMG_MORTAR | DMG_ANNIHILATION | DMG_SHOCK | DMG_DISINTEGRATING)
// these can push some objects (monsters, crates)
#define DMGM_PUSH			(DMG_BLAST | DMG_ENERGYBLAST)
// any of these can be classified as fire damage
#define DMGM_FIRE			(DMG_BURN | DMG_SLOWBURN)
// any of these can be classified as frost damage
#define DMGM_COLD			(DMG_FREEZE | DMG_SLOWFREEZE)
// any of these can be classified as poison damage
#define DMGM_POISON			(DMG_NERVEGAS | DMG_POISON | DMG_ACID)
// these produce visible effects (smoke clouds, gas, splashes, etc.)
#define DMGM_VISIBLE		(DMGM_FIRE | DMGM_POISON | DMG_PLASMA | DMG_VAPOURIZING | DMG_DISINTEGRATING)
// these make victims bleed
#define DMGM_BLEED			(DMG_CRUSH | DMG_BULLET | DMG_SLASH | DMG_BLAST | DMG_MORTAR)

// NOTE: tweak these values based on gameplay feedback:

#define PARALYZE_DURATION	10		// number of 2 second intervals to take damage
#define PARALYZE_DAMAGE		2.0		// damage to take each 2 second interval

#define NERVEGAS_DURATION	10
#define NERVEGAS_DAMAGE		5.0

#define POISON_DURATION		10
#define POISON_DAMAGE		4.0

#define RADIATION_DURATION	20
#define RADIATION_DAMAGE	2.0

#define ACID_DURATION		10
#define ACID_DAMAGE			5.0

#define SLOWBURN_DURATION	8
#define SLOWBURN_DAMAGE		10.0

#define SLOWFREEZE_DURATION	8
#define SLOWFREEZE_DAMAGE	4.0

#define DROWNRESTORE_DURATION	5
#define DROWNRESTORE_REGEN		10.0

#define	itbd_Paralyze		0
#define	itbd_NerveGas		1
#define	itbd_Poison			2
#define	itbd_Radiation		3
#define	itbd_DrownRecover	4
#define	itbd_Acid			5
#define	itbd_SlowBurn		6
#define	itbd_SlowFreeze		7
#define CDMG_TIMEBASED		8


// Qhen calling Killed(), a value that governs gib behavior is expected to be one of these values
enum GIBMODE
{
	GIB_NORMAL = 0,		// gib if entity was overkilled
	GIB_NEVER,			// never gib, no matter how much death damage is done ( freezing, etc )
	GIB_ALWAYS,			// always gib ( Houndeye Shock, Barnacle Bite )
	GIB_DISINTEGRATE,	// XDM3035: no gibs, disintegration
	GIB_VAPOURIZE,		// smoke
	GIB_MELT,			// melt in a puddle
	GIB_BLAST,			// blasted!
	GIB_BURN,			// burned into ashes!
	GIB_SONIC,			// dispercing
	GIB_ENERGYBLAST,	// blasted with powerfull energy
	GIB_ENERGYBEAM,		// cutted with energy ray
	GIB_ELECTRO,		// electrocuted
	GIB_ACID,			// acid melted
	GIB_FROZEN,			// frozen gib
	GIB_RADIATION,		// radiation gib
	GIB_FADE,			// fade body
	GIB_REMOVE			// instantly remove body
};

#endif // DAMAGE_H
