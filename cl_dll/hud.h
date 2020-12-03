/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef HUD_H
#define HUD_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif
//
// hud.h
//
// class CHud declaration
//
// CHud handles the message, calculation, and drawing the HUD
//
#include "cl_dll.h"
#include "ammo.h"
#include "gamedefs.h"// XDM

#define FADE_TIME 100

#define RGB_GREEN 1
#define RGB_YELLOW 2
#define RGB_BLUE 3
#define RGB_RED 4
#define RGB_CYAN 5

#define DHN_DRAWZERO 1
#define DHN_2DIGITS 2
#define DHN_3DIGITS 4

#define HUDSPRITEINDEX_INVALID	-1
#define MAX_ALPHA					255// used in rare occasions like change of hud elements
#define MIN_ALPHA					127// used during normal hud state
#define FADE_TIME					100
#define HUD_FADE_SPEED				20// default fade rate


typedef struct {
	int x, y;
} POSITION;

#define HUD_MAX_PLAYERS 64// XDM: WTF?!


//
//-----------------------------------------------------
//
struct extra_player_info_t 
{
	short frags;
	short deaths;
	TEAM_ID teamnumber;
	byte observer;// XDM3035c: distinct name
	float lastscoretime;
	byte ready;
};

struct team_info_t 
{
	char name[MAX_TEAM_NAME];
	short frags;
	short deaths;
	short ping;
	short packetloss;
// XDM3035a: not used	short ownteam;
	short players;
	short scores_overriden;
//	byte already_drawn;// XDM: byte
	byte color[3];// XDM3035
	byte colormap;// XDM3035
//	byte id;// XDM3035
};

extern hud_player_info_t	g_PlayerInfoList[MAX_PLAYERS+1];	// player info from the engine
extern extra_player_info_t	g_PlayerExtraInfo[MAX_PLAYERS+1];	// additional player info sent directly to the client dll
//extern byte					g_IsSpectator[MAX_PLAYERS+1];
extern team_info_t			g_TeamInfo[MAX_TEAMS+1];


#define HUD_ACTIVE			1
#define HUD_INTERMISSION	2
#define HUD_DRAW_ALWAYS		4// XDM: ignore HIDEHUD_ALL

#define ANNOUNCEMENT_MSG_LENGTH		128

float HUD_GetFOV(void);

//
//-----------------------------------------------------
//
class CHudBase
{
public:
	POSITION m_pos;
	int m_type;
	int m_iFlags; // active, moving, 
	virtual ~CHudBase() {}
	virtual int Init(void) {return 0;}
	virtual int VidInit(void) {return 0;}
	virtual int Draw(const float &flTime) {return 0;}
	virtual void Think(void) {return;}
	virtual void Reset(void) {return;}
	virtual void InitHUDData(void) {}		// called every time a server is connected to
	virtual bool IsActive(void);
	void SetActive(bool active);
};

struct HUDLIST {
	CHudBase	*p;
	HUDLIST		*pNext;
};

//
//-----------------------------------------------------
//
// CHudSpectator
#include "hud_spectator.h"
// CHudHealth
#include "health.h"

//
//-----------------------------------------------------
//
class CHudAmmo: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	virtual void Think(void);
	virtual void Reset(void);

	int DrawWList(const float &flTime);
	void UpdateCrosshair(const int &mode, const int &zoomed);// XDM
	int InitWeaponSlots(void);// XDM3035b
