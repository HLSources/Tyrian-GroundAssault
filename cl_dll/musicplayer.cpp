//=========== Copyright © 1999-2010, xawari, All rights reserved. =============
//
// Client-side dynamically loaded background music player module
// TODO: update library to a newer version
// TODO: rewrite it all (>_<)
//
//=============================================================================

#include "fmod_dyn.h"
#include "fmod_errors.h"
#include "hud.h"
#include "cl_util.h"
#include "cdll_int.h"
#include "musicplayer.h"
#include "vgui_XDMViewport.h"
#include "vgui_MusicPlayer.h"
#include "parsemsg.h"

#define MAX_SOFTWARE_CHANNELS 2// XDM3035 // do we need 4-channel samples support?

static FMOD_INSTANCE *gpFMOD = NULL;
static FSOUND_STREAM *g_pMusicStream = NULL;

bool g_iBGMInitialized = FALSE;

int gMusicStatePlaying = MST_NONE;
int gMusicStateLast = MST_NONE;
byte g_MusicPlayerAsyncCmd = MPAC_NONE;
byte g_MusicPlayerLoopTrack = false;

cvar_t *mp3player = NULL;
cvar_t *bgm_driver = NULL;
cvar_t *bgm_quality = NULL;
cvar_t *bgm_dir = NULL;
cvar_t *bgm_volume = NULL;
cvar_t *bgm_pausable = NULL;
cvar_t *bgm_pls_loop = NULL;// XDM3035
cvar_t *bgm_playmaplist = NULL;
cvar_t *bgm_playcustomlist = NULL;
cvar_t *bgm_defplaylist = NULL;

BOOL bXPLLoaded = FALSE;
char xpl_filename[32];
char gPlayList[MAX_PATH][MAX_XPL_ENTRIES];
int xpl_pos = 0;
int gPlayListNumItems = 0;


//-----------------------------------------------------------------------------
// music player console commands
//-----------------------------------------------------------------------------
void __CmdFunc_BGM_Play(void)
{
	if (CMD_ARGC() < 2)
	{
		CON_PRINTF("Usage: %s <name> [loop 0/1]\n", CMD_ARGV(0));
		return;
	}
	char path[MAX_PATH];
	sprintf(path, "%s/%s/%s.mp3", GET_GAME_DIR(), bgm_dir->string, CMD_ARGV(1));
	gMusicStateLast = gMusicStatePlaying;
	BGM_Play(path, atoi(CMD_ARGV(2)));
	if (g_pMusicStream)
		gMusicStatePlaying = MST_TRACK;
}

void __CmdFunc_BGM_PlayPath(void)
{
	if (CMD_ARGC() < 2)
	{
		CON_PRINTF("Usage: %s <full path> [loop 0/1]\n", CMD_ARGV(0));
		return;
	}
	BGM_Play(CMD_ARGV(1), atoi(CMD_ARGV(2)));
}

void __CmdFunc_BGM_Stop(void)
{
	BGM_Stop();
}

void __CmdFunc_BGM_Pause(void)
{
	BGM_Pause();
}

void __CmdFunc_BGM_Info(void)
{
	BGM_Info();
}

void __CmdFunc_BGM_SetVolume(void)
{
	if (CMD_ARGC() < 2)
	{
		if (gpFMOD)
			CON_PRINTF("music volume: %d\n", gpFMOD->FSOUND_GetVolume(FSOUND_ALL));
	}
	else
		BGM_SetVolume(atoi(CMD_ARGV(1)));
}

void __CmdFunc_BGM_Position(void)
{
	if (strlen(CMD_ARGV(1)) <= 0)
	{
//		if (stream != NULL)//( && FSOUND_IsPlaying(FSOUND_ALL))
//			CON_PRINTF("Current track position %f%%\n", gpFMOD->FSOUND_Stream_GetTime(stream)/gpFMOD->FSOUND_Stream_GetLengthMs(stream)*100);
//		else
			CON_PRINTF("Usage: %s <1-100>\n", CMD_ARGV(0));
	}
	else
		BGM_SetPosition(atof(CMD_ARGV(1)));
}



