///SDL TEMPLATE PROGRAM
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include "typedefs.h"
#include "renderer.h"
#include "loader.h"
#include "Player.h"


bool Init();
void Update();
void End();
void Display();
void HandleEvents();

//window to render
extern SDL_Window* window;
//The surface contained by the window
extern SDL_Renderer* renderer;
extern Keyboard key;
bool isRunning = true;

extern Map2D map;
extern Player player;
extern Mouse mouse;
extern float deltaTime;
Sprite2D sprite;
int fps = 0;

int main(int argc, char* argv[]){
	if(!Init())
		return -1;
	Update();
	End();
	return 0;
}

bool Init(){
	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() ); return false;
	}

	//Create window
        window = SDL_CreateWindow("Raycaster",SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE );
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() ); return false;
        }
	//Get window surface
        renderer = SDL_CreateRenderer(window,NULL);
        if( renderer == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() ); return false;
        }
	PlayerInit(200.0f,200.0f,0.5f);
	loadMap(&map,"Levels/map1.txt");
	loadSprite(&sprite,STATIC,400.0,200.0,0.0,true,0,"Sprites/Sprite2.ppm");
	return true;
}


void Update(){
	float lastTime = 0.0f, current = 0.0f;
	float lastFrameTime = 0.0f;
	while(isRunning){
		current = SDL_GetTicks()/1000.0f;
		deltaTime = current - lastTime;
		lastTime = current;
		SDL_PumpEvents(); 
		HandleEvents();
		//Game logic goes here
		PlayerUpdate();
		//Start rendering after processing the game logic
		Display();
		if(current - lastFrameTime >= 1.0f){
			printf("FPS: %d\n",fps);
			fps = 0;
			lastFrameTime = current;
		}
		else { fps++;}
	}
}

void Display(){
	SDL_SetRenderDrawColor(renderer,0,0,0,255); //Clear screen color
	SDL_RenderClear(renderer); //Clear 
	//Rendering logic goes here
	castRaysDDA(&map);
	DrawSprite2D(&sprite);
	SDL_RenderPresent(renderer); //Render the final image final image
}

void End(){
	//SDL_SetRelativeMouseMode(SDL_FALSE); 
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	freeMap(&map);
	freeSprite(&sprite);
	printf("Exiting\n");
}

void HandleEvents(){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
	switch(e.type){
		case SDL_EVENT_QUIT:
			isRunning = false;
			break;
		case (SDL_EVENT_MOUSE_MOTION):
			mouse.deltaX = e.motion.xrel;
			mouse.deltaY = e.motion.yrel;
			mouse.motion = true;
			break;
		case SDL_EVENT_KEY_DOWN:
			switch( e.key.key ){
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				case SDLK_W:
					key.w = 1;
					break;
				case SDLK_S:
					key.s = 1;
					break;
				case SDLK_A:
					key.a = 1;
					break;
				case SDLK_D:
					key.d = 1;
					break;
				case SDLK_LEFT:
					key.left = 1;
					break;
				case SDLK_RIGHT:
					key.right = 1;
					break;
				case SDLK_LSHIFT:
					key.shift = 1;
					break;
			}
			break;
		case SDL_EVENT_KEY_UP:
			switch( e.key.key ){
				case SDLK_W:
					key.w = 0;
					break;
				case SDLK_S:
					key.s = 0;
					break;
				case SDLK_A:
					key.a = 0;
					break;
				case SDLK_D:
					key.d = 0;
					break;
				case SDLK_LEFT:
					key.left = 0;
					break;
				case SDLK_RIGHT:
					key.right = 0;
					break;
				case SDLK_LSHIFT:
					key.shift = 0;
					break;

			}
			break;
		default:
			break;
	}
	}
}
