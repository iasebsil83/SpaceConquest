// ================ IMPORTATIONS ================

//standard
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>



//random
#include <time.h>



//graphics
#include "src/S2DE.h"
#define S2DE_littleText(t,s,x,y) S2DE_text(t,(s)/120.f,x,y)



//images
#include "img.c"




















/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Space Conquest ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        Adapted from an older version with S2DE :
    Project developped for ISEN Toulon, an Yncrea Mediterranee engineering school

    This project was made for an ISEN Engineering challenge at the end of 2018.

    24/01/2019 > [0.1.0]_Beta3 :
        DONE_LAST_TIME{
            - Added furniture item (Menu, Display, Keypad, MouseClick, Sprites)
            - Finished version beta 3
        }
        TO_DO{
            //- Do settings menu (Display, MouseClick, Keypad)
            //- Make and apply planet sprites (Sprites, Display)
        }
        BUGS{
            //- Rotation of the galaxy is not correct (setGCoo)
        }

    Contact : i.a.sebsil83@gmail.com

    ISEN Toulon : www.isen.fr
******************************************************************************************

    LICENCE :

    SpaceConquest_S2DE
    Copyright (C) 2017  Sebastien SILVANO
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.

    If not, see <https://www.gnu.org/licenses/>.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/




















// ================= TYPE DEFINITIONS =====================

//objects (inventory)
typedef struct obj{
	unsigned short int id;
	int data;
}obj;



//space
typedef struct planet{
	int x,y;
	obj objects[16]; //16 objects for the maximum size
	short int size, type;
	//size : { 0:6_divisions, 1:8_divisions, 2:12_divisions, 3:16_divisions                          }
	//type : { 0:grasss,      1:rock,        2:red_dirt,     3:sand,        4:lava, 5:ice, 6:diamond }
	bool atm;
}planet;

typedef struct star{
	int x,y,r;
}star;

typedef struct galaxy{
	short int nbrPlanets, nbrStars;
	planet planets[10];
	star sun, stars[16];
}galaxy;




















// ==================== DECLARATIONS =======================

//functionnal vars
const int maxl = 1600;
const int maxh = 900;
static bool starter = true;
static short int starterCnt = 0;
const float M_2PI = 2*M_PI;
const float M_PI2 = M_PI/2;
const float M_3PI2 = 3*M_PI/2;
const float M_PI4 = M_PI/4;
const float M_3PI4 = 3*M_PI/4;
const float M_5PI4 = 5*M_PI/4;
const float M_7PI4 = 7*M_PI/4;

//sprites vars
static img voidImg;
static img yncrea;
static img ISEN;



//fire
static img fire;



//player
static img playerR_stop;
static img playerR_run1;
static img playerR_run2;
static img playerL_stop;
static img playerL_run1;
static img playerL_run2;



//player with space suit
static img playerSuitR_stop;
static img playerSuitR_run1;
static img playerSuitR_run2;
static img playerSuitL_stop;
static img playerSuitL_run1;
static img playerSuitL_run2;



//icons
static img icon_inv;   static img icon_crafts;
static img icon_equip; static img icon_space;



//Items
static img item_tree;                    static img item_wood;
static img item_fiber;                   static img item_space_suit;
static img item_multi_tool;              static img item_pickaxe;
static img item_rock;                    static img item_iron;
static img item_screwdriver;             static img item_hammer;
static img item_oxygen_tank;             static img item_machine_base;
static img item_fuel_maker;              static img item_fuel;
static img item_rocket;                  static img item_rocket_high;
static img item_fruit;                   static img item_oxygen_generator;
static img item_oxygen_generator_tanked; static img item_furniture;

//game vars
static short int menu = 0; //0:start_menu, 1:start_settings, 2:planet, 3:inventory, 4:space
static short int focus = 0;
// ------- focuses for each menu -------
// menu = 0 : start_menu {}
// menu = 1 : start_settings {}
// menu = 2 : planet {
//    0:player_moving,          1:put_down_item,        2:options_panel1,
//    3:options_panel2[crafts], 4:options_panel2[equip]
//}
// menu = 3 : inventory        { 0:no_selection,  1:item_selected, 2:message_delete }
// menu = 4 : space            { 0:galaxy_view,   1:error_message                   }
// menu = 5 : rocket_animation { 0:player_moving, 1:fire,          2:take_off       }
// menu = 6 : furnitures       { 0:no_selection,  1:item_selected, 2:message_delete }
static galaxy g;



//planet
static short int planetRow = 0;
static float plRadius;
//player
static float pAngle = 0;
const float pAngleStep = 0.15;
static float cosA,sinA;
static bool pRight = true;
static short int pRunningCnt = 0;
const short int pRunningCntMax = 8; //0.33 second (time reference : real)
static short int pRunning = 0; //0:stop, 1:run1, 2:run2
//coordinates
static int pCoo[2]; //player
static int p1Coo[2]; //options_panels
static int p2Coo[2];
static int tCoo[16][2][2]; //planets divisions (triangles)
static int oCoo[16][2]; //objects
static bool tStates[16]; //if mouse is in triangle
static short int hitDiv; //division of the hit ground
//objects
char objectsName[19][20] = {
	"",             "Tree",
	"Wood",         "Fiber",
	"Space suit",   "Multi tool",
	"Pickaxe",      "Rock",
	"Iron",         "Screwdriver",
	"Hammer",       "Oxygen tank",
	"Machine base", "Fuel maker",
	"Fuel",         "Rocket",
	"Fruit",        "Oxygen generator",
	"Furniture"
};
// ------- objects references -------
//  0 : nothing       1 : tree
//  2 : wood          3 : fiber
//  4 : space suit    5 : multi tool
//  6 : pickaxe       7 : rock
//  8 : iron          9 : screwdriver
// 10 : hammer       11 : oxygen tank
// 12 : machine base 13 : fuel maker
// 14 : fuel         15 : rocket
// 16 : fruit        17 : oxygen generator (oxygen generator data = (oxygen tank charging) ? oxygen tank data + 1 : 0)
// 18 : furniture



//interface
static obj inventory[5][5]; //inventory[0] is hotbar
static short int hotbarSel = 0; //hotbar selection
static short int xSel = 0; //inventory selection
static short int ySel = 0;
static short int craftPage = 0;
const short int craftPageMax = 3;
static obj equipment[3]; //0:space suit, 1:oxygen tank1, 2:oxygen tank2
static obj stock[5][4][5]; //furnitures (5 maximum)
const short int stockMax = 5;
static short int stockSel = 0;
static short int craftedStocks = 0;
static bool fromInv = false;



//life
static int life = 30;
const int lifeMax = 30; //1 minute to die (time reference : gAngleCnt)



//oxygen
const int oxygenTankMax = 150; //5 minutes (time reference : gAngleCnt)
static int ownOxygen = 15;
const int ownOxygenMax = 15; //30 seconds (time reference : gAngleCnt)



//space visualisation
static float gAngle = 0;
const float gAngleStep = 0.0001;
static int gAngleCnt = 0;
const int gAngleCntMax = 80; //2 seconds (time reference : real)
static short int planetSel = -1; //no planet selected



//hit the ground
static unsigned short int hitCnt = 0;
const unsigned short int hitCntMax = 40; //1 second (time reference : real)



//tools
const unsigned short int toolsDurability = 60; //number of uses



//fuel maker
const short int fuelMakerMax = 5; //10 seconds (time reference : gAngleCnt)



//rocket
const short int rocketFuelMax = 8;
static int rocketAnimCnt = 0;
static float rocketAngle = 0;
static float rocketDistance = 0.f;
const float rocketDistanceStep = 10.f;



//S2DE external variables
extern int S2DE_keyState;
extern short S2DE_key;
extern int S2DE_mouseState;
extern int S2DE_mouseButton;
extern int S2DE_mouseX;
extern int S2DE_mouseY;




















// ===================== UTILITIES ========================

//Utilities
int rnd(int n){ //return random number between 0 and n equitably
	float v = rand() % n;
	if(v-(int)v >= 0.5)
		return (int)(v+1);
	else
		return (int)v;
}

bool inZone(float x,float y,float xmin,float ymin,float xmax,float ymax){
	return (x <= xmax && x >= xmin && y <= ymax && y >= ymin);
}

float norme(float x1,float y1, float x2,float y2){
	return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
}

float area(float x1,float y1, float x2,float y2, float x3,float y3){
	float n12 = norme(x1,y1,x2,y2);
	float n23 = norme(x2,y2,x3,y3);
	float n31 = norme(x3,y3,x1,y1);
	float p = ( n12+n23+n31 )/2; //semi-perimeter
	return sqrt( p*(p-n12)*(p-n23)*(p-n31) ); //Heron formula
}

bool inTriangle(float x,float y, float x1,float y1, float x2,float y2, float x3,float y3){
	float areaTot = area(x1,y1, x2,y2, x3,y3);
	float areas = area(x,y, x1,y1, x2,y2)
				+ area(x,y, x2,y2, x3,y3)
				+ area(x,y, x3,y3, x1,y1);
	return (areaTot-10 <= areas && areas <= areaTot+10);
}



//game functions
short int getDiv(){
	short int d = 4*g.planets[planetRow].size;
	if(d == 4) //only exception
		return 6;
	return d;
}

void setPlayerCoo(){
	cosA = cos(pAngle);
	sinA = sin(pAngle);
	pCoo[0] = maxl/2 + (plRadius+100)*sinA;
	pCoo[1] = maxh/2 + (plRadius+100)*cosA;
}

void setP1Coo(){
	p1Coo[0] = (pAngle > M_PI) ? -270 : 100;
	p1Coo[0] += pCoo[0];
	p1Coo[1] = pCoo[1] - 100;
}

void setP2Coo(){
	if(pAngle > M_PI)
		p2Coo[0] = p1Coo[0] - 350;
	else
		p2Coo[0] = p1Coo[0] + 180;
	p2Coo[1] = p1Coo[1] + 170;
	craftPage = 0;
}

void setTCoo(){ //put_down_item : set the two points out of the triangle for each of them
	short int div = getDiv(); //the last is the center of the window
	float da = M_2PI/div;
	float angle;
	for(short int i=0; i < div; i++){
		angle = i*M_2PI/div;
		tCoo[i][0][0] = 800 + plRadius*sin(angle);
		tCoo[i][0][1] = 450 + plRadius*cos(angle);
		tCoo[i][1][0] = 800 + plRadius*sin(angle+da);
		tCoo[i][1][1] = 450 + plRadius*cos(angle+da);
	}
}

void setOCoo(){
	short int div = getDiv();
	float angle;
	float half_da = M_PI/div;
	for(short int i=0; i < div; i++){
		angle = i*M_2PI/div + half_da;
		oCoo[i][0] = 800 + (plRadius+30)*sin(angle);
		oCoo[i][1] = 450 + (plRadius+30)*cos(angle);
	}
}

void setPlanet(){

	//set radius
	if(g.planets[planetRow].size == 1)
		plRadius = 45;
	else if(g.planets[planetRow].size == 2)
		plRadius = 95;
	else if(g.planets[planetRow].size == 3)
		plRadius = 200;
	else if(g.planets[planetRow].size == 4)
		plRadius = 258;
	setPlayerCoo();

	//set planet objects positions (thanks to tCoo & oCoo)
	setTCoo();
	setOCoo();
}

void setGCoo(){
	float cosA = cos(gAngle);
	float sinA = sin(gAngle);
	float tempx,tempy;
	for(short int p=0; p < g.nbrPlanets; p++){
		tempx = g.planets[p].x - 800; //centralize in (0,0)
		tempy = g.planets[p].y - 450;
		g.planets[p].x = 800 + tempx*cosA - tempy*sinA;
		g.planets[p].y = 450 + tempx*sinA + tempy*cosA;
	}
	for(short int s=0; s < g.nbrStars; s++){
		tempx = g.stars[s].x - 800;
		tempy = g.stars[s].y - 450;
		g.stars[s].x = 800 + tempx*cosA - tempy*sinA;
		g.stars[s].y = 450 + tempx*sinA + tempy*cosA;
	}
}

short int checkForObject(unsigned short int id){
	float div = getDiv();
	for(short int i=0; i < div; i++){
		if(g.planets[planetRow].objects[i].id == id)
			return i;
	}
	return -1;
}

short int checkForRocket(){
	float div = getDiv();
	for(short int i=0; i < div; i++){
		if(g.planets[planetRow].objects[i].id == 15){ //rocket
			if(g.planets[planetRow].objects[i].data != 0) //if have fuel
				return i;
		}
	}
	return -1;
}

int invCountItem(unsigned short i){
	unsigned short int cnt = 0;
	for(short int y=0; y < 5; y++){
		for(short int x=0; x < 5; x++){
			if(inventory[y][x].id == i)
				cnt++;
		}
	}
	return cnt;
}

void delInInv(unsigned short int i){
	for(short int y=0; y < 5; y++){
		for(short int x=0; x < 5; x++){
			if(inventory[y][x].id == i){
				inventory[y][x].id	 = 0; //nothing
				inventory[y][x].data = 0;
				return;
			}
		}
	}
}

