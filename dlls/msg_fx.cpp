#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "decals.h"
#include "globals.h"
#include "monsters.h"
#include "weapons.h"

//===========================//
//Projectile trails & impacts//
//===========================//
void FX_Trail( Vector origin, int EntIndex, int Type )
{
	MESSAGE_BEGIN( MSG_ALL, gmsgTrail );
	WRITE_COORD( origin.x );
	WRITE_COORD( origin.y );
	WRITE_COORD( origin.z );
	WRITE_SHORT( EntIndex );
	WRITE_BYTE( Type );
	MESSAGE_END();
}

//=====================//
//Beam & Bullet Effects//
//=====================//
void FX_FireBeam( Vector origin, Vector angles, Vector normal, int Surface, int Type, BOOL IsWorldBrush )
{
	MESSAGE_BEGIN( MSG_ALL, gmsgFireBeam );
	WRITE_COORD( origin.x );//start point
	WRITE_COORD( origin.y );
	WRITE_COORD( origin.z );
	WRITE_COORD( angles.x );//end point
	WRITE_COORD( angles.y );
	WRITE_COORD( angles.z );
	WRITE_COORD( normal.x );//normal
	WRITE_COORD( normal.y );
	WRITE_COORD( normal.z );
	WRITE_BYTE( Surface );
	WRITE_BYTE( Type );
	WRITE_BYTE( IsWorldBrush );
	MESSAGE_END();
}

//===================//
//weapon fire effects//
//===================//
void FX_FireGun(Vector origin, int EntIndex, int Animation, int Type, int AltFire )
{
	MESSAGE_BEGIN( MSG_ALL, gmsgFireGun );
	WRITE_COORD( origin.x );
	WRITE_COORD( origin.y );
	WRITE_COORD( origin.z );
	WRITE_SHORT( EntIndex );
	WRITE_BYTE( Animation );
	WRITE_BYTE( Type );
	WRITE_BYTE( AltFire );
	MESSAGE_END();
}

//=======================//
//quake lightning effects//
//=======================//
void FX_FireLgtng(Vector pos, Vector end, int EntIndex, int Type)
{
	MESSAGE_BEGIN( MSG_ALL, gmsgFireLgtng);
	WRITE_COORD( pos.x );
	WRITE_COORD( pos.y );
	WRITE_COORD( pos.z );
	WRITE_COORD( end.x );
	WRITE_COORD( end.y );
	WRITE_COORD( end.z );
	WRITE_SHORT( EntIndex );
	WRITE_BYTE( Type );
	MESSAGE_END();
}

//====================//
//Extra weapons icons//
//===================//
void FX_WpnIcon(edict_t *ent, int FBAmmo, int TripAmmo, int SpiderAmmo, int MortarAmmo, int SatAmmo, int AtomAmmo)
{
	MESSAGE_BEGIN( MSG_ONE, gmsgWpnIcon, NULL,  ent);
	WRITE_BYTE( FBAmmo );
	WRITE_BYTE( TripAmmo );
	WRITE_BYTE( SpiderAmmo );
	WRITE_BYTE( MortarAmmo );
	WRITE_BYTE( SatAmmo );
	WRITE_BYTE( AtomAmmo );
	MESSAGE_END();
}

