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

#define VIDIOC_QUERYCTRL_MAX 0
#define MORE
/* --------------------------------------------------------------------- */
/* print struct sizes							  	 */

void print_control(const char *cmd_name, unsigned int cmd, unsigned int cmd_size)
{
 	unsigned int dir = cmd >> _IOC_DIRSHIFT & _IOC_DIRMASK;
 	unsigned int type = cmd >> _IOC_TYPESHIFT & _IOC_TYPEMASK;
 	unsigned int nr = cmd >> _IOC_NRSHIFT & _IOC_NRMASK;
 	unsigned int size = cmd >> _IOC_SIZESHIFT & _IOC_SIZEMASK;
	printf ("%-24.24s  cmd=0x%08X  dir=%u  type='%c'  nr=%2u  size=%3u %3u\n", 
		cmd_name, cmd, dir, type, nr, size, cmd_size);
}

void print_struct_sizes()
{
	print_control("VIDIOC_QUERYCAP", VIDIOC_QUERYCAP, sizeof(struct v4l2_capability));
	//print_control("VIDIOC_RESERVED", VIDIOC_RESERVED, sizeof( 1));
	print_control("VIDIOC_ENUM_FMT", VIDIOC_ENUM_FMT, sizeof(struct v4l2_fmtdesc));
	print_control("VIDIOC_G_FMT", VIDIOC_G_FMT, sizeof(struct v4l2_format));
	print_control("VIDIOC_S_FMT", VIDIOC_S_FMT, sizeof(struct v4l2_format));
#if 0
	print_control("VIDIOC_G_COMP", VIDIOC_G_COMP, sizeof(struct v4l2_compression));
	print_control("VIDIOC_S_COMP", VIDIOC_S_COMP, sizeof(struct v4l2_compression));
#endif
	print_control("VIDIOC_REQBUFS", VIDIOC_REQBUFS, sizeof(struct v4l2_requestbuffers));
	print_control("VIDIOC_QUERYBUF", VIDIOC_QUERYBUF, sizeof(struct v4l2_buffer));
	print_control("VIDIOC_G_FBUF", VIDIOC_G_FBUF, sizeof(struct v4l2_framebuffer));
	print_control("VIDIOC_S_FBUF", VIDIOC_S_FBUF, sizeof(struct v4l2_framebuffer));
	print_control("VIDIOC_OVERLAY", VIDIOC_OVERLAY, sizeof(int));
	print_control("VIDIOC_QBUF", VIDIOC_QBUF, sizeof(struct v4l2_buffer));
	print_control("VIDIOC_DQBUF", VIDIOC_DQBUF, sizeof(struct v4l2_buffer));
	print_control("VIDIOC_STREAMON", VIDIOC_STREAMON, sizeof(int));
	print_control("VIDIOC_STREAMOFF", VIDIOC_STREAMOFF, sizeof(int));
	print_control("VIDIOC_G_PARM", VIDIOC_G_PARM, sizeof(struct v4l2_streamparm));
	print_control("VIDIOC_S_PARM", VIDIOC_S_PARM, sizeof(struct v4l2_streamparm));
	print_control("VIDIOC_G_STD", VIDIOC_G_STD, sizeof(v4l2_std_id));
	print_control("VIDIOC_S_STD", VIDIOC_S_STD, sizeof(v4l2_std_id));
	print_control("VIDIOC_ENUMSTD", VIDIOC_ENUMSTD, sizeof(struct v4l2_standard));
	print_control("VIDIOC_ENUMINPUT", VIDIOC_ENUMINPUT, sizeof(struct v4l2_input));
	print_control("VIDIOC_G_CTRL", VIDIOC_G_CTRL, sizeof(struct v4l2_control));
	print_control("VIDIOC_S_CTRL", VIDIOC_S_CTRL, sizeof(struct v4l2_control));
	print_control("VIDIOC_G_TUNER", VIDIOC_G_TUNER, sizeof(struct v4l2_tuner));
	print_control("VIDIOC_S_TUNER", VIDIOC_S_TUNER, sizeof(struct v4l2_tuner));
	print_control("VIDIOC_G_AUDIO", VIDIOC_G_AUDIO, sizeof(struct v4l2_audio));
	print_control("VIDIOC_S_AUDIO", VIDIOC_S_AUDIO, sizeof(struct v4l2_audio));
	print_control("VIDIOC_QUERYCTRL", VIDIOC_QUERYCTRL, sizeof(struct v4l2_queryctrl));
	print_control("VIDIOC_QUERYMENU", VIDIOC_QUERYMENU, sizeof(struct v4l2_querymenu));
	print_control("VIDIOC_G_INPUT", VIDIOC_G_INPUT, sizeof(int));
	print_control("VIDIOC_S_INPUT", VIDIOC_S_INPUT, sizeof(int));
	print_control("VIDIOC_G_OUTPUT", VIDIOC_G_OUTPUT, sizeof(int));
	print_control("VIDIOC_S_OUTPUT", VIDIOC_S_OUTPUT, sizeof(int));
	print_control("VIDIOC_ENUMOUTPUT", VIDIOC_ENUMOUTPUT, sizeof(struct v4l2_output));
	print_control("VIDIOC_G_AUDOUT", VIDIOC_G_AUDOUT, sizeof(struct v4l2_audioout));
	print_control("VIDIOC_S_AUDOUT", VIDIOC_S_AUDOUT, sizeof(struct v4l2_audioout));
	print_control("VIDIOC_G_MODULATOR", VIDIOC_G_MODULATOR, sizeof(struct v4l2_modulator));
	print_control("VIDIOC_S_MODULATOR", VIDIOC_S_MODULATOR, sizeof(struct v4l2_modulator));
	print_control("VIDIOC_G_FREQUENCY", VIDIOC_G_FREQUENCY, sizeof(struct v4l2_frequency));
	print_control("VIDIOC_S_FREQUENCY", VIDIOC_S_FREQUENCY, sizeof(struct v4l2_frequency));
	print_control("VIDIOC_CROPCAP", VIDIOC_CROPCAP, sizeof(struct v4l2_cropcap));
	print_control("VIDIOC_G_CROP", VIDIOC_G_CROP, sizeof(struct v4l2_crop));
	print_control("VIDIOC_S_CROP", VIDIOC_S_CROP, sizeof(struct v4l2_crop));
	print_control("VIDIOC_G_JPEGCOMP", VIDIOC_G_JPEGCOMP, sizeof(struct v4l2_jpegcompression));
	print_control("VIDIOC_S_JPEGCOMP", VIDIOC_S_JPEGCOMP, sizeof(struct v4l2_jpegcompression));
	print_control("VIDIOC_QUERYSTD", VIDIOC_QUERYSTD, sizeof(v4l2_std_id));
	print_control("VIDIOC_TRY_FMT", VIDIOC_TRY_FMT, sizeof(struct v4l2_format));
}
/* --------------------------------------------------------------------- */
/* main								  	 */

