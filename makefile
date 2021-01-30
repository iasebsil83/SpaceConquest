#executables
SpaceConquest: SpaceConquest.o libisentlib.a
	gcc -Wall -o run SpaceConquest.o libisentlib.a -lm -lglut -lGL -lX11
	rm -f *.o *.a



#libraries
libisentlib.a: ESLib.o GfxLib.o
	ar r libisentlib.a ESLib.o GfxLib.o
	ranlib libisentlib.a

ESLib.o: ESLib.c ESLib.h ErreurLib.h
	gcc -Wall -c ESLib.c -O2

GfxLib.o: GfxLib.c GfxLib.h ESLib.h
	gcc -Wall -c GfxLib.c -I/usr/include/GL -O2
