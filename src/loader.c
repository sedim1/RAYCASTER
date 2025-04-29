#include "loader.h"

void freeMap(Map2D* map){
	for(int i = map->mapHeight; i >= 0; i--){
		free(map->walls[i]);
	}
	//Delete all texture data of walls floors and ceiling
	freeTexture(&map->wallTextures);
	//Free map buffers and allocated textures
	free(map->walls);
}

void loadMap(Map2D* map,char* path){
	FILE* fp = fopen(path,"r");
	int i = 0 , j = 0;
	char texPath[50];
	if(map->walls!=NULL){ freeMap(map); }
	if(fp){
		fscanf(fp,"%d %d %d",&map->mapWidth,&map->mapHeight,&map->mapS);
		map->walls = (int**)malloc(sizeof(int*)*map->mapHeight);
		for(i = 0; i < map->mapHeight;i++)
		{
			map->walls[i] = (int*)malloc(sizeof(int)*map->mapWidth);
			for(j = 0; j < map->mapWidth;j++){
				fscanf(fp,"%d ",&map->walls[i][j]);
			}
		}
		fscanf(fp,"%s",texPath);
		loadTexture(&map->wallTextures,texPath);
		fclose(fp);
	}
	else{printf("ERROR::MAP WAS NOT FOUND::\n");}
}

void loadTexture(TEXMAP* map,char* path){
	if(map->buffer!=NULL){ freeTexture(map); }
	FILE* fp = fopen(path,"r");
	if(fp){
		int i = 0;
		fscanf(fp,"%d %d",&map->texWidth,&map->texHeight);
		map->buffer = (int*)malloc(sizeof(int) * ((map->texWidth * map->texHeight)*3));
		while(!feof(fp)){
			fscanf(fp,"%d",&map->buffer[i]);
			i += 1;
		}
		printf("Loaded texture %s was succesful!\n",path);
		fclose(fp);
	}
	else{
		printf("ERROR::TEXTURE WAS NOT FOUND::\n");
	}
}

void freeTexture(TEXMAP* map){
	free(map->buffer); map->texWidth = 0; map->texHeight = 0; map->buffer=NULL;
}
