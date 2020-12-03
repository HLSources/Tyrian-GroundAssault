#if defined(CLIENT_DLL)
#include "hud.h"
#include "cl_util.h"
#include "r_efx.h"
#else
#include "extdll.h"
#include "util.h"
#endif
#include "decals.h"
#include "shared_resources.h"

// XDM: common sprite indexes

DLL_GLOBAL int g_iModelIndexAnimglow01 = 0;
DLL_GLOBAL int g_iModelIndexBubble = 0;// holds the index for the bubbles model
DLL_GLOBAL int g_iModelIndexMuzzleFlash1 = 0;
DLL_GLOBAL int g_iModelIndexMuzzleFlash2 = 0;
DLL_GLOBAL int g_iModelIndexMuzzleFlash3 = 0;


//Ghoul: sprites for new effects
DLL_GLOBAL int g_iModelIndexAnimSpr0 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr1 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr2 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr3 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr4 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr5 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr6 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr7 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr8 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr9 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr10 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr11 = 0;
DLL_GLOBAL int g_iModelIndexAnimSpr12 = 0;

DLL_GLOBAL int g_iModelIndexExplosion0 = 0;
DLL_GLOBAL int g_iModelIndexExplosion1 = 0;
DLL_GLOBAL int g_iModelIndexExplosion2 = 0;
DLL_GLOBAL int g_iModelIndexExplosion3 = 0;
DLL_GLOBAL int g_iModelIndexExplosion4 = 0;
DLL_GLOBAL int g_iModelIndexExplosion5 = 0;


DLL_GLOBAL int g_iModelIndexPartRed = 0;
DLL_GLOBAL int g_iModelIndexPartGreen = 0;
DLL_GLOBAL int g_iModelIndexPartBlue = 0;
DLL_GLOBAL int g_iModelIndexPartViolet = 0;
DLL_GLOBAL int g_iModelIndexPartWhite = 0;
DLL_GLOBAL int g_iModelIndexPartBlack = 0;
DLL_GLOBAL int g_iModelIndexBeamsAll = 0;
DLL_GLOBAL int g_iModelIndexFlame = 0;
DLL_GLOBAL int g_iModelIndexFire = 0;
DLL_GLOBAL int g_iModelIndexPartGibs = 0;
//models
DLL_GLOBAL int g_iModelIndexExplosionModel = 0;
DLL_GLOBAL int g_iModelIndexGibModel = 0;
DLL_GLOBAL int g_iModelIndexPlrGibModel = 0;
DLL_GLOBAL int g_iModelIndexLightningFieldModel = 0;
DLL_GLOBAL int g_iModelIndexGunShellModel = 0;

