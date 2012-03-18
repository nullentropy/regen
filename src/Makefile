CC=gcc
CFLAGS=`pkg-config --cflags gtk+-2.0`
LDFLAGS=`pkg-config --libs gtk+-2.0 gthread-2.0`

regen: main/commands.o main/config.o main/channel.o main/user.o main/inbound.o main/irc.o main/network.o main/regen.o main/util.o main/server.o gui/gui.o gui/window.o gui/switchbar.o gui/display.o
	${CC} ${LDFLAGS} -o regen main/commands.o main/config.o main/channel.o main/user.o main/inbound.o main/irc.o main/network.o main/regen.o main/util.o main/server.o gui/gui.o gui/window.o gui/switchbar.o gui/display.o

main/%.o: %.c
	${CC} ${CFLAGS} -c main/$<

gui/%.o: %.c
	${CC} ${CFLAGS} -c gui/$<

clean:
	rm gui/*.o main/*.o regen
