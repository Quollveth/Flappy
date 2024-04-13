#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include "./windowManager.h"

#define MAX_PIPES 8

Rectangle *bird;
Rectangle *pipes[MAX_PIPES*2];
int nPipes = 0;

int buffered = FALSE;
int dead = FALSE;
int score = 0;

Rectangle* createPipeSegment(int top,int gap){
    Rectangle *pipe = malloc(sizeof(Rectangle));
    pipe->w = 60;
    pipe->h = 200 + (top ? -gap : gap);
    pipe->x = WINDOW_WIDTH - 60;
    pipe->y = top ? 0 : WINDOW_HEIGHT - pipe->h;

    pipe->color.hex = 0x006400; // dark green

    return pipe;
}

void createPipe(int i,int offset){
    int gap = rand() % 201 - 100;

    Rectangle *top = createPipeSegment(TRUE,gap);
    Rectangle *bottom = createPipeSegment(FALSE,gap);

    top->x += offset;
    bottom->x += offset;

    pipes[i] = top;
    pipes[i+1] = bottom;

    nPipes+=2;

    addRect(top);
    addRect(bottom);
}

void resetPipe(int i){
    pipes[i]->x = WINDOW_WIDTH + 60;
    pipes[i+1]->x = WINDOW_WIDTH + 60;

    int gap = rand() % 201 - 100;

    pipes[i]->h = 200 + gap;
    pipes[i+1]->h = 200 - gap;

    pipes[i+1]->y = WINDOW_HEIGHT - pipes[i+1]->h;
}

void gameSetup(){
    //called once, setup everything
    srand(time(NULL));
    setBGColor(0x6ca8d6); //sky blue

    bird = malloc(sizeof(Rectangle));
    bird->x = 100;
    bird->y = 0;
    bird->w = 40;
    bird->h = 40;

    bird->color.hex = 0xfcba03; //yellow

    addRect(bird);

    for(int i=0;i<MAX_PIPES;i+=2){
        createPipe(i*2,i*200);
    }

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
    printf("Score: %d\n",score);
    
    if (dead) return 0;
    
    //gravity
    bird->y += 200 * delta_time;

    //flap
    if(buffered){
        bird->y -= 50;
        buffered = FALSE;
    }

    //boundry check
    if(bird->y > WINDOW_HEIGHT - bird->h){
        bird->y = WINDOW_HEIGHT - bird->h;
    }
    if(bird->y < bird->h){
        bird->y = bird->h;
    }

    //move the pipes
    for(int i=0;i<nPipes;i+=2){
        if(pipes[i] == NULL || pipes[i+1] == NULL){
            continue;
        }

        pipes[i]->x -= 100 * delta_time;
        pipes[i+1]->x -= 100 * delta_time;

        if(pipes[i]->x + pipes[i]->w/2 < 0){
            resetPipe(i);
            score++;
        }
    }

    //collision check
    for(int i=0;i<nPipes;i+=2){
        if(pipes[i] == NULL || pipes[i+1] == NULL){
            continue;
        }

        if(
            SDL_HasIntersection(&GET_SDL_RECT(bird),&GET_SDL_RECT(pipes[i])) || 
            SDL_HasIntersection(&GET_SDL_RECT(bird),&GET_SDL_RECT(pipes[i+1]))
        ){
            dead = TRUE;
        }
    }

    return 0;
}

void gameCleanup(){
    //called once, cleanup everything
}