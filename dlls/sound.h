#ifndef SOUND_H
#define SOUND_H

#include "pm_materials.h"

#define SPEAKER_START_SILENT			1	// wait for trigger 'on' to start announcements


#define	LFO_SQUARE			1
#define LFO_TRIANGLE		2
#define LFO_RANDOM			3

#define AMBIENT_SOUND_STATIC			0	// medium radius attenuation
#define AMBIENT_SOUND_EVERYWHERE		1
#define AMBIENT_SOUND_SMALLRADIUS		2
#define AMBIENT_SOUND_MEDIUMRADIUS		4
#define AMBIENT_SOUND_LARGERADIUS		8
#define AMBIENT_SOUND_START_SILENT		16
#define AMBIENT_SOUND_NOT_LOOPING		32


#define SENTENCES_FILE		"sound/sentences.txt"// XDM

#define CSENTENCE_LRU_MAX	32		// max number of elements per sentence group
// Sound Utilities
// sentence groups
#define CBSENTENCENAME_MAX		16
#define CVOXFILESENTENCEMAX		1280		// max number of sentences in game. NOTE: this must match CVOXFILESENTENCEMAX in engine\sound.h!!!
extern char gszallsentencenames[CVOXFILESENTENCEMAX][CBSENTENCENAME_MAX];
extern int gcallsentences;

// group of related sentences
typedef struct sentenceg
{
	char szgroupname[CBSENTENCENAME_MAX];
	int count;
	unsigned char rgblru[CSENTENCE_LRU_MAX];
} SENTENCEG;

#define CSENTENCEG_MAX 200			// max number of sentence groups

// for locked doors and buttons
#define SNDCHAN_LOCK		CHAN_ITEM
#define LOCK_SENTENCEWAIT	6


typedef struct locksound_s			// sounds that doors and buttons make when locked/unlocked
{
	string_t	sLockedSound;		// sound a door makes when it's locked
	string_t	sLockedSentence;	// sentence group played when door is locked
	string_t	sUnlockedSound;		// sound a door makes when it's unlocked
	string_t	sUnlockedSentence;	// sentence group played when door is unlocked

	int		iLockedSentence;		// which sentence in sentence group to play next
	int		iUnlockedSentence;		// which sentence in sentence group to play next

	float	flwaitSound;			// time delay between playing consecutive 'locked/unlocked' sounds
	float	flwaitSentence;			// time delay between playing consecutive sentences
	byte	bEOFLocked;				// true if hit end of list of locked sentences
	byte	bEOFUnlocked;			// true if hit end of list of unlocked sentences
} locksound_t;

// runtime pitch shift and volume fadein/out structure

// NOTE: IF YOU CHANGE THIS STRUCT YOU MUST CHANGE THE SAVE/RESTORE VERSION NUMBER
// SEE BELOW (in the typedescription for the class)
typedef struct dynpitchvol
{
	// NOTE: do not change the order of these parameters
	// NOTE: unless you also change order of rgdpvpreset array elements!
	int preset;
	int pitchrun;		// pitch shift % when sound is running 0 - 255
	int pitchstart;		// pitch shift % when sound stops or starts 0 - 255
	int spinup;			// spinup time 0 - 100
	int spindown;		// spindown time 0 - 100
	int volrun;			// volume change % when sound is running 0 - 10
	int volstart;		// volume change % when sound stops or starts 0 - 10
	int fadein;			// volume fade in time 0 - 100
	int fadeout;		// volume fade out time 0 - 100
					// Low Frequency Oscillator
	int	lfotype;		// 0) off 1) square 2) triangle 3) random
	int lforate;		// 0 - 1000, how fast lfo osciallates
	int lfomodpitch;	// 0-100 mod of current pitch. 0 is off.
	int lfomodvol;		// 0-100 mod of current volume. 0 is off.
	int cspinup;		// each trigger hit increments counter and spinup pitch
	int	cspincount;
	int pitch;
	int spinupsav;
	int spindownsav;
	int pitchfrac;
	int vol;
	int fadeinsav;
	int fadeoutsav;
	int volfrac;
	int	lfofrac;
	int	lfomult;

} dynpitchvol_t;

#define CDPVPRESETMAX 27


class CAmbientGeneric : public CBaseEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual int	ObjectCaps(void) { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION); }
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	virtual void OnFreePrivateData(void);

	void EXPORT ToggleUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
//	void EXPORT StartPlayFrom(void);// XDM
	void EXPORT RampThink(void);
	void InitModulationParms(void);
	static TYPEDESCRIPTION m_SaveData[];

	float m_flAttenuation;// attenuation value
	dynpitchvol_t m_dpv;
	BOOL m_fActive;// only TRUE when the entity is playing a looping sound
	BOOL m_fLooping;// TRUE when the sound played will loop
	edict_t *m_pPlayFrom;// SHL: the entity to play from
	int		m_iChannel;// SHL: the channel to play from, for "play from X" sounds
};


class CEnvSound : public CBaseDelay// XDM3035c
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Think(void);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];
	float m_flRadius;
	float m_flRoomtype;
};


#define ANNOUNCE_MINUTES_MIN	0.25
#define ANNOUNCE_MINUTES_MAX	2.25

// ===================================================================================
//
// Speaker class. Used for announcements per level, for door lock/unlock spoken voice.
//
class CSpeaker : public CBaseEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	void EXPORT ToggleUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void EXPORT SpeakerThink(void);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];
	virtual int	ObjectCaps(void) { return (CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION); }
	int	m_preset;// preset number
};




int USENTENCEG_Pick(int isentenceg, char *szfound);
int USENTENCEG_PickSequential(int isentenceg, char *szfound, int ipick, int freset);
void USENTENCEG_InitLRU(unsigned char *plru, int count);
void SENTENCEG_Init(void);
void SENTENCEG_Stop(edict_t *entity, int isentenceg, int ipick);
int SENTENCEG_PlayRndI(edict_t *entity, int isentenceg, float volume, float attenuation, int flags, int pitch);
int SENTENCEG_PlayRndSz(edict_t *entity, const char *szrootname, float volume, float attenuation, int flags, int pitch);
int SENTENCEG_PlaySequentialSz(edict_t *entity, const char *szrootname, float volume, float attenuation, int flags, int pitch, int ipick, int freset);
int SENTENCEG_GetIndex(const char *szrootname);
int SENTENCEG_Lookup(const char *sample, char *sentencenum);

char TEXTURETYPE_Trace(TraceResult *ptr, const Vector &vecSrc, const Vector &vecEnd);// XDM
//float TEXTURETYPE_PlaySound(TraceResult *ptr, const Vector &vecSrc, const Vector &vecEnd, int iBulletType);


void PlayLockSounds(entvars_t *pev, locksound_t *pls, int flocked, float flsoundwait);//int fbutton);
char *ButtonSound(int sound);				// get string of button sound number

const char **MaterialSoundList(Materials soundMaterial, int &soundCount);
void MaterialSoundPrecache(Materials soundMaterial);
void MaterialSoundRandom(edict_t *pEdict, Materials soundMaterial, float volume);

#endif // SOUND_H
