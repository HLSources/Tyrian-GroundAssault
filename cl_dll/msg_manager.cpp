#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include <string.h>
#include <stdio.h>
#include "msg_fx.h"

int __MsgFunc_MSGManager(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ( pbuf, iSize );
	int delta = 0;
	delta = (int)pbuf;
	int cmd = READ_BYTE();
	int res = 0;

	switch(cmd)
	{
		case MSG_HEALTH:
			res = gHUD.m_Health.MsgFunc_Health(pszName,iSize,pbuf);
		break;

		case MSG_ARMOR:
			res = gHUD.m_Battery.MsgFunc_Battery(pszName,iSize,pbuf);
		break;

		case MSG_TRAIN:
			res = gHUD.m_Train.MsgFunc_Train(pszName,iSize,pbuf);
		break;

		case MSG_LOGO:
			res = gHUD.MsgFunc_Logo(pszName, iSize, pbuf);

		case MSG_ITEM_ACCURACY:
			res = gHUD.m_ItemAccuracy.MsgFunc_ItemAccuracy(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_BANANA:
			res = gHUD.m_ItemBanana.MsgFunc_ItemBanana(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_INVISIBILITY:
			res = gHUD.m_ItemInvisibility.MsgFunc_ItemInvisibility(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_INVULNERABILITY:
			res = gHUD.m_ItemInvulnerability.MsgFunc_ItemInvulnerability(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_QUADDAMAGE:
			res = gHUD.m_ItemQuaddamage.MsgFunc_ItemQuaddamage(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_HASTE:
			res = gHUD.m_ItemHaste.MsgFunc_ItemHaste(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_RAPIDFIRE:
			res = gHUD.m_ItemRapidfire.MsgFunc_ItemRapidfire(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_SHIELD_STRENGTH:
			res = gHUD.m_ItemShieldStrength.MsgFunc_ItemShieldStrength(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_WEAPON_POWER:
			res = gHUD.m_ItemWeaponPower.MsgFunc_ItemWeaponPower(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_SHIELDREGEN:
			res = gHUD.m_ItemShieldregen.MsgFunc_ItemShieldregen(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_ANTIDOTE:
			res = gHUD.m_ItemAntidote.MsgFunc_ItemAntidote(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_FIRE_SUPRESSOR:
			res = gHUD.m_ItemFireSupressor.MsgFunc_ItemFireSupressor(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_LIGHTNING_FIELD:
			res = gHUD.m_ItemLightningField.MsgFunc_ItemLightningField(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_RADSHIELD:
			res = gHUD.m_ItemRadShield.MsgFunc_ItemRadShield(pszName,iSize,pbuf);
		break;

		case MSG_ITEM_PLASMA_SHIELD:
			res = gHUD.m_ItemPlasmaShield.MsgFunc_ItemPlasmaShield(pszName,iSize,pbuf);
		break;
	default:
			break;
	}
	END_READ();
	return res;
}

void initMsgManager()
{
	HOOK_MESSAGE(MSGManager);
}