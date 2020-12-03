#ifndef CL_UTIL_H
#define CL_UTIL_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cvardef.h"
#include "cl_enginefuncs.h"
#include "util_vector.h"

/*
#ifdef _MSC_VER
//#pragma warning(disable: 4244)// 'possible loss of data converting float to int'
#pragma warning(disable: 4305)// 'truncation from 'const double' to 'float'
#endif // _MSC_VER
*/

#ifndef ARRAYSIZE
#define ARRAYSIZE(p)		(sizeof(p)/sizeof(p[0]))
#endif

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef clamp// XDM
#define clamp(a,min,max)	((a < min)?(min):((a > max)?(max):(a)))
#endif

//#define fabs(x)	   ((x) > 0 ? (x) : 0 - (x))

#define CBSENTENCENAME_MAX 16


// Macros to hook function calls into the HUD object
#define DECLARE_MESSAGE(y, x) int __MsgFunc_##x(const char *pszName, int iSize, void *pbuf) \
							{ \
								return gHUD.##y.MsgFunc_##x(pszName, iSize, pbuf); \
							}
#define HOOK_MESSAGE(x) gEngfuncs.pfnHookUserMsg(#x, __MsgFunc_##x);

#define DECLARE_COMMAND(y, x) void __CmdFunc_##x(void) \
							{ \
								gHUD.##y.UserCmd_##x(); \
							}
#define HOOK_COMMAND(x, y) gEngfuncs.pfnAddCommand(x, __CmdFunc_##y);


inline void DrawSetTextColor(const byte &r, const byte &g, const byte &b)
{
	gEngfuncs.pfnDrawSetTextColor((float)r/255.0f, (float)g/255.0f, (float)b/255.0f);
}

inline int ConsoleStringLen(const char *string)
{
	int _width, _height;
	DrawConsoleStringLen(string, &_width, &_height);
	return _width;
}

inline int RectWidth(const struct rect_s &r) { return r.right - r.left; }
inline int RectHeight(const struct rect_s &r) { return r.bottom - r.top; }

// sound functions
// Made inlines to avoid conflict with system macros and for overloading
inline void PlaySound(char *szSound, float vol) { gEngfuncs.pfnPlaySoundByName(szSound, vol); }
inline void PlaySound(int iSound, float vol) { gEngfuncs.pfnPlaySoundByIndex(iSound, vol); }
void PlaySoundSuit(char *szSound);
void PlaySoundAnnouncer(char *szSound, float duration);

float UTIL_WeaponTimeBase(void);

int UTIL_SharedRandomLong(const unsigned int &seed, const int &low, const int &high);
float UTIL_SharedRandomFloat(const unsigned int &seed, const float &low, const float &high);

int UTIL_PointContents(const Vector &vec);// XDM3035
float UTIL_WaterLevel(const Vector &position, float minz, float maxz);// XDM3035

float GetSensitivityByFOV(float newfov);
HSPRITE LoadSprite(const char *pszName);
client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount);

//bool StringToVec(const char *str, Vector &vec);
bool StringToVec(const char *str, float *vec);
//bool StringToRGB(const char *str, int &r, int &g, int &b);
bool StringToRGB(const char *str, byte &r, byte &g, byte &b);
bool StringToRGBA(const char *str, byte &r, byte &g, byte &b, byte &a);
bool StringToColor(const char *str, class Color &c);
void GetMeterColor(const float &fMeterValue, byte &r, byte &g, byte &b);

void UnpackRGB(int &r, int &g, int &b, unsigned short colorindex);
void UnpackRGB(byte &r, byte &g, byte &b, unsigned short colorindex);

