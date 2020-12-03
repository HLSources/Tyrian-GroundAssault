#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

enum MusicPlayerState_e
{
	MST_NONE = 0,// inactive
	MST_NORMAL,// normal mood/state
//	MST_COMBAT,
//	MST_DIALOG,
//	MST_HORROR,
	MST_EVENT,// custom event invoked by map entity
	MST_TRACK,// a start track specified in map properties
	MST_PLAYLIST// custom playlist
};

enum MusicPlayerAsyncCmd_e
{
	MPAC_NONE = 0,
	MPAC_LOADING,
	MPAC_PLAYING,
	MPAC_STOPPING,
	MPAC_UNKNOWN
};

void BGM_StartFrame(void);// call this from game

void BGM_Init(void);
bool BGM_Initialized(void);
void BGM_Shutdown(void);
bool BGM_Play(const char *path, int loop);
bool BGM_PlayStart(void);
bool BGM_Pause(void);
bool BGM_Stop(void);
void BGM_Info(void);
bool BGM_SetVolume(int volume);
bool BGM_SetBalance(int balance);
bool BGM_SetPosition(float percent);
int BGM_GetPosition(void);
signed char BGM_IsPlaying(void);
int BGM_GetTimeMs(void);
int BGM_GetLengthMs(void);
void BGM_GamePaused(int paused);

int BGM_PLS_Load(const char *filename);
void BGM_PLS_Play(int track, int loop);
void BGM_PLS_Next(void);
void BGM_PLS_Prev(void);
void BGM_PLS_List(void);
void BGM_PLS_SetLoopMode(int loop);
char *BGM_PLS_GetTrackName(void);

#define MAX_XPL_ENTRIES		256

extern struct cvar_s *mp3player;
extern struct cvar_s *bgm_driver;
extern struct cvar_s *bgm_quality;
extern struct cvar_s *bgm_dir;
extern struct cvar_s *bgm_volume;
extern struct cvar_s *bgm_pausable;
extern struct cvar_s *bgm_pls_loop;
extern struct cvar_s *bgm_playmaplist;
extern struct cvar_s *bgm_playcustomlist;
extern struct cvar_s *bgm_defplaylist;


#endif // MUSICPLAYER_H
