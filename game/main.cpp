#include <iostream>
#include <SDL2/SDL.h>

const int s_WIDTH = 1024;
const int s_HEIGHT = 512;
const int pSize = 32;
	
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
	
int pX = s_WIDTH / 2 - pSize / 2;
int pY = s_HEIGHT / 2 - pSize / 2;

bool sdlInit()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) 
	{
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
        }
	window = SDL_CreateWindow("game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, s_WIDTH, s_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == nullptr) 
	{
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
        }
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    	if (renderer == nullptr) 
	{
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    	}
	
	return true;
}
void events(SDL_Event& event)
{
	while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            exit(0);
        }
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    if(pY - 8 >= 0){
			    pY -= 8;
		    }
                    break;
                case SDLK_DOWN:
                    if (pY + pSize + 8 <= s_HEIGHT) {
                        pY += 8;
                    }
                    break;
                case SDLK_LEFT:
                    if (pX - 8 >= 0) {
                        pX -= 8;
                    }
                    break;
                case SDLK_RIGHT:
                    if (pX + pSize + 8 <= s_WIDTH) {
                        playerX += 8;
                    }
                    break;
            }
        }
    }
}
void gameUpdate(){}
void gameRender()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect playerRect = { pX, pY, pSize, pSize };
    SDL_RenderFillRect(renderer, &playerRect);

    SDL_RenderPresent(renderer);
}
void cleanup() 
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
int main() {
    if (!sdlInit()) {
        std::cerr << "SDL initialization failed. Exiting..." << std::endl;
        return -1;
    }

    SDL_Event event;
	
    bool quit = false;
    while (!quit) {
        events(event);
        gameUpdate();
        gameRender();
    }

    cleanup();
    return 0;
}
