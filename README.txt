-------------------- Space Conquest --------------------

Space conquest is a simple 2D game made in C using the
Ghislain OUDINET's GfxLib.

It is a sandbox-like game. You begin your adventure on a
planet and try to catch the more ressources you can.

Then you can craft some more sofisticated stuff such as
space suits or rockets !

Explore your galaxy (generated randomly) to discover new
planets.



Controls :

On planet :
 - Q/D                   : go left/right
 - Right click anywhere  : open/close menu option
 - Left click on planet  : dig into the planet ground
 - Right click on hotbar : go in drop mode
 - 1/2/3/4/5             : select hotbar slot (can be selected with left click too)

In drop mode :
 - Left click  : grab/drop item in the current slot
 - Right click : go back in planet mode

In inventory :
 - Drag and drop       : move items from a slot to another
 - Right click on item : delete current item
 - Q                   : go back in planet mode

In space :
 - Left click : go on selected planet
 - Q          : go back in planet mode

Note : To equip something,
       put an equipment in the current slot,
       go in menu option -> equipment,
       and then click on the desired slot.



It was made for a challenge of ISEN Engineering at the
end of 2018.

All it requires to run is the glut library (freeglut3-dev
on linux, should normally work on MacOS systems...)

Anyway, further informations such as versions are
available in src/SpaceConquest.c.

This is a tiny project, the game time is quite limited,
it was my first big C project.
Pay attention, there is no save system!