void addInInv(unsigned short int id, int data){
	for(short int y=0; y < 5; y++){
		for(short int x=0; x < 5; x++){
			if(inventory[y][x].id == 0){ //nothing
				inventory[y][x].id	 = id;
				inventory[y][x].data = data;
				return;
			}
		}
	}
}

void tryToPutDown(unsigned short int n, unsigned short int total){
	if(n != 0){
		if(g.planets[planetRow].objects[(int)(n-1)].id != 13){ //if not at the left of a fuel maker
			g.planets[planetRow].objects[n].id	 = inventory[0][hotbarSel].id; //passing item from hotbar to planet
			g.planets[planetRow].objects[n].data = inventory[0][hotbarSel].data;
			inventory[0][hotbarSel].id	 = 0; //removing item from hotbar
			inventory[0][hotbarSel].data = 0;
		}
	}else{
		if(g.planets[planetRow].objects[(int)(total-1)].id != 13){ //if not at the left of a fuel maker
			g.planets[planetRow].objects[n].id	 = inventory[0][hotbarSel].id; //passing item from hotbar to planet
			g.planets[planetRow].objects[n].data = inventory[0][hotbarSel].data;
			inventory[0][hotbarSel].id	 = 0; //removing item from hotbar
			inventory[0][hotbarSel].data = 0;
		}
	}
}



//Extensible game functions
void setPlColor(planet p){
	switch(p.type){
		case 0: //grass
			S2DE_setColor(0,200,0);
		break;
		case 1: //rock
			S2DE_setColor(210,210,210);
		break;
		case 2: //red_dirt
			S2DE_setColor(255,50,0);
		break;
		case 3: //sand
			S2DE_setColor(255,155,0);
		break;
		case 4: //lava
			S2DE_setColor(255,0,0);
		break;
		case 5: //ice
			S2DE_setColor(100,180,180);
		break;
		case 6: //diamond
			S2DE_setColor(255,255,255);
		break;
	}
}

void setSprites(){
	voidImg.width = 0; voidImg.height = 0;
	voidImg.data = NULL;

	//yncrea - ISEN
	yncrea.width = 227;	yncrea.height = 250;
	yncrea.data = NULL;
	yncrea = readRGBA2ARVB("Sprites/Yncrea[227x250].rgba",yncrea);
	ISEN.width = 724; ISEN.height = 200;
	ISEN.data = NULL;
	ISEN = readRGBA2ARVB("Sprites/ISEN[724x200].rgba",ISEN);

	//fire
	fire.width = 60; fire.height = 60;
	fire.data = NULL;
	fire = readRGBA2ARVB("Sprites/fire[60x60].rgba",fire);

	//player

	//playerR
	playerR_stop.width = 100; playerR_stop.height = 200;
	playerR_stop.data = NULL;
	playerR_stop = readRGBA2ARVB("Sprites/playerR_stop[100x200].rgba",playerR_stop);
	playerR_run1.width = 100; playerR_run1.height = 200;
	playerR_run1.data = NULL;
	playerR_run1 = readRGBA2ARVB("Sprites/playerR_run1[100x200].rgba",playerR_run1);
	playerR_run2.width = 100; playerR_run2.height = 200;
	playerR_run2.data = NULL;
	playerR_run2 = readRGBA2ARVB("Sprites/playerR_run2[100x200].rgba",playerR_run2);

	//playerL
	playerL_stop.width = 100; playerL_stop.height = 200;
	playerL_stop.data = NULL;
	playerL_stop = readRGBA2ARVB("Sprites/playerL_stop[100x200].rgba",playerL_stop);
	playerL_run1.width = 100; playerL_run1.height = 200;
	playerL_run1.data = NULL;
	playerL_run1 = readRGBA2ARVB("Sprites/playerL_run1[100x200].rgba",playerL_run1);
	playerL_run2.width = 100; playerL_run2.height = 200;
	playerL_run2.data = NULL;
	playerL_run2 = readRGBA2ARVB("Sprites/playerL_run2[100x200].rgba",playerL_run2);

	//player with space suit

	//playerSuitR
	playerSuitR_stop.width = 100; playerSuitR_stop.height = 200;
	playerSuitR_stop.data = NULL;
	playerSuitR_stop = readRGBA2ARVB("Sprites/playerSuitR_stop[100x200].rgba",playerSuitR_stop);
	playerSuitR_run1.width = 100; playerSuitR_run1.height = 200;
	playerSuitR_run1.data = NULL;
	playerSuitR_run1 = readRGBA2ARVB("Sprites/playerSuitR_run1[100x200].rgba",playerSuitR_run1);
	playerSuitR_run2.width = 100; playerSuitR_run2.height = 200;
	playerSuitR_run2.data = NULL;
	playerSuitR_run2 = readRGBA2ARVB("Sprites/playerSuitR_run2[100x200].rgba",playerSuitR_run2);

	//playerSuitL
	playerSuitL_stop.width = 100; playerSuitL_stop.height = 200;
	playerSuitL_stop.data = NULL;
	playerSuitL_stop = readRGBA2ARVB("Sprites/playerSuitL_stop[100x200].rgba",playerSuitL_stop);
	playerSuitL_run1.width = 100; playerSuitL_run1.height = 200;
	playerSuitL_run1.data = NULL;
	playerSuitL_run1 = readRGBA2ARVB("Sprites/playerSuitL_run1[100x200].rgba",playerSuitL_run1);
	playerSuitL_run2.width = 100; playerSuitL_run2.height = 200;
	playerSuitL_run2.data = NULL;
	playerSuitL_run2 = readRGBA2ARVB("Sprites/playerSuitL_run2[100x200].rgba",playerSuitL_run2);

	//icons
	icon_inv.width = 70; icon_inv.height = 70;
	icon_inv.data = NULL;
	icon_inv = readRGBA2ARVB("Sprites/icon_inv[70x70].rgba",icon_inv);
	icon_crafts.width = 70; icon_crafts.height = 70;
	icon_crafts.data = NULL;
	icon_crafts = readRGBA2ARVB("Sprites/icon_crafts[70x70].rgba",icon_crafts);
	icon_equip.width = 70; icon_equip.height = 70;
	icon_equip.data = NULL;
	icon_equip = readRGBA2ARVB("Sprites/icon_equip[70x70].rgba",icon_equip);
	icon_space.width = 70; icon_space.height = 70;
	icon_space.data = NULL;
	icon_space = readRGBA2ARVB("Sprites/icon_space[70x70].rgba",icon_space);

	//items

	//tree
	item_tree.width = 60; item_tree.height = 60;
	item_tree.data = NULL;
	item_tree = readRGBA2ARVB("Sprites/item_tree[60x60].rgba",item_tree);

	//wood
	item_wood.width = 60; item_wood.height = 60;
	item_wood.data = NULL;
	item_wood = readRGBA2ARVB("Sprites/item_wood[60x60].rgba",item_wood);

	//fiber
	item_fiber.width = 60; item_fiber.height = 60;
	item_fiber.data = NULL;
	item_fiber = readRGBA2ARVB("Sprites/item_fiber[60x60].rgba",item_fiber);

	//space suit
	item_space_suit.width = 60; item_space_suit.height = 60;
	item_space_suit.data = NULL;
	item_space_suit = readRGBA2ARVB("Sprites/item_space_suit[60x60].rgba",item_space_suit);

	//multi tool
	item_multi_tool.width = 60; item_multi_tool.height = 60;
	item_multi_tool.data = NULL;
	item_multi_tool = readRGBA2ARVB("Sprites/item_multi_tool[60x60].rgba",item_multi_tool);

	//pickaxe
	item_pickaxe.width = 60; item_pickaxe.height = 60;
	item_pickaxe.data = NULL;
	item_pickaxe = readRGBA2ARVB("Sprites/item_pickaxe[60x60].rgba",item_pickaxe);

	//rock
	item_rock.width = 60; item_rock.height = 60;
	item_rock.data = NULL;
	item_rock = readRGBA2ARVB("Sprites/item_rock[60x60].rgba",item_rock);

	//iron
	item_iron.width = 60; item_iron.height = 60;
	item_iron.data = NULL;
	item_iron = readRGBA2ARVB("Sprites/item_iron[60x60].rgba",item_iron);

	//screwdriver
	item_screwdriver.width = 60; item_screwdriver.height = 60;
	item_screwdriver.data = NULL;
	item_screwdriver = readRGBA2ARVB("Sprites/item_screwdriver[60x60].rgba",item_screwdriver);

	//hammer
	item_hammer.width = 60; item_hammer.height = 60;
	item_hammer.data = NULL;
	item_hammer = readRGBA2ARVB("Sprites/item_hammer[60x60].rgba",item_hammer);

	//oxygen tank
	item_oxygen_tank.width = 60; item_oxygen_tank.height = 60;
	item_oxygen_tank.data = NULL;
	item_oxygen_tank = readRGBA2ARVB("Sprites/item_oxygen_tank[60x60].rgba",item_oxygen_tank);

	//machine_base
	item_machine_base.width = 60; item_machine_base.height = 60;
	item_machine_base.data = NULL;
	item_machine_base = readRGBA2ARVB("Sprites/item_machine_base[60x60].rgba",item_machine_base);

	//fuel maker
	item_fuel_maker.width = 60; item_fuel_maker.height = 60;
	item_fuel_maker.data = NULL;
	item_fuel_maker = readRGBA2ARVB("Sprites/item_fuel_maker[60x60].rgba",item_fuel_maker);

	//fuel
	item_fuel.width = 60; item_fuel.height = 60;
	item_fuel.data = NULL;
	item_fuel = readRGBA2ARVB("Sprites/item_fuel[60x60].rgba",item_fuel);

	//rocket
	item_rocket.width = 60; item_rocket.height = 60;
	item_rocket.data = NULL;
	item_rocket = readRGBA2ARVB("Sprites/item_rocket[60x60].rgba",item_rocket);

	//rocket high
	item_rocket_high.width = 60; item_rocket_high.height = 160;
	item_rocket_high.data = NULL;
	item_rocket_high = readRGBA2ARVB("Sprites/item_rocket_high[60x160].rgba",item_rocket_high);

	//fruit
	item_fruit.width = 60; item_fruit.height = 60;
	item_fruit.data = NULL;
	item_fruit = readRGBA2ARVB("Sprites/item_fruit[60x60].rgba",item_fruit);

	//oxygen generator
	item_oxygen_generator.width = 60; item_oxygen_generator.height = 60;
	item_oxygen_generator.data = NULL;
	item_oxygen_generator = readRGBA2ARVB("Sprites/item_oxygen_generator[60x60].rgba",item_oxygen_generator);
	item_oxygen_generator_tanked.width = 60; item_oxygen_generator_tanked.height = 60;
	item_oxygen_generator_tanked.data = NULL;
	item_oxygen_generator_tanked = readRGBA2ARVB("Sprites/item_oxygen_generator_tanked[60x60].rgba",item_oxygen_generator_tanked);

	//furniture
	item_furniture.width = 60; item_furniture.height = 60;
	item_furniture.data = NULL;
	item_furniture = readRGBA2ARVB("Sprites/item_furniture[60x60].rgba",item_furniture);
}

img getSprite(int ref){
	switch(ref){

		//icons
		case -1:
			return icon_inv;
		case -2:
			return icon_crafts;
		case -3:
			return icon_equip;
		case -4:
			return icon_space;
		//items
		case 1:
			return item_tree;
		case 2:
			return item_wood;
		case 3:
			return item_fiber;
		case 4:
			return item_space_suit;
		case 5:
			return item_multi_tool;
		case 6:
			return item_pickaxe;
		case 7:
			return item_rock;
		case 8:
			return item_iron;
		case 9:
			return item_screwdriver;
		case 10:
			return item_hammer;
		case 11:
			return item_oxygen_tank;
		case 12:
			return item_machine_base;
		case 13:
			return item_fuel_maker;
		case 14:
			return item_fuel;
		case 15:
			return item_rocket;
		case 16:
			return item_fruit;
		case 17:
			return item_oxygen_generator;
		case 18:
			return item_furniture;
	}
	return voidImg;
}

