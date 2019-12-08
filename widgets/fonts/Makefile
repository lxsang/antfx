all: fontconvert

CC     = gcc
CFLAGS = -Wall -I/usr/local/Cellar/freetype/2.8/include/freetype2/
#-I/usr/local/include/freetype2 -I/usr/include/freetype2 -I/usr/include
LIBS   = -lfreetype

fontconvert: fontconvert.c
	$(CC) $(CFLAGS) $< $(LIBS) -o ../../build/$@
	strip ../../build/$@

clean:
	rm -f fontconvert
