#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <asm/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/mount.h>

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/fb.h>
#include "videodev.h"
#include "videodev2.h"
#include "spcav4l.h"
#include "utils.h"
#include "picture.h"

static int debug = 1;
#define DBG(format, args...) \
	do { \
		if (debug >= 1) \
			fprintf(stderr,format, ##args); \
	} while (0)

#define 	V4L2_DEV_NODE 		"/dev/v4l/video0"

char preview_buf[240*320*2];


int main(int argc, char *argv[])
{
	int v4l2_fd = -1;
	static int	preview_frames = 3;
	int i,ret;
	int jpgno;
  	char temp[80];
	time_t curdate;
	struct tm *tdate;
	float  tmpvalue;


	DBG("Start Main \n");
	v4l2_fd = open(V4L2_DEV_NODE, O_RDWR);
	if (v4l2_fd < 0)
	{
		perror(V4L2_DEV_NODE);
		return 1;
	}

	jpgno=0;
	DBG("Create test00.jpg. \n");
	for (i = 0; i < preview_frames; i++)
	{
		if ((ret = read(v4l2_fd, &preview_buf, 320*240*2)) < 0)
		{
			perror("read");
			return 0;
		}
	}
	getJpegPicture(preview_buf,320,240,VIDEO_PALETTE_RGB565,320*240*2,PICTURE,NULL,jpgno);
	usleep(100);

	tmpvalue = 0x100*2.48*1000/1024;
  	memset (temp, '\0', sizeof (temp));
  	time (&curdate);
  	tdate = localtime (&curdate);
  	snprintf (temp, 31, "%02d:%02d:%04d-%02d:%02d:%02d", tdate->tm_mon + 1, tdate->tm_mday, tdate->tm_year + 1900,
	    						tdate->tm_hour, tdate->tm_min, tdate->tm_sec);

	DBG("Create test01.jpg. \n");
	for (i = 0; i < preview_frames; i++)
	{
		if ((ret = read(v4l2_fd, &preview_buf, 320*240*2)) < 0)
		{
			perror("read");
			return 0;
		}
	}
	jpgno++;
	getJpegPicture(preview_buf,320,240,VIDEO_PALETTE_RGB565,320*240*2,PICTURE,NULL,jpgno);
	usleep(100);

	close(v4l2_fd);
	DBG("End ! \n");
	return 0;
}

