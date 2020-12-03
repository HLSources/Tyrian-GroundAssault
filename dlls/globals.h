#ifndef GLOBALS_H
#define GLOBALS_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

//extern DLL_GLOBAL globalvars_t *gpGlobals;

// events
extern DLL_GLOBAL	unsigned short g_usCaptureObject;
extern DLL_GLOBAL	unsigned short g_usDomPoint;
extern DLL_GLOBAL	unsigned short g_usTeleport;
extern DLL_GLOBAL	unsigned short g_usPM_Fall;

// variables
//extern DLL_GLOBAL ULONG		g_ulFrameCount;
extern DLL_GLOBAL ULONG		g_ulModelIndexPlayer;
extern DLL_GLOBAL Vector	g_vecAttackDir;
extern DLL_GLOBAL int		g_iSkillLevel;
extern DLL_GLOBAL bool		g_DisplayTitle;
extern DLL_GLOBAL int		g_Language;
extern DLL_GLOBAL int		g_iWeaponBoxCount;
extern DLL_GLOBAL int		g_iProtocolVersion;
extern DLL_GLOBAL bool		g_ServerActive;

extern DLL_GLOBAL class CWorld	*g_pWorld;// XDM: don't forget to check for NULL!
extern int gEvilImpulse101;

// messages
extern int gmsgTrail;
extern int gmsgFireBeam;
extern int gmsgFireGun;
extern int gmsgFireLgtng;
extern int gmsgMSGManager;
extern int gmsgWpnIcon;
extern int gmsgThrowGib;

extern int gmsgInitHUD;
extern int gmsgResetHUD;
extern int gmsgShake;
extern int gmsgFade;
extern int gmsgFlashBattery;
extern int gmsgShowGameTitle;
extern int gmsgCurWeapon;
extern int gmsgDamage;
//extern int gmsgDamageFX;

extern int gmsgWeaponList;
extern int gmsgAmmoX;
extern int gmsgHudText;
extern int gmsgDeathMsg;
extern int gmsgScoreInfo;
extern int gmsgTeamInfo;
extern int gmsgTeamScore;
extern int gmsgDomInfo;
extern int gmsgFlagInfo;
extern int gmsgGameMode;
extern int gmsgMOTD;
extern int gmsgServerName;
extern int gmsgAmmoPickup;
extern int gmsgWeapPickup;
extern int gmsgItemPickup;
extern int gmsgItemSpawn;
extern int gmsgHideWeapon;
extern int gmsgSetCurWeap;
extern int gmsgSayText;
extern int gmsgTextMsg;
extern int gmsgSetFOV;
extern int gmsgViewMode;
extern int gmsgShowMenu;
extern int gmsgGeigerRange;
extern int gmsgTeamNames;
extern int gmsgStatusText;
extern int gmsgStatusValue;
extern int gmsgStatusIcon;
extern int gmsgViewModel;// XDM
extern int gmsgParticles;
extern int gmsgNXPrintf;
extern int gmsgPartSys;
extern int gmsgSpectator;
//extern int gmsgAllowSpec;
extern int gmsgSnow;
extern int gmsgSetFog;
extern int gmsgSetSky;
extern int gmsgSetRain;
extern int gmsgAudioTrack;
extern int gmsgSetStaticEnt;
extern int gmsgGRInfo;
extern int gmsgGREvent;// XDM3035

extern int gmsgUpdateWeapons;
extern int gmsgUpdateAmmo;
extern int gmsgStaticSprite;
extern int gmsgBubbles;
extern int gmsgSpeakSnd;
extern int gmsgPickedEnt;
extern int gmsgSelBestItem;
extern int gmsgEnvParticle;
extern int gmsgPlayerStats;

#endif // GLOBALS_H
