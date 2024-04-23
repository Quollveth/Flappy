#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./windowManager.h"

#define birdAsset "./assets/bird.bmp"
//50x35

#define pipeMiddleAsset "./assets/pipe_middle.bmp"
#define pipeEndAsset "./assets/pipe_end.bmp"

//game objects are globals

#define PIPE_DISTANCE 400 //distance between pipes
#define PIPE_GAP 200 //gap between top and bottom pipe

int gameSetup(){
    //called once, setup everything, return 1 for failure

    createGameObject(birdAsset);

    return 0;
}

void gameInput(int key){
    //called every time a key is pressed
}

int gameLogic(float delta_time){
    //called every frame, return 1 to stop game 0 to continue

    return 0;
}

void gameCleanup(){
    //called once, cleanup everything
}