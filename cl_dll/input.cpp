//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// cl.input.c  -- builds an intended movement command to send to the server

//xxxxxx Move bob and pitch drifting code here and other stuff from view if needed

// Quake is a trademark of Id Software, Inc., (c) 1996 Id Software, Inc. All
// rights reserved.
#include "hud.h"
#include "cl_util.h"
#include "util_vector.h"
#include "camera.h"
#include "cvardef.h"
#include "usercmd.h"
#include "const.h"
#include "camera.h"
#include "in_defs.h"
#include "view.h"
#include <ctype.h>
#include "vgui_XDMViewport.h"
#include "..\game_shared\voice_status.h"

extern "C" 
{
	struct kbutton_s DLLEXPORT *KB_Find( const char *name );
	void DLLEXPORT CL_CreateMove ( float frametime, struct usercmd_s *cmd, int active );
	int DLLEXPORT HUD_Key_Event( int eventcode, int keynum, const char *pszCurrentBinding );
}

extern int g_weaponselect;
extern cl_enginefunc_t gEngfuncs;
// Defined in pm_math.c
//extern "C" float anglemod( float a );
//float anglemod( float a );

// xxx need client dll function to get and clear impuse

int	in_impulse	= 0;
int	in_cancel	= 0;

cvar_t	*m_pitch;
cvar_t	*m_yaw;
cvar_t	*m_forward;
cvar_t	*m_side;

cvar_t	*lookstrafe;
cvar_t	*lookspring;
cvar_t	*cl_pitchup;
cvar_t	*cl_pitchdown;
cvar_t	*cl_upspeed;
cvar_t	*cl_forwardspeed;
cvar_t	*cl_backspeed;
cvar_t	*cl_sidespeed;
cvar_t	*cl_movespeedkey;
cvar_t	*cl_yawspeed;
cvar_t	*cl_pitchspeed;
cvar_t	*cl_anglespeedkey;
cvar_t	*cl_vsmoothing;
/*
===============================================================================

KEY BUTTONS

Continuous button event tracking is complicated by the fact that two different
input sources (say, mouse button 1 and the control key) can both press the
same button, but the button should only be released when both of the
pressing key have been released.

When a key event issues a button command (+forward, +attack, etc), it appends
its key number as a parameter to the command so it can be matched up with
the release.

state bit 0 is the current state of the key
state bit 1 is edge triggered on the up to down transition
state bit 2 is edge triggered on the down to up transition

===============================================================================
*/


kbutton_t	in_mlook;
kbutton_t	in_klook;
kbutton_t	in_jlook;
kbutton_t	in_left;
kbutton_t	in_right;
kbutton_t	in_forward;
kbutton_t	in_back;
kbutton_t	in_lookup;
kbutton_t	in_lookdown;
kbutton_t	in_moveleft;
kbutton_t	in_moveright;
kbutton_t	in_strafe;
kbutton_t	in_speed;
kbutton_t	in_use;
kbutton_t	in_jump;
kbutton_t	in_attack;
kbutton_t	in_attack2;
kbutton_t	in_up;
kbutton_t	in_down;
kbutton_t	in_duck;
kbutton_t	in_reload;
kbutton_t	in_alt1;
kbutton_t	in_score;
kbutton_t	in_break;
kbutton_t	in_graph;  // Display the netgraph
kbutton_t	in_select;

typedef struct kblist_s
{
	struct kblist_s *next;
	kbutton_t *pkey;
	char name[32];
} kblist_t;

kblist_t *g_kbkeys = NULL;

/*
============
KB_ConvertString

Removes references to +use and replaces them with the keyname in the output string.  If
 a binding is unfound, then the original text is retained.
NOTE:  Only works for text with +word in it.
============
*/
int KB_ConvertString( char *in, char **ppout )
{
	char sz[ 4096 ];
	char binding[ 64 ];
	char *p;
	char *pOut;
	char *pEnd;
	const char *pBinding;

	if ( !ppout )
		return 0;

	*ppout = NULL;
	p = in;
	pOut = sz;
	while ( *p )
	{
		if ( *p == '+' )
		{
			pEnd = binding;
			while ( *p && ( isalnum( *p ) || ( pEnd == binding ) ) && ( ( pEnd - binding ) < 63 ) )
			{
				*pEnd++ = *p++;
			}

			*pEnd =  '\0';

			pBinding = NULL;
			if ( strlen( binding + 1 ) > 0 )
			{
				// See if there is a binding for binding?
				pBinding = gEngfuncs.Key_LookupBinding( binding + 1 );
			}

			if ( pBinding )
			{
				*pOut++ = '[';
				pEnd = (char *)pBinding;
			}
			else
			{
				pEnd = binding;
			}

			while ( *pEnd )
			{
				*pOut++ = *pEnd++;
			}

			if ( pBinding )
			{
				*pOut++ = ']';
			}
		}
		else
		{
			*pOut++ = *p++;
		}
	}

	*pOut = '\0';

	pOut = ( char * )malloc( strlen( sz ) + 1 );
	strcpy( pOut, sz );
	*ppout = pOut;

	return 1;
}

