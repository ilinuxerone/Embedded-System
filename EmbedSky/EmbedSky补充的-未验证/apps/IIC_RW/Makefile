CROSS=arm-linux-

all: i2c_rw

i2c_rw:i2c_rw.c
	$(CROSS)gcc -o i2c_rw i2c_rw.c
	$(CROSS)strip i2c_rw
clean:
	@rm -vf i2c_rw *.o *~
