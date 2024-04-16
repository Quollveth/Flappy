#include <SDL2/SDL.h>
#include "./windowManager.h"

#define birdAsset "./assets/bird.bmp"
//50x35

#define pipeMiddleAsset "./assets/pipe_middle.bmp"
#define pipeEndAsset "./assets/pipe_end.bmp"


gameObject* bird = NULL;

int gameSetup(){
    //called once, setup everything, return 1 for failure

    bird = createGameObject(birdAsset);
    if(bird == NULL) return 1;

    resizeObject(bird,50,35);
    bird->bounds->x = 100;

    return 0;
}

void gameInput(int key){
    //called every time a key is pressed
    if(key == SDLK_SPACE){
        bird->bounds->y -= 50;
    }
}


int gameLogic(float delta_time){
    //called every frame, return 1 to stop game 0 to continue
    printf("\033[H\033[J"); //clear console

    bird->bounds->y += 150 * delta_time;

    printf("Bird at %d\n",bird->bounds->y);

    return 0;
}


void gameCleanup(){
    //called once, cleanup everything
}