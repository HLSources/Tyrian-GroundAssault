#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "bot.h"
#include "bot_cvar.h"

#define NUM_TAGS 22

char *tag1[NUM_TAGS]={
"-=","-[","-]","-}","-{","<[","<]","[-","]-","{-","}-","[[","[","{","]","}","<",">","-","|","=","+"};
char *tag2[NUM_TAGS]={
"=-","]-","[-","{-","}-","]>","[>","-]","-[","-}","-{","]]","]","}","[","{",">","<","-","|","=","+"};

size_t bot_chat_count = 0;
size_t bot_taunt_count = 0;
size_t bot_whine_count = 0;

bot_chat_t bot_chat[MAX_BOT_CHAT];
bot_chat_t bot_taunt[MAX_BOT_CHAT];
bot_chat_t bot_whine[MAX_BOT_CHAT];

int recent_bot_chat[BOT_CHAT_RECENT_ITEMS];
int recent_bot_taunt[BOT_CHAT_RECENT_ITEMS];
int recent_bot_whine[BOT_CHAT_RECENT_ITEMS];

// XHL NOTE: this code is old and shitty!
// TODO: Remove all hard-coded string length values!

void LoadBotChat(void)
{
	FILE *bfp;
	char filename[256];
	char buffer[256];
	char *stat;
	int section = -1;
	int i, length;
	bot_chat_count = 0;
	bot_taunt_count = 0;
	bot_whine_count = 0;
	for (i=0; i < BOT_CHAT_RECENT_ITEMS; ++i)
	{
		recent_bot_chat[i] = -1;
		recent_bot_taunt[i] = -1;
		recent_bot_whine[i] = -1;
	}
	char *chatcfgfile;// XBM
	if (mod_id == TYRIAN_DLL)
		chatcfgfile = "botchat.txt";
	else
		chatcfgfile = "HPB_bot_chat.txt";

	UTIL_BuildFileName(filename, chatcfgfile, NULL);
	bfp = fopen(filename, "r");
	while (bfp != NULL)
	{
		stat = fgets(buffer, BOT_CHAT_STRING_LEN, bfp);
		if (stat == NULL)
		{
			fclose(bfp);
			bfp = NULL;
			continue;
		}
		buffer[BOT_CHAT_STRING_LEN] = 0;  // truncate lines longer than %d characters
		length = strlen(buffer);
		if (buffer[length-1] == '\n')
		{
			buffer[length-1] = 0;  // remove '\n'
			length--;
		}
		if (strcmp(buffer, "[bot_chat]") == 0)
		{
			section = 0;
			continue;
		}
		if (strcmp(buffer, "[bot_taunt]") == 0)
		{
			section = 1;
			continue;
		}
		if (strcmp(buffer, "[bot_whine]") == 0)
		{
			section = 2;
			continue;
		}
		if (length > 0)
		{
			if ((section == 0) && (bot_chat_count < MAX_BOT_CHAT))  // bot chat
			{
				if (buffer[0] == '!')
				{
					strcpy(bot_chat[bot_chat_count].text, &buffer[1]);
					bot_chat[bot_chat_count].can_modify = false;
				}
				else
				{
					strcpy(bot_chat[bot_chat_count].text, buffer);
					bot_chat[bot_chat_count].can_modify = true;
				}
				bot_chat[bot_chat_count].text[BOT_CHAT_STRING_LEN] = 0;
				bot_chat_count++;
			}
			else if ((section == 1) && (bot_taunt_count < MAX_BOT_CHAT))  // bot taunt
			{
				if (buffer[0] == '!')
				{
					strcpy(bot_taunt[bot_taunt_count].text, &buffer[1]);
					bot_taunt[bot_taunt_count].can_modify = false;
				}
				else
				{
					strcpy(bot_taunt[bot_taunt_count].text, buffer);
					bot_taunt[bot_taunt_count].can_modify = true;
				}
				bot_taunt[bot_taunt_count].text[BOT_CHAT_STRING_LEN] = 0;
				bot_taunt_count++;
			}
			else if ((section == 2) && (bot_whine_count < MAX_BOT_CHAT))  // bot whine
			{
				if (buffer[0] == '!')
				{
					strcpy(bot_whine[bot_whine_count].text, &buffer[1]);
					bot_whine[bot_whine_count].can_modify = false;
				}
				else
				{
					strcpy(bot_whine[bot_whine_count].text, buffer);
					bot_whine[bot_whine_count].can_modify = true;
				}
				bot_whine[bot_whine_count].text[BOT_CHAT_STRING_LEN] = 0;
				bot_whine_count++;
			}
			else
				ALERT(at_console, "Warning: Unknown section or overflow in '%s'\n", chatcfgfile);
		}
	}
}

