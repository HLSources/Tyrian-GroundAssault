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
#ifndef EXPLODE_H
#define EXPLODE_H

#define	SF_ENVEXPLOSION_NODAMAGE	SF_NODAMAGE// XDM3034
/*
#define	SF_ENVEXPLOSION_NODAMAGE	( 1 << 0 ) // when set, ENV_EXPLOSION will not actually inflict damage
#define	SF_ENVEXPLOSION_REPEATABLE	( 1 << 1 ) // can this entity be refired?
#define SF_ENVEXPLOSION_NOFIREBALL	( 1 << 2 ) // don't draw the fireball
#define SF_ENVEXPLOSION_NOSMOKE		( 1 << 3 ) // don't draw the smoke
#define SF_ENVEXPLOSION_NODECAL		( 1 << 4 ) // don't make a scorch mark
#define SF_ENVEXPLOSION_NOSPARKS	( 1 << 5 ) // don't create sparks
#define SF_ENVEXPLOSION_NOPARTICLES	( 1 << 6 ) // don't create particles
#define SF_ENVEXPLOSION_NOSOUND		( 1 << 7 ) // don't play explosion sound
#define SF_ENVEXPLOSION_NUCLEAR		( 1 << 8 ) // XDM
*/
// Spark Shower
class CShower : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void Think(void);
	virtual void Touch(CBaseEntity *pOther );
	virtual int ObjectCaps(void) {return FCAP_DONT_SAVE;}
};

class CEnvExplosion : public CGrenade//CBaseMonster
{
public:
	static CEnvExplosion *CreateExplosion(const Vector &origin, const Vector &angles, CBaseEntity *pOwner, int magnitude, int flags);
	virtual void Spawn(void);
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual BOOL IsPushable(void) { return FALSE; }// XDM
	virtual BOOL ShouldRespawn(void) { return FALSE; }// XDM3035
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);
	static TYPEDESCRIPTION m_SaveData[];
	int m_iMagnitude;// how large is the fireball? how much damage?
};

CEnvExplosion *ExplosionCreate( const Vector &center, const Vector &angles, CBaseEntity *pOwner, int magnitude, int flags, float delay);
/*
class CExplosionSphere : public CBaseEntity
{
public:
	void Spawn(void);
	void Precache(void);
	static CExplosionSphere *CreateSphere(Vector vecOrigin, Vector vecAngles, const char *model);
	static CExplosionSphere *CreateSphere(Vector vecOrigin, Vector vecAngles, int modelindex);
	void Init(float life, float delay, int rendermode, int renderfx, int startrenderamt, int endrenderamt, float startscale, float endscale, Vector startcolor, Vector endcolor);
	void EXPORT ExplodeThink(void);
//	int iA;		// pev->renderamt at the beginning
//	int iB;		// pev->renderamt at the end
//	float fA;	// start scale factor
//	float fB;	// end scale factor
//	Vector vA;	// pev->rendercolor at the beginning
//	Vector vB;	// pev->rendercolor at the end
	float dRenderAmt;
	float dScale;
	Vector dColor;
};
*/
#endif			//EXPLODE_H
