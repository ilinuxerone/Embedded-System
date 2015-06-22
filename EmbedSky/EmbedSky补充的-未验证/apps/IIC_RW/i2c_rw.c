/*************************************

NAME:i2c_rw.c
COPYRIGHT:www.embedsky.net

*************************************/

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <unistd.h>
#include <stdlib.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <assert.h>
#include <string.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define CHIP_ADDR	0x50		//设备地址
#define I2C_DEV		"/dev/i2c-0"

struct eeprom
{
	char *dev; 			// 设备名称，i2c-0
	int addr;				// 设备中地址
	int fd;				// 设备句柄
};

static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data)
{
	struct i2c_smbus_ioctl_data args;

	args.read_write = read_write;
	args.command = command;
	args.size = size;
	args.data = data;
	return ioctl(file, I2C_SMBUS, &args);
}

int eeprom_read_byte(struct eeprom* e, __u16 mem_addr)
{
	int r;
	union i2c_smbus_data data;

	ioctl(e->fd, BLKFLSBUF);

	__u8 buf =  mem_addr & 0x0ff;
	r = i2c_smbus_access(e->fd, I2C_SMBUS_WRITE, buf, I2C_SMBUS_BYTE, NULL);;
	if (r < 0)
		return r;
	if (i2c_smbus_access(e->fd, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data))
		return -1;
	else
		return 0x0FF & data.byte;
}

int eeprom_write_byte(struct eeprom *e, __u16 mem_addr, __u8 data)
{
	int r;
	__u8 command = mem_addr & 0x00ff;
	union i2c_smbus_data i2cdata;
	i2cdata.byte = data;
	r = i2c_smbus_access(e->fd, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &i2cdata);

	if(r < 0)
		printf("iic write error");
	usleep(10);
	return r;
}

int eeprom_open(char *dev_name, int addr, struct eeprom *e)
{
	int funcs, fd, r;
	e->fd = e->addr = 0;
	e->dev = 0;
	
	fd = open(dev_name, O_RDWR);
	if(fd <= 0)
	{
		printf("iic open error\n");
		return -1;
	}

	// set working device
	ioctl(fd, I2C_TENBIT, 0);
	ioctl(fd, I2C_SLAVE, addr);

	e->fd = fd;
	e->addr = addr;
	e->dev = dev_name;
	return 0;
}

int eeprom_close(struct eeprom *e)
{
	close(e->fd);
	e->fd = -1;
	e->dev = 0;
	return 0;
}

static int read_from_eeprom(struct eeprom *e, int addr, int size)
{
	int temp, i, ret;
	for(i = 0; i < size; ++i, ++addr)
	{
		temp = eeprom_read_byte(e, addr);
		if(temp < 0)
		{
			printf("iic read error !");
		}
		if( (i % 16) == 0 ) 
			printf("\n %.4x|  ", addr);
		else if( (i % 8) == 0 ) 
			printf("  ");
		printf("%.2x ", temp);
		fflush(stdout);
	}
	printf("\n\n");
	return 0;
}

static int write_to_eeprom(struct eeprom *e, int addr)
{
	int i, ret;
	for(i=0, addr=0; i<256; i++, addr++)
	{
		if( (i % 16) == 0 ) 
			printf("\n %.4x|  ", addr);
		else if( (i % 8) == 0 ) 
			printf("  ");
		printf("%.2x ", 255-i);
		fflush(stdout);
		ret = eeprom_write_byte(e, addr, 255-i);
		if(ret < 0)
		{
			printf("iic write error !");
		}
	}
	fprintf(stderr, "\n\n");
	return 0;
}

int main (void)
{
	struct eeprom e;
	int ret;

	ret = eeprom_open(I2C_DEV, CHIP_ADDR, &e);
	if(ret < 0)
	{
		printf("i2c device (AT24C02) open failed !\n");
		return (-1);
	}

	printf("  Writing 0x00-0xff into AT24C02 \n");
	write_to_eeprom(&e, 0);

	printf("  Reading 256 bytes from 0x0\n");
	read_from_eeprom(&e, 0, 256);

	eeprom_close(&e);

	return(0);
}
