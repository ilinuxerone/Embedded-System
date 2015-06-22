CC = arm-linux-gcc
LD = arm-linux-ld
EXEC = serial_test 
OBJS = serial_test.o 

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) -lpthread
	rm -f *.o

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o

