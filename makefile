#executable
run: SpaceConquest.o S2DE.o
	gcc -Wall -o run SpaceConquest.o S2DE.o -lm -lglut -lGL -lX11 -O2
	rm -f *.o



#game
SpaceConquest.o: SpaceConquest.c src/S2DE.h src/img.c
	gcc -Wall -c SpaceConquest.c -O2 -I/usr/include/GL



#S2DE
S2DE.o: src/S2DE.c src/S2DE.h
	gcc -Wall -c src/S2DE.c -O2 -I/usr/include/GL
