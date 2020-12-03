//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "hud.h"
#include "cl_util.h"

extern "C"
{
void EV_TrainPitchAdjust(struct event_args_s *args);
void EV_ZoomCrosshair(struct event_args_s *args);
void EV_CaptureObject(struct event_args_s *args);
void EV_DomPoint(struct event_args_s *args);
void EV_Teleport(struct event_args_s *args);
void EV_PM_Fall(struct event_args_s *args);
}


/*
======================
Game_HookEvents

Associate script file name with callback functions.  Callback's must be extern "C" so
 the engine doesn't get confused about name mangling stuff.  Note that the format is
 always the same.  Of course, a clever mod team could actually embed parameters, behavior
 into the actual .sc files and create a .sc file parser and hook their functionality through
 that.. i.e., a scripting system.

That was what we were going to do, but we ran out of time...oh well.
======================
*/
/*
===================
EV_HookEvents

See if game specific code wants to hook any events.
===================
*/
void EV_HookEvents(void)
{
	HOOK_EVENT("events/game/captureobject.sc",	EV_CaptureObject);
	HOOK_EVENT("events/game/dompoint.sc",		EV_DomPoint);
	HOOK_EVENT("events/game/teleport.sc",		EV_Teleport);
	HOOK_EVENT("events/train.sc",				EV_TrainPitchAdjust);
	HOOK_EVENT("events/zoom.sc",				EV_ZoomCrosshair);
	HOOK_EVENT("events/pm/fall.sc",				EV_PM_Fall);

}

//-----------------------------------------------------------------------------
// Purpose: Since we cannot invoke events from client side by index (HL BUGBUG?)
//			It is possible to call event functions directly using this wrapper
// Input  : flags - 
//			clientindex - 
//			(*EventFunc) - event function itself
//			ducking - is the invoker ducking?
//			all other parameters are same as usual
//-----------------------------------------------------------------------------
void CL_PlaybackEventDirect(int flags, int clientindex, void (*EventFunc)(struct event_args_s *args), int ducking, float *origin, float *angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2)
{
//	CON_PRINTF("CL_PlaybackEventDirect(flags %d, clientindex %d)\n", flags, clientindex);
	if (EventFunc && (flags & FEV_CLIENT))
	{
		event_args_t eargs;
		eargs.flags = flags;
		eargs.entindex = clientindex;
		VectorCopy(origin, eargs.origin);
		VectorCopy(angles, eargs.angles);
//		VectorCopy(velocity, eargs.velocity);
		VectorClear(eargs.velocity);
		eargs.ducking = ducking;
		eargs.fparam1 = fparam1;
		eargs.fparam2 = fparam2;
		eargs.iparam1 = iparam1;
		eargs.iparam2 = iparam2;
		eargs.bparam1 = bparam1;
		eargs.bparam2 = bparam2;
		EventFunc(&eargs);
	}
}
