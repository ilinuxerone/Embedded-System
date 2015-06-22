/*************************************

NAME:usb_camera.c
COPYRIGHT:www.embedsky.net

*************************************/

#include <errno.h>
#include <sys/types.h>	
#include <sys/stat.h>	
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>    
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <string.h>
#include <malloc.h>
#include <linux/fb.h>
#include <jpeglib.h>
#include <jerror.h>

typedef struct VideoBuffer
{
	unsigned char *start;
	size_t offset;
	size_t length;
} VideoBuffer;

struct fb_dev
{
	//for frame buffer
	int fb;
	void *fb_mem;	//frame buffer mmap
	int fb_width, fb_height, fb_line_len, fb_size;
	int fb_bpp;
} fbdev;

//得到framebuffer的长、宽和位宽，成功则返回0，失败返回－1 
int fb_stat(int fd)
{
	struct fb_fix_screeninfo fb_finfo;
	struct fb_var_screeninfo fb_vinfo;

	if (ioctl(fd, FBIOGET_FSCREENINFO, &fb_finfo))
	{
		perror(__func__);
		return (-1);
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, &fb_vinfo))
	{
		perror(__func__);
		return (-1);
	}

	fbdev.fb_width = fb_vinfo.xres;
	fbdev.fb_height = fb_vinfo.yres;
	fbdev.fb_bpp = fb_vinfo.bits_per_pixel;
	fbdev.fb_line_len = fb_finfo.line_length;
	fbdev.fb_size = fb_finfo.smem_len;

	return (0);
}

//转换RGB888为RGB565（因为当前LCD是采用的RGB565显示的）
unsigned short RGB888toRGB565(unsigned char red, unsigned char green, unsigned char blue)
{
	unsigned short B = (blue >> 3) & 0x001F;
	unsigned short G = ((green >> 2) << 5) & 0x07E0;
	unsigned short R = ((red >> 3) << 11) & 0xF800;

	return (unsigned short) (R | G | B);
}
//释放framebuffer的映射
int fb_munmap(void *start, size_t length)
{
	return (munmap(start, length));
}

//显示一个像素点的图像到framebuffer上
int fb_pixel(void *fbmem, int width, int height, int x, int y, unsigned short color)
{
	if ((x > width) || (y > height))
		return (-1);

	unsigned short *dst = ((unsigned short *) fbmem + y * width + x);

	*dst = color;
	return 0;
}



int main(int argc, char** argv)
{
	int numBufs;
	printf("USB Camera Test\n");

	int fd = open("/dev/video0", O_RDWR, 0);			//打开摄像头设备，使用阻塞方式打开
	if (fd<0)
	{
		printf("open error\n");
		return  -1;
	}

	struct v4l2_format fmt;						//设置获取视频的格式
	memset( &fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;				//视频数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV565;			//视频源的格式为JPEG或YUN4:2:2或RGB
	fmt.fmt.pix.width = 320;					//设置视频宽度
	fmt.fmt.pix.height = 240;					//设置视频高度
	if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0)				//使配置生效
	{
		printf("set format failed\n");
		return -1;
	}

	struct v4l2_requestbuffers req;					//申请帧缓冲
	memset(&req, 0, sizeof (req));
	req.count = 1;							//缓存数量，即可保存的图片数量
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;				//数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
	req.memory = V4L2_MEMORY_MMAP;					//存储类型：V4L2_MEMORY_MMAP或V4L2_MEMORY_USERPTR
	if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1)			//使配置生效
	{
		perror("request buffer error \n");
		return -1;
	}

	VideoBuffer *buffers = calloc(req.count, sizeof(VideoBuffer));	//将VIDIOC_REQBUFS获取内存转为物理空间
//	printf("sizeof(VideoBuffer) is %d\n", sizeof(VideoBuffer));
	struct v4l2_buffer buf;
	for (numBufs = 0; numBufs < req.count; numBufs++)
	{
		memset( &buf, 0, sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;			//数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
		buf.memory = V4L2_MEMORY_MMAP;				//存储类型：V4L2_MEMORY_MMAP（内存映射）或V4L2_MEMORY_USERPTR（用户指针）
		buf.index = numBufs;
		if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0)		//使配置生效
		{
			printf("VIDIOC_QUERYBUF error\n");
			return -1;
		}
//		printf("buf len is %d\n", sizeof(buf));
		buffers[numBufs].length = buf.length;
		buffers[numBufs].offset = (size_t) buf.m.offset;
		buffers[numBufs].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
			MAP_SHARED, fd, buf.m.offset);			//使用mmap函数将申请的缓存地址转换应用程序的绝对地址
#if 0
		printf("buffers.length = %d,buffers.offset = %d ,buffers.start[0] = %d\n",
				buffers[numBufs].length, buffers[numBufs].offset,
				buffers[numBufs].start[0]);

		printf("buf2 len is %d\n", sizeof(buffers[numBufs].start));
