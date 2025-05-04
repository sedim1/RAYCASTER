#include "Player.h"
#include "renderer.h"

Player player;
extern SDL_Window* window;
extern Keyboard key;
extern Mouse mouse;
extern float deltaTime;
extern Map2D map;
extern float MAX_FOG_DIST;
extern float ambient;
float pSpeed = 150.0f;
float rotSpeed = 50.0f;
bool motion = false; float mouseSensitivity = 1.0f;
bool isWalking = false; float t = 0.0f, tZ = 0.0f;

void PlayerInit(float x,float y, float angle){
	player.position.x = x; player.position.y = y; player.a = normalizeAngle(angle);
	player.dx = cos(degToRad(player.a)); player.dy = sin(degToRad(player.a));
	player.planeX = -player.dy * FOV_FACTOR; player.planeY = player.dx * FOV_FACTOR;
	player.l = 0;
	//SDL_SetMouseRelativeMode(true);
	SDL_SetWindowRelativeMouseMode(window,true);
	SDL_HideCursor();
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
	isWalking = false;
	int mapX = (int)(player.position.x/m->mapS); int mapY = (int)(player.position.y/m->mapS);
	int xo = (player.dx < 0) ? -20 : 20 ; int yo = (player.dy < 0) ? -20 : 20 ;
	int addX = (int)((player.position.x+xo)/m->mapS); int addY = (int)((player.position.y+yo)/m->mapS);
	int subX = (int)((player.position.x-xo)/m->mapS); int subY = (int)((player.position.y-yo)/m->mapS);
	int sladdX = (int)((player.position.x-yo)/m->mapS);  int sladdY = (int)((player.position.y+xo)/m->mapS);
	int slsubX = (int)((player.position.x+yo)/m->mapS);  int slsubY = (int)((player.position.y-xo)/m->mapS);
	if(key.shift == 1) { pSpeed = 160;} else {pSpeed = 100.0f;}
	if((key.w == 1||key.s == 1) && (key.a==1||key.d==1)) {pSpeed=pSpeed * 0.7f;}
	//Moving normally
	if(key.w == 1){ 
		if(m->walls[mapY][addX] < 1){ player.position.x += player.dx * pSpeed * deltaTime;}
		if(m->walls[addY][mapX] < 1){ player.position.y += player.dy * pSpeed * deltaTime;}
		isWalking = true;
	}
	if(key.s == 1){ 
		if(m->walls[mapY][subX] < 1){ player.position.x -= player.dx * pSpeed * deltaTime;}
		if(m->walls[subY][mapX] < 1){ player.position.y -= player.dy * pSpeed * deltaTime;}
		isWalking = true;
	}
	//strafing
	if(key.d == 1){
                if(m->walls[mapY][sladdX] < 1){ (player.position.x -= player.dy * pSpeed * deltaTime);}
                if(m->walls[sladdY][mapX] < 1){ (player.position.y += player.dx * pSpeed * deltaTime);}
		isWalking = true;
        }
        if(key.a == 1){ 
        	if(m->walls[mapY][slsubX] < 1){ player.position.x += (player.dy * pSpeed * deltaTime);}
        	if(m->walls[slsubY][mapX] < 1){ player.position.y -= (player.dx * pSpeed * deltaTime);}
		isWalking = true;
        }

	//Camera motion
	if(isWalking){
		tZ = 0.0f;
		float frequency = (key.shift == 1) ? 1299.0f : 500.0f;
		float amplitude = (key.shift == 1) ? 3.0f : 6.0f;
		player.z = (amplitude * sin(degToRad(t)));
		t += frequency * deltaTime; t = normalizeAngle(t);
	}
	else{
		t = 0.0f;
		player.z = lerpF(player.z,0.0f,tZ);
		if(tZ <= 1.0f)
			tZ += 1.0f * deltaTime * 10.0f;
		else
			tZ = 1.0f;
	}
}