//-----------------------------------------------------------------------------
// playlist-related console commands
//-----------------------------------------------------------------------------
void __CmdFunc_BGM_PLS_Load(void)
{
	if (CMD_ARGC() < 2)
	{
/*		if (bXPLLoaded)
		{
			BGM_PLS_Play(-1, -1);
		}
		else*/
			CON_PRINTF("Usage: %s <filename> [loop 0/1]\n", CMD_ARGV(0));
	}
	else
	{
		BGM_PLS_Load(CMD_ARGV(1));
//		BGM_PLS_Play(0, atoi(CMD_ARGV(2)));
	}
}

void __CmdFunc_BGM_PLS_Play(void)
{
	if (CMD_ARGC() < 2)
	{
		if (bXPLLoaded)
		{
			CON_PRINTF("Playing current list\n");
			BGM_PLS_Play(-1, -1);
		}
		else
			CON_PRINTF("Usage: %s <filename> [loop 0/1]\n", CMD_ARGV(0));
	}
	else
	{
		BGM_PLS_Load(CMD_ARGV(1));
		BGM_PLS_Play(0, atoi(CMD_ARGV(2)));
	}
}

/*void __CmdFunc_BGM_PLS_SoundTrack(void)
{
	if (strlen(CMD_ARGV(1)) <= 0)
	{
		if (bXPLLoaded)
		{
			BGM_PLS_Play(xpl_filename, (gpFMOD->FSOUND_GetLoopMode(FSOUND_ALL)==FSOUND_LOOP_OFF ? 0:1));
		}
		else
		{
			CON_PRINTF("Usage: %s <playlist>\n", CMD_ARGV(0));
			return;
		}
	}
	char path[MAX_PATH];
	sprintf(path, "%s/%s/%s", GET_GAME_DIR(), bgm_dir->string, CMD_ARGV(1));
	int loop = 0;
	loop = atoi(CMD_ARGV(2));
	BGM_PLS_Play(path, loop);
}*/

void __CmdFunc_BGM_PLS_Next(void)
{
	BGM_PLS_Next();
}

void __CmdFunc_BGM_PLS_Prev(void)
{
	BGM_PLS_Prev();
}

void __CmdFunc_BGM_PLS_List(void)
{
	BGM_PLS_List();
}

void __CmdFunc_BGM_PLS_Track(void)
{
	if (CMD_ARGC() < 2)
	{
		CON_PRINTF("Usage: %s <track#>. Current track: %d\n", CMD_ARGV(0), xpl_pos);
		return;
	}
	BGM_PLS_Play(atoi(CMD_ARGV(1)), -1);
}

void __CmdFunc_BGM_Shutdown(void)
{
	BGM_Shutdown();
}

//-----------------------------------------------------------------------------
// Purpose: Server message: map loaded, trigger fired - play background music
//-----------------------------------------------------------------------------
int __MsgFunc_AudioTrack(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	signed char track = READ_CHAR();
	int loop = READ_BYTE();
	END_READ();
	CON_PRINTF("MsgFunc_AudioTrack: %d %d\n", track, loop);
	if (track == 0)// play track from map playlist
	{
		if (bgm_playmaplist && bgm_playmaplist->value >= 1.0f)// play some music at the start of the game
		{
			int loaded = 0;
			if (bgm_playcustomlist && bgm_playcustomlist->value > 0.0f && bgm_defplaylist && bgm_defplaylist->string)// play player's custom list
			{
				if (xpl_filename && strcmp(xpl_filename, bgm_defplaylist->string) != 0)// a different playlist
					loaded = BGM_PLS_Load(bgm_defplaylist->string);
			}
			else// load map playlist
			{
				const char *level = GET_LEVEL_NAME();
				if (level)
				{
					char s[64];
					strcpy(s, level);
					s[strlen(level)-4] = 0;// '.bsp'
					strcat(s, ".xpl");
					loaded = BGM_PLS_Load(s+5);// skip maps/
				}
			}
			if (loaded > 0)
				BGM_PLS_Play(0, loop);
		}
	}
	else if (track == -1)// pause
	{
		if (mp3player->value > 1)
			BGM_Pause();
		else if (mp3player->value > 0)
			CLIENT_COMMAND("cd pause\n");
	}
	else
	{
		if (mp3player->value > 1)
		{
			char path[MAX_PATH];
			sprintf(path, "%s/%s/%d.mp3", GET_GAME_DIR(), bgm_dir->string, track);
			gMusicStateLast = gMusicStatePlaying;
			BGM_Play(path, loop);
		}
		else if (mp3player->value > 0)
		{
			char str[16];
			sprintf(str, "cd play %d\n", track);
			CLIENT_COMMAND(str);
		}
	}
	return 1;
}

