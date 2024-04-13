#include <stdio.h>
#include <SDL2/SDL.h>
#include "./windowManager.h"


void gameSetup(){
    //called once, setup everything

    Rectangle *rect1 = malloc(sizeof(Rectangle));

    rect1->x = 10;
    rect1->y = 10;
    rect1->w = 50;
    rect1->h = 50;

    rect1->color.hex = 0xFF0000;

    addRect(rect1);

    Rectangle *rect2 = malloc(sizeof(Rectangle));

    rect2->x = 100;
    rect2->y = 100;
    rect2->w = 50;
    rect2->h = 50;

    rect2->color.hex = 0x00FF00;

    addRect(rect2);

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
    free(toDraw.buffer[0]);
    free(toDraw.buffer[1]);
}