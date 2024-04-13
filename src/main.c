#include <stdio.h>
#include <SDL2/SDL.h>
#include "./windowManager.h"

void gameSetup(){
    //called once, setup everything
    Rectangle rect;

    rect.x = 10;
    rect.y = 10;

    rect.w = 50;
    rect.h = 50;

    rect.color.hex = 0x00ff8c;

    addRect(rect);
}

void gameInput(int key){
    //called every time a key is pressed
}

int gameLogic(float delta_time){
    //called every frame, return 1 to stop game 0 to continue
    return 0;
}