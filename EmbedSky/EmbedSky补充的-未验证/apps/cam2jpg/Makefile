#如果是2.6.13内核使用，就去掉第二行的"#"，然后在第三行添加"#";当前的编译器版本是4.3.3。
#CC=/opt/EmbedSky/crosstools_3.4.5_softfloat/gcc-3.4.5-glibc-2.3.6/arm-linux/bin/arm-linux-gcc
CC=/opt/EmbedSky/4.3.3/bin/arm-linux-gcc

cam2jpg:main.o spcav4l.o utils.o picture.o pargpio.o encoder.o huffman.o marker.o quant.o avilib.o
	$(CC) *.o $(INCLUDE) -o cam2jpg -O2 -Wall 

main.o:	main.c
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<
	
spcav4l.o:	spcav4l.c spcav4l.h
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<
		
utils.o:	utils.c utils.h
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<
		
picture.o:	picture.c picture.h
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<
		
pargpio.o:	pargpio.c pargpio.h
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<
		
encoder.o:	encoder.c encoder.h
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<
		
huffman.o:	huffman.c huffman.h
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<
		
marker.o:	marker.c marker.h
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<
		
quant.o:	quant.c quant.h
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<

avilib.o: 	avilib.c avilib.h
		$(CC) $(INCLUDE) -O2 -Wall -c -o $@ $<

clean:
	rm -f *.o cam2jpg

