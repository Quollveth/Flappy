#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "./windowManager.h"

#define MAX_PIPES 3

Rectangle *bird;

int buffered = FALSE;

Rectangle *pipes[MAX_PIPES*2];
int nPipes = 0;

Rectangle* createPipeSegment(int top,int gap){
    Rectangle *pipe = malloc(sizeof(Rectangle));
    pipe->w = 60;
    pipe->h = 200 + (top ? -gap : gap);
    pipe->x = WINDOW_WIDTH - 60;
    pipe->y = top ? 0 : WINDOW_HEIGHT - pipe->h;

    pipe->color.hex = 0x006400; // dark green

    return pipe;
}

void createPipe(int i){
    srand(time(NULL));
    int gap = rand() % 201 - 100;

    Rectangle *top = createPipeSegment(TRUE,gap);
    Rectangle *bottom = createPipeSegment(FALSE,gap);

    pipes[i] = top;
    pipes[i+1] = bottom;

    nPipes+=2;

    addRect(top);
    addRect(bottom);
}

void gameSetup(){
    //called once, setup everything
    setBGColor(0x6ca8d6); //sky blue

    bird = malloc(sizeof(Rectangle));
    bird->x = 100;
    bird->y = 0;
    bird->w = 40;
    bird->h = 40;

    bird->color.hex = 0xfcba03; //yellow

    addRect(bird);
    createPipe(0);

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
        bird->y -= 50;
        buffered = FALSE;
    }

    //boundry check
    if(bird->y > WINDOW_HEIGHT){
        bird->y = WINDOW_HEIGHT;
    }
    if(bird->y < 0){
        bird->y = 0;
    }

    //move the pipes
    for(int i=0;i<nPipes;i+=2){
        if(pipes[i] == NULL || pipes[i+1] == NULL){
            continue;
        }

        pipes[i]->x -= 100 * delta_time;
        pipes[i+1]->x -= 100 * delta_time;
    }

    return 0;
}

void gameCleanup(){
    //called once, cleanup everything
    free(bird);
}