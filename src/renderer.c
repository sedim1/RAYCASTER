#include "renderer.h"

#define SPRITE_SCALE 80.0f

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
extern Player player;

float zDepth[SW];

float MAX_FOG_DIST = 200.0f;
RGB ambient = {255,68,0};
//RGB ambient = {255,0,0};

RGB multiplyColor(RGB*c1,RGB*c2){
	return ((RGB){(unsigned char)((c1->r*c2->r)/255),(unsigned char)((c1->g*c2->g)/255),(unsigned char)((c1->b*c2->b)/255)});
}

void pixel(int x,int y, RGB* color){
	if (x < 0 || x >= SW || y < 0 || y >= SH) return;
	const SDL_FRect pix = {x*PIXELSCALE,y*PIXELSCALE,PIXELSCALE,PIXELSCALE};
	SDL_SetRenderDrawColor(renderer,color->r,color->g,color->b,255);
	SDL_RenderFillRect(renderer,&pix);
}

void pixelDDA(int x,int y, RGB* color){
	/*if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
	SDL_SetRenderDrawColor(renderer,color->r,color->g,color->b,255);
	SDL_RenderDrawPoint(renderer,x,y);*/
}


void DDA(int x1,int y1,int x2,int y2,RGB* color){
	/*int dx = x2 - x1, dy = y2 - y1;
	int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
	float xo = dx/(float)steps, yo = dy/(float)steps;
	float x = (float)x1, y = (float)y1;
	for(int i = 0; i < steps; i++){
		pixel((int)x,(int)y,color);
		x += xo; y += yo;
	}*/
}
////////////////////////////
void DrawMap2D(Map2D* m){
	for(int y = 0; y < m->mapHeight;y++){
		for(int x = 0; x < m->mapWidth; x++ ){
			SDL_FRect rect = {x*m->mapS/16 + 1,y*m->mapS/16 + 1,16,16};
			if(m->walls[y][x] > 0){//Dibuja el muro
				SDL_SetRenderDrawColor(renderer,255,255,255,255);
			}
			else{
				SDL_SetRenderDrawColor(renderer,255/2,255/2,255/2,255);
			}
			SDL_RenderFillRect(renderer,&rect);
		}
	}
}

void DrawPlayer(){
	RGB c1 = {255,255,0};
	SDL_FRect p = {player.position.x/16,player.position.y/16,PIXELSCALE,PIXELSCALE};
	SDL_SetRenderDrawColor(renderer,c1.r,c1.g,c1.b,255);
	SDL_RenderFillRect(renderer,&p);
}


RGB fogFactor(RGB* diffuse,RGB* fogColor, float distance){
	RGB fog;
	float weight = (distance)/MAX_FOG_DIST;
	if(weight > 1.0f) {weight = 1.0f;}
	if(weight < 0.0f) {weight = 0.0f;}
	fog.r = (unsigned char)(((1.0f-weight)*diffuse->r+weight*fogColor->r));
	fog.g = (unsigned char)(((1.0f-weight)*diffuse->g+weight*fogColor->g));
	fog.b = (unsigned char)(((1.0f-weight)*diffuse->b+weight*fogColor->b));
	return fog;
}


