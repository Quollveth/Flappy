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


/**** RECTANGLE UTILITIES ****/

//this allows to store a sdl rectangle with a color assigned

typedef struct rectangle{
    struct {
        int x, y;
        int w, h;
    };
    Color color;
}Rectangle;
#define GET_SDL_RECT(rect) ((SDL_Rect){rect->x, rect->y, rect->w, rect->h})

void printSdlRect(SDL_Rect *rect){
    printf("x: %d, y: %d, w: %d, h: %d\n",rect->x,rect->y,rect->w,rect->h);
}

/**** DYNAMIC BUFFER FUNCTIONS ****/

struct dBuffer{
    Rectangle* *buffer; //array of pointers to structs
    size_t len;
};

#define DBUF_INIT {NULL,0}

void dbAppend(struct dBuffer* db,Rectangle *item){
    if(item == NULL) return;

    Rectangle **newBuffer = (Rectangle**)realloc(db->buffer,sizeof(Rectangle*) * db->len+1);
    if(newBuffer == NULL) return;

    db->buffer = newBuffer;
    db->buffer[db->len] = item;
    db->len++;
}

void dbRemove(struct dBuffer* db,int index){
    if (index >= 0 && index < db->len) {
        for (int i = index; i < db->len - 1; i++) {
            db->buffer[i] = db->buffer[i + 1];
        }
        db->len--;
        Rectangle **newBuffer = (Rectangle**)realloc(db->buffer, sizeof(Rectangle) * db->len);
        if (newBuffer != NULL) {
            db->buffer = newBuffer;
        }
    }
}

inline void dbFree(struct dBuffer* db){
    free(db->buffer);
    db->buffer = NULL;
    db->len = 0;
}