//
// This list must be same in both server and client DLLs
//
DLL_DECALLIST g_Decals[DECAL_ENUM_SIZE] = {
	{ "{acid_splat1", 0 },	// DECAL_ACID_SPLAT1
	{ "{acid_splat2", 0 },	// DECAL_ACID_SPLAT2
	{ "{acid_splat3", 0 },	// DECAL_ACID_SPLAT3
	{ "{shot1",	0 },		// DECAL_GUNSHOT1
	{ "{shot2",	0 },		// DECAL_GUNSHOT2
	{ "{shot3",	0 },		// DECAL_GUNSHOT3
	{ "{shot4",	0 },		// DECAL_GUNSHOT4
	{ "{shot5",	0 },		// DECAL_GUNSHOT5
	{ "{largeshot1", 0 },	// DECAL_LARGESHOT1
	{ "{largeshot2", 0 },	// DECAL_LARGESHOT2
	{ "{largeshot3", 0 },	// DECAL_LARGESHOT3
	{ "{largeshot4", 0 },	// DECAL_LARGESHOT4
	{ "{largeshot5", 0 },	// DECAL_LARGESHOT5
	{ "{lambda01", 0 },		// DECAL_LAMBDA1
	{ "{lambda02", 0 },		// DECAL_LAMBDA2
	{ "{lambda03", 0 },		// DECAL_LAMBDA3
	{ "{lambda04", 0 },		// DECAL_LAMBDA4
	{ "{lambda05", 0 },		// DECAL_LAMBDA5
	{ "{lambda06", 0 },		// DECAL_LAMBDA6
	{ "{scorch1", 0 },		// DECAL_SCORCH1
	{ "{scorch2", 0 },		// DECAL_SCORCH2
	{ "{scorch3", 0 },		// DECAL_SCORCH3
	{ "{blood1", 0 },		// DECAL_BLOOD1
	{ "{blood2", 0 },		// DECAL_BLOOD2
	{ "{blood3", 0 },		// DECAL_BLOOD3
	{ "{blood4", 0 },		// DECAL_BLOOD4
	{ "{blood5", 0 },		// DECAL_BLOOD5
	{ "{blood6", 0 },		// DECAL_BLOOD6
	{ "{yblood1", 0 },		// DECAL_YBLOOD1
	{ "{yblood2", 0 },		// DECAL_YBLOOD2
	{ "{yblood3", 0 },		// DECAL_YBLOOD3
	{ "{yblood4", 0 },		// DECAL_YBLOOD4
	{ "{yblood5", 0 },		// DECAL_YBLOOD5
	{ "{yblood6", 0 },		// DECAL_YBLOOD6
	{ "{break1", 0 },		// DECAL_GLASSBREAK1
	{ "{break2", 0 },		// DECAL_GLASSBREAK2
	{ "{break3", 0 },		// DECAL_GLASSBREAK3
	{ "{bigshot1", 0 },		// DECAL_BIGSHOT1
	{ "{bigshot2", 0 },		// DECAL_BIGSHOT2
	{ "{bigshot3", 0 },		// DECAL_BIGSHOT3
	{ "{bigshot4", 0 },		// DECAL_BIGSHOT4
	{ "{bigshot5", 0 },		// DECAL_BIGSHOT5
	{ "{gaussshot1", 0 },	// DECAL_GAUSSSHOT1
	{ "{spit1", 0 },		// DECAL_SPIT1
	{ "{spit2", 0 },		// DECAL_SPIT2
	{ "{splat7", 0 },		// DECAL_SPLAT7
	{ "{splat8", 0 },		// DECAL_SPLAT8
	{ "{bproof1", 0 },		// DECAL_BPROOF1
	{ "{gargstomp", 0 },	// DECAL_GARGSTOMP1,	// Gargantua stomp crack
	{ "{smscorch1", 0 },	// DECAL_SMALLSCORCH1,	// Small scorch mark
	{ "{smscorch2", 0 },	// DECAL_SMALLSCORCH2,	// Small scorch mark
	{ "{smscorch3", 0 },	// DECAL_SMALLSCORCH3,	// Small scorch mark
	{ "{mommablob", 0 },	// DECAL_MOMMABIRTH		// BM Birth spray
	{ "{mommasplat", 0 },	// DECAL_MOMMASPLAT		// BM Mortar spray
	{ "{nucblow1", 0 },		// DECAL_NUCBLOW1
	{ "{nucblow1", 0 },		// DECAL_NUCBLOW2
	{ "{nucblow1", 0 },		// DECAL_NUCBLOW3
	{ "{woodbreak1", 0 },	// DECAL_WOODBREAK1
	{ "{woodbreak2", 0 },	// DECAL_WOODBREAK2
	{ "{woodbreak3", 0 },	// DECAL_WOODBREAK3
	{ "{bblood1", 0 },		// DECAL_BBLOOD1
	{ "{bblood2", 0 },		// DECAL_BBLOOD2
	{ "{bblood3", 0 },		// DECAL_BBLOOD3
	{ "{gblood1", 0 },		// DECAL_GBLOOD1
	{ "{gblood2", 0 },		// DECAL_GBLOOD2
	{ "{gblood3", 0 },		// DECAL_GBLOOD3
	{ "{mdscorch1", 0 },	// DECAL_MDSCORCH1
	{ "{mdscorch2", 0 },	// DECAL_MDSCORCH2
	{ "{mdscorch3", 0 },	// DECAL_MDSCORCH3
	{ "{biohaz", 0 },		// DECAL_BIOHAZ
	{ "{bloodsmearr1", 0 },	// DECAL_BLOODSMEARR1
	{ "{bloodsmearr2", 0 },	// DECAL_BLOODSMEARR2
	{ "{bloodsmearr3", 0 },	// DECAL_BLOODSMEARR3
	{ "{bloodsmeary1", 0 },	// DECAL_BLOODSMEARY1
	{ "{bloodsmeary2", 0 },	// DECAL_BLOODSMEARY2
	{ "{bloodsmeary3", 0 },	// DECAL_BLOODSMEARY3
	{ "{blow", 0 },			// DECAL_BLOW
	{ "{miniscorch1", 0 },	// DECAL_MINISCORCH1
	{ "{miniscorch2", 0 },	// DECAL_MINISCORCH2
	{ "{miniscorch3", 0 },	// DECAL_MINISCORCH3
	{ "{blast_scorch1", 0 },	// DECAL_BLASTSCORCH1
	{ "{blast_scorch2", 0 },	// DECAL_BLASTSCORCH2
	{ "{blast_scorch3", 0 },	// DECAL_BLASTSCORCH3
	{ "{exp_scorch1", 0 },	// DECAL_EXPSCORCH1
	{ "{exp_scorch2", 0 },	// DECAL_EXPSCORCH2
	{ "{exp_scorch3", 0 },	// DECAL_EXPSCORCH3
	{ "{ofscorch1", 0 },	// DECAL_OFSCORCH1
	{ "{ofscorch2", 0 },	// DECAL_OFSCORCH2
	{ "{ofscorch3", 0 },	// DECAL_OFSCORCH3
	{ "{gluon_scorch1", 0 },	// DECAL_GLUONSCORCH1
	{ "{gluon_scorch2", 0 },	// DECAL_GLUONSCORCH2
	{ "{gluon_scorch3", 0 },	// DECAL_GLUONSCORCH3
	{ "{beamshot1", 0 },	// DECAL_BEAMSHOT1
	{ "{beamshot2", 0 },	// DECAL_BEAMSHOT2
	{ "{beamshot3", 0 },	// DECAL_BEAMSHOT3
	{ "{frost_scorch1", 0 },	// DECAL_FROSTSCORCH1
	{ "{frost_scorch2", 0 },	// DECAL_FROSTSCORCH2
	{ "{frost_scorch3", 0 },	// DECAL_FROSTSCORCH3
	{ "{acid_bigsplat1", 0 },	// DECAL_ACID_BIGSPLAT1
	{ "{acid_bigsplat2", 0 },	// DECAL_ACID_BIGSPLAT2
	{ "{acid_bigsplat3", 0 },	// DECAL_ACID_BIGSPLAT3
};


