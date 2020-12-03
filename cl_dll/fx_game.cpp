#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "r_efx.h"
#include "cl_fx.h"
#include "shared_resources.h"
#include "con_nprint.h"
#include "event_api.h"
#include "pm_defs.h"
#include "eventscripts.h"
#include "decals.h"
#include "damage.h"
#include "colors.h"
#include "RenderManager.h"
#include "RenderSystem.h"
#include "RSBeam.h"
#include "RSModel.h"
#include "RSSprite.h"
#include "RSTeleParts.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "PSBeam.h"
#include "PSDrips.h"
#include "PSFlameCone.h"
#include "PSBubbles.h"
#include "PSFlatTrail.h"
#include "PSSparks.h"
#include "PSAurora.h"
#include "RSLight.h"
#include "PSBlastCone.h"
#include "RSCylinder.h"
#include "PSSparkShower.h"
#include "RSBeamStar.h"
#include "RSDelayed.h"
#include "PSSpawnEffect.h"
#include "msg_fx.h"
#include "weapondef.h"
#include "pm_materials.h"
#include "screenfade.h"
#include "shake.h"

float BulletImpact(char chTextureType, vec3_t origin, vec3_t dir)
{
	if (UTIL_PointIsFar(origin, 1.5))
	return 0;

	if (chTextureType == CHAR_TEX_EMPTY)
	return 0;

	int contents = gEngfuncs.PM_PointContents(origin, NULL);
	BOOL InWater;

	if (contents == CONTENTS_WATER || contents == CONTENTS_SLIME || contents == CONTENTS_LAVA)
		InWater = TRUE;
	else
		InWater = FALSE;

	char *rgsz[4];
	vec3_t normal_angle;

	switch (chTextureType)
	{
	case CHAR_TEX_ENERGYSHIELD:

		g_pRenderManager->AddSystem(new CPSBlastCone(1, RANDOM_FLOAT(16, 24), origin, dir, VECTOR_CONE_LOTS, RANDOM_FLOAT(4,5), 50, 255,255,255, 1.0, -1.2, g_iModelIndexExplosion4, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

		if (!InWater)
		{
			DynamicLight(origin, RANDOM_LONG(50, 75), 0,90,250, 0.15, 0.0);
			gEngfuncs.pEfxAPI->R_SparkShower( origin );

			if (!UTIL_PointIsFar(origin, 0.5))
				g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(6,9), origin, 0.5, 0.01, 0, 100, 0.8, 128,128,128, 1, 0, g_iModelIndexExplosion4, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		}		

		rgsz[0] = "player/pl_shield_impact1.wav";
		rgsz[1] = "player/pl_shield_impact2.wav";
		rgsz[2] = "player/pl_shield_impact3.wav";
		rgsz[3] = "player/pl_shield_impact1.wav";
	break;

	case CHAR_TEX_SNOW:

		if (!InWater)
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(2,3), RANDOM_FLOAT(30,50), origin, dir, VECTOR_CONE_LOTS, 8, RANDOM_FLOAT(18,24), 200,200,200, 0.6, -0.6, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_6, kRenderTransAdd, 0.2), 0, -1);

			if (!UTIL_PointIsFar(origin, 0.5))			
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(50, 80), origin, dir, VECTOR_CONE_LOTS, RANDOM_FLOAT(0.7,1.3), 0, 255,255,255, 0.8, -0.2, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
		}

		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		rgsz[3] = "player/pl_dirt4.wav";
	break;

	case CHAR_TEX_TILE:
		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(220, 260), origin, dir, VECTOR_CONE_40DEGREES, RANDOM_FLOAT(0.5,0.9), 0, 150,150,150, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_TILE_0, PARTICLE_TILE_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		if (!InWater)
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(3,6), RANDOM_FLOAT(70,95), origin, dir, VECTOR_CONE_20DEGREES, 6, RANDOM_FLOAT(12,18), 0,0,0, 0.15, -0.25, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 0.15), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			DynamicLight(origin, RANDOM_LONG(40,55), 220,180,0, 0.15, 0.0);

			if (!UTIL_PointIsFar(origin, 0.5))
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(3,6), RANDOM_FLOAT(70,95), origin, dir, VECTOR_CONE_20DEGREES, 8, RANDOM_FLOAT(12,18), 128,128,128, 0.4, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_7, kRenderTransAdd, 0.15), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		}

		rgsz[0] = "weapons/ric1.wav";
		rgsz[1] = "debris/glass2.wav";
		rgsz[2] = "weapons/ric3.wav";
		rgsz[3] = "debris/glass4.wav";
	break;		

	case CHAR_TEX_ASPHALT:
		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(200, 250), origin, dir, VECTOR_CONE_40DEGREES, RANDOM_FLOAT(1.2,1.6), 0, 55,55,55, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_ASPHALT_0, PARTICLE_ASPHALT_5), kRenderTransAlpha, 0.15), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		if (!InWater)
		{
			gEngfuncs.pEfxAPI->R_BulletImpactParticles(origin);
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(100,120), origin, dir, VECTOR_CONE_20DEGREES, 5, 30, 0,0,0, 0.25, -0.33, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 0.15), 0, -1);
		}

		rgsz[0] = "debris/concrete1.wav";
		rgsz[1] = "debris/concrete2.wav";
		rgsz[2] = "debris/concrete3.wav";
		rgsz[3] = "debris/concrete4.wav";
	break;

	case CHAR_TEX_BRICK:
		if (!InWater)
		{
			gEngfuncs.pEfxAPI->R_BulletImpactParticles(origin);
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(7,9), RANDOM_FLOAT(110,130), origin, dir, Vector(0.2,0.2,0.2), 8, 45, 131,45,27, 0.4, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_4, kRenderTransAdd, 0.15), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		}

		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(200, 250), origin, dir, VECTOR_CONE_40DEGREES, RANDOM_FLOAT(1.2,1.6), 0, 165,165,165, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_BRICK_0, PARTICLE_BRICK_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		rgsz[0] = "debris/concrete1.wav";
		rgsz[1] = "debris/concrete2.wav";
		rgsz[2] = "debris/concrete3.wav";
		rgsz[3] = "debris/concrete4.wav";
	break;

	case CHAR_TEX_SAND:
		if (!InWater)
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(5,7), RANDOM_FLOAT(80,100), origin, dir, Vector(0.2,0.2,0.2), 5, 40, 210,140,10, 0.4, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_8, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

			if (!UTIL_PointIsFar(origin, 0.5))
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(80,100), origin, dir, Vector(0.2,0.2,0.2), 5, 40, 1,1,1, 0.1, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_7, kRenderTransAlpha, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		}

		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(190, 220), origin, dir, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(0.5,0.7), 0, 200,200,200, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_SAND_0, PARTICLE_SAND_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		rgsz[3] = "player/pl_dirt4.wav";
	break;

	case CHAR_TEX_SAND_ROCK:
		if (!InWater)
		{
			gEngfuncs.pEfxAPI->R_BulletImpactParticles(origin);
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(80,100), origin, dir, Vector(0.2,0.2,0.2), 5, 40, 255,225,0, 0.4, -0.45, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_7, kRenderTransAlpha, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		}

		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(210, 240), origin, dir, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(0.6,0.8), 0, 200,200,200, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_SAND_0, PARTICLE_SAND_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		rgsz[0] = "debris/concrete1.wav";
		rgsz[1] = "debris/concrete2.wav";
		rgsz[2] = "debris/concrete3.wav";
		rgsz[3] = "debris/concrete4.wav";
	break;

	case CHAR_TEX_ICE:
		if (!InWater)
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(3,4), RANDOM_FLOAT(20,40), origin, dir, VECTOR_CONE_40DEGREES, 8, RANDOM_FLOAT(18,24), 200,200,200, 0.5, -0.6, g_iModelIndexAnimSpr5, FALSE, 0, kRenderTransAdd, 0.2), 0, -1);
		}

		if (!UTIL_PointIsFar(origin, 0.5))
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(210, 240), origin, dir, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(0.75,1.0), 0, 200,200,200, 0.6, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_ICE_0, PARTICLE_ICE_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			g_pRenderManager->AddSystem(new CPSSparks(8, origin, 0.5, 0.025, 0, 100, 0.8, 128,128,128, 1, 0, g_iModelIndexAnimSpr7, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		}
		rgsz[0] = "weapons/glass1.wav";
		rgsz[1] = "debris/glass2.wav";
		rgsz[2] = "weapons/glass3.wav";
		rgsz[3] = "debris/glass4.wav";
	break;

	case CHAR_TEX_ROCK:
		if (!InWater)
		{
			gEngfuncs.pEfxAPI->R_BulletImpactParticles(origin);
			g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(8,12), origin, RANDOM_FLOAT(0.3,0.5), RANDOM_FLOAT(0.01,0.02), 0, RANDOM_FLOAT(140,180), RANDOM_FLOAT(1,1.5), 255,255,255, RANDOM_FLOAT(1,1.5), 0, g_iModelIndexFire, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
	     	g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(8,12), RANDOM_FLOAT(140,170), origin, dir, VECTOR_CONE_40DEGREES, 8, 15, 73,32,40, 0.2, -0.2, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_6, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		}

		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(200, 220), origin, dir, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(0.4,0.7), 0, 73,32,40, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_TILE_0, PARTICLE_TILE_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		rgsz[0] = "debris/concrete1.wav";
		rgsz[1] = "debris/concrete2.wav";
		rgsz[2] = "debris/concrete3.wav";
		rgsz[3] = "debris/concrete4.wav";
	break;

	case CHAR_TEX_LEAVES:
		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(120, 150), origin, dir, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(1.4,2.0), 0, 255,255,255, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_LEAVES_0,PARTICLE_LEAVES_7), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		rgsz[3] = "player/pl_dirt4.wav";
	break;


	case CHAR_TEX_CONCRETE:
	case CHAR_TEX_CEILING:

		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(200, 250), origin, dir, VECTOR_CONE_40DEGREES, RANDOM_FLOAT(1.2,1.6), 0, 128,128,128, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_CONCRETE_0,PARTICLE_CONCRETE_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		if (!InWater)
		{
			gEngfuncs.pEfxAPI->R_BulletImpactParticles(origin);
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(60,85), origin, dir, VECTOR_CONE_20DEGREES, 8, RANDOM_FLOAT(16,24), 128,128,128, 0.4, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_4, kRenderTransAdd, 0.15), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
//			DynamicLight(origin, RANDOM_LONG(35,50), 254,110,25, 0.15, 0.0);
		}
		rgsz[0] = "debris/concrete1.wav";
		rgsz[1] = "debris/concrete2.wav";
		rgsz[2] = "debris/concrete3.wav";
		rgsz[3] = "debris/concrete4.wav";
	break;

	case CHAR_TEX_GRASS:

		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(80, 100), origin, dir, VECTOR_CONE_40DEGREES, RANDOM_FLOAT(1.2,1.6), 0, 200,200,200, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_GRASS_0, PARTICLE_GRASS_3), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
	
		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		rgsz[3] = "player/pl_dirt4.wav";
	break;

	case CHAR_TEX_DIRT:
		if (!InWater)
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(12,20), origin, dir, VECTOR_CONE_20DEGREES, 8, RANDOM_FLOAT(14,18), 86,61,44, 0.6, -0.2, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_2, kRenderTransAdd, 0.15), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(120, 180), origin, dir, VECTOR_CONE_40DEGREES, RANDOM_FLOAT(0.6,1.0), 0, 86,61,44, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_ASPHALT_0, PARTICLE_ASPHALT_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
		
		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		rgsz[3] = "player/pl_dirt4.wav";
	break;

	case CHAR_TEX_GRATE:
	case CHAR_TEX_COMPUTER:

		if (!InWater)
		{
			if (!UTIL_PointIsFar(origin, 0.5))
				g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(12,15), origin, RANDOM_FLOAT(0.35,0.5), RANDOM_FLOAT(0.02,0.03), 0, RANDOM_FLOAT(200,250), RANDOM_FLOAT(0.8,1.2), 255,255,255, RANDOM_FLOAT(1,1.5), 0, g_iModelIndexAnimSpr8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);

			gEngfuncs.pEfxAPI->R_SparkStreaks(origin,RANDOM_FLOAT(30,50),-50,50);
			gEngfuncs.pEfxAPI->R_SparkShower(origin);
			DynamicLight(origin, RANDOM_LONG(45,55), 250,160,0, 0.15, 0.0);
		}

		rgsz[0] = "debris/metal1.wav";
		rgsz[1] = "debris/metal2.wav";
		rgsz[2] = "debris/metal3.wav";
		rgsz[3] = "debris/metal4.wav";
	break;


	case CHAR_TEX_VENT:
	case CHAR_TEX_METAL:

		if (!InWater)
		{
			if (!UTIL_PointIsFar(origin, 0.5))
				g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(20,28), origin, RANDOM_FLOAT(0.5,0.8), RANDOM_FLOAT(0.01,0.02), 0, RANDOM_FLOAT(120,150), RANDOM_FLOAT(1,1.5), 255,255,255, 1, 0, g_iModelIndexAnimSpr8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

			gEngfuncs.pEfxAPI->R_SparkShower(origin);
			DynamicLight(origin, RANDOM_LONG(50,70), 254,110,25, 0.15, 0.0);			
		}
		rgsz[0] = "weapons/ric1.wav";
		rgsz[1] = "weapons/ric2.wav";
		rgsz[2] = "weapons/ric3.wav";
		rgsz[3] = "weapons/ric4.wav";
	break;

	case CHAR_TEX_WOOD:

		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(120, 170), origin, dir, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(0.8,1.6), 0, 200,200,200, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_WOOD_0, PARTICLE_WOOD_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		if (!InWater)
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(2,3), RANDOM_FLOAT(45,55), origin, dir, Vector(0.2,0.2,0.2), 8, 15, 100,60,0, 0.2, -0.3, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

			if (!UTIL_PointIsFar(origin, 0.5))			
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(2,3), RANDOM_FLOAT(45,55), origin, dir, Vector(0.2,0.2,0.2), 8, 20, 100,60,0, 0.4, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_6, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		}

		rgsz[0] = "debris/wood1.wav";
		rgsz[1] = "debris/wood2.wav";
		rgsz[2] = "debris/wood3.wav";
		rgsz[3] = "debris/wood4.wav";
	break;

	case CHAR_TEX_GLASS:
	
		if (!UTIL_PointIsFar(origin, 0.5))
			g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(160, 210), origin, dir, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(0.9,1.6), 0, 255,255,255, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_GLASS_0, PARTICLE_GLASS_5), kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		if (!InWater)
			gEngfuncs.pEfxAPI->R_SparkShower(origin);

		rgsz[0] = "debris/glass1.wav";
		rgsz[1] = "debris/glass2.wav";
		rgsz[2] = "debris/glass3.wav";
		rgsz[3] = "debris/glass4.wav";
	break;

	case CHAR_TEX_FLESH:
     	g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), 20, origin, origin, Vector(1,1,1), 5, RANDOM_FLOAT(20,26), 90,0,0, 0.5, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_6, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

		if (!UTIL_PointIsFar(origin, 0.5))		
			g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(90, 120), origin, dir, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(1.2,1.6), 0, 90,0,0, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_CONCRETE_0,PARTICLE_CONCRETE_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

		rgsz[0] = "debris/flesh1.wav";
		rgsz[1] = "debris/flesh2.wav";
		rgsz[2] = "debris/flesh3.wav";
		rgsz[3] = "debris/flesh4.wav";
		break;

	case CHAR_TEX_SLOSH:
	case CHAR_TEX_WATER:
		rgsz[0] = "player/pl_slosh1.wav";
		rgsz[1] = "player/pl_slosh3.wav";
		rgsz[2] = "player/pl_slosh2.wav";
		rgsz[3] = "player/pl_slosh4.wav";
	break;

	default:
		return 0.0;
		break;
	}
	if (!UTIL_PointIsFar(origin, 0.5))
		EMIT_SOUND(0, origin, CHAN_VOICE, rgsz[RANDOM_LONG(0,3)], 1, 1, 0, RANDOM_LONG(96,104));

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int __MsgFunc_ViewModel(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	cl_entity_t *view = gEngfuncs.GetViewModel();
	if (view)
	{
		view->curstate.rendermode		= READ_BYTE();
		view->curstate.renderfx			= READ_BYTE();
		view->curstate.rendercolor.r	= READ_BYTE();
		view->curstate.rendercolor.g	= READ_BYTE();
		view->curstate.rendercolor.b	= READ_BYTE();
		view->curstate.renderamt		= READ_BYTE();
		view->curstate.skin				= READ_BYTE();
	}
// don't display errors	END_READ();
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int __MsgFunc_Particles(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	vec3_t origin;
	origin[0] =		READ_COORD();
	origin[1] =		READ_COORD();
	origin[2] =		READ_COORD();
	float rnd_vel =	(float)READ_SHORT()*0.1f;
	float life =	(float)READ_SHORT()*0.1f;
	byte color =	READ_BYTE();
	byte number =	READ_BYTE();
	END_READ();
	ParticlesCustom(origin, rnd_vel, color, 0, number, life);
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int __MsgFunc_PartSys(const char *pszName, int iSize, void *pbuf)
{
	if (!g_pRenderManager)
		return 1;

	vec3_t origin, dir, spread;
	BEGIN_READ(pbuf, iSize);
	origin[0] = READ_COORD();
	origin[1] = READ_COORD();
	origin[2] = READ_COORD();
	dir[0] = READ_COORD();
	dir[1] = READ_COORD();
	dir[2] = READ_COORD();
	spread[0] = READ_COORD();
	spread[1] = READ_COORD();
	spread[2] = READ_COORD();
	float velocity	= (float)READ_SHORT();
	short sprindex	= READ_SHORT();
	byte r_mode		= READ_BYTE();
	short maxpart	= READ_SHORT();
	float life		= (float)READ_SHORT()*0.1f;
	short entindex	= READ_SHORT();
	short size		= READ_BYTE()*10.0f;
	short size_d	= READ_BYTE()*10.0f;
	byte r			= READ_BYTE();
	byte g			= READ_BYTE();
	byte b			= READ_BYTE();
	float a			= (float)READ_BYTE()/255.0f;
	float a_delta	= (float)READ_BYTE()/255.0f;
	byte animate	= READ_BYTE();
	byte frame		= READ_BYTE();
	short flags		= READ_SHORT();
	byte type		= READ_BYTE();
	END_READ();

	CParticleSystem *pSystem = NULL;
	if (type == PARTSYSTEM_TYPE_REMOVEANY)
	{
		pSystem = (CParticleSystem *)g_pRenderManager->FindSystemByFollowEntity(entindex);// FIFO: first in list will be found and removed
		if (pSystem)// found
		{
			pSystem->dying = true;
			pSystem->m_fDieTime = gEngfuncs.GetClientTime();
		}
	}
	else
	{
		pSystem = new CPSBlastCone(maxpart, velocity, origin, dir, spread, size, size_d, r,g,b, a, -a_delta, sprindex, (animate > 0)?TRUE:FALSE, frame, r_mode, life);
	
		if (pSystem)
			g_pRenderManager->AddSystem(pSystem, flags, entindex);

	//	CON_PRINTF("PartSys origin = %f %f %f\n", origin[0], origin[1], origin[2]);
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void SnowCallback(struct particle_s *particle, float frametime)
{
	for (int i = 0; i < 3; i++)
		particle->org[i] += particle->vel[i]*frametime;

//	if (gEngfuncs.PM_PointContents(particle->org + particle->vel*frametime, NULL) != CONTENTS_EMPTY)
//		particle->die = gEngfuncs.GetClientTime();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int __MsgFunc_Snow(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int idx =		READ_SHORT();
	int life =		READ_SHORT();
	int num =		READ_SHORT();
	int color =		READ_BYTE();
	int type =		READ_BYTE();
	END_READ();
/*
	cl_entity_t *pEnt = gEngfuncs.GetEntityByIndex(idx);
	if (!pEnt)
		return 1;

	model_s *pModel = pEnt->model;*/
	model_s *pModel = IEngineStudio.GetModelByIndex(idx);
	if (!pModel || pModel->type != mod_brush)
	{
		CON_DPRINTF("ERROR: MsgFunc_Snow: not a brush model!\n");
		return 1;
	}

	vec3_t sz = pModel->maxs - pModel->mins;
	vec3_t origin;
	vec3_t velocity;
	for (int i = 0; i<num; ++i)
	{
		origin[0] = pModel->mins[0] + RANDOM_FLOAT(0, sz[0]);
		origin[1] = pModel->mins[1] + RANDOM_FLOAT(0, sz[1]);
		origin[2] = pModel->mins[2] + RANDOM_FLOAT(0, sz[2]);
		velocity[0] = RANDOM_FLOAT(-2, 2);
		velocity[1] = RANDOM_FLOAT(-2, 2);
		velocity[2] = RANDOM_FLOAT(-1, -0.6);
		particle_t *p = gEngfuncs.pEfxAPI->R_AllocParticle(SnowCallback);
		if (p != NULL)
		{
			p->type = (ptype_t)type;
			p->color = color;
			gEngfuncs.pEfxAPI->R_GetPackedColor(&p->packedColor, p->color);
			VectorCopy(origin, p->org);
			VectorCopy(velocity, p->vel);
			p->die = gEngfuncs.GetClientTime() + (float)life * 0.1f;
		}
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int __MsgFunc_DLight(const char *pszName, int iSize, void *pbuf)
{
	vec3_t origin;
	BEGIN_READ(pbuf, iSize);
	origin[0]		= READ_COORD();
	origin[1]		= READ_COORD();
	origin[2]		= READ_COORD();
	short radius	= READ_SHORT();
	byte r			= READ_BYTE();
	byte g			= READ_BYTE();
	byte b			= READ_BYTE();
	byte minlight	= READ_BYTE();
	byte decay		= READ_BYTE();
	byte dark		= READ_BYTE();
	short life		= READ_SHORT();
	END_READ();

//	CON_DPRINTF("MsgFunc_DLight: rad %d, r %d, g %d, b %d, ml %d, dc %d, dr %d, lf %d\n", radius,r,g,b,minlight,decay,dark,life);
	dlight_t *dl = DynamicLight(origin, (float)radius*0.1f, r,g,b, (float)life*0.1f, (float)decay*0.1f);
	if (dl)
	{
		dl->minlight = (float)minlight;

		if (dark > 0)
			dl->dark = true;
		else
			dl->dark = false;
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int __MsgFunc_ELight(const char *pszName, int iSize, void *pbuf)
{
	vec3_t origin;
	BEGIN_READ(pbuf, iSize);
	origin[0]		= READ_COORD();
	origin[1]		= READ_COORD();
	origin[2]		= READ_COORD();
	short radius	= READ_SHORT();
	byte r			= READ_BYTE();
	byte g			= READ_BYTE();
	byte b			= READ_BYTE();
	byte minlight	= READ_BYTE();
	byte decay		= READ_BYTE();
	byte dark		= READ_BYTE();
	short life		= READ_SHORT();
	END_READ();

	dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocElight(0);
	if (dl)
	{
		VectorCopy(origin, dl->origin);
		dl->radius = (float)radius * 0.1f;
		dl->color.r = r;
		dl->color.g = g;
		dl->color.b = b;
		dl->minlight = (float)minlight * 0.1f;
		dl->decay = (float)decay * 0.1f;
		if (dark > 0)
			dl->dark = true;
		else
			dl->dark = false;

		dl->die = gEngfuncs.GetClientTime() + (float)life * 0.1f;
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int __MsgFunc_SetFog(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	byte cr = READ_BYTE();
	byte cg = READ_BYTE();
	byte cb = READ_BYTE();
	float startdist = (float)READ_SHORT();
	float enddist = (float)READ_SHORT();
	byte mode = READ_BYTE();
	END_READ();

//	CON_DPRINTF(">>>> CL RECV: SetFog %d %d %d, %f, %f, %d\n", cr,cg,cb, startdist, enddist, mode);
	gHUD.m_iFogMode = mode;

	if (mode > 0)
		RenderFog(cr,cg,cb, startdist, enddist, false);
	else
		ResetFog();

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int __MsgFunc_SetSky(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	gHUD.m_vecSkyPos[0] = READ_COORD();
	gHUD.m_vecSkyPos[1] = READ_COORD();
	gHUD.m_vecSkyPos[2] = READ_COORD();
	gHUD.m_iSkyMode = READ_BYTE();
	END_READ();
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: env_rain msg
// XDM3037: revised: rain now can be harmlessly toggled even if not completely shut down (active while dying > 0)
//-----------------------------------------------------------------------------
int __MsgFunc_SetRain(const char *pszName, int iSize, void *pbuf)
{
	if (!g_pRenderManager)
		return 1;

	vec3_t v;
	BEGIN_READ(pbuf, iSize);
	v[0] =			READ_COORD();
	v[1] =			READ_COORD();
	v[2] =			READ_COORD();
	short entindex =READ_SHORT();// just an index, GetModelByIndex() will not find entity with EF_NODRAW on client side
	short mdlindex =READ_SHORT();// transmit this separately (mins/maxs)
	short sprindex1=READ_SHORT();
	short sprindex2=READ_SHORT();
	short sprindex3=READ_SHORT();// XDM3035c
	short maxpart =	READ_SHORT();
	float life =	(float)READ_SHORT()*0.1f;
	float scalex =	(float)READ_BYTE()*0.1f;
	float scaley =	(float)READ_BYTE()*0.1f;
	byte r_mode =	READ_BYTE();
	byte r =		READ_BYTE();
	byte g =		READ_BYTE();
	byte b =		READ_BYTE();
	byte a =		READ_BYTE();
	short flags =	READ_SHORT();// XDM3035: extended
	END_READ();

	if (entindex <= 0)
	{
		CON_DPRINTF("CL: SetRain: bad entindex!\n");
		return 1;
	}

//	CON_DPRINTF("CL: SetRain: entindex: %d modelindex: %d\n", entindex, mdlindex);
	CPSDrips *psystem = NULL;// search for system even if the entindex is not valid
	psystem = (CPSDrips *)g_pRenderManager->FindSystemByFollowEntity(entindex);// don't allow more than one system per entity
	if (psystem != NULL)// found
	{
		CON_DPRINTF("CL: SetRain: system found %d %d", entindex, mdlindex);
//		if (type == PARTSYSTEM_TYPE_REMOVEANY)// XDM3035: !!!
		if (mdlindex == 0 && sprindex1 == 0 && sprindex2 == 0)// XDM3035: !!! since we don't transmit system type
		{
			CON_DPRINTF("shutting down.\n");
			psystem->dying = true;
			psystem->m_fDieTime = gEngfuncs.GetClientTime();
		}
		else// if (psystem->dying)
		{
			if (psystem->dying)
			{
				CON_DPRINTF("reviving.\n");
				psystem->dying = false;
			}
			else
				CON_DPRINTF("updating.\n");

//			if (life <= 0.0f)
//				psystem->m_fDieTime = -1;
//			else
//				psystem->m_fDieTime = gEngfuncs.GetClientTime() + life;
//		}
//		else// just update parameters BUGBUG: this makes rain 'toggle' after player's death in multiplayer
//		{
//			CON_DPRINTF("updating.\n");
			psystem->InitTexture(sprindex1);
			psystem->m_color.r = r;
			psystem->m_color.g = g;
			psystem->m_color.b = b;
			psystem->m_fBrightness = (float)a/255.0f;
			psystem->m_iRenderMode = r_mode;
			psystem->m_iFlags = flags;
			psystem->SetParticleSize(scalex, scaley);
			if (life <= 0.0f)
				psystem->m_fDieTime = -1;
			else
				psystem->m_fDieTime = gEngfuncs.GetClientTime() + life;
// DO NOT DO THIS!!! requires memory reallocation!			psystem->m_iMaxParticles = maxpart;
		}
		return 1;
	}

	model_s *pModel = IEngineStudio.GetModelByIndex(mdlindex);
	if (!pModel || pModel->type != mod_brush)
	{
		CON_DPRINTF("ERROR: SetRain: %d is not a brush model!\n", mdlindex);
		return 1;
	}

	CON_DPRINTF("CL: SetRain: creating %d %d\n", entindex, mdlindex);

	vec3_t org = g_vecZero;
	cl_entity_t *ent = gEngfuncs.GetEntityByIndex(entindex);
	if (ent != NULL)
		VectorCopy(ent->curstate.origin, org);

	// UNDONE: what about complex brushes?
//	CON_PRINTF("mins %f %f %f  maxs %f %f %f\n", pModel->mins[0], pModel->mins[1], pModel->mins[2], pModel->maxs[0], pModel->maxs[1], pModel->maxs[2]);
	if (g_pRenderManager)
	{
		CPSDrips *pRS = new CPSDrips(maxpart, org, pModel->mins, pModel->maxs, v, sprindex1, sprindex2, sprindex3, r_mode, scalex, scaley,/*2.0, 32.0,*/ 0.0f, 1.0, life);
		if (pRS && !pRS->IsRemoving())
		{
			pRS->m_color.r = r;
			pRS->m_color.g = g;
			pRS->m_color.b = b;
			pRS->m_fBrightness = (float)a/255.0f;
			g_pRenderManager->AddSystem(pRS, flags, entindex);
		}
	}

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: XDM3035: create static architecture using a temporary entity to save network bandwidth
// Output : int
//-----------------------------------------------------------------------------
int __MsgFunc_StaticEnt(const char *pszName, int iSize, void *pbuf)
{
	Vector origin, angles;
	color24 rendercolor;
	BEGIN_READ(pbuf, iSize);
	short entindex	= READ_SHORT();
	short modelindex= READ_SHORT();
	origin[0]		= READ_COORD();
	origin[1]		= READ_COORD();
	origin[2]		= READ_COORD();//+1.0f;// HACK
	angles[0]		= READ_ANGLE();
	angles[1]		= READ_ANGLE();
	angles[2]		= READ_ANGLE();
	byte rendermode	= READ_BYTE();
	byte renderfx	= READ_BYTE();
	rendercolor.r	= READ_BYTE();
	rendercolor.g	= READ_BYTE();
	rendercolor.b	= READ_BYTE();
	byte renderamt	= READ_BYTE();
//	byte effects	= READ_BYTE();
	byte body		= READ_BYTE();
	byte skin		= READ_BYTE();
	float scale		= READ_COORD();
	short sequence	= READ_SHORT();
	short bsp_leaf	= READ_SHORT();
	END_READ();

	CRSModel *pSystem = NULL;// search for system even if the entindex is not valid
	pSystem = (CRSModel *)g_pRenderManager->FindSystemByFollowEntity(entindex);// don't allow more than one system per entity
	if (pSystem)// found, update
	{
		if (modelindex == 0)// signal to remove
		{
			pSystem->dying = true;
			pSystem->m_fDieTime = gEngfuncs.GetClientTime();
		}
		else
		{
			if (pSystem->GetEntity())
			{
				pSystem->InitModel(modelindex);
				pSystem->m_vecOrigin = origin;
				pSystem->m_vecAngles = angles;
				pSystem->m_iRenderMode = rendermode;
				pSystem->m_color = rendercolor;
				pSystem->m_fBrightness = (float)renderamt/255.0f;
				pSystem->GetEntity()->curstate.renderfx = renderfx;
//				pSystem->GetEntity()->curstate.effects = effects;
				pSystem->GetEntity()->curstate.body = body;
				pSystem->GetEntity()->curstate.skin = skin;
				pSystem->GetEntity()->curstate.sequence = sequence;
				pSystem->m_fScale = scale;
				pSystem->m_iBSPLeaf = bsp_leaf;
			}
		}
	}
	else// create new system
	{
		pSystem = new CRSModel(origin, angles, g_vecZero, entindex, modelindex, body, skin, sequence, rendermode, renderfx, rendercolor.r, rendercolor.g, rendercolor.b, (float)renderamt/255.0f, 0.0f, scale, 0.0f, 1.0f, 0.0f);
		pSystem->m_iBSPLeaf = bsp_leaf;
		g_pRenderManager->AddSystem(pSystem, RENDERSYSTEM_FLAG_NOCLIP|RENDERSYSTEM_FLAG_DONTFOLLOW, entindex, RENDERSYSTEM_FFLAG_DONTFOLLOW);
//#ifdef _DEBUG
//		CON_DPRINTF("CL: created static model %d: RenderSystem %d\n", modelindex, idx);
//#endif
	}
#if 0 // OLD STYLE, causes overflow 9000 temporary ents!
	TEMPENTITY *pStaticEntity = NULL;
	model_s *pModel = IEngineStudio.GetModelByIndex(modelindex);
	if (!pModel)// || pModel->type != mod_studio)
		return 0;

	pStaticEntity = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
	if (pStaticEntity != NULL)
	{
#ifdef _DEBUG
		CON_DPRINTF("CL: adding static entity: %d (%s)\n", entindex, pModel->name);
#endif
		pStaticEntity->flags |= (FTENT_PERSIST | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP);
		pStaticEntity->priority = TENTPRIORITY_HIGH;// they must not disappear!!
//		pStaticEntity->clientIndex = entindex;
		pStaticEntity->entity.index = entindex;
		pStaticEntity->x = origin[0];
		pStaticEntity->y = origin[1];
		pStaticEntity->z = origin[2];
		pStaticEntity->clientIndex = 0;
		pStaticEntity->fadeSpeed = 0.0f;
		pStaticEntity->bounceFactor = 0.0f;
		VectorCopy(origin, pStaticEntity->entity.origin);
		VectorCopy(angles, pStaticEntity->entity.angles);
//NONONO!!		VectorCopy(origin, pStaticEntity->entity.baseline.origin);
		VectorClear(pStaticEntity->entity.baseline.origin);

		VectorCopy(angles, pStaticEntity->entity.baseline.angles);
//		VectorCopy(origin, pStaticEntity->entity.curstate.origin);
//		VectorCopy(angles, pStaticEntity->entity.curstate.angles);
		VectorClear(pStaticEntity->entity.baseline.velocity);
//		VectorClear(pStaticEntity->entity.curstate.velocity);

		/*pStaticEntity->entity.curstate.rendermode = */pStaticEntity->entity.baseline.rendermode = rendermode;
		pStaticEntity->entity.baseline.modelindex = modelindex;
		pStaticEntity->entity.baseline.renderfx = renderfx;
		pStaticEntity->entity.baseline.rendercolor.r = rendercolor.r;
		pStaticEntity->entity.baseline.rendercolor.g = rendercolor.g;
		pStaticEntity->entity.baseline.rendercolor.b = rendercolor.b;
		pStaticEntity->entity.baseline.renderamt = renderamt;
		pStaticEntity->entity.baseline.effects = EF_INVLIGHT;//effects;
		pStaticEntity->entity.baseline.eflags = EFLAG_DRAW_ALWAYS;
		pStaticEntity->entity.baseline.body = body;
		pStaticEntity->entity.baseline.skin = skin;
		pStaticEntity->entity.baseline.scale = scale;
		pStaticEntity->entity.baseline.sequence = sequence;

		pStaticEntity->entity.baseline.gravity = 0.0f;
		pStaticEntity->entity.baseline.solid = SOLID_NOT;
		pStaticEntity->entity.baseline.movetype = MOVETYPE_NONE;

		VectorClear(pStaticEntity->entity.baseline.startpos);
		VectorClear(pStaticEntity->entity.baseline.endpos);

//		memcpy(&pStaticEntity->entity.curstate, &pStaticEntity->entity.baseline, sizeof(entity_state_t));
		pStaticEntity->die = -1.0f;//gEngfuncs.GetClientTime() + 10000;

/*test for CTF_InfernalVillage2
	if (!stricmp(pModel->name, "models/env/tree_greyv1.mdl"))
	{
		pStaticEntity->flags |= (FTENT_SMOKETRAIL|FTENT_FLICKER);
		pStaticEntity->entity.baseline.rendercolor.r = 255;
		pStaticEntity->entity.baseline.rendercolor.g = 0;
		pStaticEntity->entity.baseline.rendercolor.b = 0;
		pStaticEntity->entity.baseline.renderamt = 255;
		pStaticEntity->z += 1.0f;
//		pStaticEntity->entity.baseline.origin.z = 1.0f;
		CON_DPRINTF("CL: WTDF!!!\n");
	}*/
	}
#endif
	return 1;
}


//-----------------------------------------------------------------------------
// Purpose: XDM3035a: create static sprite using RenderSystem to save network bandwidth
// Output : int
//-----------------------------------------------------------------------------
int __MsgFunc_StaticSpr(const char *pszName, int iSize, void *pbuf)
{
	Vector origin, angles;
	color24 rendercolor;
	BEGIN_READ(pbuf, iSize);
	short entindex	= READ_SHORT();
	short sprindex	= READ_SHORT();
	origin[0]		= READ_COORD();
	origin[1]		= READ_COORD();
	origin[2]		= READ_COORD();//+1.0f;// HACK
	angles[0]		= READ_ANGLE();
	angles[1]		= READ_ANGLE();
	angles[2]		= READ_ANGLE();
	byte rendermode	= READ_BYTE();
	rendercolor.r	= READ_BYTE();
	rendercolor.g	= READ_BYTE();
	rendercolor.b	= READ_BYTE();
	byte renderamt	= READ_BYTE();
	byte effects	= READ_BYTE();
	float scale		= READ_COORD();
	byte framerate	= READ_BYTE();
//	short bsp_leaf	= READ_SHORT();
	END_READ();
/* must be done on server
	if (rendermode == kRenderTransAdd || rendermode == kRenderTransTexture || rendermode == kRenderGlow)
	{
		if (rendercolor.r == 0 && rendercolor.g == 0 && rendercolor.b == 0)
		{
			rendercolor.r = 255;
			rendercolor.g = 255;
			rendercolor.b = 255;
		}
	}
*/
	// search for system even if the entindex is not valid
	CRSSprite *pSystem = (CRSSprite *)g_pRenderManager->FindSystemByFollowEntity(entindex);// don't allow more than one system per entity
	if (pSystem)// found, update
	{
		if (sprindex == 0)// signal to remove
		{
			pSystem->dying = true;
			pSystem->m_fDieTime = gEngfuncs.GetClientTime();
		}
		else
		{
			pSystem->m_vecOrigin = origin;
			pSystem->m_vecAngles = angles;
			pSystem->m_iRenderMode = rendermode;
//			pSystem->m_iRenderEffects = renderfx;
			if (effects & EF_NODRAW)
				pSystem->m_iFlags |= RENDERSYSTEM_FLAG_NODRAW;
			else
				pSystem->m_iFlags &= ~RENDERSYSTEM_FLAG_NODRAW;

			pSystem->m_color = rendercolor;
			pSystem->m_fBrightness = (float)renderamt/255.0f;
			pSystem->m_fScale = scale;
			pSystem->m_fFrameRate = (float)framerate;
		}
	}
	else// create new system
	{
		pSystem = new CRSSprite(origin, g_vecZero, sprindex, rendermode, rendercolor.r, rendercolor.g, rendercolor.b, (float)renderamt/255.0f, 0.0f, scale, 0.0f, (float)framerate, 0.0f);
		g_pRenderManager->AddSystem(pSystem, RENDERSYSTEM_FLAG_NOCLIP|RENDERSYSTEM_FLAG_LOOPFRAMES, entindex, RENDERSYSTEM_FFLAG_DONTFOLLOW);
#ifdef _DEBUG
		CON_DPRINTF("CL: created static sprite %d: RenderSystem %d\n", sprindex);
#endif
	}

	return 1;
}


//-----------------------------------------------------------------------------
// Purpose: Quake beam (draws with model)
// Output : int
//-----------------------------------------------------------------------------
int __MsgFunc_FireLgtng(const char *pszName, int iSize, void *pbuf)
{
	vec3_t startPos, endPos;
	BOOL InWater;
	BOOL InSky;

	BEGIN_READ(pbuf, iSize);
	startPos[0]		= READ_COORD();
	startPos[1]		= READ_COORD();
	startPos[2]		= READ_COORD();
	endPos[0]		= READ_COORD();
	endPos[1]		= READ_COORD();
	endPos[2]		= READ_COORD();
	short EntIndex = READ_SHORT();
	byte Type		= READ_BYTE();
	END_READ();

	int contents = gEngfuncs.PM_PointContents(endPos, NULL);

	if (contents == CONTENTS_WATER || contents == CONTENTS_SLIME || contents == CONTENTS_LAVA)
		InWater = TRUE;
	else
		InWater = FALSE;

	if (contents == CONTENTS_SKY)
		InSky = TRUE;
	else
		InSky = FALSE;

	switch (Type)
	{
		case TYPE_LGTNG_BEAM:
		{
			CL_AllocBeam( "models/effects/ef_lightning_field.mdl", EntIndex, startPos, endPos);

			if (!InSky)
			{
				if (!UTIL_PointIsFar(endPos, 1.0) && !InWater)
				{
  					g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(24,32), endPos, RANDOM_FLOAT(0.5,0.9), RANDOM_FLOAT(0.01,0.02), 0, RANDOM_FLOAT(180,220), RANDOM_FLOAT(1.5,2.0), 255,255,255, RANDOM_FLOAT(0.8, 1.3), 0, g_iModelIndexAnimSpr7, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
 					g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(1,2), RANDOM_FLOAT(0.5,0.9), RANDOM_FLOAT(200, 220), SPARKSHOWER_SPARKS, endPos, Vector(0,0,1), Vector(0.2,0.2,0.4), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS  | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL | RENDERSYSTEM_FLAG_DRAWDECAL, -1);

					switch (RANDOM_LONG(0,2))
					{
						case 0:	EMIT_SOUND( 0, endPos, CHAN_BODY, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 120 ); break;
						case 1:	EMIT_SOUND( 0, endPos, CHAN_ITEM, "weapons/electro5.wav", 1.0, ATTN_NORM, 0, 120 ); break;
						case 2:	EMIT_SOUND( 0, endPos, CHAN_STATIC, "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					}
				}
			}
		}
		break;

		case TYPE_LIGHTNING_BALL_BEAM:
		{
			gEngfuncs.pEfxAPI->R_BeamPoints(startPos, endPos, g_iModelIndexBeamsAll, 0.15, 4.5, 0.4, 200, 15, BLAST_SKIN_LIGHTNING, 0, 255, 255, 255 );
			EMIT_SOUND( 0, startPos, CHAN_VOICE, "weapons/fire_lightning.wav", 1, ATTN_NORM, 0, PITCH_NORM );

			if (!InSky)
			{
				if (!UTIL_PointIsFar(endPos, 1.0) && !InWater)
				{
  					g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(1,2), RANDOM_FLOAT(0.8,1.2), RANDOM_FLOAT(200, 220), SPARKSHOWER_SPARKS2, endPos, Vector(0,0,1), Vector(0.2,0.2,0.4), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
					g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(24,32), endPos, RANDOM_FLOAT(0.8,1.0), RANDOM_FLOAT(0.01,0.02), 0, RANDOM_FLOAT(180,220), RANDOM_FLOAT(1.5,2.0), 255,255,255, 1, 0, g_iModelIndexExplosion4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				}
   			}
		}
		break;

		case TYPE_GHOSTMISSILE_BEAM:
		{
			gEngfuncs.pEfxAPI->R_BeamPoints(startPos, endPos, g_iModelIndexAnimSpr2, 0.15, 1, 0, 1, 30, 0, 25, 255,255,255);
			CRSSprite *pSys = new CRSSprite(startPos, g_vecZero, g_iModelIndexPartRed, kRenderTransAdd, 255,255,255, 1.0f,-0.33f, 1.0, -2.0f, 0.0f, 0.5);
			if (pSys)
			{
				g_pRenderManager->AddSystem(pSys, 0, -1, 0);
				pSys->m_iFrame = PARTICLE_RED_36;
			}
			if (!UTIL_PointIsFar(endPos, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSpawnEffect(30, startPos, 2, 0, 10, 10, g_iModelIndexExplosion5, kRenderTransAdd, 200,200,200, 1.0, -1.0, 1));
				EMIT_SOUND(0, startPos, CHAN_VOICE, "weapons/fire_ghostmissile_teleport.wav", 1, ATTN_NORM, 0, PITCH_NORM );

				if(!InWater)
				{
					g_pRenderManager->AddSystem(new CPSSparks(24, startPos, 0.5f, 0.025f, 0.0f, 160.0f, 0.8f, 255,255,255,1.0f, -0.6f, g_iModelIndexAnimSpr2, kRenderTransAdd, 1.5f), RENDERSYSTEM_FLAG_RANDOMFRAME|RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_ADDPHYSICS|RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
					g_pRenderManager->AddSystem(new CRSSprite(endPos, Vector(0,0,0), g_iModelIndexAnimSpr4, kRenderTransAdd, 255,255,255, 0.8, 0.0, 0.5, -0.25, 20.0, 2));
					g_pRenderManager->AddSystem(new CPSSparks(32, endPos, 0.6, 0.04, 0, -200, 1, 200,200,200, 1, 0, g_iModelIndexAnimSpr5, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				}	
			}
		}
		break;

		case TYPE_PARTICLE_BEAM:
		{
			g_pRenderManager->AddSystem(new CPSSparks(32, startPos, 0.3f, 0.025f, 0.0f, 100.0f, 0.4f, 255,255,255, 1.0f, -2.0f, g_iModelIndexAnimSpr3, kRenderTransAdd, 0.5f), RENDERSYSTEM_FLAG_RANDOMFRAME|RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_ADDPHYSICS|RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			gEngfuncs.pEfxAPI->R_BeamPoints(startPos, endPos, g_iModelIndexAnimSpr3, 0.15, 1, 0, 1, 30, 0, 25, 255,255,255);
		}
		break;
	}
	return 1;
}

int __MsgFunc_Bubbles(const char *pszName, int iSize, void *pbuf)
{
	Vector v1, v2;
	BEGIN_READ(pbuf, iSize);
	byte type = READ_BYTE();
	byte count = READ_BYTE();
	v1[0] = READ_COORD();
	v1[1] = READ_COORD();
	v1[2] = READ_COORD();
	v2[0] = READ_COORD();
	v2[1] = READ_COORD();
	v2[2] = READ_COORD();
	END_READ();

	if (g_pRenderManager)
		g_pRenderManager->AddSystem(
			new CPSBubbles(count, type, v1, v2, BUBBLE_SPEED, g_iModelIndexBubble, kRenderTransAlpha, 1.0f, 0.0f, BUBBLE_SCALE, 0.0f, BUBBLE_LIFE),
			RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_CLIPREMOVE|RENDERSYSTEM_FLAG_INCONTENTSONLY, -1);

	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Item/weapon/ammo respawn effect (used to be an event, but that was unreliable)
// Output : int
//-----------------------------------------------------------------------------
int __MsgFunc_ItemSpawn(const char *pszName, int iSize, void *pbuf)
{
	Vector origin, angles;
	BEGIN_READ(pbuf, iSize);
	byte type		= READ_BYTE();
	short entindex	= READ_SHORT();
	origin[0]		= READ_COORD();
	origin[1]		= READ_COORD();
	origin[2]		= READ_COORD();//+1.0f;// HACK
	angles[0]		= READ_ANGLE();
	angles[1]		= READ_ANGLE();
	angles[2]		= READ_ANGLE();
	short modelindex= READ_SHORT();
	float scale		= 0.1f*(float)READ_BYTE();
	byte body		= READ_BYTE();
	byte skin		= READ_BYTE();
	byte sequence	= READ_BYTE();
	END_READ();

	color24 c;
	bool toofar = UTIL_PointIsFar(origin, 1.0);
	if (type == EV_ITEMSPAWN_ITEM)
	{
		c.r = 127;
		c.g = 127;
		c.b = 255;
		EMIT_SOUND(entindex, origin, CHAN_WEAPON, "items/respawn.wav", VOL_NORM, ATTN_NORM, 0, 110);

		if (!toofar)
		{
			FX_StreakSplash(origin, Vector(0,0,1), c, 24, 56.0f, false, true, false);

			if (g_pRenderManager)
				g_pRenderManager->AddSystem(new CPSSparks(48, origin, 2.0f,1.0f,-0.5f, -20.0f/*radius*/, 2.0f, c.r,c.g,c.b,1.0f,-0.75f, g_iModelIndexMuzzleFlash3, kRenderTransAdd, 2.0f), RENDERSYSTEM_FLAG_LOOPFRAMES | RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_NOCLIP);
		}
	}
	else if (type == EV_ITEMSPAWN_WEAPON)
	{
		c.r = 255;
		c.g = 255;
		c.b = 127;
		EMIT_SOUND(entindex, origin, CHAN_WEAPON, "items/respawn.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);

		if (!toofar)
		{
			FX_StreakSplash(origin, Vector(0,0,1), c, 24, 48.0f, false, true, false);

			if (g_pRenderManager)
				g_pRenderManager->AddSystem(new CPSSparks(48, origin, 2.0f,1.0f,-0.5f, -16.0f/*radius*/, 2.0f, c.r,c.g,c.b,1.0f,-0.75f, g_iModelIndexMuzzleFlash3, kRenderTransAdd, 2.0f), RENDERSYSTEM_FLAG_LOOPFRAMES | RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_NOCLIP);
		}
	}
	else if (type == EV_ITEMSPAWN_AMMO)
	{
		c.r = 127;
		c.g = 255;
		c.b = 127;
		EMIT_SOUND(entindex, origin, CHAN_WEAPON, "items/respawn.wav", VOL_NORM, ATTN_NORM, 0, 150);

		FX_StreakSplash(origin, Vector(0,0,1), c, 24, 48.0f, false, true, false);
	}
	else// if (type == EV_ITEMSPAWN_OTHER)
	{
		c.r = 255;
		c.g = 255;
		c.b = 255;
		EMIT_SOUND(entindex, origin, CHAN_WEAPON, "items/respawn.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
		gEngfuncs.pEfxAPI->R_ParticleBurst(origin, 4, 208, 10);
	}

	if (!toofar && g_pRenderManager && modelindex)// shrinking model effect
	{
		float scaletime = 0.6f;
		float startscale = 2.5f;
		float scaledelta = (scale-startscale)/scaletime;// target scale
		CRSModel *pModelSystem = new CRSModel(origin, angles, g_vecZero, entindex, modelindex, body, skin, sequence, kRenderTransTexture, kRenderFxGlowShell, c.r,c.g,c.b, 0.1f, 1.5f, startscale, scaledelta, 1.0f, scaletime);
		g_pRenderManager->AddSystem(pModelSystem, RENDERSYSTEM_FLAG_LOOPFRAMES | RENDERSYSTEM_FLAG_SIMULTANEOUS);
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Item/weapon/ammo respawn effect (used to be an event, but that was unreliable)
//-----------------------------------------------------------------------------
int __MsgFunc_EnvParticle(const char *pszName, int iSize, void *pbuf)
{
	Vector origin, angles;
	BEGIN_READ(pbuf, iSize);
	short entindex			= READ_SHORT();
	unsigned short flags	= READ_SHORT();
	char *sz = READ_STRING();
	END_READ();
	if (entindex > 0 && sz)// TODO: TESTME
	{
		CPSAurora *pSystem = (CPSAurora *)g_pRenderManager->FindSystemByFollowEntity(entindex);// don't allow more than one system per entity
		if (pSystem && (strcmp(pSystem->GetName(), sz) == 0))// found system by entity and name
		{
			if (flags == RENDERSYSTEM_FLAG_NODRAW)// server wants to remove it
			{
				pSystem->dying = true;
				pSystem->m_fDieTime = gEngfuncs.GetClientTime();
			}
//			else do what? Do not allow duplicates!
		}
		else
		{
			g_pRenderManager->AddSystem(new CPSAurora(sz, 0), RENDERSYSTEM_FLAG_NOCLIP, entindex, RENDERSYSTEM_FFLAG_ICNF_NODRAW);
		}
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: smoke trail spawner callback function
//-----------------------------------------------------------------------------
void FX_SmokeTrailCallback(struct tempent_s *ent, float frametime, float currenttime)
{
	if (currenttime < ent->entity.baseline.animtime)
		return;

	// FIX: temp entity disappears when it hits the sky
	if (ent->entity.origin == ent->entity.attachment[0])
		ent->die = currenttime;
	else
	{
		switch (ent->entity.trivial_accept)
		{
			case FX_SMOKETRAIL_SMOKE:
				gEngfuncs.pEfxAPI->R_RocketTrail (ent->entity.prevstate.origin, ent->entity.origin, 1 );
			break;

			case FX_SMOKETRAIL_FIRESMOKE:
				gEngfuncs.pEfxAPI->R_RocketTrail (ent->entity.prevstate.origin, ent->entity.origin, 0 );
			break;

			case FX_SMOKETRAIL_BLOOD:
				gEngfuncs.pEfxAPI->R_RocketTrail (ent->entity.prevstate.origin, ent->entity.origin, 2 );
			break;

			case FX_SMOKETRAIL_BEAMFOLLOW:				
				g_pRenderManager->AddSystem(new CRSBeam(ent->entity.prevstate.origin, ent->entity.origin, ent->entity.baseline.iuser1, ent->entity.baseline.iuser2, kRenderTransAdd, 255,255,255, ent->entity.baseline.fuser1, ent->entity.baseline.fuser2, ent->entity.baseline.fuser3, ent->entity.baseline.fuser4, ent->entity.baseline.impacttime));
			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: particle smoke trail
//-----------------------------------------------------------------------------
//FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexBeamsAll, BLAST_SKIN_TAUBEAM, 1.0, -2.0, 30.0, 12.0, 0.5, 10.0);

TEMPENTITY *FX_SmokeTrail(const Vector &origin, unsigned short type, int entindex, int sprindex, int frame, float a, float adelta, float scale, float scaledelta, float beamlife, float traillife)
{
	TEMPENTITY *pTrailSpawner = NULL;
	pTrailSpawner = gEngfuncs.pEfxAPI->CL_TempEntAllocNoModel(origin);
	if (pTrailSpawner != NULL)
	{
		pTrailSpawner->flags |= (FTENT_PLYRATTACHMENT | FTENT_NOMODEL | FTENT_CLIENTCUSTOM);
		pTrailSpawner->callback = FX_SmokeTrailCallback;
		pTrailSpawner->clientIndex = entindex;
		pTrailSpawner->entity.trivial_accept = type;
		pTrailSpawner->entity.baseline.iuser1 = sprindex;
		pTrailSpawner->entity.baseline.iuser2 = frame;
		pTrailSpawner->entity.baseline.fuser1 = a;
		pTrailSpawner->entity.baseline.fuser2 = adelta;
		pTrailSpawner->entity.baseline.fuser3 = scale;
		pTrailSpawner->entity.baseline.fuser4 = scaledelta;
		pTrailSpawner->entity.baseline.impacttime = beamlife;
		pTrailSpawner->die = gEngfuncs.GetClientTime() + traillife;
		pTrailSpawner->entity.baseline.animtime = gEngfuncs.GetClientTime() + 0.1f;
	}
	return pTrailSpawner;
}


//======================
//Tempmodel
//======================
void FX_TempModel(const Vector &origin, float vel, bool rnd_dir, int modelindex, int body, int skin, int r_mode, int r_fx, byte r, byte g, byte b, float a, float scale, float life, int soundtype, char flags, int effect)
{
	vec3_t angles, dir;

    angles[0] = RANDOM_LONG (-100, 100);
    angles[1] = RANDOM_LONG (-100, 100);
    angles[2] = RANDOM_LONG (-100, 100);

	dir.x=RANDOM_FLOAT(-1,1);
	dir.y=RANDOM_FLOAT(-1,1);
	if (rnd_dir)
		dir.z=RANDOM_FLOAT(-1,1);
	else
		dir.z=RANDOM_FLOAT(0,1);
	dir=dir.Normalize();

	TEMPENTITY *TempModel = gEngfuncs.pEfxAPI->R_TempModel(origin, dir*vel, angles, life, modelindex, TE_BOUNCE_NULL);
	if (TempModel)
	{
		TempModel->entity.curstate.body = body;
		TempModel->entity.curstate.skin = skin;			
		TempModel->entity.curstate.scale = scale;

		TempModel->entity.curstate.rendermode = r_mode;
		TempModel->entity.curstate.renderfx = r_fx;
		TempModel->entity.curstate.renderamt = a;

		TempModel->entity.curstate.rendercolor.r = r;
		TempModel->entity.curstate.rendercolor.g = g;
		TempModel->entity.curstate.rendercolor.b = b;
		TempModel->entity.curstate.usehull = effect;
		TempModel->priority = TENTPRIORITY_LOW;

		if (!UTIL_PointIsFar(origin, 0.33))
			TempModel->hitSound = soundtype;

		TempModel->flags |= (flags);
	}
}

void FX_RemoveRS(short EntIndex)
{
	CRenderSystem *pSystem = NULL;
	while ((pSystem = g_pRenderManager->FindSystemByFollowEntity(EntIndex)) != NULL)
	{
		pSystem->m_iFollowEntity = -1;
		pSystem->m_fDieTime = gEngfuncs.GetClientTime();
	}
}

//===========================//
//Projectile trails & impacts//
//===========================//
int __MsgFunc_Trail(const char *pszName, int iSize, void *pbuf)
{
	vec3_t dir, origin, src, end;

	BEGIN_READ( pbuf, iSize );
	origin.x = READ_COORD();
	origin.y = READ_COORD();
	origin.z = READ_COORD();
	short EntIndex = READ_SHORT();
	byte FxType = READ_BYTE();
	BEAM *pBeamTrail;
	pmtrace_t tr;

	switch (FxType)
	{
		case FX_REMOVE:
		{
			FX_RemoveRS(EntIndex);
		}
		break;

		case FX_BLACKSMOKE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(8, EntIndex*0.5, origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, 5, EntIndex*0.5, 0,0,0, 0.6, -0.15, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 0.5), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
		}
		break;

		case FX_DEFAULT_EXP:
		{
			//EntIndex here is a scale of the effect
			g_pRenderManager->AddSystem(new CPSBlastCone(40, EntIndex, origin, origin, Vector(1,1,1), 10, EntIndex*0.35, 210,200,0, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(25, EntIndex, origin, origin, Vector(1,1,1), 10, EntIndex*0.35, 255,115,15, 0.6, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(35, EntIndex, origin, origin, Vector(1,1,1), 5, EntIndex*0.35, 185,110,25, 0.7, -1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			g_pRenderManager->AddSystem(new CPSBlastCone(8, EntIndex*0.2, origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, 5, EntIndex*0.5, 0,0,0, 0.6, -0.15, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, 0.5), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSSparks(25, origin, 0.8, 0.01, 0, EntIndex*1.5, 2, 255,255,255, 1, 0, g_iModelIndexFlame, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,5), RANDOM_FLOAT(1.8,2.4), EntIndex*2.5, SPARKSHOWER_SPARKS, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
			DynamicLight(origin, EntIndex*0.75, 255,116,0, 0.6, 200.0);	
		}
		break;

		case FX_M203:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(128, 15, origin, origin, Vector(0,0,0), 10, 20, 0,0,0, 0.4, -0.25, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(128, 10, origin, origin, Vector(0,0,0), 10, 24, 120,120,120, 0.5, -0.3, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_7, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_M203_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(40, 175, origin, origin, Vector(1,1,1), 10, 125, 210,200,0, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(25, 340, origin, origin, Vector(1,1,1), 10, 150, 255,115,15, 0.6, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(35, 320, origin, origin, Vector(1,1,1), 5, 90, 185,110,25, 0.7, -1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 70, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 60, 0,0,0, 0.4, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_8, kRenderTransAlpha, 0.7), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,6), RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(220, 280), SPARKSHOWER_SPARKS, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
				g_pRenderManager->AddSystem(new CPSSparks(20, origin, 0.8, 0.01, 0, 280, 2, 255,255,255, 1, 0, g_iModelIndexFlame, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				g_pRenderManager->AddSystem(new CPSSparks(20, origin, 0.7, 0.03, 0, 320, 2, 255,255,255, 1, 0, g_iModelIndexFire, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			}
			DynamicLight(origin, 350, 128,140,0, 0.6, 200.0);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_m203.wav", 1, ATTN_LOW, 0, PITCH_NORM );
		}
		break;

		case FX_M203_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(28, 155, origin, origin, Vector(1,1,1), 10, 125, 210,200,0, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(14, 300, origin, origin, Vector(1,1,1), 10, 150, 255,115,15, 0.6, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 280, origin, origin, Vector(1,1,1), 5, 90, 185,110,25, 0.7, -1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, 200, 128,140,0, 0.6, 200.0);
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_m203.wav", 1, ATTN_NORM, 0, PITCH_NORM );
				g_pRenderManager->AddSystem(new CPSSparks(12, origin, 0.7, 0.03, 0, 320, 2, 255,255,255, 1, 0, g_iModelIndexFire, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			}
		}
		break;

		case FX_30MMGREN:
		{
			FX_SmokeTrail(origin, FX_SMOKETRAIL_SMOKE, EntIndex, NULL, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 5.0);
		}
		break;

		case FX_30MMGREN_DETONATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(320,350), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 10, 150, 128,128,128, 1, -0.9, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(300,330), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 10, 120, 128,128,128, 1, -0.9, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{	
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(60,80), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, 5, 60, 0,0,0, 0.4, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_4, kRenderTransAlpha, 0.7), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(170,210), origin, Vector(0,0,1), VECTOR_CONE_LOTS, 30, 5, 128,128,128, 1, -1.1, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
			DynamicLight(origin, RANDOM_FLOAT(270,320), 220,140,0, 0.7, 200.0);
			
			switch (RANDOM_LONG(0,2))
			{
				case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_small1.wav", 1, ATTN_LOW_HIGH, 0, 100 );break;
				case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_small2.wav", 1, ATTN_LOW_HIGH, 0, 100 );break;
				case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_small3.wav", 1, ATTN_LOW_HIGH, 0, 100 );break;
			}
		}
		break;

		case FX_30MMGREN_BLAST:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(10, RANDOM_FLOAT(320,350), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 10, 150, 128,128,128, 1, -0.9, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(10, RANDOM_FLOAT(300,330), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 10, 120, 128,128,128, 1, -0.9, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_small1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_small2.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_small3.wav", 1, ATTN_NORM, 0, 100 );break;
				}	
				DynamicLight(origin, RANDOM_FLOAT(270,320), 220,140,0, 0.7, 200.0);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(150,180), origin, Vector(0,0,1), VECTOR_CONE_LOTS, 30, 5, 128,128,128, 1, -1.1, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case FX_METEOR:
		{
			FX_SmokeTrail(origin, FX_SMOKETRAIL_SMOKE, EntIndex, NULL, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 10.0);
		}
		break;

		case FX_METEOR_DETONATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 200, origin, Vector(0,0,1), Vector(1,1,1), 5, 125, 255,255,255, 0.4, -0.5, g_iModelIndexAnimSpr7, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(240,380), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, 10, 120, 128,128,128, 1, -RANDOM_FLOAT(0.8,1.2), g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 225, origin, Vector(0,0,1), Vector(1,1,1), 5, 250, 240,80,50, 0.5, -0.6, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				DynamicLight(origin, RANDOM_FLOAT(260,300), 128,128,0, 0.6, 200.0);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 180, origin, Vector(0,0,1), Vector(3,3,1), 5, 0, 255,255,255, 1.0, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 180, origin, Vector(0,0,1), Vector(3,3,1), 7, 0, 255,255,255, 1.0, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_23, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 275, origin, Vector(0,0,1), Vector(1,1,1), 5, 250, 128,128,128, 0.5, -0.6, g_iModelIndexMuzzleFlash2, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(48, RANDOM_FLOAT(320, 360), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, RANDOM_FLOAT(2.5,4.0), 0, 0,0,0, 1.0, -0.25, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_ASPHALT_0, PARTICLE_ASPHALT_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : gEngfuncs.pEfxAPI->R_ParticleExplosion( origin ); break;
					case 1 : gEngfuncs.pEfxAPI->R_ParticleExplosion2( origin, 111, 8 ); break;
					case 2 : gEngfuncs.pEfxAPI->R_RunParticleEffect(origin, Vector(1,1,1), 0, 128); break;
				}
				switch (RANDOM_LONG(0,3))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/meteor_hit1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/meteor_hit2.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/meteor_hit3.wav", 1, ATTN_NORM, 0, 100 );break;
					case 3 : EMIT_SOUND(EntIndex, origin, CHAN_STATIC, "weapons/meteor_hit4.wav", 1, ATTN_NORM, 0, 100 );break;
				}			
			
			}
		}
		break;

		case FX_METEOR_BLAST:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin, g_vecZero, g_iModelIndexExplosion0, kRenderTransAdd, 255,255,255, 1, -1.1, 2, 4, 25.0f, 0));

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 225, origin, Vector(0,0,1), Vector(1,1,1), 5, 250, 128,128,128, 0.5, -0.6, g_iModelIndexMuzzleFlash2, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(300,330), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 10, 120, 128,128,128, 1, -0.9, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				switch (RANDOM_LONG(0,3))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/meteor_hit1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/meteor_hit2.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/meteor_hit3.wav", 1, ATTN_NORM, 0, 100 );break;
					case 3 : EMIT_SOUND(EntIndex, origin, CHAN_STATIC, "weapons/meteor_hit4.wav", 1, ATTN_NORM, 0, 100 );break;
				}			
			}
		}
		break;

		case FX_METEOR_FIRE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(128, 10, origin, origin, VECTOR_CONE_20DEGREES, 25, -15, 255,255,255, 1, -2.5, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 250,155,0, 180, NULL, 0.0, 10.0, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_METEOR_FIRE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 200, origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, 125, 255,255,255, 0.6, -0.7, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(240,380), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, 10, 120, 128,128,128, 1, -RANDOM_FLOAT(0.8,1.2), g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 225, origin, Vector(0,0,1), Vector(1,1,1), 5, 250, 255,255,255, 0.5, -0.6, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRSCylinder(origin, RANDOM_FLOAT(25,40), RANDOM_FLOAT(550,750), RANDOM_FLOAT(30,60), 48, g_iModelIndexBeamsAll, BLAST_SKIN_C4, kRenderTransAdd, 255,255,255, 1, -1, 0.3));

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				DynamicLight(origin, RANDOM_FLOAT(260,300), 128,128,0, 0.6, 200.0);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 275, origin, Vector(0,0,1), Vector(1,1,1), 5, 250, 128,128,128, 0.5, -0.6, g_iModelIndexMuzzleFlash3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(48, RANDOM_FLOAT(320, 360), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, RANDOM_FLOAT(2.5,4.0), 0, 0,0,0, 1.0, -0.25, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_ASPHALT_0, PARTICLE_ASPHALT_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

				gEngfuncs.pEventAPI->EV_SetTraceHull(2);
				gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,96), PM_STUDIO_IGNORE, -1, &tr);

				if (tr.fraction != 1)
				{
					switch (RANDOM_LONG(0,2))
					{
						case 0: g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.75, 3, 5, PARTICLE_RED_2, 0, 0.0)); break;
						case 1: g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.75, 3, 5, PARTICLE_RED_8, 0, 0.0)); break;
						case 2: g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.75, 3, 5, PARTICLE_RED_22, 0, 0.0)); break;
					}
				}
				switch (RANDOM_LONG(0,1))
				{
					case 0 : gEngfuncs.pEfxAPI->R_ParticleExplosion( origin ); break;
					case 1 : gEngfuncs.pEfxAPI->R_ParticleExplosion2( origin, 111, 8 ); break;
				}
				switch (RANDOM_LONG(0,3))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/meteor_hit1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/meteor_hit2.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/meteor_hit3.wav", 1, ATTN_NORM, 0, 100 );break;
					case 3 : EMIT_SOUND(EntIndex, origin, CHAN_STATIC, "weapons/meteor_hit4.wav", 1, ATTN_NORM, 0, 100 );break;
				}			
			
			}
		}
		break;

		case FX_METEOR_FIRE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 200, origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, 125, 255,255,255, 0.6, -0.7, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 225, origin, Vector(0,0,1), Vector(1,1,1), 5, 250, 255,255,255, 0.5, -0.6, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 275, origin, Vector(0,0,1), Vector(1,1,1), 5, 250, 128,128,128, 0.5, -0.6, g_iModelIndexMuzzleFlash3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				switch (RANDOM_LONG(0,3))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/meteor_hit1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/meteor_hit2.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/meteor_hit3.wav", 1, ATTN_NORM, 0, 100 );break;
					case 3 : EMIT_SOUND(EntIndex, origin, CHAN_STATIC, "weapons/meteor_hit4.wav", 1, ATTN_NORM, 0, 100 );break;
				}			
			}
		}
		break;

		case FX_METEOR_FROZEN:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(128, 10, origin, origin, VECTOR_CONE_20DEGREES, 30, -20, 200,200,200, 0.5, -0.75, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_METEOR_FROZEN_DETONATE:
		{
			FX_RemoveRS(EntIndex);

			if (RANDOM_LONG(0,3))
				g_pRenderManager->AddSystem(new CRSCylinder(origin, RANDOM_FLOAT(25,40), RANDOM_FLOAT(500,700), RANDOM_FLOAT(20, 45), 48, g_iModelIndexBeamsAll, BLAST_SKIN_FROSTGRENADE, kRenderTransAdd, 255,255,255, 0.75, -0.75, 0.5));

			g_pRenderManager->AddSystem(new CPSBlastCone(20, 150, origin, origin, Vector(1,1,1), 5, 110, 0,90,250, 0.3, -0.2, g_iModelIndexAnimSpr6, TRUE, -1, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 100, origin, origin, Vector(1,1,1), 5, 160, 255,255,255, 0.5, -0.4, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_6, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,3))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/meteor_hit_frozen1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/meteor_hit_frozen2.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/meteor_hit_frozen3.wav", 1, ATTN_NORM, 0, 100 );break;
					case 3 : EMIT_SOUND(EntIndex, origin, CHAN_STATIC, "weapons/meteor_hit_frozen4.wav", 1, ATTN_NORM, 0, 100 );break;
				}			
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 125, origin, origin, Vector(1,1,1), 5, 135, 255,255,255, 0.4, -0.3, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(180, 300), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(180, 300), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.5, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_6, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSSparks(12, origin, RANDOM_FLOAT(0.4,0.6), RANDOM_FLOAT(0.01,0.03), RANDOM_FLOAT(0.1,0.3), RANDOM_FLOAT(300,430), RANDOM_FLOAT(1.8,2.5), 255, 255, 255, 1, -0.5, g_iModelIndexAnimSpr2, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				g_pRenderManager->AddSystem(new CPSSparks(12, origin, RANDOM_FLOAT(0.4,0.6), RANDOM_FLOAT(0.01,0.03), RANDOM_FLOAT(0.1,0.3), RANDOM_FLOAT(300,430), RANDOM_FLOAT(1.8,2.5), 255, 255, 255, 1, -0.5, g_iModelIndexAnimSpr5, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				g_pRenderManager->AddSystem(new CPSSparks(12, origin, RANDOM_FLOAT(0.4,0.6), RANDOM_FLOAT(0.01,0.03), RANDOM_FLOAT(0.1,0.3), RANDOM_FLOAT(300,430), RANDOM_FLOAT(1.8,2.5), 255, 255, 255, 1, -0.5, g_iModelIndexAnimSpr6, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			}
		}
		break;

		case FX_METEOR_FROZEN_BLAST:
		{
			FX_RemoveRS(EntIndex);

			g_pRenderManager->AddSystem(new CPSBlastCone(12, 150, origin, origin, Vector(1,1,1), 5, 110, 0,90,250, 0.3, -0.2, g_iModelIndexAnimSpr6, TRUE, -1, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(8, 100, origin, origin, Vector(1,1,1), 5, 160, 255,255,255, 0.5, -0.4, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_6, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,3))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/meteor_hit_frozen1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/meteor_hit_frozen2.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/meteor_hit_frozen3.wav", 1, ATTN_NORM, 0, 100 );break;
					case 3 : EMIT_SOUND(EntIndex, origin, CHAN_STATIC, "weapons/meteor_hit_frozen4.wav", 1, ATTN_NORM, 0, 100 );break;
				}			
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 125, origin, origin, Vector(1,1,1), 5, 135, 255,255,255, 0.4, -0.3, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(180, 300), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.5, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_6, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
		}
		break;

		case FX_STR_TARGET_MORTAR:
		{
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_MORTAR_TARGET, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -1.0, 0.1, 5.0, 0, 1.0), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
		}
		break;

		case FX_MORTAR_DETONATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(28, 150, origin, Vector(0,0,1), Vector(3,3,1), 5, 0, 255,255,255, 1.0, -0.7, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(28, 150, origin, Vector(0,0,1), Vector(3,3,1), 7, 0, 255,255,255, 1.0, -0.7, g_iModelIndexPartRed, FALSE, PARTICLE_RED_23, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 200, origin, Vector(0,0,1), Vector(1,1,1), 5, 125, 255,255,255, 0.4, -0.5, g_iModelIndexAnimSpr7, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 325, origin, Vector(0,0,1), Vector(1,1,1), 5, 250, 240,80,50, 0.5, -0.6, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_EXPLOSION, BLAST_SKIN_FIRE, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.75, -1.5, RANDOM_FLOAT(0.03,0.05), RANDOM_FLOAT(3,5), 0, 0.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 325, origin, Vector(0,0,1), Vector(1,1,1), 5, 250, 128,128,128, 0.5, -0.6, g_iModelIndexMuzzleFlash2, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 310, origin, Vector(0,0,1), Vector(1,1,1), 10, 220, 0,0,0, 0.4, -0.5, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_3, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				gEngfuncs.pEfxAPI->R_ParticleExplosion( origin );
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,5), RANDOM_FLOAT(1.5,2.2), RANDOM_FLOAT(350, 420), SPARKSHOWER_EXP, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(10, 80, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 10, 50, 0,0,0, 0.2, -0.05, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 1), 0, -1);
				g_pRenderManager->AddSystem(new CRenderSystem(origin - Vector(0,0,38), Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.75, 5, 9, PARTICLE_RED_2, 0, 0.0));
				gEngfuncs.pEfxAPI->R_TempSprite(origin, (float *)&g_vecZero, 1.2f, g_iModelIndexAnimSpr8, kRenderGlow, kRenderFxNoDissipation, 1.0f, 0.2f, FTENT_FADEOUT|FTENT_SPRANIMATELOOP|FTENT_SPRANIMATE);
			}
			switch (RANDOM_LONG(0,2))
			{
				case 0 : EMIT_SOUND(EntIndex, origin, CHAN_AUTO, "weapons/airstrike_mortarhit.wav", 1, ATTN_LOW, 0, 100 );break;
				case 1 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode4.wav", 1, ATTN_LOW, 0, 100 );break;
				case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/airstrike_mortarhit.wav", 1, ATTN_LOW, 0, 100 );break;
			}
			DynamicLight(origin, RANDOM_FLOAT(300,330), 128,128,0, 0.6, 200.0);
		}
		break;

		case FX_PLAYER_FREEZE_EFFECT:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(40,60), origin, Vector(0,0,-1), VECTOR_CONE_40DEGREES, 25, RANDOM_FLOAT(80,100), 128,128,128, 1.0, -2.0, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 2.0), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_PLAYER_FROZENCUBE_BLAST:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(300, 350), origin, Vector(1,1,1), Vector(1,1,1), RANDOM_FLOAT(1.2,1.8), 0, 250,250,250, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_GLASS_0, PARTICLE_GLASS_5), kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_FROSTSCORCH1,DECAL_FROSTSCORCH3), &tr);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				for (int i = 0; i < 5; i++)	
					FX_TempModel(origin, RANDOM_FLOAT(175, 300), true, g_iModelIndexGibModel, RANDOM_LONG(GIB_CONCRETE_01, GIB_CONCRETE_08), SKIN_GIB_ICE, kRenderTransAdd, kRenderFxNone, 255,255,255, 64, RANDOM_FLOAT(2.0,3.2), TEMP_ENT_LIFE, 0, 0, 0);

				for (int j = 0; j < 5; j++)	
					FX_TempModel(origin, RANDOM_FLOAT(150, 250), true, g_iModelIndexGibModel, RANDOM_LONG(GIB_GLASS_01, GIB_GLASS_06), SKIN_GIB_ICE, kRenderTransAdd, kRenderFxNone, 255,255,255, 64, RANDOM_FLOAT(1.0,2.0), TEMP_ENT_LIFE, 0, 0, 0);
			}
		}
		break;

		case FX_PLAYER_BURN_EFFECT:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(110,140), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, 25, RANDOM_FLOAT(80,100), 128,128,128, 1.0, -2.0, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, RANDOM_FLOAT(1.5, 2.2)), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				FX_SmokeTrail(origin, FX_SMOKETRAIL_FIRESMOKE, EntIndex, NULL, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.5);
				g_pRenderManager->AddSystem(new CRSLight(origin, 250,155,0, 180, NULL, 0.0, 2.0, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			}
		}
		break;

		case FX_ITEM_BLAST:
		{
			src = origin+Vector(0,0,16);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 160, src, src, Vector(1,1,1), 10, 125, 210,200,0, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 340, src, src, Vector(1,1,1), 10, 150, 255,115,15, 0.6, -1, g_iModelIndexAnimSpr2, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 320, src, src, Vector(1,1,1), 5, 90, 185,110,25, 0.7, -1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_SMALLSCORCH1,DECAL_SMALLSCORCH3), &tr);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_small1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_small2.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_small3.wav", 1, ATTN_NORM, 0, 100 );break;
				}
				DynamicLight(origin, 300, 0,178,128, 0.6, 200.0);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 195, src, src, Vector(1,1,1), 10, 125, 128,128,128, 0.8, -1.0, g_iModelIndexExplosion4, FALSE, RANDOM_LONG(0,4), kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 80, src, Vector(0,0,1), VECTOR_CONE_20DEGREES, 5, 60, 185,110,25, 0.5, -0.12, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, 0.5), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,5), RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(220, 280), SPARKSHOWER_SPARKS2, origin, Vector(0,0,1), VECTOR_CONE_LOTS, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
				g_pRenderManager->AddSystem(new CPSSparks(32, src, 0.8, 0.01, 0, 280, 2, 255,255,255, 1, 0, g_iModelIndexExplosion4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			}
		}
		break;

		case FX_PLAYER_SHIELD_BLAST:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin+Vector(0,0,12), g_vecZero, g_iModelIndexExplosion0, kRenderTransAdd, 255,255,255, 1, -0.5, 1, 0.5, 24.0f, 0));

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/fire_disruptor.wav", 1, ATTN_NORM, 0, PITCH_HIGH);
				DynamicLight(origin, 300, 0,178,128, 0.6, 200.0);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 80, origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, 5, 60, 185,110,25, 0.5, -0.12, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, 0.5), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,5), RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(220, 280), SPARKSHOWER_SPARKS2, origin, Vector(0,0,1), VECTOR_CONE_LOTS, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
				g_pRenderManager->AddSystem(new CPSSparks(32, origin, 0.8, 0.01, 0, 280, 2, 255,255,255, 1, 0, g_iModelIndexExplosion4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			}
		}
		break;

		case FX_PLAYER_GENERATOR_BLAST:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin, g_vecZero, g_iModelIndexExplosion3, kRenderTransAdd, 255,255,255, 1, -0.5, 1, 0.5, 24.0f, 0.0));
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexLightningFieldModel, LGTNTG_FIELD_BALL, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -2.0, 0.01, 12, 0, 0.25), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				DynamicLight(origin, 280, 128,140,0, 0.6, 100.0);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_small1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_small2.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_small3.wav", 1, ATTN_NORM, 0, 100 );break;
				}
				
				g_pRenderManager->AddSystem(new CPSBlastCone(48, 80, origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, 20, 20, 128,128,128, 1.0, -0.7, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 1.5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
				g_pRenderManager->AddSystem(new CPSBlastCone(10, 30, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 25, 0,0,0, 0.15, -0.04, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				// gibs
				for (int j = 0; j < 6; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(180, 300), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, BOUNCE_METAL, 0, 0);

				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(1,3), RANDOM_FLOAT(1.4,2.0), RANDOM_FLOAT(200, 250), SPARKSHOWER_SPARKS, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
				g_pRenderManager->AddSystem(new CPSSparks(48, origin, 0.4, 0.03, 0, 100, 1.5, 255,255,255, 1, 0, g_iModelIndexPartViolet, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_PLAYER_WEAPON_BLAST:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 250, origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, 120, 150,100,205, 0.4, -0.3, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 200, origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, 75, 160,0,217, 0.3, -0.4, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_4, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				// gibs
				for (int j = 0; j < 6; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(120, 250), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, BOUNCE_METAL, 0, 0);

				g_pRenderManager->AddSystem(new CPSBlastCone(24, 200, origin, origin, Vector(1,1,1), 5, 125, 255,255,255, 0.3, -0.4, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_2, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(48, 180, origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, 3, 0, 255,255,255, 0.8, -0.8, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_3, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				DynamicLight(origin, 200, 160,0,220, 0.5, 80.0);
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode1.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode4.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode2.wav", 1, ATTN_NORM, 0, 100 );break;
				}
			}
		}
		break;

		case FX_PLAYER_GIB_NORMAL:
		{
			FX_RemoveRS(EntIndex);
			DynamicLight(origin, RANDOM_LONG(310,350), 128,140,0, 0.6, 200.0);

			switch (RANDOM_LONG(0,3))
			{
				case 0:
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 330, origin, origin, Vector(1,1,1), 25, 100, 255,255,255, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_17, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 300, origin, origin, Vector(1,1,1), 25, 100, 128,128,128, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_16, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 270, origin, origin, Vector(1,1,1), 15, 70, 255,255,255, 0.6, -0.7, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_4, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 900, 60, 60, g_iModelIndexBeamsAll, BLAST_SKIN_C4, kRenderTransAdd, 255,255,255, 1, -1, 0.3));
				break;

				case 1:
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 330, origin, origin, Vector(1,1,1), 25, 100, 255,255,255, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_19, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 300, origin, origin, Vector(1,1,1), 25, 100, 128,128,128, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_40, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 270, origin, origin, Vector(1,1,1), 15, 70, 255,255,255, 0.6, -0.7, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_0, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 900, 60, 60, g_iModelIndexBeamsAll, BLAST_SKIN_FIREBEAM, kRenderTransAdd, 255,255,255, 1, -1, 0.3));
				break;

				case 2:
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 330, origin, origin, Vector(1,1,1), 25, 100, 255,255,255, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_35, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 300, origin, origin, Vector(1,1,1), 25, 100, 128,128,128, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_33, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 270, origin, origin, Vector(1,1,1), 15, 70, 255,255,255, 0.6, -0.7, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_2, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 900, 60, 60, g_iModelIndexBeamsAll, BLAST_SKIN_WASTEDBEAM, kRenderTransAdd, 255,255,127, 0.8, -0.7, 0.3));
				break;

				case 3:
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 330, origin, origin, Vector(1,1,1), 25, 100, 255,255,255, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_23, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 300, origin, origin, Vector(1,1,1), 25, 100, 128,128,128, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_31, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CPSBlastCone(30, 270, origin, origin, Vector(1,1,1), 15, 70, 255,255,255, 0.6, -0.7, g_iModelIndexPartRed, FALSE, PARTICLE_RED_30, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 900, 60, 60, g_iModelIndexBeamsAll, BLAST_SKIN_ENERGYBOLT, kRenderTransAdd, 255,255,255, 1, -1, 0.3));
				break;
			}
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_SCORCH1,DECAL_SCORCH3), &tr);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,1))
				{		
					case 0:
					{
						FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_HEAD, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
						FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_CHEST, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
						FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG01, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
						FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG03, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);
						FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM01, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
						FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM03, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);
						
						for (int i = 0; i < 7; i++)		
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_OTHER01, PLR_GIB_OTHER07), SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);

						for (int j = 0; j < 7; j++)		
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);
					}
					break;

					case 1:
						{
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_HEAD, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_CHEST, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG01, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG02, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG03, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG04, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM01, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM02, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM03, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
							FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM04, SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);

							for (int i = 0; i < 3; i++)		
								FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_OTHER01, PLR_GIB_OTHER07), SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);

							for (int j = 0; j < 3; j++)		
								FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);
						}
						break;			
				}
				g_pRenderManager->AddSystem(new CPSBlastCone(10, 35, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 20, 40, 0,0,0, 0.3, -0.12, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_4, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode3.wav", 1, ATTN_NORM, 0, 100 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode4.wav", 1, ATTN_NORM, 0, 100 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "weapons/explode5.wav", 1, ATTN_NORM, 0, 100 );break;
				}

				switch (RANDOM_LONG(0,2))
				{
					case 0 : g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,5), RANDOM_FLOAT(2.0,2.8), RANDOM_FLOAT(350, 450), SPARKSHOWER_LAVA_FLAME, origin, origin, Vector(2,2,3), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1); break;
					case 1 : g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,5), RANDOM_FLOAT(2.0,2.8), RANDOM_FLOAT(350, 450), SPARKSHOWER_FLICKER, origin, origin, Vector(2,2,3), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1); break;
					case 2 : g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,5), RANDOM_FLOAT(2.0,2.8), RANDOM_FLOAT(350, 450), SPARKSHOWER_ENERGY, origin, origin, Vector(2,2,3), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1); break;
				}
			}
		}
		break;

		case FX_PLAYER_GIB_FROZEN_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			DynamicLight(origin, RANDOM_LONG(200,240), 0,0,200, 0.6, 200.0);

			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_EXPLOSION_01, BLAST_SKIN_SPHERE_SHIELD, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -1.8, RANDOM_FLOAT(0.03,0.05), RANDOM_FLOAT(1.5,1.7), 0, 0.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 230, origin, origin, Vector(1,1,1), 25, 100, 0,90,255, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_17, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 200, origin, origin, Vector(1,1,1), 25, 100, 0,90,255, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_16, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 170, origin, origin, Vector(1,1,1), 15, 70, 0,90,255, 0.6, -0.7, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_4, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_FROSTSCORCH1,DECAL_FROSTSCORCH3), &tr);
			
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				// gibs
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_HEAD, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_WHITESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_CHEST, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLUESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG01, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG02, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLUESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG03, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG04, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_WHITESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM01, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM02, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLUESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM03, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM04, SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_WHITESMOKE);

				for (int i = 0; i < 3; i++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_OTHER01, PLR_GIB_OTHER07), SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLUESMOKE);

				for (int j = 0; j < 3; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_FROZEN, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);

				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(240, 300), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(240, 300), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_6, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(4, 50, origin, origin, Vector(1,1,1), 5, 85, 255,255,255, 0.5, -0.4, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_6, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "debris/bustglass1.wav", 1, ATTN_NORM, 0, 90 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "debris/bustglass2.wav", 1, ATTN_NORM, 0, 90 );break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "debris/glass3.wav", 1, ATTN_NORM, 0, 90 );break;
				}
			}
		}
		break;

		case FX_PLAYER_GIB_SONIC:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(96, 300, origin, origin, Vector(1,1,1), 1, 0, 255,255,255, 1.0, -1.2, g_iModelIndexPartRed, FALSE, PARTICLE_RED_3, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			gEngfuncs.pEfxAPI->R_Implosion(origin, 256, 128, 0.25);

			if (!UTIL_PointIsFar(origin, 0.5))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(64, RANDOM_FLOAT(320, 360), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, RANDOM_FLOAT(2.5,4.0), 0, 0,0,0, 1.0, -0.25, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_ASPHALT_0, PARTICLE_ASPHALT_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				gEngfuncs.pEfxAPI->R_RunParticleEffect(origin, Vector(1,1,1), 0, 128);
			}

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				// gibs
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_HEAD_TYPE2, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_CHEST_TYPE2, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLACKSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_LEG01_TYPE2, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_LEG02_TYPE2, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_LEG03_TYPE2, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_LEG04_TYPE2, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLACKSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_ARM01_TYPE2, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_ARM02_TYPE2, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_ARM03_TYPE2, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, PLR_GIB_PELVIS, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLACKSMOKE);

				for (int j = 0; j < 5; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), true, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);

				gEngfuncs.pEfxAPI->R_BlobExplosion(origin);
				gEngfuncs.pEfxAPI->R_TeleportSplash(origin);
				EMIT_SOUND(EntIndex, origin, CHAN_STATIC, "weapons/explode_blackhole.wav", 1, ATTN_NORM, 0, 90 );
			}
		}
		break;

		case FX_PLAYER_GIB_ACID_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSSprite(origin+Vector(0,0,20), g_vecZero, g_iModelIndexExplosion1, kRenderTransAdd, 255,255,255, 1.0f, -0.1f, 2.0, 0.0f, 16.0f, 1.0),0, -1, 0);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_ACID_BIGSPLAT1,DECAL_ACID_BIGSPLAT3), &tr);
			
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 240, origin, origin, Vector(1,1,1), 10, 90, 0,255,0, 0.6, -0.8, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 200, origin, origin, Vector(1,1,1), 10, 70, 0,200,0, 0.7, -0.9, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				switch (RANDOM_LONG(0,1))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "debris/bustflesh1.wav", 1, ATTN_NORM, 0, 90 );break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "debris/bustflesh2.wav", 1, ATTN_NORM, 0, 90 );break;
				}
				
				// Acid gibs
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_HEAD, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_GREENSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_CHEST, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_GREENSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG01, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG02, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_GREENSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG03, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG04, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_GREENSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM01, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM02, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_GREENSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM03, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM04, SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_GREENSMOKE);

				for (int i = 0; i < 5; i++)		
					FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_OTHER01, PLR_GIB_OTHER07), SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);

				for (int j = 0; j < 5; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxGlowShell, 0,180,0, 15, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);
			}
		}
		break;

		case FX_PLAYER_GIB_VAPOURIZE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_LONG(40,50), origin-Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_20DEGREES, 25, 25, 128,128,128, 0.4, -0.1, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_0, kRenderTransAdd, 1.2), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_LONG(32,44), origin+Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_20DEGREES, 20, 20, 0,0,0, 0.3, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 1.2), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,5), RANDOM_FLOAT(1.8,2.3), RANDOM_FLOAT(300, 350), SPARKSHOWER_SMOKE, origin, Vector(0,0,1), VECTOR_CONE_LOTS, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/biohazardmissile_smoke.wav", 1, ATTN_NORM, 0, 105);
			}
		}
		break;

		case FX_PLAYER_GIB_ENERGYBLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 170, origin, origin, Vector(1,1,1), 10, 125, 210,200,0, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_3, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 210, origin, origin, Vector(1,1,1), 10, 100, 255,255,255, 0.8, -1, g_iModelIndexExplosion4, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 290, origin, origin, Vector(1,1,1), 5, 90, 0,178,178, 0.7, -1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_1, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_GLUONSCORCH1,DECAL_GLUONSCORCH3), &tr);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				DynamicLight(origin, RANDOM_FLOAT(320,350), 0,178,178, 0.6, 200.0);
				EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_scorcher.wav", 1, ATTN_NORM, 0, 110);

				// gibs
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_HEAD, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLACKSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_CHEST, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLACKSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG01, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG02, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG03, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG04, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLACKSMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM01, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM02, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM03, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM04, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_BLACKSMOKE);

				for (int i = 0; i < 3; i++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_OTHER01, PLR_GIB_OTHER07), SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE);

				for (int j = 0; j < 3; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);
			}

		}
		break;

		case FX_PLAYER_GIB_ENERGYBEAM:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SHOCKWAVE, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -1.5, 0.1, 2.5, 0, 1.0), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			TEMPENTITY *pSpr = gEngfuncs.pEfxAPI->R_TempSprite(origin, (float *)&g_vecZero, 0.8f, g_iModelIndexPartBlue, kRenderGlow, kRenderFxNoDissipation, 1.0, 0.5, FTENT_FADEOUT);
			if (pSpr)
				pSpr->entity.baseline.frame = pSpr->entity.curstate.frame = PARTICLE_BLUE_2;

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_BLASTSCORCH1,DECAL_BLASTSCORCH3), &tr);

			if (!UTIL_PointIsFar(origin, 0.5))
				g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(320, 360), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, RANDOM_FLOAT(2.5,4.0), 0, 150,150,150, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_ASPHALT_0, PARTICLE_ASPHALT_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				// gibs
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_HEAD_TYPE2, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE2);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_CHEST_TYPE2, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_WHITESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG01_TYPE2, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG02_TYPE2, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE2);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG03_TYPE2, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG04_TYPE2, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_WHITESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM01_TYPE2, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM02_TYPE2, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE2);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM03_TYPE2, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_PELVIS, SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_WHITESMOKE);

				for (int i = 0; i < 4; i++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_OTHER01, PLR_GIB_OTHER07), SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);

				for (int j = 0; j < 3; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_MELTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);

				DynamicLight(origin, RANDOM_FLOAT(330,370), 0,180,224, 0.6, 200.0);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,3), RANDOM_FLOAT(1.8,2.3), RANDOM_FLOAT(300,400), SPARKSHOWER_BLUEENERGY, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

				switch (RANDOM_LONG(0,2))
				{
					case 0:	EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_blackhole.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					case 1:	EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_ghostmissile.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					case 2:	EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_gluon.wav", 1.0, ATTN_NORM, 0, 120 ); break;
				}		
			}
		}
		break;

		case FX_PLAYER_GIB_MELT:
		{
			FX_RemoveRS(EntIndex);
			switch (RANDOM_LONG(0,1))
			{
				case 0 : gEngfuncs.pEfxAPI->R_TempSprite(origin, (float *)&g_vecZero, 3.0f, g_iModelIndexAnimSpr10, kRenderGlow, kRenderFxNoDissipation, 1.0f, 0.5f, FTENT_FADEOUT|FTENT_SPRANIMATELOOP|FTENT_SPRANIMATE); break;
				case 1 : gEngfuncs.pEfxAPI->R_TempSprite(origin, (float *)&g_vecZero, 3.0f, g_iModelIndexAnimSpr8, kRenderGlow, kRenderFxNoDissipation, 1.0f, 0.5f, FTENT_FADEOUT|FTENT_SPRANIMATELOOP|FTENT_SPRANIMATE); break;
			}

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);
		
			if (tr.fraction != 1)
			{
				g_pRenderManager->AddSystem(new CRSSprite(origin+Vector(0,0,32), g_vecZero, g_iModelIndexFlame, kRenderTransAdd, 255,255,255, 1.0f,-0.2f, 1.2, -0.1f, 24.0f, 5.0),0, -1, RENDERSYSTEM_FLAG_ZROTATION);
				DecalTrace(RANDOM_LONG(DECAL_MDSCORCH1,DECAL_MDSCORCH3), &tr);

				g_pRenderManager->AddSystem(new CRSModel(tr.endpos, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, RANDOM_LONG(BLAST_MDL_MELTED_PLAYER, BLAST_MDL_MELTED_PLAYER_03), RANDOM_LONG(BLAST_SKIN_FIRE, BLAST_SKIN_SPHERE_SUNOFGOD), 0, kRenderNormal, kRenderFxFullBright, 255,255,255, 1.0, -0.2, 1.5, -0.1, 0, TEMP_ENT_LIFE), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.2, 2, -0.2, PARTICLE_RED_8, 0, 0.0)); break;
					case 1 : g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.2, 2, -0.2, PARTICLE_RED_7, 0, 0.0)); break;
					case 2 : g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.2, 2, -0.2, PARTICLE_RED_33, 0, 0.0)); break;
				}		
			}
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(250, 320), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, RANDOM_FLOAT(1,3), 0, 255,255,255, 1.0, -0.2, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(280, 340), origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.2, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_uw.wav", 1, ATTN_NORM, 0, 95);
				DynamicLight(origin, RANDOM_FLOAT(280,330), 255,72,0, TEMP_ENT_LIFE, 20.0);
			}
		}
		break;

		case FX_PLAYER_GIB_RADIATION:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(160,180), origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, 25, RANDOM_FLOAT(80,100), 200,200,200, 0.5, -1.0, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, RANDOM_FLOAT(2.0, 2.5)), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);
		
			if (tr.fraction != 1)
			{
				g_pRenderManager->AddSystem(new CRSSprite(origin+Vector(0,0,32), g_vecZero, g_iModelIndexFlame, kRenderTransAdd, 255,255,255, 1.0f,-0.2f, 1.2, -0.1f, 24.0f, 5.0),0, -1, RENDERSYSTEM_FLAG_ZROTATION);
				DecalTrace(RANDOM_LONG(DECAL_MDSCORCH1,DECAL_MDSCORCH3), &tr);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.2, 2, -0.2, PARTICLE_RED_8, 0, 0.0)); break;
					case 1 : g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.2, 2, -0.2, PARTICLE_RED_7, 0, 0.0)); break;
					case 2 : g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.2, 2, -0.2, PARTICLE_RED_33, 0, 0.0)); break;
				}		
			}
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(64, RANDOM_FLOAT(180,210), origin, Vector(0,0,1), VECTOR_CONE_25DEGREES, 25, RANDOM_FLOAT(50,70), 0,0,0, 0.5, -0.6, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, RANDOM_FLOAT(2.5, 3.0)), RENDERSYSTEM_FLAG_CLIPREMOVE, EntIndex, 0);
				g_pRenderManager->AddSystem(new CPSBlastCone(96, RANDOM_FLOAT(240, 280), origin, Vector(0,0,1), VECTOR_CONE_30DEGREES, RANDOM_FLOAT(2,3), 0, 200,200,200, 1.0, -0.25, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, RANDOM_FLOAT(1.9, 2.2)), RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, EntIndex);
				DynamicLight(origin, RANDOM_FLOAT(280,330), 255,72,0, TEMP_ENT_LIFE, 20.0);
			}
		}
		break;

		case FX_PLAYER_GIB_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSSprite(origin, g_vecZero, g_iModelIndexAnimSpr9, kRenderTransAdd, 255,255,255, 1.0f,-0.5f, 1.5, -0.5f, 12.0f, 0.3),0, -1, RENDERSYSTEM_FLAG_LOOPFRAMES);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);
			gEngfuncs.pEfxAPI->R_ParticleExplosion(origin);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_SCORCH1,DECAL_SCORCH3), &tr);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				// gibs
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_HEAD, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_CHEST, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG01, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG02, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE2);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG03, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG04, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM01, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM02, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM03, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM04, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);

				for (int i = 0; i < 5; i++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_OTHER01, PLR_GIB_OTHER07), SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE2);

				for (int j = 0; j < 5; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);

				g_pRenderManager->AddSystem(new CPSBlastCone(3, 35, origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, 20, 30, 0,0,0, 0.3, -0.12, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				DynamicLight(origin, RANDOM_FLOAT(280,330), 255,192,0, 0.6, 200.0);
				gEngfuncs.pEfxAPI->R_ParticleExplosion2( origin, 111, 8 );

				switch (RANDOM_LONG(0,2))
				{
					case 0:	EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_bomb.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					case 1:	EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_clustergun.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					case 2:	EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_minimissile.wav", 1.0, ATTN_NORM, 0, 120 ); break;
				}
			}
		}
		break;

		case FX_PLAYER_GIB_BURN:
		{
			FX_RemoveRS(EntIndex);
			gEngfuncs.pEfxAPI->R_TempSprite(origin, (float *)&g_vecZero, 3.0f, g_iModelIndexExplosion3, kRenderGlow, kRenderFxNoDissipation, 0.7f, 1.0f, FTENT_SPRANIMATE);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_FIRE, RANDOM_LONG(BLAST_SKIN_FIRE, BLAST_SKIN_SPHERE_SUNOFGOD), 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -1.5, RANDOM_FLOAT(0.03,0.05), RANDOM_FLOAT(1.0,1.3), 0, 0.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_MDSCORCH1,DECAL_MDSCORCH3), &tr);

			if (!UTIL_PointIsFar(origin, 0.5))
				g_pRenderManager->AddSystem(new CPSBlastCone(64, RANDOM_FLOAT(320, 360), origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, RANDOM_FLOAT(2.5,4.0), 0, 0,0,0, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_ASPHALT_0, PARTICLE_ASPHALT_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				// gibs
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_HEAD, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_CHEST, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG01, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG02, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE2);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG03, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_LEG04, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM01, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM02, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM03, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, 0, 0, 0);
				FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, PLR_GIB_ARM04, SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.0, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);

				for (int i = 0; i < 3; i++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_OTHER01, PLR_GIB_OTHER07), SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, FTENT_SMOKETRAIL, TENT_TRAIL_RS_FIRE2);

				for (int j = 0; j < 3; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(250, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_WASTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);

				DynamicLight(origin, RANDOM_FLOAT(280,330), 255,128,0, 0.6, 200.0);				
				g_pRenderManager->AddSystem(new CPSBlastCone(6, 35, origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, 20, 30, 0,0,0, 0.3, -0.12, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_plasma.wav", 1.0, ATTN_NORM, 0, 120 );
			}
		}
		break;

		case FX_STARBURST_PART1:
		{
			CPSDrips *pRS = new CPSDrips(UTIL_PointIsFar(origin, 1.0)?128:256, origin+Vector(-192,-192,0), Vector(32,32,32), Vector(384,384,384), Vector(0,0,-3000), g_iModelIndexAnimSpr7, NULL, g_iModelIndexAnimSpr6, kRenderTransAdd, 0.2, 10, 0.0f, 256, 0.1);
			if (pRS)
			{
				pRS->m_color.r = 128;
				pRS->m_color.g = 128;
				pRS->m_color.b = 128;
				pRS->m_fBrightness = 200;
				g_pRenderManager->AddSystem(pRS, RENDERSYSTEM_FLAG_ADDPHYSICS, EntIndex);
			}
			EMIT_SOUND(EntIndex, origin, CHAN_STATIC, "weapons/explode_starburst.wav", 1, ATTN_LOW_HIGH, 0, 100 );
		}
		break;

		case FX_STARBURST_PART2:
		{
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
			{
				DecalTrace(DECAL_BLOW, &tr);
				g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexAnimSpr7, kRenderTransAdd, 128,128,128, 1.0, -0.5, 10, -5, 0, 0, 0.0));
			}
			
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 600, 60, 40, g_iModelIndexBeamsAll, BLAST_SKIN_TELEENTER, kRenderTransAdd, 255,255,255, 0.5, -0.8, 0.5));
			DynamicLight(origin, RANDOM_FLOAT(440,500), 0,90,250, 0.7, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 450, 40, 40, g_iModelIndexBeamsAll, BLAST_SKIN_TELEENTER, kRenderTransAdd, 255,255,255, 0.5, -0.8, 0.5));
				g_pRenderManager->AddSystem(new CPSBlastCone(96, 100, origin, origin, VECTOR_CONE_LOTS, 10, 75, 255,255,255, 1, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_gluon.wav", 1, ATTN_LOW_HIGH, 0, PITCH_NORM );
			}
		}
		break;

		case FX_STARBURST_DETONATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(48, 170, origin, origin, Vector(1,1,1), 10, 60, 170,90,250, 1, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(300,350), 0,90,250, 0.7, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,4), RANDOM_FLOAT(1.5,2.0), RANDOM_FLOAT(350,400), SPARKSHOWER_BLUEENERGY, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(48, 250, origin, origin, Vector(1,1,1), 10, 75, 255,255,255, 1, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_gluon.wav", 1, ATTN_NORM, 0, PITCH_NORM );
			}

		}
		break;

		case FX_BLOOD_RED:
		{
  			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), 24, origin, origin, Vector(1,1,1), 5, 25, 90,0,0, 0.5, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_6, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(90, 120), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(1.2,1.6), 0, 90,0,0, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_CONCRETE_0,PARTICLE_CONCRETE_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
		}
		break;

		case FX_SHIELDEFFECT:
		{
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SPHERE, BLAST_SKIN_SPHERE_SHIELD, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.5, 0.0, 0.5, 0, 0, 0.1), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0:	EMIT_SOUND(EntIndex, origin, CHAN_BODY, "player/pl_shield_impact1.wav", RANDOM_FLOAT(0.5,1.0), ATTN_NORM, 0, PITCH_NORM ); break;
					case 1:	EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "player/pl_shield_impact2.wav", RANDOM_FLOAT(0.5,1.0), ATTN_NORM, 0, PITCH_NORM ); break;
					case 2:	EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "player/pl_shield_impact3.wav", RANDOM_FLOAT(0.5,1.0), ATTN_NORM, 0, PITCH_NORM ); break;
				}
			}
		}
		break;

		case FX_SHIELDEFFECT_INVUL:
		{
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SPHERE, BLAST_SKIN_SPHERE_SIELD_INV, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.5, 0.0, 0.5, 0, 0, 0.1), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0:	EMIT_SOUND(EntIndex, origin, CHAN_BODY, "player/pl_shield_impact1.wav", RANDOM_FLOAT(0.5,1.0), ATTN_NORM, 0, PITCH_NORM ); break;
					case 1:	EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "player/pl_shield_impact2.wav", RANDOM_FLOAT(0.5,1.0), ATTN_NORM, 0, PITCH_NORM ); break;
					case 2:	EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "player/pl_shield_impact3.wav", RANDOM_FLOAT(0.5,1.0), ATTN_NORM, 0, PITCH_NORM ); break;
				}
			}

		}
		break;
		
		case FX_SHIELDEFFECT_PLASMA:
		{
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SPHERE, BLAST_SKIN_SPHERE_SIELD_PLASMA, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.3, 0.0, 0.5, 0, 0, 0.1), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparks(48, origin, 0.4, 0.02, 0, 512, 0.3, 255,255,255, 1, 0, g_iModelIndexExplosion4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_BODY, "items/plasma_shield.wav", RANDOM_FLOAT(0.5,1.0), ATTN_NORM, 0, 90 );
			}
		}
		break;

		case FX_FIRE_SUPRESSOR:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 128, origin, origin, Vector(1,1,1), 15, 120, 128,128,128, 0.5, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(64, 64, origin, origin, Vector(1,1,1), RANDOM_FLOAT(5,6), 0, 128,128,128, 1.0, -0.25, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_11, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(40, 48, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, RANDOM_FLOAT(4,6), 0, 128,128,128, 1.0, -0.2, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_11, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_uw.wav", 1, ATTN_NORM, 0, 90 );
			}
		}
		break;

		case FX_FIRE_SUPRESSOR_BUBBLES:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?92:164, RANDOM_FLOAT(48,64), origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, 0, 128,128,128, 1.0, -0.45, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_11, kRenderTransAdd, 1.0), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE | RENDERSYSTEM_FLAG_CLIPREMOVE);
		}
		break;

		case FX_SONICWAVE:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,255,0, 150, NULL, 0.0, 5.0, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr2, kRenderTransAdd, 0,255,0, 0.6, -0.05, 0.9, 0.0, 24.0, 5.0), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			if (g_pCvarTrailRS->value < 1.0f)
			{
				pBeamTrail = gEngfuncs.pEfxAPI->R_BeamFollow(EntIndex, g_iModelIndexBeamsAll, 0.1, 5, 255,255,255, 0.6);
				if (pBeamTrail)
				{
					pBeamTrail->frame = BLAST_SKIN_PSPBEAM;
					pBeamTrail->frameRate = 0;
				}
			}
			else
				FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexBeamsAll, BLAST_SKIN_PSPBEAM, 0.5, -1.2, 6.0, 32.0, 0.15, 5.0);
		}
		break;

		case FX_SONICWAVE_DETONATE:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			FX_RemoveRS(EntIndex);
   			g_pRenderManager->AddSystem(new CPSBlastCone(12, 75, origin, origin, Vector(1,1,1), 20, 45, 0,200,65, 0.5, -0.75, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 200, origin, origin, Vector(1,1,1), 10, 150, 255,255,255, 0.3, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_7, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_sonicwave.wav", VOL_NORM, ATTN_NORM, 0, 98 + RANDOM_LONG(0,4));
			}
		}
		break;

		case FX_NEEDLE_LASER:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,255,0, 150, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			if (g_pCvarTrailRS->value < 1.0f)
			{
				pBeamTrail = gEngfuncs.pEfxAPI->R_BeamFollow(EntIndex, g_iModelIndexBeamsAll, 0.1, 1.5, 255,255,255, 0.6);
				if (pBeamTrail)
				{
					pBeamTrail->frame = BLAST_SKIN_ENERGYBEAM;
					pBeamTrail->frameRate = 0;
				}
			}
			else
				FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexBeamsAll, BLAST_SKIN_ENERGYBEAM, 1.0, -0.5, 1.0, -1.0, 0.2, 5.0);
		}
		break;

		case FX_NEEDLE_LASER_DETONATE:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			DynamicLight(origin, 80, 0,255,63, 1.0f, 100.0f);

			if (!UTIL_PointIsFar(origin, 0.5))
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(200, 250), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(1.2,1.6), 0, 0,200,0, 1.0, -0.5, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
	   			g_pRenderManager->AddSystem(new CPSBlastCone(8, 32, origin, origin, Vector(1,1,1), 10, 30, 0,255,63, 0.5, -0.75, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_beam1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_beam2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_beam3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
				}
			}
			else //too far
				g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr6, kRenderTransAdd, 0,255,63, 1.0, -1.5, 0.5, 2.0, 25.0, 1.0));
		}
		break;

		case FX_NEEDLE_LASER_BLAST:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr6, kRenderTransAdd, 0,255,63, 1.0, -1.5, 0.5, 2.0, 25.0, 1.0));

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_beam1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_beam2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_beam3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
				}
			}
		}
		break;

		case FX_SHOCK_LASER:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,0,200, 150, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			if (g_pCvarTrailRS->value < 1.0f)
			{
				pBeamTrail = gEngfuncs.pEfxAPI->R_BeamFollow(EntIndex, g_iModelIndexBeamsAll, 0.15, 1.5, 255,255,255, 0.75);
				if (pBeamTrail)
				{
					pBeamTrail->frame = BLAST_SKIN_FROSTGRENADE;
					pBeamTrail->frameRate = 0;
				}
			}
			else
				FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexBeamsAll, BLAST_SKIN_FROSTGRENADE, 0.6, -0.3, 1.0, -1.0, 0.2, 5.0);
		}
		break;

		case FX_SHOCK_LASER_DETONATE:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(6,8), RANDOM_FLOAT(20,32), origin, origin, Vector(1,1,1), 10, 25, 128,128,128, 0.75, -0.75, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			DynamicLight(origin, 80, 0,63,255, 1.0f, 100.0f);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_shocklaser.wav", VOL_NORM, ATTN_NORM, 0, 98 + RANDOM_LONG(0,4));
				g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(24,32), origin, RANDOM_FLOAT(0.6,0.8), RANDOM_FLOAT(0.01,0.02), 0, RANDOM_FLOAT(180,220), RANDOM_FLOAT(1.5,2.0), 255,255,255, 1, 0, g_iModelIndexAnimSpr3, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
		}
		break;

		case FX_SHOCK_LASER_BLAST:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(6,8), RANDOM_FLOAT(20,32), origin, origin, Vector(1,1,1), 10, 25, 128,128,128, 0.75, -0.75, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_shocklaser.wav", VOL_NORM, ATTN_NORM, 0, 98 + RANDOM_LONG(0,4));
			}
			else //too far
				g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr0, kRenderTransAdd, 0,184,255, 1.0, -1.5, 0.25, 1.0, 25.0, 1.0));
		}
		break;

		case FX_MULTICANNON:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 200,0,0, 150, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_MULTICANNON_DETONATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(10, 125, origin, origin, Vector(1,1,1), 3, 1, 0,0,0, 1, -0.7, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_5, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(4, 80, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 8, 35, 255,255,255, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_22, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			DynamicLight(origin, 80, 255,0,0, 1.0f, 100.0f);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_shocklaser.wav", VOL_NORM, ATTN_NORM, 0, 100);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 80, origin, origin, Vector(1,1,1), 5, 40, 255,140,0, 0.5, -0.4, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_4, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(4, 120, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 8, 40, 255,255,255, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_23, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_MULTICANNON_BLAST:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(4, 80, origin, origin, Vector(1,1,1), 8, 35, 255,255,255, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_22, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_shocklaser.wav", VOL_NORM, ATTN_NORM, 0, 100);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 120, origin, origin, Vector(1,1,1), 8, 40, 255,255,255, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_23, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_CHARGECANNON:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,90,200, 200, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr2, kRenderTransAdd, 255,255,255, 1.0, 0.0, 0.05, 0.0, 25.0, 5.0), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_CHARGECANNON_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 125, origin, origin, Vector(1,1,1), 10, 60, 255,255,255, 0.3, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(1, 5, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 8, 40, 255,255,255, 1.0, -0.5, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			DynamicLight(origin, 100, 0,90,250, 0.7f, 50.0f);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_chargecannon.wav", VOL_NORM, ATTN_NORM, 0, 80);
				g_pRenderManager->AddSystem(new CPSBlastCone(15, 80, origin, origin, Vector(1,1,1), 5, 40, 0,90,200, 0.4, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_7, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(3, 80, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 8, 40, 255,255,255, 0.8, -1.0, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_CHARGECANNON_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 125, origin, origin, Vector(1,1,1), 10, 60, 255,255,255, 0.3, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			DynamicLight(origin, 100, 0,90,250, 0.5f, 50.0f);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparks(32, origin, RANDOM_FLOAT(0.5,0.7), RANDOM_FLOAT(0.02,0.03), 0, RANDOM_FLOAT(260,300), RANDOM_FLOAT(0.8,1.1), 255,255,255, 1, 0, g_iModelIndexAnimSpr7, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_chargecannon.wav", VOL_NORM, ATTN_NORM, 0, 80);
			}
		}
		break;

		case FX_HELLHOUNDER:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr2, kRenderTransAdd, 220,130,0, 1.0, 0.0, 0.5, 0.0, 15.0, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 220,130,0, 300, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(250, 10, origin, origin, Vector(0,0,0), 10, 50, 120,120,120, 0.5, -0.16, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_8, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_HELLHOUNDER_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 20, 450, 40, 50, g_iModelIndexBeamsAll, BLAST_SKIN_PLASMA, kRenderTransAdd, 100,100,100, 1, -1, 1));
			g_pRenderManager->AddSystem(new CPSBlastCone(48, 300, origin, origin, Vector(1,1,1), 10, 120, 100,100,100, 1, -1.2, g_iModelIndexPartRed, FALSE, PARTICLE_RED_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 70, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 60, 0,0,0, 0.4, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_1, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(24, origin, 0.8, 0.01, 0, 320, 2, 255,255,255, 1, 0, g_iModelIndexFlame, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,6), RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(220, 280), SPARKSHOWER_SPARKS, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
			}
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_hellhounder.wav", 1, ATTN_LOW, 0, 100);
			DynamicLight(origin, 300, 129,140,0, 0.6, 200.0);
		}
		break;

		case FX_HELLHOUNDER_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(28, 200, origin, origin, Vector(1,1,1), 10, 120, 100,100,100, 1, -1.2, g_iModelIndexPartRed, FALSE, PARTICLE_RED_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparks(16, origin, 0.8, 0.01, 0, 320, 2, 255,255,255, 1, 0, g_iModelIndexFlame, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(1,3), RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(220, 280), SPARKSHOWER_SPARKS, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
			}
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_hellhounder.wav", 1, ATTN_LOW_HIGH, 0, 100);
			DynamicLight(origin, 200, 129,140,0, 0.6, 200.0);
		}
		break;

		case FX_LAVABALL:
		{
			FX_SmokeTrail(origin, FX_SMOKETRAIL_FIRESMOKE, EntIndex, NULL, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 4.0);
			g_pRenderManager->AddSystem(new CRSLight(origin, 220,130,0, 120, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_MINIMISSILE:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 240,240,50, 250, NULL, 0.0, 8, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			FX_SmokeTrail(origin, FX_SMOKETRAIL_FIRESMOKE, EntIndex, NULL, 0, 0.0, 0.0, 0.0, 0.0, 0.0, 8.0);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimglow01, kRenderTransAdd, 128,128,128, 0.6, 0.0, 0.5, 0.0, 15.0, 8), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_MINIMISSILE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(40, 200, origin, origin, Vector(1,1,1), 10, 95, 255,255,255, 0.8, -1, g_iModelIndexMuzzleFlash1, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, 160, origin, Vector(0,0,1), Vector(5,5,1), 5, 0, 255,255,255, 1, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_3, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(230,260), 128,128,0, 0.6, 200.0);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_minimissile.wav", 1, ATTN_LOW_HIGH, 0, PITCH_NORM );

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 320, origin, origin, Vector(1,1,1), 10, 150, 120,0,0, 0.6, -1, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 270, origin, origin, Vector(1,1,1), 5, 180, 100,100,100, 0.7, -0.7, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 70, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 60, 0,0,0, 0.8, -0.3, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_3, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(32, origin, 1, 0.02, 0, 350, 2, 255,255,255, 1, 0, g_iModelIndexMuzzleFlash2, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_MINIMISSILE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, 170, origin, origin, Vector(1,1,1), 10, 95, 255,255,255, 0.8, -1, g_iModelIndexMuzzleFlash1, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(200,230), 128,128,0, 0.6, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_minimissile.wav", 1, ATTN_NORM, 0, PITCH_NORM );
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 280, origin, origin, Vector(1,1,1), 10, 150, 120,0,0, 0.6, -1, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 230, origin, origin, Vector(1,1,1), 5, 180, 100,100,100, 0.7, -0.7, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(24, origin, 1, 0.02, 0, 350, 2, 255,255,255, 1, 0, g_iModelIndexMuzzleFlash2, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_BIOHAZARDMISSILE:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr8, kRenderTransAdd, 100,100,100, 1.0, 0.0, 0.4, 0.0, 15.0, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 240,240,50, 280, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(196, 15, origin, origin, Vector(0,0,0), 10, 50, 0,100,0, 0.5, -0.25, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_2, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_BIOHAZARDMISSILE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 300, origin, origin, Vector(1,1,1), 5, 175, 255,255,255, 0.4, -0.6, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 250, origin, origin, Vector(1,1,1), 5, 100, 255,115,0, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_21, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 225, origin, origin, Vector(1,1,1), 5, 200, 1,1,1, 0.7, -0.9, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 275, origin, origin, Vector(1,1,1), 5, 175, 255,255,255, 0.6, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_19, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 180, origin, origin, Vector(1,1,1), 5, 185, 255,255,255, 0.5, -0.7, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_1, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_biohazardmissile.wav", 1.0, ATTN_LOW_HIGH, 0, PITCH_NORM );
			DynamicLight(origin, RANDOM_FLOAT(210,240), 170,250,0, 0.5, 180.0);
		}
		break;

		case FX_BIOHAZARDMISSILE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 275, origin, origin, Vector(1,1,1), 5, 175, 255,255,255, 0.4, -0.6, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 150, origin, origin, Vector(1,1,1), 5, 100, 255,115,0, 0.6, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_7, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_small1.wav", 1.0, ATTN_NORM, 0, 110 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_small2.wav", 1.0, ATTN_NORM, 0, 110 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_WEAPON, "weapons/explode_small3.wav", 1.0, ATTN_NORM, 0, 110 ); break;
				}
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(140, 180), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(2,3), 0, 0,200,0, 1.0, -0.25, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 250, origin, origin, Vector(1,1,1), 5, 200, 1,1,1, 0.5, -0.7, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 180, origin, origin, Vector(1,1,1), 5, 185, 255,255,255, 0.5, -0.7, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_1, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
			DynamicLight(origin, RANDOM_FLOAT(170,200), 170,250,0, 0.5, 180.0);
		}
		break;

		case FX_BIOHAZARDMISSILE_SMOKE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(128, RANDOM_FLOAT(300,330), origin, Vector(0,0,1), VECTOR_CONE_LOTS, 10, RANDOM_FLOAT(75,90), 0,160,0, 0.7, -0.35, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_3, kRenderTransAdd, 5.0), RENDERSYSTEM_FLAG_CLIPREMOVE, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_BIOHAZARDMISSILE_FINAL_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSSprite(origin+Vector(0,0,20), g_vecZero, g_iModelIndexExplosion1, kRenderTransAdd, 255,255,255, 1.0f, -0.1f, 1.5, 0.0f, 16.0f, 1.0),0, -1, 0);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 150, origin, origin, Vector(1,1,1), 5, 100, 255,115,0, 0.6, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_7, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,64), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
			{
				DecalTrace(RANDOM_LONG(DECAL_EXPSCORCH1,DECAL_EXPSCORCH3), &tr);
				DecalTrace(RANDOM_LONG(DECAL_ACID_BIGSPLAT1,DECAL_ACID_BIGSPLAT3), &tr);
			}

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_biohazardmissile.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(140, 180), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(2,3), 0, 0,200,0, 1.0, -0.25, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 180, origin, origin, Vector(1,1,1), 5, 185, 255,255,255, 0.5, -0.7, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_1, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CRSCylinder(origin+Vector(0,0,12), 20, 600, 60, 40, g_iModelIndexBeamsAll, BLAST_SKIN_ENERGYBEAM, kRenderTransAdd, 255,255,255, 1, -1.5, 0.3));
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,3), RANDOM_FLOAT(1.5,2.5), RANDOM_FLOAT(320,380), SPARKSHOWER_GREENSMOKE, origin+Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
			DynamicLight(origin, RANDOM_FLOAT(170,200), 170,250,0, 0.5, 180.0);
		}
		break;

		case FX_BANANA_DETONATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 135, origin, origin, Vector(1,1,1), 5, 180, 255,255,0, 0.4, -0.6, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(90, 120), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(1.3,1.8), 0, 255,255,255, 1.0, -0.1, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_LEAVES_0,PARTICLE_LEAVES_7), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 150, origin, origin, Vector(1,1,1), 5, 0, 220,220,22, 1.0, -1.0, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

 				switch (RANDOM_LONG(0,1))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "debris/bustflesh1.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "debris/bustflesh2.wav", 1.0, ATTN_NORM, 0, 90 ); break;
				}
			}
		}
		break;

		case FX_BANANA_BLAST:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 120, origin, origin, Vector(1,1,1), 5, 180, 255,255,0, 0.4, -0.6, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 130, origin, origin, Vector(1,1,1), 5, 0, 220,220,22, 1.0, -1.0, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

 				switch (RANDOM_LONG(0,1))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "debris/bustflesh1.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "debris/bustflesh2.wav", 1.0, ATTN_NORM, 0, 90 ); break;
				}
			}
		}
		break;

		case FX_TOILET:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(64, 15, origin, origin, Vector(0,0,0), 10, 50, 128,128,128, 0.2, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_0, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_TOILET_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 250, origin, origin, Vector(1,1,1), 5, 180, 65,44,22, 0.3, -0.5, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 275, origin, origin, Vector(1,1,1), 5, 155, 255,255,255, 0.4, -0.6, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_4, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 150, origin, origin, Vector(1,1,1), 5, 140, 65,44,22, 1.0, -1.0, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				for (int j = 0; j < 8; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(125, 200), false, g_iModelIndexGibModel, RANDOM_LONG(GIB_CONCRETE_01, GIB_CONCRETE_08), SKIN_GIB_TILE, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, BOUNCE_GLASS, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);

				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(280, 350), origin, origin, Vector(1,1,1), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.2, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_TILE_0,PARTICLE_TILE_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(1,2), RANDOM_FLOAT(0.8,1.3), RANDOM_FLOAT(220, 270), SPARKSHOWER_SPARKSMOKE, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
				g_pRenderManager->AddSystem(new CPSSparks(28, origin, 0.4, 0.04, 0, 170, 1.5, 255,255,255, 1, 0, g_iModelIndexPartWhite, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				
				switch (RANDOM_LONG(0,1))
				{
					case 0: EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "debris/bustglass1.wav", 1.0, ATTN_NORM, 0, RANDOM_LONG(50,80)); break;
					case 1: EMIT_SOUND(EntIndex, origin, CHAN_BODY, "debris/bustglass2.wav", 1.0, ATTN_NORM, 0, RANDOM_LONG(50,80)); break;
				}
			}
		}
		break;

		case FX_TOILET_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(14, 210, origin, origin, Vector(1,1,1), 5, 180, 65,44,22, 0.3, -0.5, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(18, 130, origin, origin, Vector(1,1,1), 5, 140, 65,44,22, 1.0, -1.0, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				for (int j = 0; j < 6; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(125, 200), false, g_iModelIndexGibModel, RANDOM_LONG(GIB_CONCRETE_01, GIB_CONCRETE_08), SKIN_GIB_TILE, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, BOUNCE_GLASS, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);

				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(200, 250), origin, origin, Vector(1,1,1), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.2, g_iModelIndexPartGibs, FALSE, RANDOM_LONG(PARTICLE_TILE_0,PARTICLE_TILE_5), kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				
				switch (RANDOM_LONG(0,1))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "debris/bustglass1.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "debris/bustglass2.wav", 1.0, ATTN_NORM, 0, 90 ); break;
				}
			}
		}
		break;

		case FX_GLUON:
		{
			g_pRenderManager->AddSystem(new CRSTeleparts(origin, 100, 255, 0, 10, EntIndex, 170,90,250));
			g_pRenderManager->AddSystem(new CPSBlastCone(96, 15, origin, origin, Vector(0,0,0), 10, 50, 222,100,150, 0.5, -1.0, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr6, kRenderTransAdd, 222,100,150, 1.0, 0.0, 0.7, 0.0, 24.0, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 222,28,150, 300, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			if (g_pCvarTrailRS->value < 1.0f)
			{			
				pBeamTrail = gEngfuncs.pEfxAPI->R_BeamFollow(EntIndex, g_iModelIndexBeamsAll, 0.5, 6, 255,255,255, 0.2);
				if (pBeamTrail)
				{
					pBeamTrail->frame = BLAST_SKIN_TAUBEAM;
					pBeamTrail->frameRate = 0;
				}
			}
			else
				FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexBeamsAll, BLAST_SKIN_TAUBEAM, 1.0, -2.0, 3.0, 12.0, 0.5, 10.0);
		}
		break;

		case FX_GLUON_DETONATE:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 500, 40, 40, g_iModelIndexBeamsAll, BLAST_SKIN_GLUON, kRenderTransAdd, 255,255,255, 0.8, -0.8, 0.5));
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 20, 300, 20, 40, g_iModelIndexBeamsAll, BLAST_SKIN_GLUON, kRenderTransAdd, 255,255,255, 0.8, -0.8, 0.5));
			g_pRenderManager->AddSystem(new CPSBlastCone(96, 350, origin, origin, Vector(1,1,1), 10, 80, 222,100,150, 1, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(320,360), 222,100,150, 0.7, 200.0);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_gluon.wav", 1, ATTN_LOW_HIGH, 0, PITCH_NORM );

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(64, 250, origin, Vector(0,0,1), VECTOR_CONE_LOTS, 10, 60, 222,100,150, 0.4, -0.4, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				g_pRenderManager->AddSystem(new CRSCylinder(origin, 30, 400, 30, 40, g_iModelIndexBeamsAll, BLAST_SKIN_GLUON, kRenderTransAdd, 255,255,255, 0.8, -0.8, 0.5));
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(220, 310), origin, origin, Vector(1,1,1), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexAnimSpr2, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(220, 310), origin, origin, Vector(1,1,1), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexAnimSpr3, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(220, 310), origin, origin, Vector(1,1,1), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexExplosion4, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(220, 310), origin, origin, Vector(1,1,1), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
		}
		break;

		case FX_GLUON_BLAST:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(64, 250, origin, origin, Vector(1,1,1), 10, 60, 222,100,150, 1, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(240,260), 222,100,150, 0.7, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_gluon.wav", 1, ATTN_NORM, 0, PITCH_NORM );
				g_pRenderManager->AddSystem(new CPSBlastCone(48, 200, origin, origin, Vector(1,1,1), 10, 45, 222,100,150, 0.4, -0.4, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CRSCylinder(origin, 30, 400, 30, 40, g_iModelIndexBeamsAll, BLAST_SKIN_GLUON, kRenderTransAdd, 255,255,255, 0.8, -0.8, 0.5));
			}
		}
		break;

		case FX_GLUON_COLLIDE:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(96, 400, origin, origin, Vector(1,1,1), 10, 120, 222,100,150, 1, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(520,560), 222,100,150, 0.7, 200.0);
			g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(75,0,0), g_iModelIndexPartWhite, kRenderTransAdd, 222,100,150, 1.0, -1.0, 0.1, 10, PARTICLE_WHITE_14, 0, 0.0));
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_EXPLOSION_01, BLAST_SKIN_SPHERE_SIELD_INV, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.75, -1.5, RANDOM_FLOAT(0.03,0.05), RANDOM_FLOAT(3,5), 0, 0.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_gluon.wav", 1, ATTN_LOW, 0, PITCH_HIGH );

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(64, 300, origin, origin, Vector(1,1,1), 10, 100, 222,100,150, 0.5, -0.5, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CRSCylinder(origin, 30, 900, 30, 50, g_iModelIndexBeamsAll, BLAST_SKIN_GLUON, kRenderTransAdd, 222,100,150, 0.8, -0.8, 0.5));
			}
		}
		break;

		case FX_DISRUPTOR_MAIN:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,90,200, 350, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr6, kRenderTransAdd, 255,255,255, 1.0, 0.0, 1.0, 0.0, 24.0, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			if (g_pCvarTrailRS->value < 1.0f)
			{
				pBeamTrail = gEngfuncs.pEfxAPI->R_BeamFollow(EntIndex, g_iModelIndexBeamsAll, 0.5, 6, 255,255,255, 0.2);
				if (pBeamTrail)
				{
					pBeamTrail->frame = BLAST_SKIN_TELEENTER;
					pBeamTrail->frameRate = 0;
				}
			}
			else
				FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexAnimSpr7, 0, 1.0, -1.5, 3.0, 12.0, 1.0, 10.0);
		}
		break;

		case FX_DISRUPTOR_DETONATE:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(80, 320, origin, origin, Vector(1,1,1), 10, 100, 170,90,250, 0.5, -0.6, g_iModelIndexPartRed, FALSE, PARTICLE_RED_9, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(40, 280, origin, origin, Vector(1,1,1), 10, 50, 0,90,250, 1, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(32, origin, 1, 0.1, 0, 375, 1, 255,255,255, 1, -0.75, g_iModelIndexPartBlue, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			}
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_disruptor.wav", 1, ATTN_LOW_HIGH, 0, PITCH_NORM );
			DynamicLight(origin, 400, 0,90,200, 0.7, 200.0);
		}
		break;

		case FX_DISRUPTOR_BLAST:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);

			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(28, 250, origin, origin, Vector(1,1,1), 10, 80, 170,90,250, 1, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(-40,0,0), g_iModelIndexAnimSpr3, kRenderTransAdd, 0,90,250, 1.0, -1.0, 2, 12, 0, 20, 0.0));

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 180, origin, origin, Vector(1,1,1), 10, 50, 0,90,250, 1, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(45,0,0), g_iModelIndexAnimSpr3, kRenderTransAdd, 0,90,250, 1.0, -1.0, 2, 6, 0, 20, 0.0));
			}
		}
		break;

		case FX_DISRUPTOR_BABY:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr0, kRenderTransAdd, 255,255,255, 1.0, 0.0, 0.2, 0.0, 24.0, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamFollow(EntIndex, g_iModelIndexAnimSpr2, 0.5, 6, 255,255,255, 0.2);
			else
				FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexAnimSpr2, 0, 1.0, -2.0, 3.0, 12.0, 0.5, 10.0);
		}
		break;

		case FX_HELLFIRE:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 220,130,0, 280, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr9, kRenderTransAdd, 255,255,255, 0.8, 0.0, 0.4, 0.0, 15.0, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(128, 25, origin, origin, Vector(0,0,0), 15, 30, 0,0,0, 0.6, -0.35, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_1, kRenderTransAlpha, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 10, origin, origin, VECTOR_CONE_20DEGREES, 5, 1, 255,255,255, 1, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_4, kRenderTransAdd, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_HELLFIRE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 100, origin, origin, Vector(1,1,1), 5, 80, 128,128,128, 1, -1, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 180, origin, origin, Vector(1,1,1), 10, 120, 128,128,128, 1, -1, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 180, origin, origin, Vector(1,1,1), 10, 120, 0,0,0, 0.5, -0.5, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_3, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 240, origin, origin, Vector(1,1,1), 10, 150, 128,128,128, 1, -1, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 100, origin, origin, Vector(1,1,1), 5, 80, 0,0,0, 0.5, -0.5, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_4, kRenderTransAlpha, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(96, origin, 0.5, 0.01, 0, 350, 2.3, 255,255,255, 1, 0, g_iModelIndexFlame, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 240, origin, origin, Vector(1,1,1), 10, 150, 0,0,0, 0.6, -0.6, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
			switch (RANDOM_LONG(0,2))
			{
				case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode3.wav", 1.0, ATTN_LOW_HIGH, 0, 100 ); break;
				case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode4.wav", 1.0, ATTN_LOW_HIGH, 0, 90 ); break;
				case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode5.wav", 1.0, ATTN_LOW_HIGH, 0, 110 ); break;
			}
			//fire field
			if (gEngfuncs.PM_PointContents(origin, NULL) != CONTENTS_WATER || gEngfuncs.PM_PointContents(origin, NULL) != CONTENTS_SLIME)
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?24:32, RANDOM_FLOAT(232,280), origin+Vector(0,0,4), Vector(0,0,1), VECTOR_CONE_20DEGREES, RANDOM_FLOAT(65,80), RANDOM_FLOAT(-35,-50), 128,128,128, 1, -1.0, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 5.0), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				DynamicLight(origin, 400, 128,128,40, 5.0, 20.0);

				gEngfuncs.pEventAPI->EV_SetTraceHull(2);

				for (int i = 0 ; i < 6 ; ++i)
				{
					dir.x=RANDOM_FLOAT(-1,1);
					dir.y=RANDOM_FLOAT(-1,1);
					dir.z=RANDOM_FLOAT(-1,0);
					dir=dir.Normalize();

					gEngfuncs.pEventAPI->EV_PlayerTrace(origin+Vector(0,0,128), origin+(dir*512), PM_STUDIO_IGNORE, -1, &tr);

					if (gEngfuncs.PM_PointContents(tr.endpos, NULL) != CONTENTS_WATER || gEngfuncs.PM_PointContents(tr.endpos, NULL) != CONTENTS_SLIME)
					{
						if (tr.fraction != 1)
						{
							DecalTrace(RANDOM_LONG(DECAL_OFSCORCH1,DECAL_OFSCORCH3), &tr);
							g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?16:24, RANDOM_FLOAT(164,200), tr.endpos+Vector(0,0,4), Vector(0,0,1), VECTOR_CONE_20DEGREES, RANDOM_FLOAT(40,60), RANDOM_FLOAT(-20,-30), 128,128,128, 1, -1.0, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 5.0), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
						}	
					}
				}
			}
		}
		break;

		case FX_HELLFIRE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 100, origin, origin, Vector(1,1,1), 5, 80, 128,128,128, 1, -1, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 180, origin, origin, Vector(1,1,1), 10, 120, 128,128,128, 1, -1, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 240, origin, origin, Vector(1,1,1), 10, 150, 128,128,128, 1, -1, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, 250, 128,128,40, 0.6, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode3.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode4.wav", 1.0, ATTN_NORM, 0, 90 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode5.wav", 1.0, ATTN_NORM, 0, 110 ); break;
				}
				g_pRenderManager->AddSystem(new CPSSparks(48, origin, 0.5, 0.01, 0, 350, 2.3, 255,255,255, 1, 0, g_iModelIndexFlame, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 240, origin, origin, Vector(1,1,1), 10, 150, 0,0,0, 0.6, -0.6, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case FX_PROTONMISSILE:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,230,250, 280, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 0, origin, origin, Vector(0,0,0), 10, 30, 0,0,0, 0.5, -0.2, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_12, kRenderTransAlpha, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 5, origin, origin, Vector(0,0,0), 10, 20, 0,120,250, 0.75, -0.25, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_3, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexExplosion4, kRenderTransAdd, 255,255,255, 0.8, 0.0, 0.4, 0.0, 24.0, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(164, 10, origin, origin, VECTOR_CONE_40DEGREES, 5, 2, 255,255,255, 1, -1.2, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_0, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_PROTONMISSILE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 200, origin, origin, Vector(1,1,1), 10, 100, 128,128,128, 1, -0.9, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 230, origin, origin, Vector(1,1,1), 10, 80, 128,128,128, 1, -0.9, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 100, origin, origin, Vector(1,1,1), 10, 50, 0,80,250, 0.8, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 150, origin, origin, Vector(1,1,1), 10, 60, 0,120,250, 1, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_protonmissile.wav", 1, ATTN_LOW, 0, PITCH_NORM );
			DynamicLight(origin, 300, 0,230,250, 0.6, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,5), RANDOM_FLOAT(2.0,3.0), RANDOM_FLOAT(350,400), SPARKSHOWER_BLUEENERGY, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 200, origin, origin, Vector(1,1,1), 10, 70, 0,160,250, 0.8, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 250, origin, origin, Vector(1,1,1), 10, 80, 0,200,250, 1, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(40, origin, 0.6, 0.05, 0, 200, 3, 255,255,255,1,0, g_iModelIndexAnimSpr6, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(200, 320), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(1.6,3.2), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(220, 340), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(1.6,3.2), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr2, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(-45,0,0), g_iModelIndexPartGreen, kRenderTransAdd, 128,128,128, 1.0, -0.25, 2, 5, PARTICLE_GREEN_2, 0, 0.0));
			}
		}
		break;

		case FX_PROTONMISSILE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 180, origin, origin, Vector(1,1,1), 10, 100, 128,128,128, 1, -0.9, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 200, origin, origin, Vector(1,1,1), 10, 80, 128,128,128, 1, -0.9, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 130, origin, origin, Vector(1,1,1), 10, 60, 0,120,250, 1, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_protonmissile.wav", 1, ATTN_LOW_HIGH, 0, PITCH_NORM );
			DynamicLight(origin, 240, 0,230,250, 0.6, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 220, origin, origin, Vector(1,1,1), 10, 80, 0,200,250, 1, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(32, origin, 0.6, 0.05, 0, 200, 3, 255,255,255,1,0, g_iModelIndexAnimSpr6, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(-45,0,0), g_iModelIndexPartGreen, kRenderTransAdd, 128,128,128, 1.0, -0.25, 2, 5, PARTICLE_GREEN_2, 0, 0.0));
			}
		}
		break;

		case FX_PROTONMISSILE_SHARD_DETONATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 100, origin, origin, Vector(1,1,1), 10, 100, 128,128,128, 1, -0.9, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(8, 50, origin, origin, Vector(1,1,1), 10, 50, 0,80,250, 0.8, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(8, 75, origin, origin, Vector(1,1,1), 10, 60, 0,120,250, 1, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, 200, 0,230,250, 0.6, 80.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode1.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode2.wav", 1.0, ATTN_NORM, 0, 90 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode3.wav", 1.0, ATTN_NORM, 0, 110 ); break;
				}
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 115, origin, origin, Vector(1,1,1), 10, 80, 128,128,128, 1, -0.9, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 100, origin, origin, Vector(1,1,1), 10, 70, 0,160,250, 0.8, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 140, origin, origin, Vector(1,1,1), 10, 80, 0,200,250, 1, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_11, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case FX_NUCLEARMISSILE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 35, origin, origin, Vector(0.2,0.2,0.5), 12, 50, 255,255,255, 0.25, -0.1, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_2, kRenderTransAdd, 12), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr8, kRenderTransAdd, 255,200,200, 0.8, 0.0, 0.5, 0.0, 15.0, 12), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 240,180,0, 320, NULL, 0.0, 12, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(164, 10, origin, origin, VECTOR_CONE_40DEGREES, 20, -10, 255,255,255, 1, -1.2, g_iModelIndexPartRed, FALSE, PARTICLE_RED_30, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

		}
		break;

		case FX_NUCLEARMISSILE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_nuclearmissile.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			DynamicLight(origin, 1000, 229,97,0, 2.5, 250.0);
			g_pRenderManager->AddSystem(new CPSSparks(UTIL_PointIsFar(origin, 1.0)?40:72, origin, 0.6, 0.05, 0, 600, 2, 255,255,255,1,0, g_iModelIndexFlame, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 100, origin, Vector(0,0,2), Vector(0.5,0.5,1.0), 10, 250, 170,80,32, 1, -0.25, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 50, origin, Vector(0,0,2), Vector(0.5,0.5,1.0), 10, 150, 170,80,32, 1, -0.25, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_MUSHROOM, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -0.2, 0.5, 1.5, 0, 10), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
	
			g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(4,7), RANDOM_FLOAT(3.0,4.0), RANDOM_FLOAT(750,1000), SPARKSHOWER_LAVA_FLAME, origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
		}
		break;

		case FX_NUCLEARMISSILE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode3.wav", 1, ATTN_LOW, 0, PITCH_NORM );
			DynamicLight(origin, 500, 229,97,0, 1.25, 100.0);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 80, origin, Vector(0,0,2), Vector(0.5,0.5,1.0), 10, 150, 170,80,32, 1, -0.4, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 40, origin, Vector(0,0,2), Vector(0.5,0.5,1.0), 10, 100, 170,80,32, 1, -0.4, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SPHERE, BLAST_SKIN_FIRE_NUKE, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.75, -1.5, 0.05, 12, 0, 0.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
		}
		break;

		case FX_NUCLEARMISSILE_RING_DETONATE:
		{
			EMIT_SOUND(EntIndex, origin, CHAN_STATIC, "weapons/airstrike_mortarhit.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			DynamicLight(origin, 1250, 200,200,200, 0.5, 250.0);
			g_pRenderManager->AddSystem(new CRSCylinder(origin+Vector(0,0,164), 20, 2200, 120, 70, g_iModelIndexBeamsAll, BLAST_SKIN_WASTEDBEAM, kRenderTransAdd, 145,50,0, 1, -1, 0.6));
			g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(4,7), RANDOM_FLOAT(3.0,4.0), RANDOM_FLOAT(600,800), SPARKSHOWER_FLICKER, origin+Vector(0,0,164), Vector(0,0,1), VECTOR_CONE_LOTS, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
		}
		break;

		case FX_FROSTBALL:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr11, kRenderTransAdd, 255,255,255, 1.0, 0.0, 0.25, 0.0, 20.0, 7), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(128, 40, origin, Vector(0,0,-1), Vector(0.2,0.2,0.5), 30, 60, 200,200,200, 0.3, -0.45, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 7), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

		}
		break;

		case FX_FROSTBALL_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(8, 100, origin, origin, Vector(1,1,1), 5, 110, 255,255,255, 0.3, -0.2, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(8, 75, origin, origin, Vector(1,1,1), 5, 85, 255,255,255, 0.4, -0.3, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "debris/glass1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "debris/glass2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "debris/glass3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
				}
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(120, 250), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(120, 250), origin, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_6, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(4, 50, origin, origin, Vector(1,1,1), 5, 85, 255,255,255, 0.5, -0.4, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_6, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case FX_FROSTBALL_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(4, 80, origin, origin, Vector(1,1,1), 5, 110, 255,255,255, 0.3, -0.2, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "debris/glass1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "debris/glass2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "debris/glass3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
				}
				g_pRenderManager->AddSystem(new CPSBlastCone(4, 60, origin, origin, Vector(1,1,1), 5, 85, 255,255,255, 0.4, -0.3, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case FX_ANTIMATHERIALMISSILE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(196, 35, origin, origin, Vector(0.2,0.2,0.5), 18, 60, 1,1,1, 0.4, -0.2, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_4, kRenderTransAlpha, 12), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr5, kRenderTransAdd, 255,200,200, 0.8, 0.0, 0.5, 0.0, 15.0, 12), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,180,255, 320, NULL, 0.0, 12, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSSparkShower(4, 0.05, 0, SPARKSHOWER_STREAKS, origin, origin, Vector(0,0,0), g_iModelIndexPartRed, 12), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE); 
		}
		break;

		case FX_ANTIMATHERIALMISSILE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_antimatherialmissile.wav", 1, ATTN_LOW, 0, 100 );
			g_pRenderManager->AddSystem(new CPSSparks(256, origin, 0.8, 0.03, 0, -800, 2, 0, 0, 0, 1, 0, g_iModelIndexPartBlack, kRenderTransAlpha, 5.0), 0, -1);
			g_pRenderManager->AddSystem(new CPSSparks(192, origin, 0.8, 0.03, 0, -800, 2, 255,255,255,1,0, g_iModelIndexPartWhite, kRenderTransAdd, 5.0), 0, -1);
			g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexPartBlack, PARTICLE_BLACK_9, 0, 120, kRenderTransAlpha, 0,0,0, 1, 0, 500, -100, 5.0), 0, -1);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexPartBlack, kRenderTransAlpha, 0,0,0, 1.0, 0, 1, 0.0, 0.0, 4.0), 0, -1);
		}
		break;

		case FX_ANTIMATHERIALMISSILE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(64, 480, origin, origin, Vector(1,1,1), 5, 250, 128,128,128, 0.7, -0.6, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(48, 350, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 75, 160,0,217, 0.3, -0.4, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_4, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_antimatherial.wav", 1, ATTN_LOW_HIGH, 0, PITCH_NORM );
			DynamicLight(origin, 500, 240,170,30, 0.6, 150.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 400, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 200, 255,115,0, 0.4, -0.3, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, 420, origin, origin, Vector(1,1,1), 5, 100, 0,0,217, 0.8, -0.9, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 60, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 65, 0,0,0, 0.2, -0.05, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_7, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(8, RANDOM_FLOAT(1.7,2.4), RANDOM_FLOAT(400, 600), SPARKSHOWER_SPARKSMOKE, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexExplosion5, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
			}
		}
		break;

		case FX_ANTIMATHERIALMISSILE_BLACKHOLE_DETONATE:
		{
			EMIT_SOUND(EntIndex, origin, CHAN_WEAPON, "weapons/explode_blackhole.wav", 1, ATTN_LOW, 0, 100);
			g_pRenderManager->AddSystem(new CPSBlastCone(48, 1000, origin, origin, Vector(1,1,1), 10, 350, 255,255,255, 0.2, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_7, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(48, 1200, origin, origin, Vector(1,1,1), 10, 300, 255,255,255, 0.2, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(45,0,0), g_iModelIndexPartGreen, kRenderTransAdd, 128,128,128, 1.0, -0.2, 1, 5, PARTICLE_GREEN_3, 0, 0.0));

			if (!UTIL_PointIsFar(origin, 1.0))
				g_pRenderManager->AddSystem(new CPSBlastCone(48, 900, origin, origin, Vector(1,1,1), 10, 320, 255,255,255, 0.2, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		}
		break;

		case FX_SUNOFGOD:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexMuzzleFlash2, kRenderTransAdd, 255,255,255, 1.0, 0.0, 0.9, 0.0, 24.0, 5.0), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 248,248,100, 160, NULL, 0.0, 5.0, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(128, 75, origin, origin, VECTOR_CONE_20DEGREES, 9, -3, 255,255,255, 0.5, -0.3, g_iModelIndexPartRed, FALSE, PARTICLE_RED_4, kRenderTransAdd, 5.0), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(64, 10, origin, origin, VECTOR_CONE_20DEGREES, 30, -20, 255,255,255, 0.5, -1.0, g_iModelIndexMuzzleFlash2, TRUE, 0, kRenderTransAdd, 5.0), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_SUNOFGOD_PREPARE_TO_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			gEngfuncs.pEfxAPI->R_Implosion( origin, 600, 200, 8);
			g_pRenderManager->AddSystem(new CRSLight(origin, 255,255,128, 800, NULL, 0.0, 8.2, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_STAYANDFORGET);
			g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexAnimSpr11, 0, 0, 64, kRenderTransAdd, 180,100,0, 0.7, 0, 80, -8, 8), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_STAYANDFORGET);
			g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexAnimSpr2, 0, 1, 50, kRenderTransAdd, 230,140,0, 1, 0, 100, 0, 8), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_STAYANDFORGET);
			g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?96:128, 40, origin, Vector(0,1,0), Vector(3,3,1), 50, 100, 128,128,128, 0.8, -0.9, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 8), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_STAYANDFORGET);
			g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?96:128, 150, origin, origin, Vector(1,1,1), 50, 125, 255,255,255, 0.2, -0.2, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_6, kRenderTransAdd, 8), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_STAYANDFORGET);
		}
		break;

		case FX_SUNOFGOD_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_sunofgod.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 1000, origin, origin, Vector(1,1,1), 50, 300, 128,128,128, 1, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRenderSystem(origin+Vector(0,0,64), Vector(0,0,0), Vector(75,0,0), g_iModelIndexPartWhite, kRenderTransAdd, 255,255,255, 1.0, -0.5, 5, 50, PARTICLE_WHITE_14, 0, 0.0));
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SPHERE_HD, BLAST_SKIN_SPHERE_SUNOFGOD, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -0.5, 0.5, 5.0, 0, 2.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			DynamicLight(origin, 1500, 255,255,128, 1.25, 500.0);
			g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexAnimSpr2, 0, 1, 150, kRenderTransAdd, 230,140,0, 1.0, -0.2, 200, 600, 5.0), 0, -1, RENDERSYSTEM_FFLAG_ICNF_STAYANDFORGET);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);

			for (int i = 0 ; i < 12 ; ++i)
			{
				dir.x=RANDOM_FLOAT(-1,1);
				dir.y=RANDOM_FLOAT(-1,1);
				dir.z=RANDOM_FLOAT(-1,0);
				dir=dir.Normalize();

				gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin+(dir*768), PM_STUDIO_IGNORE, -1, &tr);

				if (gEngfuncs.PM_PointContents(tr.endpos, NULL) != CONTENTS_WATER || gEngfuncs.PM_PointContents(tr.endpos, NULL) != CONTENTS_SLIME)
				{
					if (tr.fraction != 1)
					{
						DecalTrace(RANDOM_LONG(DECAL_EXPSCORCH1,DECAL_EXPSCORCH3), &tr);
						g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?12:16, RANDOM_FLOAT(92,128), tr.endpos+Vector(0,0,4), Vector(0,0,1), VECTOR_CONE_20DEGREES, RANDOM_FLOAT(40,60), RANDOM_FLOAT(-20,-30), 128,128,128, 1, -1.2, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 5.0), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					}	
				}
			}	
		}
		break;

		case FX_SUNOFGOD_BLAST:
		{
			FX_RemoveRS(EntIndex);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_sunofgod.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			g_pRenderManager->AddSystem(new CPSBlastCone(128, 700, origin, origin, Vector(1,1,1), 50, 200, 128,128,128, 1, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SPHERE, BLAST_SKIN_FIRE_SPARKS, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -0.5, 0.5, 3.0, 0, 2.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			DynamicLight(origin, 1000, 255,255,128, 0.9, 500.0);
		}
		break;

		case FX_SCORCHER:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 250,250,0, 300, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexMuzzleFlash1, 0, 1, 70, kRenderTransAdd, 255,255,255, 1, 0, 30, 0, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(96, 0, origin, origin, Vector(0,0,0), 20, -10, 255,255,255, 1, -2.5, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

		}
		break;

		case FX_SCORCHER_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_scorcher.wav", 1, ATTN_LOW_HIGH, 0, 100);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 120, origin, origin, Vector(1,1,1), 12, 90, 128,128,128, 1, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_37, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 150, origin, origin, Vector(1,1,1), 15, 120, 128,128,128, 0.8, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_39, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(320,380), 250,250,0, 0.7, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 70, origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, 60, 132,44,0, 0.4, -0.2, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,6), RANDOM_FLOAT(1.5,2.2), RANDOM_FLOAT(320, 400), SPARKSHOWER_FIREEXP, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSSparks(64, origin, 2.0, 0.02, 0, 400, 1, 255,255,255, 1, -1, g_iModelIndexMuzzleFlash1, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_SCORCHER_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 100, origin, origin, Vector(1,1,1), 12, 90, 128,128,128, 1, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_37, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 120, origin, origin, Vector(1,1,1), 15, 120, 128,128,128, 0.8, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_39, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(280,330), 250,250,0, 0.7, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_scorcher.wav", 1, ATTN_NORM, 0, 100);
				g_pRenderManager->AddSystem(new CPSSparks(48, origin, 2.0, 0.02, 0, 400, 1, 255,255,255, 1, -1, g_iModelIndexMuzzleFlash1, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_PLASMA_SHIELD_CHARGE:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,255,92, 300, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_PLASMA_SHIELD_CHARGE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			DynamicLight(origin, RANDOM_FLOAT(280,330), 0,255,92, 0.7, 200.0);

			g_pRenderManager->AddSystem(new CPSBlastCone(24, 240, origin, origin, Vector(1,1,1), 10, 90, 255,255,255, 0.6, -1.0, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 200, origin, origin, Vector(1,1,1), 10, 70, 255,255,255, 0.6, -1.0, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(60,0,0), g_iModelIndexPartGreen, kRenderTransAdd, 255,255,255, 1.0, -2.0, 0.1, 6, PARTICLE_GREEN_4, 0, 0.0));

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(300,350), origin, origin, Vector(1,1,1), 5, 0, 255,255,255, 0.8, -1.2, g_iModelIndexAnimSpr1, FALSE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 180, origin, origin, Vector(1,1,1), 10, 50, 255,255,255, 0.6, -1.0, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_plasma_shield_charge.wav", 1.0, ATTN_NORM, 0, 110 );

				for (int i = 0 ; i < 12 ; ++i)
				{
					dir.x=RANDOM_FLOAT(-1,1);
					dir.y=RANDOM_FLOAT(-1,1);
					dir.z=RANDOM_FLOAT(-1,1);
					dir=dir.Normalize();
					gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin+(dir*RANDOM_FLOAT(64, 256)), PM_STUDIO_IGNORE, -1, &tr);
					g_pRenderManager->AddSystem(new CRSBeam(origin, tr.endpos, g_iModelIndexFlame, 0, kRenderTransAdd, 0,255,92, 0.8, -2.4, 2.5, -2.5, 0.15), 0, -1, 0);
				}	
			}
		}
		break;

		case FX_PLASMA_SHIELD_CHARGE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			DynamicLight(origin, RANDOM_FLOAT(200,240), 0,255,92, 0.5, 200.0);
			g_pRenderManager->AddSystem(new CPSBlastCone(18, 200, origin, origin, Vector(1,1,1), 10, 90, 255,255,255, 0.7, -1.0, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(18, 170, origin, origin, Vector(1,1,1), 10, 70, 255,255,255, 0.7, -1.0, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(60,0,0), g_iModelIndexPartGreen, kRenderTransAdd, 255,255,255, 1.0, -2.0, 0.1, 6, PARTICLE_GREEN_4, 0, 0.0));

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 140, origin, origin, Vector(1,1,1), 10, 50, 255,255,255, 0.7, -1.0, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_plasma_shield_charge.wav", 1.0, ATTN_NORM, 0, 110 );
			}
		}
		break;

		case FX_TRIDENT:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 100,100,164, 300, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr1, kRenderTransAdd, 255,255,255, 1.0, 0.0, 0.2, 0.0, 24.0, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 10, origin, origin, VECTOR_CONE_20DEGREES, 20, -10, 255,255,255, 0.75, -2.0, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_TRIDENT_BLAST:
		{
			FX_RemoveRS(EntIndex);
			DynamicLight(origin, RANDOM_FLOAT(240,300), 100,100,100, 0.5, 200.0);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, 220, origin, origin, Vector(1,1,1), 10, 70, 128,128,128, 1, -1.3, g_iModelIndexAnimSpr1, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, 180, origin, origin, Vector(1,1,1), 10, 50, 128,128,128, 1, -1.3, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_trident.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_trident.wav", 1.0, ATTN_NORM, 0, 90 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_trident.wav", 1.0, ATTN_NORM, 0, 110 ); break;
				}
			}

		}
		break;

		case FX_TRIDENT_COLLIDE:
		{
			FX_RemoveRS(EntIndex);
			DynamicLight(origin, RANDOM_FLOAT(450,500), 100,100,100, 0.7, 200.0);

			g_pRenderManager->AddSystem(new CPSBlastCone(48, 350, origin, origin, Vector(1,1,1), 10, 110, 128,128,128, 1, -1.3, g_iModelIndexAnimSpr1, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(48, 300, origin, origin, Vector(1,1,1), 10, 90, 128,128,128, 1, -1.3, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRSModel(origin, Vector(33,0,0), g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SHOCKWAVE, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.7, -1.5, 0.1, 4.0, 0, 1.0), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(200, 250), origin, origin, Vector(1,1,1), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexAnimSpr1, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS  | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, 250, origin, origin, Vector(1,1,1), 10, 75, 128,128,128, 1, -1.2, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_0, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 200, origin, origin, Vector(1,1,1), 10, 60, 128,128,128, 1, -1.2, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_1, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(300, 350), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.33, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS  | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_trident.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_trident.wav", 1.0, ATTN_NORM, 0, 90 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_trident.wav", 1.0, ATTN_NORM, 0, 110 ); break;
				}
			}
		}
		break;

		case FX_TELEPORT:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 65,250,85, 300, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr12, kRenderTransAdd, 255,255,255, 1.0, 0.0, 0.5, 0.0, 24.0, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSSparkShower(12, 0.05, 0, SPARKSHOWER_GREEN_LIGHTNING_STRIKE, origin, origin, Vector(0,0,0), g_iModelIndexPartRed, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE); 
		}
		break;

		case FX_TELEPORT_ENTER:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 65,250,85, 300, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr4, kRenderTransAdd, 65,250,85, 1.0, 0.0, 0.4, 0.0, 24.0, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSDrips(UTIL_PointIsFar(origin, 1.0)?64:128, origin+Vector(-96,-96,0), Vector(32,32,32), Vector(164,164,164), Vector(0,0,4096), g_iModelIndexAnimSpr12, NULL, NULL, kRenderTransAdd, 0.2, 5, 0.0f, 0.0, 0.1), 0, EntIndex, RENDERSYSTEM_FLAG_ADDPHYSICS);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "common/teleport.wav", 1, ATTN_LOW_HIGH, 0, 100 );

			if (!UTIL_PointIsFar(origin, 1.0))
				g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexAnimSpr4, 0, 1, 64, kRenderTransAdd, 65,250,85, 1.0, 0.1, 60, -15, 5.0), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_TELEPORT_USE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 600, 80, 40, g_iModelIndexBeamsAll, BLAST_SKIN_DISPLACER, kRenderTransAdd, 255,255,255, 1, -1, 0.8));
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_teleport.wav", 1, ATTN_LOW, 0, 100);
			DynamicLight(origin, RANDOM_FLOAT(550,600), 65,250,85, 0.7, 200.0);
			gEngfuncs.pEfxAPI->R_TempSprite(origin, (float *)&g_vecZero, 1.0f, g_iModelIndexAnimSpr12, kRenderGlow, kRenderFxNoDissipation, 1.0f, 0.4f, FTENT_FADEOUT);

			CPSDrips *pRS = new CPSDrips(UTIL_PointIsFar(origin, 1.0)?64:128, origin+Vector(-96,-96,384), Vector(32,32,32), Vector(164,164,164), Vector(0,0,-2048), g_iModelIndexAnimSpr12, NULL, NULL, kRenderTransAdd, 0.2, 5, 0.0f, 0.0, 0.1);
			if (pRS)
			{
				pRS->m_color.r = 128;
				pRS->m_color.g = 128;
				pRS->m_color.b = 128;
				pRS->m_fBrightness = 200;
				g_pRenderManager->AddSystem(pRS, RENDERSYSTEM_FLAG_ADDPHYSICS, EntIndex);
			}
		}
		break;

		case FX_TELEPORT_ENTER_USE:
		{
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "common/teleport.wav", 1, ATTN_NORM, 0, 100);
			DynamicLight(origin, RANDOM_FLOAT(300,400), 65,250,85, 0.7, 200.0);
			g_pRenderManager->AddSystem(new CPSDrips(UTIL_PointIsFar(origin, 1.0)?64:128, origin+Vector(-96,-96,0), Vector(32,32,32), Vector(164,164,164), Vector(0,0,4096), g_iModelIndexAnimSpr12, NULL, NULL, kRenderTransAdd, 0.2, 5, 0.0f, 0.0, 0.1), RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
		}
		break;

		case FX_TELEPORT_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 600, 80, 40, g_iModelIndexBeamsAll, BLAST_SKIN_DISPLACER, kRenderTransAdd, 255,255,255, 1, -1, 0.8));
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_teleport.wav", 1, ATTN_LOW, 0, 100);
			DynamicLight(origin, RANDOM_FLOAT(550,600), 65,250,85, 0.7, 200.0);

				if (!UTIL_PointIsFar(origin, 1.0))
				{
					gEngfuncs.pEfxAPI->R_TempSprite(origin, (float *)&g_vecZero, 1.0f, g_iModelIndexAnimSpr12, kRenderGlow, kRenderFxNoDissipation, 1.0f, 0.4f, FTENT_FADEOUT);
					g_pRenderManager->AddSystem(new CPSSparks(64, origin, 1.0, 0.02, 0, 600, 1.5, 200,200,200, RANDOM_FLOAT(1,2), 0, g_iModelIndexAnimSpr12, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				}
			}
		break;

		case FX_TELEPORT_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 120, origin, origin, Vector(1,1,1), 12, 90, 0,128,0, 0.7, -0.7, g_iModelIndexPartRed, FALSE, PARTICLE_RED_37, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 150, origin, origin, Vector(1,1,1), 15, 120, 0,128,0, 0.8, -0.8, g_iModelIndexPartRed, FALSE, PARTICLE_RED_39, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_teleport.wav", 1, ATTN_LOW_HIGH, 0, 90);
			DynamicLight(origin, RANDOM_FLOAT(350,500), 65,250,85, 0.7, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 125, origin, origin, Vector(1,1,1), 15, 100, 0,200,0, 0.8, -0.8, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
 				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,5), RANDOM_FLOAT(1.0,1.6), RANDOM_FLOAT(300, 350), SPARKSHOWER_GREENENERGY, origin, Vector(0,0,1), VECTOR_CONE_LOTS, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSSparks(40, origin, 1.0, 0.01, 0, 600, 1, 200,200,200, RANDOM_FLOAT(1,2), 0, g_iModelIndexAnimSpr12, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			}	
		}
		break;

		case FX_TELEPORT_ENTER_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 1000, 40, 50, g_iModelIndexBeamsAll, BLAST_SKIN_TELEENTER, kRenderTransAdd, 255,255,255, 1, -1, 0.6));
			g_pRenderManager->AddSystem(new CPSBlastCone(60, 200, origin, origin, Vector(1,1,1), 20, 100, 0,150,80, 0.8, -1, g_iModelIndexExplosion5, FALSE, 0, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(48, 100, origin, Vector(0,0,1), Vector(3,3,1), 20, 100, 255,255,255, 0.6, -1, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_6, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				gEngfuncs.pEventAPI->EV_PlaySound( 0, origin, CHAN_VOICE, "weapons/explode_teleport.wav", 1, ATTN_NORM, 0, 150 );
				DynamicLight(origin, 300, 65,250,80, 0.6, 150.0);
			}
		}
		break;

		case FX_TELEPORT_COLLIDE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 750, 80, 20, g_iModelIndexBeamsAll, BLAST_SKIN_PSPBEAM, kRenderTransAdd, 255,255,255, 1, -1, 0.8));
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_teleport.wav", 1, ATTN_LOW, 0, 110);
			DynamicLight(origin, RANDOM_FLOAT(550,600), 65,250,85, 0.7, 200.0);

			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_NUKE_MUSHROOM, BLAST_SKIN_SPHERE_SHIELD, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.75, -1.5, RANDOM_FLOAT(0.03,0.05), RANDOM_FLOAT(3,5), 0, 0.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(33,0,0), g_iModelIndexPartGreen, kRenderTransAdd, 128,128,128, 1.0, -1.0, 1, 10, PARTICLE_GREEN_4, 0, 0.0));

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexAnimSpr12, 0, 1, 75, kRenderTransAdd, 255,255,255, 1.0, -0.75, 100, 300, 3.0), 0, -1, RENDERSYSTEM_FFLAG_ICNF_STAYANDFORGET);
				g_pRenderManager->AddSystem(new CPSSparks(40, origin, 1.0, 0.01, 0, 800, 1, 200,200,200, RANDOM_FLOAT(1,2), 0, g_iModelIndexAnimSpr12, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_LIGHTNINGBALL:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,0,200, 300, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr7, kRenderTransAdd, 255,255,255, 1.0, 0.0, 0.8, 0.0, 24.0, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSSparkShower(12, 0.05, 0, SPARKSHOWER_LIGHTNING_STRIKE, origin, origin, Vector(0,0,0), g_iModelIndexPartRed, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE); 
		}
		break;

		case FX_LIGHTNINGBALL_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexLightningFieldModel, LGTNTG_FIELD_BALL, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -2.0, 0.05, 35, 0, 0.25), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexLightningFieldModel, LGTNTG_FIELD_LIGHTNING_BLAST, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -2.0, 8.0, -14.0, 0, 0.2), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 100, origin, origin, Vector(1,1,1), 5, 120, 128,128,128, 0.25, -0.2, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_5, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(340,370), 0,90,250, 0.5, 250.0);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_lightningball.wav", 1, ATTN_LOW_HIGH, 0, 100);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(4,8), RANDOM_FLOAT(1.5,2.2), RANDOM_FLOAT(220, 300), SPARKSHOWER_SPARKS, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
				g_pRenderManager->AddSystem(new CPSSparks(28, origin, 0.4, 0.03, 0, RANDOM_FLOAT(300,400), RANDOM_FLOAT(1,2), 200,200,200, 1, -0.1, g_iModelIndexAnimSpr7, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				g_pRenderManager->AddSystem(new CPSSparks(28, origin, 0.3, 0.02, 0, RANDOM_FLOAT(700,850), RANDOM_FLOAT(1,2), 200,200,200, 1, -0.1, g_iModelIndexAnimSpr3, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
		
				for (int i = 0; i <6; i++)
				{
					dir.x=RANDOM_FLOAT(-1,1);
					dir.y=RANDOM_FLOAT(-1,1);
					dir.z=RANDOM_FLOAT(-1,1);
					dir=dir.Normalize();
					gEngfuncs.pEfxAPI->R_BeamPoints( origin, origin+(dir*RANDOM_FLOAT(164,512)), g_iModelIndexBeamsAll, RANDOM_FLOAT(0.6,1.2), RANDOM_FLOAT(1.0,1.8), 2.5, 40, 20, BLAST_SKIN_LIGHTNING, 0, 255, 255, 255);
					gEngfuncs.pEfxAPI->R_BeamPoints( origin, origin+(dir*RANDOM_FLOAT(164,512)), g_iModelIndexBeamsAll, RANDOM_FLOAT(0.6,1.2), RANDOM_FLOAT(0.75,1.5), 2.5, 40, 20, BLAST_SKIN_FROSTGRENADE, 0, 255, 255, 255);
				}
			}
		}
		break;

		case FX_LIGHTNINGBALL_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexLightningFieldModel, LGTNTG_FIELD_BALL, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -2.0, 0.05, 25, 0, 0.25), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 100, origin, origin, Vector(1,1,1), 5, 120, 128,128,128, 0.25, -0.2, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_5, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(200,220), 0,90,250, 0.2, 250.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_lightningball.wav", 1, ATTN_NORM, 0, 100);
				g_pRenderManager->AddSystem(new CPSSparks(30, origin, 0.3, 0.02, 0, RANDOM_FLOAT(700,850), RANDOM_FLOAT(1,2), 200,200,200, 1, -0.1, g_iModelIndexAnimSpr3, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
		}
		break;

		case FX_PULSE:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 100,190,250, 200, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_PULSE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			gEngfuncs.pEfxAPI->R_TempSprite(origin, (float *)&g_vecZero, 0.3f, g_iModelIndexAnimSpr3, kRenderGlow, kRenderFxNoDissipation, 0.6f, 0.05f, FTENT_FADEOUT);
			DynamicLight(origin, 220, 100,190,250, 0.2, 80.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(1, 0, origin, origin, Vector(1,1,1), 10, 25, 128,128,128, 0.8, -0.5, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				switch (RANDOM_LONG(0,2))
				{
					case 0:	EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					case 1:	EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/electro5.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					case 2:	EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 120 ); break;
				}
			}
		}
		break;

		case FX_PULSE_BLAST:
		{
			FX_RemoveRS(EntIndex);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(24,32), origin, RANDOM_FLOAT(0.6,0.8), RANDOM_FLOAT(0.015,0.025), 0, RANDOM_FLOAT(180,220), RANDOM_FLOAT(1.5,2.0), 255,255,255, 1, 0, g_iModelIndexAnimSpr3, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				switch (RANDOM_LONG(0,2))
				{
					case 0:	EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					case 1:	EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/electro5.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					case 2:	EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/electro6.wav", 1.0, ATTN_NORM, 0, 120 ); break;
				}
			}
		}
		break;

		case FX_PLASMA:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,190,0, 220, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			if (g_pCvarTrailRS->value < 1.0f)
			{
				pBeamTrail = gEngfuncs.pEfxAPI->R_BeamFollow(EntIndex, g_iModelIndexBeamsAll, 0.6, 4, 255,255,255, 0.2);
				if (pBeamTrail)
				{
					pBeamTrail->frame = BLAST_SKIN_ENERGYBEAM;
					pBeamTrail->frameRate = 0;
				}
			}
			else
				FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexBeamsAll, BLAST_SKIN_ENERGYBEAM, 0.6, -1.0, 8.0, -8.0, 0.3, 5.0);
		}
		break;

		case FX_PLASMA_BLAST:
		{
			if (g_pCvarTrailRS->value < 1.0f)
				gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexExplosion2, kRenderTransAdd, 255,255,255, 0.9, 0.0, 1.0, 1.0, 15.0, 0));
			DynamicLight(origin, 130, 0,190,20, 0.5, 120.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 25, origin, origin, Vector(1,1,1), 10, 35, 0,255,40, 0.75, -0.75, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_plasma.wav", 1, ATTN_NORM, 0, 80 );
			}
		}
		break;

		case FX_PLASMASTORM:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(196, 0, origin, origin, Vector(0,0,0), 15, 0, 255,255,255, 0.5, -1.0, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_4, kRenderTransAdd, 7), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 0,190,0, 250, NULL, 0.0, 5, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			CRSSprite *pSys = new CRSSprite(origin, g_vecZero, g_iModelIndexPartGreen, kRenderTransAdd, 255,255,255, 0.9f, 0.0f, 0.6, 0.0f, 0.0f, 7);
			if (pSys)
			{
				g_pRenderManager->AddSystem(pSys, 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
				pSys->m_iFrame = PARTICLE_GREEN_7;
			}
		}
		break;

		case FX_PLASMASTORM_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexPartGreen, PARTICLE_GREEN_5, 0, UTIL_PointIsFar(origin, 1.0)?32:64, kRenderTransAdd, 255,255,255, 0.6, -0.3, 120, -8, 2.2));
			DynamicLight(origin, RANDOM_FLOAT(300,330), 0,190,20, 1.5, 150.0);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_plasma_storm.wav", 1, ATTN_LOW_HIGH, 0, PITCH_NORM );

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparks(64, origin, 0.7, 0.01, 0, 250, 1.5, 255,255,255, 1, 0, g_iModelIndexAnimSpr12, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(190, 240), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(1,3), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(190, 240), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.25, g_iModelIndexMuzzleFlash2, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSSpawnEffect(64, origin, RANDOM_FLOAT(8,12), -1.0, 320, -5.0, g_iModelIndexAnimSpr8, kRenderTransAdd, 0,200,0, 1.0, -0.33, 3));
			}
		}
		break;

		case FX_PLASMASTORM_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexPartGreen, PARTICLE_GREEN_5, 0, UTIL_PointIsFar(origin, 1.0)?24:48, kRenderTransAdd, 255,255,255, 0.6, -0.6, 60, -4, 1.5));
			DynamicLight(origin, RANDOM_FLOAT(270,300), 0,190,20, 0.8, 150.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_plasma_storm.wav", 1, ATTN_NORM, 0, PITCH_NORM );
				g_pRenderManager->AddSystem(new CPSSparks(40, origin, 0.7, 0.01, 0, 250, 1.5, 255,255,255, 1, 0, g_iModelIndexAnimSpr12, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_PLASMASTORM_COLLIDE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexPartGreen, PARTICLE_GREEN_5, 0, UTIL_PointIsFar(origin, 1.0)?48:96, kRenderTransAdd, 255,255,255, 0.8, -1.2, 20, 500, 0.5));
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SPHERE, BLAST_SKIN_SPHERE_SIELD_PLASMA, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.5, -0.75, 0.05, 4, 0, 0.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			DynamicLight(origin, RANDOM_FLOAT(450,530), 0,190,20, 0.75, 150.0);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_plasma_storm.wav", 1, ATTN_LOW_HIGH, 0, PITCH_NORM );

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(200, 250), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(1,3), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(220, 260), origin, Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.25, g_iModelIndexMuzzleFlash2, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
		}
		break;

		case FX_SHOCKWAVE_DETONATE:
		{
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 600, 80, 40, g_iModelIndexBeamsAll, BLAST_SKIN_LIGHTNING, kRenderTransAdd, 255,255,255, 1, -1, 0.8));
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_shockwave.wav", 1, ATTN_LOW, 0, 100);
			DynamicLight(origin, 600, 0,100,250, 0.7, 200.0);
		}
		break;

		case FX_PLAYER_SPAWN_RINGTELEPORT:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr4, kRenderTransAdd, 255,255,255, 0.8, 0.0, 0.6, -0.3, 20.0, 2));
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CRSBeamStar(origin, g_iModelIndexAnimSpr7, 0, 1, 64, kRenderTransAdd, 255,255,255, 1, 0, 120, -60, 1.8), 0, -1, 0);
				EMIT_SOUND(EntIndex, origin, CHAN_WEAPON, "player/respawn.wav", 1, ATTN_NORM, 0, 100);
			}
		}
		break;

		case FX_PLAYER_SPAWN_RINGTELEPORT_PART1:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 125, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, 5, 120, 150,100,205, 0.4, -0.3, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 100, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, 5, 75, 160,0,217, 0.3, -0.4, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_4, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 150, origin, origin, Vector(1,1,1), 5, 125, 255,255,255, 0.3, -0.4, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_2, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, 80, origin, Vector(0,0,1), Vector(3,3,1), 3, 0, 255,255,255, 0.8, -0.8, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				DynamicLight(origin, 200, 160,0,220, 0.5, 80.0);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_lightningball.wav", 1, ATTN_NORM, 0, 100 );
			}
		}
		break;

		case FX_PLAYER_SPAWN_RINGTELEPORT_PART2:
		{
			//animation does not work!!! So right now this effect is on server
	//		g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexTeleportModel, 0, 0, 1, kRenderTransAdd, kRenderFxNone, 255,255,255, 0.8, 0.0, 1.0, 0, 75, 5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			gEngfuncs.pEfxAPI->R_TempSprite(origin - Vector(0,0,16), (float *)&g_vecZero, 1.5f, g_iModelIndexAnimSpr7, kRenderGlow, kRenderFxNoDissipation, 0.4f, 3.0f, FTENT_FADEOUT|FTENT_SPRANIMATELOOP|FTENT_SPRANIMATE);
		
			if (!UTIL_PointIsFar(origin, 1.0))
				g_pRenderManager->AddSystem(new CRSLight(origin, 0,0,255, 192, NULL, 0.0, 3.0, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_STAYANDFORGET);
		}
		break;

		case FX_PLAYER_SPAWN:
		{
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexAnimSpr4, kRenderTransAdd, 255,255,255, 0.8, 0.0, 0.6, -0.3, 20.0, 2));
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSpawnEffect(64, origin, 8, -2.0, 32, -4.0, g_iModelIndexAnimSpr6, kRenderTransAdd, 200,200,200, 1.0, -0.5, 2));
				g_pRenderManager->AddSystem(new CPSSparks(64, origin, RANDOM_FLOAT(0.5, 0.8), RANDOM_FLOAT(0.03, 0.03), 0, -350, 1.3, 200,200,200, 1, 0, g_iModelIndexAnimSpr5, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				EMIT_SOUND(EntIndex, origin, CHAN_WEAPON, "player/respawn.wav", 1, ATTN_NORM, 0, 100);
			}
		}
		break;

		case FX_PLAYER_LONGJUMP:
		{
			gEngfuncs.pEfxAPI->R_FlickerParticles(origin);

			if (!UTIL_PointIsFar(origin, 1.0))
				EMIT_SOUND(EntIndex, origin, CHAN_BODY, "player/pl_jump_super.wav", 0.75, ATTN_NORM, 0, 98 + RANDOM_LONG(0,4));
		}
		break;

		case FX_TRIPMINE_DETONATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(32, 420, origin, origin, Vector(1,1,1), 10, 170, 255,115,0, 0.7, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(48, 330, origin, origin, Vector(1,1,1), 5, 150, 190,130,0, 0.8, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, 250, origin, origin, Vector(1,1,1), 10, 110, 255,255,255, 0.5, -0.6, g_iModelIndexPartRed, FALSE, PARTICLE_RED_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
 			
			DynamicLight(origin, RANDOM_FLOAT(360,400), 128,128,40, 0.6, 125.0);
			switch (gEngfuncs.pfnRandomLong(0,2))
			{
				case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode3.wav", 1, ATTN_LOW_HIGH, 0, 120 );break;
				case 1 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode4.wav", 1, ATTN_LOW_HIGH, 0, 120 );break;
				case 2 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode5.wav", 1, ATTN_LOW_HIGH, 0, 120 );break;
			}

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,4), RANDOM_FLOAT(1.2,2.2), RANDOM_FLOAT(330, 390), SPARKSHOWER_FIREEXP, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 50, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 15, 35, 0,0,0, 0.2, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(48, origin, 5, 0.01, 0, 500, 2, 255, 255, 255, 1, 0, g_iModelIndexAnimSpr8, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_SPIDERMINE_DETONATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 380, origin, Vector(0,0,1), VECTOR_CONE_LOTS, 10, 170, 255,115,0, 0.7, -0.8, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_9, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 330, origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, 100, 255,255,255, 0.8, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_19, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 250, origin, Vector(0,0,1), VECTOR_CONE_LOTS, 10, 110, 255,255,255, 0.5, -0.6, g_iModelIndexPartRed, FALSE, PARTICLE_RED_17, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
 			
			DynamicLight(origin, RANDOM_FLOAT(360,400), 128,128,40, 0.6, 125.0);
			switch (gEngfuncs.pfnRandomLong(0,2))
			{
				case 0 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode1.wav", 1, ATTN_LOW_HIGH, 0, 100 );break;
				case 1 : EMIT_SOUND(EntIndex, origin, CHAN_WEAPON, "weapons/explode2.wav", 1, ATTN_LOW_HIGH, 0, 100 );break;
				case 2 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode4.wav", 1, ATTN_LOW_HIGH, 0, 100 );break;
			}

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				// gibs
				for (int j = 0; j < 6; j++)		
					FX_TempModel(origin, RANDOM_FLOAT(220, 340), true, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_NORMAL, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_FIRESMOKE);

				switch (gEngfuncs.pfnRandomLong(0,1))
				{
					case 0 : g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,4), RANDOM_FLOAT(1.2,2.2), RANDOM_FLOAT(400, 440), SPARKSHOWER_LAVA_FLAME, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1); break;
					case 1 : g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,4), RANDOM_FLOAT(1.2,2.2), RANDOM_FLOAT(400, 440), SPARKSHOWER_STREAKS, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1); break;
				}
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 50, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, 20, 100, 0,0,0, 0.3, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_5, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CRSCylinder(origin+Vector(0,0,24), 30, 600, 12, 40, g_iModelIndexBeamsAll, BLAST_SKIN_GAUSSBEAM, kRenderTransAdd, 255,255,255, 1, -0.33, 0.3));
			}
		}
		break;

		case FX_DEMOLITIONMISSILE:
		{
			if (g_pCvarTrailRS->value < 1.0f)
			{
				pBeamTrail = gEngfuncs.pEfxAPI->R_BeamFollow(EntIndex, g_iModelIndexBeamsAll, 1.2, 4, 255,255,255, 0.8);
				if (pBeamTrail)
				{
					pBeamTrail->frame = BLAST_SKIN_SHOCKWAVE;
					pBeamTrail->frameRate = 0;
				}
			}
			else
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(128, 10, origin, origin, VECTOR_CONE_20DEGREES, 18, -15, 255,255,255, 0.7, -2.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_22, kRenderTransAdd, 7), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
				FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexBeamsAll, BLAST_SKIN_SHOCKWAVE, 0.5, -1.5, 8.0, 32.0, 1.0, 10.0);
			}
			g_pRenderManager->AddSystem(new CRSLight(origin, 240,180,0, 220, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 10, origin, origin, VECTOR_CONE_20DEGREES, 18, 55, 128,128,128, 1.0, -1.25, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_8, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			CRSSprite *pSys = new CRSSprite(origin, g_vecZero, g_iModelIndexPartRed, kRenderTransAdd, 255,255,255, 0.5f,0.0f, 0.4, 0.0f, 0.0f, 10);
			if (pSys)
			{
				g_pRenderManager->AddSystem(pSys, 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
				pSys->m_iFrame = PARTICLE_RED_22;
			}
		}
		break;

		case FX_DEMOLITIONMISSILE_DETONATE:
		{
			gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, 320, origin+Vector(0,0,16), origin+Vector(0,0,16), Vector(1,1,1), 10, 170, 255,115,0, 0.7, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(40, 230, origin+Vector(0,0,16), origin+Vector(0,0,16), Vector(1,1,1), 5, 150, 190,130,0, 0.8, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSSparks(32, origin+Vector(0,0,16), 5, 0.01, 0, 400, 1, 255,255,255, 1,0, g_iModelIndexAnimSpr8, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin+Vector(0,0,16), RANDOM_FLOAT(300,330), 128,128,40, 0.8, 100.0);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SPHERE, BLAST_SKIN_FIRE_SPARKS, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.5, -2.0, 0.01, 10, 0, 0.2), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_demolitionmissile.wav", 1, ATTN_LOW_HIGH, 0, 100);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparks(32, origin+Vector(0,0,16), 1.0f, 0.05f, 0.25f, 320.0f, 2.2f, 255,255,255,1.0f, -0.6f, g_iModelIndexAnimSpr8, kRenderTransAdd, 0.5f), RENDERSYSTEM_FLAG_RANDOMFRAME|RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_ADDPHYSICS|RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				ParticlesCustom(origin, 160, 160, 16, 256, 0.8f);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,4), RANDOM_FLOAT(1.2,2.0), RANDOM_FLOAT(330, 390), SPARKSHOWER_SPARKS, origin+Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,4), RANDOM_FLOAT(1.2,2.0), RANDOM_FLOAT(330, 390), SPARKSHOWER_SPARKS2, origin+Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 50, origin+Vector(0,0,16), Vector(0,0,1), Vector(0.2,0.2,0.5), 15, 35, 0,0,0, 0.2, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, 250, origin+Vector(0,0,16), origin+Vector(0,0,16), Vector(1,1,1), 10, 110, 255,255,255, 0.5, -0.6, g_iModelIndexPartRed, FALSE, PARTICLE_RED_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
 			}
		}
		break;

		case FX_DEMOLITIONMISSILE_BLAST:
		{
			gEngfuncs.pEfxAPI->R_BeamKill(EntIndex);
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 260, origin, origin, Vector(1,1,1), 10, 170, 255,115,0, 0.7, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, 200, origin, origin, Vector(1,1,1), 5, 150, 190,130,0, 0.8, -1, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(250,300), 128,128,40, 0.6, 100.0);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SPHERE, BLAST_SKIN_FIRE_SPARKS, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.5, -2.0, 0.01, 6, 0, 0.2), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_demolitionmissile.wav", 1, ATTN_NORM, 0, 100);
				ParticlesCustom(origin, 160, 160, 16, 256, 0.8f);
 				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,4), RANDOM_FLOAT(0.8,1.2), RANDOM_FLOAT(230, 290), SPARKSHOWER_SPARKS, origin, Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 200, origin, origin, Vector(1,1,1), 10, 110, 255,255,255, 0.5, -0.6, g_iModelIndexPartRed, FALSE, PARTICLE_RED_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
 			}
		}
		break;

		case FX_HVRMISSILE:
		{
			g_pRenderManager->AddSystem(new CRSLight(origin, 196,156,13, 260, NULL, 0.0, 20, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 10, origin, origin, VECTOR_CONE_20DEGREES, 12, RANDOM_FLOAT(42,55), 128,128,128, 0.6, -0.3, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_1, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 10, origin, origin, VECTOR_CONE_20DEGREES, 30, RANDOM_FLOAT(30,40), 128,128,128, 1.0, -3.5, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			CRSSprite *pSys = new CRSSprite(origin, g_vecZero, g_iModelIndexPartRed, kRenderTransAdd, 255,255,255, 0.8f,0.0f, 0.5, 0.0f, 0.0f, 10);
			if (pSys)
			{
				g_pRenderManager->AddSystem(pSys, 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
				pSys->m_iFrame = PARTICLE_RED_7;
			}
		}
		break;

		case FX_HVRMISSILE_DETONATE:
		{
			FX_RemoveRS(EntIndex);			
			g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(250,275), origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, RANDOM_FLOAT(175,200), 128,128,128, 0.5, -0.6, g_iModelIndexPartRed, FALSE, PARTICLE_RED_8, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(30,40), origin, Vector(1,0,0), Vector(5,5,1), 5, RANDOM_FLOAT(85,100), 255,150,0, 0.5, -0.5, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(28, RANDOM_FLOAT(30,40), origin, Vector(0,1,0), Vector(5,5,1), 5, RANDOM_FLOAT(90,105), 128,128,128, 0.4, -0.4, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(270,300), 196,156,13, 0.8, 150.0);
 			switch (RANDOM_LONG(0,2))
			{
				case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode5.wav", 1.0, ATTN_LOW_HIGH, 0, 110 ); break;
				case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode3.wav", 1.0, ATTN_LOW_HIGH, 0, 110 ); break;
				case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode1.wav", 1.0, ATTN_LOW_HIGH, 0, 110 ); break;
			}
				
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(240,280), origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, RANDOM_FLOAT(175,200), 0,0,0, 0.2, -0.15, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_9, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CRenderSystem(origin, Vector(0,0,0), Vector(30,0,0), g_iModelIndexAnimSpr3, kRenderTransAdd, 196,156,15, 1.0, -0.5, 2, 10, 0, 20, 0.0));
				g_pRenderManager->AddSystem(new CPSSparks(32, origin, RANDOM_FLOAT(0.8,1.1), RANDOM_FLOAT(0.015,0.03), 0, RANDOM_FLOAT(230,260), RANDOM_FLOAT(1,1.5), 255,255,255, 1, 0, g_iModelIndexAnimSpr8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
		}
		break;

		case FX_HVRMISSILE_BLAST:
		{
			FX_RemoveRS(EntIndex);			
			g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(210,235), origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, RANDOM_FLOAT(175,200), 128,128,128, 0.5, -0.6, g_iModelIndexPartRed, FALSE, PARTICLE_RED_8, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(28, RANDOM_FLOAT(30,40), origin, Vector(0,1,0), Vector(3,3,1), 5, RANDOM_FLOAT(90,105), 128,128,128, 0.4, -0.4, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, RANDOM_FLOAT(230,260), 196,156,13, 0.8, 150.0);
				
			if (!UTIL_PointIsFar(origin, 1.0))
			{
 				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode5.wav", 1.0, ATTN_NORM, 0, 110 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode3.wav", 1.0, ATTN_NORM, 0, 110 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode1.wav", 1.0, ATTN_NORM, 0, 110 ); break;
				}
				g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(200,230), origin, Vector(0,0,1), VECTOR_CONE_LOTS, 5, RANDOM_FLOAT(175,200), 0,0,0, 0.2, -0.15, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_9, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case FX_MICROMISSILE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(256, 10, origin, origin, VECTOR_CONE_20DEGREES, 6, 45, 220,110,0, 1, -1.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_3, kRenderTransAlpha, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(256, 10, origin, origin, VECTOR_CONE_20DEGREES, 4, 0, 255,255,255, 1, -2.2, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			g_pRenderManager->AddSystem(new CRSLight(origin, 240,240,200, 220, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			CRSSprite *pSys = new CRSSprite(origin, g_vecZero, g_iModelIndexPartRed, kRenderTransAdd, 255,255,255, 0.8f,0.0f, 0.35, 0.0f, 0.0f, 10);
			if (pSys)
			{
				g_pRenderManager->AddSystem(pSys, 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
				pSys->m_iFrame = PARTICLE_RED_29;
			}
		}
		break;

		case FX_MICROMISSILE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 220, origin, origin, Vector(1,1,1), 5, 80, 255,115,0, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_9, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 150, origin, origin, Vector(1,1,1), 5, 50, 220,100,0, 0.8, -0.7, g_iModelIndexPartRed, FALSE, PARTICLE_RED_21, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 220, origin, origin, Vector(1,1,1), 5, 80, 255,115,0, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_30, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, 220, 128,140,0, 0.6, 70.0);

			switch (RANDOM_LONG(0,2))
			{
				case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_small1.wav", 1.0, ATTN_NORM, 0, 100 ); break;
				case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_small2.wav", 1.0, ATTN_NORM, 0, 100 ); break;
				case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_small3.wav", 1.0, ATTN_NORM, 0, 100 ); break;
			}

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 150, origin, origin, Vector(1,1,1), 5, 50, 220,100,0, 0.8, -0.7, g_iModelIndexPartRed, FALSE, PARTICLE_RED_32, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 70, origin, origin, Vector(1,1,1), 5, 40, 128,128,128, 1, -0.9, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 40, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 35, 0,0,0, 0.2, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_2, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(24, origin, 1.3f, 0.04f, 0.0f, 100.0f, 1.2f, 255,255,255,1.0f, -0.6f, g_iModelIndexPartViolet, kRenderTransAdd, 0.8f), RENDERSYSTEM_FLAG_RANDOMFRAME|RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
			}
		}
		break;

		case FX_MICROMISSILE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 180, origin, origin, Vector(1,1,1), 5, 80, 255,115,0, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_9, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 190, origin, origin, Vector(1,1,1), 5, 80, 255,115,0, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_30, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, 190, 128,140,0, 0.6, 70.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_small1.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_small2.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_small3.wav", 1.0, ATTN_NORM, 0, 100 ); break;
				}
				g_pRenderManager->AddSystem(new CPSBlastCone(16, 130, origin, origin, Vector(1,1,1), 5, 50, 220,100,0, 0.8, -0.7, g_iModelIndexPartRed, FALSE, PARTICLE_RED_32, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(12, 70, origin, origin, Vector(1,1,1), 5, 40, 128,128,128, 1, -0.9, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case FX_GUIDED_BOMB:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(164, 10, origin, origin, VECTOR_CONE_40DEGREES, 5, 2, 255,255,255, 1, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_3, kRenderTransAdd, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(192, 10, origin, origin, VECTOR_CONE_20DEGREES, 8, 2, 255,255,255, 1, -0.7, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(96, 20, origin, origin, Vector(0,0,0), 10, 40, 100,100,100, 0.8, -0.8, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_9, kRenderTransAdd, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexMuzzleFlash2, kRenderTransAdd, 255,255,255, 0.8, 0, 0.5, 0.0, 24.0, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 240,240,200, 300, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_GUIDED_BOMB_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 250, origin, origin, Vector(1,1,1), 10, 100, 255,255,255, 0.8, -1, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(20, 180, origin, origin, Vector(1,1,1), 10, 75, 255,255,255, 0.9, -0.9, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 700, 15, 50, g_iModelIndexBeamsAll, BLAST_SKIN_GAUSSBEAM, kRenderTransAdd, 255,255,255, 1, -1, 0.3));
			DynamicLight(origin, 320, 128,140,0, 0.8, 100.0);
			switch (RANDOM_LONG(0,2))
			{
				case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode3.wav", 1.0, ATTN_LOW, 0, 100 ); break;
				case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode4.wav", 1.0, ATTN_LOW, 0, 100 ); break;
				case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode5.wav", 1.0, ATTN_LOW, 0, 100 ); break;
			}
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(48, 200, origin, origin, Vector(1,1,1), 5, 2, 255,255,255, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 120, origin, origin, Vector(1,1,1), 12, 90, 255,255,255, 0.9, -0.9, g_iModelIndexExplosion3, FALSE, 3, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparks(24, origin, 0.5f, 0.025f, 0.0f, 160.0f, 0.8f, 255,255,255,1.0f, -0.6f, g_iModelIndexAnimSpr9, kRenderTransAdd, 1.5f), RENDERSYSTEM_FLAG_RANDOMFRAME|RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_ADDPHYSICS|RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 40, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 10, 45, 0,0,0, 0.2, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				CRSSprite *pSys = new CRSSprite(origin, g_vecZero, g_iModelIndexPartBlue, kRenderTransAdd, 255,255,255, 0.6f, -0.8f, 1, 2, 0.0f, 0);
				if (pSys)
				{
					g_pRenderManager->AddSystem(pSys, 0, -1, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
					pSys->m_iFrame = PARTICLE_BLUE_6;
				}
			}
		}
		break;

		case FX_GUIDED_BOMB_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 220, origin, origin, Vector(1,1,1), 10, 100, 255,255,255, 0.8, -1, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, 150, origin, origin, Vector(1,1,1), 10, 75, 255,255,255, 0.9, -0.9, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRSCylinder(origin, 40, 700, 15, 50, g_iModelIndexBeamsAll, BLAST_SKIN_GAUSSBEAM, kRenderTransAdd, 255,255,255, 1, -1, 0.3));
			DynamicLight(origin, 270, 128,140,0, 0.6, 100.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode3.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode4.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode5.wav", 1.0, ATTN_NORM, 0, 100 ); break;
				}

				g_pRenderManager->AddSystem(new CPSBlastCone(32, 175, origin, origin, Vector(1,1,1), 5, 2, 255,255,255, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				CRSSprite *pSys = new CRSSprite(origin, g_vecZero, g_iModelIndexPartBlue, kRenderTransAdd, 255,255,255, 0.6f, -0.8f, 1, 2, 0.0f, 0);
				if (pSys)
				{
					g_pRenderManager->AddSystem(pSys, 0, -1, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
					pSys->m_iFrame = PARTICLE_BLUE_6;
				}
			}
		}
		break;

		case FX_GUIDED_SHARD:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(128, 25, origin, origin, Vector(0,0,0), 10, 35, 100,100,100, 0.8, -0.8, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_8, kRenderTransAdd, 10), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 240,240,200, 200, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexMuzzleFlash1, kRenderTransAdd, 255,255,255, 0.8, 0, 0.4, 0.0, 24.0, 10.0), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;
	
		case FX_GUIDED_SHARD_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSSprite(origin, g_vecZero, g_iModelIndexExplosion0, kRenderTransAdd, 255,255,255, 1, -1.1, 1, 3, 25.0f, 0));
			DynamicLight(origin, 250, 128,140,0, 0.6, 100.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(40, 200, origin, origin, Vector(1,1,1), 5, 2, 255,255,255, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(4, 40, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 10, 35, 0,0,0, 0.2, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_5, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_small1.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_small2.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_small3.wav", 1.0, ATTN_NORM, 0, 100 ); break;
				}
			}
		}
		break;

		case FX_GUIDED_SHARD_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSSprite(origin, g_vecZero, g_iModelIndexExplosion0, kRenderTransAdd, 255,255,255, 1, -1.1, 1, 2, 25.0f, 0));
			DynamicLight(origin, 220, 128,140,0, 0.5, 100.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(24, 170, origin, origin, Vector(1,1,1), 5, 2, 255,255,255, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_small1.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_BODY, "weapons/explode_small2.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 2 : EMIT_SOUND(EntIndex, origin, CHAN_ITEM, "weapons/explode_small3.wav", 1.0, ATTN_NORM, 0, 100 ); break;
				}
			}
		}
		break;

		case FX_GHOSTMISSILE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(150, 25, origin, origin, Vector(0,0,0), 10, 40, 0,0,0, 0.8, -0.5, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_3, kRenderTransAlpha, 7), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(150, 20, origin, origin, Vector(0,0,0), 10, 40, 200,200,200, 0.8, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_3, kRenderTransAdd, 7), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CRSLight(origin, 220,150,0, 300, NULL, 0.0, 10, false), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
			g_pRenderManager->AddSystem(new CPSBlastCone(164, 10, origin, origin, VECTOR_CONE_20DEGREES, 25, -15, 255,255,255, 1, -2.5, g_iModelIndexPartRed, FALSE, PARTICLE_RED_4, kRenderTransAdd, 7), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_GHOSTMISSILE_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(28, 300, origin, origin, Vector(1,1,1), 10, 110, 128,128,128, 0.8, -0.75, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(28, 350, origin, origin, Vector(1,1,1), 10, 150, 128,128,128, 0.6, -0.55, g_iModelIndexAnimSpr10, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 400, origin, origin, Vector(1,1,1), 15, 5, 128,128,128, 0.5, -0.45, g_iModelIndexMuzzleFlash3, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 450, origin, origin, Vector(1,1,1), 20, 5, 255,255,255, 0.5, -0.45, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, 350, 220,150,0, 0.7, 200.0);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_ghostmissile.wav", 1, ATTN_LOW_HIGH, 0, 100 );

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(8, 65, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 60, 0,0,0, 0.8, -0.3, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_3, kRenderTransAlpha, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,4), RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(320, 420), SPARKSHOWER_FIREEXP, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,4), RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(320, 450), SPARKSHOWER_SPARKS, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
			}
		}
		break;

		case FX_GHOSTMISSILE_BLAST:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(28, 230, origin, origin, Vector(1,1,1), 10, 110, 128,128,128, 0.8, -0.75, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(24, 300, origin, origin, Vector(1,1,1), 15, 5, 128,128,128, 0.5, -0.45, g_iModelIndexMuzzleFlash3, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			DynamicLight(origin, 300, 220,150,0, 0.5, 200.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_ghostmissile.wav", 1, ATTN_NORM, 0, 100 );
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,3), RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(320, 450), SPARKSHOWER_SPARKS, origin, Vector(0,0,1), Vector(0.2,0.2,0.5), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
			}
		}
		break;

		case FX_WARPBALL_EFFECT:
		{
			DynamicLight(origin, 300, 0,255,0, 3.0f, 20.0f);

			TEMPENTITY *pSpr = gEngfuncs.pEfxAPI->R_TempSprite(origin, (float *)&g_vecZero, 1.0f, g_iModelIndexAnimSpr12, kRenderGlow, kRenderFxNoDissipation, 1.0, 3.0, FTENT_SPRANIMATE);
			if (pSpr)
			{
				pSpr->entity.baseline.rendercolor.r = pSpr->entity.curstate.rendercolor.r = 64;
				pSpr->entity.baseline.rendercolor.g = pSpr->entity.curstate.rendercolor.g = 255;
				pSpr->entity.baseline.rendercolor.b = pSpr->entity.curstate.rendercolor.b = 64;
			}
			ParticlesCustom(origin, 250, BLOOD_COLOR_GREEN, 8,160, 2.0, true);
		}
		break;

		case FX_RIPPER:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(96, 0, origin, origin, Vector(0,0,0), 15, 0, 255,255,255, 0.3, -1.2, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_1, kRenderTransAdd, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);

			if (g_pCvarTrailRS->value < 1.0f)
			{
				pBeamTrail = gEngfuncs.pEfxAPI->R_BeamFollow(EntIndex, g_iModelIndexBeamsAll, 0.15, 1.8, 255,255,255, 0.4);
				if (pBeamTrail)
				{
					pBeamTrail->frame = BLAST_SKIN_FIREBEAM;
					pBeamTrail->frameRate = 0;
				}
			}
			else
				FX_SmokeTrail(origin, FX_SMOKETRAIL_BEAMFOLLOW, EntIndex, g_iModelIndexBeamsAll, BLAST_SKIN_FIREBEAM, 0.4, -1.0, 2.0, -1.5, 0.15, 5.0);
		}
		break;

		case FX_RIPPER_HIT:
		{
			CRSSprite *pSys = new CRSSprite(origin, g_vecZero, g_iModelIndexPartRed, kRenderTransAdd, 255,255,255, 1.0f,-0.6f, 0.7, -1.4f, 0.0f, 0.5);
			if (pSys)
			{
				g_pRenderManager->AddSystem(pSys, 0, -1, 0);

				switch (RANDOM_LONG(0,2))
				{
					case 0 : pSys->m_iFrame = PARTICLE_RED_6; break;
					case 1 : pSys->m_iFrame = PARTICLE_RED_9; break;
					case 2 : pSys->m_iFrame = PARTICLE_RED_29; break;
				}		
			}
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparks(24, origin, 0.7f, 0.02f, 0.0f, 160.0f, 0.8f, 255,255,255,1.0f, -0.6f, g_iModelIndexFire, kRenderTransAdd, 1.5f), RENDERSYSTEM_FLAG_RANDOMFRAME|RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_ADDPHYSICS|RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				DynamicLight(origin, 80, 220,170,0, 0.1, 10.0);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_ripper.wav", 1, ATTN_NORM, 0, 100 );
			}
		}
		break;

		case FX_RIPPER_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,0), g_iModelIndexExplosion3, kRenderTransAdd, 255,255,255, 0.8, 0.0, 1.0, 0, 20.0, 0));

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSSparks(24, origin, RANDOM_FLOAT(0.4,0.7), RANDOM_FLOAT(0.015,0.05), 0, RANDOM_FLOAT(200,250), RANDOM_FLOAT(1,1.5), 255,255,255, RANDOM_FLOAT(1,1.5), 0, g_iModelIndexExplosion3, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

				switch (RANDOM_LONG(0,1))
				{
					case 0 : EMIT_SOUND(EntIndex, origin, CHAN_WEAPON, "weapons/explode_ripper01.wav", 1.0, ATTN_NORM, 0, 100 ); break;
					case 1 : EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_ripper02.wav", 1.0, ATTN_NORM, 0, 100 ); break;
				}
			}
		}
		break;

		case FX_STR_TARGET_SATELLITE:
		{
			//model does non move up!!  (ignores velocity)
	//		g_pRenderManager->AddSystem(new CRSModel(origin+Vector(0,0,24), Vector(90,0,0), Vector(0,0,1200), -1, gEngfuncs.pEventAPI->EV_FindModelIndex("models/projectiles.mdl"), PROJ_BODY_MULTICANNON, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -1.0, 1, 0, 0, 2.5), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_SATELLITE_TARGET, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -1.0, 0.1, 5.0, 0, 1.0), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			g_pRenderManager->AddSystem(new CRSSprite(origin, Vector(0,0,3000), g_iModelIndexAnimglow01, kRenderTransAdd, 200,0,0, 1.0f,-0.5f, 0.25, 0.0f, 0.0f, 1.5), 0, -1, RENDERSYSTEM_FFLAG_CLIPREMOVE);
		}
		break;

		case FX_SATELLITE_STRIKE:
		{
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_NUCBLOW1,DECAL_NUCBLOW3), &tr);

			g_pRenderManager->AddSystem(new CRSBeam(tr.endpos, origin+Vector(0,0,4096), g_iModelIndexBeamsAll, BLAST_SKIN_LASERBEAM, kRenderTransAdd, 255,100,0, 1.0, -0.25, 100, -5, 0.0));
			g_pRenderManager->AddSystem(new CRSModel(tr.endpos, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_LIGHT_CONE, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.75, -0.5, 0.1, 8, 0, 2.0), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			g_pRenderManager->AddSystem(new CRSCylinder(origin + Vector(0,0,96), 20, 1200, 200, 50, g_iModelIndexBeamsAll, BLAST_SKIN_FIREBEAM, kRenderTransAdd, 255,255,255, 1, -1, 0.8));
			g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos + Vector(0,0,2), Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartRed, kRenderTransAdd, 128,128,128, 1.0, -0.2, 5, 8, PARTICLE_RED_8, 0, 0.0));

			DynamicLight(origin, 1200, 225,150,0, 2.5, 400.0);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_satellite_strike.wav", 1, ATTN_NONE, 0, PITCH_NORM );

			g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?20:40, 150, origin, Vector(0,0,1), Vector(0.5,0.5,1.0), 10, 120, 110,110,110, 0.6, -0.1, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?20:40, 175, origin, Vector(0,0,1), Vector(0.5,0.5,1.0), 10, 120, 110,110,110, 0.6, -0.1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_2, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?20:40, 250, origin, origin, Vector(1,1,1), 50, 80, 185,0,255, 0.9, -0.75, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_RANDOMFRAME | RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?20:40, 350, origin, origin, Vector(1,1,1), 10, 150, 195,0,255, 0.5, -0.4, g_iModelIndexAnimSpr10, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?20:40, 400, origin, origin, Vector(1,1,1), 10, 150, 110,110,110, 0.7, -0.6, g_iModelIndexPartViolet, FALSE, PARTICLE_VIOLET_1, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CPSBlastCone(UTIL_PointIsFar(origin, 1.0)?20:40, 120, origin, Vector(0,0,1), Vector(0.5,0.5,1.0), 5, 40, 220,120,250, 0.3, -0.05, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.5))
			{
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,5), RANDOM_FLOAT(4.0,5.2), RANDOM_FLOAT(500, 800), SPARKSHOWER_FIREEXP, origin, Vector(0,0,1), VECTOR_CONE_20DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
		}
		break;

		case FX_FLASHBANG_DETONATE:
		{
			src = origin+Vector(0,0,24);
			EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_flashbang.wav", 1, ATTN_LOW_HIGH, 0, PITCH_NORM );
			g_pRenderManager->AddSystem(new CPSSparkShower(UTIL_PointIsFar(origin, 1.0)?2:6, RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(240, 320), SPARKSHOWER_SPARKS, src, Vector(0,0,1), VECTOR_CONE_40DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
     		g_pRenderManager->AddSystem(new CPSBlastCone(8, 50, src, Vector(0,0,1), VECTOR_CONE_35DEGREES, 20, 75, 0,0,0, 0.3, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_5, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
 			DynamicLight(src, 600, 128,128,128, 0.2, 250.0);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, src-Vector(0,0,64), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
				DecalTrace(RANDOM_LONG(DECAL_EXPSCORCH1,DECAL_EXPSCORCH3), &tr);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(32, 250, src, Vector(0,0,1), VECTOR_CONE_LOTS, 10, 175, 0,0,0, 0.25, -0.4, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_4, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,6), RANDOM_FLOAT(1.0,1.5), RANDOM_FLOAT(320, 400), SPARKSHOWER_SPARKSMOKE, src, Vector(0,0,1), VECTOR_CONE_20DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(1,3), RANDOM_FLOAT(1.3,2.0), RANDOM_FLOAT(320, 400), SPARKSHOWER_BLACKSMOKE, src, Vector(0,0,1), VECTOR_CONE_30DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
			}
		}
		break;

		case FX_FLASHBANG_BLAST:
		{
			src = origin+Vector(0,0,16);
			g_pRenderManager->AddSystem(new CPSSparkShower(UTIL_PointIsFar(origin, 1.0)?2:6, RANDOM_FLOAT(1.5,2.0), RANDOM_FLOAT(200, 300), SPARKSHOWER_SPARKS, src, Vector(0,0,1), VECTOR_CONE_20DEGREES, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
			DynamicLight(src, 300, 128,128,128, 0.2, 250.0);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
		  		g_pRenderManager->AddSystem(new CPSBlastCone(8, 50, src, Vector(0,0,1), VECTOR_CONE_35DEGREES, 20, 75, 0,0,0, 0.3, -0.1, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_5, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
 				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_flashbang.wav", 1, ATTN_NORM, 0, PITCH_NORM );
				g_pRenderManager->AddSystem(new CPSBlastCone(32, 250, src, src, Vector(1,1,1), 10, 175, 0,0,0, 0.25, -0.4, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_4, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;
		
		case FX_TELEPORT_TARGET_ACTIVATE:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(12, 75, origin-Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_LOTS, 20, 75, 255,255,255, 0.3, -0.6, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
 
			if (!UTIL_PointIsFar(origin, 1.0))
			{
				DynamicLight(origin, 200, 0,128,128, 0.2, 250.0);
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_shockwave.wav", 1, ATTN_NORM, 0, PITCH_NORM );
				g_pRenderManager->AddSystem(new CPSSparks(32, origin-Vector(0,0,16), 0.5f, 0.025f, 0.0f, -240.0f, 0.8f, 255,255,255,1.0f, -0.6f, g_iModelIndexAnimSpr7, kRenderTransAdd, 0.25f), RENDERSYSTEM_FLAG_RANDOMFRAME|RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case FX_ATOMBOMB_DETONATE_PART1:
		{
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(origin, origin-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			if (tr.fraction != 1)
			{
				DecalTrace(DECAL_BLOW, &tr);
				g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos, Vector(0,0,0), Vector(90,0,0), g_iModelIndexAnimSpr8, kRenderTransAdd, 255,128,0, 1.0, -0.25, 10, 5, 0, 0, 0.0));
			}

			EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "weapons/explode_nuclearmissile.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			DynamicLight(origin, 2048, 229,97,0, 5.0, 250.0);
			g_pRenderManager->AddSystem(new CPSBlastCone(256, 200, origin, Vector(0,0,2), Vector(0.5,0.5,1.0), 10, 250, 170,80,32, 1, -0.25, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 15.0),  RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			g_pRenderManager->AddSystem(new CRSModel(origin, g_vecZero, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_NUKE_MUSHROOM, BLAST_SKIN_FIRE_NUKE, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -0.05, 0.5, 0.6, 0, 20), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
		}
		break;

		case FX_ATOMBOMB_DETONATE_PART2:
		{
			EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "weapons/explode4.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			DynamicLight(origin, 2048, 229,97,0, 1.5, 250.0);
			g_pRenderManager->AddSystem(new CRSCylinder(origin+Vector(0,0,128), 20, 2800, 120, 90, g_iModelIndexBeamsAll, BLAST_SKIN_FIREBEAM, kRenderTransAdd, 145,50,0, 1, -0.75, 1.0));
		}
		break;

		case FX_ATOMBOMB_DETONATE_PART3:
		{
			EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "weapons/explode5.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			g_pRenderManager->AddSystem(new CRSCylinder(origin+Vector(0,0,512), 20, 3200, 160, 90, g_iModelIndexBeamsAll, BLAST_SKIN_C4, kRenderTransAdd, 255,255,255, 1, -0.75, 1.0));
			g_pRenderManager->AddSystem(new CPSBlastCone(64, 100, origin+Vector(0,0,512), Vector(1,1,1), Vector(2.0,2.0,0.5), 10, 250, 200,200,200, 1, -0.35, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 2.5),  RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
		}
		break;

		case FX_ATOMBOMB_DETONATE_PART4:
		{
			EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "weapons/explode1.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			g_pRenderManager->AddSystem(new CRenderSystem(origin+Vector(0,0,1224), Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartWhite, kRenderTransAdd, 145,50,0, 1.0, -0.33, 8, 150, PARTICLE_WHITE_14, 0, 0.0));
			g_pRenderManager->AddSystem(new CPSBlastCone(196, 350, origin, Vector(0,0,1), Vector(0.5,0.5,1.0), 100, 75, 255,255,255, 0.5, -0.2, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 5.0),  RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
		}
		break;

		case FX_ATOMBOMB_DETONATE_PART5:
		{
			EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "weapons/explode2.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			g_pRenderManager->AddSystem(new CRSCylinder(origin+Vector(0,0,768), 20, 4000, 200, 90, g_iModelIndexBeamsAll, BLAST_SKIN_WASTEDBEAM, kRenderTransAdd, 145,50,0, 1, -0.75, 1.0));
		}
		break;

		case FX_ATOMBOMB_DETONATE_PART6:
		{
			EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "weapons/explode3.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			g_pRenderManager->AddSystem(new CPSBlastCone(96, 100, origin+Vector(0,0,1000), Vector(1,1,1), Vector(2.0,2.0,0.5), 10, 250, 200,200,200, 1, -0.35, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 2.5),  RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
		}
		break;

		case FX_ATOMBOMB_DETONATE_PART7:
		{
			EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "weapons/explode4.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			g_pRenderManager->AddSystem(new CRenderSystem(origin+Vector(0,0,1424), Vector(0,0,0), Vector(90,0,0), g_iModelIndexPartWhite, kRenderTransAdd, 145,50,0, 1.0, -0.33, 8, 200, PARTICLE_WHITE_14, 0, 0.0));
			g_pRenderManager->AddSystem(new CPSBlastCone(196, 600, origin, Vector(0,0,1), Vector(0.5,0.5,1.0), 100, 75, 145,50,0, 0.5, -0.2, g_iModelIndexAnimSpr10, TRUE, 0, kRenderTransAdd, 5.0),  RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
		}
		break;

		case FX_ATOMBOMB_DETONATE_PART8:
		{
			g_pRenderManager->AddSystem(new CRSCylinder(origin+Vector(0,0,1024), 20, 2000, 175, 80, g_iModelIndexBeamsAll, BLAST_SKIN_WASTEDBEAM, kRenderTransAdd, 145,50,0, 1, -0.5, 1.0));
			EMIT_SOUND(EntIndex, origin, CHAN_STREAM, "weapons/explode5.wav", 1, ATTN_NONE, 0, PITCH_NORM );
			g_pRenderManager->AddSystem(new CPSBlastCone(96, 800, origin, Vector(0,0,1), Vector(0.5,0.5,1.0), 100, 75, 0,0,0, 0.3, -0.15, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_5, kRenderTransAdd, 3.0),  RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
		}
		break;

		case FX_ACID_BLOB:
		{
			g_pRenderManager->AddSystem(new CPSBlastCone(196, 15, origin, origin, Vector(0,0,0), 5, 5, 255,255,255, 0.5, -1.0, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_4, kRenderTransAdd, 5), 0, EntIndex, RENDERSYSTEM_FFLAG_ICNF_REMOVE);
		}
		break;

		case FX_ACID_BLOB_TOUCH:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(60,75), origin+Vector(0,0,6), Vector(0,0,1), VECTOR_CONE_LOTS, 5, 40, 0,125,0, 0.8, -0.8, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/hit_biorifle.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
				g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(180,220), origin+Vector(0,0,4), Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(0.8,1.1), 0, 255,255,255, 0.8, -0.1, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(180,220), origin+Vector(0,0,4), Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(1.2,1.5), 0, 255,255,255, 0.8, -0.1, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
		}
		break;

		case FX_ACID_BLOB_DETONATE:
		{
			FX_RemoveRS(EntIndex);
			g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(200,240), origin+Vector(0,0,6), origin+Vector(0,0,6), Vector(1,1,1), 5, 60, 0,125,0, 0.6, -0.8, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

			if (!UTIL_PointIsFar(origin, 1.0))
			{
				EMIT_SOUND(EntIndex, origin, CHAN_VOICE, "weapons/explode_biorifle.wav", 1.0, ATTN_NORM, 0, PITCH_NORM );
				g_pRenderManager->AddSystem(new CPSBlastCone(16, RANDOM_FLOAT(150,190), origin+Vector(0,0,6), Vector(0,0,1), VECTOR_CONE_LOTS, 5, 50, 255,255,255, 0.5, -0.7, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(80,110), origin+Vector(0,0,6), Vector(0,0,1), VECTOR_CONE_LOTS, 5, 50, 255,255,255, 0.5, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_1, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(24, RANDOM_FLOAT(250,300), origin+Vector(0,0,6), Vector(0,0,1), VECTOR_CONE_LOTS, RANDOM_FLOAT(1.0,1.5), 0, 255,255,255, 0.8, -0.1, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE  | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}

		}
		break;
	}
return 1;
}

//=====================//
//Beam & Bullet Effects//
//=====================//
int __MsgFunc_FireBeam( const char *pszName, int iSize, void *pbuf )
{
	pmtrace_t tr;
	vec3_t startPos, endPos, normal, normal_angle, src, end;

	BOOL InWater;
	BOOL InSky;

	BEGIN_READ( pbuf, iSize );
	startPos.x = READ_COORD();//Beam start point i.e. Gun Barrel
	startPos.y = READ_COORD();
	startPos.z = READ_COORD();
	endPos.x = READ_COORD();//Beam End point
	endPos.y = READ_COORD();
	endPos.z = READ_COORD();
	normal.x = READ_COORD();//Normal
	normal.y = READ_COORD();
	normal.z = READ_COORD();
	byte Surface = READ_BYTE();
	byte Type = READ_BYTE();
	BOOL IsWorldBrush = READ_BYTE();

	int iRand = RANDOM_LONG(0,0x7FFF);
	int contents = gEngfuncs.PM_PointContents(endPos, NULL);

	if (contents == CONTENTS_WATER || contents == CONTENTS_SLIME || contents == CONTENTS_LAVA)
		InWater = TRUE;
	else
		InWater = FALSE;

	if (contents == CONTENTS_SKY)
		InSky = TRUE;
	else
		InSky = FALSE;

	VectorMA(endPos, 4.0, normal, src);// units backward
	VectorMA(endPos, -4.0, normal, end);// forward (into the brush)

	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(src, end, PM_STUDIO_IGNORE, -1, &tr);
	BEAM *pBeam;

	if (Surface == CHAR_TEX_ENERGYSHIELD)
	{
		//because we draw surface effects from Impact(...) function
		if (Type != BULLET_MACHINEGUN && Type != BULLET_MINIGUN && Type != BULLET_BUCKSHOT && Type != BULLET_VULCAN && Type != BULLET_RAILGUN && Type != BULLET_SUPERRAILGUN)
		{	
			InSky = true; //prevent any decals
			g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(6,9), endPos, 0.5, 0.01, 0, 100, 0.8, 128,128,128, 1, 0, g_iModelIndexExplosion4, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

			if (!UTIL_PointIsFar(endPos, 1.0))
			{
				gEngfuncs.pEfxAPI->R_SparkShower( endPos );
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), 20, endPos, endPos, Vector(1,1,1), 10, 25, 128,128,128, 0.5, -0.75, g_iModelIndexExplosion4, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}

	}
	switch (Type)
	{
		case BULLET_MACHINEGUN:
		case BULLET_MINIGUN:
		case BULLET_BUCKSHOT:
			if (iRand < (0x7fff/2))
				g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexBeamsAll, BLAST_SKIN_TRACER, kRenderTransAdd, 255,255,127, 0.8, 0, 1.2, 0, 0.1));

			if (!InSky)
			{
				DecalTrace(RANDOM_LONG(DECAL_GUNSHOT1, DECAL_GUNSHOT5), &tr);
				BulletImpact(Surface, endPos, normal);
			}
		break;

		case BULLET_VULCAN:
			if (iRand < (0x7fff/2))
				g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexMuzzleFlash1, 0, kRenderTransAdd, 250,220,40, 1.0, 0, 1.0, 0, 0.1));

			if (!InSky)
			{
				DecalTrace(RANDOM_LONG(DECAL_BIGSHOT1, DECAL_BIGSHOT5), &tr);
				BulletImpact(Surface, endPos, normal);
			}
		break;

		case BULLET_GAUSS:
			{
				g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexBeamsAll, BLAST_SKIN_GAUSSBEAM, kRenderTransAdd, 255,255,255, 0.9, -1.1, 1.5, 0, 0.2));

				CRSSprite *pSys = new CRSSprite(startPos, g_vecZero, g_iModelIndexPartRed, kRenderTransAdd, 255,255,255, 1.0f, -1.0f, 0.0175f, -0.0175f, 0.0f, 0.2f);
				if (pSys)
				{
					g_pRenderManager->AddSystem(pSys, 0, -1, 0);
					pSys->m_iFrame = PARTICLE_RED_2;
				}

				if (!InSky)
				{
					if(!InWater)
					{
						DynamicLight(endPos, RANDOM_LONG(130,160), 240,240,50, 0.15, 150.0);

						if (IsWorldBrush)
						{
							g_pRenderManager->AddSystem(new CRSBeamStar(endPos, g_iModelIndexPartRed, PARTICLE_RED_12, 0, 64, kRenderTransAdd, 255,255,255, 0.5, -0.2, 25, 3, 8.0));
							g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(20,24), RANDOM_FLOAT(36,48), endPos, Vector(0,0,1), VECTOR_CONE_20DEGREES, 5, 15, 255,255,255, 0.6, -0.6, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_3, kRenderTransAdd, RANDOM_FLOAT(1.5, 2.5)), RENDERSYSTEM_FLAG_RANDOMFRAME, -1);
						}
					}
					DecalTrace(DECAL_GAUSSSHOT1, &tr);

					if (!UTIL_PointIsFar(endPos, 1.0))
					{
						if(!InWater)
						{
							g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(90, 140), endPos, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(1,3), 0, 255,255,255, 1.0, -0.25, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
							g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(90, 140), endPos, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.25, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
						}
						switch (RANDOM_LONG(0,2))
						{
						  case 0 : EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/explode_beam1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
						  case 1 : EMIT_SOUND( 0, endPos, CHAN_BODY, "weapons/explode_beam2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
						  case 2 : EMIT_SOUND( 0, endPos, CHAN_ITEM, "weapons/explode_beam3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
						}
					}
				}
			}
		break;

		case BULLET_HEAVY_TURRET:
			if (iRand < (0x7fff/2))
				g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexAnimSpr9, 0, kRenderTransAdd, 128,128,128, 0.9, 0, 1.8, 0, 0.1));

			if (!InSky)
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(3,5), RANDOM_FLOAT(60,85), src, normal, Vector(0.2,0.2,0.2), 8, RANDOM_LONG(20,26), 255,255,255, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_19, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				DecalTrace(RANDOM_LONG(DECAL_OFSCORCH1, DECAL_OFSCORCH3), &tr);

				if(!InWater)
				{				
					g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(3,5), RANDOM_FLOAT(90,130), src, normal, Vector(0.2,0.2,0.2), 8, RANDOM_LONG(24,32), 255,255,255, 0.8, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_17, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					DynamicLight(startPos, RANDOM_LONG(100,150), 210,160,0, 0.1, 100.0);
				}
				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if(!InWater)
					{
						g_pRenderManager->AddSystem(new CPSSparks(8, endPos, 0.6, 0.02, 0, 120, 0.75, 255,255,255, 1, 0, g_iModelIndexAnimSpr9, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(3, RANDOM_FLOAT(25,40), src, normal, VECTOR_CONE_40DEGREES, 8, 30, 255,255,255, 1.0, -1.5, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
					}
					switch (RANDOM_LONG(0,2))
					{
					  case 0 : EMIT_SOUND( 0, endPos, CHAN_ITEM, "weapons/explode1.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					  case 1 : EMIT_SOUND( 0, endPos, CHAN_BODY, "weapons/explode4.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					  case 2 : EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/explode2.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					}
				}
			}
		break;

		case BULLET_BFG:
			g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexAnimSpr12, 0, kRenderTransAdd, 128,128,128, 0.5, 0, 3.0, 0, 0.1));

			if (!InSky)
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 300, endPos, endPos, Vector(1,1,1), 10, 50, 0,200,0, 0.5, -0.7, g_iModelIndexPartRed, FALSE, PARTICLE_RED_5, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(20, 150, endPos, endPos, Vector(1,1,1), 10, 100, 150,80,0, 0.8, -1, g_iModelIndexPartRed, FALSE, PARTICLE_RED_9, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

				DynamicLight(endPos, 300, 128,128,0, 0.3, 250.0);
				DecalTrace(RANDOM_LONG(DECAL_BLASTSCORCH1, DECAL_BLASTSCORCH3), &tr);

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if(!InWater)
					{
						g_pRenderManager->AddSystem(new CPSBlastCone(20, 85, endPos, endPos, Vector(1,1,1), 10, 60, 0,0,0, 0.6, -0.3, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_3, kRenderTransAlpha, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(20, 400, endPos, endPos, Vector(1,1,1), 10, 20, 255,255,255, 1, -1.2, g_iModelIndexPartRed, FALSE, PARTICLE_RED_4, kRenderTransAdd, 1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(240, 380), endPos, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.25, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(180, 280), endPos, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(3,6), 0, 255,255,255, 1.0, -0.25, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
					}
					if (RANDOM_LONG(0,0x7FFF) < (0x7fff/2))
						EMIT_SOUND( 0, endPos, CHAN_STATIC, "weapons/explode5.wav", 1, ATTN_LOW, 10, 90 );
				}
			}
		break;

		case BULLET_LASER:
		{
			g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexBeamsAll, BLAST_SKIN_LASERBEAM, kRenderTransAdd, 255,255,255, 1.0, -0.5, 1.5, -3.0, 0.3));
			CRSSprite *pSys = new CRSSprite(startPos, g_vecZero, g_iModelIndexPartRed, kRenderTransAdd, 255,255,255, 1.0f, -1.0f, 0.0075f, -0.0075f, 0.0f, 0.3f);
			if (pSys)
			{
				g_pRenderManager->AddSystem(pSys, 0, -1, 0);
				pSys->m_iFrame = PARTICLE_RED_1;
			}

			if (!InSky)
			{
				if(!InWater)
				{
					g_pRenderManager->AddSystem(new CPSBlastCone(8, 10, src, src, Vector(1,1,1), 5, 25, 128,128,128, 1, -0.6, g_iModelIndexPartRed, FALSE, PARTICLE_RED_17, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
					DynamicLight(endPos, RANDOM_LONG(110,135), 255,0,0, 0.15, 150.0);
				}
				DecalTrace(RANDOM_LONG(DECAL_BEAMSHOT1, DECAL_BEAMSHOT3), &tr);

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if(!InWater)
					{
						g_pRenderManager->AddSystem(new CPSBlastCone(32, 75, src, normal, VECTOR_CONE_LOTS, 1, 0, 128,128,128, 1, -0.9, g_iModelIndexPartRed, FALSE, PARTICLE_RED_0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);

						if(IsWorldBrush)
							g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(12,16), RANDOM_FLOAT(14,18), endPos, Vector(0,0,1), VECTOR_CONE_20DEGREES, 5, 5, 255,255,255, 0.5, -0.5, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, RANDOM_FLOAT(1.5, 2.5)), RENDERSYSTEM_FLAG_RANDOMFRAME, -1);
					}
					switch (RANDOM_LONG(0,2))
					{
					  case 0 : EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/explode_beam1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 1 : EMIT_SOUND( 0, endPos, CHAN_STATIC, "weapons/explode_beam2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 2 : EMIT_SOUND( 0, endPos, CHAN_BODY, "weapons/explode_beam3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					}
				}
			}
		}
		break;

		case BULLET_TWINLASER:
		{
			g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexBeamsAll, BLAST_SKIN_PULSE, kRenderTransAdd, 0,190,255, 0.8, -0.5, 2, 0, 0.15));
			CRSSprite *pSys = new CRSSprite(startPos, g_vecZero, g_iModelIndexPartBlue, kRenderTransAdd, 255,255,255, 1.0f, -3.0f, 0.01f, 0.05f, 0.0f, 2.0f);
			if (pSys)
			{
				g_pRenderManager->AddSystem(pSys, 0, -1, 0);
				pSys->m_iFrame = PARTICLE_BLUE_7;
			}
			if (!InSky)
			{
				if(!InWater)
				{
					g_pRenderManager->AddSystem(new CPSBlastCone(3, RANDOM_FLOAT(6,11), endPos, Vector(0,0,1), Vector(0.2,0.2,0.5), 5, 5, 0,190,255, 0.8, -0.4, g_iModelIndexFire, TRUE, 0, kRenderTransAdd, 1.0), 0, -1);
					DynamicLight(endPos, RANDOM_LONG(115,145), 0,190,255, 0.4, 200.0);
				}
				DecalTrace(RANDOM_LONG(DECAL_BEAMSHOT1, DECAL_BEAMSHOT3), &tr);
				
				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if(!InWater)
					{
						g_pRenderManager->AddSystem(new CPSBlastCone(5, 12, endPos, endPos, Vector(1,1,1), 5, 20, 128,128,128, 0.4, -0.2, g_iModelIndexAnimSpr7, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
		     			g_pRenderManager->AddSystem(new CPSBlastCone(5, 5, endPos, endPos, Vector(1,1,1), 1, 10, 128,128,128, 0.8, -0.3, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
					}

					if(IsWorldBrush)
						g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(12,16), RANDOM_FLOAT(14,18), endPos, Vector(0,0,1), VECTOR_CONE_20DEGREES, 10, 10, 0,128,128, 0.5, -0.5, g_iModelIndexFlame, TRUE, 0, kRenderTransAdd, RANDOM_FLOAT(1.5, 2.5)), RENDERSYSTEM_FLAG_RANDOMFRAME, -1);

					switch (RANDOM_LONG(0,2))
					{
					  case 0 : EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/explode_beam1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 1 : EMIT_SOUND( 0, endPos, CHAN_STATIC, "weapons/explode_beam2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 2 : EMIT_SOUND( 0, endPos, CHAN_ITEM, "weapons/explode_beam3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					}
				}
			}
		}
		break;

		case BULLET_PHOTONGUN:
		{
			g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexBeamsAll, BLAST_SKIN_PHOTONBEAM, kRenderTransAdd, 80,80,255, 1.0, -1.2, 0.1, 5.0, 0.0));
			CRSSprite *pSys = new CRSSprite(startPos, g_vecZero, g_iModelIndexPartBlue, kRenderTransAdd, 255,255,255, 1.0f, -1.2f, 0.01f, 0.05f, 0.0f, 2.0f);
			if (pSys)
			{
				g_pRenderManager->AddSystem(pSys, 0, -1, 0);
				pSys->m_iFrame = PARTICLE_BLUE_6;
			}

			if (!InSky)
			{
				DecalTrace(DECAL_GAUSSSHOT1, &tr);
				if(!InWater)
				{
					DynamicLight(endPos, RANDOM_LONG(130,160), 80,80,255, 0.4, 240.0);
					g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(60,80), src, normal, VECTOR_CONE_40DEGREES, 8, 15, 255,255,255, 0.5, -0.6, g_iModelIndexAnimSpr2, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				}
				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if(!InWater)
					{
						CPSSparks *pSys2 = new CPSSparks(32, src, RANDOM_FLOAT(1.2,1.7), RANDOM_FLOAT(0.02,0.04), 0, RANDOM_FLOAT(250,300), RANDOM_FLOAT(1.2, 1.6), 128,128,128, 1, -0.7, g_iModelIndexPartBlue, kRenderTransAdd, 0.3);
						if (pSys2)
						{
							g_pRenderManager->AddSystem(pSys2, RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1, 0);
							pSys2->m_iFrame = PARTICLE_BLUE_6;
						}
						if(IsWorldBrush)
						{
							g_pRenderManager->AddSystem(new CRSSprite(src, Vector(0,0,0), g_iModelIndexAnimSpr5, kRenderTransAdd, 255,255,255, 1, -0.5, RANDOM_FLOAT(0.6,0.8), 0.0, 20.0, 2.0));
							g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(12,16), RANDOM_FLOAT(32,40), endPos, Vector(0,0,1), VECTOR_CONE_20DEGREES, RANDOM_FLOAT(2,3), RANDOM_FLOAT(5,6), 128,128,128, 0.4, -0.2, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_6, kRenderTransAdd, 1.5), 0, -1);
						}
					}
					switch (RANDOM_LONG(0,2))
					{
					  case 0 : EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/explode_beam1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 1 : EMIT_SOUND( 0, endPos, CHAN_STATIC, "weapons/explode_beam2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 2 : EMIT_SOUND( 0, endPos, CHAN_BODY, "weapons/explode_beam3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					}
				}
			}
		}
		break;

		case BULLET_MEZONCANNON:
			g_pRenderManager->AddSystem(new CPSFlatTrail(startPos, endPos, g_iModelIndexPartBlack, PARTICLE_BLACK_10, kRenderTransAlpha, 100,100,100, 0.7, -0.5, 1.0, -20.0, 20.0, 0));
			gEngfuncs.pEfxAPI->R_RocketTrail(startPos, endPos, 1);

			if (!InSky)
			{
				if(!InWater)
				{
					g_pRenderManager->AddSystem(new CPSBlastCone(3, 80, src, normal, Vector(0.2,0.2,0.2), 8, 25, 1,1,1, 0.8, -1.0, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_5, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
					g_pRenderManager->AddSystem(new CPSBlastCone(3, 120, src, normal, Vector(0.2,0.2,0.2), 8, 30, 1,1,1, 0.8, -1.0, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_6, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				}
				DecalTrace(RANDOM_LONG(DECAL_BEAMSHOT1, DECAL_BEAMSHOT3), &tr);

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if(!InWater && IsWorldBrush)
						g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,3), RANDOM_FLOAT(0.8,1.2), RANDOM_FLOAT(180, 240), SPARKSHOWER_SPARKSMOKE, src, normal, VECTOR_CONE_LOTS, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

					switch (RANDOM_LONG(0,2))
					{
					  case 0 : EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/explode_beam1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 1 : EMIT_SOUND( 0, endPos, CHAN_STATIC, "weapons/explode_beam2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 2 : EMIT_SOUND( 0, endPos, CHAN_BODY, "weapons/explode_beam3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					}
				}
			}
		break;

		case BULLET_RAILGUN:
			g_pRenderManager->AddSystem(new CPSFlatTrail(startPos, endPos, g_iModelIndexPartWhite, PARTICLE_WHITE_14, kRenderTransAdd, 255,255,255, 1.0, -1.5, 0.1, 3.0, 15.0, 0));

			if (!InSky)
			{
				DecalTrace(RANDOM_LONG(DECAL_BIGSHOT1, DECAL_BIGSHOT5), &tr);
				BulletImpact(Surface, endPos, normal);
			}
		break;

		case BULLET_TAUCANNON:
			g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexBeamsAll, BLAST_SKIN_TAUBEAM, kRenderTransAdd, 255,255,255, 1.0, -1.0, 1.0, 0.5, 0.0));

			pBeam = gEngfuncs.pEfxAPI->R_BeamPoints( startPos, endPos, g_iModelIndexBeamsAll, 0.8, 1, 0.08, 255, 15, BLAST_SKIN_TAUBEAM, 0, 255, 255, 255 );
			if (pBeam)
				pBeam->flags |= ( FBEAM_SINENOISE);

			if (!InSky)
			{
				if(!InWater)
				{
					g_pRenderManager->AddSystem(new CRSBeamStar(endPos, g_iModelIndexAnimSpr7, 0, 0, 60, kRenderTransAdd, 125,0,255, 1.0, -0.5, 10, 25, 3.0));
					DynamicLight(endPos, RANDOM_LONG(180,210), 160,0,210, 0.2, 250.0);
				}
				DecalTrace(RANDOM_LONG(DECAL_OFSCORCH1, DECAL_OFSCORCH3), &tr);

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if(!InWater)
					{
						g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(180, 240), endPos, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(1,3), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr7, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(120, 180), endPos, Vector(0,0,1), Vector(0.2,0.2,0.5), RANDOM_FLOAT(2,4), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr7, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
					}
					EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/fire_sunofgod.wav", 1.0, ATTN_NORM, 0, 105 );
				}
			}
		break;

		case BULLET_BEAMSPLITTER:
			g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexBeamsAll, BLAST_SKIN_PSPBEAM, kRenderTransAdd, 255,255,255, 1.0, -1.0, 2, 1, 0.0));
			g_pRenderManager->AddSystem(new CPSFlatTrail(startPos, endPos, g_iModelIndexPartBlue, PARTICLE_BLUE_7, kRenderTransAdd, 255,255,255, 1.0, -1.75, 0.1, 3.0, 20.0, 0));

			if (!InSky)
			{
				if(!InWater)
				{
					g_pRenderManager->AddSystem(new CRSBeamStar(endPos, g_iModelIndexPartBlue, PARTICLE_BLUE_8, 0, 48, kRenderTransAdd, 255,255,255, 1.0, -1.25, 4.0, 300, 4.0), RENDERSYSTEM_FLAG_LOOPFRAMES);
					DynamicLight(endPos, RANDOM_LONG(220,250), 0,160,110, 0.2, 250.0);
				}
				DecalTrace(RANDOM_LONG(DECAL_GLUONSCORCH1, DECAL_GLUONSCORCH3), &tr);

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if(!InWater)
					{
						color24 tracercolor = {55,60,144};
						FX_StreakSplash(endPos, g_vecZero, tracercolor, 80, 320.0f);
						g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(2,3), RANDOM_FLOAT(0.5,0.9), RANDOM_FLOAT(400, 600), SPARKSHOWER_BLUEENERGY, endPos, Vector(0,0,1), Vector(0.2,0.2,0.4), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
						gEngfuncs.pEfxAPI->R_TempSprite(endPos, (float *)&g_vecZero, 1.2, g_iModelIndexAnimSpr3, kRenderGlow, kRenderFxNoDissipation, 0.5, 0.0, FTENT_FADEOUT);
					}
					EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/explode_disruptor.wav", 1.0, ATTN_NORM, 0, 100);
				}
			}
		break;

		case BULLET_SUPERRAILGUN:
			g_pRenderManager->AddSystem(new CPSFlatTrail(startPos, endPos, g_iModelIndexPartGreen, PARTICLE_GREEN_5, kRenderTransAdd, 255,255,255, 1.0, -1.5, 0.1, 3.0, 15.0, 0));

			if (!InSky)
			{
				if (!UTIL_PointIsFar(endPos, 1.0) && !InWater)
				{
					g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(20,24), endPos, 1.0, 0.03, 0, 200, 0.8, 128,128,128, 1, 0, g_iModelIndexAnimSpr10, kRenderTransAdd, 0.2), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
				}
				DecalTrace(RANDOM_LONG(DECAL_BIGSHOT1, DECAL_BIGSHOT5), &tr);
				BulletImpact(Surface, endPos, normal);
			}
		break;

		case BULLET_LIGHTNING:
		{
			gEngfuncs.pEfxAPI->R_BeamPoints( startPos, endPos, g_iModelIndexBeamsAll, 0.15, 2.0, 0.4, 200, 15, BLAST_SKIN_LIGHTNING, 0, 255, 255, 255 );

			if (!InSky)
			{
				DecalTrace(RANDOM_LONG(DECAL_SMALLSCORCH1, DECAL_SMALLSCORCH3), &tr);

				if(!InWater)
					DynamicLight(endPos, RANDOM_LONG(220,250), 200,240,250, 1.7, 80.0);

				if (!UTIL_PointIsFar(endPos, 1.0) && !InWater)
				{
					g_pRenderManager->AddSystem(new CPSSparkShower(4, 0.05, 0, SPARKSHOWER_LIGHTNING_CHAOS, src, src, Vector(0,0,0), g_iModelIndexPartRed, 1.5), 0, -1, RENDERSYSTEM_FLAG_SIMULTANEOUS); 
  					g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(6,9), RANDOM_FLOAT(1.8,2.4), RANDOM_FLOAT(200, 250), SPARKSHOWER_SPARKS2, endPos, Vector(0,0,1), Vector(0.2,0.2,0.4), g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
					gEngfuncs.pEfxAPI->R_StreakSplash (endPos, normal, 0, RANDOM_FLOAT(35,40), 300, -200, 200);
					EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/explode_lightning.wav", 1, ATTN_NORM, 0, PITCH_NORM );
				}

   			}
		}
		break;

		case BULLET_CLUSTERGUN:
		{
			g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexBeamsAll, BLAST_SKIN_PLASMA, kRenderTransAdd, 127,127,127, 0.7, 0, 1.2, 0, 0.1));
			if (!InSky)
			{
				DecalTrace(RANDOM_LONG(DECAL_OFSCORCH1, DECAL_OFSCORCH3), &tr);

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					EMIT_SOUND(0, endPos, CHAN_VOICE, "weapons/explode_clustergun.wav", 1, ATTN_NORM, 0, PITCH_NORM );

					if(!InWater)
					{
						g_pRenderManager->AddSystem(new CPSSparks(32, src, RANDOM_FLOAT(1.0, 2.0), RANDOM_FLOAT(0.012, 0.020), 0.0f, RANDOM_FLOAT(200, 280), RANDOM_FLOAT(0.8, 1.2), 255,255,255, 1.0f, -0.5f, g_iModelIndexAnimSpr9, kRenderTransAdd, 0.5f), RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(3,5), RANDOM_FLOAT(80,100), src, normal, Vector(0.2,0.2,0.2), 8, RANDOM_LONG(24,32), 255,255,255, 1.0, -1.5, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(3,5), RANDOM_FLOAT(65,85), src, normal, Vector(0.2,0.2,0.2), 8, RANDOM_LONG(24,32), 1,1,1, 1.0, -0.8, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_3, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(32, 75, src, normal, VECTOR_CONE_LOTS, RANDOM_FLOAT(1,1.5), 0, 128,128,128, 1, -0.6, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
						DynamicLight(startPos, RANDOM_LONG(100,150), 230,98,0, 0.1, 100.0);
					}	
				}
			}
		}
		break;

		case BULLET_FLAK_CANNON:
		{
			g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexMuzzleFlash1, 0, kRenderTransAdd, 255,255,255, 0.9, 0.0, 1.0, 0, 0.15));

			if (!InSky)
			{
				DecalTrace(RANDOM_LONG(DECAL_SMALLSCORCH1, DECAL_SMALLSCORCH3), &tr);
				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if(!InWater)
					{
						if (iRand < (0x7fff/2))
						{
							switch (RANDOM_LONG(0,2))
							{
								case 0 : EMIT_SOUND(0, endPos, CHAN_WEAPON, "weapons/explode1.wav", 1, ATTN_NORM, 0, 110 );break;
								case 1 : EMIT_SOUND(0, endPos, CHAN_STATIC, "weapons/explode2.wav", 1, ATTN_NORM, 0, 110 );break;
								case 2 : EMIT_SOUND(0, endPos, CHAN_VOICE, "weapons/explode3.wav", 1, ATTN_NORM, 0, 110 );break;
							}
						}
						g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(6,8), src, RANDOM_FLOAT(0.5, 0.9), RANDOM_FLOAT(0.012, 0.020), 0.0f, RANDOM_FLOAT(150,180), RANDOM_FLOAT(0.8, 1.2), 255,255,255, 1.0f, -0.5f, g_iModelIndexAnimSpr9, kRenderTransAdd, 0.5f), RENDERSYSTEM_FLAG_SIMULTANEOUS|RENDERSYSTEM_FLAG_ADDPHYSICS, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(3,5), RANDOM_FLOAT(80,100), src, normal, Vector(0.2,0.2,0.2), 12, RANDOM_LONG(32,44), 255,255,255, 1.0, -1.5, g_iModelIndexPartRed, FALSE, PARTICLE_RED_30, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(3,5), RANDOM_FLOAT(65,85), src, normal, Vector(0.2,0.2,0.2), 8, RANDOM_LONG(24,32), 1,1,1, 1.0, -0.8, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_3, kRenderTransAlpha, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
						DynamicLight(startPos, RANDOM_LONG(90,120), 230,98,0, 0.1, 100.0);
					}	
				}

			}

		}
		break;

		case BULLET_SHOCK:
		{
			if (!InSky)
			{
				if (!InWater)
				{
					VectorAngles(normal, normal_angle);
					normal_angle[0] = -normal_angle[0];
					g_pRenderManager->AddSystem(new CRenderSystem(src, Vector(0,0,0), normal_angle, g_iModelIndexMuzzleFlash2, kRenderTransAdd, 255,255,255, 1.0, -0.75, RANDOM_FLOAT(0.5,0.7), 0, 0, 24, 0.3));
					DynamicLight(endPos, RANDOM_LONG(90,110), 220,220,0, 0.25, 250.0);
//	g_pRenderManager->AddSystem(new CRSModel(src, normal_angle, g_vecZero, -1, g_iModelIndexExplosionModel, BLAST_MDL_LIGHT_CONE, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 0.5, 0.0, 0.1, 5, 0, 0.25), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

				}
				DecalTrace(RANDOM_LONG(DECAL_SMALLSCORCH1, DECAL_SMALLSCORCH3), &tr);

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					if (!InWater)
					{
						g_pRenderManager->AddSystem(new CPSSparks(12, endPos, 1.2, 0.04, 0, 120, 1, 255,255,255, 1, 0, g_iModelIndexMuzzleFlash3, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
  						g_pRenderManager->AddSystem(new CPSBlastCone(8, 24, src, normal, VECTOR_CONE_LOTS, 5, 48, 200,200,0, 0.4, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
					}
					switch (RANDOM_LONG(0,2))
					{
						case 0:	EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/electro4.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
						case 1:	EMIT_SOUND( 0, endPos, CHAN_STATIC, "weapons/electro5.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
						case 2:	EMIT_SOUND( 0, endPos, CHAN_BODY, "weapons/electro6.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					}
				}
			}

		}
		break;

		case BULLET_NEYTRONGUN:
		{
			if (iRand < (0x7fff/2))
				g_pRenderManager->AddSystem(new CRSBeam(startPos, endPos, g_iModelIndexAnimSpr6, 0, kRenderTransAdd, 255,255,255, 1.0, 0, 1.5, 0, 0.1));

			if (!InSky)
			{
				VectorAngles(normal, normal_angle);
				normal_angle[0] = -normal_angle[0];
				g_pRenderManager->AddSystem(new CRenderSystem(src, Vector(0,0,0), normal_angle, g_iModelIndexAnimSpr11, kRenderTransAdd, 255,255,255, 1.0, -0.75, RANDOM_FLOAT(0.3,0.5), 0, 0, 24, 0.3));
				DecalTrace(RANDOM_LONG(DECAL_MINISCORCH1, DECAL_MINISCORCH3), &tr);
				
				if (!InWater)
				{
					DynamicLight(endPos, RANDOM_LONG(80,100), 0,150,160, 0.25, 250.0);
				}

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					g_pRenderManager->AddSystem(new CRSBeamStar(src, g_iModelIndexAnimSpr7, 0, 1, RANDOM_LONG(30, 40), kRenderTransAdd, 255,255,255, 1, -1, 30, -15, 0.5), 0, -1, 0);

					if (!InWater)
					{
 						g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(24,32), src, normal, VECTOR_CONE_40DEGREES, 5, 48, 0,150,160, 0.5, -0.6, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
					}
					switch (RANDOM_LONG(0,2))
					{
					  case 0 : EMIT_SOUND( 0, endPos, CHAN_VOICE, "weapons/explode_beam1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 1 : EMIT_SOUND( 0, endPos, CHAN_STATIC, "weapons/explode_beam2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					  case 2 : EMIT_SOUND( 0, endPos, CHAN_BODY, "weapons/explode_beam3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM ); break;
					}
				}
			}

		}
		break;

		//Cause we need "normal"
		case BULLET_PLASMABALL:
		{
			if (!InSky)
			{
				VectorAngles(normal, normal_angle);
				normal_angle[0] = -normal_angle[0];
				g_pRenderManager->AddSystem(new CRenderSystem(src, Vector(0,0,0), normal_angle, g_iModelIndexExplosion2, kRenderTransAdd, 255,255,255, 1.0, -0.75, RANDOM_FLOAT(1.2,1.6), 0, 0, 24, 0.3));

				DecalTrace(RANDOM_LONG(DECAL_OFSCORCH1, DECAL_OFSCORCH3), &tr);
				DynamicLight(endPos, RANDOM_FLOAT(200,230), 0,190,20, 0.5, 120.0);

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					EMIT_SOUND(0, endPos, CHAN_VOICE, "weapons/explode_plasma.wav", 1, ATTN_NORM, 0, 80 );

					if (!InWater)
					{
 						g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(24,32), src, normal, VECTOR_CONE_40DEGREES, 5, 48, 0,150,160, 0.5, -0.6, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(8, 25, src, src, Vector(1,1,1), 10, 35, 0,255,40, 0.75, -0.75, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
						g_pRenderManager->AddSystem(new CPSSparks(20, src, 0.7, 0.01, 0, 200, 1.0, 255,255,255, 1, 0, g_iModelIndexAnimSpr12, kRenderTransAdd, 0.5), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(180, 240), src, normal, VECTOR_CONE_40DEGREES, RANDOM_FLOAT(1,2), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(220, 280), src, normal, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(2,3), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr8, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
					}

				}
			}
		}
		break;

		case BULLET_TRIDENT:
		{
			if (!InSky)
			{
				VectorAngles(normal, normal_angle);
				normal_angle[0] = -normal_angle[0];
				g_pRenderManager->AddSystem(new CRenderSystem(src, Vector(0,0,0), normal_angle, g_iModelIndexAnimSpr1, kRenderTransAdd, 255,255,255, 1.0, -0.75, RANDOM_FLOAT(0.8,1.0), RANDOM_FLOAT(2,3), 0, 24, 0.5));

				DecalTrace(RANDOM_LONG(DECAL_MDSCORCH1, DECAL_MDSCORCH3), &tr);
				DynamicLight(endPos, RANDOM_FLOAT(300,360), 100,100,128, 0.5, 120.0);

				if (!UTIL_PointIsFar(endPos, 1.0))
				{
					switch (RANDOM_LONG(0,2))
					{
						case 0 : EMIT_SOUND(0, endPos, CHAN_VOICE, "weapons/explode_trident.wav", 1.0, ATTN_NORM, 0, 100 ); break;
						case 1 : EMIT_SOUND(0, endPos, CHAN_ITEM, "weapons/explode_trident.wav", 1.0, ATTN_NORM, 0, 90 ); break;
						case 2 : EMIT_SOUND(0, endPos, CHAN_BODY, "weapons/explode_trident.wav", 1.0, ATTN_NORM, 0, 110 ); break;
					}

					if (!InWater)
					{
 						g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(16,24), src, normal, VECTOR_CONE_40DEGREES, 5, 48, 0,150,160, 0.5, -0.6, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(8, 25, src, src, Vector(1,1,1), 10, 35, 255,255,255, 0.75, -0.75, g_iModelIndexAnimSpr1, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(12, RANDOM_FLOAT(240, 300), src, normal, VECTOR_CONE_40DEGREES, RANDOM_FLOAT(1,2), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr1, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
						g_pRenderManager->AddSystem(new CPSBlastCone(8, RANDOM_FLOAT(260, 330), src, normal, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(2,3), 0, 255,255,255, 1.0, -0.25, g_iModelIndexAnimSpr1, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
					}

				}
			}
		}
		break;

		// This effect is here, because we need "angles" for the legs!!!
		//endPos is a player angle
		case FX_PLAYER_GIB_ELECTROCUTED:
		{
			g_pRenderManager->AddSystem(new CRSSprite(startPos, g_vecZero, g_iModelIndexAnimSpr0, kRenderTransAdd, 255,255,255, 1.0f,-0.5f, 1.2, -0.5f, 32.0f, 0.2),0, -1, RENDERSYSTEM_FLAG_LOOPFRAMES);

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(startPos, startPos-Vector(0,0,128), PM_STUDIO_IGNORE, -1, &tr);

			g_pRenderManager->AddSystem(new CRSModel(tr.endpos, g_vecZero, g_vecZero, -1, g_iModelIndexLightningFieldModel, LGTNTG_FIELD_STRIKE, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -2.0, 2.0, -0.5, 0, 0.15), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);
			g_pRenderManager->AddSystem(new CRSModel(tr.endpos, g_vecZero, g_vecZero, -1, g_iModelIndexLightningFieldModel, LGTNTG_FIELD_LIGHTNING_BOLT_STRIKE, 0, 0, kRenderTransAdd, kRenderFxFullBright, 255,255,255, 1.0, -2.0, 1.75, 1.5, 0, 0.25), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

			if (tr.fraction != 1)
			{
				DecalTrace(RANDOM_LONG(DECAL_BLASTSCORCH1,DECAL_BLASTSCORCH3), &tr);
				g_pRenderManager->AddSystem(new CRenderSystem(tr.endpos+Vector(0,0,1), g_vecZero, Vector(90,0,0), g_iModelIndexAnimSpr1, kRenderTransAdd, 255,255,255, 1.0, -0.25, 3, 3, 0, 24, 0.2));
				g_pRenderManager->AddSystem(new CRSModel(tr.endpos, endPos, g_vecZero, -1, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_BOOTS01,PLR_GIB_BOOTS04), 0, 0, kRenderNormal, kRenderFxNone, 255,255,255, 1.0, 0.0, 1.0, 0.0, 0, TEMP_ENT_LIFE), RENDERSYSTEM_FLAG_DRAWALWAYS | RENDERSYSTEM_FLAG_NOCLIP, -1);

				if (!UTIL_PointIsFar(startPos, 1.0))
				{
					// gibs
					FX_TempModel(startPos, RANDOM_FLOAT(200, 450), false, g_iModelIndexPlrGibModel, PLR_GIB_HEAD, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_WHITESMOKE);
					FX_TempModel(startPos, RANDOM_FLOAT(200, 450), false, g_iModelIndexPlrGibModel, PLR_GIB_CHEST, SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, 1.25, TEMP_ENT_LIFE, BOUNCE_METAL, FTENT_SMOKETRAIL, TENT_TRAIL_RS_WHITESMOKE);

					for (int i = 0; i < 5; i++)		
						FX_TempModel(startPos, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_OTHER01, PLR_GIB_OTHER07), SKIN_PLR_GIB_METALL_SMOKED, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, FTENT_SMOKETRAIL, TENT_TRAIL_SMOKE);

					for (int j = 0; j < 5; j++)		
						FX_TempModel(startPos, RANDOM_FLOAT(200, 500), false, g_iModelIndexPlrGibModel, RANDOM_LONG(PLR_GIB_METALL01, PLR_GIB_METALL08), SKIN_PLR_GIB_METALL_RUSTED, kRenderNormal, kRenderFxNone, 255,255,255, 255, RANDOM_FLOAT(1.5,2.5), TEMP_ENT_LIFE, 0, 0, 0);

					g_pRenderManager->AddSystem(new CPSBlastCone(24, 128, startPos, startPos, Vector(1,1,1), 5, 120, 128,128,128, 0.25, -0.2, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_5, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
					g_pRenderManager->AddSystem(new CPSSparkShower(RANDOM_LONG(3,5), RANDOM_FLOAT(1.5,2.2), RANDOM_FLOAT(320, 400), SPARKSHOWER_SPARKS, startPos, Vector(0,0,1), VECTOR_CONE_LOTS, g_iModelIndexPartRed, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY | RENDERSYSTEM_FLAG_DRAWDECAL, -1);
					g_pRenderManager->AddSystem(new CPSSparks(48, startPos, RANDOM_FLOAT(0.4,0.6), RANDOM_FLOAT(0.03, 0.05), 0, RANDOM_LONG(300,400), RANDOM_FLOAT(1.2,2.0), 255,255,255, 1, -0.1, g_iModelIndexAnimSpr1, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
					DynamicLight(startPos, RANDOM_FLOAT(340,390), 0,128,255, 0.25, 200.0);

					switch (RANDOM_LONG(0,3))
					{
						case 0: g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(24,28), RANDOM_FLOAT(56,68), tr.endpos+Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_20DEGREES, 12, 20, 255,255,255, 0.4, -0.3, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_3, kRenderTransAdd, RANDOM_FLOAT(3.5, TEMP_ENT_LIFE)), 0, -1); break;
						case 1: g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(24,28), RANDOM_FLOAT(56,68), tr.endpos+Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_20DEGREES, 12, 20, 255,255,255, 0.4, -0.3, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_2, kRenderTransAdd, RANDOM_FLOAT(3.5, TEMP_ENT_LIFE)), 0, -1); break;
						case 2: g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(24,28), RANDOM_FLOAT(56,68), tr.endpos+Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_20DEGREES, 12, 20, 0,0,0, 0.4, -0.3, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, RANDOM_FLOAT(3.5, TEMP_ENT_LIFE)), 0, -1); break;
						case 3: g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(24,28), RANDOM_FLOAT(56,68), tr.endpos+Vector(0,0,16), Vector(0,0,1), VECTOR_CONE_20DEGREES, 12, 20, 0,0,0, 0.4, -0.3, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_1, kRenderTransAlpha, RANDOM_FLOAT(3.5, TEMP_ENT_LIFE)), 0, -1);	break;
					}	

					switch (RANDOM_LONG(0,2))
					{
						case 0:	EMIT_SOUND(0, startPos, CHAN_VOICE, "weapons/explode_lightningball.wav", 1.0, ATTN_NORM, 0, 120 ); break;
						case 1:	EMIT_SOUND(0, startPos, CHAN_BODY, "weapons/explode_lightning.wav", 1.0, ATTN_NORM, 0, 120 ); break;
						case 2:	EMIT_SOUND(0, startPos, CHAN_ITEM, "weapons/fire_lightningball.wav", 1.0, ATTN_NORM, 0, 120 ); break;
					}
				}
			}
		}
		break;
	}
return 1;
}

//===================//
//weapon fire effects//
//===================//
int __MsgFunc_FireGun(const char *pszName, int iSize, void *pbuf)
{
	vec3_t view_ofs, origin, vecForward, dir;

	BEGIN_READ( pbuf, iSize );
	origin.x = READ_COORD();
	origin.y = READ_COORD();
	origin.z = READ_COORD();
	short EntIndex = READ_SHORT();
	byte Animation = READ_BYTE();
	byte m_iGunType = READ_BYTE();
	byte m_iAltFire = READ_BYTE();

	const struct cl_entity_s *gun = gEngfuncs.GetEntityByIndex(EntIndex);

	if (!gun)
		return 0;

	if (!EntIndex)
		return 0;

	if (EV_IsLocal(EntIndex))
		gEngfuncs.pEventAPI->EV_WeaponAnimation(Animation, 0);

	switch (m_iGunType)
	{
		case TYRIANGUN_MACHINEGUN:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_39, 0.0, 0.1+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(60,80), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 8, 24, 1,1,1, 0.2, -0.3, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_8, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_machinegun.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(160,180), 180,100,0, 0.1, 170.0);
				FX_BrassShell((float*)&gun->attachment[1], origin[YAW], UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?TE_BOUNCE_NULL:TE_BOUNCE_SHELL, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?SHELL_RIFLE_LOD:SHELL_RIFLE_762, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?(TEMP_ENT_LIFE*0.5):TEMP_ENT_LIFE);
			}
		}
		break;

		case TYRIANGUN_MINIGUN:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_27, 0.0, 0.15+RANDOM_FLOAT(0,0.12), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(60,80), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 8, 24, 200,200,200, 0.25, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_8, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(180,200), 180,100,0, 0.1, 170.0);
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_minigun.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				FX_BrassShell((float*)&gun->attachment[1], origin[YAW], UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?TE_BOUNCE_NULL:TE_BOUNCE_SHELL, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?SHELL_RIFLE_LOD:SHELL_RIFLE_87MM, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?(TEMP_ENT_LIFE*0.5):TEMP_ENT_LIFE);
			}
		}
		break;
		
		case TYRIANGUN_VULCAN_MINIGUN:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_35, 0.0, 0.1+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(75,90), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 12, 32, 1,1,1, 0.2, -0.3, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_4, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(190,215), 180,230,60, 0.1, 170.0);
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_vulcan.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				FX_BrassShell((float*)&gun->attachment[1], origin[YAW], UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?TE_BOUNCE_NULL:TE_BOUNCE_SHOTSHELL, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?SHELL_SHOTGUN_LOD:SHELL_SHOTGUN_RED, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?(TEMP_ENT_LIFE*0.5):TEMP_ENT_LIFE);
			}
		}
		break;
		
		case TYRIANGUN_HEAVY_TURRET:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_34, 0.0, 0.2+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(60,80), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 12, 36, 200,200,200, 0.25, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(220,240), 180,120,0, 0.1, 170.0);
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_heavy_turret.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW, 0, RANDOM_LONG(98, 104));
				FX_BrassShell((float*)&gun->attachment[1], origin[YAW], UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?TE_BOUNCE_NULL:TE_BOUNCE_SHELL, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?SHELL_RIFLE_BIG_LOD:SHELL_RIFLE_14MM, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?(TEMP_ENT_LIFE*0.5):TEMP_ENT_LIFE);
			}
		}
		break;

		case TYRIANGUN_REPEATER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_23, 0.0, 0.15+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(75,90), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 12, 32, 1,1,1, 0.3, -0.5, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_4, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(220,245), 180,180,0, 0.1, 170.0);
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_repeater.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				FX_BrassShell((float*)&gun->attachment[1], origin[YAW], UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?TE_BOUNCE_NULL:TE_BOUNCE_SHOTSHELL, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?SHELL_SHOTGUN_LOD:SHELL_SHOTGUN_GREEN, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?(TEMP_ENT_LIFE*0.5):TEMP_ENT_LIFE);
			}
		}
		break;
		
		case TYRIANGUN_BANANA:
		{
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_banana.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));

				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(60,80), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 16, 48, 200,200,200, 0.25, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;
		
		case TYRIANGUN_TOILET:
		{
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_toilet.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));

				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(60,80), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 16, 48, 200,200,200, 0.3, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_1, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case TYRIANGUN_SHOCK_CANNON:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartViolet, PARTICLE_VIOLET_0, 0.0, 0.1+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_shock_cannon.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(210,230), 60,0,98, 0.1, 170.0);
			}
		}
		break;
		
		case TYRIANGUN_RAILGUN:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_4, 0.0, 0.1+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_railgun.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(120,140), 200,140,0, 0.1, 170.0);
			}
		}
		break;
		
		case TYRIANGUN_MEZON_CANNON:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartViolet, PARTICLE_VIOLET_1, 0.0, 0.25+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				gEngfuncs.pEfxAPI->R_SparkStreaks((float*)&gun->attachment[0],RANDOM_FLOAT(12,16),-180,180);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_mezoncannon.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(180,210), 40,20,64, 0.1, 170.0);
			}
		}
		break;

		case TYRIANGUN_SUPER_RAILGUN:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartGreen, PARTICLE_GREEN_3, 0.0, 0.17+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				gEngfuncs.pEfxAPI->R_BeamEnts(gun->index | 0x1000, gun->index | 0x2000, g_iModelIndexBeamsAll, 0.1, RANDOM_FLOAT(0.2,0.35), 0.7, 0.8, 25, BLAST_SKIN_TELEENTER, 0, 255, 255, 255);
				gEngfuncs.pEfxAPI->R_BeamEnts(gun->index | 0x1000, gun->index | 0x3000, g_iModelIndexBeamsAll, 0.1, RANDOM_FLOAT(0.2,0.35), 0.7, 0.8, 25, BLAST_SKIN_TELEENTER, 0, 255, 255, 255);
				gEngfuncs.pEfxAPI->R_BeamEnts(gun->index | 0x1000, gun->index | 0x4000, g_iModelIndexBeamsAll, 0.1, RANDOM_FLOAT(0.2,0.35), 0.7, 0.8, 25, BLAST_SKIN_TELEENTER, 0, 255, 255, 255);
				gEngfuncs.pEfxAPI->R_SparkStreaks((float*)&gun->attachment[0],RANDOM_FLOAT(12,16),-180,180);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_superrailgun.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(140,160), 0,160,110, 0.1, 300.0);
			}
		}
		break;
		
		case TYRIANGUN_LASER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_0, 0.0, 0.17+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(12,16), RANDOM_FLOAT(40,60), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_3DEGREES, 8, -16, 128,128,128, 1.0, -1.5, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_laser.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(150,170), 240,0,0, 0.1, 300.0);
			}
		}
		break;
		
		case TYRIANGUN_GAUSS:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_2, 0.0, 0.20+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(8,12), RANDOM_FLOAT(190, 220), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(1,2), 0, 255,255,255, 1.0, -0.5, g_iModelIndexPartRed, FALSE, PARTICLE_RED_12, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_gauss.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(210,240), 240,170,30, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_LIGHTNING:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_4, 0.0, 0.15+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				gEngfuncs.pEfxAPI->R_BeamEnts(gun->index | 0x2000, gun->index | 0x1000, g_iModelIndexBeamsAll, 0.15, RANDOM_FLOAT(0.2,0.3), 0.7, 0.8, 25, BLAST_SKIN_SHOCKWAVE, 0, 255, 255, 255);
				gEngfuncs.pEfxAPI->R_BeamEnts(gun->index | 0x3000, gun->index | 0x1000, g_iModelIndexBeamsAll, 0.15, RANDOM_FLOAT(0.2,0.3), 0.7, 0.8, 25, BLAST_SKIN_SHOCKWAVE, 0, 255, 255, 255);
			}

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_lightning.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(230,260), 255,255,30, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_MULTI_CANNON:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_9, 0.0, 0.07+RANDOM_FLOAT(0,0.07), true);
			FX_MuzzleFlashSprite((float*)&gun->attachment[1], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_9, 0.0, 0.07+RANDOM_FLOAT(0,0.07), true);
			FX_MuzzleFlashSprite((float*)&gun->attachment[2], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_9, 0.0, 0.07+RANDOM_FLOAT(0,0.07), true);
			
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_multicannon.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(210,240), 255,0,0, 0.1, 300.0);
			}
		}
		break;
		
		case TYRIANGUN_PLASMA:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartGreen, PARTICLE_GREEN_4, 0.0, 0.05+RANDOM_FLOAT(0,0.05), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(40,50), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 5, 15, 255,255,255, 0.4, -0.5, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_plasma.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(200,220), 0,190,20, 0.1, 300.0);

			}
		}
		break;
		
		case TYRIANGUN_PULSE_CANNON:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexAnimSpr2, 0, 24.0, 0.12+RANDOM_FLOAT(0,0.1), true);
			FX_MuzzleFlashSprite((float*)&gun->attachment[1], EntIndex, 0, g_iModelIndexAnimSpr2, 0, 24.0, 0.12+RANDOM_FLOAT(0,0.1), true);
			FX_MuzzleFlashSprite((float*)&gun->attachment[2], EntIndex, 0, g_iModelIndexAnimSpr2, 0, 24.0, 0.12+RANDOM_FLOAT(0,0.1), true);
			FX_MuzzleFlashSprite((float*)&gun->attachment[3], EntIndex, 0, g_iModelIndexAnimSpr2, 0, 24.0, 0.12+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_pulsecannon.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(210,240), 140,90,250, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_CHARGE_CANNON:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartGreen, PARTICLE_GREEN_6, 0.0, 0.15+RANDOM_FLOAT(0,0.12), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(6,9), RANDOM_FLOAT(50,60), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_40DEGREES, 5, 35, 20,212,255, 0.4, -0.5, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_3, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_chargecannon.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(200,220), 20,212,255, 0.1, 300.0);
			}
		}
		break;
		
		case TYRIANGUN_HELL_HOUNDER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_20, 0.0, 0.2+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(12,16), RANDOM_FLOAT(180,200), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 16, 64, 1,1,1, 0.2, -0.3, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_5, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_hellhounder.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(220,250), 220,170,0, 0.1, 300.0);
				VectorMA((float*)&gun->attachment[1], -1, (float*)&gun->attachment[0], dir);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(20,25), (float*)&gun->attachment[1], dir, VECTOR_CONE_5DEGREES, 20, -50, 255,255,255, 1.0, -1.4, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case TYRIANGUN_HELLFIRE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_35, 0.0, 0.25+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(12,16), RANDOM_FLOAT(450,500), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(1.5,2.2), 0, 255,255,255, 1.0, -0.5, g_iModelIndexPartRed, FALSE, PARTICLE_RED_28, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_hellfire.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(230,260), 220,170,0, 0.1, 300.0);
				VectorMA((float*)&gun->attachment[1], -1, (float*)&gun->attachment[0], dir);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(20,25), (float*)&gun->attachment[1], dir, VECTOR_CONE_5DEGREES, 20, -50, 255,255,255, 1.0, -1.4, g_iModelIndexExplosion3, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;
		
		case TYRIANGUN_SHOCK_LASER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_3, 0.0, 0.1+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_shocklaser.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(150,175), 0,0,128, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_NEEDLE_LASER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartGreen, PARTICLE_GREEN_1, 0.0, 0.15+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(100,120), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_10DEGREES, 8, 25, 0,200,0, 0.3, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_1, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_needlelaser.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(150,175), 0,128,0, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_30MMGRENADE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_19, 0.0, 0.10+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_30mmgrenade.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(180,215), 255,192,0, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_M203:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_16, 0.0, 0.12+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(60,80), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 18, 54, 200,200,200, 0.3, -0.4, g_iModelIndexPartBlack, FALSE, PARTICLE_BLACK_0, kRenderTransAlpha, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_m203grenade.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(160,190), 220,170,0, 0.1, 300.0);
				FX_BrassShell((float*)&gun->attachment[1], origin[YAW], UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?TE_BOUNCE_NULL:TE_BOUNCE_SHELL, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?SHELL_GLAUNCHER_LOD:SHELL_M32, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?(TEMP_ENT_LIFE*0.5):TEMP_ENT_LIFE);
			}
		}
		break;

		case TYRIANGUN_HVR_MISSILE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_33, 0.0, 0.12+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_hvrmissile.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(190,222), 200,200,0, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_DISRUPTOR:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_5, 0.0, 0.1+RANDOM_FLOAT(0,0.075), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_disruptor.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(210,240), 0,144,255, 0.1, 300.0);

				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(9,12), RANDOM_FLOAT(60,70), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 5, 25, 255,255,255, 0.2, -0.1, g_iModelIndexAnimSpr5, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case TYRIANGUN_CLUSTER_GUN:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_29, 0.0, 0.12+RANDOM_FLOAT(0,0.075), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_clustergun.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(200,220), 255,144,0, 0.1, 170.0);
				FX_BrassShell((float*)&gun->attachment[1], origin[YAW], UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?TE_BOUNCE_NULL:TE_BOUNCE_SHOTSHELL, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?SHELL_GLAUNCHER_LOD:SHELL_M32_SEC, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?(TEMP_ENT_LIFE*0.5):TEMP_ENT_LIFE);

				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), RANDOM_FLOAT(60,80), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 16, 48, 200,200,200, 0.35, -0.4, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_3, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
			}
		}
		break;

		case TYRIANGUN_SONIC_WAVE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_1, 0.0, 0.12+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_sonicwave.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				gEngfuncs.pEfxAPI->R_BeamEnts(gun->index | 0x1000, gun->index | 0x2000, g_iModelIndexBeamsAll, 0.15, RANDOM_FLOAT(0.3,0.4), 0.7, 0.8, 25, BLAST_SKIN_LIGHTNING, 0, 255, 255, 255);
				gEngfuncs.pEfxAPI->R_BeamEnts(gun->index | 0x1000, gun->index | 0x3000, g_iModelIndexBeamsAll, 0.15, RANDOM_FLOAT(0.3,0.4), 0.7, 0.8, 25, BLAST_SKIN_LIGHTNING, 0, 255, 255, 255);
				gEngfuncs.pEfxAPI->R_BeamEnts(gun->index | 0x1000, gun->index | 0x4000, g_iModelIndexBeamsAll, 0.15, RANDOM_FLOAT(0.3,0.4), 0.7, 0.8, 25, BLAST_SKIN_LIGHTNING, 0, 255, 255, 255);
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(150,170), 0,128,0, 0.1, 200.0);
			}
		}
		break;

		case TYRIANGUN_BIOHAZARD_MISSILE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_17, 0.0, 0.2+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_biohazardmissile.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(200,220), 220,104,0, 0.1, 300.0);

				VectorMA((float*)&gun->attachment[1], -1, (float*)&gun->attachment[0], dir);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(20,25), (float*)&gun->attachment[1], dir, VECTOR_CONE_5DEGREES, 20, -50, 255,255,255, 1.0, -1.4, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}		
		}
		break;

		case TYRIANGUN_TAU_CANNON:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[3], EntIndex, 0, g_iModelIndexPartViolet, PARTICLE_VIOLET_1, 0.0, 0.15+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 0.5))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(12,16), RANDOM_FLOAT(40,60), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_3DEGREES, 9, -16, 255,0,255, 1.0, -1.5, g_iModelIndexAnimSpr6, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(250,280), 160,0,210, 0.1, 300.0);
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_tau_cannon.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
			}		
		}
		break;

		case TYRIANGUN_BEAMSPLITTER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexAnimSpr12, 0, 0.0, 0.30+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(260,290), 0,140,100, 0.1, 300.0);
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_beamsplitter.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
			}		
		}
		break;

		case TYRIANGUN_FLAK_CANNON:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_21, 0.0, 0.25+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(250,280), 160,160,0, 0.1, 300.0);
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_flak_cannon.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				FX_BrassShell((float*)&gun->attachment[1], origin[YAW], UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?TE_BOUNCE_NULL:TE_BOUNCE_SHOTSHELL, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?SHELL_GLAUNCHER_LOD:SHELL_FLAKCANNON, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?(TEMP_ENT_LIFE*0.5):TEMP_ENT_LIFE);

				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(12,16), RANDOM_FLOAT(420,480), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(1.5,2.2), 0, 255,255,255, 1.0, -0.5, g_iModelIndexPartRed, FALSE, PARTICLE_RED_28, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}		
		}
		break;

		case TYRIANGUN_FROSTER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_4, 0.0, 0.12+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(140,160), 30,200,250, 0.1, 300.0);
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_froster.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));

				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(10,12), RANDOM_FLOAT(400,450), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(0.7,1.1), 0, 255,255,255, 0.8, -0.5, g_iModelIndexPartBlue, FALSE, PARTICLE_BLUE_6, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}		
		}
		break;

		case TYRIANGUN_SCORCHER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_37, 0.0, 0.15+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_scorcher.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				gEngfuncs.pEfxAPI->R_SparkShower((float*)&gun->attachment[1]);
				gEngfuncs.pEfxAPI->R_SparkShower((float*)&gun->attachment[2]);
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(220,240), 196,64,0, 0.1, 300.0);
			}		
		}
		break;

		case TYRIANGUN_PHOTONGUN:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_0, 0.0, 0.15+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_photongun.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(200,220), 0,128,128, 0.1, 300.0);
			}		
		}
		break;

		case TYRIANGUN_MICRO_MISSILE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_35, 0.0, 0.15+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_micromissile.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(200,220), 128,128,0, 0.1, 300.0);
			}		
		}
		break;

		case TYRIANGUN_MINI_MISSILE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_36, 0.0, 0.15+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_minimissile.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(220,250), 128,128,0, 0.1, 300.0);
			}		
		}
		break;

		case TYRIANGUN_DEMOLITION_MISSILE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_35, 0.0, 0.15+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_demolitionmissile.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(240,270), 156,44,0, 0.1, 300.0);
			}		
		}
		break;

		case TYRIANGUN_GHOST_MISSILE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartRed, PARTICLE_RED_35, 0.0, 0.15+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_ghostmissile.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(240,270), 156,44,0, 0.1, 300.0);
			}		
		}
		break;

		case TYRIANGUN_NEYTRON_GUN:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_9, 0.0, 0.1+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_neytrongun.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(170,200), 0,150,160, 0.1, 300.0);
			}		
		}
		break;

		case TYRIANGUN_TWIN_LASER:
		{
			if (m_iAltFire) //left gun
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), 20, (float*)&gun->attachment[1], (float*)&gun->attachment[1], Vector(1,1,1), 10, 10, 128,128,128, 0.5, -0.75, g_iModelIndexExplosion4, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(6,9), (float*)&gun->attachment[1], RANDOM_FLOAT(0.3,0.5), RANDOM_FLOAT(0.01, 0.02), 0, 100, 0.8, 128,128,128, 1, 0, g_iModelIndexAnimSpr7, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);


				if (!UTIL_PointIsFar((float*)&gun->attachment[1], 1.0))
				{
					EMIT_SOUND(EntIndex, (float*)&gun->attachment[1], CHAN_WEAPON, "weapons/fire_twin_laser.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
					DynamicLight((float*)&gun->attachment[1], RANDOM_LONG(190,210), 0,128,108, 0.1, 300.0);
				}
			}
			else
			{
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(4,6), 20, (float*)&gun->attachment[0], (float*)&gun->attachment[0], Vector(1,1,1), 10, 10, 128,128,128, 0.5, -0.75, g_iModelIndexExplosion4, TRUE, 0, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);
				g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(6,9), (float*)&gun->attachment[0], RANDOM_FLOAT(0.3,0.5), RANDOM_FLOAT(0.01, 0.02), 0, 100, 0.8, 128,128,128, 1, 0, g_iModelIndexAnimSpr7, kRenderTransAdd, 0.3), RENDERSYSTEM_FLAG_SIMULTANEOUS, -1);

				if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
				{
					EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_twin_laser.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
					DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(190,210), 0,128,108, 0.1, 300.0);
				}
			}
		}
		break;

		case TYRIANGUN_RIPPER:
		{
			g_pRenderManager->AddSystem(new CPSSparks(RANDOM_LONG(16,24), (float*)&gun->attachment[0], RANDOM_FLOAT(0.3,0.5), RANDOM_FLOAT(0.01,0.02), 0, RANDOM_FLOAT(120,150), RANDOM_FLOAT(1,1.5), 255,255,255, RANDOM_FLOAT(1,1.5), 0, g_iModelIndexFire, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))		
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_ripper.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
		}
		break;

		case TYRIANGUN_PLASMA_STORM:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartGreen, PARTICLE_GREEN_4, 0.0, 0.07+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_plasmastorm.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(270,300), 0,150,0, 0.1, 300.0);

				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(6,8), RANDOM_FLOAT(60,80), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 5, 25, 0,200,0, 0.5, -0.6, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_2, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
		
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(16,20), RANDOM_FLOAT(350, 400), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(1.2,2.0), 0, 255,255,255, 1.0, -0.33, g_iModelIndexAnimSpr10, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}
		}
		break;

		case TYRIANGUN_STARBURST:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_5, 0.0, 0.1+RANDOM_FLOAT(0,0.07), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_starburst.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(270,300), 0,150,150, 0.1, 300.0);

				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(6,8), RANDOM_FLOAT(60,80), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, 5, 25, 0,200,200, 0.5, -0.6, g_iModelIndexPartWhite, FALSE, PARTICLE_WHITE_3, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}		
		}
		break;

		case TYRIANGUN_GLUON:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartViolet, PARTICLE_VIOLET_2, 0.0, 0.15+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_gluon.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(270,300), 0,100,150, 0.1, 300.0);
			}		
		}
		break;

		case TYRIANGUN_TRIDENT:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_5, 0.0, 0.1+RANDOM_FLOAT(0,0.08), true);
			FX_MuzzleFlashSprite((float*)&gun->attachment[1], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_5, 0.0, 0.1+RANDOM_FLOAT(0,0.08), true);
			FX_MuzzleFlashSprite((float*)&gun->attachment[2], EntIndex, 0, g_iModelIndexPartBlue, PARTICLE_BLUE_5, 0.0, 0.1+RANDOM_FLOAT(0,0.08), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_trident.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(240,270), 100,100,150, 0.1, 300.0);

				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(16,20), RANDOM_FLOAT(350, 400), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_10DEGREES, RANDOM_FLOAT(1.2,2.0), 0, 255,255,255, 1.0, -0.33, g_iModelIndexAnimSpr1, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_ADDPHYSICS | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
			}		
		}
		break;

		case TYRIANGUN_BFG:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexAnimSpr8, 0.0, 24.0, 0.25+RANDOM_FLOAT(0,0.1), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_bfg.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(310,350), 190,190,0, 0.1, 300.0);
				FX_BrassShell((float*)&gun->attachment[1], origin[YAW], UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?TE_BOUNCE_NULL:TE_BOUNCE_SHOTSHELL, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?SHELL_GLAUNCHER_LOD:SHELL_FLAREGUN, UTIL_PointIsFar((float*)&gun->attachment[0], 0.5)?(TEMP_ENT_LIFE*0.5):TEMP_ENT_LIFE);
			}		
		}
		break;

		case TYRIANGUN_BIORIFLE:
		{
			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				AngleVectors(origin, vecForward, NULL, NULL);
				g_pRenderManager->AddSystem(new CPSBlastCone(RANDOM_LONG(10,12), RANDOM_FLOAT(400,450), (float*)&gun->attachment[0], vecForward, VECTOR_CONE_20DEGREES, RANDOM_FLOAT(0.7,1.1), 0, 255,255,255, 0.8, -0.1, g_iModelIndexPartGreen, FALSE, PARTICLE_GREEN_4, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_SIMULTANEOUS | RENDERSYSTEM_FLAG_CLIPREMOVE | RENDERSYSTEM_FLAG_ADDGRAVITY, -1);
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_biorifle.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_NORM, 0, RANDOM_LONG(98, 104));
			}		
		}
		break;

		case TYRIANGUN_SWEAPON_GUIDED_BOMB:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexMuzzleFlash1, 0, 24.0, 0.2+RANDOM_FLOAT(0,0.12), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_guidedbomb.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(220,250), 244,170,0, 0.1, 300.0);

				VectorMA((float*)&gun->attachment[1], -1, (float*)&gun->attachment[0], dir);
				g_pRenderManager->AddSystem(new CPSBlastCone(64, RANDOM_FLOAT(20,25), (float*)&gun->attachment[1], dir, VECTOR_CONE_5DEGREES, 20, -50, 255,255,255, 1.0, -1.4, g_iModelIndexPartRed, FALSE, PARTICLE_RED_3, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(16,22), (float*)&gun->attachment[1], dir, VECTOR_CONE_5DEGREES, 20, -50, 244,56,0, 1.0, -1.4, g_iModelIndexMuzzleFlash2, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case TYRIANGUN_SWEAPON_PROTON_MISSILE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexAnimSpr1, 0, 24.0, 0.2+RANDOM_FLOAT(0,0.12), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_protonmissile.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(320,350), 0,222,255, 0.1, 300.0);

				VectorMA((float*)&gun->attachment[1], -1, (float*)&gun->attachment[0], dir);
				g_pRenderManager->AddSystem(new CPSBlastCone(48, RANDOM_FLOAT(15,18), (float*)&gun->attachment[1], dir, VECTOR_CONE_4DEGREES, 40, -50, 255,255,255, 1.0, -1.0, g_iModelIndexAnimSpr1, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(18,22), (float*)&gun->attachment[1], dir, VECTOR_CONE_6DEGREES, 30, -40, 255,255,255, 1.0, -1.4, g_iModelIndexMuzzleFlash2, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case TYRIANGUN_SWEAPON_DISPLACER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexMuzzleFlash3, 0, 24.0, 0.2+RANDOM_FLOAT(0,0.12), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_teleport.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(350,380), 100,255,0, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_SWEAPON_SHOCK_WAVE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexMuzzleFlash3, 0, 24.0, 0.2+RANDOM_FLOAT(0,0.12), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_lightningball.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(380,430), 0,64,220, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_SWEAPON_NUCLEAR_LAUNCHER:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexMuzzleFlash3, 0, 24.0, 0.2+RANDOM_FLOAT(0,0.12), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_nuclearmissile.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(380,420), 244,170,0, 0.1, 300.0);

				VectorMA((float*)&gun->attachment[1], -1, (float*)&gun->attachment[0], dir);
				g_pRenderManager->AddSystem(new CPSBlastCone(48, RANDOM_FLOAT(15,18), (float*)&gun->attachment[1], dir, VECTOR_CONE_4DEGREES, 20, -25, 255,255,255, 1.0, -1.0, g_iModelIndexPartRed, FALSE, PARTICLE_RED_33, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
				g_pRenderManager->AddSystem(new CPSBlastCone(32, RANDOM_FLOAT(18,22), (float*)&gun->attachment[1], dir, VECTOR_CONE_6DEGREES, 20, -30, 255,255,255, 1.0, -1.4, g_iModelIndexAnimSpr9, TRUE, 0, kRenderTransAdd, 0.1), RENDERSYSTEM_FLAG_CLIPREMOVE, -1);
			}
		}
		break;

		case TYRIANGUN_SWEAPON_CHRONOSCEPTRE:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexMuzzleFlash3, 0, 24.0, 0.2+RANDOM_FLOAT(0,0.12), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_antimatherialmissile.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(300,340), 244,212,0, 0.1, 300.0);
			}
		}
		break;

		case TYRIANGUN_SWEAPON_SUN_OF_GOD:
		{
			FX_MuzzleFlashSprite((float*)&gun->attachment[0], EntIndex, 0, g_iModelIndexMuzzleFlash3, 0, 24.0, 0.2+RANDOM_FLOAT(0,0.12), true);

			if (!UTIL_PointIsFar((float*)&gun->attachment[0], 1.0))
			{
				EMIT_SOUND(EntIndex, (float*)&gun->attachment[0], CHAN_WEAPON, "weapons/fire_sunofgod_container.wav", RANDOM_FLOAT(0.92, 1.0), ATTN_LOW_HIGH, 0, RANDOM_LONG(98, 104));
				DynamicLight((float*)&gun->attachment[0], RANDOM_LONG(400,450), 255,255,0, 0.1, 300.0);
			}
		}
		break;
	}
