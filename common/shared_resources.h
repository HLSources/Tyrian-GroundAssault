//====================================================================
//
// Precached global model/sprite/sound indexes should be stored here.
// This cpp/h pair must be shared by server and client DLLs.
//
//====================================================================
#ifndef PRECACHED_RES_H
#define PRECACHED_RES_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

// should really be resource_t rather just int:
//resource_t g_iSpriteLaser = {"sprites/laserbeam.spr", t_model};// other fields should be initially cleared to 0

extern int g_iModelIndexAnimglow01;
extern int g_iModelIndexBubble;
extern int g_iModelIndexMuzzleFlash1;
extern int g_iModelIndexMuzzleFlash2;
extern int g_iModelIndexMuzzleFlash3;

//Ghoul: sprites for new effects
extern int g_iModelIndexAnimSpr0;
extern int g_iModelIndexAnimSpr1;
extern int g_iModelIndexAnimSpr2;
extern int g_iModelIndexAnimSpr3;
extern int g_iModelIndexAnimSpr4;
extern int g_iModelIndexAnimSpr5;
extern int g_iModelIndexAnimSpr6;
extern int g_iModelIndexAnimSpr7;
extern int g_iModelIndexAnimSpr8;
extern int g_iModelIndexAnimSpr9;
extern int g_iModelIndexAnimSpr10;
extern int g_iModelIndexAnimSpr11;
extern int g_iModelIndexAnimSpr12;

extern int g_iModelIndexExplosion0;
extern int g_iModelIndexExplosion1;
extern int g_iModelIndexExplosion2;
extern int g_iModelIndexExplosion3;
extern int g_iModelIndexExplosion4;
extern int g_iModelIndexExplosion5;

extern int g_iModelIndexPartRed;
extern int g_iModelIndexPartGreen;
extern int g_iModelIndexPartBlue;
extern int g_iModelIndexPartViolet;
extern int g_iModelIndexPartWhite;
extern int g_iModelIndexPartBlack;
extern int g_iModelIndexBeamsAll;
extern int g_iModelIndexFire;
extern int g_iModelIndexFlame;
extern int g_iModelIndexPartGibs;

extern int g_iModelIndexExplosionModel;
extern int g_iModelIndexGibModel;
extern int g_iModelIndexLightningFieldModel;
extern int g_iModelIndexGunShellModel;
extern int g_iModelIndexPlrGibModel;

void PrecacheSharedResources(void);

#endif // PRECACHED_RES_H
