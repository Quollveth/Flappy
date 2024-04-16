#include <SDL2/SDL.h>
#include "./windowManager.h"

gameObject* testObject;

void gameSetup(){
    //called once, setup everything
    testObject = createGameObject("./assets/placeholder.bmp");

    resizeObject(testObject,200,300);
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