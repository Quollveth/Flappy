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

#define MAX_PIECES 32 //max number of sprites per object
#define MAX_TEXT 1024 //max text buffer

enum spriteType {
    IMAGE,
    TEXT
};

typedef struct {
    SDL_Surface* spriteSurface;
    SDL_Texture* spriteTexture;
    enum spriteType type;
} Sprite;

typedef struct{
    Sprite* sprite; //reference only, multiple objects may reference the same sprite
    SDL_Rect* bounds; //relative to object bounds, used to position it on screen
    SDL_RendererFlip flip; //will be improved when i feel like doing linear algebra
} ObjectPart;

typedef struct {
    int id;
    ObjectPart* parts[MAX_PIECES];
    int partCount;
    SDL_Rect* bounds; //object bound, used for collision checking and placing all the sprites, can be different from the actual drawn bounds
}GameObject;

/* SPRITE HANDLING */

void destroySprite(Sprite* spr){
    if(spr == NULL) return;
    if(spr->spriteTexture != NULL) SDL_DestroyTexture(spr->spriteTexture);
    if(spr->spriteSurface != NULL) SDL_FreeSurface(spr->spriteSurface);
    free(spr);
}

Sprite* initializeSprite(SDL_Renderer* target, char* spritePath,enum spriteType type){
    Sprite* newSprite = malloc(sizeof(Sprite));
    if(newSprite == NULL) return NULL;
    //TODO: Proper error handling

    newSprite->spriteSurface = NULL;
    newSprite->spriteTexture = NULL;

    if(type == TEXT) return newSprite; //text sprites are not initialized since they are generated dynamically as the text updates

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

/*
* Destroys an object and all it's parts while leaving sprites untouched
* Remember to separately free the sprites with destroySprite
* Do not directly call this function if the object is on the drawList
*/
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
    //TODO: Above check done by wrapper function, can be removed
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

    newPart->flip = SDL_FLIP_NONE;

    obj->parts[obj->partCount] = newPart;
    obj->partCount++;

    if (obj->partCount == 1) {
        *obj->bounds = *newPart->bounds;
        return 0;
    }

    int minX = obj->bounds->x;
    int minY = obj->bounds->y;
    int maxX = obj->bounds->x + obj->bounds->w;
    int maxY = obj->bounds->y + obj->bounds->h;

    if (newPart->bounds->x < minX) minX = newPart->bounds->x;
    if (newPart->bounds->y < minY) minY = newPart->bounds->y;
    if (newPart->bounds->x + newPart->bounds->w > maxX) maxX = newPart->bounds->x + newPart->bounds->w;
    if (newPart->bounds->y + newPart->bounds->h > maxY) maxY = newPart->bounds->y + newPart->bounds->h;

    obj->bounds->x = minX;
    obj->bounds->y = minY;
    obj->bounds->w = maxX - minX;
    obj->bounds->h = maxY - minY;

    return 0;
}

/*
* Moves an object with no sprites
* Using on a multi part object will cause bad things
*/
inline static void moveInvisibleObject(GameObject* obj, int newX, int newY){
    if(obj == NULL) return;
    obj->bounds->x = newX;
    obj->bounds->y = newY;
}

/*
* Moves an object with only one sprite
* Using on a multi part object will cause bad things
* Do not use on invisible objects
*/
inline static void moveSimpleObject(GameObject* obj, int newX, int newY){
    moveInvisibleObject(obj,newX,newY);
    if(obj->parts[0] == NULL) return;
    obj->parts[0]->bounds->x = newX;
    obj->parts[0]->bounds->y = newY;
}

/*
* Moves an object and all it's parts to a new position
* Only needed for multi sprite objects
* Invisible or single part objects can be moved faster with moveSimpleObject and moveInvisibleObject
*/
void moveSplitObject(GameObject* obj, int newX, int newY){
    if(obj == NULL) return;

    int xDiff = newX - obj->bounds->x;
    int yDiff = newY - obj->bounds->y;
    
    obj->bounds->x = newX;
    obj->bounds->y = newY;    

    if(obj->partCount == 0) return;

    for(int i=0;i<obj->partCount;i++){
        if(obj->parts[i] == NULL) continue;
        obj->parts[i]->bounds->x += xDiff;
        obj->parts[i]->bounds->y += yDiff;
    }
}

/*
* Updates the object bounds to encompass all sprites in the object
* They should be automatically updated when adding new pieces anyways
* Does nothing to invisible objects
* Position remains unchanged
*/
void recalculateBounds(GameObject* obj){
    if(obj == NULL || obj->partCount == 0) return;

    int minX = obj->parts[0]->bounds->x;
    int minY = obj->parts[0]->bounds->y;
    int maxX = obj->parts[0]->bounds->x + obj->parts[0]->bounds->w;
    int maxY = obj->parts[0]->bounds->y + obj->parts[0]->bounds->h;

    for(int i = 1; i < obj->partCount; i++){
        if(obj->parts[i] == NULL) continue;

        if(obj->parts[i]->bounds->x < minX) {
            minX = obj->parts[i]->bounds->x;
        }

        if(obj->parts[i]->bounds->y < minY) {
            minY = obj->parts[i]->bounds->y;
        }

        if(obj->parts[i]->bounds->x + obj->parts[i]->bounds->w > maxX) {
            maxX = obj->parts[i]->bounds->x + obj->parts[i]->bounds->w;
        }

        if(obj->parts[i]->bounds->y + obj->parts[i]->bounds->h > maxY) {
            maxY = obj->parts[i]->bounds->y + obj->parts[i]->bounds->h;
        }
    }

    obj->bounds->w = maxX - minX;
    obj->bounds->h = maxY - minY;
}