#executables
SpaceConquest: SpaceConquest.o libisentlib.a
	gcc -Wall -o run SpaceConquest.o libisentlib.a -lm -lglut -lGL -lX11
	rm -f *.o *.a



#game
SpaceConquest.o: SpaceConquest.c src/GfxLib.h src/ESLib.h
	gcc -Wall -c SpaceConquest.c -O2



#libraries
libisentlib.a: ESLib.o GfxLib.o
	ar r libisentlib.a ESLib.o GfxLib.o
	ranlib libisentlib.a

ESLib.o: src/ESLib.c src/ESLib.h src/ErreurLib.h
	gcc -Wall -c src/ESLib.c -O2

GfxLib.o: src/GfxLib.c src/GfxLib.h src/ESLib.h
	gcc -Wall -c src/GfxLib.c -I/usr/include/GL -O2
