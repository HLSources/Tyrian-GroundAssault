//====================================================================
//
// Purpose: 
//
//====================================================================

#ifndef ENT_ALIAS_H
#define ENT_ALIAS_H
#ifdef _WIN32
#pragma once
#endif

#define MAX_ALIASNAME_LEN 80


//LRC- moved here from alias.cpp so that util functions can use these defs.
class CBaseAlias : public CPointEntity
{
public:
	/*virtual this ends here */BOOL IsAlias(void) { return TRUE; };
	virtual CBaseEntity *FollowAlias(CBaseEntity *pFrom);
	virtual void ChangeValue(int iszValue);
	virtual void ChangeValue(CBaseEntity *pValue);
	virtual void FlushChanges(void) {};

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	CBaseAlias *m_pNextAlias;
};


class CMultiAlias : public CBaseAlias
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual CBaseEntity *FollowAlias(CBaseEntity *pFrom);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	int		m_cTargets;
	int		m_iszTargets[MAX_MULTI_TARGETS];
	int		m_iTotalValue;
	int		m_iValues[MAX_MULTI_TARGETS];
	int		m_iMode;
};


#define SF_ALIAS_OFF			1
#define SF_ALIAS_DEBUG			2
#define INFOALIAS_MODE_TOGGLE	0
#define INFOALIAS_MODE_LIST		1
#define MAX_ALIAS_TARGETS		16 //AJH

class CInfoAlias : public CBaseAlias	//AJH Now includes 'listmode' aliasing
{
public:
	virtual void KeyValue(KeyValueData *pkvd); //AJH
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual STATE GetState(void) { return (pev->spawnflags & SF_ALIAS_OFF)?STATE_OFF:STATE_ON; }

	virtual CBaseEntity *FollowAlias(CBaseEntity *pFrom);
	virtual void ChangeValue(int iszValue);
	virtual void FlushChanges(void);

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	int		m_cTargets;	//AJH the total number of targets in this alias's fire list.
	int		m_iTargetName[MAX_ALIAS_TARGETS];// AJH list of indexes into global string array
	int		m_iMode; //AJH 0 = On/Off mode, 1 = list mode
	int		m_iCurrentTarget; //AJH the current target that is being aliased
};


// Entity variable
class CLocusAlias : public CBaseAlias
{
public:
	virtual void PostSpawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual CBaseEntity *FollowAlias(CBaseEntity *pFrom);
	virtual void FlushChanges(void);

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	EHANDLE	m_hValue;
	EHANDLE m_hChangeTo;

	static TYPEDESCRIPTION m_SaveData[];
};


#define SF_CHANGEALIAS_RESOLVE 1
#define SF_CHANGEALIAS_DEBUG 2

class CTriggerChangeAlias : public CBaseEntity
{
public:
	virtual void Spawn(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int ObjectCaps(void) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};


class CInfoGroup : public CPointEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int GetMember(const char *szMemberName);

	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	static TYPEDESCRIPTION m_SaveData[];

	int		m_cMembers;
	int		m_iszMemberName[MAX_MULTI_TARGETS];
	int		m_iszMemberValue[MAX_MULTI_TARGETS];
	int		m_iszDefaultMember;
};




// Beam maker
#define BEAM_FSINE		0x10
#define BEAM_FSOLID		0x20
#define BEAM_FSHADEIN	0x40
#define BEAM_FSHADEOUT	0x80

#define SF_LBEAM_SHADEIN	128
#define SF_LBEAM_SHADEOUT	256
#define SF_LBEAM_SOLID		512
#define SF_LBEAM_SINE		1024

class CLocusBeam : public CPointEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);
	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int Save(CSave &save);
	virtual int Restore(CRestore &restore);

	int		m_iszSprite;
	int		m_iszTargetName;
	int		m_iszStart;
	int		m_iszEnd;
	int		m_iWidth;
	int		m_iDistortion;
	float	m_fFrame;
	int		m_iScrollRate;
	float	m_fDuration;
	float	m_fDamage;
	int		m_iDamageType;
	int		m_iFlags;

	static	TYPEDESCRIPTION m_SaveData[];
};




void UTIL_FlushAliases(void);
void UTIL_AddToAliasList(CBaseAlias *pAlias);
CBaseEntity *UTIL_FollowAliasReference(CBaseEntity *pStartEntity, const char *szValue);
CBaseEntity *UTIL_FollowGroupReference(CBaseEntity *pStartEntity, const char *szGroupName, char *szMemberName);
CBaseEntity *UTIL_FollowReference(CBaseEntity *pStartEntity, const char *szName);

#endif // ENT_ALIAS_H
