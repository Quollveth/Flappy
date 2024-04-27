#include <stdbool.h>
#include <SDL2/SDL.h>
#include "./dynamic_buffer.h"
#include <SDL2/SDL_ttf.h>

//#define RENDER_DEBUG

//prototypes for game functions, implemented in main.c
int gameSetup();//called once, setup everything, return 1 for failure
void gameInput(int key);//called every time a key is pressed
int gameLogic(float delta_time);//called every frame, return 1 to quit game 0 to continue
void gameCleanup();//called once, cleanup everything

/*
* Game settings read from ini file
*/
struct {
    int WIN_HEIGHT;
    int WIN_WIDTH;
    int TARG_FPS;
    int TARG_TT; //target frame time
} gameSettings;
/*
* Internal state of the renderer
*/
static struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* mainSurface;

    int last_frame_time;
    bool isRunning;

    struct dBuffer toDraw; //all game objects that need to be drawn
} screenState;

enum err_types {
    NONE,
    SDL_INIT,
    WINDOW,
    RENDER,
    SURFACE
};

#define DEFAULT_TEXT_SIZE 24
#define MAX_TEXT_BUFFER 1024 //max text buffer



static int initialize_window() {
    if(TTF_Init() != 0){
        return (enum err_types)SDL_INIT;
    }
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        return (enum err_types)SDL_INIT;
    }
    screenState.window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        gameSettings.WIN_WIDTH,
        gameSettings.WIN_HEIGHT,
        0
    );
    if (!screenState.window) {
        return (enum err_types)WINDOW;
    }
    
    screenState.renderer = SDL_CreateRenderer(screenState.window,-1,0);
    if (!screenState.renderer) {
        return (enum err_types)RENDER;
    }

    screenState.mainSurface = SDL_GetWindowSurface(screenState.window);
    if (!screenState.mainSurface) {
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
    for(int i=0;i<screenState.toDraw.len;i++){
        if(screenState.toDraw.buffer[i] == NULL) continue;
        destroyObject(screenState.toDraw.buffer[i]);
        screenState.toDraw.buffer[i] = NULL;
    }

    //labels are used for initialization errors only
    SDL_DestroyRenderer(screenState.renderer);
    window:
    SDL_DestroyWindow(screenState.window);
    sdl_quit:
    SDL_Quit();
    skip_all:
    exit(error);

    //TODO: find what's leaking memory
}

static void setup(){

    //read settings file in here, for now they are hardcoded
    gameSettings.TARG_FPS = 30;
    gameSettings.TARG_TT = 1000/gameSettings.TARG_FPS;
    gameSettings.WIN_WIDTH = 800;
    gameSettings.WIN_HEIGHT = 600;

    //set initial state
    screenState.window = NULL;
    screenState.renderer = NULL;
    screenState.mainSurface = NULL;

    screenState.isRunning = false;
    screenState.last_frame_time = 0;

    screenState.toDraw = DBUF_INIT;

    //initialize
    int initialization = initialize_window();

    if(!initialization) closeGame(true,initialization); //if something done fucked up stop now

    //let the game setup it's own stuff    
    screenState.isRunning = !gameSetup(); //returns 1 on failure
}

static void process_input(){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type){
    
    case SDL_QUIT:
        screenState.isRunning = false;
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
    int time_to_wait = gameSettings.TARG_TT - (SDL_GetTicks() - screenState.last_frame_time);
    if(time_to_wait > 0 && time_to_wait <= gameSettings.TARG_TT){
        SDL_Delay(time_to_wait);
    }

    float delta_time = (SDL_GetTicks() - screenState.last_frame_time) / 1000.0f;
    screenState.last_frame_time = SDL_GetTicks();

    return gameLogic(delta_time); //execute game logic, it will return a 1 if it wishes to close
}

static void render(){
    //black screen
    SET_COLOR(screenState.renderer,(Color){0x7BC5ED});
    SDL_RenderClear(screenState.renderer);

    //render game objects
    for(int i=0;i < screenState.toDraw.len;i++){
        if(screenState.toDraw.buffer[i]->render == false) continue;
        for(int j=0;j < screenState.toDraw.buffer[i]->partCount; j++){

        if(screenState.toDraw.buffer[i] == NULL) continue;
        if(screenState.toDraw.buffer[i]->parts[j] == NULL) continue;

        SDL_RenderCopyEx(
            screenState.renderer,
            screenState.toDraw.buffer[i]->parts[j]->sprite,
            NULL,
            screenState.toDraw.buffer[i]->parts[j]->bounds,
            0.0,
            NULL,
            screenState.toDraw.buffer[i]->parts[j]->flip
        );

        #ifdef RENDER_DEBUG
        debug_render:
        SET_COLOR(screenState.renderer,(Color){0xEBC634});
        SDL_RenderDrawRect(
            screenState.renderer,
            screenState.toDraw.buffer[i]->parts[j]->bounds
        );
        #endif
        }
        #ifdef RENDER_DEBUG
        SET_COLOR(screenState.renderer,(Color){0xEB34C3});
        SDL_RenderDrawRect(
            screenState.renderer,
            screenState.toDraw.buffer[i]->bounds
        );
        #endif
    }

    SDL_RenderPresent(screenState.renderer);
}

