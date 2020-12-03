// Server-client networking protocol constants
// Sources: Quake, Uncle Mike, Half-Life
#ifndef PROTOCOL_H
#define PROTOCOL_H

#define PROTOCOL_VERSION	47

// Half-Life predefined messages differs from Quake protocol, thanks to Uncle Mike for info

// server to client
typedef enum
{
	svc_bad = 0,		// immediately crash client when received
	svc_nop,			// does nothing
	svc_disconnect,		// kick client from server
	svc_changing,		// changelevel by server request
	svc_version,		// [long] server version
	svc_setview,		// [short] entity number
	svc_sound,			// <see code>
	svc_time,			// [float] server time
	svc_print,			// [byte] id [string] null terminated string
	svc_stufftext,		// [string] stuffed into client's console buffer
	svc_setangle,		// [angle angle] set the view angle to this absolute value
	svc_serverdata,		// [long] protocol ...
	svc_lightstyle,		// [index][pattern]
	svc_updateuserinfo,	// [byte] playernum, [string] userinfo
	svc_deltatable,		// [table header][...]
	svc_clientdata,		// [...]
	svc_stopsound,		// <see code>
	svc_updatepings,	// [bit][idx][ping][packet_loss]
	svc_particle,		// [float*3][char*3][byte][byte]
	svc_frame,			// <OBSOLETE>
	svc_spawnstatic,	// creates a static client entity
	svc_event_reliable,	// playback event directly from message, not queue
	svc_spawnbaseline,	// <see code>
	svc_temp_entity,	// <variable sized>
	svc_setpause,		// [byte] 0 = unpaused, 1 = paused
	svc_signonnum,		// [byte] used for the signon sequence
	svc_centerprint,	// [string] to put in center of the screen
	svc_event,			// playback event queue
	svc_soundindex,		// [index][soundpath]
	svc_ambientsound,	// <see code>
	svc_intermission,	// empty message (event)
	svc_modelindex,		// [index][modelpath]
	svc_cdtrack,		// [string] trackname
	svc_serverinfo,		// [string] key [string] value
	svc_eventindex,		// [index][eventname]
	svc_weaponanim,		// [byte]iAnim [byte]body
	svc_bspdecal,		// [float*3][short][short][short]
	svc_roomtype,		// [short] room type
	svc_addangle,		// [angle] add angles when client turn on mover
	svc_usermessage,	// [byte][byte][string] REG_USER_MSG stuff
	svc_packetentities,	// [short][...]
	svc_deltapacketentities,// [short][byte][...] 
	svc_chokecount,		// [byte]
	svc_43,
	svc_deltamovevars,	// [movevars_t]
	svc_45,
	svc_46,
	svc_crosshairangle,	// [byte][byte]
	svc_soundfade,		// [float*4] sound fade parms
	svc_49,
	svc_hltv,
	svc_director,		// <variable sized>
	svc_lastmsg = 64	// start user messages at this point
} servermessages;
// UNDONE: where is svc_filetxferfailed?

// client to server
typedef enum
{
	clc_bad = 0,		// immediately drop client when received
	clc_nop,
	clc_move,			// [[usercmd_t]
	clc_stringcmd,		// [string] message
	clc_delta,			// [byte] sequence number, requests delta compression of message
	clc_resourcelist,	//
	clc_userinfo,		// [[userinfo string]
	clc_fileconsistency,//
	clc_voicedata
} clientmessages;

#define MAX_MESSAGE_STRING	512// from TE_TEXTMESSAGE

#define MAX_VISIBLE_PACKET	256// ?

// additional protocol data
#define MAX_CLIENT_BITS		5
#define MAX_CLIENTS			(1<<MAX_CLIENT_BITS)// 5 bits == 32 clients ( int32 limit )

#define MAX_WEAPON_BITS		5
#define MAX_WEAPONS			(1<<MAX_WEAPON_BITS)// 5 bits == 32 weapons ( int32 limit )

#define MAX_EVENT_BITS		10
#define MAX_EVENTS			(1<<MAX_EVENT_BITS)	// 10 bits == 1024 events (the original Half-Life limit)

#define MAX_MODEL_BITS		11
#define MAX_MODELS			(1<<MAX_MODEL_BITS)	// 11 bits == 2048 models

#define MAX_SOUND_BITS		11
#define MAX_SOUNDS			(1<<MAX_SOUND_BITS)	// 11 bits == 2048 sounds

#define MAX_CUSTOM			1024// max custom resources per level

#define MAX_USER_MESSAGES	191	// 256-svc_lastmsg

#define MAX_DLIGHTS			32	// dynamic lights (rendered per one frame)
#define MAX_ELIGHTS			64	// entity only point lights
#define MAX_LIGHTSTYLES		256	// a byte limit, don't modify
#define MAX_EDICTS			4096	// absolute limit, should be enough. (can be up to 32768)
#define MAX_RENDER_DECALS	4096	// max rendering decals per a level

// sound flags
#define SND_VOLUME			(1<<0)	// a scaled byte
#define SND_ATTENUATION		(1<<1)	// a byte
#define SND_LARGE_INDEX		(1<<2)	// a send sound as short
#define SND_PITCH			(1<<3)	// a byte
#define SND_SENTENCE		(1<<4)	// set if sound num is actually a sentence num
#define SND_STOP			(1<<5)	// stop the sound
#define SND_CHANGE_VOL		(1<<6)	// change sound vol
#define SND_CHANGE_PITCH	(1<<7)	// change sound pitch
#define SND_SPAWNING		(1<<8)	// we're spawning, used in some cases for ambients

// decal flags
#define FDECAL_PERMANENT	0x01	// This decal should not be removed in favor of any new decals
#define FDECAL_CUSTOM		0x02	// This is a custom clan logo and should not be saved/restored
#define FDECAL_DONTSAVE		0x04	// Decal was loaded from adjacent level, don't save it for this level
#define FDECAL_CLIPTEST		0x08	// Decal needs to be clip-tested
#define FDECAL_NOCLIP		0x10	// Decal is not clipped by containing polygon
#define FDECAL_USESAXIS		0x20	// Uses the s axis field to determine orientation (footprints)

// Max number of history commands to send (2 by default) in case of dropped packets
#define NUM_BACKUP_COMMAND_BITS	4
#define MAX_BACKUP_COMMANDS		(1 << NUM_BACKUP_COMMAND_BITS)

// network precision
#define COORD_INTEGER_BITS		14
#define COORD_FRACTIONAL_BITS	5
#define COORD_DENOMINATOR		(1 << (COORD_FRACTIONAL_BITS))
#define COORD_RESOLUTION		(1.0 / (COORD_DENOMINATOR))

// world size
#define MAX_COORD_INTEGER		(16384)	// world half-size, modify with precaution
#define MIN_COORD_INTEGER		(-MAX_COORD_INTEGER)
#define MAX_COORD_FRACTION		( 1.0 - (1.0 / 16.0))
#define MIN_COORD_FRACTION		(-1.0 + (1.0 / 16.0))

#define NORMAL_FRACTIONAL_BITS	11
#define NORMAL_DENOMINATOR		((1 << (NORMAL_FRACTIONAL_BITS)) - 1)
#define NORMAL_RESOLUTION		(1.0 / (NORMAL_DENOMINATOR))

// verify that coordsize.h and worldsize.h are consistently defined
#if (MAX_COORD_INTEGER != (1 << COORD_INTEGER_BITS))
#error MAX_COORD_INTEGER does not match COORD_INTEGER_BITS
#endif

#endif//PROTOCOL_H
