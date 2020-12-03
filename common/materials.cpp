//#include <string.h>
#include "extdll.h"
#include "pm_materials.h"

// TODO: write an object-oriented material system with external database

const char *gSoundsSparks[] =
{
	"buttons/spark1.wav",
	"buttons/spark2.wav",
	"buttons/spark3.wav",
	"buttons/spark4.wav",
	"buttons/spark5.wav",
	"buttons/spark6.wav",
};


const char *gSoundsRicochet[] =
{
	"weapons/ric1.wav",
	"weapons/ric2.wav",
	"weapons/ric3.wav",
	"weapons/ric4.wav",
	"weapons/ric5.wav",
};

const char *gSoundsShell9mm[] =
{
	"player/pl_shell1.wav",
	"player/pl_shell2.wav",
	"player/pl_shell3.wav",
};

const char *gSoundsShellShotgun[] =
{
	"weapons/sshell1.wav",
	"weapons/sshell2.wav",
	"weapons/sshell3.wav",
};

const char *gSoundsDropBody[] =
{
	"common/bodydrop1.wav",
	"common/bodydrop2.wav",
	"common/bodydrop3.wav",
	"common/bodydrop4.wav",
};


// NUM_SHARD_SOUNDS
const char *gSoundsWood[] =
{
	"debris/wood1.wav",
	"debris/wood2.wav",
	"debris/wood3.wav",
	"debris/wood4.wav",
	"debris/wood5.wav",
	"debris/wood6.wav",
	"debris/wood7.wav",
	"debris/wood8.wav",
};

const char *gSoundsFlesh[] =
{
	"debris/flesh1.wav",
	"debris/flesh2.wav",
	"debris/flesh3.wav",
	"debris/flesh4.wav",
	"debris/flesh5.wav",
	"debris/flesh6.wav",
	"debris/flesh7.wav",
	"debris/flesh8.wav",
};

const char *gSoundsMetal[] =
{
	"debris/metal1.wav",
	"debris/metal2.wav",
	"debris/metal3.wav",
	"debris/metal4.wav",
	"debris/metal5.wav",
	"debris/metal6.wav",
	"debris/metal7.wav",
	"debris/metal8.wav",
};

const char *gSoundsConcrete[] =
{
	"debris/concrete1.wav",
	"debris/concrete2.wav",
	"debris/concrete3.wav",
	"debris/concrete4.wav",
	"debris/concrete5.wav",
	"debris/concrete6.wav",
	"debris/concrete7.wav",
	"debris/concrete8.wav",
};

const char *gSoundsGlass[] =
{
	"debris/glass1.wav",
	"debris/glass2.wav",
	"debris/glass3.wav",
	"debris/glass4.wav",
	"debris/glass5.wav",
	"debris/glass6.wav",
	"debris/glass7.wav",
	"debris/glass8.wav",
};

const char *gSoundsCeiling[] =
{
	"debris/ceiling1.wav",
	"debris/ceiling2.wav",
	"debris/ceiling3.wav",
	"debris/ceiling4.wav",
	"debris/ceiling1.wav",// can't be helped, duplicate
	"debris/ceiling2.wav",
	"debris/ceiling3.wav",
	"debris/ceiling4.wav",
};

// NUM_PUSH_SOUNDS
const char *gPushSoundsGlass[]		= {"debris/pushglass1.wav", "debris/pushglass2.wav", "debris/pushglass3.wav"};
const char *gPushSoundsWood[]		= {"debris/pushwood1.wav", "debris/pushwood2.wav", "debris/pushwood3.wav"};
const char *gPushSoundsMetal[]		= {"debris/pushmetal1.wav", "debris/pushmetal2.wav", "debris/pushmetal3.wav"};
const char *gPushSoundsFlesh[]		= {"debris/pushflesh1.wav", "debris/pushflesh2.wav", "debris/pushflesh3.wav"};
const char *gPushSoundsConcrete[]	= {"debris/pushstone1.wav", "debris/pushstone2.wav", "debris/pushstone3.wav"};

