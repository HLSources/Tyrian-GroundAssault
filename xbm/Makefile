# undone
ARCH=i486
CPP = gcc-linux-2.95
BASEFLAGS = -Dstricmp=strcasecmp -Dstrcmpi=strcasecmp
CPPFLAGS = ${BASEFLAGS} -m486 -O2 -w -I../dlls -I../engine -I../common -I../pm_shared

OBJ = bot.o bot_chat.o bot_client.o bot_combat.o bot_commands.o bot_navigate.o dll.o engine.o h_export.o linkfunc.o namefunc.o util.o waypoint.o

XBM_$(ARCH).so: ${OBJ}
	${CPP} -fPIC -shared -o $@ ${OBJ} -Xlinker -Map -Xlinker XBM.map -ldl

clean:
	-rm -f *.o
	-rm -f *.so	

%.o:	%.cpp
	${CPP} ${CPPFLAGS} -c $< -o $@

%.o:	%.c
	${CPP} ${CPPFLAGS} -c $< -o $@