/*
============
KB_Find

Allows the engine to get a kbutton_t directly ( so it can check +mlook state, etc ) for saving out to .cfg files
============
*/
struct kbutton_s DLLEXPORT *KB_Find( const char *name )
{
	kblist_t *p;
	p = g_kbkeys;
	while ( p )
	{
		if ( !stricmp( name, p->name ) )
			return p->pkey;

		p = p->next;
	}
	return NULL;
}

/*
============
KB_Add

Add a kbutton_t * to the list of pointers the engine can retrieve via KB_Find
============
*/
void KB_Add( const char *name, kbutton_t *pkb )
{
	kblist_t *p;	
	kbutton_t *kb;

	kb = KB_Find( name );
	
	if ( kb )
		return;

	p = ( kblist_t * )malloc( sizeof( kblist_t ) );
	memset( p, 0, sizeof( *p ) );

	strcpy( p->name, name );
	p->pkey = pkb;

	p->next = g_kbkeys;
	g_kbkeys = p;
}

/*
============
KB_Init

Add kbutton_t definitions that the engine can query if needed
============
*/
void KB_Init( void )
{
	g_kbkeys = NULL;

	KB_Add( "in_graph", &in_graph );
	KB_Add( "in_mlook", &in_mlook );
	KB_Add( "in_jlook", &in_jlook );
	KB_Add( "in_select", &in_select );// XDM3035a: is this really nescessary?
}

/*
============
KB_Shutdown

Clear kblist
============
*/
void KB_Shutdown( void )
{
	kblist_t *p, *n;
	p = g_kbkeys;
	while ( p )
	{
		n = p->next;
		free( p );
		p = n;
	}
	g_kbkeys = NULL;
}

/*
============
KeyDown
============
*/
void KeyDown (kbutton_t *b)
{
	int		k;
	char	*c;

	c = CMD_ARGV(1);
	if (c[0])
		k = atoi(c);
	else
		k = -1;		// typed manually at the console for continuous down

	if (k == b->down[0] || k == b->down[1])
		return;		// repeating key

	if (!b->down[0])
		b->down[0] = k;
	else if (!b->down[1])
		b->down[1] = k;
	else
	{
		gEngfuncs.Con_DPrintf("Three keys down for a button '%c' '%c' '%c'!\n", b->down[0], b->down[1], c);
		return;
	}

	if (b->state & 1)
		return;		// still down

	b->state |= 1 + 2;	// down + impulse down
}

/*
============
KeyUp
============
*/
void KeyUp (kbutton_t *b)
{
	int		k;
	char	*c;

	c = CMD_ARGV(1);
	if (c[0])
		k = atoi(c);
	else
	{ // typed manually at the console, assume for unsticking, so clear all
		b->down[0] = b->down[1] = 0;
		b->state = 4;	// impulse up
		return;
	}

	if (b->down[0] == k)
		b->down[0] = 0;
	else if (b->down[1] == k)
		b->down[1] = 0;
	else
		return;		// key up without coresponding down (menu pass through)

	if (b->down[0] || b->down[1])
	{
		//Con_Printf ("Keys down for button: '%c' '%c' '%c' (%d,%d,%d)!\n", b->down[0], b->down[1], c, b->down[0], b->down[1], c);
		return;		// some other key is still holding it down
	}

	if (!(b->state & 1))
		return;		// still up (this should not happen)

	b->state &= ~1;		// now up
	b->state |= 4; 		// impulse up
}

