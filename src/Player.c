#include "Player.h"

Player player;
extern Keyboard key;
extern float deltaTime;
extern Map2D map;
float pSpeed = 350.0f;
float rotSpeed = 170.0f;

void PlayerInit(float x,float y, float angle){
	player.x = x; player.y = y; player.a = normalizeAngle(angle);
	player.dx = cos(degToRad(player.a)); player.dy = sin(degToRad(player.a));
	player.planeX = -player.dy * FOV_FACTOR; player.planeY = player.dx * FOV_FACTOR;
}
void PlayerUpdate(){ //Funcion que se llamara cada frame y llevara a cabo la logica del jugador
	MovePlayer(&map);
}

void MovePlayer(Map2D* m){
	if(key.left == 1){ 
		player.a -= rotSpeed * deltaTime; player.a = normalizeAngle(player.a);  
		player.dx = cos(degToRad(player.a)); player.dy = sin(degToRad(player.a));
		player.planeX = -player.dy  * FOV_FACTOR; player.planeY = player.dx  * FOV_FACTOR;
	}
	if(key.right == 1){ 
		player.a += rotSpeed * deltaTime; player.a = normalizeAngle(player.a);  
		player.dx = cos(degToRad(player.a)); player.dy = sin(degToRad(player.a));
		player.planeX = -player.dy  * FOV_FACTOR ; player.planeY = player.dx  * FOV_FACTOR ;
	}
	int mapX = (int)(player.x/m->mapS); int mapY = (int)(player.y/m->mapS);
	int xo = (player.dx < 0) ? -20 : 20 ; int yo = (player.dy < 0) ? -20 : 20 ;
	int addX = (int)((player.x+xo)/m->mapS); int addY = (int)((player.y+yo)/m->mapS);
	int subX = (int)((player.x-xo)/m->mapS); int subY = (int)((player.y-yo)/m->mapS);
	if(key.shift == 1) { pSpeed = 350 * 2;} else {pSpeed = 350.0f;}
	if(key.w == 1){ 
		if(m->walls[mapY][addX] < 1){ player.x += player.dx * pSpeed * deltaTime;}
		if(m->walls[addY][mapX] < 1){ player.y += player.dy * pSpeed * deltaTime;}
	}
	if(key.s == 1){ 
		if(m->walls[mapY][subX] < 1){ player.x -= player.dx * pSpeed * deltaTime;}
		if(m->walls[subY][mapX] < 1){ player.y -= player.dy * pSpeed * deltaTime;}
	}

	/*if(key.d == 1){ 
		if(m->map[mapY][addX] < 1){ player.x -= player.dy * pSpeed * deltaTime;}
		if(m->map[addY][mapX] < 1){ player.y += player.dx * pSpeed * deltaTime;}
	}
	if(key.a == 1){ 
		if(m->map[mapY][subX] < 1){ player.x += player.dy * pSpeed * deltaTime;}
		if(m->map[subY][mapX] < 1){ player.y -= player.dx * pSpeed * deltaTime;}
	}*/

}
