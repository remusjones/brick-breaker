//
// Created by Remus on 29/06/2025.
//


#pragma once
#include "SDL3/SDL_init.h"
#include "rendering/Display.h"
#include "systems/SystemManager.h"

#include <memory>

class Physics;
class Game
{
public:
    Game();

    SDL_AppResult Start();
    SDL_AppResult Update();

    void Shutdown();

    SDL_AppResult GetState();

    void WindowEvent(SDL_Event* event);

private:

    Display display;
    SystemManager systemManager;
    SDL_AppResult currentAppState;

    Uint64 lastFrameTime;
    float deltaTime;
};
