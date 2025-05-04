#ifndef PLAYER_H
#define PLAYER_H

#include "typedefs.h"
#include "loader.h"
#include "renderer.h"

//////////////
void PlayerInit(float x,float y, float angle);
void PlayerEnd();
void PlayerUpdate();
void MovePlayer(Map2D* m);

void DrawHand();
void PlayerDisplay();

#endif