/*
============
HUD_Key_Event

Return 1 to allow engine to process the key, otherwise, act on it as needed
============
*/
int DLLEXPORT HUD_Key_Event( int down, int keynum, const char *pszCurrentBinding )
{
	if (gViewPort)
		return gViewPort->KeyInput(down, keynum, pszCurrentBinding);
	
	return 1;
}

void IN_BreakDown(void) {KeyDown(&in_break);};
void IN_BreakUp(void) { KeyUp( &in_break ); };
void IN_KLookDown(void) {KeyDown(&in_klook);}
void IN_KLookUp(void) {KeyUp(&in_klook);}
void IN_JLookDown(void) {KeyDown(&in_jlook);}
void IN_JLookUp(void) {KeyUp(&in_jlook);}
void IN_MLookDown(void) {KeyDown(&in_mlook);}
void IN_UpDown(void) {KeyDown(&in_up);}
void IN_UpUp(void) {KeyUp(&in_up);}
void IN_DownDown(void) {KeyDown(&in_down);}
void IN_DownUp(void) {KeyUp(&in_down);}
void IN_LeftDown(void) {KeyDown(&in_left);}
void IN_LeftUp(void) {KeyUp(&in_left);}
void IN_RightDown(void) {KeyDown(&in_right);}
void IN_RightUp(void) {KeyUp(&in_right);}

void IN_ForwardDown(void)
{
	KeyDown(&in_forward);
	gHUD.m_Spectator.HandleButtonsDown( IN_FORWARD );
}

void IN_ForwardUp(void)
{
	KeyUp(&in_forward);
	gHUD.m_Spectator.HandleButtonsUp( IN_FORWARD );
}

void IN_BackDown(void)
{
	KeyDown(&in_back);
	gHUD.m_Spectator.HandleButtonsDown( IN_BACK );
}

void IN_BackUp(void)
{
	KeyUp(&in_back);
	gHUD.m_Spectator.HandleButtonsUp( IN_BACK );
}
void IN_LookupDown(void) {KeyDown(&in_lookup);}
void IN_LookupUp(void) {KeyUp(&in_lookup);}
void IN_LookdownDown(void) {KeyDown(&in_lookdown);}
void IN_LookdownUp(void) {KeyUp(&in_lookdown);}
void IN_MoveleftDown(void)
{
	KeyDown(&in_moveleft);
	gHUD.m_Spectator.HandleButtonsDown( IN_MOVELEFT );
}

void IN_MoveleftUp(void)
{
	KeyUp(&in_moveleft);
	gHUD.m_Spectator.HandleButtonsUp( IN_MOVELEFT );
}

void IN_MoverightDown(void)
{
	KeyDown(&in_moveright);
	gHUD.m_Spectator.HandleButtonsDown( IN_MOVERIGHT );
}

void IN_MoverightUp(void)
{
	KeyUp(&in_moveright);
	gHUD.m_Spectator.HandleButtonsUp( IN_MOVERIGHT );
}
void IN_SpeedDown(void) {KeyDown(&in_speed);}
void IN_SpeedUp(void) {KeyUp(&in_speed);}
void IN_StrafeDown(void) {KeyDown(&in_strafe);}
void IN_StrafeUp(void) {KeyUp(&in_strafe);}

void IN_Attack2Down(void) 
{
	KeyDown(&in_attack2);
	gHUD.m_Spectator.HandleButtonsDown( IN_ATTACK2 );
}

void IN_Attack2Up(void) {KeyUp(&in_attack2);}
void IN_UseDown (void)
{
	KeyDown(&in_use);
	gHUD.m_Spectator.HandleButtonsDown( IN_USE );
}
void IN_UseUp (void) {KeyUp(&in_use);}
void IN_JumpDown (void)
{
	KeyDown(&in_jump);
	gHUD.m_Spectator.HandleButtonsDown( IN_JUMP );

}
void IN_JumpUp (void) {KeyUp(&in_jump);}
void IN_DuckDown(void)
{
	KeyDown(&in_duck);
	gHUD.m_Spectator.HandleButtonsDown( IN_DUCK );

}
void IN_DuckUp(void) {KeyUp(&in_duck);}
void IN_ReloadDown(void) {KeyDown(&in_reload);}
void IN_ReloadUp(void) {KeyUp(&in_reload);}
void IN_Alt1Down(void) {KeyDown(&in_alt1);}
void IN_Alt1Up(void) {KeyUp(&in_alt1);}
void IN_GraphDown(void) {KeyDown(&in_graph);}
void IN_GraphUp(void) {KeyUp(&in_graph);}

