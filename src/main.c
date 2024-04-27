#include <time.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./windowManager.h"

#define birdAsset "./assets/bird.bmp"
//50x35
#define pipeMiddleAsset "./assets/pipe_middle.bmp"
#define pipeEndAsset "./assets/pipe_end.bmp"

#define fontAsset "./assets/VT323/VT323.ttf"

#define PIPE_DISTANCE 300 //distance between pipes
#define PIPE_GAP 150 //gap between top and bottom pipe

typedef struct {
    GameObject* top;
    GameObject* bottom;
    int xPos;
}Pipe; //there is no way for a object to have multiple bounding boxes but i still want pipes to be one thing instead of two separate objects

//gamestate things for this file
Sprite birdSprite;
Sprite pipeEndSprite;
Sprite pipeMiddleSprite;

GameObject* bird;

TTF_Font* VT3;
GameObject* scoreBoard;

GameObject* pauseText;

int maxPipeSegments; //how many segments are needed to fill the screen
int maxPipes;
Pipe* *pipes; //Pipe* pipes[maxPipes] but that's a dynamic value

int score;
bool paused;
bool dead;

inline static int randomNumber(int cap) {return rand() % cap;}

Pipe* createPipe(int xPos){
    Pipe* newPipe = malloc(sizeof(Pipe));
    // place a random amount of pipes at the bottom
    // skip the gap distance
    // finish with the top pipe 

    int nPipes = randomNumber(maxPipeSegments - 2); //2 extra for the ends

    //first the bottom one
    newPipe->bottom = createGameObject(pipeEndSprite,50,50);
    moveSimpleObject(newPipe->bottom,xPos,gameSettings.WIN_HEIGHT - ((nPipes+1)*50));

    for (int i = 1; i <= nPipes; i++) {
        buildGameObject(
            newPipe->bottom,
            pipeMiddleSprite,
            0,
            50 * i, //add pipes below the end piece
            50,
            50
        );
    }

    //now the top one
    newPipe->top = createGameObject(pipeEndSprite,50,50);
    nPipes = maxPipeSegments - 2 - nPipes;
    moveSimpleObject(newPipe->top,xPos,nPipes * 50);
    newPipe->top->parts[0]->flip = SDL_FLIP_VERTICAL;

    for (int i = 1; i <= nPipes; i++) {
        buildGameObject(
            newPipe->top,
            pipeMiddleSprite,
            0,
            -50, //add pipes above, each new piece moves the bounding box Y position so no multiplication is needed
            50,
            50
        );
    }

    newPipe->xPos = xPos;
    return newPipe;
}

void destroyPipe(Pipe* pipe){
    destroyGameObject(pipe->top);
    destroyGameObject(pipe->bottom);
    free(pipe);
}

inline static void movePipe(Pipe* pipe,int newX){
    pipe->xPos = newX;
    moveSplitObject(pipe->top,newX,0);
    moveSplitObject(pipe->bottom,newX,pipe->bottom->bounds->y);
}

int gameSetup(){
    //called once, setup everything, return 1 for failure
    srand(time(NULL));
    score = 0;
    paused = false;

    birdSprite = loadSprite(birdAsset);
    pipeEndSprite = loadSprite(pipeEndAsset);
    pipeMiddleSprite = loadSprite(pipeMiddleAsset);

    maxPipeSegments = (gameSettings.WIN_HEIGHT - PIPE_GAP)/50;

    bird = createGameObject(birdSprite,50,35);
    moveSimpleObject(bird,100,50);

    maxPipes = (gameSettings.WIN_WIDTH/PIPE_DISTANCE)+1;

    pipes = malloc(sizeof(Pipe*)*maxPipes);

    for (int i = 0; i < maxPipes; i++) {
        pipes[i] = createPipe(gameSettings.WIN_WIDTH + (i*PIPE_DISTANCE));
    }

    VT3 = TTF_OpenFont(fontAsset,32);
    scoreBoard = createTextObject(
        VT3,
        (Color){0x000000},
        "Score:"
    );

    moveSimpleObject(scoreBoard,10,10);

    pauseText = createTextObject(
        VT3,
        (Color){0x000000},
        "Game is paused"
    );
    pauseText->render = false;

    moveSimpleObject(
        pauseText,
        gameSettings.WIN_WIDTH/2 - pauseText->bounds->w/2,
        gameSettings.WIN_HEIGHT/2 - pauseText->bounds->h/2
    );
    return 0;
}