void castRaysDDA(Map2D* m){
	for(int x = 0; x < SW; x++){
		float cameraX = 2 * x / (float)(SW) - 1.0f;
		float rayDirX = (player.dx + player.planeX * cameraX) + 0.000001f;
		float rayDirY = (player.dy + player.planeY * cameraX) + 0.000001f;
		//posicion relativa al mapa
		int mapX = (int)(player.position.x/m->mapS);
		int mapY = (int)(player.position.y/m->mapS);
		float sideDistX, sideDistY,deltaDistX,deltaDistY;
		deltaDistX = (rayDirX == 0) ? 0.0001f : fabsf(1/rayDirX);
		deltaDistY = (rayDirY == 0) ? 0.0001f : fabsf(1/rayDirY);
		float perspDistWall;
		int stepX, stepY;
		bool hit = false;
		int side; // 0 vertical, 1 horizontal
		float playerTileX = player.position.x /m->mapS;
		float playerTileY = player.position.y / m->mapS;
		//DEfinir la direccion del rayo
		if (rayDirX < 0)
		{
			stepX = -1;
			sideDistX = (playerTileX - mapX) * deltaDistX;
		}
		else
		{
			stepX = 1;
			sideDistX = (mapX + 1.0 - playerTileX) * deltaDistX;
		}
	        if (rayDirY < 0)
		{
			stepY = -1;
			sideDistY = (playerTileY - mapY) * deltaDistY;
		}
		else
		{
			stepY = 1;
			sideDistY = (mapY + 1.0 - playerTileY) * deltaDistY;
		}
		//Empezar a castear el rayp
		while(hit == false){
			if(sideDistX < sideDistY){
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			}
			else{
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}
			if(mapX >= 0 && mapX < m->mapWidth && mapY >= 0 && mapY < m->mapHeight){
				if(m->walls[mapY][mapX] > 0){ hit = true; }// muro detectado
			}
			else
			{hit = true;}
		}

		int viewOffset = player.l + player.z;
		perspDistWall = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
		zDepth[x] = perspDistWall * m->mapS;
		int lineHeight = (int)(SH/perspDistWall);
		int drawStart = (-lineHeight / 2 + SH / 2)+viewOffset;
		if(drawStart < 0)drawStart = 0;
		int drawEnd = (lineHeight / 2 + SH / 2)+viewOffset;
		if(drawEnd >= SH)drawEnd = SH;
		//Calcula las coordenadas uv
		float mapVal = m->walls[mapY][mapX] - 1;
		double wallX;
		float shade = 1;
		if (side == 0) {wallX = playerTileY + perspDistWall * rayDirY;}
		else		{wallX = playerTileX + perspDistWall * rayDirX; shade = 0.5f;}
		wallX -= floor((wallX));
		//x coordinate on the texture
		int texX = (int)(wallX * (double)(m->wallTextures.texWidth));
		if(side == 0 && rayDirX > 0) texX = m->wallTextures.texWidth - texX - 1;
		if(side == 1 && rayDirY < 0) texX = m->wallTextures.texWidth - texX - 1;
		double step = (1.0 * m->wallTextures.texWidth / lineHeight);
		double texPos = ((drawStart - SH / 2 + lineHeight / 2) - viewOffset) * step;
		RGB color={0,0,0};
		if(side == 0) {color.r = color.r * 0.5;}
		//Dibuja el muro
		for(int y = drawStart; y <= drawEnd && drawStart <= SH && drawEnd >= 0; y++){
			// Cast the texture coordinate to integer, and mask with (texHeight - 1) in case of overflow
			int texY = (int)texPos & (m->wallTextures.texWidth - 1);
			texPos += step;
			int p = (texY * m->wallTextures.texWidth + texX) * 3 + (mapVal * m->wallTextures.texWidth * m->wallTextures.texWidth * 3);
			color.r = m->wallTextures.buffer[p+0] * shade;
			color.g = m->wallTextures.buffer[p+1] * shade;
			color.b = m->wallTextures.buffer[p+2] * shade;
			color = multiplyColor(&color,&ambient);
			color = fogFactor(&color,&((RGB){0, 0, 0}),zDepth[x]);
			pixel(x,y,&color);
		}
		//Dibuja el suelo
		float floorWallX, floorWallY;
		if(side == 0 && rayDirX > 0){ floorWallX = mapX; floorWallY = mapY + wallX; }
		else if(side == 0 && rayDirX < 0){ floorWallX = mapX + 1.0f; floorWallY = mapY + wallX; }
		else if(side == 1 && rayDirY > 0){ floorWallX = mapX + wallX; floorWallY = mapY; }
		else { floorWallX = mapX + wallX; floorWallY = mapY + 1.0f; }
		double distWall, distPlayer, currentDist;
		if (drawEnd < 0) drawEnd = 0;
		else { drawEnd += 1;}

		distWall = perspDistWall; distPlayer = 0.0f;
		float dist = 0.0f;
		//Dibuja el suelo
		for(int y = drawEnd; y < SH; y++){
			currentDist = SH / (2.0 * (y - viewOffset) - SH);
			double weight = (currentDist - distPlayer)/(distWall - distPlayer);
			double currentFloorX = weight * floorWallX + (1.0 - weight) * playerTileX;
			double currentFloorY = weight * floorWallY + (1.0 - weight) * playerTileY;
			int floorTexX, floorTexY;
			floorTexX = (int)(currentFloorX * m->floorTextures.texWidth) % m->floorTextures.texWidth;
			floorTexY = ((int)(currentFloorY * m->floorTextures.texWidth) % m->floorTextures.texWidth);
			int iFx = (int)currentFloorX, iFy = (int)currentFloorY;
			if(iFx >= 0 && iFx < m->mapWidth && iFy >= 0 && iFy < m->mapHeight){
				int floorMapVal = m->floor[iFy][iFx] - 1;
				if(floorMapVal >= 0){
					int pf = (floorTexY * m->floorTextures.texWidth + floorTexX) * 3 +  (floorMapVal * m->floorTextures.texWidth * m->floorTextures.texWidth * 3);
					color.r = m->floorTextures.buffer[pf+0] * 0.7f;
					color.g = m->floorTextures.buffer[pf+1] * 0.7f;
					color.b = m->floorTextures.buffer[pf+2] * 0.7f;
					color = multiplyColor(&color,&ambient);
					color = fogFactor(&color,&((RGB){0,0,0}),currentDist*m->mapS);
					pixel(x,y,&color);
				}
			}
		}
		//Dibuja el techo
		if(drawStart > SH) {drawStart = SH;}
		for(int y = drawStart-1; y >= 0; y--){
			currentDist = SH / (SH - 2.0 * (y - viewOffset));
			double weight = (currentDist - distPlayer)/(distWall - distPlayer);
			double currentCeilingX = weight * floorWallX + (1.0 - weight) * playerTileX;
			double currentCeilingY = weight * floorWallY + (1.0 - weight) * playerTileY;
			int ceilingTexX, ceilingTexY;
			ceilingTexX = (int)(currentCeilingX * m->ceilingTextures.texWidth) % m->ceilingTextures.texWidth;
			ceilingTexY = ((int)(currentCeilingY * m->ceilingTextures.texWidth) % m->ceilingTextures.texWidth);
			int iCx = (int)currentCeilingX, iCy = (int)currentCeilingY;
			if(iCx >= 0 && iCx < m->mapWidth && iCy >= 0 && iCy < m->mapHeight){
				int ceilingMapVal = m->ceiling[iCy][iCx] - 1;
				if(ceilingMapVal >= 0){
					int pc = (ceilingTexY * m->ceilingTextures.texWidth + ceilingTexX) * 3 +  (ceilingMapVal * m->ceilingTextures.texWidth * m->ceilingTextures.texWidth * 3);
					color.r = m->ceilingTextures.buffer[pc+0] * 0.7f;
					color.g = m->ceilingTextures.buffer[pc+1] * 0.7f;
					color.b = m->ceilingTextures.buffer[pc+2] * 0.7f;
					color = multiplyColor(&color,&ambient);
					color = fogFactor(&color,&((RGB){0, 0, 0}),currentDist*m->mapS);
					pixel(x,y,&color);
				}
			}
		}

	}
}