// NUM_BREAK_SOUNDS
const char *gBreakSoundsGlass[]		= {"debris/bustglass1.wav", "debris/bustglass2.wav"};
const char *gBreakSoundsWood[]		= {"debris/bustcrate1.wav", "debris/bustcrate2.wav"};
const char *gBreakSoundsMetal[]		= {"debris/bustmetal1.wav", "debris/bustmetal2.wav"};
const char *gBreakSoundsFlesh[]		= {"debris/bustflesh1.wav", "debris/bustflesh2.wav"};
const char *gBreakSoundsConcrete[]	= {"debris/bustconcrete1.wav", "debris/bustconcrete2.wav"};
const char *gBreakSoundsCeiling[]	= {"debris/bustceiling1.wav", "debris/bustceiling2.wav"};

// sorted by Materials struct
const char **gPushSounds[] = {
	{gPushSoundsGlass},// matGlass
	{gPushSoundsWood},
	{gPushSoundsMetal},
	{gPushSoundsFlesh},
	{gPushSoundsConcrete},
	{gPushSoundsWood},
	{gPushSoundsMetal},
	{gPushSoundsGlass},
	{gPushSoundsConcrete},
	{gPushSoundsMetal},
	{gPushSoundsMetal},
	{NULL},// matNone
	{NULL}// matLastMaterial
};

// sorted by Materials struct
const char **gBreakSounds[] = {
	{gBreakSoundsGlass},// matGlass
	{gBreakSoundsWood},
	{gBreakSoundsMetal},
	{gBreakSoundsFlesh},
	{gBreakSoundsConcrete},
	{gBreakSoundsCeiling},
	{gBreakSoundsMetal},
	{gBreakSoundsGlass},
	{gBreakSoundsConcrete},
	{gPushSoundsMetal},
	{gPushSoundsMetal},
	{NULL},// matNone
	{NULL}// matLastMaterial
};

// NUM_STEP_SOUNDS
const char *gStepSoundsDefault[] =
{
	"player/pl_step1.wav",
	"player/pl_step3.wav",
	"player/pl_step2.wav",
	"player/pl_step4.wav"
};
const char *gStepSoundsMetal[] =
{
	"player/pl_metal1.wav",
	"player/pl_metal3.wav",
	"player/pl_metal2.wav",
	"player/pl_metal4.wav"
};
const char *gStepSoundsDirt[] =
{
	"player/pl_dirt1.wav",
	"player/pl_dirt3.wav",
	"player/pl_dirt2.wav",
	"player/pl_dirt4.wav"
};
const char *gStepSoundsDuct[] =
{
	"player/pl_duct1.wav",
	"player/pl_duct3.wav",
	"player/pl_duct2.wav",
	"player/pl_duct4.wav"
};
const char *gStepSoundsGrate[] =
{
	"player/pl_grate1.wav",
	"player/pl_grate3.wav",
	"player/pl_grate2.wav",
	"player/pl_grate4.wav"
};
const char *gStepSoundsSlosh[] =
{
	"player/pl_slosh1.wav",
	"player/pl_slosh3.wav",
	"player/pl_slosh2.wav",
	"player/pl_slosh4.wav"
};
const char *gStepSoundsWade[] =
{
	"player/pl_wade1.wav",
	"player/pl_wade3.wav",
	"player/pl_wade2.wav",
	"player/pl_wade4.wav"
};
const char *gStepSoundsLadder[] =
{
	"player/pl_ladder1.wav",
	"player/pl_ladder3.wav",
	"player/pl_ladder2.wav",
	"player/pl_ladder4.wav"
};

const char *gStepSoundsSnow[] =
{
	"player/pl_snow1.wav",
	"player/pl_snow3.wav",
	"player/pl_snow2.wav",
	"player/pl_snow4.wav"
};
const char *gStepSoundsGrass[] =
{
	"player/pl_grass1.wav",
	"player/pl_grass3.wav",
	"player/pl_grass2.wav",
	"player/pl_grass4.wav"
};
const char *gStepSoundsEshield[] =
{
	"player/pl_shield_impact1.wav",
	"player/pl_shield_impact2.wav",
	"player/pl_shield_impact3.wav",
	"player/pl_shield_impact1.wav"
};
// WARNING! All array indexes must be valid material IDs!
/*static */const char **gStepSounds[] = {
	{gStepSoundsDefault},// STEP_CONCRETE == 0
	{gStepSoundsMetal},
	{gStepSoundsDirt},
	{gStepSoundsDuct},
	{gStepSoundsGrate},
	{gStepSoundsSlosh},
	{gStepSoundsWade},
	{gStepSoundsLadder},
	{gStepSoundsSnow},
	{gStepSoundsGrass},
	{gStepSoundsEshield}
};

