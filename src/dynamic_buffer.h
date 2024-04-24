#include "./gameObjects.h"

struct dBuffer{
    /* array of pointers
    the toDraw buffer (declared in windowManager.h) contains a reference to every GameObject to be drawn, objects may need to be updated every frame so only a reference to it is stored */
    GameObject* *buffer;
    size_t len;
};

#define DBUF_INIT (struct dBuffer){NULL,0}

int dbAppend(struct dBuffer* db,GameObject *item){
    if(item == NULL) return 1;

    GameObject **newBuffer = (GameObject**)realloc(db->buffer,sizeof(GameObject*) * db->len+1);
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
        GameObject **newBuffer = (GameObject**)realloc(db->buffer, sizeof(GameObject) * db->len);
        if (newBuffer != NULL) {
            db->buffer = newBuffer;
        }
    }
}

//Removes object with given id
int dbRemoveById(struct dBuffer* db,int id){
    for(int i = 0;i < db->len;i++){
        if(db->buffer[i]->id == id){
            dbRemove(db,i);
            return 0; //success
        }
    }
    return 1; //could not find id
} 

//Gets object with given id
GameObject* dbGetObject(struct dBuffer* db,int id){
    for(int i = 0;i < db->len;i++){
        if(db->buffer[i]->id == id){
            return db->buffer[i];
        }
    }
    return NULL; //could not find id
}

inline void dbFree(struct dBuffer* db){
    free(db->buffer);
    //objects themselves are not freed as this is only a reference, managing the objects memory is left to the game code itself
    db->buffer = NULL;
    db->len = 0;
}