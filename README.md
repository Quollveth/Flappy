# Flappy Bird

### Project Info

#### Title:
Flappy Bird in C

#### Author:
Klaus, aka Quollveth 
Github: [Quollveth](https://github.com/Quollveth/)  

City and country: São Paulo, Brazil  
Video recorded on july 14, 2024

#### Description:   
This is a copy of the 'flappy bird' game created in the C programming language.  
It utilizes only the C standard library and the SDL 2 library.  
The player controls the bird that constantly falls and can jump with spacebar and must avoid the pipes as more pipes continuously spawn at the left.

#### Video Demo:  
URL goes here

### Explanation of files and code
The game is composed of 5 files, not counting images for the sprites.

#### gameObjects.h  
Starts with a utility for defining colors using a union datatype, a union places all elements in the same memory space, allowing the struct members to index into the uint32.
This allows the color be usable either as individual rgb components, or as a single hex value. 
The color as a single value is better to pass around and allows the colorpicker in vscode, while the color with individual rgb components is in a better format for SDL2 to use.  
There are also two preprocessor macros to easily convert between this format and the SDL2 color format.  
![Color Union](./assets/readme/color-union.png)
  

Next the game objects are defined.  
Objects have to be made of multiple parts, a part contains a sprite with it's bounds and a option to flip the texture vertically (used for top pipes) or horizontally (not used in this game, but supported by SDL2).  

Objects themselves contain a unique id, an array of parts, and it's own bounds, this allows to easily check collision with an object without checking the bounds of each part. This is used for the pipes as each is composed of several pieces.
![Game Objects](./assets/readme/game-objects.png)  

Next are the functions to manage game objects.
- initializeSprite  
Loads a sprite from a bmp texture  

- destroyObject  
deallocates memory from an object and all it's parts  

- initializeObject  
creates a new empty object and allocates memory for it  

- addObjectPart
adds a new part with a sprite to an object, also used for the very first sprite as the initialization function doesn't give the object one


#### dynamic_buffer.h
This file defines a implementation of a dynamic buffer, or a resizeable array, in base C.  
It stores references to gameObjects allocating more memory as needed.  
![Dynamic Buffer](./assets/readme/d_buffer.png)

This buffer is used in windowManager.h to render objects to the screen.
This file only handles the buffer implementation and the functions to manipulate it.


#### windowManager.h
This is the most important file in the project, handling the game window and drawing objects to the screen, in a way it serves as a "miniature game engine".

The file starts declaring the prototype for the game functions, these are defined in main.c and implement game specific behavior.
The prototypes define what the function is expected to receive and return,
    - gameSetup: sets up the initial state of the game
    - gameInput: receives a pressed key and processes the input for game behavior
    - gameLogic: runs on every frame to update the state of the game before being drawn on to the screen, it receives the time passed since last frame
    - gameCleanup: runs once the game is closed, frees the used memory allocated in gameSetup to prevent leaks

Then two structs are defined, gameSettings holds the configuration for the game, currently only the size of the window and target fps
ScreenState is to hold the SDL2 state, it is defined as static as it's only used in the windowManager.h file and cannot be acessed from outside it.
![Window Manager Declarations](./assets/readme/windowManStart.png)

Next there are the functions to handle the SDL2 state, starting with a initialization and closing function.
Both use a enum to store the specific error caused during SDL2 initialization as attempting to free a unitialized SDL object will cause a crash, so it's important to know the specific error caused during initialization so the gameClose function can properly exit the game.
![Window initialization](./assets/readme/windowInit.png)
![Window closing](./assets/readme/gameClose.png)


Next the functions that manage the window operations, these mimmick the prototype game functions and call the specific game function

##### setup
Initializes the initial state of the game
![Initial setup](./assets/readme/Basesetup.png)

##### process_input
Handles input events using SDL, the SDL_QUIT event (triggered when the close button is pressed on the window, or other similar events like alt-f4, etc) is handled appropriately while keyboard events are passed to the game for processing
![Initial input](./assets/readme/baseInput.png)

##### update
Runs every frame, delays the game if needed to maintain the target fps, calculates delta time and passes it over to the game
![Initial update](./assets/readme/baseUpdate.png)

##### render
Responsible for rendering the game objects to the screen, most of the work is done by SDL2 itself.
First clear the current frame and set the background to a light blue, then loop through each game object and copy it to the render part by part
![Render](./assets/readme/render.png)


##### object wrappers
Next in the file are wrappers for functions provided by gameObjects.h, these facilitate managing gameObjects
![Objects wrappers](./assets/readme/objectWrappers.png)


##### text handling
Next in the file are functions for handling writing text to the screen, text has to be rendered into a texture (done by the SDLTTF library) that gets loaded into a special text gameObject to be rendered as a normal sprite by the render functiom

First the text is written to a texture using a variadic function and a wrapper around it
![Text loading](./assets/readme/textSprite.png)

This sprite can be used to create a gameObject containing the text
![Text object](./assets/readme/textObject.png)

A function is provided to update the text in a object, this involves recreating the texture, but not the object
![Text update](./assets/readme/textUpdate.png)

##### entrypoint
The entrypoint for the program is located in the windowManager so to avoid the game logic passing over the SDL wrappers

The main function is a simple loop that processes input, updates the game state and renders, for as long as the game is running
![Entrypoint](./assets/readme/entrypoint.png)


#### main.c
In here the game logic for the flappy bird game itself is defined, if the other files serve as a primitive game engine this is the game itself.


##### Game definitions
The file starts with global definitions to be used by all functions in this file, this include assets, sprites, objects and variables.
These cannot be declared in a function scope as they need to be acessed by multiple functions since we do not have access to the entrypoint from here, in hindsight this could be handled with a "gameState" struct

The game only has 4 types of sprites it needs to manage, the bird itself, the pipes and the scoreboard, and the text that shows while paused, this is reused for the endscreen text

A pipe is composed of two objects, one for the top pipe and one for the bottom pipe, since it fills the screen vertically only it's x position is needed
![Game definitions](./assets/readme/gameDefinitions.png)


##### Pipe handling
Next the file provides functions for handling the pipes, a pipe is created at the right of the screen, moves to the left where it is destroyed.
Pipes are built from the bottom, first filling a random amount of segments, skipping the gap distance and filling the segments needed for the top pipe
![Pipe functions](./assets/readme/pipeHandling.png)


##### Game setup
Next is the first of the game functions whose protoype was declated in windowManager.h, gameSetup sets up the initial state of the game allocating all the global objects defined above
![Game setup](./assets/readme/gameSetup.png)


##### Pause and resey
Next two simple functions for pausing and resetting the game, gameReset redoes the process from gameSetup without the allocation so all objects are back at their initial state
![Game reset](./assets/readme/gameReset.png)


##### Game input
Next a function to handle keyboard input, called the main loop in window manager when a keyboard event is detected, the game only has two inputs, space to jump and esc to pause/exit the game
![Game input](./assets/readme/gameInput.png)


##### Game logic
Next the function to handle the logic of the game, called by the window manager on every frame, it receives the delta time of the game and handles
 - bird gravity
 - updating the score
 - checking for collision
 - moving, creating and destroying the pipes
![Game logic](./assets/readme/gameLogic.png)


##### Game cleanup
Last is the cleanup function, called when the game exits to free the memory allocated by the setup function, only the and heap allocated objects need to be cleaned up as the global variables are stack allocated and are freed by the os on program exit
![Game cleanup](./assets/readme/gameCleanup.png)


### Controls
- Spacebar | Jump
- Esc      | Pause

## License
Sprites free for non commercial use from [Spriters Resource](https://www.spriters-resource.com/)
VT323 Font licensed under [SIL Open Font License](https://openfontlicense.org/)

This project is licensed under the [MIT License](LICENSE.md).
