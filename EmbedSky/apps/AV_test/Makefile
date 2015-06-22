CC=/opt/EmbedSky/crosstools_3.4.5_softfloat/gcc-3.4.5-glibc-2.3.6/arm-linux/bin/arm-linux-gcc
LD=/opt/EmbedSky/crosstools_3.4.5_softfloat/gcc-3.4.5-glibc-2.3.6/arm-linux/bin/arm-linux-ld

AV_EXEC=AV_test
AV_OBJS=AV_test.o

CFLAGS +=
LDFLAGS +=

all:$(AV_EXEC) $(AV_EXEC)
	rm *.o

$(AV_EXEC):$(AV_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(AV_OBJS) $(LDLIBS$(LDLIBS_$@))

clean:
	-rm -f $(AV_EXEC) *.elf *.gdb *.o
