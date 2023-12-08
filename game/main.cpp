#include <iostream>
#include <SDL2/SDL.h>
#include <thread>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PLAYER_SPEED = 5;

SDL_Window* window;
SDL_Renderer* renderer;

class Player {
public:
    int x, y;

    Player(int startX, int startY) : x(startX), y(startY) {}

    void move(int dx, int dy) {
        x += dx;
        y += dy;
    }

    void render() {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect playerRect = { x, y, 50, 50 };
        SDL_RenderFillRect(renderer, &playerRect);
    }
};

Player player(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

void gameLoop() {
    using namespace std::chrono;

    auto lastTime = high_resolution_clock::now();
    double lag = 0.0;
    const double MS_PER_UPDATE = 16.0; // Update at 60 FPS

    while (true) {
        auto currentTime = high_resolution_clock::now();
        auto elapsedTime = duration_cast<milliseconds>(currentTime - lastTime).count();
        lastTime = currentTime;

        lag += elapsedTime;

        // Input handling
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                exit(0);
            }
        }

        // Update
        while (lag >= MS_PER_UPDATE) {
            // Movement logic (independent of framerate)
            const Uint8* keyState = SDL_GetKeyboardState(NULL);
            int dx = 0, dy = 0;
            if (keyState[SDL_SCANCODE_LEFT])  dx -= 1;
            if (keyState[SDL_SCANCODE_RIGHT]) dx += 1;
            if (keyState[SDL_SCANCODE_UP])    dy -= 1;
            if (keyState[SDL_SCANCODE_DOWN])  dy += 1;

            player.move(dx * PLAYER_SPEED, dy * PLAYER_SPEED);

            lag -= MS_PER_UPDATE;
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        player.render();

        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    window = SDL_CreateWindow("SDL2 RPG Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    std::thread gameThread(gameLoop);

    // Wait for the user to close the window
    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            }
        }
    }

    gameThread.join(); // Wait for the game loop thread to finish

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
