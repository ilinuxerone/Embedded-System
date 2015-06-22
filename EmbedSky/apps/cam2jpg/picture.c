/***************************************************************************#
# jpegenc: library to encode a jpeg frame from various input palette.       #
# jpegenc works for embedded device without libjpeg                         #
#.                                                                          #
# 		Copyright (C) 2005 Michel Xhaard                            #
#                                                                           #
# This program is free software; you can redistribute it and/or modify      #
# it under the terms of the GNU General Public License as published by      #
# the Free Software Foundation; either version 2 of the License, or         #
# (at your option) any later version.                                       #
#                                                                           #
# This program is distributed in the hope that it will be useful,           #
# but WITHOUT ANY WARRANTY; without even the implied warranty of            #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             #
# GNU General Public License for more details.                              #
#                                                                           #
# You should have received a copy of the GNU General Public License         #
# along with this program; if not, write to the Free Software               #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA #
#  CREDIT:								    #
# Original code from Nitin Gupta India (?)					    #
#                                                                           #
#***************************************************************************/

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/types.h>
#include <string.h>
#include <fcntl.h>
#include <wait.h>
#include <time.h>
#include <limits.h>
#include <linux/videodev.h>

#include "jdatatype.h"
#include "encoder.h"
#include "jconfig.h"
#include "picture.h"

/* helping  buffer pictures */
static unsigned char *outpict = NULL;
static unsigned char *inpict = NULL;

static int debug = 0;
#define DBG(format, args...) \
	do { \
		if (debug >= 1) \
			fprintf(stderr,format, ##args); \
	} while (0)

void
getJpegPicture (unsigned char *src, int w, int h, int format, int size,int mode, avi_t *fd, int num)
{
	static char filename[32],no[5];
	__u32 *lpix;
	__u16 *pix;
	__u8 *dest;
	int i;
	int sizein = 0;
	int sizeout = 0;
	FILE *foutpict=0;

	if (format == VIDEO_PALETTE_RAW_JPEG)
	return;
	memset (filename, 0, sizeof (filename));
	/* allocate the outpict buffer make room for rgb24 */
	sizein = size;
	sizeout = w * h * 3;
	outpict = (unsigned char *) realloc (outpict, sizeout);
	inpict = (unsigned char *) realloc (inpict, sizeout);

	dest = (__u8 *) outpict;
	if (mode == PICTURE)
	{
		sprintf(no,"%02d",num);
		strcpy(filename,"test");
		strcat(filename, no);
		strcat(filename,".jpg");
	}
	if(mode == PICTWRD)
  		snprintf(filename,13,"%s","new.jpg");
	if (mode == PICTURE || mode == PICTWRD)
	{
  		foutpict = fopen (filename, "wb");
  	}
	if (format == VIDEO_PALETTE_JPEG)
	{
		sizeout = get_jpegsize( src,sizein);
		if (mode == PICTURE || mode == PICTWRD)
		{
			DBG (" picture jpeg %s\n", filename);
			fwrite (src, sizeof (char), sizeout, foutpict);
			fclose (foutpict);
		}
		else
		{
			if(AVI_write_frame ( fd,(unsigned char *) src,sizeout) < 0)
			DBG ("GetjpegPicture write error on avi out \n");
      		}
	}
	else
	{
		switch (format)
		{
			case VIDEO_PALETTE_RGB565:
	  			pix = (__u16 *) src;
	  			for (i = 0; i < (sizeout - 3); i += 3)
				{
					inpict[i + 2] = (*pix & 0xF800) >> 8;
					inpict[i + 1] = (*pix & 0x07E0) >> 3;
					inpict[i] = (*pix & 0x001F) << 3;
					pix++;
				}
	 			sizeout = encode_image (inpict,dest,1024,RGBto420,w,h);
	 			DBG (" picture rgb565 %s,sizeout=%d\n", filename,sizeout);
				break;
			case VIDEO_PALETTE_RGB24:
				memcpy(inpict,src,size);
	  			sizeout = encode_image (inpict,dest,1024,RGBto420,w,h);
	  			DBG (" picture rgb24 %s\n", filename);
				break;
			case VIDEO_PALETTE_RGB32:
	  			lpix = (__u32 *) src;
	  			for (i = 0; i < sizeout; i += 3)
				{
					inpict[i + 2] = (*lpix & 0x00FF0000) >> 16;
					inpict[i + 1] = (*lpix & 0x0000FF00) >> 8;
					inpict[i + 0] = (*lpix & 0x000000FF);
					lpix++;
				}
	  			sizeout = encode_image (inpict,dest,1024,RGBto420,w,h);
	  			DBG (" picture rgb32 %s\n", filename);
				break;
			case VIDEO_PALETTE_YUV420P:
	  			memcpy(inpict,src,size);
	  			sizeout = encode_image (inpict,dest,1024,YUVto420,w,h);
	  			DBG (" picture yuv420p %s\n", filename);
				break;
			default:
				break;
		}
		if (mode == PICTURE)
		{
			fwrite (outpict, sizeof (char), sizeout, foutpict);
			fclose (foutpict);
		}
		else
		{
      		if(AVI_write_frame ( fd,(unsigned char *) outpict,sizeout) < 0)
			DBG ("write error on avi out \n");
		}
	}
  	free (outpict);
  	outpict = NULL;
  	free (inpict);
  	inpict = NULL;
}

