#include <SDL2/SDL.h>
#include <iostream>

void close(SDL_Window* window){
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void wait(int milliseconds){
    SDL_Delay(milliseconds);
}

void init(){
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << SDL_GetError() << std::endl;
        exit(1);
    }
}

SDL_Window* create_window(const char* title, int width, int height){
    SDL_Window* window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << SDL_GetError() << std::endl;
        close(window);
    }    
    
    return window;
}

SDL_Renderer* create_renderer(SDL_Window* window){
    return SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}