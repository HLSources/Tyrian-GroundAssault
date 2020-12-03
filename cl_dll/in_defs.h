//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#if !defined( IN_DEFSH )
#define IN_DEFSH
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */

#include "kbutton.h"

//#define DLLEXPORT __declspec( dllexport )

// joystick defines and variables
// where should defines be moved?
#define JOY_ABSOLUTE_AXIS	0x00000000		// control like a joystick
#define JOY_RELATIVE_AXIS	0x00000010		// control like a mouse, spinner, trackball
#define	JOY_MAX_AXES		6				// X, Y, Z, R, U, V
#define JOY_AXIS_X			0
#define JOY_AXIS_Y			1
#define JOY_AXIS_Z			2
#define JOY_AXIS_R			3
#define JOY_AXIS_U			4
#define JOY_AXIS_V			5

enum
{
	CAM_MODE_OFF = 0,
	CAM_MODE_LOCKED,
	CAM_MODE_ROTATE,
	CAM_MODE_FREE,
};

enum
{
	MMM_NONE = 0,
	MMM_USE,
	MMM_PICK,
	MMM_MOVE,
	MMM_EDIT,
	MMM_CREATE,
};// mouse_manipulation_mode_e;


void InitInput(void);
void ShutdownInput(void);// XDM: moved here from hud.cpp

void IN_Commands(void);
void IN_Init(void);
void IN_Shutdown(void);
void IN_StartupMouse(void);
void IN_GetMousePos(int *mx, int *my);
void IN_ResetMouse(void);
void IN_Move(const float &frametime, struct usercmd_s *cmd);

byte CL_IsThirdPersonAlloed(void);

void V_Init(void);
void VectorAngles(const float *forward, float *angles);
int CL_ButtonBits(int bResetState);
void CL_ResetButtonBits(int bits);
void CL_MouseEvent(const int &button, byte state);
void CL_EntitySelected(int entindex, vec3_t &hitpoint);

extern int g_iMouseManipulationMode;
extern int g_iVisibleMouse;


extern kbutton_t	in_strafe;
extern kbutton_t	in_mlook;
extern kbutton_t	in_speed;
extern kbutton_t	in_jlook;

extern cvar_t *in_joystick;
extern cvar_t *m_pitch;
extern cvar_t *m_yaw;
extern cvar_t *m_forward;
extern cvar_t *m_side;
extern cvar_t *m_filter;
extern cvar_t *sensitivity;
extern cvar_t *lookstrafe;
extern cvar_t *lookspring;
extern cvar_t *cl_pitchup;
extern cvar_t *cl_pitchdown;
extern cvar_t *cl_upspeed;
extern cvar_t *cl_forwardspeed;
extern cvar_t *cl_backspeed;
extern cvar_t *cl_sidespeed;
extern cvar_t *cl_movespeedkey;
extern cvar_t *cl_yawspeed;
extern cvar_t *cl_pitchspeed;
extern cvar_t *cl_anglespeedkey;
extern cvar_t *cl_vsmoothing;


#endif
