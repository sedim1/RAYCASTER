#ifndef RENDERER_H
#define RENDERER_H

#include "typedefs.h"
#include <SDL3/SDL.h>

//Funciones principales de dibujo
void pixel(int x,int y, RGB* color);
void pixelDDA(int x,int y, RGB* color);
void DDA(int x1,int y1,int x2,int y2,RGB* color);

//Rendering functions of player
void DrawMap2D(Map2D* m);
void DrawPlayer();
void DrawPlayerView(Map2D* m);
void castRaysDDA(Map2D* m);

//Rendering functions of sprites
void DrawSprite2D(Sprite2D* sprite);

#endif
