//
// Created by Remus on 29/06/2025.
//

#include "Game.h"

#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "systems/Physics.h"

Game::Game() : currentAppState(SDL_APP_FAILURE), lastFrameTime(0)
{
}

SDL_AppResult Game::Start()
{
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Brick Breaker", 800, 600, SDL_WINDOW_FULLSCREEN, &display.window, &display.renderer))
    {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        currentAppState = SDL_APP_FAILURE;
        return currentAppState;
    }

    systemManager.RegisterSystem<Physics>("physics");
    currentAppState = SDL_APP_CONTINUE;

    return currentAppState;
}

SDL_AppResult Game::Update()
{
    Uint64 currentFrameTime = SDL_GetPerformanceCounter();
    Uint64 frequency = SDL_GetPerformanceFrequency();
    deltaTime = static_cast<float>(currentFrameTime - lastFrameTime) / static_cast<float>(frequency);
    lastFrameTime = currentFrameTime;

    systemManager.Update(deltaTime);

    display.PreRender();
    display.Render();
    display.PostRender();

    return currentAppState;
}

void Game::Shutdown()
{
}

SDL_AppResult Game::GetState()
{
    return currentAppState;
}

void Game::WindowEvent(SDL_Event* event)
{
    if (event->key.key == SDLK_ESCAPE)
    {
        currentAppState = SDL_APP_SUCCESS;
    }
}
