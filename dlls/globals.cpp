#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "soundent.h"
#include "decals.h"

DLL_GLOBAL ULONG		g_ulModelIndexPlayer = 1;
DLL_GLOBAL Vector		g_vecAttackDir;// XDM: OBSOLETE: try to avoid using this at all costs!
DLL_GLOBAL bool			g_DisplayTitle = false;
DLL_GLOBAL int			g_Language = 0;

CGlobalState gGlobalState;

DLL_GLOBAL	unsigned short g_usCaptureObject;
DLL_GLOBAL	unsigned short g_usDomPoint;
DLL_GLOBAL	unsigned short g_usTeleport;
DLL_GLOBAL unsigned short g_usPM_Fall;

int gmsgTrail = 0;
int gmsgFireBeam = 0;
int gmsgFireGun = 0;
int gmsgFireLgtng = 0;
int gmsgMSGManager = 0;
int gmsgWpnIcon = 0;
int gmsgThrowGib = 0;

int gmsgInitHUD = 0;
int gmsgResetHUD = 0;
int gmsgCurWeapon = 0;
int gmsgShake = 0;
int gmsgFade = 0;
int gmsgFlashBattery = 0;
int gmsgShowGameTitle = 0;
int gmsgDamage = 0;
int gmsgItemSpawn = 0;
//int gmsgDamageFx = 0;

int gmsgWeaponList = 0;
int gmsgAmmoX = 0;
int gmsgHudText = 0;
int gmsgDeathMsg = 0;
int gmsgScoreInfo = 0;
int gmsgTeamInfo = 0;
int gmsgTeamScore = 0;
int gmsgDomInfo = 0;// XDM
int gmsgFlagInfo = 0;
int gmsgGameMode = 0;
int gmsgMOTD = 0;
int gmsgServerName = 0;
int gmsgAmmoPickup = 0;
int gmsgWeapPickup = 0;
int gmsgItemPickup = 0;
int gmsgHideWeapon = 0;
int gmsgSetCurWeap = 0;
int gmsgSayText = 0;
int gmsgTextMsg = 0;
int gmsgSetFOV = 0;
int gmsgViewMode = 0;
int gmsgShowMenu = 0;
int gmsgGeigerRange = 0;
int gmsgTeamNames = 0;
int gmsgStatusText = 0;
int gmsgStatusValue = 0;
int gmsgStatusIcon = 0;
int gmsgViewModel = 0;// XDM
int gmsgParticles = 0;
int gmsgPartSys = 0;
int gmsgSpectator = 0;

int gmsgSnow = 0;
int gmsgSetFog = 0;
int gmsgSetSky = 0;
int gmsgSetRain = 0;
int gmsgAudioTrack = 0;
int gmsgSetStaticEnt = 0;
int gmsgGRInfo = 0;
int gmsgGREvent = 0;// XDM3035: awards, combobreakers )


int gmsgUpdateWeapons = 0;// XDM3035
int gmsgUpdateAmmo = 0;
int gmsgStaticSprite = 0;// XDM3035a
int gmsgBubbles = 0;
int gmsgSpeakSnd = 0;
int gmsgPickedEnt = 0;

int gmsgSelBestItem = 0;
int gmsgEnvParticle = 0;// XDM3035c
int gmsgPlayerStats = 0;// XDM3037
