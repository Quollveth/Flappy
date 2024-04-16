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


/**** gameObject UTILITIES ****/

typedef struct gameObject{
    int id;
    SDL_Surface* spriteSurface;
    SDL_Texture* spriteTexture;
    SDL_Rect* bounds;
}gameObject;

void destroyObject(gameObject* obj){
    if(obj == NULL) return;

    free(obj->bounds);
    SDL_DestroyTexture(obj->spriteTexture);
    SDL_FreeSurface(obj->spriteSurface);
    free(obj);
}

gameObject* initializeObject(SDL_Renderer* target, char* spritePath){
    static int id = 1;

    gameObject* obj = malloc(sizeof(gameObject));
    if(obj == NULL) return NULL;

    obj->spriteSurface = NULL;
    obj->spriteTexture = NULL;
    obj->bounds = NULL;

    obj->spriteSurface = SDL_LoadBMP(spritePath);
    if(obj->spriteSurface == NULL){
        free(obj);
        return NULL;
    }

    obj->spriteTexture = SDL_CreateTextureFromSurface(target,obj->spriteSurface);
    if(obj->spriteTexture == NULL){
        SDL_FreeSurface(obj->spriteSurface);
        free(obj);
        return NULL;
    }

    obj->bounds = malloc(sizeof(SDL_Rect));
    if(obj->bounds == NULL){
        SDL_DestroyTexture(obj->spriteTexture);
        SDL_FreeSurface(obj->spriteSurface);
        free(obj);
        return NULL;
    }

    obj->bounds->x = 0;
    obj->bounds->y = 0;
    obj->bounds->h = obj->spriteSurface->h;
    obj->bounds->w = obj->spriteSurface->w;

    obj->id = id;
    id++;

    return obj;    
}