void DrawSprite2D(Sprite2D* sprite){
	if(sprite->on == false) { return;}
	double spriteX = sprite->position.x - player.position.x;
	double spriteY = sprite->position.y - player.position.y;

	double invDet = 1.0f / (player.planeX * player.dy - player.dx * player.planeY);

	double transformX = invDet * (player.dy * spriteX - player.dx * spriteY);
	double transformY = invDet * (-player.planeY * spriteX + player.planeX * spriteY); //Profundidad actual del objeto en el eje de projundidad
	
	int screenX = (int)((SW/2)*(1+transformX/transformY)); // posision en la pantalla en X
	int viewOffset = (player.l + player.z) + sprite->z;
	//height of sprite on the screen
	int spriteHeight = abs((int)(SH/transformY*SPRITE_SCALE));
	//Calcular desde donde empezara a dibujar cada columna del pixel
	int drawStartY = (SH/2 - spriteHeight/2) + viewOffset; 
	if(drawStartY < 0) {drawStartY = 0;}
	int drawEndY = (SH/2 + spriteHeight/2) + viewOffset;
	if(drawEndY > SH) {drawEndY = SH;}
	//Width of sprite on screen
	int spriteWidth = abs((int)(SH/transformY*SPRITE_SCALE));
	int drawStartX = screenX - spriteWidth/2;
	if(drawStartX < 0) {drawStartX = 0;}
	int drawEndX = screenX + spriteWidth/2;
	if(drawEndX > SW) {drawEndX = SW;}
	//Dibuja el sprite columna por columna, pixel por pixel
	if (transformY <= 0 ) {return;} //No dibujar el sprite si se encuentra fuera de la pantalla
	for(int x = drawStartX; x < drawEndX; x++){
		int texX = ((int)((256 * (x - (-spriteWidth / 2 + screenX)) * sprite->texture.texWidth / spriteWidth) / 256));
		if(zDepth[x] < transformY) {continue;}
		for(int y = drawStartY; y < drawEndY; y++){
			int d  = (y-viewOffset) * 256 - SH * 128  + spriteHeight * 128;
			int texY = (((d * sprite->texture.texHeight) / spriteHeight) / 256);
			RGB color = {0,0,0};
			int p = (texY * sprite->texture.texWidth + texX) * 3 + (sprite->mapVal * sprite->texture.texWidth * sprite->texture.texHeight * 3);
			 if ((texY < 0 || texY >= sprite->texture.texHeight)){continue;}
			color.r = sprite->texture.buffer[p];
			color.g = sprite->texture.buffer[p+1];
			color.b = sprite->texture.buffer[p+2];
			if(color.r == 255 && color.g == 0 && color.b == 255) { continue;}
			color = multiplyColor(&color,&ambient);
			color = fogFactor(&color,&((RGB){0, 0, 0}),transformY);
			pixel(x,y,&color);
		}
	}
}
