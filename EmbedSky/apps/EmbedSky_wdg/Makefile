CROSS=arm-linux-

all: EmbedSky_wdg

EmbedSky_wdg:EmbedSky_wdg.c
	$(CROSS)gcc -o EmbedSky_wdg EmbedSky_wdg.c
	$(CROSS)strip EmbedSky_wdg
clean:
	@rm -vf EmbedSky_wdg *.o *~