void BotTrimBlanks(const char *in_string, char *out_string, const size_t max_len)
{
	size_t i, pos;
	char *dest;

	pos = 0;
	while ((pos < max_len) && (in_string[pos] == ' '))  // skip leading blanks
		pos++;

	dest = &out_string[0];
	while ((pos < max_len) && (in_string[pos]))
	{
		*dest++ = in_string[pos];
		pos++;
	}
	*dest = 0;  // store the null

	i = strlen(out_string) - 1;
	while ((i > 0) && (out_string[i] == ' '))  // remove trailing blanks
	{
		out_string[i] = 0;
		i--;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Try to remove shitty clan tags from player names
// Input  : *original_name - 
//			*out_name - 
//			max_len - 
// Output : int
//-----------------------------------------------------------------------------
bool BotChatTrimTag(const char *original_name, char *out_name, const size_t max_len)
{
	int i;
	bool result = false;
	char *pos1 = NULL, *pos2 = NULL, *src, *dest;
	char *in_name = new char[max_len];

	strncpy(in_name, original_name, max_len);
	in_name[max_len-1] = 0;

	size_t tag1l = 0;
	for (i=0; i < NUM_TAGS; ++i)
	{
//		pos1 = strstr(in_name, tag1[i]);
// not just single char		if (in_name[0] != tag1[i])// XDM3037: only search for tags at the beginning
		tag1l = strlen(tag1[i]);
		if (strncmp(in_name, tag1[i], tag1l))
			continue;

		pos1 = in_name;
		if (pos1)
			pos2 = strstr(pos1+tag1l, tag2[i]);
		else
			pos2 = NULL;
		
		if (pos1 && pos2 && pos1 < pos2)
			break;
	}

	if (pos1 == NULL)// not found? search AFTER the name
	{
		size_t inlen = strlen(in_name);
		size_t tag2l = 0;
		for (i=0; i < NUM_TAGS; ++i)
		{
			tag2l = strlen(tag2[i]);
			if (strncmp(in_name+(inlen-tag2l), tag2[i], tag2l))
				continue;

			pos2 = in_name+(inlen-tag2l);
			if (pos2)
				pos1 = strstr(in_name, tag2[i]);
			else
				pos1 = NULL;

			if (pos1 && pos2 && pos1 < pos2)
				break;
		}
	}

	if (pos1 && pos2 && pos1 < pos2)
	{
		src = pos2 + strlen(tag2[i]);
		dest = pos1;
		while (*src)
			*dest++ = *src++;

		*dest = *src;  // copy the null;
		result = true;
	}

	strcpy(out_name, in_name);
	BotTrimBlanks(out_name, in_name, max_len);
	if (strlen(in_name) == 0)  // is name just a tag?
	{
		strncpy(in_name, original_name, max_len);
		in_name[max_len-1] = 0;
		// strip just the tag part...
		for (i=0; i < NUM_TAGS; ++i)
		{
			pos1=strstr(in_name, tag1[i]);
			if (pos1)
				pos2 = strstr(pos1+strlen(tag1[i]), tag2[i]);
			else
				pos2 = NULL;
			
			if (pos1 && pos2 && pos1 < pos2)
			{
				src = pos1 + strlen(tag1[i]);
				dest = pos1;
				while (*src)
					*dest++ = *src++;
				*dest = *src;  // copy the null;
				src = pos2 - strlen(tag2[i]);
				*src = 0; // null out the rest of the string
			}
		}
	}

	BotTrimBlanks(in_name, out_name, max_len);
	out_name[max_len-1] = 0;
	delete [] in_name;
	return result;
}

//-----------------------------------------------------------------------------
// Purpose: Humanify players name, pretend someone is typing it (omit tags, case)
// Input  : *original_name - 
//			*out_name - 
//-----------------------------------------------------------------------------
void BotChatName(const char *original_name, char *out_name, const size_t max_len)
{
	size_t pos;
	if (RANDOM_LONG(1, 100) <= g_bot_chat_tag_percent.value)
	{
		char *temp_name = new char[max_len];
		strncpy(temp_name, original_name, max_len);
		temp_name[max_len-1] = 0;
		while (BotChatTrimTag(temp_name, out_name, max_len))
			strcpy(temp_name, out_name);

		delete [] temp_name;
	}
	else
	{
		strncpy(out_name, original_name, max_len);
	}
	out_name[max_len-1] = 0;

	// bots tendency to write in lower case
	if (RANDOM_LONG(1, 100) <= g_bot_chat_lower_percent.value)
	{
		pos = 0;
		while ((pos < max_len) && (out_name[pos]))
		{
			out_name[pos] = tolower(out_name[pos]);
			++pos;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Randomly write initial text in lowercase
// Warning: keep strings size == BOT_CHAT_STRING_LEN!
// Input  : *in_text - 
//			*out_text - 
//-----------------------------------------------------------------------------
void BotChatText(const char *in_text, char *out_text, const size_t max_len)
{
	if (RANDOM_LONG(1, 100) <= g_bot_chat_lower_percent.value)
	{
		size_t pos = 0;
		char *temp_text = new char[max_len];
		temp_text[max_len-1] = 0;
		while (in_text[pos] && (pos < max_len-1))//temp_text[pos])
		{
			temp_text[pos] = tolower(in_text[pos]);//temp_text[pos]);
			++pos;
		}
		if (pos >= max_len)
			pos = max_len;

		temp_text[pos] = 0;// this iteration was skipped
		strcpy(out_text, temp_text);
//		strncpy(out_text, temp_text, max_len-2);
		delete [] temp_text;
	}
	else
	{
		strncpy(out_text, in_text, max_len);
		out_text[max_len-1] = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Fill name tags
// Input  : *bot_say_msg - final text to print
//			*chat_text - an input string containing tags
//			*chat_name - name to fill in %n and NOT to fill in %r
//-----------------------------------------------------------------------------
void BotChatFillInName(bot_t *pBot, char *bot_say_msg, const char *chat_text, const char *chat_name)
{
	if (bot_say_msg == NULL)
		return;
	if (chat_text == NULL)
		return;

	char random_name[BOT_NAME_LEN];
	int name_offset = 0, rand_offset = 0;
	char *name_pos = NULL;
	char *rand_pos = NULL;

	size_t chat_index = 0;
	size_t say_index = 0;
	bot_say_msg[0] = 0;

	try
	{
	name_pos = strstr(&chat_text[chat_index], "%n");
	rand_pos = strstr(&chat_text[chat_index], "%r");

	if (name_pos == NULL && chat_name != NULL)// if there's no name tag, but the message is addressed to someone, just add name first
	{
		sprintf(bot_say_msg, "%s: ", chat_name);
		say_index = strlen(chat_name) + 2;// ": "
		bot_say_msg[say_index] = 0;
	}

	while (name_pos || rand_pos)
	{
		if (name_pos)
			name_offset = name_pos - chat_text;
		if (rand_pos)
			rand_offset = rand_pos - chat_text;

		if ((rand_pos == NULL) || ((name_offset < rand_offset) && name_pos))
		{
			if (chat_name)
			{
				while (&chat_text[chat_index] < name_pos)
					bot_say_msg[say_index++] = chat_text[chat_index++];

				bot_say_msg[say_index] = 0;  // add null terminator
				chat_index += 2;  // skip the "%n"
				strcat(bot_say_msg, chat_name);
				say_index += strlen(chat_name);
				bot_say_msg[say_index] = 0;
			}
		}
		else // use random player name...
		{
			bool is_bad;
			byte count = 0;// number of iterations
			int player_index = RANDOM_LONG(1, gpGlobals->maxClients);// XDM3035: new method, faster. // start random, follow sequentially to avoid repeats
			edict_t *pPlayer = NULL;
			const char *pName = NULL;
			do
			{
				is_bad = true;
				pPlayer = UTIL_ClientByIndex(player_index);
				if (pBot)// TESTME
				{
					if (pPlayer == pBot->pEdict)// don't randomly insert MY name
						is_bad = true;
				}
				if (pPlayer && (pPlayer->v.netname != 0))
				{
					pName = STRING(pPlayer->v.netname);
					if (chat_name && strcmp(pName, chat_name) == 0)
						is_bad = true;// all names are valid EXCEPT for the chat_name
					else
						is_bad = false;
				}
				++count;
				if (player_index < gpGlobals->maxClients)
					++player_index;
				else
					player_index = 1;// loop
			}
			while (is_bad && count < MAX_PLAYERS);

			if (pName)// TESTME
			{
				BotChatName(pName, random_name, BOT_NAME_LEN);

				while (&chat_text[chat_index] < rand_pos)
					bot_say_msg[say_index++] = chat_text[chat_index++];

				bot_say_msg[say_index] = 0;  // add null terminator
				chat_index += 2;  // skip the "%r"
				strcat(bot_say_msg, random_name);
				say_index += strlen(random_name);
				bot_say_msg[say_index] = 0;
			}
/*			else
			{
				throw "BotChatFillInName Got NO NAME!";
			}*/
		}
		name_pos = strstr(&chat_text[chat_index], "%n");
		rand_pos = strstr(&chat_text[chat_index], "%r");
	}
	// copy the rest of the chat_text into the bot_say_msg...
	while (chat_text[chat_index])
		bot_say_msg[say_index++] = chat_text[chat_index++];
	
	bot_say_msg[say_index] = 0;  // add null terminator

	}
	catch(...)
	{
		printf("XBM: BotChatFillInName(%s, %s, %s) exception!\n", bot_say_msg, chat_text, chat_name);
	}
}

/*
void BotChatAnalyzeText(const char *in_text, edict_t *pFrom)
{
}
*/

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pBot - 
//			*pText - 
//			delay - 
//-----------------------------------------------------------------------------
void BotSpeakDirect(bot_t *pBot, const char *pText, float delay)
{
	if (pBot && pText)
	{
		strncpy(pBot->bot_say_msg, pText, 256);
		pBot->b_bot_say = true;
		pBot->f_bot_say = gpGlobals->time + delay;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Fill bot_say_msg with some random chat and delay a little bit
// Input  : *pBot - who is talking
//-----------------------------------------------------------------------------
void BotSpeakChat(bot_t *pBot)
{
	if (bot_chat_count == 0)
		return;
	if (pBot == NULL)
		return;

	int chat_index = 0;
	bool used;
	int i, recent_count;
	char chat_text[BOT_CHAT_STRING_LEN];
	recent_count = 0;
	while (recent_count < 5)
	{
		chat_index = RANDOM_LONG(0, bot_chat_count-1);
		used = FALSE;
		for (i=0; i < BOT_CHAT_RECENT_ITEMS; ++i)
		{
			if (recent_bot_chat[i] == chat_index)
				used = TRUE;
		}
		if (used)
			++recent_count;
		else
			break;
	}

	for (i=BOT_CHAT_RECENT_ITEMS-1; i > 0; --i)
		recent_bot_chat[i] = recent_bot_chat[i-1];

	recent_bot_chat[0] = chat_index;
	if (bot_chat[chat_index].can_modify)
		BotChatText(bot_chat[chat_index].text, chat_text, BOT_CHAT_STRING_LEN);
	else
		strcpy(chat_text, bot_chat[chat_index].text);

//	ALERT(at_console, "XBM: BotSpeakChat(%s): '%s'\n", pBot->name, pBot->bot_say_msg);
	try
	{
	BotChatFillInName(pBot, pBot->bot_say_msg, chat_text, NULL);
	}
	catch(...)
	{
		printf("BotSpeakChat() exception!!!!!!!\n");
		DBG_FORCEBREAK
	}
//	ALERT(at_console, "XBM: BotSpeakChat(%s): '%s'\n", pBot->name, pBot->bot_say_msg);
	// set chat flag and time to chat...
	pBot->b_bot_say = true;
	pBot->f_bot_say = gpGlobals->time + RANDOM_FLOAT(5.0, 10.0);
}

//-----------------------------------------------------------------------------
// Purpose: Fill bot_say_msg with some taunt and delay a little bit
// Input  : *pBot - who is talking
//			*pVictim - who's name to fill
//-----------------------------------------------------------------------------
void BotSpeakTaunt(bot_t *pBot, edict_t *pVictim)
{
	if (bot_taunt_count == 0)
		return;
	if (pBot == NULL || pVictim == NULL)
		return;

	char chat_text[BOT_CHAT_STRING_LEN];
	char chat_name[BOT_NAME_LEN];
	char temp_name[BOT_NAME_LEN];
	int taunt_index = 0;
	bool used;
	int i, recent_count;
	recent_count = 0;
	while (recent_count < 5)
	{
		taunt_index = RANDOM_LONG(0, bot_taunt_count-1);
		used = FALSE;
		for (i=0; i < BOT_CHAT_RECENT_ITEMS; ++i)
		{
			if (recent_bot_taunt[i] == taunt_index)
				used = TRUE;
		}
		if (used)
			recent_count++;
		else
			break;
	}

	for (i=BOT_CHAT_RECENT_ITEMS-1; i > 0; --i)
		recent_bot_taunt[i] = recent_bot_taunt[i-1];

	recent_bot_taunt[0] = taunt_index;

	if (bot_taunt[taunt_index].can_modify)
		BotChatText(bot_taunt[taunt_index].text, chat_text, BOT_CHAT_STRING_LEN);
	else
		strcpy(chat_text, bot_taunt[taunt_index].text);

	GetEntityPrintableName(pVictim, temp_name, BOT_NAME_LEN);
	BotChatName(temp_name, chat_name, BOT_NAME_LEN);
	BotChatFillInName(pBot, pBot->bot_say_msg, chat_text, chat_name);
	// set chat flag and time to chat...
	pBot->b_bot_say = true;
	pBot->f_bot_say = gpGlobals->time + RANDOM_FLOAT(2.0, 5.0);
}

//-----------------------------------------------------------------------------
// Purpose: Fill bot_say_msg with some whining and delay a little bit
// Input  : *pBot - who is talking
//			*pKiller - who's name to fill
//-----------------------------------------------------------------------------
void BotSpeakWhine(bot_t *pBot, edict_t *pKiller)
{
	if (bot_whine_count == 0)
		return;
	if (pBot == NULL || pKiller == NULL)
		return;

//...	int entitytype = XDM_EntityIs(pent);

	int whine_index = 0;
	bool used;
	int i, recent_count;
	char chat_text[BOT_CHAT_STRING_LEN];
	char chat_name[BOT_NAME_LEN];
	char temp_name[BOT_NAME_LEN];
	recent_count = 0;
	while (recent_count < 5)
	{
		whine_index = RANDOM_LONG(0, bot_whine_count-1);
		used = FALSE;
		for (i=0; i < BOT_CHAT_RECENT_ITEMS; ++i)
		{
			if (recent_bot_whine[i] == whine_index)
				used = TRUE;
		}
		if (used)
			++recent_count;
		else
			break;
	}

	for (i=BOT_CHAT_RECENT_ITEMS-1; i > 0; --i)
		recent_bot_whine[i] = recent_bot_whine[i-1];

	recent_bot_whine[0] = whine_index;

	if (bot_whine[whine_index].can_modify)
		BotChatText(bot_whine[whine_index].text, chat_text, BOT_CHAT_STRING_LEN);
	else
		strcpy(chat_text, bot_whine[whine_index].text);

	GetEntityPrintableName(pKiller, temp_name, BOT_NAME_LEN);
	BotChatName(temp_name, chat_name, BOT_NAME_LEN);
	BotChatFillInName(pBot, pBot->bot_say_msg, chat_text, chat_name);
	// set chat flag and time to chat...
	pBot->b_bot_say = true;
	pBot->f_bot_say = gpGlobals->time + RANDOM_FLOAT(3.0, 8.0);
}
