#ifndef LOADER_H
#define LOADER_H

#include"typedefs.h"

//Loading maps
void loadMap(Map2D* map,char* path);
void freeMap(Map2D* map);

//Loding textures
TEXMAP* createTexture();
void loadTexture(TEXMAP* map,char* path);
void freeTexture(TEXMAP* map);

#endif