return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Client side gibs (func_breakable)
//-----------------------------------------------------------------------------
int __MsgFunc_ThrowGib(const char *pszName, int iSize, void *pbuf)
{
	vec3_t pos, maxs, mins;
	int iGibModel = NULL;
	int iBodyMin = NULL;
	int iBodyMax = NULL;
	int iSkin = NULL;
	int iFlags = NULL;
	int iBounceSnd = NULL;

	BEGIN_READ(pbuf, iSize);
	maxs[0]		= READ_COORD();
	maxs[1]		= READ_COORD();
	maxs[2]		= READ_COORD();
	mins[0]		= READ_COORD();
	mins[1]		= READ_COORD();
	mins[2]		= READ_COORD();
	short modelindex= READ_SHORT();
	float scale		= (float)READ_SHORT()*0.1;
	byte amount		= READ_BYTE();
	byte material	= READ_BYTE();
	byte effect	= READ_BYTE();
	float a	= READ_BYTE();
	END_READ();

	if (effect)
		iFlags = FTENT_SMOKETRAIL;

	if (a < 1)
		a = 255;

	(!modelindex)?(iGibModel = g_iModelIndexGibModel):(iGibModel = modelindex);

	switch (material)
	{
		case matUnbreakableGlass:
		case matGlass:
			iBodyMin = GIB_GLASS_01;
			iBodyMax = GIB_GLASS_08;
			iSkin = SKIN_GIB_GLASS;
			iBounceSnd = BOUNCE_GLASS;
		break;

		case matWood:
			iBodyMin = GIB_WOOD_01;
			iBodyMax = GIB_WOOD_10;
			iSkin = SKIN_GIB_WOOD;
			iBounceSnd = BOUNCE_WOOD;
		break;

		case matCeilingTile:
			iBodyMin = GIB_GLASS_01;
			iBodyMax = GIB_GLASS_08;
			iSkin = SKIN_GIB_TILE;
			iBounceSnd = BOUNCE_CONCRETE;
		break;

		case matRocks:
			iBodyMin = GIB_CONCRETE_01;
			iBodyMax = GIB_CONCRETE_08;
			iSkin = SKIN_GIB_ROCK;
			iBounceSnd = BOUNCE_CONCRETE;
		break;

		case matFlesh:
			iBodyMin = GIB_CONCRETE_01;
			iBodyMax = GIB_CONCRETE_08;
			iSkin = SKIN_GIB_FLESH;
			iBounceSnd = BOUNCE_FLESH;
		break;

		case matCinderBlock:
			iBodyMin = GIB_CONCRETE_01;
			iBodyMax = GIB_CONCRETE_08;
			iSkin = SKIN_GIB_CONCRETE;
			iBounceSnd = BOUNCE_CONCRETE;
		break;

		case matGrate:
			iBodyMin = GIB_GRATE_01;
			iBodyMax = GIB_GRATE_08;
			iSkin = SKIN_GIB_GRATE;
			iBounceSnd = BOUNCE_METAL;
		break;

		case matVent:
			iBodyMin = GIB_VENT_01;
			iBodyMax = GIB_VENT_08;
			iSkin = SKIN_GIB_METALL;
			iBounceSnd = BOUNCE_METAL;
		break;

		case matMetal:
			iBodyMin = GIB_GRATE_01;
			iBodyMax = GIB_GRATE_08;
			iSkin = SKIN_GIB_METALL;
			iBounceSnd = BOUNCE_METAL;
		break;

		case matComputer:
			iBodyMin = GIB_GRATE_01;
			iBodyMax = GIB_VENT_08;
			iSkin = SKIN_GIB_METALL;
			iBounceSnd = BOUNCE_SHRAP;
		break;
	}

	for (int i = 0; i < amount; i++)
	{
		pos[0] = RANDOM_FLOAT(mins[0], maxs[0]);
		pos[1] = RANDOM_FLOAT(mins[1], maxs[1]);
		pos[2] = RANDOM_FLOAT(mins[2], maxs[2]);
		FX_TempModel(pos, RANDOM_FLOAT(300,400), true, iGibModel, RANDOM_LONG(iBodyMin,iBodyMax), iSkin, (a < 255)?kRenderTransTexture:kRenderNormal, kRenderFxNone, 255,255,255, a, scale+(scale*RANDOM_FLOAT(0,0.5)), TEMP_ENT_LIFE, iBounceSnd, iFlags, effect);
	}
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Register all defined messages so the engine may use them
//-----------------------------------------------------------------------------
void HookFXMessages(void)
{
	HOOK_MESSAGE(Trail);
	HOOK_MESSAGE(FireBeam);
	HOOK_MESSAGE(FireGun);
	HOOK_MESSAGE(FireLgtng);

	HOOK_MESSAGE(ViewModel);
	HOOK_MESSAGE(Particles);
	HOOK_MESSAGE(SetFog);
	HOOK_MESSAGE(PartSys);
	HOOK_MESSAGE(Snow);
	HOOK_MESSAGE(DLight);
	HOOK_MESSAGE(ELight);
	HOOK_MESSAGE(SetSky);
	HOOK_MESSAGE(SetRain);
	HOOK_MESSAGE(ItemSpawn);
	HOOK_MESSAGE(StaticEnt);
	HOOK_MESSAGE(StaticSpr);
	HOOK_MESSAGE(Bubbles);
	HOOK_MESSAGE(EnvParticle);
	HOOK_MESSAGE(ThrowGib);
}
