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


/**** OBJECT UTILITIES ****/

#define MAX_PIECES 16 //max number of sprites per object

typedef struct {
    SDL_Surface* spriteSurface;
    SDL_Texture* spriteTexture;
} Sprite;

typedef struct{
    Sprite* sprite; //reference only, multiple objects may reference the same sprite
    SDL_Rect* bounds; //relative to object bounds, used to position it on screen
} ObjectPart;

typedef struct {
    int id;
    ObjectPart* parts[MAX_PIECES];
    int partCount;
    SDL_Rect* bounds; //object bound, used for collision checking and placing all the sprites, can be different from the actual drawn bounds
    bool flipHorizontal; //more than that is too much of a headache, i may do it later when i'm in the mood for linear algebra
}GameObject;

/* SPRITE HANDLING */

void destroySprite(Sprite* spr){
    if(spr == NULL) return;
    if(spr->spriteTexture != NULL) SDL_DestroyTexture(spr->spriteTexture);
    if(spr->spriteSurface != NULL) SDL_FreeSurface(spr->spriteSurface);
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

    return newSprite;
}

/* OBJECT HANDLING */

void destroyObject(GameObject* obj){
    if(obj == NULL) return;
    if(obj->bounds != NULL) free(obj->bounds);
    if(obj->partCount > 0){
        for(int i=0;i<obj->partCount;i++){
            if(obj->parts[i] == NULL) continue;
            free(obj->parts[i]->bounds);
            //sprites are only referenced, the same sprite may be used by multiple objects so they are not freed here
            obj->parts[i] = NULL;
        }
    }
    free(obj);
}

GameObject* initializeObject(SDL_Renderer* target){
    static int id = 1;

    GameObject* obj = malloc(sizeof(GameObject));
    if(obj == NULL) return NULL;
    //TODO: Proper error handling
    obj->bounds = NULL;

    obj->bounds = malloc(sizeof(SDL_Rect));
    if(obj->bounds == NULL){
        free(obj);
        return NULL;
    }

    *obj->bounds = (SDL_Rect){0,0,0,0};

    obj->flipHorizontal = false;

    for(int i=0;i<MAX_PIECES;i++){
        obj->parts[i] = NULL;
    }
    obj->partCount = 0;

    obj->id = id;
    id++;

    return obj;    
}

int addObjectPart(SDL_Renderer* target,GameObject* obj,Sprite* sprite,int xOffset, int yOffset,int spriteHeight, int spriteWidth){
    if(sprite == NULL) return 1;

    ObjectPart* newPart = malloc(sizeof(ObjectPart));
    if(newPart == NULL) return 1;
    //TODO: Proper error handling
    newPart->bounds = malloc(sizeof(SDL_Rect));
    if(newPart->bounds == NULL){
        free(newPart);
        return 1;
    }

    newPart->sprite = sprite;

    newPart->bounds->x = obj->bounds->x + xOffset;
    newPart->bounds->y = obj->bounds->y + yOffset;

    newPart->bounds->h = spriteHeight;
    newPart->bounds->w = spriteWidth;

    obj->parts[obj->partCount] = newPart;
    obj->partCount++;

    return 0;
}