void showCrafts(){
	S2DE_setColor(150,150,150);
	S2DE_rectangle(p2Coo[0],p2Coo[1],p2Coo[0]+340,p2Coo[1]-315,true);

	//first craft base
	S2DE_setColor(210,210,210);
	S2DE_rectangle(p2Coo[0]+5,p2Coo[1]-5,p2Coo[0]+335,p2Coo[1]-95,true); //base
	S2DE_setColor(150,150,150);
	S2DE_rectangle(p2Coo[0]+10,p2Coo[1]-10,p2Coo[0]+70,p2Coo[1]-70,true); //item created
	S2DE_rectangle(p2Coo[0]+75,p2Coo[1]-10,p2Coo[0]+245,p2Coo[1]-90,true); //items needed
	S2DE_rectangle(p2Coo[0]+250,p2Coo[1]-35,p2Coo[0]+330,p2Coo[1]-65,true); //craft button
	S2DE_setColor(255,0,0);
	S2DE_setThickness(4);
	S2DE_littleText("CRAFT",20,p2Coo[0]+255,p2Coo[1]-60);

	//second craft case
	S2DE_setColor(210,210,210);
	S2DE_rectangle(p2Coo[0]+5,p2Coo[1]-100,p2Coo[0]+335,p2Coo[1]-190,true); //base
	S2DE_setColor(150,150,150);
	S2DE_rectangle(p2Coo[0]+10,p2Coo[1]-105,p2Coo[0]+70,p2Coo[1]-165,true); //item created
	S2DE_rectangle(p2Coo[0]+75,p2Coo[1]-105,p2Coo[0]+245,p2Coo[1]-185,true); //items needed
	S2DE_rectangle(p2Coo[0]+250,p2Coo[1]-130,p2Coo[0]+330,p2Coo[1]-160,true); //craft button
	S2DE_setColor(255,0,0);
	S2DE_setThickness(4);
	S2DE_littleText("CRAFT",20,p2Coo[0]+255,p2Coo[1]-155);

	//third craft case
	S2DE_setColor(210,210,210);
	S2DE_rectangle(p2Coo[0]+5,p2Coo[1]-195,p2Coo[0]+335,p2Coo[1]-285,true); //base
	S2DE_setColor(150,150,150);
	S2DE_rectangle(p2Coo[0]+10,p2Coo[1]-200,p2Coo[0]+70,p2Coo[1]-260,true); //item created
	S2DE_rectangle(p2Coo[0]+75,p2Coo[1]-200,p2Coo[0]+245,p2Coo[1]-280,true); //items needed
	S2DE_rectangle(p2Coo[0]+250,p2Coo[1]-225,p2Coo[0]+330,p2Coo[1]-255,true); //craft button
	S2DE_setColor(255,0,0);
	S2DE_setThickness(4);
	S2DE_littleText("CRAFT",20,p2Coo[0]+255,p2Coo[1]-250);

	//down arrow
	S2DE_setColor(210,210,210);
	S2DE_rectangle(p2Coo[0]+5,p2Coo[1]-290,p2Coo[0]+335,p2Coo[1]-310,true); //base
	S2DE_setColor(0,0,0);
	S2DE_triangle(p2Coo[0]+157,p2Coo[1]-295,p2Coo[0]+162,p2Coo[1]-305,p2Coo[0]+167,p2Coo[1]-295,true); //arrow

	//contents
	S2DE_setThickness(2);
	switch(craftPage){
		case 0:
			//first craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Space suit",12,p2Coo[0]+6,p2Coo[1]-85);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-70, item_space_suit);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Fiber(x3)",12,p2Coo[0]+77,p2Coo[1]-38);

			//second craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Multi tool",12,p2Coo[0]+11,p2Coo[1]-180);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-165, item_multi_tool);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Wood(x1)",12,p2Coo[0]+77,p2Coo[1]-133);
			S2DE_littleText("Fiber(x1)",12,p2Coo[0]+77,p2Coo[1]-174);

			//third craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Pickaxe",12,p2Coo[0]+18,p2Coo[1]-275);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-260, item_pickaxe);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Multi tool(x1)",12,p2Coo[0]+77,p2Coo[1]-228);
			S2DE_littleText("Rock(x2)",12,p2Coo[0]+77,p2Coo[1]-269);
		break;
		case 1:
			//first craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Screwdriver",12,p2Coo[0]+6,p2Coo[1]-85);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-70, item_screwdriver);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Multi tool(x1)",12,p2Coo[0]+77,p2Coo[1]-38);
			S2DE_littleText("Iron(x1)",12,p2Coo[0]+77,p2Coo[1]-79);

			//second craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Hammer",12,p2Coo[0]+13,p2Coo[1]-180);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-165, item_hammer);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Multi tool(x1)",12,p2Coo[0]+77,p2Coo[1]-133);
			S2DE_littleText("Iron(x2)",12,p2Coo[0]+77,p2Coo[1]-174);

			//third craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Machine base",9,p2Coo[0]+7,p2Coo[1]-275);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-260, item_machine_base);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Rock(x2)",12,p2Coo[0]+77,p2Coo[1]-228);
			S2DE_littleText("Iron(x2)",12,p2Coo[0]+77,p2Coo[1]-269);
			S2DE_littleText("Multi tool(x1)",12,p2Coo[0]+162,p2Coo[1]-228);
		break;
		case 2:
			//first craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Fuel maker",11,p2Coo[0]+7,p2Coo[1]-85);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-70, item_fuel_maker);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Machine base(x1)",11,p2Coo[0]+77,p2Coo[1]-38);
			S2DE_littleText("Hammer(x1)",12,p2Coo[0]+77,p2Coo[1]-79);

			//second craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Rocket",12,p2Coo[0]+20,p2Coo[1]-180);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-165, item_rocket);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Machine base(x2)",10,p2Coo[0]+77,p2Coo[1]-133);
			S2DE_littleText("Wood(x2)",12,p2Coo[0]+77,p2Coo[1]-174);
			S2DE_littleText("Screwdriver(x2)",10,p2Coo[0]+169,p2Coo[1]-133);
			S2DE_littleText("Iron(x2)",12,p2Coo[0]+162,p2Coo[1]-174);

			//third craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Oxygen tank",12,p2Coo[0]+6,p2Coo[1]-275);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-260, item_oxygen_tank);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Iron (x3)",12,p2Coo[0]+77,p2Coo[1]-228);
		break;
		case 3:
			//first craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Oxygen generator",10,p2Coo[0]+5,p2Coo[1]-85);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-70, item_oxygen_generator);
			S2DE_setColor(0,0,255);
			S2DE_point(p2Coo[0]+34,p2Coo[1]-50);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Machine base(x1)",11,p2Coo[0]+77,p2Coo[1]-38);
			S2DE_littleText("Hammer(x2)",12,p2Coo[0]+77,p2Coo[1]-72);
			S2DE_littleText("Fiber(x3)",12,p2Coo[0]+178,p2Coo[1]-38);
			S2DE_littleText("Fruit(x1)",12,p2Coo[0]+178,p2Coo[1]-72);

			//second craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Furniture",12,p2Coo[0]+8,p2Coo[1]-180);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-165, item_furniture);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Iron(x4)",12,p2Coo[0]+77,p2Coo[1]-133);
			S2DE_littleText("Screwdriver(x1)",12,p2Coo[0]+77,p2Coo[1]-174);
			S2DE_setColor(0,0,255);
			S2DE_littleText("Only craftable",12,p2Coo[0]+250,p2Coo[1]-120);
			S2DE_littleText("5 times",12,p2Coo[0]+270,p2Coo[1]-180);
		break;
		/* patern for new craft pages
		case n:
			//first craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Element1",12,p2Coo[0]+6,p2Coo[1]-85);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-70, item_Element1);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Need 11 (x1)",12,p2Coo[0]+77,p2Coo[1]-38);
			S2DE_littleText("Need 12 (x2)",12,p2Coo[0]+77,p2Coo[1]-79);
			S2DE_littleText("Need 13 (x3)",12,p2Coo[0]+162,p2Coo[1]-38);
			S2DE_littleText("Need 14 (x4)",12,p2Coo[0]+162,p2Coo[1]-79);
			S2DE_setColor(0,0,255);
			S2DE_littleText("Only craftable",11,p2Coo[0]+252,p2Coo[1]-25);
			S2DE_littleText("ONCE",11,p2Coo[0]+273,p2Coo[1]-85);

			//second craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Element2",12,p2Coo[0]+6,p2Coo[1]-180);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-165, item_Element2);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Need 21 (x1)",12,p2Coo[0]+77,p2Coo[1]-133);
			S2DE_littleText("Need 22 (x2)",12,p2Coo[0]+77,p2Coo[1]-174);
			S2DE_littleText("Need 23 (x3)",12,p2Coo[0]+162,p2Coo[1]-133);
			S2DE_littleText("Need 24 (x4)",12,p2Coo[0]+162,p2Coo[1]-174);
			S2DE_setColor(0,0,255);
			S2DE_littleText("Only craftable",11,p2Coo[0]+252,p2Coo[1]-120);
			S2DE_littleText("ONCE",11,p2Coo[0]+273,p2Coo[1]-180);

			//third craft
			S2DE_setColor(0,0,0);
			S2DE_littleText("Element3",12,p2Coo[0]+6,p2Coo[1]-275);
			ecrisImageIMG(p2Coo[0]+10,p2Coo[1]-260, item_Element3);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Need 31 (x1)",12,p2Coo[0]+77,p2Coo[1]-228);
			S2DE_littleText("Need 32 (x2)",12,p2Coo[0]+77,p2Coo[1]-269);
			S2DE_littleText("Need 33 (x3)",12,p2Coo[0]+162,p2Coo[1]-228);
			S2DE_littleText("Need 34 (x4)",12,p2Coo[0]+162,p2Coo[1]-269);
		break;
		*/
	}
}




















// ==================== MEGASWITCHES =======================

