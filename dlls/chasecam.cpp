#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "gamerules.h"
#include "player.h"
#include "client.h"

// Obsolete?

class CChaseCam : public CBaseEntity
{
public:
	virtual void Spawn(void);
	void EXPORT IdleThink(void);

	Vector m_VecCamViewOffset;
};

LINK_ENTITY_TO_CLASS(entity_chasecam, CChaseCam);

void CChaseCam::Spawn(void)
{
	CBaseEntity *pPlayer = NULL;
	pPlayer = UTIL_FindEntityByClassname(pPlayer, "player");
	if (pPlayer == NULL)
	{
		UTIL_Remove(this);
		return;
	}
	CBasePlayer *player = GetClassPtr((CBasePlayer *)pPlayer->pev);
    pev->movetype = MOVETYPE_FLY;
    pev->solid = SOLID_NOT;
    pev->renderamt = 0;
	TraceResult tr;
	UTIL_MakeVectors(player->pev->v_angle);
	UTIL_TraceLine(player->pev->origin + player->pev->view_ofs, player->pev->origin + player->pev->view_ofs + gpGlobals->v_forward * -110 + gpGlobals->v_up * 20 , dont_ignore_monsters, player->edict(), &tr );
	m_VecCamViewOffset = Vector(20, 0, -110);
	UTIL_SetOrigin(pev, tr.vecEndPos);
    pev->angles.z = player->pev->angles.z;
    pev->angles.y = player->pev->angles.y;
    pev->angles.x = 5;
    pev->fixangle = TRUE;
	UTIL_SetView(pPlayer->edict(), edict());
	SET_MODEL(ENT(pev), "models/projectiles.mdl");
	SetTouchNull();
	SetThink(&CChaseCam::IdleThink);
	pev->nextthink = gpGlobals->time + 0.01;
}

void CChaseCam::IdleThink(void)
{
	CBaseEntity *pPlayer = NULL;
	CBasePlayer *player = NULL;
	pPlayer = UTIL_FindEntityByClassname( pPlayer, "player" );
	player = GetClassPtr((CBasePlayer *)pPlayer->pev);
	if (pPlayer)
	{
		UTIL_MakeVectors(player->pev->v_angle);
		TraceResult tr;
		UTIL_TraceLine(player->pev->origin + player->pev->view_ofs,player->pev->origin + player->pev->view_ofs + gpGlobals->v_forward * m_VecCamViewOffset.z + gpGlobals->v_up * m_VecCamViewOffset.x +gpGlobals->v_right * m_VecCamViewOffset.y, dont_ignore_monsters, player->edict(), &tr);
		UTIL_SetOrigin(pev, tr.vecEndPos);
		pev->angles.z = player->pev->angles.z;
		pev->angles.y = player->pev->angles.y;
		pev->angles.x = 5;
		pev->fixangle = TRUE;
		UTIL_SetView(pPlayer->edict(), edict());
	}
	else
	{
		UTIL_SetView(pPlayer->edict(), pPlayer->edict());
		REMOVE_ENTITY(ENT(pev));
		return;
	}
	pev->nextthink = gpGlobals->time + 0.01;
}