//	int GetWeaponSlot(const int &iId);
	int GetWeaponSlotPos(const int &iId, int &wslot, int &wpos);

	int MsgFunc_CurWeapon(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_WeaponList(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoX(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_AmmoPickup(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_WeapPickup(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_ItemPickup(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_HideWeapon(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_UpdWeapons(const char *pszName, int iSize, void *pbuf);//XDM3035
	int MsgFunc_UpdAmmo(const char *pszName, int iSize, void *pbuf);

	void SlotInput(int iSlot);
	void _cdecl UserCmd_Slot1(void);
	void _cdecl UserCmd_Slot2(void);
	void _cdecl UserCmd_Slot3(void);
	void _cdecl UserCmd_Slot4(void);
	void _cdecl UserCmd_Slot5(void);
	void _cdecl UserCmd_Slot6(void);
	void _cdecl UserCmd_Slot7(void);
	void _cdecl UserCmd_Slot8(void);
	void _cdecl UserCmd_Close(void);
	void _cdecl UserCmd_NextWeapon(void);
	void _cdecl UserCmd_PrevWeapon(void);
	void _cdecl UserCmd_InvUp(void);// XDM
	void _cdecl UserCmd_InvDown(void);

private:
	char m_szWeaponSlotConfig[128];
	float m_fFade;
	HUD_WEAPON *m_pWeapon;
	int	m_HUD_bucket0;
	int m_HUD_selection;
	int m_iRecentPickedWeapon;
	wrect_t	*m_prc1;
};

//
//-----------------------------------------------------
//
class CHudAmmoSecondary: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual void Reset(void);
	virtual int Draw(const float &flTime);

//	int MsgFunc_SecAmmoVal(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_SecAmmoIcon(const char *pszName, int iSize, void *pbuf);

private:
	enum {
		MAX_SEC_AMMO_VALUES = 4
	};

	int m_HUD_ammoicon; // sprite indices
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];
	float m_fFade;
};

//
//-----------------------------------------------------
//
class CHudGeiger: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_Geiger(const char *pszName, int iSize, void *pbuf);
	
private:
	int m_iGeigerRange;

};

//
//-----------------------------------------------------
//
class CHudTrain: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_Train(const char *pszName, int iSize, void *pbuf);

private:
	HSPRITE m_hSprite;
	int m_iPos;

};

//
//-----------------------------------------------------
//
class CHudItemAccuracy: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemAccuracy(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemAntidote: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemAntidote(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemFireSupressor: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemFireSupressor(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemRadShield: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemRadShield(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemPlasmaShield: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemPlasmaShield(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemLightningField: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemLightningField(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};
//
//-----------------------------------------------------
//
class CHudItemBanana: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemBanana(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemInvisibility: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemInvisibility(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemHaste: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemHaste(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemShieldregen: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemShieldregen(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemShieldStrength: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemShieldStrength(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iAmount;
};

//
//-----------------------------------------------------
//
class CHudItemWeaponPower: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemWeaponPower(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iAmount;
};
//
//-----------------------------------------------------
//
class CHudItemRapidfire: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemRapidfire(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};
//
//-----------------------------------------------------
//
class CHudItemInvulnerability: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemInvulnerability(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudItemQuaddamage: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_ItemQuaddamage(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iTimer;
};

//
//-----------------------------------------------------
//
class CHudWpnIcon: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_WpnIcon(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSpriteFB;
	int	m_iSpriteTrip;
	int	m_iSpriteSpider;
	int	m_iSpriteMortar;
	int	m_iSpriteSat;
	int	m_iSpriteAtom;

	int m_iAmmoFB;
	int m_iAmmoTrip;
	int m_iAmmoSpider;
	int m_iAmmoMortar;
	int m_iAmmoSat;
	int m_iAmmoAtom;
};

//
//-----------------------------------------------------
//
class CHudStatusBar : public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	virtual void Reset(void);

	void ParseStatusString(int line_num);

	int MsgFunc_StatusText(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_StatusValue(const char *pszName, int iSize, void *pbuf);

	cvar_t	*m_pCvarCenterId;

protected:
	enum { 
		MAX_STATUSTEXT_LENGTH = 128,
		MAX_STATUSBAR_VALUES = 8,
		MAX_STATUSBAR_LINES = 2,
	};

	char m_szStatusText[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];// a text string describing how the status bar is to be drawn
	char m_szStatusBar[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];// the constructed bar that is drawn
//	char m_szClientTraceText[MAX_STATUSTEXT_LENGTH];// XDM3035
	int m_iStatusValues[MAX_STATUSBAR_VALUES];// an array of values for use in the status bar
	int m_bReparseString; // set to TRUE whenever the m_szStatusBar needs to be recalculated
	// an array of colors...one color for each line
	unsigned char m_pflNameColors[MAX_STATUSBAR_LINES][3];
};

//
//-----------------------------------------------------
//
#define DEATHNOTICE_MSG_LENGTH		64
//#define KILL_AWARD_TIME				3.0// XDM: UT FX
#define MAX_DEATHNOTICES			32// XDM3035: absolute maximum
//#define DEATHNOTICE_TOP				32

// Death notices
// These are for display only, not for network.
typedef enum 
{
	KILL_NORMAL = 0,// somebody killed somebody
	KILL_LOCAL,		// local player killed somebody
	KILL_SELF,		// somebody killed self
	KILL_TEAM,		// somebody killed a teammate
	KILL_MONSTER,	// monster killed somebody
	KILL_THISPLAYER,// somebody killed local player
	KILL_UNKNOWN,
} CL_KILLTYPE;

struct DeathNoticeItem
{
	char szKiller[MAX_PLAYER_NAME_LENGTH*2];
	char szVictim[MAX_PLAYER_NAME_LENGTH*2];
	int iId;	// the index number of the associated sprite
	byte iKillType;// byte is enough
	float flDisplayTime;
	int KillerColor[3];
	int VictimColor[3];
	int WeaponColor[3];
};

class CHudDeathNotice : public CHudBase
{
public:
	CHudDeathNotice();
	virtual ~CHudDeathNotice();
	virtual int Init(void);
	virtual void InitHUDData(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);

	void FreeData(void);
	void SetupWeaponColor(int *rgb, const byte &iKillType);// XDM: don't bother optimizing to byte[3] because engine still requires integers
	int AddNotice(int killer_index, int victim_index, char *killedwith);

	int MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf);

private:
	DeathNoticeItem *m_pDeathNoticeList;// XDM
	int		m_iNumDeathNotices;// XDM

	int		m_iDefaultSprite;

	char	m_szScoreMessage[DEATHNOTICE_MSG_LENGTH];// XDM
	char	m_szMessage[DEATHNOTICE_MSG_LENGTH];// XDM

	client_textmessage_t m_ScoreMessage;
	client_textmessage_t m_Message;

	cvar_t	*m_pCvarDNNum;// XDM3035
	cvar_t	*m_pCvarDNTime;
	cvar_t	*m_pCvarDNEcho;
	cvar_t	*m_pCvarDNTop;// XDM3035
	int		m_iLastKilledBy;// XDM3035a: move to server?
};

//
//-----------------------------------------------------
//
#define MAX_LINES			32
#define MAX_CHARS_PER_LINE	256/* it can be less than this, depending on char size */
// Left margin
#define LINE_START			10
// allow 20 pixels on either side of the text
#define MAX_LINE_WIDTH		(ScreenWidth - 40)

// TODO
struct SayTextItem
{
	char szLineBuffer[MAX_CHARS_PER_LINE];
	byte StringColor[3];
	byte NameColor[3];
	size_t iNameLength;// it'll be converted to size_t in string operations anyway
};

class CHudSayText : public CHudBase
{
friend class CHudSpectator;
public:
	CHudSayText();
	virtual ~CHudSayText();
	virtual int Init(void);
	virtual void InitHUDData(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);

	void FreeData(void);
	int ScrollTextUp(void);
	void SayTextPrint(const char *pszBuf, byte clientIndex, bool teamonly = false);
	void EnsureTextFitsInOneLineAndWrapIfHaveTo(int line);

	int MsgFunc_SayText(const char *pszName, int iSize, void *pbuf);

	char	m_szLocalizedTeam[MAX_TEAMNAME_LENGTH];// XDM: localized "TEAM" word
/*
	char	m_szLineBuffer[MAX_LINES + 1][MAX_CHARS_PER_LINE];
	byte	m_pflNameColors[MAX_LINES + 1][3];
	byte	m_pflStringColors[MAX_LINES + 1][3];// XDM3035
	int		m_iNameLengths[MAX_LINES + 1];
*/
	float	m_flScrollTime;// the time at which the lines next scroll up
	int		Y_START;
	int		line_height;

private:
	SayTextItem *m_pSayTextList;// XDM
	int		m_iMaxLines;// XDM

	cvar_t	*m_pCvarSayText;
	cvar_t	*m_pCvarSayTextTime;
	cvar_t	*m_pCvarSayTextHighlight;
	cvar_t	*m_pCvarSayTextLines;
};

//
//-----------------------------------------------------
//
class CHudBattery: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);

	int MsgFunc_Battery(const char *pszName, int iSize, void *pbuf);
	
protected:
	HSPRITE	m_hShieldSprite;
	wrect_t	*m_prc1;
	wrect_t	*m_prc2;
	int		m_iBat;	
	float	m_fFade;
};

//
//-----------------------------------------------------
//
class CHudFlashlight: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	int MsgFunc_FlashBat(const char *pszName, int iSize, void *pbuf);
	
protected:
	HSPRITE m_hSprite2;
	wrect_t *m_prc2;

	float	m_flBat;	
	int		m_iBat;	
	float	m_fFade;
	int		m_iWidth;
	int		m_iTyrianGun;
};

//
//-----------------------------------------------------
//
const int maxHUDMessages = 16;
struct message_parms_t
{
	client_textmessage_t	*pMessage;
	float	time;
	int x, y;
	int	totalWidth, totalHeight;
	int width;
	int lines;
	int lineLength;
	int length;
	int r, g, b;
	int text;
	int fadeBlend;
	float charTime;
	float fadeTime;
};

// client_textmessage_t name length
#define MAX_TITLE_NAME		32

//
//-----------------------------------------------------
//
class CHudTextMessage: public CHudBase
{
public:
	virtual int Init(void);
	int MsgFunc_TextMsg(const char *pszName, int iSize, void *pbuf);
};

//
//-----------------------------------------------------
//
class CHudMessage: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	virtual void Reset(void);

	float FadeBlend(float fadein, float fadeout, float hold, float localTime);
	int	XPosition(float x, int width, int lineWidth);
	int YPosition(float y, int height);

	void MessageAdd(const char *pName, float time);
	void MessageAdd(client_textmessage_t *newMessage);
	void MessageDrawScan(client_textmessage_t *pMessage, float time);
	void MessageScanStart(void);
	void MessageScanNextChar(void);

	int MsgFunc_HudText(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_GameTitle(const char *pszName, int iSize, void *pbuf);

private:
	client_textmessage_t		*m_pMessages[maxHUDMessages];
	float						m_startTime[maxHUDMessages];
	message_parms_t				m_parms;
	float						m_gameTitleTime;
	client_textmessage_t		*m_pGameTitle;
	int m_HUD_title_life;
	int m_HUD_title_half;
};

//
//-----------------------------------------------------
//
#define MAX_SPRITE_NAME_LENGTH	24

class CHudStatusIcons: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual void Reset(void);
	virtual int Draw(const float &flTime);
	//had to make these public so CHud could access them (to enable concussion icon)
	//could use a friend declaration instead...
	void EnableIcon(char *pszIconName, unsigned char red, unsigned char green, unsigned char blue);
	void DisableIcon(char *pszIconName);

	int MsgFunc_StatusIcon(const char *pszName, int iSize, void *pbuf);

	enum {
		MAX_ICONSPRITENAME_LENGTH = MAX_SPRITE_NAME_LENGTH,
		MAX_ICONSPRITES = 4,
	};

private:

	typedef struct
	{
		char szSpriteName[MAX_ICONSPRITENAME_LENGTH];
		HSPRITE spr;
		wrect_t rc;
		unsigned char r, g, b;
	} icon_sprite_t;

	icon_sprite_t m_IconList[MAX_ICONSPRITES];
};

//
//-----------------------------------------------------
//
class CHudZoomCrosshair: public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	virtual void Reset(void);
	virtual void InitHUDData(void);
	virtual void Think(void);
	void SetParams(int spr_idx1, int rendermode, float finalfov);// rendermode -1 to disable

private:
	float m_fFinalFOV;
	int m_iTextureIndex1;
	float m_fPlaySoundTime;
	int m_iRenderMode;
	struct model_s *m_pTexture1;
};

//
//-----------------------------------------------------
//
class CHudRocketScreen : public CHudBase
{
public:
	virtual int Init(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);

private:
	HSPRITE	m_hSprText;
	int m_iOffset;
	int m_iFrames;
	int m_iCurFrame;
};

//
//-----------------------------------------------------
//
#define MAX_DOM_POINTS		6
#define DOM_NAME_LENGTH		64
#define DOM_MSG_LENGTH		(64 + DOM_NAME_LENGTH)

class CHudDomDisplay : public CHudBase
{
public:
	virtual int Init(void);
	virtual void InitHUDData(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	void SetEntTeam(int entindex, int teamindex);
	int MsgFunc_DomInfo(const char *pszName, int iSize, void *pbuf);

private:
	int	m_iSprite;
	int m_iNumDomPoints;
	int m_iDomPointEnts[MAX_DOM_POINTS];
	int m_iDomPointTeam[MAX_DOM_POINTS];
	char m_szDomPointNames[MAX_DOM_POINTS][DOM_NAME_LENGTH];
	char m_szMessage[DOM_MSG_LENGTH];
	client_textmessage_t m_Message;
	HSPRITE m_hsprOvwIcon;
};

//
//-----------------------------------------------------
//
#define MAX_CAPTURE_ENTS	2

class CHudFlagDisplay : public CHudBase
{
public:
	virtual int Init(void);
	virtual void InitHUDData(void);
	virtual int VidInit(void);
	virtual int Draw(const float &flTime);
	virtual void Reset(void);
	int MsgFunc_FlagInfo(const char *pszName, int iSize, void *pbuf);
	void SetEntState(int entindex, int team, int state);

private:
	int	m_iFlagNormal;
	int	m_iFlagTaken;
	int	m_iFlagDropped;
	int m_iNumFlags;
	int m_iFlagEnts[MAX_CAPTURE_ENTS];
	int m_iFlagTeam[MAX_CAPTURE_ENTS];
	int m_iFlagState[MAX_CAPTURE_ENTS];
	HSPRITE m_hsprOvwIcon;
};

//
//-----------------------------------------------------
//
class CHud
{
public:
	CHud();
	~CHud();			// destructor, frees allocated memory

	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Think(void);
	virtual int Redraw(const float &flTime, const int &intermission);
	virtual int UpdateClientData(client_data_t *cdata, const float &time);
	virtual void OnGamePaused(int paused);// XDM
	virtual void OnGameActivated(int active);// XDM

	bool PlayerIsAlive(void);// XDM3038a

	void AddHudElem(CHudBase *p);
	int GetHUDBottomLine(void);// XDM3038c

	int DrawHudStringReverse(int xpos, int ypos, int iMinX, char *szString, const int &r, const int &g, const int &b);
	int DrawHudNumber(int x, int y, int iFlags, int iNumber, const int &r, const int &g, const int &b);
	int DrawHudNumberLarge(int x, int y, int iFlags, int iNumber, const int &r, const int &g, const int &b);
	int DrawHudNumberSmall(int x, int y, int iFlags, int iNumber, const int &r, const int &g, const int &b);


	int DrawHudNumberString(int xpos, int ypos, int iMinX, int iNumber, const int &r, const int &g, const int &b);
	int GetNumWidth(const int &iNumber, const int &iFlags);
	float GetUpdatedDefaultFOV(void);// XDM
	float GetSensitivity(void);

	HSPRITE GetSprite(int index) { return (index < 0) ? 0 : m_rghSprites[index]; }
	wrect_t& GetSpriteRect(int index) { return m_rgrcRects[index]; }
	int GetSpriteIndex(const char *SpriteName);	// gets a sprite index, for use in the m_rghSprites[] array

	void IntermissionStart(void);// XDM3035
	void IntermissionEnd(void);// XDM3035
	bool IsSpectator(void);

	void CheckRemainingScoreAnnouncements(void);// XDM3035a
	void CheckRemainingTimeAnnouncements(void);// XDM3035a
	void GameRulesEndGame(void);// XDM3035c
	void GameRulesEvent(int gameevent, short data1, short data2);// XDM3035c

	// user messages
	int _cdecl MsgFunc_GameMode(const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_Logo(const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_ResetHUD(const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_InitHUD( const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_ViewMode( const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_SetFOV(const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_GRInfo(const char *pszName, int iSize, void *pbuf);
	int _cdecl MsgFunc_GREvent(const char *pszName, int iSize, void *pbuf);
//	int _cdecl MsgFunc_Concuss( const char *pszName, int iSize, void *pbuf);

	int	m_iPlayerFrozen; 

	CHudItemAccuracy m_ItemAccuracy;
	CHudItemBanana m_ItemBanana;
	CHudItemInvisibility m_ItemInvisibility;
	CHudItemInvulnerability m_ItemInvulnerability;
	CHudItemQuaddamage m_ItemQuaddamage;
	CHudItemHaste m_ItemHaste;
	CHudItemShieldregen m_ItemShieldregen;
	CHudItemRapidfire m_ItemRapidfire;
	CHudItemAntidote m_ItemAntidote;
	CHudItemLightningField m_ItemLightningField;
	CHudItemRadShield m_ItemRadShield;
	CHudItemShieldStrength m_ItemShieldStrength;
	CHudItemFireSupressor m_ItemFireSupressor;
	CHudItemPlasmaShield m_ItemPlasmaShield;
	CHudItemWeaponPower m_ItemWeaponPower;

	CHudAmmo m_Ammo;
	CHudHealth m_Health;
	CHudSpectator m_Spectator;
	CHudGeiger m_Geiger;
	CHudBattery	m_Battery;
	CHudTrain m_Train;
	CHudFlashlight m_Flash;
	CHudMessage m_Message;
	CHudStatusBar m_StatusBar;
	CHudDeathNotice m_DeathNotice;
	CHudSayText m_SayText;
	CHudAmmoSecondary m_AmmoSecondary;
	CHudTextMessage m_TextMessage;
	CHudStatusIcons m_StatusIcons;
//	CHudLightLevel m_LightLevel;// XDM
	CHudZoomCrosshair m_ZoomCrosshair;
//	CHudRocketScreen m_RocketScreen;
	CHudDomDisplay m_DomDisplay;
	CHudFlagDisplay m_FlagDisplay;

	CHudWpnIcon m_WpnIcon;


	// Screen information
	SCREENINFO m_scrinfo;
	int	m_iWeaponBits;
	int	m_fPlayerDead;
	int m_iIntermission;
	// sprite indexes
	int m_HUD_number_0;
	int m_HUD_sm_number_0;

	HSPRITE m_hsprCursor;
	float	m_flTime;		// the current client time
	float	m_fOldTime;	// the time at which the HUD was last redrawn
	double	m_flTimeDelta;// the difference between flTime and fOldTime
	float	m_flShotTime;
	float	m_flNextAnnounceTime;// XDM3035
	float	m_flTimeLeft;// XDM3037
	int		m_iScoreLeft;
	int		m_iTimeLeftLast;
	int		m_iScoreLeftLast;
	int		m_iFragLimit;
	int		m_iScoreLimit;
	byte	m_iRoundsPlayed;
	byte	m_iRoundsLimit;
	int		m_iPlayerMaxHealth;

	vec3_t	m_vecOrigin;
	vec3_t	m_vecAngles;

	int		m_iKeyBits;
	int		m_iHideHUDDisplay;
	int		m_iFOV;// this should NEVER be zero!
	short	m_iGameType;
	short	m_iGameMode;
	short	m_iGameSkillLevel;
	short	m_iGameFlags;
	short	m_iRevengeMode;
	int		m_iRes;
	int		m_iFontHeight;
	int		m_iHardwareMode;
	short		m_bFrozen;// XDM3037
	// XDM
	int		m_fLastScoreAward;// XDM: last type of award (n in n-kill)
	char	m_szMessageAward[DEATHNOTICE_MSG_LENGTH];// XDM3035
	char	m_szMessageCombo[DEATHNOTICE_MSG_LENGTH];// XDM3035
	char	m_szMessageTimeLeft[ANNOUNCEMENT_MSG_LENGTH];
	char	m_szMessageScoreLeft[ANNOUNCEMENT_MSG_LENGTH];
	char	m_szMessageAnnouncement[ANNOUNCEMENT_MSG_LENGTH];

	client_textmessage_t m_MessageAward;
	client_textmessage_t m_MessageCombo;
	client_textmessage_t m_MessageTimeLeft;
	client_textmessage_t m_MessageScoreLeft;
	client_textmessage_t m_MessageAnnouncement;

	unsigned long m_iDrawColorMain;// 3 bytes RGB for fast conversion/usage
	unsigned long m_iDrawColorRed;
	unsigned long m_iDrawColorBlue;
	unsigned long m_iDrawColorCyan;
	unsigned long m_iDrawColorYellow;

	TEAM_ID m_iTeamNumber;

	int m_iPaused;// game is paused
	int m_iActive;// game is active (not loading or playing demo)
	int m_iFogMode;
	int m_iSkyMode;
	int m_iCameraMode;
	vec3_t m_vecSkyPos;
	entity_state_s	m_LocalPlayerState;// XDM
//	unsigned long m_iFogColor;// RGB
	float m_flFogStart;
	float m_flFogEnd;

	cl_entity_t		*m_pLocalPlayer;// XDM3037

	cvar_t	*m_pCvarStealMouse;
	cvar_t	*m_pCvarDraw;
	cvar_t	*m_pCvarUseTeamColor;// XDM
	cvar_t	*m_pCvarUsePlayerColor;
	cvar_t	*m_pCvarMiniMap;
	cvar_t	*m_pCvarColorMain;
	cvar_t	*m_pCvarColorRed;
	cvar_t	*m_pCvarColorBlue;
	cvar_t	*m_pCvarColorCyan;
	cvar_t	*m_pCvarColorYellow;
	cvar_t	*m_pCvarTakeShots;

private:
	HUDLIST			*m_pHudList;
	HSPRITE			m_hsprLogo;
	int				m_iLogo;
	client_sprite_t	*m_pSpriteList;
	int				m_iSpriteCount;
	int				m_iSpriteCountAllRes;
	float			m_flMouseSensitivity;
//	int				m_iConcussionEffect; 
	// the memory for these arrays are allocated in the first call to CHud::VidInit(), when the hud.txt and associated sprites are loaded.
	// freed in ~CHud()
	HSPRITE *m_rghSprites;	/*[HUD_SPRITE_COUNT]*/// the sprites loaded from hud.txt
	wrect_t *m_rgrcRects;	/*[HUD_SPRITE_COUNT]*/
	char *m_rgszSpriteNames; /*[HUD_SPRITE_COUNT][MAX_SPRITE_NAME_LENGTH]*/
};


extern CHud gHUD;

// ScreenHeight returns the height of the screen, in pixels
#define ScreenHeight	(gHUD.m_scrinfo.iHeight)
// ScreenWidth returns the width of the screen, in pixels
#define ScreenWidth		(gHUD.m_scrinfo.iWidth)

// Use this to set any co-ords in 640x480 space
#define XRES(x)			((int)(float(x) * ((float)ScreenWidth / 640.0f) + 0.5f))
#define YRES(y)			((int)(float(y) * ((float)ScreenHeight / 480.0f) + 0.5f))

// use this to project world coordinates to screen coordinates
#define XPROJECT(x)		((1.0f+(x))*ScreenWidth*0.5f)
#define YPROJECT(y)		((1.0f-(y))*ScreenHeight*0.5f)
// Here x should belong to [-ScreenResPx/2...+ScreenResPx/2]
#define XUNPROJECT(x)	(2*(x/ScreenWidth))
#define YUNPROJECT(y)	(2*(y/ScreenHeight))

extern int g_iAlive;
extern int g_iUser1;
extern int g_iUser2;
extern int g_iUser3;

#endif // HUD_H