//display
void DisplayMegaSwitch(){
	short int div;
	float angle,angle1,angle2, cosA,sinA,cosA2,sinA2, temp1,temp2;
	S2DE_setColor(0,0,22);
	S2DE_rectangle(0,0, 1600,900, 1);
	switch(menu){

		case 0: //start_menu
			S2DE_setColor(50,50,100);
			S2DE_rectangle(200,600,1400,700,true); //title + play & settings buttons
			S2DE_rectangle(580,300,1020,380,true);
			//S2DE_rectangle(620,150,980,200,true);
			S2DE_setColor(245,245,245);
			S2DE_setThickness(13);
			S2DE_littleText("SPACE CONQUEST",90,350,610);
			S2DE_setThickness(10);
			S2DE_littleText("PLAY",70,700,310);
			//S2DE_setThickness(7);
			//S2DE_littleText("SETTINGS",40,700,155);
			S2DE_setThickness(4);
			S2DE_littleText("by I.A.",20,20,20);
			S2DE_littleText("[0.1.0]",20,1520,20);
		break;

		case 1: //start_settings
		break;

		case 2: //planet

			//life
			S2DE_setColor(155,0,0);
			S2DE_rectangle(0,900,400,885,true);
			S2DE_setColor(255,0,0);
			S2DE_rectangle(0,900,400*life/lifeMax,885,true);

			//oxygen
			S2DE_setColor(0,155,155);
			S2DE_rectangle(0,885,400,870,true);
			S2DE_setColor(0,255,255);
			temp1 = ownOxygen;
			temp2 = ownOxygenMax;
			if(equipment[1].id == 11){
				temp1 += equipment[1].data;
				temp2 += oxygenTankMax;
			}
			if(equipment[2].id == 11){
				temp1 += equipment[2].data;
				temp2 += oxygenTankMax;
			}
			S2DE_rectangle(0,885,400*temp1/temp2,870,true);

			//both
			S2DE_setThickness(2);
			S2DE_setColor(255,255,255);
			S2DE_littleText("Life",12,405,887);
			S2DE_littleText("Oxygen",12,405,872);

			//current planet
			if(g.planets[planetRow].atm){
				S2DE_setColor(0,80,80);
				circle(800,450,plRadius+300,true,true);
				S2DE_setThickness(3);
				S2DE_setColor(150,230,230);
				circle(800,450,plRadius+300,true,false);
			}
			setPlColor(g.planets[planetRow]);
			circle(800,450,plRadius,true,true);

			//ground suffering
			if(hitCnt > 0){
				S2DE_setColor(150,0,0);
				S2DE_triangle(800,450,
					tCoo[hitDiv][0][0],tCoo[hitDiv][0][1],
					tCoo[hitDiv][1][0],tCoo[hitDiv][1][1],
				true);
			}

			//objects
			div = getDiv();
			for(short int i=0; i < div; i++){
				angle = (i+0.5)*M_2PI/div;
				if(g.planets[planetRow].objects[i].id == 15) //if object is a rocket
					writeRotatedImage(
						oCoo[i][0]+50*sin(angle), //display it half higher
						oCoo[i][1]+50*cos(angle),
						item_rocket_high,
						angle,
					true);
				else{
					if(g.planets[planetRow].objects[i].id == 13){ //if object is a fuel maker
						writeRotatedImage(
							oCoo[i][0],oCoo[i][1],
							item_fuel_maker,
							angle,
						true);
						if(g.planets[planetRow].objects[i].data != 0) //override sprite with a fiber when producing
							writeRotatedImage(
								oCoo[i][0],oCoo[i][1],
								item_fiber,
								angle,
							true);
						angle1 = angle - M_2PI/div;			//draw a bar on his right side
						angle2 = angle + M_2PI/div;
						cosA  = cos(angle1); sinA  = sin(angle1);
						cosA2 = cos(angle2); sinA2 = sin(angle2);
						S2DE_setColor(80,80,80);
						S2DE_setThickness(5);
						if(i == div-1)
							S2DE_line(
								oCoo[i][0]-30*sinA,
								oCoo[i][1]-30*cosA,
								oCoo[0][0]-30*sinA2,
								oCoo[0][1]-30*cosA2
							);
						else
							S2DE_line(
								oCoo[i][0]-30*sinA,
								oCoo[i][1]-30*cosA,
								oCoo[(int)(i+1)][0]-30*sinA2,
								oCoo[(int)(i+1)][1]-30*cosA2
							);
					}else if(g.planets[planetRow].objects[i].id == 17){ //if object is an oxygen generator
						if(g.planets[planetRow].objects[i].data == 0) //if not active
							writeRotatedImage(
								oCoo[i][0],oCoo[i][1],
								item_oxygen_generator,
								angle,
							true);
						else{										  //if active
							writeRotatedImage(
								oCoo[i][0],oCoo[i][1],
								item_oxygen_generator_tanked,
								angle,
							true);
							if(g.planets[planetRow].objects[i].data == 1+oxygenTankMax){ //if oxygenTank charged
								S2DE_setColor(0,255,0);
								cosA = cos(angle);
								sinA = sin(angle);
								S2DE_point(
									oCoo[i][0]+15*cosA-15*sinA,
									oCoo[i][1]+15*sinA+15*cosA
								);
							}
						}
					}else
						writeRotatedImage(
							oCoo[i][0],oCoo[i][1],
							getSprite(g.planets[planetRow].objects[i].id),
							angle,
						true);
				}
				if(focus == 1){ //put_down_item (triangles)
					S2DE_setThickness(3);
					S2DE_setColor(0,240,0);
					S2DE_triangle(800,450,
						tCoo[i][0][0],tCoo[i][0][1],
						tCoo[i][1][0],tCoo[i][1][1],
					tStates[i]);
				}
			}

			//player
			if(equipment[0].id == 4){ //player equip a space suit
				if(pRight){
					if(pRunning == 0)
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerSuitR_stop,
							pAngle,
						true);
					else if(pRunning == 1)
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerSuitR_run1,
							pAngle,
						true);
					else
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerSuitR_run2,
							pAngle,
						true);
				}else{
					if(pRunning == 0)
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerSuitL_stop,
							pAngle,
						true);
					else if(pRunning == 1)
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerSuitL_run1,
							pAngle,
						true);
					else
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerSuitL_run2,
							pAngle,
						true);
				}
			}else{
				if(pRight){
					if(pRunning == 0)
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerR_stop,
							pAngle,
						true);
					else if(pRunning == 1)
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerR_run1,
							pAngle,
						true);
					else
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerR_run2,
							pAngle,
						true);
				}else{
					if(pRunning == 0)
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerL_stop,
							pAngle,
						true);
					else if(pRunning == 1)
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerL_run1,
							pAngle,
						true);
					else
						writeRotatedImage(
							pCoo[0],pCoo[1],
							playerL_run2,
							pAngle,
						true);
				}
			}

			//hotbar
			S2DE_setColor(150,150,150);
			S2DE_rectangle(640,0,1000,80,true);
			S2DE_setColor(90,90,90);
			for(short int i=0; i < 5; i++){
				S2DE_rectangle(650+i*70,10,710+i*70,70,true);
				ecrisImageIMG(650+i*70,10, getSprite(inventory[0][i].id));
			}

			//see if mouse is on a hotbar item (display its name)
			for(short int i=0; i < 5; i++){
				if(inZone(S2DE_mouseX,S2DE_mouseY,
				   650+i*70,10,710+i*70,70)
				   && inventory[0][i].id != 0){ //<=> mouse not on void case
					S2DE_setColor(255,255,255);
					S2DE_setThickness(2);
					S2DE_littleText(objectsName[inventory[0][i].id],15,650+i*70,85);

					//percentages items
					if(inventory[0][i].id == 6) //pickaxe
						showNumber( (int)(100*inventory[0][i].data/toolsDurability) ,15,710+i*70,85);
					else if(inventory[0][i].id == 11) //oxygen tank
						showNumber( (int)(100*inventory[0][i].data/oxygenTankMax) ,15,745+i*70,85);
					else if(inventory[0][i].id == 15) //rocket
						showNumber( (int)(100*inventory[0][i].data/rocketFuelMax) ,15,700+i*70,85);
					break;
				}
			}
			S2DE_setThickness(3);
			S2DE_setColor(0,0,255);
			S2DE_rectangle(645+hotbarSel*70,5,715+hotbarSel*70,75,false);

			//options_panels
			if(focus >= 2){ //options_panel1 or more
				S2DE_setColor(150,150,150);
				S2DE_rectangle(p1Coo[0],p1Coo[1],p1Coo[0]+170,p1Coo[1]+170,true);
				S2DE_setColor(210,210,210);

				//alpha zone : inventory
				S2DE_rectangle(p1Coo[0]+10 ,p1Coo[1]+90 ,p1Coo[0]+80,p1Coo[1]+160,true);
				ecrisImageIMG(p1Coo[0]+10,p1Coo[1]+90, getSprite(-1)); //icon_inventory

				//beta zone : crafts
				S2DE_rectangle(p1Coo[0]+90,p1Coo[1]+90 ,p1Coo[0]+160,p1Coo[1]+160,true);
				ecrisImageIMG(p1Coo[0]+90,p1Coo[1]+90, getSprite(-2)); //icon_crafts

				//gamma zone : equip
				S2DE_rectangle(p1Coo[0]+10 ,p1Coo[1]+10 ,p1Coo[0]+80,p1Coo[1]+80 ,true);
				ecrisImageIMG(p1Coo[0]+10,p1Coo[1]+10, getSprite(-3)); //icon_equip

				//delta zone : space
				S2DE_rectangle(p1Coo[0]+90,p1Coo[1]+10 ,p1Coo[0]+160,p1Coo[1]+80 ,true);
				ecrisImageIMG(p1Coo[0]+90,p1Coo[1]+10, getSprite(-4)); //icon_space

				//options_panel2
				if(focus == 3) //options_panel2[crafts]
					showCrafts();
				else if(focus == 4){ //options_panel2[equip]
					S2DE_setColor(150,150,150);
					S2DE_rectangle(p2Coo[0],p2Coo[1],p2Coo[0]+340,p2Coo[1]-220,true);
					S2DE_setColor(210,210,210);
					S2DE_rectangle(p2Coo[0]+5,p2Coo[1]-5,p2Coo[0]+336,p2Coo[1]-215,true);
					S2DE_setColor(150,150,150);
					S2DE_rectangle(p2Coo[0]+120,p2Coo[1]-10,p2Coo[0]+220,p2Coo[1]-210,true);
					S2DE_rectangle(p2Coo[0]+30,p2Coo[1]-80,p2Coo[0]+90,p2Coo[1]-140,true); //space suit
					S2DE_rectangle(p2Coo[0]+250,p2Coo[1]-20,p2Coo[0]+310,p2Coo[1]-80,true); //oxygen tank1
					S2DE_rectangle(p2Coo[0]+250,p2Coo[1]-130,p2Coo[0]+310,p2Coo[1]-190,true); //oxygen tank2
					if(equipment[0].id == 4){ //if space suit equiped
						if(pRight)
							ecrisImageIMG(p2Coo[0]+120,p2Coo[1]-210,playerSuitR_stop);
						else
							ecrisImageIMG(p2Coo[0]+120,p2Coo[1]-210,playerSuitL_stop);
					}else{
						if(pRight)
							ecrisImageIMG(p2Coo[0]+120,p2Coo[1]-210,playerR_stop);
						else
							ecrisImageIMG(p2Coo[0]+120,p2Coo[1]-210,playerL_stop);
					}
					ecrisImageIMG(p2Coo[0]+30,p2Coo[1]-140,getSprite(equipment[0].id)); //space suit
					ecrisImageIMG(p2Coo[0]+250,p2Coo[1]-80,getSprite(equipment[1].id)); //oxygen tank1
					ecrisImageIMG(p2Coo[0]+250,p2Coo[1]-190,getSprite(equipment[2].id)); //oxygen tank2

					//show names and percentages
					if(inZone(S2DE_mouseX,S2DE_mouseY, //space suit
					   p2Coo[0]+30,p2Coo[1]-140,p2Coo[0]+90,p2Coo[1]-80) && equipment[0].id == 4){
						S2DE_setThickness(2);
						S2DE_setColor(255,255,255);
						S2DE_littleText("Space suit",14,p2Coo[0]+25,p2Coo[1]-155);
					}else if(inZone(S2DE_mouseX,S2DE_mouseY, //oxygen tank1
					   p2Coo[0]+250,p2Coo[1]-80,p2Coo[0]+310,p2Coo[1]-20) && equipment[1].id == 11){
						S2DE_setThickness(2);
						S2DE_setColor(255,255,255);
						S2DE_littleText("Oxygen tank",11,p2Coo[0]+222,p2Coo[1]-95);
						showNumber( (int)(100*equipment[1].data/oxygenTankMax) ,11,p2Coo[0]+295,p2Coo[1]-95);
					}else if(inZone(S2DE_mouseX,S2DE_mouseY, //oxygen tank2
					   p2Coo[0]+250,p2Coo[1]-190,p2Coo[0]+310,p2Coo[1]-130) && equipment[2].id == 11){
						S2DE_setThickness(2);
						S2DE_setColor(255,255,255);
						S2DE_littleText("Oxygen tank",11,p2Coo[0]+222,p2Coo[1]-205);
						showNumber( (int)(100*equipment[2].data/oxygenTankMax) ,11,p2Coo[0]+295,p2Coo[1]-205);
					}
				}
			}
		break;

		case 3: //inventory

			//planet (viewed from the player)
			setPlColor(g.planets[planetRow]);
			circle(800,0,400,false,true);

			//inventory
			S2DE_setColor(150,150,150);
			S2DE_rectangle(620,305,980,595,true);
			S2DE_setColor(90,90,90);
			for(short int i=0; i < 5; i++){
				for(short int j=0; j < 4; j++){
					S2DE_rectangle(630+i*70,315+j*70,690+i*70,375+j*70,true);
					if(focus != 1 || xSel != i || ySel != j+1) //item_selected & it is the [j+1][i]
						ecrisImageIMG(630+i*70,315+j*70,getSprite(inventory[(int)(j+1)][i].id));
				}
			}

			//see if mouse is on an inventory item (display its name)
			for(short int i=0; i < 5; i++){ //see what is selected
				for(short int j=0; j < 4; j++){
					if(inZone(S2DE_mouseX,S2DE_mouseY,
					   630+i*70,315+j*70,690+i*70,375+j*70)
					   && inventory[(int)(j+1)][i].id != 0){ //<=> mouse not on void case
						S2DE_setThickness(2);
						S2DE_setColor(255,255,255);
						S2DE_littleText(objectsName[inventory[(int)(j+1)][i].id],15,650+i*70,390+j*70);

						//percentages items
						if(inventory[(int)(j+1)][i].id == 6){
							//pickaxe
							showNumber( (int)(100*inventory[(int)(j+1)][i].data/toolsDurability) ,15,710+i*70,390+j*70);
						}else if(inventory[(int)(j+1)][i].id == 11){
							//oxygen tank
							showNumber( (int)100*(inventory[(int)(j+1)][i].data/oxygenTankMax) ,15,745+i*70,390+j*70);
						}else if(inventory[(int)(j+1)][i].id == 15){
							//rocket
							showNumber( (int)(100*inventory[(int)(j+1)][i].data/rocketFuelMax) ,15,700+i*70,390+j*70);
						}
						break;
					}
				}
			}

			//hotbar
			S2DE_setColor(150,150,150);
			S2DE_rectangle(640,0,1000,80,true);
			S2DE_setColor(90,90,90);
			for(short int i=0; i < 5; i++){
				S2DE_rectangle(650+i*70,10,710+i*70,70,true);
				if(focus != 1 || xSel != i || ySel != 0) //item_selected & it is the [0][i]
					ecrisImageIMG(650+i*70,10, getSprite(inventory[0][i].id));
			}
			S2DE_setColor(0,0,255);
			S2DE_rectangle(645+hotbarSel*70,5,715+hotbarSel*70,75,false);

			//see if mouse is on a hotbar item (display its name)
			for(short int i=0; i < 5; i++){
				if(inZone(S2DE_mouseX,S2DE_mouseY,
				   650+i*70,10,710+i*70,70)
				   && inventory[0][i].id != 0){ //<=> mouse not on void case
					S2DE_setThickness(2);
					S2DE_setColor(255,255,255);
					S2DE_littleText(objectsName[inventory[0][i].id],15,650+i*70,85);

					//percentages items
					if(inventory[0][i].id == 6){
						//pickaxe
						showNumber( (int)(100*inventory[0][i].data/toolsDurability) ,15,710+i*70,85);
					}else if(inventory[0][i].id == 11){
						//oxygen tank
						showNumber( (int)(100*inventory[0][i].data/oxygenTankMax) ,15,745+i*70,85);
					}else if(inventory[0][i].id == 15){
						//rocket
						showNumber( (int)(100*inventory[0][i].data/rocketFuelMax) ,15,700+i*70,85);
					}
					break;
				}
			}

			//focuses reactions
			if(focus == 1) //item_selected
				ecrisImageIMG(S2DE_mouseX-30,S2DE_mouseY-30,getSprite(inventory[ySel][xSel].id));
			else if(focus == 2){ //message_delete
				S2DE_setColor(150,150,150);
				S2DE_rectangle(600,350,1000,550,true);
				S2DE_setColor(0,0,0);
				S2DE_rectangle(600,350,1000,550,false);
				S2DE_setColor(90,90,90); //yes & cancel buttons
				S2DE_rectangle(680,370,780,430,true);
				S2DE_rectangle(820,370,920,430,true);
				S2DE_setColor(255,255,255);
				S2DE_setThickness(3);
				S2DE_littleText("Delete item ?",30,700,480);
				S2DE_littleText("Yes    Cancel",25,700,380);
			}
		break;

		case 4: //space

			//sun
			S2DE_setColor(255,255,0);
			circle(g.sun.x,g.sun.y,g.sun.r,true,true);

			//stars
			S2DE_setColor(255,255,0);
			for(short int s=0; s < g.nbrStars; s++)
				S2DE_point(g.stars[s].x,g.stars[s].y);

			//planets
			for(short int p=0; p < g.nbrPlanets; p++){
				setPlColor(g.planets[p]);
				circle(g.planets[p].x,
					   g.planets[p].y,
					   g.planets[p].size*6,
				true,true);
				if(p == planetSel){ //marking selected planet
					S2DE_setColor(255,255,255);
					circle(g.planets[p].x,
						   g.planets[p].y,
						   g.planets[p].size*6+4,
					true,false);
				}
			}

			//marking current planet
			S2DE_setColor(255,0,255);
			circle(g.planets[planetRow].x,
				   g.planets[planetRow].y,
				   g.planets[planetRow].size*6+4,
			true,false);
			if(focus == 1){ //error message
				S2DE_setColor(150,150,150);
				S2DE_rectangle(600,350,1000,550,true);
				S2DE_setColor(0,0,0);
				S2DE_rectangle(600,350,1000,550,false);
				S2DE_setColor(90,90,90); //OK buttons
				S2DE_rectangle(740,360,880,420,true);
				S2DE_setColor(255,255,255);
				S2DE_setThickness(3);
				S2DE_littleText("Rocket or player not ok",25,640,480);
				S2DE_littleText("OK",25,790,380);
			}
		break;

		case 5: //rocket_animation

			//current planet
			if(g.planets[planetRow].atm){
				S2DE_setColor(0,80,80);
				circle(800,450,plRadius+300,true,true);
				S2DE_setThickness(3);
				S2DE_setColor(150,230,230);
				circle(800,450,plRadius+300,true,false);
			}
			setPlColor(g.planets[planetRow]);
			circle(800,450,plRadius,true,true);

			//objects
			div = getDiv();
			for(short int i=0; i < div; i++){
				angle = (i+0.5)*M_2PI/div;
				if(g.planets[planetRow].objects[i].id == 15){ //if object is a rocket
					cosA = cos(angle);
					sinA = sin(angle);
					writeRotatedImage(
						oCoo[i][0]+(50+rocketDistance)*sinA, //display it half higher + rocketDistance
						oCoo[i][1]+(50+rocketDistance)*cosA,
						item_rocket_high,
						angle,
					true);
					if(focus > 0) //take off (fire)
						writeRotatedImage(
							oCoo[i][0]+ (rocketDistance-40)*sinA, //display it lower + rocketDistance
							oCoo[i][1]+ (rocketDistance-40)*cosA,
							fire,
							angle,
						true);
				}else{
					writeRotatedImage(
						oCoo[i][0],oCoo[i][1],
						getSprite(g.planets[planetRow].objects[i].id),
						angle,
					true);
					if(g.planets[planetRow].objects[i].id == 13){ //if object is a fuel maker
						if(g.planets[planetRow].objects[i].data != 0) //override sprite with a fiber when producing
							writeRotatedImage(
								oCoo[i][0],oCoo[i][1],
								item_fiber,
								angle,
							true);
						angle1 = angle - M_2PI/div;			//draw a bar on his right side
						angle2 = angle + M_2PI/div;
						cosA  = cos(angle1); sinA  = sin(angle1);
						cosA2 = cos(angle2); sinA2 = sin(angle2);
						S2DE_setColor(80,80,80);
						S2DE_setThickness(5);
						if(i == div-1)
							S2DE_line(
								oCoo[i][0]-30*sinA,
								oCoo[i][1]-30*cosA,
								oCoo[0][0]-30*sinA2,
								oCoo[0][1]-30*cosA2
							);
						else
							S2DE_line(
								oCoo[i][0]-30*sinA,
								oCoo[i][1]-30*cosA,
								oCoo[(int)(i+1)][0]-30*sinA2,
								oCoo[(int)(i+1)][1]-30*cosA2
							);
					}else if(g.planets[planetRow].objects[i].id == 17){ //if object is an oxygen generator
						if(g.planets[planetRow].objects[i].data == 0) //if not active
							writeRotatedImage(
								oCoo[i][0],oCoo[i][1],
								item_oxygen_generator,
								angle,
							true);
						else{										  //if active
							writeRotatedImage(
								oCoo[i][0],oCoo[i][1],
								item_oxygen_generator_tanked,
								angle,
							true);
							if(g.planets[planetRow].objects[i].data == 1+oxygenTankMax){ //if oxygenTank charged
								S2DE_setColor(0,255,0); //draw a green point at the top right corner
								cosA = cos(angle);
								sinA = sin(angle);
								S2DE_point(
									oCoo[i][0]+15*cosA-15*sinA,
									oCoo[i][1]+15*sinA+15*cosA
								);
							}
						}
					}
				}
			}

			//player
			if(focus == 0){ //player moving
				if(pRunning == 1)
					writeRotatedImage(
						pCoo[0],pCoo[1],
						playerSuitR_run1,
						pAngle,
					true);
				else
					writeRotatedImage(
						pCoo[0],pCoo[1],
						playerSuitR_run2,
						pAngle,
					true);
			}
		break;

		case 6: //furniture

			//planet (viewed from the player)
			setPlColor(g.planets[planetRow]);
			circle(800,0,400,false,true);

			//stock[stockSel]
			S2DE_setColor(150,150,150);
			S2DE_rectangle(620,305,980,595,true);
			S2DE_setColor(90,90,90);
			if(fromInv){
				for(short int i=0; i < 5; i++){
					for(short int j=0; j < 4; j++){
						S2DE_rectangle(630+i*70,315+j*70,690+i*70,375+j*70,true);
						ecrisImageIMG(630+i*70,315+j*70,getSprite(stock[stockSel][j][i].id));
					}
				}
			}else{
				for(short int i=0; i < 5; i++){
					for(short int j=0; j < 4; j++){
						S2DE_rectangle(630+i*70,315+j*70,690+i*70,375+j*70,true);
						if(focus != 1 || xSel != i || ySel != j) //item_selected & it is the [j][i]
							ecrisImageIMG(630+i*70,315+j*70,getSprite(stock[stockSel][j][i].id));
					}
				}
			}

			//see if mouse is on a stock[stockSel] item (display its name)
			for(short int i=0; i < 5; i++){ //see what is selected
				for(short int j=0; j < 4; j++){
					if(inZone(S2DE_mouseX,S2DE_mouseY,
					   630+i*70,315+j*70,690+i*70,375+j*70)
					   && stock[stockSel][j][i].id != 0){ //<=> mouse not on void case
						S2DE_setThickness(2);
						S2DE_setColor(255,255,255);
						S2DE_littleText(objectsName[stock[stockSel][j][i].id],15,650+i*70,390+j*70);

						//percentages items
						if(stock[stockSel][j][i].id == 6){
							//pickaxe
							showNumber( (int)(100*stock[stockSel][j][i].data/toolsDurability) ,15,710+i*70,390+j*70);
						}else if(stock[stockSel][j][i].id == 11){
							//oxygen tank
							showNumber( (int)100*(stock[stockSel][j][i].data/oxygenTankMax) ,15,745+i*70,390+j*70);
						}else if(stock[stockSel][j][i].id == 15){
							//rocket
							showNumber( (int)(100*stock[stockSel][j][i].data/rocketFuelMax) ,15,700+i*70,390+j*70);
						}
						break;
					}
				}
			}

			//hotbar
			S2DE_setColor(150,150,150);
			S2DE_rectangle(640,0,1000,80,true);
			S2DE_setColor(90,90,90);
			if(fromInv){
				for(short int i=0; i < 5; i++){
					S2DE_rectangle(650+i*70,10,710+i*70,70,true);
					if(focus != 1 || xSel != i) //item_selected & it is the [0][i]
						ecrisImageIMG(650+i*70,10, getSprite(inventory[0][i].id));
				}
			}else{
				for(short int i=0; i < 5; i++){
					S2DE_rectangle(650+i*70,10,710+i*70,70,true);
					ecrisImageIMG(650+i*70,10, getSprite(inventory[0][i].id));
				}
			}
			S2DE_setColor(0,0,255);
			S2DE_rectangle(645+hotbarSel*70,5,715+hotbarSel*70,75,false);

			//see if mouse is on a hotbar item (display its name)
			for(short int i=0; i < 5; i++){
				if(inZone(S2DE_mouseX,S2DE_mouseY,
				   650+i*70,10,710+i*70,70)
				   && inventory[0][i].id != 0){ //<=> mouse not on void case
					S2DE_setThickness(2);
					S2DE_setColor(255,255,255);
					S2DE_littleText(objectsName[inventory[0][i].id],15,650+i*70,85);

					//percentages items
					if(inventory[0][i].id == 6){
						//pickaxe
						showNumber( (int)(100*inventory[0][i].data/toolsDurability) ,15,710+i*70,85);
					}else if(inventory[0][i].id == 11){
						//oxygen tank
						showNumber( (int)(100*inventory[0][i].data/oxygenTankMax) ,15,745+i*70,85);
					}else if(inventory[0][i].id == 15){
						//rocket
						showNumber( (int)(100*inventory[0][i].data/rocketFuelMax) ,15,700+i*70,85);
					}
					break;
				}
			}

			//focuses reactions
			if(focus == 1){ //item_selected
				if(fromInv)
					ecrisImageIMG(S2DE_mouseX-30,S2DE_mouseY-30,getSprite(inventory[0][xSel].id));
				else
					ecrisImageIMG(S2DE_mouseX-30,S2DE_mouseY-30,getSprite(stock[stockSel][ySel][xSel].id));
			}
			else if(focus == 2){ //message_delete
				S2DE_setColor(150,150,150);
				S2DE_rectangle(600,350,1000,550,true);
				S2DE_setColor(0,0,0);
				S2DE_rectangle(600,350,1000,550,false);
				S2DE_setColor(90,90,90); //yes & cancel buttons
				S2DE_rectangle(680,370,780,430,true);
				S2DE_rectangle(820,370,920,430,true);
				S2DE_setColor(255,255,255);
				S2DE_setThickness(3);
				S2DE_littleText("Delete item ?",30,700,480);
				S2DE_littleText("Yes    Cancel",25,700,380);
			}
		break;
	}
}



