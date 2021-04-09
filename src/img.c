// ---------------- IMPORTATIONS ----------------
#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include <glut.h>
#endif




















// ---------------- DEFINITIONS ----------------
static const double Pi2 = 2*M_PI;
struct IMG{
	int width,height;
	int* data;
};
typedef struct IMG img;




















// ---------------- UTILITIES ----------------
void circle(float centreX, float centreY, float R, bool complete, bool full){ //complete ? Pi2 : M_PI
	unsigned short int div = 20; //Accuracy (division segments)
	float course;
	if(complete)
		course = Pi2;
	else
		course = M_PI;
	const double aDiv = course/div; //angle division
	double angle;
	if(full){
		for(unsigned short int i = 0; i < div; i++){
			angle = i*course/div; //actual angle
			S2DE_triangle(centreX,centreY,
					 centreX + R*cos(angle),      centreY + R*sin(angle),
					 centreX + R*cos(angle+aDiv), centreY + R*sin(angle+aDiv),
			true);
		}
	}else{
		for(unsigned short int i = 0; i < div; i++){
			angle = course*i/div; //actual angle
			S2DE_line(centreX + R*cos(angle),      centreY + R*sin(angle),
				  centreX + R*cos(angle+aDiv), centreY + R*sin(angle+aDiv)
			);
		}
	}
}

void showNumber(int nbr, float taille, float x, float y){
	char c[10];
	sprintf(c,"(%d/100)",nbr);
	S2DE_text(c,taille/120.f,x,y);
}




















// ---------------- IMAGES -----------------
int *BVRA2ARVB(int largeur, int hauteur, const unsigned char *donnees){
	const unsigned char *ptrDonnees;
	unsigned char *pixels = (unsigned char*)malloc(largeur*hauteur*sizeof(int));
	unsigned char *ptrPixel;
	int index;

	ptrPixel = pixels;
	ptrDonnees = donnees;
	for(index = largeur*hauteur; index != 0; --index){
		ptrPixel[0] = ptrDonnees[0];
		ptrPixel[1] = ptrDonnees[1];
		ptrPixel[2] = ptrDonnees[2];
		ptrPixel[3] = ptrDonnees[3];
		ptrDonnees += 4;
		ptrPixel += 4;
	}
	return (int*)pixels;
}

int charToInt(char c){
	switch(c){
		case '1':
			return 1;
		case '2':
			return 2;
		case '3':
			return 3;
		case '4':
			return 4;
		case '5':
			return 5;
		case '6':
			return 6;
		case '7':
			return 7;
		case '8':
			return 8;
		case '9':
			return 9;
	}
	return 0;
}

//read .rgba file and convert it to an ARVB
img readRGBA2ARVB(char name[100], img image){
	char data[(int)(3*4*image.width*image.height)];
	FILE* file;
	file = fopen(name,"r");
	int d = 0;
	char c = '_';
	if(file != NULL){
		while( (c = fgetc(file)) != EOF )
			data[d++] = c;
	}
	fclose(file);

	unsigned char* intData = (unsigned char*)malloc(4*image.width*image.height*sizeof(unsigned char));
	int a=0;
	for(int h=image.height-1; h >= 0; h--){
		for(int i=0; i < 4*image.width; i++){
			intData[a] = (unsigned char)(
				100*charToInt(data[ (int)(h*12*image.width+3*i  ) ]) +
				 10*charToInt(data[ (int)(h*12*image.width+3*i+1) ]) +
					charToInt(data[ (int)(h*12*image.width+3*i+2) ])
			);
			a++;
		}
	}
	image.data = BVRA2ARVB(image.width, image.height, intData);
	return image;
}

void ecrisImageIMG(int x, int y, img image){ //(x,y) will be the left bottom corner of the image
	if(image.data == NULL)
		return;

	glRasterPos2i(x,y);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glDrawPixels(image.width, image.height, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, image.data);
}

