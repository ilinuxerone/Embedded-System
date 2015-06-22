PREFIX = ${HOME}
CONSOLEFONTS = /usr/share/kbd/consolefonts
XFONTS = /usr/share/fonts

CC=arm-linux-gcc

EXES = v4ltest v4ltune

CFLAGS+=-Wall

all: $(EXES)

v4ltest: v4l-test.o struct-dump.o struct-v4l.o struct-v4l2.o
	$(CC) $(CFLAGS) v4l-test.o struct-dump.o struct-v4l.o struct-v4l2.o -o v4ltest

v4ltune: v4l-tune.o struct-dump.o struct-v4l.o struct-v4l2.o
	$(CC) $(CFLAGS) v4l-tune.o struct-dump.o struct-v4l.o struct-v4l2.o -o v4ltune

install: all
	install -d $(PREFIX)/bin
	cp -v v4ltest v4ltune tune tuneall $(PREFIX)/bin

tar: clean
	( cd .. ; tar jcvf ${HOME}/v4ltest-0.1.2.tar.bz2 v4ltest )

clean:
	-rm -f *.o $(EXES)

mrproper: clean
