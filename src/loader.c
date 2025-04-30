#include "loader.h"

void freeMap(Map2D* map){
	printf("Deleting Buffer map..\n");
	for(int i = map->mapHeight-1; i >= 0; i--){
		free(map->walls[i]);
		free(map->floor[i]);
		free(map->ceiling[i]);
	}
	//Delete all texture data of walls floors and ceiling
	freeTexture(&map->wallTextures); freeTexture(&map->floorTextures); freeTexture(&map->ceilingTextures);
	//Free map buffers and allocated textures
	free(map->walls); free(map->floor); free(map->ceiling);
}

void loadMap(Map2D* map,char* path){
	FILE* fp = fopen(path,"r");
	int i = 0 , j = 0;
	char texPath[50];
	if(map->walls!=NULL){ freeMap(map); }
	if(fp){
		fscanf(fp,"%d %d %d",&map->mapWidth,&map->mapHeight,&map->mapS);
		map->walls = (int**)malloc(sizeof(int*)*map->mapHeight);
		map->floor = (int**)malloc(sizeof(int*)*map->mapHeight);
		map->ceiling = (int**)malloc(sizeof(int*)*map->mapHeight);
		//Read wall buffer
		for(i = 0; i < map->mapHeight;i++)
		{
			map->walls[i] = (int*)malloc(sizeof(int)*map->mapWidth);
			for(j = 0; j < map->mapWidth;j++){
				fscanf(fp,"%d ",&map->walls[i][j]);
			}
		}
		//Read floor buffer
		for(i = 0; i < map->mapHeight;i++)
		{
			map->floor[i] = (int*)malloc(sizeof(int)*map->mapWidth);
			for(j = 0; j < map->mapWidth;j++){
				fscanf(fp,"%d ",&map->floor[i][j]);
			}
		}
		//Read ceiling buffer
		for(i = 0; i < map->mapHeight;i++)
		{
			map->ceiling[i] = (int*)malloc(sizeof(int)*map->mapWidth);
			for(j = 0; j < map->mapWidth;j++){
				fscanf(fp,"%d ",&map->ceiling[i][j]);
			}
		}
		fscanf(fp,"%s",texPath);
		loadTexture(&map->wallTextures,texPath);
		fscanf(fp,"%s",texPath);
		loadTexture(&map->floorTextures,texPath);
		fscanf(fp,"%s",texPath);
		loadTexture(&map->ceilingTextures,texPath);
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
	printf("Deleting TextureMap map..\n");
	free(map->buffer); map->texWidth = 0; map->texHeight = 0; map->buffer=NULL;
}