void IN_AttackDown(void)
{
	KeyDown( &in_attack );
	gHUD.m_Spectator.HandleButtonsDown( IN_ATTACK );
}

void IN_AttackUp(void)
{
	KeyUp( &in_attack );
	in_cancel = 0;
}

// Special handling
void IN_Cancel(void)
{
	in_cancel = 1;
}

void IN_Impulse(void)
{
	in_impulse = atoi(CMD_ARGV(1));
}

void IN_ScoreDown(void)
{
	KeyDown(&in_score);

	if (gViewPort)
		gViewPort->ShowScoreBoard();
}

void IN_ScoreUp(void)
{
	KeyUp(&in_score);

	if (gViewPort)
		gViewPort->HideScoreBoard();
}

void IN_MLookUp (void)
{
	KeyUp( &in_mlook );
	if ( !( in_mlook.state & 1 ) && lookspring->value )
	{
		V_StartPitchDrift();
	}
}
/* this doesn't work right
// XDM3035a: enable entity manipulation mode
void IN_SelectDown(void)
{
	KeyDown(&in_select);
	// other values are possible in the future so check carefully
	if (g_iMouseManipulationMode == 0)
		g_iMouseManipulationMode = 1;

	gViewPort->UpdateCursorState();
}

// XDM3035a: disable entity manipulation mode
void IN_SelectUp(void)
{
	KeyUp(&in_select);
//	if (in_select.state & 2)
	{
		if (g_iMouseManipulationMode == 1)
			g_iMouseManipulationMode = 0;

		gViewPort->UpdateCursorState();
	}
}*/


/*
===============
CL_KeyState

Returns 0.25 if a key was pressed and released during the frame,
0.5 if it was pressed and held
0 if held then released, and
1.0 if held for the entire time
===============
*/
float CL_KeyState (kbutton_t *key)
{
	float		val = 0.0;
	int			impulsedown, impulseup, down;
	
	impulsedown = key->state & 2;
	impulseup	= key->state & 4;
	down		= key->state & 1;
	
	if ( impulsedown && !impulseup )
	{
		// pressed and held this frame?
		val = down ? 0.5f : 0.0f;
	}

	if ( impulseup && !impulsedown )
	{
		// released this frame?
		val = down ? 0.0f : 0.0f;
	}

	if ( !impulsedown && !impulseup )
	{
		// held the entire frame?
		val = down ? 1.0f : 0.0f;
	}

	if ( impulsedown && impulseup )
	{
		if ( down )
		{
			// released and re-pressed this frame
			val = 0.75;	
		}
		else
		{
			// pressed and released this frame
			val = 0.25;	
		}
	}

	// clear impulses
	key->state &= 1;		
	return val;
}

/*
================
CL_AdjustAngles

Moves the local angle positions
================
*/
void CL_AdjustAngles(const float &frametime, float *viewangles)
{
	float	speed;
	float	up, down;
	
	if (in_speed.state & 1)
	{
		speed = frametime * cl_anglespeedkey->value;
	}
	else
	{
		speed = frametime;
	}

	if (!(in_strafe.state & 1))
	{
		viewangles[YAW] -= speed*cl_yawspeed->value * CL_KeyState (&in_right);
		viewangles[YAW] += speed*cl_yawspeed->value * CL_KeyState (&in_left);
		viewangles[YAW] = anglemod(viewangles[YAW]);
	}
	if (in_klook.state & 1)
	{
		V_StopPitchDrift ();
		viewangles[PITCH] -= speed*cl_pitchspeed->value * CL_KeyState (&in_forward);
		viewangles[PITCH] += speed*cl_pitchspeed->value * CL_KeyState (&in_back);
	}
	
	up = CL_KeyState (&in_lookup);
	down = CL_KeyState(&in_lookdown);
	
	viewangles[PITCH] -= speed*cl_pitchspeed->value * up;
	viewangles[PITCH] += speed*cl_pitchspeed->value * down;

	if (up || down)
		V_StopPitchDrift ();
		
	if (viewangles[PITCH] > cl_pitchdown->value)
		viewangles[PITCH] = cl_pitchdown->value;
	if (viewangles[PITCH] < -cl_pitchup->value)
		viewangles[PITCH] = -cl_pitchup->value;

	if (viewangles[ROLL] > 50)
		viewangles[ROLL] = 50;
	if (viewangles[ROLL] < -50)
		viewangles[ROLL] = -50;
}

