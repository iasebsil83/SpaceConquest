SpaceConquest: SpaceConquest.o libisentlib.a
	gcc -Wall SpaceConquest.o -o SpaceConquest libisentlib.a -lm -lglut -lGL -lX11

libisentlib.a: ESLib.o GfxLib.o
	ar r libisentlib.a ESLib.o GfxLib.o
	ranlib libisentlib.a

ESLib.o: ESLib.c ESLib.h ErreurLib.h
	gcc -Wall -O2 -c ESLib.c

GfxLib.o: GfxLib.c GfxLib.h ESLib.h
	gcc -Wall -O2 -c GfxLib.c -I/usr/include/GL

clean:
	rm -f *~ *.o

deepclean: clean
	rm -f SpaceConquest libisentlib.a

