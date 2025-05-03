#include "typedefs.h"

Map2D map;

Keyboard key;

Mouse mouse;

float deltaTime = 0.0f;

float degToRad(float degrees){
	return ((M_PI/180.0f) * degrees);
}

float normalizeAngle(float angle){
	float x = angle;
	if(angle >= 360.0f)
		x = angle - 360.0f;
	else if(angle < 0.0f)
		x = angle + 360.0f;
	return x;
}

float distance(float x1,float y1,float x2,float y2){
	float dx = x2 -x1;
	float dy = y2 -y1;
	return sqrt((dx*dx)+(dy*dy));
}

float length(float x, float y){
	float a = (x), b = (y);
	return sqrt(a*a + b*b);
}

void normalize(float* x,float *y){
	float a = *x, b = *y;
	float l = length(a,b);
	(*x) = (*x)/l; (*y) = (*y)/l;
}

//----------------------------------
VECTOR2I CartesianToCellCords(VECTOR2* A,int unit){
	return (VECTOR2I){ (int)(A->x / unit), (int)(A->y / unit) };
}
VECTOR2 CellCordToCartesian(VECTOR2I* A,int unit){
	return (VECTOR2){(float)A->x*unit + unit/2.0f,(float)A->y*unit + unit/2.0f};
}
