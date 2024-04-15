#include "./gameObjects.h"

struct dBuffer{
    gameObject* *buffer; //array of pointers to structs
    size_t len;
};

#define DBUF_INIT {NULL,0}

void dbAppend(struct dBuffer* db,gameObject *item){
    if(item == NULL) return;

    gameObject **newBuffer = (gameObject**)realloc(db->buffer,sizeof(gameObject*) * db->len+1);
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
        gameObject **newBuffer = (gameObject**)realloc(db->buffer, sizeof(gameObject) * db->len);
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