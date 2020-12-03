//-----------------------------------------------------------------------------
// X-Half-Life: Deathmatch code
// Copyright (c) 2001-2013
//-----------------------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include "skill.h"
#include "items.h"
#include "game.h"
#include "pm_shared.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CGameRulesSinglePlay::CGameRulesSinglePlay(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: Runs every server frame, should handle any timer tasks, periodic events, etc.
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::StartFrame(void)
{
}

//-----------------------------------------------------------------------------
// Purpose: Initialize HUD (client data) for a client
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::InitHUD(CBasePlayer *pPlayer)
{
	UpdateGameMode(pPlayer);// XDM3035a: client may have some invalid data
}

//-----------------------------------------------------------------------------
// Purpose: A network client is connecting
// Input  : *pEntity -
//			*pszName -
//			*pszAddress -
//			] -
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{
	pEntity->v.team = TEAM_NONE;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: client has been disconnected
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::ClientDisconnected(CBasePlayer *pPlayer)
{
}

//-----------------------------------------------------------------------------
// Purpose: Called when player picks up a new weapon
// Input  : *pPlayer - 
//			*pWeapon - 
// Output : Returns TRUE or FALSE
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon)
{
	if (!pWeapon->CanDeploy())
		return false;// that weapon can't deploy anyway.

	if (pPlayer->m_pActiveItem == NULL)
		return true;// player doesn't have an active item!

	if (!pPlayer->m_pActiveItem->CanHolster())
		return false;

//	if (pWeapon->iWeight() > pPlayer->m_pActiveItem->iWeight())
//		return true;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesSinglePlay::FlPlayerFallDamage(CBasePlayer *pPlayer)
{
	// subtract off the speed at which a player is allowed to fall without being hurt,
	// so damage will be based on speed beyond that, not the entire fall
	pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
	return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::PlayerSpawn(CBasePlayer *pPlayer)
{
	g_pGameRules->PlayerUseSpawnSpot(pPlayer);// XDM

	CBaseEntity	*pWeaponEntity = NULL;
	while ((pWeaponEntity = UTIL_FindEntityByClassname(pWeaponEntity, "game_player_equip")) != NULL)
		pWeaponEntity->Touch(pPlayer);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::PlayerThink(CBasePlayer *pPlayer)
{
}

//-----------------------------------------------------------------------------
// Purpose: Can the player respawn NOW?
// Input  : *pPlayer - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::FPlayerCanRespawn(CBasePlayer *pPlayer)
{
	return false;// XDM
}

//-----------------------------------------------------------------------------
// Purpose: how many points awarded to anyone that kills this entity
// Input  : *pAttacker - 
//			*pKilled - 
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesSinglePlay::IPointsForKill(CBaseEntity *pAttacker, CBaseEntity *pKilled)
{
/*	if (pKilled->IsPlayer())
		return 0;
	else */if (pKilled->IsMonster())
		return 1;

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: A player got killed
// Input  : *pVictim - 
//			*pKiller - 
//			*pInflictor - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::PlayerKilled(CBasePlayer *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
}

//-----------------------------------------------------------------------------
// Purpose: A monster got killed // XDM3035
// Input  : *pVictim - 
//			*pKiller - 
//			*pInflictor - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::MonsterKilled(CBaseMonster *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
	pKiller->pev->frags += IPointsForKill(pKiller, (CBasePlayer *)pVictim);
}

//-----------------------------------------------------------------------------
// Purpose: Work out what killed the player, and send a message to all clients about it
// Input  : *pVictim - 
//			*pKiller - 
//			*pInflictor - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::DeathNotice(CBaseEntity *pVictim, CBaseEntity *pKiller, CBaseEntity *pInflictor)
{
}

//-----------------------------------------------------------------------------
// Purpose: Player picked up a weapon
// Input  : *pPlayer - 
//			*pWeapon - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon)
{
}

//-----------------------------------------------------------------------------
// Purpose: what is the time in the future at which this weapon may spawn?
// Input  : *pWeapon - 
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesSinglePlay::FlWeaponRespawnTime(CBasePlayerItem *pWeapon)
{
	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: Weapon tries to respawn
// Input  : *pWeapon - 
// Output : float  the time at which it can try to spawn again (0 == now)
//-----------------------------------------------------------------------------
float CGameRulesSinglePlay::FlWeaponTryRespawn(CBasePlayerItem *pWeapon)
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: where should this weapon spawn?
// Some game variations may choose to randomize spawn locations
// Input  : *pWeapon - 
// Output : Vector
//-----------------------------------------------------------------------------
Vector CGameRulesSinglePlay::VecWeaponRespawnSpot(CBasePlayerItem *pWeapon)
{
	return pWeapon->pev->origin;
}

//-----------------------------------------------------------------------------
// Purpose: any conditions inhibiting the respawning of this weapon?
// Input  : *pWeapon - 
// Output : int
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::WeaponShouldRespawn(CBasePlayerItem *pWeapon)
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//			*pItem - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::CanHaveItem(CBasePlayer *pPlayer, CItem *pItem)
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//			*pItem - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem)
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pItem - 
// Output : int
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::ItemShouldRespawn(CItem *pItem)
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: At what time in the future may this Item respawn?
// Input  : *pItem - 
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesSinglePlay::FlItemRespawnTime(CItem *pItem)
{
	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: Some game variations may choose to randomize spawn locations
// Input  : *pItem - 
// Output : Vector
//-----------------------------------------------------------------------------
Vector CGameRulesSinglePlay::VecItemRespawnSpot(CItem *pItem)
{
	return pItem->pev->origin;
}

//-----------------------------------------------------------------------------
// Purpose: Entity restrictions may apply here
// Input  : *pEntity - 
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::IsAllowedToSpawn(CBaseEntity *pEntity)
{
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//			*szName - 
//			iCount - 
//-----------------------------------------------------------------------------
void CGameRulesSinglePlay::PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, const int &iCount)
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pAmmo - 
// Output : int
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::AmmoShouldRespawn(CBasePlayerAmmo *pAmmo)
{
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: Ammo respawn time (absolute value, gpGlobals->time)
// Input  : *pAmmo - 
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesSinglePlay::FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo)
{
	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: Wall-mounted charger renew time
// Output : float
//-----------------------------------------------------------------------------
float CGameRulesSinglePlay::FlHealthChargerRechargeTime(void)
{
	return 0;// don't recharge
}

//-----------------------------------------------------------------------------
// Purpose: which weapons should be packed and dropped
// Input  : *pPlayer - 
// Output : int GR_NONE
//-----------------------------------------------------------------------------
int CGameRulesSinglePlay::DeadPlayerWeapons(CBasePlayer *pPlayer)
{
	return GR_PLR_DROP_GUN_NO;
}

//-----------------------------------------------------------------------------
// Purpose: which ammo should be packed and dropped
// Input  : *pPlayer - 
// Output : int GR_NONE
//-----------------------------------------------------------------------------
int CGameRulesSinglePlay::DeadPlayerAmmo(CBasePlayer *pPlayer)
{
	return GR_PLR_DROP_AMMO_NO;
}

//-----------------------------------------------------------------------------
// Purpose: determines relationship between given player and entity
// Input  : *pPlayer - 
//			*pTarget - 
// Output : int
//-----------------------------------------------------------------------------
int CGameRulesSinglePlay::PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget)
{
	// why would a single player in half life need this? 
	return GR_NOTTEAMMATE;
}

//-----------------------------------------------------------------------------
// Purpose: Are effects allowed on this server?
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::FAllowEffects(void)// XDM
{
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: Get player with the best score
// Input  : team - may be TEAM_NONE
// Output : CBasePlayer *
//-----------------------------------------------------------------------------
CBasePlayer *CGameRulesSinglePlay::GetBestPlayer(TEAM_ID team)
{
	return UTIL_ClientByIndex(gpGlobals->maxClients);// just don't want to put "1" here
}

//-----------------------------------------------------------------------------
// Purpose: client entered a console command
// Input  : *pPlayer - client
//			*pcmd - command line, use CMD_ARGC() and CMD_ARGV()
// Output : Returns TRUE on success, FALSE on failure.
//-----------------------------------------------------------------------------
bool CGameRulesSinglePlay::ClientCommand(CBasePlayer *pPlayer, const char *pcmd)
{
/*	const char *arg1 = CMD_ARGV(1);
	const char *arg2 = CMD_ARGV(2);
	const char *arg3 = CMD_ARGV(3);
*/
	if (DeveloperCommand(pPlayer, pcmd))
	{
	}
	else if (FStrEq(pcmd, "VModEnable"))
	{
	}
	else return false;

	return true;
}