void Int2RGB(int rgb, int &r, int &g, int &b);
void Int2RGB(DWORD rgb, byte &r, byte &g, byte &b);
void Int2RGBA(DWORD rgb, byte &r, byte &g, byte &b, byte &a);
// unsigned long == unsigned int == __uint32
#define RGBA2INT(r,g,b,a)	((unsigned long)(((byte)(r)|((WORD)((byte)(g))<<8))|(((DWORD)(byte)(b))<<16)|(((DWORD)(byte)(a))<<24)))
#define RGB2INT(r,g,b)		((unsigned long)(((byte)(r)|((WORD)((byte)(g))<<8))|(((DWORD)(byte)(b))<<16)))
#define RGB2R(rgb)			((unsigned char)(rgb))
#define RGB2G(rgb)			((unsigned char)(((unsigned short)(rgb)) >> 8))
#define RGB2B(rgb)			((unsigned char)((rgb)>>16))
#define RGB2A(rgba)			((unsigned char)((rgba)>>24))
#define RGBA2A(rgba)		((unsigned char)((rgba)>>24))
//void rgb2hsv(float R, float G, float B, float &H, float &S, float &V);
//void hsv2rgb(float H, float S, float V, float &R, float &G, float &B);
// XDM3035
void RGB2HSL(float &r, float &g, float &b, float &h, float &s, float &l);// r,g,b 0...1; h,s,l must be != 0 to be calculated
void RGB2HSL(byte &rb, byte &gb, byte &bb, float &h, float &s, float &l);// h,s,l 0...255; must be != 0 to be calculated
void HSL2RGB(float h, float s, float l, float &r, float &g, float &b);// r,g,b 0...1
void HSL2RGB(float h, float s, float l, byte &rb, byte &gb, byte &bb);// r,g,b 0...255

void ScaleColors(int &r, int &g, int &b, const float &a);
void ScaleColors(int &r, int &g, int &b, const int &a);
void ScaleColors(byte &r, byte &g, byte &b, const float &a);
void ScaleColors(byte &r, byte &g, byte &b, const byte &a);

void GetTeamColor(TEAM_ID team, int &r, int &g, int &b);
void GetTeamColor(TEAM_ID team, byte &r, byte &g, byte &b);
void PlayerColor(int halfcolormap, byte &r, byte &g, byte &b);
//void GetPlayerColor(hud_player_info_t *pPlayer, TEAM_ID team, byte &r, byte &g, byte &b);// obsolete
bool GetPlayerColor(int client, byte &r, byte &g, byte &b);

//int UTIL_ListFiles(const char *searchdir, const char *searchname, const char *searchext);
unsigned int UTIL_ListFiles(const char *search);
void Pathname_Convert(char *pathname);

char *LocaliseTextString(const char *msg, char *dst_buffer, int buffer_size);
char *BufferedLocaliseTextString(const char *msg);
char *LookupString(const char *msg_name, int *msg_dest = NULL);
//char *StripEndNewlineFromString(char *str);
void StripEndNewlineFromString(char *str);
char *ConvertCRtoNL(char *str);

void ExtractFileName(const char *fullpath, char *dir, char *name, char *ext);// XDM3030
int LoadModel(const char *pszName, struct model_s *pModel = NULL);

void CL_AllocBeam( const char *model, int ent, Vector start, Vector end );
void CL_ClearBeams( void );

bool CL_IsDead(void);
bool IsValidTeam(const TEAM_ID &team_id);
bool IsActiveTeam(const TEAM_ID &team_id);
bool IsActivePlayer(cl_entity_t *ent);
bool IsActivePlayer(const int &idx);
bool IsValidPlayerIndex(const int &idx);
bool IsSpectator(const int &idx);
bool IsTeamGame(const int &gamegules);
bool IsRoundBasedGame(const int &gamerules);// XDM3037
int GetGameMode(void);
int GetGameFlags(void);
const char *GetGameDescription(const int &gamerules);
char *GetGameRulesIntroText(const short &gametype, const short &gamemode);

float UTIL_PointViewDist(const Vector &point);// XDM3035c
bool UTIL_PointIsFar(const Vector &point, float k);// XDM3035c
bool UTIL_PointIsVisible(const Vector &point, bool check_backplane);// XDM3035

int GetPhysent(int entindex);
void GetEntityPrintableName(int entindex, char *output, const size_t max_len);// XDM3035c
cl_entity_t *GetUpdatingEntity(int entindex);// XDM3035c

const Vector &GetCEntAttachment(const struct cl_entity_s *pEntity, const int attachment);// XDM3038c

extern double g_cl_gravity;// XDM3035

extern Vector g_vecViewOrigin;// XDM: real view point
extern Vector g_vecViewAngles;
extern Vector g_vecViewForward;
extern Vector g_vecViewRight;
extern Vector g_vecViewUp;
extern struct ref_params_s *g_pRefParams;
extern bool g_ThirdPersonView;

#endif // CL_UTIL_H
