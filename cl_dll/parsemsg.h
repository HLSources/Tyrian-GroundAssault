#ifndef PARSEMSG_H
#define PARSEMSG_H
#ifdef _WIN32
#ifndef __MINGW32__
#pragma once
#endif /* not __MINGW32__ */
#endif

#include "protocol.h"

#define MSG_STRING_BUFFER	MAX_MESSAGE_STRING//2048

void BEGIN_READ(void *buffer, int size);
int READ_REMAINING(void);// XDM3035
int END_READ(void);// XDM3035

signed char READ_CHAR(void);
byte READ_BYTE(void);
short READ_SHORT(void);
word READ_WORD(void);
long READ_LONG(void);
float READ_FLOAT(void);
float READ_COORD(void);
float READ_ANGLE(void);
float READ_HIRESANGLE(void);
char *READ_STRING(void);

#endif // PARSEMSG_H