/*
================
CL_CreateMove

Send the intended movement message to the server
if active == 1 then we are 1) not playing back demos ( where our commands are ignored ) and
2 ) we have finished signing on to server
================
*/
void DLLEXPORT CL_CreateMove ( float frametime, struct usercmd_s *cmd, int active )
{
	float spd;
	vec3_t viewangles;
	static vec3_t oldangles;

	if (gHUD.m_iActive != active)
		gHUD.OnGameActivated(active);

	if ( active )
	{
		//memset( viewangles, 0, sizeof( vec3_t ) );
		//viewangles[ 0 ] = viewangles[ 1 ] = viewangles[ 2 ] = 0.0;
		gEngfuncs.GetViewAngles((float *)viewangles);

		if( !gHUD.m_iPlayerFrozen )
			CL_AdjustAngles(frametime, viewangles);

		memset(cmd, 0, sizeof(*cmd));

		gEngfuncs.SetViewAngles((float *)viewangles);

		if ( in_strafe.state & 1 )
		{
			cmd->sidemove += cl_sidespeed->value * CL_KeyState (&in_right);
			cmd->sidemove -= cl_sidespeed->value * CL_KeyState (&in_left);
		}

		cmd->sidemove += cl_sidespeed->value * CL_KeyState (&in_moveright);
		cmd->sidemove -= cl_sidespeed->value * CL_KeyState (&in_moveleft);

		cmd->upmove += cl_upspeed->value * CL_KeyState (&in_up);
		cmd->upmove -= cl_upspeed->value * CL_KeyState (&in_down);

		if ( !(in_klook.state & 1 ) )
		{	
			cmd->forwardmove += cl_forwardspeed->value * CL_KeyState (&in_forward);
			cmd->forwardmove -= cl_backspeed->value * CL_KeyState (&in_back);
		}	

		// adjust for speed key
		if ( in_speed.state & 1 )
		{
			cmd->forwardmove *= cl_movespeedkey->value;
			cmd->sidemove *= cl_movespeedkey->value;
			cmd->upmove *= cl_movespeedkey->value;
		}

		// clip to maxspeed
		spd = gEngfuncs.GetClientMaxspeed();
		if ( spd != 0.0 )
		{
			// scale the 3 speeds so that the total velocity is not > cl.maxspeed
			float fmov = (float)sqrt( (cmd->forwardmove*cmd->forwardmove) + (cmd->sidemove*cmd->sidemove) + (cmd->upmove*cmd->upmove) );

			if ( fmov > spd )
			{
				float fratio = spd / fmov;
				cmd->forwardmove *= fratio;
				cmd->sidemove *= fratio;
				cmd->upmove *= fratio;
			}
		}

		// Allow mice and other controllers to add their inputs
	if( !gHUD.m_iPlayerFrozen )
		IN_Move ( frametime, cmd );
	}

	cmd->impulse = in_impulse;
	in_impulse = 0;

	cmd->weaponselect = g_weaponselect;
	g_weaponselect = 0;
	//
	// set button and flag bits
	//
	cmd->buttons = CL_ButtonBits( 1 );

	try
	{
		// If they're in a modal dialog, ignore the attack button.
		if (GetClientVoiceMgr()->IsInSquelchMode())
			cmd->buttons &= ~IN_ATTACK;
	}
	catch (...)
	{
		CON_PRINTF("GetClientVoiceMgr() CL_CreateMove() exception!\n");
	}

	// Using joystick?
	if (in_joystick && in_joystick->value )
	{
		if ( cmd->forwardmove > 0 )
		{
			cmd->buttons |= IN_FORWARD;
		}
		else if ( cmd->forwardmove < 0 )
		{
			cmd->buttons |= IN_BACK;
		}
	}

// XDM3035c: strangely this line affects PlayerPostThink() calls frequency on server!! (look at the gluon beam)
	gEngfuncs.GetViewAngles( (float *)viewangles );

	// Set current view angles.
	if ( g_iAlive && !gHUD.m_iPlayerFrozen)
	{
		VectorCopy( viewangles, cmd->viewangles );
		VectorCopy( viewangles, oldangles );
	}
	else
	{
		VectorCopy( oldangles, cmd->viewangles );
	}
}