//-----------------------------------------------------------------------------
// Purpose: Get indexes for fast use of common resources
// NOTE: These must have been precached on server! Otherwise CL_LoadModel() -1
//-----------------------------------------------------------------------------
void PrecacheSharedResources(void)
{
#if defined(CLIENT_DLL)


	gEngfuncs.CL_LoadModel("sprites/bubble.spr",		&g_iModelIndexBubble);

//Ghoul: sprites for new effects
	gEngfuncs.CL_LoadModel("sprites/animglow01.spr",		&g_iModelIndexAnimglow01);
	gEngfuncs.CL_LoadModel("sprites/anim_spr0.spr",			&g_iModelIndexAnimSpr0);
	gEngfuncs.CL_LoadModel("sprites/anim_spr1.spr",			&g_iModelIndexAnimSpr1);
	gEngfuncs.CL_LoadModel("sprites/anim_spr2.spr",			&g_iModelIndexAnimSpr2);
	gEngfuncs.CL_LoadModel("sprites/anim_spr3.spr",			&g_iModelIndexAnimSpr3);	
	gEngfuncs.CL_LoadModel("sprites/anim_spr4.spr",			&g_iModelIndexAnimSpr4);
	gEngfuncs.CL_LoadModel("sprites/anim_spr5.spr",			&g_iModelIndexAnimSpr5);
	gEngfuncs.CL_LoadModel("sprites/anim_spr6.spr",			&g_iModelIndexAnimSpr6);	
	gEngfuncs.CL_LoadModel("sprites/anim_spr7.spr",			&g_iModelIndexAnimSpr7);
	gEngfuncs.CL_LoadModel("sprites/anim_spr8.spr",			&g_iModelIndexAnimSpr8);
	gEngfuncs.CL_LoadModel("sprites/anim_spr9.spr",			&g_iModelIndexAnimSpr9);	
	gEngfuncs.CL_LoadModel("sprites/anim_spr10.spr",		&g_iModelIndexAnimSpr10);
	gEngfuncs.CL_LoadModel("sprites/anim_spr11.spr",		&g_iModelIndexAnimSpr11);
	gEngfuncs.CL_LoadModel("sprites/anim_spr12.spr",		&g_iModelIndexAnimSpr12);	

	gEngfuncs.CL_LoadModel("sprites/explosion0.spr",		&g_iModelIndexExplosion0);
	gEngfuncs.CL_LoadModel("sprites/explosion1.spr",		&g_iModelIndexExplosion1);
	gEngfuncs.CL_LoadModel("sprites/explosion2.spr",		&g_iModelIndexExplosion2);
	gEngfuncs.CL_LoadModel("sprites/explosion3.spr",		&g_iModelIndexExplosion3);
	gEngfuncs.CL_LoadModel("sprites/explosion4.spr",		&g_iModelIndexExplosion4);
	gEngfuncs.CL_LoadModel("sprites/explosion5.spr",		&g_iModelIndexExplosion5);

	gEngfuncs.CL_LoadModel("sprites/particles_red.spr",		&g_iModelIndexPartRed);
	gEngfuncs.CL_LoadModel("sprites/particles_green.spr",	&g_iModelIndexPartGreen);
	gEngfuncs.CL_LoadModel("sprites/particles_blue.spr",	&g_iModelIndexPartBlue);
	gEngfuncs.CL_LoadModel("sprites/particles_violet.spr",	&g_iModelIndexPartViolet);
	gEngfuncs.CL_LoadModel("sprites/particles_white.spr",	&g_iModelIndexPartWhite);
	gEngfuncs.CL_LoadModel("sprites/particles_black.spr",	&g_iModelIndexPartBlack);
	gEngfuncs.CL_LoadModel("sprites/beams_all.spr",			&g_iModelIndexBeamsAll);
	gEngfuncs.CL_LoadModel("sprites/flame.spr",				&g_iModelIndexFlame);
	gEngfuncs.CL_LoadModel("sprites/fire.spr",				&g_iModelIndexFire);
	gEngfuncs.CL_LoadModel("sprites/particles_gibs.spr",	&g_iModelIndexPartGibs);

	gEngfuncs.CL_LoadModel("sprites/muzzleflash1.spr",	&g_iModelIndexMuzzleFlash1);
	gEngfuncs.CL_LoadModel("sprites/muzzleflash2.spr",	&g_iModelIndexMuzzleFlash2);
	gEngfuncs.CL_LoadModel("sprites/muzzleflash3.spr",	&g_iModelIndexMuzzleFlash3);

	gEngfuncs.CL_LoadModel("models/effects/ef_lightning_field.mdl",	&g_iModelIndexLightningFieldModel);
	gEngfuncs.CL_LoadModel("models/effects/ef_explosions.mdl",		&g_iModelIndexExplosionModel);
	gEngfuncs.CL_LoadModel("models/effects/ef_gibs.mdl",			&g_iModelIndexGibModel);
	gEngfuncs.CL_LoadModel("models/effects/ef_gun_shells.mdl",		&g_iModelIndexGunShellModel);
	gEngfuncs.CL_LoadModel("models/effects/ef_player_gibs.mdl",		&g_iModelIndexPlrGibModel);

	int i = 0;
	for (i = 0; i < DECAL_ENUM_SIZE; ++i)//ARRAYSIZE(g_Decals); ++i)
		g_Decals[i].index = gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(g_Decals[i].name);// this is a DECAL idex, not TEXTURE!


#else// Now same for the server DLL

	g_iModelIndexBubble		= PRECACHE_MODEL("sprites/bubble.spr");//bubbles

	//Ghoul: sprites for new effects
	g_iModelIndexAnimglow01		= PRECACHE_MODEL("sprites/animglow01.spr");
	g_iModelIndexMuzzleFlash1	= PRECACHE_MODEL("sprites/muzzleflash1.spr");
	g_iModelIndexMuzzleFlash2	= PRECACHE_MODEL("sprites/muzzleflash2.spr");
	g_iModelIndexMuzzleFlash3	= PRECACHE_MODEL("sprites/muzzleflash3.spr");

	g_iModelIndexAnimSpr0	= PRECACHE_MODEL("sprites/anim_spr0.spr");
	g_iModelIndexAnimSpr1	= PRECACHE_MODEL("sprites/anim_spr1.spr");
	g_iModelIndexAnimSpr2	= PRECACHE_MODEL("sprites/anim_spr2.spr");
	g_iModelIndexAnimSpr3	= PRECACHE_MODEL("sprites/anim_spr3.spr");
	g_iModelIndexAnimSpr4	= PRECACHE_MODEL("sprites/anim_spr4.spr");
	g_iModelIndexAnimSpr5	= PRECACHE_MODEL("sprites/anim_spr5.spr");
	g_iModelIndexAnimSpr6	= PRECACHE_MODEL("sprites/anim_spr6.spr");
	g_iModelIndexAnimSpr7	= PRECACHE_MODEL("sprites/anim_spr7.spr");
	g_iModelIndexAnimSpr8	= PRECACHE_MODEL("sprites/anim_spr8.spr");
	g_iModelIndexAnimSpr9	= PRECACHE_MODEL("sprites/anim_spr9.spr");
	g_iModelIndexAnimSpr10	= PRECACHE_MODEL("sprites/anim_spr10.spr");
	g_iModelIndexAnimSpr11	= PRECACHE_MODEL("sprites/anim_spr11.spr");
	g_iModelIndexAnimSpr12	= PRECACHE_MODEL("sprites/anim_spr12.spr");

	g_iModelIndexExplosion0	= PRECACHE_MODEL("sprites/explosion0.spr");
	g_iModelIndexExplosion1	= PRECACHE_MODEL("sprites/explosion1.spr");
	g_iModelIndexExplosion2	= PRECACHE_MODEL("sprites/explosion2.spr");
	g_iModelIndexExplosion3	= PRECACHE_MODEL("sprites/explosion3.spr");
	g_iModelIndexExplosion4	= PRECACHE_MODEL("sprites/explosion4.spr");
	g_iModelIndexExplosion5	= PRECACHE_MODEL("sprites/explosion5.spr");

	g_iModelIndexPartRed	= PRECACHE_MODEL("sprites/particles_red.spr");
	g_iModelIndexPartGreen	= PRECACHE_MODEL("sprites/particles_green.spr");
	g_iModelIndexPartBlue	= PRECACHE_MODEL("sprites/particles_blue.spr");
	g_iModelIndexPartViolet	= PRECACHE_MODEL("sprites/particles_violet.spr");
	g_iModelIndexPartWhite	= PRECACHE_MODEL("sprites/particles_white.spr");
	g_iModelIndexPartBlack	= PRECACHE_MODEL("sprites/particles_black.spr");
	g_iModelIndexBeamsAll	= PRECACHE_MODEL("sprites/beams_all.spr");
	g_iModelIndexFlame		= PRECACHE_MODEL("sprites/flame.spr");
	g_iModelIndexFire		= PRECACHE_MODEL("sprites/fire.spr");
	g_iModelIndexPartGibs	= PRECACHE_MODEL("sprites/particles_gibs.spr");

	g_iModelIndexLightningFieldModel	= PRECACHE_MODEL("models/effects/ef_lightning_field.mdl");
	g_iModelIndexExplosionModel			= PRECACHE_MODEL("models/effects/ef_explosions.mdl");
	g_iModelIndexGibModel				= PRECACHE_MODEL("models/effects/ef_gibs.mdl");
	g_iModelIndexPlrGibModel			= PRECACHE_MODEL("models/effects/ef_player_gibs.mdl");
	g_iModelIndexGunShellModel			= PRECACHE_MODEL("models/effects/ef_gun_shells.mdl");
#endif
}