/** Wrappers for Gameobject functions **/

/**
 * Loads a sprite from the given file path.
 *
 * @param path The file path of the sprite image.
 * @return A pointer to the loaded Sprite object, or NULL if an error occurred.
 */
Sprite loadSprite(char* path){
    Sprite newSprite = initializeSprite(screenState.renderer,path);
    if(newSprite == NULL) return NULL;
    //TODO: Proper error handling

    return newSprite;
}

/**
 * Creates a new GameObject with the specified Sprite and dimensions.
 *
 * @param firstPart The Sprite for the first part of the GameObject or NULL for invisible object
 * @param w The width of the GameObject.
 * @param h The height of the GameObject.
 * @return A pointer to the newly created GameObject, or NULL if an error occurred.
 */
GameObject* createGameObject(Sprite firstPart,int w,int h){
    GameObject* newObj = initializeObject(screenState.renderer);
    if(newObj == NULL) return NULL;

    if(firstPart != NULL){
        addObjectPart(
            screenState.renderer,
            newObj,
            firstPart,
            0,
            0,
            h,
            w
        );
    }

    dbAppend(&screenState.toDraw,newObj);
    return newObj;
}

/**
 * Adds a new part to a game object
 * Wrapper for addObjectPart() with error handling
 * 
 * @param obj The game object to add the part to.
 * @param sprite The sprite to use for the part.
 * @param xOffset The x offset of the part relative to the game object's position.
 * @param yOffset The y offset of the part relative to the game object's position.
 * @param spriteHeight The height of the sprite.
 * @param spriteWidth The width of the sprite.
 * @return Returns 0 on success, 1 on failure.
**/
int buildGameObject(GameObject* obj,Sprite newPart,int xOffset, int yOffset,int spriteHeight, int spriteWidth){
    if(obj == NULL) return 1;
    if(newPart == NULL) return 1;
    //TODO: Proper error handling

    return addObjectPart(
        screenState.renderer,
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
    dbRemoveById(&screenState.toDraw,obj->id);
    destroyObject(obj);
}

/** Text handling **/
Sprite vscreateTextSprite(SDL_Renderer* target ,TTF_Font* font, Color color, const char* fmt, va_list ap){
    char buffer[MAX_TEXT_BUFFER] = {0};

    SDL_vsnprintf(buffer,MAX_TEXT_BUFFER,fmt,ap);

    SDL_Surface* textSurface = TTF_RenderText_Solid(
        font,
        buffer,
        CONVERT_COLOR(color)
    );
    if(textSurface == NULL){
        //TODO: Proper error handling
        return NULL;
    }

    Sprite textTexture = SDL_CreateTextureFromSurface(target,textSurface);
    if(textTexture == NULL){
        //TODO: Proper error handling
    }
    SDL_FreeSurface(textSurface);
    return textTexture;
}


/**
 * Creates a Sprite containing formatted text using the given font, color, and format string.
 *
 * @param target The renderer to create the texture on.
 * @param font The font to use for rendering the text.
 * @param color The color of the text.
 * @param fmt The format string for the text.
 * @param ... Additional arguments for the format string.
 * @return The created Sprite, or NULL if an error occurred.
 */
Sprite createTextSprite(SDL_Renderer* target, TTF_Font* font, Color color, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

    Sprite textTexture = vscreateTextSprite(target, font, color, fmt, args);

    va_end(args);

    return textTexture;
}

/**
 * Creates a Game Object containing formatted text using the given font, color, and format string.
 *
 * @param font The font to use for rendering the text.
 * @param color The color of the text.
 * @param fmt The format string for the text.
 * @return The created object, or NULL if an error occurred.
*/

GameObject* createTextObject(TTF_Font* font, Color color, const char* fmt, ...){
    GameObject* newObj = createGameObject(NULL,0,0);
    if(newObj == NULL){
        //TODO: Proper error handling
    }

    va_list args;
    va_start(args,fmt);

    Sprite textTexture = vscreateTextSprite(screenState.renderer, font, color, fmt, args);

    va_end(args);

    if(textTexture == NULL){
        //TODO: Proper error handling
        destroyObject(newObj);
        return NULL;
    }
    int textH, textW;
    SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);

    addObjectPart(
        screenState.renderer,
        newObj,
        textTexture,
        0,
        0,
        textH,
        textW
    );

    return newObj;
}

void updateObjectText(GameObject* obj, TTF_Font* font, Color color, const char* fmt, ...){
    va_list args;
    va_start(args,fmt);

    Sprite textTexture = vscreateTextSprite(screenState.renderer, font, color, fmt, args);

    va_end(args);

    if(textTexture == NULL){
        return;
        //TODO: Proper error handling
    }
    int textH, textW;
    SDL_QueryTexture(textTexture, NULL, NULL, &textW, &textH);

    obj->parts[obj->partCount-1]->sprite = textTexture;
    obj->parts[obj->partCount-1]->bounds->h = textH;
    obj->parts[obj->partCount-1]->bounds->w = textW;
}

//TODO: Move entry point somewhere that makes more sense
int main() {
    setup();

    while(screenState.isRunning){
        process_input();
        if (update()){
            break;
        }
        render();
    }

    closeGame(false,NONE);
    return 0;
}