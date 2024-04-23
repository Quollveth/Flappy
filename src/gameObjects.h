#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

/**** COLOR UTILITIES ****/

typedef union {
	uint32_t hex;
	struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };
} Color;
#define SET_COLOR(sdlRender, color) SDL_SetRenderDrawColor(sdlRender, color.b, color.g, color.r, 255)


/**** GameObject UTILITIES ****/

#define MAX_PIECES 16 //max number of sprites per object

typedef struct {
    SDL_Surface* spriteSurface;
    SDL_Texture* spriteTexture;
    SDL_Rect* bounds;
} Sprite;

typedef struct {
    int id;
    Sprite* sprites[MAX_PIECES];
    SDL_Rect* bounds;
    bool flipHorizontal;
    int spriteCount;
}GameObject;

void destroyObject(GameObject* obj){
    if(obj == NULL) return;
    

    free(obj->bounds);
    free(obj);
}

void destroySprite(Sprite* spr){
    if(spr == NULL) return;
    if(spr->spriteTexture != NULL) SDL_DestroyTexture(spr->spriteTexture);
    if(spr->spriteSurface != NULL) SDL_FreeSurface(spr->spriteSurface);
    if(spr->bounds != NULL) free(spr->bounds);
    free(spr);
}

Sprite* initializeSprite(SDL_Renderer* target, char* spritePath){
    Sprite* newSprite = malloc(sizeof(Sprite));
    if(newSprite == NULL) return NULL;
    //TODO: Proper error handling

    newSprite->spriteSurface = NULL;
    newSprite->spriteTexture = NULL;

    newSprite->spriteSurface = SDL_LoadBMP(spritePath);
    if(newSprite->spriteSurface == NULL){
        destroySprite(newSprite);
        return NULL;
        //TODO: Proper error handling
    }

    newSprite->spriteTexture = SDL_CreateTextureFromSurface(target,newSprite->spriteSurface);
    if(newSprite->spriteTexture == NULL){
        destroySprite(newSprite);
        return NULL;
        //TODO: Proper error handling
    }

    newSprite->bounds = malloc(sizeof(SDL_Rect));
    if(newSprite->bounds == NULL){
        destroySprite(newSprite);
        return NULL;
        //TODO: Proper error handling
    }

    newSprite->bounds->h = newSprite->spriteSurface->h;
    newSprite->bounds->w = newSprite->spriteSurface->w;

    newSprite->bounds->x = 0;
    newSprite->bounds->y = 0;

    return newSprite;
}

GameObject* initializeObject(SDL_Renderer* target){
    static int id = 1;

    GameObject* obj = malloc(sizeof(GameObject));
    if(obj == NULL) return NULL;

    obj->bounds = NULL;

    obj->bounds = malloc(sizeof(SDL_Rect));
    if(obj->bounds == NULL){
        free(obj);
        return NULL;
    }

    obj->bounds->x = 0;
    obj->bounds->y = 0;
    obj->bounds->h = 0;
    obj->bounds->w = 0;

    obj->flipHorizontal = false;

    for(int i=0;i<MAX_PIECES;i++){
        obj->sprites[i] = NULL;
    }
    obj->spriteCount = 0;

    obj->id = id;
    id++;

    return obj;    
}

void addSpriteToObject(SDL_Renderer* target,GameObject* obj,char* spritePath,int x, int y){
    Sprite* newSprite = initializeSprite(target,spritePath);
    if(newSprite == NULL) return;

    if(obj->spriteCount != 0){
        newSprite->bounds->x += x;
        newSprite->bounds->y += y;
    }

    obj->sprites[obj->spriteCount] = newSprite;
    obj->spriteCount++;
}

void resizeObject(GameObject* obj,int w, int h){
    obj->bounds->h = h;
    obj->bounds->w = w;
}