///SDL TEMPLATE PROGRAM
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
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
SDL_Window* window = NULL;
//The surface contained by the window
extern SDL_Renderer* renderer;
extern Keyboard key;
bool isRunning = true;

extern Map2D map;
extern Player player;
extern float deltaTime;

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
        window = SDL_CreateWindow("Raycaster",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			SCREEN_WIDTH, SCREEN_HEIGHT, 0);
        if( window == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() ); return false;
        }
	//Get window surface
        renderer = SDL_CreateRenderer(window,-1,0);
        if( renderer == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() ); return false;
        }
	PlayerInit(300.0f,300.0f,0.5f);
	loadMap(&map,"Levels/map1.txt");
	return true;
}


void Update(){
	float lastTime = 0.0f, current = 0.0f;
	while(isRunning){
		current = SDL_GetTicks()/1000.0f;
		deltaTime = current - lastTime;
		lastTime = current;
		HandleEvents();
		//Game logic goes here
		PlayerUpdate();
		//Start rendering after processing the game logic
		Display();
	}
}

void Display(){
	SDL_SetRenderDrawColor(renderer,0,0,0,255); //Clear screen color
	SDL_RenderClear(renderer); //Clear 
	//Rendering logic goes here
	castRaysDDA(&map);
	DrawMap2D(&map);
	DrawPlayer();
	SDL_RenderPresent(renderer); //Render the final image final image
}

void End(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	freeMap(&map);
	printf("Exiting\n");
}

void HandleEvents(){
	SDL_Event e;
	SDL_PollEvent(&e);
	switch(e.type){
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			switch( e.key.keysym.sym ){
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				case SDLK_w:
					key.w = 1;
					break;
				case SDLK_s:
					key.s = 1;
					break;
				case SDLK_a:
					key.a = 1;
					break;
				case SDLK_d:
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
		case SDL_KEYUP:
			switch( e.key.keysym.sym ){
				case SDLK_w:
					key.w = 0;
					break;
				case SDLK_s:
					key.s = 0;
					break;
				case SDLK_a:
					key.a = 0;
					break;
				case SDLK_d:
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
