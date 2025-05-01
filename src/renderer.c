#include "renderer.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
extern Player player;

void pixel(int x,int y, RGB* color){
	if (x < 0 || x >= SW || y < 0 || y >= SH) return;
	SDL_Rect pix = {x*PIXELSCALE,y*PIXELSCALE,PIXELSCALE,PIXELSCALE};
	SDL_SetRenderDrawColor(renderer,color->r,color->g,color->b,255);
	SDL_RenderFillRect(renderer,&pix);
}

void pixelDDA(int x,int y, RGB* color){
	if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
	SDL_SetRenderDrawColor(renderer,color->r,color->g,color->b,255);
	SDL_RenderDrawPoint(renderer,x,y);
}


void DDA(int x1,int y1,int x2,int y2,RGB* color){
	int dx = x2 - x1, dy = y2 - y1;
	int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
	float xo = dx/(float)steps, yo = dy/(float)steps;
	float x = (float)x1, y = (float)y1;
	for(int i = 0; i < steps; i++){
		pixel((int)x,(int)y,color);
		x += xo; y += yo;
	}
}
////////////////////////////
void DrawMap2D(Map2D* m){
	for(int y = 0; y < m->mapHeight;y++){
		for(int x = 0; x < m->mapWidth; x++ ){
			SDL_Rect rect = {x*m->mapS/16 + 1,y*m->mapS/16 + 1,16,16};
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
	SDL_Rect p = {player.x/16,player.y/16,PIXELSCALE,PIXELSCALE};
	SDL_SetRenderDrawColor(renderer,c1.r,c1.g,c1.b,255);
	SDL_RenderFillRect(renderer,&p);
	SDL_RenderDrawLine(renderer,player.x/16 + PIXELSCALE/2,player.y/16 + PIXELSCALE/2,(player.x+player.dx*20)/16 + PIXELSCALE/2,(player.y+player.dy*20)/16 + PIXELSCALE/2);
}

void castRays(Map2D* m){
	RGB color = {255,0,0};
	float ra = normalizeAngle(player.a - (FOV/2)); float rx, ry,hrx, hry, vrx,vry,xo, yo;
	int dof =0; int r = 0;
	float dist = 0.0f;
	bool hasBegun = false; int lastTopY, lastBottomY;
	for(r = 0; r < SW; r+=1){
		//Lineas horizontales
		float distH = 10000.0f;
		float ra = normalizeAngle(player.a - FOV / 2 + ((float)r / SW) * FOV);
		dof = 0;
		hrx = player.x; hry = player.y;
		if(ra == 0.0f || ra == 180.0f){ ra += 0.01f; ra = normalizeAngle(ra); }
		if(ra > 180.0f){ 
			hry = (int)((player.y/m->mapS))*m->mapS - 0.0001f; hrx =  player.x - (player.y-hry)/tan(degToRad(ra));
			yo = -m->mapS; xo = -m->mapS/tan(degToRad(ra)); 
		}
		if(ra < 180.0f){ 
			hry = (int)((player.y/m->mapS))*m->mapS + m->mapS; hrx =  player.x - (player.y-hry)/tan(degToRad(ra));
			yo = m->mapS; xo = m->mapS/tan(degToRad(ra)); 
		}
		while(dof < DOF){
			//posicion actual del rayo en el mapa
			int mapX = (int)(hrx/m->mapS); int mapY = (int)(hry/m->mapS);
			if(mapX >= 0 && mapX < m->mapWidth && mapY >= 0 && mapY < m->mapHeight){//Muro detectado
				if(m->walls[mapY][mapX] > 0){
					dof = DOF; distH = distance(player.x,player.y,hrx,hry);
				}
				else{
					hrx += xo;
					hry += yo;
					dof+=1;
				}
			}
			else{
				hrx += xo;
				hry += yo;
				dof+=1;
			}
		}
		//Lineas verticales
		float distV = 10000.0f;
		dof = 0;
		vrx = player.x; vry = player.y;
		if(ra == 90.0f || ra == 270.0f){ ra += 0.0001f; ra = normalizeAngle(ra);}
		if(ra > 90.0f && ra < 270.0f){ //Izquerda
			vrx = (int)((player.x/m->mapS))*m->mapS - 0.0001f; vry =  player.y - (player.x-vrx)*tan(degToRad(ra));
			xo = -m->mapS; yo = -m->mapS*tan(degToRad(ra)); 
		}
		if(ra < 90.0f || ra > 270.0f){ //derecha
			vrx = (int)((player.x/m->mapS))*m->mapS + m->mapS; vry =  player.y - (player.x-vrx)*tan(degToRad(ra));
			xo = m->mapS; yo = m->mapS*tan(degToRad(ra)); 
		}
		while(dof < DOF){
			//posicion actual del rayo en el mapa
			int mapX = (int)(vrx/m->mapS); int mapY = (int)(vry/m->mapS);
			if(mapX >= 0 && mapX < m->mapWidth && mapY >= 0 && mapY < m->mapHeight){//Muro detectado
				if(m->walls[mapY][mapX] > 0){
					dof = DOF; distV = distance(player.x,player.y,vrx,vry);
				}
				else{
					vrx += xo;
					vry += yo;
					dof+=1;
				}
			}
			else{
				vrx += xo;
				vry += yo;
				dof+=1;
			}

		}
		if(distV < distH ){
			rx = vrx; ry = vry; dist = distV;
			color.r = 255;
		}
		else if (distH < distV){
			rx = hrx; ry = hry; dist = distH;
			color.r = 255 * 0.5;
		}
		dist = dist * cos(degToRad(player.a-ra));
		float lineHeight = 64/dist * distFromProjectionPlane;
		int yStart = - lineHeight / 2 + SH / 2;
		int yEnd = lineHeight / 2 + SH / 2;
		for(int y = yStart; y < yEnd; y++){
			pixel(r,y,&color);
		}

	}
}

void castRaysDDA(Map2D* m){
	for(int x = 0; x < SW; x++){
		float cameraX = 2 * x / (float)(SW) - 1.0f;
		float rayDirX = (player.dx + player.planeX * cameraX) + 0.000001f;
		float rayDirY = (player.dy + player.planeY * cameraX) + 0.000001f;
		//posicion relativa al mapa
		int mapX = (int)(player.x/m->mapS);
		int mapY = (int)(player.y/m->mapS);
		float sideDistX, sideDistY,deltaDistX,deltaDistY;
		deltaDistX = (rayDirX == 0) ? 0.0001f : fabsf(1/rayDirX);
		deltaDistY = (rayDirY == 0) ? 0.0001f : fabsf(1/rayDirY);
		float perspDistWall;
		int stepX, stepY;
		bool hit = false;
		int side; // 0 vertical, 1 horizontal
		float playerTileX = player.x /m->mapS;
		float playerTileY = player.y / m->mapS;
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
		perspDistWall = (side == 0) ? (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
		int lineHeight = (int)(SH/perspDistWall);
		int drawStart = (-lineHeight / 2 + SH / 2)+player.l;
		if(drawStart < 0)drawStart = 0;
		int drawEnd = (lineHeight / 2 + SH / 2)+player.l;
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
		double texPos = ((drawStart - SH / 2 + lineHeight / 2) - player.l) * step;
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
		distWall = perspDistWall; distPlayer = 0.0f;
		//Dibuja el suelo
		for(int y = drawEnd+1; y < SH; y++){
			currentDist = SH / (2.0 * (y - player.l) - SH);
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
					color.r = m->floorTextures.buffer[pf+0] * 0.7;
					color.g = m->floorTextures.buffer[pf+1] * 0.7;
					color.b = m->floorTextures.buffer[pf+2] * 0.7;
					pixel(x,y,&color);
				}
				/*if(ceilingMapVal >= 0){
					int pc = (floorTexY * m->ceilingTextures.texWidth + floorTexX) * 3 +  (ceilingMapVal * m->ceilingTextures.texWidth * m->ceilingTextures.texWidth * 3);
					color.r = m->ceilingTextures.buffer[pc+0] * 0.7;
					color.g = m->ceilingTextures.buffer[pc+1] * 0.7;
					color.b = m->ceilingTextures.buffer[pc+2] * 0.7;
					pixel(x,SH-y,&color);
				}*/
			}
		}
		//Dibuja el techo
		/*if(drawStart > SH) {drawStart = SH;}
		for(int y = drawStart-1; y >= 0; y--){
			printf("xx\n");
			currentDist = SH / (2.0 * (y + player.l) - SH);
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
					color.r = m->ceilingTextures.buffer[pc+0] * 0.7;
					color.g = m->ceilingTextures.buffer[pc+1] * 0.7;
					color.b = m->ceilingTextures.buffer[pc+2] * 0.7;
					pixel(x,y,&color);
				}
			}
		}*/

	}
}
