CC=arm-linux-gcc
CROSS=arm-linux-

all: usb_camera

usb_camera:usb_camera.c
	$(CROSS)gcc -o usb_camera usb_camera.c -ljpeg -lpthread
	$(CROSS)strip usb_camera
clean:
	@rm -vf usb_camera *.o *~