//timed updates
void TimedUpdateMegaSwitch(){
	float temp;
	short int div, place;
	int tempData;
	switch(menu){

		case 2: //planet

			//player animation
			if(pRunningCnt > 0){
				pRunningCnt--;
				if(pRunningCnt == 0)
					pRunning = 0;
				else{
					temp = pRunningCnt/2.5;
					if(temp == (int)temp)
						pRunning++;
					if(pRunning > 2)
						pRunning = 1;
				}
			}

			//ground hitting
			if(hitCnt > 0)
				hitCnt--;
		break;

		case 4: //space
			if(gAngleCnt == 0)
				setGCoo();
		break;

		case 5: //rocket_animation
			if(focus != 0)
				rocketAnimCnt++;
			else{

				//player animation
				if(pRunningCnt > 0){
					pRunningCnt--;
					if(pRunningCnt == 0)
						pRunning = 0;
					else{
						temp = pRunningCnt/2.5;
						if(temp == (int)temp)
							pRunning++;
						if(pRunning > 2)
							pRunning = 1;
					}
				}
				pAngle += pAngleStep/8; //player auto-move
				if(pAngle > M_2PI)
					pAngle -= M_2PI;
				setPlayerCoo();
				if(abs(pAngle - rocketAngle) < pAngleStep/7)
					focus = 1; //fire
			}
			if(focus == 2) //take_off
				rocketDistance += rocketDistanceStep;
			if(focus == 1 && rocketAnimCnt > 60)
				focus = 2; //take_off
			if(rocketAnimCnt > 250){
				rocketAnimCnt = 0;
				gAngleCnt = 0;
				place = checkForObject(15);
				tempData = g.planets[planetRow].objects[place].data;
				g.planets[planetRow].objects[place].id = 0; //delete old rocket
				g.planets[planetRow].objects[place].data = 0;
				planetRow = planetSel;
				setPlanet(); //setPlayerCoo(), setTCoo() and setOCoo() included
				place = checkForObject(0); //cannot be -1
				pAngle = place*M_2PI/getDiv();
				setPlayerCoo();
				pRight = false;
				g.planets[planetRow].objects[place].id = 15; //deplace the new rocket
				g.planets[planetRow].objects[place].data = tempData;
				menu = 2; //planet
				focus = 0; //player_move
			}
		break;
	}

	//executed every 2 seconds when not taking off from the planet
	if(gAngleCnt == 0 && rocketAnimCnt == 0){
		if(g.planets[planetRow].atm){

			//oxygen regeneration
			temp = 0; //here used like a bool to know if oxygen was previously regenerated
			if(ownOxygen < ownOxygenMax){ //firstly, charge ownOxygen
				ownOxygen += 5;
				temp = 1; //oxygen regenerated
				if(ownOxygen > ownOxygenMax)
					ownOxygen = ownOxygenMax;
			}
			if(equipment[1].id == 11 && temp == 0){ //if oxygen tank1 equiped && oxygen not already regenerated
				if(equipment[1].data < oxygenTankMax){ //secondly, charge oxygen tank1
					equipment[1].data += 5;
					temp = 1; //oxygen regenerated
					if(equipment[1].data > oxygenTankMax)
						equipment[1].data = oxygenTankMax;
				}
			}
			if(equipment[2].id == 11 && temp == 0){ //if oxygen tank2 equiped and oxygen not already regenerated
				if(equipment[2].data < oxygenTankMax){ //finally, charge oxygen tank2
					equipment[2].data += 5;
					if(equipment[2].data > oxygenTankMax)
						equipment[2].data = oxygenTankMax;
				}
			}
		}else{

			//oxygen consumption
			temp = 0; //here used like a bool to know if oxygen was previously consummed
			if(equipment[2].id == 11){ //if oxygen tank2 equiped
				if(equipment[2].data > 0){ //firstly, use the oxygen of tank2
					equipment[2].data--;
					temp = 1; //oxygen consummed
				}
			}
			if(equipment[1].id == 11 && temp == 0){ //if oxygen tank1 equiped and no oxygen already consummed
				if(equipment[1].data > 0){ //secondly, use the oxygen of tank1
					equipment[1].data--;
					temp = 1; //oxygen consummed
				}
			}
			if(ownOxygen > 0 && temp == 0) //finally, use ownOxygen (if no oxygen already consummed)
				ownOxygen--;

			//without suit sufferring
			if(equipment[0].id != 4)
				life--;
		}

		//suffocation
		if(ownOxygen == 0 || (equipment[0].id != 4 && !g.planets[planetRow].atm)) //if no oxygen or (dont equip a space suit in a without atm planet)
			life--;

		//for each object
		div = getDiv();
		for(short int i=0; i < div; i++){
			if(g.planets[planetRow].objects[i].id == 13){ //fuel maker

				//for each fuel maker
				if(g.planets[planetRow].objects[i].data > 0){ //if producing
					g.planets[planetRow].objects[i].data--;
					if(g.planets[planetRow].objects[i].data == 0){ //if stopped (end of production)
						if(i != div-1){
							g.planets[planetRow].objects[(int)(i+1)].id = 14; //fuel
							g.planets[planetRow].objects[(int)(i+1)].data = 0;
						}else{
							g.planets[planetRow].objects[0].id = 14; //fuel
							g.planets[planetRow].objects[0].data = 0;
						}
					}
				}
			}else if(g.planets[planetRow].objects[i].id == 17){ //oxygen generator

				//for each oxygen generator
				if(g.planets[planetRow].objects[i].data != 0 && g.planets[planetRow].objects[i].data < oxygenTankMax+1){
					g.planets[planetRow].objects[i].data += 5;
					if(g.planets[planetRow].objects[i].data > oxygenTankMax)
						g.planets[planetRow].objects[i].data = oxygenTankMax;
				}
			}
		}

		//trees spawning (even if player is viewing galaxy or inventory)
		if(g.planets[planetRow].type == 0){ //if planet type is grass
			if(rnd(3) == 0){ //if a little bit of chance
				div = getDiv();
				temp = 0;
				for(short int i=0; i < div; i++){ //count how many trees on planet
					if(g.planets[planetRow].objects[i].id == 1) //tree
						temp++;
				}
				if(temp < 4){ //maximum natural growing : 4 trees
					temp = rnd(div-1);
					if(g.planets[planetRow].objects[(int)temp].id == 0){ //if nothing at #temp# place
						if(temp != 0){
							if(g.planets[planetRow].objects[(int)(temp-1)].id != 13){ //if not at the left of a fuel maker
								g.planets[planetRow].objects[(int)temp].id	 = 1; //spawn a tree
								g.planets[planetRow].objects[(int)temp].data = 0;
							}
						}else{
							if(g.planets[planetRow].objects[(int)(div-1)].id != 13){ //if not at the left of a fuel maker
								g.planets[planetRow].objects[(int)temp].id	 = 1; //spawn a tree
								g.planets[planetRow].objects[(int)temp].data = 0;
							}
						}
					}
				}
			}
		}

		//galaxy moving
		gAngle += gAngleStep;
		if(gAngle >= M_2PI)
			gAngle -= M_2PI;
		gAngleCnt = gAngleCntMax;
	}else
		gAngleCnt--;
}



