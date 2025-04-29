#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#define RESOLUTION 1
#define SW 256 //Pixeles en el ancho de pantalla logica
#define SH 164 //Pixeles en altura de pantalla logica
#define PIXELSCALE 4/RESOLUTION
#define SCREEN_WIDTH (SW*PIXELSCALE) //Ajustar ancho de ventana en base al tamaño del pixel
#define SCREEN_HEIGHT (SH*PIXELSCALE) //Ajustar altura de ventana en base al tamaño del pixel

#define M_PI 3.14159265358979323846
#define MAX 300

#define FOV (90.0f)
#define FOV_FACTOR (tan(degToRad(FOV) / 2.0f))
#define DOF 20
#define ANGLE_STEP (FOV/SCREEN_WIDTH) * PIXELSCALE
#define distFromProjectionPlane ((SW/2)/tan((FOV/2)*(M_PI/180.f)))

typedef struct{
	int w,a,s,d,left,right,shift;
}Keyboard;

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
	int** walls, floors, ceiling;
	TEXMAP wallTextures;
}Map2D;

typedef struct{
	float x,y; //Posicion
	float a; //Angle
	float dx,dy;
	float planeX, planeY; //camera 
}Player;

float degToRad(float degrees);
float normalizeAngle(float angle);
float distance(float x1,float y1,float x2,float y2);
void normalize(float* x,float *y);
float length(float x, float y);
#endif