int main(int argc, char *argv[])
{
	//char dummy[256];
	char *device = "/dev/video0";
	int tab = 1; //, ok = 0;
	int fd;
	struct v4l2_capability capability;
	v4l2_std_id std_id;
#if defined(MORE)
	struct v4l2_queryctrl qctrl;
	struct v4l2_querymenu qmenu;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_format format;
	struct v4l2_requestbuffers reqbuf;

	struct {
		void *start;
		size_t length;
	} *buffers;
	int i, j, e;

#endif

	if (argc > 1)
		device = argv[1];
	
	fd = open(device,O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr,"open %s: %s\n",device,strerror(errno));
		exit(1);
	};

	printf("general info\n");
	memset(&capability,0,sizeof(capability));
	if (-1 == ioctl(fd,VIDIOC_QUERYCAP,&capability))
		exit(1);
	printf("    VIDIOC_QUERYCAP\n");
	print_struct(stdout,desc_v4l2_capability,&capability,"",tab);
	printf("\n");

	if (-1 == ioctl(fd,VIDIOC_G_STD,&std_id))
		exit(1);
	printf("    VIDIOC_G_STD\n");
	print_struct(stdout,desc_v4l2_std_id,&std_id,"",tab);
	printf("\n");

#if defined(MORE)
	if (capability.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
		printf("video capture\n");
		memset(&fmtdesc,0,sizeof(fmtdesc));
		fmtdesc.index = 0;
		fmtdesc.type  = 0;
		if (-1 == ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc))
			/**/;
		printf("    VIDIOC_ENUM_FMT(%d,VIDEO_CAPTURE)\n",0);
		print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		for (i = 0;; i++) {
			memset(&fmtdesc,0,sizeof(fmtdesc));
			fmtdesc.index = i;
			fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (-1 == ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc))
				break;
			printf("    VIDIOC_ENUM_FMT(%d,VIDEO_CAPTURE)\n",i);
			print_struct(stdout,desc_v4l2_fmtdesc,&fmtdesc,"",tab);
		}
		memset(&format,0,sizeof(format));
		format.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == ioctl(fd,VIDIOC_G_FMT,&format)) {
			perror("VIDIOC_G_FMT(VIDEO_CAPTURE)");
		} else {
			printf("    VIDIOC_G_FMT(VIDEO_CAPTURE)\n");
			print_struct(stdout,desc_v4l2_format,&format,"",tab);
		}
		printf("\n");
	}


	for (i = 0; i < VIDIOC_QUERYCTRL_MAX; i++) {
		memset(&qctrl,0,sizeof(qctrl));
		qctrl.id = V4L2_CID_PRIVATE_BASE+i;
		if (-1 == ioctl(fd,VIDIOC_QUERYCTRL,&qctrl))
			break;
		if (qctrl.flags & V4L2_CTRL_FLAG_DISABLED)
			continue;
		printf("    VIDIOC_QUERYCTRL(PRIVATE_BASE+%d)\n",i);
		print_struct(stdout,desc_v4l2_queryctrl,&qctrl,"",tab);
		
		if (qctrl.type == V4L2_CTRL_TYPE_MENU) {
			for (j = qctrl.minimum-1; j <= qctrl.maximum+1; j++) {
				memset(&qmenu,0,sizeof(qmenu));
				qmenu.id = qctrl.id;
				qmenu.index = j;
				e  = ioctl(fd,VIDIOC_QUERYMENU,&qmenu);
				printf("      VIDIOC_QUERYMENU(PRIVATE_BASE+%d, %d)\n",i,j);
				print_struct(stdout,desc_v4l2_querymenu,&qmenu,"",tab+1);
				if (-1 == e) {
					printf("***   VIDIOC_QUERYMENU failed (%d)\n", errno);
				}
			}
		}
	}

	printf("streaming is%s available\n", (capability.capabilities & V4L2_CAP_STREAMING) ? "" : " NOT");

	if (capability.capabilities & V4L2_CAP_STREAMING) {
		/* setup buffers */
		memset (&reqbuf, 0, sizeof (reqbuf));
		reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		reqbuf.memory = V4L2_MEMORY_MMAP;
		reqbuf.count = 20;
		if (-1 == ioctl(fd, VIDIOC_REQBUFS, &reqbuf, 0)) {
			printf("***   VIDIOC_REQBUFS failed (%d)\n", errno);
			exit (1);
		}
		printf("buffers available = %d\n", reqbuf.count);

		if (reqbuf.count < 2) {
			/* You may need to free the buffers here. */
			printf ("Not enough buffer memory\n");
			exit (1);
		}

		buffers = calloc (reqbuf.count, sizeof (*buffers));
		assert (buffers != NULL);

		for (i = 0; i < reqbuf.count; i++) {
			struct v4l2_buffer buffer;

			memset (&buffer, 0, sizeof (buffer));
			buffer.type = reqbuf.type;
			buffer.memory = V4L2_MEMORY_MMAP;
			buffer.index = i;

			if (-1 == ioctl (fd, VIDIOC_QUERYBUF, &buffer)) {
				perror ("VIDIOC_QUERYBUF");
				exit (1);
			}

			buffers[i].length = buffer.length; /* remember for munmap() */

			buffers[i].start = mmap (NULL, buffer.length,
					 PROT_READ | PROT_WRITE, /* required */
					 MAP_SHARED,	     /* recommended */
					 fd, buffer.m.offset);

			if (buffers[i].start == MAP_FAILED) {
				/* You may need to unmap and free the so far
				   mapped buffers here. */
				perror ("mmap");
				exit (1);
			}
		}

		for (i = 0; i < reqbuf.count; i++)
			munmap (buffers[i].start, buffers[i].length);
	}
#endif

	close(fd);

	print_struct_sizes();

	return 0;
}

