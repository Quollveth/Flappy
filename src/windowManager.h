#include <stdbool.h>
#include <SDL2/SDL.h>
#include "./dynamic_buffer.h"

//#define RENDER_DEBUG

//prototypes for game functions, implemented in main.c
int gameSetup();//called once, setup everything, return 1 for failure
void gameInput(int key);//called every time a key is pressed
int gameLogic(float delta_time);//called every frame, return 1 to stop game 0 to continue
void gameCleanup();//called once, cleanup everything

struct {
    int WIN_HEIGHT;
    int WIN_WIDTH;
    int TARG_FPS;
    int TARG_TT; //target frame time
} gameSettings;

static struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* mainSurface;

    int last_frame_time;
    bool isRunning;

    struct dBuffer toDraw; //all game objects that need to be drawn
} gameState;

enum err_types {
    NONE,
    SDL_INIT,
    WINDOW,
    RENDER,
    SURFACE
};

static int initialize_window() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return (enum err_types)SDL_INIT;
    }
    gameState.window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        gameSettings.WIN_WIDTH,
        gameSettings.WIN_HEIGHT,
        0
    );
    if (!gameState.window) {
        return (enum err_types)WINDOW;
    }
    
    gameState.renderer = SDL_CreateRenderer(gameState.window,-1,0);
    if (!gameState.renderer) {
        return (enum err_types)RENDER;
    }

    gameState.mainSurface = SDL_GetWindowSurface(gameState.window);
    if (!gameState.mainSurface) {
        return (enum err_types)SURFACE;        
    }

    return true;
}

static void closeGame(bool error,enum err_types err_type){
    if(!error) goto full_quit; //all is fine, perform the full routine

    //if we got an error during initialization go to the last performed step
    switch(err_type){
        case (enum err_types)SDL_INIT:
            fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
            goto skip_all; //nothing was initialized
        case (enum err_types)WINDOW:
            fprintf(stderr, "Error creating SDL window: %s\n", SDL_GetError());
            goto sdl_quit;
        case (enum err_types)RENDER:
            fprintf(stderr, "Error creating SDL renderer: %s\n", SDL_GetError());
            goto window;
        case (enum err_types)SURFACE:
            fprintf(stderr, "Error creating window surface: %s\n", SDL_GetError());
            goto window;
        
        default:
            break;
    }

    full_quit:
    gameCleanup();
    //cleanup gameObjects
    //for(int i=0;i<gameState.toDraw.len;i++){
    //    if(gameState.toDraw.buffer != NULL) free(gameState.toDraw.buffer);
    //}

    SDL_DestroyRenderer(gameState.renderer);
    window:
    SDL_DestroyWindow(gameState.window);
    sdl_quit:
    SDL_Quit();
    skip_all:
    exit(error);
}

static void setup(){

    //read settings file in here, for now they are hardcoded
    gameSettings.TARG_FPS = 30;
    gameSettings.TARG_TT = 1000/gameSettings.TARG_FPS;
    gameSettings.WIN_WIDTH = 800;
    gameSettings.WIN_HEIGHT = 600;

    //set initial state
    gameState.window = NULL;
    gameState.renderer = NULL;
    gameState.mainSurface = NULL;

    gameState.isRunning = false;
    gameState.last_frame_time = 0;

    gameState.toDraw = DBUF_INIT;

    //initialize
    int initialization = initialize_window();

    if(!initialization) closeGame(true,initialization); //if something done fucked up stop now

    //let the game setup it's own stuff    
    gameState.isRunning = !gameSetup(); //returns 1 on failure
}

static void process_input(){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type){
    
    case SDL_QUIT:
        gameState.isRunning = false;
        break;
    
    case SDL_KEYDOWN:
        gameInput(event.key.keysym.sym); //send input to game
        break;

    default:
        break;
    }
}

static int update(){
    // if we are ahead wait for a bit to keep the target fps
    int time_to_wait = gameSettings.TARG_TT - (SDL_GetTicks() - gameState.last_frame_time);
    if(time_to_wait > 0 && time_to_wait <= gameSettings.TARG_TT){
        SDL_Delay(time_to_wait);
    }

    float delta_time = (SDL_GetTicks() - gameState.last_frame_time) / 1000.0f;
    gameState.last_frame_time = SDL_GetTicks();

    return gameLogic(delta_time); //execute game logic, it will return a 1 if it wishes to close
}

static void render(){
    //black screen
    SET_COLOR(gameState.renderer,(Color){0x000000});
    SDL_RenderClear(gameState.renderer);

    //render game objects
    for(int i=0;i < gameState.toDraw.len;i++){
        for(int j=0;j < gameState.toDraw.buffer[i]->partCount; j++){
        #ifndef RENDER_DEBUG

        SDL_RenderCopyEx(
            gameState.renderer,
            gameState.toDraw.buffer[i]->parts[j]->sprite->spriteTexture,
            NULL,
            gameState.toDraw.buffer[i]->parts[j]->bounds,
            0.0,
            NULL,
            gameState.toDraw.buffer[i]->flipHorizontal?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE
        );

        #else
        SET_COLOR(gameState.renderer,(Color){0xFF00FF});
        SDL_RenderFillRect(
            gameState.renderer,
            gameState.toDraw.buffer[i]->parts[j]->bounds
        );

        #endif
        }
    }

    SDL_RenderPresent(gameState.renderer);
}

Sprite* loadSprite(char* path){
    Sprite* newSprite = initializeSprite(gameState.renderer,path);
    if(newSprite == NULL) return NULL;
    //TODO: Proper error handling

    return newSprite;
}

/**
 * Creates a new GameObject with the specified Sprite and dimensions.
 *
 * @param firstPart The Sprite for the first part of the GameObject.
 * @param w The width of the GameObject.
 * @param h The height of the GameObject.
 * @return A pointer to the newly created GameObject, or NULL if an error occurred.
 */
GameObject* createGameObject(Sprite* firstPart,int w,int h){
    GameObject* newObj = initializeObject(gameState.renderer);
    if(newObj == NULL) return NULL;

    if(firstPart != NULL){
        addObjectPart(
            gameState.renderer,
            newObj,
            firstPart,
            0,
            0,
            h,
            w
        );
    }

    dbAppend(&gameState.toDraw,newObj);
    return newObj;
}

/**
 * Adds a new part to a game object
 * 
 * @param obj The game object to add the part to.
 * @param sprite The sprite to use for the part.
 * @param xOffset The x offset of the part relative to the game object's position.
 * @param yOffset The y offset of the part relative to the game object's position.
 * @param spriteHeight The height of the sprite.
 * @param spriteWidth The width of the sprite.
 * @return Returns 0 on success, 1 on failure.
**/
int buildGameObject(GameObject* obj,Sprite* newPart,int xOffset, int yOffset,int spriteHeight, int spriteWidth){
    if(obj == NULL) return 1;
    if(newPart == NULL) return 1;
    //TODO: Proper error handling

    return addObjectPart(
        gameState.renderer,
        obj,
        newPart,
        xOffset,
        yOffset,
        spriteHeight,
        spriteWidth
    );
}

/*
* Wrapper for destroyObject function that removes it from drawList first
* Remember to separately free the sprites with destroySprite
*/
void destroyGameObject(GameObject* obj){
    dbRemoveById(&gameState.toDraw,obj->id);
    destroyObject(obj);
}

int main() {
    setup();

    while(gameState.isRunning){
        process_input();
        if (update()){
            break;
        }
        render();
    }

    closeGame(false,NONE);
    return 0;
}