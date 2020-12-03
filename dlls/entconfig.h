#ifndef ENTCONFIG_H
#define ENTCONFIG_H

char *SV_ParseEdict(char *pData);
void ENTCONFIG_Init(void);
void ENTCONFIG_ExecMapPreConfig(void);
void ENTCONFIG_ExecMapConfig(void);
bool ENTCONFIG_WriteEntity(entvars_t *pev);
bool ENTCONFIG_Evaluate(const int &left, char *cmp_operator, const int &right);
bool ENTCONFIG_ValidateCommand(void);

#define MAX_ENTITY_KEYVALUES		256

extern bool g_MapConfigCommands;
extern bool g_MultiplayerOnlyCommands;
extern int g_iGamerulesSpecificCommands;
extern int g_iNumEntitiesAddedExternally;
extern char g_iGamerulesSpecificCommandsOp[4];

#endif // ENTCONFIG_H
