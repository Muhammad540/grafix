#include "SDL2/SDL.h"
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// surface is like a Image datatype 
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloWorld = NULL;

/* starts up SDL and creates a window*/
bool init() {
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not be initialized! SDL ERROR: " << SDL_GetError() << std::endl;
        success = false;
    } else {
        gWindow = SDL_CreateWindow("Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL) {
            std::cout << "Window could not be created ! SDL Error" << SDL_GetError() << std::endl;
            success = false;
        } else {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }        
    }

    return success;
}

bool loadMedia() {
    bool success = true;
    
    gHelloWorld = SDL_LoadBMP("hello_world.bmp");
    if (gHelloWorld == NULL) {
        std::cout << "Unable to load the image " << "hello_world.bmp ! SDL Error: " << SDL_GetError() << std::endl; 
        success = false;
    }

    return success;
}

void close() {
    SDL_FreeSurface(gHelloWorld);
    gHelloWorld = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}

int main(){
    if (!init()){
        std::cout << "Failed to Initialize !" << std::endl;
    } else {
        if (!loadMedia()){
            std::cout << "Failed to load media !" << std::endl;
        } else {
            // update the back buffer
            SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
            // pushes the blit to the front 
            // you only display to the screen after all your blits for the current frame are completed
            // then you call the update window surface 
            SDL_UpdateWindowSurface(gWindow);
            // HACK ! to keep the window up
            SDL_Event e;
            bool quit = false;
            while (quit == false){
                while (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) quit = true;
                }
            }
        }
    }
    close();
    return 0;
}