#endif
		if (buffers[numBufs].start == MAP_FAILED)
		{
			perror("buffers error\n");
			return -1;
		}
		if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)			//放入缓存队列
		{
			printf("VIDIOC_QBUF error\n");
			return -1;
		}

	}

	enum v4l2_buf_type type;					//开始视频显示
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;				//数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
	if (ioctl(fd, VIDIOC_STREAMON, &type) < 0)
	{
		printf("VIDIOC_STREAMON error\n");
		return -1;
	}

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;				//数据流类型，永远都是V4L2_BUF_TYPE_VIDEO_CAPTURE
	if (ioctl(fd, VIDIOC_G_FMT, &fmt) < 0)				//读取视频源格式
	{
		printf("get format failed\n");
		return -1;
	}
	else
	{
		printf("Picture:Width = %d   Height = %d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
//		printf("Image size = %d\n", fmt.fmt.pix.sizeimage);
//		printf("pixelformat = %d\n", fmt.fmt.pix.pixelformat);
	}

	FILE * fd_y_file = 0;
	int a=0;
	int k = 0;

	//设置显卡设备framebuffer
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *infile;							//Jpeg文件的句柄
	unsigned char *buffer;

	int fb;
	char *fb_device;
	unsigned int x;
	unsigned int y;

	if ((fb = open("/dev/fb0", O_RDWR)) < 0)			//打开显卡设备
	{
		perror(__func__);
		return (-1);
	}

	//获取framebuffer的状态
	fb_stat(fb);							//获取显卡驱动中的长、宽和显示位宽
	printf("frame buffer: %dx%d,  %dbpp, 0x%xbyte= %d\n", 
		fbdev.fb_width, fbdev.fb_height, fbdev.fb_bpp, fbdev.fb_size, fbdev.fb_size);

	//映射framebuffer的地址
	fbdev.fb_mem = mmap (NULL, fbdev.fb_size, PROT_READ|PROT_WRITE,MAP_SHARED,fb,0);
									//映射显存地址
	fbdev.fb = fb;

	//预览采集到的图像
	while (1)
	{
		//如果把处理JPEG格式的数据和显示程序分离，把处理JPEG部分的数据作成一个新的线程，预览时会更加流畅。
		for (numBufs = 0; numBufs < req.count; numBufs++)
		{
			char s[15];
			sprintf(s, "%d.jpg", a);
			if ((fd_y_file = fopen(s, "wb")) < 0)
			{
				printf("Unable to create y frame recording file\n");
				return -1;
			}

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;		//取得原始采集数据
			buf.memory = V4L2_MEMORY_MMAP;			//存储类型：V4L2_MEMORY_MMAP（内存映射）或V4L2_MEMORY_USERPTR（用户指针）
			if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0)
			{
				perror("VIDIOC_DQBUF failed.\n");
				return -1;
			}

			unsigned char *ptcur = buffers[numBufs].start;		//开始霍夫曼解码
			int i1;
			for (i1=0; i1<buf.bytesused; i1++)
			{
				if ((buffers[numBufs].start[i1] == 0x000000FF)
						&& (buffers[numBufs].start[i1+1] == 0x000000C4))
				{
					break;
				}
			}
			if (i1 == buf.bytesused)
				printf("huffman table don't exist! \n");
			int i;
			for (i=0; i<buf.bytesused; i++)
			{
				if ((buffers[numBufs].start[i] == 0x000000FF)
						&& (buffers[numBufs].start[i+1] == 0x000000D8))
					break;
				ptcur++;
			}
			int imagesize = buf.bytesused - i;

			fwrite(ptcur, imagesize, 1, fd_y_file);			//开始向LCD发送数据显示采集到的图像
			fclose(fd_y_file);

			if ((infile = fopen(s, "rb")) == NULL)
			{
				fprintf(stderr, "open %s failed\n", s);
				exit(-1);
			}
			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_decompress(&cinfo);

			//导入要解压的Jpeg文件infile
			jpeg_stdio_src(&cinfo, infile);

			//读取jpeg文件的文件头
			jpeg_read_header(&cinfo, TRUE);

			//开始解压Jpeg文件，解压后将分配给scanline缓冲区，
			jpeg_start_decompress(&cinfo);

			buffer = (unsigned char *) malloc(cinfo.output_width
					* cinfo.output_components);
			y = 0;
			while (cinfo.output_scanline < cinfo.output_height)
			{
				jpeg_read_scanlines(&cinfo, &buffer, 1);
				if (fbdev.fb_bpp == 16)
				{
					unsigned short color;
					for (x = 0; x < cinfo.output_width; x++)
					{
						color = RGB888toRGB565(buffer[x * 3],
								buffer[x * 3 + 1], buffer[x * 3 + 2]);
						fb_pixel(fbdev.fb_mem, fbdev.fb_width, fbdev.fb_height, x, y, color);
					}
				}
				else if (fbdev.fb_bpp == 24)
				{
					memcpy((unsigned char *)fbdev.fb_mem + y * fbdev.fb_width * 3, buffer,
							cinfo.output_width * cinfo.output_components);
				}
				y++;						//下一个scanline
			}

			//完成Jpeg解码，释放Jpeg文件
			jpeg_finish_decompress(&cinfo);
			jpeg_destroy_decompress(&cinfo);

			//释放帧缓冲区
			free(buffer);

			//关闭Jpeg输入文件
			fclose(infile);

			//获取下一帧视频数据
			if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
			{
				printf("VIDIOC_QBUF error\n");
				return -1;
			}
		}
	}
	fb_munmap(fbdev.fb_mem, fbdev.fb_size);					//释放framebuffer映射
	close(fb);								//关闭Framebuffer设备

	close(fd);
}

