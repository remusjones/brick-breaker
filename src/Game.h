//
// Created by Remus on 29/06/2025.
//


#pragma once
#include "ECS/HelloECS.h"
#include "InputManager.h"
#include "SDL3/SDL_init.h"
#include "rendering/Display.h"
#include "systems/SystemManager.h"

#include <memory>

class LevelSystem;
class PhysicsSystem;
class Game
{
public:
    Game();

    SDL_AppResult Start();
    SDL_AppResult Update();
    SDL_AppResult GetState() const;
    void WindowEvent(SDL_Event* event);
    void Shutdown();

private:
    void CreateLevel();

    EntityHandle paddleEntity = INVALID_ENTITY;
    SDL_AppResult currentAppState;
    Uint64 lastFrameTime;
    float deltaTime;

    Display display;
    SystemManager systemManager;
    InputManager inputManager;
    std::unique_ptr<HelloECS> ecs;
    LevelSystem* levelSystem;
};
