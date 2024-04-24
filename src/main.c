#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./windowManager.h"

#define birdAsset "./assets/bird.bmp"
//50x35
#define pipeMiddleAsset "./assets/pipe_middle.bmp"
#define pipeEndAsset "./assets/pipe_end.bmp"

Sprite* birdSprite;
Sprite* pipeEndSprite;
Sprite* pipeMiddleSprite;

#define PIPE_DISTANCE 400 //distance between pipes
#define PIPE_GAP 200 //gap between top and bottom pipe

//game objects are globals
GameObject* bird;
GameObject* pipe;

typedef struct {
    GameObject* top;
    GameObject* bottom;
    int xPos;
}Pipe; //there is no way for a object to have multiple bounding boxes but i still want pipes to be one thing instead of two separate objects

inline static int randomNumber(int cap) {
    return rand() % cap;
}


Pipe* createPipe(){
    Pipe* newPipe = malloc(sizeof(Pipe));
    // place a random amount of pipes at the bottom
    // skip the gap distance
    // finish with the top pipe 

    newPipe->top = createGameObject(pipeEndSprite,50,50);
    int nPipes = randomNumber(8);

    nPipes = 8;
    for (int i = 1; i <= nPipes; i++) {
        buildGameObject(
            newPipe->top,
            pipeMiddleSprite,
            0,
            50 * i, //add pipes above the end piece
            50,
            50
        );
    }
    moveSplitObject(
        newPipe->top,
        0,
        gameSettings.WIN_HEIGHT - ((nPipes+1)*50)
    );



    newPipe->xPos = 0;

    return newPipe;
}

int gameSetup(){
    //called once, setup everything, return 1 for failure
    srand(time(NULL));

    birdSprite = loadSprite(birdAsset);
    pipeEndSprite = loadSprite(pipeEndAsset);
    pipeMiddleSprite = loadSprite(pipeMiddleAsset);

    bird = createGameObject(birdSprite,50,35);

    moveSimpleObject(bird,500,50);

    createPipe();

    return 0;
}

void gameInput(int key){
    //called every time a key is pressed
}

int gameLogic(float delta_time){
    //called every frame, return 1 to quit game 0 to continue

    return 0;
}

void gameCleanup(){
    //called once, cleanup everything
    destroyGameObject(bird);
}