void resetGame(){
    moveSimpleObject(bird,100,50);

    for (int i = 0; i < maxPipes; i++){
        destroyPipe(pipes[i]);
        pipes[i] = createPipe(gameSettings.WIN_WIDTH + (i*PIPE_DISTANCE));
    }

    score = 0;
    paused = false;
    updateObjectText(
        pauseText,
        VT3,
        (Color){0x000000},
        "Game is paused"
    );
    pauseText->render = false;

    moveSimpleObject(
        pauseText,
        gameSettings.WIN_WIDTH/2 - pauseText->bounds->w/2,
        gameSettings.WIN_HEIGHT/2 - pauseText->bounds->h/2
    );

    dead = false;
    paused = false;
}

void pauseGame(){
    paused = !paused;
    pauseText->render = paused;
}

int quit = 0;
void gameInput(int key){
    //called every time a key is pressed
    if (key == SDLK_SPACE) {
        if(paused) return;
        if(dead){
            resetGame();
            return;
        }
        moveSimpleObject(bird, bird->bounds->x, bird->bounds->y - 50);
    }
    if (key == SDLK_ESCAPE) {
        if(dead){
            quit = 1;
            return;
        }
        pauseGame();
    }
}

void die(){
    dead = true;
    updateObjectText(
        pauseText,
        VT3,
        (Color){0x000000},
        "Press esc to quit or space to continue"
    );
    moveSimpleObject(
        pauseText,
        gameSettings.WIN_WIDTH/2 - pauseText->bounds->w,
        pauseText->bounds->y
    );
    pauseText->render = true;
}

int gameLogic(float delta_time){
    //called every frame, return 1 to quit game 0 to continue
    if(paused) return 0;
    if(dead) return quit;

    updateObjectText(
        scoreBoard,
        VT3,
        (Color){0x000000},
        "Score: %d",score
    );

    static int lastScored = -1;

    // bird gravity
    moveSimpleObject(bird,bird->bounds->x,bird->bounds->y + 150 * delta_time);

    if (bird->bounds->y < 0) {
        bird->bounds->y = 0;
    } else if (bird->bounds->y + bird->bounds->h > gameSettings.WIN_HEIGHT) {
        bird->bounds->y = gameSettings.WIN_HEIGHT - bird->bounds->h;
    }    

    for (int i = 0; i < maxPipes; i++) {
        if(pipes[i] == NULL) continue;

        movePipe(pipes[i],pipes[i]->xPos - (100 * delta_time));

        //bird collision check
        if(pipes[i]->xPos > 100 && pipes[i]->xPos < 200){
            if(
                SDL_HasIntersection(bird->bounds,pipes[i]->top->bounds)
                ||
                SDL_HasIntersection(bird->bounds,pipes[i]->bottom->bounds)
            ){
                die();
            }
        }

        if(pipes[i]->xPos < 100 && i != lastScored){
            lastScored = i;
            score++;
        }

        //wrap around
        if(pipes[i]->xPos < -50){
            destroyPipe(pipes[i]);
            pipes[i] = createPipe(gameSettings.WIN_WIDTH);
            continue;
        }
    }

    return 0;
}

void gameCleanup(){
    //called once, cleanup everything
    destroyGameObject(pauseText);
    destroyGameObject(scoreBoard);
    TTF_CloseFont(VT3);

    for (int i = 0; i < maxPipes; i++){
        if(pipes[i] == NULL) continue;
        destroyPipe(pipes[i]);
    }

    destroyGameObject(bird);

    DESTROY_SPRITE(pipeMiddleSprite);
    DESTROY_SPRITE(pipeEndSprite);
    DESTROY_SPRITE(birdSprite);
}