#executable
run: SpaceConquest.o libisentlib.a
	gcc -Wall -o run SpaceConquest.o libisentlib.a -lm -lglut -lGL -lX11
	rm -f *.o *.a



#game
SpaceConquest.o: src/SpaceConquest.c lib/GfxLib.h lib/ESLib.h
	gcc -Wall -c src/SpaceConquest.c -O2



#libraries
libisentlib.a: ESLib.o GfxLib.o
	ar r libisentlib.a ESLib.o GfxLib.o
	ranlib libisentlib.a

ESLib.o: lib/ESLib.c lib/ESLib.h lib/ErreurLib.h
	gcc -Wall -c lib/ESLib.c -O2

GfxLib.o: lib/GfxLib.c lib/GfxLib.h lib/ESLib.h
	gcc -Wall -c lib/GfxLib.c -I/usr/include/GL -O2