//keyboard
void KeyboardMegaSwitch(){
	switch(menu){

		case 0: //start_menu
			switch(S2DE_key){

				case S2DE_KEY_P: //play
				case S2DE_KEY_p:
					menu = 2; //planet
				break;

				case S2DE_KEY_O: //go to start_settings
				case S2DE_KEY_o:
					//menu = 1;
				break;

				case S2DE_KEY_Q: //quit game
				case S2DE_KEY_q:
					S2DE_stop();
				break;
			}
		break;

		case 1: //start_settings
		break;

		case 2: //planet
			switch(S2DE_key){

				case S2DE_KEY_D:
				case S2DE_KEY_d:
					if(focus == 0){ //player_move
						if(pRunningCnt < 3){ //right move (from the top of the planet)
							pAngle += pAngleStep;
							if(pAngle > M_2PI)
								pAngle -= M_2PI;
							pRight = true;
							pRunningCnt = pRunningCntMax;
							if(pRunning == 0)
								pRunning = 1;
							setPlayerCoo();
						}
					}
				break;

				case S2DE_KEY_Q:
				case S2DE_KEY_q:
					if(focus == 0){ //player_move
						if(pRunningCnt < 3){ //left move (from the top of the planet)
							pAngle -= pAngleStep;
							if(pAngle < 0)
								pAngle += M_2PI;
							pRight = false;
							pRunningCnt = pRunningCntMax;
							if(pRunning == 0)
								pRunning = 1;
							setPlayerCoo();
						}
					}else if(focus == 1 || focus == 2) //put_down_item or options_panel1
						focus = 0; //player_move
					else if(focus == 3 || focus == 4) //options_panel2[crafts or equip]
						focus = 2; //options_panel1
				break;

				case S2DE_KEY_SPACE: //eat key
					if(inventory[0][hotbarSel].id == 16){ //if fruit in hand
						inventory[0][hotbarSel].id = 0;
						inventory[0][hotbarSel].data = 0;
						life += 5; //1/6 of lifeMax
						if(life > lifeMax)
							life = lifeMax;
					}
				break;

				case S2DE_KEY_1:
					hotbarSel = 0;
				break;

				case S2DE_KEY_2:
					hotbarSel = 1;
				break;

				case S2DE_KEY_3:
					hotbarSel = 2;
				break;

				case S2DE_KEY_4:
					hotbarSel = 3;
				break;

				case S2DE_KEY_5:
					hotbarSel = 4;
				break;
			}
		break;

		case 3: //inventory
			switch(S2DE_key){

				case S2DE_KEY_Q: //return to planet menu
				case S2DE_KEY_q:
					switch(focus){

						case 0: //no_selection
							menu = 2; //planet
							focus = 0; //player_move
						break;

						case 1: //item_selected
							focus = 0; //no_selection
						break;

						case 2: //message_delete
							focus = 0; //no_selection
						break;
					}
				break;
			}
		break;

		case 4: //space
			switch(S2DE_key){

				/* -------- Debug --------
				case S2DE_KEY_A: //increase rotation speed
				case S2DE_KEY_a:
					gAngle += gAngleStep;
				break;
				*/

				case S2DE_KEY_Q: //go back to actual planet
				case S2DE_KEY_q:
					if(focus == 1) //error_message
						focus = 0; //galaxy_view
					else{ //galaxy_view
						menu = 2; //planet
						focus = 0; //player_move
					}
				break;
			}
		break;

		case 6: //furniture
			switch(S2DE_key){

				case S2DE_KEY_Q: //return to planet menu
				case S2DE_KEY_q:
					switch(focus){

						case 0: //no_selection
							menu = 2; //planet
							focus = 0; //player_move
						break;

						case 1: //item_selected
							focus = 0; //no_selection
						break;

						case 2: //message_delete
							focus = 0; //no_selection
						break;
					}
				break;
			}
		break;
	}
}



