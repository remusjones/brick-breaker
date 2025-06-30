//
// Created by Remus on 29/06/2025.
//

#include "Game.h"

#include "LevelSystem.h"
#include "RenderSystem.h"
#include "SDL3/SDL_log.h"
#include "SDL3/SDL_timer.h"
#include "systems/PhysicsSystem.h"
#include <SDL3/SDL.h>

Game::Game() : currentAppState(SDL_APP_FAILURE), lastFrameTime(0), deltaTime(0), levelSystem(nullptr)
{
}

SDL_AppResult Game::Start()
{
    currentAppState = display.Init();
    if (currentAppState != SDL_APP_CONTINUE)
        return currentAppState;

    ecs = std::make_unique<HelloECS>();
    systemManager.RegisterSystem<PhysicsSystem>("physics", ecs.get());
    systemManager.RegisterSystem<RenderSystem>("render", &display, ecs.get());
    levelSystem = systemManager.RegisterSystem<LevelSystem>("level", &display, ecs.get());


    systemManager.Init();
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
    systemManager.Shutdown();
}

void Game::CreateLevel()
{

}

SDL_AppResult Game::GetState() const
{
    return currentAppState;
}

void Game::WindowEvent(SDL_Event* event)
{
    if (event->key.key == SDLK_ESCAPE)
    {
        currentAppState = SDL_APP_SUCCESS;
    }

    if (levelSystem && event->motion.type == SDL_EVENT_MOUSE_MOTION)
    {
        levelSystem->SetMouseLocation(event->motion.x, event->motion.y);
    }
}