void ecrisImageRVBA(int x, int y, int largeur, int hauteur, const int *donnees){
	glRasterPos2i(x, y);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glDrawPixels(largeur, hauteur, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8_REV, donnees);
}

int min4i(int l0, int l1, int l2, int l3){
	int m = l0;
	m = (l1 < m) ? l1 : m; //3 conditions are better than a for()
	m = (l2 < m) ? l2 : m;
	m = (l3 < m) ? l3 : m;
	return m;
}
int max4i(int l0, int l1, int l2, int l3){
	int m = l0;
	m = (l1 > m) ? l1 : m; //3 conditions are better than a for()
	m = (l2 > m) ? l2 : m;
	m = (l3 > m) ? l3 : m;
	return m;
}

void writeRotatedImage(int posX, int posY, img image, float angle, bool antialiasing){ //position from the center of the image
	if(image.data == NULL)
		return;

	//corner's rotations
	float cosA = cos(angle);
	float sinA = -sin(angle);
	int halfW = image.width/2;
	int halfH = image.height/2;
	int p[4][2] = {
		{-halfW,-halfH}, {-halfW, halfH},
		{ halfW,-halfH}, { halfW, halfH}
	};
	int tempP[4][2] = {
		{-halfW,-halfH}, {-halfW, halfH},
		{ halfW,-halfH}, { halfW, halfH}
	};
	for(short int i=0; i < 4; i++){
		p[i][0] = (int)(tempP[i][0]*cosA - tempP[i][1]*sinA);
		p[i][1] = (int)(tempP[i][0]*sinA + tempP[i][1]*cosA);
	}

	//get needed buffer dimensions
	int mx,my, Mx,My;
	mx = min4i(p[0][0],p[1][0],p[2][0],p[3][0]); my = min4i(p[0][1],p[1][1],p[2][1],p[3][1]);
	Mx = max4i(p[0][0],p[1][0],p[2][0],p[3][0]); My = max4i(p[0][1],p[1][1],p[2][1],p[3][1]);
	int newImageW = Mx - mx; //Width & Height
	int newImageH = My - my;
	//printf("angle : %f, newImageW : %d, newImageH : %d\n",angle,newImageW,newImageH);

	//starting to create new image
	int* newImageData = (int*)malloc(newImageW*newImageH*sizeof(int));

	//set new image data to void (memory allocation)
	for(int y=0; y < newImageH; y++){
		for(int x=0; x < newImageW; x++){
			newImageData[(int)(newImageW*y + x)] = 0;
		}
	}

	//fill new image thanks to rotated pixels in image
	int halfNIW = newImageW/2;
	int halfNIH = newImageH/2;
	int newX,newY, tempX,tempY, imageRow,newImageRow;
	for(int y=0; y < image.height; y++){
		for(int x=0; x < image.width; x++){
			imageRow = (int)(image.width*y + x);

			//centralize to rotate from the center
			tempX = x-halfW;
			tempY = y-halfH;

			//Rotate + uncentralize
			newX = (int)(halfNIW + tempX*cosA - tempY*sinA);
			newY = (int)(halfNIH + tempX*sinA + tempY*cosA);

			//writing at the good pos
			newImageRow = (int)(newImageW*newY + newX);
			if(newImageRow >= 0 && newImageRow < newImageW*newImageH) //not necessary : security
				newImageData[newImageRow] = image.data[imageRow];
		}
	}
	if(antialiasing){
		for(int y=1; y < newImageH-1; y++){
			for(int x=1; x < newImageW-1; x++){
				newImageRow = (int)(newImageW*y + x);
				if(newImageData[newImageRow] == 0){ //if void pixel
					if(newImageData[(int)(newImageRow-1)] != 0 && newImageData[(int)(newImageRow+1)] != 0) //if lonely pixel
						newImageData[newImageRow] = newImageData[(int)(newImageRow+1)]; //take the value of the next
				}
			}
		}
	}
	ecrisImageRVBA(posX-halfNIW,posY-halfNIH,newImageW,newImageH, newImageData);
	free(newImageData);
}