//mouse click
void MouseClickMegaSwitch(){
	int x = S2DE_mouseX;
	int y = S2DE_mouseY;
	short int div;
	bool state;
	int temp1;
	switch(menu){

		case 0: //start_menu
			if(S2DE_mouseButton == S2DE_LEFT_BUTTON){
				if(inZone(x,y,580,300,1020,380)) //play button
					menu = 2; //planet
				//else if(inZone(x,y,620,150,980,200)) //settings button
				//	menu = 1; //start_settings
			}
		break;

		case 1: //start_settings
		break;

		case 2: //planet
			if(S2DE_mouseButton == S2DE_RIGHT_BUTTON){
				switch(focus){

					case 0: //player_move
						state = false;
						for(short int i=0; i < 5; i++){
							if(inZone(x,y,650+i*70,10,710+i*70,70)){
								hotbarSel = i;
								state = true;
								focus = 1; //put_down_item

								//just in case
								for(short int a=0; a < 16; a++)
									tStates[a] = false;
								break;
							}
						}
						if(!state){
							setP1Coo();
							focus = 2; //options_panel1
						}
					break;

					case 1: //put_down_object
						focus = 0; //player_move
					break;

					case 2: //options_panel1
						focus = 0; //player_move
					break;

					case 3: //options_panel2[crafts]
						focus = 2; //options_panel1
					break;

					case 4: //options_panel2[equip]
						focus = 2; //options_panel1
					break;
				}
			}else if(S2DE_mouseButton == S2DE_LEFT_BUTTON){
				for(short int i=0; i < 5; i++){
					if(inZone(x,y, 650+i*70,10,710+i*70,70)){
						hotbarSel = i;
						break;
					}
				}
				switch(focus){

					case 0: //player_moving
						//hit the ground analysis
						if(hitCnt == 0){
							div = getDiv();
							for(short int i=0; i < div; i++){ //if mouse in the i triangle
								if(inTriangle(x,y,800,450,
									tCoo[i][0][0],tCoo[i][0][1],
									tCoo[i][1][0],tCoo[i][1][1]
								)){
									if(g.planets[planetRow].objects[i].id == 1){ //if there is a tree at this place
										hitDiv = i; //hit the ground
										hitCnt = hitCntMax;
										temp1 = rnd(7);
										if(temp1 < 2)
											g.planets[planetRow].objects[i].id = 0; //nothing
										else if(temp1 < 3)
											g.planets[planetRow].objects[i].id = 3; //fiber
										else if(temp1 < 4)
											g.planets[planetRow].objects[i].id = 16; //fruit
										else
											g.planets[planetRow].objects[i].id = 2; //wood
										break;
									}else if(g.planets[planetRow].objects[i].id == 18){ //else if there is a furniture at this place
										stockSel = g.planets[planetRow].objects[i].data;
										menu = 6; //furniture
										focus = 0; //no_selection
									}else if(g.planets[planetRow].objects[i].id == 0){ //else if there is nothing at this place
										hitDiv = i; //hit the ground
										hitCnt = hitCntMax;
										if(inventory[0][hotbarSel].id == 6){ //if pickaxe in hand
											inventory[0][hotbarSel].data--; //use it
											if(inventory[0][hotbarSel].data == 0) //if out of use
												inventory[0][hotbarSel].id = 0; //break it
											temp1 = rnd(20);
											if(temp1 < 16)
												g.planets[planetRow].objects[i].id = 7; //rock
											else if(temp1 < 20)
												g.planets[planetRow].objects[i].id = 8; //iron
											else
												g.planets[planetRow].objects[i].id = 14; //fuel (very rare)
										}else{
											temp1 = rnd(10);
											if(temp1 < 5)
												g.planets[planetRow].objects[i].id = 0; //nothing
											else if(temp1 < 8)
												g.planets[planetRow].objects[i].id = 3; //fiber
											else
												g.planets[planetRow].objects[i].id = 7; //rock
										}
										break;
									}
								}
							}
						}
					break;

					case 1: //put_down_item
						div = getDiv();
						for(short int i=0; i < div; i++){
							if(tStates[i]){

								//put an object down
								if(g.planets[planetRow].objects[i].id == 13 //if object is fuel maker && we have fiber in hand && we are not already producing fuel
								&& inventory[0][hotbarSel].id == 3 && g.planets[planetRow].objects[i].data == 0){
									inventory[0][hotbarSel].id = 0; //remove the fiber from the hand
									inventory[0][hotbarSel].data = 0;
									g.planets[planetRow].objects[i].data = fuelMakerMax; //launch fuel production
								}else if(g.planets[planetRow].objects[i].id == 15 //if object is rocket && we have fuel in hand && we can put fuel in rocket
								&& inventory[0][hotbarSel].id == 14 && g.planets[planetRow].objects[i].data < rocketFuelMax){
									inventory[0][hotbarSel].id = 0; //remove the fuel from the hand
									inventory[0][hotbarSel].data = 0;
									g.planets[planetRow].objects[i].data++;
								}else if(g.planets[planetRow].objects[i].id == 17 //if object is oxygen generator and it is not active and we have oxygen tank in hand
								&& g.planets[planetRow].objects[i].data == 0 && inventory[0][hotbarSel].id == 11){
									g.planets[planetRow].objects[i].data = 1 + inventory[0][hotbarSel].data;
									inventory[0][hotbarSel].id = 0; //remove the oxygen tank from the hand
									inventory[0][hotbarSel].data = 0;
								}else if(g.planets[planetRow].objects[i].id == 0){ //if no item at the place
									if(inventory[0][hotbarSel].id != 13){ //if what we want to put down is not a fuel maker
										tryToPutDown(i,div);
									}else{
										if(i != div-1){
											if(g.planets[planetRow].objects[(int)(i+1)].id == 0) //if right side of object is free
												tryToPutDown(i,div);
										}else{
											if(g.planets[planetRow].objects[0].id == 0) //if right side of object is free
												tryToPutDown(i,div);
										}
									}

								//get an object from the ground
								}else if(inventory[0][hotbarSel].id == 0 && g.planets[planetRow].objects[i].id != 1){ //the planet object gotten cannot be a tree
									if(g.planets[planetRow].objects[i].id == 13){ //if object is a fuel maker
										if(g.planets[planetRow].objects[i].data == 0){ //if not producing

											//get the object in the hotbar
											inventory[0][hotbarSel].id = g.planets[planetRow].objects[i].id; //passing item from planet to hotbar
											inventory[0][hotbarSel].data = g.planets[planetRow].objects[i].data;
											g.planets[planetRow].objects[i].id = 0; //removing item from planet
											g.planets[planetRow].objects[i].data = 0;
										}
									}else if(g.planets[planetRow].objects[i].id == 17){ //if object is an oxygen generator
										if(g.planets[planetRow].objects[i].data == 0){ //if not active

											//get the object in the hotbar
											inventory[0][hotbarSel].id = g.planets[planetRow].objects[i].id; //passing item from planet to hotbar
											inventory[0][hotbarSel].data = g.planets[planetRow].objects[i].data;
											g.planets[planetRow].objects[i].id = 0; //removing item from planet
										}else{
											inventory[0][hotbarSel].id = 11; //give an oxygen tank
											inventory[0][hotbarSel].data = g.planets[planetRow].objects[i].data - 1; //with the quantity of oxygen in the generator
											g.planets[planetRow].objects[i].data = 0; //set generator not active
										}
									}else{
										//get the object in the hotbar
										inventory[0][hotbarSel].id = g.planets[planetRow].objects[i].id; //passing item from planet to hotbar
										inventory[0][hotbarSel].data = g.planets[planetRow].objects[i].data;
										g.planets[planetRow].objects[i].id = 0; //removing item from planet
										g.planets[planetRow].objects[i].data = 0;
									}
								}
								break;
							}
						}
					break;

					case 2: //options_panel1
						if(inZone(x,y,p1Coo[0]+10, p1Coo[1]+90,
									  p1Coo[0]+80, p1Coo[1]+160)){ //go to inventory
							menu = 3; //inventory
							focus = 0; //no_selection
						}else if(inZone(x,y,p1Coo[0]+90, p1Coo[1]+90, //show crafts
											p1Coo[0]+160,p1Coo[1]+160)){
							setP2Coo();
							focus = 3; //options_panel2[crafts]
						}else if(inZone(x,y,p1Coo[0]+10, p1Coo[1]+10,
											p1Coo[0]+80, p1Coo[1]+80)){ //show equipment
							setP2Coo();
							focus = 4; //options_panel2[equip]
						}else if(inZone(x,y,p1Coo[0]+90, p1Coo[1]+10,
											p1Coo[0]+160,p1Coo[1]+80)){ //go to space
							menu = 4; //space
							focus = 0; //galaxy_view
							planetSel = -1;
						}
					break;

					case 3: //options_panel2[crafts]
						if(inZone(x,y,p2Coo[0]+250,p2Coo[1]-65,
									  p2Coo[0]+330,p2Coo[1]-35)){ //first craft
							switch(craftPage){

								case 0:
									if(invCountItem(3) >= 3){ //at least 3 fiber
										delInInv(3); //fiber
										delInInv(3);
										delInInv(3);
										addInInv(4,0); //space suit
									}
								break;

								case 1:
									if(invCountItem(5) >= 1 && invCountItem(8) >= 1){ //at least 1 multi tool, 1 iron
										delInInv(5); //multi tool
										delInInv(8); //iron
										addInInv(9,0); //screwdriver
									}
								break;

								case 2:
									if(invCountItem(12) >= 1 && invCountItem(10) >= 1){ //at least 1 machine base, 1 hammer
										delInInv(12); //machine base
										delInInv(10); //hammer
										addInInv(13,0); //fuel maker
									}
								break;

								case 3:
									if(invCountItem(12) >= 1 && invCountItem(10) >= 2
									&& invCountItem(16) >= 1 && invCountItem(3) >= 3){ //at least 1 machine base, 2 hammer, 1 fruit, 3 fiber
										delInInv(10);	delInInv(10); //hammer
										delInInv(3);	delInInv(3); //fiber
										delInInv(3);
										delInInv(12); //machine base
										delInInv(16); //fruit
										addInInv(17,0); //oxygen generator
									}
								break;
							}
						}else if(inZone(x,y,p2Coo[0]+250,p2Coo[1]-160,
											p2Coo[0]+330,p2Coo[1]-130)){ //second craft
							switch(craftPage){

								case 0:
									if(invCountItem(2) >= 1 && invCountItem(3) >= 1){ //at least 1 wood, 1 fiber
										delInInv(2); //wood
										delInInv(3); //fiber
										addInInv(5,0); //multi tool
									}
								break;

								case 1:
									if(invCountItem(5) >= 1 && invCountItem(8) >= 2){ //at least 1 multi tool, 2 iron
										delInInv(5); //multi tool
										delInInv(8); //iron
										delInInv(8);
										addInInv(10,0); //hammer
									}
								break;

								case 2:
									if(invCountItem(12) >= 2 && invCountItem(2) >= 2
									&& invCountItem(9) >= 2 && invCountItem(8) >= 2){ //at least 2 machine base, 2 wood, 2 screwdriver, 2 iron
										delInInv(12);   delInInv(12); //machine base
										delInInv(2);	delInInv(2); //wood
										delInInv(9);	delInInv(9); //screwdriver
										delInInv(8);	delInInv(8); //iron
										addInInv(15,0); //rocket
									}
								break;

								case 3:
									if(craftedStocks < stockMax && invCountItem(12) >= 1 && invCountItem(9) >= 1){ //at least 4 iron, 1 screwdriver
										delInInv(8);	delInInv(8); //iron
										delInInv(8);	delInInv(8);
										delInInv(9); //screwdriver
										addInInv(18,craftedStocks); //furniture
										for(short int i=0; i < 5; i++){ //init furniture
											for(short int j=0; j < 4; j++){
												stock[craftedStocks][j][i].id = 0;
												stock[craftedStocks][j][i].data = 0;
											}
										}
										craftedStocks++;
									}
								break;
							}
						}else if(inZone(x,y,p2Coo[0]+250,p2Coo[1]-255,
											p2Coo[0]+330,p2Coo[1]-225)){ //third craft
							switch(craftPage){

								case 0:
									if(invCountItem(5) >= 1 && invCountItem(7) >= 2){ //at least 1 multi tool, 2 rock
										delInInv(5); //multi tool
										delInInv(7); //rock
										delInInv(7);
										addInInv(6,toolsDurability); //pickaxe
									}
								break;

								case 1:
									if(invCountItem(7) >= 2 && invCountItem(8) >= 2
									&& invCountItem(5) >=  1){ //at least 2 rock, 2 iron, 1 multi tool
										delInInv(7); //rock
										delInInv(7);
										delInInv(8); //iron
										delInInv(8);
										delInInv(5); //multi tool
										addInInv(12,0); //machine base
									}
								break;

								case 2:
									if(invCountItem(8) >= 3){ //at least 3 iron
										delInInv(8); //iron
										delInInv(8);
										delInInv(8);
										addInInv(11,oxygenTankMax); //oxygen tank
									}
								break;
							}
						}else if(inZone(x,y,p2Coo[0]+5,p2Coo[1]-310,
											p2Coo[0]+335,p2Coo[1]-290)){ //down arrow
							craftPage++;
							if(craftPage > craftPageMax)
								craftPage = 0;
						}
					break;

					case 4: //options_panel2[equip]
						if(inZone(x,y,p2Coo[0]+30,p2Coo[1]-140,p2Coo[0]+90,p2Coo[1]-80)){
							if(inventory[0][hotbarSel].id == 4 && equipment[0].id == 0){ //space suit
								equipment[0].id = 4;
								inventory[0][hotbarSel].id = 0; //remove it
							}else if(inventory[0][hotbarSel].id == 0 && equipment[0].id == 4){ //nothing
								inventory[0][hotbarSel].id = 4; //space suit
								equipment[0].id = 0;
							}
						}else if(inZone(x,y,p2Coo[0]+250,p2Coo[1]-80,p2Coo[0]+310,p2Coo[1]-20)){
							if(inventory[0][hotbarSel].id == 11 && equipment[1].id == 0){ //oxygen tank1
								equipment[1].id = 11;
								equipment[1].data = inventory[0][hotbarSel].data;
								inventory[0][hotbarSel].id = 0; //remove it
								inventory[0][hotbarSel].data = 0;
							}else if(inventory[0][hotbarSel].id == 0 && equipment[1].id == 11){ //nothing
								inventory[0][hotbarSel].id = 11; //oxygen tank
								inventory[0][hotbarSel].data = equipment[1].data;
								equipment[1].id = 0;
								equipment[1].data = 0;
							}
						}else if(inZone(x,y,p2Coo[0]+250,p2Coo[1]-190,p2Coo[0]+310,p2Coo[1]-130)){
							if(inventory[0][hotbarSel].id == 11 && equipment[2].id == 0){ //oxygen tank2
								equipment[2].id = 11;
								equipment[2].data = inventory[0][hotbarSel].data;
								inventory[0][hotbarSel].id = 0; //remove it
								inventory[0][hotbarSel].data = 0;
							}else if(inventory[0][hotbarSel].id == 0 && equipment[2].id == 11){ //nothing
								inventory[0][hotbarSel].id = 11; //oxygen tank
								inventory[0][hotbarSel].data = equipment[2].data;
								equipment[2].id = 0;
								equipment[2].data = 0;
							}
						}
					break;
				}
			}
		break;

		case 3: //inventory
			switch(focus){

				case 0: //no_selection
					if(S2DE_mouseButton == S2DE_LEFT_BUTTON || S2DE_mouseButton == S2DE_RIGHT_BUTTON){
						for(short int i=0; i < 5; i++){ //see what is selected
							for(short int j=0; j < 4; j++){
								if(inZone(x,y,630+i*70,315+j*70,
											  690+i*70,375+j*70)){
									xSel = i;
									ySel = j+1;
									focus = 1 + (int)(S2DE_mouseButton == S2DE_RIGHT_BUTTON); //item_selected if S2DE_LEFT_BUTTON or message_delete if S2DE_RIGHT_BUTTON
									break;
								}
							}
						}

						//don't forget the hotbar
						for(short int i=0; i < 5; i++){
							if(inZone(x,y,650+i*70,10,710+i*70,70)){
								xSel = i;
								ySel = 0;
								focus = 1 + (int)(S2DE_mouseButton == S2DE_RIGHT_BUTTON); //item_selected if S2DE_LEFT_BUTTON or message_delete if S2DE_RIGHT_BUTTON
								break;
							}
						}
						if(focus == 2 && inventory[ySel][xSel].id == 0) //cannot delete nothing
							focus = 0; //no_selection
					}
				break;

				case 2: //message_delete
					if(inventory[ySel][xSel].id != 18){ //furniture (can't be deleted)
						if(S2DE_mouseButton == S2DE_LEFT_BUTTON){
							if(inZone(x,y,680,370,780,430)){
								inventory[ySel][xSel].id = 0; //nothing
								inventory[ySel][xSel].data = 0;
								focus = 0; //no_selection
							}else if(inZone(x,y,820,370,920,430))
								focus = 0; //no_selection
						}else if(S2DE_mouseButton == S2DE_RIGHT_BUTTON)
							focus = 0; //no_selection
					}else
						focus = 0; //no_selection
				break;
			}
		break;

		case 4: //space
			if(focus == 0 && S2DE_mouseButton == S2DE_LEFT_BUTTON){ //galaxy_view
				if(planetSel != -1){
					temp1 = checkForRocket();
					if(temp1 != -1 && equipment[0].id == 4){ //<=> if a rocket with fuel exists on the planet && player equip a space suit

						//taking off from the actual planet
						rocketDistance = 0;
						rocketAnimCnt = 0;
						rocketAngle = temp1*M_2PI/getDiv();
						g.planets[planetRow].objects[temp1].data--;
						pRight = true;
						pRunning = 1;
						pRunningCnt = 5*pRunningCntMax;
						menu = 5; //rocket_animation
						focus = 0; //player_moving
					}else
						focus = 1; //error_message
				}
			}else if(focus == 1 //error_message
			&& ( (S2DE_mouseButton == S2DE_LEFT_BUTTON && inZone(x,y,740,360,880,420)) || S2DE_mouseButton == S2DE_RIGHT_BUTTON ) )
					focus = 0; //galaxy_view
		break;

		case 6: //furniture
			switch(focus){

				case 0: //no_selection
					if(S2DE_mouseButton == S2DE_LEFT_BUTTON || S2DE_mouseButton == S2DE_RIGHT_BUTTON){
						for(short int i=0; i < 5; i++){ //see what is selected
							for(short int j=0; j < 4; j++){
								if(inZone(x,y,630+i*70,315+j*70,
											  690+i*70,375+j*70)){
									xSel = i;
									ySel = j;
									fromInv = false;
									focus = 1 + (int)(S2DE_mouseButton == S2DE_RIGHT_BUTTON); //item_selected if S2DE_LEFT_BUTTON or message_delete if S2DE_RIGHT_BUTTON
									break;
								}
							}
						}

						//don't forget the hotbar
						for(short int i=0; i < 5; i++){
							if(inZone(x,y,650+i*70,10,710+i*70,70)){
								xSel = i;
								fromInv = true;
								focus = 1 + (int)(S2DE_mouseButton == S2DE_RIGHT_BUTTON); //item_selected if S2DE_LEFT_BUTTON or message_delete if S2DE_RIGHT_BUTTON
								break;
							}
						}
						if(focus == 2 && ((fromInv && inventory[0][xSel].id == 0) || (!fromInv && stock[stockSel][ySel][xSel].id == 0)) ) //cannot delete nothing
							focus = 0; //no_selection
					}
				break;

				case 2: //message_delete
					if(fromInv){
						if(inventory[0][xSel].id != 18){ //furniture (can't be deleted)
							if(S2DE_mouseButton == S2DE_LEFT_BUTTON){
								if(inZone(x,y,680,370,780,430)){
									inventory[0][xSel].id = 0; //nothing
									inventory[0][xSel].data = 0;
									focus = 0; //no_selection
								}else if(inZone(x,y,820,370,920,430))
									focus = 0; //no_selection
							}else if(S2DE_mouseButton == S2DE_RIGHT_BUTTON)
								focus = 0; //no_selection
						}else
							focus = 0; //no selection
					}else{
						if(stock[stockSel][ySel][xSel].id != 18){ //furniture (can't be deleted)
							if(S2DE_mouseButton == S2DE_LEFT_BUTTON){
								if(inZone(x,y,680,370,780,430)){
									stock[stockSel][ySel][xSel].id = 0; //nothing
									stock[stockSel][ySel][xSel].data = 0;
									focus = 0; //no_selection
								}else if(inZone(x,y,820,370,920,430))
									focus = 0; //no_selection
							}else if(S2DE_mouseButton == S2DE_RIGHT_BUTTON)
								focus = 0; //no_selection
						}else
							focus = 0; //no_selection
					}
				break;
			}
		break;
	}
}