/*
============
CL_ButtonBits

Returns appropriate button info for keyboard and mouse state
Set bResetState to 1 to clear old state info
============
*/
int CL_ButtonBits( int bResetState )
{
	int bits = 0;

	if (in_attack.state & 3)
	{
		bits |= IN_ATTACK;
	}
	
	if (in_duck.state & 3)
	{
		bits |= IN_DUCK;
	}
 
	if (in_jump.state & 3)
	{
		bits |= IN_JUMP;
	}

	if (in_forward.state & 3)
	{
		bits |= IN_FORWARD;
	}
	
	if (in_back.state & 3)
	{
		bits |= IN_BACK;
	}

	if (in_use.state & 3)
	{
		bits |= IN_USE;
	}

	if (in_cancel)
	{
		bits |= IN_CANCEL;
	}

	if (in_left.state & 3)
	{
		bits |= IN_LEFT;
	}
	
	if (in_right.state & 3)
	{
		bits |= IN_RIGHT;
	}
	
	if (in_moveleft.state & 3)
	{
		bits |= IN_MOVELEFT;
	}
	
	if (in_moveright.state & 3)
	{
		bits |= IN_MOVERIGHT;
	}

	if (in_attack2.state & 3)
	{
		bits |= IN_ATTACK2;
	}

	if (in_reload.state & 3)
	{
		bits |= IN_RELOAD;
	}

	if (in_alt1.state & 3)
	{
		bits |= IN_ALT1;
	}

	if (in_score.state & 3)
	{
		bits |= IN_SCORE;
	}

	// Dead or in intermission? Shore scoreboard, too
	if (CL_IsDead() || gHUD.m_iIntermission)
	{
		bits |= IN_SCORE;
	}

	if (bResetState)
	{
		in_attack.state &= ~2;
		in_duck.state &= ~2;
		in_jump.state &= ~2;
		in_forward.state &= ~2;
		in_back.state &= ~2;
		in_use.state &= ~2;
		in_left.state &= ~2;
		in_right.state &= ~2;
		in_moveleft.state &= ~2;
		in_moveright.state &= ~2;
		in_attack2.state &= ~2;
		in_reload.state &= ~2;
		in_alt1.state &= ~2;
		in_score.state &= ~2;
		in_select.state &= ~2;// XDM3035a
	}
	return bits;
}

/*
============
CL_ResetButtonBits

============
*/
void CL_ResetButtonBits( int bits )
{
	int bitsNew = CL_ButtonBits( 0 ) ^ bits;

	// Has the attack button been changed
	if ( bitsNew & IN_ATTACK )
	{
		// Was it pressed? or let go?
		if ( bits & IN_ATTACK )
		{
			KeyDown( &in_attack );
		}
		else
		{
			// totally clear state
			in_attack.state &= ~7;
		}
	}
}

