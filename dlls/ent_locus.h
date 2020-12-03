//====================================================================
//
// Purpose: 
//
//====================================================================

#ifndef ENT_LOCUS_H
#define ENT_LOCUS_H
#ifdef _WIN32
#pragma once
#endif


Vector CalcLocus_Position(CBaseEntity *pEntity, CBaseEntity *pLocus, const char *szText);
Vector CalcLocus_Velocity(CBaseEntity *pEntity, CBaseEntity *pLocus, const char *szText);
float CalcLocus_Ratio(CBaseEntity *pLocus, const char *szText); 
float CalcLocus_Ratio(CBaseEntity *pLocus, const char *szText,int mode); //AJH added 'mode' = ratio to return




class CCalcPosition : public CPointEntity
{
public:
	virtual Vector CalcPosition(CBaseEntity *pLocus);
};


class CCalcRatio : public CPointEntity
{
public:
	virtual float CalcRatio(CBaseEntity *pLocus, int mode);
};


#define SF_CALCVELOCITY_NORMALIZE	1
#define SF_CALCVELOCITY_SWAPZ		2 // MJB this should more correctly be called 'invertZ', but never mind.
#define SF_CALCVELOCITY_SWAPXY		4 // MJB axis swapping (pitch and yaw)
#define SF_CALCVELOCITY_SWAPYZ		8 // MJB axis swapping (yaw and roll)
#define SF_CALCVELOCITY_SWAPXZ		16 // MJB axis swapping (pitch and roll)
#define SF_CALCVELOCITY_DEBUGSWAP	32 // So what the hell is the swapping DOING?

class CCalcSubVelocity : public CPointEntity
{
	Vector Convert(CBaseEntity *pLocus, const Vector &vecVel);
	Vector ConvertAngles(CBaseEntity *pLocus, const Vector &vecAngles);
public:
	virtual Vector CalcVelocity(CBaseEntity *pLocus);
};


class CCalcVelocityPath : public CPointEntity
{
public:
	virtual Vector CalcVelocity(CBaseEntity *pLocus);
};


class CCalcVelocityPolar : public CPointEntity
{
public:
	virtual Vector CalcVelocity(CBaseEntity *pLocus);
};


// Position marker
class CMark : public CPointEntity
{
public:
	virtual Vector CalcVelocity(CBaseEntity *pLocus) { return pev->movedir; }
	virtual float CalcRatio(CBaseEntity *pLocus, int mode ) { return pev->frags; }//AJH added 'mode' = ratio to return
	virtual void Think(void) { SUB_Remove(); }
};


class CLocusVariable : public CPointEntity
{
public:
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual Vector CalcVelocity(CBaseEntity *pLocus) { return pev->movedir; }
	virtual float CalcRatio(CBaseEntity *pLocus, int mode) { return pev->frags; }//AJH added 'mode' = ratio to return
	virtual void KeyValue(KeyValueData *pkvd);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static	TYPEDESCRIPTION m_SaveData[];

	int m_iszPosition;
	int m_iszVelocity;
	int m_iszRatio;
	int m_iszTargetName;
	int m_iszFireOnSpawn;
	float m_fDuration;
};

#endif // ENT_LOCUS_H
