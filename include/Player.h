#ifndef PLAYER_H
#define PLAYER_H

#include "typedefs.h"

void PlayerInit(float x,float y, float angle);
void PlayerUpdate();
void MovePlayer(Map2D* m);


#endif
