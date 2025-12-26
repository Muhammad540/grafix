#include "SDL2/SDL.h"
#include <iostream>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// surface is like a Image datatype 
SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gXout = NULL;

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
    
    gXout = SDL_LoadBMP("x.bmp");
    if (gXout == NULL) {
        std::cout << "Unable to load the image " << "x.bmp ! SDL Error: " << SDL_GetError() << std::endl; 
        success = false;
    }

    return success;
}

void close() {
    SDL_FreeSurface(gXout);
    gXout = NULL;

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
            bool quit = false;
            SDL_Event event;
            while (!quit){
                // SDL_PollEvent returns 0 when the event queue is empty
                while (SDL_PollEvent(&event) != 0){
                    if (event.type == SDL_QUIT){
                        quit = true;
                    }
                }
                /* Double Buffering Logic
                there are two buffers (back and front)
                    1. Update the back buffer 
                    2. Push the blit to the front buffer
                You should first complete all your blits for the current frame and only then you call the update window surface
                */
                SDL_BlitSurface(gXout, NULL, gScreenSurface, NULL);
                SDL_UpdateWindowSurface(gWindow);
            }
        }
    }
    close();
    return 0;
}


