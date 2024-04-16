#include <stdio.h>
#include <stdlib.h>
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
    SDL_Surface* sprite;
    SDL_Rect* bounds;
}gameObject;

