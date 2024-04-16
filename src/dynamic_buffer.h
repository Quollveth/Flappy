#include "./gameObjects.h"

struct dBuffer{
    /* array of pointers
    the toDraw buffer (declared in windowManager.h) contains a reference to every gameObject to be drawn, objects may need to be updated every frame so only a reference to it is stored */
    gameObject* *buffer;
    size_t len;
};

#define DBUF_INIT {NULL,0}

int dbAppend(struct dBuffer* db,gameObject *item){
    if(item == NULL) return 1;

    gameObject **newBuffer = (gameObject**)realloc(db->buffer,sizeof(gameObject*) * db->len+1);
    if(newBuffer == NULL) return 1;

    db->buffer = newBuffer;
    db->buffer[db->len] = item;
    db->len++;

    return 0;
}

void dbRemove(struct dBuffer* db,int index){
    if (index >= 0 && index < db->len) {
        for (int i = index; i < db->len - 1; i++) {
            db->buffer[i] = db->buffer[i + 1];
        }
        db->len--;
        gameObject **newBuffer = (gameObject**)realloc(db->buffer, sizeof(gameObject) * db->len);
        if (newBuffer != NULL) {
            db->buffer = newBuffer;
        }
    }
}

inline void dbFree(struct dBuffer* db){
    free(db->buffer);
    //objects themselves are not freed as this is only a reference, managing the objects memory is left to the game code itself
    db->buffer = NULL;
    db->len = 0;
}