/*
============
InitInput
============
*/
void InitInput (void)
{
	ADD_DOMMAND("+moveup",IN_UpDown);
	ADD_DOMMAND("-moveup",IN_UpUp);
	ADD_DOMMAND("+movedown",IN_DownDown);
	ADD_DOMMAND("-movedown",IN_DownUp);
	ADD_DOMMAND("+left",IN_LeftDown);
	ADD_DOMMAND("-left",IN_LeftUp);
	ADD_DOMMAND("+right",IN_RightDown);
	ADD_DOMMAND("-right",IN_RightUp);
	ADD_DOMMAND("+forward",IN_ForwardDown);
	ADD_DOMMAND("-forward",IN_ForwardUp);
	ADD_DOMMAND("+back",IN_BackDown);
	ADD_DOMMAND("-back",IN_BackUp);
	ADD_DOMMAND("+lookup", IN_LookupDown);
	ADD_DOMMAND("-lookup", IN_LookupUp);
	ADD_DOMMAND("+lookdown", IN_LookdownDown);
	ADD_DOMMAND("-lookdown", IN_LookdownUp);
	ADD_DOMMAND("+strafe", IN_StrafeDown);
	ADD_DOMMAND("-strafe", IN_StrafeUp);
	ADD_DOMMAND("+moveleft", IN_MoveleftDown);
	ADD_DOMMAND("-moveleft", IN_MoveleftUp);
	ADD_DOMMAND("+moveright", IN_MoverightDown);
	ADD_DOMMAND("-moveright", IN_MoverightUp);
	ADD_DOMMAND("+speed", IN_SpeedDown);
	ADD_DOMMAND("-speed", IN_SpeedUp);
	ADD_DOMMAND("+attack", IN_AttackDown);
	ADD_DOMMAND("-attack", IN_AttackUp);
	ADD_DOMMAND("+attack2", IN_Attack2Down);
	ADD_DOMMAND("-attack2", IN_Attack2Up);
	ADD_DOMMAND("+use", IN_UseDown);
	ADD_DOMMAND("-use", IN_UseUp);
	ADD_DOMMAND("+jump", IN_JumpDown);
	ADD_DOMMAND("-jump", IN_JumpUp);
	ADD_DOMMAND("+klook", IN_KLookDown);
	ADD_DOMMAND("-klook", IN_KLookUp);
	ADD_DOMMAND("+mlook", IN_MLookDown);
	ADD_DOMMAND("-mlook", IN_MLookUp);
	ADD_DOMMAND("+jlook", IN_JLookDown);
	ADD_DOMMAND("-jlook", IN_JLookUp);
	ADD_DOMMAND("+duck", IN_DuckDown);
	ADD_DOMMAND("-duck", IN_DuckUp);
	ADD_DOMMAND("+reload", IN_ReloadDown);
	ADD_DOMMAND("-reload", IN_ReloadUp);
	ADD_DOMMAND("+alt1", IN_Alt1Down);
	ADD_DOMMAND("-alt1", IN_Alt1Up);
	ADD_DOMMAND("+score", IN_ScoreDown);
	ADD_DOMMAND("-score", IN_ScoreUp);
	ADD_DOMMAND("+showscores", IN_ScoreDown);
	ADD_DOMMAND("-showscores", IN_ScoreUp);
	ADD_DOMMAND("+graph", IN_GraphDown);
	ADD_DOMMAND("-graph", IN_GraphUp);
	ADD_DOMMAND("+break",IN_BreakDown);
	ADD_DOMMAND("-break",IN_BreakUp);
//	ADD_DOMMAND("+select",IN_SelectDown);// XDM3035a
//	ADD_DOMMAND("-select",IN_SelectUp);
	ADD_DOMMAND("impulse", IN_Impulse);

	lookstrafe			= CVAR_CREATE("lookstrafe", "0", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	lookspring			= CVAR_CREATE("lookspring", "0", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	cl_anglespeedkey	= CVAR_CREATE("cl_anglespeedkey", "0.67", FCVAR_CLIENTDLL);
	cl_yawspeed			= CVAR_CREATE("cl_yawspeed", "210", FCVAR_CLIENTDLL);
	cl_pitchspeed		= CVAR_CREATE("cl_pitchspeed", "225", FCVAR_CLIENTDLL);
	cl_upspeed			= CVAR_CREATE("cl_upspeed", "320", FCVAR_CLIENTDLL);
	cl_forwardspeed		= CVAR_CREATE("cl_forwardspeed", "400", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	cl_backspeed		= CVAR_CREATE("cl_backspeed", "400", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	cl_sidespeed		= CVAR_CREATE("cl_sidespeed", "400", FCVAR_CLIENTDLL);
	cl_movespeedkey		= CVAR_CREATE("cl_movespeedkey", "0.3", FCVAR_CLIENTDLL);
	cl_pitchup			= CVAR_CREATE("cl_pitchup", "89", FCVAR_CLIENTDLL);
	cl_pitchdown		= CVAR_CREATE("cl_pitchdown", "89", FCVAR_CLIENTDLL);
	cl_vsmoothing		= CVAR_CREATE("cl_vsmoothing", "0.05", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_pitch			    = CVAR_CREATE("m_pitch","0.022", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_yaw				= CVAR_CREATE("m_yaw","0.022", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_forward			= CVAR_CREATE("m_forward","1", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);
	m_side				= CVAR_CREATE("m_side","0.8", FCVAR_ARCHIVE | FCVAR_CLIENTDLL);

	// Initialize third person camera controls.
//	CAM_Init();
	// Initialize inputs
	IN_Init();
	// Initialize keyboard
	KB_Init();
	// Initialize view system
	V_Init();
}

/*
============
ShutdownInput
============
*/
void ShutdownInput (void)
{
	IN_Shutdown();
	KB_Shutdown();
}