/*
typedef struct
{
	int material;
	const char **sounds;
} stepsounds_t;

stepsounds_t gStepSounds[] = {
	{STEP_CONCRETE,		gStepSoundsDefault},
	{STEP_METAL,		gStepSoundsMetal},
	{STEP_DIRT,			gStepSoundsDirt},
	{STEP_VENT,			gStepSoundsDuct},
	{STEP_GRATE,		gStepSoundsGrate},
	{STEP_SLOSH,		gStepSoundsSlosh},
	{STEP_WADE,			gStepSoundsWade},
	{STEP_LADDER,		gStepSoundsLadder},
	{STEP_SNOW,			gStepSoundsSnow},
	{STEP_GRASS,		gStepSoundsGrass}
};*/


// this table MUST include MAXIMUM amount of possible materials
// Materials must be unique! No duplicate entries allowed except sounds!
// undone: matUnbreakableGlass?
material_t gMaterials[] = 
{
	{CHAR_TEX_CONCRETE,	STEP_CONCRETE,	matCinderBlock,	gSoundsConcrete,	gBreakSoundsConcrete,	gPushSoundsConcrete,	gStepSoundsDefault	},
	{CHAR_TEX_BRICK,	STEP_CONCRETE,	matCinderBlock,	gSoundsConcrete,	gBreakSoundsConcrete,	gPushSoundsConcrete,	gStepSoundsDefault	},
	{CHAR_TEX_ASPHALT,	STEP_CONCRETE,	matCinderBlock,	gSoundsConcrete,	gBreakSoundsConcrete,	gPushSoundsConcrete,	gStepSoundsDefault	},
	{CHAR_TEX_ROCK,		STEP_CONCRETE,	matCinderBlock,	gSoundsConcrete,	gBreakSoundsConcrete,	gPushSoundsConcrete,	gStepSoundsDefault	},
	{CHAR_TEX_SAND_ROCK,		STEP_CONCRETE,	matCinderBlock,	gSoundsConcrete,	gBreakSoundsConcrete,	gPushSoundsConcrete,	gStepSoundsDefault	},
	{CHAR_TEX_METAL,	STEP_METAL,		matMetal,		gSoundsMetal,		gBreakSoundsMetal,		gPushSoundsMetal,		gStepSoundsMetal	},
	{CHAR_TEX_DIRT,		STEP_DIRT,		matRocks,		gSoundsConcrete,	gBreakSoundsConcrete,	gPushSoundsConcrete,	gStepSoundsDirt		},
	{CHAR_TEX_SAND,		STEP_DIRT,		matRocks,		gSoundsConcrete,	gBreakSoundsConcrete,	gPushSoundsConcrete,	gStepSoundsDirt		},
	{CHAR_TEX_VENT,		STEP_VENT,		matVent,		gSoundsMetal,		gBreakSoundsMetal,		gPushSoundsMetal,		gStepSoundsDuct		},
	{CHAR_TEX_GRATE,	STEP_GRATE,		matGrate,		gSoundsMetal,		gBreakSoundsMetal,		gPushSoundsMetal,		gStepSoundsGrate	},
	{CHAR_TEX_TILE,		STEP_CONCRETE,	matNone,		gSoundsConcrete,	gBreakSoundsConcrete,	gPushSoundsConcrete,	gStepSoundsDefault	},
	{CHAR_TEX_SLOSH,	STEP_SLOSH,		matNone,		gSoundsFlesh,		gBreakSoundsFlesh,		gPushSoundsFlesh,		gStepSoundsSlosh	},
	{CHAR_TEX_EMPTY,	STEP_CONCRETE,	matNone,		gSoundsConcrete,	gBreakSoundsConcrete,	gPushSoundsConcrete,	gStepSoundsDefault	},
	{CHAR_TEX_WOOD,		STEP_CONCRETE,	matWood,		gSoundsWood,		gBreakSoundsWood,		gPushSoundsWood,		gStepSoundsDefault	},
	{CHAR_TEX_COMPUTER,	STEP_METAL,		matComputer,	gSoundsMetal,		gBreakSoundsMetal,		gPushSoundsMetal,		gStepSoundsMetal	},
	{CHAR_TEX_GLASS,	STEP_CONCRETE,	matGlass,		gSoundsGlass,		gBreakSoundsGlass,		gPushSoundsGlass,		gStepSoundsDefault	},
	{CHAR_TEX_ICE,		STEP_CONCRETE,	matNone,		gSoundsGlass,		gBreakSoundsGlass,		gPushSoundsGlass,		gStepSoundsDefault	},
	{CHAR_TEX_FLESH,	STEP_SLOSH,		matFlesh,		gSoundsFlesh,		gBreakSoundsFlesh,		gPushSoundsFlesh,		gStepSoundsSlosh	},
	{CHAR_TEX_SNOW,		STEP_SNOW,		matNone,		gSoundsFlesh,		gBreakSoundsFlesh,		gPushSoundsConcrete,	gStepSoundsSnow		},
	{CHAR_TEX_GRASS,	STEP_GRASS,		matNone,		gSoundsCeiling,		gBreakSoundsCeiling,	gPushSoundsConcrete,	gStepSoundsGrass	},
	{CHAR_TEX_LEAVES,	STEP_GRASS,		matNone,		gSoundsCeiling,		gBreakSoundsCeiling,	gPushSoundsConcrete,	gStepSoundsGrass	},
	{CHAR_TEX_CEILING,	STEP_CONCRETE,	matCeilingTile,	gSoundsCeiling,		gBreakSoundsCeiling,	gPushSoundsConcrete,	gStepSoundsDefault	},
	{CHAR_TEX_WATER,	STEP_WADE,		matNone,		gSoundsFlesh,		gBreakSoundsFlesh,		gPushSoundsFlesh,		gStepSoundsWade		},
	{CHAR_TEX_ENERGYSHIELD,	STEP_ESHIELD,	matNone,	gStepSoundsEshield,	gStepSoundsEshield,	gStepSoundsEshield,			gStepSoundsEshield	}
};


