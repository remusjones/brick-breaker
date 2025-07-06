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
    if (currentAppState != SDL_APP_CONTINUE){ return currentAppState; }


    // Preregister the ecs types here
    ecs = std::make_unique<HelloECS>();
    ecs->RegisterComponent<Position>();
    ecs->RegisterComponent<Attached>();
    ecs->RegisterComponent<Body>();
    ecs->RegisterComponent<Circle>();
    ecs->RegisterComponent<Dimension>();
    ecs->RegisterComponent<Level>();
    ecs->RegisterComponent<Powerup>();
    ecs->RegisterComponent<Color>();
    ecs->RegisterComponent<Paddle>();
    ecs->RegisterComponent<DestroyTag>();

    systemManager.RegisterSystem<PhysicsSystem>("physics", ecs.get());
    systemManager.RegisterSystem<RenderSystem>("render", &display, ecs.get());
    levelSystem = systemManager.RegisterSystem<LevelSystem>("level", &display, ecs.get(), &inputManager);

    lastFrameTime = SDL_GetTicks();
    systemManager.Init();
    return currentAppState;
}

SDL_AppResult Game::Update()
{
    Uint64 currentFrameTime = SDL_GetTicks();
    deltaTime = static_cast<float>(currentFrameTime - lastFrameTime);
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

    // only process mouse down events for simplicity
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        inputManager.ProcessInput(event->button);
    }
}
