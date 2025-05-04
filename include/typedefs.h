#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define RESOLUTION 1
#define SW 254 * RESOLUTION //Pixeles en el ancho de pantalla logica
#define SH 190 * RESOLUTION //Pixeles en altura de pantalla logica
#define PIXELSCALE 4/RESOLUTION
#define SCREEN_WIDTH (SW*PIXELSCALE) //Ajustar ancho de ventana en base al tamaño del pixel
#define SCREEN_HEIGHT (SH*PIXELSCALE) //Ajustar altura de ventana en base al tamaño del pixel

#define M_PI 3.14159265358979323846
#define MAX 300

//#define FOV (60.0f)
//#define FOV_FACTOR (tan(degToRad(FOV) / 2.0f))
#define DOF 20
#define ANGLE_STEP (FOV/SCREEN_WIDTH) * PIXELSCALE
#define distFromProjectionPlane ((SW/2)/tan((FOV/2)*(M_PI/180.f)))

typedef struct{
	int w,a,s,d,left,right,shift;
}Keyboard;

//Useful for cell Pos
typedef struct{
	int x;
	int y;
}VECTOR2I;

typedef struct{
	float x,y;
}VECTOR2;

typedef struct{
	int posX;
	int posY;
	int deltaX;
	int deltaY;
	bool motion;
}Mouse;

typedef struct{
	int r,g,b;
}RGB;

typedef struct{
	int* buffer;
	int texWidth; //Anco actual de la textura
	int texHeight; //aLTURA ACTUAL DE LA TEXTURA
}TEXMAP;

typedef struct{
	int mapWidth, mapHeight, mapS;
	int** walls;
	int** floor; 
	int** ceiling;
	TEXMAP wallTextures;
	TEXMAP floorTextures;
	TEXMAP ceilingTextures;
}Map2D;

enum SpriteType{
	STATIC, //Sprite is single image
	ANIMATED, //Sprite is animated
};

typedef struct{
	enum SpriteType type;
	bool on;
	VECTOR2 position;
	float z;
	float a;
	int mapVal; //This will be used for a billboard (8 sprites per image)
	TEXMAP texture;
}Sprite2D;

typedef struct{
	VECTOR2 position; //Posicion
	float z; //Height
	float a; //Angle
	float dx,dy;
	float planeX, planeY; //camera 
	int l; //Loook up and down
}Player;


float degToRad(float degrees);
float normalizeAngle(float angle);
float lerpF(float a, float b,float t);


float distance(float x1,float y1,float x2,float y2);
void normalize(float* x,float *y);
float length(float x, float y);

VECTOR2I CartesianToCellCords(VECTOR2* A,int unit);
VECTOR2 CellCordToCartesian(VECTOR2I* A,int unit);

#endif