int MapTextureTypeStepType(const char &chTextureType)
{
/* slower
	for (int i=0; i<NUM_MATERIALS; ++i)
		if (gMaterials[i].texture_id == chTextureType)
			return gMaterials[i].step_id;
*/
	switch (chTextureType)
	{
		default:
		case CHAR_TEX_GLASS:	return STEP_CONCRETE; break;
		case CHAR_TEX_ICE:		return STEP_CONCRETE; break;
		case CHAR_TEX_CONCRETE:	return STEP_CONCRETE; break;
		case CHAR_TEX_ASPHALT:	return STEP_CONCRETE; break;
		case CHAR_TEX_ROCK:		return STEP_CONCRETE; break;
		case CHAR_TEX_SAND_ROCK:	return STEP_CONCRETE; break;
		case CHAR_TEX_BRICK:	return STEP_CONCRETE; break;
		case CHAR_TEX_COMPUTER: return STEP_CONCRETE; break;
		case CHAR_TEX_METAL:	return STEP_METAL; break;
		case CHAR_TEX_DIRT:		return STEP_DIRT; break;
		case CHAR_TEX_SAND:		return STEP_DIRT; break;
		case CHAR_TEX_VENT:		return STEP_VENT; break;
		case CHAR_TEX_GRATE:	return STEP_GRATE; break;
		case CHAR_TEX_TILE:		return STEP_CONCRETE; break;
		case CHAR_TEX_FLESH:	return STEP_SLOSH; break;
		case CHAR_TEX_SLOSH:	return STEP_SLOSH; break;
		case CHAR_TEX_WOOD:		return STEP_CONCRETE; break;
		case CHAR_TEX_SNOW:		return STEP_SNOW; break;
		case CHAR_TEX_GRASS:	return STEP_GRASS; break;
		case CHAR_TEX_LEAVES:	return STEP_GRASS; break;
		case CHAR_TEX_ENERGYSHIELD:	return STEP_ESHIELD; break;
		case CHAR_TEX_EMPTY:	return STEP_CONCRETE; break;
	}
}
