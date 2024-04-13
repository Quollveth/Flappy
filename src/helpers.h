#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

typedef union {
	uint32_t hex;
	struct {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    };
} Color;

typedef struct rectangle{
    struct {
        int x, y;
        int w, h;
    };
    Color color;
}Rectangle;

#define GET_SDL_RECT(rect) ((SDL_Rect){.x = (rect).x, .y = (rect).y, .w = (rect).w, .h = (rect).h})

struct dBuffer{
    Rectangle *buffer;
    size_t len;
};

#define DBUF_INIT {NULL,0}
#define SET_COLOR(sdlRender, color) SDL_SetRenderDrawColor(sdlRender, color.b, color.g, color.r, 255)

//TODO: change this to use pointers
void dbAppend(struct dBuffer* db,Rectangle *item){
    if(item == NULL) return;

    Rectangle *newBuffer = (Rectangle*)realloc(db->buffer,sizeof(Rectangle) * db->len+1);
    if(newBuffer == NULL) return;

    db->buffer = newBuffer;
    db->buffer[db->len] = *item;
    db->len++;
}

void dbRemove(struct dBuffer* db,int index){
    if (index >= 0 && index < db->len) {
        for (int i = index; i < db->len - 1; i++) {
            db->buffer[i] = db->buffer[i + 1];
        }
        db->len--;
        Rectangle *newBuffer = (Rectangle*)realloc(db->buffer, sizeof(Rectangle) * db->len);
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