//-----------------------------------------------------------------------------
// background music player functions
//-----------------------------------------------------------------------------
int BGM_UpdateDriver(void)
{
	int driver = (int)bgm_driver->value;
	if (driver > 6 || driver < 0)
	{
		CON_PRINTF("Incorrect driver! Using default.\n");
#ifdef _WIN32
		driver = FSOUND_OUTPUT_DSOUND;// default for windows
#else
		driver = FSOUND_OUTPUT_OSS;// default for x
#endif // _WIN32
	}
	CVAR_SET_FLOAT("bgm_driver", driver);
	return driver;
}

//-----------------------------------------------------------------------------
// Purpose: Initialize music player (load DLL, set parameters, etc.). Once.
//-----------------------------------------------------------------------------
void BGM_Init(void)
{
	// this CVar and MSG MUST be created!
	mp3player			= CVAR_CREATE("mp3player",			"2",			FCVAR_CLIENTDLL | FCVAR_UNLOGGED);
	bgm_playmaplist		= CVAR_CREATE("bgm_playmaplist",	"1",			FCVAR_CLIENTDLL | FCVAR_UNLOGGED);
	bgm_playcustomlist	= CVAR_CREATE("bgm_playcustomlist",	"0",			FCVAR_CLIENTDLL | FCVAR_UNLOGGED);
	bgm_defplaylist		= CVAR_CREATE("bgm_defplaylist",	"default.xpl",	FCVAR_CLIENTDLL | FCVAR_UNLOGGED);

	HOOK_MESSAGE(AudioTrack);

	if (mp3player->value < 2)
		return;

	char path[MAX_PATH];
	sprintf(path, "%s/cl_dlls/fmod.dll", GET_GAME_DIR());
	gpFMOD = FMOD_CreateInstance(path);

	if (!gpFMOD)
	{
		CON_PRINTF("CL_DLL: ERROR! Unable to load '%s'!\n", path);
		mp3player->value = 1.0f;
		CVAR_SET_FLOAT("mp3player", 1.0f);
		return;
	}

	float ver = gpFMOD->FSOUND_GetVersion();
	if (ver != FMOD_VERSION)
		CON_PRINTF("CL_DLL: WARNING! Incorrect fmod.dll version %f, expected %f!\n", ver, FMOD_VERSION);

	bgm_driver		= CVAR_CREATE("bgm_driver",		"2",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL | FCVAR_UNLOGGED);
	bgm_quality		= CVAR_CREATE("bgm_quality",	"1",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL | FCVAR_UNLOGGED);
	bgm_dir			= CVAR_CREATE("bgm_dir",		"music",	FCVAR_ARCHIVE | FCVAR_CLIENTDLL | FCVAR_UNLOGGED);
	bgm_volume		= CVAR_CREATE("bgm_volume",		"255",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL | FCVAR_UNLOGGED);
	bgm_pausable	= CVAR_CREATE("bgm_pausable",	"0",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL | FCVAR_UNLOGGED);
	bgm_pls_loop	= CVAR_CREATE("bgm_pls_loop",	"1",		FCVAR_ARCHIVE | FCVAR_CLIENTDLL | FCVAR_UNLOGGED);

	HOOK_COMMAND("bgm_play",		BGM_Play);
	HOOK_COMMAND("bgm_playpath",	BGM_PlayPath);
	HOOK_COMMAND("bgm_stop",		BGM_Stop);
	HOOK_COMMAND("bgm_pause",		BGM_Pause);
	HOOK_COMMAND("bgm_info",		BGM_Info);
	HOOK_COMMAND("bgm_setvolume",	BGM_SetVolume);
	HOOK_COMMAND("bgm_position",	BGM_Position);
	HOOK_COMMAND("bgm_pls_next",	BGM_PLS_Next);
	HOOK_COMMAND("bgm_pls_prev",	BGM_PLS_Prev);
	HOOK_COMMAND("bgm_pls_list",	BGM_PLS_List);
	HOOK_COMMAND("bgm_pls_track",	BGM_PLS_Track);
	HOOK_COMMAND("bgm_pls_play",	BGM_PLS_Play);
	HOOK_COMMAND("bgm_pls_load",	BGM_PLS_Load);
	HOOK_COMMAND("bgm_shutdown",	BGM_Shutdown);

	try
	{
		gpFMOD->FSOUND_SetDriver(BGM_UpdateDriver());
		gpFMOD->FSOUND_SetBufferSize(200);
		CON_PRINTF("CL_DLL: BGM_Init() <%s>\n", FMOD_ErrorString(gpFMOD->FSOUND_GetError()));
	}
	catch (...)
	{
		printf("*** CL_DLL BGM_Init() exception!\n");
		CON_PRINTF("*** CL_DLL BGM_Init() exception!\n");
//		DBG_FORCEBREAK
	}
	g_iBGMInitialized = TRUE;
}

