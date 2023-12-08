#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <thread>
#include <vector>

// Screen dimensions
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Player properties
const int PLAYER_SIZE = 32;
const float PLAYER_SPEED = 200.0f;

// SDL window and renderer
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// Player position
float playerX = SCREEN_WIDTH / 2 - PLAYER_SIZE / 2;
float playerY = SCREEN_HEIGHT / 2 - PLAYER_SIZE / 2;

// Background texture
SDL_Texture* backgroundTexture = nullptr;

// Function to initialize SDL
bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("Multithreaded Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initialize SDL_image for loading image files
    if (IMG_Init(IMG_INIT_PNG) < 0) {
        std::cerr << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        return false;
    }

    return true;
}

// Function to load the background image
bool loadBackground() {
    SDL_Surface* surface = IMG_Load("background.png");
    if (surface == nullptr) {
        std::cerr << "Failed to load background image: " << IMG_GetError() << std::endl;
        return false;
    }

    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (backgroundTexture == nullptr) {
        std::cerr << "Failed to create background texture: " << SDL_GetError() << std::endl;
        return false;
    }

    return true;
}

// Function to handle events
void handleEvents(SDL_Event& event) {
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            // User clicks the close button
            exit(0);
        }
        else if (event.type == SDL_KEYDOWN) {
            // Handle key presses
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                    playerY -= PLAYER_SPEED * getDeltaTime();
                    break;
                case SDLK_DOWN:
                    playerY += PLAYER_SPEED * getDeltaTime();
                    break;
                case SDLK_LEFT:
                    playerX -= PLAYER_SPEED * getDeltaTime();
                    break;
                case SDLK_RIGHT:
                    playerX += PLAYER_SPEED * getDeltaTime();
                    break;
            }
        }
    }
}

// Function to get the time elapsed since the last frame
float getDeltaTime() {
    static Uint32 prevTicks = SDL_GetTicks();
    Uint32 currentTicks = SDL_GetTicks();
    Uint32 deltaTicks = currentTicks - prevTicks;
    prevTicks = currentTicks;

    // Convert milliseconds to seconds
    return static_cast<float>(deltaTicks) / 1000.0f;
}

// Function to update game logic
void updateGame() {
    // Add game logic here
}

// Function to render the game
void renderGame() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw the background
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    // Draw the player
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect playerRect = { static_cast<int>(playerX), static_cast<int>(playerY), PLAYER_SIZE, PLAYER_SIZE };
    SDL_RenderFillRect(renderer, &playerRect);

    // Present the renderer
    SDL_RenderPresent(renderer);
}

// Function to clean up resources
void cleanup() {
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

int main() {
    if (!initSDL()) {
        std::cerr << "SDL initialization failed. Exiting..." << std::endl;
        return -1;
    }

    if (!loadBackground()) {
        std::cerr << "Failed to load background image. Exiting..." << std::endl;
        cleanup();
        return -1;
    }

    SDL_Event event;

    // Number of threads to use
    const unsigned int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;

    // Game loop
    bool quit = false;
    while (!quit) {
        handleEvents(event);

        // Spawn threads for game logic and rendering
        for (unsigned int i = 0; i < numThreads; ++i) {
            threads.emplace_back([&]() {
                updateGame();
            });
        }

        // Wait for all threads to finish
        for (auto& thread : threads) {
            thread.join();
        }
        threads.clear();

        renderGame();
    }

    cleanup();
    return 0;
}
