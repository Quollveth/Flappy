#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./windowManager.h"

#define birdAsset "./assets/bird.bmp"
//50x35

#define pipeMiddleAsset "./assets/pipe_middle.bmp"
#define pipeEndAsset "./assets/pipe_end.bmp"


//a pipe is made of several segments, each one being it's own object, it fills an entire column of the screen except for a gap that starts at the specified height
//all segments being wrapped in a struct with it's own bounding rect makes collision check easier as only the two rects need to be checked over each segment
//boundsT is top pipe, boundsB for bottom pipe
typedef struct pipe{
    struct dBuffer segments;
    int gap; //gap height
    SDL_Rect boundsT;
    SDL_Rect boundsB;
} Pipe;

//game objects are globals
gameObject* bird = NULL;
Pipe* test;
int maxPipes;

Pipe* *pipes; //would be pipes[maxPipes] but that value is dependent on the screen size

#define PIPE_DISTANCE 400 //distance between pipes
#define PIPE_GAP 200 //gap between top and bottom pipe

void debugRender(SDL_Renderer* aaa){
    SDL_SetRenderDrawColor(aaa, 255, 0, 0, 255);
    SDL_RenderFillRect(aaa,&test->boundsB);

    SDL_SetRenderDrawColor(aaa, 0, 255, 0, 255);
    SDL_RenderFillRect(aaa,&test->boundsT);

    SDL_SetRenderDrawColor(aaa, 0, 0, 255, 255);
    SDL_RenderFillRect(aaa,bird->bounds);
}

Pipe* createPipe(int gap){
    Pipe* pipe = malloc(sizeof(Pipe));
    if(pipe == NULL) return NULL;

    pipe->segments = (struct dBuffer){NULL,0};

    //create pipe head, then loop add more segments until we leave the screen

    //bottom pipe
    gameObject* pipeEndBt = createGameObject(pipeEndAsset);
    if(pipeEndBt == NULL) return NULL;
    resizeObject(pipeEndBt,50,50);
    pipeEndBt->bounds->y = gap;

    dbAppend(&pipe->segments,pipeEndBt);

    for(int i=gap+50;i<=gameSettings.WIN_HEIGHT;i+=50){
        gameObject* newSegment = createGameObject(pipeMiddleAsset);
        if(newSegment == NULL){
            //destroy pipe
            return NULL;
        }
        resizeObject(newSegment,50,50);
        newSegment->bounds->y = i;

        dbAppend(&pipe->segments,newSegment);
    }
    
    //top pipe
    gameObject* pipeEndTp = createGameObject(pipeEndAsset);
    if(pipeEndTp == NULL) return NULL;
    resizeObject(pipeEndTp,50,50);
    pipeEndTp->bounds->y = gap - PIPE_GAP;
    pipeEndTp->flipped = SDL_FLIP_VERTICAL;

    dbAppend(&pipe->segments,pipeEndTp);

    for(int i=gap - 50 - PIPE_GAP;i>= 0;i-=50){
        gameObject* newSegment = createGameObject(pipeMiddleAsset);
        if(newSegment == NULL){
            //destroy pipe
            return NULL;
        }
        resizeObject(newSegment,50,50);
        newSegment->bounds->y = i;

        dbAppend(&pipe->segments,newSegment);
    }

    pipe->boundsB.w = 50;
    pipe->boundsT.w = 50;

    pipe->boundsB.y = gap;
    pipe->boundsB.h = gameSettings.WIN_HEIGHT - gap;
    
    pipe->boundsT.y = 0;
    pipe->boundsT.h = gap - 150;

    return pipe;
}

void destroyPipe(Pipe** pipe){

}

void movePipe(Pipe** pipe,int by){
    //goes through every segment and sets their position
    (*pipe)->boundsB.x += by;
    (*pipe)->boundsT.x += by;

    for(int i=0;i<(*pipe)->segments.len;i++){
        (*pipe)->segments.buffer[i]->bounds->x += by;
    }
}

int gameSetup(){
    //called once, setup everything, return 1 for failure
    maxPipes = gameSettings.WIN_WIDTH/PIPE_DISTANCE + 1;
    pipes = malloc(maxPipes * sizeof(Pipe*));

    srand(time(NULL));

    for(int i=0;i<maxPipes;i++){
        pipes[i] = createPipe(300);
        movePipe(&pipes[i],gameSettings.WIN_WIDTH + i*PIPE_DISTANCE);
    }

    bird = createGameObject(birdAsset);
    if(bird == NULL) return 1;

    resizeObject(bird,50,35);
    bird->bounds->x = 100;
    bird->bounds->y = 150;

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

    //bird->bounds->y += 150 * delta_time;

    
    for(int i=0;i<maxPipes;i++){
        movePipe(&pipes[i],-200*delta_time);
        if(pipes[i]->boundsB.x < 150 && pipes[i]->boundsB.x > 100){
            //detect collision
        }
        if(pipes[i]->boundsB.x < -50){
            destroyPipe(&pipes[i]);
            pipes[i] = createPipe(300);
            movePipe(&pipes[i],gameSettings.WIN_WIDTH + 50);
        }
    }


    return 0;
}


void gameCleanup(){
    //called once, cleanup everything
}