//-----------------------------------------------------------------------------
// Purpose: For external use
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool BGM_Initialized(void)
{
	return g_iBGMInitialized;
}

//-----------------------------------------------------------------------------
// Purpose: Shutdown
//-----------------------------------------------------------------------------
void BGM_Shutdown(void)
{
	if (gpFMOD)
	{
		if (g_pMusicStream)
		{
			gpFMOD->FSOUND_Stream_Close(g_pMusicStream);
			g_pMusicStream = NULL;
		}

		gpFMOD->FSOUND_Close();
		FMOD_FreeInstance(gpFMOD);
		gpFMOD = NULL;
	}
	g_iBGMInitialized = FALSE;
}

//-----------------------------------------------------------------------------
// Purpose: A stream has ended
// Input  : *stream - A pointer to the stream that ended.
//			*buff - This is NULL for end of stream callbacks, or a string for synch callbacks.
//			len - This is reserved and is always 0 for end and synch callbacks. ignore.
//			param - This is the value passed to FSOUND_Stream_SetEndCallback or FSOUND_Stream_SetSynchCallback as a user data value.
// Output : signed char F_CALLBACKAPI ?
//-----------------------------------------------------------------------------
signed char F_CALLBACKAPI BGM_EndCallback(FSOUND_STREAM *stream, void *buff, int len, int param)
{
	if (gpFMOD == NULL || stream == NULL)
		return 0;

//	int off = gpFMOD->FSOUND_Stream_GetTime(stream);
//	if (buff == NULL || (off >= gpFMOD->FSOUND_Stream_GetLengthMs(stream)))// end of stream callback doesnt have a 'buff' value
	{
		if ((gMusicStatePlaying == MST_PLAYLIST)/* && (gpFMOD->FSOUND_GetLoopMode(FSOUND_ALL) == 0)*/)
		{
			// Optimization: don't unload and reload the same track
			if ((gPlayListNumItems <= 1 || (xpl_pos < gPlayListNumItems-1 && strcmp(gPlayList[xpl_pos], gPlayList[xpl_pos+1]) == 0))
				&& bgm_pls_loop->value > 0.0f)// just loop the track
			{
				if (gPlayListNumItems > 1)// found duplicate entry, next track is the same!
					++xpl_pos;// advance to prevent eternal loop

//				CON_DPRINTF("CL: BGM_EndCallback() looping track\n");
				gpFMOD->FSOUND_Stream_Play(FSOUND_FREE, g_pMusicStream);
				return TRUE;
			}

//			gpFMOD->FSOUND_Stream_Stop(stream);
			BGM_Stop();
			gpFMOD->FSOUND_Stream_Close(stream);// XDM3035a TESTME
			if (g_pMusicStream == stream)
				g_pMusicStream = NULL;
			else
				CON_DPRINTF("CL: BGM_EndCallback() stream differs from g_pMusicStream!\n");

			BGM_PLS_Next();
		}
		else if (gMusicStatePlaying != MST_NONE)
		{
			//if (gMusicStatePlaying == MST_TRACK)
			if (g_MusicPlayerLoopTrack)// XDM3037
			{
				g_MusicPlayerAsyncCmd = MPAC_LOADING;// this will eventually force player to PlayStart()
			}
			else
			{
				gMusicStatePlaying = MST_NONE;
				CON_PRINTF("CL: Music state cleared.\n");

				if (bXPLLoaded && gMusicStateLast == MST_PLAYLIST)// a playlist was loaded before a music event ocurred
					BGM_PLS_Play(-1, -1);// resume
			}
		}
	}
	return 1;// ignored
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *path - absolute path (for normal user playlist support)
//			loop - 
// Note   : Should not change or use gMusicStatePlaying/last
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool BGM_Play(const char *path, int loop)
{
	if (path == NULL)
		return false;

#ifdef CLDLL_NEWFUNCTIONS
	if (mp3player->value == 1.0f)
	{
		gEngfuncs.pfnMP3_InitStream(path, 0/*bgm_pls_loop->value*/);// UNTESTED
		CLIENT_COMMAND("mp3 play\n");
		return true;
	}
#endif

	if (!g_iBGMInitialized)
		BGM_Init();

	if (!g_iBGMInitialized)// still have problems?
		return false;

	CLIENT_COMMAND("mp3 stop\n");// stop soundtrack/title song played by HL engine

//	ASSERT(g_MusicPlayerAsyncCmd == MPAC_NONE);

	try
	{
		unsigned int hz = 22050;
		if (bgm_quality->value > 0.0f)
			hz = 44100;

		if (!gpFMOD->FSOUND_Init(hz, MAX_SOFTWARE_CHANNELS, 0/*FSOUND_INIT_GLOBALFOCUS*/))
		{
			CON_PRINTF("CL: Unable to initialize fmod player!\n");
			return false;
		}
		gpFMOD->FSOUND_SetOutput(BGM_UpdateDriver());
		gpFMOD->FSOUND_SetPan(FSOUND_ALL, FSOUND_STEREOPAN);

	//	Pathname_Convert(path);// works fine without this
		g_pMusicStream = gpFMOD->FSOUND_Stream_OpenFile(path, FSOUND_LOOP_OFF|FSOUND_NONBLOCKING, 0);

		if (g_pMusicStream)
		{
			if (loop != -1)
				g_MusicPlayerLoopTrack = loop;
			CON_PRINTF("BGM: loading '%s' <loop: track %d list %d> <%s>\n", path, loop, (int)bgm_pls_loop->value, FMOD_ErrorString(gpFMOD->FSOUND_GetError()));
			g_MusicPlayerAsyncCmd = MPAC_LOADING;
			return true;
		}
	}
	catch (...)
	{
		CON_PRINTF("*** CL_DLL BGM_Play() exception!\n");
//		DBG_FORCEBREAK
	}
	g_MusicPlayerAsyncCmd = MPAC_NONE;
	return false;
}

// XDM3037: async
bool BGM_PlayStart(void)
{
	ASSERT(g_MusicPlayerAsyncCmd == MPAC_LOADING);
	try
	{
		if (g_pMusicStream)
		{
			int vol = (int)bgm_volume->value;
			if (vol)
				gpFMOD->FSOUND_SetVolume(FSOUND_ALL, vol);

			if (gpFMOD->FSOUND_Stream_Play(FSOUND_FREE, g_pMusicStream) != -1)
			{
				g_MusicPlayerAsyncCmd = MPAC_PLAYING;
				CON_PRINTF("BGM: playing <loop list %s> <%s>\n", bgm_pls_loop->string, FMOD_ErrorString(gpFMOD->FSOUND_GetError()));
				if (gpFMOD->FSOUND_Stream_SetEndCallback(g_pMusicStream, BGM_EndCallback, 0) == FALSE)
					CON_DPRINTF("CL: BGM: SetEndCallback() failed!\n");

				if (gViewPort && gViewPort->GetMusicPlayer())
					gViewPort->GetMusicPlayer()->Update();

				return true;
			}
			else
				CON_PRINTF("CL: Unable to play! (%s)\n", FMOD_ErrorString(gpFMOD->FSOUND_GetError()));
		}
		else
			CON_PRINTF("BGM_PlayStart: null stream! (%s)\n", FMOD_ErrorString(gpFMOD->FSOUND_GetError()));
	}
	catch (...)
	{
		CON_PRINTF("*** CL_DLL BGM_PlayStart() exception!\n");
//		DBG_FORCEBREAK
	}
//	g_MusicPlayerAsyncCmd = MPAC_NONE;
	return false;
}

// called by the game
void BGM_StartFrame(void)
{
	if (gpFMOD == NULL)
		return;

	if (g_MusicPlayerAsyncCmd == MPAC_LOADING)
	{
		if (gpFMOD->FSOUND_Stream_GetOpenState(g_pMusicStream) == 0)// success
			if (BGM_PlayStart() == false)
				g_MusicPlayerAsyncCmd = MPAC_NONE;
	}
	else if (g_MusicPlayerAsyncCmd == MPAC_PLAYING)
		g_MusicPlayerAsyncCmd = MPAC_NONE;
	else if (g_MusicPlayerAsyncCmd == MPAC_STOPPING)
		g_MusicPlayerAsyncCmd = MPAC_NONE;// UNDONE: async
}

bool BGM_Pause(void)
{
/*	if (mp3player->value == 1.0f)
	{
there's no such command		CLIENT_COMMAND("mp3 pause\n");
	}*/

	if (g_pMusicStream)
	{
		if (gpFMOD->FSOUND_SetPaused(FSOUND_ALL, !gpFMOD->FSOUND_GetPaused(FSOUND_ALL)) == TRUE)
		{
			CON_PRINTF("music (un)paused <%s>\n", FMOD_ErrorString(gpFMOD->FSOUND_GetError()));
			return true;
		}
	}
	return false;
}

bool BGM_Stop(void)
{
	if (mp3player->value == 1.0f)
	{
		CLIENT_COMMAND("mp3 stop\n");
	}

	g_MusicPlayerAsyncCmd = MPAC_STOPPING;// UNDONE: async
	gMusicStatePlaying = MST_NONE;
	if (g_pMusicStream)
	{
		if (gpFMOD->FSOUND_Stream_Stop(g_pMusicStream) == TRUE)
		{
			CON_PRINTF("music stopped <%s>\n", FMOD_ErrorString(gpFMOD->FSOUND_GetError()));
			return true;
		}
	}
	return false;
}

void BGM_Info(void)
{
	if (g_pMusicStream)
	{
		CON_PRINTF("Music player information\n driver: %s, version: %f, %s\n",
			gpFMOD->FSOUND_GetDriverName(gpFMOD->FSOUND_GetDriver()), gpFMOD->FSOUND_GetVersion(), FMOD_ErrorString(gpFMOD->FSOUND_GetError()));
	}
}

bool BGM_SetVolume(int volume)
{
	CVAR_SET_FLOAT("bgm_volume", volume);
//	CVAR_SET_FLOAT("MP3Volume", volume);

	if (gpFMOD)
		if (gpFMOD->FSOUND_SetVolume(FSOUND_ALL, volume) == TRUE)
			return true;

	return false;
}

bool BGM_SetBalance(int balance)
{
	if (gpFMOD)
		if (gpFMOD->FSOUND_SetPan(FSOUND_ALL, balance) == TRUE)
			return true;

	return false;
}

bool BGM_SetPosition(float percent)
{
	if (gpFMOD)
	{
		if (percent < 0)
			percent = 0;
		else if (percent > 100)
			percent = 100;

		if (gpFMOD->FSOUND_Stream_SetTime(g_pMusicStream, (int)((float)gpFMOD->FSOUND_Stream_GetLengthMs(g_pMusicStream)*(float)percent/100.0f)) == TRUE)
			return true;
	}
	return false;
}

int BGM_GetPosition(void)
{
	if (gpFMOD && gpFMOD->FSOUND_Stream_GetLength(g_pMusicStream)>0)
		return (int)(((float)gpFMOD->FSOUND_Stream_GetTime(g_pMusicStream)/(float)gpFMOD->FSOUND_Stream_GetLengthMs(g_pMusicStream))*100.0f);

	return 0;
}

signed char BGM_IsPlaying(void)
{
	if (gpFMOD && g_pMusicStream)
		return (gpFMOD->FSOUND_Stream_GetTime(g_pMusicStream) > 0)?1:0;// does not work gpFMOD->FSOUND_IsPlaying(FSOUND_ALL);

	return 0;
}

int BGM_GetTimeMs(void)
{
	if (gpFMOD && g_pMusicStream)
		return gpFMOD->FSOUND_Stream_GetTime(g_pMusicStream);

	return 0;
}

int BGM_GetLengthMs(void)
{
	if (gpFMOD && g_pMusicStream)
		return gpFMOD->FSOUND_Stream_GetLengthMs(g_pMusicStream);

	return 0;
}

void BGM_GamePaused(int paused)
{
	if (!g_iBGMInitialized)
		return;

	if (bgm_pausable->value <= 0 || gpFMOD == NULL || g_pMusicStream == NULL)
		return;

	if (paused > 0)// player has paused the game
	{
		if (!gpFMOD->FSOUND_GetPaused(FSOUND_ALL))// playing
			gpFMOD->FSOUND_SetPaused(FSOUND_ALL, 1);
	}
	else// player has unpaused the game
	{
		if (gpFMOD->FSOUND_GetPaused(FSOUND_ALL))// paused
			BGM_Pause();
	}
}


//-----------------------------------------------------------------------------
// playlist-related functions
//-----------------------------------------------------------------------------

int BGM_PLS_Load(const char *filename)
{
	if (!g_iBGMInitialized)
		return 0;

	if (filename)
	{
// XDM: allow reloading of a playlist		if (xpl_filename != NULL && strcmp(xpl_filename, filename) == 0)
//			return 1;// already loaded

		char path[MAX_PATH];
		sprintf(path, "%s/%s/%s", GET_GAME_DIR(), bgm_dir->string, filename);

		FILE *f = fopen(path, "r");
		if (!f)
		{
			CON_PRINTF("BGM: Unable to load %s!\n", filename);
			return 0;
		}

		xpl_pos = 0;
		gPlayListNumItems = 0;
		while (fgets(gPlayList[gPlayListNumItems], MAX_PATH, f) != NULL)
		{
			if (!strncmp(gPlayList[gPlayListNumItems], "//", 2) || gPlayList[0][gPlayListNumItems] == '\n')
			{
				continue;// skip comments or blank lines
			}
			else if (!stricmp(gPlayList[gPlayListNumItems], "#EXTM3U\n"))// skip m3u tags
			{
				CON_DPRINTF("M3U playlist format recognized\n");
				continue;
			}
			else if (gPlayList[gPlayListNumItems][0] == '#')//(!strnicmp(playlist[gPlayListNumItems], "#EXTINF", 7))
				continue;

			char *ch = strchr(gPlayList[gPlayListNumItems], '\n');
			if (ch == NULL)// try windows-style
				strchr(gPlayList[gPlayListNumItems], '\r');

			if (ch)// force all strings to end only with file extension
				*ch = '\0';// not \n or something else

			++gPlayListNumItems;

			if (gPlayListNumItems >= MAX_XPL_ENTRIES)
				break;
		}
		fclose(f);
		CON_PRINTF("BGM: %s loaded: %d(max %d) entries.\n", filename, gPlayListNumItems, MAX_XPL_ENTRIES);
		bXPLLoaded = TRUE;
		strcpy(xpl_filename, filename);
		return gPlayListNumItems;
	}
	return 0;
}

void BGM_PLS_Play(int track, int loop)
{
	if (!g_iBGMInitialized)
		return;

	if (!bXPLLoaded)
		CON_PRINTF("BGM: playlist not loaded!\n");

	if (track < 0)// -1 means "play curently selected track"
		track = xpl_pos;

	// now it's safe to validate track ID
	if (track < 0 || track >= gPlayListNumItems)
	{
		CON_PRINTF("BGM: Track number is incorrect!\n");
		return;
	}

	if (gPlayList[track] != NULL)
	{
		xpl_pos = track;

		BGM_PLS_SetLoopMode(loop);

		CON_PRINTF(" BGM track %d: ", xpl_pos);
		if (strnicmp(gPlayList[xpl_pos], "%musicdir%", 10) == 0)// a special tag for XDM
		{
			char path[MAX_PATH];
			sprintf(path, "%s/%s%s", GET_GAME_DIR(), bgm_dir->string, gPlayList[xpl_pos]+10);
			BGM_Play(path, loop);
		}
		else
		{
			char drive[_MAX_DRIVE];
			_splitpath(gPlayList[xpl_pos], drive, NULL, NULL, NULL);
			if (drive[0] != '\0')// this is an absolute path
			{
				BGM_Play(gPlayList[xpl_pos], loop);
			}
			else// relative path (just like winamp understands this)
			{
				char path[MAX_PATH];
				sprintf(path, "%s/%s/%s", GET_GAME_DIR(), bgm_dir->string, gPlayList[xpl_pos]);
				BGM_Play(path, loop);
			}
		}
	}
	gMusicStatePlaying = MST_PLAYLIST;
}

void BGM_PLS_Next(void)
{
	if (!bXPLLoaded)
	{
		CON_PRINTF("BGM: Playlist is not loaded!\n");
		return;
	}
	if (!g_iBGMInitialized)
		return;

	++xpl_pos;

	if (xpl_pos >= gPlayListNumItems)//			gpFMOD->FSOUND_Stream_Stop(g_pMusicStream);
	{
		if (bgm_pls_loop->value > 0.0f)
		{
			xpl_pos = 0;
		}
		else
		{
			xpl_pos = gPlayListNumItems-1;// don't advance
			return;// don't play
		}
	}
	BGM_PLS_Play(-1, -1);
}

void BGM_PLS_Prev(void)
{
	if (!bXPLLoaded)
	{
		CON_PRINTF("BGM: Playlist is not loaded!\n");
		return;
	}
	if (!g_iBGMInitialized)
		return;

	--xpl_pos;

	if (xpl_pos < 0)
	{
		if (bgm_pls_loop->value > 0.0f)
		{
			xpl_pos = gPlayListNumItems-1;
		}
		else
		{
			xpl_pos = 0;// don't advance
			return;// don't play
		}
	}
	BGM_PLS_Play(-1, -1);
}

void BGM_PLS_List(void)
{
	if (!bXPLLoaded)
	{
		CON_PRINTF("BGM: Playlist is not loaded\n");
		return;
	}

	for(int i = 0; i < gPlayListNumItems; ++i)
		CON_PRINTF(" %d: %s\n", i, gPlayList[i]);
}

void BGM_PLS_SetLoopMode(int loop)
{
	if (loop >= 0)// -1 means no change
		bgm_pls_loop->value = (float)loop;
}

char *BGM_PLS_GetTrackName(void)
{
	return gPlayList[xpl_pos];
}
