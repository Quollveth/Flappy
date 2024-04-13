#include <SDL2/SDL.h>
#include "./helpers.h"

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH  800
#define TARGET_FPS 60

#define addRect(rect) dbAppend(&toDraw, &(rect))
#define removeRect(i) dbRemove(&toDraw, i)
#define setBGColor(color) (BG_COLOR = (color))

/**** INTERNAL ****/

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

#define FRAME_TT 1000/TARGET_FPS
#define FALSE 0
#define TRUE 1
#define getkey key.keysym.sym

Color BG_COLOR = {0x000000};
int last_frame_time = 0;
int isRunning = FALSE;

static struct dBuffer toDraw = DBUF_INIT;

void gameSetup();//called once, setup everything
void gameInput(int key);//called every time a key is pressed
int gameLogic(float delta_time);//called every frame, return 1 to stop game 0 to continue

int initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
        return FALSE;
    }
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );
    if (!window) {
        fprintf(stderr, "Error creating SDL window: %s\n", SDL_GetError());
        return FALSE;
    }
    
    renderer = SDL_CreateRenderer(window,-1,0);
    if (!renderer) {
        fprintf(stderr, "Error creating SDL renderer: %s\n", SDL_GetError());
        return FALSE;
    }

    return TRUE;
}

void destroy_window(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void setup(){
    gameSetup();
}

void process_input(){
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type){
    
    case SDL_QUIT:
        isRunning = FALSE;
        break;

    case SDL_KEYDOWN:
        if (event.getkey == SDLK_ESCAPE){
            isRunning = FALSE;
            break;
        }
        gameInput(event.getkey);
        break;

    default:
        break;
    }
}

int update(){
    int time_to_wait = FRAME_TT - (SDL_GetTicks() - last_frame_time);
    if(time_to_wait > 0 && time_to_wait <= FRAME_TT){
        SDL_Delay(time_to_wait);
    }
    
    float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0f;
    last_frame_time = SDL_GetTicks();

    return gameLogic(delta_time);
}

void render(){
    SET_COLOR(renderer,BG_COLOR);
    SDL_RenderClear(renderer);

    for(int i=0;i<toDraw.len;i++){
        
    }

    SDL_RenderPresent(renderer);
}

int main() {
    isRunning = initialize_window();
    setup();

    while(isRunning){
        process_input();
        if (update()){
            break;
        }
        render();
    }

    for(int i=0;i<toDraw.len;i++){
        printRect(&GET_SDL_RECT(toDraw.buffer[i]));
    }

    destroy_window();
    return 0;
}