//mouse release
void MouseReleaseMegaSwitch(){
	int x = S2DE_mouseX;
	int y = S2DE_mouseY;
	int temp1,temp2;
	switch(menu){

		case 3: //inventory

			//item_selected
			if(focus == 1 && S2DE_mouseButton == S2DE_LEFT_BUTTON){
				for(short int i=0; i < 5; i++){ //see what is selected
					for(short int j=0; j < 4; j++){
						if(inZone(x,y,630+i*70,315+j*70, //if mouse on a case
									  690+i*70,375+j*70)){
							temp1 = inventory[(int)(j+1)][i].id; //switching items
							temp2 = inventory[(int)(j+1)][i].data;
							inventory[(int)(j+1)][i].id = inventory[ySel][xSel].id;
							inventory[(int)(j+1)][i].data = inventory[ySel][xSel].data;
							inventory[ySel][xSel].id = temp1;
							inventory[ySel][xSel].data = temp2;
							focus = 0; //no_selection
							break;
						}
					}
				}

				//don't forget the hotbar
				for(short int i=0; i < 5; i++){
					if(inZone(x,y,650+i*70,10,710+i*70,70)){ //if mouse on a case
						temp1 = inventory[0][i].id;
						temp2 = inventory[0][i].data;
						inventory[0][i].id = inventory[ySel][xSel].id;
						inventory[0][i].data = inventory[ySel][xSel].data;
						inventory[ySel][xSel].id = temp1;
						inventory[ySel][xSel].data = temp2;
						focus = 0; //no_selection
						break;
					}
				}
				if(focus != 0) //<=> release click out of all void cases
					focus = 0;
			}

		break;

		case 6: //furniture

			//item selected
			if(focus == 1 && S2DE_mouseButton == S2DE_LEFT_BUTTON){
				for(short int i=0; i < 5; i++){ //see what is selected
					for(short int j=0; j < 4; j++){
						if(inZone(x,y,630+i*70,315+j*70, //if mouse on a case
									  690+i*70,375+j*70)){
							temp1 = stock[stockSel][j][i].id; //switching items
							temp2 = stock[stockSel][j][i].data;
							if(fromInv){
								stock[stockSel][j][i].id = inventory[0][xSel].id;
								stock[stockSel][j][i].data = inventory[0][xSel].data;
								inventory[0][xSel].id = temp1;
								inventory[0][xSel].data = temp2;
							}else{
								stock[stockSel][j][i].id = stock[stockSel][ySel][xSel].id;
								stock[stockSel][j][i].data = stock[stockSel][ySel][xSel].data;
								stock[stockSel][ySel][xSel].id = temp1;
								stock[stockSel][ySel][xSel].data = temp2;
							}
							focus = 0; //no_selection
							break;
						}
					}
				}

				//don't forget the hotbar
				for(short int i=0; i < 5; i++){
					if(inZone(x,y,650+i*70,10,710+i*70,70)){ //if mouse on a case
						temp1 = inventory[0][i].id;
						temp2 = inventory[0][i].data;
						if(fromInv){
							inventory[0][i].id = inventory[0][xSel].id;
							inventory[0][i].data = inventory[0][xSel].data;
							inventory[0][xSel].id = temp1;
							inventory[0][xSel].data = temp2;
						}else{
							inventory[0][i].id = stock[stockSel][ySel][xSel].id;
							inventory[0][i].data = stock[stockSel][ySel][xSel].data;
							stock[stockSel][ySel][xSel].id = temp1;
							stock[stockSel][ySel][xSel].data = temp2;
						}
						focus = 0; //no_selection
						break;
					}
				}
				if(focus != 0) //<=> release click out of all void cases
					focus = 0;
			}
		break;
	}
}



//mouse move
void MouseMoveMegaSwitch(){
	int x = S2DE_mouseX;
	int y = S2DE_mouseY;
	short int r, div;
	switch(menu){

		case 2: //planet
			if(focus == 1){ //put_down_item
				div = getDiv();
				for(short int i=0; i < div; i++)
					tStates[i] = inTriangle(x,y,800,450,
						tCoo[i][0][0],tCoo[i][0][1],
						tCoo[i][1][0],tCoo[i][1][1]
					);
			}
		break;

		case 4: //space
			if(focus == 0){ //galaxy_view
				planetSel = -1;
				for(short int p=0; p < g.nbrPlanets; p++){
					if(p != planetRow){
						r = g.planets[p].size*6+4;
						if(inZone(x,y, g.planets[p].x-r,g.planets[p].y-r, //square zone
									   g.planets[p].x+r,g.planets[p].y+r)) //to select planets
							planetSel = p;
					}
				}
			}
		break;
	}
}




















// ======================== EXECUTION ===========================

//manage all types of event
void S2DE_event(int event){
	switch(event){

		case S2DE_TIMER:
			if(starter)
				starterCnt++;
			else
				TimedUpdateMegaSwitch();
			S2DE_refresh();
		break;

		case S2DE_DISPLAY:
			if(starter){ //Icons interface
				if(starterCnt < 10){
					S2DE_setColor(0,0,0);
					S2DE_rectangle(0,0, 1600,900, 1);
				}else if(starterCnt < 30)
					ecrisImageIMG(686,325,yncrea);
				else if(starterCnt < 40){
					S2DE_setColor(0,0,0);
					S2DE_rectangle(0,0, 1600,900, 1);
				}else if(starterCnt < 60)
					ecrisImageIMG(438,350,ISEN);
				else if(starterCnt > 80)
					starter = false;
			}else
				DisplayMegaSwitch();
		break;

		case S2DE_KEYBOARD:
			if(S2DE_keyState == S2DE_KEY_RELEASED) //catch only "key presses" events
				break;

			if(!starter)
				KeyboardMegaSwitch();
		break;

		case S2DE_MOUSECLICK:
			if(!starter){
				if(S2DE_mouseState == S2DE_MOUSE_PRESSED)
					MouseClickMegaSwitch();
				else
					MouseReleaseMegaSwitch();
			}
		break;

		case S2DE_MOUSEMOVE:
			MouseMoveMegaSwitch();
		break;
	}
}

int main(int argc, char **argv){

	//init graphics
	S2DE_init(argc,argv, "Space Conquest [0.1.0]", 1600,900);
	S2DE_setTimer(25); //<=> 30fps
	setSprites();

	//init random
	srand(time(NULL));

	//set sun
	g.sun.x = 800; g.sun.y = 450; g.sun.r = 30;

	//set stars
	double radius, theta;
	g.nbrStars = 2+rnd(14);
	for(short int s=0; s < g.nbrStars; s++){

		//set position thanks to radius and angle (theta)
		radius = g.sun.r + rnd(365-g.sun.r);
		theta = (2*s + rnd(3)/2)*M_PI/g.nbrStars;
		g.stars[s].x = 800 + radius*cos(theta);
		g.stars[s].y = 450 + radius*sin(theta);
		g.stars[s].r = radius;
	}

	//set planets
	g.nbrPlanets = 1+rnd(9);
	for(short int p=0; p < g.nbrPlanets; p++){
		radius = g.sun.r + rnd(395-g.sun.r);
		theta = (2*p + rnd(3)/2)*M_PI/g.nbrPlanets;
		g.planets[p].x = 800 + radius*cos(theta); //square zone to spawn
		g.planets[p].y = 450 + radius*sin(theta);
		g.planets[p].size = 1+rnd(3);
		g.planets[p].atm = !(bool)rnd(20);
		g.planets[p].type = rnd(5)+(bool)rnd(80); //diamond type is rare

		//reset objects
		for(short int o=0; o < 16; o++){
			g.planets[p].objects[o].id = 0; //nothing
			g.planets[p].objects[o].data = 0;
		}

		//starter planet
		if(p == 0){
			g.planets[0].atm = true;
			g.planets[0].type = 0; //grass
			g.planets[0].size = 2;
			g.planets[0].objects[2].id = 1; //tree
			g.planets[0].objects[3].id = 1; //tree
			g.planets[0].objects[5].id = 1; //tree
			g.planets[0].objects[7].id = 1; //tree
		}
	}
	setPlanet(); //setPlayerCoo(), setTCoo() and setOCoo() included

	//show galaxy in console
	printf("%d stars generated\n",g.nbrStars);
	for(short int s=0; s < g.nbrStars; s++)
		printf("  - star %d : radius from sun %d, position (%d;%d)\n",s,
			g.stars[s].r, g.stars[s].x,g.stars[s].y
		);
	printf("%d planets generated\n",g.nbrPlanets);
	for(short int p=0; p < g.nbrPlanets; p++)
		printf("  - planet %d : size %d, type %d, atm %d, position (%d;%d)\n",p,
			g.planets[p].size, g.planets[p].type, (int)g.planets[p].atm, g.planets[p].x,g.planets[p].y
		);

	//set equipment
	equipment[0].id = 0; //nothing
	equipment[0].data = 0;
	equipment[1].id = 0; //nothing
	equipment[1].data = 0;
	equipment[2].id = 0; //nothing
	equipment[2].data = 0;

	//set inventory
	for(short int a=0; a < 5; a++){
		for(short int b=0; b < 5; b++){
			inventory[a][b].id = 0; //nothing
			inventory[a][b].data = 0;
		}
	}
	/*
	inventory[0][0].id = 1; //tree
	inventory[0][1].id = 2; //wood
	inventory[0][2].id = 3; //fiber
	inventory[0][3].id = 4; //space suit
	inventory[0][4].id = 5; //multi tool
	inventory[1][0].id = 6; //pickaxe
	inventory[1][0].data = toolsDurability;
	inventory[1][1].id = 7; //rock
	inventory[1][2].id = 8; //iron
	inventory[1][3].id = 9; //screwdriver
	inventory[1][4].id = 10; //hammer
	inventory[2][0].id = 11; //oxygen tank
	inventory[2][0].data = oxygenTankMax;
	inventory[2][1].id = 12; //machine base
	inventory[2][2].id = 13; //fuel maker
	inventory[2][3].id = 14; //fuel
	inventory[2][4].id = 15; //rocket
	inventory[3][0].id = 16; //fruit
	inventory[3][1].id = 17; //oxygen generator
	inventory[3][2].id = 18; //furniture
	*/

	//launch event loop
	S2DE_start();

	return EXIT_SUCCESS;
}
