#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <linux/watchdog.h>

int main(int argc,char **argv)
{
	int fd = 0;
	int n =0;
	fd = open("/dev/watchdog",O_RDONLY );

	if(fd < 0) {
		perror("/dev/watchdog");
		return -1;
	}
	for(;;) {
		ioctl(fd,WDIOC_KEEPALIVE);
		sleep(3);
	}
	close(fd);
	return 0;
}
