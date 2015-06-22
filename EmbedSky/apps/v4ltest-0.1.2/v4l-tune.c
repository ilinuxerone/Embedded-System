#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <ctype.h>
#include <assert.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "videodev.h"
#include "videodev2.h"

#include "struct-dump.h"
#include "struct-v4l.h"
#include "struct-v4l2.h"

/* --------------------------------------------------------------------- */
/* main								  	 */

int main(int argc, char *argv[])
{
	char *device = "/dev/video0";
	char *freq = "";
	int fd;
	int ok;
	struct v4l2_capability capability;
	struct v4l2_frequency frequency;
	struct v4l2_tuner tuner;
	v4l2_std_id std_id;

	if (argc > 1) {
		device = argv[1];
	}
	if (argc > 2) {
		freq = argv[2];
	}
	
	fd = open(device,O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr,"open %s: %s\n",device,strerror(errno));
		exit(1);
	};

	//printf("    VIDIOC_QUERYCAP\n");
	memset(&capability,0,sizeof(capability));
	if ((ok = ioctl(fd,VIDIOC_QUERYCAP,&capability)) == -1) {
		printf("    VIDIOC_QUERYCAP failed (%d) %s\n", errno, strerror(errno));
		exit(1);
	}
	//print_struct(stdout,desc_v4l2_capability,&capability,"",1);
	//printf("\n");

	//printf("    VIDIOC_G_STD\n");
	if ((ok = ioctl(fd,VIDIOC_G_STD,&std_id)) == -1) {
		printf("    VIDIOC_G_STD failed (%d) %s\n", errno, strerror(errno));
		exit(1);
	}
	//print_struct(stdout,desc_v4l2_std_id,&std_id,"",1);
	//printf("\n");

	if (strcmp(freq, "") == 0) {
		//printf("    VIDIOC_G_FREQUENCY\n");
		frequency.tuner = 0;
		if ((ok = ioctl(fd,VIDIOC_G_FREQUENCY,&frequency)) == -1) {
			printf("    VIDIOC_G_FREQUENCY failed (%d) %s\n", errno, strerror(errno));
			exit(1);
		}
		//print_struct(stdout,desc_v4l2_frequency,&frequency,"",1);
		printf("frequency=%d \"%d\"\n", frequency.frequency, frequency.frequency*125/2);
		//printf("\n");

		//printf("    VIDIOC_G_TUNER\n");
		tuner.index = 0;
		if ((ok = ioctl(fd,VIDIOC_G_TUNER,&tuner)) == -1) {
			printf("    VIDIOC_G_TUNER failed (%d) %s\n", errno, strerror(errno));
			exit(1);
		}
		//print_struct(stdout,desc_v4l2_tuner,&tuner,"",1);
		//printf("\n");
	} else {
		//printf("    VIDIOC_S_FREQUENCY\n");
		frequency.tuner = 0;
    		frequency.type = V4L2_TUNER_ANALOG_TV;
		frequency.frequency = ((atoi(freq)*2)+62) / 125;
		if (frequency.frequency == 0) {
			printf("invalid frequency \"%s\"\n", freq);
			exit(2);
		}
		printf("frequency=%d \"%s\"\n", frequency.frequency, freq);
		if ((ok = ioctl(fd,VIDIOC_S_FREQUENCY,&frequency)) == -1) {
			printf("    VIDIOC_S_FREQUENCY failed (%d) %s\n", errno, strerror(errno));
			exit(1);
		}
		//print_struct(stdout,desc_v4l2_frequency,&frequency,"",1);
		//printf("\n");
	}

	//printf("done\n");
	close(fd);

	return 0;
}
