#include "Player.h"
#include "renderer.h"

Player player;
extern SDL_Window* window;
extern Keyboard key;
extern Mouse mouse;
extern float deltaTime;
extern Map2D map;
float pSpeed = 350.0f;
float rotSpeed = 130.0f;
bool motion = false; float mouseSensitivity = 1.0f;


void PlayerInit(float x,float y, float angle){
	player.position.x = x; player.position.y = y; player.a = normalizeAngle(angle);
	player.dx = cos(degToRad(player.a)); player.dy = sin(degToRad(player.a));
	player.planeX = -player.dy * FOV_FACTOR; player.planeY = player.dx * FOV_FACTOR;
	player.l = 0;
	SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);
}
void PlayerUpdate(){ //Funcion que se llamara cada frame y llevara a cabo la logica del jugador
	MovePlayer(&map);
}

void MovePlayer(Map2D* m){
	if(mouse.motion == true){
        	float rotH = -mouse.deltaX * rotSpeed * mouseSensitivity * deltaTime; 
        	float rotY = -mouse.deltaY * rotSpeed * mouseSensitivity * deltaTime;
		player.l+= rotY; 
		if(player.l > 160.5f){ player.l = 160.5f;}
		if(player.l < -160.5f){ player.l = -160.5f;}
        	player.a = normalizeAngle(player.a - rotH);
        	player.dx = cos(degToRad(player.a)); 
        	player.dy = sin(degToRad(player.a));
        	player.planeX = -player.dy * FOV_FACTOR;
        	player.planeY = player.dx * FOV_FACTOR;
		SDL_WarpMouseInWindow(window, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
        	mouse.motion = false;
	}
	int mapX = (int)(player.position.x/m->mapS); int mapY = (int)(player.position.y/m->mapS);
	int xo = (player.dx < 0) ? -20 : 20 ; int yo = (player.dy < 0) ? -20 : 20 ;
	int addX = (int)((player.position.x+xo)/m->mapS); int addY = (int)((player.position.y+yo)/m->mapS);
	int subX = (int)((player.position.x-xo)/m->mapS); int subY = (int)((player.position.y-yo)/m->mapS);
	float n = (key.w != 1 || key.s != 1) ? 1.0f : 0.5f;
	if(key.shift == 1) { pSpeed = 350;} else {pSpeed = 250.0f;}
	if(key.w == 1){ 
		if(m->walls[mapY][addX] < 1){ player.position.x += player.dx * pSpeed * deltaTime;}
		if(m->walls[addY][mapX] < 1){ player.position.y += player.dy * pSpeed * deltaTime;}
	}
	if(key.s == 1){ 
		if(m->walls[mapY][subX] < 1){ player.position.x -= player.dx * pSpeed * deltaTime;}
		if(m->walls[subY][mapX] < 1){ player.position.y -= player.dy * pSpeed * deltaTime;}
	}
	 addX = (int)((player.position.x-yo)/m->mapS);  addY = (int)((player.position.y+xo)/m->mapS);
	 subX = (int)((player.position.x+yo)/m->mapS);  subY = (int)((player.position.y-xo)/m->mapS);
	 if(key.d == 1){
                if(m->walls[mapY][addX] < 1){ (player.position.x -= player.dy * pSpeed*n * deltaTime);}
                if(m->walls[addY][mapX] < 1){ (player.position.y += player.dx * pSpeed*n * deltaTime);}
        }
        if(key.a == 1){ 
        	if(m->walls[mapY][subX] < 1){ player.position.x += (player.dy * pSpeed*n * deltaTime);}
        	if(m->walls[subY][mapX] < 1){ player.position.y -= (player.dx * pSpeed*n * deltaTime);}
        }
}
