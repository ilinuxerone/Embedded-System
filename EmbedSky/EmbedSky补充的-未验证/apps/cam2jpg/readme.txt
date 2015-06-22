本拍照程序适用于天嵌科技提供的OV9650和AV模块。


当使用Linux-2.6.13时，不管是使用OV9650还是AV模块，直接编译即可（命令使用：make），然后生成名为cam2jpg的可执行文件，将其复制到开发板的文件系统的/sbin目录下，然后挂载驱动，执行程序，即可在更目录下生成test00.jpg和test01.jpg两个图片。
对于OV9650使用如下命令挂载：
“insmod /lib/s3c2440_camif.ko；insmod /lib/TQ2440_OV9650.ko”，然后在执行cam2jpg即可。
对于AV模块使用如下命令挂载：
“insmod /lib/s3c2440_av.ko；insmod /lib//lib/av_input.ko”，然后在执行cam2jpg.

当使用Linux-2.6.30.4时，请修改Makefile文件中的编译器的版本为4.3.3的（如果您用的是4.3.3的编译器），同时修改源码main.c文件的31行，将“/dev/v4l/video0”改为“/dev/camera”，然后重新编译即可获取拍照程序。


