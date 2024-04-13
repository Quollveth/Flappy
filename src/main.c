#include <stdio.h>
#include <SDL2/SDL.h>
#include "./windowManager.h"


Rectangle *bird;

int buffered = FALSE;

void gameSetup(){
    //called once, setup everything
    setBGColor(0x6ca8d6); //sky blue

    bird = malloc(sizeof(Rectangle));
    bird->x = 100;
    bird->y = 0;
    bird->w = 50;
    bird->h = 50;

    bird->color.hex = 0xfcba03; //yellow

    addRect(bird);

}

void gameInput(int key){
    //called every time a key is pressed

    if(key == SDLK_SPACE){
        buffered = TRUE;
    }

}


int gameLogic(float delta_time){
    //called every frame, return 1 to stop game 0 to continue
    printf("\033[2J"); // clear terminal screen
    printf("\033[H"); // move cursor to top-left corner
    printf("\033[0;32mDelta time: %f\n\033[0m", delta_time);
    

    //gravity
    bird->y += 200 * delta_time;

    if(buffered){
        bird->y -= 100;
        buffered = FALSE;
    }

    return 0;
}

void gameCleanup(){
    //called once, cleanup everything
    free(bird);
}